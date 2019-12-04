
// RouterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Router.h"
#include "RouterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//�Զ���һ����Ϣ�������·�ɱ���
#define WM_ROUTEPACKET WM_USER+1
//�Զ���һ����Ϣ�������ARP����
#define WM_ARPPACKET WM_USER+2
//��ȡ��������MAC��ַ֮��ʼ��ʼ��·�ɻ���
#define WM_BEGINROUTE WM_USER+3

CMutex mMutex(0, 0, 0);          //����
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


// CRouterDlg �Ի���



CRouterDlg::CRouterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ROUTER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//��ʼ���߳̿��Ʋ���
	stop_ARP_Catch_Thread = true;
	stop_Route_Catch_Thread = true;
}

void CRouterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, EtherNet_interface_ctrl);
	DDX_Control(pDX, IDC_EDIT1, log_ctrl);
	DDX_Control(pDX, IDC_LIST1, router_table_ctrl);
	DDX_Control(pDX, IDC_IPADDRESS1, netmask_ctrl);
	DDX_Control(pDX, IDC_IPADDRESS2, des_net_ctrl);
	DDX_Control(pDX, IDC_IPADDRESS3, next_hops_ctrl);
}

BEGIN_MESSAGE_MAP(CRouterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_DBLCLK(IDC_LIST2, &CRouterDlg::OnLbnDblclkList2)
	ON_MESSAGE(WM_ARPPACKET, &CRouterDlg::OnARPPacket)
	ON_MESSAGE(WM_ROUTEPACKET, &CRouterDlg::OnROUTEPacket)
	ON_MESSAGE(WM_BEGINROUTE, &CRouterDlg::OnBeginRoute)
	ON_BN_CLICKED(IDC_ADD, &CRouterDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DEL, &CRouterDlg::OnBnClickedDel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRouterDlg ��Ϣ�������

BOOL CRouterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	//assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//assert(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		//assert(bNameValid);
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
	if (flag == -1)																	//������
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
	
	//����û�����ָʾ��Ϣ
	log_ctrl.SetWindowTextA("��ӭ����LW-1711350·�������豸�б��ѻ�ȡ����˫����Ŀ��������ʼ·�ɣ�\r\n\
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");

	//��ȡ�豸�б�����ϣ��ͷ��豸�б�
	//pcap_freealldevs(alldevs);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CRouterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRouterDlg::OnPaint()
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
HCURSOR CRouterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//%%%%%%�Զ��庯�����岿��

// ��14�ֽڵ�ַЭ���ַתΪ4�ֽ�IP��ַ����
DWORD GetAddr_IP(sockaddr* addr)
{
	if (addr == NULL)
		return 0x0;
	else
	{
		DWORD* address = (DWORD *)((addr->sa_data) + 2);
		return *address;
	}
	return 0;
}


// ��sockaddr�е�ַת��Ϊ�ַ�������
CString convert_sockaddr_to_str(sockaddr* addr)
{
	if (addr == NULL)
		return "";
	else
	{
		CString str;
		unsigned char * temp = (unsigned char *)addr->sa_data;
		str.Format("%d.%d.%d.%d", temp[2], temp[3], temp[4], temp[5]);
		return str;
	}
	return CString();
}

// ��IPaddr�е�ַת��Ϊ�ַ�������
CString convert_IPaddr_to_str(DWORD addr)
{
	CString str;
	unsigned char * temp = (unsigned char *)&addr;
	str.Format("%d.%d.%d.%d", temp[0], temp[1], temp[2], temp[3]);
	return str;
}

// ��MAC addr�е�ַת��Ϊ�ַ�������
CString convert_MAC_to_str(BYTE* MAC)
{
	if (MAC == NULL)
		return "";
	else
	{
		//��ʽ��MAC��ַ����
		CString hostmac;
		hostmac.Format("%02x:%02x:%02x:%02x:%02x:%02x", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
		return hostmac;
	}
	return CString();
}

//����MAC��ַ
void CopyMAC(BYTE* MAC1, BYTE*MAC2) {
	for (int i = 0; i < 6; i++)
	{
		MAC2[i] = MAC1[i];
	}
}

//�洢IPMACӳ���ϵ
bool saveIP_MAC(DWORD IP, BYTE* MAC) {
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;
	POSITION pos = Dlg->IP_MAC.GetHeadPosition();
	while (pos != NULL)
	{
		if (compIP(Dlg->IP_MAC.GetAt(pos)->IP, IP))					//��ַӳ���ϵ�Ѿ�����
		{
			return false;											//�˳�
		}
		Dlg->IP_MAC.GetNext(pos);
	}
	//���IP MAC ӳ���¼
	IP_MAC_t* ipmac = new IP_MAC_t;
	ipmac->IP = IP;
	CopyMAC(MAC, ipmac->MAC);
	Dlg->IP_MAC.AddTail(ipmac);
	return true;
}
//��ȡIP��Ӧ��MAC��ַ
BYTE* getMACForIP(DWORD IP) {
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;
	POSITION pos = Dlg->IP_MAC.GetHeadPosition();
	while (pos != NULL)
	{
		if (compIP(Dlg->IP_MAC.GetAt(pos)->IP, IP))					//��ַӳ���ϵ�Ѿ�����
		{
			return (Dlg->IP_MAC.GetAt(pos)->MAC);						//�˳�,����MAC��ַ
		}
		Dlg->IP_MAC.GetNext(pos);
	}
	return NULL;	//������ӳ���ϵ�����ؿ�ָ��
}

//�ж�MAC��ַ�Ƿ����
bool compMAC(BYTE* m1, BYTE* m2)
{
	for (int i = 0; i < 6; i++)
	{
		if (int(m1[i]) != int(m2[i]))
			return false;
	}
	return true;
}
//�ж�IP��ַ�Ƿ����
bool compIP(DWORD ip1, DWORD ip2)
{
	return (int(ip1) == int(ip2));
}
// IPͷ�������㷨
WORD IPHeader_ckeck(WORD* IPHeader)
{
	WORD ans = 0;
	CString str;
	for (int i = 0; i < 10; i++)
	{
		if (i == 5) continue;			//�����������
		if ((ans + IPHeader[i]) % 0x10000 < ans)
			ans = (ans + IPHeader[i]) % 0x10000 + 1;
		else
			ans += IPHeader[i];
		//str.Format(L"%08x,%08x,%08x,%d", ans, IPHeader[i], ans + IPHeader[i], ans + IPHeader[i]<ans);
		//packet_list_ctrl.InsertString(packet_list_ctrl.GetCount(),str);
	}
	return WORD(ntohs(~ans));
}

// ICMPͷ�������㷨
WORD ICMPHeader_ckeck(WORD* ICMPHeader)
{
	WORD ans = 0;
	CString str;
	for (int i = 0; i < 4; i++)
	{
		if (i == 1) continue;			//�����������
		if ((ans + ICMPHeader[i]) % 0x10000 < ans)
			ans = (ans + ICMPHeader[i]) % 0x10000 + 1;
		else
			ans += ICMPHeader[i];
		//str.Format(L"%08x,%08x,%08x,%d", ans, IPHeader[i], ans + IPHeader[i], ans + IPHeader[i]<ans);
		//packet_list_ctrl.InsertString(packet_list_ctrl.GetCount(),str);
	}
	return WORD(ntohs(~ans));
}

// �������������ARP���ݰ����������ݰ�����,���ͳɹ�����true
bool SendARP(pcap_t* opened_pcap ,BYTE* SendHa_t, DWORD SendIP_t, DWORD RecvIP_t)
{
	ARPFrame_t* ARP_Frame = new ARPFrame_t;
	for (int i = 0; i < 6; i++) {
		ARP_Frame->FrameHeader.DesMAC[i] = 0xff;			//Ĭ��Ϊ�㲥��ַ����ȫ1
		ARP_Frame->FrameHeader.SrcMAC[i] = SendHa_t[i];		//ԴMAC��ַΪ����MAC��ַ
		ARP_Frame->SendHa[i] = SendHa_t[i];					//���Ͷ�MAC��ַ
		ARP_Frame->RecvHa[i] = 0x0;							//Ŀ��MAC��ַ��Ӧ������Ϊ0
	}

	ARP_Frame->FrameHeader.FrameType = htons(0x0806);		//֡����ΪARP
	ARP_Frame->HardwareType = htons(0x0001);				//Ӳ������Ϊ��̫��
	ARP_Frame->ProtocolType = htons(0x0800);				//Э������ΪIP
	ARP_Frame->HLen = 6;									//Ӳ����ַ����Ϊ6
	ARP_Frame->PLen = 4;									//Э���ַ����Ϊ4
	ARP_Frame->Operation = htons(0x0001);					//��������ΪARP����
															//���ݲ����Զ�������
	ARP_Frame->SendIP = SendIP_t;							//���Ͷ�IP��ַ
	ARP_Frame->RecvIP = RecvIP_t;							//���ն�IP��ַ����Ϊ�����IP��ַ

	if (pcap_sendpacket(opened_pcap,   //ͨ���Ŀ��������ͱ���
		(u_char*)ARP_Frame, //��������
		sizeof(ARPFrame_t)) != 0) // ���ĳ���
	{				//������
		delete ARP_Frame;
		return false;
	}
	delete ARP_Frame;
	return true;
}

//ת�����ݰ��ĺ���,����������Ǵ�֡ͷ�������ݰ�
bool sendIPFrame(pcap_t* opened_pcap,u_char* IPFrame,int FrameLen) {
	if (pcap_sendpacket(opened_pcap,   //ͨ���Ŀ��������ͱ���
		(u_char*)IPFrame, //��������
		FrameLen) != 0) // ���ĳ���
	{				//������
		return false;
	}
	return true;
}
// ����У���
unsigned short ChecksumCompute(unsigned short * buffer, int size)
{
	// 32λ���ӳٽ�λ
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer++;
		// 16λ���
		size -= sizeof(unsigned short);
	}
	if (size)
	{
		// �������е���8λ
		cksum += *(unsigned char *)buffer;
	}
	// ����16λ��λ������16λ
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	// ȡ��
	return (unsigned short)(~cksum);
}

//����ICMP����
bool sendICMP(pcap_t* opened_pcap, BYTE type, BYTE code, const u_char* pkt_data) {
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;

	u_char * ICMPBuf = new u_char[70];

	// ���֡�ײ�
	memcpy(((FrameHeader_t *)ICMPBuf)->DesMAC, ((FrameHeader_t *)pkt_data)->SrcMAC, 6);
	memcpy(((FrameHeader_t *)ICMPBuf)->SrcMAC, ((FrameHeader_t *)pkt_data)->DesMAC, 6);
	((FrameHeader_t *)ICMPBuf)->FrameType = htons(0x0800);

	// ���IP�ײ�
	((IPHeader_t *)(ICMPBuf + 14))->Ver_HLen = ((IPHeader_t *)(pkt_data + 14))->Ver_HLen;
	((IPHeader_t *)(ICMPBuf + 14))->TOS = ((IPHeader_t *)(pkt_data + 14))->TOS;
	((IPHeader_t *)(ICMPBuf + 14))->TotalLen = htons(56);
	((IPHeader_t *)(ICMPBuf + 14))->ID = ((IPHeader_t *)(pkt_data + 14))->ID;
	((IPHeader_t *)(ICMPBuf + 14))->Flag_Segment = ((IPHeader_t *)(pkt_data + 14))->Flag_Segment;
	((IPHeader_t *)(ICMPBuf + 14))->TTL = 64;
	((IPHeader_t *)(ICMPBuf + 14))->Protocol = 1;
	((IPHeader_t *)(ICMPBuf + 14))->SrcIP =Dlg->IP_addr[0].IP;//((IPHeader_t *)(pkt_data+14))->DstIP;
	((IPHeader_t *)(ICMPBuf + 14))->DstIP = ((IPHeader_t *)(pkt_data + 14))->SrcIP;
	((IPHeader_t *)(ICMPBuf + 14))->Checksum = 0;
	((IPHeader_t *)(ICMPBuf + 14))->Checksum = ChecksumCompute((unsigned short *)(ICMPBuf + 14), 20);

	// ���ICMP�ײ�
	((ICMPHeader_t *)(ICMPBuf + 34))->Type = type;
	((ICMPHeader_t *)(ICMPBuf + 34))->Code = code;
	((ICMPHeader_t *)(ICMPBuf + 34))->Id = 0;
	((ICMPHeader_t *)(ICMPBuf + 34))->Sequence = 0;
	((ICMPHeader_t *)(ICMPBuf + 34))->Checksum = 0;
	
	// �������
	memcpy((u_char *)(ICMPBuf + 42), (IPHeader_t *)(pkt_data + 14), 20);
	memcpy((u_char *)(ICMPBuf + 62), (u_char *)(pkt_data + 34), 8);
	((ICMPHeader_t *)(ICMPBuf + 34))->Checksum = ChecksumCompute((unsigned short *)(ICMPBuf + 34), 36);

	// �������ݰ�
	pcap_sendpacket(Dlg->opened_pcap, (u_char *)ICMPBuf, 70);

	delete[] ICMPBuf;
	return true;
}

//�߳�ִ�к���������ARP���ݰ�
UINT RecvARP(PVOID hwnd) {
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;
	pcap_pkthdr* pkt_header = NULL;		//��¼��������ݰ�ͷ
	const u_char* pkt_data = NULL;				//��¼��������ݰ�
	while (!Dlg->stop_ARP_Catch_Thread)			//ѭ�������������ݰ���ͨ������stop_thread ����ֹͣ����
	{
		//�������ݰ���������룬����pcap_next_ex����ִ��
		int flag = 0;
		TRY{
			flag = pcap_next_ex(
					Dlg->opened_pcap,					//pcap_tָ�룬˵�������Ǹ������ϵ����ݰ�
					&pkt_header,						//pcap_pkthdr ָ�룬��¼���ݰ������Ϣ
					&pkt_data							//uchar*ָ�룬�������ݰ�����
				);
		}CATCH(CException, e)
		{
			TCHAR error[1024]("\0");
			e->GetErrorMessage(error, 1024);
			Dlg->MessageBox("$$$$$�����쳣" + (CString)error);
			memset(error, 0, 1024);
			continue;
		}END_CATCH;
		//�ɹ���������,���Բ���ĺ������з���
		if (flag == 1)
		{
			FrameHeader_t* FHeader = (FrameHeader_t*)pkt_data;
			if (int(ntohs(FHeader->FrameType)) == int(0x0806))				//���յ�ARP����
			{
				ARPFrame_t* ARP = (ARPFrame_t*)pkt_data;
				if (int(ntohs(ARP->ProtocolType)) == int(0x0800))
				{
					if (int(ntohs(ARP->Operation)) == int(0x0002))						//���յ�ARPӦ����
					{
						if ((ARP->SendIP == Dlg->IP_addr[0].IP || ARP->SendIP == Dlg->IP_addr[1].IP))		//�����ȡ��ARP��Ӧ���Ա��������ͨ����ARP��Ӧ��ȡ������MAC��ַ
						{
							if (Dlg->host_MAC != NULL) continue;											//����Ѿ����չ��ˣ�������ñ���
							//assert(!Dlg->host_MAC);
							Dlg->host_MAC = new BYTE[6];
							CopyMAC(ARP->SendHa, Dlg->host_MAC);										//��ȡ������MAC��ַ������־�н����������������Ӧ�Ļ���
							CString log_t;
							Dlg->log_ctrl.GetWindowTextA(log_t);
							Dlg->log_ctrl.SetWindowTextA(log_t + ">>����MAC��ַ��" + convert_MAC_to_str(Dlg->host_MAC) + "\r\n");
							Dlg->PostMessageA(WM_BEGINROUTE, 0, 0);
						}
						else {
							//Dlg->MessageBox("���յ��Ǳ������͵�ARP����");
							Dlg->ARP_pktdata_list.AddTail(pkt_data);			//�����Ĵ��뻺����
							Dlg->PostMessageA(WM_ARPPACKET, 0, 0);				//���ͱ��Ľ��ճɹ���Ϣ����ARP���Ľ�������  
						}
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
	Dlg->stop_ARP_Catch_Thread = 0;								//���ò���Ϊ0
	CString log_t;
	Dlg->log_ctrl.GetWindowTextA(log_t);
	Dlg->log_ctrl.SetWindowTextA(log_t + ">>���񱾵�ARP���ݰ��߳���ֹ��" + "\r\n");
	return 0;
}

//�߳�ִ�к���������Route���ݰ�
UINT RecvRouteP(PVOID hwnd) {
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;
	pcap_pkthdr* pkt_header = NULL;				//��¼��������ݰ�ͷ
	const u_char* pkt_data = NULL;				//��¼��������ݰ�
	while (!Dlg->stop_Route_Catch_Thread)			//ѭ�������������ݰ���ͨ������stop_thread ����ֹͣ����
	{
		//�������ݰ���������룬����pcap_next_ex����ִ��
		int flag = 0;
		TRY{
			flag = pcap_next_ex(
			Dlg->opened_pcap,					//pcap_tָ�룬˵�������Ǹ������ϵ����ݰ�
				&pkt_header,						//pcap_pkthdr ָ�룬��¼���ݰ������Ϣ
				&pkt_data							//uchar*ָ�룬�������ݰ�����
				);
		}CATCH(CException, e)
		{
			TCHAR error[1024]("\0");
			e->GetErrorMessage(error, 1024);
			Dlg->MessageBox("$$$$$�����쳣" + (CString)error);
			memset(error, 0, 1024);
			continue;
		}END_CATCH;
		//�ɹ���������,���Բ���ĺ������з���
		if (flag == 1)
		{
			FrameHeader_t* FHeader = (FrameHeader_t*)pkt_data;
			if (int(ntohs(FHeader->FrameType))== int(0x0800))				//���յ�IPv4����
			{
				//Data_t* IP_Data = (Data_t*)pkt_data;
				//if (!(compMAC(IP_Data->FrameHeader.DesMAC, Dlg->host_MAC)
				//	&& !compIP(IP_Data->IPHeader.DstIP, Dlg->IP_addr[0].IP)
				//	&& !compIP(IP_Data->IPHeader.DstIP, Dlg->IP_addr[1].IP)))	//������Ҫ·�ɵı��ģ�ֱ������
				//	continue;
				Dlg->pkthdr_list.AddTail(pkt_header);
				Dlg->pktdata_list.AddTail(pkt_data);     //�����Ĵ��뻺����
				Dlg->PostMessageA(WM_ROUTEPACKET, 0, 0);      //���ͱ��Ľ��ճɹ���Ϣ�������Ľ�������  
			}
			if (int(ntohs(FHeader->FrameType)) == int(0x0806))				//���յ�ARP����
			{
				Dlg->ARP_pktdata_list.AddTail(pkt_data);			//�����Ĵ��뻺����
				Dlg->PostMessageA(WM_ARPPACKET, 0, 0);				//���ͱ��Ľ��ճɹ���Ϣ����ARP���Ľ�������  
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
	Dlg->stop_Route_Catch_Thread = 0;								//���ò���Ϊ0
	CString log_t;
	Dlg->log_ctrl.GetWindowTextA(log_t);
	Dlg->log_ctrl.SetWindowTextA(log_t + ">>����·�����ݰ��߳���ֹ��" + "\r\n");
	return 0;
}



// ������·�����ݰ��ĺ���,�����ݰ�����ת��
LRESULT CRouterDlg::OnROUTEPacket(WPARAM wParam, LPARAM lParam)
{
	TRY{
		//�ӻ����������ȡ���ݱ���
	pcap_pkthdr* pkt_header = pkthdr_list.GetHead();			//��¼��������ݰ�ͷ
	const u_char* pkt_data = pktdata_list.GetHead();			//��¼��������ݰ�
	//����õ�IPV4���İ���IP���ݰ���ʽ��
	Data_t* IP_Data = (Data_t*)pkt_data;
	//�����־
	CString log_t;
	//�������ģ����Ƿ�����Ҫ·�ɵı���,��Ҫ·�ɵı����ص��Ǳ��ĵ�Ŀ��MAC��ַ�Ǳ���������Ŀ��IP��ַ���Ǳ���
	//if (!(compMAC(IP_Data->FrameHeader.DesMAC, host_MAC)
	//	&& !compIP(IP_Data->IPHeader.DstIP, IP_addr[0].IP)
	//	&& !compIP(IP_Data->IPHeader.DstIP, IP_addr[1].IP)))	//������Ҫ·�ɵı��ģ�ֱ������
	//{
	//	pkthdr_list.RemoveHead();							//�Ƴ��������ͷ�����ݰ�
	//	pktdata_list.RemoveHead();
	//	log_ctrl.GetWindowTextA(log_t);
	//	log_ctrl.SetWindowTextA(log_t + "=========================================================\r\n" +
	//		">>���ղ���·�ɱ���:" +
	//		convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + " -- " +
	//		convert_IPaddr_to_str(IP_Data->IPHeader.DstIP) + "  " +
	//		convert_MAC_to_str(IP_Data->FrameHeader.SrcMAC) + " -- " +
	//		convert_MAC_to_str(IP_Data->FrameHeader.DesMAC) + "\r\n");
	//	return LRESULT();
	//}

	log_ctrl.GetWindowTextA(log_t);
	log_ctrl.SetWindowTextA(log_t + "=========================================================\r\n"+
		">>���տ�·�ɱ���:"  +
		convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + " -- " +
		convert_IPaddr_to_str(IP_Data->IPHeader.DstIP) + "  " +
		convert_MAC_to_str(IP_Data->FrameHeader.SrcMAC) + " -- " +
		convert_MAC_to_str(IP_Data->FrameHeader.DesMAC) + "\r\n");
	//�����յ������ݰ�ԴIP��ַ��MAC��ַӳ���ϵ������IPMACӳ���
	saveIP_MAC(IP_Data->IPHeader.SrcIP, IP_Data->FrameHeader.SrcMAC);
	//����·�ɱ��Ĵ���
	IP_Data->IPHeader.TTL = IP_Data->IPHeader.TTL - 1;   //TTL �ݼ�
	IP_Data->IPHeader.Checksum = htons(IPHeader_ckeck((WORD*)&IP_Data->IPHeader));//���¼���У���
	//���ݰ���ʱ��ת��ICMP���Ĵ�������
	CString t;
	t.Format("%d", IP_Data->IPHeader.TTL);
	log_ctrl.SetWindowTextA(log_t + "--->>TTL:" +
		t+ "\r\n");
	
	if (int(IP_Data->IPHeader.TTL) <= 0)
	{
		//assert(IP_Data->IPHeader.TTL <= 0);
		pkthdr_list.RemoveHead();							//�Ƴ��������ͷ�����ݰ�
		pktdata_list.RemoveHead();

		//ICMP����  ��ʱ type = 11, code =0  
		if (sendICMP(opened_pcap, 11, 0, pkt_data))
		{
			//����ICMP�ɹ�
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + "--->>�ɹ���Դ��ַ����ICMP��ʱ����: ԴIP��ַ:"+
				convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + "\r\n");
		}
		else
		{
			//����ʧ��
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + "--->>ERROR:��Դ��ַ����ICMP��ʱ����ʧ��: ԴIP��ַ:" +
				convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + "\r\n");
		}
		return LRESULT();
	}

	if (IPHeader_ckeck((WORD*)(&IP_Data->IPHeader)) != ntohs(IP_Data->IPHeader.Checksum))
	{

		//��־��Ϣ���
		CString checksum;
		checksum.Format("����У���:0x%04x,����У���:0x%04x", IP_Data->IPHeader.Checksum, IPHeader_ckeck((WORD*)(&IP_Data->IPHeader)));		//�����ʽ��
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + "--->>ERROR:У��ʹ���\r\n======>" +checksum+ "\r\n");
		//ICMP����
		pkthdr_list.RemoveHead();							//�Ƴ��������ͷ�����ݰ�
		pktdata_list.RemoveHead();
		return LRESULT();
	}
	//·�ɱ��ѯ
	DWORD nextHop_IP = 0x0;
	DWORD longestNetMask = 0x0;
	bool find = false;
	//����·�ɱ�
	POSITION pos = RouteTable.GetHeadPosition();
	while (pos != NULL)
	{
		RouteTable_t* item = RouteTable.GetAt(pos);
		if (compIP(item->netMask&IP_Data->IPHeader.DstIP, item->destNet)
			&&int(item->netMask)>int(longestNetMask))			//ƥ�䵽��Ӧ��·�ɲ������볤�ȴ���֮ǰ�����볤�ȣ�Ĭ������Ϊ0
		{
			find = true;
			longestNetMask = item->netMask;
			nextHop_IP = item->nextHops;
		}
		RouteTable.GetNext(pos);
	}
	if (!find)													//δƥ�䵽·��
	{
		pkthdr_list.RemoveHead();							//�Ƴ��������ͷ�����ݰ�
		pktdata_list.RemoveHead();
		//ICMP����
		//ICMP����  ���粻�ɴ� type = 3, code =0  
		if (sendICMP(opened_pcap, 3, 0, pkt_data))
		{
			//����ICMP�ɹ�
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + "--->>�ɹ���Դ��ַ����ICMP���粻�ɴﱨ��: ԴIP��ַ:" +
				convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + "\r\n");
		}
		else
		{
			//����ʧ��
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + "--->>ERROR:��Դ��ַ����ICMP���粻�ɴﱨ��ʧ��: ԴIP��ַ:" +
				convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + "\r\n");
		}
		return LRESULT();
	}

	//ƥ�䵽·�ɱ����ѯIPMACӳ�����ȡMAC��ַ�������ֱ��·��
	if (nextHop_IP == 0x0)			//ֱ��·��
	{
		//ֱ�ӽ�Ŀ��IP��ַ������һ������ַ���к�������
		nextHop_IP = IP_Data->IPHeader.DstIP;

		CString test;
		test.Format("0x%04x-->0x%04x", ntohs(IP_Data->IPHeader.Checksum), IPHeader_ckeck((WORD*)&IP_Data->IPHeader));
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + "--->>ֱ��·�ɣ����¼���У���:" + test + "\r\n");
	}
	else
	{	
		//��ֱ��·��
		CString test;
		test.Format("0x%04x-->0x%04x", ntohs(IP_Data->IPHeader.Checksum),IPHeader_ckeck((WORD*)&IP_Data->IPHeader));
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + "--->>��ֱ��·�ɣ����¼���У���:" + test + "\r\n");
	}
	
	//·�ɵĽ�һ������
	//�����ܳ����ֶ�,��8λ�ֽ�Ϊ��λ
	int totalLen = ntohs(IP_Data->IPHeader.TotalLen);						//��ȡIP�����ܳ���
	totalLen += sizeof(FrameHeader_t);
	if (getMACForIP(IP_Data->IPHeader.DstIP) == NULL)
	{
		//û���ҵ���ӦIP��MAC��ַ,ͨ��ARP���Ļ�ȡMAC��ַ
		//ͨ���Ա�������һ��ARP�����ȡĿ�ĵ�ַ��IPMACӳ���ϵ
		DWORD SendIP_t=0x0;													//����IP��ַ��MAC��ַ
		BYTE* SendHa_t = NULL;
		for (int i = 0; i < 2; i++)
		{
			if (compIP(nextHop_IP&IP_addr[i].netMask, IP_addr[i].IP&IP_addr[i].netMask))
			{
				SendIP_t = IP_addr[i].IP;
				SendHa_t = new BYTE[6];
				CopyMAC(IP_addr[i].MAC, SendHa_t);
				break;
			}
		}
		//assert(SendIP_t!=0x0&&SendHa_t!=NULL);
		DWORD RecvIP_t = nextHop_IP;										//Ŀ��IP��ַ��ΪĿ�ĵ�ַ
		SendARP(opened_pcap, SendHa_t, SendIP_t, RecvIP_t);
		//��¼��־
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + "--->>����ARP�����ĵ� IP:"+convert_IPaddr_to_str(RecvIP_t) + "\r\n");	
		//���淢�ͱ���
		int ID = 0;
		if ((ID = getTimerID()) == 0)
		{
			//����������
			//�����־
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + ">>���ͻ���������(15/15)���������ݰ�:" +
				convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + " -- " +
				convert_IPaddr_to_str(IP_Data->IPHeader.DstIP) + "  " +
				convert_MAC_to_str(IP_Data->FrameHeader.SrcMAC) + " -- " +
				convert_MAC_to_str(IP_Data->FrameHeader.DesMAC) + "\r\n");
		}
		else
		{
			mMutex.Lock(INFINITE);
			//assert(ID > 0);
			//assert(ID < IPFrameBufLen);
			SendPacket_t* IPFrame = new SendPacket_t;
			IPFrame->len = totalLen;
			for (int i = 0; i < IPFrame->len; i++)
		r	{
				IPFrame->PktData[i] = ((BYTE*)(pkt_data))[i];
			}
			IPFrame->TargetIP = nextHop_IP;
			SetTimer(ID, 10000, 0);		//��ʱ����Ϊ10��
			sendIPFrame_list.AddTail(IPFrame);
			mMutex.Unlock();
			//�����־
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + ">>���汨�ģ��ȴ�����ARP����:" +
				convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + " -- " +
				convert_IPaddr_to_str(IP_Data->IPHeader.DstIP) + "  " +
				convert_MAC_to_str(IP_Data->FrameHeader.SrcMAC) + " -- " +
				convert_MAC_to_str(IP_Data->FrameHeader.DesMAC) + "\r\n");
		}
		//�˳�ת������
		pkthdr_list.RemoveHead();							//�Ƴ��������ͷ�����ݰ�
		pktdata_list.RemoveHead();
		return LRESULT();
	}
	//����MACӳ�䣬��������
	BYTE * nextHop_MAC = new BYTE[6];
	CopyMAC(getMACForIP(IP_Data->IPHeader.DstIP), nextHop_MAC);
	//log_ctrl.GetWindowTextA(log_t);
	//log_ctrl.SetWindowTextA(log_t + "--->>�ɹ���ȡ��һ����IPMACӳ���ϵ: IP:" + convert_IPaddr_to_str(nextHop_IP)
	//	+" -- MAC:"+convert_MAC_to_str(nextHop_MAC)+ "\r\n");

	
	//�޸�֡ͷ����Ϣ��ȡ��֡ͷ������
	CopyMAC(nextHop_MAC,IP_Data->FrameHeader.DesMAC );
	CopyMAC(host_MAC, IP_Data->FrameHeader.SrcMAC);
	if (sendIPFrame(opened_pcap, (u_char*)IP_Data, totalLen))
	{
		//�ɹ�ת��
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + "--->>ת�����ݰ��ɹ���from:"+
			convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP)+" -- "
			+"to:"+convert_IPaddr_to_str(IP_Data->IPHeader.DstIP)+
			"\r\n next hop IP<->MAC:"+
			convert_IPaddr_to_str( nextHop_IP)+
			" <--> "+convert_MAC_to_str(nextHop_MAC)+"\r\n");
	}
	else
	{
		//δת���ɹ�
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + "--->>ERROR:ת��ʧ�ܣ�from:" +
			convert_IPaddr_to_str(IP_Data->IPHeader.SrcIP) + " -- "
			+ "to:" + convert_IPaddr_to_str(IP_Data->IPHeader.DstIP) +
			"\r\n next hop IP<->MAC:" +
			convert_IPaddr_to_str(nextHop_IP) +
			" <--> " + convert_MAC_to_str(nextHop_MAC) + "\r\n");
	}
	pkthdr_list.RemoveHead();							//�Ƴ��������ͷ�����ݰ�
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

