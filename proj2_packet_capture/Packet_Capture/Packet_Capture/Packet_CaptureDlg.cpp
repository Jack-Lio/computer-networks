
// Packet_CaptureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Packet_Capture.h"
#include "Packet_CaptureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//����localtime�����İ汾����
#pragma warning (disable: 4996)

//�Զ���һ����Ϣ
#define WM_PACKET WM_USER+1

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


// CPacket_CaptureDlg �Ի���



CPacket_CaptureDlg::CPacket_CaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PACKET_CAPTURE_DIALOG, pParent)
	, alldevs(NULL)
	, m_capture(NULL)
	, stop_thread(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPacket_CaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, EtherNet_interface_ctrl);
	DDX_Control(pDX, IDC_LIST2, interface_detail_ctrl);
	DDX_Control(pDX, IDC_EDIT1, condition_ctrl);
	DDX_Control(pDX, IDC_LIST3, packet_list_ctrl);
}

BEGIN_MESSAGE_MAP(CPacket_CaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_DBLCLK(IDC_LIST1, &CPacket_CaptureDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDCANCEL, &CPacket_CaptureDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CPacket_CaptureDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_PACKET, &CPacket_CaptureDlg::OnPacket)  
	ON_BN_CLICKED(IDC_BUTTON2, &CPacket_CaptureDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CPacket_CaptureDlg ��Ϣ�������

BOOL CPacket_CaptureDlg::OnInitDialog()
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

	/*
	* ��ȡ��ǰ�������豸�б�����pcap_findalldevs_ex()����
	* �����ص��豸�б�ָ�븳�������ж����alldevs���� 
	*/
	int flag = pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf);		//��ȡ�豸�б�
	if (flag == -1)						//������
	{
		MessageBox(CString(errbuf), L"ERROR", MB_OKCANCEL | MB_ICONERROR);
	}
	else
	{
		//��ʾ�豸�ӿ��б�
		for (d = alldevs; d != NULL; d = d->next)
		{
			EtherNet_interface_ctrl.InsertString(0, (CString)d->name);
		}
	}


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPacket_CaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPacket_CaptureDlg::OnPaint()
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
HCURSOR CPacket_CaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//˫�������豸�б�������������ڽӿ���ϸ��Ϣ����ʾ��Ӧ�豸�ӿڵ���ϸ��Ϣ
void CPacket_CaptureDlg::OnLbnDblclkList1()			//��ʾ�豸�ӿ���ϸ��Ϣ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���֮ǰ��ϸ��Ϣ���е���Ϣ
	while (interface_detail_ctrl.GetCount())
		interface_detail_ctrl.DeleteString(0);
	//��ȡѡ�е��豸�ӿ�����
	int sno = EtherNet_interface_ctrl.GetCurSel();
	CString sitem;
	EtherNet_interface_ctrl.GetText(sno,sitem);
	//���豸�ӿ���д����ϸ��Ϣ��
	interface_detail_ctrl.InsertString(0, sitem);
	//��ʾ�豸�ӿ���ϸ��Ϣ
	for (d = alldevs; d != NULL; d = d->next)
		if ((CString)(d->name) == sitem)
		{
			interface_detail_ctrl.InsertString(interface_detail_ctrl.GetCount(), (CString)d->description);
			break;
		}
}

//��ⷵ�ذ�ť�������������ͷ��豸�б���˳�����
void CPacket_CaptureDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//�ͷ��豸�б�
	pcap_freealldevs(alldevs);

	CDialogEx::OnCancel();
}

//�߳�ִ�к������������ݰ��Ĳ�����
UINT Capturer(PVOID hwnd) {
	pcap_pkthdr* pkt_header;		//��¼��������ݰ�ͷ
	const u_char* pkt_data;			//��¼��������ݰ�
	CPacket_CaptureDlg* Dlg = (CPacket_CaptureDlg*)AfxGetApp()->m_pMainWnd;  //��ȡ�����ھ��
	while (!Dlg->stop_thread)			//ѭ�������������ݰ���ͨ������stop_thread ����ֹͣ����
	{
		//�������ݰ���������룬����pcap_next_ex����ִ��
		int flag = pcap_next_ex(Dlg->opened_pcap,		//pcap_tָ�룬˵�������Ǹ������ϵ����ݰ�
				&pkt_header,		//pcap_pkthdr ָ�룬��¼���ݰ������Ϣ
				&pkt_data			//uchar*ָ�룬�������ݰ�����
			);
		if (flag == 1)		//�ɹ���������
		{
			Dlg->pkthdr_list.AddTail(pkt_header);
			Dlg->pktdata_list.AddTail(pkt_data);
			Dlg->PostMessageW(WM_PACKET, 0, 0);
		}
		else if (flag == 0)
		{
			//Dlg->MessageBox(L"δ�ڹ涨ʱ���ڲ������ݰ�");
		}
		else if (flag == -1)
		{
			Dlg->MessageBox(L"ִ�д���");
		}
	}
	Dlg->stop_thread = 0;				//���ò���Ϊ0
	Dlg->MessageBox(L"�߳���ֹ");		//��ʾֹͣ�������ݰ�
	return 1;
}

