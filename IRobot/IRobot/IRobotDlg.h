
// IRobotDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"
#include "MainSheet.h"

// CIRobotDlg 对话框
class CIRobotDlg : public CDialog
{
// 构造
public:
	CIRobotDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CIRobotDlg();

	CMainSheet m_sheet;

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
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonClean();
	DECLARE_MESSAGE_MAP()

public:
	CRichEditCtrl m_ctrlLogMsg;
};
