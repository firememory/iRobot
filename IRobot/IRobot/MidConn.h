#pragma once

#include "KDGateway.h"

class CMidConn
{
public:
	CMidConn(void);
	~CMidConn(void);

	BOOL Init();
	BOOL Connect();
	BOOL Login();
	void DisConnect();

	CKDGateway * GetKDGateWay() const { return m_pKDGateWay; }
	void SetKDGateWay(CKDGateway * val) { m_pKDGateWay = val; }

private:
	CKDGateway *m_pKDGateWay;
};
