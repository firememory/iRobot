#pragma once

#include "kdmidcli.h"

class CKcxpConn
{
public:
	CKcxpConn(void);
	~CKcxpConn(void);

	BOOL InitKcxp();
	BOOL OpLogin();

private:
	tagKCBPConnectOption stKCBPConnection; //KCBP ����ѡ��
	CKDMidCli m_clKdMidCli;

};
