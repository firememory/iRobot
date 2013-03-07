#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* ����ҵ��                                                             */
/************************************************************************/
class CFundVistor
	: public IPCKDGateWayVistor
{
public:
	CFundVistor(void);
	virtual ~CFundVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual void ParseKcxpRetMsg();

	// ��ִ��ǰ����ȡһ��������ݣ���ִ�к����ݽ��бȽ�
	BOOL InitUserData();
	BOOL GetOrderData();

	BOOL TestCase_1(); 

	BOOL ChkData();

	BOOL SaveCapital();

	BOOL SaveShares();

private:
	MID_821_ORDER_RET_MSG *m_pMsg;

	// �������
	float m_fOrderAmt;		// ���׶�����
	float m_fOrderFrzAmt;		// ���׶�������

	int m_nShareBln_Old;			// �ɷ����
	int m_nShareAvl_Old;			// �ɷݿ���
	int m_nShareTrdFrz_Old;			// ���׶���
	int m_nShareOtd_Old;			// ��;����

	float m_fCptlBln_Old;			// �ʽ����
	float m_fCptlAvl_Old;			// �ʽ����
	float m_fCptlTrdFrz_Old;		// ���׶���
	float m_fCptlOutstanding_Old;	// ��;�ʽ�
	float m_fCptlOtdAvl_Old;		// ��;����

	int m_nShareBln_New;			// �ɷ����
	int m_nShareAvl_New;			// �ɷݿ���
	int m_nShareTrdFrz_New;			// ���׶���
	int m_nShareOtd_New;			// ��;����

	float m_fCptlBln_New;			// �ʽ����
	float m_fCptlAvl_New;			// �ʽ����
	float m_fCptlTrdFrz_New;		// ���׶���
	float m_fCptlOutstanding_New;	// ��;�ʽ�
	float m_fCptlOtdAvl_New;		// ��;����
};
