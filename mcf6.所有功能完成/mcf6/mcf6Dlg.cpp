// mcf6Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mcf6.h"
#include "mcf6Dlg.h"
#include <regex>
#include "InputForm.h"
#include "MyBase64.h"
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
DWORD WINAPI lixsinff_CapThread(LPVOID lpParameter);


/*
*��һ���������ж�ȡһ�� ���������е��û���������
@param data PUPDATA �洢��ȡ�����û�������
@param stream ifstream ������
@return bool �ɹ�����true ʧ��false
**/
bool readUserPassword(PUPDATA data, std::ifstream & stream){
	//�����ԭ������
	data->password = data->user = "";
	if (!stream)
		return false;
	std::string line;
	const unsigned maxLen = 1024;
	char buff[maxLen] = { 0 };
	if (!stream.getline(buff, maxLen))
		return false;  //����һ��ʧ�ܣ������Ѿ�û��������
	int i = 0;
	while (i < maxLen && buff[i] != '\n' && buff[i] != ' ')
	{
		data->user += buff[i];
		++i;
	}
	while (i < maxLen && buff[i] == ' ')
	{
		++i;
	}
	while (i < maxLen && buff[i] != '\n')
	{
		data->password += buff[i];
		i++;
	}
	return true;
}

/*
*�����û�������ṹ��һ�������
@param data PUPDATA ��������û�������
@param stream ofstream �����
*@return bool ����ɹ�����true ʧ�ܷ���false
*/
bool saveUserAndPass(PUPDATA data, std::ofstream & stream){
	if (!stream)
		return false;
	stream << data->user << " " << data->password << std::endl;
	return true;
}

/*
*�ж�һ��u_char�����Ƿ�����Ԫ�ض���0
*@param u_char arr[]  ����֤������
*@param unsigned length ���鳤��
*@return bool  ȫ����0 ����true  ���򷵻�false
*/
bool u_char_all_zero(u_char arr[], unsigned length){
	for (unsigned i = 0; i < length; ++i){
		if (arr[i])
			return false;
	}
	return true;
}

/*
*�Ƚ�����u_char ����
*@param u_char arr1[]  ���Ƚϵ�����
*@param u_char arr2[]  ���Ƚϵ�����
*@param unsigned length ���鳤��
*@return int ��ȷ�0  ���򷵻�1 ���� -1
*/
int u_char_compare(u_char arr1[], u_char arr2[], unsigned length){
	for (unsigned i = 0; i < length; ++i){
		if (arr1[i] == arr2[i])
			continue;
		else if (arr1[i] > arr2[i])
			return 1;
		else
			return -1;
	}
	return 0;
}

