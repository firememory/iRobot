#pragma once

class CCfg
{
public:
	CCfg(void);
	~CCfg(void);

	BOOL ReadCfg();
	BOOL SetCfg();

	CString GetKcxpIp() const { return m_strKcxpIp; }
	void SetKcxpIp(CString val) { m_strKcxpIp = val; }

	CString GetKcxpPort() const { return m_strKcxpPort; }
	void SetKcxpPort(CString val) { m_strKcxpPort = val; }

	CString GetKcxpSendQ() const { return m_strKcxpSendQ; }
	void SetKcxpSendQ(CString val) { m_strKcxpSendQ = val; }

	CString GetKcxpRecvQ() const { return m_strKcxpRecvQ; }
	void SetKcxpRecvQ(CString val) { m_strKcxpRecvQ = val; }

	CString GetMidIp() const { return m_strMidIp; }
	void SetMidIp(CString val) { m_strMidIp = val; }

	CString GetMidPort() const { return m_strMidPort; }
	void SetMidPort(CString val) { m_strMidPort = val; }

	CString GetLogPath() const { return m_strLogPath; }
	void SetLogPath(CString val) { m_strLogPath = val; }

	int GetTestMode() const { return m_nTestMode; }
	void SetTestMode(int val) { m_nTestMode = val; }

	CString GetDBConnStr() const { return m_strDBConnStr; }
	void SetDBConnStr(CString val) { m_strDBConnStr = val; }

	CString GetDBUser() const { return m_strDBUser; }
	void SetDBUser(CString val) { m_strDBUser = val; }

	CString GetDBPwd() const { return m_strDBPwd; }
	void SetDBPwd(CString val) { m_strDBPwd = val; }

private:
	CString m_strCfgPath;

	CString m_strKcxpIp;
	CString m_strKcxpPort;	
	CString m_strKcxpSendQ;
	CString m_strKcxpRecvQ;
	
	CString m_strMidIp;
	CString m_strMidPort;

	CString m_strLogPath;
	int m_nTestMode;

	CString m_strDBConnStr;
	CString m_strDBUser;
	
	CString m_strDBPwd;
	
};
