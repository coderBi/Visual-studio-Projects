// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <string>
#include <stdlib.h>

/**********************可以认为  全局的钩子  必须要写到dll中 *************************/

//搞一个共享数据段  放一些多次加载dll的共享数据  数据段的名字叫做shared
#pragma data_seg("shared")
//共享数据段中放置计数器 表示已经注入的次数
int count = 0;

//设置shared数据段的访问权限
#pragma comment(linker,"/section:shared,RWS"

HHOOK hGetMessageHook = nullptr, hCallWndProcHook = nullptr;  //用在保存钩子句柄 
HMODULE hinstance = nullptr; //当前模块的实例句柄

//申明两个函数为可导出的
extern "C" _declspec(dllexport) bool Inject(DWORD threadId);
extern "C" _declspec(dllexport) bool ReleaseHook();


//勾到WH_GETMESSAGE消息之后进行自己想要的消息处理过程
LRESULT CALLBACK  GetMessageHookProc(int code, WPARAM wParam, LPARAM lParam){
	//lParam传递的是发给捕获到的窗口的消息的指针
	PMSG pMsg = (PMSG)(lParam);
	if (!pMsg)
		return -1;
	//选择需要处理掉的消息进行相应的个性化处理
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
						//wparam高位字节表示事件号  低字节表示子空间id（包括菜单和页面按钮等）
						wsprintf(toPrint, L"%d...%d", LOWORD(pMsg->wParam), HIWORD(pMsg->wParam));
						MessageBox(pMsg->hwnd, toPrint, L"WM_COMMAND", MB_OK);
						break;
	}
	default:
		break;
	}
}


//注入指定的线程
bool Inject(DWORD threadId){
	
	//设置一个获取消息队列的钩子 SetWindowsHookEx 失败返回null 可以用GetLastError获取失败消息
	//WH_CALLWNDPROC 进入窗口处理过程的时候进行监听  WH_GETMESSAGE调用GetMessage函数获取消息之前监听
	hGetMessageHook = SetWindowsHookEx(WH_GETMESSAGE, GetMessageHookProc, hinstance, threadId);
	if (!hGetMessageHook){
		return false;
	}
	
	return true;
}

//解除钩子
bool ReleaseHook(){
	if (hGetMessageHook){
		//UnhookWindowsHookEx 执行成功返回非0  执行失败返回0
		if (!UnhookWindowsHookEx(hGetMessageHook))
			return false;
	}
	return true;
}

/**dllmain参数：
	第一个参数是dll资源句柄 
	第二个参数是dll被调用的原因 可以有四个值，DLL_PROCESS_ATTACH在进行第一次加载dll的时候执行，以后进行中再次加载只会添加计数。
	DLL_THREAD_ATTACH 在一个进程中创建一个线程，首先会扫描这个进程的所有dll资源，然后以DLL_THREAD_ATTACH为第二个参数进行dllmain的调用
	DLL_THREAD_DETACH 进程中一个线程（正常）终止的时候以DLL_THREAD_DETACH作为第二个参数进行dllmain的调用
	DLL_PROCESS_DETACH 进行（正常）结束的时候，进行dll的资源的删除。
	----------note: 无论是线程还是进程的不正常关闭，例如 TerminateThread TerminateProcess 都会导致相应的dll资源得不到有效释放
	--------note: 进程是内存管理的单位  线程是cpu调度的单位
	第三个参数： 保留，目前没有用
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

