// InputForm.cpp : 实现文件
//

#include "stdafx.h"
#include "mcf6.h"
#include "InputForm.h"
#include "afxdialogex.h"
#include "mcf6Dlg.h"


// InputForm 对话框

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


// InputForm 消息处理程序

BOOL InputForm::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_nameOfU.SetWindowTextW(CString("username"));
	m_nameOfP.SetWindowTextW(CString("password"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void InputForm::OnOK()
{
	//获取调用的窗口
	Cmcf6Dlg* parent = (Cmcf6Dlg*)this->GetParent();

	//传递user
	parent->m_nameOfUser = "";
	CString tmp;
	this->m_nameOfU.GetWindowTextW(tmp);
	for (unsigned i = 0; i < tmp.GetLength(); ++i)
		parent->m_nameOfUser += tmp[i];

	//传递password
	parent->m_nameOfPass = "";
	this->m_nameOfP.GetWindowTextW(tmp);
	for (unsigned i = 0; i < tmp.GetLength(); ++i)
		parent->m_nameOfPass += tmp[i];

	CDialogEx::OnOK();
}
