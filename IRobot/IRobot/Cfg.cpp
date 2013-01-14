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

	// ȡLog Path
	memset(szTmp, 0x00, sizeof(szTmp));
	GetPrivateProfileString( "COMMON", "LOGPATH", "d:/irobot.ini", szTmp, sizeof(szTmp) - 1, pCfgPath);
	m_strLogPath = szTmp;

	// ȡTest Mode	
	m_nTestMode = GetPrivateProfileInt( "COMMON", "MODE", 1, pCfgPath);

	return TRUE;
}

BOOL CCfg::SetCfg()
{
	char *pCfgPath = m_strCfgPath.GetBuffer();

	WritePrivateProfileString("KCXP", "IP", m_strKcxpIp.GetBuffer(), pCfgPath);
	WritePrivateProfileString("KCXP", "SEND", m_strKcxpSendQ.GetBuffer(), pCfgPath);
	WritePrivateProfileString("KCXP", "RECV", m_strKcxpRecvQ.GetBuffer(), pCfgPath);

	WritePrivateProfileString("MID", "IP", m_strMidIp.GetBuffer(), pCfgPath);
	WritePrivateProfileString("MID", "PORT", m_strMidPort.GetBuffer(), pCfgPath);
	
	WritePrivateProfileString("COMMON", "LOGPATH", m_strLogPath.GetBuffer(), pCfgPath);

	CString strMode;
	strMode.Format("%d", m_nTestMode);

	WritePrivateProfileString("COMMON", "MODE", strMode.GetBuffer(), pCfgPath);
	return TRUE;
}
