
// Mail_ServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CMail_ServerDlg �Ի���
class CMail_ServerDlg : public CDialogEx
{
// ����
public:
	CMail_ServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIL_SERVER_DIALOG };
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
	// ��ʾ�ʼ���Ϣ
	CEdit mail_text_ctrl;
	// ��־��ʾ
	CEdit log_list_ctrl;
};
