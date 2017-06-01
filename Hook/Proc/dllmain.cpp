// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"

//****************���dll�ļ��д�ŵ���ʵ�ʶ�ָ��������Ϣ����Ӧ����******************************//

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
	HWND hWnd = GetMainWindow(); // ��ȡ��ע��Ĵ���  
	TCHAR windowText[1024];
	::GetWindowText(hWnd, windowText, 1024);// ��ȡ��ע��Ĵ��ڵı���  
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(hWnd, windowText, L"ע��ɹ�", MB_OK);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(hWnd, windowText, L"ж��ע��", MB_OK);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

