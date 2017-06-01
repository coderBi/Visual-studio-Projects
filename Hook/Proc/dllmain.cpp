// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

//****************这个dll文件中存放的是实际对指定窗口消息的响应过程******************************//

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwCurProcessId = *((DWORD*)lParam);
	DWORD dwProcessId = 0;

	GetWindowThreadProcessId(hwnd, &dwProcessId);
	if (dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
	{
		*((HWND *)lParam) = hwnd;
		return FALSE;
	}
	return TRUE;
}


HWND GetMainWindow()
{
	DWORD dwCurrentProcessId = GetCurrentProcessId();
	if (!EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId))
	{
		return (HWND)dwCurrentProcessId;
	}
	return NULL;
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	HWND hWnd = GetMainWindow(); // 获取被注入的窗口  
	TCHAR windowText[1024];
	::GetWindowText(hWnd, windowText, 1024);// 获取被注入的窗口的标题  
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(hWnd, windowText, L"注入成功", MB_OK);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(hWnd, windowText, L"卸载注入", MB_OK);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

