
// FTP_ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FTP_Client.h"
#include "FTP_ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma warning(disable : 4996)

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFTP_ClientDlg �Ի���



CFTP_ClientDlg::CFTP_ClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FTP_CLIENT_DIALOG, pParent)
	, hostPort(_T("20"))//�ͻ��˰󶨶˿ں�Ĭ��Ϊ20
	, serverPort(_T("200"))//����˰󶨶˿ں�Ĭ��Ϊ200
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	logined = false;
}

void CFTP_ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, hostIPCtrl);
	DDX_Control(pDX, IDC_EDIT8, logCtrl);
	DDX_Control(pDX, IDC_EDIT2, hostPortCtrl);
	DDX_Control(pDX, IDC_EDIT3, ServerPortCtrl);
	DDX_Control(pDX, IDC_IPADDRESS1, ServerIPCtrl);
	DDX_Control(pDX, IDC_EDIT5, userNameCtrl);
	DDX_Control(pDX, IDC_EDIT6, passwordCtrl);
	DDX_Control(pDX, IDC_LIST1, FileTreeDisplayCtrl);
	DDX_Control(pDX, IDC_EDIT10, currentPathCtrl);
	DDX_Control(pDX, IDC_EDIT11, commandCtrl);
	DDX_Control(pDX, IDC_EDIT12, argvCtrl);
	DDX_Text(pDX, IDC_EDIT2, hostPort);
	DDX_Text(pDX, IDC_EDIT3, serverPort);
}

BEGIN_MESSAGE_MAP(CFTP_ClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFTP_ClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_StartClient, &CFTP_ClientDlg::OnBnClickedStartclient)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ConnectServer, &CFTP_ClientDlg::OnBnClickedConnectserver)
	ON_BN_CLICKED(IDC_login, &CFTP_ClientDlg::OnBnClickedlogin)
	ON_BN_CLICKED(IDC_BUTTON4, &CFTP_ClientDlg::OnBnClickedButton4)
	ON_LBN_DBLCLK(IDC_LIST1, &CFTP_ClientDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON5, &CFTP_ClientDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CFTP_ClientDlg ��Ϣ�������

BOOL CFTP_ClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//��ʼ������IP
	hostIPCtrl.SetWindowTextA(getHostIPStr());

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFTP_ClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFTP_ClientDlg::OnPaint()
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
HCURSOR CFTP_ClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//��ȡ�����ļ�·����ť��Ӧ����
void CFTP_ClientDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//����Դ�ļ�����������ȡ��������Դ�ļ�����λ��
	CString strPath = ("");
	CFileDialog Open(TRUE, ("*.bmp"), NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		("Files (*.txt)|*.txt|JPEG Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"),
		NULL); //��_T()�У��������޸ģ����Զ�����Ҫ�򿪵��ļ����͸�ʽ  
	if (Open.DoModal() == IDOK)
	{
		strPath = Open.GetPathName();//����ļ���ȫ·��  
	}
	//MessageBox(strPath);
}

//�����ͻ��ˣ���ʼ���ͻ��˻�����Ӧ����
void CFTP_ClientDlg::OnBnClickedStartclient()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//��ȡ����Ŀͻ��˰󶨶˿�
	hostPortCtrl.GetWindowTextA(hostPort);
	if (hostPort == "")
	{
		MessageBox("�˿ڲ���Ϊ�գ�");
		return;
	}
	UpdateData(true);
	UINT port = atoi(hostPort);
	if (port > 65535 || port < 0)
	{
		MessageBox("�˿����ô���");
		return;
	}

	//���������Ŀͻ������ӿ����׽���
	CNSocket = new ConnectSocket;
	bool flag = CNSocket->Create(port, SOCK_DGRAM, FD_READ);
	if (!flag)
	{
		log("�׽��ִ���ʧ�ܣ�");
	}
	else
	{
		CString logText;
		logText.Format("����UDP�׽��֣������˿�%d", port);
		log(logText);
	}
	//�����׽�����ϣ��������ͻ��˰�ť����Ϊ������״̬
	((CButton*)GetDlgItem(IDC_StartClient))->EnableWindow(false);
}
//��ʾ�ļ�Ŀ¼��
void CFTP_ClientDlg::displayFileTree(CString files)
{
	//����������
	while (FileTreeDisplayCtrl.GetCount())
	{
		FileTreeDisplayCtrl.DeleteString(0);
	}
	currentPathCtrl.SetWindowTextA("");
	commandCtrl.SetWindowTextA("");
	argvCtrl.SetWindowTextA("");
	CNSocket->files_list.RemoveAll();

	int pos = 0;
	while (pos < files.GetLength())
	{
		int nextpos = files.Find("\r\n", pos);
		CString file;
		file = files.Mid(pos, nextpos - pos);
		pos = nextpos + 2;
		FileTreeDisplayCtrl.InsertString(FileTreeDisplayCtrl.GetCount(), file);
	}
}

