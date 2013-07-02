#pragma once

#include "IPCKDGateWayVistor.h"
#include "SH_ETFVistor.h"

/************************************************************************/
/* 沪 债券ETF业务                                                         */
/************************************************************************/
class CSH_BOND_ETFVistor
	: public IPCKDGateWayVistor
{
public:
	CSH_BOND_ETFVistor(void);
	virtual ~CSH_BOND_ETFVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual void ParseKcxpRetMsg();

	// 在执行前，获取一把相关数据，与执行后数据进行比较
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetMatchedData();

	BOOL GetEtfInfo(char *pEtfIntl);
	BOOL GetEtfSecuInfo(char *pEtfIntl);
	BOOL GetEtfSecuShares(int nType);

	// ETF买卖
	BOOL TestCase_1(char *pEtf_Code, char* pTrd_Id); 
	BOOL ChkData_1();

	// ETF申购. 无现金替代/赎回
	BOOL TestCase_2(char *pEtf_Code, char* pTrd_Id); 
	BOOL ChkData_2();

	// ETF申购, 部分现金替代
	BOOL TestCase_3(char *pEtf_Code, char* pTrd_Id); 
	BOOL ChkData_3();

	// ETF赎回
	BOOL TestCase_4(char *pEtf_Code, char* pTrd_Id); 
	BOOL ChkData_4();

	BOOL SaveCapital();
	BOOL SaveShares();

	// 默认nType为0,只有在TestCase_3中nType为1,构造部分现金替代的情况
	BOOL SaveEtfSecuShares(int nType = 0);

	// 计算现金替代金额
	BOOL CalInsteadAmt(float &fInsteadAmt);

private:
	MID_403_ORDER_RET_MSG *m_pMsg;
	
	ETF_INFO *m_pEtfInfo; // ETF定义信息
	ETF_SECU_INFO *m_pEtfSecuInfo; // ETF 成份股信息
	int m_nEtfSecuCnt; // ETF 成份股数量
	float m_fInsteadAmt; // 现金替代金额
		

	// 检测数据
	float m_fMatched_OrderFrzAmt;	// 成交的交易冻结金额
	float m_fMatched_Price;	// 成交价格
	int m_nOrder_Qty; // 委托数量
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
