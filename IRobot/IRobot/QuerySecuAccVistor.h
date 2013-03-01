#pragma once

#include "IPCKDGateWayVistor.h"

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

	// ��KCXP������Ϣ
	BOOL SendKcxpMsg(char *);

	// ��Mid������Ϣ
	BOOL SendMidMsg(char *);

	BOOL TestCase_1();

	BOOL ChkData(MID_501_QUERY_SECU_ACC_RET_MSG *pMsg);

private:
	MID_501_QUERY_SECU_ACC_RET_MSG *m_pMsg;
	int m_nRowNum;
};