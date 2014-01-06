#pragma once
#include "afxwin.h"
#include "KcxpConn.h"
#include "MidConn.h"

// CPageInterTest dialog

class CPageInterTest : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageInterTest)

public:
	CPageInterTest();
	virtual ~CPageInterTest();

// Dialog Data
	enum { IDD = IDD_PAGEINTERFACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CButton m_ctrlKcxp;
	CButton m_ctrlMid;	
	CEdit m_ctrlCommand;
	CString m_strCommand;	

	int m_nTestMode;

	CKDGateway *m_pKDGateWay; // MID连接指针
	CKDMidCli *m_pKcxpConn; // KXCP连接指针

	// MID返回值
	int m_nRowNum;
	int m_nFieldNum;

public:
	afx_msg void OnBnClickedButtonInvoke();
	afx_msg void OnBnClickedKcxp();
	afx_msg void OnBnClickedMid();
	
	BOOL CPageInterTest::SendMidMsg(char *pCmd);
	BOOL CPageInterTest::SendKcxpMsg( char *pCmd );

};
