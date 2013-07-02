#include "StdAfx.h"
#include "SH_BOND_ETFVistor.h"
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

CSH_BOND_ETFVistor::CSH_BOND_ETFVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;
	m_fInsteadAmt = 0;
	m_nEtfSecuCnt = 0;

	m_fMatched_OrderFrzAmt = m_fMatched_Price = m_nMatched_Qty = m_fMatchedAmt = m_fMatched_SettAmt = 0;

	m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;

	m_fCptlBln_Old = m_fCptlAvl_Old = m_fCptlTrdFrz_Old = m_fCptlOutstanding_Old = m_fCptlOtdAvl_Old = 0;

	m_nShareBln_New = m_nShareAvl_New = m_nShareTrdFrz_New = m_nShareOtd_New = 0;

	m_fCptlBln_New = m_fCptlAvl_New = m_fCptlTrdFrz_New = m_fCptlOutstanding_New = m_fCptlOtdAvl_New = 0;

	strcpy_s(m_szServiceName, "上海债券ETF业务");

	strcpy_s(m_szETF_Intl, "1511011");

	strcpy_s(m_szSecu_Code, "511011");
	strcpy_s(m_szSecu_Intl, "1511011");
	strcpy_s(m_szQty, "10000"); // 申购数量必须是1W的整数倍
	strcpy_s(m_szPrice, "1");

	// 沪A
	strcpy_s(m_szMarket_Board, "10");	
	strcpy_s(m_szTrdId, "0B");
	strcpy_s(m_szCurrency, "0");  // 人民币	
}

CSH_BOND_ETFVistor::~CSH_BOND_ETFVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CSH_BOND_ETFVistor::Vistor()
{
	BOOL bRet = TRUE;

	if (!GetEtfInfo(m_szETF_Intl)
	|| !GetEtfSecuInfo(m_szETF_Intl))
	{
		// ETF 配置出错,直接报错
		bRet = FALSE;
		g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "ETF %s Info Config Wrong!", m_szETF_Intl);
	}
	else
	{
		// 一篮子ETF数量
		sprintf_s(m_szQty, "%d", m_pEtfInfo->nCreationRedemptionUnit);

		/*ExecTestCase1(TestCase_1, "国债ETF买入 0B", "511010", "0B");
		ExecTestCase1(TestCase_1, "国债ETF卖出 0S", "511010", "0S");	
		ExecTestCase1(TestCase_2, "国债ETF申购 7K(无现金替代)", "511011", "7K");
		ExecTestCase1(TestCase_3, "国债ETF申购 7K(部分现金替代)", "511011", "7K");
		*/
		ExecTestCase1(TestCase_4, "国债ETF赎回 7L", "511011", "7L");	
	}
	
	return bRet;
}

BOOL CSH_BOND_ETFVistor::ResultStrToTable(char *pRetStr)
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

/************************************************************************/
/* 测试用例:买入/卖出
/************************************************************************/
BOOL CSH_BOND_ETFVistor::TestCase_1(char *pEtf_Code, char* pTrd_Id)
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, pTrd_Id);
	strcpy_s(m_szSecu_Code, pEtf_Code);
	sprintf_s(m_szSecu_Intl, "1%s", pEtf_Code);

	// 获取当前价格
	GetSecuCurPrice(m_szSecu_Intl, m_szPrice);

	// 存钱
	SaveCapital();

	// 存股份
	SaveShares();

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// 拼接发送给KCXP的命令字符串
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

		if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
		{
			ParseKcxpRetMsg();
		}
	}

	if (bRet == FALSE)
	{
		return FALSE;	
	}

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData_1();

	return bRet;
}

