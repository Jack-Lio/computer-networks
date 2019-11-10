
// MAC_IPDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

//����pcap��
#include "pcap.h"  

#pragma pack(1)
typedef struct FrameHeader_t {  //֡�ײ�
	BYTE DesMAC[6];
	BYTE SrcMAC[6];
	WORD FrameType;
}FrameHeader_t;

typedef struct ARPFrame_t {   //ARP֡
	FrameHeader_t FrameHeader;
	WORD		  HardwareType;
	WORD		  ProcotolType;
	BYTE		  HLen;
	BYTE		  PLen;
	WORD		  Openration;
	BYTE		  SendHa[6];
	DWORD		  SendIP;
	BYTE		  RecvHa[6];
	DWORD		  RecvIP;
}ARPFrame_t;

#pragma pack()



// CMAC_IPDlg �Ի���
class CMAC_IPDlg : public CDialogEx
{
// ����
public:
	CMAC_IPDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAC_IP_DIALOG };
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
	//������Ϣ������
	char errbuf[PCAP_ERRBUF_SIZE];
	// �豸�б���ʾ���ڿؼ�
	CListBox EtherNet_interface_ctrl;
	// �ӿ�������ʾ���ڿؼ�
	CEdit interface_detail_ctrl;
	//��Ӧ��ȡIPMAC��Ӧ��ϵ
	afx_msg void OnBnClickedGetipMac();
	// IP MAC ��Ӧ��ϵ��ʾ����
	CEdit IP_MAC_ctrl;
	//��Ӧ���豸�ӿ�
	afx_msg void OnLbnDblclkList1();
	// ���е������豸�б�ָ��ָ���һ���豸�б�ĵ�ַ
	pcap_if_t* alldevs;
	// ��¼ѡ�еĽӿ���Ϣ
	pcap_if_t *d;
	// ָ��ӿڵ�ַ�б�
	pcap_addr_t* a;
	// ����ַת��Ϊ�ַ�������	
	CString convert_addr_to_str(sockaddr* addr);
	//��¼��������ݰ�ͷ
	CList<pcap_pkthdr*> pkthdr_list;
	//��¼��������ݰ�
	CList<const u_char*> pktdata_list;
	// ��ǰ�����Ĳ������ݰ��߳�ָ��
	CWinThread* m_capture;
	// ���������ݰ��ĺ���
	LRESULT OnPacket(WPARAM wParam, LPARAM lParam);
	// ��ֹ���̵ı�־
	int stop_thread;
	// ��¼�򿪵�pcap�ӿ�
	pcap_t* opened_pcap;
	afx_msg void OnBnClickedCancel();
	// ARP֡���ݰ�
	ARPFrame_t *ARP_Frame;
	// �������������ARP���ݰ�,����֡ͷ����Ŀ��MAC��ַĬ��Ϊ�㲥��ַ
	ARPFrame_t* mk_ARPFrame(BYTE* SendHa_t, DWORD SendIP_t, BYTE* RecvHa_t, DWORD RecvIP_t, BYTE* SrcMAC_t, BYTE* DesMAC_t = NULL);
	// ��14�ֽڵ�ַЭ���ַתΪ4�ֽ�IP��ַ����
	DWORD GetAddr_IP(sockaddr* addr);
	// ��¼������MAC��ַ
	BYTE* host_MAC;
	// �ȴ���ñ���MAC��ַ��ѭ����������ֹ������ѭ��
	int waiter;
	// ��¼����ѡȡ�Ľӿ��豸��Ϣ
	CString interface_info;
	// ������յ�IPMAC��ַӳ��֮�����ʾ����
	LRESULT OnDisplayInfo(WPARAM wParam, LPARAM lParam);
	// ����IP��ַ
	CIPAddressCtrl IPAddr_ctrl;
};

//���Ĳ����߳̿��ƺ�������
UINT Capturer(PVOID hwnd);

