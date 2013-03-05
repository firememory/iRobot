#include "StdAfx.h"
#include "IPCKDGateWayVistor.h"
#include "MidConn.h"
#include "KcxpConn.h"

extern CMidConn *g_pMidConn;
extern CKcxpConn *g_pKcxpConn;

CRITICAL_SECTION IPCKDGateWayVistor::m_caSendMsgLock;

IPCKDGateWayVistor::IPCKDGateWayVistor()
{
	m_pKcxpConn = g_pKcxpConn->GetKdMidCli();
	m_pKDGateWay = g_pMidConn->GetKDGateWay();

	InitializeCriticalSection(&m_caSendMsgLock);
}

IPCKDGateWayVistor::~IPCKDGateWayVistor()
{
	DeleteCriticalSection(&m_caSendMsgLock);
}

LPSTR IPCKDGateWayVistor::GetNextLine(LPSTR lpstr)
{
	LPSTR lpstrTemp = lpstr;
	DWORD i = 0;
	while(1)
	{
		lpstrTemp = strstr(lpstrTemp, "|");
		if ( lpstrTemp == NULL ) return NULL;
		lpstrTemp++;
		i++;

		if ( i >= m_nFieldNum )break;
	}
	return lpstrTemp;
}

// ��Mid������Ϣ
BOOL IPCKDGateWayVistor::SendMidMsg(char *pCmd)
{	
	EnterCriticalSection(&m_caSendMsgLock);
	g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Send:%s", pCmd);
	if (m_pKDGateWay->WaitAnswer(pCmd)!=TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "MID����ʧ��!");
		LeaveCriticalSection(&m_caSendMsgLock);
		return FALSE;
	}

	// �Թ�̨���ص�ֵ���н���
	g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Recv:%s", m_pKDGateWay->m_pReturnData);

	char szTemp[2048] = {0};
	strcpy_s(szTemp, m_pKDGateWay->m_pReturnData);

	m_nRowNum = m_pKDGateWay->GetRecNum();

	m_nFieldNum = m_pKDGateWay->GetFieldNum();

	LeaveCriticalSection(&m_caSendMsgLock);

	if (ResultStrToTable(&szTemp[0]) != TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "MID������Ϣ����ʧ��!");
		return FALSE;
	}

	return TRUE;
}

BOOL IPCKDGateWayVistor::SendKcxpMsg( char *pCmd )
{

}
