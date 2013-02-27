#pragma once

#include "IPCKDGateWayVistor.h"

#define DEFAULT_LEN 30

/************************************************************************/
/* �ͻ��ɶ���ѯ                                                        */
/************************************************************************/
class CQuerySecuAccVistor
	: public IPCKDGateWayVistor
{
public:
	CQuerySecuAccVistor(void);
	virtual ~CQuerySecuAccVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual BOOL SendMsg(char *);

	// ��KCXP������Ϣ
	BOOL SendKcxpMsg();

	// ��Mid������Ϣ
	BOOL SendMidMsg();

	BOOL ChkPnt1();

	BOOL ChkData(MID_501_QUERY_SECU_ACC_RET_MSG *pMsg);

private:
	MID_501_QUERY_SECU_ACC_RET_MSG *m_pMsg;
	int m_nRowNum;
};
