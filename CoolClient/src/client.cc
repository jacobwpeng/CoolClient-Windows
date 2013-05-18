#include "client.h"
#include "job.h"
#include "payload_type.h"
#include "netpack.h"
#include "client_connection_handler.h"
#include "job_info_collector.h"
#include "tracker.pb.h"
#include "torrent.pb.h"
#include "client.pb.h"
#include "job_history.pb.h"
#include "resource_conn.h"
#include "utilities.h"

#include <cstdio>
#include <set>
#include <algorithm>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <Poco/Logger.h>
#include <Poco/Exception.h>
#include <Poco/Util/Application.h>
#include <Poco/Environment.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/LocalDateTime.h>
#include <Poco/Bugcheck.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/NetException.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/TextConverter.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>
#include <Poco/FileChannel.h>
#include <Poco/String.h>




using std::set;
using std::find;
using std::ifstream;
using std::ofstream;
using Poco::Logger;
using Poco::Exception;

using Poco::Util::Application;
using Poco::LocalDateTime;
using Poco::Net::ServerSocket;
using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Thread;
using Poco::RunnableAdapter;


using namespace TrackerProto;
using namespace ClientProto;
using namespace JobHistory;

namespace CoolDown{
    namespace Client{

            namespace {
                void sock_guard(LocalSockManager::LocalSockManagerPtr& sockManager, const string& clientid, SockPtr* sock){
                    sockManager->return_sock(clientid, *sock);
                }
            }

			ClientRunnable::ClientRunnable(CoolClient* pCoolClient)
				:pCoolClient_(pCoolClient){
			}

			void ClientRunnable::run(){
				int argc = 1;
				char* argv[] = {
					"CoolClient.exe"
				};
				pCoolClient_->run(argc, argv);
			}


            CoolClient::CoolClient()
            :jobThreads_("JobThread"),
            uploadManager_(logger()),
			pUserConfig_(new PropertyFileConfiguration){
				
            }

            void CoolClient::initialize(Application& self){
                loadConfiguration();
				ServerApplication::initialize(self);
                
				Logger& logger_ = Logger::get("FileLogger");
				Poco::FileChannel* pChannel = dynamic_cast<Poco::FileChannel*>(logger_.getChannel());
				if( pChannel ){
					//we read the logging config file and this is A file logger
					pChannel->setProperty("rotation", "10 minutes");
				}
				//logger_.getChannel()->setProperty("rotation", "10 minutes");
                setLogger(logger_);

				
                LOCAL_PORT = (unsigned short) config().getInt("client.message_port", 9025);
				
                this->clientid_ = Verification::get_verification_code( Poco::Environment::nodeId() );
                string default_history_file_path = format("%s%c%s", Path::current(), Path::separator(), string("coolclient.history") );
                string default_local_torrent_dir_path = format("%s%s", Path::current(),
																string("Torrents"));
                this->init_error_ = false;
				job_index_ = 1;
                string msg;
				
				StringList key_list;
                try{
					{
						//load default settings
						default_setting_["AutoStartDownloading"] = "1";
						default_setting_["DefaultTorrentPath"] = default_local_torrent_dir_path;
						default_setting_["MaxParallelTask"] = "5";
						default_setting_["MaxDownloadSpeed"] = "512";
						default_setting_["MaxUploadSpeed"] = "512";
						default_setting_["DefaultDownloadPath"] = "E:\\download";
						default_setting_["DownloadNotificationSound"] = "1";
						key_list.push_back("AutoStartDownloading");
						key_list.push_back("DefaultTorrentPath");
						key_list.push_back("MaxParallelTask");
						key_list.push_back("MaxDownloadSpeed");
						key_list.push_back("MaxUploadSpeed");
						key_list.push_back("DefaultDownloadPath");
						key_list.push_back("DownloadNotificationSound");

					}
					this->user_config_path_ = "userconfig.proerties";
					this->history_file_path_ = config().getString("client.history_path", default_history_file_path);
					this->local_torrent_dir_path_ = config().getString("DefaultTorrentPath", default_setting_["DefaultTorrentPath"]);
					this->exiting_ = false;
					current_setting_ = default_setting_;

					File configFile( this->user_config_path_ );
					if( configFile.exists() ){
						pUserConfig_->load(this->user_config_path_);
						config().add(pUserConfig_, -100);

						for(int pos = 0; pos != key_list.size(); ++pos){
							const string& key(key_list[pos]); 
							current_setting_[key] = config().getString(key, default_setting_[key]);
						}
					}
					

					sockManager_.assign( new LocalSockManager );
					if( sockManager_.isNull() ){
						msg = "Cannot create LocalSockManager.";
						throw Exception(msg);
					}

					File torrent_dir(local_torrent_dir_path_);
					if( torrent_dir.exists() == false){
						torrent_dir.createDirectories();
					}

					retcode_t ret = this->ReloadJobHistory(this->history_file_path_);
					if( ret != ERROR_OK ){
						poco_warning_f1(logger(), "ReloadJobHistory returns %d", (int)ret);
					}

                }
                catch(Exception& e){
                    poco_error_f1(logger(), "Got exception in initialize : %s", msg);
					poco_error_f1(logger(), "%s", e.displayText());
                    this->init_error_ = true;
                }

				

            }

            void CoolClient::uninitialize(){
				this->SaveUserConfig();
                this->SaveJobHistory( history_file_path_ );
				poco_trace(logger(), "Return from SaveJobHistory");
				
				//crash when waiting two threads below.... so we just do not wait them....
				/*
				if( this->report_progress_thread_.isRunning() ){
					while( this->report_progress_thread_.tryJoin( 1000 ) == false ){
						printf("first\n");
						this->reportProgressCond_.broadcast();
					}
				}

				if( this->job_info_collector_thread_.isRunning() ){
					while( this->job_info_collector_thread_.tryJoin( 1000 ) == false ){
						printf("second\n");
						this->jobInfoCollectorTerminateCond_.broadcast();
					}
				}
				*/
                ServerApplication::uninitialize();
				exiting_ = true;
            }

