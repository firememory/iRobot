#pragma once

#include "IPCKDGateWayVistor.h"

#define DEFAULT_LEN 30

/************************************************************************/
/* ��A �޼�����                                                         */
/************************************************************************/
class CSHA_SJ_BuyVistor
	: public IPCKDGateWayVistor
{
public:
	CSHA_SJ_BuyVistor(void);
	virtual ~CSHA_SJ_BuyVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual BOOL SendMsg(char *);

	// ��ִ��ǰ����ȡһ��������ݣ���ִ�к����ݽ��бȽ�
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetMatchedData();

	// ��KCXP������Ϣ
	BOOL SendKcxpMsg();

	// ��Mid������Ϣ
	BOOL SendMidMsg();

	BOOL ChkPnt1();
	void ChkPnt2();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;
	int m_nRowNum;

	char m_szSecu_intl[DEFAULT_LEN];		// ֤ȯ����
	char m_szPrice[DEFAULT_LEN];			// ί�м۸�
	char m_szQty[DEFAULT_LEN];				// ί������

	char m_szMarket[2];
	char m_szBoard[2];

	// �������
	float m_fMatched_OrderFrzAmt;	// �ɽ��Ľ��׶�����
	float m_fMatched_Price;	// �ɽ��۸�
	float m_fMatched_Qty;	// �ɽ�����
	float m_fMatchedAmt;	// �ɽ����
	float m_fMatched_SettAmt; // ������

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
