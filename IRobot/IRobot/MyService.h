#pragma once

#include "IPCKDGateWayVistor.h"
#include "PageFuncTest.h"

#include "LoginVistor.h"
#include "SZA_BuyVistor.h"
#include "SZA_SellVistor.h"
#include "SHA_BuyVistor.h"
#include "SHA_SellVistor.h"

#include "SZB_BuyVistor.h"
#include "SHB_BuyVistor.h"
#include "SZB_SellVistor.h"
#include "SHB_SellVistor.h"
#include "SB_BuyVistor.h"
#include "SB_SellVistor.h"

#include "SZ_LOF_Vistor.h"
#include "SH_LOF_Vistor.h"

#include "QuerySecuAccVistor.h"
#include "QueryShares.h"


class CMyService
{
public:
	CMyService(void);
	~CMyService(void);

	void Init();
	void RegisterProtocal(IPCKDGateWayVistor *);
	void Run();

	CPageFuncTest * GetDlg() const { return m_pDlg; }
	void SetDlg(CPageFuncTest * val) { m_pDlg = val; }

private:
	CArray <IPCKDGateWayVistor*, IPCKDGateWayVistor*> m_ptServiceArray;
	CPageFuncTest *m_pDlg;

	int m_nTotalCaseCnt;
	int m_nSuccCaseCnt;
	int m_nFailCaseCnt;
	
	CLoginVistor LoginVistor;
	CSZA_BuyVistor SZA_BuyVistor; // 深圳A股买入
	CSZA_SellVistor SZA_SellVistor; // 深圳A股卖出
	CSHA_BuyVistor SHA_BuyVistor; // 上海A股买入
	CSHA_SellVistor SHA_SellVistor; // 上海A股卖出

	CSZB_BuyVistor SZB_BuyVistor; // 深圳B股买入
	CSHB_BuyVistor SHB_BuyVistor; // 上海B股买入
	CSZB_SellVistor SZB_SellVistor; // 深圳B股卖出
	CSHB_SellVistor SHB_SellVistor; // 上海B股卖出

	CSB_BuyVistor SB_BuyVistor; // 三板买入
	CSB_SellVistor SB_SellVistor; // 三板卖出

	CSZLOFVistor SZ_LOFVistor; // 深圳LOF基金业务
	CSHLOFVistor SH_LOFVistor; // 上海LOF基金业务

	CQueryShares QueryShares; // 查询股份
	CQuerySecuAccVistor QuerySecuAcc; // 查询股东
};
