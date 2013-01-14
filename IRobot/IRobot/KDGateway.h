/***************************************************************************
CKDGateway.cpp : implementation file

  金证新一代柜台外围网关通讯类
  
	author: luosj 200610
	
****************************************************************************/

#include "stdafx.h"
#include "CMiniSocket.h"
#include "error.h"

#define DEF_SBUFLEN 32    //定义小缓冲区大小
#define DEF_LBUFLEN_8192 8192  //定义大缓冲区大小

struct tagAnswerPacket //应答包
{
	char szCRCCode[DEF_SBUFLEN];  //3CRC校验码	CHAR(8), 见注5,7
	char szVersion[DEF_SBUFLEN];  //4版本号	当前协议版本编号(固定串“KDGATEWAY1.0”)
	char szRetCode[DEF_SBUFLEN];  //5返回码	VARCHAR(10), “0”表示正常
	char szRetMsg[DEF_SBUFLEN];   //6返回信息	VARCHAR(200),返回非0则表示交易处理出现某种交易错误或系统错误(见数据字典)
	char szReqFunNop[DEF_SBUFLEN]; //11原请求功能号	VARCHAR(20)       2006-4-12增加
	char szPacketID[DEF_SBUFLEN];      //11请求包序列号	VARCHAR(20)       2006-4-12增加
	char szReserved[DEF_SBUFLEN]; //保留字段3	VARCHAR(20)
	
	int bIsNext;    //后续包标示	CHAR(1),‘0’－无，‘1’－表示有后续包 (取后续包发99请求)
	int iFieldNum;  //应答字段数	VARCHAR(10)
	int iRecNum;    //应答记录数	VARCHAR(10)
	char *pBuffer;  //指到接收的数据缓冲区
	
	int iHeadLen;  //包头长度
	DWORD dwDataLen;  //数据长度
};

/////////////////////////////////////////////////////////////////////////////
// CKDGateway 金证新一代网关接口

class CKDGateway:public CError
{
public:
	CMiniSocket *m_pSocket; 
	
	//请求相关信息
	char m_szOP_USER[DEF_SBUFLEN];   //操作员代码
	char m_szOP_ROLE[DEF_SBUFLEN];   //操作角色
	char m_szOP_SITE[DEF_SBUFLEN];   //操作站点
	char m_szOP_BRANCH[DEF_SBUFLEN]; //操作分支
	char m_szCHANNEL[DEF_SBUFLEN];   //操作渠道
	DWORD m_dwReqPakSN;              //包头序列号，防止串户
	char m_szVersion[DEF_SBUFLEN];   //接口版本号:"KDGATEWAY1.2"
	char m_szCRCCode[DEF_SBUFLEN];   //CRC校验
	char m_szWorkKey[DEF_SBUFLEN];   //工作密钥
	char m_szSession[DEF_SBUFLEN];   //通讯Session
	
	char m_szRequestBuffer[DEF_LBUFLEN_8192]; //请求缓冲区
public:
	//  CKDGWResult
	char *m_pReturnData;       //返回的数据区，初始分配m_iDataBufferSize=512K
	DWORD m_dwDataBufferSize;
	
	DWORD m_dwTotalDataLen;    //缓冲区内总的数据长度
	DWORD m_dwRecNo;           //记录总数
	DWORD m_dwFieldNo;		 //字段总数
	
	//  tagRequestPacket m_RequestPacket;
	//  char m_szPacketBuffer[DEF_LBUFLEN_8192]; //发送和接收缓冲区
	
	char *m_pSingleRecvBuffer;   //单包缓冲区
	int m_iSingleRecvBufferSize; //单包缓冲区大小
	
public:
	CKDGateway();
	~CKDGateway();
	
	//与KDGateway建立连接
	BOOL Connect(const char *p_pszIPAddress, WORD p_wPort); 
	
	//与KDGateway断开连接
	BOOL Disconnect();
	
	//发送请求包
	BOOL SendRequest(char *p_pszDataSect);
	
	//Recv接收请答包 //p_bCRC表示是否要对收包进行CRC校验，因为签入包是不需要校验的
	BOOL RecvAnswer(tagAnswerPacket *p_pAnswerPacket, int p_bCRC=0);
	
	//签入KDGateway
	BOOL CheckIn();
	
	//登录
	BOOL Login(char p_cUserType, char *p_pszUserID, char *p_pszPassword);
	
	//发送请求
	BOOL WaitAnswer(char *p_pszDataSect);
	
	void EncryptPassWd(char *pPassWd);
	
	//*****************  处理接收到的包 *****************
	
    //得到应答记录数，如果是单结果集，则固定为1
    DWORD GetRecNum(){return m_dwRecNo;}; 
	
    //得到指定记录的指定字段，并返回,如果为NULL，可以用GetLastError来得到最后的错误
    char* GetFieldValue(DWORD p_dwRecNo, char *p_pszFieldName, char *p_pszValue, int p_iValueSize);	
	DWORD GetFieldNum(){return m_dwFieldNo;};	
	
	//为了程序简单，不设置地址入参，注意缓冲区长度
	//设置包头常量
	inline void SetOP_USER  (char *p_pszOP_USER  ) { strncpy_s(m_szOP_USER  , p_pszOP_USER, DEF_SBUFLEN); };   //操作员代码
	inline void SetOP_ROLE  (char *p_pszOP_ROLE  ) { strncpy_s(m_szOP_ROLE, p_pszOP_ROLE, DEF_SBUFLEN); };   //操作角色
	inline void SetOP_SITE  (char *p_pszOP_SITE  ) { strncpy_s(m_szOP_SITE  , p_pszOP_SITE, DEF_SBUFLEN); };   //操作站点
	inline void SetOP_BRANCH(char *p_pszOP_BRANCH) { strncpy_s(m_szOP_BRANCH, p_pszOP_BRANCH, DEF_SBUFLEN); };   //操作分支
	inline void SetCHANNEL  (char *p_pszCHANNEL  ) { strncpy_s(m_szCHANNEL, p_pszCHANNEL, DEF_SBUFLEN); };   //操作渠道
	inline void SetSession  (char *p_pszSession  ) { strncpy_s(m_szSession, p_pszSession, DEF_SBUFLEN); };   //操作渠道
	
	//char m_szSession[DEF_SBUFLEN];   //通讯Session
	//得到包头常量
	inline char *GetOP_USER  () { return m_szOP_USER  ; };   //操作员代码
	inline char *GetOP_ROLE  () { return m_szOP_ROLE  ; };   //操作角色
	inline char *GetOP_SITE  () { return m_szOP_SITE  ; };   //操作站点
	inline char *GetOP_BRANCH() { return m_szOP_BRANCH; };   //操作分支
	inline char *GetCHANNEL  () { return m_szCHANNEL  ; };   //操作渠道
	
	inline char *GetRequestBuffer() { return m_szRequestBuffer; }; //得到发送缓冲区
	inline char *GetWorkKey() { return m_szWorkKey; }; //得到发送缓冲区

	int ResultStrToTable(const char *pReturnData);
	LPSTR GetNextLine(LPSTR lpstr);
	int GetFieldInfo(LPSTR lpstr, int *iPos);
};
