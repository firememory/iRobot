
// IRobotDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"

// CIRobotDlg �Ի���
class CIRobotDlg : public CDialog
{
// ����
public:
	CIRobotDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CIRobotDlg();

// �Ի�������
	enum { IDD = IDD_IROBOT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	BOOL ReadCfg();

	BOOL SetCfg();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

	CEdit m_ctrlRefreshDbGap; // ˢ�����ݿ����ݵȴ�ʱ��

	CEdit m_ctrlTotalCaseNum;	// ������������
	CEdit m_ctrlSuccCaseNum;	// ��������ִ�гɹ���
	CEdit m_ctrlFailCaseNum;	// ��������ִ��ʧ����

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
