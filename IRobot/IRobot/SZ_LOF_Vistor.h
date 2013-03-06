#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* ����LOF                                                         */
/************************************************************************/
class CSZLOFVistor
	: public IPCKDGateWayVistor
{
public:
	CSZLOFVistor(void);
	virtual ~CSZLOFVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual void ParseKcxpRetMsg();

	// ��ִ��ǰ����ȡһ��������ݣ���ִ�к����ݽ��бȽ�
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetOrderData();

	// ����1: �޼�����
	BOOL TestCase_1(); 

	// ����2: �м�����: �嵵��ʱ�ɽ�ʣ�೷�� VB
	BOOL TestCase_2();

	// ����3: �м�����: �嵵��ʱ�ɽ�ʣ��ת�� UB
	BOOL TestCase_3();

	BOOL ChkData();

	BOOL SaveCapital();

	BOOL SaveShares();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;

	char m_szSecu_Intl[DEFAULT_LEN];		// ֤ȯ����
	char m_szSecu_Code[DEFAULT_LEN];		// ֤ȯ����
	char m_szPrice[DEFAULT_LEN];			// ί�м۸�
	char m_szQty[DEFAULT_LEN];				// ί������

	char m_szMarket_Board[3];
	char m_szTrdId[3];
	char m_szCurrency[3];

	// �������
	float m_fOrder_RltFrzAmt;		// ���׶�����
	float m_fOrder_RltFrzQty;		// ���׶�������

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
