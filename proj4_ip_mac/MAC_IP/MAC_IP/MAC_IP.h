
// MAC_IP.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMAC_IPApp: 
// �йش����ʵ�֣������ MAC_IP.cpp
//

class CMAC_IPApp : public CWinApp
{
public:
	CMAC_IPApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMAC_IPApp theApp;