#pragma once

#include "IPCKDGateWayVistor.h"

struct ORDER_403_RET_MSG
{
	char szBizNo[11];
	char szOrderID[11];
	char szAccount[21];
	char szPrice[21]; // ί�м۸�
	char szQty[21];		// ί������
	char szOrderAmt[21];	// ί�н��
	char szOrderFrzAmt[21];	// ���׶�����
	char szSeat[7];
	char szExtInst[11];
	char szExtAcc[33];
	char szExtSubAcc[33];
	char szExtFrzAmt[21];
};

#define DEFAULT_LEN 30

/************************************************************************/
/* ��A �м�����                                                         */
/************************************************************************/
class CSZA_SJ_BuyVistor
	: public IPCKDGateWayVistor
{
public:
	CSZA_SJ_BuyVistor(void);
	virtual ~CSZA_SJ_BuyVistor(void);

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
	ORDER_403_RET_MSG *m_pMsg;
	int m_nRowNum;

	char m_szSecu_intl[DEFAULT_LEN];		// ֤ȯ����
	char m_szPrice[DEFAULT_LEN];			// ί�м۸�
	char m_szQty[DEFAULT_LEN];				// ί������

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
