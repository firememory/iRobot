#pragma once

#define SERVICE_STRNCPY(member)\
{\
	strncpy_s(m_pMsg[nRow].member, q, sizeof(m_pMsg[nRow].member));\
}

#define DB_COMPARE(x, y)\
{\
	TheValue = g_pDBConn->m_pRecordset->GetCollect(x);\
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


class IPCKDGateWayVistor
{
public:
	virtual BOOL Vistor() = 0;
	virtual BOOL ResultStrToTable(char *) = 0;
	virtual BOOL SendMsg(char *) = 0;
};
