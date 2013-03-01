// PageStressTest.cpp : implementation file
//

#include "stdafx.h"
#include "IRobot.h"
#include "PageStressTest.h"
#include "ParseKcbpLog.h"

#define  MAX_THREAD_NUM 3
HANDLE g_Event;
DWORD WINAPI RunThread_1(LPVOID);
DWORD WINAPI RunThread_2(LPVOID);
DWORD WINAPI RunThread_3(LPVOID);
CRITICAL_SECTION ca;

extern BOOL g_bThreadExit;
extern CParseKcbpLog *g_pParseKcbpLog;

// CPageStressTest dialog

IMPLEMENT_DYNAMIC(CPageStressTest, CPropertyPage)

CPageStressTest::CPageStressTest()
	: CPropertyPage(CPageStressTest::IDD)
	, m_strKcbpLogPath(_T("d:/kcbp.log"))
	, m_nThreadNum(1)
	, m_nActiveThreadNum(0)
{
	InitializeCriticalSection(&ca);

	// 注意Event一定要设置成自动模式
	g_Event =  CreateEvent(NULL, FALSE, FALSE, NULL);
}

CPageStressTest::~CPageStressTest()
{
}

void CPageStressTest::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_KCBP_LOG_PATH, m_ctrlKcbpLogPath);
	DDX_Text(pDX,IDC_EDIT_KCBP_LOG_PATH, m_strKcbpLogPath);
	DDX_Control(pDX, IDC_BUTTON_PERFORMANCE, m_ctrlBtnRun);
	DDX_Control(pDX, IDC_BUTTON_PAUSE_THREAD, m_ctrlBtnPause);
	DDX_Control(pDX, IDC_EDIT_THREAD_NUM, m_ctrlThreadNum);
	DDX_Text(pDX,IDC_EDIT_THREAD_NUM, m_nThreadNum);
	DDX_Control(pDX, IDC_BUTTON_READ_KCBP_LOG, m_ctrlBtnInit);
	DDX_Control(pDX, IDC_SLIDER_THREAD_NUM, m_ctrlSlider);
	DDX_Control(pDX, IDC_EDIT_ACTIVE_THREAD_NUM, m_ctrlActiveThreadNum);
	DDX_Text(pDX,IDC_EDIT_ACTIVE_THREAD_NUM, m_nActiveThreadNum);
}

#define WM_STATUS_UPDATE WM_USER + 110

BEGIN_MESSAGE_MAP(CPageStressTest, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PERFORMANCE, &CPageStressTest::OnBnClickedButtonPerformance)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THREAD_NUM, &CPageStressTest::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE_THREAD, &CPageStressTest::OnBnClickedPauseThread)
	ON_BN_CLICKED(IDC_BUTTON_READ_KCBP_LOG, &CPageStressTest::OnBnClickedButtonReadKcbpLog)
	ON_MESSAGE(WM_STATUS_UPDATE, &CPageStressTest::OnStatusUpdate)
END_MESSAGE_MAP()


// CPageStressTest message handlers
void CPageStressTest::OnBnClickedButtonPerformance()
{
	// TODO: Add your control notification handler code here
	m_ctrlSlider.EnableWindow(FALSE);
	m_ctrlBtnRun.EnableWindow(FALSE);
	m_ctrlBtnPause.EnableWindow();

	g_bThreadExit = FALSE;
	m_nActiveThreadNum = m_nThreadNum;
	UpdateData(FALSE);

	for (int n=0;n<m_nThreadNum;n++)
	{
		// 激活线程
		SetEvent(g_Event);
	}
}
void CPageStressTest::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	m_nThreadNum = m_ctrlSlider.GetPos();
	UpdateData(FALSE);

	*pResult = 0;
}

void CPageStressTest::OnBnClickedPauseThread()
{
	// TODO: Add your control notification handler code here
	m_ctrlBtnPause.EnableWindow(FALSE);
	m_ctrlBtnInit.EnableWindow();
	m_ctrlSlider.EnableWindow();

	// 线程挂起
	g_bThreadExit = TRUE;
}

void CPageStressTest::OnBnClickedButtonReadKcbpLog()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	g_pParseKcbpLog->SetKcbpLogPath(m_strKcbpLogPath.GetBuffer());
	if (TRUE == g_pParseKcbpLog->ReadLog())
	{			
		m_ctrlBtnInit.EnableWindow(FALSE);
		m_ctrlKcbpLogPath.EnableWindow(FALSE);

		m_ctrlBtnRun.EnableWindow();
		m_ctrlSlider.EnableWindow();

		//设置滑动范围
		m_ctrlSlider.SetRange(1, MAX_THREAD_NUM);

		//每10个单位画一刻度
		m_ctrlSlider.SetTicFreq(10);
		m_ctrlSlider.SetTic(10);
		m_ctrlSlider.SetPos(1);
		UpdateData(FALSE);

		for (int i=0;i<MAX_THREAD_NUM;i++)
		{
			::CreateThread(NULL, 0, RunThread_1, this, 0, NULL); 
			::CreateThread(NULL, 0, RunThread_2, this, 0, NULL); 
			::CreateThread(NULL, 0, RunThread_3, this, 0, NULL); 
		}
	}		
}

LRESULT CPageStressTest::OnStatusUpdate( WPARAM wParam, LPARAM lParam )
{
	m_nActiveThreadNum --;
	UpdateData(FALSE);

	if (m_nActiveThreadNum == 0)
	{
		// 无活动线程
		m_ctrlBtnRun.EnableWindow(FALSE);
		m_ctrlBtnPause.EnableWindow(FALSE);
		m_ctrlBtnInit.EnableWindow();
	}

	return 1;
}

DWORD WINAPI RunThread_1(LPVOID pParam)
{
	DWORD dwThreadId = 0;
	DWORD dwCnt = 0;

	CPageStressTest *pPageStressTest = (CPageStressTest *)pParam;	

	while(WAIT_OBJECT_0 == ::WaitForSingleObject(g_Event, INFINITE))
	{					
		g_pParseKcbpLog->ExecMultiCmds_1();
		pPageStressTest->PostMessage(WM_STATUS_UPDATE, 0, 0);		
	}

	return 1;
}

DWORD WINAPI RunThread_2(LPVOID pParam)
{
	DWORD dwThreadId = 0;
	DWORD dwCnt = 0;

	CPageStressTest *pPageStressTest = (CPageStressTest *)pParam;	

	while(WAIT_OBJECT_0 == ::WaitForSingleObject(g_Event, INFINITE))
	{					
		g_pParseKcbpLog->ExecMultiCmds_2();
		pPageStressTest->PostMessage(WM_STATUS_UPDATE, 0, 0);
	}

	return 1;
}

DWORD WINAPI RunThread_3(LPVOID pParam)
{
	DWORD dwThreadId = 0;
	DWORD dwCnt = 0;

	CPageStressTest *pPageStressTest = (CPageStressTest *)pParam;	

	while(WAIT_OBJECT_0 == ::WaitForSingleObject(g_Event, INFINITE))
	{			
		g_pParseKcbpLog->ExecMultiCmds_3();
		pPageStressTest->PostMessage(WM_STATUS_UPDATE, 0, 0);		
	}

	return 1;
}