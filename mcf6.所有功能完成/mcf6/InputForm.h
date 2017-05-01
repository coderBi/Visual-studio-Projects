#pragma once
#include "afxwin.h"


// InputForm 对话框

class InputForm : public CDialogEx
{
	DECLARE_DYNAMIC(InputForm)

public:
	InputForm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~InputForm();

// 对话框数据
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 用户名字段名称
	CEdit m_nameOfU;
	// 密码字段名
	CEdit m_nameOfP;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
