
// RouterDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

//����pcap��
#include "pcap.h"  

#define IPFrameBufLen 16


//֡���ݽṹ����
#pragma pack(1)
typedef struct FrameHeader_t {		//֡�ײ�
	BYTE DesMAC[6];					//Ŀ�ĵ�ַ
	BYTE SrcMAC[6];					//ԭ��ַ
	WORD FrameType;					//֡����
}FrameHeader_t;

typedef struct ARPFrame_t {		  // ARP֡
	FrameHeader_t	FrameHeader;  // ֡�ײ�
	WORD			HardwareType; // Ӳ������
	WORD			ProtocolType; // Э������
	BYTE			HLen;         // Ӳ����ַ����
	BYTE			PLen;         // Э���ַ����
	WORD			Operation;    // ����ֵ
	UCHAR			SendHa[6];    // ԴMAC��ַ
	ULONG			SendIP;       // ԴIP��ַ
	UCHAR			RecvHa[6];    // Ŀ��MAC��ַ
	ULONG			RecvIP;       // Ŀ��IP��ַ
} ARPFrame_t;



typedef struct IPHeader_t {		  // IP�ײ�
	BYTE	Ver_HLen;             // �汾+ͷ������
	BYTE	TOS;                  // ��������
	WORD	TotalLen;             // �ܳ���
	WORD	ID;                   // ��ʶ
	WORD	Flag_Segment;         // ��־+Ƭƫ��
	BYTE	TTL;                  // ����ʱ��
	BYTE	Protocol;             // Э��
	WORD	Checksum;             // ͷ��У���
	ULONG	SrcIP;                // ԴIP��ַ
	ULONG	DstIP;                // Ŀ��IP��ַ
} IPHeader_t;


typedef struct ICMPHeader_t {     // ICMP�ײ�
	BYTE    Type;                 // ����
	BYTE    Code;                 // ����
	WORD    Checksum;             // У���
	WORD    Id;                   // ��ʶ
	WORD    Sequence;             // ���к�
} ICMPHeader_t;

typedef struct Data_t {
	FrameHeader_t FrameHeader;
	IPHeader_t  IPHeader;
}Data_t;

//·�ɱ���
typedef struct RouteTable_t {
	DWORD	netMask;			//��������
	DWORD	destNet;			//Ŀ������
	DWORD	nextHops;			//��һ������ַ
}RouteTable_t;

//IP MAC��ַӳ�����
typedef struct IP_MAC_t {
	DWORD  IP;
	BYTE   MAC[6];
}IP_MAC_t;

//IP ��ַ��Ϣ�洢�ṹ��
typedef struct IP_Info_t {
	DWORD  IP;
	DWORD  netMask;
	BYTE   MAC[6];
}IP_Info_t;


typedef struct SendPacket_t {	  // �������ݰ��ṹ
	int				len;          // ����
	BYTE			PktData[2000];		// ���ݻ���
	ULONG			TargetIP;     // Ŀ��IP��ַ
	UINT_PTR		n_mTimer;     // ��ʱ��
} SendPacket_t;

#pragma pack()


// CRouterDlg �Ի���
class CRouterDlg : public CDialogEx
{
// ����
public:
	CRouterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROUTER_DIALOG };
#endif

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
//WinPcapʹ��������ݳ�Ա����
	//������Ϣ������
	char errbuf[PCAP_ERRBUF_SIZE];
	// ���е������豸�б�ָ��ָ���һ���豸�б�ĵ�ַ
	pcap_if_t* alldevs;
	// ��¼ѡ�еĽӿ���Ϣ
	pcap_if_t *d;
	// ָ��ӿڵ�ַ�б�
	pcap_addr_t* a;
	// ��¼�򿪵�pcap�ӿ�
	pcap_t* opened_pcap;
	// ��¼������MAC��ַ
	BYTE*  host_MAC;
	//����IP��ַ,����
	IP_Info_t IP_addr[2];
	//·�ɱ�
	CList<RouteTable_t*> RouteTable;
	//IPMACӳ���
	CList<IP_MAC_t*> IP_MAC;
