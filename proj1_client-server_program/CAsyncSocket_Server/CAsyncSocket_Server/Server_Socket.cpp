#include "stdafx.h"
#include "Server_Socket.h"
#include "CAsyncSocket_ServerDlg.h"

Server_Socket::Server_Socket()
{
}


Server_Socket::~Server_Socket()
{
}

void Server_Socket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	TCHAR lBuffer[4096](L"");   //������Ҫ���г�ʼ��������ᱨ
	int m_length = sizeof(lBuffer);        // �������ĳ��� 
	CString Client_IP ;						//IP��ַ
	UINT Client_port;						// �˿ں�
	// ��ȡ�Ի�����
	CCAsyncSocket_ServerDlg* pDlg  = (CCAsyncSocket_ServerDlg*)(AfxGetApp()->GetMainWnd());
	// ��ȡ����,����ֵΪ���ձ��ĵĳ���
	m_length = ReceiveFrom(lBuffer , 8192, Client_IP, Client_port, 0);
	//��ȡ����ʱ�䲢תΪ�ַ�����ʽ
	CString log;
	CTime tm; tm = CTime::GetCurrentTime();
	CString date, time;
	date.Format(L"%d/%d/%d", tm.GetYear(), tm.GetMonth(), tm.GetDay());
	time.Format(L"%d:%d:%d", tm.GetHour(), tm.GetMinute(), tm.GetSecond());
	
	if (m_length!=-1)		// ������
	{
		CString command = CString(lBuffer).MakeLower();
		CString response; 
		if (command == L"date")
		{
			response=date;
		}
		else if (command == L"time")
		{
			response = time;
		}
		else
		{
			response.Format(L"��������");
		}
		// ���´���ش���Ӧ����������жϻش��Ƿ�ɹ���������ɹ������¼������־
		if (SendTo(response.GetBuffer(),
			response.GetLength()*sizeof(TCHAR),
			Client_port,
			Client_IP))
		{
			log.Format(L"%s %s:�յ�IP=%s Port=%d ����%s��,��Ӧ��%s��", date, time, Client_IP, Client_port, lBuffer, response);
		}
		else
			log.Format(L"ERROR������Ϣ�ش�����%s %s:�յ�IP=%s Port=%d ����%s��,��Ӧ��%s��", date, time, Client_IP, Client_port, lBuffer, response);
		pDlg->list_ctrl.InsertString(0, log);
	}
	else
	{
		pDlg->MessageBox(L"���ձ��ĳ��ִ���");
		log.Format(L"ERROR�������ձ��Ĵ���%s %s:�յ�IP=  Port=   ���󡾡�,��Ӧ����", date, time);
		pDlg->list_ctrl.InsertString(0, log);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}
