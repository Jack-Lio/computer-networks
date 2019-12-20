#include "stdafx.h"
#include "ConnectSocket.h"
#include	"FTP_ClientDlg.h"

//���� inet_ntoa �������ñ���
#pragma warning(disable : 4996)

ConnectSocket::ConnectSocket()
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
	status = 0;
	userID = 0;
}


ConnectSocket::~ConnectSocket()
{
}

//��������������Ӧ������
/*
���
LOGI ��¼/ע��
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
336 ������׼�����ݴ���
*/

//�����׽������ݽ��շ�������
void ConnectSocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (status == finished)													//�Ͽ����Ӻ��ٽ�����Ӧ
		return;	
	if (files_list.GetCount() == 0&&status==isTransfer)						//���ݽ�����ϣ������µ����ݣ�Ӧ�ûص������ģʽ
	{
		status = isCommunicating;
	}
	CFTP_ClientDlg*  Dlg = (CFTP_ClientDlg*)(AfxGetApp()->GetMainWnd());
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
	logText.Format("recv ACK %d SEQ %d", FTPH->ACKNO, FTPH->SEQNO);
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
				if (header->SEQNO >= recvACK || header->SEQNO<= Seq)
				{
					this->SendTo(item->PktData,item->len, item->TargetPort, item->TargetIP,0);
					CString logText;
					logText.Format("������������ACK���ش�����: SEQ = %d", header->SEQNO);
					Dlg->log(logText);
					item->ResendTime++;			//�ش�������¼����1
				}
				sendPKT_list.GetNext(pos);
			}
		}
	}
	else  if(((FTPHeader_t*)(sendPKT_list.GetHead()->PktData))->SEQNO == FTPH->ACKNO -1)	//�������ACK,���ڻ������е�SEQ�������У����ֻ��Ҫ�ȶԻ������е�һ������
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
	case SYN | ACK:
	{
		//���ܵ�����������Ӧ
		if (status == isConnecting)			//���յ�����������������Ӧ
		{
			//���淵�ص��û�ID
			this->userID = FTPH->UserID;
			//�޸�����״̬Ϊ����ͨ��״̬
			this->status = isCommunicating;
			//����һ���������ݰ��ṹ��
			u_char res[256] = "OK Client is ready!";
			int res_len = strlen((char*)(res));
			//���·������к�
			Seq += res_len;
			//����һ��ACK���ģ�ȷ�����յ����ӷ�����Ӧ��ȷ�Ͻ�������
			sendFTPPacket(this, serverIP, serverPort, res, res_len, userID, 0, Seq, Ack, ACK);
			//��־��¼ 
			Dlg->log(((CString)Data).Left(byteLen - sizeof(FTPHeader_t)));
		}
		break;
	}
	case FIN | ACK:{
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
			sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK|FIN);//����FIN|ACK����
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
		else if (this->status == isCommunicating)
		{
			if (byteLen == sizeof(FTPHeader_t))	//����ָ����Ϣ��ΪACK���ģ�����Ҫ����Ӧ
			{
				break;
			}
			CString command = CString(Data);
			if (command.Left(3) == "332")		//��֤ͨ������¼�ɹ�
			{
				Dlg->log("�ɹ���½������");
				Dlg->logined = true;
				sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
				break;
			}
			if (command.Left(3) == "333")		//ע��ɹ�����Ŀ¼������
			{
				Dlg->log("�����ڸ��û������Զ�ע��ɹ���");
				Dlg->logined = true;
				sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
				break;
			}
			if (command.Left(3) == "334")		//�������
			{
				Dlg->log("�û��������벻ƥ�䣬�����������û��������룡");
				sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
				break;
			}
			if (command.Left(3) == "200")		//�����ɹ�
			{
				Dlg->log("�����ɹ���");
				sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
				break;
			}

			if (command.Left(3) == "500")		//����ʧ��
			{
				Dlg->log("����ʧ��");
				sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
				break;
			}

			if (command.Left(3) == "335")		//��ȡĿ¼�ɹ�
			{
				Dlg->log("Զ��Ŀ¼��ȡ�ɹ���");
				//Dlg->MessageBox(command.Left(byteLen-sizeof(FTPHeader_t)));
				Dlg->displayFileTree(command.Mid(4,byteLen - sizeof(FTPHeader_t)-6));
				sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
				break;
			}

			if (command.Left(3) == "501")		//�˳�
			{
				Dlg->log("�˳��ɹ���");
				status = start;
				sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
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
				status = 0;
				userID = 0;
				break;
			}

		}
		else if (status == isTransfer)			//���ݴ��䱨��
		{
			if (byteLen == sizeof(FTPHeader_t))	//����ָ�����ݶ���Ϣ��ΪACK���ģ�����Ҫ����Ӧ
			{
				break;
			}
			//���ݲ�ͬ��DataID,�����ݴ��벻ͬ��filecontent��
			POSITION pos = files_list.GetHeadPosition();
			while (pos != NULL)
			{
				if (files_list.GetAt(pos)->DataID == FTPH->DataID)			//���ݱ�ǩƥ��
				{
					//���ݽ�βΪ\r\n\r\n
					if (CString(Data).Left(byteLen - sizeof(FTPHeader_t)) == "\r\n\r\n\r\n\r\n")
					{
						if(writeFile("./test/"+files_list.GetAt(pos)->filename,
							files_list.GetAt(pos)->filecontent,
							files_list.GetAt(pos)->len))
						{
							//���ļ�ɾ��
							Dlg->log(files_list.GetAt(pos)->filename + "�ļ����سɹ���");
							files_list.RemoveAt(pos);
							break;
						}
						else {
							//д�ļ�����Ҳɾ�����ص��ļ�
							Dlg->log(files_list.GetAt(pos)->filename + "д�ļ�����");
							files_list.RemoveAt(pos);
							break;
						}
					}
					//�����ݰ�˳����룬���ڷ��Ͷ��ǰ����ͣ����ն�Ҳ�ǰ�����գ����ֻ��Ҫֱ�Ӻϲ�����
					copyData(Data, files_list.GetAt(pos)->filecontent + files_list.GetAt(pos)->len, byteLen - sizeof(FTPHeader_t));
					files_list.GetAt(pos)->len += byteLen - sizeof(FTPHeader_t);
					break;
				}
				files_list.GetNext(pos);
			}
			sendFTPPacket(this, serverIP, serverPort, new u_char, 0, userID, 0, Seq, Ack, ACK);//����ACK����
		}


		break;
	}
	default: {

		Dlg->log("�޷��������ģ�");
	}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