// ������ARP���ݰ��ĺ���,�����ݰ����н���
LRESULT CRouterDlg::OnARPPacket(WPARAM wParam, LPARAM lParam)
{
	TRY{
		//�ӻ����������ȡ���ݱ���
	const u_char* pkt_data = ARP_pktdata_list.GetHead();			//��¼��������ݰ�
																//��ȡ֡ͷ�������ʱ������ݺ���������
																//����ARP���ģ�����IP��ַ��MAC��ַ��ӳ���ϵ����ʾ��IPMAC��ʾ����
	ARPFrame_t*  ARP = (ARPFrame_t*)pkt_data;

	bool getNewMAC =  saveIP_MAC(ARP->SendIP, ARP->SendHa);				//�洢IPMACӳ���ϵ
	//�����־
	CString log_t;
	//POSITION pos = IP_MAC.GetHeadPosition();
	//while (pos != NULL)
	//{
	//	IP_MAC_t*t = IP_MAC.GetAt(pos);
	//	log_ctrl.GetWindowTextA(log_t);
	//	log_ctrl.SetWindowTextA(log_t + ">>IP-MAC:" +
	//		"IP:" + convert_IPaddr_to_str(t->IP) +  " <--> " +
	//		"MAC:" + convert_MAC_to_str(t->MAC) + "\r\n");
	//	IP_MAC.GetNext(pos);
	//}
	log_ctrl.GetWindowTextA(log_t);
	log_ctrl.SetWindowTextA(log_t + ">>����ARP���Ľ����ɹ�:" +
		"IP:"+convert_IPaddr_to_str(ARP->SendIP)+" <--> "+
		"MAC:"+convert_MAC_to_str(ARP->SendHa) +"\r\n");

	ARP_pktdata_list.RemoveHead();							//�Ƴ������ɹ���ͷ�����ݰ�

	if (getNewMAC)			//��ȡ�µ�MACӳ���ϵ����鷢�ͻ��������Ƿ��б����ܹ�����
	{
		mMutex.Lock(INFINITE);
		int time = sendIPFrame_list.GetCount();
		while (time--) {
			POSITION pos = sendIPFrame_list.GetHeadPosition();
			while (pos != NULL)
			{
				SendPacket_t* item = sendIPFrame_list.GetAt(pos);
				if (compIP(item->TargetIP, ARP->SendIP))			//ƥ����Է��͵ı���
				{
					//�޸�֡ͷ����Ϣ��ȡ��֡ͷ������
					CopyMAC(ARP->SendHa, ((Data_t*)(item->PktData))->FrameHeader.DesMAC);
					CopyMAC(host_MAC, ((Data_t*)(item->PktData))->FrameHeader.SrcMAC);
					if (sendIPFrame(opened_pcap, (u_char*)item->PktData, item->len))
					{
						//�ɹ�ת��
						log_ctrl.GetWindowTextA(log_t);
						log_ctrl.SetWindowTextA(log_t + "--->>ת�����������ݰ��ɹ���from:" +
							convert_IPaddr_to_str(((Data_t*)(item->PktData))->IPHeader.SrcIP) + " -- "
							+ "to:" + convert_IPaddr_to_str(((Data_t*)(item->PktData))->IPHeader.DstIP) +
							"\r\n next hop IP<->MAC:" +
							convert_IPaddr_to_str(item->TargetIP) +
							" <--> " + convert_MAC_to_str(ARP->SendHa) + "\r\n");
					}
					else
					{
						//δת���ɹ�
						log_ctrl.GetWindowTextA(log_t);
						log_ctrl.SetWindowTextA(log_t + "--->>ERROR:ת�����������ݰ�ʧ�ܣ��������ݰ���from:" +
							convert_IPaddr_to_str(((Data_t*)(item->PktData))->IPHeader.SrcIP) + " -- "
							+ "to:" + convert_IPaddr_to_str(((Data_t*)(item->PktData))->IPHeader.DstIP) +
							"\r\n next hop IP<->MAC:" +
							convert_IPaddr_to_str(item->TargetIP) +
							" <--> " + convert_MAC_to_str(ARP->SendHa) + "\r\n");
					}
					backTimerID(item->n_mTimer);
					sendIPFrame_list.RemoveAt(pos);
					break;
				}
				sendIPFrame_list.GetNext(pos);
			}
		}
		mMutex.Unlock();
	}
	}CATCH(CException, e)
	{
		TCHAR error[1024]("\0");
		e->GetErrorMessage(error, 1024);
		MessageBox("$$$$$�����쳣" + (CString)error);
		memset(error, 0, 1024);
	}END_CATCH;
	return LRESULT();
}