            int CoolClient::main(const vector<string>& args){
                if( this->init_error_ ){
                    return Application::EXIT_TEMPFAIL;
                }
				poco_notice(logger(), "enter main of CoolClient.");
				//poco_debug_f1(logger(), "%s", ConvertGBKToUtf8("UTF8 String of ascii charactors."));
				//return Application::EXIT_OK;

                string tracker_ip = config().getString("TrackerAddress", "127.0.0.1");
                string tracker_address( format("%s:%d", tracker_ip, (int)CoolClient::TRACKER_PORT) );
				string resource_server_ip = config().getString("ResourceServerAddress", "127.0.0.1");
				string resource_server_address( format("%s:%d", resource_server_ip, (int)CoolClient::RESOURCE_SERVER_PORT));

				retcode_t ret = this->LoginTracker(tracker_ip, TRACKER_PORT);
				this->tracker_addr_ = format("%s:%d", tracker_ip, (int)TRACKER_PORT);
				poco_debug_f1(logger(), "LoginTracker returns %d", (int)ret);

				ret = this->ConnectResourceServer(resource_server_ip);
				this->resource_server_ip_ = resource_server_ip;
				poco_debug_f1(logger(), "ConnectResourceServer returns %d", (int)ret);

				poco_debug_f1(logger(), "run application with args count :%d", (int)args.size());
				//init routine threads
                this->job_info_collector_thread_.setOSPriority( Thread::getMaxOSPriority() );
                this->job_info_collector_thread_.start( *(new JobInfoCollector) );
                Poco::RunnableAdapter<CoolClient> reportProgressRunnable( *this, &CoolClient::ReportProgressRoutine );
                this->report_progress_thread_.start( reportProgressRunnable );


				ServerSocket svs(LOCAL_PORT);
				svs.setReusePort(false);

				SocketReactor reactor;
				SocketAcceptor<ClientConnectionHandler> acceptor(svs, reactor);


				Thread thread;
				thread.start(reactor);

				poco_trace(logger(), "Going to wait for the StopClient");
				waitForTerminationRequest();
				poco_trace(logger(), "Wake up by call StopClient");

				reactor.stop();
				thread.join();

                return Application::EXIT_OK;
            }

            NetTaskManager& CoolClient::upload_manager(){
                return this->uploadManager_;
            }

			retcode_t CoolClient::PublishResource(const string& torrent_name, const Torrent::Torrent& torrent){
				string torrent_path = get_torrent_path(torrent_name);
				poco_trace_f1(logger(), "Publish torrent('%s').", torrent_path);
				Torrent::Torrent torrent_info(torrent);

				bool is_completed_publish = true;
				//first publish all file to Tracker
				{
					BOOST_FOREACH(const Torrent::File& oneFile, torrent_info.file()){
						string fileid( oneFile.checksum() );
						retcode_t ret = this->PublishResourceToTracker(this->tracker_addr_, fileid);
						if( ret != ERROR_OK ){
							poco_warning_f3(logger(), "Publish file(%s) to tracker(%s) return %d",
								fileid, this->tracker_addr_, (int)ret);
							is_completed_publish = false;
						}
					}
				}

				//then publish the torrent to Resource Server
				{
					SockPtr resource_server_sock = this->sockManager_->get_resource_server_sock();
					poco_assert( resource_server_sock.isNull() == false );
					string torrent_name = Path(torrent_path).getBaseName();
					string torrent_content;
					torrent.SerializeToString(&torrent_content);
					ostringstream encode_oss;
					Poco::Base64Encoder encoder(encode_oss);
					encoder.write(torrent_content.data(), (std::streamsize) torrent_content.size());
					encoder.close();
					
					string encoded_torrent_content = encode_oss.str();
					int ret = CoolDown::Client::upload(resource_server_sock.get(), encoded_torrent_content, torrent_info.type(), 
						torrent_name, torrent_info.introduction(), torrent_info.totalsize());
					if( ret != 0 ){
						poco_warning_f1(logger(), "in CoolClient::PublishResource, Call CoolDown::Client::upload returns %d",
										(int)ret);
						is_completed_publish = false;
					}
				}
				return is_completed_publish ? ERROR_OK : ERROR_PUBLISH_NOT_COMPLETE;
			}

			retcode_t CoolClient::DownloadTorrent(int id, const string& torrent_name){
				SockPtr resource_server_sock = this->sockManager_->get_resource_server_sock();
				if( resource_server_sock.isNull() ){
					return ERROR_NET_NOT_CONNECTED;
				}
				string encoded_torrent_content;
				int download_ret = CoolDown::Client::download(resource_server_sock.get(), id, &encoded_torrent_content);
				if( download_ret != 0 ){
					poco_warning_f1(logger(), "in CoolClient::DownloadTorrent, CoolDown::Client::download returns %d",
									download_ret);
					return ERROR_UNKNOWN;
				}

				istringstream  decode_iss(encoded_torrent_content);
				Poco::Base64Decoder decoder(decode_iss);
				string torrent_content;
				//decode_iss >> torrent_content;

				int c = decoder.get();
				while (c != -1) { torrent_content += char(c); c = decoder.get(); }

				string torrent_path( get_torrent_path(torrent_name) );
				std::locale loc = std::locale::global(std::locale(""));
				ofstream ofs(torrent_path.c_str(), ofstream::binary);
				std::locale::global(loc);
				ofs.write(torrent_content.data(), torrent_content.length());
				ofs.close();
				//return ERROR_OK;


				Torrent::Torrent torrent;
				if( false == torrent.ParseFromString(torrent_content) ){
					poco_warning(logger(), "in CoolClient::DownloadTorrent, cannot ParseFromString");
					return ERROR_PROTO_PARSE_ERROR;
				}
				return ERROR_OK;
			}

			string CoolClient::GetConfig(const string& key) const{
				SettingMap::const_iterator iter = current_setting_.find(key);
				if( iter != current_setting_.end() ){
					return iter->second;
				}
				return "";
			}
			int CoolClient::SetConfig(const string& key, const string& value){
				if( default_setting_.find(key) == default_setting_.end() ){
					return -1;
				}else{
					current_setting_[key] = value;
					return 0;
				}
			}

			void CoolClient::ResetConfig(){
				this->current_setting_ = this->default_setting_;
			}

			string CoolClient::GetLocalPath(int handle){
				JobPtr pJob = this->GetJobByHandle(handle);
				if( pJob.isNull() ){
					return "";
				}else{
					string top_path = pJob->MutableJobInfo()->localFileInfo.top_path();
					string file_relative_path = this->torrent_path_map_[handle];
					Path p(file_relative_path);
					string torrent_file_name = p.getFileName();
					string file_name = torrent_file_name.substr(0, torrent_file_name.find_last_of("."));
					return format("%s%s", top_path, file_name);
				}
			}

			JobStatusMap CoolClient::JobStatuses(){
				FastMutex::ScopedLock lock_(job_status_mutex_);
				return this->job_status_;
			}

