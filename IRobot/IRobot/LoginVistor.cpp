#include "StdAfx.h"
#include "LoginVistor.h"
#include "Cfg.h"
#include "MidConn.h"
#include "loginterface.h"

extern CCfg *g_pCfg;
extern CMidConn *g_pMidConn;
extern CLoginterface *g_pLog;

CLoginVistor::CLoginVistor(void)
{
	m_pLoginMsg = NULL;

	strcpy_s(m_szServiceName, "客户登录");
}

CLoginVistor::~CLoginVistor(void)
{
	if (NULL != m_pLoginMsg)
	{
		delete m_pLoginMsg;
		m_pLoginMsg = NULL;
	}
}

BOOL CLoginVistor::Vistor()
{
	CString strAccount = g_pCfg->GetAccount();
	CString strUserCode = g_pCfg->GetCustID();
	CString strPwd = g_pCfg->GetCustPwd();

	m_pKDGateWay->SetOP_USER(strAccount.GetBuffer());

	char szPwd[32] = {0};
	strcpy_s(szPwd, strPwd.GetBuffer());

	m_pKDGateWay->EncryptPassWd((char *)&szPwd);

	char szTemp[512] = {0};
	sprintf_s(szTemp, "409101|Z|%s|%s|", strAccount.GetBuffer(), szPwd);
	
	if (TRUE != SendMsg(szTemp))
	{
		return FALSE;
	}

	// 登录成功后，须将包头的OP_USER改回为客户号码,供后面的业务使用
	m_pKDGateWay->SetOP_USER(strUserCode.GetBuffer());

	GetSession();

	return TRUE;
}

BOOL CLoginVistor::Vistor( char *szUserCode )
{	
	m_pKDGateWay->SetOP_USER(szUserCode);

	char szPwd[32] = "123444";

	m_pKDGateWay->EncryptPassWd((char *)&szPwd);

	char szTemp[512] = {0};
	sprintf_s(szTemp, "409101|U|%s|%s|", szUserCode, szPwd);

	if (TRUE != SendMsg(szTemp))
	{
		return FALSE;
	}
}

BOOL CLoginVistor::ResultStrToTable(char *pRetStr)
{
	int nRecNo = m_pKDGateWay->GetRecNum();
	m_pLoginMsg = new MID_409101_LOGIN_RET_MSG[nRecNo];
	memset(m_pLoginMsg, 0x00, sizeof(MID_409101_LOGIN_RET_MSG)*nRecNo);

	char *p = (char *)pRetStr;

	// 获得第二行，即数据开始行
	char *q = m_pKDGateWay->GetNextLine(p);
	char *tmp = q;

	// 获取每行的数据
	for (DWORD nRow=0; nRow<m_pKDGateWay->GetRecNum(); nRow++)
	{
		// 获取每列的长度
		for (DWORD nCol=0; nCol<m_pKDGateWay->GetFieldNum(); nCol++)
		{
			while(tmp++)
			{
				if (*tmp == '|')
				{
					*tmp = '\0';					

					switch (nCol)
					{
					case 0:
						strncpy_s(m_pLoginMsg[nRow].szUserCode, q, 9);
						break;
					case 1:
						strncpy_s(m_pLoginMsg[nRow].szMarket, q, 3);												
						break;
					case 2:
						strncpy_s(m_pLoginMsg[nRow].szBoard, q, 2);
						break;
					case 3:
						{						
							strncpy_s(m_pLoginMsg[nRow].szSecuAcc, q, 15);
							if (strcmp(m_pLoginMsg[nRow].szMarket, "00") == 0)
							{
								// 深A
								g_pCfg->SetSecu_Acc_SZA(q);
							}
							else if (strcmp(m_pLoginMsg[nRow].szMarket, "10") == 0)
							{
								// 沪A
								g_pCfg->SetSecu_Acc_SHA(q);
							}
							else if (strcmp(m_pLoginMsg[nRow].szMarket, "01") == 0)
							{
								// 深B
								g_pCfg->SetSecu_Acc_SZB(q);
							}
							else if (strcmp(m_pLoginMsg[nRow].szMarket, "11") == 0)
							{
								// 沪B
								g_pCfg->SetSecu_Acc_SHB(q);
							}
							else if (strcmp(m_pLoginMsg[nRow].szMarket, "02") == 0)
							{
								// 沪B
								g_pCfg->SetSecu_Acc_XSB(q);
							}
						}
						break;
					case 4:
						strncpy_s(m_pLoginMsg[nRow].szSecuAccName, q, 20);
						break;
					case 5:
						strncpy_s(m_pLoginMsg[nRow].szAccount, q, 10);
						break;
					case 6:
						strncpy_s(m_pLoginMsg[nRow].szUserName, q, 20);
						break;
					case 7:
						strncpy_s(m_pLoginMsg[nRow].szBrh, q, 10);
						break;
					case 8:
						strncpy_s(m_pLoginMsg[nRow].szMainFlag, q, 2);
						break;
					case 9:
						strncpy_s(m_pLoginMsg[nRow].szSession, q, 20);
						break;
					case 10:
						strncpy_s(m_pLoginMsg[nRow].szTaCode, q, 5);
						break;
					case 11:
						strncpy_s(m_pLoginMsg[nRow].szCertDays, q, 2);
						break;
					case 12:
						strncpy_s(m_pLoginMsg[nRow].szEtokenDays, q, 2);
						break;
					}

					q = tmp +1;					
					break;
				}
			}
		}		
	}

	return TRUE;
}

void CLoginVistor::GetSession()
{
	m_pKDGateWay->SetSession(m_pLoginMsg[0].szSession);

	g_pLog->WriteRunLogEx(__FILE__,__LINE__,"[409101] 登录接口, 获取Session:%s", m_pLoginMsg[0].szSession);
}

BOOL CLoginVistor::SendMsg( char *szMsg )
{
	if (TRUE != m_pKDGateWay->WaitAnswer(szMsg))
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[409101] 登录接口, 调用失败!");
		return FALSE;
	}

	if (TRUE != ResultStrToTable(m_pKDGateWay->m_pReturnData))
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[409101] 登录接口, 返回值解析失败!");
		return FALSE;
	}

	return TRUE;
}
