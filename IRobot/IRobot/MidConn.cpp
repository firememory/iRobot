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
	CMiniSocket InitMiniSocket;//��ʼ��
	if (InitMiniSocket.InitWinSocket()!=0)
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_EMERGENT, "��ʼ��MiniSocketʧ��!");
		return FALSE;
	}

	m_pKDGateWay = new CKDGateway;

	if (m_pKDGateWay==NULL)
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_EMERGENT, "��ʼ��KDGateWayʧ��!");
		return FALSE;
	}

	CRC_Init();

	return TRUE;
}
