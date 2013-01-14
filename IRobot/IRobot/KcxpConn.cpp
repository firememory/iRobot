#include "StdAfx.h"
#include "KcxpConn.h"
#include "Cfg.h"
#include "public.h"

extern CCfg *g_pCfg;

CKcxpConn::CKcxpConn(void)
{
	ZEROMEM(&stKCBPConnection, sizeof(stKCBPConnection));
}

CKcxpConn::~CKcxpConn(void)
{
	m_clKdMidCli.DisConnectForce();
}

BOOL CKcxpConn::InitKcxp()
{
	int nRet;

	strcpy_s(stKCBPConnection.szServerName, "KCBPA01");
	stKCBPConnection.nProtocal = 0;
	strcpy_s(stKCBPConnection.szAddress, g_pCfg->GetKcxpIp().GetBuffer());
	stKCBPConnection.nPort = 21000;
	strcpy_s(stKCBPConnection.szSendQName,g_pCfg->GetKcxpSendQ().GetBuffer());
	strcpy_s(stKCBPConnection.szReceiveQName,g_pCfg->GetKcxpRecvQ().GetBuffer());

	m_clKdMidCli.SetConnectOption(stKCBPConnection);
	m_clKdMidCli.SetCliTimeOut(120);

	nRet = m_clKdMidCli.ConnectServer(stKCBPConnection.szServerName, "KCXP00", "888888");

	if (KCBP_MSG_OK != nRet)
	{
		return FALSE;
	}

	return TRUE;
}