            retcode_t CoolClient::LoginTracker(const string& tracker_address, int port){
                retcode_t ret = sockManager_->connect_tracker(tracker_address, port);
                if( ret != ERROR_OK ){
                    poco_warning_f3(logger(), "Cannot connect tracker, ret : %d, addr : %s, port : %d.", (int)ret, tracker_address, port);
                    return ret;
                }
				FastMutex::ScopedLock lock_(this->tracker_sock_mutex_);
                LocalSockManager::SockPtr sock( sockManager_->get_tracker_sock( format("%s:%d", tracker_address, port)) );
                poco_assert( sock.isNull() == false );

                Login msg;
                msg.set_clientid( this->clientid() );
                msg.set_messageport( LOCAL_PORT );
                SharedPtr<MessageReply> r;

                ret = handle_reply_message<MessageReply>( sock, msg, PAYLOAD_LOGIN, &r);
                return ret;
            }

            retcode_t CoolClient::LogoutTracker(const string& tracker_ip, int port){
				const string& tracker_addr( format("%s:%d", tracker_ip, port) );
				FastMutex::ScopedLock lock_(this->tracker_sock_mutex_);
                LocalSockManager::SockPtr sock( sockManager_->get_tracker_sock( tracker_addr) );
                if( sock.isNull() ){
                    return ERROR_NET_CONNECT;
                }
                Logout msg;
                msg.set_clientid( this->clientid() );
                SharedPtr<MessageReply> r;
                retcode_t ret = handle_reply_message<MessageReply>( sock, msg, PAYLOAD_LOGOUT, &r);
				this->sockManager_->logout_tracker(tracker_ip, port);
                return ret;
            }

			retcode_t CoolClient::ConnectResourceServer(const string& resource_server_address, int port /* = RESOURCE_SERVER_PORT */){
				return this->sockManager_->connect_resource_server(resource_server_address, port);
			}

            retcode_t CoolClient::PublishResourceToTracker(const string& tracker_address, const string& fileid){
				FastMutex::ScopedLock lock_(this->tracker_sock_mutex_);
                LocalSockManager::SockPtr sock( sockManager_->get_tracker_sock(tracker_address) );
                if( sock.isNull() ){
                    return ERROR_NET_NOT_CONNECTED;
                }
				::TrackerProto::PublishResource msg;
                msg.set_clientid(this->clientid());
                msg.set_fileid(fileid);
                SharedPtr<MessageReply> r;
                retcode_t ret = handle_reply_message<MessageReply>( sock, msg, PAYLOAD_PUBLISH_RESOURCE, &r);
                return ret;

            }

            retcode_t CoolClient::ReportProgress(const string& tracker_address, const string& fileid, int percentage){
				FastMutex::ScopedLock lock_(this->tracker_sock_mutex_);
                LocalSockManager::SockPtr sock( sockManager_->get_tracker_sock(tracker_address) );
                if( sock.isNull() ){
                    return ERROR_NET_CONNECT;
                }
                TrackerProto::ReportProgress msg;
                msg.set_clientid( this->clientid() );
                msg.set_fileid( fileid );
                msg.set_percentage( percentage );
                SharedPtr<MessageReply> r;
                retcode_t ret = handle_reply_message<MessageReply>(sock, msg, PAYLOAD_REPORT_PROGRESS, &r);
                return ret;
            }

            retcode_t CoolClient::RequestClients(const string& tracker_address, const string& fileid, int currentPercentage, 
                int needCount, const ClientIdCollection& clientids, FileOwnerInfoPtrList* pInfoList){
                poco_assert( pInfoList != NULL );
                poco_assert( currentPercentage >= 0 );
                poco_assert( needCount > 0 );
				
				FastMutex::ScopedLock lock_(this->tracker_sock_mutex_);
                LocalSockManager::SockPtr sock( sockManager_->get_tracker_sock(tracker_address ));
                if( sock.isNull() ){
                    return ERROR_NET_CONNECT;
                }

                QueryPeer msg;
                msg.set_fileid(fileid);
                msg.set_percentage(currentPercentage);
                msg.set_needcount(needCount);
                ClientIdCollection::const_iterator iter = clientids.begin();
                while( iter != clientids.end() ){
                    msg.add_ownedclients(*iter);
                    ++iter;
                }

                SharedPtr<QueryPeerReply> r;
                retcode_t ret = handle_reply_message< QueryPeerReply >( sock, msg, PAYLOAD_REQUEST_PEER, &r);
                poco_notice_f2(logger(), "Recv %d clients by QueryPeer. fileid = '%s'", r->info().size(), fileid);
                if( ret != ERROR_OK || r->returncode() != ERROR_OK ){
                    poco_warning_f2(logger(), "CoolClient::RequestClients error. func ret : %d, request ret : %d",
                            (int)ret, (int)r->returncode() );
                    return ERROR_UNKNOWN;
                }

                int peer_count = r->info().size();
                for( int pos = 0; pos != peer_count; ++pos ){
                    const PeerFileInfo& peer = r->info().Get(pos);
                    pInfoList->push_back( new FileOwnerInfo(
                                peer.client().clientid(), 
                                peer.client().ip(), 
                                peer.client().messageport(),
                                peer.percentage()) );
                }
                return ret;
            }

			retcode_t CoolClient::SearchResource(const string& keywords, int type, 
											int record_start, int record_end, InfoList* pInfo){
				SockPtr resource_server_sock = this->GetResourceServerSock();
				if( resource_server_sock.isNull() ){
					poco_warning(logger(), "Cannot Connect to Resource Server now.");
					return ERROR_NET_NOT_CONNECTED;
				}
				int ret = CoolDown::Client::search(resource_server_sock.get(), keywords, type, record_start, record_end, pInfo);
				if(ret != 0){
					poco_warning_f2(logger(), "CoolDown::Client::search returns %d, pInfo has %u elements", ret, pInfo->size());
					return ERROR_UNKNOWN;
				}
				return ERROR_OK;
			}

			retcode_t CoolClient::SearchResourceCount(const string& keywords, int type, int* pCount){
				SockPtr resource_server_sock = this->GetResourceServerSock();
				if( resource_server_sock.isNull() ){
					poco_warning(logger(), "Cannot Connect to Resource Server now.");
					return ERROR_NET_NOT_CONNECTED;
				}
				int ret = CoolDown::Client::search_count(resource_server_sock.get(), keywords, type, pCount);
				if(ret != 0){
					poco_warning_f1(logger(), "CoolDown::Client::search_count returns %d", ret);
					return ERROR_UNKNOWN;
				}
				return ERROR_OK;
			}

