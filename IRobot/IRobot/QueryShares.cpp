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

	strcpy_s(m_szServiceName, "�ͻ��ɷݲ�ѯ");
}

CQueryShares::~CQueryShares(void)
{
	DELCLS(m_pMsg);
}

BOOL CQueryShares::Vistor()
{
	BOOL bRet = TRUE;
	bRet = ChkPnt1();

	return bRet;
}

BOOL CQueryShares::ResultStrToTable( char *pRetStr )
{	
	int nRecNo = m_pKDGateWay->GetRecNum();
	m_nRecNum = nRecNo;

	m_pMsg = new MID_504_QUERY_SHARES_RET_MSG[nRecNo];
	memset(m_pMsg, 0x00, sizeof(MID_504_QUERY_SHARES_RET_MSG)*nRecNo);

	int nLen = strlen(pRetStr);

	char *p = new char[nLen+1];

	strncpy(p, pRetStr, nLen);
	p[nLen] = '\0';

	// ��õڶ��У������ݿ�ʼ��
	char *q = m_pKDGateWay->GetNextLine(p);
	char *tmp = q;

	// ��ȡÿ�е�����
	for (DWORD nRow=0; nRow<m_pKDGateWay->GetRecNum(); nRow++)
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

BOOL CQueryShares::ChkPnt1()
{
	BOOL bRet = TRUE;
	char szMsg[512];
	sprintf_s(szMsg,"504|%s|%s|||||", g_pCfg->GetCustID(), g_pCfg->GetAccount());

	if (TRUE != SendMidMsg(szMsg))
	{
		return FALSE;
	}

	try
	{
		CString strSql;
		strSql.Format("select * from shares where account = %s", g_pCfg->GetAccount());

		BSTR bstrSQL = strSql.AllocSysString();
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		_variant_t TheValue; //VARIANT��������
		char szTmp[100] = {0};

		while(!pRecordSet->adoEOF)
		{	
			TheValue = pRecordSet->GetCollect("SECU_INTL");

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
						
						//TODO: ��ʱ���Ƚ���ֵ����Ϊmid����ֵ�����ݿⲻ̫һ��
						// У�� MKT_VAL ��ֵ
						//DB_COMPARE("MKT_VAL", szMktVal);

						break;
					}
				}
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

		bRet = FALSE;
	}

	return bRet;
}
