#ifndef ONCLICK_H
#define ONCLICK_H


#include <string>
#include <iostream>
#include <vector>

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/StreamCopier.h"
#include "Poco/Path.h"
#include "Poco/Exception.h"
#include <Poco/SharedPtr.h>
#include <Poco/Types.h>
#include <Poco/ByteOrder.h>

#include "resource.pb.h"



using namespace std;
using std::string;


using Poco::Net::StreamSocket;
using Poco::Net::SocketStream;
using Poco::Net::SocketAddress;
using Poco::StreamCopier;
using Poco::Path;
using Poco::Exception;
using Poco::SharedPtr;

using Poco::Int64;
using Poco::Int32;

namespace CoolDown{
	namespace Client{

		typedef vector<Info> InfoList;

		//�����������Ϣ
		int connent(Ask request, StreamSocket* sock ,Reply* back);


		int search(StreamSocket* sock, string key, int type, int counta, int countb, InfoList* info);       //������������

		int search_count(StreamSocket* sock, string key, int type, int* count);


		int check (StreamSocket* sock, int fileid, string* brief);       //�鿴��Դ������������


		int download(StreamSocket* sock, int fileid, string* seed);       //������������


		int upload(StreamSocket* sock, string seed, int type, string key, string brief, Int64 size);       //�ϴ���������
	}
}


	



#endif




