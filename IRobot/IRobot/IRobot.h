
// IRobot.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CIRobotApp:
// �йش����ʵ�֣������ IRobot.cpp
//

class CIRobotApp : public CWinAppEx
{
public:
	CIRobotApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CIRobotApp theApp;