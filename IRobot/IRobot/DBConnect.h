// DBConnect.h: interface for the CDBConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBCONNECT_H__EC570EB4_4670_4371_89B3_501DCEDF30F9__INCLUDED_)
#define AFX_DBCONNECT_H__EC570EB4_4670_4371_89B3_501DCEDF30F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DB_COMPARE(x, y)\
{\
	TheValue = pRecordSet->GetCollect(x);\
	if (TheValue.vt!=VT_NULL)\
	{\
	strncpy_s(szTmp, (char*)_bstr_t(TheValue), 100);\
	float fDst = (float)atof(szTmp);\
	float fSrc = (float)atof(m_pMsg[i].y);\
	if (fSrc != fDst)\
		{\
		g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "%s 查询不正确! mid:%f, db:%f", x, fSrc, fDst);\
		bRet = FALSE;\
		}\
	}\
	else\
	{\
	g_pLog->WriteRunLog(MID_MODE, LOG_WARN, "%s DB无法访问!", x);\
	bRet = FALSE;\
	}\
}

#define DB_GET_VALUE_STRING(x, y)\
{\
	TheValue = pRecordSet->GetCollect(x);\
	if(TheValue.vt!=VT_NULL)\
	{\
	strcpy_s(y, (char*)_bstr_t(TheValue));\
	}\
}

#define DB_GET_VALUE_FLOAT(x, y)\
{\
	TheValue = pRecordSet->GetCollect(x);\
	if(TheValue.vt!=VT_NULL)\
	{\
	y = (float)atof((char*)_bstr_t(TheValue));\
	}\
}

#define DB_GET_VALUE_INT(x, y)\
{\
	TheValue = pRecordSet->GetCollect(x);\
	if(TheValue.vt!=VT_NULL)\
	{\
	y = atoi((char*)_bstr_t(TheValue));\
	}\
}

class CDBConnect  
{
public:
	CDBConnect();
	virtual ~CDBConnect();

	BOOL init();
	void Disconnect();

	BOOL QueryData(BSTR, _RecordsetPtr &ptr);

public:
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;

private:
	char m_szConnStr[100];
	char m_szUser[20];
	char m_szPwd[20];

	CRITICAL_SECTION m_caQueryData;
};

#endif // !defined(AFX_DBCONNECT_H__EC570EB4_4670_4371_89B3_501DCEDF30F9__INCLUDED_)
