#include "StdAfx.h"
#include "QuerySecuAccVistor.h"
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

CQuerySecuAccVistor::CQuerySecuAccVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;
	strcpy_s(m_szTestCaseName, "�ͻ��ɶ���ѯ");
}

CQuerySecuAccVistor::~CQuerySecuAccVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CQuerySecuAccVistor::Vistor()
{
	BOOL bRet = TRUE;	

	bRet = ChkPnt1();
	
	return bRet;
}

BOOL CQuerySecuAccVistor::ResultStrToTable(char *pRetStr)
{
	m_nRowNum = m_pKDGateWay->GetRecNum();
	
	m_pMsg = new MID_501_QUERY_SECU_ACC_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_501_QUERY_SECU_ACC_RET_MSG)*m_nRowNum);

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
			while(tmp)
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
						SERVICE_STRNCPY(szMarket);
						break;
					case 2:						
						SERVICE_STRNCPY(szSecuAcc);
						break;
					case 3:						
						SERVICE_STRNCPY(szSecuAccName);
						break;
					case 4:						
						SERVICE_STRNCPY(szAccount);
						break;
					case 5:						
						SERVICE_STRNCPY(szMainFlag);
						break;
					case 6:						
						SERVICE_STRNCPY(szBindSeat);
						break;
					case 7:						
						SERVICE_STRNCPY(szBindStatus);
						break;
					case 8:						
						SERVICE_STRNCPY(szStatus);
						break;									
					}

					q = tmp +1;					
					break;
				}
				tmp++;
			}
		}		
	}

	return TRUE;
}


BOOL CQuerySecuAccVistor::ChkPnt1()
{
	/*
		���������Ϣ
		1.1.����̨���ص�MARKET,SECU_ACC,SEAT�Ƿ������ݿ�һ��
	*/
	BOOL bRet = TRUE;

	// ��������
	if (TRUE != SendMsg(NULL))
	{
		return FALSE;
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	// 1.1.����̨���ص�MARKET,SECU_ACC,SEAT�Ƿ������ݿ�һ��
	for (int i=0;i<m_nRowNum;i++)
	{
		if (ChkData(&m_pMsg[i]) == FALSE)
		{
			return FALSE;
		}
		
	}

	return bRet;
}

BOOL CQuerySecuAccVistor::SendMsg(char *pMsg)
{
	BOOL bRet = TRUE;

	if (g_pCfg->GetTestMode() == USE_MID)
	{
		bRet = SendMidMsg();
	}
	else
	{
		bRet = SendKcxpMsg();
		
		// �����־������������һ�β���
		g_pParseKcbpLog->Clean();
	}

	return bRet;
}

BOOL CQuerySecuAccVistor::SendKcxpMsg()
{
	int iRetCode = KCBP_MSG_OK;

	if (NULL == m_pKcxpConn)
	{
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "��ȡKCXP����ʧ��!");
		return FALSE;
	}

	// ƴ�ӷ��͸�KCXP�������ַ���
	char szTemp[2048] = {0};		
	sprintf_s(szTemp,"<  29294788> BEGIN:L0301002:27-10:39:01-384953  [_CA=2.3&_ENDIAN=0&F_OP_ROLE=2&F_OP_USER=%s&F_OP_BRANCH=%s&F_SESSION=%s&F_CHANNEL=0&F_OP_SITE=99999&CUSTOMER=%s]",
		g_pCfg->GetOpId().GetBuffer(), g_pCfg->GetBranch().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetCustID().GetBuffer());

	// ������Ϣ
	try
	{
		// ��������
		g_pParseKcbpLog->ParseCmd(&szTemp[0]);

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

					m_pMsg = new MID_501_QUERY_SECU_ACC_RET_MSG[m_nRowNum];
					memset(m_pMsg, 0x00, sizeof(MID_501_QUERY_SECU_ACC_RET_MSG)*m_nRowNum);
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

						SERVICE_KCXP_STRNCPY("USER_CODE", szUserCode);
						SERVICE_KCXP_STRNCPY("MARKET", szMarket);
						SERVICE_KCXP_STRNCPY("SECU_ACC", szSecuAcc);
						SERVICE_KCXP_STRNCPY("SECU_ACC_NAME", szSecuAccName);
						SERVICE_KCXP_STRNCPY("DFT_ACC", szAccount);
						SERVICE_KCXP_STRNCPY("MAIN_FLAG", szMainFlag);
						SERVICE_KCXP_STRNCPY("BIND_SEAT", szBindSeat);
						SERVICE_KCXP_STRNCPY("BIND_STATUS", szBindStatus);
						SERVICE_KCXP_STRNCPY("STATUS", szStatus);
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
		g_pLog->WriteRunLog(KCXP_MODE, LOG_DEBUG, "LBM[L0301002]�����쳣��");
		return FALSE;
	}	

	return TRUE;
}

BOOL CQuerySecuAccVistor::SendMidMsg()
{
	char szTemp[512];		
	sprintf_s(szTemp,"501|%s|||", g_pCfg->GetCustID());

	if (m_pKDGateWay->WaitAnswer(&szTemp[0])!=TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[501] ί�нӿ�, ����ʧ��!");
		return FALSE;
	}

	// �Թ�̨���ص�ֵ���н���
	if (ResultStrToTable(m_pKDGateWay->m_pReturnData) != TRUE)
	{
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "[501] ί�нӿ�, ����ʧ��!");
		return FALSE;
	}

	return TRUE;
}

BOOL CQuerySecuAccVistor::ChkData(MID_501_QUERY_SECU_ACC_RET_MSG *pMsg)
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
	CString strSql;
	strSql.Format("select * from  secu_acc where secu_acc = '%s' and bind_seat = %s and dft_acc = %s",
		pMsg->szSecuAcc, pMsg->szBindSeat, g_pCfg->GetAccount().GetBuffer());

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		g_pDBConn->m_pRecordset->Open(bstrSQL, (IDispatch*)g_pDBConn->m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 
		
		if (g_pDBConn->m_pRecordset->adoEOF)
		{
			// �����ѯ�������������ʧ��
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.1 Fail!");
			return FALSE;
		}
		else
		{
			// ���ÿͻ��󶨵�ϯλ
			if (strcmp(pMsg->szMarket, "00") == 0)
			{
				// ��A
				g_pCfg->SetSZA_BIND_SEAT(pMsg->szBindSeat);
			}
			else if (strcmp(pMsg->szMarket, "10") == 0)
			{
				// ��A
				g_pCfg->SetSHA_BIND_SEAT(pMsg->szBindSeat);
			}
			else if (strcmp(pMsg->szMarket, "01") == 0)
			{
				// ��B
				g_pCfg->SetSZB_BIND_SEAT(pMsg->szBindSeat);
			}
			else if (strcmp(pMsg->szMarket, "11") == 0)
			{
				// ��B
				g_pCfg->SetSHB_BIND_SEAT(pMsg->szBindSeat);
			}
			else if (strcmp(pMsg->szMarket, "02") == 0)
			{
				// ����
				g_pCfg->SetSZB_BIND_SEAT(pMsg->szBindSeat);
			}

			g_pDBConn->m_pRecordset->Close();
			return TRUE;
		}
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
