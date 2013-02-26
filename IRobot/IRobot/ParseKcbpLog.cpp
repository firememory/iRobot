#include "StdAfx.h"
#include "ParseKcbpLog.h"
#include "KcxpConn.h"
#include "Cfg.h"
#include "loginterface.h"
#include "LoginVistor.h"
#include "DBConnect.h"
#include "public.h"

extern CKcxpConn *g_pKcxpConn;
extern CCfg *g_pCfg;
extern CLoginterface *g_pLog;
extern CDBConnect *g_pDBConn;
extern BOOL g_bThreadExit;

extern LPFNDLLFUNC1 lpfnEncrypt;
extern LPFNDLLFUNC2 lpfnDecrypt;

CParseKcbpLog::CParseKcbpLog(void)
{
	m_pKcxpConn = NULL;
}

CParseKcbpLog::~CParseKcbpLog(void)
{
}

BOOL CParseKcbpLog::ReadLog()
{
	// 打开log、
	FILE *fp = NULL;
	fopen_s(&fp, m_szKcbpLogPath, "r");

	if (NULL == fp)
	{		
		return FALSE;
	}

	char szBuf[1024] = {0};

	// 逐行读取日志文件
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

void CParseKcbpLog::ExecMultiCmds()
{
	DWORD dwThreadId = GetCurrentThreadId();

	g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "Begin thread %d", dwThreadId);

	int iRetCode = KCBP_MSG_OK;
	int n = m_arrCmds.GetCount();

	POSITION pos;
	CString key, value;	

	EnterCriticalSection(&ca);
	for (int i=0; i<n; i++)
	{
		if (g_bThreadExit == TRUE)
		{
			// 线程挂起
			break; 
		}

		LBM_PARAM_INFO *pLBM = m_arrCmds.GetAt(i);

		pLBM->mapParams.Lookup("F_OP_ROLE", value);

		if (value.Compare("2") == 0)
		{
			// OP_ROLE为2， 通过操作员委托
			// 需要设置 测试操作员的权限

			char szCustCode[50] = {0};

			// cust_grant
			pLBM->mapParams.Lookup("CUSTOMER", value);
			if (value.IsEmpty() == TRUE)
			{
				pLBM->mapParams.Lookup("CUST_CODE", value);
				if (value.IsEmpty() == TRUE)
				{
					pLBM->mapParams.Lookup("ACCOUNT", value);
					if (value.IsEmpty() == TRUE)
					{
						// 根据资金帐号查找客户的客户号
						GetCustCodeViaAccount(value.GetBuffer(), &szCustCode[0]);
						value.Format("%s", szCustCode);
					}
				}
			}

			// 设置 测试操作员的权限
			OpGrant(value.GetBuffer());
		}
		else if (value.Compare("3") == 0)
		{
			// OP_ROLE为1， 通过MID委托
			// 需要首先重置客户交易密码为123444，然后登录客户

			// 重置客户密码
			if (pLBM->mapParams.Lookup("F_OP_USER", value) == TRUE)
			{
				if (ResetUserPwd(value.GetBuffer()) == TRUE)
				{
					// 客户登录
					CLoginVistor login;
					login.Vistor(value.GetBuffer());
				}				
			}
			else
			{
				// 可能是登录消息， 不处理
			}		
		}
		LeaveCriticalSection(&ca);

		m_pKcxpConn->BeginWrite();

		for( pos = pLBM->mapParams.GetStartPosition(); pos != NULL; )
		{
			pLBM->mapParams.GetNextAssoc( pos, key, value);

			if (iRetCode = m_pKcxpConn->SetValue(key.GetBuffer(), value.GetBuffer()) != KCBP_MSG_OK)
			{
				g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "KCXP SET VALUE Failed: LBM:%s, ParamName:%s, ParamVal:%s",
					pLBM->szLbmId, key.GetBuffer(), value.GetBuffer());				
			}
		}

		// Execute LBM
		if ((iRetCode = m_pKcxpConn->CallProgramAndCommit(pLBM->szLbmId)) != KCBP_MSG_OK)
		{	
			g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "LBM[%s]调用失败,ERRCODE = %ld", pLBM->szLbmId, iRetCode);			
		}

		// 判断执行结果
		int nRow = 0;

		if ((iRetCode = m_pKcxpConn->RsOpen()) == KCBP_MSG_OK)
		{
			// 获取结果集行数，注意行数是包括标题的，因此行数要减1
			m_pKcxpConn->RsGetRowNum(&nRow);

			if (nRow <= 1)
			{
				g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "LBM[%s]执行失败,ERRCODE = %ld", pLBM->szLbmId, iRetCode);
			}
		}

		// 休眠，等待数据库更新
		Sleep(g_pCfg->GetRefreshDBGap());

		g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "End thread %d", dwThreadId);
	}
}

BOOL CParseKcbpLog::ResetUserPwd( char *szUserCode )
{
	CKDMidCli *m_pKcxpConn = g_pKcxpConn->GetKdMidCli();

	int iRetCode = KCBP_MSG_OK;
	char szTemp[MAX_PATH] = {0};

	lpfnEncrypt(atol(szUserCode), "123444", szTemp);

	m_pKcxpConn->BeginWrite();

	if ((iRetCode = m_pKcxpConn->SetValue("F_OP_USER",      g_pCfg->GetOpId().GetBuffer())) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_OP_ROLE",   "2")) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_OP_SITE",   "999999999999999")) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_OP_BRANCH", "999")) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_CHANNEL",   '0')) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_SESSION", g_pKcxpConn->GetSession())) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("USER_CODE",  szUserCode)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("AUTH_TYPE",   "0") != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("AUTH_INFO",   szTemp)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("USER_ROLE",   "1")) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("CHECKPOINT", "0")) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("OP_REMARK", "交易密码")) != KCBP_MSG_OK))
	{
		g_pLog->WriteRunLogEx(__FILE__, __LINE__, "LBM[L0102017]设置参数失败,ERRCODE = %ld",iRetCode);
	}

	if ((iRetCode = m_pKcxpConn->CallProgramAndCommit("L0102017")) != KCBP_MSG_OK)
	{	
		g_pLog->WriteRunLogEx(__FILE__,__LINE__, "LBM[L0102017]调用失败,ERRCODE = %ld",iRetCode);
		return FALSE;
	}

	return TRUE;
}

