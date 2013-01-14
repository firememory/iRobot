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
	int iSecretLevel;//���ܼ��� 0:������ 1:��ͨ 2:ѹ�� 3:DES 4:֤��,add by luosj 2004/07/20 

	int iPerformance;//���ܲ��� 0:���Ӳ��� >0 ԽСԽ��,��ʾ����������ʱ�� add by luosj 2004/08/19
};
#endif
//--------------------------------------------------------------
enum NET_PROTO {
	TCP,
	SPX
};

typedef struct 
{
	char Node[9];					//���κ�
	char Address[16];			//������
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

		int m_iSuccessConnect;//�Ƿ�ɹ�����
		int m_iSecretLevel;
	public:
		int InitWinSocket();
		int Connect(char *p_pszNetwork,char *p_pszAddress,int p_iPort,int p_iProtoType,int p_iTimeOut=30);
		int ReConnect(int p_iTimeOut=3);
		int Close();
		int Clear();
		int ReceiveLength();
		int ReceiveBuf(void *p_pRecvBuff,int p_iBuffLen);
		
		int WaitReceive(int p_iWaitTime);//�ȴ�����,�������<0���ʾ������ =0��ʾ��ʱ >0��ʾ�����ݿɽ���
		
		int SendBuf(void *p_pSendBuff,int p_iBuffLen);	

		int CheckSocket();//���SOCKET״̬�Ƿ�����

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





