			retcode_t CoolClient::GetResourceTorrentById(int torrent_id, const string& torrent_name, string* local_torrent_path){
				SockPtr resource_server_sock = this->GetResourceServerSock();
				if( resource_server_sock.isNull() ){
					poco_warning(logger(), "Cannot Connect to Resource Server now.");
					return ERROR_NET_NOT_CONNECTED;
				}
				string torrent_content;
				int ret = CoolDown::Client::download(resource_server_sock.get(), torrent_id, &torrent_content);
				if( ret != 0 ){
					poco_warning_f1(logger(), "CoolDown::Client::download returns %d", ret);
					return ERROR_UNKNOWN;
				}

				string torrent_path( get_torrent_path(torrent_name) );
				std::locale loc = std::locale::global(std::locale(""));
				ofstream ofs(torrent_path.c_str());
				std::locale::global(loc);
				ofs << torrent_content;
				ofs.close();
				local_torrent_path->swap(torrent_path);
				return ERROR_OK;
			}

			SockPtr CoolClient::GetResourceServerSock(){

				SockPtr res(this->sockManager_->get_resource_server_sock());
				if( res.isNull() ){
					if( ERROR_OK != this->ConnectResourceServer(this->resource_server_ip_) ){
						//cannot connect to resource server now
					}else{
						//test if sock has not been closed
						try{
							char buf[2];
							int ret = res->receiveBytes(buf, 0);
						}catch(Poco::Net::ConnectionResetException& e){
							//if closed, reconnect
							this->ConnectResourceServer(this->resource_server_ip_);
						}
						res = this->sockManager_->get_resource_server_sock();
					}
				}
				return res;
			}

            template<typename ReplyMessageType>
            retcode_t CoolClient::handle_reply_message(LocalSockManager::SockPtr& sock, 
                    const Message& msg, int payload_type, SharedPtr<ReplyMessageType>* out){

                    NetPack req(payload_type, msg);
                    poco_information_f1(logger(), "pack to send : \n%s", req.debug_string());
                    retcode_t ret = req.sendBy(*sock);
                    string error_msg("Unknown");
                    if( ret != ERROR_OK ){
                        error_msg = format("cannot send request, payload type=%d", payload_type);
                        goto err;
                    }
                    ret = req.receiveFrom(*sock);
                    if( ret != ERROR_OK){
                        error_msg = "Cannot receive comfirm message from tracker";
                        goto err;
                    }

                    *out = req.message().cast<ReplyMessageType>();
                    if( out->isNull() ){
                        error_msg = "Cannot cast retrun message to certain type";
                        goto err;
                    }

                    if( (*out)->returncode() != ERROR_OK ){
                        error_msg = format("Invalid message return code %d from tracker", int((*out)->returncode()) );
                        goto err;
                    }
                    return ERROR_OK;
    err:
                    poco_warning_f3(logger(), "in handle_reply_message: %s, ret:%d, remote addr:%s", 
                            error_msg, (int)ret, sock->peerAddress().toString());
                    return ret;
                
            }

            retcode_t CoolClient::MakeTorrent(const Path& path, const string& torrent_filename, 
				Int32 chunk_size, Int32 type, const string& tracker_address, 
				Torrent::Torrent* pTorrent, MakeTorrentProgressObj* pProgressObj){

                string pathmsg( format("Call MakeTorrent with path : %s, torrent_filename : %s", 
                                    path.toString(), torrent_filename
                                )
                            );
                poco_notice_f4(logger(), "%s, chunk_size : %d, type : %d, tracker_address : %s", 
                        pathmsg, chunk_size, type, tracker_address);

                File f( path );
                string top_path = Path(f.path()).parent().toString();
                if( !f.exists() ){
                    return ERROR_FILE_NOT_EXISTS;
                }
				string torrent_file_path = get_torrent_path( UTF82GBK(torrent_filename) );

				std::locale loc = std::locale::global(std::locale(""));
				ofstream ofs( torrent_file_path.c_str(), ofstream::binary);
				std::locale::global(loc);
				if( !ofs ){
					return ERROR_FILE_CANNOT_CREATE;
				}

                //fill torrent info
				poco_assert( pTorrent != NULL);
                Torrent::Torrent& torrent = *pTorrent;
                torrent.set_type(type);
                torrent.set_createby(clientid());
                torrent.set_createtime(current_time());
                torrent.set_trackeraddress( tracker_address );
                Int64 total_size = 0;

                FileList files;
                if( f.isFile() ){
                    files.push_back(f);
                }else{
                    list_dir_recursive(f, &files);
                }
				int total_chunk_count = 0; //used for progress bar
				{
					FileList::iterator iter = files.begin();
					FileList::iterator end = files.end();
					for(; iter != end; ++iter){
						total_chunk_count += Verification::get_file_chunk_count(*iter, chunk_size);
					}
				}

				if( pProgressObj ){
					pProgressObj->set_total_count(total_chunk_count);
				}

                FileList::iterator iter = files.begin();
                FileList::iterator end = files.end();


                string torrent_id_source;
				int current_chunk_count = 0;
				bool continue_progress = true;
                while( iter != end ){
					try{
						//Process one File
						Path p(iter->path());
						//poco_trace_f1(logger(), "in MakeTorrent, Processing %s.", iter->path());
						string file_check_sum;
						Verification::ChecksumList checksums;
						continue_progress = Verification::get_file_and_chunk_checksum_list(*iter, chunk_size,
							&file_check_sum, &checksums, pProgressObj);
						if( continue_progress == false ){
							break;
						}
						//string file_check_sum = Verification::get_file_verification_code( iter->path() );
						Int64 file_size = iter->getSize();
						total_size += file_size;
	                    
						//Verification::get_file_checksum_list(*iter, chunk_size, &checksums);
						int last_chunk_size = file_size % chunk_size;

						//file file info in Torrent
						Torrent::File* aFile = torrent.add_file();
						string abs_path( p.parent().toString() );
						pair<string::iterator, string::iterator> pa = mismatch(top_path.begin(), top_path.end(), abs_path.begin() );
						string relative_path( pa.second, abs_path.end() );
						aFile->set_relativepath( relative_path );
						aFile->set_filename( p.getFileName() );
						aFile->set_size( file_size );
						aFile->set_checksum( file_check_sum );

						torrent_id_source.append( p.parent().toString() ).append( p.getFileName() ).append( file_check_sum );

						Verification::ChecksumList::iterator checksum_iter = checksums.begin();

						//Process chunks in a File
						while( checksum_iter != checksums.end() ){
							int this_chunk_size = ( checksum_iter == checksums.end() -1 ) ? last_chunk_size : chunk_size;
							//fill chunk info in File
							Torrent::Chunk* aChunk = aFile->add_chunk();
							aChunk->set_checksum( *checksum_iter );
							aChunk->set_size( this_chunk_size );
							++checksum_iter;
						}
					}catch(Poco::Exception& e){
						poco_warning_f2(logger(), "catch Poco::Exception in CoolClient::MakeTorrent while processing %s, msg : %s", 
							iter->path(), e.displayText());
					}catch(std::exception& e){
						poco_warning_f2(logger(), "catch std::exception in CoolClient::MakeTorrent while processing %s, msg : %s", 
							iter->path(), string(e.what()));
					}

                    ++iter;
                    
                }
				retcode_t ret = ERROR_OK;
				if( continue_progress == false ){
					ret = ERROR_VERIFY_STOPPED_BY_CLIENT;
					poco_trace(logger(), "MakeTorrent failed because stopped by client!");
					ofs.close();
					//delete this file when it's stopped by client
					int remove_ret = remove(torrent_file_path.c_str());
					if( remove_ret != 0 ){
						poco_warning_f2(logger(), "in CoolClient::MakeTorrent, Cannot remove file('%s'), ret : %d", 
									torrent_file_path, remove_ret);
					}
				}else{
					string torrent_id = Verification::get_verification_code( torrent_id_source );
					torrent.set_totalsize( total_size );
					torrent.set_torrentid( torrent_id );
					poco_assert( torrent.SerializeToOstream(&ofs) );
					poco_trace_f1(logger(), "MakeTorrent succeed! torrent file : %s", torrent_file_path);
					ofs.close();
					{
						//test if we can parse this torrent
						//Torrent::Torrent torrent;
						//this->ParseTorrent(torrent_file_path, &torrent);
					}
				}
				
                return ret;
            }