/************************************************************************/
/* 测试用例:申购, 无现金替代
/************************************************************************/
BOOL CSH_BOND_ETFVistor::TestCase_2(char *pEtf_Code, char* pTrd_Id)
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, pTrd_Id);
	strcpy_s(m_szSecu_Code, pEtf_Code);
	sprintf_s(m_szSecu_Intl, "1%s", pEtf_Code);

	// 获取申赎单位, 使用最小单位进行委托		
	sprintf_s(m_szQty, "%d", m_pEtfInfo->nCreationRedemptionUnit);

	// ETF申赎,委托价格固定为1元
	strcpy_s(m_szPrice, "1");

	// 存钱
	SaveCapital();

	// 普通ETF,国债ETF申购,需要给不能现金替代的成份股,预先存入股份
	if (StrCmp(m_szTrdId, "7K") == 0)
	{		
		// 存入股份
		SaveEtfSecuShares();
	}
	else
	{
		SaveShares();
	}

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// 拼接发送给KCXP的命令字符串
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

		if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
		{
			ParseKcxpRetMsg();
		}
	}

	if (bRet == FALSE)
	{
		return FALSE;	
	}

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData_2();

	return bRet;
}

/************************************************************************/
/* 测试用例:申购, 部分现金替代
/************************************************************************/
BOOL CSH_BOND_ETFVistor::TestCase_3(char *pEtf_Code, char* pTrd_Id)
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, pTrd_Id);
	strcpy_s(m_szSecu_Code, pEtf_Code);
	sprintf_s(m_szSecu_Intl, "1%s", pEtf_Code);

	// 获取申赎单位, 使用最小单位进行委托		
	sprintf_s(m_szQty, "%d", m_pEtfInfo->nCreationRedemptionUnit);

	// ETF申赎,委托价格固定为1元
	strcpy_s(m_szPrice, "1");

	// 存钱
	SaveCapital();

	// 存入成份股股份,为构成部分现金替代的情况,将第一个成份股的股份可用修改为0
	SaveEtfSecuShares(1);

	// 计算现金替代金额, 必须要在下委托前计算
	CalInsteadAmt(m_fInsteadAmt);

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// 拼接发送给KCXP的命令字符串
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

		if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
		{
			ParseKcxpRetMsg();
		}
	}

	if (bRet == FALSE)
	{
		return FALSE;	
	}

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData_3();

	return bRet;
}

/************************************************************************/
/* 测试用例:赎回
/************************************************************************/
BOOL CSH_BOND_ETFVistor::TestCase_4(char *pEtf_Code, char* pTrd_Id)
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, pTrd_Id);
	strcpy_s(m_szSecu_Code, pEtf_Code);
	sprintf_s(m_szSecu_Intl, "1%s", pEtf_Code);

	// 获取申赎单位, 使用最小单位进行委托		
	sprintf_s(m_szQty, "%d", m_pEtfInfo->nCreationRedemptionUnit);

	// ETF申赎,委托价格固定为1元
	strcpy_s(m_szPrice, "1");

	// 存钱
	SaveCapital();

	// 存入一手511010
	SaveShares();

	// 计算现金替代金额, 必须要在下委托前计算
	CalInsteadAmt(m_fInsteadAmt);

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// 拼接发送给KCXP的命令字符串
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

		if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
		{
			ParseKcxpRetMsg();
		}
	}

	if (bRet == FALSE)
	{
		return FALSE;	
	}

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData_3();

	return bRet;
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
BOOL CSH_BOND_ETFVistor::InitUserData()
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取Shares表
	CString strSql;

	// TODO:债券ETF 写死为1511010
	strSql.Format("select * from  shares where account = %s and secu_intl = 1511010",
		g_pCfg->GetAccount().GetBuffer());

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		// 客户当前没有持仓此股票
		if (pRecordSet->adoEOF)
		{
			m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;
		}

		while(!pRecordSet->adoEOF)
		{	
			// 1.1 股份余额
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_Old);

			// 1.2 股份可用
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_Old);

			// 1.3 交易冻结
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_Old);

			// 1.4 在途数量
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_Old);

			// 1.5 在途可用, ETF申赎当天可用
			DB_GET_VALUE_INT("SHARE_OTD_AVL", m_nSHareOtd_Avl_Old);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();

		// 获取成份股的股份余额
		GetEtfSecuShares(1);
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

		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		if (pRecordSet->adoEOF)
		{
			// 数据库返回的结果为空
			pRecordSet->Close();

			g_pLog->WriteRunLogEx(__FILE__,__LINE__, "获取Capitals表数据失败!");
			return FALSE;
		}

		while(!pRecordSet->adoEOF)
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

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();
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


