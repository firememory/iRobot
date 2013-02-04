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
	RegisterProtocal(&BuyVistor);
	RegisterProtocal(&QueryShares);

	m_nTotalCaseCnt = m_ptServiceArray.GetCount();

	m_pDlg->SetCtrlTotalCaseNum(m_nTotalCaseCnt);

	m_pDlg->UpdateData(TRUE);
}

void CMyService::RegisterProtocal(IPCKDGateWayVistor *pService)
{
	m_ptServiceArray.Add(pService);
}

void CMyService::Run()
{
	int nCnt = m_ptServiceArray.GetCount();
	char szResult[10] = {0};

	for (int i=0; i<nCnt; i++)
	{
		m_ptServiceArray[i]->BeginTest();
		if (!m_ptServiceArray[i]->Vistor())
		{
			strcpy_s(szResult, "Failed");
			m_nFailCaseCnt++;
		}
		else
		{
			strcpy_s(szResult, "Succeed");
			m_nSuccCaseCnt++;
		}

		m_ptServiceArray[i]->EndTest(&szResult[0]);

		m_pDlg->m_ctrlWait.StepIt();
	}

	m_pDlg->m_ctrlWait.SetPos(0);

	m_pDlg->SetCtrlSuccCaseNum(m_nSuccCaseCnt);
	m_pDlg->SetCtrlFailCaseNum(m_nFailCaseCnt);

	m_pDlg->UpdateData(TRUE);
}
