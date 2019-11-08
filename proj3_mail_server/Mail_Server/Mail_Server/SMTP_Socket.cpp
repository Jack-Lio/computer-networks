#include "stdafx.h"
#include "SMTP_Socket.h"
#include "Mail_ServerDlg.h"

SMTP_Socket::SMTP_Socket()
	: recv_post(_T(""))
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
	Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), "***�յ���������");
	//�����������ӵ��׽���
	SMTP_Socket * pSocket = new SMTP_Socket();
	if (Accept(*pSocket))
	{
		char* t = "220 Simple Mail Server Ready for Mail\r\n";
		pSocket->Send(t, strlen(t));
		pSocket->AsyncSelect(FD_READ);			//���ý��պ������ջ�Ӧ
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), "***��������");
		//��¼��־
		Dlg->log_list_ctrl.InsertString(
			Dlg->log_list_ctrl.GetCount(),
			"S:220 Simple Mail Server Ready for Mai");
	}
	else
	{
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), "***���Ӵ���");
	}
	CAsyncSocket::OnAccept(nErrorCode);
}




void SMTP_Socket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	//��ȡ��������
	CMail_ServerDlg* Dlg = (CMail_ServerDlg*)AfxGetApp()->m_pMainWnd;
	//���ձ���
	int len = Receive(lbuf, sizeof(lbuf),0);
	if (len == -1)		//������
	{
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), "***������Ϣ����");
		return;
	}
	else if (len == 0)
	{
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), "***���ձ���Ϊ��");
		return;
	}
	//ת����ʽ
	CString recv_info =(CString)lbuf;
	//���û�п�ʼ�������ݣ�������յ������������Ӧ
	if (!begin_data_recv)
	{
		Dlg->log_list_ctrl.InsertString(Dlg->log_list_ctrl.GetCount(), "C:" + recv_info.Left(len));
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
				"S:" + (CString)ret);
			return;
		}
		else if (recv_info.Left(4)== "NOOP")
		{
			char *ret = "250 OK No Operation\r\n";
			Send(ret, strlen(ret));
			//�������ձ�����Ӧ����
			AsyncSelect(FD_READ);
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				"S:" + (CString)ret);
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
				"S:" + (CString)ret);
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
				"S:" + (CString)ret);
			return;
		}
		else if (recv_info.Left(4) == "QUIT"||recv_info.Left(4)== "RSET")
		{

			char *ret = "221 Quit, Goodbye\r\n";
			Send(ret, strlen(ret));
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				"S:" + (CString)ret);
			//�ر�����
			this->Close();
			return;
		}
		else  if (recv_info.Left(4) == "DATA")
		{
			if (!begin_data_recv)
			{
				//��ʼ�������ݣ����ñ�־λ
				begin_data_recv = true;		
			   //��յ�ǰ�ı������ڵ��ı��ռ�
				Dlg->mail_text_ctrl.SetWindowTextA("");
			}
			//������Ӧ
			char *ret = "354 Go ahead.End with <CRLF>.<CRLF>\r\n";
			Send(ret, strlen(ret));
			//�������ձ�����Ӧ����
			AsyncSelect(FD_READ);
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				"S:" + (CString)ret);
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
				"S:" + (CString)ret);
			//�ر�����
			this->Close();
			return;
		}
	}else    //�������ݱ��Ĳ���ʾ���ʼ���ʾ��
	{	
		//���ն�����ӣ�����Ӧ�������ȶ������
		recv_post += recv_info.Left(len);

		if (recv_info.Find("\r\n.\r\n") != -1)
		{
			//������������
			begin_data_recv = false;
			//���������ӽ��յı����������ʾ���ĵ��ı�����
			Dlg->mail_text_ctrl.SetWindowTextA(recv_post);

			////�ӳٷ���ȷ���յ���Ϣ�����ڲ��Գ���Ĳ���ͨ��
			//srand((unsigned)time(NULL));
			//if((rand()%15)<3)   //����ӳ�
			//	Sleep(10000);

			//�������ݽ��ս�������Ӧ
			char *ret = "250 Message accepted for delivery\r\n";
			Send(ret, strlen(ret));
			//��¼��־
			Dlg->log_list_ctrl.InsertString(
				Dlg->log_list_ctrl.GetCount(),
				"S:" + (CString)ret);
			//�����Ĵ���ȫ�ֵı��Ļ������
			mail_t* cur_mail = new mail_t;
			Dlg->mail_text_ctrl.GetWindowTextA(cur_mail->mail_post);
			Dlg->mail_list.AddTail(cur_mail);
			//���½����ʼ���Ŀ
			CString cur;
			cur.Format("%d", Dlg->mail_list.GetCount());
			Dlg->cur_ctrl.SetWindowTextA(cur);
			Dlg->total_ctrl.SetWindowTextA(cur);
			Dlg->UpdateData(true);
			//������ʾ�ʼ�������Ϣ�������߳̽����ʼ�������������ʾ
			Dlg->PostMessage(WM_DISPLAYMAIL,0,0);
		}
		//�������ձ�����Ӧ����
		AsyncSelect(FD_READ);
	}
	//memset(lbuf, 0, sizeof(lbuf) / sizeof(char));
	CAsyncSocket::OnReceive(nErrorCode);
}
