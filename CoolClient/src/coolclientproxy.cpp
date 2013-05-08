#include "coolclientproxy.h"
#include "utilities.h"
#include <string>
#include <Poco/Thread.h>
#include <Poco/Logger.h>
#include <Poco/Types.h>

using std::string;
using Poco::Int64;

CoolClient* CoolClientProxy::pCoolClient = new CoolClient;
Logger& CoolClientProxy::logger_ = CoolClientProxy::pCoolClient->logger();

CoolClientProxy* __stdcall CoolClientProxy::Instance(void*){

	static CoolClientProxy* pCoolClientProxy = NULL;
	if(pCoolClientProxy == NULL)
	{
		pCoolClientProxy = new CoolClientProxy();
	}
	return pCoolClientProxy;
}

static XLLRTGlobalAPI CoolClientProxyMemberFunctions[] = {

	//{"CreateInstance",CoolClientProxy::CreateInstance},
	{"RunClientAsync", CoolClientProxy::RunClientAsync},
	{"SearchResource", CoolClientProxy::SearchResource},
	{"StopClient", CoolClientProxy::StopClient},
	//{"TestTable", CoolClientProxy::TestTable},
	{NULL,NULL}
};

int CoolClientProxy::RunClientAsync(lua_State* luaState){
	ClientThread* backgroudClient = new ClientThread(CoolClientProxy::pCoolClient);
	Poco::Thread* pThread = new Poco::Thread;
	pThread->start(*backgroudClient);
	//lua_pushboolean(luaState, 1);
	return 0;
}

int CoolClientProxy::SearchResource(lua_State* luaState){
	using CoolDown::Client::InfoList;
	
	CoolClientProxy** ppCoolClientProxy = reinterpret_cast<CoolClientProxy**>(luaL_checkudata(luaState, 1, COOLCLIENT_PROXY_LUA_CLASS));
	static string last_request_keywords;
	static int last_request_type;
	static int total_record_count = 0;
	{
		//DumpLuaState(luaState);
	}


	//if this is the first search of this keywords and type
	bool new_search = false;
	//TODO: Get The total count of records when first call.
	if( lua_isfunction(luaState, 6) ){
		long functionRef = luaL_ref(luaState,LUA_REGISTRYINDEX);
		string keywords( UTF82GBK(lua_tostring(luaState, 2)) );
		int type = lua_tointeger(luaState, 3);
		int record_begin = lua_tointeger(luaState, 4);
		int record_end = lua_tointeger(luaState, 5);
		

		if( type < 0 ){
			poco_notice_f2(logger_, "Call CoolClientProxy::SearchResource with keywords : %s, invalid type : %d", keywords, type);
			return 0;
		}
		if( record_begin < 0 || record_end < 0 || record_begin >= record_end ){
			poco_notice_f2(logger_, "Call CoolClientProxy::SearchResource with Invalid range ' %d - %d'", record_begin, record_end);
			return 0;
		}

		if( last_request_keywords != keywords || last_request_type != type ){
			//a new search 
			last_request_keywords = keywords;
			last_request_type = type;
			//Add get Total Count of records of this search
			//total_record_count = ...
			//current use this nasty way to get it.
			InfoList tmpList;
			retcode_t ret = pCoolClient->SearchResource(keywords, type, 
				0, 999999, &tmpList);
			if( ret != ERROR_OK ){
				poco_warning_f1(logger_, "Cannot get total_record_count, pCoolClient->SearchResource returns %d", (int)ret);
				return 0;
			}
			total_record_count = tmpList.size();
		}

		InfoList records;
		poco_debug_f4(logger_, "Keywords : %s, type : %d, begin : %d, end : %d", GBK2UTF8(keywords), type, record_begin, record_end);
		retcode_t ret = pCoolClient->SearchResource(keywords, type, 
												record_begin, record_end, &records);
		if( ret != ERROR_OK ){
			poco_warning_f1(logger_, "Call CoolClient::SearchResource returns %d", (int)ret);
			lua_pushinteger(luaState, (int)ret);
			return 1;
		}

		size_t record_count = records.size();

		
		
		for(size_t i = 0; i != record_count; ++i){
			Info& oneRecord = records[i];
			string name(GBK2UTF8(oneRecord.filename()));
			string released_time(oneRecord.time());
			int fileid(oneRecord.fileid());
			Int64 file_size(oneRecord.size());
			int file_type(oneRecord.type());
			int download_total = 9999;
			int upload_total = 1;
		
			int nNowTop = lua_gettop(luaState);
			lua_rawgeti(luaState, LUA_REGISTRYINDEX, functionRef);
			lua_createtable(luaState, 0, 7);

			lua_pushstring(luaState, "TotalCount");
			lua_pushinteger(luaState, total_record_count);
			lua_settable(luaState, -3);

			lua_pushstring(luaState, "TorrentId");
			lua_pushinteger(luaState, fileid);
			lua_settable(luaState, -3);

			lua_pushstring(luaState, "Type");
			lua_pushinteger(luaState, file_type);
			lua_settable(luaState, -3);

			lua_pushstring(luaState, "Name");
			lua_pushstring(luaState, name.c_str());
			lua_settable(luaState, -3);

			lua_pushstring(luaState, "Time");
			lua_pushstring(luaState, released_time.c_str());
			lua_settable(luaState, -3);

			lua_pushstring(luaState, "Size");
			lua_pushinteger(luaState, file_size);
			lua_settable(luaState, -3);

			lua_pushstring(luaState, "Upload");
			lua_pushinteger(luaState, upload_total);
			lua_settable(luaState, -3);

			lua_pushstring(luaState, "Download");
			lua_pushinteger(luaState, download_total);
			lua_settable(luaState, -3);
			
			//DumpLuaState(luaState);
			int nLuaResult = XLLRT_LuaCall(luaState,1,0, L"CoolClientProxy::SearchResource");
			lua_settop(luaState,nNowTop);
		}
	}
	return 0;
}

