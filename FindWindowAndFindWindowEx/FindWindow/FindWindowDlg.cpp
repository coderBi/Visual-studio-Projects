
// FindWindowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FindWindow.h"
#include "FindWindowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFindWindowDlg �Ի���



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


// CFindWindowDlg ��Ϣ�������

BOOL CFindWindowDlg::OnInitDialog()
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

void CFindWindowDlg::OnPaint()
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
HCURSOR CFindWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HWND hToFindWindow = nullptr; //��ȡ����Ҫhook�Ĵ��ھ��
HMODULE  hModule = nullptr;  //���ص�dll��Դ���

void CFindWindowDlg::OnBnClickedButton1()
{
	//ʵ�ʲ��Է��� ���Լ�������toFind�ǶԻ������ �����button����¼���û�и������ڷ���wm_command��Ϣ��
	//ͨ����ѯ��ص����Ϸ���������ؼ���������丸��������¼��Ĵ�����ô�Ͳ�����ȥ�������ڷ���wm_command��Ϣ���д����ˡ�
	//hToFindWindow = ::FindWindow(nullptr, L"toFind");

	//��ȡ�ض�����Ĵ���  ����ʹ��window sdk�ĺ���  ������ CWND��
	hToFindWindow = ::FindWindow(L"notepad", nullptr);
	if (!hToFindWindow){
		::MessageBox(m_hWnd, L"û���ҵ��κ�Ҫע��Ĵ���", L"message", MB_OK);
		return;
	}
	//��ȡ�����ڶ�Ӧ���߳�id GetWindowThreadProcessId�������ص����߳�id  �ڶ����������ڽ��ܷ��ص����߳����ڽ���id  ����ǵ��̵߳Ľ�����ô�߳�id�����idһ��
	DWORD threadId = GetWindowThreadProcessId(hToFindWindow, nullptr);
	//����dll
	hModule = ::LoadLibrary(L"DLL.dll");
	if (!hModule){
		::MessageBox(m_hWnd, L"DLL.dll�ļ�û�ҵ�", L"message", MB_OK);
		return;
	}
	//����Inject����
	typedef bool(*Inject)(DWORD);
	Inject inject = (Inject)GetProcAddress(hModule, "Inject");
	if (!inject){
		::MessageBox(m_hWnd, L"dll��û���ҵ�Inject����", L"message", MB_OK);
		//�ͷ�dll
		FreeLibrary(hModule);
		return;
	}
	//ִ��inject����  ��ToFind���ڶ�Ӧ���߳�id��Ϊ��������
	if (!inject(threadId)){
		::MessageBox(m_hWnd, L"ע�뵽ָ���Ĵ���ʧ��", L"message", MB_OK);
	}
}

/********** �ͷŹ���  ���ע�� *********/
void CFindWindowDlg::OnBnClickedButton2()
{
	if (!hToFindWindow){
		::MessageBox(m_hWnd, L"��ǰû��ע���κδ���", L"message", MB_OK);
		return;
	}
	//Ҫע��Ĵ��ڶ�Ӧ���߳�id
	DWORD threadId = GetWindowThreadProcessId(hToFindWindow, nullptr);

	//����ReleaseHook����
	typedef bool(*ReleaseHook)();
	ReleaseHook releaseHook = (ReleaseHook)GetProcAddress(hModule, "ReleaseHook");
	if (!releaseHook){
		::MessageBox(m_hWnd, L"dll��û���ҵ�ReleaseHook����", L"message", MB_OK);
		//�ͷ�dll
		FreeLibrary(hModule);
		return;
	}
	//ִ��releaseHook����
	if (!releaseHook()){
		::MessageBox(m_hWnd, L"�������ʧ��ʧ��", L"message", MB_OK);
	}
	else{
		::MessageBox(m_hWnd, L"�ѳɹ����ע��", L"message", MB_OK);
	}
	//�ͷ�dll
	FreeLibrary(hModule);
}


void CFindWindowDlg::OnBnClickedButton3()
{
	if (!hToFindWindow){
		::MessageBox(m_hWnd, L"��û�н���ע�������ע��ʧ�ܣ����Բ��ܸ�ָ�����ڷ�����Ϣ", L"message", MB_OK);
		return;
	}
	//ʹ��windows sdk  ���ȡ����ToFind���ڷ�һ����Ϣ
	::PostMessage(hToFindWindow, WM_NULL, 0, 0);
	::PostMessage(hToFindWindow, WM_COMMAND, 0, 0);
}


void CFindWindowDlg::OnBnClickedButton4()
{
	// �����񵽵Ĵ��ڷ���һ��wm_close��Ϣ
	::PostMessage(hToFindWindow, WM_CLOSE, 0, 0);
}
