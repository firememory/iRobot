#include "StdAfx.h"
#include "Share.h"

CShare::CShare(void)
{
	m_pStocks = NULL;
}

CShare::~CShare(void)
{
}

BOOL CShare::InitShares()
{
	// 从柜台同步客户的Shares信息
	return TRUE;

}
