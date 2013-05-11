#ifndef MAKE_TORRENT_RUNNABLE
#define MAKE_TORRENT_RUNNABLE

#include "error_code.h"

#include "verification.h"
#include <Poco/Runnable.h>
#include <boost/function.hpp>


namespace CoolDown{
	namespace Client{

		class CoolClient;

		//typedef function<retcode_t(make_torrent_progress_callback_t)> MakeTorrentRunnableCallback;

		class MakeTorrentRunnable : public Poco::Runnable{
		public:
			MakeTorrentRunnable(CoolClient* pCoolClient, const string& path, const string& torrent_name, int chunk_size, int torrent_type,
				const string& tracker_address, make_torrent_progress_callback_t callback);
			virtual void run();

		private:
			CoolClient* pCoolClient_;
			const string path_;
			const string torrent_name_;
			const int chunk_size_;
			const int torrent_type_;
			const string tracker_address_;
			MakeTorrentProgressObj progress_obj_;
		};

	}
}

#endif