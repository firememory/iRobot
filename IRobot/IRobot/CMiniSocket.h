/*
Create by luosj 2003/10/23

Mini Socket Class. a little socket
*/

#ifndef __CMiniSocket_Include__
	#define __CMiniSocket_Include__

//--------------------------------------------------------------
#include <winsock.h>
#include <wsipx.h>
#include <wsnwlink.h>

//--------------------------------------------------------------

#ifndef BYTE
	typedef unsigned char  BYTE;
#endif
#ifndef WORD
	typedef unsigned short WORD;
#endif
#ifndef DWORD
	typedef unsigned long  DWORD;
#endif

#define MAX_COMM_BUFF_LEN 8192

//--------------------------------------------------------------
#ifndef NET_SERVER_ADDRESS
 #define NET_SERVER_ADDRESS
struct tagNetServerAddress {
	char pszAddress[32];
	char pszNetwork[32];
	int iPort;
	int iProtoType;
	int iSecretLevel;//加密级别 0:不加密 1:普通 2:压缩 3:DES 4:证书,add by luosj 2004/07/20 

	int iPerformance;//性能参数 0:连接不上 >0 越小越好,表示中心商连接时间 add by luosj 2004/08/19
};
#endif
//--------------------------------------------------------------
enum NET_PROTO {
	TCP,
	SPX
};

typedef struct 
{
	char Node[9];					//网段号
	char Address[16];			//网卡号
	unsigned int  Socket; //Socket
}tagNETWORKADD;

//--------------------------------------------------------------

class CMiniSocket
{
	private:
		SOCKET m_hSocket;
		char m_pszAddress[32];
		char m_pszNetwork[32];
		int  m_iPort;
		int  m_iProtoType;

		char *m_pszErrorMessage;
		int  m_iTimeOut;
	private:
		unsigned char BtoH(char p_ch);
		void AtoH(char *p_szDest, char *p_szSource, int p_iCount);
	public:
		CMiniSocket();
		virtual ~CMiniSocket();

		int m_iSuccessConnect;//是否成功连接
		int m_iSecretLevel;
	public:
		int InitWinSocket();
		int Connect(char *p_pszNetwork,char *p_pszAddress,int p_iPort,int p_iProtoType,int p_iTimeOut=30);
		int ReConnect(int p_iTimeOut=3);
		int Close();
		int Clear();
		int ReceiveLength();
		int ReceiveBuf(void *p_pRecvBuff,int p_iBuffLen);
		
		int WaitReceive(int p_iWaitTime);//等待接收,如果返回<0则表示无数据 =0表示超时 >0表示有数据可接收
		
		int SendBuf(void *p_pSendBuff,int p_iBuffLen);	

		int CheckSocket();//检测SOCKET状态是否正常

		int SetReadTimeOut(int p_iTimeOut);
		int SetWriteTimeOut(int p_iTimeOut);
		int SetUnBlockMode(DWORD p_dwBlockMode); //0/1  
		int WaitReply(int p_iTimeOut);
		
		SOCKET GetSocket();
//---------------------------------------------------------------
		char * GetErrorMessage();
		void SetErrorMessage(char *p_pszErrorMessage);
};

#endif





















