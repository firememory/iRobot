#include "StdAfx.h"
#include "SZA_XJ_BuyVistor.h"
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

CSZA_XJ_BuyVistor::CSZA_XJ_BuyVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;

	m_fMatched_OrderFrzAmt = m_fMatched_Price = m_fMatched_Qty = m_fMatchedAmt = m_fMatched_SettAmt = 0;

	m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;

	m_fCptlBln_Old = m_fCptlAvl_Old = m_fCptlTrdFrz_Old = m_fCptlOutstanding_Old = m_fCptlOtdAvl_Old = 0;

	m_nShareBln_New = m_nShareAvl_New = m_nShareTrdFrz_New = m_nShareOtd_New = 0;
	
	m_fCptlBln_New = m_fCptlAvl_New = m_fCptlTrdFrz_New = m_fCptlOutstanding_New = m_fCptlOtdAvl_New = 0;

	strcpy_s(m_szTestCaseName, "����A��-�޼�����");
	strcpy_s(m_szSecu_intl, "000002");
	strcpy_s(m_szQty, "100");
	strcpy_s(m_szPrice, "1");
}

CSZA_XJ_BuyVistor::~CSZA_XJ_BuyVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CSZA_XJ_BuyVistor::Vistor()
{
	BOOL bRet = TRUE;	

	if ((bRet = InitUserData()))
	{
		bRet = ChkPnt1();
	}	

	return bRet;
}

BOOL CSZA_XJ_BuyVistor::ResultStrToTable(char *pRetStr)
{
	CKDGateway *pKDGateWay = g_pMidConn->GetKDGateWay();

	int nRecNo = pKDGateWay->GetRecNum();
	
	m_pMsg = new ORDER_403_RET_MSG[nRecNo];
	memset(m_pMsg, 0x00, sizeof(ORDER_403_RET_MSG)*nRecNo);

	int nLen = strlen(pRetStr);

	char *p = new char[nLen+1];

	strncpy(p, pRetStr, nLen);
	p[nLen] = '\0';

	// ��õڶ��У������ݿ�ʼ��
	char *q = pKDGateWay->GetNextLine(p);
	char *tmp = q;

	// ��ȡÿ�е�����
	for (DWORD nRow=0; nRow<pKDGateWay->GetRecNum(); nRow++)
	{
		// ��ȡÿ�еĳ���
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


BOOL CSZA_XJ_BuyVistor::ChkPnt1()
{
	/*
		���������Ϣ
		1.1.����̨matching���е������Ƿ���ί��һ��	
		1.2.���captial���С��ʽ�������
		1.3.���captial���С��ʽ���á����� matching���еġ����׶����
		1.4.���captial���С����׶��᡿���ӡ��ֲֳɱ���
		1.5 ���Shares���С���;�ɷݡ�����matching���еġ��ɽ�������
	*/
	BOOL bRet = TRUE;

	// ��������
	if (TRUE != SendMsg(NULL))
	{
		return FALSE;
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	// 1.1.����̨matching���е������Ƿ���ί��һ��	
	if (!(bRet = GetMatchedData()))
	{
		// ��ȡMatcing������ʧ��
		return bRet;
	}	

	// ��ȡ�ͻ����µĹɷݺ��ʽ�����
	if (!UpdateUserData())
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "���²�������ʧ��!");
		bRet = FALSE;
	}
	else
	{
		// 1.2.���captial���С��ʽ�������
		if (m_fCptlBln_Old != m_fCptlBln_New)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "Chk 1.2 Fail!");
		}

		// 1.3.���captial���С��ʽ���á����� matching���еġ����׶����
		if (m_fCptlAvl_Old - m_fCptlAvl_New != m_fMatched_OrderFrzAmt)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "Chk 1.3 Fail!");
		}

		// 1.4.���captial���С����׶��᡿����matching���еġ������
		if (m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old != m_fMatched_SettAmt)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "Chk 1.4 Fail!");
		}

		// 1.5 ���Shares���С���;�ɷݡ�����matching���еġ��ɽ�������
		if (m_nShareOtd_New - m_nShareOtd_Old != m_fMatched_Qty)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "Chk 1.5 Fail!");
		}
	}

	return bRet;
}

void CSZA_XJ_BuyVistor::ChkPnt2()
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

	try
	{
		BSTR bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		_variant_t TheValue; //VARIANT��������
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
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());
	}
}

BOOL CSZA_XJ_BuyVistor::SendMsg(char *pMsg)
{
	BOOL bRet = TRUE;

	if (g_pCfg->GetTestMode() == USE_MID)
	{
		bRet = SendMidMsg();
	}
	else
	{
		bRet = SendKcxpMsg();	
	}

	return bRet;
}

