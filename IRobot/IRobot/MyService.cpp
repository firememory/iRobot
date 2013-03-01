#include "StdAfx.h"
#include "MyService.h"

CMyService::CMyService(void)
{
	m_pDlg = NULL;
	m_nTotalCaseCnt = m_nSuccCaseCnt = m_nFailCaseCnt = 0;
}

CMyService::~CMyService(void)
{
	/*
	int nCnt = m_ptServiceArray.GetCount();
	for (int i=0; i<nCnt; i++)
	{
		delete m_ptServiceArray[i];
	}
	*/

	m_ptServiceArray.RemoveAll();
}

void CMyService::Init()
{
	RegisterProtocal(&LoginVistor);

	RegisterProtocal(&QueryShares);
	RegisterProtocal(&QuerySecuAcc);
	//RegisterProtocal(&SZA_BuyVistor);
	//RegisterProtocal(&SHA_BuyVistor);
	RegisterProtocal(&SHA_SellVistor);

	m_nTotalCaseCnt = m_ptServiceArray.GetCount();

	m_pDlg->SetCtrlTotalCaseNum(m_nTotalCaseCnt);
	m_pDlg->SetCtrlSuccCaseNum(m_nSuccCaseCnt);
	m_pDlg->SetCtrlFailCaseNum(m_nFailCaseCnt);

	// ³õÊ¼»¯ Progress bar	
	m_pDlg->m_ctrlWait.SetRange(0, m_nTotalCaseCnt);
	m_pDlg->m_ctrlWait.SetStep(1);
	m_pDlg->m_ctrlWait.SetBarColor(COLORREF(RGB(147, 153, 236)));

	m_pDlg->UpdateData(TRUE);
}

void CMyService::RegisterProtocal(IPCKDGateWayVistor *pService)
{
	m_ptServiceArray.Add(pService);
}

void CMyService::Run()
{
	m_pDlg->m_ctrlWait.SetPos(0);

	int nCnt = m_ptServiceArray.GetCount();
	char szResult[10] = {0};

	for (int i=0; i<nCnt; i++)
	{
		m_ptServiceArray[i]->BeginTest();

		if (!m_ptServiceArray[i]->Vistor())
		{			
			m_nFailCaseCnt++;		
		}
		else
		{			
			m_nSuccCaseCnt++;		
		}

		m_pDlg->m_ctrlWait.StepIt();
	}	

	m_pDlg->SetCtrlSuccCaseNum(m_nSuccCaseCnt);
	m_pDlg->SetCtrlFailCaseNum(m_nFailCaseCnt);

	m_pDlg->UpdateData(TRUE);
}
