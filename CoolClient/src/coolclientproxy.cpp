#include "coolclientproxy.h"
#include "utilities.h"
#include "job_info.h"
#include "make_torrent_runnable.h"
#include <Commdlg.h>
#include <Shlobj.h>
#include <string>
#include <boost/bind.hpp>
#include <Poco/ThreadPool.h>
#include <Poco/Logger.h>
#include <Poco/Types.h>
#include <Poco/Path.h>

using std::string;
using Poco::Int64;
using CoolDown::Client::MakeTorrentRunnable;

namespace{
	string OpenFileSelectDialog(const string& init_path){
		OPENFILENAMEA ofn;      // 公共对话框结构。
		CHAR szFile[MAX_PATH]; // 保存获取文件名称的缓冲区。          

		// 初始化选择文件对话框。
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		//
		//
		ofn.lpstrFile[0] = L'\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = NULL;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = init_path.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// 显示打开选择文件对话框。
		if ( GetOpenFileNameA(&ofn) )
		{
			return string(szFile);
			//显示选择的文件。
			//MessageBox(NULL,szFile,_T("File Selected"),MB_OK);
		}else{
			return string();
		}
	}

	string OpenDirSelectDialog(const string& init_path){
		CHAR szDir[MAX_PATH];
		BROWSEINFOA bInfo;
		bInfo.hwndOwner = NULL;
		bInfo.pidlRoot = NULL; 
		bInfo.pszDisplayName = szDir; // Address of a buffer to receive the display name of the folder selected by the user
		bInfo.lpszTitle = "请选择一个目录"; // Title of the dialog
		bInfo.ulFlags = 0 ;
		bInfo.lpfn = NULL;
		bInfo.lParam = 0;
		bInfo.iImage = -1;

		LPITEMIDLIST lpItem = SHBrowseForFolderA( &bInfo);
		if( lpItem != NULL )
		{
			SHGetPathFromIDListA(lpItem, szDir);
			return string(szDir);
			//MessageBoxA(NULL, szDir, "目录选择成功", MB_OK);
		}else{
			return string();
		}
	}
}

CoolClient* CoolClientProxy::pCoolClient(new CoolClient);
Logger& CoolClientProxy::logger_( CoolClientProxy::pCoolClient->logger() );
atomic_bool CoolClientProxy::stop_making_torrent(false);

CoolClientProxy* __stdcall CoolClientProxy::Instance(void*){

	static CoolClientProxy* pCoolClientProxy = NULL;
	if(pCoolClientProxy == NULL)
	{
		pCoolClientProxy = new CoolClientProxy();
	}
	return pCoolClientProxy;
}



int CoolClientProxy::RunClientAsync(lua_State* luaState){
	//just run once, so no resource/memroy leak
	ClientRunnable* backgroudClient = new ClientRunnable(CoolClientProxy::pCoolClient);
	Poco::Thread* pThread = new Poco::Thread;
	//create the global table for job status used in lua
	{
		lua_newtable(luaState);
		lua_setfield(luaState, LUA_GLOBALSINDEX, JOB_STATUS_TABLE_NAME);
	}
	//set the job status callback so we get the job status automaticlly
	pCoolClient->set_job_status_callback(boost::bind<void>(&CoolClientProxy::JobStatusCallback, luaState, _1));
	pThread->start(*backgroudClient);
	return 0;
}

