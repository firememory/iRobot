#pragma once

#include "kdmidcli.h"

typedef  void (__cdecl * LPFNDLLFUNC1)(long, const char * , char *);
typedef  void (__cdecl * LPFNDLLFUNC2)(long, const char * , char *);

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