BOOL CSH_BOND_ETFVistor::UpdateUserData()
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取Shares表
	CString strSql;

	// TODO:债券ETF写死为1511010
	strSql.Format("select * from  shares where account = %s and secu_intl = 1511010",
		g_pCfg->GetAccount().GetBuffer());

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		while(!pRecordSet->adoEOF)
		{	
			// 1.1 股份余额
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_New);

			// 1.2 股份可用
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_New);

			// 1.3 交易冻结
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_New);

			// 1.4 在途数量
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_New);

			// 1.5 在途可用, ETF申赎当天可用
			DB_GET_VALUE_INT("SHARE_OTD_AVL", m_nSHareOtd_Avl_New);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();

		// 获取成份股的股份余额
		GetEtfSecuShares(2);

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

		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		if (pRecordSet->adoEOF)
		{
			// 数据库返回的结果为空
			pRecordSet->Close();
			g_pLog->WriteRunLogEx(__FILE__,__LINE__,"获取Capitals表数据失败!");
			return FALSE;
		}

		while(!pRecordSet->adoEOF)
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

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();
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

BOOL CSH_BOND_ETFVistor::GetMatchedData()
{
	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");
	BOOL bRet = TRUE;

	try
	{
		// 1.1 检查柜台matching表中的数据是否与委托一致，并获取【持仓成本】
		CString strSql;

		// 买入ETF份额
		strSql.Format("select * from  matching where trd_date = %s and trd_id = '%s'and order_id = '%s'"
			" and account = %s and secu_intl = %s and order_qty = %s",
			strDate, m_szTrdId, m_pMsg[0].szOrderID, m_pMsg[0].szAccount, m_szSecu_Intl, m_szQty);

		m_nWaitMatchingCnt = 0;
		BSTR bstrSQL = strSql.AllocSysString();
		_RecordsetPtr pRecordSet;

		while(m_nWaitMatchingCnt < MAX_WAIT_MATCH_CNT)
		{						
			pRecordSet.CreateInstance(__uuidof(Recordset));

			if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
				return FALSE;

			if (pRecordSet->adoEOF && m_nWaitMatchingCnt == MAX_WAIT_MATCH_CNT - 1)
			{
				// 数据库返回的结果为空
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.1 Fail!");
				pRecordSet->Close();
				return FALSE;
			}
			else if (pRecordSet->adoEOF)
			{
				pRecordSet->Close();
			}
			else if (!pRecordSet->adoEOF)
			{
				break;;
			}

			m_nWaitMatchingCnt++;
			Sleep(g_pCfg->GetRefreshDBGap());
		}

		_variant_t TheValue; //VARIANT数据类型
		char szTmp[100] = {0};

		while(!pRecordSet->adoEOF)
		{								
			DB_GET_VALUE_FLOAT("ORDER_FRZ_AMT", m_fMatched_OrderFrzAmt);
			DB_GET_VALUE_FLOAT("MATCHED_PRICE", m_fMatched_Price);
			DB_GET_VALUE_INT("MATCHED_QTY", m_nMatched_Qty);			
			DB_GET_VALUE_FLOAT("MATCHED_AMT", m_fMatchedAmt);
			DB_GET_VALUE_FLOAT("SETT_AMT", m_fMatched_SettAmt);
			DB_GET_VALUE_INT("ORDER_QTY", m_nOrder_Qty);

			pRecordSet->MoveNext();
		}
		pRecordSet->Close();
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

BOOL CSH_BOND_ETFVistor::ChkData_1()
{
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

		if (strcmp(m_szTrdId, "0B") == 0)
		{
			// ETF买入

			//======================校验资金===============================

			// 1.3.检查captial表中【资金可用 AVAILABLE】减少 应该等于 Matching表中的【SETT_AMT】
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

			// 1.5 在途和在途可用，无变化
			if (CutFloatPnt( m_fCptlOutstanding_New - m_fCptlOutstanding_Old) != 0
				|| CutFloatPnt( m_fCptlOtdAvl_New - m_fCptlOtdAvl_Old) != 0)
			{			
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.5 Fail!");
			}

			//======================校验股份===============================

			// 1.6 检查Shares表中ETF份额的【SHARE_BLN】、【SHARE_AVL】、【SHARE_TRD_FRZ】无变化
			// TODO:国债ETF比较特殊,买入成交后,SHARE_AVL立即增加
			if (m_nShareBln_New - m_nShareBln_Old != 0				
				||m_nShareTrdFrz_New - m_nShareTrdFrz_Old != 0)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.6 Fail!");
			}	

			// 1.7 检查Shares表中ETF份额的【SHARE_AVL】,【SHARE_OTD】增加等于 matching表中的【MATCHED_QTY】
			if ((m_nShareOtd_New - m_nShareOtd_Old != m_nMatched_Qty)
				||(m_nShareAvl_New - m_nShareAvl_Old != m_nMatched_Qty))
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.7 Fail!");
			}
		}
		else if (strcmp(m_szTrdId, "0S") == 0)
		{
			// ETF卖出

			//======================校验资金===============================
			// 1.3.检查captial表中的【资金可用 AVAILABLE】、【OUTSTANDING】、【OTD_AVL】增加 应该等于Matching表中的【SETT_AMT】					
			if (abs(CutFloatPnt(m_fCptlAvl_New - m_fCptlAvl_Old) - m_fMatched_SettAmt) > 1
				|| abs(CutFloatPnt(m_fCptlOutstanding_New - m_fCptlOutstanding_Old) - m_fMatched_SettAmt) > 1
				|| abs(CutFloatPnt(m_fCptlOtdAvl_New- m_fCptlOtdAvl_Old) - m_fMatched_SettAmt) > 1)
			{								
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}

			// 1.4.检查captial表中【交易冻结 TRD_FRZ】无变化
			if (CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old) != 0)
			{			
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
			}

			//======================校验股份===============================
			// 1.5 检查Shares表中【股份余额 SHARE_BLN】不变
			if (m_nShareBln_New != m_nShareBln_Old)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.7 Fail!");
			}

			// 1.8 检查Shares表中【股份可用 SHARE_AVL】减少 应该等于Matching表中的【ORDER_QTY】			
			if (m_nShareAvl_Old - m_nShareAvl_New != m_nOrder_Qty)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.8 Fail!");
			}

			// 1.9 检查Shares表中【股份冻结 SHARE_TRD_FRZ】增加 应该等于Matching表中的【ORDER_QTY】-【MATCHED_qty】
			if ((m_nShareTrdFrz_New - m_nShareTrdFrz_Old) != (m_nOrder_Qty - m_nMatched_Qty))
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.9 Fail!");
			}

			// 2.0 检查Shares表中【在途股份】 不变
			if (m_nShareOtd_New != m_nShareOtd_Old)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 2.0 Fail!");
			}
		}
	}

	return bRet;
}

