#pragma once
#include "afxsock.h"

//��������״̬
#define start        0		//������ʼ��״̬
#define isConnecting 1		//���ڽ�������
#define isCommunicating 2	//���ڽ��������
#define isTransfer	3		//���ڽ������ݴ���
#define sendedFIN 4			//���ڽ��в�����ӹ���,�ѷ��Ͳ����������
#define recvedFIN 5			//���յ��������������ĶϿ���������
#define finished 6			//�����Ѿ����


//�����ʶλ��
#define SYN 0x0001				//��ʶ��������λ
#define ACK 0x0002				//��ʶACKλ��Ч
#define FIN 0x0004				//��ʶ������������λ
#define RST 0x0008				//��ʶ�������ӣ������ر�����


//��ʱ������
#define TIMERLIMITNUM 20        //������������20�����������ݰ�
//�ط���������
#define RESENDTIMELIMIT 5		//�ط���������Ϊ5�Σ�����ط�û���յ���Ӧ�����б���

#pragma pack(1)
//����FTPЭ�鱨�ĸ�ʽ
typedef struct FTPHeader_t {	//FTP�����ײ�
	WORD UserID;				//�û���ʶ������ͬ���û�
	WORD DataID;				//���ݰ���ʶ��ʶ��ͬһ�����Ĳ�ͬ���ݱ���
	DWORD SEQNO;					//�������к�
	DWORD ACKNO;					//ȷ�����к�
	WORD CheckSum;				//У��ͣ������ļ��ϴ�ֻ��ͷ�������Լ�IP��ַ�Ͷ˿�������У��
	WORD Flags;					//��־λ�ֶ�
}FrameHeader_t;

//�������ݰ��ڻ������еĸ�ʽ
typedef struct SendPacket_t {
	int len;				//���ݰ�����
	BYTE PktData[2000];		//���ݰ�
	CString TargetIP;		//Ŀ��IP��ַ��ʹ���ַ�����ʽ
	UINT   TargetPort;		//Ŀ�Ķ˿ں�
	UINT_PTR Timer;			//��ʱ�����
	int ResendTime;			//�ط�����
};
typedef struct DownloadFile_t {
	WORD DataID;			//�ļ�ID
	CString filename;		//�ļ���
	u_char filecontent[10000000];	//�ļ�����
	long long int len;		//�ļ����ݳ���
} DownloadFile_t;
#pragma pack()


// ����У���
unsigned short ChecksumCompute(unsigned short * buffer, int size);
//��ȡ����������ʱ����Ϣ���ַ�����ʽ��
void getDateTimeStr(CString&date, CString&time);
//��ȡ������HostIP��ַ
CString getHostIPStr();
//��IP��ַתΪ�ַ�����ʽ
CString getIPStr(DWORD IP);
//�������ݰ�����
bool sendFTPPacket(CAsyncSocket* socket,			//�����׽���
	CString toIP,									//������IP
	UINT toPort,									//�������˿�
	u_char* sendData,								//���������׵�ַ
	int dataLen,									//�������ݳ���
	WORD userID = 0,									//�û�ID��ʶ
	WORD dataID = 0,									//�����ֶα�ʶ,���ļ��ְ�ʱʹ��
	DWORD seq = 0,										//�������к�
	DWORD ack = 0,										//ȷ�����к�
	WORD flags = 0);									//��ʶλ

//��������
void copyData(u_char* srcData, u_char*destData, int len);
//������ʱ��
bool newTimer(UINT_PTR ID);
//���ļ�����
bool readFile(CString filePath, u_char* &data, long long int&d_len);
//д�ļ�����
bool writeFile(CString filePath, u_char* content, long long int len);


class ConnectSocket :
	public CAsyncSocket
{
public:
	ConnectSocket();
	~ConnectSocket();
	//���ձ�����Ӧ����
	virtual void OnReceive(int nErrorCode);
	//��ȡ��ʱ����ID
	UINT_PTR getTimerID();
	//���ն�ʱ��ID
	void backTimerID(UINT_PTR ID);
  
	//�����ļ������б�
	CList<DownloadFile_t*> files_list;
	//������IP��ַ
	CString Server_IP;
	//�������˿ں�
	UINT Server_Port;
	//���յ���ȷ�����к�
	DWORD recvACK;
	//���յ���ͬȷ�����кŵĴ���
	int	  sameTimes;
	//���յ�����һ��seq���
	DWORD recvSEQ;
	//��ǰ�������к�
	DWORD Seq;
	//��ǰ��ȷ�Ͻ��ձ������к�
	DWORD Ack;
	//��ʶ��ǰ���ӵ�״̬
	int status;						
	//���������ص��û�ID
	WORD userID;					
	//���ͻ�����
	CList<SendPacket_t*> sendPKT_list;
	//���ͻ�������ʱ��ʹ������䣬���Ʒ��ͻ������Ĵ�С
	UINT_PTR  sendTimerLimit[TIMERLIMITNUM];
};

