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
	g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "#TestCase# %s %s开始", m_szServiceName, y);\
	if (FALSE == x())\
	{\
		g_pLog->WriteRunLog(TEST_CASE_MODE_FAIL, LOG_NOTIFY, "#TestCase# %s %s失败", m_szServiceName, y);\
		bRet = FALSE;\
	}\
	else\
	{\
		g_pLog->WriteRunLog(TEST_CASE_MODE_SUCC, LOG_NOTIFY, "#TestCase# %s %s成功", m_szServiceName, y);\
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
protected:
	char m_szServiceName[MAX_PATH]; // Service名称	

	CKDGateway *m_pKDGateWay; // MID连接指针
	CKDMidCli *m_pKcxpConn; // KXCP连接指针
	int m_nWaitMatchingCnt; // 等待成交次数

	// MID返回值
	int m_nRowNum;
	int m_nFieldNum;

private:
	static CRITICAL_SECTION m_caSendMsgLock; // 发送消息的锁
};