BOOL CSH_BOND_ETFVistor::ChkData_2()
{
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

		if (strcmp(m_szTrdId, "7K") == 0)
		{
			// 债券ETF申购

			//======================校验资金===============================
			// 无现金替代的情况

			// 1.3.检查captial表中的【资金可用 AVAILABLE】、【FROZEN】、【TRD_FRZ】、【OUTSTANDING】、【OTD_AVL】保存不变
			if (m_fCptlAvl_Old != m_fCptlAvl_New
				|| m_fCptlTrdFrz_Old != m_fCptlTrdFrz_New
				|| m_fCptlOutstanding_Old != m_fCptlOutstanding_New
				|| m_fCptlOtdAvl_Old != m_fCptlOtdAvl_New)
			{								
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}			

			//======================校验股份===============================
			// 1.4 成份股交易交易冻结,可用减少[SHARE_AVL]减少,[SHARE_TRD_FRZ]增加 等于 etf_secu_info表[SHARE_QTY]乘以篮子数
			int nBasketNum = m_nOrder_Qty / m_pEtfInfo->nCreationRedemptionUnit;

			for (int n=0; n<m_nEtfSecuCnt; n++)
			{				
				if (m_pEtfSecuInfo[n].shares_old.nShareAvl - m_pEtfSecuInfo[n].shares_new.nShareAvl != nBasketNum * m_pEtfSecuInfo[n].nShareQty
					|| m_pEtfSecuInfo[n].shares_new.nShareTrdFrz - m_pEtfSecuInfo[n].shares_old.nShareTrdFrz  != nBasketNum * m_pEtfSecuInfo[n].nShareQty)
				{
					bRet = FALSE;
					g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail! seccu_intl = %s", m_pEtfSecuInfo[n].szSecuIntl);
				}
			}

			// 1.5 正股股份可用,股份在途,股份在途可用增加,其它字段无变化
			// [SHARE_AVL], {SHARE_OTD}, [SHARE_OTD_AVL]增加等于MATCHING表中[MATCHED_QTY]
			if (m_nShareBln_New != m_nShareBln_Old
				|| m_nShareTrdFrz_New != m_nShareTrdFrz_Old
				|| m_nShareAvl_New - m_nShareAvl_Old != m_nMatched_Qty				
				|| m_nShareOtd_New - m_nShareOtd_Old != m_nMatched_Qty)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.5 Fail!");
			}
		}
	}

	return bRet;
}