BOOL CSZA_XJ_BuyVistor::SendKcxpMsg()
{
	CKDMidCli *pKcxpConn = g_pKcxpConn->GetKdMidCli();

	if (NULL == pKcxpConn)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "��ȡKCXP����ʧ��!");
		return FALSE;
	}

	int iRetCode = KCBP_MSG_OK;
	char szTemp[64];

	pKcxpConn->BeginWrite();

	if ((iRetCode = pKcxpConn->SetValue("F_OP_USER",      g_pCfg->GetOpId().GetBuffer())) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_OP_ROLE",   "2")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_OP_SITE",   "999999999999999")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_OP_BRANCH", "999")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_CHANNEL",   '0')) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("F_SESSION", g_pKcxpConn->GetSession())) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("OPER_FLAG",   "1")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("CUSTOMER",   g_pCfg->GetCustID().GetBuffer())) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("MARKET",   "0")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("BOARD",   "0")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("SECU_ACC",   g_pCfg->GetSecu_Acc_SZA())) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("ACCOUNT",   g_pCfg->GetAccount().GetBuffer())) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("SECU_INTL",   m_szSecu_intl)) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("SEAT",   "002600")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("SERIAL_NO",   "-1")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("DELIST_CHANNEL",   "0")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("QTY",   m_szQty)) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("TRD_ID",   "0B")) != KCBP_MSG_OK
		|| (iRetCode = pKcxpConn->SetValue("PRICE",   m_szPrice)) != KCBP_MSG_OK)
	{		
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM[L0303001]���ò���ʧ��,ERRCODE = %ld",iRetCode);
		
		return FALSE;
	}

	if ((iRetCode = pKcxpConn->CallProgramAndCommit("L0303001")) != KCBP_MSG_OK)
	{	
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM[L0303001]����ʧ��,ERRCODE = %ld",iRetCode);
		return FALSE;
	}

	int nRow = 0;				

	if ((iRetCode = pKcxpConn->RsOpen()) == KCBP_MSG_OK)
	{
		// ��ȡ�����������ע�������ǰ�������ģ��������Ҫ��1
		pKcxpConn->RsGetRowNum(&nRow);
		
		if (nRow>1)
		{
			m_nRowNum = nRow - 1;
			
			m_pMsg = new ORDER_403_RET_MSG[m_nRowNum];
			memset(m_pMsg, 0x00, sizeof(ORDER_403_RET_MSG)*m_nRowNum);
		}
		else
		{
			g_pLog->WriteRunLogEx(__FILE__,__LINE__,"��������������쳣!");
			m_nRowNum = 0;
			return FALSE;
		}
						
		if ((iRetCode = pKcxpConn->RsFetchRow()) == KCBP_MSG_OK)
		{
			if ((iRetCode = pKcxpConn->RsGetCol(1, szTemp)) == KCBP_MSG_OK)
			{
				if ((iRetCode = pKcxpConn->RsGetCol(2, szTemp)) == KCBP_MSG_OK)
				{
					if(strcmp(szTemp,"0") != 0)
					{
						iRetCode = pKcxpConn->RsGetCol(3, szTemp);
						
						g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡ���������Ϣʧ��,ERRCODE = %ld", iRetCode);
						return FALSE;
					}
				}
			}
			else
			{
				g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡ���������Ϣʧ��,ERRCODE = %ld", iRetCode);
				
				return FALSE;
			}
		}

		//ȡ�ڶ����������		
		if (iRetCode = pKcxpConn->RsMore() == KCBP_MSG_OK)
		{
			int nRow = 0;
			while(nRow < m_nRowNum)
			{
				if(pKcxpConn->RsFetchRow() != KCBP_MSG_OK)
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
		g_pLog->WriteRunLogEx(__FILE__,__LINE__,"�򿪽����ʧ��,ERRCODE = %ld", iRetCode);
		
		return FALSE;
	}

	return TRUE;
}

/*
 *	��ȡ��������
 	1. Shares��
		1.1 �ɷ����
		1.2 �ɷݿ���
		1.3 ���׶���
		1.4 ��;����
	2. Capitals��
		2.1 �ʽ����
		2.2 �ʽ����
		2.3 ���׶���
		2.4 ��;�ʽ�
		2.5 ��;����
 */
BOOL CSZA_XJ_BuyVistor::InitUserData()
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
	CString strSql;
	strSql.Format("select * from  shares where account = %s and secu_intl = %s",
		g_pCfg->GetAccount().GetBuffer(), m_szSecu_intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{	
			// 1.1 �ɷ����
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_Old);

			// 1.2 �ɷݿ���
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_Old);

			// 1.3 ���׶���
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_Old);

			// 1.4 ��;����
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_Old);

			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}
	

	// 2. ��ȡCapitals��
	try
	{
		strSql.Format("select * from  capital where account = %s and currency = 0",
			g_pCfg->GetAccount().GetBuffer());

		bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			g_pDBConn->m_pRecordset->Close();

			g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡCapitals������ʧ��!");
			return FALSE;
		}

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{	
			// 2.1 �ʽ����
			DB_GET_VALUE_FLOAT("BALANCE", m_fCptlBln_Old);

			// 2.2 �ʽ����
			DB_GET_VALUE_FLOAT("AVAILABLE", m_fCptlAvl_Old);

			// 2.3 ���׶���
			DB_GET_VALUE_FLOAT("TRD_FRZ", m_fCptlTrdFrz_Old);

			// 2.4 ��;�ʽ�
			DB_GET_VALUE_FLOAT("OUTSTANDING", m_fCptlOutstanding_Old);

			// 2.5 ��;����
			DB_GET_VALUE_FLOAT("OTD_AVL", m_fCptlOtdAvl_Old);

			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}

	return TRUE;
}


