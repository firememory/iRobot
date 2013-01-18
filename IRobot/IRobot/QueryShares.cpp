#include "StdAfx.h"
#include "QueryShares.h"

#include "Cfg.h"
#include "MidConn.h"
#include "loginterface.h"
#include "DBConnect.h"

#include "public.h"

extern CCfg *g_pCfg;
extern CMidConn *g_pMidConn;
extern CLoginterface *g_pLog;
extern CDBConnect *g_pDBConn;


CQueryShares::CQueryShares(void)
{
	m_pMsg = NULL;
	m_nRecNum = 0;
}

CQueryShares::~CQueryShares(void)
{
	DELCLS(m_pMsg);
}

BOOL CQueryShares::Vistor()
{
	ChkPnt1();

	return TRUE;
}

BOOL CQueryShares::ResultStrToTable( char *pRetStr )
{
	CKDGateway *pKDGateWay = g_pMidConn->GetKDGateWay();

	int nRecNo = pKDGateWay->GetRecNum();
	m_nRecNum = nRecNo;

	m_pMsg = new MID_QUERY_SHARES_504[nRecNo];
	memset(m_pMsg, 0x00, sizeof(MID_QUERY_SHARES_504)*nRecNo);

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
						SERVICE_STRNCPY(szUserCode);
						break;
					case 1:
						SERVICE_STRNCPY(szAccount);
						break;
					case 2:						
						SERVICE_STRNCPY(szMarket);
						break;
					case 3:						
						SERVICE_STRNCPY(szSecuAcc);
						break;
					case 4:						
						SERVICE_STRNCPY(szSecuName);
						break;
					case 5:						
						SERVICE_STRNCPY(szSecuCode);
						break;
					case 6:						
						SERVICE_STRNCPY(szSecuIntl);
						break;
					case 7:						
						SERVICE_STRNCPY(szSeat);
						break;
					case 8:						
						SERVICE_STRNCPY(szBranch);
						break;
					case 9:						
						SERVICE_STRNCPY(szExtInst);
						break;
					case 10:						
						SERVICE_STRNCPY(szCurrency);
						break;
					case 11:						
						SERVICE_STRNCPY(szShareBln);
						break;
					case 12:						
						SERVICE_STRNCPY(szShareAvl);
						break;
					case 13:						
						SERVICE_STRNCPY(szShareTrdFrz);
						break;
					case 14:						
						SERVICE_STRNCPY(szShareOtd);
						break;
					case 15:						
						SERVICE_STRNCPY(szShareFrz);
						break;
					case 16:						
						SERVICE_STRNCPY(szShareUnTrdQty);
						break;
					case 17:						
						SERVICE_STRNCPY(szCurrentCost);
						break;
					case 18:						
						SERVICE_STRNCPY(szMktValOld);
						break;
					case 19:						
						SERVICE_STRNCPY(szCostPrice);
						break;
					case 20:						
						SERVICE_STRNCPY(szShareOtdAvl);
						break;
					case 21:						
						SERVICE_STRNCPY(szCurrPrice);
						break;
					case 22:						
						SERVICE_STRNCPY(szCost2Price);
						break;
					case 23:						
						SERVICE_STRNCPY(szMktQty);
						break;
					case 24:						
						SERVICE_STRNCPY(szMktVal);
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

BOOL CQueryShares::SendMsg( char *pMsg )
{
	CKDGateway *pKDGateWay = g_pMidConn->GetKDGateWay();

	if (pKDGateWay->WaitAnswer(pMsg)!=TRUE)
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

BOOL CQueryShares::ChkPnt1()
{
	BOOL bRet = TRUE;
	char szMsg[512];
	sprintf_s(szMsg,"504|18798721|85807073|||||");

	if (TRUE != SendMsg(szMsg))
	{
		return FALSE;
	}

	CString strSql;
	strSql.Format("select * from shares where account = 85807073");

	BSTR bstrSQL = strSql.AllocSysString();
	g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 

	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	while(!g_pDBConn->m_pRecordset->adoEOF)
	{	
		TheValue = g_pDBConn->m_pRecordset->GetCollect("SECU_INTL");

		if(TheValue.vt!=VT_NULL)
		{	
			strncpy_s(szTmp, (char*)_bstr_t(TheValue), 100);

			for (int i=0;i<m_nRecNum;i++)
			{	
				if (!strcmp(m_pMsg[i].szSecuIntl, szTmp))
				{
					g_pLog->WriteRunLog(MID_MODE, LOG_DEBUG, "��ʼ�ȽϹ�Ʊ:%s", szTmp);

					// ���SHARE_BLN �ɷ����
					DB_COMPARE("SHARE_BLN", szShareBln);					

					// У��	SHARE_AVL �ɷݿ���
					DB_COMPARE("SHARE_AVL", szShareAvl);					

					// У�� SHARE_TRD_FRZ ���׶���
					DB_COMPARE("SHARE_TRD_FRZ", szShareTrdFrz);										
				
					// У�� SHARE_OTD ��;����
					DB_COMPARE("SHARE_OTD", szShareOtd);					

					// У�� CURRENT_COST ����ɱ�
					DB_COMPARE("CURRENT_COST", szCurrentCost);

					// У�� MKT_VAL ��ֵ
					DB_COMPARE("MKT_VAL", szMktVal);

					break;
				}
			}
		}
		g_pDBConn->m_pRecordset->MoveNext();
	}

	g_pDBConn->m_pRecordset->Close();

	return bRet;
}