            //communicate with client
            retcode_t CoolClient::shake_hand(const ShakeHand& self, ShakeHand& peer){
                string peer_clientid( peer.clientid() );
                poco_trace_f1(logger(), "assert if connect to '%s'", peer_clientid);
                poco_assert( sockManager_->is_connected(peer_clientid) );
                poco_trace_f2(logger(), "pass assert at file : %s, line : %d", string(__FILE__), static_cast<int>(__LINE__ - 1));

                LocalSockManager::SockPtr sock( sockManager_->get_idle_client_sock(peer_clientid) );
                poco_assert( sock.isNull() == false );
                poco_trace_f2(logger(), "pass assert at file : %s, line : %d", string(__FILE__), static_cast<int>(__LINE__ - 1));

                //use guard to return_sock automatically 
                typedef shared_ptr<LocalSockManager::SockPtr> SockGuard;
                SockGuard guard(&sock, boost::bind(sock_guard, sockManager_, peer_clientid, _1) );

                NetPack req( PAYLOAD_SHAKE_HAND, self );
                retcode_t ret = req.sendBy( *sock );
                if( ret != ERROR_OK ){
                    poco_warning(logger(), "send shake_hand error.");
                    return ret;
                }
                NetPack res;
                ret = res.receiveFrom( *sock );
                if( ret != ERROR_OK ){
                    poco_warning(logger(), "receive shake_hand error.");
                    return ret;
                }

                peer = *(res.message().cast<ShakeHand>());
                return ERROR_OK;
            }

            retcode_t CoolClient::StartJob(int handle){
                FastMutex::ScopedLock lock(mutex_);
                retcode_t ret = this->ResumeJobWithoutLock(handle);
                if( ret != ERROR_OK ){
                    return ret;
                }
                JobPtr pJob = this->GetJobWithoutLock(handle);
                poco_assert( pJob.isNull() == false );
                if( pJob->is_running() ){
                    return ERROR_JOB_ALREADY_STARTED;
                }
                jobThreads_.start(*pJob);
                return ERROR_OK;
            }

            retcode_t CoolClient::StopJob(int handle){
                FastMutex::ScopedLock lock(mutex_);
                JobPtr pJob = this->GetJobWithoutLock(handle);
                if( pJob.isNull() ){
                    return ERROR_JOB_NOT_EXISTS;
                }
                pJob->MutableJobInfo()->downloadInfo.is_stopped = true;
                return ERROR_OK;
            }

            retcode_t CoolClient::PauseJobWithoutLock(int handle){
                JobPtr pJob = this->GetJobWithoutLock(handle);
                if( pJob.isNull() ){
                    return ERROR_JOB_NOT_EXISTS;
                }
                pJob->MutableJobInfo()->downloadInfo.is_download_paused = true;
                return ERROR_OK;
            }

            retcode_t CoolClient::PauseJob(int handle){
                FastMutex::ScopedLock lock(mutex_);
                return this->PauseJobWithoutLock(handle);
            }

            retcode_t CoolClient::ResumeJob(int handle){
                FastMutex::ScopedLock lock(mutex_);
                return this->ResumeJobWithoutLock(handle);
            }

            retcode_t CoolClient::RemoveJob(int handle){
                JobPtr pJob = this->GetJobByHandle(handle);
                if( pJob.isNull() ){
                    return ERROR_JOB_NOT_EXISTS;
                }
                pJob->MutableJobInfo()->downloadInfo.is_job_removed = true;
                pJob->MutableJobInfo()->downloadInfo.download_pause_cond.broadcast();
                pJob->MutableJobInfo()->downloadInfo.download_speed_limit_cond.broadcast();
				{
					FastMutex::ScopedLock lock_(this->mutex_);
					this->jobs_.erase(handle);
				}
				{
					FastMutex::ScopedLock lock_(this->job_status_mutex_);
					this->job_status_.erase(handle);
				}

                //since we never roll back the handle, we never destructor the job until application exit;
                this->removed_jobs_[handle] = pJob;
                return ERROR_OK;
            }

            retcode_t CoolClient::ResumeJobWithoutLock(int handle){
                JobPtr pJob = this->GetJobWithoutLock(handle);
                if( pJob.isNull() ){
                    return ERROR_JOB_NOT_EXISTS;
                }

                pJob->MutableJobInfo()->downloadInfo.is_stopped = false;
                pJob->MutableJobInfo()->downloadInfo.is_download_paused = false;
                pJob->MutableJobInfo()->downloadInfo.download_pause_cond.broadcast();
                pJob->MutableJobInfo()->downloadInfo.job_stopped_cond.broadcast();
                return ERROR_OK;
            }

