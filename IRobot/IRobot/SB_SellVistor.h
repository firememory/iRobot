#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* ��������                                                             */
/************************************************************************/
class CSB_SellVistor
	: public IPCKDGateWayVistor
{
public:
	CSB_SellVistor(void);
	virtual ~CSB_SellVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);	

	// ��ִ��ǰ����ȡһ��������ݣ���ִ�к����ݽ��бȽ�
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetMatchedData();

	// ��KCXP������Ϣ
	BOOL SendKcxpMsg(char *);

	// ��Mid������Ϣ
	BOOL SendMidMsg(char *);

	BOOL TestCase_1(); 

	BOOL ChkData();

	// ����ɷ�
	BOOL SaveShares();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;
	int m_nRowNum;

	char m_szSecu_Intl[DEFAULT_LEN];		// ֤ȯ����
	char m_szSecu_Code[DEFAULT_LEN];		// ֤ȯ����
	char m_szPrice[DEFAULT_LEN];			// ί�м۸�
	char m_szQty[DEFAULT_LEN];				// ί������

	char m_szMarket_Board[3];
	char m_szTrdId[3];
	char m_szCurrency[2]; // ����

	// �������
	float m_fMatched_OrderFrzAmt;	// �ɽ��Ľ��׶�����
	float m_fMatched_Price;	// �ɽ��۸�
	int m_nMatched_Qty;	// �ɽ�����
	int m_nOrder_Qty; // ί������
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
