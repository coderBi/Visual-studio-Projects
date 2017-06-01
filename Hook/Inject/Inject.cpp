// Inject.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Inject.h"

////********这个文件存放的是寻找匹配的窗口  并且进行WH_GETMESSAGE钩子的注入与解除****************///

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//申明可导出
extern "C" _declspec(dllexport) void SetHook(DWORD ThreadId);
extern "C" _declspec(dllexport) void UnHook();

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CInjectApp

BEGIN_MESSAGE_MAP(CInjectApp, CWinApp)
END_MESSAGE_MAP()


// CInjectApp 构造

CInjectApp::CInjectApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CInjectApp 对象

CInjectApp theApp;

//不同Instance共享的该变量hinst    
#pragma data_seg("SHARED")    
static HINSTANCE hinst = NULL; //本dll的实例句柄 (injectDll.dll)    
#pragma data_seg()    
#pragma comment(linker, "/section:SHARED,RWS")    

HHOOK hook = NULL;          //是否对目标进行了Hook    
void SetHook(DWORD ThreadId);       //开启钩子    
void UnHook();      //关闭钩子    
LRESULT CALLBACK  GetMessageHookProc(int code, WPARAM wParam, LPARAM lParam);

// CinjectDllApp 初始化  

BOOL CInjectApp::InitInstance()
{
	hinst = AfxGetInstanceHandle(); //本dll句柄  
	return CWinApp::InitInstance();
}

void SetHook(DWORD ThreadId)
{
	hook = ::SetWindowsHookEx(WH_GETMESSAGE, GetMessageHookProc, hinst, ThreadId);
	if (hook != NULL) {
		AfxMessageBox(L"hook成功");
	}
}

void UnHook()
{
	if (hook != NULL) {
		::UnhookWindowsHookEx(hook);
		AfxMessageBox(L"hook卸载成功");
	}
}

LRESULT CALLBACK  GetMessageHookProc(int code, WPARAM wParam, LPARAM lParam) {
	PMSG pMsg = (PMSG)lParam;
	if (pMsg->message == WM_NULL) {
		AfxMessageBox(L"激活消息到来");
		::LoadLibraryA("Proc.dll");
	}
	return CallNextHookEx(0, code, wParam, lParam);
}

