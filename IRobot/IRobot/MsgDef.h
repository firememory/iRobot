#define DEFAULT_LEN 30

// ����ί�з��ؽ��
struct MID_403_ORDER_RET_MSG
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

// ��ѯ�ֲַ��ؽ��
struct MID_504_QUERY_SHARES_RET_MSG
{
	char szUserCode[10];
	char szAccount[21];
	char szMarket[3];
	char szSecuAcc[11];
	char szSecuName[17];
	char szSecuCode[10];
	char szSecuIntl[10];
	char szSeat[10];
	char szBranch[10];
	char szExtInst[10];
	char szCurrency[2];
	char szShareBln[20];
	char szShareAvl[20];
	char szShareTrdFrz[20];
	char szShareOtd[20];
	char szShareFrz[20];
	char szShareUnTrdQty[20];
	char szCurrentCost[20];
	char szMktValOld[20];
	char szCostPrice[20];
	char szShareOtdAvl[20];
	char szCurrPrice[20];
	char szCost2Price[20];
	char szMktQty[20];
	char szMktVal[20];
};

// ��¼���ؽ����Ϣ
struct MID_409101_LOGIN_RET_MSG
{
	char szUserCode[9];
	char szMarket[3];
	char szBoard[2];
	char szSecuAcc[15];
	char szSecuAccName[20];
	char szAccount[10];
	char szUserName[20];
	char szBrh[10];
	char szMainFlag[2];
	char szSession[20];
	char szTaCode[5];
	char szCertDays[3];
	char szEtokenDays[3];
};


// ��ѯ�ͻ��ɶ���Ϣ���ؽ����Ϣ
struct MID_501_QUERY_SECU_ACC_RET_MSG
{
	char szUserCode[11];
	char szMarket[3];
	char szSecuAcc[11];
	char szSecuAccName[20];
	char szAccount[20];
	char szMainFlag[2];
	char szBindSeat[10];
	char szBindStatus[10];
	char szStatus[3];
};

// ����ί�з��ؽ��
struct MID_821_ORDER_RET_MSG
{
	char szAccount[21];
	char szOrderSn[30];
	char szAppSn[30];
	char szOrder_Amt[21];
	char szOrder_Frz_Amt[21];
	char szFee_Type[3];
	char szDiscount_Ratio[10];
};
