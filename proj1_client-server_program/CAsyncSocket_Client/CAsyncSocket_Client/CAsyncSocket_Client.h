
// CAsyncSocket_Client.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCAsyncSocket_ClientApp: 
// �йش����ʵ�֣������ CAsyncSocket_Client.cpp
//

class CCAsyncSocket_ClientApp : public CWinApp
{
public:
	CCAsyncSocket_ClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCAsyncSocket_ClientApp theApp;