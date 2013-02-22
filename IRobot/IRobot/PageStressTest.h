#pragma once
#include "afxwin.h"
#include "ParseKcbpLog.h"
#include "afxcmn.h"

// CPageStressTest dialog

DWORD WINAPI RunThread(LPVOID);

class CPageStressTest : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageStressTest)

public:
	CPageStressTest();
	virtual ~CPageStressTest();

// Dialog Data
	enum { IDD = IDD_PAGESTRESSTEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButtonPerformance();
	afx_msg void OnBnClickedPauseThread();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedButtonReadKcbpLog();
	DECLARE_MESSAGE_MAP()	

private:
	CEdit m_ctrlKcbpLogPath;
	CString m_strKcbpLogPath;
	CButton m_ctrlBtnRun;
	CButton m_ctrlBtnPause;
	CParseKcbpLog *m_pParsKcbplog;
	CEdit m_ctrlThreadNum;
	int m_nThreadNum;
	CButton m_ctrlBtnInit;
	CSliderCtrl m_ctrlSlider;
};
