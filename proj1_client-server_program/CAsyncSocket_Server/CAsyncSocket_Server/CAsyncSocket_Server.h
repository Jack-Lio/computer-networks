
// CAsyncSocket_Server.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCAsyncSocket_ServerApp: 
// �йش����ʵ�֣������ CAsyncSocket_Server.cpp
//

class CCAsyncSocket_ServerApp : public CWinApp
{
public:
	CCAsyncSocket_ServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCAsyncSocket_ServerApp theApp;