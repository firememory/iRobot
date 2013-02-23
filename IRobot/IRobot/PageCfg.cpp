// PageCfg.cpp : implementation file
//

#include "stdafx.h"
#include "IRobot.h"
#include "PageCfg.h"

#include "loginterface.h"
#include "Cfg.h"
#include "KcxpConn.h"
#include "MidConn.h"
#include "DBConnect.h"

/************************************************************************/
/* 全局变量                                                             */
/************************************************************************/
extern CCfg *g_pCfg;
extern CLoginterface *g_pLog;
extern CKcxpConn *g_pKcxpConn;
extern CMidConn *g_pMidConn;
extern CDBConnect *g_pDBConn;

// CPageCfg dialog

IMPLEMENT_DYNAMIC(CPageCfg, CPropertyPage)

CPageCfg::CPageCfg()
	: CPropertyPage(CPageCfg::IDD)
	, m_strKcxpIp(_T(""))
	, m_strKcxpPort(_T(""))
	, m_strKcxpSendQ(_T(""))
	, m_strKcxpRecvQ(_T(""))
	, m_strMidIp(_T(""))	
	, m_strMidPort(_T(""))
	, m_strLogPath(_T(""))
	, m_strDBConnStr(_T(""))
	, m_strDBUser(_T(""))
	, m_strDBPwd(_T(""))	
	, m_strCustID(_T(""))
	, m_strAccount(_T(""))
	, m_strCustPwd(_T(""))
	, m_strOpId(_T(""))
	, m_strOpPwd(_T(""))
	, m_strBranch(_T(""))
	, m_nLogLevel(0)
	, m_nRefreshDBGap(1000)
{
	m_nTestMode = USE_MID;
}

CPageCfg::~CPageCfg()
{
}

void CPageCfg::OnBnClickedGetCfg()
{
	ReadCfg();

	// 必须在g_pLog创建后，再调用这两句
	InitComboxLogLevel();
	UpdateData(FALSE);

	m_ctrlKcxpIp.EnableWindow(TRUE);
	m_ctrlKcxpPort.EnableWindow(TRUE);
	m_ctrlKcxpSendQ.EnableWindow(TRUE);
	m_ctrlKcxpRecvQ.EnableWindow(TRUE);

	m_ctrlMidIp.EnableWindow(TRUE);
	m_ctrlMidPort.EnableWindow(TRUE);

	m_ctrlLogPath.EnableWindow(TRUE);

	m_ctrlUserMid.EnableWindow(TRUE);
	m_ctrlUseKcxp.EnableWindow(TRUE);

	m_ctrlDBConStr.EnableWindow(TRUE);
	m_ctrlDBUser.EnableWindow(TRUE);
	m_ctrlDBPwd.EnableWindow(TRUE);

	m_ctrlCustID.EnableWindow(TRUE);
	m_ctrlAccount.EnableWindow(TRUE);
	m_ctrlCustPwd.EnableWindow(TRUE);

	m_ctrlOpId.EnableWindow(TRUE);
	m_ctrlOpPwd.EnableWindow(TRUE);
	m_ctrlBranch.EnableWindow(TRUE);

	m_ctrlBtnGetCfg.EnableWindow(FALSE);
	m_ctrlBtnSetCfg.EnableWindow();
}

