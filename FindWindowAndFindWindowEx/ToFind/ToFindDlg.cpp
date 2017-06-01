
// ToFindDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ToFind.h"
#include "ToFindDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToFindDlg 对话框



CToFindDlg::CToFindDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CToFindDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CToFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CToFindDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_BUTTON1, &CToFindDlg::OnBnClickedButton1)
//	ON_COMMAND(IDD_TOFIND_DIALOG, &CToFindDlg::OnIddTofindDialog)
//ON_WM_LBUTTONDOWN()
ON_COMMAND(IDD_TOFIND_DIALOG, &CToFindDlg::OnIddTofindDialog)
END_MESSAGE_MAP()


// CToFindDlg 消息处理程序

BOOL CToFindDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	//在界面上添加一个手动的添加一个按钮，这里发现如果是拖上去的按钮，好像按钮类本身已经处理了点击事件，所有就不会向父窗口发送
	// WM_COMMAND消息，这里手动给一个按钮
	::CreateWindow(L"Button",L"手动添加的一个按钮，进行wm_command消息的测试",WS_VISIBLE	| WS_CHILD | BS_PUSHBUTTON,10,100,500,50,this->m_hWnd,(HMENU)999,nullptr,nullptr);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CToFindDlg::OnPaint()
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
HCURSOR CToFindDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CToFindDlg::OnBnClickedButton1()
//{
//	// TODO:  在此添加控件通知处理程序代码
//}





//void CToFindDlg::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO:  在此添加消息处理程序代码和/或调用默认值
//
//	::MessageBox(m_hWnd, L"wm_lbuttondown", L"self", MB_OK);
//
//	CDialogEx::OnLButtonDown(nFlags, point);
//}


void CToFindDlg::OnIddTofindDialog()
{
	// TODO:  在此添加命令处理程序代码
	::MessageBox(m_hWnd, L"cmmd", L"cmmd", MB_OK);
}
