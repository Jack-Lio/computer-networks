#pragma once

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
#define TIMERLIMITNUM  40        //������������20�����������ݰ�
//�ط���������
#define RESENDTIMELIMIT 5		//�ط���������Ϊ5�Σ�����ط�û���յ���Ӧ�����б���
//�������ݶ���󳤶�
#define DATAMAXLEN 1456

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
	int ResendTime;			//��¼�ش�����
}SendPacket_t;


//�û���Ϣ
typedef struct UserInfo_t {
	CString userName;
	CString password;
}UserInfo_t;

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
//�����ļ���Ŀ¼
bool makeDir(CString DirName);
//��ȡ�ļ�Ŀ¼�������ļ���Ϣ,�Թ̶���ʽ�ַ�������
CString getDirInfo(CString DirName);
// ɾ���ļ����ڶ�������bDelete��ʾ�Ƿ�ɾ��������վ,Ĭ��ɾ��������վ��
BOOL RecycleFileOrFolder(CString strPath, BOOL bDelete=FALSE);
//��ȡ�ļ����ݣ����ַ�������ȡ���������ݰ��ַ�������
bool readFile(CString filePath,u_char* &data,long long int& len);
//д�ļ����ݣ����ַ���д��
bool writeFile(CString filePath, u_char* content, long long int len);
//�����ļ������̣߳���ȡ�ļ�����������Ŀ���û������ļ�
UINT dataThread(LPVOID lpParam);