BOOL CSH_BOND_ETFVistor::ChkData_3()
{
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

		if (strcmp(m_szTrdId, "7K") == 0)
		{
			// 债券ETF申购

			//======================校验资金===============================
			// 无现金替代的情况		

			// 1.3.检查captial表中的【资金可用 AVAILABLE】减少等于现金替代金额
			if (abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) - CutFloatPnt(m_fInsteadAmt)) > 1)
			{				
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}

			// 1.4.检查captial表中交易冻结],[在途],[在途可用]不变
			if ( m_fCptlTrdFrz_Old != m_fCptlTrdFrz_New
				|| m_fCptlOutstanding_Old != m_fCptlOutstanding_New
				|| m_fCptlOtdAvl_Old != m_fCptlOtdAvl_New)
			{								
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
			}			

			//======================校验股份===============================
			// 1.5 成份股交易交易冻结,可用减少[SHARE_AVL]减少,[SHARE_TRD_FRZ]增加 等于 etf_secu_info表[SHARE_QTY]乘以篮子数
			int nBasketNum = m_nOrder_Qty / m_pEtfInfo->nCreationRedemptionUnit;

			for (int n=0; n<m_nEtfSecuCnt; n++)
			{	
				// 如果成份股可用小于替代数量,则该成份股使用了现金替代,不用比较
				if (m_pEtfSecuInfo[n].shares_old.nShareAvl < m_pEtfSecuInfo[n].nShareQty)
				{
					continue;
				}

				if (m_pEtfSecuInfo[n].shares_old.nShareAvl - m_pEtfSecuInfo[n].shares_new.nShareAvl != nBasketNum * m_pEtfSecuInfo[n].nShareQty
					|| m_pEtfSecuInfo[n].shares_new.nShareTrdFrz - m_pEtfSecuInfo[n].shares_old.nShareTrdFrz  != nBasketNum * m_pEtfSecuInfo[n].nShareQty)
				{
					bRet = FALSE;
					g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.5 Fail! seccu_intl = %s", m_pEtfSecuInfo[n].szSecuIntl);
				}
			}

			// 1.6 正股股份可用,股份在途,股份在途可用增加,其它字段无变化
			// [SHARE_AVL], {SHARE_OTD}, [SHARE_OTD_AVL]增加等于MATCHING表中[MATCHED_QTY]
			if (m_nShareBln_New != m_nShareBln_Old
				|| m_nShareTrdFrz_New != m_nShareTrdFrz_Old
				|| m_nShareAvl_New - m_nShareAvl_Old != m_nMatched_Qty				
				|| m_nShareOtd_New - m_nShareOtd_Old != m_nMatched_Qty)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.6 Fail!");
			}
		}
	}

	return bRet;
}

