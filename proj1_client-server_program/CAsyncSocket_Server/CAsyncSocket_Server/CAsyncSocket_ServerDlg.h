
// CAsyncSocket_ServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "Server_Socket.h"

// CCAsyncSocket_ServerDlg �Ի���
class CCAsyncSocket_ServerDlg : public CDialogEx
{
// ����
public:
	CCAsyncSocket_ServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CASYNCSOCKET_SERVER_DIALOG };
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
	CListBox list_ctrl;
	Server_Socket  Server;
	// �������׽��ֶ˿ں�
	CComboBox server_port_ctrl;
	CString server_address;
	CString server_hostname;
	CString server_port;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
