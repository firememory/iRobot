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

	m_nTotalCaseCnt = m_ptServiceArray.GetCount();
	
	CString strTotalCaseCnt;
	strTotalCaseCnt.Format("%d", m_nTotalCaseCnt);

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
	for (int i=0; i<nCnt; i++)
	{
		m_ptServiceArray[i]->Vistor();
	}
}