void CParseKcbpLog::ParseCmd(char *pCmd)
{	
	LBM_PARAM_INFO *pLbmInfo = new LBM_PARAM_INFO;

	// 取LBM编号
	char szLbmId[10] = {0};
	char *pFind = strstr(pCmd, "BEGIN:");
	if (NULL != pFind)
	{
		// 如果找到BEGIN，说明是执行行，则继续查找
		
		int pos = strlen(pCmd);

		// 将行尾的] 符号 删除		
		if (pCmd[pos - 1] == '\n')
		{	
			// 是从日志导入，将行尾变为行结束符
			pCmd[pos - 2] = '\0';			
		}
		else
		{
			// 是从代码中导入
			pCmd[pos - 1] = '\0';
		}
		

		// 查找 LBM ID
		pFind += 6;

		char *pColon = strstr(pFind, ":");

		*pColon = '\0';

		strcpy_s(szLbmId, pFind);			
		printf("LBM=%s\n", szLbmId);
		strcpy_s(pLbmInfo->szLbmId, szLbmId);

		// 查找参数, 默认参数是以F_型参数开始
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

			// 如果Value为空
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
	CString key, value;

	for (int i=0; i<n; i++)
	{
		// 清理掉节点
		LBM_PARAM_INFO *pLBM = m_arrCmds.GetAt(i);
		DELCLS(pLBM);
	}

	// 清空列表
	m_arrCmds.RemoveAll();
}

BOOL CParseKcbpLog::ExecSingleCmd()
{
	CKDMidCli *m_pKcxpConn = g_pKcxpConn->GetKdMidCli();
	int iRetCode = KCBP_MSG_OK;

	POSITION pos;
	CString key, value;	

	LBM_PARAM_INFO *pLBM = m_arrCmds.GetAt(0);

	m_pKcxpConn->BeginWrite();

	for( pos = pLBM->mapParams.GetStartPosition(); pos != NULL; )
	{
		pLBM->mapParams.GetNextAssoc( pos, key, value);

		if (iRetCode = m_pKcxpConn->SetValue(key.GetBuffer(), value.GetBuffer()) != KCBP_MSG_OK)
		{
			g_pLog->WriteRunLog(KCXP_MODE, LOG_WARN, "KCXP SET VALUE Failed: LBM:%s, ParamName:%s, ParamVal:%s",
				pLBM->szLbmId, key.GetBuffer(), value.GetBuffer());
		}
	}

	// Execute LBM
	if ((iRetCode = m_pKcxpConn->CallProgramAndCommit(pLBM->szLbmId)) != KCBP_MSG_OK)
	{	
		g_pLog->WriteRunLog(KCXP_MODE,LOG_WARN,"LBM[%s]调用失败,ERRCODE = %ld", pLBM->szLbmId, iRetCode);
		return FALSE;
	}

	return TRUE;
}

void CParseKcbpLog::OpGrant( char *pCust )
{	
	try
	{
		CString strSql;
		// 查找该客户是否已经在CUST_GRANT表中
		strSql.Format("select * from cust_grant where cust_code = %s", pCust);
		BSTR bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// 不存在，则新增授权
			strSql.Format("insert into CUST_GRANT (CUST_CODE, OP_CODE, LOGIN_SITE, BGN_TIME, END_TIME)"
				"values (%s, %s, '0016768a2e43', '06:00:00 ', '23:59:00')", pCust, g_pCfg->GetOpId());
		}
		else
		{
			// 存在，则update授权给测试用的OP
			strSql.Format("update CUST_GRANT set op_code = %s where cust_code = %s", g_pCfg->GetOpId(), pCust);
		}

		g_pDBConn->m_pRecordset->Close();
		_variant_t RecordsAffected; //VARIANT数据类型

		bstrSQL = strSql.AllocSysString();	

		g_pDBConn->m_pConnection->BeginTrans();
		g_pDBConn->m_pConnection->Execute(bstrSQL, &RecordsAffected, adCmdText);
		g_pDBConn->m_pConnection->CommitTrans();
	}
	catch (_com_error e)//异常处理
	{
		CString strMsg;
		strMsg.Format(_T("错误描述：%s\n错误消息%s"),
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		return;
	}
}

void CParseKcbpLog::GetCustCodeViaAccount( char *pAccount, char *pCustCode )
{
	try
	{
		CString strSql;
		strSql.Format("select * from accounts where account = %s", pAccount);
		BSTR bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		_variant_t TheValue; //VARIANT数据类型
		char szTmp[50] = {0};

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{				
			TheValue = g_pDBConn->m_pRecordset->GetCollect("USER_CODE");
			if(TheValue.vt!=VT_NULL)
			{				
				strncpy_s(szTmp, (char*)_bstr_t(TheValue), 100);
			}
			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();

		strncpy(pCustCode, szTmp, 50);
	}
	catch (_com_error e)//异常处理
	{
		CString strMsg;
		strMsg.Format(_T("错误描述：%s\n错误消息%s"),
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		return;
	}
}