void CPageCfg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_KCXP_IP, m_ctrlKcxpIp);
	DDX_Control(pDX, IDC_KCXP_PORT, m_ctrlKcxpPort);
	DDX_Control(pDX, IDC_KCXP_SENDQ, m_ctrlKcxpSendQ);
	DDX_Control(pDX, IDC_KCXP_RECVQ, m_ctrlKcxpRecvQ);
	DDX_Control(pDX, IDC_MID_IP, m_ctrlMidIp);
	DDX_Control(pDX, IDC_MID_PORT, m_ctrlMidPort);
	DDX_Control(pDX, IDC_LOG_PATH, m_ctrlLogPath);

	DDX_Control(pDX, IDC_USE_MID, m_ctrlUserMid);
	DDX_Control(pDX, IDC_USE_KCXP, m_ctrlUseKcxp);
	DDX_Control(pDX, IDC_DB_CONN, m_ctrlDBConStr);
	DDX_Control(pDX, IDC_DB_USER, m_ctrlDBUser);
	DDX_Control(pDX, IDC_DB_PWD, m_ctrlDBPwd);

	DDX_Control(pDX, IDC_AGENT_CUSTID, m_ctrlCustID);
	DDX_Control(pDX, IDC_AGENT_ACCOUNT, m_ctrlAccount);
	DDX_Control(pDX, IDC_AGENT_CUSTPWD, m_ctrlCustPwd);
	DDX_Control(pDX, IDC_AGENT_OPID, m_ctrlOpId);
	DDX_Control(pDX, IDC_AGENT_OPPWD, m_ctrlOpPwd);
	DDX_Control(pDX, IDC_AGENT_BRANCH, m_ctrlBranch);


	DDX_Control(pDX, IDC_COMBOBOXEX_LOG_LEVEL, m_ctrlLogLevel);		

	DDX_Control(pDX, IDC_EDIT_DB_GETDATA_GAP, m_ctrlRefreshDbGap);
	DDX_Text(pDX, IDC_EDIT_DB_GETDATA_GAP, m_nRefreshDBGap);

	DDX_Text(pDX, IDC_KCXP_IP, m_strKcxpIp);
	DDX_Text(pDX, IDC_KCXP_PORT, m_strKcxpPort);
	DDX_Text(pDX, IDC_KCXP_SENDQ, m_strKcxpSendQ);
	DDX_Text(pDX, IDC_KCXP_RECVQ, m_strKcxpRecvQ);

	DDX_Text(pDX, IDC_MID_IP, m_strMidIp);
	DDX_Text(pDX, IDC_MID_PORT, m_strMidPort);
	DDX_Text(pDX, IDC_LOG_PATH, m_strLogPath);

	DDX_Text(pDX, IDC_DB_CONN, m_strDBConnStr);
	DDX_Text(pDX, IDC_DB_USER, m_strDBUser);
	DDX_Text(pDX, IDC_DB_PWD, m_strDBPwd);	

	DDX_Text(pDX, IDC_AGENT_CUSTID, m_strCustID);
	DDX_Text(pDX, IDC_AGENT_ACCOUNT, m_strAccount);
	DDX_Text(pDX, IDC_AGENT_CUSTPWD, m_strCustPwd);
	DDX_Text(pDX, IDC_AGENT_OPID, m_strOpId);
	DDX_Text(pDX, IDC_AGENT_OPPWD, m_strOpPwd);
	DDX_Text(pDX, IDC_AGENT_BRANCH, m_strBranch);
	DDX_Control(pDX, IDC_GET_CFG, m_ctrlBtnGetCfg);
	DDX_Control(pDX, IDC_SET_CFG, m_ctrlBtnSetCfg);
}


BEGIN_MESSAGE_MAP(CPageCfg, CPropertyPage)
	ON_BN_CLICKED(IDC_USE_MID, &CPageCfg::OnBnClickedUseMid)
	ON_BN_CLICKED(IDC_USE_KCXP, &CPageCfg::OnBnClickedUseKcxp)
	ON_BN_CLICKED(IDC_SET_CFG, &CPageCfg::OnBnClickedSetCfg)
	ON_BN_CLICKED(IDC_GET_CFG, &CPageCfg::OnBnClickedGetCfg)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_LOG_LEVEL, &CPageCfg::OnCbnSelchangeComboboxex1)
END_MESSAGE_MAP()


// CPageCfg message handlers
void CPageCfg::OnBnClickedUseMid()
{
	// TODO: Add your control notification handler code here
	m_nTestMode = USE_MID;
}

void CPageCfg::OnBnClickedUseKcxp()
{
	// TODO: Add your control notification handler code here
	m_nTestMode = USE_KCXP;
}

void CPageCfg::OnCbnSelchangeComboboxex1()
{
	// TODO: Add your control notification handler code here
	int nSel = m_ctrlLogLevel.GetCurSel();
	m_nLogLevel = nSel;

	g_pLog->SetLogLevel(nSel);
}

void CPageCfg::InitComboxLogLevel()
{
	COMBOBOXEXITEM   Item;
	for(int i=0; i<MAX_LOG_LEVEL; i++)
	{
		Item.mask = CBEIF_TEXT;
		Item.pszText   =  g_pLog->m_stLogLevelDescription[i].description;
		Item.iItem   =  g_pLog->m_stLogLevelDescription[i].level;  
		m_ctrlLogLevel.InsertItem(&Item);
	}

	m_ctrlLogLevel.SetCurSel(m_nLogLevel);
}

