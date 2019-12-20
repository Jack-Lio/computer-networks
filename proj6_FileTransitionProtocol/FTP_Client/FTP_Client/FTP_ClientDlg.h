
// FTP_ClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ConnectSocket.h"

// CFTP_ClientDlg �Ի���
class CFTP_ClientDlg : public CDialogEx
{
// ����
public:
	CFTP_ClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTP_CLIENT_DIALOG };
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
	//�ж��Ƿ��Ѿ���½
	bool logined;
	//�����������׽���
	ConnectSocket* CNSocket;
	// ��ʾ����IP��ַ
	CEdit hostIPCtrl;
	// ��־��ʾ
	CEdit logCtrl;
	//�����־��Ϣ����¼֮ǰ�������¼��ʱ��
	void log(CString logInfo);
	afx_msg void OnBnClickedButton1();
	// �����󶨶˿ں�
	CEdit hostPortCtrl;
	// ���������Ӷ˿�
	CEdit ServerPortCtrl;
	// ������IP��ַ��ȡ�ؼ�
	CIPAddressCtrl ServerIPCtrl;
	// �û�����ȡ�ؼ�
	CEdit userNameCtrl;
	// �����ȡ�ؼ�
	CEdit passwordCtrl;
	// �ļ�����ʾ�б�ؼ�
	CListBox FileTreeDisplayCtrl;
	// ��ǰ����·����ʾ�ؼ�
	CEdit currentPathCtrl;
	// �����ȡ�ؼ�
	CEdit commandCtrl;
	// ���������ѡ��ؼ�
	CEdit argvCtrl;
	afx_msg void OnBnClickedStartclient();
	//��ʾ�ļ�Ŀ¼
	void displayFileTree(CString files);
	// �ͻ����׽��ְ󶨶˿ں�
	CString hostPort;
	// ���������Ӷ˿ں�
	CString serverPort;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedConnectserver();
	afx_msg void OnBnClickedlogin();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedButton5();
};