// 客户资金蓝补
BOOL CSH_BOND_ETFVistor::SaveCapital()
{
	// 资金余额蓝补1W, 资金可用蓝补1W

	// 发送数据
	char szTemp[2048] = {0};
	BOOL bRet = TRUE;

	// 拼接发送给KCXP的命令字符串
	sprintf_s(szTemp,"BEGIN:L0107022:04-14:42:45-051781  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
		"&ACC_USER=%s&ACC_USER_ROLE=1&ACCOUNT=%s&CURRENCY=%s&BLN_ADJUST_AMT=10000.00&AVL_ADJUST_AMT=10000.00&OP_REMARK=]",
		g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
		g_pCfg->GetCustID().GetBuffer(), g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

	if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
	{
		ParseKcxpRetMsg();
	}

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	return bRet;
}

BOOL CSH_BOND_ETFVistor::SaveShares()
{
	// 发送数据
	char szTemp[2048] = {0};
	BOOL bRet = TRUE;

	// 拼接发送给KCXP的命令字符串
	// 债券ETF, 固定存入511010
	sprintf_s(szTemp,"BEGIN:L0301009:04-10:03:27-577242  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
		"&OPER_FLAG=1&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=1511010&SEAT=%s&EXT_INST=0&CQLB=0--存入股份&QTY=%s&AVG_PRICE=%s&OP_REMARK=]",
		g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
		g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
		m_szSecu_Intl, g_pCfg->GetSHA_BIND_SEAT(), m_szQty, m_szPrice);

	// TODO:由于L0301009 执行成功，也返回FALSE，因此返回值无效， 此函数永远返回TRUE
	if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
	{
		ParseKcxpRetMsg();
	}

	// 休眠，等待数据库更新
	Sleep(g_pCfg->GetRefreshDBGap());

	return bRet;
}

// 给客户存入成份股
BOOL CSH_BOND_ETFVistor::SaveEtfSecuShares(int nType)
{
	// 发送数据
	char szTemp[2048] = {0};
	BOOL bRet = TRUE;

	for (int i=0; i<m_nEtfSecuCnt; i++)
	{		
		// 先清空客户已拥有的的成份股股份
		try
		{			
			//下面是查询数据的例子
			CString strSql;
			strSql.Format("delete from shares where account = %s and secu_intl = %s", g_pCfg->GetAccount(), m_pEtfSecuInfo[i].szSecuIntl);

			BSTR bstrSQL = strSql.AllocSysString();

			_variant_t RecordsAffected; //VARIANT数据类型

			g_pDBConn->m_pConnection->BeginTrans();
			g_pDBConn->m_pConnection->Execute(bstrSQL,&RecordsAffected,adCmdText);
			g_pDBConn->m_pConnection->CommitTrans();			
		}
		catch (_com_error e)//异常处理
		{
			CString strMsg;
			strMsg.Format(_T("错误描述：%s\n错误消息%s"),
				(LPCTSTR)e.Description(),
				(LPCTSTR)e.ErrorMessage());

			return FALSE;
		}

		if (nType == 1 && i == 0)
		{
			// 构造部分现金替代的情况
			continue;
		}
		else
		{
			// 存入成份股股份
			// 拼接发送给KCXP的命令字符串		
			sprintf_s(szTemp,"BEGIN:L0301009:04-10:03:27-577242  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
				"&OPER_FLAG=1&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SEAT=%s&EXT_INST=0&CQLB=0--存入股份&QTY=%d&AVG_PRICE=%s&OP_REMARK=]",
				g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
				g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
				m_pEtfSecuInfo[i].szSecuIntl, g_pCfg->GetSHA_BIND_SEAT(), m_pEtfSecuInfo[i].nShareQty, m_szPrice);

			// TODO:由于L0301009 执行成功，也返回FALSE，因此返回值无效， 此函数永远返回TRUE
			if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
			{
				ParseKcxpRetMsg();
			}

			// 休眠，等待数据库更新
			Sleep(g_pCfg->GetRefreshDBGap());				
		}		
	}

	return bRet;
}


void CSH_BOND_ETFVistor::ParseKcxpRetMsg()
{
	m_pMsg = new MID_403_ORDER_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_403_ORDER_RET_MSG)*m_nRowNum);

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

