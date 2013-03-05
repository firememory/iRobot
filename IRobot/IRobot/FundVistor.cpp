#include "StdAfx.h"
#include "FundVistor.h"
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

CFundVistor::CFundVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;

	m_fOrderAmt = m_fOrderFrzAmt = 0;

	m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;

	m_fCptlBln_Old = m_fCptlAvl_Old = m_fCptlTrdFrz_Old = m_fCptlOutstanding_Old = m_fCptlOtdAvl_Old = 0;

	m_nShareBln_New = m_nShareAvl_New = m_nShareTrdFrz_New = m_nShareOtd_New = 0;
	
	m_fCptlBln_New = m_fCptlAvl_New = m_fCptlTrdFrz_New = m_fCptlOutstanding_New = m_fCptlOtdAvl_New = 0;

	strcpy_s(m_szServiceName, "����ҵ��");

	strcpy_s(m_szFund_Intl, "1");
	strcpy_s(m_szOrderAmt, "10000");
	strcpy_s(m_szTrdId, "22");  // �깺
	strcpy_s(m_szTA_Code, "3");
	strcpy_s(m_szCurrency, "0"); // �����
}

CFundVistor::~CFundVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CFundVistor::Vistor()
{
	BOOL bRet = TRUE;
	
	ExecTestCase(TestCase_1, "�깺 22");
	//ExecTestCase(TestCase_2, "��� 24");
	//ExecTestCase(TestCase_3, "�Ϲ� 20");

	return bRet;
}

BOOL CFundVistor::ResultStrToTable(char *pRetStr)
{
	m_nRowNum = m_pKDGateWay->GetRecNum();
	
	m_pMsg = new MID_821_ORDER_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_821_ORDER_RET_MSG)*m_nRowNum);

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
						SERVICE_STRNCPY(szAccount);
						break;
					case 1:
						SERVICE_STRNCPY(szOrderSn);
						break;
					case 2:						
						SERVICE_STRNCPY(szAppSn);
						break;
					case 3:						
						SERVICE_STRNCPY(szOrder_Amt);
						break;
					case 4:						
						SERVICE_STRNCPY(szOrder_Frz_Amt);
						break;
					case 5:						
						SERVICE_STRNCPY(szFee_Type);
						break;
					case 6:						
						SERVICE_STRNCPY(szDiscount_Ratio);
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

