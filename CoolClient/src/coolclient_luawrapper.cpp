#include "coolclient_luawrapper.h"
#include <string>
using std::string;

bool CoolClientLuaWrapper::CheckClassPointer(CoolClient** ppCoolClient){
	return ppCoolClient && *ppCoolClient;
}

CoolClient** CoolClientLuaWrapper::GetCoolClientObj(lua_State* luaState){
	return reinterpret_cast<CoolClient**>( luaL_checkudata(luaState, 1, COOLCLIENT_LUA_CLASS) );
}

int CoolClientLuaWrapper::HasThisTorrent(lua_State* luaState){

	CoolClient** ppCoolClient = GetCoolClientObj(luaState);
	if( CheckClassPointer(ppCoolClient) == true){
		string torrent_id = lua_tostring(luaState, 2);
		bool has_this_torrent = (*ppCoolClient)->HasThisTorrent(torrent_id);
		lua_pushboolean(luaState, has_this_torrent);
	}else{
		lua_pushnil(luaState);
	}
	return 0;
}

int CoolClientLuaWrapper::RunClientAsync(lua_State* luaState){
	CoolClient** ppCoolClient = GetCoolClientObj(luaState);
	if( CheckClassPointer(ppCoolClient) ){
		ClientThread* pThread = new ClientThread(*ppCoolClient);
		pThread->run();
		lua_pushboolean(luaState, 1);
	}
	lua_pushboolean(luaState, 0);
	return 0;
}

int CoolClientLuaWrapper::DeleteSelf(lua_State* luaState){
	CoolClient** ppCoolClient = GetCoolClientObj(luaState);
	if( CheckClassPointer(ppCoolClient) ){
		delete *ppCoolClient;
	}
	return 0;
}

static XLLRTGlobalAPI CoolClientLuaWrapperMemberFunctions[] = {

	{"HasThisTorrent",CoolClientLuaWrapper::HasThisTorrent},
	{"RunClientAsync", CoolClientLuaWrapper::RunClientAsync},
	//{"AttachResultListener",LuaMyClass::AttachResultListener},
	{"__gc",CoolClientLuaWrapper::DeleteSelf},
	{NULL,NULL}
};

void CoolClientLuaWrapper::RegisterClass(XL_LRT_ENV_HANDLE hEnv){
	if( hEnv ){
		long res = XLLRT_RegisterClass(hEnv,COOLCLIENT_LUA_CLASS, CoolClientLuaWrapperMemberFunctions,NULL,0);
	}
}

int CoolClientLuaWrapperFactory::CreateInstance(lua_State* luaState){

	CoolClientLuaWrapper* pCoolClient = new CoolClientLuaWrapper();
	XLLRT_PushXLObject(luaState,COOLCLIENT_LUA_CLASS, pCoolClient);
	return 0;
}

CoolClientLuaWrapperFactory* __stdcall CoolClientLuaWrapperFactory::Instance(void*){

	static CoolClientLuaWrapperFactory* pCoolClientLuaWrapperFactory = NULL;
	if(pCoolClientLuaWrapperFactory == NULL)
	{
		pCoolClientLuaWrapperFactory = new CoolClientLuaWrapperFactory();
	}
	return pCoolClientLuaWrapperFactory;
}

static XLLRTGlobalAPI CoolClientLuaWrapperFactoryMemberFunctions[] = {

	{"CreateInstance",CoolClientLuaWrapperFactory::CreateInstance},
	{NULL,NULL}
};

void CoolClientLuaWrapperFactory::RegisterObj(XL_LRT_ENV_HANDLE hEnv){
	if(hEnv == NULL)
	{
		return ;
	}

	XLLRTObject theObject;
	theObject.ClassName = COOLCLIENT_FACTORY_LUA_CLASS;
	theObject.MemberFunctions = CoolClientLuaWrapperFactoryMemberFunctions;
	theObject.ObjName = COOLCLIENT_FACTORY_LUA_OBJ;
	theObject.userData = NULL;
	theObject.pfnGetObject = (fnGetObject)CoolClientLuaWrapperFactory::Instance;

	XLLRT_RegisterGlobalObj(hEnv,theObject); 
}