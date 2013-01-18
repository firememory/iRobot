#include "StdAfx.h"
#include "BuyVistor.h"
#include "Cfg.h"
#include "MidConn.h"
#include "loginterface.h"
#include "DBConnect.h"

extern CCfg *g_pCfg;
extern CMidConn *g_pMidConn;
extern CLoginterface *g_pLog;
extern CDBConnect *g_pDBConn;

CBuyVistor::CBuyVistor(void)
{
	m_pMsg = NULL;
}

CBuyVistor::~CBuyVistor(void)
{
	if (NULL != m_pMsg)
	{
		delete m_pMsg;
		m_pMsg = NULL;
	}
}

BOOL CBuyVistor::Vistor()
{
	// 正常参数 
	/*
		检查一下信息
		1.matcing表中是否有该记录，获取【持仓成本】
		2.captial表中【资金余额】不变
		3.captial表中【资金可用】减少“委托金额+5”
		4.captial表中【现金可取】 = 【资金可用】
		5.captial表中【支票可取】 = 【资金可用】
		4.captial表中【交易冻结】增加【持仓成本】
		5.如果模拟成交是半数成交的话，shares表中【在途数量】增加“委托数量/2”
	*/
	ChkPnt1();

	// 买入数量超过最大可买，检查系统是否校验
	ChkPnt2();

	return TRUE;
}

BOOL CBuyVistor::ResultStrToTable(char *pRetStr)
{
	CKDGateway *pKDGateWay = g_pMidConn->GetKDGateWay();

	int nRecNo = pKDGateWay->GetRecNum();
	
	m_pMsg = new MID_ORDER_403[nRecNo];
	memset(m_pMsg, 0x00, sizeof(MID_ORDER_403)*nRecNo);

	int nLen = strlen(pRetStr);

	char *p = new char[nLen+1];

	strncpy(p, pRetStr, nLen);
	p[nLen] = '\0';

	// 获得第二行，即数据开始行
	char *q = pKDGateWay->GetNextLine(p);
	char *tmp = q;

	// 获取每行的数据
	for (DWORD nRow=0; nRow<pKDGateWay->GetRecNum(); nRow++)
	{
		// 获取每列的长度
		for (DWORD nCol=0; nCol<pKDGateWay->GetFieldNum(); nCol++)
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

void CBuyVistor::ChkPnt1()
{
	char szTemp[512];
	sprintf_s(szTemp,"403|18798721|00|0123492912|85807073||000002|0B|2.00|200||||||||||||");

	if (TRUE != SendMsg(szTemp))
	{

	}

	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");

	CString strSql;
	strSql.Format("select * from  matching where trd_date = %s and order_id = '%s'",
		strDate, m_pMsg[0].szOrderID);

	BSTR bstrSQL = strSql.AllocSysString();
	g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	while(!g_pDBConn->m_pRecordset->adoEOF)
	{	
		TheValue = g_pDBConn->m_pRecordset->GetCollect("CUST_CODE");
		if(TheValue.vt!=VT_NULL)
		{				
			strncpy_s(szTmp, (char*)_bstr_t(TheValue), 100);
		}
		g_pDBConn->m_pRecordset->MoveNext();
	}

	g_pDBConn->m_pRecordset->Close();
}

void CBuyVistor::ChkPnt2()
{
	char szTemp[512];
	sprintf_s(szTemp,"403|18798721|00|0123492912|85807073||000002|0B|2.00|200||||||||||||");

	if (TRUE != SendMsg(szTemp))
	{

	}

	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");

	CString strSql;
	strSql.Format("select * from  matching where trd_date = %s and order_id = '%s'",
		strDate, m_pMsg[0].szOrderID);

	BSTR bstrSQL = strSql.AllocSysString();
	g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

	_variant_t TheValue; //VARIANT数据类型
	char szTmp[100] = {0};

	while(!g_pDBConn->m_pRecordset->adoEOF)
	{	
		TheValue = g_pDBConn->m_pRecordset->GetCollect("CUST_CODE");
		if(TheValue.vt!=VT_NULL)
		{				
			strncpy_s(szTmp, (char*)_bstr_t(TheValue), 100);
		}
		g_pDBConn->m_pRecordset->MoveNext();
	}

	g_pDBConn->m_pRecordset->Close();
}

BOOL CBuyVistor::SendMsg( char *pMsg)
{
	CKDGateway *pKDGateWay = g_pMidConn->GetKDGateWay();

	if (pKDGateWay->WaitAnswer(pMsg)!=TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[403] 委托接口, 调用失败!");
		return FALSE;
	}

	// 对柜台返回的值进行解析
	if (ResultStrToTable(pKDGateWay->m_pReturnData) != TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[403] 委托接口, 解析失败!");
		return FALSE;
	}

	return TRUE;
	
}
