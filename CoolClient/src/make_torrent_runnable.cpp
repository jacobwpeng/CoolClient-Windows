#include "make_torrent_runnable.h"
#include "client.h"
#include "torrent.pb.h"
#include "utilities.h"
#include <exception>
#include <Poco/Exception.h>
#include <Poco/Util/Application.h>

namespace CoolDown{
	namespace Client{

		MakeTorrentRunnable::MakeTorrentRunnable(CoolClient* pCoolClient, const string& path, const string& torrent_name, 
			int chunk_size, int torrent_type, const string& tracker_address, make_torrent_progress_callback_t callback)
			:pCoolClient_(pCoolClient), path_(path), torrent_name_(torrent_name), chunk_size_(chunk_size), torrent_type_(torrent_type),
				tracker_address_(tracker_address), progress_obj_(callback){

		}

		void MakeTorrentRunnable::run(){
			Logger& logger_ = Application::instance().logger();
			try{
				Torrent::Torrent torrent;
				retcode_t ret = pCoolClient_->MakeTorrent( path_, torrent_name_, chunk_size_, torrent_type_, tracker_address_, 
					&torrent, &progress_obj_);
				poco_information_f1(logger_, "in MakeTorrentRunnable::run, Call CoolClient::MakeTorrent returns %d", 
									(int)ret);
				if( ret == ERROR_VERIFY_STOPPED_BY_CLIENT ){
					return;
				}
				ret = pCoolClient_->PublishResource(GBK2UTF8(torrent_name_), torrent);
				poco_information_f1(logger_, "in MakeTorrentRunnable::run, Call CoolClient::PublishResource returns %d", 
					(int)ret);
				string torrent_path = pCoolClient_->get_torrent_path(torrent_name_);
				int handle;
				string top_path = Path(path_).parent().toString();
				ret = pCoolClient_->AddNewUploadJob(torrent_path, top_path, torrent, &handle);
				poco_information_f1(logger_, "in MakeTorrentRunnable::run, Call CoolClient::AddNewUploadJob returns %d",
					(int)ret);
				ret = pCoolClient_->StartJob(handle);
				poco_information_f1(logger_, "in MakeTorrentRunnable::run, Call CoolClient::StartJob returns %d",
					(int)ret);
			}catch(Poco::Exception& e){
				Application::instance().logger().log(e);
			}catch(std::exception& e){
				Application::instance().logger().warning(e.what());
			}catch(...){
				Application::instance().logger().warning("Catch Unknown Exception in MakeTorrentRunnable::run");
			}
			//to destory itself, so this object must construct on heap
			delete this;
		}
	}
}