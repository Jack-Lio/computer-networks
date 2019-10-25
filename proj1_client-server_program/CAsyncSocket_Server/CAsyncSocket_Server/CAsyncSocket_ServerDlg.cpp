
// CAsyncSocket_ServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CAsyncSocket_Server.h"
#include "CAsyncSocket_ServerDlg.h"
#include "afxdialogex.h"

//���� inet_ntoa �������ñ���
#pragma warning(disable : 4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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


// CCAsyncSocket_ServerDlg �Ի���



CCAsyncSocket_ServerDlg::CCAsyncSocket_ServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CASYNCSOCKET_SERVER_DIALOG, pParent)
	, server_port(_T("200"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	char temp[256];
	gethostname(temp, 256);      //��ȡ�ͻ�������
	hostent* host = gethostbyname(temp); //�ͻ�����IP
	server_hostname = temp;
	server_address = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);   //�ͻ���IP��ַתΪ�ַ�������
}

void CCAsyncSocket_ServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, list_ctrl);
	DDX_Control(pDX, IDC_COMBO1, server_port_ctrl);
	DDX_CBString(pDX, IDC_COMBO1, server_port);
	DDX_Text(pDX, IDC_EDIT1, server_hostname);
	DDX_Text(pDX, IDC_EDIT2, server_address);
}

BEGIN_MESSAGE_MAP(CCAsyncSocket_ServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCAsyncSocket_ServerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CCAsyncSocket_ServerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCAsyncSocket_ServerDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CCAsyncSocket_ServerDlg ��Ϣ�������

BOOL CCAsyncSocket_ServerDlg::OnInitDialog()
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCAsyncSocket_ServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCAsyncSocket_ServerDlg::OnPaint()
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
//��ʾ��s
HCURSOR CCAsyncSocket_ServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//��ȡ��ǰ������ʱ�䣬���ַ�����ʽ����
CString getDateTime()
{
	CTime tm; tm = CTime::GetCurrentTime();
	CString date, time;
	date.Format(L"%d/%d/%d", tm.GetYear(), tm.GetMonth(), tm.GetDay());
	time.Format(L"%d:%d:%d", tm.GetHour(), tm.GetMinute(), tm.GetSecond());
	return date + L" " + time;
}


void CCAsyncSocket_ServerDlg::OnBnClickedOk()   // ����������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (Server != INVALID_SOCKET)
	{
		list_ctrl.InsertString(0, getDateTime() + L":������socket�Ѿ����� port:" + server_port);
	}
	else {
		int flag = Server.Create(_ttoi(server_port), SOCK_DGRAM, FD_WRITE | FD_READ);
		//Server.Bind(m_port);
		if (!flag) {
			MessageBox(L"����������ʧ�ܣ�����socket���ã�");
			list_ctrl.InsertString(0, getDateTime() + L":������socket����ʧ�� port:" + server_port);
		}
		else {
			list_ctrl.InsertString(0, getDateTime() + L":������socket�����ɹ� port:" + server_port);
		}
	}
}


void CCAsyncSocket_ServerDlg::OnBnClickedButton1()		//���÷�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	list_ctrl.InsertString(0, getDateTime() + L":���Թرշ�������ǰ�˿ڵ�socket��");  //�����־
	OnBnClickedButton2();									//�����ȹرշ�����
	UpdateData(true);											//��ȡ����
	if (server_port_ctrl.FindString(0,server_port) == CB_ERR)   //����������µĶ˿ںţ������combox��ȥ
	{
		server_port_ctrl.AddString(server_port);
	}
	list_ctrl.InsertString(0, getDateTime() + L":�������´���������socket��");
	int flag = Server.Create(_ttoi(server_port), SOCK_DGRAM, FD_WRITE | FD_READ);
	if (!flag) {											//������
		MessageBox(L"�����������˿�ʧ�ܣ�����socket���ã�");
		list_ctrl.InsertString(0, getDateTime() + L":�����������˿�����ʧ�� port:" + server_port);
	}
	else {
		list_ctrl.InsertString(0, getDateTime() + L":�����������˿����óɹ� port:" + server_port);
	}
}


void CCAsyncSocket_ServerDlg::OnBnClickedButton2()    //�رշ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (Server != INVALID_SOCKET)			//�жϵ�ǰ���׽����Ƿ��Ѿ����ر�
	{
		Server.Close();
		list_ctrl.InsertString(0, getDateTime() + L":������socket�رճɹ� port:" + server_port);
	}
	else
	{
		list_ctrl.InsertString(0, getDateTime() + L":������socket�Ѿ��ر�");
	}
}
