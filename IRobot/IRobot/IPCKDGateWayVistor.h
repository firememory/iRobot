#pragma once

#include "MsgDef.h"
#include "loginterface.h"
#include "KcxpConn.h"
#include "MidConn.h"

extern CLoginterface *g_pLog;

#define SERVICE_STRNCPY(member)\
{\
	strcpy_s(m_pMsg[nRow].member, q);\
}

#define SERVICE_KCXP_STRNCPY(key, member)\
{\
	m_pKcxpConn->RsGetColByName(key, szTemp);\
	strcpy_s(m_pMsg[nRow].member, szTemp);\
}

#define ExecTestCase(x,y)\
{\
	g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "#TestCase# %s %s��ʼ", m_szServiceName, y);\
	if (FALSE == x())\
	{\
		g_pLog->WriteRunLog(TEST_CASE_MODE_FAIL, LOG_NOTIFY, "#TestCase# %s %sʧ��", m_szServiceName, y);\
		bRet = FALSE;\
	}\
	else\
	{\
		g_pLog->WriteRunLog(TEST_CASE_MODE_SUCC, LOG_NOTIFY, "#TestCase# %s %s�ɹ�", m_szServiceName, y);\
	}\
}

#define MAX_WAIT_MATCH_CNT 6

class IPCKDGateWayVistor
{
public:
	IPCKDGateWayVistor();
	~IPCKDGateWayVistor();

	virtual BOOL Vistor() = 0;
	virtual BOOL ResultStrToTable(char *) = 0;
	virtual void ParseKcxpRetMsg() = 0;

	virtual void BeginTest(){g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "%s", m_szServiceName);};
	virtual LPSTR GetNextLine(LPSTR lpstr);
	virtual BOOL SendMidMsg(char *pCmd);
	virtual BOOL SendKcxpMsg(char *pCmd);
	virtual BOOL GetSecuCurPrice(char *pSecu_intl, char *pPrice);
	//virtual BOOL GetSecuInfo(char *pSecuIntl, char* pAccount);
protected:
	char m_szServiceName[MAX_PATH]; // Service����	

	CKDGateway *m_pKDGateWay; // MID����ָ��
	CKDMidCli *m_pKcxpConn; // KXCP����ָ��
	int m_nWaitMatchingCnt; // �ȴ��ɽ�����

	// MID����ֵ
	int m_nRowNum;
	int m_nFieldNum;

	char m_szSecu_Intl[DEFAULT_LEN];		// ֤ȯ����
	char m_szSecu_Code[DEFAULT_LEN];		// ֤ȯ����

	char m_szFund_Intl[DEFAULT_LEN];		// �������
	char m_szOrderAmt[DEFAULT_LEN];			// ί�н��

	char m_szPrice[DEFAULT_LEN];			// ί�м۸�
	char m_szQty[DEFAULT_LEN];				// ί������

	char m_szMarket_Board[3];
	char m_szTrdId[3];
	char m_szCurrency[2]; // ����
	char m_szTA_Code[3];	

private:
	static CRITICAL_SECTION m_caSendMsgLock; // ������Ϣ����
};
