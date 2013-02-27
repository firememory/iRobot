#pragma once

#include "IPCKDGateWayVistor.h"
#include "PageFuncTest.h"

#include "LoginVistor.h"
#include "SZA_XJ_BuyVistor.h"
#include "SZA_SJ_BuyVistor.h"
#include "SHA_SJ_BuyVistor.h"
#include "SHA_XJ_BuyVistor.h"

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
	CSZA_XJ_BuyVistor SZA_XJ_BuyVistor; // ����A���޼�ί��
	CSZA_SJ_BuyVistor SZA_SJ_BuyVistor; // ����A���м�ί��
	
	CSHA_XJ_BuyVistor SHA_XJ_BuyVistor; // �Ϻ�A���޼�ί��
	CSHA_SJ_BuyVistor SHA_SJ_BuyVistor; // �Ϻ�A���м�ί��

	CQueryShares QueryShares; // ��ѯ�ɷ�
	CQuerySecuAccVistor QuerySecuAcc; // ��ѯ�ɶ�
};