/*
*���˳��û���������
*@param up PUPDATA �����洢���˵�������
*@param data u_char* һ����������ָ��
*@return bool ִ�гɹ�����true ʧ��false
*/
bool Cmcf6Dlg::filterUserAndPass(PUPDATA up, const u_char * data, unsigned len){
	if (!up || !data)
		return false;
	up->password = up->user = "";
	if ("ftp" == m_protocol){
		////////////note: ftp���û������������������ֱ���ģ�����һ��ֻ�ܻ�ȡ���û�����������////////////////////
		//ftpͷ֮ǰƫ�� ����֡ͷ14 +��ip����ͷ + tcpͷ20  ftp�ǻ���tcp��
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 20;
		if (len <= offset || (data[offset] != 'U' && data[offset] != 'P'))
			return false;

		unsigned i = offset;
		//�Թ�ǰ��� USER ���� PASS �ַ���
		for (; i < len && data[i] != ' '; ++i){}

		//�Թ�ǰ�ÿո�
		for (; i < len && data[i] == ' '; ++i){}

		std::string content = "";
		//��ȡ�û�����������
		for (; i < len && data[i] != '\r' && data[i] != '\n'; ++i){
			content += data[i];
		}
		if ('U' == data[offset]){
			up->user = content;
		}
		else{
			up->password = content;
		}
	}
	else if ("http" == m_protocol){
		//��ʼƫ�� ����֡14 + ipͷ
		unsigned offset = 14 + 4 * (data[14] & 0xf);

		//����GET ���� POST�ؼ���
		int i = offset;
		std::string method = "";
		const char * cData = (const char*)data;
		for (; i < len; ++i){
			if (i + 4 < len && strncmp(cData + i, "POST", strlen("POST")) == 0){
				method = "POST";
				break;
			}
			else if (i + 3 < len && strncmp(cData + i, "GET", strlen("GET")) == 0){
				method = "GET";
				break;
			}
		}
		if ("" == method)
			return false;
		//��ȡhttpͷ
		std::string str = "";
		for (; i < len && !(data[i] == '\n' && data[i - 2] == '\n'); ++i)
			str += data[i];
		str += data[i++];

		//�����post����  ���Ż�ȡ��������
		if ("POST" == method)
		for (; i < len && data[i] != '\n' && data[i] != '\r'; ++i)
			str += data[i];

		//��������
		std::string pattern = "(" + m_nameOfUser + "|" + m_nameOfPass + ")=([^\\s=&]*)";

		std::smatch sm;
		while (std::regex_search(str, sm, std::regex(pattern))){
			auto it = sm.begin();
			std::string key = *(it + 1);
			std::string value = *(it + 2);
			if (key == m_nameOfUser)
				up->user = value;
			else if (key == m_nameOfPass)
				up->password = value;
			str = sm.suffix().str();
		}
	}
	else if ("snmp" == m_protocol){
		//ͨ���������ϺͲ���  snmp�ײ��ߵ���ip/udp  udp����ͷ�̶�8�ֽ�
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 8;

		// snmp v1 v2c�ǹ��ϵĲ���ȫ��  snmp v3�����˰�ȫ����
		//�������ı�ʶ snmpΪ30
		offset++;
		//���������� ����������һ���ʾΪ1~3�ֽ�  ��snmp���ĳ���Ϊ128�ֽ����±�ʾΪ1�ֽ�
		if (data[offset] & 0x80){
			unsigned lenLen = (data[offset] & 0xf);
			offset += 1 + lenLen;
		}
		else
		{
			offset++;
		}
		//��ȡ�汾��
		//�������ͱ�ʶ��
		offset++;
		//��ȡ�汾����ռ����
		unsigned versionLen = data[offset];
		u_char version = 0;
		offset++;
		for (unsigned i = 0; i < versionLen; ++i){
			version <<= 8;
			version += data[offset + i];
		}
		//+1��ʾ���հ汾��
		version = data[offset] + 1;
		//�汾��С��3 ��ʾ v1 v2
		if (version < 3){
			offset += versionLen + 1;
			//�����볤�ȱ�ʶ
			unsigned opLen = data[offset];

			//��ȡ����� opLen�ĳ��Ⱦ��� community  ��v1 v2��Ҳ���ǲ�������
			offset++;
			std::string password = "";
			for (unsigned i = offset; i < offset + opLen; ++i){
				password += data[i];
			}
			up->password = password;
		}
		else
		{
			//����snmp v3   ��������  Ŀǰv3 ���������Ĵ������
		}
	}
	else if ("pop3" == m_protocol){
		//pop3�ǻ���tcp��
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 20;

		//��ȡ�������� USER PASS �ؼ���ֻ���ܳ����ڵ�һ��
		std::string str = "";
		for (unsigned i = offset; i < len && data[i] != '\n'; ++i)
			str += data[i];

		//��������
		std::string pattern = "(USER|PASS)\\s+([^\\s\\r\\n]*)";

		std::smatch sm;
		while (std::regex_search(str, sm, std::regex(pattern))){
			auto it = sm.begin();
			std::string key = *(it + 1);
			std::string value = *(it + 2);
			if (key == "USER")
				up->user = value;
			else if (key == "PASS")
				up->password = value;
			str = sm.suffix().str();
		}
	}
	else if ("imap" == m_protocol){
		//imap�ǻ���tcp��
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 20;

		//��ȡ��������
		std::string str = "";
		for (unsigned i = offset; i < len && data[i] != '\n'; ++i)
			str += data[i];

		//��������
		std::string pattern = "\\s+LOGIN\\s+([^\\s\\r\\n]*)\\s+(\"|')([^\\s\\r\\n]*)(\\2)";

		std::smatch sm;
		if (std::regex_search(str, sm, std::regex(pattern))){
			auto it = sm.begin();
			up->user = *(it + 1);
			up->password = *(it + 3);
		}
	}
	else if ("smtp" == m_protocol){
		//������mac  �ͻ���mac   ���û�н��뵽�û����������������������mac����0  
		static u_char srcMac[6] = { 0 }, desMac[6] = { 0 };

		//smtp�ǻ���tcp��  Ŀǰ�������Ѿ�û����plain��ʽ��֤�� ��������base64�ı���  ����ֱ�ӽ���base64����
		//note�� ��Ȼsmtp��չ�����������õ���base64���д���  ����base64�����ǹ�����  �������ݽ��н���  ����������
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 20;

		//smtp�İ���ɢ������һ��ֻ��һ���ַ��������ռ�������������
		static int flag = 0; //0;û�н����û�����������Ĳ�׽  1�����ڲ�׽�û���  2�����ڲ�׽����
		static std::string value = "";

		std::string str = "";
		unsigned i = offset;
		for (; i < len && data[i] != '\r' && data[i] != '\n' && data[i] != '\0'; ++i){
			str += data[i];
		}


		datapkt * pkt = (datapkt*)m_localDataList.GetTail();		
		CString xx;
		xx.Format(_T("%d"), flag);
		OutputDebugString(xx);
		if ((u_char_all_zero(srcMac, 6) && u_char_all_zero(desMac, 6)) ||
			(!u_char_compare(srcMac, pkt->ethh->src, 6) && !u_char_compare(desMac, pkt->ethh->dest, 6))){
			if (str.empty())
				return false;
			//str �ǿ� ���������ڷ����� �ͽ�����һ������
			std::string tmp = "";
			if (flag && MyBase64::Base64Decode(value, &tmp)){
				if (flag == 1)
					up->user = tmp;
				else if (flag == 2)
					up->password = tmp;
			}
			value = "";

			//��׽�ض���������
			//�������� 334 ״̬���ʾ��¼�ͻ������� ������ŵ��ǵȴ�������ֶ���
			std::string pattern = "\\s*334\\s+([^\\s\\r\\n]*)";
			std::smatch sm;
			//�жϷ�������ָ���ǲ���Ҫ�������û�����������
			if (std::regex_search(str, sm, std::regex(pattern))){
				auto it = sm.begin();
				std::string fieldname = *(it + 1);
				std::string output = "";
				if (MyBase64::Base64Decode(fieldname, &output)){
					std::transform(output.begin(), output.end(), output.begin(), ::tolower);
					//�������û�����������
					if (output.find("username") == 0 || output.find("password") == 0){
						for (unsigned i = 0; i < 6; ++i){
							srcMac[i] = pkt->ethh->src[i];
							desMac[i] = pkt->ethh->dest[i];
						}
						flag = output.find("username") == 0 ? 1 : (output.find("password") == 0 ? 2 : 0);
						return true;
					}
				}

			}
			for (unsigned i = 0; i < 6; ++i){
				srcMac[i] = desMac[i] = '\0';
			}
			flag = 0;
			return false;
		}
		else if ((!(u_char_compare(srcMac, pkt->ethh->dest, 6) && !u_char_compare(desMac, pkt->ethh->src, 6))) &&
			((1 == flag || 2 == flag))){
			value += str;
		}
	}
	else{
		//��������Э��...
	}
	return true;
}


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cmcf6Dlg �Ի���




Cmcf6Dlg::Cmcf6Dlg(CWnd* pParent /*=NULL*/)
: CDialog(Cmcf6Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmcf6Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_comboBox);
	DDX_Control(pDX, IDC_COMBO2, m_comboBoxRule);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_buttonStart);
	DDX_Control(pDX, IDC_BUTTON2, m_buttonStop);
	DDX_Control(pDX, IDC_BUTTON5, m_buttonSave);
	DDX_Control(pDX, IDC_BUTTON4, m_buttonRead);
}

