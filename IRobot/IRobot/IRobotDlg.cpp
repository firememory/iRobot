
// IRobotDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IRobot.h"
#include "IRobotDlg.h"
#include "Cfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/************************************************************************/
/* ȫ�ֱ���                                                             */
/************************************************************************/
CCfg *g_pCfg = NULL;

enum TEST_MODE
{
	USE_MID = 0,
	USE_KCXP
};

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
	, m_strKcxpSendQ(_T(""))
	, m_strKcxpRecvQ(_T(""))
	, m_strMidIp(_T(""))	
	, m_strMidPort(_T(""))
	, m_strLogPath(_T(""))
{
	m_nTestMode = USE_MID;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KCXP_IP, m_ctrlKcxpIp);
	DDX_Control(pDX, IDC_KCXP_SENDQ, m_ctrlKcxpSendQ);
	DDX_Control(pDX, IDC_KCXP_RECVQ, m_ctrlKcxpRecvQ);
	DDX_Control(pDX, IDC_MID_IP, m_ctrlMidIp);
	DDX_Control(pDX, IDC_MID_PORT, m_ctrlMidPort);
	DDX_Control(pDX, IDC_LOG_PATH, m_ctrlLogPath);

	DDX_Text(pDX, IDC_KCXP_IP, m_strKcxpIp);
	DDX_Text(pDX, IDC_KCXP_SENDQ, m_strKcxpSendQ);
	DDX_Text(pDX, IDC_KCXP_RECVQ, m_strKcxpRecvQ);
	DDX_Text(pDX, IDC_MID_IP, m_strMidIp);
	DDX_Text(pDX, IDC_MID_PORT, m_strMidPort);
	DDX_Text(pDX, IDC_LOG_PATH, m_strLogPath);
	DDX_Control(pDX, IDC_USE_MID, m_ctrlUserMid);
	DDX_Control(pDX, IDC_USE_KCXP, m_ctrlUseKcxp);
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
	g_pCfg = new CCfg;
	if (NULL == g_pCfg)
	{
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
	
	// ��ȡ���õĲ���
	UpdateData(TRUE);

	// ��������Ϊ���ɱ༭
	m_ctrlKcxpIp.EnableWindow(FALSE);
	m_ctrlKcxpSendQ.EnableWindow(FALSE);
	m_ctrlKcxpRecvQ.EnableWindow(FALSE);

	m_ctrlMidIp.EnableWindow(FALSE);
	m_ctrlMidPort.EnableWindow(FALSE);

	m_ctrlLogPath.EnableWindow(FALSE);

	m_ctrlUserMid.EnableWindow(FALSE);
	m_ctrlUseKcxp.EnableWindow(FALSE);

	// ��������
	g_pCfg->SetKcxpIp(m_strKcxpIp);
	g_pCfg->SetKcxpSendQ(m_strKcxpSendQ);
	g_pCfg->SetKcxpRecvQ(m_strKcxpRecvQ);

	g_pCfg->SetMidIp(m_strMidIp);
	g_pCfg->SetMidPort(m_strMidPort);

	g_pCfg->SetLogPath(m_strLogPath);
	g_pCfg->SetTestMode(m_nTestMode);

	g_pCfg->SetCfg();

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
