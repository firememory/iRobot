#pragma once

#include "IPCKDGateWayVistor.h"

struct MID_QUERY_SHARES_504
{
	char szUserCode[10];
	char szAccount[21];
	char szMarket[3];
	char szSecuAcc[11];
	char szSecuName[17];
	char szSecuCode[10];
	char szSecuIntl[10];
	char szSeat[10];
	char szBranch[10];
	char szExtInst[10];
	char szCurrency[2];
	char szShareBln[20];
	char szShareAvl[20];
	char szShareTrdFrz[20];
	char szShareOtd[20];
	char szShareFrz[20];
	char szShareUnTrdQty[20];
	char szCurrentCost[20];
	char szMktValOld[20];
	char szCostPrice[20];
	char szShareOtdAvl[20];
	char szCurrPrice[20];
	char szCost2Price[20];
	char szMktQty[20];
	char szMktVal[20];
};

class CQueryShares
	: public IPCKDGateWayVistor
{
public:
	CQueryShares(void);
	virtual ~CQueryShares(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual BOOL SendMsg(char *);

	BOOL ChkPnt1();

private:
	MID_QUERY_SHARES_504 *m_pMsg;

	int m_nRecNum;
};
