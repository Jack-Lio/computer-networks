
// FTP_ServerDlg.h : ͷ�ļ�

#pragma once
#include "afxwin.h"
#include "ServerSocket.h"

// CFTP_ServerDlg �Ի���
class CFTP_ServerDlg : public CDialogEx
{
// ����
public:
	CFTP_ServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTP_SERVER_DIALOG };
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
	//�������ӹ����׽���
	ServerSocket*  CNSocket;

	// ����IP��ַ��ʾ
	CEdit hostIPCtrl;
	// �����˿ڻ�ȡ
	CEdit hostPortCtrl;
	// ��ʾ��־
	CEdit logCtrl;
	//�����־��Ϣ����¼֮ǰ�������¼��ʱ��
	void log(CString logInfo);

	//���������������߳�
	afx_msg void OnBnClickedStartserver();
	// �����˿�����
	CString hostPort;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};



