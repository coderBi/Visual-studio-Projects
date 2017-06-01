// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

//申明为可导出
extern "C"_declspec(dllexport) void func1();

void func1(){
	MessageBox(nullptr, L"this is func1", L"call func in dll", MB_OK);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

