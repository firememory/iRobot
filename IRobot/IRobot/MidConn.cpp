#include "StdAfx.h"
#include "MidConn.h"
#include "loginterface.h"
#include "CMiniSocket.h"
#include "CRC.h"

extern CLoginterface *g_pLog;

CMidConn::CMidConn(void)
{
	m_pKDGateWay = NULL;
}

CMidConn::~CMidConn(void)
{
}

BOOL CMidConn::Init()
{
	CMiniSocket InitMiniSocket;//初始化
	if (InitMiniSocket.InitWinSocket()!=0)
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_EMERGENT, "初始化MiniSocket失败!");
		return FALSE;
	}

	m_pKDGateWay = new CKDGateway;

	if (m_pKDGateWay==NULL)
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_EMERGENT, "初始化KDGateWay失败!");
		return FALSE;
	}

	CRC_Init();

	return TRUE;
}
