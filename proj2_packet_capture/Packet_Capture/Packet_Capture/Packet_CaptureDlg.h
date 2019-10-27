
// Packet_CaptureDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
//����pcap��
#include "pcap.h"  

// CPacket_CaptureDlg �Ի���
class CPacket_CaptureDlg : public CDialogEx
{
// ����
public:
	CPacket_CaptureDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PACKET_CAPTURE_DIALOG };
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
	//��̫���豸�б���ʾ�ؼ�
	CListBox EtherNet_interface_ctrl;
	// ����ӿ���ϸ��Ϣlistbox�ؼ�
	CListBox interface_detail_ctrl;
	// ��������
	CEdit condition_ctrl;
	// ������Ϣ��ʾ���ڿؼ�
	CListBox packet_list_ctrl;
	// ���е������豸�б�ָ��ָ���һ���豸�б�ĵ�ַ
	pcap_if_t* alldevs;
	//������Ϣ������
	char errbuf[PCAP_ERRBUF_SIZE];		
	//��¼ѡ�еĽӿ���Ϣ
	pcap_if_t *d;
	//ָ��ӿڵ�ַ�б�
	pcap_addr_t a;
	//��Ӧ�����������Ϣ������
	afx_msg void OnLbnDblclkList1();		//ѡ����Ӧ�Ľӿ���Ӧ����
	afx_msg void OnBnClickedCancel();		//�˳�
	afx_msg void OnBnClickedButton1();		//����
	afx_msg void OnBnClickedButton2();		//ֹͣ����
	//��¼�򿪵�pcap�ӿ�
	pcap_t* opened_pcap;
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
	// IPͷ�������㷨
	WORD IPHeader_ckeck(WORD*  IPHeader);
};


//֡���ݣ�IP���ݰ����ݽṹ����
#pragma pack(1)
typedef struct FrameHeader_t{		//֡�ײ�
	BYTE DesMAC[6];					//Ŀ�ĵ�ַ
	BYTE SrcMAC[6];					//ԭ��ַ
	WORD FrameType;					//֡����
}FrameHeader_t;

typedef struct IPHeader_t {
	BYTE Ver_HLen;
	BYTE TOS;
	WORD TotalLen;
	WORD ID;
	WORD Flag_Segment;
	BYTE TLL;
	BYTE Protocol;
	WORD Checksum;
	ULONG SrcIP;
	ULONG DstIP;
}IPHeader_t;

typedef struct Data_t {
	FrameHeader_t FrameHeader;
	IPHeader_t  IPHeader;
}Data_t;

#pragma pack()