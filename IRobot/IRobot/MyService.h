#pragma once

#include "IPCKDGateWayVistor.h"
#include "LoginVistor.h"

class CMyService
{
public:
	CMyService(void);
	~CMyService(void);

	void Init();
	void RegisterProtocal(IPCKDGateWayVistor *);
	void Run();

private:
	CArray <IPCKDGateWayVistor*, IPCKDGateWayVistor*> m_ptServiceArray;

	CLoginVistor LoginVistor;
};