            CoolClient::JobPtr CoolClient::GetJobWithoutLock(int handle){
                JobPtr res(NULL);
                if( jobs_.find(handle) != jobs_.end() ){
                    res = jobs_[handle];
                }
                return res;
            }

            CoolClient::JobPtr CoolClient::GetJobByHandle(int handle){
                FastMutex::ScopedLock lock(mutex_);
                return this->GetJobWithoutLock(handle);
            }

            CoolClient::JobPtr CoolClient::GetJobByFileid(const string& fileid){
                FastMutex::ScopedLock lock(mutex_);
                BOOST_FOREACH(JobMap::value_type& p, jobs_){
                    const vector<string>& fileidlist = p.second->ConstJobInfo().fileidlist();
                    if( fileidlist.end() != find(fileidlist.begin(), fileidlist.end(), fileid) ){
                        return p.second;
                    }
                }
                return JobPtr(NULL);
            }


            bool CoolClient::HasThisTorrent(const string& torrent_id){
                return torrent_ids_.end() != torrent_ids_.find( torrent_id );
            }

            void CoolClient::RegisterTorrent(const string& torrent_id){
                poco_debug_f1(logger(),"register torrent_id : %s", torrent_id);
                this->torrent_ids_.insert( torrent_id );
            }

			void CoolClient::SaveUserConfig(){
				SettingMap::const_iterator iter = current_setting_.begin();
				while( iter != current_setting_.end() ){
					try{
						string value = Poco::replace(iter->second, "\\", "\\\\");
						//Poco::replaceInPlace(value, "\\t", "\\\\t");
						//Poco::replaceInPlace(value, "\\n", "\\\\n");
						//Poco::replaceInPlace(value, "\\f", "\\\\f");
						pUserConfig_->setString(iter->first, value);
					}catch(Poco::Exception& e){
						poco_warning_f1(logger(), "in CoolClient::SaveUserConfig, Got exception : %s", e.displayText());
					}
					
					++iter;
				}
				pUserConfig_->save(user_config_path_);
			}

            retcode_t CoolClient::SaveJobHistory(const string& filename){
				ofstream ofs(filename.c_str(), ofstream::binary);
                if( !ofs ){
                    poco_warning_f1(logger(), "Cannot open history file : %s for write.", filename);
                    return ERROR_FILE_NOT_EXISTS;
                }

                JobHistory::History history;
                FastMutex::ScopedLock lock(mutex_);

                BOOST_FOREACH(JobMap::value_type& p, jobs_){
                    int handle = p.first;
                    JobPtr pJob = p.second;
                    torrent_path_map_t::iterator iter = torrent_path_map_.find(handle);
                    poco_assert( iter != torrent_path_map_.end() );
                    string torrent_path( iter->second );
                    string top_path( pJob->MutableJobInfo()->localFileInfo.top_path() );

                    JobHistory::JobHistoryInfo* oneJob = history.add_jobinfo();
                    oneJob->set_torrentpath( torrent_path );
                    oneJob->set_localtoppath( top_path );
                    oneJob->set_uploadtotal( pJob->MutableJobInfo()->downloadInfo.upload_total );
                    oneJob->set_downloadtotal( pJob->MutableJobInfo()->downloadInfo.download_total );

                    const TorrentInfo::file_map_t& file_map = pJob->MutableJobInfo()->torrentInfo.get_file_map();
                    BOOST_FOREACH(const TorrentInfo::file_map_t::value_type& p, file_map){
                        string fileid( p.first );
                        file_bitmap_ptr pBitmap = pJob->MutableJobInfo()->downloadInfo.bitmap_map[fileid];

                        BOOST_FOREACH(const TorrentFileInfoPtr& file_info_ptr, p.second){
                            JobHistory::FileInfo* oneFile = oneJob->add_file();
                            oneFile->set_fileid( fileid );
                            oneFile->set_relativepath( file_info_ptr->relative_path() );
                            oneFile->set_filename( file_info_ptr->filename() );
                            oneFile->set_filebitcount( pBitmap->size() );
                            to_block_range( *pBitmap, google::protobuf::RepeatedFieldBackInserter(oneFile->mutable_filebit()) );
                            //double percentage = pBitmap->count() / pBitmap->size();
                            //poco_debug_f2(logger(), "in SaveJobHistory, file '%s', percentage = '%f'", oneFile->fileid(), percentage);
                        }
                    }
                }

                if( false == history.SerializeToOstream(&ofs) ){
                    poco_warning_f1(logger(), "Cannot save job history to file : %s", filename);
                    return ERROR_PROTO_SERIALIZE_ERROR;
                }
                return ERROR_OK;
            }

            retcode_t CoolClient::ReloadJobHistory(const string& filename){
				ifstream ifs(filename.c_str(), ifstream::binary);
                if( !ifs ){
                    poco_warning_f1(logger(), "Cannot load job history file : %s", filename);
                    return ERROR_FILE_NOT_EXISTS;
                }
                JobHistory::History history;
                if ( false == history.ParseFromIstream(&ifs) ){
                    poco_warning_f1(logger(), "Cannot parse JobHistoryInfo from %s", filename);
                    return ERROR_PROTO_PARSE_ERROR;
                }

                for(int i = 0; i != history.jobinfo().size(); ++i){
                    const JobHistoryInfo& oneHistory = history.jobinfo().Get(i);
                    Torrent::Torrent torrent;
                    if( ERROR_OK != this->ParseTorrent(oneHistory.torrentpath(), &torrent) ){
                        continue;
                    }
                    if( ERROR_OK != this->ReloadOneJob(torrent, oneHistory) ){
                        poco_warning(logger(), "Error while reload one job in ReloadHistoryDownload.");
                        continue;
                    }
                }
                return ERROR_OK;
            }

