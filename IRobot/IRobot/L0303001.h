#pragma once

#define LBPARANAME_LEN 51
#define LBPARAVAL_LEN 201

#include "KcxpConn.h"

struct L0303001_SEND_MSG
{
	char szOPER_FLAG[LBPARAVAL_LEN];
	char szCUSTOMER[LBPARAVAL_LEN];
	char szMARKET[LBPARAVAL_LEN];
	char szBOARD[LBPARAVAL_LEN];
	char szSECU_ACC[LBPARAVAL_LEN];
	char szACCOUNT[LBPARAVAL_LEN];
	char szSECU_INTL[LBPARAVAL_LEN];
	char szSEAT[LBPARAVAL_LEN];
	char szSERIAL_NO[LBPARAVAL_LEN];
	char szDELIST_CHANNEL[LBPARAVAL_LEN];
	char szQTY[LBPARAVAL_LEN];
	char szTRD_ID[LBPARAVAL_LEN];
	char szPRICE[LBPARAVAL_LEN];
};

struct L0303001_RECV_MSG
{
	char szBizNo[LBPARAVAL_LEN];
	char szOrderID[LBPARAVAL_LEN];
	char szAccount[LBPARAVAL_LEN];
	char szPrice[LBPARAVAL_LEN]; // 委托价格
	char szQty[LBPARAVAL_LEN];		// 委托数量
	char szOrderAmt[LBPARAVAL_LEN];	// 委托金额
	char szOrderFrzAmt[LBPARAVAL_LEN];	// 交易冻结金额
	char szSeat[LBPARAVAL_LEN];
	char szExtInst[LBPARAVAL_LEN];
	char szExtAcc[LBPARAVAL_LEN];
	char szExtSubAcc[LBPARAVAL_LEN];
	char szExtFrzAmt[LBPARAVAL_LEN];
};


class CL0303001
{
public:
	CL0303001(void);
	~CL0303001(void);

	// 执行LBM
	BOOL ExecuteLbm(L0303001_SEND_MSG *pSendMsg);
	
	// 获取LBM执行结果
	BOOL FetchRslt(int ,L0303001_RECV_MSG *pRecvMsg);

	// 获取LBM执行结果集的行数
	int GetRsltRowNum();


private:
	CKDMidCli *m_pKcxpConn;
};