//��ʼ��·�ɻ���
LRESULT CRouterDlg::OnBeginRoute(WPARAM wParam, LPARAM lParam)
{
	CString log_t;
	log_ctrl.GetWindowTextA(log_t);
	log_ctrl.SetWindowTextA(log_t + ">>��ʼ��ʼ��·�ɱ�" + "\r\n");
	//��ʼ��������
	for (int i = 0; i < IPFrameBufLen; i++)
	{
		SendTimerLimit[i] = i;			//���ͻ�������ౣ��ʮ��������͵�����֡
	}
	sendIPFrame_list.RemoveAll();
	//ɾ�����е�·�ɱ���Ϣ��IPMACӳ���
	int count = router_table_ctrl.GetCount();
	for (int i = 0; i < count; i++)
	{
		router_table_ctrl.DeleteString(0);
	}
	RouteTable.RemoveAll();
	IP_MAC.RemoveAll();
	//�رջ�ȡ����IPMACӳ������ݰ������̺߳����ݰ������߳�
	stop_ARP_Catch_Thread = true;
	stop_Route_Catch_Thread = true;
	//���ݽṹ��ʼ��
	for (int i = 0; i < 2; i++)
	{
		//��ʼ����IPMACӳ�������ݣ����Ƚ�������IPMACӳ����ȥ
		IP_MAC_t* ipmac = new IP_MAC_t;
		ipmac->IP = IP_addr[i].IP;
		//��������IP��ַ��ֵ��IP��Ϣ��Žṹ��
		for (int j = 0; j < 6; j++)
		{
			IP_addr[i].MAC[j] = host_MAC[j];
			ipmac->MAC[j] = host_MAC[j];
		}
		IP_MAC.AddTail(ipmac);
		//��������Ĭ��ֱ��·�ɴ���·�ɱ���������ʾ
		RouteTable_t* routeItem = new RouteTable_t;
		routeItem->netMask = IP_addr[i].netMask;
		routeItem->destNet = IP_addr[i].IP&IP_addr[i].netMask;
		routeItem->nextHops = 0x0;		//ֱ��Ͷ��
		RouteTable.AddTail(routeItem);		
		//��Ĭ�ϵ�ֱ��·�����
		CString rt;						
		rt.Format("%s -- %s -- %s (ֱ��Ͷ��)",
			convert_IPaddr_to_str(routeItem->netMask),
			convert_IPaddr_to_str(routeItem->destNet),
			convert_IPaddr_to_str(routeItem->nextHops));
		router_table_ctrl.InsertString(router_table_ctrl.GetCount(), rt);
	}

	log_ctrl.GetWindowTextA(log_t);
	log_ctrl.SetWindowTextA(log_t + ">>���·�ɱ��ʼ����" + "\r\n");

	// ���ù��˹���:��������arp��Ӧ֡����Ҫ·�ɵ�֡
	
	// ���ù��˹���:��������arp��Ӧ֡����Ҫ·�ɵ�֡
	CString Filter;
	struct bpf_program fcode;
	Filter.Format("(ether dst %s) and ((arp and (ether[21]=0x2))\
 or (not((ip dst host %s) or (ip dst host %s))))", convert_MAC_to_str(host_MAC),
		convert_IPaddr_to_str(IP_addr[0].IP),
		convert_IPaddr_to_str(IP_addr[1].IP));
	
	if ((pcap_compile(opened_pcap, &fcode, Filter, 1, IP_addr[0].netMask) <0) || (pcap_setfilter(opened_pcap, &fcode)<0))
	{
			MessageBox(Filter + L"���˹�����벻�ɹ���������д�Ĺ����﷨�Ƿ���ȷ�����ù���������");
			return LRESULT();
	}
	//MessageBox(Filter);
	//����·�����ݰ������߳�
	stop_Route_Catch_Thread = false;						//�����µ��߳�֮ǰ����Ҫ�Ƚ�ԭ�����̹߳ر�,�����߳̿��Ʋ���stop_thread��λΪ0

	m_RouteCaptureThread = AfxBeginThread(RecvRouteP,	//�������̵߳Ŀ��ƺ���
		NULL,										//�������ƺ����Ĳ�����һ��Ϊĳ���ݽṹ��ָ�룬����Ϊ��
		THREAD_PRIORITY_NORMAL);					//�߳����ȼ���Ĭ��Ϊ���������ȼ�

	log_ctrl.GetWindowTextA(log_t);
	log_ctrl.SetWindowTextA(log_t + ">>�ɹ��������ݰ������̣߳�" + "\r\n"
		+ ">>>>>>>>>>>>>>>>>>>>>>> ��ʼ·�� <<<<<<<<<<<<<<<<<<<<<<<" + "\r\n");

	return LRESULT();
}