void CPageCfg::OnBnClickedSetCfg()
{	
	g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "========Server Start========");

	// TODO: Add your control notification handler code here
	
	// 获取设置的参数
	UpdateData(TRUE);

	// 保存配置到配置文件
	SetCfg();

	m_ctrlBtnSetCfg.EnableWindow(FALSE);
	m_ctrlBtnGetCfg.EnableWindow();
}

BOOL CPageCfg::ReadCfg()
{
	g_pCfg->ReadCfg();

	m_strKcxpIp = g_pCfg->GetKcxpIp();
	m_strKcxpPort = g_pCfg->GetKcxpPort();
	m_strKcxpSendQ = g_pCfg->GetKcxpSendQ();
	m_strKcxpRecvQ = g_pCfg->GetKcxpRecvQ();

	m_strMidIp = g_pCfg->GetMidIp();
	m_strMidPort = g_pCfg->GetMidPort();

	m_strLogPath = g_pCfg->GetLogPath();

	m_nTestMode = g_pCfg->GetTestMode();

	if (m_nTestMode == USE_MID)
	{
		m_ctrlUserMid.SetCheck(TRUE);
		m_ctrlUseKcxp.SetCheck(FALSE);
	}
	else
	{
		m_ctrlUserMid.SetCheck(FALSE);
		m_ctrlUseKcxp.SetCheck(TRUE);
	}

	m_nRefreshDBGap = g_pCfg->GetRefreshDBGap();

	m_strDBConnStr = g_pCfg->GetDBConnStr();
	m_strDBUser = g_pCfg->GetDBUser();
	m_strDBPwd = g_pCfg->GetDBPwd();

	m_strCustID = g_pCfg->GetCustID();
	m_strAccount = g_pCfg->GetAccount();
	m_strCustPwd = g_pCfg->GetCustPwd();
	m_strOpId = g_pCfg->GetOpId();
	m_strOpPwd = g_pCfg->GetOpPwd();
	m_strBranch = g_pCfg->GetBranch();
	m_nLogLevel = g_pCfg->GetLogLevel();

	return TRUE;
}

BOOL CPageCfg::SetCfg()
{
	// 将参数设为不可编辑
	m_ctrlKcxpIp.EnableWindow(FALSE);
	m_ctrlKcxpPort.EnableWindow(FALSE);
	m_ctrlKcxpSendQ.EnableWindow(FALSE);
	m_ctrlKcxpRecvQ.EnableWindow(FALSE);

	m_ctrlMidIp.EnableWindow(FALSE);
	m_ctrlMidPort.EnableWindow(FALSE);

	m_ctrlLogPath.EnableWindow(FALSE);

	m_ctrlUserMid.EnableWindow(FALSE);
	m_ctrlUseKcxp.EnableWindow(FALSE);

	m_ctrlDBConStr.EnableWindow(FALSE);
	m_ctrlDBUser.EnableWindow(FALSE);
	m_ctrlDBPwd.EnableWindow(FALSE);

	m_ctrlCustID.EnableWindow(FALSE);
	m_ctrlAccount.EnableWindow(FALSE);
	m_ctrlCustPwd.EnableWindow(FALSE);

	m_ctrlOpId.EnableWindow(FALSE);
	m_ctrlOpPwd.EnableWindow(FALSE);
	m_ctrlBranch.EnableWindow(FALSE);

	// 保存配置
	g_pCfg->SetKcxpIp(m_strKcxpIp);
	g_pCfg->SetKcxpPort(m_strKcxpPort);
	g_pCfg->SetKcxpSendQ(m_strKcxpSendQ);
	g_pCfg->SetKcxpRecvQ(m_strKcxpRecvQ);

	g_pCfg->SetMidIp(m_strMidIp);
	g_pCfg->SetMidPort(m_strMidPort);

	g_pCfg->SetLogPath(m_strLogPath);
	g_pCfg->SetTestMode(m_nTestMode);
	g_pCfg->SetRefreshDBGap(m_nRefreshDBGap);

	g_pCfg->SetDBConnStr(m_strDBConnStr);
	g_pCfg->SetDBUser(m_strDBUser);
	g_pCfg->SetDBPwd(m_strDBPwd);

	g_pCfg->SetCustID(m_strCustID);
	g_pCfg->SetAccount(m_strAccount);
	g_pCfg->SetCustPwd(m_strCustPwd);
	g_pCfg->SetOpId(m_strOpId);
	g_pCfg->SetOpPwd(m_strOpPwd);
	g_pCfg->SetBranch(m_strBranch);
	g_pCfg->SetLogLevel(m_nLogLevel);

	g_pCfg->SetCfg();

	return TRUE;
}