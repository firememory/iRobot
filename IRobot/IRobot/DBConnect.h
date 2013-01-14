// DBConnect.h: interface for the CDBConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBCONNECT_H__EC570EB4_4670_4371_89B3_501DCEDF30F9__INCLUDED_)
#define AFX_DBCONNECT_H__EC570EB4_4670_4371_89B3_501DCEDF30F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDBConnect  
{
public:
	CDBConnect();
	virtual ~CDBConnect();

	void init(char*, char*, char*);

public:
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;

};

#endif // !defined(AFX_DBCONNECT_H__EC570EB4_4670_4371_89B3_501DCEDF30F9__INCLUDED_)