BOOL CSZA_XJ_BuyVistor::UpdateUserData()
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
	CString strSql;
	strSql.Format("select * from  shares where account = %s and secu_intl = %s",
		g_pCfg->GetAccount().GetBuffer(), m_szSecu_intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{	
			// 1.1 �ɷ����
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_New);

			// 1.2 �ɷݿ���
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_New);

			// 1.3 ���׶���
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_New);

			// 1.4 ��;����
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_New);

			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}
	
	

	// 2. ��ȡCapitals��
	try
	{
		strSql.Format("select * from  capital where account = %s and currency = 0",
			g_pCfg->GetAccount().GetBuffer());

		bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			g_pDBConn->m_pRecordset->Close();
			g_pLog->WriteRunLogEx(__FILE__,__LINE__,"��ȡCapitals������ʧ��!");
			return FALSE;
		}

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{	
			// 2.1 �ʽ����
			DB_GET_VALUE_FLOAT("BALANCE", m_fCptlBln_New);

			// 2.2 �ʽ����
			DB_GET_VALUE_FLOAT("AVAILABLE", m_fCptlAvl_New);

			// 2.3 ���׶���
			DB_GET_VALUE_FLOAT("TRD_FRZ", m_fCptlTrdFrz_New);

			// 2.4 ��;�ʽ�
			DB_GET_VALUE_FLOAT("OUTSTANDING", m_fCptlOutstanding_New);

			// 2.5 ��;����
			DB_GET_VALUE_FLOAT("OTD_AVL", m_fCptlOtdAvl_New);

			g_pDBConn->m_pRecordset->MoveNext();
		}

		g_pDBConn->m_pRecordset->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}
	
	return TRUE;
}

BOOL CSZA_XJ_BuyVistor::GetMatchedData()
{
	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");
	BOOL bRet = TRUE;

	try
	{
		// 1.1 ����̨matching���е������Ƿ���ί��һ�£�����ȡ���ֲֳɱ���
		CString strSql;
		strSql.Format("select * from  matching where trd_date = %s and trd_id = '0B'and order_id = '%s'"
			" and account = %s and secu_intl = %s and order_price = %s and order_qty = %s",
			strDate, m_pMsg[0].szOrderID, m_pMsg[0].szAccount, m_szSecu_intl, m_szPrice, m_szQty);

		BSTR bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		_variant_t TheValue; //VARIANT��������
		char szTmp[100] = {0};

		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "Chk 1.1 Fail!");
			g_pDBConn->m_pRecordset->Close();
			return FALSE;
		}	

		while(!g_pDBConn->m_pRecordset->adoEOF)
		{								
			DB_GET_VALUE_FLOAT("ORDER_FRZ_AMT", m_fMatched_OrderFrzAmt);
			DB_GET_VALUE_FLOAT("MATCHED_PRICE", m_fMatched_Price);
			DB_GET_VALUE_FLOAT("MATCHED_QTY", m_fMatched_Qty);
			DB_GET_VALUE_FLOAT("MATCHED_AMT", m_fMatchedAmt);
			DB_GET_VALUE_FLOAT("SETT_AMT", m_fMatched_SettAmt);

			g_pDBConn->m_pRecordset->MoveNext();
		}
		g_pDBConn->m_pRecordset->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		bRet = FALSE;
	}

	return bRet;
}

BOOL CSZA_XJ_BuyVistor::SendMidMsg()
{
	CKDGateway *pKDGateWay = g_pMidConn->GetKDGateWay();

	char szTemp[512];		
	sprintf_s(szTemp,"403|%s|00|%s|%s||%s|0B|%s|%s||||||||||||",
		g_pCfg->GetCustID(), g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_intl, m_szPrice, m_szQty);

	if (pKDGateWay->WaitAnswer(&szTemp[0])!=TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[403] ί�нӿ�, ����ʧ��!");
		return FALSE;
	}

	// �Թ�̨���ص�ֵ���н���
	if (ResultStrToTable(pKDGateWay->m_pReturnData) != TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[403] ί�нӿ�, ����ʧ��!");
		return FALSE;
	}

	return TRUE;
}