BOOL CSH_BOND_ETFVistor::GetEtfSecuInfo( char *pEtfIntl )
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取Shares表
	CString strSql;
	float fPrice = 0;

	strSql.Format("select * from etf_secu_info where etf_intl = %s",pEtfIntl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet))
		{
			return FALSE;
		}

		// 该ETF没有成份股信息, 说明成份股信息未导入
		if (pRecordSet->adoEOF || (m_nEtfSecuCnt = pRecordSet->RecordCount) == 0)
		{
			return FALSE;
		}

		// 初始化成份股信息
		m_pEtfSecuInfo = new ETF_SECU_INFO[m_nEtfSecuCnt];
		if (NULL == m_pEtfSecuInfo)
		{
			g_pLog->WriteRunLogEx(__FILE__, __LINE__, "初始化成份股信息 失败!");
			return FALSE;
		}

		int n = 0;
		while(!pRecordSet->adoEOF)
		{	
			// ETF成份股信息
			DB_GET_VALUE_STRING("SECU_INTL", m_pEtfSecuInfo[n].szSecuIntl);
			DB_GET_VALUE_INT("SHARE_QTY", m_pEtfSecuInfo[n].nShareQty);
			DB_GET_VALUE_INT("INSTEAD_FLAG", m_pEtfSecuInfo[n].nInsteadFlag);
			DB_GET_VALUE_FLOAT("PRICE_RATIO", m_pEtfSecuInfo[n].fPrice_Ratio);						

			pRecordSet->MoveNext();
			n++;
		}

		pRecordSet->Close();		
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

/************************************************************************/
/* nType: 1为获取历史股份,2为获取最新的股份
*/
/************************************************************************/
BOOL CSH_BOND_ETFVistor::GetEtfSecuShares(int nType)
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取Shares表
	CString strSql;

	for(int i=0; i<m_nEtfSecuCnt; i++)
	{
		strSql.Format("select * from shares where secu_intl = %s and account = %s", m_pEtfSecuInfo[i].szSecuIntl, g_pCfg->GetAccount());

		BSTR bstrSQL = strSql.AllocSysString();

		try
		{
			_RecordsetPtr pRecordSet;
			pRecordSet.CreateInstance(__uuidof(Recordset));

			if (!g_pDBConn->QueryData(bstrSQL, pRecordSet))
			{
				return FALSE;
			}

			// 没有数据
			if (pRecordSet->adoEOF)
			{
				return FALSE;
			}

			while(!pRecordSet->adoEOF)
			{	
				if (nType == 1)
				{
					// 获取初始股份数据

					// 1.1 股份余额
					DB_GET_VALUE_INT("SHARE_BLN", m_pEtfSecuInfo[i].shares_old.nShareBln);

					// 1.2 股份可用
					DB_GET_VALUE_INT("SHARE_AVL", m_pEtfSecuInfo[i].shares_old.nShareAvl);

					// 1.3 交易冻结
					DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_pEtfSecuInfo[i].shares_old.nShareTrdFrz);

					// 1.4 在途数量
					DB_GET_VALUE_INT("SHARE_OTD", m_pEtfSecuInfo[i].shares_old.nShareOtd);
				}
				else
				{
					// 获取最新股份数据

					// 1.1 股份余额
					DB_GET_VALUE_INT("SHARE_BLN", m_pEtfSecuInfo[i].shares_new.nShareBln);

					// 1.2 股份可用
					DB_GET_VALUE_INT("SHARE_AVL", m_pEtfSecuInfo[i].shares_new.nShareAvl);

					// 1.3 交易冻结
					DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_pEtfSecuInfo[i].shares_new.nShareTrdFrz);

					// 1.4 在途数量
					DB_GET_VALUE_INT("SHARE_OTD", m_pEtfSecuInfo[i].shares_new.nShareOtd);
				}

				pRecordSet->MoveNext();
			}

			pRecordSet->Close();
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

	return TRUE;
}

