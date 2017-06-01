// Inject.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Inject.h"

////********����ļ���ŵ���Ѱ��ƥ��Ĵ���  ���ҽ���WH_GETMESSAGE���ӵ�ע������****************///

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//�����ɵ���
extern "C" _declspec(dllexport) void SetHook(DWORD ThreadId);
extern "C" _declspec(dllexport) void UnHook();

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CInjectApp

BEGIN_MESSAGE_MAP(CInjectApp, CWinApp)
END_MESSAGE_MAP()


// CInjectApp ����

CInjectApp::CInjectApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CInjectApp ����

CInjectApp theApp;

//��ͬInstance����ĸñ���hinst    
#pragma data_seg("SHARED")    
static HINSTANCE hinst = NULL; //��dll��ʵ����� (injectDll.dll)    
#pragma data_seg()    
#pragma comment(linker, "/section:SHARED,RWS")    

HHOOK hook = NULL;          //�Ƿ��Ŀ�������Hook    
void SetHook(DWORD ThreadId);       //��������    
void UnHook();      //�رչ���    
LRESULT CALLBACK  GetMessageHookProc(int code, WPARAM wParam, LPARAM lParam);

// CinjectDllApp ��ʼ��  

BOOL CInjectApp::InitInstance()
{
	hinst = AfxGetInstanceHandle(); //��dll���  
	return CWinApp::InitInstance();
}

void SetHook(DWORD ThreadId)
{
	hook = ::SetWindowsHookEx(WH_GETMESSAGE, GetMessageHookProc, hinst, ThreadId);
	if (hook != NULL) {
		AfxMessageBox(L"hook�ɹ�");
	}
}

void UnHook()
{
	if (hook != NULL) {
		::UnhookWindowsHookEx(hook);
		AfxMessageBox(L"hookж�سɹ�");
	}
}

LRESULT CALLBACK  GetMessageHookProc(int code, WPARAM wParam, LPARAM lParam) {
	PMSG pMsg = (PMSG)lParam;
	if (pMsg->message == WM_NULL) {
		AfxMessageBox(L"������Ϣ����");
		::LoadLibraryA("Proc.dll");
	}
	return CallNextHookEx(0, code, wParam, lParam);
}

