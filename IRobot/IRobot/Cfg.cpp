#include "StdAfx.h"
#include "Cfg.h"

CCfg::CCfg(void)
{
	m_strCfgPath = "./irobot.ini";
}

CCfg::~CCfg(void)
{
}

BOOL CCfg::ReadCfg()
{
	char *pCfgPath = m_strCfgPath.GetBuffer();
	char szTmp[512];

	// ȡKCXP IP
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "KCXP", "IP", "10.137.31.54", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strKcxpIp = szTmp;

	// ȡKCXP PORT
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "KCXP", "PORT", "10.137.31.54", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strKcxpPort = szTmp;

	// ȡKCXP SendQ
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "KCXP", "SEND", "reqkxctest8", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strKcxpSendQ = szTmp;

	// ȡKCXP RecvQ
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "KCXP", "RECV", "anskxctest8", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strKcxpRecvQ = szTmp;

	// ȡMID IP	
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "MID", "IP", "10.137.31.187", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strMidIp = szTmp;

	// ȡKCXP PORT
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "MID", "PORT", "21000", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strMidPort = szTmp;

	// ȡDB ConnStr
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "DB", "CONNSTR", "JZJYKF", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strDBConnStr = szTmp;

	// ȡDB USER
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "DB", "USER", "xctest", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strDBUser = szTmp;

	// ȡDB PWD
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "DB", "PASSWORD", "xctest", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strDBPwd = szTmp;
	
	// ȡAgent CUSTID
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "AGENT", "CUSTID", "18798721", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strCustID = szTmp;

	// ȡAgent ACCOUNT
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "AGENT", "ACCOUNT", "85807073", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strAccount = szTmp;

	// ȡAgent CUSTPWD
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "AGENT", "CUSTPWD", "753951", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strCustPwd = szTmp;

	// ȡAgent OPID
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "AGENT", "OPID", "11726", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strOpId = szTmp;

	// ȡAgent OPPWD
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "AGENT", "OPPWD", "753951", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strOpPwd= szTmp;

	// ȡAgent BRANCH
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "AGENT", "BRANCH", "8070", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strBranch = szTmp;

	// ȡLog Path
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "COMMON", "LOGPATH", "d:/irobot.ini", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strLogPath = szTmp;

	// Log Level
	m_nLogLevel = GetPrivateProfileInt( "COMMON", "LOGLEVEL", 0, pCfgPath);

	// ȡTest Mode	
	m_nTestMode = GetPrivateProfileInt( "COMMON", "MODE", 1, pCfgPath);

	m_nRefreshDBGap = GetPrivateProfileInt( "COMMON", "DBREFRESHTIME", 1, pCfgPath);

	return TRUE;
}

BOOL CCfg::SetCfg()
{
	char *pCfgPath = m_strCfgPath.GetBuffer();

	WritePrivateProfileString("KCXP", "IP", m_strKcxpIp.GetBuffer(), pCfgPath);
	WritePrivateProfileString("KCXP", "PORT", m_strKcxpPort.GetBuffer(), pCfgPath);
	WritePrivateProfileString("KCXP", "SEND", m_strKcxpSendQ.GetBuffer(), pCfgPath);
	WritePrivateProfileString("KCXP", "RECV", m_strKcxpRecvQ.GetBuffer(), pCfgPath);

	WritePrivateProfileString("MID", "IP", m_strMidIp.GetBuffer(), pCfgPath);
	WritePrivateProfileString("MID", "PORT", m_strMidPort.GetBuffer(), pCfgPath);

	WritePrivateProfileString("DB", "CONNSTR", m_strDBConnStr.GetBuffer(), pCfgPath);
	WritePrivateProfileString("DB", "USER", m_strDBUser.GetBuffer(), pCfgPath);
	WritePrivateProfileString("DB", "PASSWORD", m_strDBPwd.GetBuffer(), pCfgPath);

	WritePrivateProfileString("AGENT", "CUSTID", m_strCustID.GetBuffer(), pCfgPath);
	WritePrivateProfileString("AGENT", "CUSTPWD", m_strCustPwd.GetBuffer(), pCfgPath);
	WritePrivateProfileString("AGENT", "ACCOUNT", m_strAccount.GetBuffer(), pCfgPath);
	WritePrivateProfileString("AGENT", "OPID", m_strOpId.GetBuffer(), pCfgPath);
	WritePrivateProfileString("AGENT", "OPPWD", m_strOpPwd.GetBuffer(), pCfgPath);
	WritePrivateProfileString("AGENT", "BRANCH", m_strBranch.GetBuffer(), pCfgPath);
	
	WritePrivateProfileString("COMMON", "LOGPATH", m_strLogPath.GetBuffer(), pCfgPath);

	CString strLogLevel;
	strLogLevel.Format("%d", m_nLogLevel);
	WritePrivateProfileString("COMMON", "LOGLEVEL", strLogLevel.GetBuffer(), pCfgPath);

	CString strMode;
	strMode.Format("%d", m_nTestMode);
	WritePrivateProfileString("COMMON", "MODE", strMode.GetBuffer(), pCfgPath);

	CString strRefreshDBGap;
	strRefreshDBGap.Format("%d", m_nRefreshDBGap);

	WritePrivateProfileString("COMMON", "DBREFRESHTIME", strRefreshDBGap.GetBuffer(), pCfgPath);
	return TRUE;
}
