#include "utilities.h"
#include <windows.h>
#include <Poco/Bugcheck.h>

string GBK2UTF8(const string& gbkString){
	int len = MultiByteToWideChar(CP_ACP, 0, gbkString.c_str(), -1, NULL,0);
	poco_assert( len != 0 );

	unsigned short * wszUtf8 = new unsigned short[len+1];
	memset(wszUtf8, 0, len * 2 + 2);

	int ret = MultiByteToWideChar(CP_ACP, 0, gbkString.c_str(), -1, (LPWSTR)wszUtf8, len);
	poco_assert( ret != 0 );

	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
	poco_assert( len != 0 );

	char *szUtf8=new char[len + 1];
	memset(szUtf8, 0, len + 1);

	ret = WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);
	poco_assert( ret != 0 );

	string utf8String = szUtf8;
	delete[] szUtf8; 
	delete[] wszUtf8;
	return utf8String;
}
string UTF82GBK(const string& utf8String){
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, NULL,0);
	poco_assert( len != 0 );

	unsigned short * wszGBK = new unsigned short[len+1];
	memset(wszGBK, 0, len * 2 + 2);

	int ret = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, (LPWSTR)wszGBK, len);
	poco_assert( ret != 0 );

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	poco_assert( len != 0 );

	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);

	ret = WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL,NULL);
	poco_assert( ret != 0 );

	string gbkString = szGBK;
	delete[] szGBK;
	delete[] wszGBK;
	return gbkString;
}