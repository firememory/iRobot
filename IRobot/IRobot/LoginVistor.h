#pragma once
#include "ipckdgatewayvistor.h"

class CLoginVistor :
	public IPCKDGateWayVistor
{
public:
	CLoginVistor(void);
	~CLoginVistor(void);

	virtual BOOL Vistor();
	BOOL Vistor(char *);

	virtual BOOL ResultStrToTable(char *);
	virtual BOOL SendMsg(char *);

	void GetSession();

private:
	MID_409101_LOGIN_RET_MSG *m_pLoginMsg;	
};
