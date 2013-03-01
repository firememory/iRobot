#include "StdAfx.h"
#include "SZA_BuyVistor.h"
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

CSZA_BuyVistor::CSZA_BuyVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;

	m_fMatched_OrderFrzAmt = m_fMatched_Price = m_nMatched_Qty = m_fMatchedAmt = m_fMatched_SettAmt = 0;

	m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;

	m_fCptlBln_Old = m_fCptlAvl_Old = m_fCptlTrdFrz_Old = m_fCptlOutstanding_Old = m_fCptlOtdAvl_Old = 0;

	m_nShareBln_New = m_nShareAvl_New = m_nShareTrdFrz_New = m_nShareOtd_New = 0;
	
	m_fCptlBln_New = m_fCptlAvl_New = m_fCptlTrdFrz_New = m_fCptlOutstanding_New = m_fCptlOtdAvl_New = 0;

	strcpy_s(m_szServiceName, "����A������");
	strcpy_s(m_szSecu_Code, "000002");
	strcpy_s(m_szSecu_Intl, "2");
	strcpy_s(m_szQty, "200");
	strcpy_s(m_szPrice, "2.55");

	// ��A
	strcpy_s(m_szMarket_Board, "00");	
	strcpy_s(m_szTrdId, "0B");
}

CSZA_BuyVistor::~CSZA_BuyVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CSZA_BuyVistor::Vistor()
{
	BOOL bRet = TRUE;

	ExecTestCase(TestCase_1, "�޼����� 0B");
	ExecTestCase(TestCase_2, "���ַ����ż۸�ί�� YB");
	ExecTestCase(TestCase_3, "�������м۸�ί�� XB");
	ExecTestCase(TestCase_4, "��ʱ�ɽ�ʣ�೷��ί�� 2B");
	ExecTestCase(TestCase_5, "�嵵��ʱ�ɽ�ʣ�೷�� VB");
	ExecTestCase(TestCase_6, "ȫ��ɽ�����ί�� WB");

	return bRet;
}

BOOL CSZA_BuyVistor::ResultStrToTable(char *pRetStr)
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

// �޼�����
BOOL CSZA_BuyVistor::TestCase_1()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "0B");

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

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

BOOL CSZA_BuyVistor::TestCase_2()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "YB");

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

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

BOOL CSZA_BuyVistor::TestCase_3()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "XB");

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

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


BOOL CSZA_BuyVistor::TestCase_4()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "2B");

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

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


BOOL CSZA_BuyVistor::TestCase_5()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "VB");

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

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


BOOL CSZA_BuyVistor::TestCase_6()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "WB");

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
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

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

BOOL CSZA_BuyVistor::SendKcxpMsg(char *pCmd)
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

					m_pMsg = new MID_403_ORDER_RET_MSG[m_nRowNum];
					memset(m_pMsg, 0x00, sizeof(MID_403_ORDER_RET_MSG)*m_nRowNum);
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
BOOL CSZA_BuyVistor::InitUserData()
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
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		// �ͻ���ǰû�гֲִ˹�Ʊ
		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;
		}

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


BOOL CSZA_BuyVistor::UpdateUserData()
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

BOOL CSZA_BuyVistor::GetMatchedData()
{
	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");
	BOOL bRet = TRUE;

	try
	{
		// 1.1 ����̨matching���е������Ƿ���ί��һ�£�����ȡ���ֲֳɱ���
		CString strSql;

		if (strcmp(m_szTrdId, "0B") == 0)
		{
			// �޼�����
			strSql.Format("select * from  matching where trd_date = %s and trd_id = '%s'and order_id = '%s'"
				" and account = %s and secu_intl = %s and order_price = %s and order_qty = %s",
				strDate, m_szTrdId, m_pMsg[0].szOrderID, m_pMsg[0].szAccount, m_szSecu_Intl, m_szPrice, m_szQty);
		}
		else
		{
			// �м����� �����ж�ί�м۸�
			strSql.Format("select * from  matching where trd_date = %s and trd_id = '%s'and order_id = '%s'"
				" and account = %s and secu_intl = %s and order_qty = %s and matched_amt > 0",
				strDate, m_szTrdId, m_pMsg[0].szOrderID, m_pMsg[0].szAccount, m_szSecu_Intl, m_szQty);
		}

		BSTR bstrSQL = strSql.AllocSysString();
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

		_variant_t TheValue; //VARIANT��������
		char szTmp[100] = {0};

		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
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
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		bRet = FALSE;
	}

	return bRet;
}

