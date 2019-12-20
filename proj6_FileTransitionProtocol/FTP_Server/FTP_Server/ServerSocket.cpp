#include "stdafx.h"
#include "ServerSocket.h"
#include  "FTP_ServerDlg.h"

#pragma warning(disable : 4996)

typedef struct DownloadFile_t {
	WORD DataID;			//�ļ�ID
	CString filename;		//�ļ���
	ServerSocket* server;	//�׽��֣���ȡSEQ��ACK����
	CString toIP;
	UINT port;
} DownloadFile_t;

ServerSocket::ServerSocket()
{
	//��ʼ����ʱ��ID��
	for (int i = 0; i < TIMERLIMITNUM; i++)
	{
		sendTimerLimit[i] = i;
	}
	//��ʼ��ȷ�����кźͷ������к�
	Ack = 0;
	Seq = 0;
	sameTimes = 0;
	recvACK = 0;
	recvSEQ = 0;
	status = start;
	currentDir = ".";
	//��ʼ��һ��test�û�
	UserInfo_t * test = new UserInfo_t;
	test->userName = "test";
	test->password = "test";
	UserInfo_list.AddTail(test);
}


ServerSocket::~ServerSocket()
{
}
//��������������Ӧ������
/*
���
USER �����û���
PASS �����û�����
AUTH ע��
LIST ��ȡԶ�̷������û�Ŀ¼�������ļ��б���Ӳ�����Ϊ���ض��ļ���
STOR �ϴ������ļ���Զ��Ŀ¼�ļ�ʱĿ¼��
RETR ��ȡԶ��Ŀ¼�µ��ļ���֧�ֶ��ļ�ͬʱ����
MDIR ��Զ���û�Ŀ¼�´����ļ���
DELE ��Զ���û�Ŀ¼��ɾ���ļ��л��ļ�

��Ӧ��
100 ��ȡ��������ɹ�
200 �����ɹ���Ӧ
404 Ŀ����󲻴���
500 ����ʧ����Ӧ
332 ��֤ͨ��
333 ע��ɹ��������û�Ŀ¼
334 �������
335 ��ȡĿ¼�ɹ�
*/

void ServerSocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	// TODO: �ڴ����ר�ô����/����û���
	if (status == finished)													//�Ͽ����Ӻ��ٽ�����Ӧ
		return;
	CFTP_ServerDlg*  Dlg = (CFTP_ServerDlg*)(AfxGetApp()->GetMainWnd());
	//���ý��ջ�����
	const int BUFFERLEN = 4096;
	u_char buffer[BUFFERLEN];
	CString serverIP;		//IP 
	UINT serverPort;		//Port
							//��������
	int byteLen = ReceiveFrom(buffer, BUFFERLEN, serverIP, serverPort, 0);
	if (byteLen < 0)
	{
		Dlg->log("�����������ݽ��ճ���");
		return;
	}

	if (ChecksumCompute((unsigned short *)buffer, byteLen) != 0)			//������
	{
		Dlg->log("���ݰ�У��ͼ������");
		return;
	}
	//���ݽ��������������������
	FTPHeader_t* FTPH = (FTPHeader_t*)(buffer);					//FTP�ײ�

																//���ACK SEQ������־
	CString logText;
	logText.Format("recv ACK %d SEQ %d from IP:%s Port:%d", FTPH->ACKNO, FTPH->SEQNO, serverIP, serverPort);
	Dlg->log(logText);

	//�����ݱ�ͷ�������ж�
	if (FTPH->ACKNO == recvACK)									//�����ظ�ACK������
	{
		sameTimes++;
		if (sameTimes == 4)										//���յ������ظ�������ACK,��Ҫ�ش�֮����������ݰ�
		{
			//�ش�recvACK-Seq֮����������ݱ���,���ڻ������д�ͷ��β�ķ������кŵ����Ҿ�С��SEQ������ȫ����Ҫ�ش�
			POSITION pos = sendPKT_list.GetHeadPosition();
			while (pos != NULL)
			{
				SendPacket_t* item = sendPKT_list.GetAt(pos);
				FTPHeader_t* header = ((FTPHeader_t*)(&item->PktData));
				if (header->SEQNO >= recvACK || header->SEQNO <= Seq)
				{
					this->SendTo(item->PktData, item->len, item->TargetPort, item->TargetIP, 0);
					CString logText;
					logText.Format("������������ACK���ش�����: SEQ = %d", header->SEQNO);
					Dlg->log(logText);
					item->ResendTime++;			//�ش�������¼����1
				}
				sendPKT_list.GetNext(pos);
			}
		}
	}
	else  if (((FTPHeader_t*)(sendPKT_list.GetHead()->PktData))->SEQNO == FTPH->ACKNO - 1)	//�������ACK,���ڻ������е�SEQ�������У����ֻ��Ҫ�ȶԻ������е�һ������
	{
		CString logText;
		logText.Format("�ӻ������Ƴ�����: SEQ = %d", ((FTPHeader_t*)(sendPKT_list.GetHead()->PktData))->SEQNO);
		Dlg->log(logText);
		//�Ƴ�����
		backTimerID(sendPKT_list.GetHead()->Timer);
		sendPKT_list.RemoveHead();
		
	}
	else if (((FTPHeader_t*)(sendPKT_list.GetHead()->PktData))->SEQNO < Ack)			//�ظ�����
	{
		sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK );//����FIN|ACK����
		return;
	}
	recvACK = FTPH->ACKNO;		//���½��յ��µ�ACK��ţ����¼�������ACK
	sameTimes = 1;
	Ack = FTPH->SEQNO + 1;										//ACK����

	u_char* Data = (u_char*)(buffer + sizeof(FTPHeader_t));		//FTP���ݲ���
	switch (FTPH->Flags)
	{
	case SYN:						//�������������Ӧ
	{
		if (status == start)
		{//����һ���������ݰ��ṹ��
			u_char res[256] = "OK Server is ready!";
			int res_len = strlen((char*)(res));
			//���·������к�
			Seq += res_len;
			//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
			sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, SYN | ACK);
			//��־��¼ 
			CString logText;
			logText.Format("recv '%s' from IP:%s Port:%d",
				((CString)Data).Left(byteLen - sizeof(FTPHeader_t)),
				serverIP, serverPort);
			Dlg->log(logText);
			status = isConnecting;
		}
		break;
	}
	case FIN | ACK: {
		//���ܵ����������Ӧ
		if (this->status == sendedFIN)			//���͹����ӶϿ����������Ͽ�����
		{
			sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);
			Dlg->log("���յ����ӶϿ���Ӧ���ر����ӣ�");
			this->status = finished;
		}
		if (this->status == isTransfer || this->status == isCommunicating)           //�����Ͽ�����
		{
			sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);		//����ACK
			Sleep(200);
			sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK | FIN);//����FIN|ACK����
			this->status = recvedFIN;
			Dlg->log("���յ����ӶϿ�����,׼���Ͽ����ӣ�");
		}
		break;
	}
	case RST | ACK:
	{
		//���յ�ǿ�Ʋ�����ӱ��ģ�����������е�������ݹر��׽���
		Dlg->log("���յ�������������ֱ�ӹر����ӣ�");
		this->status = finished;			//������״ֱ̬������Ϊ�Ͽ�״̬
		break;
	}
	case  ACK:
	{
		//���յ�������Ӧ���ģ��������״̬������Ӧ�Ĵ���
		if (this->status == sendedFIN)					//����FIN����ֻ��ӦFIN����
		{
			Dlg->log("���յ�FIN���ĵ�ACK��Ӧ");
			return;
		}
		else if (this->status == recvedFIN)				//�Ѿ����յ�FIN���ģ����ACK��Ӧ���ر�����
		{
			this->status = finished;
			Dlg->log("���յ�FIN���ĵ�ACK��Ӧ,�ر����ӣ�");
			return;
		}
		//����������Ӧ�Ĵ���
		if (status == isConnecting)
		{
			//����һ��ACK���ģ�ȷ�����յ���������ͬ�⽨������
			sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
			//��־��¼ 
			Dlg->log(((CString)Data).Left(byteLen - sizeof(FTPHeader_t)));
			status = isCommunicating;
			Dlg->log("��ͻ��˽�������,�ȴ�������֤��Ϣ��");
		}
		if (status == isCommunicating)
		{
			if (byteLen == sizeof(FTPHeader_t))	//����ָ����Ϣ��ΪACK���ģ�����Ҫ����Ӧ
			{
				break;   
			}
			CString command = CString(Data);

			if (command.Left(4) == "LOGI")		//��Ȩ��Ϣ
			{
				int usernamebegin = command.Find(" ",0);
				int usernameend = command.Find(" ", usernamebegin + 1);
				CString username = command.Mid(usernamebegin + 1, usernameend-usernamebegin-1);
				int passend = command.Find("\r\n", usernameend);
				CString password = command.Mid(usernameend + 1, passend-usernameend-1);
				Dlg->log("��ȡ�û����͵�¼���룺userName:" + username + " password:" + password);
				//��֤�����ע�ᴦ��
				if (UserInfo_list.GetCount() == 0) //û���û�����
				{
					//ֱ��д�룬��Ϊע��ɹ�
					u_char res[256] = "333 register successful";
					int res_len = strlen((char*)(res));
					//���·������к�
					Seq += res_len;
					//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
					sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
					currentDir = currentDir + "/" + username;
					makeDir(currentDir);
					return;
				}
				else
				{
					//��Ϊ�գ�ѭ�����������Ƿ����ƥ��
					POSITION pos = UserInfo_list.GetHeadPosition();
					while (pos != NULL) {
						if (UserInfo_list.GetAt(pos)->userName == username)
						{
							if (UserInfo_list.GetAt(pos)->password == password)
							{
							//ƥ��ɹ�
								u_char res[256] = "332 login successful";
								int res_len = strlen((char*)(res));
								//���·������к�
								Seq += res_len;
								//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
								sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
								currentDir = currentDir + "/" + username;
								//makeDir(currentDir);
								return;
							}
							else
							{
								//ƥ��ʧ��
								u_char res[256] = "334 login failed";
								int res_len = strlen((char*)(res));
								//���·������к�
								Seq += res_len;
								//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
								sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
								return;
							}
						}
						UserInfo_list.GetNext(pos);
					}
					//�����ڶ�Ӧ���û���
					//����µ��û���������
					UserInfo_t * test = new UserInfo_t;
					test->userName = username;
					test->password = password;
					UserInfo_list.AddTail(test);
					//ֱ��д�룬��Ϊע��ɹ�
					u_char res[256] = "333 register successful";
					int res_len = strlen((char*)(res));
					//���·������к�
					Seq += res_len;
					//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
					sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
					currentDir = currentDir + "/" + username;
					makeDir(currentDir);
					return;
				}
				break;
			}

			if (command.Left(4) == "LIST")			//���ļ���
			{
				int dirbegin = command.Find(" ", 0);
				int dirend = command.Find("\r\n", dirbegin + 1);
				CString dir = command.Mid(dirbegin + 1, dirend-dirbegin-1);
				if (dir == ".")
					currentDir = currentDir;
				else if (dir == "..")
				{
					;
				}
				else 
					currentDir = currentDir + "/" + dir;
				CString dirInfo = getDirInfo(currentDir);
				//��Ӧ����
				u_char res[1472] = "335 ";
				strcat((char*)res, dirInfo+"\r\n");
				int res_len = strlen((char*)(res));
				//���·������к�
				Seq += res_len;
				//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
				sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
				break;
			}

			if (command.Left(4) == "RETR")
			{
				//Dlg->MessageBox(command.Left(byteLen-sizeof(FTPHeader_t)));
				//����һ��ACK���ģ�ȷ�����յ���������ͬ�⽨������
				sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
				int count = 1;
				int pos = command.Find(" ", 0)+1;
				while (pos < (byteLen - sizeof(FTPHeader_t)))
				{
					int nextpos = command.Find("\r\n", pos);
					CString file;
					file = command.Mid(pos, nextpos - pos);
					pos = nextpos + 2;
					//Dlg->MessageBox(file);
					DownloadFile_t *d = new DownloadFile_t;
					d->DataID = count++;
					d->filename = currentDir+"/"+file; //�ļ�����Ŀ¼
					d->server = this;
					d->toIP = serverIP;
					d->port = serverPort;
					CWinThread *pThread = AfxBeginThread(			//���������ļ��߳�
						dataThread,
						d,
						THREAD_PRIORITY_NORMAL,
						0,
						CREATE_SUSPENDED
						);
					pThread->ResumeThread();
				}
				
			}


			if (command.Left(4) == "DELE")			//ɾ���ļ��л��ļ���
			{
				int dirbegin = command.Find(" ", 0);
				int dirend = command.Find("\r\n", dirbegin + 1);
				CString dir = command.Mid(dirbegin + 1, dirend - dirbegin - 1);
			
				bool flag  = RecycleFileOrFolder(currentDir + "/" + dir);
				if (flag) {
					//��Ӧ����
					u_char res[1472] = "200 dir delete successful";
					int res_len = strlen((char*)(res));
					//���·������к�
					Seq += res_len;
					//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
					sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
					break;
				}
				else
				{
					//��Ӧ����
					u_char res[1472] = "500 dir delete failed";
					int res_len = strlen((char*)(res));
					//���·������к�
					Seq += res_len;
					//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
					sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
					break;
				}
			}

			if (command.Left(4) == "MDIR")			//�����ļ���
			{
				int dirbegin = command.Find(" ", 0);
				int dirend = command.Find("\r\n", dirbegin + 1);
				CString dir = command.Mid(dirbegin + 1, dirend - dirbegin - 1);

				bool flag = makeDir(currentDir + "/" + dir);
				if (flag) {
					//��Ӧ����
					u_char res[1472] = "200 dir make successful";
					int res_len = strlen((char*)(res));
					//���·������к�
					Seq += res_len;
					//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
					sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
					break;
				}
				else
				{
					//��Ӧ����
					u_char res[1472] = "500 dir make failed";
					int res_len = strlen((char*)(res));
					//���·������к�
					Seq += res_len;
					//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
					sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
					break;
				}
			}

			if (command.Left(4) == "EXIT")			//�����ļ���
			{
			//��Ӧ����
				u_char res[1472] = "501 out successful";
				int res_len = strlen((char*)(res));
				//���·������к�
					Seq += res_len;
					//����һ��SYN|ACK���ģ�ȷ�����յ���������ͬ�⽨������
					sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack,ACK);
					//��ʼ����ʱ��ID��
					for (int i = 0; i < TIMERLIMITNUM; i++)
					{
						sendTimerLimit[i] = i;
					}
					//��ʼ��ȷ�����кźͷ������к�
					Ack = 0;
					Seq = 0;
					sameTimes = 0;
					recvACK = 0;
					recvSEQ = 0;
					status = start;
					currentDir = ".";
					//��ʼ��һ��test�û�
					UserInfo_t * test = new UserInfo_t;
					test->userName = "test";
					test->password = "test";
					UserInfo_list.AddTail(test);
					Dlg->log("�û��˳��ɹ���");
					break;
			}
		}
		break;
	}
	default: {

		Dlg->log("�޷��������ģ�");
	}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

UINT_PTR ServerSocket::getTimerID()
{
	UINT_PTR ID = 0;
	for (int i = 1; i < TIMERLIMITNUM; i++)
	{
		if (sendTimerLimit[i] != 0)
		{
			ID = sendTimerLimit[i];
			sendTimerLimit[i] = 0;
			break;
		}
	}
	assert(ID >= 1 || ID == 0);
	assert(ID < TIMERLIMITNUM);
	return ID;
	return UINT_PTR();
}

void ServerSocket::backTimerID(UINT_PTR ID)
{
	CFTP_ServerDlg*  Dlg = (CFTP_ServerDlg*)(AfxGetApp()->GetMainWnd());
	if (ID >= TIMERLIMITNUM) return;
	assert(ID >= 1);
	assert(ID < TIMERLIMITNUM);
	Dlg->KillTimer(ID);
	sendTimerLimit[ID] = ID;
}
