
// ToFindDlg.h : ͷ�ļ�
//

#pragma once


// CToFindDlg �Ի���
class CToFindDlg : public CDialogEx
{
// ����
public:
	CToFindDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TOFIND_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedButton1();
//	afx_msg void OnIddTofindDialog();
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnIddTofindDialog();
};