//˫��Ŀ������������ø������ϵ�IP��ַ�Լ�MAC��ַ������ʼ��·����·�ɻ���
void CRouterDlg::OnLbnDblclkList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���û���
	host_MAC = NULL;
	//�ر������������е����ݰ������߳�
	if (stop_ARP_Catch_Thread == false)
	{
		stop_ARP_Catch_Thread = true;
		CString log_t;
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + ">>���ڹر�ARP���Ĳ����̣߳�" + "\r\n");
	}
	if (stop_Route_Catch_Thread == false)
	{
		stop_Route_Catch_Thread = true;
		CString log_t;
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + ">>���ڹر�·�ɱ��Ĳ����̣߳�" + "\r\n");
	}
	Sleep(1000);										//�ݶ�200ms��ͬ���߳�
	//��ȡѡ�������豸����Ϣ
	int sno = EtherNet_interface_ctrl.GetCurSel();	//��ȡѡ�е��豸�ӿ����
	CString  sitem;									//����CString������¼�ӿ���ϸ��Ϣ���Լ���¼�豸�ӿ���
	EtherNet_interface_ctrl.GetText(sno, sitem);	//��ȡ�豸�ӿ���
	for (d = alldevs; d != NULL; d = d->next)		//ͨ��ѡ�еĽӿ�������豸�ӿڵĵ�ַ����ֵ�������Աd��
		if ((CString)(d->name) == sitem)			//֮�����ͨ��d��ȡ�ýӿ���Ϣ�������豸�ӿ������͸����ַ��ʾ
			break;
	
	
	//����Ϣ���
	sitem += "\r\n>>�豸������";					//����
	sitem += (CString)d->description;				//�豸����


	//�����־��Ϣ
	CString log_t;
	log_ctrl.GetWindowTextA(log_t);
	log_ctrl.SetWindowTextA(log_t + ">>ѡ��������"+sitem+"\r\n");


	//�жϸ������Ƿ��������IP��ַ��������ʾ��������б���
	int IPCount = 0;								//ͳ��IP��ַ����
	for (a = d->addresses; a != NULL; a = a->next)
	{
		if (a->addr->sa_family == AF_INET)					// �жϸĵ�ַ�Ƿ�ΪIP��ַ
		{
			CString  ip_str;
			ip_str.Format(">>IP��ַ%d��%s  ��������%d��%s", IPCount, convert_sockaddr_to_str(a->addr), IPCount, convert_sockaddr_to_str(a->netmask));
			//�����־��Ϣ
			CString log_t;
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + ip_str + "\r\n");
			IP_addr[IPCount].IP = GetAddr_IP(a->addr);			//��¼����������IP��ַ
			IP_addr[IPCount].netMask = GetAddr_IP(a->netmask);	//��¼������������������	
			IPCount++;											//��¼��1
			if (IPCount == 2) break;							//�������IP�����˳�����ǰ��a���������ĵڶ���IP��ַ
		}
	}

	if (IPCount < 2)										//����������������IP��ַ��������˳�����
	{
		//�����־��Ϣ
		CString log_t;
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + ">>ERROR:���������߱�����IP��ַ���޷���Ϊ·���豸��������ѡ��" + "\r\n\
====================================================\r\n");
		return;
	}



	//ͨ����ȡ��IP��ַȡ����Ӧ��mac��ַ

	//��ѡ����豸
	//���豸�ӿ�
	opened_pcap = pcap_open(d->name,			//�豸�ӿ�����ֱ�Ӵ����Աdָ���л�ȡ��һ��ѡ��֮��dָ��ָ��ѡ����豸�ӿ�
		4096,									//��ȡ���ݰ�����󳤶�
		PCAP_OPENFLAG_PROMISCUOUS,				//���豸�ӿڻ�ȡ�������ݰ���ʽ���Ĳ���Ϊ����ģʽ����ȡ��������������ӿڵ����ݰ�
		1000,									//�ȴ�һ�����ݰ������ʱ��
		NULL,									//Զ���豸�����������ݰ�ʹ�ã���ʵ��ֻ������ΪNULL
		errbuf);								//������Ϣ������
	if (opened_pcap == NULL)					//������
		MessageBox(CString(errbuf), "ERROR", MB_OKCANCEL | MB_ICONERROR);
	else
	{
		stop_ARP_Catch_Thread = false;						//�����µ��߳�֮ǰ����Ҫ�Ƚ�ԭ�����̹߳ر�,�����߳̿��Ʋ���stop_thread��λΪ0

		m_ARPCaptureThread = AfxBeginThread(RecvARP,	//�������̵߳Ŀ��ƺ���
			NULL,										//�������ƺ����Ĳ�����һ��Ϊĳ���ݽṹ��ָ�룬����Ϊ��
			THREAD_PRIORITY_NORMAL);					//�߳����ȼ���Ĭ��Ϊ���������ȼ�
		CString log_t;
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + ">>�ɹ�����ARP���Ĳ����̣߳�" + "\r\n");
	}
	//����ARP���ݰ������߳�


	//ͨ���Ա�������һ��ARP�����ȡ������IPMACӳ���ϵ
	BYTE SendHa_t[6]{ 0x66,0x66,0x66,0x66,0x66,0x66 };				//���ԴMAC��ַ 66-66-66-66-66-66
	DWORD SendIP_t = 0x70707070;									//���ԴIP��ַ 112.112.112.112
	DWORD RecvIP_t = IP_addr[0].IP;									//����IP��ַ��ΪĿ�ĵ�ַ

	//assert(RecvIP_t != 0);


	if (!SendARP(opened_pcap, SendHa_t, SendIP_t, RecvIP_t))				//����һ��ARP ����	
	{																						//ʧ�ܣ��˳�����
		CString log_t;
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + ">>ERROR:����ARP���󵽱���ʧ�ܣ�" + "\r\n\
====================================================\r\n");
		return;
	}
	else {
		CString log_t;
		log_ctrl.GetWindowTextA(log_t);
		log_ctrl.SetWindowTextA(log_t + ">>�ɹ�����ARP���󵽱�����" + "\r\n");
	}
}
//ˢ����ʾ��·�ɱ�
void updateRouteTable() {
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;
	//ɾ�����е�·�ɱ���Ϣ������ǰ����Ĭ��·��
	int count = Dlg->router_table_ctrl.GetCount();
	for (int i = 0; i < count-2; i++)			
	{
		Dlg->router_table_ctrl.DeleteString(2);
	}
	//��һ�������·����Ϣ
	POSITION pos = Dlg->RouteTable.GetHeadPosition();
	while (pos != NULL)
	{
		RouteTable_t* item = Dlg->RouteTable.GetNext(pos);
		//�ж��޸ĵ�·���Ƿ�Ϊ�Զ����ɵ�ֱ��·�ɣ��������
		if ((int(item->destNet) == int(Dlg->IP_addr[0].IP&Dlg->IP_addr[0].netMask) && int(item->netMask) == int(Dlg->IP_addr[0].netMask))
			|| (int(item->destNet) == int(Dlg->IP_addr[1].IP&Dlg->IP_addr[1].netMask) && int(item->netMask) == int(Dlg->IP_addr[1].netMask)))
			continue;
		//��·�����
		CString rt;
		rt.Format("%s -- %s -- %s",
			convert_IPaddr_to_str(item->netMask),
			convert_IPaddr_to_str(item->destNet),
			convert_IPaddr_to_str(item->nextHops));
		Dlg->router_table_ctrl.InsertString(Dlg->router_table_ctrl.GetCount(), rt);
	}
}

