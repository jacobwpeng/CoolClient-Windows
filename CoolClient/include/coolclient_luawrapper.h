#ifndef COOLCLIENTLUAWRAPPER
#define COOLCLIENTLUAWRAPPER

#define COOLCLIENT_LUA_CLASS "CoolDown.CoolClient"
#define COOLCLIENT_FACTORY_LUA_CLASS "CoolDown.CoolClient.Factory.Class"
#define COOLCLIENT_FACTORY_LUA_OBJ "CoolDown.CoolClient.Factory"

#include "client.h"
#include <Windows.h>
#include <XLLuaRuntime.h>

using CoolDown::Client::CoolClient;
using CoolDown::Client::ClientThread;


class CoolClientLuaWrapper{
public:
	//api from CoolClient
	static int HasThisTorrent(lua_State* luaState);
	static int RunClientAsync(lua_State* luaState);
public:
	//function used to work with lua
	static int DeleteSelf(lua_State* luaState);
	static void RegisterClass(XL_LRT_ENV_HANDLE hEnv);

private:
	static CoolClient** GetCoolClientObj(lua_State* luaState);
	bool static CheckClassPointer(CoolClient** ppCoolClient);
};

class CoolClientLuaWrapperFactory{
public:
	static int CreateInstance(lua_State* luaState);
	static CoolClientLuaWrapperFactory* __stdcall Instance(void*);
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);
};

#endif