//����������ݰ���ť����������
//���豸��ϸ��Ϣ���ڻ�ȡ������ϢȻ�����Ӧ�����豸�ӿ�
//�������ݰ������߳�
void CPacket_CaptureDlg::OnBnClickedButton1()			
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (interface_detail_ctrl.GetCount() == 0)			//δѡ���豸�ӿڣ��˳����������д�����ʾ
	{
		MessageBox(L"δѡ���豸�ӿڣ���˫���豸�ӿ��б�ѡ�����ѡ��",L"WARNING", MB_OKCANCEL | MB_ICONWARNING);
		return;
	}
	//���豸�ӿ�
	opened_pcap = pcap_open(d->name,			//�豸�ӿ�����ֱ�Ӵ����Աdָ���л�ȡ��һ��ѡ��֮��dָ��ָ��ѡ����豸�ӿ�
						4096,				//��ȡ���ݰ�����󳤶�
						PCAP_OPENFLAG_PROMISCUOUS,	//���豸�ӿڻ�ȡ�������ݰ���ʽ���Ĳ���Ϊ����ģʽ����ȡ��������������ӿڵ����ݰ�
						1000,				//�ȴ�һ�����ݰ������ʱ��
						NULL,				//Զ���豸�����������ݰ�ʹ�ã���ʵ��ֻ������ΪNULL
						errbuf);			//������Ϣ������
	if (opened_pcap == NULL)		//������
		MessageBox(CString(errbuf),L"ERROR", MB_OKCANCEL | MB_ICONERROR);
	else {
		//�豸�򿪳ɹ��������߳̽������ݰ�����,����һ���߳�ָ��
		m_capture = AfxBeginThread(Capturer,	//�������̵߳Ŀ��ƺ���
									NULL,		//�������ƺ����Ĳ�����һ��Ϊĳ���ݽṹ��ָ�룬����Ϊ��
								THREAD_PRIORITY_NORMAL);	//�߳����ȼ���Ĭ��Ϊ���������ȼ�
		//�ɹ������߳�֮�󽫰�ť����Ϊ���ɵ��״̬
		GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
		//MessageBox(CString(d->name));
	}
}


// ���������ݰ��ĺ���,�����ݰ����н���
LRESULT CPacket_CaptureDlg::OnPacket(WPARAM wParam, LPARAM lParam)
{
	pcap_pkthdr* pkt_header = pkthdr_list.GetHead();		//��¼��������ݰ�ͷ
	const u_char* pkt_data = pktdata_list.GetHead();			//��¼��������ݰ�
	pkthdr_list.RemoveHead();
	pktdata_list.RemoveHead();
	CString   header_s,data_s;
	char timebuf[128];
	time_t t(pkt_header->ts.tv_sec);
	strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));
	header_s.Format(L"ʱ���:%s,CAPLEN:%d��LEN:%d",CString(timebuf), pkt_header->caplen,ntohs(pkt_header->len));
	packet_list_ctrl.InsertString(packet_list_ctrl.GetCount(), header_s);
	Data_t* Data = (Data_t*)pkt_data;
	if (ntohs(Data->FrameHeader.FrameType) ==0x0800)
	{
		data_s.Format(L"Э������:0x%04x,Ŀ�ĵ�ַ:%s,Դ��ַ:%s,У���:%04x,�����У����:%04x,��ʶ��:%04x", ntohs(Data->FrameHeader.FrameType), (CString)Data->FrameHeader.DesMAC, (CString)Data->FrameHeader.SrcMAC, ntohs(Data->IPHeader.Checksum), IPHeader_ckeck((WORD*)(&Data->IPHeader)),Data->IPHeader.ID);
		packet_list_ctrl.InsertString(packet_list_ctrl.GetCount(), data_s);
	}
	
	//MessageBox(L"�����Ƿ���������");
	return LRESULT();
}

//ֹͣ�������ݰ�����ֹ�������ݰ����߳�
//������صı��������������ݰ��İ�ť�ָ�
void CPacket_CaptureDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	stop_thread = 1;
	//�ָ��������ݰ���ť������״̬
	GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
}


// IPͷ�������㷨
WORD CPacket_CaptureDlg::IPHeader_ckeck(WORD* IPHeader)
{
	WORD ans = 0;
	CString str;
	for (int i = 0; i < 10; i++)
	{
		if (i == 5) continue;			//�����������
		if ((ans + IPHeader[i])%0x10000 < ans)
			ans = ans + IPHeader[i] + 1;
		else
			ans += IPHeader[i];
		//str.Format(L"%08x,%08x,%08x,%d", ans, IPHeader[i], ans + IPHeader[i], ans + IPHeader[i]<ans);
		//packet_list_ctrl.InsertString(packet_list_ctrl.GetCount(),str);
	}
	return WORD(ntohs(~ans));
}