//��־��ʾ����
void CFTP_ClientDlg::log(CString logInfo)
{
	CString alreadyLog, date, time;
	logCtrl.GetWindowTextA(alreadyLog);
	getDateTimeStr(date, time);
	logCtrl.SetWindowTextA(date + "-" + time + " : " + logInfo + "\r\n" + alreadyLog);
}

//��ʱ����Ӧ���������ͻ��������ݳ�ʱδ��𸴣��ش�����
void CFTP_ClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (CNSocket->sendPKT_list.GetCount() == 0)
	{
		MessageBox("��ʱ������");
		return;
	}
	POSITION pos = CNSocket->sendPKT_list.GetHeadPosition();
	while (pos != NULL)
	{
		if (CNSocket->sendPKT_list.GetAt(pos)->Timer == nIDEvent)
		{
			if (CNSocket->sendPKT_list.GetAt(pos)->ResendTime >= RESENDTIMELIMIT)		//�ش������������ޣ�����ɾ���ش�����
			{
				CString logText;
				logText.Format("�ش��������ޣ�ɾ�����ģ�SEQ:", ((FTPHeader_t*)(CNSocket->sendPKT_list.GetAt(pos)->PktData))->SEQNO);
				log(logText);
				CNSocket->backTimerID(nIDEvent);
				CNSocket->sendPKT_list.RemoveAt(pos);
				break;
			}
			CNSocket->SendTo(CNSocket->sendPKT_list.GetAt(pos)->PktData,
				CNSocket->sendPKT_list.GetAt(pos)->len,
				CNSocket->sendPKT_list.GetAt(pos)->TargetPort,
				CNSocket->sendPKT_list.GetAt(pos)->TargetIP);
			CNSocket->sendPKT_list.GetAt(pos)->ResendTime++;		//����������1
			CString logText;
			logText.Format("��ʱ�ش���SEQ:%d", ((FTPHeader_t*)(CNSocket->sendPKT_list.GetAt(pos)->PktData))->SEQNO);
			log(logText);
			break;
		}
		CNSocket->sendPKT_list.GetNext(pos);
	}
	CDialogEx::OnTimer(nIDEvent);
}

//���ӷ���������ȡ������IP���Ͷ˿ڣ����䷢����������
void CFTP_ClientDlg::OnBnClickedConnectserver()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ServerIPCtrl.GetWindowTextA(CNSocket->Server_IP);
	//MessageBox(CNSocket->Server_IP);
	CString Sport;
	ServerPortCtrl.GetWindowTextA(Sport);
	CNSocket->Server_Port = (UINT)(atoi(Sport));
	UpdateData(true);
	//����һ���������ݰ��ṹ��
	u_char res[256] = "HELO this is LI WEI!";
	int res_len = strlen((char*)(res));
	//���·������к�
	CNSocket->Seq += res_len;
	//��������
	sendFTPPacket(CNSocket, CNSocket->Server_IP, CNSocket->Server_Port, res, res_len, CNSocket->userID, 0, CNSocket->Seq, CNSocket->Ack, SYN);
	CNSocket->status = isConnecting;
	//��־��¼ 
	CString logText;
	logText.Format("Send '%s' to IP:%s Port:%d", res, CNSocket->Server_IP, CNSocket->Server_Port);
	log(logText);
	//���ӷ������ɹ����������ͻ��˰�ť����Ϊ������״̬
	//((CButton*)GetDlgItem(IDC_StartClient))->EnableWindow(false);
}


