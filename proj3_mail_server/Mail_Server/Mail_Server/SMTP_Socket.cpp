#include "stdafx.h"
#include "SMTP_Socket.h"
#include "Mail_ServerDlg.h"
#include "base64.h"

SMTP_Socket::SMTP_Socket()
{
	begin_data_recv = false;
}


SMTP_Socket::~SMTP_Socket()
{
}


void SMTP_Socket::OnAccept(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	//��ȡ�����ھ��
	CMail_ServerDlg* Dlg = (CMail_ServerDlg*)AfxGetApp()->m_pMainWnd;
	//��¼��־
	Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), L"***�յ���������");
	//�����������ӵ��׽���
	SMTP_Socket * pSocket = new SMTP_Socket();
	if (Accept(*pSocket))
	{
		char* t = "220 Simple Mail Server Ready for Mail\r\n";
		pSocket->Send(t, strlen(t));
		pSocket->AsyncSelect(FD_READ);			//���ý��պ������ջ�Ӧ
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), L"***��������");
		//��¼��־
		Dlg->log_list_ctrl.InsertString(
			Dlg->log_list_ctrl.GetCount(),
			L"S:220 Simple Mail Server Ready for Mail");
	}
	else
	{
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), L"***���Ӵ���");
	}
	CAsyncSocket::OnAccept(nErrorCode);
}



void SMTP_Socket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	//��ȡ��������
	CMail_ServerDlg* Dlg = (CMail_ServerDlg*)AfxGetApp()->m_pMainWnd;
	memset(lbuf, 0, bufferlen);
	//���ձ���
	int len = Receive(lbuf, strlen(lbuf),0);
	if (len == -1)		//������
	{
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), L"***������Ϣ����");
		return;
	}
	//ת����ʽ
	CString recv_info;
	recv_info += lbuf;
	//���û�п�ʼ�������ݣ�������յ������������Ӧ
	if (!begin_data_recv)
	{
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), L"C:" + recv_info.Left(len));
		if (recv_info.Left(4) == "EHLO")
		{
			//���ͻ�Ӧ
			char *ret= "250 192.168.56.1\r\n";
			Send(ret, strlen(ret));
			//�������ձ�����Ӧ����
			AsyncSelect(FD_READ);
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				L"S:" + (CString)ret);
			return;
		}
		else if (recv_info.Left(4)=="NOOP")
		{
			char *ret = "250 OK No Operation\r\n";
			Send(ret, strlen(ret));
			//�������ձ�����Ӧ����
			AsyncSelect(FD_READ);
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				L"S:" + (CString)ret);
			return;
		}
		else if (recv_info.Left(10) == "MAIL FROM:")
		{
			char *ret = "250 Sender OK\r\n";
			Send(ret, strlen(ret));
			//�������ձ�����Ӧ����
			AsyncSelect(FD_READ);
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				L"S:" + (CString)ret);
			return;
		}
		else if ( recv_info.Left(8) == "RCPT TO:")
		{
			char *ret = "250 Receiver OK\r\n";
			Send(ret, strlen(ret));
			//�������ձ�����Ӧ����
			AsyncSelect(FD_READ);
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				L"S:" + (CString)ret);
			return;
		}
		else if (recv_info.Left(4) == "QUIT"| recv_info.Left(4)=="RSET")
		{

			char *ret = "221 Quit, Goodbye\r\n";
			Send(ret, strlen(ret));
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				L"S:" + (CString)ret);
			//�ر�����
			Close();
			return;
		}
		else  if (recv_info.Left(4) == "DATA")
		{
			if (!begin_data_recv)
			{
				//��ʼ�������ݣ����ñ�־λ
				begin_data_recv = true;		
			   //��յ�ǰ�ı������ڵ��ı��ռ�
				Dlg->mail_text_ctrl.SetWindowTextW(L"");
			}
			//������Ӧ
			char *ret = "354 Go ahead.End with <CRLF>.<CRLF>\r\n";
			Send(ret, strlen(ret));
			//�������ձ�����Ӧ����
			AsyncSelect(FD_READ);
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				L"S:" + (CString)ret);
			return;
		}
		else
		{
			char *ret = "500 order is wrong\r\n";
			Send(ret, strlen(ret));
			//�������ձ�����Ӧ����
			AsyncSelect(FD_READ);
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				L"S:" + (CString)ret);
			//�ر�����
			Close();
			return;
		}
	}else    //�������ݱ��Ĳ���ʾ���ʼ���ʾ��
	{
		//��ʾ�յ����ʼ���������
		CString mail_text;
		Dlg->mail_text_ctrl.GetWindowTextW(mail_text);
		mail_text += recv_info.Left(len);
		Dlg->mail_text_ctrl.SetWindowTextW(mail_text);

		if (recv_info.Find(_T("\r\n.\r\n")) != -1)
		{
			begin_data_recv = false;
			char *ret = "250 Message accepted for delivery\r\n";
			Send(ret, strlen(ret));
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				L"S:" + (CString)ret);
			////���Ѿ����յ����ʼ��������д�����У�ʵ�ּ�¼����ʼ��Ĺ���
			//int linecount = Dlg->mail_text_ctrl.GetLineCount();
			//mail_t* temp = new mail_t;
			//for (int i = 0; i < linecount; i++)
			//{
			//	TCHAR line[bufferlen];
			//	Dlg->mail_text_ctrl.GetLine(i, line);
			//	temp->mail_text_line.AddTail(line);
			//}
			//Dlg->mail_list.AddTail(temp);
		}
		//�������ձ�����Ӧ����
		AsyncSelect(FD_READ);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}