//���·��
void CRouterDlg::OnBnClickedAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ȡ�����·����Ϣ
	DWORD des_net;
	des_net_ctrl.GetAddress(des_net);
	des_net = ntohl(des_net);
	DWORD netMask;
	netmask_ctrl.GetAddress(netMask);
	netMask = ntohl(netMask);
	DWORD nexthop_ip;
	next_hops_ctrl.GetAddress(nexthop_ip);
	nexthop_ip = ntohl(nexthop_ip);
	//�ж���ӵ�·����Ϣ�Ƿ��ܹ�Ӧ���ڱ��ӿڣ������Ƿ����ڱ��������ܹ�����·�ɵ�
	if (!compIP(des_net&netMask, des_net))
	{
		MessageBox("��ӵ�·�ɴ���", "ERROR", MB_OKCANCEL | MB_ICONERROR);
		return;
	}
	//ͨ���ж�����һ������IP��ַ�Ƿ��Ǳ����������ڵ���������
	bool InTheSameNet = false;
	for (int i = 0; i < 2; i++)
	{
		////assert(int(IP_addr[i].netMask&nexthop_ip) - int(IP_addr[i].netMask&IP_addr[i].IP)!=0);
		if (int(IP_addr[i].netMask&nexthop_ip) == int(IP_addr[i].netMask&IP_addr[i].IP))
		{
			InTheSameNet = true;
			break;
		}
	}
	if (!InTheSameNet)
	{
		MessageBox("��ӵ�·����һ�����޷��ִ", "ERROR", MB_OKCANCEL | MB_ICONERROR);
		return;
	}
	//�ж�·����Ϣ�Ƿ�����е��ظ�����ظ�������ʾ�����޸�Ϊ���µ�·�ɱ���
	POSITION pos = RouteTable.GetHeadPosition();
	while (pos != NULL)
	{
		RouteTable_t* item = RouteTable.GetAt(pos);
		//MessageBox(convert_IPaddr_to_str(item->destNet)+" "+convert_IPaddr_to_str(item->nextHops));
		if (int(item->destNet)==int(des_net)&&int(item->netMask)==int(netMask))			//·�ɹ�ϵ�Ѿ�����
		{		
			//�ж��޸ĵ�·���Ƿ�Ϊ�Զ����ɵ�ֱ��·�ɣ��������޸�
			if ((int(item->destNet) == int(IP_addr[0].IP&IP_addr[0].netMask)&&int(item->netMask )== int(IP_addr[0].netMask))
				|| (int(item->destNet) == int(IP_addr[1].IP&IP_addr[1].netMask)&&int(item->netMask) ==int( IP_addr[1].netMask)))
			{
				MessageBox("��ͼ�޸�ֱ��·�ɣ��Ƿ�����δͨ����", "ERROR", MB_OKCANCEL | MB_ICONERROR);
				return;
			}
			CString log;
			log.Format("$$���·���Ѵ���, ���޸�Ϊ�����ύ�汾:\r\n\
>>> netmask:%s dest net:%s nexthop: %s(old)-->%s(new)\r\n",
				convert_IPaddr_to_str(netMask),
				convert_IPaddr_to_str(des_net),
				convert_IPaddr_to_str(item->nextHops),
				convert_IPaddr_to_str(nexthop_ip));
			item->nextHops = nexthop_ip;							//�޸�Ϊ���µ�·������
			CString log_t;											//��¼��־��Ϣ
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + log);
			MessageBox("���·���Ѵ���,���޸�Ϊ�����ύ�汾��", "��ʾ", MB_OKCANCEL | MB_ICONINFORMATION);
			updateRouteTable();										//ˢ��·�ɱ�����֮���˳�����
			return;
		}
		RouteTable.GetNext(pos);
	}
	//����ӵ�·�ɴ���·�ɱ���������ʾ
	RouteTable_t* routeItem = new RouteTable_t;
	routeItem->netMask =netMask;
	routeItem->destNet = des_net;
	routeItem->nextHops =nexthop_ip;		//��һ����
	RouteTable.AddTail(routeItem);			//��·�ɱ���
	CString rt;
	rt.Format("%s -- %s -- %s ",
		convert_IPaddr_to_str(routeItem->netMask),
		convert_IPaddr_to_str(routeItem->destNet),
		convert_IPaddr_to_str(routeItem->nextHops));
	router_table_ctrl.InsertString(router_table_ctrl.GetCount(), rt);
	CString log;
	log.Format("$$���·��:netmask:%s dest net:%s nexthop: %s\r\n",
		convert_IPaddr_to_str(netMask),
		convert_IPaddr_to_str(des_net),
		convert_IPaddr_to_str(nexthop_ip));
	CString log_t;											//��¼��־��Ϣ
	log_ctrl.GetWindowTextA(log_t);
	log_ctrl.SetWindowTextA(log_t + log );
	
}

