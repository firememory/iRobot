#pragma once

struct STOCK_INFO
{
	char szSecuIntl[10];	// 证券代码
	int nShareBln;			// 股份余额
	int nShareAvl;			// 股份可用
	int nShareTrdFrz;		// 交易冻结
	int nShareOtd;			// 在途数量
	int nShareFrz;			// 异常冻结
	int fShareUnTrdQty;		// 非流通数
	float fCurrentCost;		// 买入成本
	float fCostPrice;		// 买入均价
	int nMktVal;			// 市值
};

// 客户的股份信息
class CShare
{
public:
	CShare(void);
	~CShare(void);

	STOCK_INFO *m_pStocks;

	BOOL InitShares();

};