BEGIN_MESSAGE_MAP(Cmcf6Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &Cmcf6Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Cmcf6Dlg::OnBnClickedButton2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &Cmcf6Dlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &Cmcf6Dlg::OnNMCustomdrawList1)
	ON_BN_CLICKED(IDC_BUTTON5, &Cmcf6Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &Cmcf6Dlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// Cmcf6Dlg ��Ϣ�������

BOOL Cmcf6Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_NORMAL);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listCtrl.InsertColumn(0, _T("���"), 3, 30);                        //1��ʾ�ң�2��ʾ�У�3��ʾ��
	m_listCtrl.InsertColumn(1, _T("ʱ��"), 3, 130);
	m_listCtrl.InsertColumn(2, _T("����"), 3, 72);
	m_listCtrl.InsertColumn(3, _T("ԴMAC��ַ"), 3, 140);
	m_listCtrl.InsertColumn(4, _T("Ŀ��MAC��ַ"), 3, 140);
	m_listCtrl.InsertColumn(5, _T("Э��"), 3, 70);
	m_listCtrl.InsertColumn(6, _T("ԴIP��ַ"), 3, 145);
	m_listCtrl.InsertColumn(7, _T("Ŀ��IP��ַ"), 3, 145);
	m_listCtrl.InsertColumn(8, _T("�û���"), 3, 130);
	m_listCtrl.InsertColumn(9, _T("����"), 3, 130);

	m_comboBox.AddString(_T("��ѡ��һ�������ӿ�(��ѡ)"));
	m_comboBoxRule.AddString(_T("��ѡ����˹���(��ѡ)"));

	if (lixsniff_initCap()<0)
		return FALSE;

	/*��ʼ���ӿ��б�*/
	for (dev = alldev; dev; dev = dev->next)
	{
		if (dev->description)
			m_comboBox.AddString(CString(dev->description));  //////////////////////////////Problem 1�ַ�������
	}

	/*��ʼ�����˹����б�*/
	m_comboBoxRule.AddString(_T("tcp"));
	m_comboBoxRule.AddString(_T("udp"));
	m_comboBoxRule.AddString(_T("ip"));
	m_comboBoxRule.AddString(_T("icmp"));
	m_comboBoxRule.AddString(_T("arp"));
	m_comboBoxRule.AddString(_T("ftp"));
	m_comboBoxRule.AddString(_T("http"));
	m_comboBoxRule.AddString(_T("snmp"));
	m_comboBoxRule.AddString(_T("pop3"));
	m_comboBoxRule.AddString(_T("imap"));
	m_comboBoxRule.AddString(_T("smtp"));

	m_comboBox.SetCurSel(0);
	m_comboBoxRule.SetCurSel(0);

	m_buttonStop.EnableWindow(FALSE);
	m_buttonSave.EnableWindow(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Cmcf6Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cmcf6Dlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cmcf6Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////���¼�������///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��ʼ��ť
void Cmcf6Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//����Ѿ��������ˣ���ʾ��������
	if (!this->m_dataSaved && this->m_localDataList.IsEmpty() == FALSE)
	{
		if (MessageBox(_T("ȷ�ϲ��������ݣ�"), _T("����"), MB_YESNO) == IDNO)
		{
			this->lixsniff_saveFile();
		}
	}

	this->npkt = 1;													//���¼���
	this->m_localDataList.RemoveAll();				//ÿ��һ��ʼ�ͽ���ǰ���������յ�
	this->m_netDataList.RemoveAll();
	this->m_upDataList.RemoveAll(); //����û�������ṹ����
	memset(&(this->npacket), 0, sizeof(struct pktcount));
	this->lixsniff_updateNPacket();

	//�����ļ�ָ��
	this->dumpfile = nullptr;
	this->upstream = nullptr;

	if (this->lixsniff_startCap()<0)
		return;
	this->m_listCtrl.DeleteAllItems();
	this->m_treeCtrl.DeleteAllItems();
	this->m_edit.SetWindowText(_T(""));
	this->m_buttonStart.EnableWindow(FALSE);
	this->m_buttonStop.EnableWindow(TRUE);
	this->m_buttonSave.EnableWindow(FALSE);
}

//������ť
void Cmcf6Dlg::OnBnClickedButton2()
{
	//�ļ�����ˢ������
	pcap_dump_flush(this->dumpfile);
	pcap_dump_close(this->dumpfile);
	this->upstream->close();
	delete this->upstream;

	if (NULL == this->m_ThreadHandle)
		return;
	if (TerminateThread(this->m_ThreadHandle, -1) == 0)
	{
		MessageBox(_T("�ر��̴߳������Ժ�����"));
		return;
	}
	this->m_ThreadHandle = NULL;
	this->m_buttonStart.EnableWindow(TRUE);
	this->m_buttonStop.EnableWindow(FALSE);
	this->m_buttonSave.EnableWindow(TRUE);
}

//�б�
void Cmcf6Dlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int index;
	index = this->m_listCtrl.GetHotItem();

	if (index>this->m_localDataList.GetCount() - 1)
		return;

	this->lixsniff_updateEdit(index);
	this->lixsniff_updateTree(index);
	*pResult = 0;
}

//���水ť
void Cmcf6Dlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (this->lixsniff_saveFile()<0)
		return;
}

//��ȡ��ť
void Cmcf6Dlg::OnBnClickedButton4()
{
	//���ļ��Ի���
	CFileDialog   FileDlg(TRUE, _T(".lix"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	FileDlg.m_ofn.lpstrInitialDir = _T("c:\\");
	if (FileDlg.DoModal() == IDOK)
	{
		//��ȡ֮ǰ��ListCtrl���
		this->m_listCtrl.DeleteAllItems();
		this->npkt = 1;													//�б����¼���
		this->m_localDataList.RemoveAll();				//ÿ��һ��ʼ�ͽ���ǰ���������յ�
		this->m_netDataList.RemoveAll();
		this->m_upDataList.RemoveAll();
		memset(&(this->npacket), 0, sizeof(struct pktcount));//������������

		int ret = this->lixsniff_readFile(FileDlg.GetPathName());
		if (ret < 0)
			return;
	}
}

//�ı�ListCtrlÿ����ɫ
void Cmcf6Dlg::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMLVCUSTOMDRAW pNMCD = (LPNMLVCUSTOMDRAW)pNMHDR;
	*pResult = 0;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage){
		COLORREF crText;
		char buf[10];
		memset(buf, 0, 10);
		POSITION pos = this->m_localDataList.FindIndex(pNMCD->nmcd.dwItemSpec);
		struct datapkt * local_data = (struct datapkt *)this->m_localDataList.GetAt(pos);
		strcpy(buf, local_data->pktType);

		if (strcmp(buf, "IPV6") == 0)
			crText = RGB(111, 224, 254);
		else if (strcmp(buf, "UDP") == 0)
			crText = RGB(194, 195, 252);
		else if (strcmp(buf, "TCP") == 0)
			crText = RGB(230, 230, 230);
		else if (strcmp(buf, "ARP") == 0)
			crText = RGB(226, 238, 227);
		else if (strcmp(buf, "ICMP") == 0)
			crText = RGB(49, 164, 238);
		else if (strcmp(buf, "HTTP") == 0)
			crText = RGB(238, 232, 180);
		else if (strcmp(buf, "ICMPv6") == 0)
			crText = RGB(189, 254, 76);

		pNMCD->clrTextBk = crText;
		*pResult = CDRF_DODEFAULT;
	}
}
//////////////////////////////////////////�۹��ܺ�����///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��ʼ��winpcap
int Cmcf6Dlg::lixsniff_initCap()
{
	devCount = 0;
	if (pcap_findalldevs(&alldev, errbuf) == -1)
		return -1;
	for (dev = alldev; dev; dev = dev->next)
		devCount++;
	return 0;
}