int CoolClientProxy::SearchResource(lua_State* luaState){
	using CoolDown::Client::InfoList;
	
	//CoolClientProxy** ppCoolClientProxy = reinterpret_cast<CoolClientProxy**>(luaL_checkudata(luaState, 1, COOLCLIENT_PROXY_LUA_CLASS));
	static string last_request_keywords;
	static int last_request_type;
	static int total_record_count = 0;

	//if this is the first search of this keywords and type
	bool new_search = false;
	//TODO: Get The total count of records when first call.
	if( lua_isfunction(luaState, 6) ){
		long functionRef = luaL_ref(luaState,LUA_REGISTRYINDEX);
		string keywords( lua_tostring(luaState, 2) );
		int type = lua_tointeger(luaState, 3);
		int record_begin = lua_tointeger(luaState, 4);
		int record_end = lua_tointeger(luaState, 5);
		

		if( type < 0 ){
			poco_notice_f2(logger_, "Call CoolClientProxy::SearchResource with keywords : %s, invalid type : %d", keywords, type);
			//lua_pushstring(luaState, "")
			return 1;
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

			//InfoList tmpList;
			//retcode_t ret = pCoolClient->SearchResource(keywords, type, 
			//	0, 999999, &tmpList);
			//if( ret != ERROR_OK ){
			//	poco_warning_f1(logger_, "Cannot get total_record_count, pCoolClient->SearchResource returns %d", (int)ret);
			//	return 0;
			//}
			//total_record_count = tmpList.size();
			int count = 0;
			retcode_t ret = pCoolClient->SearchResourceCount(keywords, type, &count);
			if( ret != ERROR_OK ){
				poco_warning_f1(logger_, "Cannot get total_record_count, pCoolClient->SearchResourceCount returns %d", (int)ret);
				return 0;
			}
			total_record_count = count;

		}

		InfoList records;
		poco_debug_f4(logger_, "Keywords : %s, type : %d, begin : %d, end : %d", keywords, type, record_begin, record_end);
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
			string name(oneRecord.filename());
			string released_time(oneRecord.time());
			int fileid(oneRecord.fileid());
			Int64 file_size(oneRecord.size());
			int file_type(oneRecord.type());
			int download_total = 9999;
			int upload_total = 1;
		
			int nNowTop = lua_gettop(luaState);
			lua_rawgeti(luaState, LUA_REGISTRYINDEX, functionRef);
			lua_createtable(luaState, 0, 8);

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
			lua_pushnumber(luaState, file_size);
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
	if( lua_isnumber(luaState, 2) && lua_isstring(luaState, 3) ){
		int torrent_id = lua_tointeger(luaState, 2);
		string torrent_name = lua_tostring(luaState, 3);
		poco_debug_f2(logger_, "Call CoolClientProxy::GetResourceTorrentById with torrent_id : %d, torrent_name : %s",
			torrent_id, torrent_name);
		string local_torrent_path;
		retcode_t ret = pCoolClient->GetResourceTorrentById(torrent_id, torrent_name, &local_torrent_path);
		if( ret != ERROR_OK ){
			poco_warning_f1(logger_, "pCoolClient->GetResourceTorrentById returns %d", (int)ret);
		}
	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::GetResourceTorrentById.");
		DumpLuaState(luaState);
	}


	return 0;
}

int CoolClientProxy::ChoosePath(lua_State* luaState){
	if( lua_isnumber(luaState, 2) && lua_isstring(luaState, 3) && lua_isfunction(luaState, 4) ){
		int path_type = lua_tointeger(luaState, 2);
		string base_path = lua_tostring(luaState, 3);
		long functionRef = luaL_ref(luaState,LUA_REGISTRYINDEX);

		string init_path;
		Poco::Path p(base_path);
		
		if( p.isDirectory() ){
			 init_path = p.toString();
		}else if( p.isFile() ){
			init_path = p.parent().toString();
		}else{
			//leave init_path empty
		}

		string resource_path;
		if( path_type == 1 ){
			//选择文件
			string filename = OpenFileSelectDialog(init_path);
			if(filename.empty()){
				return 0;
			}else{
				resource_path.swap(filename);
			}
		}else if(path_type == 2){
			//选择目录
			string dirname = OpenDirSelectDialog(init_path);
			if( dirname.empty() ){
				return 0;
			}else{
				resource_path.swap(dirname);
			}
		}else{
			poco_warning_f1(logger_, "Invalid path_type : %d", path_type);
			return 0;
		}
		//resource_path = GBK2UTF8(resource_path);
		int top = lua_gettop(luaState);
		lua_rawgeti(luaState, LUA_REGISTRYINDEX, functionRef);
		lua_pushstring(luaState, resource_path.c_str());
		int nLuaResult = XLLRT_LuaCall(luaState,1,0, L"CoolClientProxy::ChoosePath");
		lua_settop(luaState, top);
		return 0;
	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::ChoosePath.");
		DumpLuaState(luaState);
	}

	return 0;
}

int CoolClientProxy::MakeTorrentAndPublish(lua_State* luaState){
	if( lua_isstring(luaState, 2) 
		&& lua_isnumber(luaState, 3)
		&& lua_isstring(luaState, 4)
		&& lua_isstring(luaState, 5)
		&& lua_isfunction(luaState, 6)){
			string path = lua_tostring(luaState, 2);
			string torrent_filename = GetTorrentNameByResourcePath(path);
			int type = lua_tointeger(luaState, 3);
			string tracker_address = lua_tostring(luaState, 4);
			string brief_introduction = lua_tostring(luaState, 5);
			poco_debug_f4(logger_, 
				"Call CoolClientProxy::MakeTorrentAndPublish, path : %s, filename : %s, type : %d, intro : %s",
				path, torrent_filename, type, brief_introduction);

			long functionRef = luaL_ref(luaState,LUA_REGISTRYINDEX);
			const static int chunk_size = 1 << 21;
			MakeTorrentRunnable *p = new MakeTorrentRunnable(pCoolClient, path, torrent_filename, chunk_size, type, tracker_address, 
				boost::bind<bool>( &CoolClientProxy::MakeTorrentProgressCallback, _1, _2, 
				luaState, functionRef) );
			stop_making_torrent = false;
			ThreadPool::defaultPool().start(*p);
			
	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::MakeTorrentAndPublish.");
		DumpLuaState(luaState);
	}
	return 0;
}

string CoolClientProxy::GetTorrentNameByResourcePath(const string& resource_path){
	Path path(resource_path);
	string path_str = path.toString();
	size_t end = path_str.find_last_of(Path::separator());
	string name;
	if( end != path_str.length() - 1 ){
		name = path_str.substr(end+1);
	}else{
		size_t begin = path_str.find_last_of(Path::separator(), end - 1 );
		name = path_str.substr(begin + 1 , end - begin - 1); 
	}   
	return name + ".cd";
}

int CoolClientProxy::SelectTorrent(lua_State* luaState){
	string torrent_path = OpenFileSelectDialog("");
	string utf8_torrent_path = GBK2UTF8(torrent_path);
	Torrent::Torrent torrent;
	retcode_t parse_ret = pCoolClient->ParseTorrent(utf8_torrent_path, &torrent);
	if( parse_ret != ERROR_OK ){
		poco_warning_f2(logger_, "in CoolClientProxy::SelectTorrent, cannot parsetorrent of path : %s, ret : %d", 
			utf8_torrent_path, (int)parse_ret);
		lua_pushinteger(luaState, -1);
		return 1;
	}else{
		int file_count = torrent.file().size();
		Path p(utf8_torrent_path);
		string name = p.getBaseName();
		lua_pushstring(luaState, utf8_torrent_path.c_str());
		lua_pushstring(luaState, name.c_str());
		lua_pushinteger(luaState, torrent.type());
		lua_createtable(luaState, 0, file_count);
		for(int pos = 0; pos != file_count; ++pos){
			const Torrent::File& oneFile = torrent.file(pos);
			string full_relative_path = oneFile.relativepath() + oneFile.filename();
			lua_pushstring(luaState, full_relative_path.c_str());
			lua_pushnumber(luaState, oneFile.size());
			lua_settable(luaState, -3);
		}
		return 4;
	}

}

int CoolClientProxy::AddNewDownload(lua_State* luaState){
	if( lua_isstring(luaState, 2)			//torrent_path
		&& lua_isstring(luaState, 3)		//local_path
		&& lua_istable(luaState, 4)			//files selected
		){
			string torrent_path = lua_tostring(luaState, 2);
			string local_path = lua_tostring(luaState, 3);
			CoolDown::Client::FileIdentityInfoList needs;
			poco_trace(logger_, "Before traverse the needs file table");
			//lua_pushnil(luaState);
			lua_pushinteger(luaState, 1);
			while(lua_next(luaState, -1) != 0){
				string full_relative_path( lua_tostring(luaState, -1) );
				Path p(full_relative_path);
				string filename = p.getFileName();
				string relative_path = p.parent().toString();
				poco_trace_f3(logger_, "Split %s into path : %s, name : %s",
					full_relative_path, filename, relative_path);
				needs.push_back(CoolDown::Client::FileIdentityInfo(relative_path, filename));
				
				lua_pop(luaState, 1);
			}
			
			Torrent::Torrent torrent;
			poco_assert(ERROR_OK == pCoolClient->ParseTorrent(torrent_path, &torrent));
			int handle;
			retcode_t ret = pCoolClient->AddNewDownloadJob(torrent_path, torrent, needs, local_path, &handle);
			if( ERROR_OK != ret ){
				poco_warning_f1(logger_, "in CoolClientProxy::AddNewDownload, CoolClient::AddNewDownloadJob returns %d",
					(int)ret);
				lua_pushinteger(luaState, -1);
				return 1;
			}else{
				poco_trace_f3(logger_, "in CoolClientProxy::AddNewDownload, add new download succeed, "
					"torrent_path : %s, local_path : %s, count of files to download : %d",
					torrent_path, local_path, (int)needs.size());
				return 0;
			}

	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::AddNewDownload");
		DumpLuaState(luaState);
		return 0;
	}
	
}

int CoolClientProxy::AddNewUpload(lua_State* luaState){
	if( lua_isstring(luaState, 2)				//torrent path
		&&lua_isstring(luaState, 3)				//local path
		){
		string torrent_path = lua_tostring(luaState, 2);
		string local_path = lua_tostring(luaState, 3);
		Torrent::Torrent torrent;
		poco_assert(ERROR_OK == pCoolClient->ParseTorrent(torrent_path, &torrent));
		int handle;
		retcode_t ret = pCoolClient->AddNewUploadJob(torrent_path, local_path, torrent, &handle);
		if( ret != ERROR_OK ){
			poco_warning_f1(logger_, "in CoolClientProxy::AddNewUpload, CoolClient::AddNewUploadJob returns %d",
				(int)ret);
			lua_pushinteger(luaState, -1);
			return 1;
		}else{
			poco_trace_f2(logger_, "in CoolClientProxy::AddNewUpload, add new upload succeed, torrent_path : %s, local_path : %s"
				,torrent_path, local_path);
			return 0;
		}
	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::AddNewUpload");
		DumpLuaState(luaState);
		return 0;
	}
}

int CoolClientProxy::StopMakingTorrent(lua_State* luaState){
	stop_making_torrent = true;
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

static XLLRTGlobalAPI CoolClientProxyMemberFunctions[] = {

	//{"CreateInstance",CoolClientProxy::CreateInstance},
	{"RunClientAsync", CoolClientProxy::RunClientAsync},
	{"SearchResource", CoolClientProxy::SearchResource},
	{"GetResourceTorrentById", CoolClientProxy::GetResourceTorrentById},
	{"StopMakingTorrent", CoolClientProxy::StopMakingTorrent},
	{"StopClient", CoolClientProxy::StopClient},
	{"SelectTorrent", CoolClientProxy::SelectTorrent},
	{"AddNewDownload", CoolClientProxy::AddNewDownload},
	{"AddNewUpload", CoolClientProxy::AddNewUpload},
	{"ChoosePath", CoolClientProxy::ChoosePath},
	{"MakeTorrentAndPublish", CoolClientProxy::MakeTorrentAndPublish},
	{NULL,NULL}
};

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

void CoolClientProxy::JobStatusCallback(lua_State* luaState, const CoolDown::Client::JobStatusMap& job_status){
	lua_getfield(luaState, LUA_GLOBALSINDEX, JOB_STATUS_TABLE_NAME);
	int status_count = job_status.size();
	CoolDown::Client::JobStatusMap::const_iterator citer = job_status.begin();
	CoolDown::Client::JobStatusMap::const_iterator cend = job_status.end();
	while( citer != cend ){
		int handle = citer->first;
		const CoolDown::Client::JobStatus& status = citer->second;
		UpdateJobStatusTable(luaState, status);
		++citer;
	}
	return;
}

void CoolClientProxy::UpdateJobStatusTable(lua_State* luaState, const CoolDown::Client::JobStatus& status){

}

bool CoolClientProxy::MakeTorrentProgressCallback(int current_count, int total_count, lua_State* luaState, long functionRef){
	poco_debug_f2(logger_, "Call CoolClientProxy::MakeTorrentProgressCallback with current_count : %d, total_count : %d",
		current_count, total_count);

	int top = lua_gettop(luaState);
	lua_rawgeti(luaState, LUA_REGISTRYINDEX, functionRef);


	if( stop_making_torrent ){
		lua_pushnumber(luaState, -1);
	}else{
		lua_pushnumber(luaState, current_count);
	}

	lua_pushnumber(luaState, total_count);
	XLLRT_LuaCall(luaState, 2, 0, L"CoolClientProxy::MakeTorrentProgressCallback");
	lua_settop(luaState, top);
	return !stop_making_torrent;
}

void CoolClientProxy::DumpLuaState(lua_State* luaState){
	Logger& logger_ = pCoolClient->logger();
	int top = lua_gettop(luaState);
	poco_notice(logger_, "********************************************************************************");
	for(int i = 0; i <= top; ++i){
		poco_notice_f2(pCoolClient->logger(), "lua state index %d is %s", i, string(luaL_typename(luaState, i)));
	}
}