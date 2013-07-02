#include "StdAfx.h"
#include "SHA_BOND_ETFVistor.h"
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

	m_fMatched_OrderFrzAmt = m_fMatched_Price = m_nMatched_Qty = m_fMatchedAmt = m_fMatched_SettAmt = 0;

	m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;

	m_fCptlBln_Old = m_fCptlAvl_Old = m_fCptlTrdFrz_Old = m_fCptlOutstanding_Old = m_fCptlOtdAvl_Old = 0;

	m_nShareBln_New = m_nShareAvl_New = m_nShareTrdFrz_New = m_nShareOtd_New = 0;

	m_fCptlBln_New = m_fCptlAvl_New = m_fCptlTrdFrz_New = m_fCptlOutstanding_New = m_fCptlOtdAvl_New = 0;

	strcpy_s(m_szServiceName, "�Ϻ�ծȯETFҵ��");

	strcpy_s(m_szSecu_Code, "511011");
	strcpy_s(m_szSecu_Intl, "1511011");
	strcpy_s(m_szQty, "10000"); // �깺����������1W��������
	strcpy_s(m_szPrice, "1");

	// ��A
	strcpy_s(m_szMarket_Board, "10");	
	strcpy_s(m_szTrdId, "0B");
	strcpy_s(m_szCurrency, "0");  // �����
}