//��ʼ����
int Cmcf6Dlg::lixsniff_startCap()
{
	int if_index, filter_index, count;
	u_int netmask;
	struct bpf_program fcode;
	char upfilename[MAX_PATH] = { 0 };

	lixsniff_initCap();

	//��ýӿں͹���������
	if_index = this->m_comboBox.GetCurSel();
	filter_index = this->m_comboBoxRule.GetCurSel();

	if (0 == if_index || CB_ERR == if_index)
	{
		MessageBox(_T("��ѡ��һ�����ʵ������ӿ�"));
		return -1;
	}
	if (CB_ERR == filter_index)
	{
		MessageBox(_T("������ѡ�����"));
		return -1;
	}

	/*���ѡ�е������ӿ�*/
	dev = alldev;
	for (count = 0; count<if_index - 1; count++)
		dev = dev->next;

	if ((adhandle = pcap_open_live(dev->name,	// �豸��
		65536,											//�������ݰ�����																					
		1,													// ����ģʽ (��0��ζ���ǻ���ģʽ)
		1000,												// ����ʱ����
		errbuf											// ������Ϣ
		)) == NULL)
	{
		MessageBox(_T("�޷��򿪽ӿڣ�" + CString(dev->description)));
		pcap_freealldevs(alldev);
		return -1;
	}

	/*����Ƿ�Ϊ��̫��*/
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		MessageBox(_T("�ⲻ�ʺ��ڷ���̫��������!"));
		pcap_freealldevs(alldev);
		return -1;
	}

	if (dev->addresses != NULL)
		netmask = ((struct sockaddr_in *)(dev->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		netmask = 0xffffff;

	//���������
	if (0 == filter_index)
	{
		char filter[] = "";
		if (pcap_compile(adhandle, &fcode, filter, 1, netmask) <0)
		{
			MessageBox(_T("�﷨�����޷����������"));
			pcap_freealldevs(alldev);
			return -1;
		}
	}
	else{
		CString str;
		char *filter;
		int len, x;
		this->m_comboBoxRule.GetLBText(filter_index, str);
		m_protocol = str;
		m_dataSaved = false;
		m_nameOfPass = "";
		m_nameOfUser = "";
		//�����http ���������
		if ("http" == str){
			InputForm inf;
			if (IDOK != inf.DoModal()){
				return -1;
			}
		}

		//���ftp http snmp pop3 imap smtp����
		if ("ftp" == str || "http" == str || "snmp" == str || "pop3" == str || "imap" == str || "smtp" == str)
			str = CString("port ") + str;

		len = str.GetLength() + 1;
		filter = (char*)malloc(len);
		for (x = 0; x<len; x++)
		{
			filter[x] = str.GetAt(x);
		}

		if (pcap_compile(adhandle, &fcode, filter, 0, netmask) <0)
		{
			MessageBox(_T("�﷨�����޷����������"));
			pcap_freealldevs(alldev);
			return -1;
		}
	}


	//���ù�����
	if (pcap_setfilter(adhandle, &fcode)<0)
	{
		MessageBox(_T("���ù���������"));
		pcap_freealldevs(alldev);
		return -1;
	}

	/* �������ݰ��洢·��*/
	CFileFind file;
	char thistime[30];
	struct tm *ltime;
	memset(filepath, 0, 512);
	memset(filename, 0, 64);
	memset(upfilepath, 0, 512);

	if (!file.FindFile(_T("SavedData")))
	{
		CreateDirectory(_T("SavedData"), NULL);
	}

	time_t nowtime;
	time(&nowtime);
	ltime = localtime(&nowtime);
	strftime(thistime, sizeof(thistime), "%Y%m%d %H%M%S", ltime);
	strcpy(filepath, "SavedData\\");
	strcat(filename, thistime);
	strcat(upfilename, thistime);
	strcat(filename, ".lix");
	strcat(upfilename, ".up");

	strcat(upfilepath, filepath);

	strcat(filepath, filename);
	dumpfile = pcap_dump_open(adhandle, filepath);
	if (dumpfile == NULL)
	{
		MessageBox(_T("�ļ���������"));
		return -1;
	}

	//��һ���û������뱣���ļ�
	strcat(upfilepath, upfilename);
	std::ofstream * upstream = new std::ofstream(upfilepath, std::ios::binary);
	if (!upstream){
		MessageBox(_T("�ļ���������"));
		return -1;
	}
	this->upstream = upstream;

	pcap_freealldevs(alldev);

	/*�������ݣ��½��̴߳���*/
	LPDWORD threadCap = NULL;
	m_ThreadHandle = CreateThread(NULL, 0, lixsinff_CapThread, this, 0, threadCap);
	if (m_ThreadHandle == NULL)
	{
		int code = GetLastError();
		CString str;
		str.Format(_T("�����̴߳��󣬴���Ϊ%d."), code);
		MessageBox(str);
		return -1;
	}
	return 1;
}

DWORD WINAPI lixsinff_CapThread(LPVOID lpParameter)
{
	int res, nItem;
	struct tm *ltime;
	CString timestr, buf, srcMac, destMac;
	time_t local_tv_sec;
	struct pcap_pkthdr *header;									  //���ݰ�ͷ
	const u_char *pkt_data = NULL, *pData = NULL;     //�������յ����ֽ�������
	u_char *ppkt_data;

	Cmcf6Dlg *pthis = (Cmcf6Dlg*)lpParameter;
	if (NULL == pthis->m_ThreadHandle)
	{
		MessageBox(NULL, _T("�߳̾������"), _T("��ʾ"), MB_OK);
		return -1;
	}

	while ((res = pcap_next_ex(pthis->adhandle, &header, &pkt_data)) >= 0)
	{
		if (res == 0)				//��ʱ
			continue;

		struct datapkt *data = (struct datapkt*)malloc(sizeof(struct datapkt));
		memset(data, 0, sizeof(struct datapkt));

		if (NULL == data)
		{
			MessageBox(NULL, _T("�ռ��������޷������µ����ݰ�"), _T("Error"), MB_OK);
			return -1;
		}

		//������������������ݰ����ڴ���Χ��
		if (analyze_frame(pkt_data, data, &(pthis->npacket))<0)
			continue;

		//�����ݰ����浽�򿪵��ļ���
		if (pthis->dumpfile != NULL)
		{
			pcap_dump((unsigned char*)pthis->dumpfile, header, pkt_data);
		}


		//�����ػ��������װ��һ�������У��Ա����ʹ��		
		ppkt_data = (u_char*)malloc(header->len);
		memcpy(ppkt_data, pkt_data, header->len);

		pthis->m_localDataList.AddTail(data);
		pthis->m_netDataList.AddTail(ppkt_data);

		//���������û���������
		PUPDATA up = new UPDATA;


		//�����û���������
		pthis->filterUserAndPass(up, pkt_data, header->len);

		//���û�������ṹ��������
		pthis->m_upDataList.AddTail(up);

		//���û�������������ļ�

		if (nullptr != pthis->upstream){
			saveUserAndPass(up, *(pthis->upstream));
		}


		/*Ԥ�������ʱ�䡢����*/
		data->len = header->len;								//��·���յ������ݳ���
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		data->time[0] = ltime->tm_year + 1900;
		data->time[1] = ltime->tm_mon + 1;
		data->time[2] = ltime->tm_mday;
		data->time[3] = ltime->tm_hour;
		data->time[4] = ltime->tm_min;
		data->time[5] = ltime->tm_sec;

		/*Ϊ�½��յ������ݰ���listControl���½�һ��item*/
		buf.Format(_T("%d"), pthis->npkt);
		nItem = pthis->m_listCtrl.InsertItem(pthis->npkt, buf);

		/*��ʾʱ���*/
		timestr.Format(_T("%d/%d/%d  %d:%d:%d"), data->time[0],
			data->time[1], data->time[2], data->time[3], data->time[4], data->time[5]);
		pthis->m_listCtrl.SetItemText(nItem, 1, timestr);
		//pthis->m_listCtrl.setitem

		/*��ʾ����*/
		buf.Empty();
		buf.Format(_T("%d"), data->len);
		pthis->m_listCtrl.SetItemText(nItem, 2, buf);

		/*��ʾԴMAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->src[0], data->ethh->src[1],
			data->ethh->src[2], data->ethh->src[3], data->ethh->src[4], data->ethh->src[5]);
		pthis->m_listCtrl.SetItemText(nItem, 3, buf);

		/*��ʾĿ��MAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->dest[0], data->ethh->dest[1],
			data->ethh->dest[2], data->ethh->dest[3], data->ethh->dest[4], data->ethh->dest[5]);
		pthis->m_listCtrl.SetItemText(nItem, 4, buf);

		/*���Э��*/
		pthis->m_listCtrl.SetItemText(nItem, 5, CString(data->pktType));

		/*���ԴIP*/
		buf.Empty();
		if (0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_srcip[0],
				data->arph->ar_srcip[1], data->arph->ar_srcip[2], data->arph->ar_srcip[3]);
		}
		else if (0x0800 == data->ethh->type) {
			struct  in_addr in;
			in.S_un.S_addr = data->iph->saddr;
			buf = CString(inet_ntoa(in));
		}
		else if (0x86dd == data->ethh->type){
			int n;
			for (n = 0; n<8; n++)
			{
				if (n <= 6)
					buf.AppendFormat(_T("%02x:"), data->iph6->saddr[n]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->saddr[n]);
			}
		}
		pthis->m_listCtrl.SetItemText(nItem, 6, buf);

		/*���Ŀ��IP*/
		buf.Empty();
		if (0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_destip[0],
				data->arph->ar_destip[1], data->arph->ar_destip[2], data->arph->ar_destip[3]);
		}
		else if (0x0800 == data->ethh->type){
			struct  in_addr in;
			in.S_un.S_addr = data->iph->daddr;
			buf = CString(inet_ntoa(in));
		}
		else if (0x86dd == data->ethh->type){
			int n;
			for (n = 0; n<8; n++)
			{
				if (n <= 6)
					buf.AppendFormat(_T("%02x:"), data->iph6->daddr[n]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->daddr[n]);
			}
		}
		pthis->m_listCtrl.SetItemText(nItem, 7, buf);

		//��ʾ�û���
		buf.Empty();
		for (unsigned i = 0; i < up->user.length(); ++i)
			buf += up->user[i];
		pthis->m_listCtrl.SetItemText(nItem, 8, buf);

		//��ʾ����
		buf.Empty();
		for (unsigned i = 0; i < up->password.length(); ++i)
			buf += up->password[i];
		pthis->m_listCtrl.SetItemText(nItem, 9, buf);

		/*�԰�����*/
		pthis->npkt++;

	}
	return 1;
}

