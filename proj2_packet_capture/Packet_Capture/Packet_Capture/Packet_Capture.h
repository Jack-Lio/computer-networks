
// Packet_Capture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPacket_CaptureApp: 
// �йش����ʵ�֣������ Packet_Capture.cpp
//

class CPacket_CaptureApp : public CWinApp
{
public:
	CPacket_CaptureApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPacket_CaptureApp theApp;