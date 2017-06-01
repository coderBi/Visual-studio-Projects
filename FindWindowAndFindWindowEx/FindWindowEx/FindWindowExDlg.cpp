
// FindWindowExDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FindWindowEx.h"
#include "FindWindowExDlg.h"
#include "afxdialogex.h"
#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFindWindowExDlg �Ի���



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


// CFindWindowExDlg ��Ϣ�������

BOOL CFindWindowExDlg::OnInitDialog()
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

void CFindWindowExDlg::OnPaint()
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
HCURSOR CFindWindowExDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

std::list<HWND> hwnds;  //��������ƥ�䵽�����еķ��������Ĵ��� ���
std::list<HHOOK> hooks;  //�����������еĹ��Ӿ��


//��ȡָ����������еĴ���  �ҵ��˷���true û�ҵ�����false
bool CFindWindowExDlg::GetAllMatchWindows(LPCWSTR lpClassName, LPCWSTR lpWindowTitle){
	//ʹ��windows sdk�ĺ���  ��һ�������Ǹ����ڵľ���������null��ʾ����������Ϊ������  �ڶ���������ָ������һ���Ӵ��ڵľ�����濪ʼѰ��,���Ϊnull��ʾ��Ѱ�����е��ַ�����
	HWND hwnd = ::FindWindowEx(nullptr, nullptr, lpClassName, lpWindowTitle);
	if (!hwnd){
		::MessageBox(nullptr, L"û���ҵ��κε�ƥ�������Ĵ���", L"����", MB_OK);
		return false;
	}
	do{
		//���ƥ��Ĵ��ھ����list���汣��
		hwnds.push_back(hwnd);
		//����Ѱ����һ������
		hwnd = ::FindWindowEx(nullptr, hwnd, lpClassName, lpWindowTitle);
	} while (!hwnd);
	return true;
}

void CFindWindowExDlg::OnBnClickedButton1()
{
	//�����һ���Ѿ�������ע��  ��һ�ξ��Ƚ�����һ�εĽ��
	

}


void CFindWindowExDlg::OnBnClickedButton2()
{
	// ������д��ڵ�ע��
	for each (HHOOK var in hooks)
	{
		if (!::UnhookWindowsHookEx(var)){
			::MessageBox(nullptr, L"���ڴ���û�ܳɹ�ж�ع���,���������г���ָ�����", L"����", MB_OK);
			return;
		}
	}
}


void CFindWindowExDlg::OnBnClickedButton3()
{
	// �����еĴ��ڷ���WM_NULL��Ϣ
	for each (HWND var in hwnds)
	{
		::PostMessage(var, WM_NULL, 0L, 0L);
	}
}


void CFindWindowExDlg::OnBnClickedButton4()
{
	// �رն����
}
