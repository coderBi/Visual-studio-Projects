// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <string>
#include <stdlib.h>

/**********************������Ϊ  ȫ�ֵĹ���  ����Ҫд��dll�� *************************/

//��һ���������ݶ�  ��һЩ��μ���dll�Ĺ�������  ���ݶε����ֽ���shared
#pragma data_seg("shared")
//�������ݶ��з��ü����� ��ʾ�Ѿ�ע��Ĵ���
int count = 0;

//����shared���ݶεķ���Ȩ��
#pragma comment(linker,"/section:shared,RWS"

HHOOK hGetMessageHook = nullptr, hCallWndProcHook = nullptr;  //���ڱ��湳�Ӿ�� 
HMODULE hinstance = nullptr; //��ǰģ���ʵ�����

//������������Ϊ�ɵ�����
extern "C" _declspec(dllexport) bool Inject(DWORD threadId);
extern "C" _declspec(dllexport) bool ReleaseHook();


//����WH_GETMESSAGE��Ϣ֮������Լ���Ҫ����Ϣ�������
LRESULT CALLBACK  GetMessageHookProc(int code, WPARAM wParam, LPARAM lParam){
	//lParam���ݵ��Ƿ������񵽵Ĵ��ڵ���Ϣ��ָ��
	PMSG pMsg = (PMSG)(lParam);
	if (!pMsg)
		return -1;
	//ѡ����Ҫ���������Ϣ������Ӧ�ĸ��Ի�����
	switch (pMsg->message)
	{
	case WM_NULL:{
					 wchar_t windowText[1024] = { 0 };
					 GetWindowText(pMsg->hwnd, windowText, 1024);
					 MessageBox(pMsg->hwnd, L"WM_NULL message got", windowText, MB_OK);
					 break;
	}
	case WM_COMMAND:{
						std::string str = "event:";
						TCHAR  toPrint[1024] = { 0 };
						//wparam��λ�ֽڱ�ʾ�¼���  ���ֽڱ�ʾ�ӿռ�id�������˵���ҳ�水ť�ȣ�
						wsprintf(toPrint, L"%d...%d", LOWORD(pMsg->wParam), HIWORD(pMsg->wParam));
						MessageBox(pMsg->hwnd, toPrint, L"WM_COMMAND", MB_OK);
						break;
	}
	default:
		break;
	}
}


//ע��ָ�����߳�
bool Inject(DWORD threadId){
	
	//����һ����ȡ��Ϣ���еĹ��� SetWindowsHookEx ʧ�ܷ���null ������GetLastError��ȡʧ����Ϣ
	//WH_CALLWNDPROC ���봰�ڴ�����̵�ʱ����м���  WH_GETMESSAGE����GetMessage������ȡ��Ϣ֮ǰ����
	hGetMessageHook = SetWindowsHookEx(WH_GETMESSAGE, GetMessageHookProc, hinstance, threadId);
	if (!hGetMessageHook){
		return false;
	}
	
	return true;
}

//�������
bool ReleaseHook(){
	if (hGetMessageHook){
		//UnhookWindowsHookEx ִ�гɹ����ط�0  ִ��ʧ�ܷ���0
		if (!UnhookWindowsHookEx(hGetMessageHook))
			return false;
	}
	return true;
}

/**dllmain������
	��һ��������dll��Դ��� 
	�ڶ���������dll�����õ�ԭ�� �������ĸ�ֵ��DLL_PROCESS_ATTACH�ڽ��е�һ�μ���dll��ʱ��ִ�У��Ժ�������ٴμ���ֻ����Ӽ�����
	DLL_THREAD_ATTACH ��һ�������д���һ���̣߳����Ȼ�ɨ��������̵�����dll��Դ��Ȼ����DLL_THREAD_ATTACHΪ�ڶ�����������dllmain�ĵ���
	DLL_THREAD_DETACH ������һ���̣߳���������ֹ��ʱ����DLL_THREAD_DETACH��Ϊ�ڶ�����������dllmain�ĵ���
	DLL_PROCESS_DETACH ���У�������������ʱ�򣬽���dll����Դ��ɾ����
	----------note: �������̻߳��ǽ��̵Ĳ������رգ����� TerminateThread TerminateProcess ���ᵼ����Ӧ��dll��Դ�ò�����Ч�ͷ�
	--------note: �������ڴ����ĵ�λ  �߳���cpu���ȵĵ�λ
	������������ ������Ŀǰû����
*/
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	hinstance = hModule;
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

