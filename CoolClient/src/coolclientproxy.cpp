#include "coolclientproxy.h"
#include <string>
#include <Poco/Thread.h>
#include <Poco/Logger.h>
using std::string;

CoolClient* CoolClientProxy::pCoolClient = new CoolClient;

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

	CoolClientProxy** ppCoolClientProxy = reinterpret_cast<CoolClientProxy**>(luaL_checkudata(luaState, 1, COOLCLIENT_PROXY_LUA_CLASS));
	if( lua_isfunction(luaState, 4) ){
		string keywords = lua_tostring(luaState, 2);
		int type = lua_tointeger(luaState, 3);
		long functionRef = luaL_ref(luaState,LUA_REGISTRYINDEX);

		
		lua_createtable(luaState, 0, 6);

		lua_pushstring(luaState, "Type");
		lua_pushstring(luaState, "ÒôÀÖ");
		lua_settable(luaState, -3);

		lua_pushstring(luaState, "Name");
		lua_pushstring(luaState, "King of Fight");
		lua_settable(luaState, -3);

		lua_pushstring(luaState, "Size");
		lua_pushstring(luaState, "1048576");
		lua_settable(luaState, -3);

		lua_pushstring(luaState, "Upload");
		lua_pushstring(luaState, "1");
		lua_settable(luaState, -3);

		lua_pushstring(luaState, "Download");
		lua_pushstring(luaState, "99");
		lua_settable(luaState, -3);

		int nNowTop = lua_gettop(luaState);
		lua_rawgeti(luaState, LUA_REGISTRYINDEX, functionRef);

		int nLuaResult = XLLRT_LuaCall(luaState,1,0, L"CoolClientProxy::SearchResource");
		lua_settop(luaState,nNowTop);
	}
	return 0;
}

int CoolClientProxy::TestTable(lua_State* luaState){
	//poco_notice_f1(pCoolClient->logger(), "Call TestTable with argument type : %s", string(luaL_typename(luaState, 1)));
	/*int top = lua_gettop(luaState);
	for(int i = 0; i <= top; ++i){
		poco_notice_f2(pCoolClient->logger(), "lua state index %d is %s", i, string(luaL_typename(luaState, i)));
	}*/
	CoolClientProxy** ppCoolClientProxy = reinterpret_cast<CoolClientProxy**>(luaL_checkudata(luaState, 1, COOLCLIENT_PROXY_LUA_CLASS));
	if( lua_isfunction(luaState, 2) ){
		int nNowTop = lua_gettop(luaState);
		long functionRef = luaL_ref(luaState,LUA_REGISTRYINDEX);
		lua_rawgeti(luaState, LUA_REGISTRYINDEX, functionRef);
		lua_createtable(luaState, 0, 1);
		lua_pushinteger(luaState, 1);
		lua_pushinteger(luaState, 2);
		lua_settable(luaState, -3);
		int nLuaResult = XLLRT_LuaCall(luaState,1,0, L"CoolClientProxy::TestTable");
		lua_settop(luaState,nNowTop);
	}

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