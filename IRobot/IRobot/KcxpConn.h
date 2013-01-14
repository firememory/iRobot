#pragma once

#include "kdmidcli.h"

class CKcxpConn
{
public:
	CKcxpConn(void);
	~CKcxpConn(void);

	BOOL InitKcxp();

private:
	tagKCBPConnectOption stKCBPConnection; //KCBP ����ѡ��
	CKDMidCli m_clKdMidCli;

};
