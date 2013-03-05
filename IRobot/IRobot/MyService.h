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
	CSZA_BuyVistor SZA_BuyVistor; // ����A������
	CSZA_SellVistor SZA_SellVistor; // ����A������
	CSHA_BuyVistor SHA_BuyVistor; // �Ϻ�A������
	CSHA_SellVistor SHA_SellVistor; // �Ϻ�A������

	CSZB_BuyVistor SZB_BuyVistor; // ����B������
	CSHB_BuyVistor SHB_BuyVistor; // �Ϻ�B������
	CSZB_SellVistor SZB_SellVistor; // ����B������
	CSHB_SellVistor SHB_SellVistor; // �Ϻ�B������

	CSB_BuyVistor SB_BuyVistor; // ��������
	CSB_SellVistor SB_SellVistor; // ��������

	CSZLOFVistor SZ_LOFVistor; // ����LOF����ҵ��
	CSHLOFVistor SH_LOFVistor; // �Ϻ�LOF����ҵ��

	CQueryShares QueryShares; // ��ѯ�ɷ�
	CQuerySecuAccVistor QuerySecuAcc; // ��ѯ�ɶ�
};
