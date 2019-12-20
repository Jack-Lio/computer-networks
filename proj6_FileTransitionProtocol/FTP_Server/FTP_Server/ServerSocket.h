#pragma once
#include "afxsock.h"
#include "defines.h"

class ServerSocket :
	public CAsyncSocket
{
public:
	ServerSocket();
	~ServerSocket();
	//���ձ�����Ӧ����
	virtual void OnReceive(int nErrorCode);
	//��ȡ��ʱ����ID
	UINT_PTR getTimerID();
	//���ն�ʱ��ID
	void backTimerID(UINT_PTR ID);

	//�û��������뱣���ļ�
	CList<UserInfo_t*> UserInfo_list;
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
	//��ǰ�ļ�Ŀ¼
	CString currentDir;
	//���ͻ�����
	CList<SendPacket_t*> sendPKT_list;
	//���ͻ�������ʱ��ʹ������䣬���Ʒ��ͻ������Ĵ�С
	UINT_PTR  sendTimerLimit[TIMERLIMITNUM];
};

