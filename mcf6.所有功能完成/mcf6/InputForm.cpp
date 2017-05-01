// InputForm.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mcf6.h"
#include "InputForm.h"
#include "afxdialogex.h"
#include "mcf6Dlg.h"


// InputForm �Ի���

IMPLEMENT_DYNAMIC(InputForm, CDialogEx)

InputForm::InputForm(CWnd* pParent /*=NULL*/)
	: CDialogEx(InputForm::IDD, pParent)
{

}

InputForm::~InputForm()
{
}

void InputForm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_nameOfU);
	DDX_Control(pDX, IDC_EDIT2, m_nameOfP);
}


BEGIN_MESSAGE_MAP(InputForm, CDialogEx)
END_MESSAGE_MAP()


// InputForm ��Ϣ�������

BOOL InputForm::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_nameOfU.SetWindowTextW(CString("username"));
	m_nameOfP.SetWindowTextW(CString("password"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void InputForm::OnOK()
{
	//��ȡ���õĴ���
	Cmcf6Dlg* parent = (Cmcf6Dlg*)this->GetParent();

	//����user
	parent->m_nameOfUser = "";
	CString tmp;
	this->m_nameOfU.GetWindowTextW(tmp);
	for (unsigned i = 0; i < tmp.GetLength(); ++i)
		parent->m_nameOfUser += tmp[i];

	//����password
	parent->m_nameOfPass = "";
	this->m_nameOfP.GetWindowTextW(tmp);
	for (unsigned i = 0; i < tmp.GetLength(); ++i)
		parent->m_nameOfPass += tmp[i];

	CDialogEx::OnOK();
}
