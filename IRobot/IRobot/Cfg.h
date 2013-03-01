#pragma once

enum TEST_MODE
{
	USE_MID = 0,
	USE_KCXP
};

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

	CString GetCustID() const { return m_strCustID; }
	void SetCustID(CString val) { m_strCustID = val; }

	CString GetAccount() const { return m_strAccount; }
	void SetAccount(CString val) { m_strAccount = val; }

	CString GetCustPwd() const { return m_strCustPwd; }
	void SetCustPwd(CString val) { m_strCustPwd = val; }

	CString GetOpId() const { return m_strOpId; }
	void SetOpId(CString val) { m_strOpId = val; }

	CString GetOpPwd() const { return m_strOpPwd; }
	void SetOpPwd(CString val) { m_strOpPwd = val; }

	CString GetBranch() const { return m_strBranch; }
	void SetBranch(CString val) { m_strBranch = val; }

	char* GetSecu_Acc_SZA() { return &m_szSecu_Acc_SZA[0]; }
	void SetSecu_Acc_SZA(char *val) { strcpy_s(m_szSecu_Acc_SZA, val);}

	char* GetSecu_Acc_SHA() { return &m_szSecu_Acc_SHA[0]; }
	void SetSecu_Acc_SHA(char *val) { strcpy_s(m_szSecu_Acc_SHA, val); }

	char* GetSecu_Acc_SZB() { return &m_szSecu_Acc_SZB[0]; }
	void SetSecu_Acc_SZB(char *val) { strcpy_s(m_szSecu_Acc_SZB, val); }

	char* GetSecu_Acc_SHB() { return &m_szSecu_Acc_SHB[0]; }
	void SetSecu_Acc_SHB(char *val) { strcpy_s(m_szSecu_Acc_SHB, val); }

	int GetRefreshDBGap() const { return m_nRefreshDBGap; }
	void SetRefreshDBGap(int val) { m_nRefreshDBGap = val; }

	int GetLogLevel() const { return m_nLogLevel; }
	void SetLogLevel(int val) { m_nLogLevel = val; }


	char* GetSZA_BIND_SEAT() { return &m_szSZA_BIND_SEAT[0]; }
	void SetSZA_BIND_SEAT(char *val) { strcpy_s(m_szSZA_BIND_SEAT ,val); }

	char* GetSHA_BIND_SEAT() { return &m_szSHA_BIND_SEAT[0]; }
	void SetSHA_BIND_SEAT(char *val) { strcpy_s(m_szSHA_BIND_SEAT ,val); }

	char* GetSZB_BIND_SEAT() { return &m_szSZB_BIND_SEAT[0]; }
	void SetSZB_BIND_SEAT(char* val) { strcpy_s(m_szSZB_BIND_SEAT ,val); }

	char* GetSHB_BIND_SEAT() { return &m_szSHB_BIND_SEAT[0]; }
	void SetSHB_BIND_SEAT(char* val) { strcpy_s(m_szSHB_BIND_SEAT ,val); }

	char* GetSB_BIND_SEAT() { return &m_szSB_BIND_SEAT[0]; }
	void SetSB_BIND_SEAT(char* val) { strcpy_s(m_szSB_BIND_SEAT ,val); }

private:
	CString m_strCfgPath;

	CString m_strKcxpIp;
	CString m_strKcxpPort;	
	CString m_strKcxpSendQ;
	CString m_strKcxpRecvQ;
	
	CString m_strMidIp;
	CString m_strMidPort;

	CString m_strLogPath;
	int m_nLogLevel;
	
	int m_nTestMode;
	int m_nRefreshDBGap; // 等待数据库数据刷新时长(毫秒)
	
	CString m_strDBConnStr;
	CString m_strDBUser;
	CString m_strDBPwd;

	CString m_strCustID; // 客户代码	
	CString m_strAccount; // 资金账户	
	CString m_strCustPwd; // 客户密码	
	CString m_strOpId; // 操作员号
	CString m_strOpPwd; // 操作员密码	
	CString m_strBranch; // 营业部

	char m_szSecu_Acc_SZA[15]; // 深圳A股 股东代码
	char m_szSecu_Acc_SHA[15]; // 上海A股 股东代码
	char m_szSecu_Acc_SZB[15]; // 深圳B股 股东代码
	char m_szSecu_Acc_SHB[15]; // 上海B股 股东代码

	char m_szSZA_BIND_SEAT[10]; // 深A席位
	char m_szSHA_BIND_SEAT[10]; // 沪A席位
	char m_szSZB_BIND_SEAT[10]; // 深B席位
	char m_szSHB_BIND_SEAT[10]; // 沪B席位
	char m_szSB_BIND_SEAT[10]; // 三板席位
};