//������Ϣ
int Cmcf6Dlg::lixsniff_updateEdit(int index)
{
	POSITION localpos, netpos;
	localpos = this->m_localDataList.FindIndex(index);
	netpos = this->m_netDataList.FindIndex(index);

	struct datapkt* local_data = (struct datapkt*)(this->m_localDataList.GetAt(localpos));
	u_char * net_data = (u_char*)(this->m_netDataList.GetAt(netpos));

	CString buf;
	print_packet_hex(net_data, local_data->len, &buf);
	//buf.Format(_T("%s"), net_data);

	this->m_edit.SetWindowText(buf);

	return 1;
}

//����ͳ������
int Cmcf6Dlg::lixsniff_updateNPacket()
{
	return 1;
}

//�������οؼ�
int Cmcf6Dlg::lixsniff_updateTree(int index)
{
	POSITION localpos;
	CString str;
	int i;

	this->m_treeCtrl.DeleteAllItems();

	localpos = this->m_localDataList.FindIndex(index);
	struct datapkt* local_data = (struct datapkt*)(this->m_localDataList.GetAt(localpos));

	HTREEITEM root = this->m_treeCtrl.GetRootItem();
	str.Format(_T("���յ��ĵ�%d�����ݰ�"), index + 1);
	HTREEITEM data = this->m_treeCtrl.InsertItem(str, root);

	/*����֡����*/
	HTREEITEM frame = this->m_treeCtrl.InsertItem(_T("��·������"), data);
	//ԴMAC
	str.Format(_T("ԴMAC��"));
	for (i = 0; i<6; i++)
	{
		if (i <= 4)
			str.AppendFormat(_T("%02x-"), local_data->ethh->src[i]);
		else
			str.AppendFormat(_T("%02x"), local_data->ethh->src[i]);
	}
	this->m_treeCtrl.InsertItem(str, frame);
	//Ŀ��MAC
	str.Format(_T("Ŀ��MAC��"));
	for (i = 0; i<6; i++)
	{
		if (i <= 4)
			str.AppendFormat(_T("%02x-"), local_data->ethh->dest[i]);
		else
			str.AppendFormat(_T("%02x"), local_data->ethh->dest[i]);
	}
	this->m_treeCtrl.InsertItem(str, frame);
	//����
	str.Format(_T("���ͣ�0x%02x"), local_data->ethh->type);
	this->m_treeCtrl.InsertItem(str, frame);

	/*����IP��ARP��IPv6���ݰ�*/
	if (0x0806 == local_data->ethh->type)							//ARP
	{
		HTREEITEM arp = this->m_treeCtrl.InsertItem(_T("ARPЭ��ͷ"), data);
		str.Format(_T("Ӳ�����ͣ�%d"), local_data->arph->ar_hrd);
		this->m_treeCtrl.InsertItem(str, arp);
		str.Format(_T("Э�����ͣ�0x%02x"), local_data->arph->ar_pro);
		this->m_treeCtrl.InsertItem(str, arp);
		str.Format(_T("Ӳ����ַ���ȣ�%d"), local_data->arph->ar_hln);
		this->m_treeCtrl.InsertItem(str, arp);
		str.Format(_T("Э���ַ���ȣ�%d"), local_data->arph->ar_pln);
		this->m_treeCtrl.InsertItem(str, arp);
		str.Format(_T("�����룺%d"), local_data->arph->ar_op);
		this->m_treeCtrl.InsertItem(str, arp);

		str.Format(_T("���ͷ�MAC��"));
		for (i = 0; i<6; i++)
		{
			if (i <= 4)
				str.AppendFormat(_T("%02x-"), local_data->arph->ar_srcmac[i]);
			else
				str.AppendFormat(_T("%02x"), local_data->arph->ar_srcmac[i]);
		}
		this->m_treeCtrl.InsertItem(str, arp);

		str.Format(_T("���ͷ�IP��"), local_data->arph->ar_hln);
		for (i = 0; i<4; i++)
		{
			if (i <= 2)
				str.AppendFormat(_T("%d."), local_data->arph->ar_srcip[i]);
			else
				str.AppendFormat(_T("%d"), local_data->arph->ar_srcip[i]);
		}
		this->m_treeCtrl.InsertItem(str, arp);

		str.Format(_T("���շ�MAC��"), local_data->arph->ar_hln);
		for (i = 0; i<6; i++)
		{
			if (i <= 4)
				str.AppendFormat(_T("%02x-"), local_data->arph->ar_destmac[i]);
			else
				str.AppendFormat(_T("%02x"), local_data->arph->ar_destmac[i]);
		}
		this->m_treeCtrl.InsertItem(str, arp);

		str.Format(_T("���շ�IP��"), local_data->arph->ar_hln);
		for (i = 0; i<4; i++)
		{
			if (i <= 2)
				str.AppendFormat(_T("%d."), local_data->arph->ar_destip[i]);
			else
				str.AppendFormat(_T("%d"), local_data->arph->ar_destip[i]);
		}
		this->m_treeCtrl.InsertItem(str, arp);

	}
	else if (0x0800 == local_data->ethh->type){					//IP

		HTREEITEM ip = this->m_treeCtrl.InsertItem(_T("IPЭ��ͷ"), data);

		str.Format(_T("�汾��%d"), local_data->iph->version);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("IPͷ����%d"), local_data->iph->ihl);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("�������ͣ�%d"), local_data->iph->tos);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("�ܳ��ȣ�%d"), local_data->iph->tlen);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("��ʶ��0x%02x"), local_data->iph->id);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("��ƫ�ƣ�%d"), local_data->iph->frag_off);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("�����ڣ�%d"), local_data->iph->ttl);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("Э�飺%d"), local_data->iph->proto);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("ͷ��У��ͣ�0x%02x"), local_data->iph->check);
		this->m_treeCtrl.InsertItem(str, ip);

		str.Format(_T("ԴIP��"));
		struct in_addr in;
		in.S_un.S_addr = local_data->iph->saddr;
		str.AppendFormat(CString(inet_ntoa(in)));
		this->m_treeCtrl.InsertItem(str, ip);

		str.Format(_T("Ŀ��IP��"));
		in.S_un.S_addr = local_data->iph->daddr;
		str.AppendFormat(CString(inet_ntoa(in)));
		this->m_treeCtrl.InsertItem(str, ip);

		/*�������ICMP��UDP��TCP*/
		if (1 == local_data->iph->proto)							//ICMP
		{
			HTREEITEM icmp = this->m_treeCtrl.InsertItem(_T("ICMPЭ��ͷ"), data);

			str.Format(_T("����:%d"), local_data->icmph->type);
			this->m_treeCtrl.InsertItem(str, icmp);
			str.Format(_T("����:%d"), local_data->icmph->code);
			this->m_treeCtrl.InsertItem(str, icmp);
			str.Format(_T("���:%d"), local_data->icmph->seq);
			this->m_treeCtrl.InsertItem(str, icmp);
			str.Format(_T("У���:%d"), local_data->icmph->chksum);
			this->m_treeCtrl.InsertItem(str, icmp);

		}
		else if (6 == local_data->iph->proto){				//TCP

			HTREEITEM tcp = this->m_treeCtrl.InsertItem(_T("TCPЭ��ͷ"), data);

			str.Format(_T("  Դ�˿�:%d"), local_data->tcph->sport);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  Ŀ�Ķ˿�:%d"), local_data->tcph->dport);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  ���к�:0x%02x"), local_data->tcph->seq);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  ȷ�Ϻ�:%d"), local_data->tcph->ack_seq);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  ͷ������:%d"), local_data->tcph->doff);

			HTREEITEM flag = this->m_treeCtrl.InsertItem(_T(" +��־λ"), tcp);

			str.Format(_T("cwr %d"), local_data->tcph->cwr);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("ece %d"), local_data->tcph->ece);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("urg %d"), local_data->tcph->urg);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("ack %d"), local_data->tcph->ack);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("psh %d"), local_data->tcph->psh);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("rst %d"), local_data->tcph->rst);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("syn %d"), local_data->tcph->syn);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("fin %d"), local_data->tcph->fin);
			this->m_treeCtrl.InsertItem(str, flag);

			str.Format(_T("  ����ָ��:%d"), local_data->tcph->urg_ptr);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  У���:0x%02x"), local_data->tcph->check);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  ѡ��:%d"), local_data->tcph->opt);
			this->m_treeCtrl.InsertItem(str, tcp);
		}
		else if (17 == local_data->iph->proto){				//UDP
			HTREEITEM udp = this->m_treeCtrl.InsertItem(_T("UDPЭ��ͷ"), data);

			str.Format(_T("Դ�˿�:%d"), local_data->udph->sport);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("Ŀ�Ķ˿�:%d"), local_data->udph->dport);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("�ܳ���:%d"), local_data->udph->len);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("У���:0x%02x"), local_data->udph->check);
			this->m_treeCtrl.InsertItem(str, udp);
		}
	}
	else if (0x86dd == local_data->ethh->type){		//IPv6
		HTREEITEM ip6 = this->m_treeCtrl.InsertItem(_T("IPv6Э��ͷ"), data);

		//////////////////////////////////////////////////////////////////////////////////////////
		str.Format(_T("�汾:%d"), local_data->iph6->flowtype);
		this->m_treeCtrl.InsertItem(str, ip6);
		str.Format(_T("������:%d"), local_data->iph6->version);
		this->m_treeCtrl.InsertItem(str, ip6);
		///////////////////////////////////////////////////////////////////////////////////////////
		str.Format(_T("����ǩ:%d"), local_data->iph6->flowid);
		this->m_treeCtrl.InsertItem(str, ip6);
		str.Format(_T("��Ч�غɳ���:%d"), local_data->iph6->plen);
		this->m_treeCtrl.InsertItem(str, ip6);
		str.Format(_T("��һ���ײ�:0x%02x"), local_data->iph6->nh);
		this->m_treeCtrl.InsertItem(str, ip6);
		str.Format(_T("������:%d"), local_data->iph6->hlim);
		this->m_treeCtrl.InsertItem(str, ip6);

		str.Format(_T("Դ��ַ:"));
		int n;
		for (n = 0; n<8; n++)
		{
			if (n <= 6)
				str.AppendFormat(_T("%02x:"), local_data->iph6->saddr[n]);
			else
				str.AppendFormat(_T("%02x"), local_data->iph6->saddr[n]);
		}
		this->m_treeCtrl.InsertItem(str, ip6);

		str.Format(_T("Ŀ�ĵ�ַ:"));
		for (n = 0; n<8; n++)
		{
			if (n <= 6)
				str.AppendFormat(_T("%02x:"), local_data->iph6->saddr[n]);
			else
				str.AppendFormat(_T("%02x"), local_data->iph6->saddr[n]);
		}
		this->m_treeCtrl.InsertItem(str, ip6);

		/*�������ICMPv6��UDP��TCP*/
		if (0x3a == local_data->iph6->nh)							//ICMPv6
		{
			HTREEITEM icmp6 = this->m_treeCtrl.InsertItem(_T("ICMPv6Э��ͷ"), data);

			str.Format(_T("����:%d"), local_data->icmph6->type);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("����:%d"), local_data->icmph6->code);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("���:%d"), local_data->icmph6->seq);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("У���:%d"), local_data->icmph6->chksum);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-����:%d"), local_data->icmph6->op_type);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-����%d"), local_data->icmph6->op_len);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-��·���ַ:"));
			int i;
			for (i = 0; i<6; i++)
			{
				if (i <= 4)
					str.AppendFormat(_T("%02x-"), local_data->icmph6->op_ethaddr[i]);
				else
					str.AppendFormat(_T("%02x"), local_data->icmph6->op_ethaddr[i]);
			}
			this->m_treeCtrl.InsertItem(str, icmp6);

		}
		else if (0x06 == local_data->iph6->nh){				//TCP

			HTREEITEM tcp = this->m_treeCtrl.InsertItem(_T("TCPЭ��ͷ"), data);

			str.Format(_T("  Դ�˿�:%d"), local_data->tcph->sport);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  Ŀ�Ķ˿�:%d"), local_data->tcph->dport);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  ���к�:0x%02x"), local_data->tcph->seq);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  ȷ�Ϻ�:%d"), local_data->tcph->ack_seq);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  ͷ������:%d"), local_data->tcph->doff);

			HTREEITEM flag = this->m_treeCtrl.InsertItem(_T("��־λ"), tcp);

			str.Format(_T("cwr %d"), local_data->tcph->cwr);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("ece %d"), local_data->tcph->ece);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("urg %d"), local_data->tcph->urg);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("ack %d"), local_data->tcph->ack);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("psh %d"), local_data->tcph->psh);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("rst %d"), local_data->tcph->rst);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("syn %d"), local_data->tcph->syn);
			this->m_treeCtrl.InsertItem(str, flag);
			str.Format(_T("fin %d"), local_data->tcph->fin);
			this->m_treeCtrl.InsertItem(str, flag);

			str.Format(_T("  ����ָ��:%d"), local_data->tcph->urg_ptr);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  У���:0x%02x"), local_data->tcph->check);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  ѡ��:%d"), local_data->tcph->opt);
			this->m_treeCtrl.InsertItem(str, tcp);
		}
		else if (0x11 == local_data->iph6->nh){				//UDP
			HTREEITEM udp = this->m_treeCtrl.InsertItem(_T("UDPЭ��ͷ"), data);

			str.Format(_T("Դ�˿�:%d"), local_data->udph->sport);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("Ŀ�Ķ˿�:%d"), local_data->udph->dport);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("�ܳ���:%d"), local_data->udph->len);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("У���:0x%02x"), local_data->udph->check);
			this->m_treeCtrl.InsertItem(str, udp);
		}
	}

	return 1;
}


