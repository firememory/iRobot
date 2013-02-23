#pragma once
#include "afxwin.h"


// CPageCfg dialog

class CPageCfg : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageCfg)

public:
	CPageCfg();
	virtual ~CPageCfg();

// Dialog Data
	enum { IDD = IDD_PAGECFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	afx_msg void OnBnClickedUseMid();
	afx_msg void OnBnClickedUseKcxp();
	afx_msg void OnBnClickedGetCfg();
	afx_msg void OnBnClickedSetCfg();
	afx_msg void OnCbnSelchangeComboboxex1();

	DECLARE_MESSAGE_MAP()
private:
	CEdit m_ctrlKcxpIp;
	CEdit m_ctrlKcxpPort;
	CEdit m_ctrlKcxpSendQ;
	CEdit m_ctrlKcxpRecvQ;
	CEdit m_ctrlMidIp;
	CEdit m_ctrlMidPort;
	CEdit m_ctrlLogPath;
	CEdit m_ctrlDBConStr;
	CEdit m_ctrlDBUser;
	CEdit m_ctrlDBPwd;

	CEdit m_ctrlCustID;
	CEdit m_ctrlAccount;
	CEdit m_ctrlCustPwd;	
	CEdit m_ctrlOpId;	
	CEdit m_ctrlOpPwd;	
	CEdit m_ctrlBranch;

	CButton m_ctrlUserMid;
	CButton m_ctrlUseKcxp;

	CEdit m_ctrlRefreshDbGap; // 刷新数据库数据等待时长

	CComboBoxEx m_ctrlLogLevel;	

	CString m_strKcxpIp;
	CString m_strKcxpPort;
	CString m_strKcxpSendQ;
	CString m_strKcxpRecvQ;
	CString m_strMidIp;
	CString m_strMidPort;
	CString m_strLogPath;
	int m_nTestMode;
	CString m_strDBConnStr;
	CString m_strDBUser;
	CString m_strDBPwd;

	CString m_strCustID;
	CString m_strAccount;
	CString m_strCustPwd;
	CString m_strOpId;
	CString m_strOpPwd;
	CString m_strBranch;

	int m_nRefreshDBGap;	
	int m_nLogLevel;

public:
	
	int GetRefreshDBGap() const { return m_nRefreshDBGap; }
	void SetRefreshDBGap(int val) { m_nRefreshDBGap = val; }
	void InitComboxLogLevel();

	BOOL ReadCfg();
	BOOL SetCfg();

	void Init();
private:
	CButton m_ctrlBtnGetCfg;
	CButton m_ctrlBtnSetCfg;
};
