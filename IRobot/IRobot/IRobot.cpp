
// IRobot.cpp : ����Ӧ�ó��������Ϊ��
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
/* ȫ�ֱ���                                                             */
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


// CIRobotApp ����

CIRobotApp::CIRobotApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CIRobotApp ����

CIRobotApp theApp;


// CIRobotApp ��ʼ��

BOOL CIRobotApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	g_pCfg = new CCfg;
	if (NULL == g_pCfg)
	{
		AfxMessageBox("����������ʧ��!", MB_OK, 0);
		return FALSE;
	}

	if (g_pCfg->ReadCfg() == FALSE)
	{
		AfxMessageBox("��ȡ������ʧ��!", MB_OK, 0);
		return FALSE;
	}	

	g_pLog = new CLoginterface(g_pCfg->GetLogPath().GetBuffer());
	if (NULL == g_pLog)
	{
		AfxMessageBox("������־��ʧ��!", MB_OK, 0);
		return FALSE;
	}

	g_pKcxpConn = new CKcxpConn;
	if (NULL == g_pKcxpConn)
	{
		AfxMessageBox("����KCXP������ʧ��!", MB_OK, 0);
		return FALSE;
	}

	g_pMidConn = new CMidConn;
	if (NULL == g_pMidConn)
	{
		AfxMessageBox("����MID������ʧ��!", MB_OK, 0);
		return FALSE;
	}

	g_pDBConn = new CDBConnect;
	if (NULL == g_pDBConn)
	{
		AfxMessageBox("����DB������ʧ��!", MB_OK, 0);		
		return FALSE;
	}

	CIRobotDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
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
