// MainSheet.cpp : implementation file
//

#include "stdafx.h"
#include "IRobot.h"
#include "MainSheet.h"

// CMainSheet

IMPLEMENT_DYNAMIC(CMainSheet, CPropertySheet)

CMainSheet::CMainSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CMainSheet::CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{	
	AddPage(&m_pageCfg);
	AddPage(&m_pageFuncTest);
	AddPage(&m_pageStressTest);
}

CMainSheet::~CMainSheet()
{
}


BEGIN_MESSAGE_MAP(CMainSheet, CPropertySheet)
END_MESSAGE_MAP()


// CMainSheet message handlers