CSH_BOND_ETFVistor::~CSH_BOND_ETFVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CSH_BOND_ETFVistor::Vistor()
{
	BOOL bRet = TRUE;

	ExecTestCase1(TestCase_1, "��ծETF���� 0B", "511010", "0B");
	ExecTestCase1(TestCase_1, "��ծETF���� 0S", "511010", "0S");	

	/*
	ExecTestCase1(TestCase_1, "��ծETF�깺 7K", "511011", "7K");
	ExecTestCase1(TestCase_1, "��ծETF��� 7L", "511011", "7L");	
	*/
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

	// ��õڶ��У������ݿ�ʼ��
	char *q = m_pKDGateWay->GetNextLine(p);
	char *tmp = q;

	// ��ȡÿ�е�����
	for (DWORD nRow=0; nRow<m_nRowNum; nRow++)
	{
		// ��ȡÿ�еĳ���
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
/* ��������:
��ͨETF 510050 ����/����
��ծETF 511010 ����/����
�羳ETF 510900 ����/����
����ETF 511990 ����/����
/************************************************************************/
BOOL CSH_BOND_ETFVistor::TestCase_1(char *pEtf_Code, char* pTrd_Id)
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, pTrd_Id);
	strcpy_s(m_szSecu_Code, pEtf_Code);
	sprintf_s(m_szSecu_Intl, "1%s", pEtf_Code);

	if (StrCmp(m_szTrdId, "0B") == 0 || StrCmp(m_szTrdId, "0S") == 0)
	{
		// ��ȡ��ǰ�۸�
		GetSecuCurPrice(m_szSecu_Intl, m_szPrice);
	}	
	else
	{
		// ETF����,ί�м۸�̶�Ϊ1Ԫ
		strcpy_s(m_szPrice, "1");
	}

	// ��ȡETF����
	GetEtfInfo(m_szSecu_Intl);

	// ��ȡETF�ɷݹ���Ϣ
	GetEtfSecuInfo(m_szSecu_Intl);

	// ��Ǯ
	SaveCapital();

	// ���
	if (StrCmp(m_szTrdId, "0B") == 0 || StrCmp(m_szTrdId, "7K") == 0)
	{
	}
	else
	{
		SaveShares();
	}

	// ��ͨETF,��ծETF�깺,��Ҫ�������ֽ�����ĳɷݹ�,Ԥ�ȴ���ɷ�
	if (StrCmp(m_szTrdId, "7K") == 0)
	{
		SaveEtfSecuShares();
	}

	bRet = InitUserData();
	if (bRet == FALSE)
	{
		return FALSE;
	}

	// ��������
	char szTemp[2048] = {0};
	if (g_pCfg->GetTestMode() == USE_MID)
	{
		sprintf_s(szTemp,"403|%s|%s|%s|%s||%s|%s|%s|%s||||||||||||",
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���
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

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData();

	return bRet;
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
BOOL CSH_BOND_ETFVistor::InitUserData()
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
	CString strSql;
	strSql.Format("select * from  shares where account = %s and secu_intl = %s",
		g_pCfg->GetAccount().GetBuffer(), m_szSecu_Intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		// �ͻ���ǰû�гֲִ˹�Ʊ
		if (pRecordSet->adoEOF)
		{
			m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;
		}

		while(!pRecordSet->adoEOF)
		{	
			// 1.1 �ɷ����
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_Old);

			// 1.2 �ɷݿ���
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_Old);

			// 1.3 ���׶���
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_Old);

			// 1.4 ��;����
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_Old);

			// 1.5 ��;����, ETF���굱�����
			DB_GET_VALUE_INT("SHARE_OTD_AVL", m_nSHareOtd_Avl_Old);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();

		// ��ȡ�ɷݹɵĹɷ����
		GetEtfSecuShares(1);
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
		strSql.Format("select * from  capital where account = %s and currency = %s",
			g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

		bstrSQL = strSql.AllocSysString();

		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		if (pRecordSet->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			pRecordSet->Close();

			g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡCapitals������ʧ��!");
			return FALSE;
		}

		while(!pRecordSet->adoEOF)
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

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();
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


BOOL CSH_BOND_ETFVistor::UpdateUserData()
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
	CString strSql;
	strSql.Format("select * from  shares where account = %s and secu_intl = %s",
		g_pCfg->GetAccount().GetBuffer(), m_szSecu_Intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		while(!pRecordSet->adoEOF)
		{	
			// 1.1 �ɷ����
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_New);

			// 1.2 �ɷݿ���
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_New);

			// 1.3 ���׶���
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_New);

			// 1.4 ��;����
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_New);

			// 1.5 ��;����, ETF���굱�����
			DB_GET_VALUE_INT("SHARE_OTD_AVL", m_nSHareOtd_Avl_New);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();

		// ��ȡ�ɷݹɵĹɷ����
		GetEtfSecuShares(2);

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
		strSql.Format("select * from  capital where account = %s and currency = %s",
			g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

		bstrSQL = strSql.AllocSysString();

		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		if (pRecordSet->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			pRecordSet->Close();
			g_pLog->WriteRunLogEx(__FILE__,__LINE__,"��ȡCapitals������ʧ��!");
			return FALSE;
		}

		while(!pRecordSet->adoEOF)
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

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();
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

BOOL CSH_BOND_ETFVistor::GetMatchedData()
{
	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");
	BOOL bRet = TRUE;

	try
	{
		// 1.1 ����̨matching���е������Ƿ���ί��һ�£�����ȡ���ֲֳɱ���
		CString strSql;

		// ����ETF�ݶ�
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
				// ���ݿⷵ�صĽ��Ϊ��
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

		_variant_t TheValue; //VARIANT��������
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
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		bRet = FALSE;
	}

	return bRet;
}

BOOL CSH_BOND_ETFVistor::ChkData()
{
	BOOL bRet = TRUE;

	// 1.1.����̨matching���е������Ƿ���ί��һ��	
	if (!(bRet = GetMatchedData()))
	{
		// ��ȡMatcing������ʧ��
		return bRet;
	}	

	// ��ȡ�ͻ����µĹɷݺ��ʽ�����
	if (!UpdateUserData())
	{
		g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "���²�������ʧ��!");
		bRet = FALSE;
	}
	else
	{
		// 1.2.���captial���С��ʽ�������
		if (m_fCptlBln_Old != m_fCptlBln_New)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.2 Fail!");
		}		

		if (strcmp(m_szTrdId, "0B") == 0)
		{
			// ETF����

			//======================У���ʽ�===============================

			// 1.3.���captial���С��ʽ���� AVAILABLE������ Ӧ�õ��� Matching���еġ�SETT_AMT��
			// ���ھ������⣬����1���ڵĲ���
			if (abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) - m_fMatched_OrderFrzAmt) > 1)
			{				
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}

			// 1.4.���captial���С����׶��� TRD_FRZ������ ����Matching���еġ�ORDER_FRZ_AMT��-��SETT_AMT��
			if (abs(CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old) - CutFloatPnt(m_fMatched_OrderFrzAmt - m_fMatched_SettAmt)) > 1)
			{
				float n1 = m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old;
				float n2 = m_fMatched_SettAmt;

				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
			}

			// 1.5 ��;����;���ã��ޱ仯
			if (CutFloatPnt( m_fCptlOutstanding_New - m_fCptlOutstanding_Old) != 0
				|| CutFloatPnt( m_fCptlOtdAvl_New - m_fCptlOtdAvl_Old) != 0)
			{			
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.5 Fail!");
			}

			//======================У��ɷ�===============================
			
			// 1.6 ���Shares����ETF�ݶ�ġ�SHARE_BLN������SHARE_AVL������SHARE_TRD_FRZ���ޱ仯
			// TODO:��ծETF�Ƚ�����,����ɽ���,SHARE_AVL��������
			if (m_nShareBln_New - m_nShareBln_Old != 0
				||m_nShareAvl_New - m_nShareAvl_Old != 0
				||m_nShareTrdFrz_New - m_nShareTrdFrz_Old != 0)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.6 Fail!");
			}	

			// 1.7 ���Shares����ETF�ݶ�ġ�SHARE_AVL��,��SHARE_OTD�����ӵ��� matching���еġ�MATCHED_QTY��
			if ((m_nShareOtd_New - m_nShareOtd_Old != m_nMatched_Qty)
				||(m_nShareAvl_New - m_nShareAvl_Old != m_nMatched_Qty))
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.7 Fail!");
			}
		}
		else if (strcmp(m_szTrdId, "0S") == 0)
		{
			// ETF����

			//======================У���ʽ�===============================
			// 1.3.���captial���еġ��ʽ���� AVAILABLE������OUTSTANDING������OTD_AVL������ Ӧ�õ���Matching���еġ�SETT_AMT��					
			if (abs(CutFloatPnt(m_fCptlAvl_New - m_fCptlAvl_Old) - m_fMatched_SettAmt) > 1
				|| abs(CutFloatPnt(m_fCptlOutstanding_New - m_fCptlOutstanding_Old) - m_fMatched_SettAmt) > 1
				|| abs(CutFloatPnt(m_fCptlOtdAvl_New- m_fCptlOtdAvl_Old) - m_fMatched_SettAmt) > 1)
			{								
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}

			// 1.4.���captial���С����׶��� TRD_FRZ���ޱ仯
			if (CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old) != 0)
			{			
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
			}

			//======================У��ɷ�===============================
			// 1.5 ���Shares���С��ɷ���� SHARE_BLN������
			if (m_nShareBln_New != m_nShareBln_Old)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.7 Fail!");
			}

			// 1.8 ���Shares���С��ɷݿ��� SHARE_AVL������ Ӧ�õ���Matching���еġ�ORDER_QTY��			
			if (m_nShareAvl_Old - m_nShareAvl_New != m_nOrder_Qty)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.8 Fail!");
			}

			// 1.9 ���Shares���С��ɷݶ��� SHARE_TRD_FRZ������ Ӧ�õ���Matching���еġ�ORDER_QTY��-��MATCHED_qty��
			if ((m_nShareTrdFrz_New - m_nShareTrdFrz_Old) != (m_nOrder_Qty - m_nMatched_Qty))
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.9 Fail!");
			}

			// 2.0 ���Shares���С���;�ɷݡ� ����
			if (m_nShareOtd_New != m_nShareOtd_Old)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 2.0 Fail!");
			}


		}
	}

	return bRet;
}

