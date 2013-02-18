#include "StdAfx.h"
#include "KcxpConn.h"
#include "Cfg.h"
#include "public.h"
#include "loginterface.h"

extern CCfg *g_pCfg;
extern CLoginterface *g_pLog;

LPFNDLLFUNC1 lpfnEncrypt;
LPFNDLLFUNC2 lpfnDecrypt;

CKcxpConn::CKcxpConn(void)
{
	ZEROMEM(&stKCBPConnection, sizeof(stKCBPConnection));
	m_pKdMidCli = NULL;

	ZEROMEM(m_szSession, sizeof(m_szSession));
}

CKcxpConn::~CKcxpConn(void)
{
	m_clKdMidCli.DisConnectForce();
	m_pKdMidCli = NULL;
	ZEROMEM(m_szSession, sizeof(m_szSession));
}

BOOL CKcxpConn::InitKcxp()
{
	int nRet;

	HINSTANCE hLibrary;
	hLibrary = LoadLibrary(".\\CITICS_CE.dll");
	if (hLibrary)
	{
		if ((lpfnEncrypt = (LPFNDLLFUNC1)GetProcAddress(hLibrary, "UF_CommEncrypt"))
			&& (lpfnDecrypt = (LPFNDLLFUNC2)GetProcAddress(hLibrary, "UF_CommDecrypt")))
		{
			strcpy_s(stKCBPConnection.szServerName, "KCBPA01");
			stKCBPConnection.nProtocal = 0;
			strcpy_s(stKCBPConnection.szAddress, g_pCfg->GetKcxpIp().GetBuffer());
			int nPort = atoi(g_pCfg->GetKcxpPort().GetBuffer());
			stKCBPConnection.nPort = nPort;
			strcpy_s(stKCBPConnection.szSendQName,g_pCfg->GetKcxpSendQ().GetBuffer());
			strcpy_s(stKCBPConnection.szReceiveQName,g_pCfg->GetKcxpRecvQ().GetBuffer());

			m_clKdMidCli.SetConnectOption(stKCBPConnection);
			m_clKdMidCli.SetCliTimeOut(120);

			nRet = m_clKdMidCli.ConnectServer(stKCBPConnection.szServerName, "KCXP00", "888888");

			if (KCBP_MSG_OK != nRet)
			{
				g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "KCXP Connect to Server Failed!");
				return FALSE;
			}

			m_pKdMidCli = &m_clKdMidCli;
			return TRUE;		
		}
		else
		{
			g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "LoadLibrary CITICS_CE.dll Failed!");

			FreeLibrary(hLibrary);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL CKcxpConn::OpLogin()
{
	int nRet = KCBP_MSG_OK;
	char szErrMsg[64+1],szTemp[64+1];
	ZEROMEM(szTemp, sizeof(szTemp));

	lpfnEncrypt(atol(g_pCfg->GetOpId().GetBuffer()), g_pCfg->GetOpPwd().GetBuffer(), szTemp);	
	m_clKdMidCli.BeginWrite();

	if ((nRet = m_clKdMidCli.SetValue("F_OP_USER", g_pCfg->GetOpId().GetBuffer())) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("F_OP_ROLE",   "2")) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("F_OP_SITE",   "999999999999999")) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("F_OP_BRANCH", "999")) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("F_CHANNEL",   '0')) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("F_SESSION",   "0123456789")) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("USER_ID_TYPE",  '0')) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("USER_ID",  '2')) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("USER_ID_INFO",  g_pCfg->GetOpId().GetBuffer())) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("USER_ROLE",  '2')) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("AUTH_TYPE",  '0')) != KCBP_MSG_OK
		|| (nRet = m_clKdMidCli.SetValue("AUTH_INFO",  szTemp)) != KCBP_MSG_OK)
	{
		
		sprintf_s(szErrMsg,"LBM[L0102019]调用失败,ERRCODE = %ld",nRet);		
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, szErrMsg);

		return FALSE;
	}
	if ((nRet = m_clKdMidCli.CallProgramAndCommit("L0102019")) != KCBP_MSG_OK)
	{
		sprintf_s(szErrMsg,"LBM[L0102019]调用失败,ERRCODE = %ld",nRet);
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, szErrMsg);
		return FALSE;
	}
	
	if ((nRet = m_clKdMidCli.RsOpen()) == KCBP_MSG_OK)
	{
		if ((nRet = m_clKdMidCli.RsFetchRow()) == KCBP_MSG_OK)
		{
			int iColNum= -1;
			nRet = m_clKdMidCli.RsGetTableColNum(1, &iColNum);
			if ((nRet = m_clKdMidCli.RsGetCol(1, szTemp)) == KCBP_MSG_OK)
			{
				if ((nRet = m_clKdMidCli.RsGetCol(2, szTemp)) == KCBP_MSG_OK)
				{
					if(strcmp(szTemp,"0") != 0)
					{
						nRet = m_clKdMidCli.RsGetCol(3, szTemp);
                        
						g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, szErrMsg);
						return FALSE;
					}
					
				}
			}
			else
			{
				sprintf_s(szErrMsg,"获取结果集列信息失败,ERRCODE = %ld",nRet);				
				g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, szErrMsg);
				return FALSE;
			}
		}

		/*
		KCBP LBM通过以下代码，向客户端返回结果

		// 设置成功信息
		g_clKDMsg.SetMsg(KD_MSG_OK, szLbmMsg);
		clKDMid.KD_SendMessage(); 
		clKDMid.KD_RsNewTable("USER_LOGIN", 4, 
			"USER_CODE, USER_NAME, OPEN_BRANCH, SESSION");
		clKDMid.KD_RsAddRow();
		clKDMid.KD_RsSetColByName("USER_CODE",   lUserCode); 
		clKDMid.KD_RsSetColByName("USER_NAME",   szUserName); 
		clKDMid.KD_RsSetColByName("OPEN_BRANCH", siBranch); 
		clKDMid.KD_RsSetColByName("SESSION",     szSession);
		
		clKDMid.KD_RsSaveRow();                                    
		// 提交事务
		clKDMid.KD_Commit();  
		*/

		//取第二结果集数据
		if (nRet = m_clKdMidCli.RsMore() == KCBP_MSG_OK)
		{
			int n = 0;
			while (n < 2)
			{
				if (nRet = m_clKdMidCli.RsFetchRow() != KCBP_MSG_OK)
				{
					break;
				}

				nRet =  m_clKdMidCli.RsGetColByName("USER_CODE", szTemp);			
				nRet =  m_clKdMidCli.RsGetColByName("USER_NAME", szTemp);				
				nRet =  m_clKdMidCli.RsGetColByName("OPEN_BRANCH", szTemp);
				nRet =  m_clKdMidCli.RsGetColByName("SESSION", szTemp);

				strcpy_s(m_szSession, szTemp);
				n++;
			}
		}
	}
	else
	{
		sprintf_s(szErrMsg,"打开结果集失败,ERRCODE = %ld",nRet);
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, szErrMsg);

		return FALSE;
	}

	return TRUE;
}
