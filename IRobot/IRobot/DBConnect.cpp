// DBConnect.cpp: implementation of the CDBConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBConnect.h"
#include "Cfg.h"
#include "loginterface.h"

extern CCfg *g_pCfg;
extern CLoginterface *g_pLog;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBConnect::CDBConnect()
{
	m_pConnection.CreateInstance(__uuidof(Connection));
    m_pRecordset.CreateInstance(__uuidof(Recordset));
}

CDBConnect::~CDBConnect()
{
	if (m_pRecordset)
	{
      if (m_pRecordset->State == adStateOpen)
         m_pRecordset->Close();
	}

	if (m_pConnection)
	{
      if (m_pConnection->GetState() == adStateOpen)
         m_pConnection->Close();
	}
}

BOOL CDBConnect::init()
{
	strcpy_s(m_szConnStr, g_pCfg->GetDBConnStr().GetBuffer());
	strcpy_s(m_szUser, g_pCfg->GetDBUser().GetBuffer());
	strcpy_s(m_szPwd, g_pCfg->GetDBPwd());

	try
	{
		//没有配置oracle客户端情况下ado连接字符串
		//CString  m_sConn="Provider=OraOLEDB.Oracle.1;Password=platform1234;Persist Security Info=True;User ID=platform;Data Source=\"(DESCRIPTION =(ADDRESS_LIST =(ADDRESS = (PROTOCOL = TCP)(HOST = 服务器地址)(PORT = 1521)) )(CONNECT_DATA = (SID = 数据库实例名)))\"";
		
		//配置好了oracle客户端 情况下ado连接字符串
		CString m_sConn;
		m_sConn.Format("Provider=OraOLEDB.Oracle.1;Persist Security Info=True;server=serveraddress;Data Source=%s;User ID=%s;Password=%s",
			m_szConnStr, m_szUser, m_szPwd);
		
		m_pConnection->Open((_bstr_t)m_sConn,"","",adConnectUnspecified);
		
		//下面是查询数据的例子
		CString strSql="select * from  customers where cust_code = 10007887";
		BSTR bstrSQL = strSql.AllocSysString();
		m_pRecordset->Open(bstrSQL, (IDispatch*)m_pConnection, adOpenDynamic, adLockOptimistic, adCmdText); 
		
		_variant_t TheValue; //VARIANT数据类型
		char szTmp[100] = {0};

		while(!m_pRecordset->adoEOF)
		{				
			TheValue = m_pRecordset->GetCollect("CUST_CODE");
			if(TheValue.vt!=VT_NULL)
			{				
				strncpy_s(szTmp, (char*)_bstr_t(TheValue), 100);
			}
			m_pRecordset->MoveNext();
		}

		m_pRecordset->Close();
	}
	catch (_com_error e)//异常处理
	{
		AfxMessageBox(e.Description());
		long errorCode=e.WCode();
		if(3127==errorCode) AfxMessageBox("表不存在");
		if(3092==errorCode) AfxMessageBox("表已经存在");
		return FALSE;
	}

	g_pLog->WriteRunLog(SYS_MODE, LOG_DEBUG, "初始化DBConn成功! ConnStr:%s, User:%s, Pwd:%s", m_szConnStr, m_szUser, m_szPwd);
	return TRUE;
}