//�߳̿��Ʋ���
	//ֹͣARP���ݰ������߳�
	volatile bool stop_ARP_Catch_Thread;	
	//ֹͣ·�����ݰ������߳�
	volatile bool stop_Route_Catch_Thread;
	//ARP���Ĳ����߳�
	CWinThread* m_ARPCaptureThread;
	//·�ɱ��Ĳ����߳�
	CWinThread* m_RouteCaptureThread;
//�Ի�����Ƽ������ؼ�
	// �豸�б���ʾ���ڿؼ�
	CListBox EtherNet_interface_ctrl;
	// ��־��ʾ����
	CEdit log_ctrl;
	// ·�ɱ���ʾ����
	CListBox router_table_ctrl;
	// �����������봰��
	CIPAddressCtrl netmask_ctrl;
	// Ŀ����������ؼ�
	CIPAddressCtrl des_net_ctrl;
	// ��һ�������봰��
	CIPAddressCtrl next_hops_ctrl;
//���ݰ�������
	//��¼�����·�����ݰ���ͷ
	CList<pcap_pkthdr*> pkthdr_list;
	//��¼�����·�����ݰ�
	CList<const u_char*> pktdata_list;
	//��¼�����ARPӦ���Ļ�����
	CList<const u_char*> ARP_pktdata_list;
	//���ͻ�����
	CList<SendPacket_t*> sendIPFrame_list;
	//���ͻ�������ౣ��ʮ��������͵�����֡
	UINT_PTR SendTimerLimit[IPFrameBufLen];
//��غ�������
	//˫��Ŀ������������ø������ϵ�IP��ַ�Լ�MAC��ַ������ʼ��·����·�ɻ���
	afx_msg void OnLbnDblclkList2();
	//·��ת������
	LRESULT OnROUTEPacket(WPARAM wParam, LPARAM lParam);
	//ARP���Ĵ�����
	LRESULT OnARPPacket(WPARAM wParam, LPARAM lParam);
	//·�ɻ�����ʼ��
	LRESULT OnBeginRoute(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

// ��14�ֽڵ�ַЭ���ַתΪ4�ֽ�IP��ַ����
DWORD GetAddr_IP(sockaddr* addr);
// ��sockaddr�е�ַת��Ϊ�ַ�������
CString convert_sockaddr_to_str(sockaddr* addr);
// ��IPaddr�е�ַת��Ϊ�ַ�������
CString convert_IPaddr_to_str(DWORD addr);
// ��MAC addr�е�ַת��Ϊ�ַ�������
CString convert_MAC_to_str(BYTE* MAC);
// �������������ARP���ݰ����������ݰ�����,���ͳɹ�����true
bool SendARP(pcap_t* opened_pcap, BYTE* SendHa_t, DWORD SendIP_t, DWORD RecvIP_t);
//�߳�ִ�к���������ARP���ݰ�
UINT RecvARP(PVOID hwnd);
//�߳�ִ�к�����������Ҫ·�ɵ����ݰ�
UINT RecvRouteP(PVOID hwnd);
//��ֵMAC��ַ
void CopyMAC(BYTE* MAC1, BYTE*MAC2);
//ˢ����ʾ��·�ɱ�
void updateRouteTable();
// IPͷ�������㷨
WORD IPHeader_ckeck(WORD* IPHeader);
// ICMPͷ�������㷨
WORD ICMPHeader_ckeck(WORD* ICMPHeader);
//�ж�MAC��ַ�Ƿ����
bool compMAC(BYTE* m1, BYTE* m2);
//�ж�IP��ַ�Ƿ����
bool compIP(DWORD ip1, DWORD ip2);
//�洢IPMACӳ���ϵ
bool saveIP_MAC(DWORD IP, BYTE* MAC);
//��ȡIP��Ӧ��MAC��ַ
BYTE* getMACForIP(DWORD IP);
//ת�����ݰ��ĺ���
bool sendIPFrame(pcap_t* opened_pcap, u_char* IPFrame, int Framelen);
//����ICMP����
bool sendICMP(pcap_t* opened_pcap, BYTE type,BYTE code, const u_char* pkt_data);		
//��ȡ��ʱ����ID
UINT_PTR getTimerID();
//���ն�ʱ��ID
void backTimerID(UINT_PTR ID );
  


