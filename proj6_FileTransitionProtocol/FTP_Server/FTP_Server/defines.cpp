#include "stdafx.h"
#include "defines.h"
#include "FTP_ServerDlg.h"
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
#include <afx.h>
#include "ServerSocket.h"


//���� inet_ntoa �������ñ���
#pragma warning(disable : 4996)

// �������
HANDLE hMutex = NULL;
//�������ص�Ԫ
typedef struct DownloadFile_t {
	WORD DataID;			//�ļ�ID
	CString filename;		//�ļ���
	ServerSocket* server;	//�׽��֣���ȡSEQ��ACK����
	CString toIP;
	UINT port;
} DownloadFile_t;


//������ʱ��
bool newTimer(UINT_PTR ID) {
	CFTP_ServerDlg*  Dlg = (CFTP_ServerDlg*)(AfxGetApp()->GetMainWnd());
	Dlg->SetTimer(ID, 5000, NULL);				//���ü�ʱ����ʱ��Ϊ5s
	return true;
}


// ɾ���ļ����ڶ�������bDelete��ʾ�Ƿ�ɾ��������վ,Ĭ��ɾ��������վ��
BOOL RecycleFileOrFolder(CString strPath, BOOL bDelete/*=FALSE*/)
{
	strPath += '\0';
	SHFILEOPSTRUCT  shDelFile;
	memset(&shDelFile, 0, sizeof(SHFILEOPSTRUCT));
	shDelFile.fFlags |= FOF_SILENT;				// ����ʾ����
	shDelFile.fFlags |= FOF_NOERRORUI;			// �����������Ϣ
	shDelFile.fFlags |= FOF_NOCONFIRMATION;		// ֱ��ɾ����������ȷ��

												// ����SHFILEOPSTRUCT�Ĳ���Ϊɾ����׼��
	shDelFile.wFunc = FO_DELETE;		// ִ�еĲ���
	shDelFile.pFrom = strPath;			// �����Ķ���Ҳ����Ŀ¼��ע�⣺�ԡ�\0\0����β��
	shDelFile.pTo = NULL;				// ��������ΪNULL
	if (bDelete) //���ݴ��ݵ�bDelete����ȷ���Ƿ�ɾ��������վ
	{
		shDelFile.fFlags &= ~FOF_ALLOWUNDO;    //ֱ��ɾ�������������վ
	}
	else
	{
		shDelFile.fFlags |= FOF_ALLOWUNDO;    //ɾ��������վ
	}

	BOOL bres = SHFileOperation(&shDelFile);    //ɾ��
	return !bres;
}
//���ļ�����
bool readFile(CString filePath, u_char* &data, long long int&d_len)
{
	CFile file;
	file.Open(filePath, CFile::modeRead, NULL);
	long long int len = file.GetLength();
	u_char* Buf =new u_char[len];
	file.Read(Buf, len);
	data = new u_char[len];
	copyData(Buf,data,len);
	d_len = len;
	return  true;
}
//д�ļ�����
bool writeFile(CString filePath,u_char* content,long long int len)
{
	assert(content!=NULL);
	CFile file;

	file.Open(filePath, CFile::modeCreate | CFile::modeWrite, NULL);

	file.Write(content, len);

	file.Close();
	return true;
}
//�߳̿��ƺ���
UINT dataThread(LPVOID lpParam)
{
	DownloadFile_t *pInfo = (DownloadFile_t*)lpParam;     //ָ��ṹ���ʵ����
	CFTP_ServerDlg*  Dlg = (CFTP_ServerDlg*)(AfxGetApp()->GetMainWnd());
	//��¼��־
	CString t;
	t.Format("���������ļ��߳�%d,%s", pInfo->DataID, pInfo->filename);
	// �ȴ��������֪ͨ
	WaitForSingleObject(hMutex, INFINITE);
	Dlg->log(t);
	// �ͷŻ������
	ReleaseMutex(hMutex);
	//�������ݶ�ȡ
	u_char* data;
	long long int data_len;
	readFile(pInfo->filename,data, data_len);
	t.Format("%s ��ʼ���䣡�ļ���С:%d", pInfo->filename, data_len);
	Dlg->log(t);
	//Dlg->MessageBox((CString)data);
	//��ʼ����
	long long  sendedSeq = 0 ;
	while (sendedSeq < data_len)
	{
		if (data_len - sendedSeq>=DATAMAXLEN)		//���ݳ��ȴ��ڱ��ĳ��س���
		{
			// �ȴ��������֪ͨ
			WaitForSingleObject(hMutex, INFINITE);
			u_char* tt = new u_char[DATAMAXLEN];
			copyData(data + sendedSeq, tt, DATAMAXLEN);
			pInfo->server->Seq += DATAMAXLEN;
			sendFTPPacket(pInfo->server, pInfo->toIP, pInfo->port, tt, DATAMAXLEN,pInfo->server->userID, pInfo->DataID, pInfo->server->Seq, pInfo->server->Ack, ACK);
			sendedSeq += DATAMAXLEN;
			t.Format("%s �������:%d/%d", pInfo->filename,sendedSeq, data_len);
			Dlg->log(t);
			// �ͷŻ������
			ReleaseMutex(hMutex);
		}
		else {
			// �ȴ��������֪ͨ
			WaitForSingleObject(hMutex, INFINITE);
			u_char* tt = new u_char[data_len - sendedSeq];
			copyData(data + sendedSeq, tt, data_len - sendedSeq);
			pInfo->server->Seq += data_len - sendedSeq;
			sendFTPPacket(pInfo->server, pInfo->toIP, pInfo->port, tt, data_len - sendedSeq, pInfo->server->userID, pInfo->DataID, pInfo->server->Seq, pInfo->server->Ack, ACK);
			sendedSeq += data_len - sendedSeq;
			t.Format("%s �������:%d/%d", pInfo->filename, sendedSeq, data_len);
			Dlg->log(t);
			// �ͷŻ������
			ReleaseMutex(hMutex);
		}
	}
	// �ȴ��������֪ͨ
	WaitForSingleObject(hMutex, INFINITE);
	u_char end[123] = "\r\n\r\n\r\n\r\n";
	int str_len = strlen((char *)end);
	pInfo->server->Seq += str_len;
	sendFTPPacket(pInfo->server, pInfo->toIP, pInfo->port, end,str_len, pInfo->server->userID, pInfo->DataID, pInfo->server->Seq, pInfo->server->Ack, ACK);
	// �ͷŻ������
	ReleaseMutex(hMutex);
	t.Format("%s �ļ�������ϣ��ļ���С:%d", pInfo->filename, data_len);
	// �ȴ��������֪ͨ
	WaitForSingleObject(hMutex, INFINITE);
	Dlg->log(t);
	// �ͷŻ������
	ReleaseMutex(hMutex);
	return 0;
}

