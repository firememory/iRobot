#pragma once

#include "IPCKDGateWayVistor.h"

struct ETF_SECU_SHARES
{
	int nShareBln;			// �ɷ����
	int nShareAvl;			// �ɷݿ���
	int nShareTrdFrz;		// ���׶���
	int nShareOtd;			// ��;����
	int nSHareOtd_Avl;		// ��;����
};

// ETF������Ϣ
struct ETF_INFO
{
	char szEtfIntl[11];				// ETF����
	int nCreationRedemptionUnit;	// ÿ�����ӣ���С�깺����ص�λ����Ӧ��ETF����
	float fMaxCashRatio;			// �ܵ��ֽ��������
	int nRecordNum;					// �ɷ�֤ȯ����Ŀ
	float fEstimateCashComponent;	// T��ÿ�����ӵ�Ԥ���ֽ���
	float fCashComponent;			// ǰһ���ֽ���
	float fNavPerCu;				// ǰһ����С���굥λ��ֵ
	float fNav;						// ǰһ�ջ���ݶֵ
};

// ETF�ɷݹ���Ϣ
struct ETF_SECU_INFO
{
	char szSecuIntl[11];	// �ɷݹ�����
	int nShareQty;			// �ɷݹ��������
	int nInsteadFlag;		// �Ƿ���ֽ���� 1:���� 2:��
	float fPrice_Ratio;		// ��۱���
	ETF_SECU_SHARES shares_old;
	ETF_SECU_SHARES shares_new;
};

/************************************************************************/
/* ��ETFҵ��                                                         */
/************************************************************************/
class CSH_ETFVistor
	: public IPCKDGateWayVistor
{
public:
	CSH_ETFVistor(void);
	virtual ~CSH_ETFVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);
	virtual void ParseKcxpRetMsg();

	// ��ִ��ǰ����ȡһ��������ݣ���ִ�к����ݽ��бȽ�
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetMatchedData();

	BOOL GetEtfInfo(char *pEtfIntl);
	BOOL GetEtfSecuInfo(char *pEtfIntl);
	BOOL GetEtfSecuShares(int nType);

	BOOL TestCase_1(char *pEtf_Code, char* pTrd_Id); 

	BOOL ChkData();

	BOOL SaveCapital();
	BOOL SaveShares();
	BOOL SaveEtfSecuShares();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;
		
	ETF_INFO *m_pEtfInfo;			// ETF������Ϣ
	ETF_SECU_INFO *m_pEtfSecuInfo;	// ETF �ɷݹ���Ϣ
	int m_nEtfSecuCnt;				// ETF �ɷݹ�����

	// �������
	float m_fMatched_OrderFrzAmt;	// �ɽ��Ľ��׶�����
	float m_fMatched_Price;	// �ɽ��۸�
	int m_nOrder_Qty; // ί������
	int m_nMatched_Qty;	// �ɽ�����
	float m_fMatchedAmt;	// �ɽ����
	float m_fMatched_SettAmt; // ������

	int m_nShareBln_Old;			// �ɷ����
	int m_nShareAvl_Old;			// �ɷݿ���
	int m_nShareTrdFrz_Old;			// ���׶���
	int m_nShareOtd_Old;			// ��;����
	int m_nSHareOtd_Avl_Old;		// ��;����

	float m_fCptlBln_Old;			// �ʽ����
	float m_fCptlAvl_Old;			// �ʽ����
	float m_fCptlTrdFrz_Old;		// ���׶���
	float m_fCptlOutstanding_Old;	// ��;�ʽ�
	float m_fCptlOtdAvl_Old;		// ��;����

	int m_nShareBln_New;			// �ɷ����
	int m_nShareAvl_New;			// �ɷݿ���
	int m_nShareTrdFrz_New;			// ���׶���
	int m_nShareOtd_New;			// ��;����
	int m_nSHareOtd_Avl_New;		// ��;����

	float m_fCptlBln_New;			// �ʽ����
	float m_fCptlAvl_New;			// �ʽ����
	float m_fCptlTrdFrz_New;		// ���׶���
	float m_fCptlOutstanding_New;	// ��;�ʽ�
	float m_fCptlOtdAvl_New;		// ��;����
};
