#include "stdafx.h"
#include "Client_Socket.h"
#include "CAsyncSocket_ClientDlg.h"

Client_Socket::Client_Socket()
{
}


Client_Socket::~Client_Socket()
{
}


void Client_Socket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CCAsyncSocket_ClientDlg* pDlg = (CCAsyncSocket_ClientDlg*)(AfxGetApp()->GetMainWnd());
	TCHAR  lBuffer[4096](L"");
	CString Server_IP;
	UINT Server_Port;
	int m_length = sizeof(lBuffer);
	m_length = ReceiveFrom(lBuffer, m_length, Server_IP, Server_Port, 0);
	/*CString str;
	str.Format(L"%s,%s,%d", lBuffer, Server_IP, Server_Port);
	pDlg->MessageBox(str);*/
	if (m_length != -1)
	{
		pDlg->response_m = lBuffer;
		pDlg->UpdateData(false);
		pDlg->KillTimer(1);		//�ɹ�������Ϣ���رռ�ʱ��
		pDlg->resend_count = 0;	//�����ط�������Ϊ0
		CString str;
		str.Format(L":���� IP:%s Port: %d ��Ӧ��%s��" , Server_IP  , Server_Port , lBuffer );
		pDlg->command_log_ctrl.InsertString(0, pDlg->getDateTime() + str);
	}
	else
	{
		pDlg->command_log_ctrl.InsertString(0, pDlg->getDateTime() + L"ERROR:δ���յ��ش����ģ����ط�����������ã�");
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

