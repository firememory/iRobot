#pragma once

#include "IPCKDGateWayVistor.h"
#include "PageFuncTest.h"
#include "LoginVistor.h"
#include "SZA_XJ_BuyVistor.h"
#include "SZA_SJ_BuyVistor.h"
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
	CSZA_XJ_BuyVistor SZA_XJ_BuyVistor; // 深圳A股限价委托
	CSZA_SJ_BuyVistor SZA_SJ_BuyVistor; // 深圳A股市价委托
	CQueryShares QueryShares;
};
