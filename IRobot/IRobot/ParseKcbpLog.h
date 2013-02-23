#pragma once

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
	BOOL ExecMultiCmds();
	BOOL ExecSingleCmd();

	BOOL ResetUserPwd(char *);

	void SetKcbpLogPath(char *val) { strcpy_s(m_szKcbpLogPath, val);}

	void Clean();

private:
	char m_szKcbpLogPath[MAX_PATH];
	
	CArray <LBM_PARAM_INFO*, LBM_PARAM_INFO*> m_arrCmds;
};
