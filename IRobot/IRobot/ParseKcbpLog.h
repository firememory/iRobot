#pragma once

#include "KcxpConn.h"

struct PARAM_UNIT
{
	char szName[50];
	char szValue[MAX_PATH];
};

struct LBM_PARAM_INFO
{
	char szLbmId[20];
	CMapStringToString mapParams;
};

class CParseKcbpLog
{
public:
	CParseKcbpLog(void);
	~CParseKcbpLog(void);

	void ParseCmd(char *pCmd);
	BOOL ReadLog();
	void ReadRlt();
	void ExecMultiCmds_1();
	void ExecMultiCmds_2();
	void ExecMultiCmds_3();
	BOOL ExecSingleCmd();

	BOOL ResetUserPwd(char *);

	void SetKcbpLogPath(char *val) { strcpy_s(m_szKcbpLogPath, val);}

	void Clean();

	// 给操作员授权
	void OpGrant(char *pCust);

	// 根据客户的资金帐号，查找对应的客户号
	void GetCustCodeViaAccount(char *pAccount, char* pCustCode);

	void SetKcxpConn(CKDMidCli * val) { m_pKcxpConn = val; }

private:
	char m_szKcbpLogPath[MAX_PATH];
	
	CArray <LBM_PARAM_INFO*, LBM_PARAM_INFO*> m_arrCmds;

	CKDMidCli *m_pKcxpConn;
};
