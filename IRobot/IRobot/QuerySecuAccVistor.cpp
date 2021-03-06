#include "StdAfx.h"
#include "QuerySecuAccVistor.h"
#include "Cfg.h"
#include "MidConn.h"
#include "KcxpConn.h"
#include "loginterface.h"
#include "DBConnect.h"
#include "public.h"

extern CCfg *g_pCfg;
extern CMidConn *g_pMidConn;
extern CKcxpConn *g_pKcxpConn;
extern CLoginterface *g_pLog;
extern CDBConnect *g_pDBConn;

CQuerySecuAccVistor::CQuerySecuAccVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;
	strcpy_s(m_szServiceName, "客户股东查询");
}

CQuerySecuAccVistor::~CQuerySecuAccVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CQuerySecuAccVistor::Vistor()
{
	BOOL bRet = TRUE;
	ExecTestCase(TestCase_1, "客户股东查询");
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


BOOL CQuerySecuAccVistor::TestCase_1()
{
	/*
		检查以下信息
		1.1.检查柜台返回的MARKET,SECU_ACC,SEAT是否与数据库一致
	*/
	BOOL bRet = TRUE;

	// 发送数据

	// TODO: KCXP 接口有问题, Market参数返回1位, 因此暂不提供KCXP接口测试
	char szTemp[2048] = {0};
	sprintf_s(szTemp,"501|%s|||", g_pCfg->GetCustID());
	bRet = SendMidMsg(&szTemp[0]);
	/*
	if (g_pCfg->GetTestMode() == USE_MID)
	{
		sprintf_s(szTemp,"501|%s|||", g_pCfg->GetCustID());
		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		sprintf_s(szTemp,"<  29294788> BEGIN:L0301002:27-10:39:01-384953  [_CA=2.3&_ENDIAN=0&F_OP_ROLE=2&F_OP_USER=%s&F_OP_BRANCH=%s&F_SESSION=%s&F_CHANNEL=0&F_OP_SITE=99999&CUSTOMER=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pCfg->GetBranch().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetCustID().GetBuffer());

		bRet = SendKcxpMsg(&szTemp[0]);

		// 清空日志解析，便于下一次操作
		g_pParseKcbpLog->Clean();
	}
	*/

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

BOOL CQuerySecuAccVistor::ChkData(MID_501_QUERY_SECU_ACC_RET_MSG *pMsg)
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取Shares表
	CString strSql;
	strSql.Format("select * from  secu_acc where secu_acc = '%s' and dft_acc = %s",
		pMsg->szSecuAcc, g_pCfg->GetAccount().GetBuffer());

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
				g_pCfg->SetXSB_BIND_SEAT(pMsg->szBindSeat);
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

void CQuerySecuAccVistor::ParseKcxpRetMsg()
{
	m_pMsg = new MID_501_QUERY_SECU_ACC_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_501_QUERY_SECU_ACC_RET_MSG)*m_nRowNum);

	//取结果集数据
	int iRetCode = KCBP_MSG_OK;
	char szTemp[512] = {0};
	
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
