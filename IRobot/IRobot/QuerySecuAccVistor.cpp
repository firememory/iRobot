#include "StdAfx.h"
#include "QuerySecuAccVistor.h"
#include "Cfg.h"
#include "MidConn.h"
#include "KcxpConn.h"
#include "loginterface.h"
#include "DBConnect.h"
#include "public.h"
#include "ParseKcbpLog.h"

extern CCfg *g_pCfg;
extern CMidConn *g_pMidConn;
extern CKcxpConn *g_pKcxpConn;
extern CLoginterface *g_pLog;
extern CDBConnect *g_pDBConn;
extern CParseKcbpLog *g_pParseKcbpLog;

CQuerySecuAccVistor::CQuerySecuAccVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;
	strcpy_s(m_szTestCaseName, "客户股东查询");
}

CQuerySecuAccVistor::~CQuerySecuAccVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CQuerySecuAccVistor::Vistor()
{
	BOOL bRet = TRUE;	

	bRet = ChkPnt1();
	
	return bRet;
}

BOOL CQuerySecuAccVistor::ResultStrToTable(char *pRetStr)
{
	m_nRowNum = m_pKDGateWay->GetRecNum();
	
	m_pMsg = new MID_501_QUERY_SECU_ACC_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_501_QUERY_SECU_ACC_RET_MSG)*m_nRowNum);

	int nLen = strlen(pRetStr);

	char *p = new char[nLen+1];

	strncpy(p, pRetStr, nLen);
	p[nLen] = '\0';

	// 获得第二行，即数据开始行
	char *q = m_pKDGateWay->GetNextLine(p);
	char *tmp = q;

	// 获取每行的数据
	for (DWORD nRow=0; nRow<m_nRowNum; nRow++)
	{
		// 获取每列的长度
		for (DWORD nCol=0; nCol<m_pKDGateWay->GetFieldNum(); nCol++)
		{
			while(tmp)
			{
				if (*tmp == '|')
				{
					*tmp = '\0';					

					switch (nCol)
					{
					case 0:						
						SERVICE_STRNCPY(szUserCode);
						break;
					case 1:
						SERVICE_STRNCPY(szMarket);
						break;
					case 2:						
						SERVICE_STRNCPY(szSecuAcc);
						break;
					case 3:						
						SERVICE_STRNCPY(szSecuAccName);
						break;
					case 4:						
						SERVICE_STRNCPY(szAccount);
						break;
					case 5:						
						SERVICE_STRNCPY(szMainFlag);
						break;
					case 6:						
						SERVICE_STRNCPY(szBindSeat);
						break;
					case 7:						
						SERVICE_STRNCPY(szBindStatus);
						break;
					case 8:						
						SERVICE_STRNCPY(szStatus);
						break;									
					}

					q = tmp +1;					
					break;
				}
				tmp++;
			}
		}		
	}

	return TRUE;
}


BOOL CQuerySecuAccVistor::ChkPnt1()
{
	/*
		检查以下信息
		1.1.检查柜台返回的MARKET,SECU_ACC,SEAT是否与数据库一致
	*/
	BOOL bRet = TRUE;

	// 发送数据
	if (TRUE != SendMsg(NULL))
	{
		return FALSE;
	}

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	// 1.1.检查柜台返回的MARKET,SECU_ACC,SEAT是否与数据库一致
	for (int i=0;i<m_nRowNum;i++)
	{
		if (ChkData(&m_pMsg[i]) == FALSE)
		{
			return FALSE;
		}
		
	}

	return bRet;
}

BOOL CQuerySecuAccVistor::SendMsg(char *pMsg)
{
	BOOL bRet = TRUE;

	if (g_pCfg->GetTestMode() == USE_MID)
	{
		bRet = SendMidMsg();
	}
	else
	{
		bRet = SendKcxpMsg();
		
		// 清空日志解析，便于下一次操作
		g_pParseKcbpLog->Clean();
	}

	return bRet;
}

