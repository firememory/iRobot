
// IRobotDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"
#include "MainSheet.h"

// CIRobotDlg �Ի���
class CIRobotDlg : public CDialog
{
// ����
public:
	CIRobotDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CIRobotDlg();

	CMainSheet m_sheet;

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
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonClean();
	DECLARE_MESSAGE_MAP()

public:
	CRichEditCtrl m_ctrlLogMsg;
};
