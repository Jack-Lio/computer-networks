
// CAsyncSocket_ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CAsyncSocket_Client.h"
#include "CAsyncSocket_ClientDlg.h"
#include "afxdialogex.h"
 
//���� inet_ntoa �������ñ���
#pragma warning(disable : 4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_MYMESSAGE  WM_USER+1

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


// CCAsyncSocket_ClientDlg �Ի���



CCAsyncSocket_ClientDlg::CCAsyncSocket_ClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CASYNCSOCKET_CLIENT_DIALOG, pParent)
	, IP_m(_T("192.168.56.1"))
	, port_m(_T("200"))
	, command_m(_T("Date"))
	, response_m(_T(""))
	, resend_count(0)
	, resend_limit(5)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_client_port = L"2000";					 //�ͻ��˶˿ں�
	char temp[256];
	gethostname(temp,256);      //��ȡ�ͻ�������
	hostent* host = gethostbyname(temp); //�ͻ�����IP
	m_host_name = temp;
	m_client_address = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);   //�ͻ���IP��ַתΪ�ַ�������
}

void CCAsyncSocket_ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, IP_ctrl);
	DDX_CBString(pDX, IDC_COMBO1, IP_m);
	DDX_CBString(pDX, IDC_COMBO3, port_m);
	DDX_CBString(pDX, IDC_COMBO2, command_m);
	DDX_Control(pDX, IDC_COMBO3, port_ctrl);
	DDX_Control(pDX, IDC_COMBO2, command_ctrl);
	DDX_Text(pDX, IDC_EDIT1, response_m);
	DDX_Text(pDX, IDC_EDIT3, m_client_address);
	DDX_Text(pDX, IDC_COMBO4, m_client_port);
	DDX_Control(pDX, IDC_COMBO4, m_client_port_ctrl);
	DDX_Control(pDX, IDC_LIST1, command_log_ctrl);
	DDX_Text(pDX, IDC_EDIT4, m_host_name);
}

BEGIN_MESSAGE_MAP(CCAsyncSocket_ClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCAsyncSocket_ClientDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CCAsyncSocket_ClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCAsyncSocket_ClientDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_MYMESSAGE, &CCAsyncSocket_ClientDlg::my_send)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//��ȡ��ǰ������ʱ�䣬���ַ�����ʽ����
CString CCAsyncSocket_ClientDlg::getDateTime()
{
	CTime tm; tm = CTime::GetCurrentTime();
	CString date, time;
	date.Format(L"%d/%d/%d", tm.GetYear(), tm.GetMonth(), tm.GetDay());
	time.Format(L"%d:%d:%d", tm.GetHour(), tm.GetMinute(), tm.GetSecond());
	return date + L" " + time;
}

// CCAsyncSocket_ClientDlg ��Ϣ�������

BOOL CCAsyncSocket_ClientDlg::OnInitDialog()
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
	BOOL bFlag = Client.Create(_ttoi(m_client_port), SOCK_DGRAM, FD_READ|FD_WRITE);   //�����ͻ����׽���
	if (!bFlag)								 //������
	{
		MessageBox(L"�ͻ���Socket������������˿�ʹ�������");
		command_log_ctrl.InsertString(0, getDateTime() + L":�ͻ���socket����ʧ��");
	}
	else
	{
		command_log_ctrl.InsertString(0, getDateTime() + L":�ͻ���socket�����ɹ� port:" + m_client_port);
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCAsyncSocket_ClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCAsyncSocket_ClientDlg::OnPaint()
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
HCURSOR CCAsyncSocket_ClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCAsyncSocket_ClientDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (Client == INVALID_SOCKET)		//�׽��ִ��ڹر�״̬���ܷ��ͱ��ģ��ṩ����
	{
		command_log_ctrl.InsertString(0, getDateTime() + L":�ͻ����Ѿ��رգ��������ͻ��˺����·���");
		return;
	}

	UpdateData(true);
	//����ʱ�ӽ��м�ʱ ��200ms�ط�һ�Σ������޸���һ���������ڳ�ʱ��ʱ������
	SetTimer(1, 200, NULL);         
	my_send();                      //����������

	//���½�����ӵ�����������¼��comb box ��
	if (IP_ctrl.FindString(0, IP_m) == CB_ERR)
	{
		IP_ctrl.AddString(IP_m);
	}
	if (command_ctrl.FindString(0, command_m)== CB_ERR)
	{
		command_ctrl.AddString(command_m);
	}
	if (port_ctrl.FindString(0, port_m) == CB_ERR)
	{
		port_ctrl.AddString(port_m);
	}
	UpdateData(false);
}


