#pragma once
#include "resource.h"
#include "afxwin.h"

// CPageFuncTest dialog
#define WM_FUNC_PAGE_UPDATE WM_USER + 111

class CPageFuncTest : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageFuncTest)

public:
	CPageFuncTest();
	virtual ~CPageFuncTest();

// Dialog Data
	enum { IDD = IDD_PAGEFUNCTEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedButtonInit();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg LRESULT OnStatusUpdate(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CEdit m_ctrlTotalCaseNum;	// 测试用例总数
	CEdit m_ctrlSuccCaseNum;	// 测试用例执行成功数
	CEdit m_ctrlFailCaseNum;	// 测试用例执行失败数

	CButton m_ctrlBtnRun;
	CButton m_ctrlBtnConn;
	CButton m_ctrlBtnDisConn;

	int m_nTotalCaseNum;
	int m_nSuccCaseNum;
	int m_nFailCaseNum;

public:
	void SetCtrlTotalCaseNum(int);
	void SetCtrlSuccCaseNum(int);
	void SetCtrlFailCaseNum(int);

	CProgressCtrl m_ctrlWait;	
};
