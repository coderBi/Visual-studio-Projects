// mcf6Dlg.h : ͷ�ļ�
//
#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <fstream>
#include <string>
#include <string.h>
#include"pcap.h"
#include "Protocol.h"
#include "utilities.h"

//�û���������ṹ
typedef struct _UPDATA{
	std::string user;
	std::string password;
} UPDATA,*PUPDATA;

// Cmcf6Dlg �Ի���
class Cmcf6Dlg : public CDialog
{
// ����
public:
	Cmcf6Dlg(CWnd* pParent = NULL);	// ��׼���캯��

	/////////////////////////////////////////////[my fuction]//////////////////////////////////////////////
	int lixsniff_initCap();
	int lixsniff_startCap();
	int lixsniff_updateTree(int index);
	int lixsniff_updateEdit(int index);
	int lixsniff_updateNPacket();
	int lixsniff_saveFile();
	int lixsniff_readFile(CString path);
	
	//////////////////////////////////////////////��my data��/////////////////////////////////////////////
	int devCount;
	struct pktcount npacket;				//�������ݰ�����
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *alldev;
	pcap_if_t *dev;
	pcap_t *adhandle;
	pcap_dumper_t *dumpfile;
	std::ofstream *upstream;
	char filepath[512];							//	�ļ�����·��
	char filename[64];							//	�ļ�����	
	char upfilepath[512];						//�û������뱣��·��

	HANDLE m_ThreadHandle;			//�߳�

	CPtrList m_pktList;							//���������ŵ�����

// �Ի�������
	enum { IDD = IDD_MCF6_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listCtrl;
	CComboBox m_comboBox;
	CComboBox m_comboBoxRule;
	CTreeCtrl m_treeCtrl;
	CEdit m_edit;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CButton m_buttonStart;
	CButton m_buttonStop;
	CPtrList m_localDataList;				//���汻���ػ�������ݰ�
	CPtrList m_netDataList;					//�����������ֱ�ӻ�ȡ�����ݰ�
	CPtrList m_upDataList;					//�����ȡ�����û���������
	CBitmapButton m_bitButton	;		//ͼƬ��ť
	int npkt;
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton5();
	CButton m_buttonSave;
	CButton m_buttonRead;
	afx_msg void OnBnClickedButton4();
	//�û����ֶ�����
	std::string m_nameOfUser;
	//�����ֶ�����
	std::string m_nameOfPass;
	//�������û���������
	bool filterUserAndPass(PUPDATA up, const u_char * data, unsigned len);
	//���浱ǰ���˵�Э������
	CString m_protocol;
	//�����Ƿ��Ѿ�����
	bool m_dataSaved;
};