//ɾ��·��
void CRouterDlg::OnBnClickedDel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ȡ�����·����Ϣ ,ע�������������������
	DWORD des_net;
	des_net_ctrl.GetAddress(des_net);
	des_net = ntohl(des_net);
	DWORD netMask;
	netmask_ctrl.GetAddress(netMask);
	netMask = ntohl(netMask);
	DWORD nexthop_ip;
	next_hops_ctrl.GetAddress(nexthop_ip);
	nexthop_ip = ntohl(nexthop_ip);
	//�ж�·����Ϣ�Ƿ�����е���ͬ����ͬ��ɾ����������ʾ������
	POSITION pos = RouteTable.GetHeadPosition();
	while (pos != NULL)
	{
		RouteTable_t* item = RouteTable.GetAt(pos);
		if (int(item->destNet) == int(des_net) && int(item->netMask) == int(netMask))			//·�ɹ�ϵ�Ѿ�����
		{
			//�ж��޸ĵ�·���Ƿ�Ϊ�Զ����ɵ�ֱ��·�ɣ�������ɾ��
			if ((int(item->destNet) == int(IP_addr[0].IP&IP_addr[0].netMask) && int(item->netMask) == int(IP_addr[0].netMask))
				|| (int(item->destNet) == int(IP_addr[1].IP&IP_addr[1].netMask) && int(item->netMask) == int(IP_addr[1].netMask)))
			{
				MessageBox("��ͼɾ��ֱ��·�ɣ��Ƿ�����δͨ����", "ERROR", MB_OKCANCEL | MB_ICONERROR);
				return;
			}
			RouteTable.RemoveAt(pos);  
			CString log;
			log.Format("$$ɾ��·�ɳɹ�: netmask:%s dest net:%s nexthop: %s\r\n",
				convert_IPaddr_to_str(netMask),
				convert_IPaddr_to_str(des_net),
				convert_IPaddr_to_str(item->nextHops));
			CString log_t;											//��¼��־��Ϣ
			log_ctrl.GetWindowTextA(log_t);
			log_ctrl.SetWindowTextA(log_t + log);
			updateRouteTable();										//ˢ��·�ɱ�����֮���˳�����
			return;
		}
		RouteTable.GetNext(pos);
	}
	MessageBox("ɾ����·�ɱ�����ڣ�", "ERROR", MB_OKCANCEL | MB_ICONERROR);
}


void CRouterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;
	mMutex.Lock(INFINITE);
	POSITION pos = Dlg->sendIPFrame_list.GetHeadPosition();
	while (pos != NULL)
	{
		if (Dlg->sendIPFrame_list.GetAt(pos)->n_mTimer == nIDEvent)
		{
			Dlg->sendIPFrame_list.RemoveAt(pos);				//��������ʱ
			backTimerID(nIDEvent);								//����ID
			CDialogEx::OnTimer(nIDEvent);
			return;
		}
		Dlg->sendIPFrame_list.GetNext(pos);
	}
	mMutex.Unlock();
	CDialogEx::OnTimer(nIDEvent);
}
//��ȡ��ʱ����ID
UINT_PTR getTimerID() {
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;
	UINT_PTR ID = 0;
	for (int i = 1; i < IPFrameBufLen; i++)
	{
		if (Dlg->SendTimerLimit[i] != 0)
		{
			ID = Dlg->SendTimerLimit[i];
			Dlg->SendTimerLimit[i] = 0;
			break;
		}
	}
	//assert(ID >= 1||ID==0);
	//assert(ID < IPFrameBufLen);
	return ID;
}
//���ն�ʱ��ID
void backTimerID(UINT_PTR ID) {
	//��ȡ�����ھ���������Ա��ͨ���˾����ȡ�������Ա���޸�
	CRouterDlg*  Dlg = (CRouterDlg*)AfxGetApp()->m_pMainWnd;
	if (ID >= IPFrameBufLen) return;
	//assert(ID >= 1);
	//assert(ID < IPFrameBufLen);
	Dlg->KillTimer(ID);
	Dlg->SendTimerLimit[ID] = ID;
}


