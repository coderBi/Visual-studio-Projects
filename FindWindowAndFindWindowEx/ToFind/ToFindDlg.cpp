
// ToFindDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ToFind.h"
#include "ToFindDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToFindDlg �Ի���



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


// CToFindDlg ��Ϣ�������

BOOL CToFindDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	//�ڽ��������һ���ֶ������һ����ť�����﷢�����������ȥ�İ�ť������ť�౾���Ѿ������˵���¼������оͲ����򸸴��ڷ���
	// WM_COMMAND��Ϣ�������ֶ���һ����ť
	::CreateWindow(L"Button",L"�ֶ���ӵ�һ����ť������wm_command��Ϣ�Ĳ���",WS_VISIBLE	| WS_CHILD | BS_PUSHBUTTON,10,100,500,50,this->m_hWnd,(HMENU)999,nullptr,nullptr);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CToFindDlg::OnPaint()
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
HCURSOR CToFindDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CToFindDlg::OnBnClickedButton1()
//{
//	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//}





//void CToFindDlg::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
//
//	::MessageBox(m_hWnd, L"wm_lbuttondown", L"self", MB_OK);
//
//	CDialogEx::OnLButtonDown(nFlags, point);
//}


void CToFindDlg::OnIddTofindDialog()
{
	// TODO:  �ڴ���������������
	::MessageBox(m_hWnd, L"cmmd", L"cmmd", MB_OK);
}
