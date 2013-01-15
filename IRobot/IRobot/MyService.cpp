#include "StdAfx.h"
#include "MyService.h"

CMyService::CMyService(void)
{
}

CMyService::~CMyService(void)
{
	int nCnt = m_ptServiceArray.GetCount();
	for (int i=0; i<nCnt; i++)
	{
		delete m_ptServiceArray[i];
	}

	m_ptServiceArray.RemoveAll();
}

void CMyService::Init()
{
	RegisterProtocal(&LoginVistor);
}

void CMyService::RegisterProtocal(IPCKDGateWayVistor *pService)
{
	m_ptServiceArray.Add(pService);
}

void CMyService::Run()
{
	int nCnt = m_ptServiceArray.GetCount();
	for (int i=0; i<nCnt; i++)
	{
		m_ptServiceArray[i]->Vistor();
	}
}
