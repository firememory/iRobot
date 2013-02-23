#include "StdAfx.h"
#include "ParseKcbpLog.h"
#include "KcxpConn.h"
#include "Cfg.h"
#include "loginterface.h"
#include "LoginVistor.h"
#include "public.h"

extern CKcxpConn *g_pKcxpConn;
extern CCfg *g_pCfg;
extern CLoginterface *g_pLog;
extern LPFNDLLFUNC1 lpfnEncrypt;
extern LPFNDLLFUNC2 lpfnDecrypt;

CParseKcbpLog::CParseKcbpLog(void)
{
}

CParseKcbpLog::~CParseKcbpLog(void)
{
}

BOOL CParseKcbpLog::ReadLog()
{
	// ��log��
	FILE *fp = NULL;
	fopen_s(&fp, m_szKcbpLogPath, "r");

	if (NULL == fp)
	{		
		return FALSE;
	}

	char szBuf[1024] = {0};

	// ���ж�ȡ��־�ļ�
	while(NULL != fgets(&szBuf[0], 1024, fp))
	{
		ParseCmd(&szBuf[0]);
	}

	return TRUE;
}

void CParseKcbpLog::ReadRlt()
{
	int n = m_arrCmds.GetCount();
	POSITION pos;
	CString key, value;

	for (int i=0; i<n; i++)
	{
		LBM_PARAM_INFO *pLBM = m_arrCmds.GetAt(i);

		int nLBMParamCnt = pLBM->mapParams.GetCount();
		printf("LBM:%s has %d params \n", pLBM->szLbmId, nLBMParamCnt);
		
		for( pos = pLBM->mapParams.GetStartPosition(); pos != NULL; )
		{
			pLBM->mapParams.GetNextAssoc( pos, key, value);
		}
	}
}

BOOL CParseKcbpLog::ExecMultiCmds()
{
	CKDMidCli *pKcxpConn = g_pKcxpConn->GetKdMidCli();
	int iRetCode = KCBP_MSG_OK;
	int n = m_arrCmds.GetCount();

	POSITION pos;
	CString key, value;	

	for (int i=0; i<n; i++)
	{
		LBM_PARAM_INFO *pLBM = m_arrCmds.GetAt(i);

		pLBM->mapParams.Lookup("F_OP_ROLE", value);

		if (value.Compare("2") == 0)
		{
			// OP_ROLEΪ2�� ͨ������Աί��
			// ��Ҫ���� ���Բ���Ա��Ȩ��

			// cust_grant
		}
		else if (value.Compare("3") == 0)
		{
			// OP_ROLEΪ1�� ͨ��MIDί��
			// ��Ҫ�������ÿͻ���������Ϊ123444��Ȼ���¼�ͻ�

			// ���ÿͻ�����
			if (pLBM->mapParams.Lookup("F_OP_USER", value) == TRUE)
			{
				if (ResetUserPwd(value.GetBuffer()) == TRUE)
				{
					// �ͻ���¼
					CLoginVistor login;
					login.Vistor(value.GetBuffer());
				}
				
			}
			else
			{
				// �����ǵ�¼��Ϣ�� ������
			}		
		}

		pKcxpConn->BeginWrite();

		for( pos = pLBM->mapParams.GetStartPosition(); pos != NULL; )
		{
			pLBM->mapParams.GetNextAssoc( pos, key, value);

			if (iRetCode = pKcxpConn->SetValue(key.GetBuffer(), value.GetBuffer()) != KCBP_MSG_OK)
			{
				g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "KCXP SET VALUE Failed: LBM:%s, ParamName:%s, ParamVal:%s",
					pLBM->szLbmId, key.GetBuffer(), value.GetBuffer());				
			}
		}

		// Execute LBM
		if ((iRetCode = pKcxpConn->CallProgramAndCommit(pLBM->szLbmId)) != KCBP_MSG_OK)
		{	
			g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "LBM[%s]����ʧ��,ERRCODE = %ld", pLBM->szLbmId, iRetCode);
			return FALSE;
		}

		// �ж�ִ�н��
		int nRow = 0;

		if ((iRetCode = pKcxpConn->RsOpen()) == KCBP_MSG_OK)
		{
			// ��ȡ�����������ע�������ǰ�������ģ��������Ҫ��1
			pKcxpConn->RsGetRowNum(&nRow);

			if (nRow <= 1)
			{
				g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "LBM[%s]ִ��ʧ��,ERRCODE = %ld", pLBM->szLbmId, iRetCode);
			}
		}

		// ���ߣ��ȴ����ݿ����
		Sleep(g_pCfg->GetRefreshDBGap());
	}

	return TRUE;
}

