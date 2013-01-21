#pragma once

struct PARAM_UNIT
{
	char szName[50];
	char szValue[50];
};

struct LBM_PARAM_INFO
{
	char szLbmId[20];
	CArray <PARAM_UNIT*, PARAM_UNIT*> arrParams;
};

class CParseKcbpLog
{
public:
	CParseKcbpLog(void);
	~CParseKcbpLog(void);

	void ParseLog();
	void ReadRlt();

private:
	char m_szKcbpLogPath[MAX_PATH];
	CArray <LBM_PARAM_INFO*, LBM_PARAM_INFO*> m_arrCmds;
};
