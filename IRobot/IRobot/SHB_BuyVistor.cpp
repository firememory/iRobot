#include "StdAfx.h"
#include "SHB_BuyVistor.h"
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

CSHB_BuyVistor::CSHB_BuyVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;

	m_fMatched_OrderFrzAmt = m_fMatched_Price = m_nMatched_Qty = m_fMatchedAmt = m_fMatched_SettAmt = 0;

	m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;

	m_fCptlBln_Old = m_fCptlAvl_Old = m_fCptlTrdFrz_Old = m_fCptlOutstanding_Old = m_fCptlOtdAvl_Old = 0;

	m_nShareBln_New = m_nShareAvl_New = m_nShareTrdFrz_New = m_nShareOtd_New = 0;
	
	m_fCptlBln_New = m_fCptlAvl_New = m_fCptlTrdFrz_New = m_fCptlOutstanding_New = m_fCptlOtdAvl_New = 0;

	strcpy_s(m_szServiceName, "上海B股买入");
	strcpy_s(m_szSecu_Code, "900948");
	strcpy_s(m_szSecu_Intl, "1900948");
	strcpy_s(m_szQty, "200");
	strcpy_s(m_szPrice, "2.55");

	// 沪B
	strcpy_s(m_szMarket_Board, "11");	
	strcpy_s(m_szTrdId, "0B");
	strcpy_s(m_szCurrency, "2"); // 美元
}

CSHB_BuyVistor::~CSHB_BuyVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CSHB_BuyVistor::Vistor()
{
	BOOL bRet = TRUE;

	// B股目前只支持限价委托
	ExecTestCase(TestCase_1, "限价买入 0B");

	return bRet;
}

BOOL CSHB_BuyVistor::ResultStrToTable(char *pRetStr)
{
	m_nRowNum = m_pKDGateWay->GetRecNum();

	m_pMsg = new MID_403_ORDER_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_403_ORDER_RET_MSG)*m_nRowNum);

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
			while(tmp++)
			{
				if (*tmp == '|')
				{
					*tmp = '\0';					

					switch (nCol)
					{
					case 0:						
						SERVICE_STRNCPY(szBizNo);
						break;
					case 1:
						SERVICE_STRNCPY(szOrderID);
						break;
					case 2:						
						SERVICE_STRNCPY(szAccount);
						break;
					case 3:						
						SERVICE_STRNCPY(szPrice);
						break;
					case 4:						
						SERVICE_STRNCPY(szQty);
						break;
					case 5:						
						SERVICE_STRNCPY(szOrderAmt);
						break;
					case 6:						
						SERVICE_STRNCPY(szOrderFrzAmt);
						break;
					case 7:						
						SERVICE_STRNCPY(szSeat);
						break;
					case 8:						
						SERVICE_STRNCPY(szExtInst);
						break;
					case 9:						
						SERVICE_STRNCPY(szExtAcc);
						break;
					case 10:						
						SERVICE_STRNCPY(szExtSubAcc);
						break;
					case 11:						
						SERVICE_STRNCPY(szExtFrzAmt);
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

// 限价买入
BOOL CSHB_BuyVistor::TestCase_1()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "0B");

	SaveCapital();

	bRet = InitUserData();
	if (bRet == FALSE)
	{
		return FALSE;
	}

	// 发送数据
	char szTemp[2048] = {0};
	if (g_pCfg->GetTestMode() == USE_MID)
	{
		sprintf_s(szTemp,"403|%s|%s|%s|%s||%s|%s|%s|%s||||||||||||",
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SHB(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// 拼接发送给KCXP的命令字符串		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHB(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHB_BIND_SEAT());

		bRet = SendKcxpMsg(&szTemp[0]);

		// 清空日志解析，便于下一次操作
		g_pParseKcbpLog->Clean();
	}

	if (bRet == FALSE)
	{
		return FALSE;	
	}

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData();

	return bRet;
}


BOOL CSHB_BuyVistor::SendKcxpMsg(char *pCmd)
{
	g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, pCmd);

	int iRetCode = KCBP_MSG_OK;
	char szTemp[512] = {0};

	if (NULL == m_pKcxpConn)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "获取KCXP连接失败!");
		return FALSE;
	}	

	// 发送消息
	try
	{
		// 解析命令
		g_pParseKcbpLog->ParseCmd(pCmd);

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

					m_pMsg = new MID_403_ORDER_RET_MSG[m_nRowNum];
					memset(m_pMsg, 0x00, sizeof(MID_403_ORDER_RET_MSG)*m_nRowNum);
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
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM[L0303001]调用异常！");
		return FALSE;
	}	

	return TRUE;
}

