//#include "client.h"
//#include "verification.h"
//#include <iostream>
//#include <Poco/Exception.h>
//
//using std::cout;
//using std::cerr;
//using std::endl;
//using Poco::Exception;
//using CoolDown::Client::Verification;
//using CoolDown::Client::CoolClient;
//
//int main(int argc, char* argv[]){
//    try{
//        CoolClient c;
//        return c.run(argc, argv);
//    }catch(Exception& e){
//        cerr << e.message() << endl;
//    }
//}
#include "coolclient_luawrapper.h"
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <XLUE.h>
#include <XLGraphic.h>
#include <XLLuaRuntime.h>
#include <string>
#include <Shlwapi.h>


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

			MessageBoxW(0,str.c_str(),L"为了帮助我们改进质量,请反馈此脚本错误",MB_ICONERROR | MB_OK);

		}
		else
		{
			MessageBoxW(0,wcszLuaErrorString ? wcszLuaErrorString : L"" ,L"为了帮助我们改进质量,请反馈此脚本错误",MB_ICONERROR | MB_OK);
		}
		s_bEnter = false;
	}
	return 0;
}



bool InitXLUE()
{
	//初始化图形库
	XLGraphicParam param;
	XL_PrepareGraphicParam(&param);
	param.textType = XLTEXT_TYPE_FREETYPE;
	long result = XL_InitGraphicLib(&param);
	result = XL_SetFreeTypeEnabled(TRUE);
	//初始化XLUE,这函数是一个复合初始化函数
	//完成了初始化Lua环境,标准对象,XLUELoader的工作
	result = XLUE_InitLoader(NULL);

	//设置一个简单的脚本出错提示
	XLLRT_ErrorHandle(LuaErrorHandle);

	return true; 
}

void UninitXLUE()
{
	//退出流程
	XLUE_Uninit(NULL);
	XLUE_UninitLuaHost(NULL);
	XL_UnInitGraphicLib();
	XLUE_UninitHandleMap(NULL);
}

bool LoadMainXAR()
{
	long result = 0;
	//设置XAR的搜索路径
	result = XLUE_AddXARSearchPath(GetResDir());
	//加载主XAR,此时会执行该XAR的启动脚本onload.lua
	result = XLUE_LoadXAR("HelloBolt");
	if(result != 0)
	{
		return false;
	}
	return true;
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	// TODO: 在此放置代码。

	if(!InitXLUE())
	{
		MessageBoxW(NULL,L"初始化XLUE 失败!",L"错误",MB_OK);
		return 1;
	}

	XL_LRT_ENV_HANDLE hEnv = XLLRT_GetEnv(NULL);
	CoolClientLuaWrapper::RegisterClass(hEnv);
	CoolClientLuaWrapperFactory::RegisterObj(hEnv);


	if(!LoadMainXAR())
	{
		MessageBoxW(NULL,L"Load XAR失败!",L"错误",MB_OK);
		return 1;
	}



	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UninitXLUE();

	return (int) msg.wParam;
}
