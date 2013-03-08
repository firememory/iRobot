#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* ��A������                                                         */
/************************************************************************/
class CSZA_BuyVistor
	: public IPCKDGateWayVistor
{
public:
	CSZA_BuyVistor(void);
	virtual ~CSZA_BuyVistor(void);

	// ���ò���
	virtual BOOL Vistor();

	// ����Mid�ӿڷ�����Ϣ
	virtual BOOL ResultStrToTable(char *);
	
	// ����KCXP�ӿڷ�����Ϣ
	virtual void ParseKcxpRetMsg();

	// ����������
	BOOL TestCase_1();  // �м�����
	BOOL TestCase_2();  // ���ַ����ż۸�ί��
	BOOL TestCase_3();	// �������м۸�ί��
	BOOL TestCase_4();	// ��ʱ�ɽ�ʣ�೷��ί�� 
	BOOL TestCase_5();	// �嵵��ʱ�ɽ�ʣ�೷��
	BOOL TestCase_6();	// ȫ��ɽ�����ί��

	// ����
	BOOL ChkData();
	BOOL InitUserData();   // ί��ǰ���ͻ��ʽ𡢹ɷݿ���
	BOOL UpdateUserData(); // �ɽ��󣬿ͻ��ʽ𡢹ɷݿ���
	BOOL GetMatchedData(); // �ɽ����

	// �ʽ�����
	BOOL SaveCapital();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;

	// �������
	float m_fMatched_OrderFrzAmt;	// �ɽ��Ľ��׶�����
	float m_fMatched_Price;	// �ɽ��۸�
	int m_nMatched_Qty;	// �ɽ�����
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
