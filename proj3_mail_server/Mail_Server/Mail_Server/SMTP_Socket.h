#pragma once
#include "afxsock.h"
#define bufferlen 1440

//�Զ���socket�࣬��������ʼ�������������ݽ����ͱ��Ľ��չ���
class SMTP_Socket :					
	public CAsyncSocket
{
public:
	SMTP_Socket();
	~SMTP_Socket();
	//������Ӧ����
	virtual void OnAccept(int nErrorCode);
	//���ջ���
	char lbuf[bufferlen];
	//��������Ƿ�ʼ����
	bool begin_data_recv;
	//������Ϣ��Ӧ����
	virtual void OnReceive(int nErrorCode);
	// ��¼�������ӽ��յı�������
	CString recv_post;
};

