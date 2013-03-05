#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* 基金业务                                                             */
/************************************************************************/
class CFundVistor
	: public IPCKDGateWayVistor
{
public:
	CFundVistor(void);
	virtual ~CFundVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);	

	// 在执行前，获取一把相关数据，与执行后数据进行比较
	BOOL InitUserData();
	BOOL GetOrderData();

	// 向KCXP发送消息
	BOOL SendKcxpMsg(char *);

	BOOL TestCase_1(); 

	BOOL ChkData();

	BOOL SaveCapital();

	BOOL SaveShares();

private:
	MID_821_ORDER_RET_MSG *m_pMsg;

	char m_szFund_Intl[DEFAULT_LEN];		// 基金代码
	char m_szOrderAmt[DEFAULT_LEN];			// 委托金额
	char m_szTrdId[3];
	char m_szTA_Code[3];
	char m_szCurrency[3];

	// 检测数据
	float m_fOrderAmt;		// 交易冻结金额
	float m_fOrderFrzAmt;		// 交易冻结数量

	int m_nShareBln_Old;			// 股份余额
	int m_nShareAvl_Old;			// 股份可用
	int m_nShareTrdFrz_Old;			// 交易冻结
	int m_nShareOtd_Old;			// 在途数量

	float m_fCptlBln_Old;			// 资金余额
	float m_fCptlAvl_Old;			// 资金可用
	float m_fCptlTrdFrz_Old;		// 交易冻结
	float m_fCptlOutstanding_Old;	// 在途资金
	float m_fCptlOtdAvl_Old;		// 在途可用

	int m_nShareBln_New;			// 股份余额
	int m_nShareAvl_New;			// 股份可用
	int m_nShareTrdFrz_New;			// 交易冻结
	int m_nShareOtd_New;			// 在途数量

	float m_fCptlBln_New;			// 资金余额
	float m_fCptlAvl_New;			// 资金可用
	float m_fCptlTrdFrz_New;		// 交易冻结
	float m_fCptlOutstanding_New;	// 在途资金
	float m_fCptlOtdAvl_New;		// 在途可用
};
