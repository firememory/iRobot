#include "StdAfx.h"
#include "IPCKDGateWayVistor.h"
#include "MidConn.h"
#include "KcxpConn.h"

extern CMidConn *g_pMidConn;
extern CKcxpConn *g_pKcxpConn;

IPCKDGateWayVistor::IPCKDGateWayVistor()
{
	m_pKcxpConn = g_pKcxpConn->GetKdMidCli();
	m_pKDGateWay = g_pMidConn->GetKDGateWay();
}
