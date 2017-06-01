
// FindWindowExDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FindWindowEx.h"
#include "FindWindowExDlg.h"
#include "afxdialogex.h"
#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFindWindowExDlg 对话框



CFindWindowExDlg::CFindWindowExDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFindWindowExDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFindWindowExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFindWindowExDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFindWindowExDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFindWindowExDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFindWindowExDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CFindWindowExDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CFindWindowExDlg 消息处理程序

BOOL CFindWindowExDlg::OnInitDialog()
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

void CFindWindowExDlg::OnPaint()
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
HCURSOR CFindWindowExDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

std::list<HWND> hwnds;  //用来保存匹配到的所有的符合条件的窗口 句柄
std::list<HHOOK> hooks;  //用来保存所有的钩子句柄


//获取指定标题的所有的窗口  找到了返回true 没找到返回false
bool CFindWindowExDlg::GetAllMatchWindows(LPCWSTR lpClassName, LPCWSTR lpWindowTitle){
	//使用windows sdk的函数  第一个参数是父窗口的句柄，如果是null表示从整个桌面为父窗口  第二个参数是指定从哪一个子窗口的句柄后面开始寻找,如果为null表示会寻找所有的字符串口
	HWND hwnd = ::FindWindowEx(nullptr, nullptr, lpClassName, lpWindowTitle);
	if (!hwnd){
		::MessageBox(nullptr, L"没有找到任何的匹配条件的窗口", L"警告", MB_OK);
		return false;
	}
	do{
		//添加匹配的窗口句柄到list里面保存
		hwnds.push_back(hwnd);
		//接着寻找下一个窗口
		hwnd = ::FindWindowEx(nullptr, hwnd, lpClassName, lpWindowTitle);
	} while (!hwnd);
	return true;
}

void CFindWindowExDlg::OnBnClickedButton1()
{
	//如果上一次已经进行了注入  这一次就先进行上一次的解除
	

}


void CFindWindowExDlg::OnBnClickedButton2()
{
	// 解除所有窗口的注入
	for each (HHOOK var in hooks)
	{
		if (!::UnhookWindowsHookEx(var)){
			::MessageBox(nullptr, L"存在窗口没能成功卸载钩子,请重新运行程序恢复正常", L"警告", MB_OK);
			return;
		}
	}
}


void CFindWindowExDlg::OnBnClickedButton3()
{
	// 向所有的窗口发送WM_NULL消息
	for each (HWND var in hwnds)
	{
		::PostMessage(var, WM_NULL, 0L, 0L);
	}
}


void CFindWindowExDlg::OnBnClickedButton4()
{
	// 关闭顶层的
}
