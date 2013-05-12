#ifndef COOLCLIENTPROXY
#define COOLCLIENTPROXY

#define COOLCLIENT_PROXY_LUA_CLASS "CoolDown.CoolClient.Proxy.Class"
#define COOLCLIENT_PROXY_LUA_OBJ "CoolDown.CoolClient.Proxy"

#include "client.h"
#include <Windows.h>
#include <XLLuaRuntime.h>
#include <Poco/Logger.h>
#include <boost/atomic.hpp>

using Poco::Logger;
using CoolDown::Client::CoolClient;
using CoolDown::Client::ClientThread;
using CoolDown::Client::MakeTorrentProgressObj;
using boost::atomic_bool;

class CoolClientProxy{
public:
	//CoolClientProxy();
	//called by lua files
	static int RunClientAsync(lua_State* luaState);
	static int TestTable(lua_State* luaState);
	static int SearchResource(lua_State* luaState);
	static int GetResourceTorrentById(lua_State* luaState);
	static int ChoosePath(lua_State* luaState);
	static int MakeTorrentAndPublish(lua_State* luaState);
	static int StopMakingTorrent(lua_State* luaState);
	static int StopClient(lua_State* luaState);

public:
	//work with lua runtime
	static CoolClientProxy* __stdcall Instance(void*);
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);

private:
	static bool MakeTorrentProgressCallback(int current_count, int total_count, lua_State* luaState, long functionRef);
	static string GetTorrentNameByResourcePath(const string& resource_path);
	static void DumpLuaState(lua_State* luaState);
	
	static CoolClient* pCoolClient;
	static Logger& logger_;
	static atomic_bool stop_making_torrent;
};

#endif