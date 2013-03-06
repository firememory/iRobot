#pragma once

#include "IPCKDGateWayVistor.h"

class CQueryShares
	: public IPCKDGateWayVistor
{
public:
	CQueryShares(void);
	virtual ~CQueryShares(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual void ParseKcxpRetMsg(){};

	BOOL ChkPnt1();

private:
	MID_504_QUERY_SHARES_RET_MSG *m_pMsg;

	int m_nRecNum;
};
