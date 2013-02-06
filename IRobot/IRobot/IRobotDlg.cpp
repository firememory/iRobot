
// IRobotDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IRobot.h"
#include "IRobotDlg.h"
#include "public.h"

#include "loginterface.h"
#include "Cfg.h"
#include "KcxpConn.h"
#include "MidConn.h"
#include "DBConnect.h"
#include "MyService.h"

#include "ParseKcbpLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/************************************************************************/
/* ȫ�ֱ���                                                             */
/************************************************************************/
CCfg *g_pCfg = NULL;
CLoginterface *g_pLog = NULL;
CKcxpConn *g_pKcxpConn = NULL;
CMidConn *g_pMidConn = NULL;
CDBConnect *g_pDBConn = NULL;
CMyService *g_pMyService = NULL;


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CIRobotDlg �Ի���
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
	, m_strCustID(_T(""))
	, m_strAccount(_T(""))
	, m_strCustPwd(_T(""))
	, m_strOpId(_T(""))
	, m_strOpPwd(_T(""))
	, m_strBranch(_T(""))
	, m_nTotalCaseNum(0)
	, m_nSuccCaseNum(0)
	, m_nFailCaseNum(0)
	, m_nLogLevel(0)
	, m_nRefreshDBGap(1000)
{
	m_nTestMode = USE_MID;
	m_bAllowSetCfg = TRUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CIRobotDlg::~CIRobotDlg()
{	
	DELCLS(g_pCfg);
	DELCLS(g_pLog);
	DELCLS(g_pMyService);
	DELCLS(g_pMidConn);
	DELCLS(g_pDBConn);
	DELCLS(g_pKcxpConn);
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

	DDX_Control(pDX, IDC_AGENT_CUSTID, m_ctrlCustID);
	DDX_Control(pDX, IDC_AGENT_ACCOUNT, m_ctrlAccount);
	DDX_Control(pDX, IDC_AGENT_CUSTPWD, m_ctrlCustPwd);
	DDX_Control(pDX, IDC_AGENT_OPID, m_ctrlOpId);
	DDX_Control(pDX, IDC_AGENT_OPPWD, m_ctrlOpPwd);
	DDX_Control(pDX, IDC_AGENT_BRANCH, m_ctrlBranch);

	DDX_Control(pDX, IDC_TOTAL_CASE, m_ctrlTotalCaseNum);
	DDX_Control(pDX, IDC_SUCC_CASE, m_ctrlSuccCaseNum);
	DDX_Control(pDX, IDC_FAIL_CASE, m_ctrlFailCaseNum);
	DDX_Control(pDX, IDC_COMBOBOXEX_LOG_LEVEL, m_ctrlLogLevel);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlWait);
	DDX_Control(pDX, IDC_EDIT_LOG_MSG, m_ctrlLogMsg);
	DDX_Control(pDX, IDOK, m_ctrlButtonOk);
	DDX_Control(pDX, IDC_EDIT_DB_GETDATA_GAP, m_ctrlRefreshDbGap);

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

	DDX_Text(pDX, IDC_TOTAL_CASE, m_nTotalCaseNum);
	DDX_Text(pDX, IDC_SUCC_CASE, m_nSuccCaseNum);
	DDX_Text(pDX, IDC_FAIL_CASE, m_nFailCaseNum);

	DDX_Text(pDX, IDC_EDIT_DB_GETDATA_GAP, m_nRefreshDBGap);
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
	ON_BN_CLICKED(IDC_SET_CFG, &CIRobotDlg::OnBnClickedSetCfg)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_LOG_LEVEL, &CIRobotDlg::OnCbnSelchangeComboboxex1)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CIRobotDlg::OnBnClickedButtonClean)
	ON_BN_CLICKED(IDCANCEL, &CIRobotDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_PERFORMANCE, &CIRobotDlg::OnBnClickedButtonPerformance)
END_MESSAGE_MAP()


// CIRobotDlg ��Ϣ�������

BOOL CIRobotDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	// ��ʼ�� Progress bar
	m_ctrlWait.SetRange(0, 3);
	m_ctrlWait.SetStep(1);

	g_pCfg = new CCfg;
	if (NULL == g_pCfg)
	{
		AfxMessageBox("����������ʧ��!", MB_OK, 0);
		return FALSE;
	}
	
	ReadCfg();

	g_pLog = new CLoginterface(m_strLogPath.GetBuffer());
	if (NULL == g_pLog)
	{
		AfxMessageBox("������־��ʧ��!", MB_OK, 0);
		return FALSE;
	}
	else
	{		
		g_pLog->SetDlg(this);
		g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "========Server Start========");
	}

	// ������g_pLog�������ٵ���������
	InitComboxLogLevel();
	UpdateData(FALSE);

	g_pKcxpConn = new CKcxpConn;
	if (NULL == g_pKcxpConn)
	{
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "����KCXP������ʧ��!");
		return FALSE;
	}
	
	g_pMidConn = new CMidConn;
	if (NULL == g_pMidConn)
	{
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "����MID������ʧ��!");
		return FALSE;
	}

	g_pDBConn = new CDBConnect;
	if (NULL == g_pDBConn)
	{
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "����DB������ʧ��!");
		return FALSE;
	}

	g_pMyService = new CMyService;
	if (NULL == g_pMyService)
	{
		g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "����MyService��ʧ��!");
		return FALSE;
	}
	else
	{
		g_pMyService->SetDlg(this);
		g_pMyService->Init();
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CIRobotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CIRobotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIRobotDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	g_pMyService->Run();
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

BOOL CIRobotDlg::ReadCfg()
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

	return TRUE;
}

BOOL CIRobotDlg::SetCfg()
{
	// ��������Ϊ���ɱ༭
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

	// ��������
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

	g_pCfg->SetCfg();

	if (!g_pMidConn->Init())
	{
		return FALSE;
	}
	
	if (!g_pDBConn->init())
	{
		return FALSE;
	}
	
	return TRUE;
}

void CIRobotDlg::OnBnClickedSetCfg()
{
	// TODO: Add your control notification handler code here
	if (FALSE == m_bAllowSetCfg)
	{
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

		m_bAllowSetCfg = TRUE;

		g_pMidConn->DisConnect();
	}
	else
	{
		// ��ȡ���õĲ���
		UpdateData(TRUE);

		// �������õ������ļ�
		m_bAllowSetCfg = FALSE;
		SetCfg();

		// ��ʼ��KCXP����
		if (g_pKcxpConn->InitKcxp())
		{
			if (g_pKcxpConn->OpLogin())
			{
				g_pLog->WriteRunLog(SYS_MODE, LOG_DEBUG, "KCXP���ӳɹ�!");
			}
			else
			{
				g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "KCXP����ʧ��!");
				if (m_nTestMode == KCXP_MODE)
				{
					m_ctrlButtonOk.EnableWindow(FALSE);
				}
			}
		}		

		// ��ʼ��MID����
		if (g_pMidConn->Connect())
		{
			g_pLog->WriteRunLog(SYS_MODE, LOG_DEBUG, "MID���ӳɹ�!");
		}
		else
		{
			g_pLog->WriteRunLog(SYS_MODE, LOG_WARN, "MID����ʧ��!");
			if (m_nTestMode == MID_MODE)
			{
				m_ctrlButtonOk.EnableWindow(FALSE);
			}
		}		
	}
}

void CIRobotDlg::SetCtrlTotalCaseNum( int nCnt)
{
	CString strTmp;
	strTmp.Format("%d", nCnt);
	m_ctrlTotalCaseNum.SetWindowText(strTmp.GetBuffer());
}

void CIRobotDlg::SetCtrlSuccCaseNum( int nCnt)
{
	CString strTmp;
	strTmp.Format("%d", nCnt);
	m_ctrlSuccCaseNum.SetWindowText(strTmp.GetBuffer());
}

void CIRobotDlg::SetCtrlFailCaseNum( int nCnt)
{
	CString strTmp;
	strTmp.Format("%d", nCnt);
	m_ctrlFailCaseNum.SetWindowText(strTmp.GetBuffer());
}

void CIRobotDlg::OnCbnSelchangeComboboxex1()
{
	// TODO: Add your control notification handler code here
	int nSel = m_ctrlLogLevel.GetCurSel();
	
	g_pLog->SetLogLevel(nSel);
}

void CIRobotDlg::InitComboxLogLevel()
{
	COMBOBOXEXITEM   Item;
	for(int i=0; i<MAX_LOG_LEVEL; i++)
	{
		Item.mask = CBEIF_TEXT;
		Item.pszText   =  g_pLog->m_stLogLevelDescription[i].description;
		Item.iItem   =  g_pLog->m_stLogLevelDescription[i].level;  
		m_ctrlLogLevel.InsertItem(&Item);
	}

	m_ctrlLogLevel.SetCurSel(LOG_NOTIFY);
}

void CIRobotDlg::OnBnClickedButtonClean()
{
	// TODO: Add your control notification handler code here
	m_ctrlLogMsg.SetWindowText("");	
}

void CIRobotDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "========Server Shutdown========");
	OnCancel();
}

void CIRobotDlg::OnBnClickedButtonPerformance()
{
	// TODO: Add your control notification handler code here
	CParseKcbpLog parslog;
	parslog.ParseLog();
	parslog.ReadRlt();
	parslog.Exec();
}
