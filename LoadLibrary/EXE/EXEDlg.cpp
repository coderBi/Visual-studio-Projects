
// EXEDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EXE.h"
#include "EXEDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//ִ��ע��
void loadDLL(){
	OutputDebugString(L"loadDLL...........in\n");
	HMODULE hm = ::LoadLibrary(L"D:/lib/DLL.dll");
	if (!hm){
		MessageBox(nullptr, _T("load dll fail"), L"message", MB_OK);
		return;
	}
	//��ȡ����ĺ���
	typedef void(*Pfunc1)();
	Pfunc1 f1 = (Pfunc1)GetProcAddress(hm,"func1");
	if (!f1){
		MessageBox(nullptr, _T("the function named 'func1' not found in dll"), L"message", MB_OK);
		return;
	}
	f1();
	OutputDebugString(L"loadDLL...........out\n");
	//�ͷ�dll
	FreeLibrary(hm);
}


// CEXEDlg �Ի���



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


// CEXEDlg ��Ϣ�������

BOOL CEXEDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CEXEDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CEXEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEXEDlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	loadDLL();
}
