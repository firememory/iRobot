#include "StdAfx.h"
#include "ParseKcbpLog.h"
#include "KcxpConn.h"
#include "Cfg.h"
#include "loginterface.h"
#include "LoginVistor.h"

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

void CParseKcbpLog::ParseLog()
{
	// ��log��
	FILE *fp = NULL;
	fopen_s(&fp, "d:/kcbp.log", "r");

	if (NULL == fp)
	{		
		return;
	}

	char szBuf[1024] = {0};

	// ���ж�ȡ��־�ļ�
	while(NULL != fgets(&szBuf[0], 1024, fp))
	{
		int pos = strlen(szBuf);

		// ����β��Ϊ�н�����
		if (szBuf[pos - 1] == '\n')
		{
			szBuf[pos - 1] = '\0';
		}		

		LBM_PARAM_INFO *pLbmInfo = new LBM_PARAM_INFO;

		// ȡLBM���
		char szLbmId[10] = {0};
		char *pFind = strstr(szBuf, "BEGIN:");
		if (NULL != pFind)
		{
			// ����ҵ�BEGIN��˵����ִ���У����������
			// ����β��] ���� ɾ��
			szBuf[pos - 2] = '\0';

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
				printf( " %s\n", token );				

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

void CParseKcbpLog::Exec()
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

		for( pos = pLBM->mapParams.GetStartPosition(); pos != NULL; )
		{
			pLBM->mapParams.GetNextAssoc( pos, key, value);

			if (iRetCode = pKcxpConn->SetValue(key.GetBuffer(), value.GetBuffer()) != KCBP_MSG_OK)
			{
				g_pLog->WriteRunLogEx(__FILE__, __LINE__, "KCXP SET VALUE Failed: LBM:%s, ParamName:%s, ParamVal:%s",
					pLBM->szLbmId, key.GetBuffer(), value.GetBuffer());
			}
		}

		// Execute LBM
		if ((iRetCode = pKcxpConn->CallProgramAndCommit(pLBM->szLbmId)) != KCBP_MSG_OK)
		{	
			g_pLog->WriteRunLogEx(__FILE__,__LINE__,"LBM[%s]����ʧ��,ERRCODE = %ld", pLBM->szLbmId, iRetCode);
			return;
		}

		// �ж�ִ�н��
		int nRow = 0;

		if ((iRetCode = pKcxpConn->RsOpen()) == KCBP_MSG_OK)
		{
			// ��ȡ�����������ע�������ǰ�������ģ��������Ҫ��1
			pKcxpConn->RsGetRowNum(&nRow);

			if (nRow <= 1)
			{
				g_pLog->WriteRunLogEx(__FILE__,__LINE__,"LBM[%s]ִ��ʧ��,ERRCODE = %ld", pLBM->szLbmId, iRetCode);
			}
		}		

		Sleep(1000);
	}
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

