
// IRobotDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"

// CIRobotDlg 对话框
class CIRobotDlg : public CDialog
{
// 构造
public:
	CIRobotDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CIRobotDlg();

// 对话框数据
	enum { IDD = IDD_IROBOT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	BOOL ReadCfg();

	BOOL SetCfg();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonClean();

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
	CButton m_ctrlButtonOk;

	CEdit m_ctrlRefreshDbGap; // 刷新数据库数据等待时长

	CEdit m_ctrlTotalCaseNum;	// 测试用例总数
	CEdit m_ctrlSuccCaseNum;	// 测试用例执行成功数
	CEdit m_ctrlFailCaseNum;	// 测试用例执行失败数

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

	int m_nTotalCaseNum;
	int m_nSuccCaseNum;
	int m_nFailCaseNum;

	int m_nRefreshDBGap;	
	int m_nLogLevel;

	BOOL m_bAllowSetCfg;


public:
	afx_msg void OnBnClickedUseMid();
	afx_msg void OnBnClickedUseKcxp();
	afx_msg void OnBnClickedSetCfg();
	afx_msg void OnCbnSelchangeComboboxex1();

	void SetCtrlTotalCaseNum(int);
	void SetCtrlSuccCaseNum(int);
	void SetCtrlFailCaseNum(int);

	void InitComboxLogLevel();
	
public:	
	CProgressCtrl m_ctrlWait;
	CEdit m_ctrlLogMsg;		

	int GetRefreshDBGap() const { return m_nRefreshDBGap; }
	void SetRefreshDBGap(int val) { m_nRefreshDBGap = val; }
	afx_msg void OnBnClickedCancel();
};