//��ȡ��ʱ����ID
UINT_PTR ConnectSocket::getTimerID() {
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
	assert(ID >= 1||ID==0);
	assert(ID < TIMERLIMITNUM);
	return ID;
}

//���ն�ʱ��ID
void ConnectSocket::backTimerID(UINT_PTR ID) {
	CFTP_ClientDlg*  Dlg = (CFTP_ClientDlg*)(AfxGetApp()->GetMainWnd());
	if (ID >= TIMERLIMITNUM) return;
	assert(ID >= 1);
	assert(ID < TIMERLIMITNUM);
	Dlg->KillTimer(ID);
	sendTimerLimit[ID] = ID;
}

//������ʱ��
bool newTimer(UINT_PTR ID) {
	CFTP_ClientDlg*  Dlg = (CFTP_ClientDlg*)(AfxGetApp()->GetMainWnd());
	Dlg->SetTimer(ID, 5000, NULL);				//���ü�ʱ����ʱ��Ϊ5s
	return true;
}

//��ȡ����������ʱ����Ϣ���ַ�����ʽ��
void getDateTimeStr(CString&date, CString&time) {
	//��ȡ����ʱ�䲢תΪ�ַ�����ʽ
	CTime tm; tm = CTime::GetCurrentTime();
	date.Format("%d/%d/%d", tm.GetYear(), tm.GetMonth(), tm.GetDay());
	time.Format("%d:%d:%d", tm.GetHour(), tm.GetMinute(), tm.GetSecond());
}

//��ȡ������HostIP��ַ
CString getHostIPStr() {
	char temp[256];
	gethostname(temp, 256);											//��ȡ�ͻ�������
	hostent* host = gethostbyname(temp);							//�ͻ�����IP
	return inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);		//�ͻ���IP��ַתΪ�ַ������ͷ���
}