//��¼ע��
void CFTP_ClientDlg::OnBnClickedlogin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (CNSocket->status != isCommunicating)
	{
		MessageBox("�������ӷ�������");
		return;
	}
	CString username;
	userNameCtrl.GetWindowTextA(username);
	
	CString pass;
	passwordCtrl.GetWindowTextA(pass);

	//����һ���������ݰ��ṹ��
	u_char res[128] = "LOGI ";
	strcat((char*)res, username+" "+pass+"\r\n");
	int res_len = strlen((const char*)res);
	//���·������к�
	CNSocket->Seq += res_len;
	//��������
	sendFTPPacket(CNSocket, CNSocket->Server_IP, CNSocket->Server_Port,(u_char*)(&res), res_len, CNSocket->userID, 0, CNSocket->Seq, CNSocket->Ack, ACK);
}

//���������
void CFTP_ClientDlg::OnBnClickedButton4()
{
	if (logined == false)
	{
		MessageBox("���ȵ�¼��������");
		return;
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString command;
	commandCtrl.GetWindowTextA(command);
	if (command == "")
	{
		MessageBox("�������벻��Ϊ�գ�");
		return;
	}
	else if (command == "RETR")
	{
		CNSocket->status = isTransfer;
	}
	CString argvs;
	argvCtrl.GetWindowTextA(argvs);

	//����һ���������ݰ��ṹ��
	u_char res[128] = "";
	strcat((char*)res,command+" "+argvs + "\r\n");
	int res_len = strlen((const char*)res);

	//���·������к�
	CNSocket->Seq += res_len;
	//��������
	sendFTPPacket(CNSocket, CNSocket->Server_IP, CNSocket->Server_Port, (u_char*)(&res), res_len, CNSocket->userID, 0, CNSocket->Seq, CNSocket->Ack, ACK);
}

//ѡ�������ļ�����׼�����ص��ļ���������б�ͬʱ����ȥ�ع���
void CFTP_ClientDlg::OnLbnDblclkList1()
{
	if (logined == false)
	{
		MessageBox("���ȵ�¼��������");
		return;
	}

	CString command;
	commandCtrl.GetWindowTextA(command);
	CString allowType;
	if (command == "LIST"||command == "MDIR")
	{
		allowType = "D";
	}
	else if (command == "RETR"||command=="STOR")
	{
		allowType = "F";
	}
	else if (command == "DELE")
	{
		allowType = "DF";
	}
	else {
		MessageBox("������д��ȷ���");
		return;
	}

	int cursel = FileTreeDisplayCtrl.GetCurSel();
	CString fileinfo;
	FileTreeDisplayCtrl.GetText(cursel, fileinfo);
	//MessageBox(fileinfo);

	CString Type = fileinfo.Left(1);
	if (allowType != "DF")
	{
		if (Type != allowType)
		{
			MessageBox("����д���֧��ѡ���ļ����ͣ�");
			return;
		}
	}

	
	int p1 = fileinfo.Find(" ", 0);
	int p2 = fileinfo.Find(" ", p1 + 1);
	CString filename = fileinfo.Mid(p1+1,p2-p1-1);
	CString length = fileinfo.Mid(p2 + 1, fileinfo.GetLength() - p2 - 1);
	//MessageBox(filename+","+length);

	POSITION pos = CNSocket->files_list.GetHeadPosition();
	while (pos != NULL)
	{
		if (CNSocket->files_list.GetAt(pos)->filename == filename)
		{
			MessageBox("�ļ����ڲ��������У������ظ���ӣ�");
			return;
		}
		CNSocket->files_list.GetNext(pos);
	}

	DownloadFile_t * d = new DownloadFile_t;
	d->DataID = CNSocket->files_list.GetCount() + 1;
	d->filename = filename;
	//d->len = atoll(length);
	d->len = 0;
	CNSocket->files_list.AddTail(d);
	CString argvs;
	argvCtrl.GetWindowTextA(argvs);
	if(argvs=="")
		argvCtrl.SetWindowTextA(filename);
	else
		argvCtrl.SetWindowTextA(argvs + "\r\n" + filename);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

//��������
void CFTP_ClientDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	commandCtrl.SetWindowTextA("");
	argvCtrl.SetWindowTextA("");
	CNSocket->files_list.RemoveAll();
}
