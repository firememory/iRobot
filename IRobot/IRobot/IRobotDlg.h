
// IRobotDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

// CIRobotDlg �Ի���
class CIRobotDlg : public CDialog
{
// ����
public:
	CIRobotDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IROBOT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);
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
	CButton m_ctrlUserMid;
	CButton m_ctrlUseKcxp;

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

public:
	afx_msg void OnBnClickedUseMid();
	afx_msg void OnBnClickedUseKcxp();	
	
};
