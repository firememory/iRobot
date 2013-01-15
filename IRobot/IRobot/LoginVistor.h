#pragma once
#include "ipckdgatewayvistor.h"

struct MID_LOGIN_409101
{
	char szUserCode[9];
	char szMarket[3];
	char szBoard[2];
	char szSecuAcc[15];
	char szSecuAccName[20];
	char szAccount[10];
	char szUserName[20];
	char szBrh[10];
	char szMainFlag[2];
	char szSession[20];
	char szTaCode[5];
	char szCertDays[3];
	char szEtokenDays[3];
};

class CLoginVistor :
	public IPCKDGateWayVistor
{
public:
	CLoginVistor(void);
	~CLoginVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);

	void GetSession();

private:
	MID_LOGIN_409101 *m_pLogin_Msg;	
};
