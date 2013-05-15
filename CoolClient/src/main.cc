#include "coolclientproxy.h"
#include <windows.h>
#include <Commdlg.h>
#include <Shlobj.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <XLUE.h>
#include <XLGraphic.h>
#include <XLLuaRuntime.h>
#include <string>
#include <Shlwapi.h>

#include <boost/interprocess/shared_memory_object.hpp>


using namespace std;

const WCHAR* GetResDir()
{
	static WCHAR wszModulePath[MAX_PATH];
	GetModuleFileNameW(NULL,wszModulePath,MAX_PATH);
	PathAppend(wszModulePath, L"..\\..\\XAR");
	return wszModulePath;
}

int __stdcall LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo,const wchar_t* wcszLuaErrorString,PXL_LRT_ERROR_STACK pStackInfo)
{
	static bool s_bEnter = false;
	if (!s_bEnter)
	{
		s_bEnter = true;
		if(pExtInfo != NULL)
		{
			wstring str = wcszLuaErrorString ? wcszLuaErrorString : L"";
			luaState;
			pExtInfo;
			wcszLuaErrorString;
			str += L" @ ";
			str += pExtInfo;

			MessageBoxW(0,str.c_str(),L"Ϊ�˰������ǸĽ�����,�뷴���˽ű�����",MB_ICONERROR | MB_OK);

		}
		else
		{
			MessageBoxW(0,wcszLuaErrorString ? wcszLuaErrorString : L"" ,L"Ϊ�˰������ǸĽ�����,�뷴���˽ű�����",MB_ICONERROR | MB_OK);
		}
		s_bEnter = false;
	}
	return 0;
}



bool InitXLUE()
{
	//��ʼ��ͼ�ο�
	XLGraphicParam param;
	XL_PrepareGraphicParam(&param);
	param.textType = XLTEXT_TYPE_FREETYPE;
	long result = XL_InitGraphicLib(&param);
	result = XL_SetFreeTypeEnabled(TRUE);
	//��ʼ��XLUE,�⺯����һ�����ϳ�ʼ������
	//����˳�ʼ��Lua����,��׼����,XLUELoader�Ĺ���
	result = XLUE_InitLoader(NULL);

	//����һ���򵥵Ľű�������ʾ
	XLLRT_ErrorHandle(LuaErrorHandle);

	return true; 
}

void UninitXLUE()
{
	//�˳�����
	XLUE_Uninit(NULL);
	XLUE_UninitLuaHost(NULL);
	XL_UnInitGraphicLib();
	XLUE_UninitHandleMap(NULL);
}

bool LoadMainXAR()
{
	long result = 0;
	//����XAR������·��
	result = XLUE_AddXARSearchPath(GetResDir());
	//������XAR,��ʱ��ִ�и�XAR�������ű�onload.lua
	result = XLUE_LoadXAR("HelloBolt");
	if(result != 0)
	{
		return false;
	}
	return true;
}

void test_dialog(){
	//TCHAR szDir[MAX_PATH];
	//BROWSEINFO bInfo;
	//bInfo.hwndOwner = NULL;
	//bInfo.pidlRoot = NULL; 
	//bInfo.pszDisplayName = szDir; // Address of a buffer to receive the display name of the folder selected by the user
	//bInfo.lpszTitle = _T("Please, select a folder"); // Title of the dialog
	//bInfo.ulFlags = 0 ;
	//bInfo.lpfn = NULL;
	//bInfo.lParam = 0;
	//bInfo.iImage = -1;

	//LPITEMIDLIST lpItem = SHBrowseForFolder( &bInfo);
	//if( lpItem != NULL )
	//{
	//	SHGetPathFromIDList(lpItem, szDir );
	//	MessageBox(NULL, szDir, _T("Dir selected"), MB_OK);
	//}
	// ���ļ��򿪶Ի������ѡ���ļ�����NewGame
}

bool test_unique(){
	using namespace boost::interprocess;
	try{
		static shared_memory_object shm_obj
			(create_only                  //only create
			,COOLCLIENT_UNIQUE_ID         //name
			,read_only                   //read-write mode
			);
		return true;
	}catch(...){
		return false;
	}
	//return true;

}



int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	// TODO: �ڴ˷��ô��롣

	bool isUnique = test_unique();
	if( isUnique == false ){
		MessageBoxA(NULL, "ֻ������һ��CoolClientʵ��!", NULL, MB_OK);
		return -1;
	}
	if(!InitXLUE())
	{
		MessageBoxW(NULL,L"��ʼ��XLUE ʧ��!",L"����",MB_OK);
		return 1;
	}

	XL_LRT_ENV_HANDLE hEnv = XLLRT_GetEnv(NULL);
	//CoolClientLuaWrapper::RegisterClass(hEnv);
	CoolClientProxy::RegisterObj(hEnv);


	if(!LoadMainXAR())
	{
		MessageBoxW(NULL,L"Load XARʧ��!",L"����",MB_OK);
		return 1;
	}



	MSG msg;

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UninitXLUE();


	return (int) msg.wParam;
}
