#include "StdAfx.h"
#include "MidConn.h"
#include "loginterface.h"
#include "CMiniSocket.h"
#include "CRC.h"
#include "Cfg.h"

extern CLoginterface *g_pLog;
extern CCfg *g_pCfg;

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

	m_pKDGateWay->SetCHANNEL("7");
	m_pKDGateWay->SetOP_BRANCH("8070");
	m_pKDGateWay->SetOP_ROLE("1");
	m_pKDGateWay->SetOP_SITE("3@00247E030C03@172018009084");

	return TRUE;
}

BOOL CMidConn::Connect()
{
	int nPort = atoi(g_pCfg->GetMidPort().GetBuffer());
	
	if (TRUE != m_pKDGateWay->Connect(g_pCfg->GetMidIp().GetBuffer(), nPort))
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_DEBUG, "连接MID失败!");
		return FALSE;
	}

	if (TRUE != m_pKDGateWay->CheckIn())
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_DEBUG, "KDGateWay Check In失败!");
		return FALSE;
	}
	
	return TRUE;
}

void CMidConn::DisConnect()
{
	m_pKDGateWay->Disconnect();
}
