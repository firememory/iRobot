#pragma once

#include "IPCKDGateWayVistor.h"
#include "PageFuncTest.h"

#include "LoginVistor.h"
#include "SZA_BuyVistor.h"
#include "SZA_SellVistor.h"
#include "SHA_BuyVistor.h"
#include "SHA_SellVistor.h"

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
	CSZA_BuyVistor SZA_BuyVistor; // ����A������
	CSZA_SellVistor SZA_SellVistor; // ����A������
	CSHA_BuyVistor SHA_BuyVistor; // �Ϻ�A������
	CSHA_SellVistor SHA_SellVistor; // �Ϻ�A������

	CQueryShares QueryShares; // ��ѯ�ɷ�
	CQuerySecuAccVistor QuerySecuAcc; // ��ѯ�ɶ�
};