// �ͻ��ʽ�����
BOOL CSH_BOND_ETFVistor::SaveCapital()
{
	// �ʽ��������1W, �ʽ��������1W

	// ��������
	char szTemp[2048] = {0};
	BOOL bRet = TRUE;

	// ƴ�ӷ��͸�KCXP�������ַ���
	sprintf_s(szTemp,"BEGIN:L0107022:04-14:42:45-051781  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
		"&ACC_USER=%s&ACC_USER_ROLE=1&ACCOUNT=%s&CURRENCY=%s&BLN_ADJUST_AMT=10000.00&AVL_ADJUST_AMT=10000.00&OP_REMARK=]",
		g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
		g_pCfg->GetCustID().GetBuffer(), g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

	if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
	{
		ParseKcxpRetMsg();
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	return bRet;
}

BOOL CSH_BOND_ETFVistor::SaveShares()
{
	// ��������
	char szTemp[2048] = {0};
	BOOL bRet = TRUE;

	// ƴ�ӷ��͸�KCXP�������ַ���		
	sprintf_s(szTemp,"BEGIN:L0301009:04-10:03:27-577242  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
		"&OPER_FLAG=1&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SECU_NAME=%s&SEAT=%s&EXT_INST=0&CQLB=0--����ɷ�&QTY=%s&AVG_PRICE=%s&OP_REMARK=]",
		g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
		g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
		m_szSecu_Intl, "510050--50ETF", g_pCfg->GetSHA_BIND_SEAT(), m_szQty, m_szPrice);

	// TODO:����L0301009 ִ�гɹ���Ҳ����FALSE����˷���ֵ��Ч�� �˺�����Զ����TRUE
	if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
	{
		ParseKcxpRetMsg();
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	return bRet;
}

// ���ͻ�����ɷݹ�
BOOL CSH_BOND_ETFVistor::SaveEtfSecuShares()
{
	// ��������
	char szTemp[2048] = {0};
	BOOL bRet = TRUE;

	for (int i=0; i<m_nEtfSecuCnt; i++)
	{
		// ֻ�������ֽ�����ĳɷݹ�,����ɷ�
		if (m_pEtfSecuInfo[i].nInsteadFlag == 0)
		{
			// ƴ�ӷ��͸�KCXP�������ַ���		
			sprintf_s(szTemp,"BEGIN:L0301009:04-10:03:27-577242  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
				"&OPER_FLAG=1&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SEAT=%s&EXT_INST=0&CQLB=0--����ɷ�&QTY=%s&AVG_PRICE=%s&OP_REMARK=]",
				g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
				g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
				m_pEtfSecuInfo[i].szSecuIntl, g_pCfg->GetSHA_BIND_SEAT(), m_pEtfSecuInfo[i].nShareQty, m_szPrice);

			// TODO:����L0301009 ִ�гɹ���Ҳ����FALSE����˷���ֵ��Ч�� �˺�����Զ����TRUE
			if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
			{
				ParseKcxpRetMsg();
			}

			// ���ߣ��ȴ����ݿ����
			Sleep(g_pCfg->GetRefreshDBGap());
		}		
	}

	return bRet;
}


void CSH_BOND_ETFVistor::ParseKcxpRetMsg()
{
	m_pMsg = new MID_403_ORDER_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_403_ORDER_RET_MSG)*m_nRowNum);

	//ȡ���������
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
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
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

		// ��ETFû�гɷݹ���Ϣ, ˵���ɷݹ���Ϣδ����
		if (pRecordSet->adoEOF || (m_nEtfSecuCnt = pRecordSet->RecordCount) == 0)
		{
			return FALSE;
		}

		// ��ʼ���ɷݹ���Ϣ
		m_pEtfSecuInfo = new ETF_SECU_INFO[m_nEtfSecuCnt];
		if (NULL == m_pEtfSecuInfo)
		{
			g_pLog->WriteRunLogEx(__FILE__, __LINE__, "��ʼ���ɷݹ���Ϣ ʧ��!");
			return FALSE;
		}

		int n = 0;
		while(!pRecordSet->adoEOF)
		{	
			// ETF�ɷݹ���Ϣ
			DB_GET_VALUE_STRING("SECU_INTL", m_pEtfSecuInfo[n].szSecuIntl);
			DB_GET_VALUE_INT("SHARE_QTY", m_pEtfSecuInfo[n].nShareQty);
			DB_GET_VALUE_INT("INSTEAD_FLAG", m_pEtfSecuInfo[n].nInsteadFlag);
			DB_GET_VALUE_FLOAT("PRICE_RATIO", m_pEtfSecuInfo[n].fPrice_Ratio);						

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();		
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
}

/************************************************************************/
/* nType: 1Ϊ��ȡ��ʷ�ɷ�,2Ϊ��ȡ���µĹɷ�
*/
/************************************************************************/
BOOL CSH_BOND_ETFVistor::GetEtfSecuShares(int nType)
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
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

			// û������
			if (pRecordSet->adoEOF)
			{
				return FALSE;
			}

			while(!pRecordSet->adoEOF)
			{	
				if (nType == 1)
				{
					// ��ȡ��ʼ�ɷ�����

					// 1.1 �ɷ����
					DB_GET_VALUE_INT("SHARE_BLN", m_pEtfSecuInfo[i].shares_old.nShareBln);

					// 1.2 �ɷݿ���
					DB_GET_VALUE_INT("SHARE_AVL", m_pEtfSecuInfo[i].shares_old.nShareAvl);

					// 1.3 ���׶���
					DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_pEtfSecuInfo[i].shares_old.nShareTrdFrz);

					// 1.4 ��;����
					DB_GET_VALUE_INT("SHARE_OTD", m_pEtfSecuInfo[i].shares_old.nShareOtd);
				}
				else
				{
					// ��ȡ���¹ɷ�����

					// 1.1 �ɷ����
					DB_GET_VALUE_INT("SHARE_BLN", m_pEtfSecuInfo[i].shares_new.nShareBln);

					// 1.2 �ɷݿ���
					DB_GET_VALUE_INT("SHARE_AVL", m_pEtfSecuInfo[i].shares_new.nShareAvl);

					// 1.3 ���׶���
					DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_pEtfSecuInfo[i].shares_new.nShareTrdFrz);

					// 1.4 ��;����
					DB_GET_VALUE_INT("SHARE_OTD", m_pEtfSecuInfo[i].shares_new.nShareOtd);
				}

				pRecordSet->MoveNext();
			}

			pRecordSet->Close();
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
	}

	return TRUE;
}

BOOL CSH_BOND_ETFVistor::GetEtfInfo( char *pEtfIntl )
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡetf_info��
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

		// ��ѯ����ETF��Ϣ
		if (pRecordSet->adoEOF)
		{
			return FALSE;
		}

		// ��ʼ���ɷݹ���Ϣ
		m_pEtfInfo = new ETF_INFO;
		if (NULL == m_pEtfInfo)
		{
			g_pLog->WriteRunLogEx(__FILE__, __LINE__, "��ʼ��ETF��Ϣʧ��!");
			return FALSE;
		}

		int n = 0;
		while(!pRecordSet->adoEOF)
		{	
			// ��ǰ�۸�
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
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}	
}
