#include "coolclientproxy.h"
#include "utilities.h"
#include "job_info.h"
#include "make_torrent_runnable.h"
#include <Commdlg.h>
#include <Shlobj.h>
#include <string>
#include <set>
#include <boost/bind.hpp>
#include <Poco/ThreadPool.h>
#include <Poco/Logger.h>
#include <Poco/Types.h>
#include <Poco/Path.h>
#include <Poco/Random.h>
#include <boost/interprocess/shared_memory_object.hpp>

using std::set;
using std::string;
using Poco::Int64;
using CoolDown::Client::MakeTorrentRunnable;

static Poco::Random rand_gen;
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
			return GBK2UTF8(string(szFile));
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
			return GBK2UTF8(string(szDir));
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
	//seed the Random generator
	rand_gen.seed();
	//just run once, so no resource/memroy leak
	ClientRunnable* backgroudClient = new ClientRunnable(CoolClientProxy::pCoolClient);
	Poco::Thread* pThread = new Poco::Thread;
	//create the global table for job status used in lua
	{
		lua_newtable(luaState);
		lua_setfield(luaState, LUA_GLOBALSINDEX, JOB_STATUS_TABLE_NAME);
	}
	//set the job status callback so we get the job status automaticlly
	//pCoolClient->set_job_status_callback(boost::bind<void>(&CoolClientProxy::JobStatusCallback, luaState, _1));
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

			int download_total = rand_gen.next(20);
			int upload_total = rand_gen.next(10);
		
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
		torrent_name += ".cd";
		poco_debug_f2(logger_, "Call CoolClientProxy::GetResourceTorrentById with torrent_id : %d, torrent_name : %s",
			torrent_id, torrent_name);
		string local_torrent_path;
		retcode_t ret = pCoolClient->DownloadTorrent(torrent_id, UTF82GBK(torrent_name));
		//retcode_t ret = pCoolClient->GetResourceTorrentById(torrent_id, UTF82GBK(torrent_name), &local_torrent_path);
		if( ret != ERROR_OK ){
			poco_warning_f1(logger_, "pCoolClient->GetResourceTorrentById returns %d", (int)ret);
			lua_pushinteger(luaState, -1);
			return 1;
		}else{
			string gb_torrent_path = pCoolClient->get_torrent_path(UTF82GBK(torrent_name));
			Torrent::Torrent torrent;
			retcode_t parse_ret = pCoolClient->ParseTorrent(gb_torrent_path, &torrent);
			if( parse_ret != ERROR_OK ){
				poco_warning_f2(logger_, "in CoolClientProxy::SelectTorrent, cannot parsetorrent of path : %s, ret : %d", 
					GBK2UTF8(gb_torrent_path), (int)parse_ret);
				lua_pushinteger(luaState, -1);
				return 1;
			}else{
				int file_count = torrent.file().size();
				Path p(gb_torrent_path);
				string name = p.getBaseName();
				lua_pushstring(luaState, GBK2UTF8(gb_torrent_path).c_str());
				lua_pushstring(luaState, GBK2UTF8(name).c_str());
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
	string gb_torrent_path = UTF82GBK(torrent_path);
	Torrent::Torrent torrent;
	retcode_t parse_ret = pCoolClient->ParseTorrent(gb_torrent_path, &torrent);
	if( parse_ret != ERROR_OK ){
		poco_warning_f2(logger_, "in CoolClientProxy::SelectTorrent, cannot parsetorrent of path : %s, ret : %d", 
			torrent_path, (int)parse_ret);
		lua_pushinteger(luaState, -1);
		return 1;
	}else{
		int file_count = torrent.file().size();
		Path p(gb_torrent_path);
		string name = p.getBaseName();
		lua_pushstring(luaState, torrent_path.c_str());
		lua_pushstring(luaState, GBK2UTF8(name).c_str());
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
			string gb_torrent_path( UTF82GBK(torrent_path) );
			string local_path = lua_tostring(luaState, 3);
			CoolDown::Client::FileIdentityInfoList needs;
			poco_trace(logger_, "Before traverse the needs file table");
			
			int table_index = lua_gettop(luaState);
			lua_pushnil(luaState);
			//lua_pushinteger(luaState, 1);
			vector<string> filenames;
			while(lua_next(luaState, table_index) != 0){
				int key = lua_tointeger(luaState, -2);
				string value = lua_tostring(luaState, -1);
				filenames.push_back( value );
				lua_pop(luaState, 1);
			}

			for(int i = 0; i != filenames.size(); ++i){
				string full_relative_path( filenames[i] );
				Path p(full_relative_path);
				string filename = p.getFileName();
				string relative_path = p.parent().toString();
				poco_trace_f3(logger_, "Split %s into path : %s, name : %s",
					full_relative_path, filename, relative_path);
				needs.push_back(CoolDown::Client::FileIdentityInfo(relative_path, filename));
			}
			
			Torrent::Torrent torrent;
			poco_assert(ERROR_OK == pCoolClient->ParseTorrent(gb_torrent_path, &torrent));
			int handle;
			retcode_t ret = pCoolClient->AddNewDownloadJob(gb_torrent_path, torrent, needs, local_path + Path::separator(), &handle);
			if( ERROR_OK != ret ){
				poco_warning_f1(logger_, "in CoolClientProxy::AddNewDownload, CoolClient::AddNewDownloadJob returns %d",
					(int)ret);
				lua_pushinteger(luaState, -1);
				return 1;
			}else{
				poco_trace_f3(logger_, "in CoolClientProxy::AddNewDownload, add new download succeed, "
					"torrent_path : %s, local_path : %s, count of files to download : %d",
					torrent_path, local_path, (int)needs.size());
				pCoolClient->StartJob(handle);
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
		string top_path = Path(local_path).parent().toString();
		Torrent::Torrent torrent;
		poco_assert(ERROR_OK == pCoolClient->ParseTorrent(torrent_path, &torrent));
		int handle;
		poco_debug_f2(logger_, "in CoolClientProxy::AddNewUpload, torrent_path : %s, top_path : %s",
			torrent_path, top_path);
		retcode_t ret = pCoolClient->AddNewUploadJob(torrent_path, top_path, torrent, &handle);
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

int CoolClientProxy::GetJobStatusTable(lua_State* luaState){
	int top = lua_gettop(luaState);
	lua_getfield(luaState, LUA_GLOBALSINDEX, JOB_STATUS_TABLE_NAME);
	int table_index = lua_gettop(luaState);
	CoolDown::Client::JobStatusMap job_status;
	job_status.swap( pCoolClient->JobStatuses() );
	set<int> already_processed_handles;

	//process the job info that already in the lua table
	lua_pushnil(luaState);
	while(lua_next(luaState, table_index) != 0){
		//now the key is at -2, value is at -1

		//value is a table, so we get the attr Handle from it
		lua_getfield(luaState, -1, "Handle");
		//now the Handle is at -1
		if( lua_isnumber(luaState, -1) ){
			int handle = lua_tonumber(luaState, -1);
			already_processed_handles.insert(handle);
			CoolDown::Client::JobStatusMap::iterator iter = job_status.find(handle);

			if( iter == job_status.end() ){
				//this job has been removed, so we delete this job info table
				//notice the 2 is ( handle + table )
				lua_pop(luaState, 2);
				//push nil to gc
				lua_pushnil(luaState);
			}else{
				//we still have this job, so pop the handle
				lua_pop(luaState, 1);
				//now the job info table is at -1
				UpdateJobStatusTable(luaState, iter->second);
			}
			//the job info(value) or a nil value(for removed job) is at -1, so we must pop it to do the next iteration
			lua_pop(luaState, 1);

		}else{
			poco_warning_f1(logger_, "Invalid Handle type : %s", string(lua_typename(luaState, -1)) );
			lua_pushinteger(luaState, -1);
			break;
		}
	}
	
	//make the global job info table is at the top
	lua_settop(luaState, table_index);
	//process new added job
	CoolDown::Client::JobStatusMap::const_iterator citer = job_status.begin();
	CoolDown::Client::JobStatusMap::const_iterator cend = job_status.end();
	while( citer != cend ){
		//pre condition : the global table is at the top of stack
		//post condition : the global table is still at the top of stack
		if( already_processed_handles.find(citer->first) == already_processed_handles.end() ){
			//this job info has not been added to lua table
			lua_createtable(luaState, 0, 9);
			int this_table_index = lua_gettop(luaState);

			const CoolDown::Client::JobStatus& status = citer->second;
			lua_pushstring(luaState, "Handle");
			lua_pushinteger(luaState, status.handle);
			lua_settable(luaState, this_table_index);

			lua_pushstring(luaState, "Name");
			lua_pushstring(luaState, status.name.c_str());
			lua_settable(luaState, this_table_index);

			lua_pushstring(luaState, "Type");
			lua_pushinteger(luaState, (int)status.type);
			lua_settable(luaState, this_table_index);

			lua_pushstring(luaState, "Size");
			lua_pushnumber(luaState, status.size);
			lua_settable(luaState, this_table_index);

			UpdateJobStatusTable(luaState, status);
			//now the table for this job is still at the top
			//so we add this table to the global table
			//first the the length of this table
			unsigned len = lua_objlen(luaState, table_index);
			//then push the next key to stack
			lua_pushinteger(luaState, len + 1);
			//and set the value
			//now key is at -1, value is at -2
			//so we exchange it 
			lua_insert(luaState, -2);
			//now key is at -2, value is at -1, so we set table
			lua_settable(luaState, table_index);
			//now both the key(handle) and the value(table for this job) is poped, so post condition satisfied
		}else{
			//we have processed this job
		}
		++citer;
	}
	lua_settop(luaState, table_index);
	return 1;
	//CoolDown::Client::JobStatusMap job_status;
	//job_status.swap( pCoolClient->JobStatuses() );
	////const CoolDown::Client::JobStatusMap& job_status = pCoolClient->ConstJobStatuses();
	//int status_count = job_status.size();
	//CoolDown::Client::JobStatusMap::const_iterator citer = job_status.begin();
	//CoolDown::Client::JobStatusMap::const_iterator cend = job_status.end();

	//while( citer != cend ){
	//	int this_top = lua_gettop(luaState);
	//	int handle = citer->first;
	//	const CoolDown::Client::JobStatus& status = citer->second;
	//	//lua_pushinteger(luaState, handle);
	//	lua_pushinteger(luaState, handle);
	//	lua_gettable(luaState, table_index);
	//	poco_notice(logger_, "Before judge the type of obj in given index");
	//	//DumpLuaState(luaState);
	//	if( lua_type(luaState, -1) == LUA_TTABLE) {
	//		//we have this job info in table, so just update it
	//	}
	//	else if( lua_type(luaState, -1) == LUA_TNIL){
	//		//DumpLuaState(luaState);
	//		lua_pop(luaState, 1);
	//		//DumpLuaState(luaState);
	//		//we create the job info table
	//		lua_createtable(luaState, 0, 8);
	//		//DumpLuaState(luaState);
	//		int this_table_index = lua_gettop(luaState);
	//		//push invariant variables here( eg : Name, Type, Size )
	//		lua_pushstring(luaState, "Name");
	//		lua_pushstring(luaState, status.name.c_str());
	//		lua_settable(luaState, this_table_index);

	//		lua_pushstring(luaState, "Type");
	//		lua_pushinteger(luaState, (int)status.status);
	//		lua_settable(luaState, this_table_index);

	//		lua_pushstring(luaState, "Size");
	//		lua_pushnumber(luaState, status.size);
	//		lua_settable(luaState, this_table_index);

	//	}else{
	//		poco_warning_f2(logger_, "Unknown type of lua_Status at index : %d, type : %d",
	//			handle, lua_type(luaState, -1));
	//		lua_pushinteger(luaState, -1);
	//		return 1;
	//	}
	//	//when we are here, the top of the stack is the table for this job
	//	//we just update the variants
	//	//DumpLuaState(luaState);
	//	UpdateJobStatusTable(luaState, status);
	//	//DumpLuaState(luaState);
	//	lua_pushinteger(luaState, handle);
	//	lua_insert(luaState, -2);
	//	//poco_notice(logger_, "Before set table of one job");
	//	//DumpLuaState(luaState);
	//	lua_settable(luaState, table_index);
	//	lua_settop(luaState, this_top);
	//	++citer;
	//}
	//return 1;
}

int CoolClientProxy::StopMakingTorrent(lua_State* luaState){
	stop_making_torrent = true;
	return 0;
}


int CoolClientProxy::StopClient(lua_State* luaState){

	poco_trace(logger_, "Call CoolClientProxy::StopClient");
	pCoolClient->StopClient();
	while( pCoolClient->exiting() == false);
	using namespace boost::interprocess;
	shared_memory_object::remove(COOLCLIENT_UNIQUE_ID);
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

int CoolClientProxy::PauseJob(lua_State* luaState){
	if( lua_isnumber(luaState, -1) ){
		int job_handle = lua_tointeger(luaState, -1);
		retcode_t ret = pCoolClient->PauseJob(job_handle);
		if( ret != ERROR_OK ){
			lua_pushinteger(luaState, -1);
			return 1;
		}
		return 0;

	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::PauseJob");
		DumpLuaState(luaState);
		return 0;
	}
}

int CoolClientProxy::ResumeJob(lua_State* luaState){
	if( lua_isnumber(luaState, -1) ){
		int job_handle = lua_tointeger(luaState, -1);
		retcode_t ret = pCoolClient->ResumeJob(job_handle);
		if( ret != ERROR_OK ){
			lua_pushinteger(luaState, -1);
			return 1;
		}
		return 0;

	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::ResumeJob");
		DumpLuaState(luaState);
		return 0;
	}
}
int CoolClientProxy::StopJob(lua_State* luaState){
	if( lua_isnumber(luaState, -1) ){
		int job_handle = lua_tointeger(luaState, -1);
		retcode_t ret = pCoolClient->StopJob(job_handle);
		if( ret != ERROR_OK ){
			lua_pushinteger(luaState, -1);
			return 1;
		}
		return 0;

	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::StopJob");
		DumpLuaState(luaState);
		return 0;
	}
}

int CoolClientProxy::RemoveJob(lua_State* luaState){
	if( lua_isnumber(luaState, -1) ){
		int job_handle = lua_tointeger(luaState, -1);
		retcode_t ret = pCoolClient->RemoveJob(job_handle);
		if( ret != ERROR_OK ){
			lua_pushinteger(luaState, -1);
			return 1;
		}
		return 0;

	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::RemoveJob");
		DumpLuaState(luaState);
		return 0;
	}
}

int CoolClientProxy::SetConfig(lua_State* luaState){
	if( lua_isstring(luaState, 2)		//config key
		&& lua_isstring(luaState, 3)	//config value
		){
		string key = lua_tostring(luaState, 2);
		string value = lua_tostring(luaState, 3);
		int ret = pCoolClient->SetConfig(key, value);
		if( ret == -1 ){
			lua_settop(luaState, 1);
			lua_pushinteger(luaState, -1);
			return 1;
		}else{
			return 0;
		}

	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::SetConfig");
		DumpLuaState(luaState);
		return 0;
	}
}

int CoolClientProxy::GetConfig(lua_State* luaState){
	if( lua_isstring(luaState, 2) ){
		string key = lua_tostring(luaState, 2);
		string value = pCoolClient->GetConfig(key);
		lua_pushstring(luaState, value.c_str());
	}else{
		poco_warning(logger_, "Invalid args of CoolClientProxy::SetConfig");
		DumpLuaState(luaState);
		return 0;
	}
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
	{"GetJobStatusTable", CoolClientProxy::GetJobStatusTable},
	{"RemoveJob", CoolClientProxy::RemoveJob},
	{"StopJob", CoolClientProxy::StopJob},
	{"ResumeJob", CoolClientProxy::ResumeJob},
	{"PauseJob", CoolClientProxy::PauseJob},
	{"SetConfig", CoolClientProxy::SetConfig},
	{"GetConfig", CoolClientProxy::GetConfig},
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
	return;
}

void CoolClientProxy::UpdateJobStatusTable(lua_State* luaState, const CoolDown::Client::JobStatus& status){

	//pre condition : the table of This job is at the top of luaState
	//post condition : the table is filled with info and still at the top of luaState
	poco_assert(lua_type(luaState, -1) == LUA_TTABLE);
	/*
	table = {Status, Type, Name, Size, Progress, Retime, DownloadSpeed, UploadSpeed}
	*/
	
	lua_pushstring(luaState, "Status");
	lua_pushinteger(luaState, (int)status.status);
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "Progress");
	lua_pushnumber(luaState, status.percentage);
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "DownloadSpeed");
	lua_pushinteger(luaState, status.download_speed_per_second_in_bytes);
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "UploadSpeed");
	lua_pushinteger(luaState, status.upload_speed_per_second_in_bytes);
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "Retime");
	lua_pushnumber(luaState, status.remaining_time_in_seconds);
	lua_settable(luaState, -3);

	poco_assert(lua_type(luaState, -1) == LUA_TTABLE);
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
	poco_notice_f1(logger_, "***************************************%d****************************************", top);
	for(int i = 0; i <= top; ++i){
		string type = luaL_typename(luaState, i);
		poco_notice_f2(pCoolClient->logger(), "lua state index %d is %s", i, type);
	}
}