            retcode_t CoolClient::ReloadOneJob(const Torrent::Torrent& torrent, const JobHistoryInfo& history){
                poco_assert( this->HasThisTorrent(torrent.torrentid()) == false );
                this->RegisterTorrent( torrent.torrentid() );
                FileIdentityInfoList needs;
                for(int i = 0; i != history.file().size(); ++i){
                    const JobHistory::FileInfo& oneFile = history.file().Get(i);
                    needs.push_back( FileIdentityInfoList::value_type(oneFile.relativepath(), oneFile.filename()) );
                }

                SharedPtr<JobInfo> info( new JobInfo( torrent, history.localtoppath(), needs) );
                info->downloadInfo.upload_total = history.uploadtotal();
                info->downloadInfo.download_total = history.downloadtotal();
                for(int i = 0; i != history.file().size(); ++i){
                    const JobHistory::FileInfo& oneFile = history.file().Get(i);
                    file_bitmap_ptr& pBitmap = info->downloadInfo.bitmap_map[ oneFile.fileid() ];
                    pBitmap.assign( new file_bitmap_t( oneFile.filebit().begin(), oneFile.filebit().end() ) );
                    pBitmap->resize( oneFile.filebitcount() );
                    info->downloadInfo.percentage_map[ oneFile.fileid() ] = pBitmap->count() / pBitmap->size();
                    //double percentage = pBitmap->count() / pBitmap->size();
                    //poco_debug_f2(logger(), "in ReloadOneJob, file '%s', percentage = '%f'", oneFile.fileid(), percentage);
                }
                info->downloadInfo.is_job_removed = false;
                info->downloadInfo.is_stopped = false;
                info->downloadInfo.is_download_paused = true;


                int handle;
                retcode_t ret = this->AddNewJob(info, history.torrentpath(), &handle);
                if( ret != ERROR_OK ){
                    return ret;
                }
			
				this->StartJob(handle);
                return ERROR_OK;
            }

            retcode_t CoolClient::AddNewJob(const SharedPtr<JobInfo>& info, const string& torrent_path, int* handle){
                int this_job_index = job_index_;
                
				{
					FastMutex::ScopedLock lock(mutex_);
					JobPtr job(new Job(info, *(this->sockManager_), logger()));
					jobs_[job_index_] = job;
				}
                poco_debug_f1(logger(), "add Job to jobs_, torrent_id : %s", info->torrentInfo.torrentid());
                ++job_index_;
                *handle = this_job_index;
                torrent_path_map_[this_job_index] = torrent_path;

				JobStatus status;
				Path tmp(torrent_path);
				status.name = GBK2UTF8(tmp.getBaseName());
				status.size = info->torrentInfo.get_total_size();
				status.type = info->torrentInfo.get_type();
				status.status = JOB_PAUSED;	
				status.handle = this_job_index;
				FastMutex::ScopedLock lock_(this->job_status_mutex_);
				job_status_[this_job_index] = status;
                return ERROR_OK;
            }

            retcode_t CoolClient::AddNewDownloadJob(const string& torrent_path, const Torrent::Torrent& torrent, 
                    const FileIdentityInfoList& needs, const string& top_path, int* handle){

                string torrent_id( torrent.torrentid() );
                if( this->HasThisTorrent(torrent_id) ){
                    return ERROR_JOB_EXISTS;
                }
                poco_debug_f1(logger(), "in  AddNewDownloadJob, pass unique check of torrent_id : %s", torrent_id);
                this->RegisterTorrent( torrent_id );
                SharedPtr<JobInfo> info( new JobInfo( torrent, top_path, needs) );
                retcode_t ret = this->AddNewJob(info, torrent_path, handle);
				return ret;
            }

            retcode_t CoolClient::AddNewUploadJob(const string& torrent_path, const string& top_path, 
                    const Torrent::Torrent& torrent, int* handle){

                string torrent_id( torrent.torrentid() );
                if( this->HasThisTorrent(torrent_id) ){
                    return ERROR_JOB_EXISTS;
                }
                poco_debug_f1(logger(), "in  AddNewUploadJob, pass unique check of torrent_id : %s", torrent_id);
                this->RegisterTorrent( torrent_id );

                FileIdentityInfoList needs;
                for(int i = 0; i != torrent.file().size(); ++i){
                    const Torrent::File& file = torrent.file().Get(i);
                    needs.push_back(FileIdentityInfoList::value_type(file.relativepath(), file.filename()) );
                }
                SharedPtr<JobInfo> info( new JobInfo( torrent, top_path, needs ) );

                set<string> same_fileid;
                for(int pos = 0; pos != torrent.file().size(); ++pos){
                    const Torrent::File& file = torrent.file().Get(pos);
                    string fileid( file.checksum() );
                    if( same_fileid.find(fileid) != same_fileid.end() ){
                        continue;
                    }else{
                        same_fileid.insert(fileid);
                        info->downloadInfo.bitmap_map[fileid]->set();
						info->downloadInfo.percentage_map[fileid] = 100;
						info->downloadInfo.is_download_paused = true;
                        retcode_t publish_ret = this->PublishResourceToTracker(torrent.trackeraddress(), fileid);
                        poco_debug_f2(logger(), "Publish '%s' to tracker return %d", fileid, (int)publish_ret);
                    }
                }
				info->downloadInfo.download_total = info->torrentInfo.get_total_size();
                return this->AddNewJob(info, torrent_path, handle);
            }