BOOL CSH_BOND_ETFVistor::GetEtfInfo( char *pEtfIntl )
{
	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	// 1. 获取etf_info表
	CString strSql;
	float fPrice = 0;

	strSql.Format("select * from etf_info where etf_intl = %s",pEtfIntl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet))
		{
			return FALSE;
		}

		// 查询不到ETF信息
		if (pRecordSet->adoEOF)
		{
			return FALSE;
		}

		// 初始化成份股信息
		m_pEtfInfo = new ETF_INFO;
		if (NULL == m_pEtfInfo)
		{
			g_pLog->WriteRunLogEx(__FILE__, __LINE__, "初始化ETF信息失败!");
			return FALSE;
		}

		int n = 0;
		while(!pRecordSet->adoEOF)
		{	
			// 当前价格
			DB_GET_VALUE_STRING("ETF_INTL", m_pEtfInfo->szEtfIntl);
			DB_GET_VALUE_INT("CREATIONREDEMPTIONUNIT", m_pEtfInfo->nCreationRedemptionUnit);
			DB_GET_VALUE_FLOAT("MAXCASHRATIO", m_pEtfInfo->fMaxCashRatio);
			DB_GET_VALUE_FLOAT("ESTIMATECASHCOMPONENT", m_pEtfInfo->fEstimateCashComponent);
			DB_GET_VALUE_FLOAT("CASHCOMPONENT", m_pEtfInfo->fCashComponent);
			DB_GET_VALUE_FLOAT("NAVPERCU", m_pEtfInfo->fNavPerCu);
			DB_GET_VALUE_FLOAT("NAV", m_pEtfInfo->fNav);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();		
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

BOOL CSH_BOND_ETFVistor::CalInsteadAmt(float &fInsteatAmt)
{
	_variant_t TheValue; //VARIANT数据类型
	CString strSql;

	strSql.Format("SELECT SUM(%s / a.creationredemptionunit * CASE "
		"WHEN b.share_avl - b.share_qty > 0 THEN 0 "
		"ELSE b.share_qty - b.share_avl END * b.closing_price * (1 + b.price_ratio)) AS INSTEAD_AMT "
		"from (select creationredemptionunit, maxcashratio from etf_info "
		"where etf_intl = %s) A, (SELECT A.SECU_INTL, CASE	WHEN B.SECU_CLS IN ('1', '2', '7', 'D') THEN "
		"B.SHARE_AVL ELSE COALESCE(B.SHARE_AVL - B.SHARE_OTD_AVL, 0) END as SHARE_AVL, COALESCE(B.SHARE_OTD - B.SHARE_OTD_AVL, 0) as SHARE_OTD, "
		"A.SHARE_QTY, A.INSTEAD_FLAG, A.INSTEAD_AMT, A.PRICE_RATIO,	A.CREATIONAMT, A.REDEPTIONAMT, A.MARKET, COALESCE(C.CLOSING_PRICE, 0) as CLOSING_PRICE, "
		"COALESCE(C.CURRENT_PRICE, 0) as CURRENT_PRICE,	(CASE WHEN D.SECU_CLS IN ('1', '2', '7', 'D') THEN	COALESCE(C.BOND_INT, 0.0) "
		"ELSE 0.0 END) FROM ETF_SECU_INFO A, SECU_MKT_INFO C, SECURITIES D, (SELECT * FROM SHARES WHERE ACCOUNT = %s AND SECU_ACC = '%s' "
		"AND SEAT = %s AND EXT_INST = 0) B WHERE A.ETF_INTL = %s AND A.SECU_INTL = B.SECU_INTL(+) AND A.SECU_INTL = C.SECU_INTL(+) AND A.SECU_INTL = D.SECU_INTL) B ",
		m_szQty, m_szSecu_Intl, g_pCfg->GetAccount(), g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetSHA_BIND_SEAT(), m_szSecu_Intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet))
		{
			return FALSE;
		}

		// 查询不到信息
		if (pRecordSet->adoEOF)
		{
			return FALSE;
		}

		// 初始化成份股信息


		int n = 0;
		while(!pRecordSet->adoEOF)
		{	
			// 当前价格		
			DB_GET_VALUE_FLOAT("INSTEAD_AMT", fInsteatAmt);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();		
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
