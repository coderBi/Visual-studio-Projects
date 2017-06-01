
// EXEDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EXE.h"
#include "EXEDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//执行注入
void loadDLL(){
	OutputDebugString(L"loadDLL...........in\n");
	HMODULE hm = ::LoadLibrary(L"D:/lib/DLL.dll");
	if (!hm){
		MessageBox(nullptr, _T("load dll fail"), L"message", MB_OK);
		return;
	}
	//获取里面的函数
	typedef void(*Pfunc1)();
	Pfunc1 f1 = (Pfunc1)GetProcAddress(hm,"func1");
	if (!f1){
		MessageBox(nullptr, _T("the function named 'func1' not found in dll"), L"message", MB_OK);
		return;
	}
	f1();
	OutputDebugString(L"loadDLL...........out\n");
	//释放dll
	FreeLibrary(hm);
}


// CEXEDlg 对话框



CEXEDlg::CEXEDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEXEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEXEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEXEDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CEXEDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CEXEDlg 消息处理程序

BOOL CEXEDlg::OnInitDialog()
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

void CEXEDlg::OnPaint()
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
HCURSOR CEXEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEXEDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	loadDLL();
}
