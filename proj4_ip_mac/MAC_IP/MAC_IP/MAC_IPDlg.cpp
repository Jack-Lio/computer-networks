
// MAC_IPDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MAC_IP.h"
#include "MAC_IPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//�Զ���һ����Ϣ�����������
#define WM_PACKET WM_USER+1
//���崥����ȡ����IPMAC��ַ����ʾ�ӿ���Ϣ����Ϣ
#define WM_DISPLAY WM_USER+2


//����localtime�����İ汾����
#pragma warning (disable: 4996)

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


// CMAC_IPDlg �Ի���



CMAC_IPDlg::CMAC_IPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAC_IP_DIALOG, pParent)
	, alldevs(NULL)
	, d(NULL)
	, a(NULL)
	, opened_pcap(NULL)
	, m_capture(NULL)
	, stop_thread(0)
	, ARP_Frame(NULL)
	, host_MAC(NULL)
	, waiter(0)
	, interface_info(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMAC_IPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, EtherNet_interface_ctrl);
	DDX_Control(pDX, IDC_EDIT1, interface_detail_ctrl);
	DDX_Control(pDX, IDC_EDIT2, IP_MAC_ctrl);
	DDX_Control(pDX, IDC_IPADDRESS2, IPAddr_ctrl);
}

BEGIN_MESSAGE_MAP(CMAC_IPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GETIP_MAC, &CMAC_IPDlg::OnBnClickedGetipMac)
	ON_LBN_DBLCLK(IDC_LIST1, &CMAC_IPDlg::OnLbnDblclkList1)
	ON_MESSAGE(WM_PACKET, &CMAC_IPDlg::OnPacket)
	ON_BN_CLICKED(IDCANCEL, &CMAC_IPDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_DISPLAY, &CMAC_IPDlg::OnDisplayInfo)
END_MESSAGE_MAP()


// CMAC_IPDlg ��Ϣ�������

BOOL CMAC_IPDlg::OnInitDialog()
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
		MessageBox(CString(errbuf), "ERROR", MB_OKCANCEL | MB_ICONERROR);
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

void CMAC_IPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMAC_IPDlg::OnPaint()
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
HCURSOR CMAC_IPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//��ȡIP_MAC ��ַӳ���ϵ
void CMAC_IPDlg::OnBnClickedGetipMac()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD IP;
	IPAddr_ctrl.GetAddress(IP);   //��ȡ�����IP��ַ
	if (opened_pcap != NULL)
	{
		mk_ARPFrame(host_MAC, GetAddr_IP(a->addr), NULL, ntohl(IP), host_MAC);  //���ͻ�ȡ��IP��ַӳ���ϵ��ARP����
	}
	else {
		MessageBox("δѡ���豸�ӿڣ�");
	}
}

// ��14�ֽڵ�ַЭ���ַתΪ4�ֽ�IP��ַ����
DWORD CMAC_IPDlg::GetAddr_IP(sockaddr* addr)
{
	if (addr == NULL)
		return 0x0;
	else
	{
		DWORD* address = (DWORD *)((addr->sa_data)+2);
		return *address;
	}
	return 0;
}

LRESULT CMAC_IPDlg::OnDisplayInfo(WPARAM wParam, LPARAM lParam)
{
	if (host_MAC == NULL)
	{
		stop_thread = 1;
		MessageBox("��ȡ����IP_MACӳ��ʧ�ܣ�");
		return LRESULT();
	}
	//��ʽ��MAC��ַ����
	CString hostmac;
	hostmac.Format("%02x-%02x-%02x-%02x-%02x-%02x\r\n", host_MAC[0], host_MAC[1], host_MAC[2], host_MAC[3], host_MAC[4], host_MAC[5]);
	//MessageBox(hostmac);
	//��MAC��ַ�����Ӧ����ʾλ�ò���ʾ�ӿ���Ϣ
	interface_info.Insert(interface_info.Find("\r\n", interface_info.Find("\r\n") + 2) + 2, hostmac);
	interface_detail_ctrl.SetWindowTextA(interface_info);   
	UpdateData(true);
	return LRESULT();
}