int CoolClientProxy::GetResourceTorrentById(lua_State* luaState){
	int torrent_id = lua_tointeger(luaState, 1);
	string torrent_name = lua_tostring(luaState, 2);
	poco_debug_f2(logger_, "Call CoolClientProxy::GetResourceTorrentById with torrent_id : %d, torrent_name : %s",
		torrent_id, torrent_name);
	retcode_t ret = pCoolClient->GetResourceTorrentById(torrent_id, UTF82GBK(torrent_name) );
	if( ret != ERROR_OK ){
		poco_warning_f1(logger_, "pCoolClient->GetResourceTorrentById returns %d", (int)ret);
	}

	return 0;
}

int CoolClientProxy::StopClient(lua_State* luaState){

	poco_trace(logger_, "Call CoolClientProxy::StopClient");
	pCoolClient->StopClient();
	while( pCoolClient->exiting() == false);
	return 0;
}

int CoolClientProxy::TestTable(lua_State* luaState){
	//poco_notice_f1(pCoolClient->logger(), "Call TestTable with argument type : %s", string(luaL_typename(luaState, 1)));
	//int top = lua_gettop(luaState);
	//for(int i = 0; i <= top; ++i){
	//	poco_notice_f2(pCoolClient->logger(), "lua state index %d is %s", i, string(luaL_typename(luaState, i)));
	//}
	//CoolClientProxy** ppCoolClientProxy = reinterpret_cast<CoolClientProxy**>(luaL_checkudata(luaState, 1, COOLCLIENT_PROXY_LUA_CLASS));
	//if( lua_isfunction(luaState, 2) ){
	//	int nNowTop = lua_gettop(luaState);
	//	long functionRef = luaL_ref(luaState,LUA_REGISTRYINDEX);
	//	lua_rawgeti(luaState, LUA_REGISTRYINDEX, functionRef);
	//	lua_createtable(luaState, 0, 1);
	//	lua_pushinteger(luaState, 1);
	//	lua_pushinteger(luaState, 2);
	//	lua_settable(luaState, -3);
	//	int nLuaResult = XLLRT_LuaCall(luaState,1,0, L"CoolClientProxy::TestTable");
	//	lua_settop(luaState,nNowTop);
	//}

	return 0;

}

void CoolClientProxy::RegisterObj(XL_LRT_ENV_HANDLE hEnv){
	if(hEnv == NULL)
	{
		return ;
	}

	XLLRTObject theObject;
	theObject.ClassName = COOLCLIENT_PROXY_LUA_CLASS;
	theObject.MemberFunctions = CoolClientProxyMemberFunctions;
	theObject.ObjName = COOLCLIENT_PROXY_LUA_OBJ;
	theObject.userData = NULL;
	theObject.pfnGetObject = (fnGetObject)CoolClientProxy::Instance;

	XLLRT_RegisterGlobalObj(hEnv,theObject); 
}

void CoolClientProxy::DumpLuaState(lua_State* luaState){
	Logger& logger_ = pCoolClient->logger();
	int top = lua_gettop(luaState);
	poco_notice(logger_, "********************************************************************************");
	for(int i = 0; i <= top; ++i){
		poco_notice_f2(pCoolClient->logger(), "lua state index %d is %s", i, string(luaL_typename(luaState, i)));
	}
}