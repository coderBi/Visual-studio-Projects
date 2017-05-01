// mcf6Dlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
DWORD WINAPI lixsinff_CapThread(LPVOID lpParameter);


/*
*从一个输入流中读取一行 解析出其中的用户名和密码
@param data PUPDATA 存储获取到的用户名密码
@param stream ifstream 输入流
@return bool 成功返回true 失败false
**/
bool readUserPassword(PUPDATA data, std::ifstream & stream){
	//先清空原来数据
	data->password = data->user = "";
	if (!stream)
		return false;
	std::string line;
	const unsigned maxLen = 1024;
	char buff[maxLen] = { 0 };
	if (!stream.getline(buff, maxLen))
		return false;  //读入一行失败，可能已经没有数据了
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
*保存用户名密码结构到一个输出流
@param data PUPDATA 待保存的用户名密码
@param stream ofstream 输出流
*@return bool 保存成功返回true 失败返回false
*/
bool saveUserAndPass(PUPDATA data, std::ofstream & stream){
	if (!stream)
		return false;
	stream << data->user << " " << data->password << std::endl;
	return true;
}

/*
*判断一个u_char数组是否所有元素都是0
*@param u_char arr[]  待验证的数组
*@param unsigned length 数组长度
*@return bool  全都是0 返回true  否则返回false
*/
bool u_char_all_zero(u_char arr[], unsigned length){
	for (unsigned i = 0; i < length; ++i){
		if (arr[i])
			return false;
	}
	return true;
}

/*
*比较两个u_char 数组
*@param u_char arr1[]  待比较的数组
*@param u_char arr2[]  待比较的数组
*@param unsigned length 数组长度
*@return int 相等返0  否则返回1 或者 -1
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
*过滤出用户名和密码
*@param up PUPDATA 用来存储过滤到的数据
*@param data u_char* 一个包的数据指针
*@return bool 执行成功返回true 失败false
*/
bool Cmcf6Dlg::filterUserAndPass(PUPDATA up, const u_char * data, unsigned len){
	if (!up || !data)
		return false;
	up->password = up->user = "";
	if ("ftp" == m_protocol){
		////////////note: ftp的用户名和密码是两个包分别传输的，所以一次只能获取到用户名或者密码////////////////////
		//ftp头之前偏移 数据帧头14 +　ip报文头 + tcp头20  ftp是基于tcp的
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 20;
		if (len <= offset || (data[offset] != 'U' && data[offset] != 'P'))
			return false;

		unsigned i = offset;
		//略过前面的 USER 或者 PASS 字符串
		for (; i < len && data[i] != ' '; ++i){}

		//略过前置空格
		for (; i < len && data[i] == ' '; ++i){}

		std::string content = "";
		//获取用户名或者密码
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
		//初始偏移 数据帧14 + ip头
		unsigned offset = 14 + 4 * (data[14] & 0xf);

		//捕获GET 或者 POST关键字
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
		//获取http头
		std::string str = "";
		for (; i < len && !(data[i] == '\n' && data[i - 2] == '\n'); ++i)
			str += data[i];
		str += data[i++];

		//如果是post请求  接着获取请求数据
		if ("POST" == method)
		for (; i < len && data[i] != '\n' && data[i] != '\r'; ++i)
			str += data[i];

		//构造正则
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
		//通过查阅资料和测试  snmp底层走的是ip/udp  udp报文头固定8字节
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 8;

		// snmp v1 v2c是公认的不安全的  snmp v3增加了安全配置
		//跳过报文标识 snmp为30
		offset++;
		//跳过长度域 这个长度域的一般表示为1~3字节  在snmp报文长度为128字节以下表示为1字节
		if (data[offset] & 0x80){
			unsigned lenLen = (data[offset] & 0xf);
			offset += 1 + lenLen;
		}
		else
		{
			offset++;
		}
		//获取版本号
		//跳过类型标识符
		offset++;
		//获取版本号所占长度
		unsigned versionLen = data[offset];
		u_char version = 0;
		offset++;
		for (unsigned i = 0; i < versionLen; ++i){
			version <<= 8;
			version += data[offset + i];
		}
		//+1表示最终版本号
		version = data[offset] + 1;
		//版本号小于3 表示 v1 v2
		if (version < 3){
			offset += versionLen + 1;
			//操作码长度标识
			unsigned opLen = data[offset];

			//获取后面的 opLen的长度就是 community  在v1 v2中也就是操作密码
			offset++;
			std::string password = "";
			for (unsigned i = offset; i < offset + opLen; ++i){
				password += data[i];
			}
			up->password = password;
		}
		else
		{
			//处理snmp v3   经过测试  目前v3 不存在明文传输情况
		}
	}
	else if ("pop3" == m_protocol){
		//pop3是基于tcp的
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 20;

		//获取首行内容 USER PASS 关键字只可能出现在第一行
		std::string str = "";
		for (unsigned i = offset; i < len && data[i] != '\n'; ++i)
			str += data[i];

		//构造正则
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
		//imap是基于tcp的
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 20;

		//获取首行内容
		std::string str = "";
		for (unsigned i = offset; i < len && data[i] != '\n'; ++i)
			str += data[i];

		//构造正则
		std::string pattern = "\\s+LOGIN\\s+([^\\s\\r\\n]*)\\s+(\"|')([^\\s\\r\\n]*)(\\2)";

		std::smatch sm;
		if (std::regex_search(str, sm, std::regex(pattern))){
			auto it = sm.begin();
			up->user = *(it + 1);
			up->password = *(it + 3);
		}
	}
	else if ("smtp" == m_protocol){
		//服务器mac  客户端mac   如果没有进入到用户名或者密码的输入这两个mac都是0  
		static u_char srcMac[6] = { 0 }, desMac[6] = { 0 };

		//smtp是基于tcp的  目前主流的已经没有用plain形式验证了 基本都是base64的编码  这里直接进行base64解码
		//note： 虽然smtp发展到现在主流用的是base64进行传输  但是base64编码是公开的  捕获到内容进行解码  与明文无异
		unsigned offset = 14 + 4 * (data[14] & 0xf) + 20;

		//smtp的包很散，可能一次只有一个字符，这里收集多个包进行组合
		static int flag = 0; //0;没有进行用户名或者密码的捕捉  1：正在捕捉用户名  2：正在捕捉密码
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
			//str 非空 而且来自于服务器 就结束上一次输入
			std::string tmp = "";
			if (flag && MyBase64::Base64Decode(value, &tmp)){
				if (flag == 1)
					up->user = tmp;
				else if (flag == 2)
					up->password = tmp;
			}
			value = "";

			//捕捉特定的输入标记
			//构造正则 334 状态码表示登录客户端输入 后面跟着的是等待输入的字段名
			std::string pattern = "\\s*334\\s+([^\\s\\r\\n]*)";
			std::smatch sm;
			//判断服务器的指令是不是要求输入用户名或者密码
			if (std::regex_search(str, sm, std::regex(pattern))){
				auto it = sm.begin();
				std::string fieldname = *(it + 1);
				std::string output = "";
				if (MyBase64::Base64Decode(fieldname, &output)){
					std::transform(output.begin(), output.end(), output.begin(), ::tolower);
					//让输入用户名或者密码
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
		//处理其他协议...
	}
	return true;
}


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
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


// Cmcf6Dlg 对话框




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


// Cmcf6Dlg 消息处理程序

BOOL Cmcf6Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_NORMAL);

	// TODO: 在此添加额外的初始化代码
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listCtrl.InsertColumn(0, _T("编号"), 3, 30);                        //1表示右，2表示中，3表示左
	m_listCtrl.InsertColumn(1, _T("时间"), 3, 130);
	m_listCtrl.InsertColumn(2, _T("长度"), 3, 72);
	m_listCtrl.InsertColumn(3, _T("源MAC地址"), 3, 140);
	m_listCtrl.InsertColumn(4, _T("目的MAC地址"), 3, 140);
	m_listCtrl.InsertColumn(5, _T("协议"), 3, 70);
	m_listCtrl.InsertColumn(6, _T("源IP地址"), 3, 145);
	m_listCtrl.InsertColumn(7, _T("目的IP地址"), 3, 145);
	m_listCtrl.InsertColumn(8, _T("用户名"), 3, 130);
	m_listCtrl.InsertColumn(9, _T("密码"), 3, 130);

	m_comboBox.AddString(_T("请选择一个网卡接口(必选)"));
	m_comboBoxRule.AddString(_T("请选择过滤规则(可选)"));

	if (lixsniff_initCap()<0)
		return FALSE;

	/*初始化接口列表*/
	for (dev = alldev; dev; dev = dev->next)
	{
		if (dev->description)
			m_comboBox.AddString(CString(dev->description));  //////////////////////////////Problem 1字符集问题
	}

	/*初始化过滤规则列表*/
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cmcf6Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cmcf6Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////［事件函数］///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//开始按钮
void Cmcf6Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//如果已经有数据了，提示保存数据
	if (!this->m_dataSaved && this->m_localDataList.IsEmpty() == FALSE)
	{
		if (MessageBox(_T("确认不保存数据？"), _T("警告"), MB_YESNO) == IDNO)
		{
			this->lixsniff_saveFile();
		}
	}

	this->npkt = 1;													//重新计数
	this->m_localDataList.RemoveAll();				//每次一开始就将以前存的数据清空掉
	this->m_netDataList.RemoveAll();
	this->m_upDataList.RemoveAll(); //清空用户名密码结构链表
	memset(&(this->npacket), 0, sizeof(struct pktcount));
	this->lixsniff_updateNPacket();

	//重置文件指针
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

//结束按钮
void Cmcf6Dlg::OnBnClickedButton2()
{
	//文件内容刷到磁盘
	pcap_dump_flush(this->dumpfile);
	pcap_dump_close(this->dumpfile);
	this->upstream->close();
	delete this->upstream;

	if (NULL == this->m_ThreadHandle)
		return;
	if (TerminateThread(this->m_ThreadHandle, -1) == 0)
	{
		MessageBox(_T("关闭线程错误，请稍后重试"));
		return;
	}
	this->m_ThreadHandle = NULL;
	this->m_buttonStart.EnableWindow(TRUE);
	this->m_buttonStop.EnableWindow(FALSE);
	this->m_buttonSave.EnableWindow(TRUE);
}

//列表
void Cmcf6Dlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int index;
	index = this->m_listCtrl.GetHotItem();

	if (index>this->m_localDataList.GetCount() - 1)
		return;

	this->lixsniff_updateEdit(index);
	this->lixsniff_updateTree(index);
	*pResult = 0;
}

//保存按钮
void Cmcf6Dlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	if (this->lixsniff_saveFile()<0)
		return;
}

