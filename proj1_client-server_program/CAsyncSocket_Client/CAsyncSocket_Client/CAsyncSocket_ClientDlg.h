
// CAsyncSocket_ClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxsock.h"
#include "Client_Socket.h"

// CCAsyncSocket_ClientDlg �Ի���
class CCAsyncSocket_ClientDlg : public CDialogEx
{
// ����
public:
	CCAsyncSocket_ClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CASYNCSOCKET_CLIENT_DIALOG };
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
	afx_msg void OnBnClickedOk();
	CComboBox IP_ctrl;
	CString IP_m;
	CString command_m;
	CComboBox port_ctrl;
	CComboBox command_ctrl;
	CString response_m;
	// �ͻ���socket
	Client_Socket Client;   
	CString m_client_port;
	CString m_client_address;
	CString port_m;
	CComboBox m_client_port_ctrl;
	CListBox command_log_ctrl;
	CString m_host_name;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	// ��¼�ط�����
	int resend_count;
	// ��¼�ط���������
	int resend_limit;
	// ���ͱ��ĺ���
	LRESULT my_send(WPARAM wParam = 0, LPARAM lParam = 0);
	//��ȡʱ�亯��
	CString getDateTime();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
