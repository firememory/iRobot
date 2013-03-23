#pragma once

#include "IPCKDGateWayVistor.h"

struct ETF_SECU_SHARES
{
	int nShareBln;			// 股份余额
	int nShareAvl;			// 股份可用
	int nShareTrdFrz;		// 交易冻结
	int nShareOtd;			// 在途数量
	int nSHareOtd_Avl;		// 在途可用
};

// ETF成份股信息
struct ETF_SECU_INFO
{
	char szSecuIntl[11]; // 成份股内码
	int nShareQty;		 // 成份股替代数量
	int nInsteadFlag;	 // 是否可现金替代 1:不可 2:可

	ETF_SECU_SHARES shares_old;
	ETF_SECU_SHARES shares_new;
};

/************************************************************************/
/* 沪ETF业务                                                         */
/************************************************************************/
class CSH_ETFVistor
	: public IPCKDGateWayVistor
{
public:
	CSH_ETFVistor(void);
	virtual ~CSH_ETFVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual void ParseKcxpRetMsg();

	// 在执行前，获取一把相关数据，与执行后数据进行比较
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetMatchedData();

	BOOL GetEtfSecuInfo(char *pEtfIntl);
	BOOL GetEtfSecuShares(char *pSecuIntl, ETF_SECU_SHARES* pShares);

	BOOL TestCase_1(); 
	BOOL TestCase_2();
	BOOL TestCase_3();
	BOOL TestCase_4(); 
	BOOL TestCase_5();
	BOOL TestCase_6();

	BOOL ChkData();

	BOOL SaveCapital();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;
	
	int m_nEtfSecuCnt; // ETF 成份股数量
	ETF_SECU_INFO *m_pEtfSecuInfo; // ETF 成份股信息	

	// 检测数据
	float m_fMatched_OrderFrzAmt;	// 成交的交易冻结金额
	float m_fMatched_Price;	// 成交价格
	int m_nMatched_Qty;	// 成交数量
	float m_fMatchedAmt;	// 成交金额
	float m_fMatched_SettAmt; // 清算金额

	int m_nShareBln_Old;			// 股份余额
	int m_nShareAvl_Old;			// 股份可用
	int m_nShareTrdFrz_Old;			// 交易冻结
	int m_nShareOtd_Old;			// 在途数量
	int m_nSHareOtd_Avl_Old;		// 在途可用

	float m_fCptlBln_Old;			// 资金余额
	float m_fCptlAvl_Old;			// 资金可用
	float m_fCptlTrdFrz_Old;		// 交易冻结
	float m_fCptlOutstanding_Old;	// 在途资金
	float m_fCptlOtdAvl_Old;		// 在途可用

	int m_nShareBln_New;			// 股份余额
	int m_nShareAvl_New;			// 股份可用
	int m_nShareTrdFrz_New;			// 交易冻结
	int m_nShareOtd_New;			// 在途数量
	int m_nSHareOtd_Avl_New;		// 在途可用

	float m_fCptlBln_New;			// 资金余额
	float m_fCptlAvl_New;			// 资金可用
	float m_fCptlTrdFrz_New;		// 交易冻结
	float m_fCptlOutstanding_New;	// 在途资金
	float m_fCptlOtdAvl_New;		// 在途可用
};
