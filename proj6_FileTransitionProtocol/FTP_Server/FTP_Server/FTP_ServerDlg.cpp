
// FTP_ServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FTP_Server.h"
#include "FTP_ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//���� inet_ntoa �������ñ���
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


// CFTP_ServerDlg �Ի���



CFTP_ServerDlg::CFTP_ServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FTP_SERVER_DIALOG, pParent)
	, hostPort(_T("200"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFTP_ServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, hostIPCtrl);
	DDX_Control(pDX, IDC_EDIT1, hostPortCtrl);
	DDX_Control(pDX, IDC_EDIT3, logCtrl);
	DDX_Text(pDX, IDC_EDIT1, hostPort);
}

BEGIN_MESSAGE_MAP(CFTP_ServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_StartServer, &CFTP_ServerDlg::OnBnClickedStartserver)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CFTP_ServerDlg ��Ϣ�������

BOOL CFTP_ServerDlg::OnInitDialog()
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
	//��ʼ��������IP��ַ
	hostIPCtrl.SetWindowTextA(getHostIPStr());
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFTP_ServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFTP_ServerDlg::OnPaint()
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
HCURSOR CFTP_ServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//��־��ʾ����
void CFTP_ServerDlg::log(CString logInfo)
{
	CString alreadyLog,date,time;
	logCtrl.GetWindowTextA(alreadyLog);
	getDateTimeStr(date, time);
	logCtrl.SetWindowTextA(date + "-" + time + " : " + logInfo + "\r\n" + alreadyLog);
}

//����������FTP���Ʒ�����̣������û��������󣬽���ʶ���ж���Ȩ��Ϣ
void CFTP_ServerDlg::OnBnClickedStartserver()
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
	//�������������ӹ����׽��֣���ʼ�����û��˵���������,����������ť����ΪʧЧ
	CNSocket = new ServerSocket;
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
	((CButton*)GetDlgItem(IDC_StartServer))->EnableWindow(false);
}

//��ʱ����Ӧ
void CFTP_ServerDlg::OnTimer(UINT_PTR nIDEvent)
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
			CNSocket->sendPKT_list.GetAt(pos)->ResendTime++;
			CString logText;
			logText.Format("��ʱ�ش���SEQ:%d", ((FTPHeader_t*)(CNSocket->sendPKT_list.GetAt(pos)->PktData))->SEQNO);
			log(logText);
			break;
		}
		CNSocket->sendPKT_list.GetNext(pos);
	}
	CDialogEx::OnTimer(nIDEvent);
}
