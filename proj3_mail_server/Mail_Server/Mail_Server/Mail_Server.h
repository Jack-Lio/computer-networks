
// Mail_Server.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMail_ServerApp: 
// �йش����ʵ�֣������ Mail_Server.cpp
//

class CMail_ServerApp : public CWinApp
{
public:
	CMail_ServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMail_ServerApp theApp;