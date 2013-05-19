#ifndef UPLOAD_TASK_H
#define UPLOAD_TASK_H

#include <string>
#include <Poco/Task.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/SharedPtr.h>
#include <Poco/File.h>
#include <Poco/Types.h>
#include <Poco/Format.h>

using std::string;
using Poco::Task;
using Poco::Net::StreamSocket;
using Poco::SharedPtr;
using Poco::File;
using Poco::UInt64;
using Poco::format;

namespace CoolDown{
    namespace Client{

        typedef SharedPtr<StreamSocket> SockPtr;
        typedef SharedPtr<File> FilePtr;

        struct DownloadInfo;
        class UploadTask : public Task{
            public:
                UploadTask(DownloadInfo& downloadInfo, HANDLE hFile, UInt64 offset, int chunk_size, StreamSocket& sock);
                ~UploadTask();

                void runTask();
                UInt64 offset() const{
                    return this->offset_;
                }

                int chunk_size() const{
                    return this->chunk_size_;
                }

                string peer_address() const{
                    return this->peerAddress_;
                }

                string DebugString() const{
                    return format("peer addr = '%s', offset = %Lu, chunk_size = %d", peer_address(), offset(), chunk_size());
                }

            private:
                string peerAddress_;
                DownloadInfo& downloadInfo_;
                //FilePtr file_;
				HANDLE hFile_;
                UInt64 offset_;
                int chunk_size_;
                StreamSocket& sock_;

        };
    }
}

#endif