BOOL CSZA_BuyVistor::SendMidMsg(char *pCmd)
{
	g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Send:%s", pCmd);
	if (m_pKDGateWay->WaitAnswer(pCmd)!=TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[403] ί�нӿ�, ����ʧ��!");
		return FALSE;
	}

	// �Թ�̨���ص�ֵ���н���
	g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "Recv:%s", m_pKDGateWay->m_pReturnData);
	if (ResultStrToTable(m_pKDGateWay->m_pReturnData) != TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[403] ί�нӿ�, ����ʧ��!");
		return FALSE;
	}

	return TRUE;
}

BOOL CSZA_BuyVistor::ChkData()
{
	/*
		���������Ϣ
		1.1.����̨matching���е������Ƿ���ί��һ��	
		1.2.���captial���С��ʽ���
		1.3.���captial���С��ʽ���á�
		1.4.���captial���С����׶��᡿
		1.5 ���Shares���С���;�ɷݡ�
	*/

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
			// �޼����룬TRD_FRZ���� ���� 
			// 1.3.���captial���С��ʽ���� AVAILABLE������ Ӧ�õ��� Matching���еġ�ORDER_FRZ_AMT��
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
		}
		else if (strcmp(m_szTrdId, "YB") == 0)
		{
			// �м�ί�� ���ַ����ż۸�ί��
			// 1.3.���captial���С��ʽ���� AVAILABLE������ Ӧ�õ���Matching���еġ�SETT_AMT��
			// ���ھ������⣬����1���ڵĲ���
			float n1 = CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New);
			float n2 = abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) - m_fMatched_SettAmt);

			if (abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) - m_fMatched_SettAmt) > 1)
			{								
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}

			// 1.4.���captial���С����׶��� TRD_FRZ������ ���� 0
			if (CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old) != 0)
			{
				float n1 = m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old;
				float n2 = m_fMatched_SettAmt;

				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
			}
		}
		else if (strcmp(m_szTrdId, "XB") == 0 || strcmp(m_szTrdId, "2B") == 0 
			|| strcmp(m_szTrdId, "VB") == 0 || strcmp(m_szTrdId, "WB") == 0)
		{
			// �м�ί�� �������м۸�ί�� XB
			// �м�ί�� ��ʱ�ɽ�ʣ�೷��ί�� 2B
			// �м�ί�� �嵵��ʱ�ɽ�ʣ�೷�� VB
			// �м�ί�� ȫ��ɽ�����ί�� WB

			// 1.3.���captial���С��ʽ���� AVAILABLE������ Ӧ�õ���Matching���еġ�ORDER_FRZ_AMT��
			// ���ھ������⣬����1���ڵĲ���
			float n1 = CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New);
			float n2 = abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) - CutFloatPnt(m_fMatched_OrderFrzAmt));

			if (abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) - CutFloatPnt(m_fMatched_OrderFrzAmt)) > 1)
			{
				float n1 = CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New);
				float n2 = abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) != CutFloatPnt(m_fMatched_OrderFrzAmt));

				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}

			// 1.4.���captial���С����׶��� TRD_FRZ������ ����Matching���еġ�ORDER_FRZ_AMT��-��SETT_AMT��
			// ���ھ������⣬����1���ڵĲ���
			n1 = CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old);
			float n3 =  CutFloatPnt( m_fMatched_OrderFrzAmt - m_fMatched_SettAmt);
			n2 = abs(CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old) - CutFloatPnt( m_fMatched_OrderFrzAmt - m_fMatched_SettAmt));

			if (abs(CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old) - CutFloatPnt( m_fMatched_OrderFrzAmt - m_fMatched_SettAmt)) > 1)
			{
				float n1 = CutFloatPnt(m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old);
				float n2 = CutFloatPnt( m_fMatched_OrderFrzAmt - m_fMatched_SettAmt);

				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
			}
		}

		// 1.5 ���Shares���С���;�ɷݡ�����matching���еġ��ɽ�������
		if (m_nShareOtd_New - m_nShareOtd_Old != m_nMatched_Qty)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.5 Fail!");
		}
	}

	return bRet;
}