//��IP��ַתΪ�ַ�����ʽ
CString getIPStr(DWORD IP) {
	CString IPStr;
	u_char* ip_int = (u_char*)&IP;
	IPStr.Format("%d:%d:%d:%d", ip_int[0], ip_int[1], ip_int[2], ip_int[3]);
	return IPStr;
}

//���ֽڸ�������
void copyData(u_char* srcData, u_char*destData, int len) {
	assert(srcData != NULL&&destData != NULL);
	for (int i = 0; i < len; i++)
	{
		destData[i] = srcData[i];
	}
}

//�������ݰ�����
bool sendFTPPacket(CAsyncSocket* socket,			//�����׽���
	CString toIP,									//������IP
	UINT toPort,									//�������˿�
	u_char* sendData,								//���������׵�ַ
	int dataLen,									//�������ݳ���
	WORD userID ,									//�û�ID��ʶ
	WORD dataID ,									//�����ֶα�ʶ
	DWORD seq ,										//�������к�
	DWORD ack ,										//ȷ�����к�
	WORD flags)										//��ʶλ
{
	int totalLen = dataLen + sizeof(FTPHeader_t);
	assert(totalLen <= 1472);							//��֤������������ݳ���С��ʣ��Ĵ���ռ�1472 - 16 = 1456�ֽ�
	u_char* buffer = new u_char[totalLen];					//���ǵ�MTU�Ĵ�С����UDP�������ݵ�����С������1472�ֽ�����
	FTPHeader_t* FTPH = (FTPHeader_t*)(buffer);
	FTPH->UserID = userID;
	FTPH->DataID = dataID;
	FTPH->Flags = flags;
	FTPH->SEQNO = seq;
	FTPH->ACKNO = ack;
	copyData(sendData, (buffer + sizeof(FTPHeader_t)), dataLen);	//�������ݵ����ͻ�����
	FTPH->CheckSum = 0;												//�Ƚ�У�����λ0
	FTPH->CheckSum = ChecksumCompute((unsigned short*)buffer, totalLen);

	int flg = socket->SendTo(buffer, totalLen, toPort, toIP, 0);	//�������ݱ�
	if (flg < 0)
	{
		return false;
	}
	//������ݰ�ֻ�а�ͷ��û�����ݣ�������ӵ����ͻ�����
	if (totalLen - sizeof(FTPHeader_t) == 0)
	{
		return true;
	}
	//ÿ����һ�����ݰ�����Ҫ�����ݰ��������ݻ�����������Ӽ�ʱ��
	SendPacket_t* packet = new SendPacket_t;
	copyData(buffer,packet->PktData, totalLen);
	packet->len = totalLen;
	packet->TargetIP = toIP;
	packet->TargetPort = toPort;
	packet->Timer = ((ConnectSocket*)(socket))->getTimerID();
	packet->ResendTime = 0;												//�ط�������ʼ��Ϊ0
	((ConnectSocket*)(socket))->sendPKT_list.AddTail(packet);			//���뻺����	
	assert(newTimer(packet->Timer));									//��Ӽ�ʱ��
	return true;
}


// ����У���
unsigned short ChecksumCompute(unsigned short * buffer, int size)
{
	// 32λ���ӳٽ�λ
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer++;
		// 16λ���
		size -= sizeof(unsigned short);
	}
	if (size)
	{
		// �������е���8λ
		cksum += *(unsigned char *)buffer;
	}
	// ����16λ��λ������16λ
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	// ȡ��
	return (unsigned short)(~cksum);
}


//���ļ�����
bool readFile(CString filePath, u_char* &data, long long int&d_len)
{
	CFile file;
	file.Open(filePath, CFile::modeRead, NULL);
	long long int len = file.GetLength();
	u_char* Buf = new u_char[len];
	file.Read(Buf, len);
	data = new u_char[len];
	copyData(Buf, data, len);
	d_len = len;
	return  true;
}
//д�ļ�����
bool writeFile(CString filePath, u_char* content, long long int len)
{
	//AfxMessageBox(CString(content));
	assert(content != NULL);
	CFile file;

	file.Open(filePath, CFile::modeCreate | CFile::modeWrite, NULL);

	file.Write(content, len);

	file.Close();
	return true;
}