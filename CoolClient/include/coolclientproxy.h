#ifndef COOLCLIENTPROXY
#define COOLCLIENTPROXY

#define COOLCLIENT_PROXY_LUA_CLASS "CoolDown.CoolClient.Proxy.Class"
#define COOLCLIENT_PROXY_LUA_OBJ "CoolDown.CoolClient.Proxy"
#define JOB_STATUS_TABLE_NAME "JobStatusTable"
#define COOLCLIENT_UNIQUE_ID  "AD4S5FQ4E58F5A1FDS231FC56D1C5W"

#include "client.h"
#include <Windows.h>
#include <XLLuaRuntime.h>
#include <Poco/Logger.h>
#include <boost/atomic.hpp>

using Poco::Logger;
using CoolDown::Client::CoolClient;
using CoolDown::Client::ClientRunnable;
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
	static int SelectTorrent(lua_State* luaState);
	static int AddNewDownload(lua_State* luaState);
	static int AddNewUpload(lua_State* luaState);
	static int GetJobStatusTable(lua_State* luaState);
	static int PauseJob(lua_State* luaState);
	static int ResumeJob(lua_State* luaState);
	static int StopJob(lua_State* luaState);
	static int RemoveJob(lua_State* luaState);
	static int SetConfig(lua_State* luaState);
	static int GetConfig(lua_State* luaState);

	static int StopMakingTorrent(lua_State* luaState);
	static int StopClient(lua_State* luaState);

public:
	//work with lua runtime
	static CoolClientProxy* __stdcall Instance(void*);
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);

private:
	static void JobStatusCallback(lua_State* luaState, const CoolDown::Client::JobStatusMap& job_status);
	static void UpdateJobStatusTable(lua_State* luaState, const CoolDown::Client::JobStatus& status);
	static bool MakeTorrentProgressCallback(int current_count, int total_count, lua_State* luaState, long functionRef);
	static string GetTorrentNameByResourcePath(const string& resource_path);
	static void DumpLuaState(lua_State* luaState);
	
	static CoolClient* pCoolClient;
	static Logger& logger_;
	static atomic_bool stop_making_torrent;
};

#endif