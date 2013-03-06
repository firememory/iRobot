#include "StdAfx.h"
#include "IPCKDGateWayVistor.h"
#include "MidConn.h"
#include "KcxpConn.h"
#include "ParseKcbpLog.h"

extern CMidConn *g_pMidConn;
extern CKcxpConn *g_pKcxpConn;
extern CParseKcbpLog *g_pParseKcbpLog;

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
	EnterCriticalSection(&m_caSendMsgLock);
	g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, pCmd);

	int iRetCode = KCBP_MSG_OK;
	char szTemp[512] = {0};

	if (NULL == m_pKcxpConn)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "��ȡKCXP����ʧ��!");
		LeaveCriticalSection(&m_caSendMsgLock);
		return FALSE;
	}	

	// ������Ϣ
	try
	{
		// ��������
		g_pParseKcbpLog->ParseCmd(pCmd);

		// ��KCXP��������
		BOOL bRet = g_pParseKcbpLog->ExecSingleCmd();

		// �����־������������һ�β���
		g_pParseKcbpLog->Clean();

		if (FALSE != bRet)
		{
			// ��ȡִ�н��
			int nRow = 0;				

			if ((iRetCode = m_pKcxpConn->RsOpen()) == KCBP_MSG_OK)
			{
				// ��ȡ�����������ע�������ǰ�������ģ��������Ҫ��1
				m_pKcxpConn->RsGetRowNum(&nRow);

				if (nRow>1)
				{
					m_nRowNum = nRow - 1;					
				}
				else
				{
					g_pLog->WriteRunLogEx(__FILE__,__LINE__,"��������������쳣!");
					m_nRowNum = 0;
					LeaveCriticalSection(&m_caSendMsgLock);
					return FALSE;
				}

				if ((iRetCode = m_pKcxpConn->RsFetchRow()) == KCBP_MSG_OK)
				{
					if ((iRetCode = m_pKcxpConn->RsGetCol(1, szTemp)) == KCBP_MSG_OK)
					{
						if ((iRetCode = m_pKcxpConn->RsGetCol(2, szTemp)) == KCBP_MSG_OK)
						{
							if(strcmp(szTemp,"0") != 0)
							{
								iRetCode = m_pKcxpConn->RsGetCol(3, szTemp);

								g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡ���������Ϣʧ��,ERRCODE = %ld", iRetCode);
								LeaveCriticalSection(&m_caSendMsgLock);
								return FALSE;
							}
						}
					}
					else
					{
						g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡ���������Ϣʧ��,ERRCODE = %ld", iRetCode);
						LeaveCriticalSection(&m_caSendMsgLock);
						return FALSE;
					}
				}				
			}
			else
			{	
				g_pLog->WriteRunLogEx(__FILE__,__LINE__,"�򿪽����ʧ��,ERRCODE = %ld", iRetCode);
				LeaveCriticalSection(&m_caSendMsgLock);
				return FALSE;
			}
		}
		else
		{
			LeaveCriticalSection(&m_caSendMsgLock);
			return FALSE;
		}
	}
	catch(...)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM�����쳣��");
		LeaveCriticalSection(&m_caSendMsgLock);
		return FALSE;
	}	

	LeaveCriticalSection(&m_caSendMsgLock);
	return TRUE;
}
