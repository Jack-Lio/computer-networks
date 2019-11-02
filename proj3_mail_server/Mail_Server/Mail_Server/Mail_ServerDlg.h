
// Mail_ServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "SMTP_Socket.h"
#include "afxtempl.h"

//�Զ������ݽṹ��¼�ʼ���Ϣ
typedef struct mail_info {
	CList<CString>  mail_text_line;	//��¼ÿһ���е��ı�
	CString get_mail_text() {
		CString text;
		POSITION pos = mail_text_line.GetHeadPosition();
		for (int i = 0;POSITION( pos + i) != mail_text_line.GetTailPosition(); i++)
		{
			text += mail_text_line.GetAt(pos + i);
		}
		return text;
	}
}mail_t;

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
	//����SMTP������Socket
	SMTP_Socket SMTP_Server;
	// ��ʾ��־��Ϣ
	CListBox log_list_ctrl;
	// ��¼���յ����ʼ�����,mail_tΪ�Զ���Ľṹ���ͣ���¼�ռ��˺ͷ������������Ϣ
	CList<mail_t*> mail_list;
	afx_msg void OnDestroy();
	// ת���ʼ���Ŀ��SMTP���������߳�
	CWinThread* m_mail_sender;
};