//˫���豸�б��ѡ���ض��б����Ϣ������ʾ��
//ͬʱ��ѡ�е��豸�ӿڣ����ڴ˽ӿڴ������ձ����߳�
//�߳̽��ո������ϵ����б��ģ�
//����ɸѡ֮��ARP����֡�洢���Ļ�������������Ϣ���ô�����OnPacket���н���
void CMAC_IPDlg::OnLbnDblclkList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���µ��豸�ӿڣ������Ƿ����߳��������У�����stop_thread������λΪ1
	stop_thread = 1;
	//����豸�ӿ�������ʾ����
	interface_detail_ctrl.SetWindowTextA("");
	//��ȡѡ�е��豸�ӿ�����
	int sno = EtherNet_interface_ctrl.GetCurSel();
	//����CString������¼�ӿ���ϸ��Ϣ���Լ���¼�豸�ӿ���
	CString  sitem;
	//��ȡ�豸�ӿ���
	EtherNet_interface_ctrl.GetText(sno, sitem);
	//ͨ��ѡ�еĽӿ�������豸�ӿڵĵ�ַ����ֵ�������Աd��
	//֮�����ͨ��d��ȡ�ýӿ���Ϣ�������豸�ӿ������͸����ַ��ʾ
	for (d = alldevs; d != NULL; d = d->next)
		if ((CString)(d->name) == sitem)
			break;
	//����Ϣ���
	sitem += "\r\n";   //����
	sitem += (CString)d->description;  //�豸����
	//��ȡ���豸��IP��ַ��Ϣ����ʾ
	for (a = d->addresses; a != NULL; a = a->next)
	{
		if (a->addr->sa_family == AF_INET)     // �жϸĵ�ַ�Ƿ�ΪIP��ַ
		{
			sitem += "\r\n";   //����
			sitem += convert_addr_to_str(a->addr);  //��ȡIP��ַ
			sitem += "\r\n";   //����
			sitem += convert_addr_to_str(a->netmask); //��ȡ��������
			sitem += "\r\n";   //����
			sitem += convert_addr_to_str(a->broadaddr); //��ȡ�㲥��ַ
			sitem += "\r\n";   //����
			sitem += convert_addr_to_str(a->dstaddr); //��ȡĿ�ĵ�ַ
			break;
		}
	}
	if (a == NULL) {
		MessageBox("���豸�ӿ�û��IP��ַ����ѡ���µ��豸��");
		return;
	}
	interface_info = sitem;       //����¼�Ĳ�����Ϣ�������������
	//���豸�ӿ�
	opened_pcap = pcap_open(d->name,			//�豸�ӿ�����ֱ�Ӵ����Աdָ���л�ȡ��һ��ѡ��֮��dָ��ָ��ѡ����豸�ӿ�
		4096,				//��ȡ���ݰ�����󳤶�
		PCAP_OPENFLAG_PROMISCUOUS,	//���豸�ӿڻ�ȡ�������ݰ���ʽ���Ĳ���Ϊ����ģʽ����ȡ��������������ӿڵ����ݰ�
		1000,				//�ȴ�һ�����ݰ������ʱ��
		NULL,				//Զ���豸�����������ݰ�ʹ�ã���ʵ��ֻ������ΪNULL
		errbuf);			//������Ϣ������
	if (opened_pcap == NULL)		//������
		MessageBox(CString(errbuf), "ERROR", MB_OKCANCEL | MB_ICONERROR);
	else {
		//�����µ��߳�֮ǰ����Ҫ�Ƚ�ԭ�����̹߳ر�,�����߳̿��Ʋ���stop_thread��λΪ0
		stop_thread =0;
		//�豸�򿪳ɹ��������߳̽������ݰ�����,����һ���߳�ָ��
		m_capture = AfxBeginThread(Capturer,	//�������̵߳Ŀ��ƺ���
			NULL,		                //�������ƺ����Ĳ�����һ��Ϊĳ���ݽṹ��ָ�룬����Ϊ��
			THREAD_PRIORITY_NORMAL);	//�߳����ȼ���Ĭ��Ϊ���������ȼ�
										//�ɹ������߳�֮�󽫰�ť����Ϊ���ɵ��״̬
		//MessageBox("���ݰ������̴߳����ɹ���");
	}
	//ͨ���Ա�������һ��ARP�����ȡ������IPMACӳ���ϵ
	BYTE SendHa_t[6]{ 0x66,0x66,0x66,0x66,0x66,0x66 };  //���ԴMAC��ַ 66-66-66-66-66-66
	DWORD SendIP_t = 0x70707070;				//���ԴIP��ַ 112.112.112.112
	DWORD RecvIP_t = GetAddr_IP(a->addr);			//����IP��ַ��ΪĿ�ĵ�ַ
													//CString str;
													//str.Format("0x%08x.0x%08x.0x%02x", SendIP_t, RecvIP_t,SendHa_t[0]);
													//MessageBox(str);
	//CString str;
	//str.Format("%08x,%08x", GetAddr_IP(a->addr), GetAddr_IP(a->addr));
	//MessageBox("���յ�ARP���ݱ���" + str);
	mk_ARPFrame(SendHa_t, SendIP_t, NULL, RecvIP_t, SendHa_t);   //����һ��ARP ����	
}


