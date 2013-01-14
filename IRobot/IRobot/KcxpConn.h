#pragma once

#include "kdmidcli.h"

class CKcxpConn
{
public:
	CKcxpConn(void);
	~CKcxpConn(void);

	BOOL InitKcxp();

private:
	tagKCBPConnectOption stKCBPConnection; //KCBP 连接选项
	CKDMidCli m_clKdMidCli;

};
