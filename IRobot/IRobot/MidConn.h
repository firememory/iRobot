#pragma once

#include "KDGateway.h"

class CMidConn
{
public:
	CMidConn(void);
	~CMidConn(void);

	BOOL Init();

private:
	CKDGateway *m_pKDGateWay;

};
