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

#define DB_COMPARE(x, y)\
{\
	TheValue = g_pDBConn->m_pRecordset->GetCollect(x);\
	if (TheValue.vt!=VT_NULL)\
	{\
		strncpy_s(szTmp, (char*)_bstr_t(TheValue), 100);\
		float fDst = (float)atof(szTmp);\
		float fSrc = (float)atof(m_pMsg[i].y);\
		if (fSrc != fDst)\
		{\
			g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "%s ��ѯ����ȷ! mid:%f, db:%f", x, fSrc, fDst);\
			bRet = FALSE;\
		}\
	}\
	else\
	{\
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "%s DB�޷�����!", x);\
		bRet = FALSE;\
	}\
}

#define DB_GET_VALUE_STRING(x, y)\
{\
	TheValue = g_pDBConn->m_pRecordset->GetCollect(x);\
	if(TheValue.vt!=VT_NULL)\
	{\
		strcpy_s(y, (char*)_bstr_t(TheValue));\
	}\
}

#define DB_GET_VALUE_FLOAT(x, y)\
{\
	TheValue = g_pDBConn->m_pRecordset->GetCollect(x);\
	if(TheValue.vt!=VT_NULL)\
	{\
		y = (float)atof((char*)_bstr_t(TheValue));\
	}\
}

#define DB_GET_VALUE_INT(x, y)\
{\
	TheValue = g_pDBConn->m_pRecordset->GetCollect(x);\
	if(TheValue.vt!=VT_NULL)\
	{\
		y = atoi((char*)_bstr_t(TheValue));\
	}\
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

class IPCKDGateWayVistor
{
public:
	IPCKDGateWayVistor();
	~IPCKDGateWayVistor(){};

	virtual BOOL Vistor() = 0;
	virtual BOOL ResultStrToTable(char *) = 0;	
	virtual void BeginTest(){g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "%s", m_szServiceName);};
protected:
	char m_szServiceName[MAX_PATH]; // Service����	

	CKDGateway *m_pKDGateWay; // MID����ָ��
	CKDMidCli *m_pKcxpConn; // KXCP����ָ��
};