//�����ļ��У�����ļ����Ѿ����ڷ���false�������ɹ�����true
bool makeDir(CString DirName)
{
	if (!PathIsDirectory(DirName))
	{
		::CreateDirectory(DirName, 0);
		return true;
	}
	else
	{
		return false;
	}
}
//��ȡһ���ļ�Ŀ¼�������ļ���Ϣ
CString getDirInfo(CString DirName)
{
	DirName += "\\*";
	HANDLE file;
	WIN32_FIND_DATA fileData;
	file = FindFirstFile(DirName.GetBuffer(), &fileData);
	CString fileString = "";
	if (file != INVALID_HANDLE_VALUE)
	{
		if (fileData.dwFileAttributes& FILE_ATTRIBUTE_DIRECTORY)
		{
			CString fileAttr;
			fileAttr.Format("D %s %d\r\n", fileData.cFileName, fileData.nFileSizeLow | (ULONGLONG)fileData.nFileSizeHigh << 32);
			fileString+= fileAttr;
		}
		else {
			CString fileAttr;
			fileAttr.Format("F %s %d\r\n", fileData.cFileName, fileData.nFileSizeLow | (ULONGLONG)fileData.nFileSizeHigh << 32);
			fileString += fileAttr;
		}
		bool bState = false;
		bState = FindNextFile(file, &fileData);
		while (bState) {
			if (fileData.dwFileAttributes& FILE_ATTRIBUTE_DIRECTORY)
			{
			CString fileAttr;
			fileAttr.Format("D %s %d\r\n", fileData.cFileName, fileData.nFileSizeLow | (ULONGLONG)fileData.nFileSizeHigh << 32);
			fileString += fileAttr;
			}
			else
			{
				CString fileAttr;
				fileAttr.Format("F %s %d\r\n", fileData.cFileName, fileData.nFileSizeLow | (ULONGLONG)fileData.nFileSizeHigh << 32);
				fileString += fileAttr;
			}
			bState = FindNextFile(file, &fileData);
		}
	}
	return fileString;
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
	WORD userID,									//�û�ID��ʶ
	WORD dataID,									//�����ֶα�ʶ
	DWORD seq,										//�������к�
	DWORD ack,										//ȷ�����к�
	WORD flags)										//��ʶλ
{
	int totalLen = dataLen + sizeof(FTPHeader_t);
	assert(totalLen <= DATAMAXLEN+ sizeof(FTPHeader_t));							//��֤������������ݳ���С��ʣ��Ĵ���ռ�1472 - 16 = 1456�ֽ�
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
	copyData(buffer, packet->PktData, totalLen);
	packet->len = totalLen;
	packet->TargetIP = toIP;
	packet->TargetPort = toPort;
	packet->ResendTime = 0;
	packet->Timer = ((ServerSocket*)(socket))->getTimerID();
	((ServerSocket*)(socket))->sendPKT_list.AddTail(packet);			//���뻺����	
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