/*
 *	获取以下数据
 	1. Shares表
		1.1 股份余额
		1.2 股份可用
		1.3 交易冻结
		1.4 在途数量
	2. Capitals表
		2.1 资金余额
		2.2 资金可用
		2.3 交易冻结
		2.4 在途资金
		2.5 在途可用
 */
BOOL CSHB_BuyVistor::InitUserData()
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取Shares表
	CString strSql;
	strSql.Format("select * from  shares where account = %s and secu_intl = %s",
		g_pCfg->GetAccount().GetBuffer(), m_szSecu_Intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		// 客户当前没有持仓此股票
		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;
		}

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{	
			// 1.1 股份余额
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_Old);

			// 1.2 股份可用
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_Old);

			// 1.3 交易冻结
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_Old);

			// 1.4 在途数量
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_Old);

			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();
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
	

	// 2. 获取Capitals表
	try
	{
		strSql.Format("select * from  capital where account = %s and currency = %s",
			g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

		bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// 数据库返回的结果为空
			g_pDBConn->m_pRecordset->Close();

			g_pLog->WriteRunLogEx(__FILE__,__LINE__, "获取Capitals表数据失败!");
			return FALSE;
		}

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{	
			// 2.1 资金余额
			DB_GET_VALUE_FLOAT("BALANCE", m_fCptlBln_Old);

			// 2.2 资金可用			
			DB_GET_VALUE_FLOAT("AVAILABLE", m_fCptlAvl_Old);

			// 2.3 交易冻结
			DB_GET_VALUE_FLOAT("TRD_FRZ", m_fCptlTrdFrz_Old);

			// 2.4 在途资金
			DB_GET_VALUE_FLOAT("OUTSTANDING", m_fCptlOutstanding_Old);

			// 2.5 在途可用
			DB_GET_VALUE_FLOAT("OTD_AVL", m_fCptlOtdAvl_Old);

			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();
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

	return TRUE;
}


BOOL CSHB_BuyVistor::UpdateUserData()
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取Shares表
	CString strSql;
	strSql.Format("select * from  shares where account = %s and secu_intl = %s",
		g_pCfg->GetAccount().GetBuffer(), m_szSecu_Intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{	
			// 1.1 股份余额
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_New);

			// 1.2 股份可用
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_New);

			// 1.3 交易冻结
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_New);

			// 1.4 在途数量
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_New);

			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();
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

	// 2. 获取Capitals表, 注意深B取港币信息
	try
	{
		strSql.Format("select * from  capital where account = %s and currency = %s",
			g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

		bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// 数据库返回的结果为空
			g_pDBConn->m_pRecordset->Close();
			g_pLog->WriteRunLogEx(__FILE__,__LINE__,"获取Capitals表数据失败!");
			return FALSE;
		}

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{	
			// 2.1 资金余额
			DB_GET_VALUE_FLOAT("BALANCE", m_fCptlBln_New);

			// 2.2 资金可用
			DB_GET_VALUE_FLOAT("AVAILABLE", m_fCptlAvl_New);

			// 2.3 交易冻结
			DB_GET_VALUE_FLOAT("TRD_FRZ", m_fCptlTrdFrz_New);

			// 2.4 在途资金
			DB_GET_VALUE_FLOAT("OUTSTANDING", m_fCptlOutstanding_New);

			// 2.5 在途可用
			DB_GET_VALUE_FLOAT("OTD_AVL", m_fCptlOtdAvl_New);

			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();
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
	
	return TRUE;
}

BOOL CSHB_BuyVistor::GetMatchedData()
{
	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");
	BOOL bRet = TRUE;

	try
	{
		// 1.1 检查柜台matching表中的数据是否与委托一致，并获取【持仓成本】
		CString strSql;

		// 限价买入
		strSql.Format("select * from  matching where trd_date = %s and trd_id = '%s'and order_id = '%s'"
			" and account = %s and secu_intl = %s and order_price = %s and order_qty = %s",
			strDate, m_szTrdId, m_pMsg[0].szOrderID, m_pMsg[0].szAccount, m_szSecu_Intl, m_szPrice, m_szQty);

		BSTR bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		_variant_t TheValue; //VARIANT数据类型
		char szTmp[100] = {0};

		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// 数据库返回的结果为空
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.1 Fail!");
			g_pDBConn->m_pRecordset->Close();
			return FALSE;
		}	

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{								
			DB_GET_VALUE_FLOAT("ORDER_FRZ_AMT", m_fMatched_OrderFrzAmt);
			DB_GET_VALUE_FLOAT("MATCHED_PRICE", m_fMatched_Price);
			DB_GET_VALUE_INT("MATCHED_QTY", m_nMatched_Qty);
			DB_GET_VALUE_FLOAT("MATCHED_AMT", m_fMatchedAmt);
			DB_GET_VALUE_FLOAT("SETT_AMT", m_fMatched_SettAmt);

			g_pDBConn->m_pRecordset->MoveNext();
		}
		g_pDBConn->m_pRecordset->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("错误描述：%s\n错误消息%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		bRet = FALSE;
	}

	return bRet;
}

BOOL CSHB_BuyVistor::SendMidMsg(char *pCmd)
{
	g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Send:%s", pCmd);
	if (m_pKDGateWay->WaitAnswer(pCmd)!=TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[403] 委托接口, 调用失败!");
		return FALSE;
	}

	// 对柜台返回的值进行解析
	g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Recv:%s", m_pKDGateWay->m_pReturnData);
	if (ResultStrToTable(m_pKDGateWay->m_pReturnData) != TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[403] 委托接口, 解析失败!");
		return FALSE;
	}

	return TRUE;
}

BOOL CSHB_BuyVistor::ChkData()
{
	/*
		检查以下信息
		1.1.检查柜台matching表中的数据是否与委托一致	
		1.2.检查captial表中【资金余额】
		1.3.检查captial表中【资金可用】
		1.4.检查captial表中【交易冻结】
		1.5 检查Shares表中【在途股份】
	*/

	BOOL bRet = TRUE;

	// 1.1.检查柜台matching表中的数据是否与委托一致	
	if (!(bRet = GetMatchedData()))
	{
		// 获取Matcing表数据失败
		return bRet;
	}	

	// 获取客户最新的股份和资金数据
	if (!UpdateUserData())
	{
		g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "更新测试数据失败!");
		bRet = FALSE;
	}
	else
	{
		// 1.2.检查captial表中【资金余额】不变
		if (m_fCptlBln_Old != m_fCptlBln_New)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.2 Fail!");
		}
				
		// 1.3.检查captial表中【资金可用 AVAILABLE】减少 应该等于 Matching表中的【ORDER_FRZ_AMT】
		// 由于精度问题，允许1以内的差异
		if (abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) - m_fMatched_OrderFrzAmt) > 1)
		{				
			bRet = FALSE;
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
		}

		// 1.4.检查captial表中【交易冻结 TRD_FRZ】增加 等于Matching表中的【ORDER_FRZ_AMT】-【SETT_AMT】
		if (abs(CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old) - CutFloatPnt(m_fMatched_OrderFrzAmt - m_fMatched_SettAmt)) > 1)
		{
			float n1 = m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old;
			float n2 = m_fMatched_SettAmt;

			bRet = FALSE;
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
		}
		
		// 1.5 检查Shares表中【在途股份】增加matching表中的【成交数量】
		if (m_nShareOtd_New - m_nShareOtd_Old != m_nMatched_Qty)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.5 Fail!");
		}
	}

	return bRet;
}

// 客户资金蓝补
BOOL CSHB_BuyVistor::SaveCapital()
{
	// 资金余额蓝补1W, 资金可用蓝补1W

	// 发送数据
	char szTemp[2048] = {0};

	// 拼接发送给KCXP的命令字符串
	sprintf_s(szTemp,"BEGIN:L0107022:04-14:42:45-051781  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
		"&ACC_USER=%s&ACC_USER_ROLE=1&ACCOUNT=%s&CURRENCY=%s&BLN_ADJUST_AMT=10000.00&AVL_ADJUST_AMT=10000.00&OP_REMARK=]",
		g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
		g_pCfg->GetCustID().GetBuffer(), g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

	BOOL bRet = SendKcxpMsg(&szTemp[0]);

	// 清空日志解析，便于下一次操作
	g_pParseKcbpLog->Clean();

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	return TRUE;
}