BOOL CParseKcbpLog::ResetUserPwd( char *szUserCode )
{
	CKDMidCli *pKcxpConn = g_pKcxpConn->GetKdMidCli();

	int iRetCode = KCBP_MSG_OK;
	char szTemp[MAX_PATH] = {0};

	lpfnEncrypt(atol(szUserCode), "123444", szTemp);

	pKcxpConn->BeginWrite();

	if ((iRetCode = pKcxpConn->SetValue("F_OP_USER",      g_pCfg->GetOpId().GetBuffer())) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_OP_ROLE",   "2")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_OP_SITE",   "999999999999999")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_OP_BRANCH", "999")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_CHANNEL",   '0')) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_SESSION", g_pKcxpConn->GetSession())) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("USER_CODE",  szUserCode)) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("AUTH_TYPE",   "0") != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("AUTH_INFO",   szTemp)) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("USER_ROLE",   "1")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("CHECKPOINT", "0")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("OP_REMARK", "��������")) != KCBP_MSG_OK))
	{
		g_pLog->WriteRunLogEx(__FILE__, __LINE__, "LBM[L0102017]���ò���ʧ��,ERRCODE = %ld",iRetCode);
	}

	if ((iRetCode = pKcxpConn->CallProgramAndCommit("L0102017")) != KCBP_MSG_OK)
	{	
		g_pLog->WriteRunLogEx(__FILE__,__LINE__, "LBM[L0102017]����ʧ��,ERRCODE = %ld",iRetCode);
		return FALSE;
	}

	return TRUE;
}

void CParseKcbpLog::ParseCmd(char *pCmd)
{	
	LBM_PARAM_INFO *pLbmInfo = new LBM_PARAM_INFO;

	// ȡLBM���
	char szLbmId[10] = {0};
	char *pFind = strstr(pCmd, "BEGIN:");
	if (NULL != pFind)
	{
		// ����ҵ�BEGIN��˵����ִ���У����������
		
		int pos = strlen(pCmd);

		// ����β��] ���� ɾ��		
		if (pCmd[pos - 1] == '\n')
		{	
			// �Ǵ���־���룬����β��Ϊ�н�����
			pCmd[pos - 2] = '\0';			
		}
		else
		{
			// �ǴӴ����е���
			pCmd[pos - 1] = '\0';
		}
		

		// ���� LBM ID
		pFind += 6;

		char *pColon = strstr(pFind, ":");

		*pColon = '\0';

		strcpy_s(szLbmId, pFind);			
		printf("LBM=%s\n", szLbmId);
		strcpy_s(pLbmInfo->szLbmId, szLbmId);

		// ���Ҳ���, Ĭ�ϲ�������F_�Ͳ�����ʼ
		pColon++;
		pFind = strstr(pColon, "&F_");

		char arr[50][MAX_PATH];
		memset(&arr, 0x00, sizeof(arr));

		int i = 0;

		char *token = strtok(pFind, "&");
		while( token != NULL )
		{
			// While there are tokens in "string"						
			strcpy_s(arr[i], token);
			i++;

			// Get next token: 
			token = strtok( NULL, "&" ); 		
		}

		for (int idx=0;idx<i;idx++)
		{
			pFind = arr[idx];

			char *pName = strtok(pFind, "=");
			char *pValue = strtok(NULL, "=");											

			// ���ValueΪ��
			if (NULL == pValue)
			{
				pLbmInfo->mapParams.SetAt(pName, "");
			}
			else
			{
				pLbmInfo->mapParams.SetAt(pName, pValue);
			}												
		}

		m_arrCmds.Add(pLbmInfo);
	}
}

void CParseKcbpLog::Clean()
{
	int n = m_arrCmds.GetCount();
	POSITION pos;
	CString key, value;

	for (int i=0; i<n; i++)
	{
		// ������ڵ�
		LBM_PARAM_INFO *pLBM = m_arrCmds.GetAt(i);
		DELCLS(pLBM);
	}

	// ����б�
	m_arrCmds.RemoveAll();
}

BOOL CParseKcbpLog::ExecSingleCmd()
{
	CKDMidCli *pKcxpConn = g_pKcxpConn->GetKdMidCli();
	int iRetCode = KCBP_MSG_OK;

	POSITION pos;
	CString key, value;	

	LBM_PARAM_INFO *pLBM = m_arrCmds.GetAt(0);

	pKcxpConn->BeginWrite();

	for( pos = pLBM->mapParams.GetStartPosition(); pos != NULL; )
	{
		pLBM->mapParams.GetNextAssoc( pos, key, value);

		if (iRetCode = pKcxpConn->SetValue(key.GetBuffer(), value.GetBuffer()) != KCBP_MSG_OK)
		{
			g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "KCXP SET VALUE Failed: LBM:%s, ParamName:%s, ParamVal:%s",
				pLBM->szLbmId, key.GetBuffer(), value.GetBuffer());
		}
	}

	// Execute LBM
	if ((iRetCode = pKcxpConn->CallProgramAndCommit(pLBM->szLbmId)) != KCBP_MSG_OK)
	{	
		g_pLog->WriteRunLog(KCXP_MODE,LOG_WARN,"LBM[%s]����ʧ��,ERRCODE = %ld", pLBM->szLbmId, iRetCode);
		return FALSE;
	}

	return TRUE;
}