int Cmcf6Dlg::lixsniff_saveFile()
{
	CFileFind find;
	if (NULL == find.FindFile(CString(filepath)) || NULL == find.FindFile(CString(upfilepath)))
	{
		MessageBox(_T("�����ļ�����δ֪����"));
		return -1;
	}

	//���ļ��Ի���
	CFileDialog   FileDlg(FALSE, _T("lix"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	FileDlg.m_ofn.lpstrInitialDir = _T("c:\\");
	if (FileDlg.DoModal() == IDOK)
	{
		CopyFile(CString(this->filepath), FileDlg.GetPathName(), FALSE);

		//��ȡ���Ӧ�ı����û��������ļ���·��
		CString filepath = FileDlg.GetPathName();
		char * charpath = (char*)malloc(filepath.GetLength() + 1);
		ZeroMemory(charpath, filepath.GetLength() + 1);
		for (unsigned i = 0; i < filepath.GetLength(); ++i){
			charpath[i] = filepath[i];
		}
		char* lastDot = strrchr(charpath, '.');
		unsigned upPathLen = lastDot - charpath;
		const char * upSuffix = ".up";  //�û������ֵ�Ա�����ļ�����׺
		upPathLen += strlen(upSuffix);
		char * upPath = (char*)malloc((upPathLen + 1) * sizeof(char));
		ZeroMemory(upPath, upPathLen + 1);
		unsigned j = 0;
		for (char* i = charpath; i < lastDot && j <= upPathLen; ++i, ++j){
			upPath[j] = (*i);
		}
		strcat(upPath, upSuffix);

		CopyFile(CString(this->upfilepath), CString(upPath), FALSE);

		free(upPath);
		free(charpath);
		this->m_dataSaved = true;
	}
	return 1;
}

int Cmcf6Dlg::lixsniff_readFile(CString path)
{
	int res, nItem, i;
	struct tm *ltime;
	CString timestr, buf, srcMac, destMac;
	time_t local_tv_sec;
	struct pcap_pkthdr *header;									  //���ݰ�ͷ
	const u_char *pkt_data = NULL;     //�������յ����ֽ�������
	u_char *ppkt_data;

	Cmcf6Dlg *pthis = this;						//Щ���������lixsinff_CapThread��Ϊ��Լ���������ʱ���pthisָ��
	pcap_t *fp;

	//·����Ҫ��char *���ͣ�������CStringǿ��ת�����char *
	int len = path.GetLength() + 1;
	char* charpath = (char *)malloc(len);
	memset(charpath, 0, len);
	if (NULL == charpath)
		return -1;

	for (i = 0; i<len; i++)
		charpath[i] = (char)path.GetAt(i);

	//������ļ�
	if ((fp = pcap_open_offline(charpath, errbuf)) == NULL)
	{
		MessageBox(_T("���ļ�����") + CString(errbuf));
		return -1;
	}

	char* lastDot = strrchr(charpath, '.');
	unsigned upPathLen = lastDot - charpath;
	const char * upSuffix = ".up";  //�û������ֵ�Ա�����ļ�����׺
	upPathLen += strlen(upSuffix);
	char * upPath = (char*)malloc((upPathLen + 1) * sizeof(char));
	ZeroMemory(upPath, upPathLen + 1);
	unsigned j = 0;
	for (char* i = charpath; i < lastDot && j <= upPathLen; ++i, ++j){
		upPath[j] = (*i);
	}
	strcat(upPath, upSuffix);

	std::ifstream upstream(upPath);
	if (!upstream){
		MessageBox(_T("���ļ�����") + CString(errbuf));
		return -1;
	}

	free(upPath);
	free(charpath);

	while ((res = pcap_next_ex(fp, &header, &pkt_data)) >= 0)
	{
		struct datapkt *data = (struct datapkt*)malloc(sizeof(struct datapkt));
		memset(data, 0, sizeof(struct datapkt));

		if (NULL == data)
		{
			MessageBox(_T("�ռ��������޷������µ����ݰ�"));
			return  -1;
		}

		//������������������ݰ����ڴ���Χ��
		if (analyze_frame(pkt_data, data, &(pthis->npacket))<0)
			continue;

		//���¸������ݰ�����
		pthis->lixsniff_updateNPacket();

		//�����ػ��������װ��һ�������У��Ա����ʹ��		
		ppkt_data = (u_char*)malloc(header->len);
		memcpy(ppkt_data, pkt_data, header->len);

		pthis->m_localDataList.AddTail(data);
		pthis->m_netDataList.AddTail(ppkt_data);

		//������������û�������
		PUPDATA up = new UPDATA;
		readUserPassword(up, upstream);

		/*Ԥ�������ʱ�䡢����*/
		data->len = header->len;								//��·���յ������ݳ���
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		data->time[0] = ltime->tm_year + 1900;
		data->time[1] = ltime->tm_mon + 1;
		data->time[2] = ltime->tm_mday;
		data->time[3] = ltime->tm_hour;
		data->time[4] = ltime->tm_min;
		data->time[5] = ltime->tm_sec;

		/*Ϊ�½��յ������ݰ���listControl���½�һ��item*/
		buf.Format(_T("%d"), pthis->npkt);
		nItem = pthis->m_listCtrl.InsertItem(pthis->npkt, buf);

		/*��ʾʱ���*/
		timestr.Format(_T("%d/%d/%d  %d:%d:%d"), data->time[0],
			data->time[1], data->time[2], data->time[3], data->time[4], data->time[5]);
		pthis->m_listCtrl.SetItemText(nItem, 1, timestr);

		/*��ʾ����*/
		buf.Empty();
		buf.Format(_T("%d"), data->len);
		pthis->m_listCtrl.SetItemText(nItem, 2, buf);

		/*��ʾԴMAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->src[0], data->ethh->src[1],
			data->ethh->src[2], data->ethh->src[3], data->ethh->src[4], data->ethh->src[5]);
		pthis->m_listCtrl.SetItemText(nItem, 3, buf);

		/*��ʾĿ��MAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->dest[0], data->ethh->dest[1],
			data->ethh->dest[2], data->ethh->dest[3], data->ethh->dest[4], data->ethh->dest[5]);
		pthis->m_listCtrl.SetItemText(nItem, 4, buf);

		/*���Э��*/
		pthis->m_listCtrl.SetItemText(nItem, 5, CString(data->pktType));

		/*���ԴIP*/
		buf.Empty();
		if (0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_srcip[0],
				data->arph->ar_srcip[1], data->arph->ar_srcip[2], data->arph->ar_srcip[3]);
		}
		else  if (0x0800 == data->ethh->type){
			struct  in_addr in;
			in.S_un.S_addr = data->iph->saddr;
			buf = CString(inet_ntoa(in));
		}
		else if (0x86dd == data->ethh->type){
			int i;
			for (i = 0; i<8; i++)
			{
				if (i <= 6)
					buf.AppendFormat(_T("%02x-"), data->iph6->saddr[i]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->saddr[i]);
			}
		}
		pthis->m_listCtrl.SetItemText(nItem, 6, buf);

		/*���Ŀ��IP*/
		buf.Empty();
		if (0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_destip[0],
				data->arph->ar_destip[1], data->arph->ar_destip[2], data->arph->ar_destip[3]);
		}
		else if (0x0800 == data->ethh->type) {
			struct  in_addr in;
			in.S_un.S_addr = data->iph->daddr;
			buf = CString(inet_ntoa(in));
		}
		else if (0x86dd == data->ethh->type){
			int i;
			for (i = 0; i<8; i++)
			{
				if (i <= 6)

					buf.AppendFormat(_T("%02x-"), data->iph6->daddr[i]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->daddr[i]);
			}
		}
		pthis->m_listCtrl.SetItemText(nItem, 7, buf);

		//��ʾ�û���
		buf.Empty();
		for (unsigned i = 0; i < up->user.length(); ++i)
			buf += up->user[i];
		pthis->m_listCtrl.SetItemText(nItem, 8, buf);

		//��ʾ����
		buf.Empty();
		for (unsigned i = 0; i < up->password.length(); ++i)
			buf += up->password[i];
		pthis->m_listCtrl.SetItemText(nItem, 9, buf);

		/*�԰�����*/
		pthis->npkt++;
	}

	pcap_close(fp);
	upstream.close();

	return 1;
}