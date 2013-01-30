#pragma once

#include "kdmidcli.h"

class CKcxpConn
{
public:
	CKcxpConn(void);
	~CKcxpConn(void);

	BOOL InitKcxp();
	BOOL OpLogin();


	CKDMidCli *GetKdMidCli() const { return m_pKdMidCli; }
	void SetKdMidCli(CKDMidCli * val) { m_pKdMidCli = val; }

	char* GetSession() { return &m_szSession[0]; }
	void SetSession(char *val) {strcpy_s(m_szSession, val); }

private:
	tagKCBPConnectOption stKCBPConnection; //KCBP 连接选项
	CKDMidCli m_clKdMidCli;
	CKDMidCli *m_pKdMidCli;

	char m_szSession[50];
};