//读取按钮
void Cmcf6Dlg::OnBnClickedButton4()
{
	//打开文件对话框
	CFileDialog   FileDlg(TRUE, _T(".lix"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	FileDlg.m_ofn.lpstrInitialDir = _T("c:\\");
	if (FileDlg.DoModal() == IDOK)
	{
		//读取之前将ListCtrl清空
		this->m_listCtrl.DeleteAllItems();
		this->npkt = 1;													//列表重新计数
		this->m_localDataList.RemoveAll();				//每次一开始就将以前存的数据清空掉
		this->m_netDataList.RemoveAll();
		this->m_upDataList.RemoveAll();
		memset(&(this->npacket), 0, sizeof(struct pktcount));//各类包计数清空

		int ret = this->lixsniff_readFile(FileDlg.GetPathName());
		if (ret < 0)
			return;
	}
}

//改变ListCtrl每行颜色
void Cmcf6Dlg::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMLVCUSTOMDRAW pNMCD = (LPNMLVCUSTOMDRAW)pNMHDR;
	*pResult = 0;
	// TODO: 在此添加控件通知处理程序代码
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
//////////////////////////////////////////［功能函数］///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//初始化winpcap
int Cmcf6Dlg::lixsniff_initCap()
{
	devCount = 0;
	if (pcap_findalldevs(&alldev, errbuf) == -1)
		return -1;
	for (dev = alldev; dev; dev = dev->next)
		devCount++;
	return 0;
}

//开始捕获
int Cmcf6Dlg::lixsniff_startCap()
{
	int if_index, filter_index, count;
	u_int netmask;
	struct bpf_program fcode;
	char upfilename[MAX_PATH] = { 0 };

	lixsniff_initCap();

	//获得接口和过滤器索引
	if_index = this->m_comboBox.GetCurSel();
	filter_index = this->m_comboBoxRule.GetCurSel();

	if (0 == if_index || CB_ERR == if_index)
	{
		MessageBox(_T("请选择一个合适的网卡接口"));
		return -1;
	}
	if (CB_ERR == filter_index)
	{
		MessageBox(_T("过滤器选择错误"));
		return -1;
	}

	/*获得选中的网卡接口*/
	dev = alldev;
	for (count = 0; count<if_index - 1; count++)
		dev = dev->next;

	if ((adhandle = pcap_open_live(dev->name,	// 设备名
		65536,											//捕获数据包长度																					
		1,													// 混杂模式 (非0意味着是混杂模式)
		1000,												// 读超时设置
		errbuf											// 错误信息
		)) == NULL)
	{
		MessageBox(_T("无法打开接口：" + CString(dev->description)));
		pcap_freealldevs(alldev);
		return -1;
	}

	/*检查是否为以太网*/
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		MessageBox(_T("这不适合于非以太网的网络!"));
		pcap_freealldevs(alldev);
		return -1;
	}

	if (dev->addresses != NULL)
		netmask = ((struct sockaddr_in *)(dev->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		netmask = 0xffffff;

	//编译过滤器
	if (0 == filter_index)
	{
		char filter[] = "";
		if (pcap_compile(adhandle, &fcode, filter, 1, netmask) <0)
		{
			MessageBox(_T("语法错误，无法编译过滤器"));
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
		//如果是http 弹出输出表单
		if ("http" == str){
			InputForm inf;
			if (IDOK != inf.DoModal()){
				return -1;
			}
		}

		//添加ftp http snmp pop3 imap smtp过滤
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
			MessageBox(_T("语法错误，无法编译过滤器"));
			pcap_freealldevs(alldev);
			return -1;
		}
	}


	//设置过滤器
	if (pcap_setfilter(adhandle, &fcode)<0)
	{
		MessageBox(_T("设置过滤器错误"));
		pcap_freealldevs(alldev);
		return -1;
	}

	/* 设置数据包存储路径*/
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
		MessageBox(_T("文件创建错误！"));
		return -1;
	}

	//开一个用户名密码保存文件
	strcat(upfilepath, upfilename);
	std::ofstream * upstream = new std::ofstream(upfilepath, std::ios::binary);
	if (!upstream){
		MessageBox(_T("文件创建错误！"));
		return -1;
	}
	this->upstream = upstream;

	pcap_freealldevs(alldev);

	/*接收数据，新建线程处理*/
	LPDWORD threadCap = NULL;
	m_ThreadHandle = CreateThread(NULL, 0, lixsinff_CapThread, this, 0, threadCap);
	if (m_ThreadHandle == NULL)
	{
		int code = GetLastError();
		CString str;
		str.Format(_T("创建线程错误，代码为%d."), code);
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
	struct pcap_pkthdr *header;									  //数据包头
	const u_char *pkt_data = NULL, *pData = NULL;     //网络中收到的字节流数据
	u_char *ppkt_data;

	Cmcf6Dlg *pthis = (Cmcf6Dlg*)lpParameter;
	if (NULL == pthis->m_ThreadHandle)
	{
		MessageBox(NULL, _T("线程句柄错误"), _T("提示"), MB_OK);
		return -1;
	}

	while ((res = pcap_next_ex(pthis->adhandle, &header, &pkt_data)) >= 0)
	{
		if (res == 0)				//超时
			continue;

		struct datapkt *data = (struct datapkt*)malloc(sizeof(struct datapkt));
		memset(data, 0, sizeof(struct datapkt));

		if (NULL == data)
		{
			MessageBox(NULL, _T("空间已满，无法接收新的数据包"), _T("Error"), MB_OK);
			return -1;
		}

		//分析出错或所接收数据包不在处理范围内
		if (analyze_frame(pkt_data, data, &(pthis->npacket))<0)
			continue;

		//将数据包保存到打开的文件中
		if (pthis->dumpfile != NULL)
		{
			pcap_dump((unsigned char*)pthis->dumpfile, header, pkt_data);
		}


		//将本地化后的数据装入一个链表中，以便后来使用		
		ppkt_data = (u_char*)malloc(header->len);
		memcpy(ppkt_data, pkt_data, header->len);

		pthis->m_localDataList.AddTail(data);
		pthis->m_netDataList.AddTail(ppkt_data);

		//用来保存用户名和密码
		PUPDATA up = new UPDATA;


		//过滤用户名与密码
		pthis->filterUserAndPass(up, pkt_data, header->len);

		//将用户名密码结构存入链表
		pthis->m_upDataList.AddTail(up);

		//将用户名与密码存入文件

		if (nullptr != pthis->upstream){
			saveUserAndPass(up, *(pthis->upstream));
		}


		/*预处理，获得时间、长度*/
		data->len = header->len;								//链路中收到的数据长度
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		data->time[0] = ltime->tm_year + 1900;
		data->time[1] = ltime->tm_mon + 1;
		data->time[2] = ltime->tm_mday;
		data->time[3] = ltime->tm_hour;
		data->time[4] = ltime->tm_min;
		data->time[5] = ltime->tm_sec;

		/*为新接收到的数据包在listControl中新建一个item*/
		buf.Format(_T("%d"), pthis->npkt);
		nItem = pthis->m_listCtrl.InsertItem(pthis->npkt, buf);

		/*显示时间戳*/
		timestr.Format(_T("%d/%d/%d  %d:%d:%d"), data->time[0],
			data->time[1], data->time[2], data->time[3], data->time[4], data->time[5]);
		pthis->m_listCtrl.SetItemText(nItem, 1, timestr);
		//pthis->m_listCtrl.setitem

		/*显示长度*/
		buf.Empty();
		buf.Format(_T("%d"), data->len);
		pthis->m_listCtrl.SetItemText(nItem, 2, buf);

		/*显示源MAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->src[0], data->ethh->src[1],
			data->ethh->src[2], data->ethh->src[3], data->ethh->src[4], data->ethh->src[5]);
		pthis->m_listCtrl.SetItemText(nItem, 3, buf);

		/*显示目的MAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->dest[0], data->ethh->dest[1],
			data->ethh->dest[2], data->ethh->dest[3], data->ethh->dest[4], data->ethh->dest[5]);
		pthis->m_listCtrl.SetItemText(nItem, 4, buf);

		/*获得协议*/
		pthis->m_listCtrl.SetItemText(nItem, 5, CString(data->pktType));

		/*获得源IP*/
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

		/*获得目的IP*/
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

		//显示用户名
		buf.Empty();
		for (unsigned i = 0; i < up->user.length(); ++i)
			buf += up->user[i];
		pthis->m_listCtrl.SetItemText(nItem, 8, buf);

		//显示密码
		buf.Empty();
		for (unsigned i = 0; i < up->password.length(); ++i)
			buf += up->password[i];
		pthis->m_listCtrl.SetItemText(nItem, 9, buf);

		/*对包计数*/
		pthis->npkt++;

	}
	return 1;
}

//更新信息
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

//更新统计数据
int Cmcf6Dlg::lixsniff_updateNPacket()
{
	return 1;
}

//更新树形控件
int Cmcf6Dlg::lixsniff_updateTree(int index)
{
	POSITION localpos;
	CString str;
	int i;

	this->m_treeCtrl.DeleteAllItems();

	localpos = this->m_localDataList.FindIndex(index);
	struct datapkt* local_data = (struct datapkt*)(this->m_localDataList.GetAt(localpos));

	HTREEITEM root = this->m_treeCtrl.GetRootItem();
	str.Format(_T("接收到的第%d个数据包"), index + 1);
	HTREEITEM data = this->m_treeCtrl.InsertItem(str, root);

	/*处理帧数据*/
	HTREEITEM frame = this->m_treeCtrl.InsertItem(_T("链路层数据"), data);
	//源MAC
	str.Format(_T("源MAC："));
	for (i = 0; i<6; i++)
	{
		if (i <= 4)
			str.AppendFormat(_T("%02x-"), local_data->ethh->src[i]);
		else
			str.AppendFormat(_T("%02x"), local_data->ethh->src[i]);
	}
	this->m_treeCtrl.InsertItem(str, frame);
	//目的MAC
	str.Format(_T("目的MAC："));
	for (i = 0; i<6; i++)
	{
		if (i <= 4)
			str.AppendFormat(_T("%02x-"), local_data->ethh->dest[i]);
		else
			str.AppendFormat(_T("%02x"), local_data->ethh->dest[i]);
	}
	this->m_treeCtrl.InsertItem(str, frame);
	//类型
	str.Format(_T("类型：0x%02x"), local_data->ethh->type);
	this->m_treeCtrl.InsertItem(str, frame);

	/*处理IP、ARP、IPv6数据包*/
	if (0x0806 == local_data->ethh->type)							//ARP
	{
		HTREEITEM arp = this->m_treeCtrl.InsertItem(_T("ARP协议头"), data);
		str.Format(_T("硬件类型：%d"), local_data->arph->ar_hrd);
		this->m_treeCtrl.InsertItem(str, arp);
		str.Format(_T("协议类型：0x%02x"), local_data->arph->ar_pro);
		this->m_treeCtrl.InsertItem(str, arp);
		str.Format(_T("硬件地址长度：%d"), local_data->arph->ar_hln);
		this->m_treeCtrl.InsertItem(str, arp);
		str.Format(_T("协议地址长度：%d"), local_data->arph->ar_pln);
		this->m_treeCtrl.InsertItem(str, arp);
		str.Format(_T("操作码：%d"), local_data->arph->ar_op);
		this->m_treeCtrl.InsertItem(str, arp);

		str.Format(_T("发送方MAC："));
		for (i = 0; i<6; i++)
		{
			if (i <= 4)
				str.AppendFormat(_T("%02x-"), local_data->arph->ar_srcmac[i]);
			else
				str.AppendFormat(_T("%02x"), local_data->arph->ar_srcmac[i]);
		}
		this->m_treeCtrl.InsertItem(str, arp);

		str.Format(_T("发送方IP："), local_data->arph->ar_hln);
		for (i = 0; i<4; i++)
		{
			if (i <= 2)
				str.AppendFormat(_T("%d."), local_data->arph->ar_srcip[i]);
			else
				str.AppendFormat(_T("%d"), local_data->arph->ar_srcip[i]);
		}
		this->m_treeCtrl.InsertItem(str, arp);

		str.Format(_T("接收方MAC："), local_data->arph->ar_hln);
		for (i = 0; i<6; i++)
		{
			if (i <= 4)
				str.AppendFormat(_T("%02x-"), local_data->arph->ar_destmac[i]);
			else
				str.AppendFormat(_T("%02x"), local_data->arph->ar_destmac[i]);
		}
		this->m_treeCtrl.InsertItem(str, arp);

		str.Format(_T("接收方IP："), local_data->arph->ar_hln);
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

		HTREEITEM ip = this->m_treeCtrl.InsertItem(_T("IP协议头"), data);

		str.Format(_T("版本：%d"), local_data->iph->version);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("IP头长：%d"), local_data->iph->ihl);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("服务类型：%d"), local_data->iph->tos);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("总长度：%d"), local_data->iph->tlen);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("标识：0x%02x"), local_data->iph->id);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("段偏移：%d"), local_data->iph->frag_off);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("生存期：%d"), local_data->iph->ttl);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("协议：%d"), local_data->iph->proto);
		this->m_treeCtrl.InsertItem(str, ip);
		str.Format(_T("头部校验和：0x%02x"), local_data->iph->check);
		this->m_treeCtrl.InsertItem(str, ip);

		str.Format(_T("源IP："));
		struct in_addr in;
		in.S_un.S_addr = local_data->iph->saddr;
		str.AppendFormat(CString(inet_ntoa(in)));
		this->m_treeCtrl.InsertItem(str, ip);

		str.Format(_T("目的IP："));
		in.S_un.S_addr = local_data->iph->daddr;
		str.AppendFormat(CString(inet_ntoa(in)));
		this->m_treeCtrl.InsertItem(str, ip);

		/*处理传输层ICMP、UDP、TCP*/
		if (1 == local_data->iph->proto)							//ICMP
		{
			HTREEITEM icmp = this->m_treeCtrl.InsertItem(_T("ICMP协议头"), data);

			str.Format(_T("类型:%d"), local_data->icmph->type);
			this->m_treeCtrl.InsertItem(str, icmp);
			str.Format(_T("代码:%d"), local_data->icmph->code);
			this->m_treeCtrl.InsertItem(str, icmp);
			str.Format(_T("序号:%d"), local_data->icmph->seq);
			this->m_treeCtrl.InsertItem(str, icmp);
			str.Format(_T("校验和:%d"), local_data->icmph->chksum);
			this->m_treeCtrl.InsertItem(str, icmp);

		}
		else if (6 == local_data->iph->proto){				//TCP

			HTREEITEM tcp = this->m_treeCtrl.InsertItem(_T("TCP协议头"), data);

			str.Format(_T("  源端口:%d"), local_data->tcph->sport);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  目的端口:%d"), local_data->tcph->dport);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  序列号:0x%02x"), local_data->tcph->seq);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  确认号:%d"), local_data->tcph->ack_seq);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  头部长度:%d"), local_data->tcph->doff);

			HTREEITEM flag = this->m_treeCtrl.InsertItem(_T(" +标志位"), tcp);

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

			str.Format(_T("  紧急指针:%d"), local_data->tcph->urg_ptr);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  校验和:0x%02x"), local_data->tcph->check);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  选项:%d"), local_data->tcph->opt);
			this->m_treeCtrl.InsertItem(str, tcp);
		}
		else if (17 == local_data->iph->proto){				//UDP
			HTREEITEM udp = this->m_treeCtrl.InsertItem(_T("UDP协议头"), data);

			str.Format(_T("源端口:%d"), local_data->udph->sport);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("目的端口:%d"), local_data->udph->dport);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("总长度:%d"), local_data->udph->len);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("校验和:0x%02x"), local_data->udph->check);
			this->m_treeCtrl.InsertItem(str, udp);
		}
	}
	else if (0x86dd == local_data->ethh->type){		//IPv6
		HTREEITEM ip6 = this->m_treeCtrl.InsertItem(_T("IPv6协议头"), data);

		//////////////////////////////////////////////////////////////////////////////////////////
		str.Format(_T("版本:%d"), local_data->iph6->flowtype);
		this->m_treeCtrl.InsertItem(str, ip6);
		str.Format(_T("流类型:%d"), local_data->iph6->version);
		this->m_treeCtrl.InsertItem(str, ip6);
		///////////////////////////////////////////////////////////////////////////////////////////
		str.Format(_T("流标签:%d"), local_data->iph6->flowid);
		this->m_treeCtrl.InsertItem(str, ip6);
		str.Format(_T("有效载荷长度:%d"), local_data->iph6->plen);
		this->m_treeCtrl.InsertItem(str, ip6);
		str.Format(_T("下一个首部:0x%02x"), local_data->iph6->nh);
		this->m_treeCtrl.InsertItem(str, ip6);
		str.Format(_T("跳限制:%d"), local_data->iph6->hlim);
		this->m_treeCtrl.InsertItem(str, ip6);

		str.Format(_T("源地址:"));
		int n;
		for (n = 0; n<8; n++)
		{
			if (n <= 6)
				str.AppendFormat(_T("%02x:"), local_data->iph6->saddr[n]);
			else
				str.AppendFormat(_T("%02x"), local_data->iph6->saddr[n]);
		}
		this->m_treeCtrl.InsertItem(str, ip6);

		str.Format(_T("目的地址:"));
		for (n = 0; n<8; n++)
		{
			if (n <= 6)
				str.AppendFormat(_T("%02x:"), local_data->iph6->saddr[n]);
			else
				str.AppendFormat(_T("%02x"), local_data->iph6->saddr[n]);
		}
		this->m_treeCtrl.InsertItem(str, ip6);

		/*处理传输层ICMPv6、UDP、TCP*/
		if (0x3a == local_data->iph6->nh)							//ICMPv6
		{
			HTREEITEM icmp6 = this->m_treeCtrl.InsertItem(_T("ICMPv6协议头"), data);

			str.Format(_T("类型:%d"), local_data->icmph6->type);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("代码:%d"), local_data->icmph6->code);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("序号:%d"), local_data->icmph6->seq);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("校验和:%d"), local_data->icmph6->chksum);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("选项-类型:%d"), local_data->icmph6->op_type);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("选项-长度%d"), local_data->icmph6->op_len);
			this->m_treeCtrl.InsertItem(str, icmp6);
			str.Format(_T("选项-链路层地址:"));
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

			HTREEITEM tcp = this->m_treeCtrl.InsertItem(_T("TCP协议头"), data);

			str.Format(_T("  源端口:%d"), local_data->tcph->sport);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  目的端口:%d"), local_data->tcph->dport);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  序列号:0x%02x"), local_data->tcph->seq);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  确认号:%d"), local_data->tcph->ack_seq);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  头部长度:%d"), local_data->tcph->doff);

			HTREEITEM flag = this->m_treeCtrl.InsertItem(_T("标志位"), tcp);

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

			str.Format(_T("  紧急指针:%d"), local_data->tcph->urg_ptr);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  校验和:0x%02x"), local_data->tcph->check);
			this->m_treeCtrl.InsertItem(str, tcp);
			str.Format(_T("  选项:%d"), local_data->tcph->opt);
			this->m_treeCtrl.InsertItem(str, tcp);
		}
		else if (0x11 == local_data->iph6->nh){				//UDP
			HTREEITEM udp = this->m_treeCtrl.InsertItem(_T("UDP协议头"), data);

			str.Format(_T("源端口:%d"), local_data->udph->sport);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("目的端口:%d"), local_data->udph->dport);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("总长度:%d"), local_data->udph->len);
			this->m_treeCtrl.InsertItem(str, udp);
			str.Format(_T("校验和:0x%02x"), local_data->udph->check);
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
		MessageBox(_T("保存文件遇到未知意外"));
		return -1;
	}

	//打开文件对话框
	CFileDialog   FileDlg(FALSE, _T("lix"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	FileDlg.m_ofn.lpstrInitialDir = _T("c:\\");
	if (FileDlg.DoModal() == IDOK)
	{
		CopyFile(CString(this->filepath), FileDlg.GetPathName(), FALSE);

		//获取相对应的保存用户名密码文件的路径
		CString filepath = FileDlg.GetPathName();
		char * charpath = (char*)malloc(filepath.GetLength() + 1);
		ZeroMemory(charpath, filepath.GetLength() + 1);
		for (unsigned i = 0; i < filepath.GetLength(); ++i){
			charpath[i] = filepath[i];
		}
		char* lastDot = strrchr(charpath, '.');
		unsigned upPathLen = lastDot - charpath;
		const char * upSuffix = ".up";  //用户密码键值对保存的文件名后缀
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
	struct pcap_pkthdr *header;									  //数据包头
	const u_char *pkt_data = NULL;     //网络中收到的字节流数据
	u_char *ppkt_data;

	Cmcf6Dlg *pthis = this;						//些代码改造自lixsinff_CapThread，为节约工作量，故保留pthis指针
	pcap_t *fp;

	//路径需要用char *类型，不能用CString强制转换后的char *
	int len = path.GetLength() + 1;
	char* charpath = (char *)malloc(len);
	memset(charpath, 0, len);
	if (NULL == charpath)
		return -1;

	for (i = 0; i<len; i++)
		charpath[i] = (char)path.GetAt(i);

	//打开相关文件
	if ((fp = pcap_open_offline(charpath, errbuf)) == NULL)
	{
		MessageBox(_T("打开文件错误") + CString(errbuf));
		return -1;
	}

	char* lastDot = strrchr(charpath, '.');
	unsigned upPathLen = lastDot - charpath;
	const char * upSuffix = ".up";  //用户密码键值对保存的文件名后缀
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
		MessageBox(_T("打开文件错误") + CString(errbuf));
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
			MessageBox(_T("空间已满，无法接收新的数据包"));
			return  -1;
		}

		//分析出错或所接收数据包不在处理范围内
		if (analyze_frame(pkt_data, data, &(pthis->npacket))<0)
			continue;

		//更新各类数据包计数
		pthis->lixsniff_updateNPacket();

		//将本地化后的数据装入一个链表中，以便后来使用		
		ppkt_data = (u_char*)malloc(header->len);
		memcpy(ppkt_data, pkt_data, header->len);

		pthis->m_localDataList.AddTail(data);
		pthis->m_netDataList.AddTail(ppkt_data);

		//解析出保存的用户名密码
		PUPDATA up = new UPDATA;
		readUserPassword(up, upstream);

		/*预处理，获得时间、长度*/
		data->len = header->len;								//链路中收到的数据长度
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		data->time[0] = ltime->tm_year + 1900;
		data->time[1] = ltime->tm_mon + 1;
		data->time[2] = ltime->tm_mday;
		data->time[3] = ltime->tm_hour;
		data->time[4] = ltime->tm_min;
		data->time[5] = ltime->tm_sec;

		/*为新接收到的数据包在listControl中新建一个item*/
		buf.Format(_T("%d"), pthis->npkt);
		nItem = pthis->m_listCtrl.InsertItem(pthis->npkt, buf);

		/*显示时间戳*/
		timestr.Format(_T("%d/%d/%d  %d:%d:%d"), data->time[0],
			data->time[1], data->time[2], data->time[3], data->time[4], data->time[5]);
		pthis->m_listCtrl.SetItemText(nItem, 1, timestr);

		/*显示长度*/
		buf.Empty();
		buf.Format(_T("%d"), data->len);
		pthis->m_listCtrl.SetItemText(nItem, 2, buf);

		/*显示源MAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->src[0], data->ethh->src[1],
			data->ethh->src[2], data->ethh->src[3], data->ethh->src[4], data->ethh->src[5]);
		pthis->m_listCtrl.SetItemText(nItem, 3, buf);

		/*显示目的MAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->dest[0], data->ethh->dest[1],
			data->ethh->dest[2], data->ethh->dest[3], data->ethh->dest[4], data->ethh->dest[5]);
		pthis->m_listCtrl.SetItemText(nItem, 4, buf);

		/*获得协议*/
		pthis->m_listCtrl.SetItemText(nItem, 5, CString(data->pktType));

		/*获得源IP*/
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

		/*获得目的IP*/
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

		//显示用户名
		buf.Empty();
		for (unsigned i = 0; i < up->user.length(); ++i)
			buf += up->user[i];
		pthis->m_listCtrl.SetItemText(nItem, 8, buf);

		//显示密码
		buf.Empty();
		for (unsigned i = 0; i < up->password.length(); ++i)
			buf += up->password[i];
		pthis->m_listCtrl.SetItemText(nItem, 9, buf);

		/*对包计数*/
		pthis->npkt++;
	}

	pcap_close(fp);
	upstream.close();

	return 1;
}