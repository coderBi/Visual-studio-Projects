
// FindWindowExDlg.h : 头文件
//

#pragma once


// CFindWindowExDlg 对话框
class CFindWindowExDlg : public CDialogEx
{
// 构造
public:
	CFindWindowExDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FINDWINDOWEX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	//获取指定标题的所有的窗口
	bool GetAllMatchWindows(LPCWSTR lpClassName, LPCWSTR lpWindowTitle);
};
