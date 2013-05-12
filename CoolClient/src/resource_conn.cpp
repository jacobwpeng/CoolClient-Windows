#include "resource_conn.h"
//#include "utilities.h"
#include <iostream>
using namespace std;

namespace CoolDown{
	namespace Client{
//*	//�����������Ϣ
	int connent(Ask request, StreamSocket* sock ,Reply* back)
	{
		int mark = 0;
		char buffer[1024] = {0};
		
		try 
		{
			string content;															
			request.SerializeToString(&content);                    //������Ϣ	

			Int32 a = content.length();
			Int32 b = Poco::ByteOrder::toNetwork(a);
			sock->sendBytes(&b, sizeof(Int32));

			sock->sendBytes(content.data(), content.length()); 
			
			int m,n;
			m = sizeof(Int32);
			n = 0;
			while( n != m ){
				n += sock->receiveBytes( (char*)buffer + n, m - n );       //������Ϣ��С
				if(n <= 0){
					return 3;
				}
			}


			int c = *( reinterpret_cast<Int32*>(buffer) );
			int d = Poco::ByteOrder::fromNetwork(c);


			string receive;
			receive.clear();
			n = 0;
			while( n != d){
				n = sock->receiveBytes(buffer, 1024);              //������Ϣ
				if( n <= 0 ){
					return 3;
				}
				receive.append(buffer, n);
				d = d - n ;
				n = 0;
			}

            
			if(back->ParseFromArray(receive.data(), receive.length()))    //�����ɹ�
			{
		//		std::cout << back->DebugString() << std::endl;                      //ɾ
				if(back->num() == 0)mark = 2;                                          //���ʹ�
				else if(back->flag() == false)mark = 1;                                //������
			}
			else mark = 3;                                                              //���մ�
		}   
		catch (Exception& exc)
		{
			mark = 3;                                                                   //���մ�
			std::cerr << exc.displayText() << std::endl; 

		}
		return mark;
	}

	/*		while(1)
			{
				int n = sock->receiveBytes(buffer, 1024);                //������Ϣ
				if(n>0)
				{
					receive.append(buffer, n);
					if(back.ParseFromArray(receive.data(), receive.length()))
					{
						std::cout << back.DebugString() << std::endl;  
						receive.clear();
						break;
					}
				}
			}
	// */




	int search(StreamSocket* sock, string key, int type, int counta, int countb, vector<Info>* info)       //������������
	{
		Ask request;
		request.Clear();
		request.set_num(1);
		request.set_key( key );
		request.set_type(type);
		request.set_counta(counta);
		request.set_countb(countb);

		Reply* back = new Reply;
		back->Clear();
	    int mark = connent(request, sock, back);
		if( mark == 0)
		{
			info->clear();
			for(int i=0; i< back->info().size(); i++)
			{
				info->push_back( back->info().Get(i) );
			}
		}
		delete back;
		return mark;
	}

	int search_count(StreamSocket* sock, string key, int type, int* count)
	{
		Ask request;
		request.set_num(5);
		request.set_key( key );
		request.set_type(type);

		Reply* back = new Reply;
		int mark = connent(request, sock, back);
		if ( mark == 0)
		{
			*count = back->count(); 
		}
		delete back;
		return mark;
	}


	int check (StreamSocket* sock, int fileid, string* brief)       //�鿴��Դ������������
	{
		Ask request;
		request.Clear();
		request.set_num(2);
		request.set_fileid(fileid);
		
		Reply* back = new Reply;
		back->Clear();
	    int mark = connent(request, sock, back);
		if( mark == 0)
		{
			*brief = back->brief();
		}
		delete back;
		return mark;
	}


	int download(StreamSocket* sock, int fileid, string* seed)       //������������
	{
		Ask request;
		request.Clear();
		request.set_num(3);
		request.set_fileid(fileid);
		
		Reply* back = new Reply;
		back->Clear();
	    int mark = connent(request, sock, back);
		if( mark == 0)
		{
			*seed =  back->seed();
		}
		delete back;
		return mark;
	}


	int upload(StreamSocket* sock, string seed, int type, string key, string brief, Int64 size)       //�ϴ���������
	{
		Ask request;
		request.Clear();
		request.set_num( 4 );
		request.set_seed( seed );
		request.set_type( type );
		request.set_key( key );
		request.set_brief( brief );
		request.set_size( size );
	//	request.mutable_info()->CopyFrom(info);

		Reply* back = new Reply;
		back->Clear();
		int mark = connent(request, sock, back);
		delete back;
		return mark;
	}


	}
}



