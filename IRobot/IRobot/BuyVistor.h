#pragma once

#include "IPCKDGateWayVistor.h"

struct MID_ORDER_403
{
	char szBizNo[11];
	char szOrderID[11];
	char szAccount[21];
	char szPrice[21];
	char szQty[21];
	char szOrderAmt[21];
	char szOrderFrzAmt[21];
	char szSeat[7];
	char szExtInst[11];
	char szExtAcc[33];
	char szExtSubAcc[33];
	char szExtFrzAmt[21];
};

/************************************************************************/
/* 深A 限价买入                                                         */
/************************************************************************/
class CBuyVistor
	: public IPCKDGateWayVistor
{
public:
	CBuyVistor(void);
	virtual ~CBuyVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);

	virtual BOOL SendMsg(char *);

	void ChkPnt1();
	void ChkPnt2();

private:
	MID_ORDER_403 *m_pMsg;	
};