void CCAsyncSocket_ClientDlg::OnBnClickedButton1()			//���õ�ǰ��socket����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	command_log_ctrl.InsertString(0, getDateTime() + L":���Թرյ�ǰ�˿ڵĿͻ���socket");
	OnBnClickedButton2();	         //�ȵ��ùر�socket����
	UpdateData(true);
	if (m_client_port_ctrl.FindString(0, m_client_port) == CB_ERR)   //����������µĶ˿ںţ������combox��ȥ
	{
		m_client_port_ctrl.AddString(m_client_port);
	}
	command_log_ctrl.InsertString(0, getDateTime() + L":���������ͻ���socket");
	BOOL bFlag = Client.Create(_ttoi(m_client_port), SOCK_DGRAM, FD_READ | FD_WRITE);   //�����ͻ����׽���
	if (!bFlag)   //������
	{
		MessageBox(L"�ͻ���Socket������������˿�ʹ�������");
		command_log_ctrl.InsertString(0, getDateTime() + L":�ͻ���socket����ʧ��");
	}
	else
	{
		command_log_ctrl.InsertString(0, getDateTime() + L":�ͻ���socket�����ɹ� port:" + m_client_port);
	}
}


void CCAsyncSocket_ClientDlg::OnBnClickedButton2()			// �رյ���ǰ��socket
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (Client != INVALID_SOCKET)        
	{
		Client.Close();
		command_log_ctrl.InsertString(0, getDateTime() + L":�ͻ���socket�ر� port:" + m_client_port);
	}
	else
	{
		command_log_ctrl.InsertString(0, getDateTime() + L":�ͻ���socket�Ѿ��ر�" );
	}
}


// ���ͱ��ĺ���
LRESULT  CCAsyncSocket_ClientDlg::my_send(WPARAM wParam, LPARAM lParam)     //��װ�˷��ͱ��ĵĲ���
{
	int flag = Client.SendTo(command_m.GetBuffer(), command_m.GetLength()*sizeof(TCHAR), _ttoi(port_m), IP_m);   //����ֵΪ���͵��ַ����ֽڳ���
	if (flag == -1) {
		//MessageBox(L"ERROR:���ͱ��ĳ��������׽������ã�");
		command_log_ctrl.InsertString(0, getDateTime() + L"ERROR:���ͱ��ĳ��������׽������ã�");
	}
	else {
		command_log_ctrl.InsertString(0, getDateTime() + L"�� IP:" + IP_m + L" Port:" + port_m + L"��������" + command_m + L"��");
	}
	return false;
}


void CCAsyncSocket_ClientDlg::OnTimer(UINT_PTR nIDEvent)   // ��ʱ�ط�����Ҫ����ʵ�ֺ��������ڼ�ʱ��ʵ��
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (resend_count >= resend_limit)		//����һ���ط�������ֹͣ�ط����رռ�ʱ��
	{
		KillTimer(1);
		resend_count = 0;
		command_log_ctrl.InsertString(0, getDateTime() + L"ERROR:�ط��������ޣ�����ʧ�ܣ�����������ã�");
	}
	else {
		my_send();    //�ط���Ϣ
		resend_count++;   //�ط���������
		CString str; str.Format(L"�� %d ���ط� port:",resend_count);
		command_log_ctrl.InsertString(0, getDateTime() + str + port_m);
	}
	CDialogEx::OnTimer(nIDEvent);
}
