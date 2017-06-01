
// FindWindowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FindWindow.h"
#include "FindWindowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFindWindowDlg 对话框



CFindWindowDlg::CFindWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFindWindowDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFindWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFindWindowDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFindWindowDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFindWindowDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFindWindowDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CFindWindowDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CFindWindowDlg 消息处理程序

BOOL CFindWindowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFindWindowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFindWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HWND hToFindWindow = nullptr; //获取到的要hook的窗口句柄
HMODULE  hModule = nullptr;  //加载的dll资源句柄

void CFindWindowDlg::OnBnClickedButton1()
{
	//实际测试发现 我自己创建的toFind是对话框程序 里面的button点击事件并没有给父窗口发送wm_command消息。
	//通过查询相关的资料发现是如果控件类或者是其父类进行了事件的处理，那么就不会在去给父窗口发送wm_command消息进行处理了。
	//hToFindWindow = ::FindWindow(nullptr, L"toFind");

	//获取特定标题的窗口  这里使用window sdk的函数  而不是 CWND的
	hToFindWindow = ::FindWindow(L"notepad", nullptr);
	if (!hToFindWindow){
		::MessageBox(m_hWnd, L"没有找到任何要注入的窗口", L"message", MB_OK);
		return;
	}
	//获取到窗口对应的线程id GetWindowThreadProcessId函数返回的是线程id  第二个参数用于接受返回的是线程所在进程id  如果是单线程的进程那么线程id与进程id一致
	DWORD threadId = GetWindowThreadProcessId(hToFindWindow, nullptr);
	//加载dll
	hModule = ::LoadLibrary(L"DLL.dll");
	if (!hModule){
		::MessageBox(m_hWnd, L"DLL.dll文件没找到", L"message", MB_OK);
		return;
	}
	//查找Inject函数
	typedef bool(*Inject)(DWORD);
	Inject inject = (Inject)GetProcAddress(hModule, "Inject");
	if (!inject){
		::MessageBox(m_hWnd, L"dll中没有找到Inject方法", L"message", MB_OK);
		//释放dll
		FreeLibrary(hModule);
		return;
	}
	//执行inject函数  将ToFind窗口对应的线程id作为参数传入
	if (!inject(threadId)){
		::MessageBox(m_hWnd, L"注入到指定的窗口失败", L"message", MB_OK);
	}
}

/********** 释放钩子  解除注入 *********/
void CFindWindowDlg::OnBnClickedButton2()
{
	if (!hToFindWindow){
		::MessageBox(m_hWnd, L"当前没有注入任何窗口", L"message", MB_OK);
		return;
	}
	//要注入的窗口对应的线程id
	DWORD threadId = GetWindowThreadProcessId(hToFindWindow, nullptr);

	//查找ReleaseHook函数
	typedef bool(*ReleaseHook)();
	ReleaseHook releaseHook = (ReleaseHook)GetProcAddress(hModule, "ReleaseHook");
	if (!releaseHook){
		::MessageBox(m_hWnd, L"dll中没有找到ReleaseHook方法", L"message", MB_OK);
		//释放dll
		FreeLibrary(hModule);
		return;
	}
	//执行releaseHook函数
	if (!releaseHook()){
		::MessageBox(m_hWnd, L"解除钩子失败失败", L"message", MB_OK);
	}
	else{
		::MessageBox(m_hWnd, L"已成功解除注入", L"message", MB_OK);
	}
	//释放dll
	FreeLibrary(hModule);
}


void CFindWindowDlg::OnBnClickedButton3()
{
	if (!hToFindWindow){
		::MessageBox(m_hWnd, L"还没有进行注入或者是注入失败，所以不能给指定窗口发送消息", L"message", MB_OK);
		return;
	}
	//使用windows sdk  像获取到的ToFind窗口发一个消息
	::PostMessage(hToFindWindow, WM_NULL, 0, 0);
	::PostMessage(hToFindWindow, WM_COMMAND, 0, 0);
}


void CFindWindowDlg::OnBnClickedButton4()
{
	// 给捕获到的窗口发送一个wm_close消息
	::PostMessage(hToFindWindow, WM_CLOSE, 0, 0);
}
