
// IRobot.cpp : 定义应用程序的类行为。
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
CMyService *g_pMyService = NULL;


// CIRobotApp

BEGIN_MESSAGE_MAP(CIRobotApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CIRobotApp 构造

CIRobotApp::CIRobotApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CIRobotApp 对象

CIRobotApp theApp;


// CIRobotApp 初始化

BOOL CIRobotApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	g_pCfg = new CCfg;
	if (NULL == g_pCfg)
	{
		AfxMessageBox("创建配置类失败!", MB_OK, 0);
		return FALSE;
	}

	if (g_pCfg->ReadCfg() == FALSE)
	{
		AfxMessageBox("读取配置类失败!", MB_OK, 0);
		return FALSE;
	}	

	g_pLog = new CLoginterface(g_pCfg->GetLogPath().GetBuffer());
	if (NULL == g_pLog)
	{
		AfxMessageBox("创建日志类失败!", MB_OK, 0);
		return FALSE;
	}

	g_pKcxpConn = new CKcxpConn;
	if (NULL == g_pKcxpConn)
	{
		AfxMessageBox("创建KCXP连接类失败!", MB_OK, 0);
		return FALSE;
	}

	g_pMidConn = new CMidConn;
	if (NULL == g_pMidConn)
	{
		AfxMessageBox("创建MID连接类失败!", MB_OK, 0);
		return FALSE;
	}

	g_pDBConn = new CDBConnect;
	if (NULL == g_pDBConn)
	{
		AfxMessageBox("创建DB连接类失败!", MB_OK, 0);		
		return FALSE;
	}

	CIRobotDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

CIRobotApp::~CIRobotApp()
{
	g_pLog->WriteRunLog(SYS_MODE, LOG_NOTIFY, "========Server Shutdown========");
	DELCLS(g_pCfg);
	DELCLS(g_pLog);
	DELCLS(g_pMyService);
	DELCLS(g_pMidConn);
	DELCLS(g_pDBConn);
	DELCLS(g_pKcxpConn);	
}
