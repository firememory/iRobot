#pragma once

#include "PageCfg.h"
#include "PageFuncTest.h"
#include "PageStressTest.h"

// CMainSheet

class CMainSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMainSheet)

public:
	CMainSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CMainSheet();
	
	CPageCfg m_pageCfg;
	CPageFuncTest m_pageFuncTest;
	CPageStressTest m_pageStressTest;

protected:
	DECLARE_MESSAGE_MAP()
};


