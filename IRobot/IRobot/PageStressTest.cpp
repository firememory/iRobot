// PageStressTest.cpp : implementation file
//

#include "stdafx.h"
#include "IRobot.h"
#include "PageStressTest.h"
#include "ParseKcbpLog.h"

#define  MAX_THREAD_NUM 30
HANDLE g_Event;
DWORD WINAPI RunThread(LPVOID);
CRITICAL_SECTION ca;


// CPageStressTest dialog

IMPLEMENT_DYNAMIC(CPageStressTest, CPropertyPage)

CPageStressTest::CPageStressTest()
	: CPropertyPage(CPageStressTest::IDD)
	, m_strKcbpLogPath(_T("d:/kcbp.log"))
	, m_nThreadNum(1)
{
	m_pParsKcbplog = NULL;

	InitializeCriticalSection(&ca);

	g_Event =  CreateEvent(NULL, TRUE, FALSE, NULL);

	for (int i=0;i<MAX_THREAD_NUM;i++)
	{
		::CreateThread(NULL, 0, RunThread, this, 0, NULL); 
	}
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
	DDX_Control(pDX, IDCANCEL, m_ctrlBtnPause);
	DDX_Control(pDX, IDC_EDIT_THREAD_NUM, m_ctrlThreadNum);
	DDX_Text(pDX,IDC_EDIT_THREAD_NUM, m_nThreadNum);
	DDX_Control(pDX, IDC_BUTTON_READ_KCBP_LOG, m_ctrlBtnInit);
	DDX_Control(pDX, IDC_SLIDER_THREAD_NUM, m_ctrlSlider);
}


BEGIN_MESSAGE_MAP(CPageStressTest, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PERFORMANCE, &CPageStressTest::OnBnClickedButtonPerformance)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CPageStressTest::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDCANCEL, &CPageStressTest::OnBnClickedPauseThread)
	ON_BN_CLICKED(IDC_BUTTON_READ_KCBP_LOG, &CPageStressTest::OnBnClickedButtonReadKcbpLog)
END_MESSAGE_MAP()


// CPageStressTest message handlers
void CPageStressTest::OnBnClickedButtonPerformance()
{
	// TODO: Add your control notification handler code here
	m_ctrlSlider.EnableWindow(FALSE);
	m_ctrlBtnRun.EnableWindow(FALSE);
	m_ctrlBtnPause.EnableWindow();	

	if (m_pParsKcbplog != NULL)
	{
		for (int i=0; i<m_nThreadNum; i++)
		{
			SetEvent(g_Event);
		}
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
	m_ctrlBtnRun.EnableWindow();
	m_ctrlSlider.EnableWindow();

	for (int i=0; i<m_nThreadNum; i++)
	{
		ResetEvent(g_Event);
	}
}

void CPageStressTest::OnBnClickedButtonReadKcbpLog()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (NULL == m_pParsKcbplog)
	{
		m_pParsKcbplog = new CParseKcbpLog;
	}
	
	if (m_pParsKcbplog != NULL)
	{
		m_pParsKcbplog->SetKcbpLogPath(m_strKcbpLogPath.GetBuffer());
		if (TRUE == m_pParsKcbplog->ParseLog())
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
			m_ctrlSlider.SetPos(0);
		}		
	}
}

DWORD WINAPI RunThread(LPVOID pParam)
{
	DWORD dwThreadId = 0;
	DWORD dwCnt = 0;

	while(1)
	{		
		::WaitForSingleObject(g_Event, INFINITE);

		dwThreadId = GetCurrentThreadId();

		EnterCriticalSection(&ca);
		//m_pParsKcbplog->ReadRlt();
		//m_pParsKcbplog->Exec();
	
		printf("Thread:%d, Cnt:%d\n", dwThreadId, dwCnt);
		dwCnt++;
		LeaveCriticalSection(&ca);

		ResetEvent(g_Event);	
	}

	return 1;
}