            void CoolClient::onJobInfoCollectorWakeUp(Timer& timer){
                FastMutex::ScopedLock lock(mutex_);
                progress_info_list_t progress_to_report;
				JobStatusMap job_status;
				{
					FastMutex::ScopedLock lock_(this->job_status_mutex_);
					job_status = this->job_status_;
				}
                BOOST_FOREACH(JobMap::value_type& p, jobs_){
                    int handle = p.first;
					poco_assert( job_status.find(handle) != job_status.end() );
					JobStatus& status = job_status[handle];

                    JobInfoPtr pInfo = p.second->MutableJobInfo();
					//Upload Speed of this job
                    UInt64 bytes_upload_this_second = pInfo->downloadInfo.bytes_upload_this_second;
					status.upload_speed_per_second_in_bytes = status.upload_speed_per_second_in_bytes * 0.8
																+ 0.2 * static_cast<int>(bytes_upload_this_second);

					//Download Speed of this Job
                    UInt64 bytes_download_this_second = pInfo->downloadInfo.bytes_download_this_second;
					status.download_speed_per_second_in_bytes = status.download_speed_per_second_in_bytes * 0.8 
																+ 0.2 * static_cast<int>(bytes_download_this_second);

					if( pInfo->downloadInfo.is_download_paused ){
						status.download_speed_per_second_in_bytes = 0;
					}

					UInt64 bytes_left = status.size - pInfo->downloadInfo.download_total;
					if( status.download_speed_per_second_in_bytes == 0){
						status.remaining_time_in_seconds = -1;
					}else{
						status.remaining_time_in_seconds = static_cast<int>( 
															(double)bytes_left / status.download_speed_per_second_in_bytes
														);
					}


                    string upload_speed, download_speed;
                    format_speed(bytes_upload_this_second, &upload_speed);
                    format_speed(bytes_download_this_second, &download_speed);


					//see if it's time to report progress to Tracker
					{
						pInfo->downloadInfo.time_to_next_report -= 1;
						bool need_to_report;
						if( pInfo->downloadInfo.time_to_next_report == 0 
							&& p.second->is_running() == true
							){

								need_to_report = true;
								pInfo->downloadInfo.time_to_next_report = DownloadInfo::report_period;
						}else{
							need_to_report = false;
						}

						string tracker_address( pInfo->torrentInfo.tracker_address());
						BOOST_FOREACH(DownloadInfo::file_bitmap_map_t::value_type& p, pInfo->downloadInfo.bitmap_map){
							string fileid(p.first);
							//calc the download percentage of one file
							int percentage = static_cast<int>( (double)p.second->count() / p.second->size() * 100 );
							pInfo->downloadInfo.percentage_map[fileid] = percentage;
							if( need_to_report ){
								ProgressInfo oneInfo = { tracker_address, fileid, percentage };
								progress_to_report.push_back( oneInfo );
							}
						}
					}

                    pInfo->downloadInfo.upload_total += bytes_upload_this_second;
                    pInfo->downloadInfo.download_total += bytes_download_this_second;
                    pInfo->downloadInfo.bytes_upload_this_second = 0;
                    pInfo->downloadInfo.bytes_download_this_second = 0;
                    pInfo->downloadInfo.download_speed_limit_cond.broadcast();
		
					status.percentage = ((double)pInfo->downloadInfo.download_total / status.size) * 100;

					int status_code = -1;
					if( pInfo->downloadInfo.is_stopped ){
						status_code = JOB_STOPPED;
					}else if( pInfo->downloadInfo.is_download_paused ){
						status_code = JOB_PAUSED;
					}else if( bytes_download_this_second != 0 ){
						status_code = JOB_DOWNLOADING;
					}else if( bytes_upload_this_second != 0 ){
						status_code = JOB_UPLOADING;
					}else{
						status_code = JOB_INACTIVE;
					}

					if( pInfo->downloadInfo.download_total == pInfo->torrentInfo.get_total_size()){
						if( status.upload_speed_per_second_in_bytes != 0 ){
							status_code = JOB_UPLOADING;
						}else{
							status_code = JOB_INACTIVE;
						}
						
						status.percentage = 100;
						status.remaining_time_in_seconds = 0;
					}

					poco_assert(status_code != -1);
					status.status = (JobTransportStatus)status_code;
                }

				//see if something to report
                if( progress_to_report.size() != 0 ){
                    {
                        FastMutex::ScopedLock lock(this->progress_info_mutex_);
                        this->progress_info_to_report_.swap(progress_to_report);
                    }
                    this->reportProgressCond_.signal();
                }

				//see if set a status callback
				if( this->status_callback_ ){
					this->status_callback_(job_status);
				}

				//copy the status map back to the class member 
				{
					FastMutex::ScopedLock lock_(this->job_status_mutex_);
					this->job_status_ = job_status;
				}
            }

            void CoolClient::ReportProgressRoutine(){
                while(1){
                    if( this->progress_info_to_report_.size() == 0 && exiting_ == false){
                        FastMutex mutex;
                        this->reportProgressCond_.wait(mutex);
                    }else if( exiting_ == true ){
                        break;
                    }else{
                        progress_info_list_t progress_info_to_report;
                        {
                            FastMutex::ScopedLock lock(this->progress_info_mutex_);
                            progress_info_to_report.swap(this->progress_info_to_report_);
                        }
                        BOOST_FOREACH(const progress_info_list_t::value_type& oneInfo, progress_info_to_report){
                            retcode_t ret = this->ReportProgress(oneInfo.tracker_address, oneInfo.fileid, oneInfo.percentage);
                            poco_debug_f3(logger(), "Report progress of file '%s', percentage '%d', return %d",
                                    oneInfo.fileid, oneInfo.percentage, (int)ret);
                        }

                    }
                }
            }

			void CoolClient::StopClient(){
				this->terminate();
			}

            retcode_t CoolClient::ParseTorrent(const Path& torrent_file_path, Torrent::Torrent* pTorrent){
				std::locale loc = std::locale::global(std::locale(""));
				string gb_torrent_path = torrent_file_path.toString();
				ifstream ifs( gb_torrent_path.c_str(), ifstream::binary);
				std::locale::global(loc);

                if( !ifs ){
                    poco_debug_f1(logger(), "Cannot open %s in ParseTorrent.", torrent_file_path.toString());
                    return ERROR_FILE_NOT_EXISTS;
                }
                poco_assert(pTorrent != NULL);
                pTorrent->Clear();
                if( pTorrent->ParseFromIstream(&ifs) == false){
                    poco_warning_f1(logger(), "cannot parse torrent from file '%s'.", torrent_file_path.toString());
                    return ERROR_FILE_PARSE_ERROR;
                }
                return ERROR_OK;
            }

            string CoolClient::current_time() const{
                LocalDateTime now;
                return Poco::DateTimeFormatter::format(now, Poco::DateTimeFormat::HTTP_FORMAT);
            }

			string CoolClient::get_torrent_path(const string& torrent_name) const{
				return format("%s%c%s", this->local_torrent_dir_path_, Path::separator(), torrent_name);
			}

            void CoolClient::format_speed(UInt64 speed, string* formatted_speed){
                if( speed > (1 << 20) ){
                    // MB level
                    format(*formatted_speed, "%.2f MB/s", (double)speed / (1 << 20) );
                }else if ( speed > (1 << 10) ){
                    format(*formatted_speed, "%.2f KB/s", (double)speed / (1 << 10) );
                }else{
                    format(*formatted_speed, "%Lu B/s", speed);
                }
            }

            string CoolClient::clientid() const{
                return this->clientid_;
            }

            /*void CoolClient::set_make_torrent_progress_callback(make_torrent_progress_callback_t callback){
                this->make_torrent_progress_callback_ = callback;
            }*/

            void CoolClient::list_dir_recursive(const File& file, FileList* pList){
                FileList files;
                file.list(files);
                for(int i = 0; i != files.size(); ++i){
                    File& file = files[i];
                    if( file.canRead() == false){
                        continue;
                    }
                    else if( file.isFile() ){
                        pList->push_back(file);
                    }
                    else if( file.isDirectory() ){
                        list_dir_recursive(file, pList);
                    }else{
                        //drop other files
                    }
                }
            }

    }
}