// ����ַת��Ϊ�ַ�������
CString CMAC_IPDlg::convert_addr_to_str(sockaddr* addr)
{
	if (addr == NULL)
		return "NULL";
	else
	{
		CString str;
		unsigned char * temp = (unsigned char *)addr->sa_data;
		str.Format("%d.%d.%d.%d",  temp[2], temp[3], temp[4], temp[5]);
		return str;
	}
	return CString();
}

//�߳�ִ�к������������ݰ��Ĳ�����
UINT Capturer(PVOID hwnd) {
	pcap_pkthdr* pkt_header = NULL;							//��¼��������ݰ�ͷ
	const u_char* pkt_data = NULL;			//��¼��������ݰ�
	CMAC_IPDlg* Dlg = (CMAC_IPDlg*)AfxGetApp()->m_pMainWnd;  //��ȡ�����ھ��
	while (!Dlg->stop_thread)			//ѭ�������������ݰ���ͨ������stop_thread ����ֹͣ����
	{
		//�������ݰ���������룬����pcap_next_ex����ִ��
		int flag = 0;
		TRY{
			flag = pcap_next_ex(Dlg->opened_pcap,	//pcap_tָ�룬˵�������Ǹ������ϵ����ݰ�
			&pkt_header,						//pcap_pkthdr ָ�룬��¼���ݰ������Ϣ
				&pkt_data							//uchar*ָ�룬�������ݰ�����
				);
		}CATCH(CException, e)
		{
			TCHAR error[1024]("\0");
			e->GetErrorMessage(error, 1024);
			Dlg->MessageBox( "$$$$$�����쳣" + (CString)error);
			memset(error, 0, 1024);
			continue;
		}END_CATCH;
		//�ɹ���������,���Բ���ĺ������з���
		if (flag == 1)
		{
			FrameHeader_t* FHeader = (FrameHeader_t*)pkt_data;
			if (ntohs(FHeader->FrameType) == WORD(0x0806))//���յ�ARP����
			{
				ARPFrame_t* ARP = (ARPFrame_t*)pkt_data;
				if (ntohs(ARP->Openration) == 2)     //���յ�ARPӦ����
				{
					/*CString str;
					str.Format("%08x,%08x", ARP->SendIP, Dlg->GetAddr_IP(Dlg->a->addr));
					Dlg->MessageBox("���յ�ARP���ݱ���" + str);*/
					if (ARP->SendIP == Dlg->GetAddr_IP(Dlg->a->addr))  //�����ȡ��ARP��Ӧ���Ա��������ͨ����ARP��Ӧ��ȡ������MAC��ַ
					{
						Dlg->host_MAC = ARP->SendHa;
						//CString str;
						//str.Format("%02x-%02x-%02x-%02x-%02x-%02x", Dlg->host_MAC[0]
						//	, Dlg->host_MAC[1]
						//	, Dlg->host_MAC[2]
						//	, Dlg->host_MAC[3]
						//	, Dlg->host_MAC[4]
						//	, Dlg->host_MAC[5]);
						// Dlg->MessageBox(str);
						Dlg->PostMessageA(WM_DISPLAY, 0, 0);  //������Ϣ��ʾ�����ӿ���Ϣ����IPMACӳ���ϵ
						
						//Dlg->pkthdr_list.AddTail(pkt_header);
						//Dlg->pktdata_list.AddTail(pkt_data);     //�����Ĵ��뻺����
						//Dlg->PostMessageA(WM_PACKET, 0, 0);
					}
					else {
						//Dlg->MessageBox("���յ��Ǳ������͵�ARP����");
						Dlg->pkthdr_list.AddTail(pkt_header);   
						Dlg->pktdata_list.AddTail(pkt_data);     //�����Ĵ��뻺����
						Dlg->PostMessageA(WM_PACKET, 0, 0);      //���ͱ��Ľ��ճɹ���Ϣ�������Ľ�������  
					}
				}
			}
		}
		else if (flag == 0)
		{
			//Dlg->MessageBox(L"δ�ڹ涨ʱ���ڲ������ݰ�");
		}
		else if (flag == -1)
		{
			Dlg->MessageBox("���ݱ�������ִ�д���");
		}
	}
	Dlg->stop_thread = 0;				//���ò���Ϊ0
	Dlg->MessageBox( "�������ݰ��߳�ֹͣ��");
	return 0;
}

