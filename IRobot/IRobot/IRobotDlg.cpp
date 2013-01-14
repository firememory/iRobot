
// IRobotDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IRobot.h"
#include "IRobotDlg.h"

#include "loginterface.h"
#include "Cfg.h"
#include "KcxpConn.h"
#include "MidConn.h"
#include "DBConnect.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/************************************************************************/
/* 全局变量                                                             */
/************************************************************************/
CCfg *g_pCfg = NULL;
CLoginterface *g_pLog = NULL;
CKcxpConn *g_pKcxpConn = NULL;
CMidConn *g_pMidConn = NULL;
CDBConnect *g_pDBConn = NULL;

enum TEST_MODE
{
	USE_MID = 0,
	USE_KCXP
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CIRobotDlg 对话框




CIRobotDlg::CIRobotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIRobotDlg::IDD, pParent)
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
{
	m_nTestMode = USE_MID;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
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
}

BEGIN_MESSAGE_MAP(CIRobotDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CIRobotDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &CIRobotDlg::OnNMCustomdrawProgress1)
	ON_BN_CLICKED(IDC_USE_MID, &CIRobotDlg::OnBnClickedUseMid)
	ON_BN_CLICKED(IDC_USE_KCXP, &CIRobotDlg::OnBnClickedUseKcxp)
END_MESSAGE_MAP()


// CIRobotDlg 消息处理程序

BOOL CIRobotDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	g_pCfg = new CCfg;
	if (NULL == g_pCfg)
	{
		AfxMessageBox("创建配置类失败!", MB_OK, 0);
		return FALSE;
	}
	
	g_pCfg->ReadCfg();

	m_strKcxpIp = g_pCfg->GetKcxpIp();
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
	
	UpdateData(FALSE);

	g_pLog = new CLoginterface(m_strLogPath.GetBuffer());
	if (NULL == g_pLog)
	{
		AfxMessageBox("创建日志类失败!", MB_OK, 0);
		return FALSE;
	}

	g_pKcxpConn = new CKcxpConn;
	if (NULL == g_pKcxpConn)
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_EMERGENT, "创建KCXP连接类失败!");
		return FALSE;
	}
	
	g_pMidConn = new CMidConn;
	if (NULL == g_pMidConn)
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_EMERGENT, "创建MID连接类失败!");
		return FALSE;
	}
	else
	{
		if (!g_pMidConn->Init())
		{
			return FALSE;
		}
	}

	g_pDBConn = new CDBConnect;
	if (NULL == g_pDBConn)
	{
		g_pLog->WriteRunLog(__FILE__, __LINE__, LOG_EMERGENT, "创建DB连接类失败!");
		return FALSE;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIRobotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIRobotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIRobotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIRobotDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	// 获取设置的参数
	UpdateData(TRUE);

	// 将参数设为不可编辑
	m_ctrlKcxpIp.EnableWindow(FALSE);
	m_ctrlKcxpSendQ.EnableWindow(FALSE);
	m_ctrlKcxpRecvQ.EnableWindow(FALSE);

	m_ctrlMidIp.EnableWindow(FALSE);
	m_ctrlMidPort.EnableWindow(FALSE);

	m_ctrlLogPath.EnableWindow(FALSE);

	m_ctrlUserMid.EnableWindow(FALSE);
	m_ctrlUseKcxp.EnableWindow(FALSE);

	// 保存配置
	g_pCfg->SetKcxpIp(m_strKcxpIp);
	g_pCfg->SetKcxpSendQ(m_strKcxpSendQ);
	g_pCfg->SetKcxpRecvQ(m_strKcxpRecvQ);

	g_pCfg->SetMidIp(m_strMidIp);
	g_pCfg->SetMidPort(m_strMidPort);

	g_pCfg->SetLogPath(m_strLogPath);
	g_pCfg->SetTestMode(m_nTestMode);

	g_pCfg->SetCfg();

	g_pKcxpConn->InitKcxp();
}

void CIRobotDlg::OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CIRobotDlg::OnBnClickedUseMid()
{
	// TODO: Add your control notification handler code here
	m_nTestMode = USE_MID;
}

void CIRobotDlg::OnBnClickedUseKcxp()
{
	// TODO: Add your control notification handler code here
	m_nTestMode = USE_KCXP;
}
