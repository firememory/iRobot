#include "StdAfx.h"
#include "MyService.h"

#define MAX_FUNC_TEST_THREAD_NUM 1

HANDLE g_hFuncTestEvent;
DWORD WINAPI FuncTestRunThread(LPVOID);
CRITICAL_SECTION g_caFuncTest;
CRITICAL_SECTION g_caFuncTest_Statics;
CRITICAL_SECTION g_caFuncTest_PageUpdate;

CMyService::CMyService(void)
{
	m_pDlg = NULL;
	m_nTotalCaseCnt = m_nSuccCaseCnt = m_nFailCaseCnt = 0;
	m_nCurCaseIdx = 0;
	InitializeCriticalSection(&g_caFuncTest);
	InitializeCriticalSection(&g_caFuncTest_PageUpdate);
	InitializeCriticalSection(&g_caFuncTest_Statics);

	// 注意Event一定要设置成手动模式
	g_hFuncTestEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	for (int i =0; i<MAX_FUNC_TEST_THREAD_NUM; i++)
	{
		::CreateThread(NULL, 0, FuncTestRunThread, this, 0, NULL); 
	}	
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
	
	RegisterProtocal(&SZA_BuyVistor);
	RegisterProtocal(&SHA_BuyVistor);
	RegisterProtocal(&SHA_SellVistor);
	RegisterProtocal(&SZA_SellVistor);
	RegisterProtocal(&SZB_BuyVistor);
	RegisterProtocal(&SHB_BuyVistor);
	RegisterProtocal(&SZB_SellVistor);
	RegisterProtocal(&SHB_SellVistor);
	RegisterProtocal(&SB_BuyVistor);
	RegisterProtocal(&SB_SellVistor);
	RegisterProtocal(&SZ_LOFVistor);
	RegisterProtocal(&SH_LOFVistor);
	/*
	RegisterProtocal(&SH_ETFVistor);
	*/
	m_nTotalCaseCnt = m_ptServiceArray.GetCount();

	m_pDlg->SetCtrlTotalCaseNum(m_nTotalCaseCnt);
	m_pDlg->SetCtrlSuccCaseNum(m_nSuccCaseCnt);
	m_pDlg->SetCtrlFailCaseNum(m_nFailCaseCnt);

	// 初始化 Progress bar	
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
	m_nSuccCaseCnt = m_nFailCaseCnt = 0;

	m_pDlg->SetCtrlSuccCaseNum(m_nSuccCaseCnt);
	m_pDlg->SetCtrlFailCaseNum(m_nFailCaseCnt);

	m_pDlg->UpdateData(TRUE);

	m_nCurCaseIdx = 0;
	SetEvent(g_hFuncTestEvent);
}

DWORD WINAPI FuncTestRunThread(LPVOID pParam)
{
	DWORD dwThreadId = 0;	

	CMyService *pMyService = (CMyService *)pParam;
	CPageFuncTest *pPageFuncTest = pMyService->m_pDlg;
	IPCKDGateWayVistor *pService;

	int nCnt = pMyService->m_ptServiceArray.GetCount();

	while (WAIT_OBJECT_0 == ::WaitForSingleObject(g_hFuncTestEvent, INFINITE))
	{
		if (pMyService->m_nCurCaseIdx == nCnt)
		{
			ResetEvent(g_hFuncTestEvent);
		}
		else
		{
			EnterCriticalSection(&g_caFuncTest);
			pService = pMyService->m_ptServiceArray.GetAt(pMyService->m_nCurCaseIdx);
			pMyService->m_nCurCaseIdx++;
			LeaveCriticalSection(&g_caFuncTest);

			pService->BeginTest();
			if (!pService->Vistor())
			{				
				INCREMENT_LOCK(g_caFuncTest_Statics, pMyService->m_nFailCaseCnt);
			}
			else
			{
				INCREMENT_LOCK(g_caFuncTest_Statics, pMyService->m_nSuccCaseCnt);
			}

			// 更新界面
			pPageFuncTest->PostMessage(WM_FUNC_PAGE_UPDATE, 0, 0);
		}		
	}

	return 1;
}