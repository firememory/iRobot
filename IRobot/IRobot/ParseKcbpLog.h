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

	void ParseLog();
	void ReadRlt();
	void Exec();

	BOOL ResetUserPwd(char *);

private:
	char m_szKcbpLogPath[MAX_PATH];
	CArray <LBM_PARAM_INFO*, LBM_PARAM_INFO*> m_arrCmds;
};
