#include <stdafx.h>
#include <winsock.h>
#include <wsipx.h>
#include <wsnwlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <TIME.H>
#include "CMiniSocket.h"

extern char *g_pRecvBuffer;
//--------------------------------------------------------------
//    AfxMessageBox("初始化 Windows Sockets 失败!", MB_OK|MB_ICONERROR);
//--------------------------------------------------------------

//extern tagConfig g_Config;  

//extern void WriteRunLog(char *p_pszMessage);

CMiniSocket::CMiniSocket()
{
	m_iTimeOut = 40; //超时时间 秒为单位

	m_pszErrorMessage = NULL;

	m_iSuccessConnect = 0;
}
//--------------------------------------------------------------
CMiniSocket::~CMiniSocket()
{
	Close();	
	
	if (m_pszErrorMessage!=NULL)
		delete m_pszErrorMessage;
	m_pszErrorMessage = NULL;
}
//--------------------------------------------------------------
int CMiniSocket::InitWinSocket()
{
	//初始化Windows Sockets，并检查版本。
  WSADATA WSAData;
  if (WSAStartup(2, &WSAData)) {
	  SetErrorMessage("初始化 Windows Sockets 失败!");
    return -1;
  }

  if (WSAData.wVersion != 2) {
	  SetErrorMessage("Windows Sockets 版本不支持!");
    WSACleanup();
    return -1;
  }

  return 0;
}
//--------------------------------------------------------------
int CMiniSocket::Connect(char *p_pszNetwork , char *p_pszAddress , int p_iPort , int p_iProtoType,int p_iTimeOut)
{
int iLen;

	switch(p_iProtoType) {
		case TCP:
			sockaddr_in TCP_ServerAddr;

			m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (m_hSocket == INVALID_SOCKET) {
				SetErrorMessage("创建Socket失败!");
				return -1;
			}
			
			SetReadTimeOut(p_iTimeOut*1000);
			SetWriteTimeOut(p_iTimeOut*1000);
			SetUnBlockMode(1);

			TCP_ServerAddr.sin_family = AF_INET;
			TCP_ServerAddr.sin_addr.s_addr=inet_addr(p_pszAddress);
			TCP_ServerAddr.sin_port=htons(p_iPort);
			connect(m_hSocket, (LPSOCKADDR)&TCP_ServerAddr, sizeof(TCP_ServerAddr));

			//			if (connect(m_hSocket, (LPSOCKADDR)&TCP_ServerAddr, sizeof(TCP_ServerAddr)) == SOCKET_ERROR) {
			if (WaitReply(p_iTimeOut)!=0) {
				SetErrorMessage("连接失败!");
				return -1;
			}		

			SetUnBlockMode(1);
			SetReadTimeOut(30000);
			SetWriteTimeOut(30000);

			iLen = sizeof(TCP_ServerAddr);
			getsockname(m_hSocket,(LPSOCKADDR)&TCP_ServerAddr, &iLen);		
			break;
		case SPX:	
			SOCKADDR_IPX m_IPX_ServerAddr;
			m_hSocket = socket(AF_IPX, SOCK_STREAM, NSPROTO_SPX);
			if (m_hSocket == INVALID_SOCKET) {
				SetErrorMessage("创建Socket失败!");
				return -1;
			}
			
			SetReadTimeOut(p_iTimeOut*1000);
			SetWriteTimeOut(p_iTimeOut*1000);
			SetUnBlockMode(1);

			m_IPX_ServerAddr.sa_family = AF_IPX;
			AtoH((char*)m_IPX_ServerAddr.sa_netnum, p_pszNetwork, 4);
			AtoH((char*)m_IPX_ServerAddr.sa_nodenum, p_pszAddress, 6);
			m_IPX_ServerAddr.sa_socket = htons(p_iPort);

			connect(m_hSocket, (LPSOCKADDR)&m_IPX_ServerAddr, sizeof(m_IPX_ServerAddr));

			//if (connect(m_hSocket, (LPSOCKADDR)&m_IPX_ServerAddr, sizeof(m_IPX_ServerAddr)) == SOCKET_ERROR) {
			if (WaitReply(p_iTimeOut)!=0) {
				SetErrorMessage("连接失败!");
				return -1;
			}		
			SetUnBlockMode(1);
			SetReadTimeOut(30000);
			SetWriteTimeOut(30000);

			iLen = sizeof(m_IPX_ServerAddr);
			getsockname(m_hSocket,(LPSOCKADDR)&m_IPX_ServerAddr, &iLen);
		
			break;
		default:
			SetErrorMessage("不可识别的协议类型");
			return -1;			
	}
	
	strncpy(m_pszNetwork,p_pszNetwork,32);
	strncpy(m_pszAddress,p_pszAddress,32);
	m_iPort = p_iPort;
	m_iProtoType = p_iProtoType;
	
	m_iSuccessConnect = 1;
	return 0;
}
//--------------------------------------------------------------
int CMiniSocket::ReConnect(int p_iTimeOut)
{
	Close();
//	Sleep(500);
	return Connect(m_pszNetwork,m_pszAddress,m_iPort,m_iProtoType,p_iTimeOut);
}
//--------------------------------------------------------------
int CMiniSocket::Close(void)
{	  
	int iRetCode = 0;
	if (m_hSocket != INVALID_SOCKET) 
	{
		iRetCode = closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	return iRetCode;
}
//--------------------------------------------------------------
int CMiniSocket::Clear(void)
{
char pszTemp[512];
int iRetCode;
	
	if ((iRetCode=ReceiveLength())>0) {
		ReceiveBuf(pszTemp,iRetCode);
	}
	
	return 0;
}
//--------------------------------------------------------------
int CMiniSocket::ReceiveLength(void)
{
	int iRetCode;
	u_long lLen;
	lLen = 0;
	fd_set fdr;
	struct timeval timeout;
	FD_ZERO(&fdr);
	FD_SET(m_hSocket, &fdr);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	
	iRetCode = select(m_hSocket , &fdr , (fd_set *)0, (fd_set *)0, &timeout); 
	if (iRetCode<=0)
		return iRetCode;

	iRetCode = ioctlsocket(m_hSocket,FIONREAD,&lLen);
	if (iRetCode == SOCKET_ERROR) 
	{
		SetErrorMessage("CMiniSocket::ReceiveLength::ioctlsocket失败");
		return 0;
	}
	return lLen;
}
//--------------------------------------------------------------
int CMiniSocket::WaitReceive(int p_iWaitTime)
{//等待接收,如果返回<0则表示无数据 =0表示超时 >0表示有数据可接收
	int iRetCode;
	time_t timeBegin;		
//	fd_set fdr;
//	struct timeval timeout;
	
	timeBegin = time(NULL);	
	int iOK = 0;
	iRetCode = -1;
	while(time(NULL)-timeBegin<p_iWaitTime) { //等20秒钟返回数据		
		if ((iRetCode=ReceiveLength())>0) {
			iOK = 1;
			break;
		}
		Sleep(1);
	}

	return iRetCode;
}
//--------------------------------------------------------------
/*
int CMiniSocket::ReceiveBuf(void *p_pSendBuf,int p_iBuffLen)
作用:往此类Socket上发送数据p_pSendBuff,
返回:-1:类Socket无效
     -2:WinSocket出现严重错误,类Socket被自动关闭
	 -3:send时出错,不重试
	 -99:等待发送数据超时
	 其它:接收数据包的大小
作者:罗时俊 2003/10/24
*/
int CMiniSocket::ReceiveBuf(void *p_pRecvBuf,int p_iBuffLen)
{
	memset(p_pRecvBuf, 0, p_iBuffLen);
	int iRetCode;
	time_t timeBegin = time(NULL);		
	long iReadBytes;

	if (m_hSocket == INVALID_SOCKET) 
	{
		SetErrorMessage("CMiniSocket::ReadBuf发现无效的Socket");
		return -1;
	}
	
	iRetCode = -1;
	
	char *pszData = (char*)p_pRecvBuf;
	iReadBytes = 0;
	while ((time(NULL)-timeBegin)<m_iTimeOut)
	{
	
		if (WaitReceive(m_iTimeOut)>0) { //有数据可读
			iRetCode = recv(m_hSocket,(char*)p_pRecvBuf+iReadBytes,p_iBuffLen-iReadBytes,0);
			if (iRetCode==SOCKET_ERROR) {
				SetErrorMessage("CMiniSocket::ReadBuf::recv错误");
				return -3;
			}
			iReadBytes+=iRetCode;

			if (p_iBuffLen-iReadBytes<=0) { //收满一个包则退出
				return p_iBuffLen;
			}
		}
	}

	SetErrorMessage("CMiniSocket::ReadBuf超时");
	return -99;
}
//--------------------------------------------------------------

//--------------------------------------------------------------
/*
int CMiniSocket::SendBuf(void *p_pSendBuf,int p_iBuffLen)
作用:往此类Socket上发送数据p_pSendBuff,
返回:-1:类Socket无效
     -2:WinSocket出现严重错误,类Socket被自动关闭
	 -3:send时出错,不重试
	 -99:等待发送数据超时
	 其它:发送数据的大小
作者:罗时俊 2003/10/24
*/
int CMiniSocket::SendBuf(void *p_pSendBuf,int p_iBuffLen)
{
	int iRetCode;
	time_t timeBegin = time(NULL);		
	fd_set fdw;
	struct timeval timeout;

	if (m_hSocket == INVALID_SOCKET) 
	{
		SetErrorMessage("CMiniSocket::SendPackage发现无效的Socket");
		return -1;
	}

	iRetCode = 0;
	while ((time(NULL)-timeBegin)<m_iTimeOut)
	{
		do{
			FD_ZERO(&fdw);
			FD_SET(m_hSocket, &fdw);
			timeout.tv_sec =  m_iTimeOut;
			timeout.tv_usec = 0;		
			iRetCode = select(m_hSocket , (fd_set *)0, &fdw, (fd_set *)0, &timeout); 
			if (iRetCode<1) {
				Sleep(200);
			}
		}while( (iRetCode == SOCKET_ERROR) && (time(NULL)-timeBegin<m_iTimeOut) );

		if (iRetCode == SOCKET_ERROR) 
		{
			SetErrorMessage("CMiniSocket::SendPackage::select失败");
			return -2;
		}
		else if (iRetCode == 0) //没有准备好发送数据
		{
			Sleep(10);
			continue;
		}
		else if (iRetCode>0)
		{
//			MessageBox(NULL,"即将发送请请包","调试",MB_OK);
			
			iRetCode = send(m_hSocket,(char*)p_pSendBuf,p_iBuffLen,MSG_DONTROUTE);
			if (iRetCode==SOCKET_ERROR) {
				SetErrorMessage("CMiniSocket::SendPackage::send错误");
				return -3;
			}
			return iRetCode;
		}
		else {
			SetErrorMessage("CMiniSocket::SendPackage失败:select出现错误");
			return -99;
		}
	}

	SetErrorMessage("CMiniSocket::SendPackage超时");
	return -99;
}
//--------------------------------------------------------------
unsigned char CMiniSocket::BtoH(char p_ch)
{
  if (p_ch >= '0' && p_ch <= '9')
    return (p_ch - '0');

  if (p_ch >= 'A' && p_ch <= 'F')
    return (p_ch - 'A' + 0xA);

  if (p_ch >= 'a' && p_ch <= 'f')
    return (p_ch - 'a' + 0xA);
	
	return 0;
}
//--------------------------------------------------------------------

void CMiniSocket::AtoH(char *p_szDest, char *p_szSource, int p_iCount)
{
  while (p_iCount--)
    *p_szDest++ = (BtoH(*p_szSource++) << 4) + BtoH(*p_szSource++);
  return;
}
//--------------------------------------------------------------------
char * CMiniSocket::GetErrorMessage()
{
	return m_pszErrorMessage;
}
//--------------------------------------------------------------------
void CMiniSocket::SetErrorMessage(char *p_pszErrorMessage)
{
int iLen;
	iLen = strlen(p_pszErrorMessage)+1;
	if (m_pszErrorMessage!=NULL)
		delete m_pszErrorMessage;

	m_pszErrorMessage = new char[iLen];
	
	strncpy(m_pszErrorMessage,p_pszErrorMessage,iLen);

//	WriteLog(m_pszErrorMessage);
}
//--------------------------------------------------------------------
int CMiniSocket::SetReadTimeOut(int p_iTimeOut)
{
	if(::setsockopt(m_hSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&p_iTimeOut,sizeof(p_iTimeOut))==SOCKET_ERROR)
		return -1;

	return 0;
}
//--------------------------------------------------------------------
int CMiniSocket::SetWriteTimeOut(int p_iTimeOut)
{
	if(::setsockopt(m_hSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&p_iTimeOut,sizeof(p_iTimeOut))==SOCKET_ERROR)
		return -1;
	
	return 0;
}
//--------------------------------------------------------------------
int CMiniSocket::SetUnBlockMode(DWORD p_dwBlockMode) //0/1   
{
	if (ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&p_dwBlockMode)==SOCKET_ERROR)
		return -1;

	return 0;
}
//--------------------------------------------------------------------
int CMiniSocket::WaitReply(int p_iTimeOut)
//select 模型，即设置超时
{
	fd_set r;
	struct timeval timeout;

	FD_ZERO(&r);
	FD_SET(m_hSocket, &r);
	timeout.tv_sec = p_iTimeOut; //连接超时15秒
	timeout.tv_usec =0;
	if (::select(0, 0, &r, 0, &timeout)<=0)
		return -1;

	return 0;
}

//--------------------------------------------------------------------
SOCKET CMiniSocket::GetSocket()
{
  return m_hSocket;
}




















