#pragma once

struct STOCK_INFO
{
	char szSecuIntl[10];	// ֤ȯ����
	int nShareBln;			// �ɷ����
	int nShareAvl;			// �ɷݿ���
	int nShareTrdFrz;		// ���׶���
	int nShareOtd;			// ��;����
	int nShareFrz;			// �쳣����
	int fShareUnTrdQty;		// ����ͨ��
	float fCurrentCost;		// ����ɱ�
	float fCostPrice;		// �������
	int nMktVal;			// ��ֵ
};

// �ͻ��Ĺɷ���Ϣ
class CShare
{
public:
	CShare(void);
	~CShare(void);

	STOCK_INFO *m_pStocks;

	BOOL InitShares();

};
