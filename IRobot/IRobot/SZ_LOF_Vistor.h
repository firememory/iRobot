#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* 深圳LOF                                                         */
/************************************************************************/
class CSZLOFVistor
	: public IPCKDGateWayVistor
{
public:
	CSZLOFVistor(void);
	virtual ~CSZLOFVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual void ParseKcxpRetMsg();

	// 在执行前，获取一把相关数据，与执行后数据进行比较
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetOrderData();

	// 用例1: 限价买入
	BOOL TestCase_1(); 

	// 用例2: 市价买入: 五档即时成交剩余撤销 VB
	BOOL TestCase_2();

	// 用例3: 市价买入: 五档即时成交剩余转限 UB
	BOOL TestCase_3();

	BOOL ChkData();

	BOOL SaveCapital();

	BOOL SaveShares();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;

	char m_szSecu_Intl[DEFAULT_LEN];		// 证券代码
	char m_szSecu_Code[DEFAULT_LEN];		// 证券代码
	char m_szPrice[DEFAULT_LEN];			// 委托价格
	char m_szQty[DEFAULT_LEN];				// 委托数量

	char m_szMarket_Board[3];
	char m_szTrdId[3];
	char m_szCurrency[3];

	// 检测数据
	float m_fOrder_RltFrzAmt;		// 交易冻结金额
	float m_fOrder_RltFrzQty;		// 交易冻结数量

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
