#pragma once
#include "afxwin.h"


// InputForm �Ի���

class InputForm : public CDialogEx
{
	DECLARE_DYNAMIC(InputForm)

public:
	InputForm(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~InputForm();

// �Ի�������
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// �û����ֶ�����
	CEdit m_nameOfU;
	// �����ֶ���
	CEdit m_nameOfP;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
