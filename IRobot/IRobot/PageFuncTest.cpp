// PageFuncTest.cpp : implementation file
//

#include "stdafx.h"
#include "IRobot.h"
#include "PageFuncTest.h"

#include "loginterface.h"
#include "Cfg.h"
#include "KcxpConn.h"
#include "MidConn.h"
#include "DBConnect.h"
#include "MyService.h"

/************************************************************************/
/* 全局变量                                                             */
/************************************************************************/
extern CCfg *g_pCfg;
extern CLoginterface *g_pLog;
extern CKcxpConn *g_pKcxpConn;
extern CMidConn *g_pMidConn;
extern CDBConnect *g_pDBConn;
extern CMyService *g_pMyService;

// CPageFuncTest dialog

IMPLEMENT_DYNAMIC(CPageFuncTest, CPropertyPage)

CPageFuncTest::CPageFuncTest()
	: CPropertyPage(CPageFuncTest::IDD)
{
}

CPageFuncTest::~CPageFuncTest()
{
}

void CPageFuncTest::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TOTAL_CASE, m_ctrlTotalCaseNum);
	DDX_Control(pDX, IDC_SUCC_CASE, m_ctrlSuccCaseNum);
	DDX_Control(pDX, IDC_FAIL_CASE, m_ctrlFailCaseNum);

	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlWait);
	DDX_Control(pDX, IDC_EDIT_LOG_MSG, m_ctrlLogMsg);
	DDX_Control(pDX, IDOK3, m_ctrlBtnRun);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_ctrlBtnConn);
	DDX_Control(pDX, IDC_BUTTON_DISCONNECT, m_ctrlBtnDisConn);
}


BEGIN_MESSAGE_MAP(CPageFuncTest, CPropertyPage)
	ON_BN_CLICKED(IDOK3, &CPageFuncTest::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &CPageFuncTest::OnNMCustomdrawProgress1)			
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CPageFuncTest::OnBnClickedButtonClean)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CPageFuncTest::OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CPageFuncTest::OnBnClickedButtonDisconnect)
END_MESSAGE_MAP()


// CPageFuncTest message handlers

void CPageFuncTest::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	g_pMyService->Run();
}

void CPageFuncTest::OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CPageFuncTest::SetCtrlTotalCaseNum( int nCnt)
{
	CString strTmp;
	strTmp.Format("%d", nCnt);
	m_ctrlTotalCaseNum.SetWindowText(strTmp.GetBuffer());
}

void CPageFuncTest::SetCtrlSuccCaseNum( int nCnt)
{
	CString strTmp;
	strTmp.Format("%d", nCnt);
	m_ctrlSuccCaseNum.SetWindowText(strTmp.GetBuffer());
}

void CPageFuncTest::SetCtrlFailCaseNum( int nCnt)
{
	CString strTmp;
	strTmp.Format("%d", nCnt);
	m_ctrlFailCaseNum.SetWindowText(strTmp.GetBuffer());
}


void CPageFuncTest::OnBnClickedButtonClean()
{
	// TODO: Add your control notification handler code here
	m_ctrlLogMsg.SetWindowText("");	

	SetCtrlSuccCaseNum(0);
	SetCtrlFailCaseNum(0);

}
void CPageFuncTest::OnBnClickedButtonInit()
{
	// TODO: Add your control notification handler code here
	g_pLog->SetDlg(this);

	// 初始化MID连接
	if (!g_pMidConn->Init())
	{		
		return;
	}

	if (g_pMidConn->Connect())
	{
		g_pLog->WriteRunLog(SYS_MODE, LOG_DEBUG, "MID连接成功!");
	}
	else
	{
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "MID连接失败!");
		if (g_pCfg->GetTestMode() == MID_MODE)
		{
			return;
		}
	}			

	// 初始化KCXP连接
	if (g_pKcxpConn->InitKcxp())
	{
		if (g_pKcxpConn->OpLogin())
		{
			g_pLog->WriteRunLog(SYS_MODE, LOG_DEBUG, "KCXP连接成功!");
		}
		else
		{
			g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "KCXP连接失败!");
			if (g_pCfg->GetTestMode() == KCXP_MODE)
			{
				return;
			}
		}
	}		

	// 初始化数据库连接
	if (!g_pDBConn->init())
	{
		return;
	}

	g_pMyService = new CMyService;
	if (NULL == g_pMyService)
	{
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "创建MyService类失败!");
		return;
	}
	else
	{
		g_pMyService->SetDlg(this);
		g_pMyService->Init();
	}

	m_ctrlBtnRun.EnableWindow();
	m_ctrlBtnDisConn.EnableWindow();
	m_ctrlBtnConn.EnableWindow(FALSE);
}

void CPageFuncTest::OnBnClickedButtonDisconnect()
{
	// TODO: Add your control notification handler code here
	g_pMidConn->DisConnect();
	g_pKcxpConn->DisconnectKcxp();
	g_pDBConn->Disconnect();

	m_ctrlBtnDisConn.EnableWindow(FALSE);
	m_ctrlBtnConn.EnableWindow(TRUE);
}
