/***************************************************************************
CKDGateway.cpp : implementation file

  ��֤��һ����̨��Χ����ͨѶ��
  
	author: luosj 200610
	
****************************************************************************/

#include "stdafx.h"
#include "CMiniSocket.h"
#include "error.h"

#define DEF_SBUFLEN 32    //����С��������С
#define DEF_LBUFLEN_8192 8192  //����󻺳�����С

struct tagAnswerPacket //Ӧ���
{
	char szCRCCode[DEF_SBUFLEN];  //3CRCУ����	CHAR(8), ��ע5,7
	char szVersion[DEF_SBUFLEN];  //4�汾��	��ǰЭ��汾���(�̶�����KDGATEWAY1.0��)
	char szRetCode[DEF_SBUFLEN];  //5������	VARCHAR(10), ��0����ʾ����
	char szRetMsg[DEF_SBUFLEN];   //6������Ϣ	VARCHAR(200),���ط�0���ʾ���״������ĳ�ֽ��״����ϵͳ����(�������ֵ�)
	char szReqFunNop[DEF_SBUFLEN]; //11ԭ�����ܺ�	VARCHAR(20)       2006-4-12����
	char szPacketID[DEF_SBUFLEN];      //11��������к�	VARCHAR(20)       2006-4-12����
	char szReserved[DEF_SBUFLEN]; //�����ֶ�3	VARCHAR(20)
	
	int bIsNext;    //��������ʾ	CHAR(1),��0�����ޣ���1������ʾ�к����� (ȡ��������99����)
	int iFieldNum;  //Ӧ���ֶ���	VARCHAR(10)
	int iRecNum;    //Ӧ���¼��	VARCHAR(10)
	char *pBuffer;  //ָ�����յ����ݻ�����
	
	int iHeadLen;  //��ͷ����
	DWORD dwDataLen;  //���ݳ���
};

/////////////////////////////////////////////////////////////////////////////
// CKDGateway ��֤��һ�����ؽӿ�

class CKDGateway:public CError
{
public:
	CMiniSocket *m_pSocket; 
	
	//���������Ϣ
	char m_szOP_USER[DEF_SBUFLEN];   //����Ա����
	char m_szOP_ROLE[DEF_SBUFLEN];   //������ɫ
	char m_szOP_SITE[DEF_SBUFLEN];   //����վ��
	char m_szOP_BRANCH[DEF_SBUFLEN]; //������֧
	char m_szCHANNEL[DEF_SBUFLEN];   //��������
	DWORD m_dwReqPakSN;              //��ͷ���кţ���ֹ����
	char m_szVersion[DEF_SBUFLEN];   //�ӿڰ汾��:"KDGATEWAY1.2"
	char m_szCRCCode[DEF_SBUFLEN];   //CRCУ��
	char m_szWorkKey[DEF_SBUFLEN];   //������Կ
	char m_szSession[DEF_SBUFLEN];   //ͨѶSession
	
	char m_szRequestBuffer[DEF_LBUFLEN_8192]; //���󻺳���
public:
	//  CKDGWResult
	char *m_pReturnData;       //���ص�����������ʼ����m_iDataBufferSize=512K
	DWORD m_dwDataBufferSize;
	
	DWORD m_dwTotalDataLen;    //���������ܵ����ݳ���
	DWORD m_dwRecNo;           //��¼����
	DWORD m_dwFieldNo;		 //�ֶ�����
	
	//  tagRequestPacket m_RequestPacket;
	//  char m_szPacketBuffer[DEF_LBUFLEN_8192]; //���ͺͽ��ջ�����
	
	char *m_pSingleRecvBuffer;   //����������
	int m_iSingleRecvBufferSize; //������������С
	
public:
	CKDGateway();
	~CKDGateway();
	
	//��KDGateway��������
	BOOL Connect(const char *p_pszIPAddress, WORD p_wPort); 
	
	//��KDGateway�Ͽ�����
	BOOL Disconnect();
	
	//���������
	BOOL SendRequest(char *p_pszDataSect);
	
	//Recv�������� //p_bCRC��ʾ�Ƿ�Ҫ���հ�����CRCУ�飬��Ϊǩ����ǲ���ҪУ���
	BOOL RecvAnswer(tagAnswerPacket *p_pAnswerPacket, int p_bCRC=0);
	
	//ǩ��KDGateway
	BOOL CheckIn();
	
	//��¼
	BOOL Login(char p_cUserType, char *p_pszUserID, char *p_pszPassword);
	
	//��������
	BOOL WaitAnswer(char *p_pszDataSect);
	
	void EncryptPassWd(char *pPassWd);
	
	//*****************  ������յ��İ� *****************
	
    //�õ�Ӧ���¼��������ǵ����������̶�Ϊ1
    DWORD GetRecNum(){return m_dwRecNo;}; 
	
    //�õ�ָ����¼��ָ���ֶΣ�������,���ΪNULL��������GetLastError���õ����Ĵ���
    char* GetFieldValue(DWORD p_dwRecNo, char *p_pszFieldName, char *p_pszValue, int p_iValueSize);	
	DWORD GetFieldNum(){return m_dwFieldNo;};	
	
	//Ϊ�˳���򵥣������õ�ַ��Σ�ע�⻺��������
	//���ð�ͷ����
	inline void SetOP_USER  (char *p_pszOP_USER  ) { strncpy_s(m_szOP_USER  , p_pszOP_USER, DEF_SBUFLEN); };   //����Ա����
	inline void SetOP_ROLE  (char *p_pszOP_ROLE  ) { strncpy_s(m_szOP_ROLE, p_pszOP_ROLE, DEF_SBUFLEN); };   //������ɫ
	inline void SetOP_SITE  (char *p_pszOP_SITE  ) { strncpy_s(m_szOP_SITE  , p_pszOP_SITE, DEF_SBUFLEN); };   //����վ��
	inline void SetOP_BRANCH(char *p_pszOP_BRANCH) { strncpy_s(m_szOP_BRANCH, p_pszOP_BRANCH, DEF_SBUFLEN); };   //������֧
	inline void SetCHANNEL  (char *p_pszCHANNEL  ) { strncpy_s(m_szCHANNEL, p_pszCHANNEL, DEF_SBUFLEN); };   //��������
	inline void SetSession  (char *p_pszSession  ) { strncpy_s(m_szSession, p_pszSession, DEF_SBUFLEN); };   //��������
	
	//char m_szSession[DEF_SBUFLEN];   //ͨѶSession
	//�õ���ͷ����
	inline char *GetOP_USER  () { return m_szOP_USER  ; };   //����Ա����
	inline char *GetOP_ROLE  () { return m_szOP_ROLE  ; };   //������ɫ
	inline char *GetOP_SITE  () { return m_szOP_SITE  ; };   //����վ��
	inline char *GetOP_BRANCH() { return m_szOP_BRANCH; };   //������֧
	inline char *GetCHANNEL  () { return m_szCHANNEL  ; };   //��������
	
	inline char *GetRequestBuffer() { return m_szRequestBuffer; }; //�õ����ͻ�����
	inline char *GetWorkKey() { return m_szWorkKey; }; //�õ����ͻ�����

	int ResultStrToTable(const char *pReturnData);
	LPSTR GetNextLine(LPSTR lpstr);
	int GetFieldInfo(LPSTR lpstr, int *iPos);
};
