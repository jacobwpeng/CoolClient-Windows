#include "make_torrent_runnable.h"
#include "client.h"
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
			try{
				retcode_t ret = pCoolClient_->MakeTorrent(path_, torrent_name_, chunk_size_, torrent_type_, tracker_address_, &progress_obj_);
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