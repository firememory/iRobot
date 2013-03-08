#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* 深A股买入                                                         */
/************************************************************************/
class CSZA_BuyVistor
	: public IPCKDGateWayVistor
{
public:
	CSZA_BuyVistor(void);
	virtual ~CSZA_BuyVistor(void);

	// 调用测试
	virtual BOOL Vistor();

	// 解析Mid接口返回消息
	virtual BOOL ResultStrToTable(char *);
	
	// 解析KCXP接口返回消息
	virtual void ParseKcxpRetMsg();

	// 测试用例集
	BOOL TestCase_1();  // 市价买入
	BOOL TestCase_2();  // 对手方最优价格委托
	BOOL TestCase_3();	// 本方最有价格委托
	BOOL TestCase_4();	// 即时成交剩余撤销委托 
	BOOL TestCase_5();	// 五档即时成交剩余撤销
	BOOL TestCase_6();	// 全额成交或撤销委托

	// 检查点
	BOOL ChkData();
	BOOL InitUserData();   // 委托前，客户资金、股份快照
	BOOL UpdateUserData(); // 成交后，客户资金、股份快照
	BOOL GetMatchedData(); // 成交结果

	// 资金蓝补
	BOOL SaveCapital();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;

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
