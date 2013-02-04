#include "StdAfx.h"
#include "L0303001.h"

CL0303001::CL0303001(void)
{
	m_pKcxpConn = g_pKcxpConn->GetKdMidCli();
}

CL0303001::~CL0303001(void)
{
	if (NULL != m_pKcxpConn)
	{
		m_pKcxpConn = NULL;
	}	
}

BOOL CL0303001::ExecuteLbm( L0303001_SEND_MSG *pSendMsg)
{
	if (NULL == m_pKcxpConn)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "获取KCXP连接失败!");
		return FALSE;
	}

	int iRetCode = KCBP_MSG_OK;

	m_pKcxpConn->BeginWrite();

	if ((iRetCode = m_pKcxpConn->SetValue("F_OP_USER",      g_pCfg->GetOpId().GetBuffer())) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_OP_ROLE",   "2")) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_OP_SITE",   "999999999999999")) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_OP_BRANCH", "999")) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_CHANNEL",   '0')) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("F_SESSION", g_pKcxpConn->GetSession())) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("OPER_FLAG",   pSendMsg->szOPER_FLAG)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("CUSTOMER",   pSendMsg->szCUSTOMER) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("MARKET",   pSendMsg->szMARKET)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("BOARD",   pSendMsg->szBOARD)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("SECU_ACC",   pSendMsg->szSECU_ACC)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("ACCOUNT",   pSendMsg->szACCOUNT)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("SECU_INTL",   pSendMsg->szSECU_INTL)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("SEAT",   pSendMsg->szSEAT)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("SERIAL_NO",   pSendMsg->szSERIAL_NO)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("DELIST_CHANNEL", pSendMsg->szDELIST_CHANNEL)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("QTY",   pSendMsg->szQTY)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("TRD_ID", pSendMsg->szTRD_ID)) != KCBP_MSG_OK
		|| (iRetCode = m_pKcxpConn->SetValue("PRICE",   pSendMsg->szPRICE)) != KCBP_MSG_OK)
	{		
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM[L0303001]设置参数失败,ERRCODE = %ld",iRetCode);

		return FALSE;
	}

	if ((iRetCode = m_pKcxpConn->CallProgramAndCommit("L0303001")) != KCBP_MSG_OK)
	{	
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM[L0303001]调用失败,ERRCODE = %ld",iRetCode);
		return FALSE;
	}

	return TRUE;
}

BOOL CL0303001::FetchRslt( int nRow, L0303001_RECV_MSG *pRecvMsg )
{
	int iRetCode = KCBP_MSG_OK;
	char szTemp[64];

	if ((iRetCode = pKcxpConn->RsOpen()) == KCBP_MSG_OK)
	{
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
			int nIdx = 0;
			while(nIdx < nRow)
			{
				if(m_pKcxpConn->RsFetchRow() != KCBP_MSG_OK)
				{
					break;
				}

				SERVICE_KCXP_STRNCPY("BIZ_NO", szBizNo);
				SERVICE_KCXP_STRNCPY("ORDER_ID", szOrderID);
				SERVICE_KCXP_STRNCPY("ACCOUNT", szAccount);
				SERVICE_KCXP_STRNCPY("PRICE", szPrice);
				SERVICE_KCXP_STRNCPY("QTY", szQty);
				SERVICE_KCXP_STRNCPY("ORDER_AMT", szOrderAmt);
				SERVICE_KCXP_STRNCPY("ORDER_FRZ_AMT", szOrderFrzAmt);
				SERVICE_KCXP_STRNCPY("SEAT", szSeat);
				SERVICE_KCXP_STRNCPY("EXT_INST", szExtInst);
				SERVICE_KCXP_STRNCPY("EXT_ACC", szExtAcc);
				SERVICE_KCXP_STRNCPY("EXT_SUB_ACC", szExtSubAcc);
				SERVICE_KCXP_STRNCPY("EXT_FRZ_AMT", szExtFrzAmt);		
				nIdx++;
			}		
		}
	}
	else
	{	
		g_pLog->WriteRunLogEx(__FILE__,__LINE__,"打开结果集失败,ERRCODE = %ld", iRetCode);

		return FALSE;
	}
}

int CL0303001::GetRsltRowNum()
{
	int nRow = 0;

	if ((iRetCode = m_pKcxpConn->RsOpen()) == KCBP_MSG_OK)
	{
		// 获取结果集行数，注意行数是包括标题的，因此行数要减1
		m_pKcxpConn->RsGetRowNum(&nRow);

		if (nRow > 0)
		{
			nRow -= 1;
		}
	}

	return nRow;
}
