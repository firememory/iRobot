#pragma once

#include "IPCKDGateWayVistor.h"

struct ORDER_403_RET_MSG
{
	char szBizNo[11];
	char szOrderID[11];
	char szAccount[21];
	char szPrice[21]; // 委托价格
	char szQty[21];		// 委托数量
	char szOrderAmt[21];	// 委托金额
	char szOrderFrzAmt[21];	// 交易冻结金额
	char szSeat[7];
	char szExtInst[11];
	char szExtAcc[33];
	char szExtSubAcc[33];
	char szExtFrzAmt[21];
};

#define DEFAULT_LEN 30

/************************************************************************/
/* 深A 市价买入                                                         */
/************************************************************************/
class CSZA_SJ_BuyVistor
	: public IPCKDGateWayVistor
{
public:
	CSZA_SJ_BuyVistor(void);
	virtual ~CSZA_SJ_BuyVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual BOOL SendMsg(char *);

	// 在执行前，获取一把相关数据，与执行后数据进行比较
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetMatchedData();

	// 向KCXP发送消息
	BOOL SendKcxpMsg();

	// 向Mid发送消息
	BOOL SendMidMsg();

	BOOL ChkPnt1();
	void ChkPnt2();

private:
	ORDER_403_RET_MSG *m_pMsg;
	int m_nRowNum;

	char m_szSecu_intl[DEFAULT_LEN];		// 证券代码
	char m_szPrice[DEFAULT_LEN];			// 委托价格
	char m_szQty[DEFAULT_LEN];				// 委托数量

	// 检测数据
	float m_fMatched_OrderFrzAmt;	// 成交的交易冻结金额
	float m_fMatched_Price;	// 成交价格
	float m_fMatched_Qty;	// 成交数量
	float m_fMatchedAmt;	// 成交金额
	float m_fMatched_SettAmt; // 清算金额

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