BOOL CQuerySecuAccVistor::SendKcxpMsg()
{
	int iRetCode = KCBP_MSG_OK;

	if (NULL == m_pKcxpConn)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "获取KCXP连接失败!");
		return FALSE;
	}

	// 拼接发送给KCXP的命令字符串
	char szTemp[2048] = {0};		
	sprintf_s(szTemp,"<  29294788> BEGIN:L0301002:27-10:39:01-384953  [_CA=2.3&_ENDIAN=0&F_OP_ROLE=2&F_OP_USER=%s&F_OP_BRANCH=%s&F_SESSION=%s&F_CHANNEL=0&F_OP_SITE=99999&CUSTOMER=%s]",
		g_pCfg->GetOpId().GetBuffer(), g_pCfg->GetBranch().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetCustID().GetBuffer());

	// 发送消息
	try
	{
		// 解析命令
		g_pParseKcbpLog->ParseCmd(&szTemp[0]);

		// 向KCXP发送命令
		if (FALSE != g_pParseKcbpLog->ExecSingleCmd())
		{
			// 获取执行结果
			int nRow = 0;				

			if ((iRetCode = m_pKcxpConn->RsOpen()) == KCBP_MSG_OK)
			{
				// 获取结果集行数，注意行数是包括标题的，因此行数要减1
				m_pKcxpConn->RsGetRowNum(&nRow);

				if (nRow>1)
				{
					m_nRowNum = nRow - 1;

					m_pMsg = new MID_501_QUERY_SECU_ACC_RET_MSG[m_nRowNum];
					memset(m_pMsg, 0x00, sizeof(MID_501_QUERY_SECU_ACC_RET_MSG)*m_nRowNum);
				}
				else
				{
					g_pLog->WriteRunLogEx(__FILE__,__LINE__,"结果集返回行数异常!");
					m_nRowNum = 0;
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

								g_pLog->WriteRunLogEx(__FILE__,__LINE__, "获取结果集列信息失败,ERRCODE = %ld", iRetCode);
								return FALSE;
							}
						}
					}
					else
					{
						g_pLog->WriteRunLogEx(__FILE__,__LINE__, "获取结果集列信息失败,ERRCODE = %ld", iRetCode);

						return FALSE;
					}
				}

				//取第二结果集数据		
				if (iRetCode = m_pKcxpConn->RsMore() == KCBP_MSG_OK)
				{
					int nRow = 0;
  					while(nRow < m_nRowNum)
					{
						if(m_pKcxpConn->RsFetchRow() != KCBP_MSG_OK)
						{

							break;
						}

						SERVICE_KCXP_STRNCPY("USER_CODE", szUserCode);
						SERVICE_KCXP_STRNCPY("MARKET", szMarket);
						SERVICE_KCXP_STRNCPY("SECU_ACC", szSecuAcc);
						SERVICE_KCXP_STRNCPY("SECU_ACC_NAME", szSecuAccName);
						SERVICE_KCXP_STRNCPY("DFT_ACC", szAccount);
						SERVICE_KCXP_STRNCPY("MAIN_FLAG", szMainFlag);
						SERVICE_KCXP_STRNCPY("BIND_SEAT", szBindSeat);
						SERVICE_KCXP_STRNCPY("BIND_STATUS", szBindStatus);
						SERVICE_KCXP_STRNCPY("STATUS", szStatus);
						nRow++;
					}		
				}
			}
			else
			{	
				g_pLog->WriteRunLogEx(__FILE__,__LINE__,"打开结果集失败,ERRCODE = %ld", iRetCode);

				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	catch(...)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM[L0301002]调用异常！");
		return FALSE;
	}	

	return TRUE;
}

BOOL CQuerySecuAccVistor::SendMidMsg()
{
	char szTemp[512];		
	sprintf_s(szTemp,"501|%s|||", g_pCfg->GetCustID());

	if (m_pKDGateWay->WaitAnswer(&szTemp[0])!=TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[501] 委托接口, 调用失败!");
		return FALSE;
	}

	// 对柜台返回的值进行解析
	if (ResultStrToTable(m_pKDGateWay->m_pReturnData) != TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[501] 委托接口, 解析失败!");
		return FALSE;
	}

	return TRUE;
}

BOOL CQuerySecuAccVistor::ChkData(MID_501_QUERY_SECU_ACC_RET_MSG *pMsg)
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取Shares表
	CString strSql;
	strSql.Format("select * from  secu_acc where secu_acc = '%s' and bind_seat = %s and dft_acc = %s",
		pMsg->szSecuAcc, pMsg->szBindSeat, g_pCfg->GetAccount().GetBuffer());

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 
		
		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// 如果查询不到结果，返回失败
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.1 Fail!");
			return FALSE;
		}
		else
		{
			// 设置客户绑定的席位
			if (strcmp(pMsg->szMarket, "00") == 0)
			{
				// 深A
				g_pCfg->SetSZA_BIND_SEAT(pMsg->szBindSeat);
			}
			else if (strcmp(pMsg->szMarket, "10") == 0)
			{
				// 沪A
				g_pCfg->SetSHA_BIND_SEAT(pMsg->szBindSeat);
			}
			else if (strcmp(pMsg->szMarket, "01") == 0)
			{
				// 深B
				g_pCfg->SetSZB_BIND_SEAT(pMsg->szBindSeat);
			}
			else if (strcmp(pMsg->szMarket, "11") == 0)
			{
				// 沪B
				g_pCfg->SetSHB_BIND_SEAT(pMsg->szBindSeat);
			}
			else if (strcmp(pMsg->szMarket, "02") == 0)
			{
				// 三板
				g_pCfg->SetSZB_BIND_SEAT(pMsg->szBindSeat);
			}

			g_pDBConn->m_pRecordset->Close();
			return TRUE;
		}
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("错误描述：%s\n错误消息%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}
}