// �깺
BOOL CFundVistor::TestCase_1()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "22");

	SaveCapital();

	bRet = InitUserData();
	if (bRet == FALSE)
	{
		return FALSE;
	}

	// ��������
	char szTemp[2048] = {0};
	if (g_pCfg->GetTestMode() == USE_MID)
	{
		sprintf_s(szTemp,"821|%s|%s|%s|%s|%s||%s|||",
			m_szTA_Code, g_pCfg->GetCustID(), g_pCfg->GetAccount(), m_szTrdId, m_szFund_Intl, m_szOrderAmt);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���
		sprintf_s(szTemp,"BEGIN:L0403001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&TA_CODE=%s&CUSTOMER=%s&ACCOUNT=%s&TRD_ID=%s&FUND_INTL=%s&ORDER_AMT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			m_szTA_Code, g_pCfg->GetCustID(), g_pCfg->GetAccount(), m_szTrdId, m_szFund_Intl, m_szOrderAmt);

		bRet = SendKcxpMsg(&szTemp[0]);

		// �����־������������һ�β���
		g_pParseKcbpLog->Clean();
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

BOOL CFundVistor::SendKcxpMsg(char *pCmd)
{
	g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, pCmd);

	int iRetCode = KCBP_MSG_OK;
	char szTemp[512] = {0};

	if (NULL == m_pKcxpConn)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "��ȡKCXP����ʧ��!");
		return FALSE;
	}	

	// ������Ϣ
	try
	{
		// ��������
		g_pParseKcbpLog->ParseCmd(pCmd);

		// ��KCXP��������
		if (FALSE != g_pParseKcbpLog->ExecSingleCmd())
		{
			// ��ȡִ�н��
			int nRow = 0;				

			if ((iRetCode = m_pKcxpConn->RsOpen()) == KCBP_MSG_OK)
			{
				// ��ȡ�����������ע�������ǰ�������ģ��������Ҫ��1
				m_pKcxpConn->RsGetRowNum(&nRow);

				if (nRow>1)
				{
					m_nRowNum = nRow - 1;

					m_pMsg = new MID_821_ORDER_RET_MSG[m_nRowNum];
					memset(m_pMsg, 0x00, sizeof(MID_821_ORDER_RET_MSG)*m_nRowNum);
				}
				else
				{
					g_pLog->WriteRunLogEx(__FILE__,__LINE__,"��������������쳣!");
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
				if (iRetCode = m_pKcxpConn->RsMore() == KCBP_MSG_OK)
				{
					int nRow = 0;
					while(nRow < m_nRowNum)
					{
						if(m_pKcxpConn->RsFetchRow() != KCBP_MSG_OK)
						{

							break;
						}

						SERVICE_KCXP_STRNCPY("ACCOUNT", szAccount);
						SERVICE_KCXP_STRNCPY("ORDER_SN", szOrderSn);
						SERVICE_KCXP_STRNCPY("APP_SN", szAppSn);
						SERVICE_KCXP_STRNCPY("ORDER_AMT", szOrder_Amt);
						SERVICE_KCXP_STRNCPY("ORDER_FRZ_AMT", szOrder_Frz_Amt);
						SERVICE_KCXP_STRNCPY("FEE_TYPE", szFee_Type);
						SERVICE_KCXP_STRNCPY("DISCOUNT_RATIO", szDiscount_Ratio);								
						nRow++;
					}		
				}
			}
			else
			{	
				g_pLog->WriteRunLogEx(__FILE__,__LINE__,"�򿪽����ʧ��,ERRCODE = %ld", iRetCode);

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
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM[L0303001]�����쳣��");
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
BOOL CFundVistor::InitUserData()
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
	CString strSql;
	BSTR bstrSQL;

	// 2. ��ȡCapitals��
	try
	{
		strSql.Format("select * from  capital where account = %s and currency = %s",
			g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

		bstrSQL = strSql.AllocSysString();
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet))
		{
			return FALSE;
		}

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

BOOL CFundVistor::GetOrderData()
{
	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");
	BOOL bRet = TRUE;

	try
	{
		// 1.1 ����̨Orders���е������Ƿ���ί��һ��
		CString strSql;

		strSql.Format("select * from fund_orders where trd_date = %s and account = %s and trd_id = '%s' and fund_intl = %s "
			"and order_sn = '%s' and app_sn = '%s' and ta_code = '%s' and order_amt = %s ",
			strDate, g_pCfg->GetAccount(), m_szTrdId, m_szFund_Intl,
			m_pMsg[0].szOrderSn, m_pMsg[0].szAppSn, m_szTA_Code, m_szOrderAmt);
		
		BSTR bstrSQL = strSql.AllocSysString();
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		_variant_t TheValue; //VARIANT��������
		char szTmp[100] = {0};

		if (pRecordSet->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.1 Fail!");
			pRecordSet->Close();
			return FALSE;
		}

		while(!pRecordSet->adoEOF)
		{								
			DB_GET_VALUE_FLOAT("ORDER_FRZ_AMT", m_fOrderFrzAmt);
			DB_GET_VALUE_FLOAT("ORDER_AMT", m_fOrderAmt);
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

BOOL CFundVistor::ChkData()
{
	BOOL bRet = TRUE;

	// 1.1.����̨Orders���е������Ƿ���ί��һ��	
	if (!(bRet = GetOrderData()))
	{
		// ��ȡMatcing������ʧ��
		return bRet;
	}

	return bRet;
}

// �ͻ��ʽ�����
BOOL CFundVistor::SaveCapital()
{
	// �ʽ��������1W, �ʽ��������1W

	// ��������
	char szTemp[2048] = {0};

	// ƴ�ӷ��͸�KCXP�������ַ���
	sprintf_s(szTemp,"BEGIN:L0107022:04-14:42:45-051781  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
		"&ACC_USER=%s&ACC_USER_ROLE=1&ACCOUNT=%s&CURRENCY=%s&BLN_ADJUST_AMT=10000.00&AVL_ADJUST_AMT=10000.00&OP_REMARK=]",
		g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
		g_pCfg->GetCustID().GetBuffer(), g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

	BOOL bRet = SendKcxpMsg(&szTemp[0]);

	// �����־������������һ�β���
	g_pParseKcbpLog->Clean();

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	return TRUE;
}


BOOL CFundVistor::SaveShares()
{
	// ��������
	char szTemp[2048] = {0};

	// TODO:����L0301009 ִ�гɹ���Ҳ����FALSE����˷���ֵ��Ч�� �˺�����Զ����TRUE
	BOOL bRet = SendKcxpMsg(&szTemp[0]);

	// �����־������������һ�β���
	g_pParseKcbpLog->Clean();

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	return TRUE;
}