// ���������ݰ��ĺ���,�����ݰ����н���
LRESULT CMAC_IPDlg::OnPacket(WPARAM wParam, LPARAM lParam)
{
	TRY{
	//�ӻ����������ȡ���ݱ���
	pcap_pkthdr* pkt_header = pkthdr_list.GetHead();		//��¼��������ݰ�ͷ
	const u_char* pkt_data = pktdata_list.GetHead();			//��¼��������ݰ�
																//��ȡ֡ͷ�������ʱ������ݺ���������
	//����ARP���ģ�����IP��ַ��MAC��ַ��ӳ���ϵ����ʾ��IPMAC��ʾ����
	ARPFrame_t*  ARP = (ARPFrame_t*)pkt_data;
	unsigned char* IP = (unsigned char*)(&ARP->SendIP);
	BYTE* MAC = ARP->SendHa;
	CString IP_str,MAC_str;
	//��ʽ��
	IP_str.Format("%d.%d.%d.%d",IP[0],IP[1],IP[2],IP[3]); 
	//MessageBox(IP_str);
	MAC_str.Format(" %02x-%02x-%02x-%02x-%02x-%02x\r\n",MAC[0],MAC[1],MAC[2],MAC[3],MAC[4],MAC[5]);
	//MessageBox(MAC_str);
	CString IP_MAC;
	IP_MAC_ctrl.GetWindowTextA(IP_MAC);
	IP_MAC_ctrl.SetWindowTextA(IP_MAC + IP_str+"-->"+MAC_str);

	pkthdr_list.RemoveHead();							//�Ƴ�ͷ�����ݰ�
	pktdata_list.RemoveHead();

	}CATCH(CException, e)
	{
		TCHAR error[1024]("\0");
		e->GetErrorMessage(error, 1024);
		MessageBox("$$$$$�����쳣" + (CString)error);
		memset(error, 0, 1024);
	}END_CATCH;
	//MessageBox(L"�����Ƿ���������");
	return LRESULT();
}

void CMAC_IPDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	//�ͷ��豸�б�
	pcap_freealldevs(alldevs);
	//����߳�δ�ر���ر��߳�
	stop_thread = 1;				//���ò���Ϊ1

	CDialogEx::OnCancel();
}



// �������������ARP���ݰ����������ݰ�����
ARPFrame_t* CMAC_IPDlg::mk_ARPFrame(BYTE* SendHa_t, DWORD SendIP_t, BYTE* RecvHa_t, DWORD RecvIP_t, BYTE* SrcMAC_t, BYTE* DesMAC_t)
{
	ARP_Frame = new ARPFrame_t;
	for (int i = 0; i < 6; i++) {
		ARP_Frame->FrameHeader.DesMAC[i] = 0xff;       //Ĭ��Ϊ�㲥��ַ����ȫ1
		ARP_Frame->FrameHeader.SrcMAC[i] = SrcMAC_t[i];  //ԴMAC��ַΪ����MAC��ַ
		ARP_Frame->SendHa[i] = SendHa_t[i];           //���Ͷ�MAC��ַ
		ARP_Frame->RecvHa[i] = 0x0;			 //Ŀ��MAC��ַ��Ӧ������Ϊ0
	}
	ARP_Frame->FrameHeader.FrameType = htons(0x0806); //֡����ΪARP
	ARP_Frame->HardwareType = htons(0x0001);   //Ӳ������Ϊ��̫��
	ARP_Frame->ProcotolType = htons(0x0800); //Э������ΪIP
	ARP_Frame->HLen = 6;				//Ӳ����ַ����Ϊ6
	ARP_Frame->PLen = 4;				//Э���ַ����Ϊ4
	ARP_Frame->Openration = htons(0x0001); //��������ΪARP����
	//���ݲ����Զ�������
	ARP_Frame->SendIP = SendIP_t;    //���Ͷ�IP��ַ
	ARP_Frame->RecvIP = RecvIP_t;	//���ն�IP��ַ����Ϊ�����IP��ַ

	if (pcap_sendpacket(opened_pcap,   //ͨ���Ŀ��������ͱ���
		(u_char*)ARP_Frame, //��������
		sizeof(ARPFrame_t)) != 0) // ���ĳ���
	{				//������
		MessageBox("ARP���ݰ�����ʧ�ܣ�");
	}
	else
	{
		//���ͳɹ�
		MessageBox("ARP���ݰ����ͳɹ���");
	}
	return ARP_Frame;
}



