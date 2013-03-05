// CKDGateway.cpp : implementation file
//

#include "stdafx.h"
#include "Blowfish.h"
#include "crc.h"

#include "CMiniSocket.h"
#include "KDGateway.h"
#include "Tools.h"

#define TDX_UNUSED			(10000) /* 未使用 */

/////////////////////////////////////////////////////////////////////////////
// CKDGateway 金证新一代网关接口
//---------------------------------------------------------------------------
CKDGateway::CKDGateway()
{
	//初始化所有运行变量
	memset(m_szOP_USER  , 0, DEF_SBUFLEN);   //操作员代码
	memset(m_szOP_ROLE  , 0, DEF_SBUFLEN);   //操作角色
	memset(m_szOP_SITE  , 0, DEF_SBUFLEN);   //操作站点
	memset(m_szOP_BRANCH, 0, DEF_SBUFLEN);   //操作分支
	memset(m_szCHANNEL  , 0, DEF_SBUFLEN);   //操作渠道
	memset(m_szVersion  , 0, DEF_SBUFLEN);   //接口版本号:"KDGATEWAY1.2"
	memset(m_szCRCCode  , 0, DEF_SBUFLEN);   //CRC校验
	memset(m_szWorkKey  , 0, DEF_SBUFLEN);   //工作密钥
	memset(m_szSession  , 0, DEF_SBUFLEN);   //通讯Session
	
	memset(m_szRequestBuffer, 0, DEF_LBUFLEN_8192); //请求缓冲区
	
	m_dwReqPakSN = 0;
	m_pSocket = NULL;
	SetLastError(0,"OK.");
	
	
	strcpy_s(m_szVersion, "KDGATEWAY1.2");//接口版本号
	strcpy_s(m_szCRCCode, "12345678");//CRC校验
	strcpy_s(m_szWorkKey, "12345678");//工作密钥
	
	m_iSingleRecvBufferSize = 64*1024L;
	m_pSingleRecvBuffer = new char[m_iSingleRecvBufferSize];
	
	m_dwDataBufferSize = 512*1024L;
	m_pReturnData = new char[m_dwDataBufferSize];
	memset(m_pReturnData,0,m_dwDataBufferSize);
	m_dwTotalDataLen = 0;
	m_dwRecNo = 0;
}

//---------------------------------------------------------------------------
CKDGateway::~CKDGateway()
{
	if (m_pSocket!=NULL)
		delete m_pSocket;
}

//---------------------------------------------------------------------------
//与KDGateway建立连接
BOOL CKDGateway::Connect(const char *p_pIPAddress, WORD p_wPort)
{
	if (m_pSocket==NULL)
	{//创建通讯类
		m_pSocket = new CMiniSocket;
		if (m_pSocket==NULL)
		{
			SetLastError(-1,"创建通讯类失败");
			return FALSE;
		}
	}
	
	//关闭可能的连接
	if (m_pSocket->GetSocket()!= INVALID_SOCKET)
		m_pSocket->Close();
	
	if (m_pSocket->Connect("255.255.255.0",(char *)p_pIPAddress,p_wPort,TCP)!=0)
	{
        SetLastError(-2,"连接外围网关失败！");
		return FALSE;
	}
	return TRUE;
}

//与KDGateway断开连接
//---------------------------------------------------------------------------
BOOL CKDGateway::Disconnect()
{
	if (m_pSocket==NULL)
	{
		SetLastError(-3,"尚未成功连接不能 Disconnect！");
		return FALSE;
	}
	
	//关闭可能的连接
	if (m_pSocket->GetSocket()!= INVALID_SOCKET)
		m_pSocket->Close();
	
	delete m_pSocket;
	m_pSocket = NULL;
	return TRUE;
}

//---------------------------------------------------------------------------
//发送请求包
BOOL CKDGateway::SendRequest(char *p_pszDataSect)
{
	if (m_pSocket==NULL)
	{
		SetLastError(-3,"尚未成功连接KDGATEWAY 不能发送请求！");
		return FALSE;
	}
	
	m_pSocket->Clear(); //因为是同步模式，所以在发送前把接收缓冲区清空，以防止收到错乱包，导致接收数据出错
	
	//在请求序列包内字段内填入 线程ID:请求包数
	m_dwReqPakSN++;
	
	char szReqPakSN[DEF_SBUFLEN];
	sprintf_s(szReqPakSN, sizeof(szReqPakSN), "%x:%x", GetCurrentThreadId(), m_dwReqPakSN);
	
	sprintf_s(m_szRequestBuffer, sizeof(m_szRequestBuffer), "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
		"0000", //包头大小
		"0000", //数据大小
		m_szWorkKey, //CRC
		m_szVersion,
		m_szOP_USER, // 必须填写客户代码 USER_CODE而不是 操作员代码
		m_szOP_SITE,
		m_szOP_BRANCH,
		m_szCHANNEL,
		m_szSession,
		m_szOP_ROLE,
		szReqPakSN,
		""
		);
	
	int iHeadLen;
	int iDataLen;
	char szTemp[DEF_SBUFLEN];
	
	//写入包头长度
	sprintf_s(szTemp, sizeof(szTemp), "%04d",strlen(m_szRequestBuffer));
	memcpy(m_szRequestBuffer,szTemp,4);
	iHeadLen = atoi(szTemp);
	
	//写入请求段长度
	sprintf_s(szTemp, sizeof(szTemp), "%04d",strlen(p_pszDataSect)+1); //加1是为了增加数据包内的最后一个'|'
	memcpy(m_szRequestBuffer+5,szTemp,4);
	iDataLen = atoi(szTemp);
	
	strcat_s(m_szRequestBuffer, sizeof(m_szRequestBuffer), p_pszDataSect);
	strcat_s(m_szRequestBuffer, sizeof(m_szRequestBuffer), "|");
	
	//设置CRC校验码
	char szCRCCode[DEF_SBUFLEN];
	memset(szCRCCode,0,DEF_SBUFLEN);
	CRC_Get((unsigned char *)m_szRequestBuffer, iHeadLen+iDataLen, (unsigned char *)szCRCCode);
	szCRCCode[8] = 0;
	memcpy(m_szRequestBuffer+10,szCRCCode,8);
	
	if (m_pSocket->SendBuf(m_szRequestBuffer, strlen(m_szRequestBuffer))<=0)
	{
		SetLastError(-2,"发送请求包失败!");
		return FALSE;
	}
	
	return TRUE;
}

//---------------------------------------------------------------------------
//Recv接收请答包
BOOL CKDGateway::RecvAnswer(tagAnswerPacket *p_pAnswerPacket, int p_bCRC)
{
	int iLen;	
	int iPos;
	int iHeadLen;
	DWORD dwDataLen;
	char szTemp[DEF_SBUFLEN];
	//	memset(m_pSingleRecvBuffer, 0, DEF_LBUFLEN_8192);
	
	/*
	if ((iLen=g_pSocket->WaitReceive(100))<=0)
	{
	AfxMessageBox("没有应答结果");
	g_pSocket->Close();
	return;
	}
	*/
	
	/*
    char szBuffer[81920];
	
	  int i = m_pSocket->ReceiveLength();
	  m_pSocket->ReceiveBuf(szBuffer, i);
	  
		memset(p_pAnswerPacket, 0, sizeof(tagAnswerPacket));
		return FALSE;
	*/
	
	//先收包头的4位，得到包头大小
	iPos = 0;
	iLen = 4;
	if (m_pSocket->ReceiveBuf(m_pSingleRecvBuffer + iPos, iLen)!=iLen)
	{
		SetLastError(-4,"接收应答包错误！No.1");
		return FALSE;
	}
	iPos+=iLen;
	
	m_pSingleRecvBuffer[iPos+iLen] = 0;
	iHeadLen = atoi(m_pSingleRecvBuffer);  
	if ( (iHeadLen>2048) || (iHeadLen<20) )
	{
		m_pSocket->Clear(); //出现这种错误可能是因为错位，所以缓冲区内的数据全部清掉
		
		SetLastError(-5,"应答包头尺寸非法，可能是错包！ size=%d", iHeadLen);
		return FALSE;
	}
	
	//再取整个包头
	iLen = iHeadLen-iPos;
	if (m_pSocket->ReceiveBuf(m_pSingleRecvBuffer+iPos, iLen)!=iLen)
	{
		SetLastError(-6,"接收应答包错误！No.2");
		return FALSE;
	}
	iPos+=iLen;
	
	m_pSingleRecvBuffer[iPos+iLen] = 0;
	
	//包头取到后，判断序列号字段，以避免串户情况
	char szReqPakSN[DEF_SBUFLEN];
	sprintf_s(szReqPakSN, sizeof(szReqPakSN), "%x:%x", GetCurrentThreadId(), m_dwReqPakSN);
	
	GetValue(m_pSingleRecvBuffer,szTemp, 11, '|', DEF_SBUFLEN);
	if (strcmp(szReqPakSN,szTemp)!=0)
	{
		m_pSocket->Clear(); //出现这种错误可能是因为错位，所以缓冲区内的数据全部清掉
		SetLastError(-6,"应答包非请求包数据。");
		return FALSE;
	}
	
	//得到数据区大小
	GetValue(m_pSingleRecvBuffer, szTemp, 2, '|', DEF_SBUFLEN);
	dwDataLen = atol(szTemp);
	
	if (dwDataLen!=0)
	{//如果出错的话，是不会有数据段的
		//再接收整个数据段
		iLen = dwDataLen;
		
		if (iPos+iLen>(m_iSingleRecvBufferSize-iHeadLen))
		{
			SetLastError(-7,"接收应答包错误！单包太大! (包长：%d)", iLen);
			return FALSE;
		}
		
		if (m_pSocket->ReceiveBuf(m_pSingleRecvBuffer+iPos, iLen)!=iLen)
		{
			SetLastError(-7,"接收应答包错误！No.3 (包长：%d)", iLen);
			return FALSE;
		}
		iPos+=dwDataLen;
	}
	
	//判断是否还有因异常产生的剩余数据，如果有就清掉，为下一次接收做准备，因为协议依赖顺序关系
	if (m_pSocket->ReceiveLength()!=0)
	{
		m_pSocket->Clear();
		AfxMessageBox("警告：接收缓冲区还有剩余数据！做清除处理");
	}
	
	if (p_bCRC)
	{
		//检查CRC校验码
		char szCRCCode_Answer[DEF_SBUFLEN]; 
		char szCRCCode[DEF_SBUFLEN];
		
		memset(szCRCCode_Answer,0,DEF_SBUFLEN);
		memset(szCRCCode,0,DEF_SBUFLEN);
		
		char *pCRC;
		pCRC = strchr(m_pSingleRecvBuffer,'|');
		if (pCRC==NULL)
		{ //CRC校验码失败！
			SetLastError(-10,"包格式非法1，缺少|");
			return FALSE;
		}
		pCRC++;
		pCRC = strchr(pCRC,'|');
		if (pCRC==NULL)
		{ //CRC校验码失败！
			SetLastError(-10,"包格式非法2，缺少|");
			return FALSE;
		}
		pCRC++;
		strncpy_s(szCRCCode_Answer, pCRC, 8);  //备份以前的CRC码
		
		memcpy(pCRC, m_szWorkKey, 8); //写入工作密钥
		CRC_Get((unsigned char *)m_pSingleRecvBuffer, iHeadLen+dwDataLen, (unsigned char *)szCRCCode);
		szCRCCode[8] = 0;
		if (strncmp(szCRCCode,szCRCCode_Answer,8)!=0)
		{ //CRC校验码失败！
			SetLastError(-8,"CRC校验失败！");
			return FALSE;
		}
	}
	
	//  AfxMessageBox(m_pSingleRecvBuffer);
	
	GetValue(m_pSingleRecvBuffer, p_pAnswerPacket->szVersion, 4, '|', DEF_SBUFLEN);
	GetValue(m_pSingleRecvBuffer, p_pAnswerPacket->szRetCode, 5, '|', DEF_SBUFLEN);
	GetValue(m_pSingleRecvBuffer, p_pAnswerPacket->szRetMsg, 6, '|', DEF_SBUFLEN);
	
	GetValue(m_pSingleRecvBuffer, szTemp, 7, '|', DEF_SBUFLEN);//是否有下条记录
	p_pAnswerPacket->bIsNext = atoi(szTemp);
	
	GetValue(m_pSingleRecvBuffer, szTemp, 8, '|', DEF_SBUFLEN);//字段数量
	p_pAnswerPacket->iFieldNum = atoi(szTemp);
	
	GetValue(m_pSingleRecvBuffer, szTemp, 9, '|', DEF_SBUFLEN);//记录数量
	p_pAnswerPacket->iRecNum = atoi(szTemp);
	
	p_pAnswerPacket->iHeadLen = iHeadLen;
	p_pAnswerPacket->dwDataLen = dwDataLen;
	
	if (m_dwTotalDataLen<(m_dwDataBufferSize-dwDataLen))
	{//数据可以放到缓冲区内
		m_dwRecNo += p_pAnswerPacket->iRecNum;
		
		memcpy(m_pReturnData+m_dwTotalDataLen, m_pSingleRecvBuffer+iHeadLen, dwDataLen);
		m_dwTotalDataLen+=dwDataLen;
		
		*(m_pReturnData+m_dwTotalDataLen) = 0;
	}
	
	return TRUE;
}

//---------------------------------------------------------------------------
//签入KDGateway
BOOL CKDGateway::CheckIn()
{
	m_dwTotalDataLen = 0;
	
	//发送签入包
	if (SendRequest("100")!=TRUE)
		return FALSE;
	
	tagAnswerPacket AnswerPacket;
	if (RecvAnswer(&AnswerPacket, 0)!=TRUE) //0表示不校验CRC
		return FALSE;
	
	if (atol(AnswerPacket.szRetCode)!=0)
	{
		SetLastError(-8,"签入失败! ErrCode:%s ErrMsg:%s", AnswerPacket.szRetCode, AnswerPacket.szRetMsg);
		return FALSE;
	}
	
	char szWorkKey[256];
	memset(szWorkKey,0,256);
	GetValue(m_pSingleRecvBuffer+AnswerPacket.iHeadLen, szWorkKey, 2, '|', DEF_SBUFLEN);//记录数量  
	
	char szWorkKey_True[256];
	CBlowFish oBlowFish((unsigned char *)"SZKINGDOM", 9);
	oBlowFish.Decrypt((const unsigned char*)szWorkKey, (unsigned char*)szWorkKey_True, strlen(szWorkKey));
	
	strcpy_s(m_szWorkKey,szWorkKey_True);
	m_szWorkKey[8] = 0;
	return TRUE;
}

//---------------------------------------------------------------------------
//发送请求
BOOL CKDGateway::WaitAnswer(char *p_pszDataSect)
{
	m_dwTotalDataLen = 0;
	m_dwRecNo = 0;
	m_dwFieldNo = 0;	

	//发送请求
	if (SendRequest(p_pszDataSect)!=TRUE)
		return FALSE;
	
	//接收应答
	tagAnswerPacket AnswerPacket;
	if (RecvAnswer(&AnswerPacket)!=TRUE)
		return FALSE;
	
	if (atol(AnswerPacket.szRetCode)!=0)
	{
		SetLastError(-8,"请求失败！ErrCode:%s ErrMsg:%s", AnswerPacket.szRetCode, AnswerPacket.szRetMsg);
		return FALSE;
	}
	
	while(AnswerPacket.bIsNext)
	{
		//发送请求
		if (SendRequest("99")!=TRUE) //取后继包
			return FALSE;
		
		//接收应答
		if (RecvAnswer(&AnswerPacket)!=TRUE)
			return FALSE;
		
		if (atol(AnswerPacket.szRetCode)!=0)
		{
			SetLastError(-8,"请求失败！ErrCode:%s ErrMsg:%s", AnswerPacket.szRetCode, AnswerPacket.szRetMsg);
			return FALSE;
		}
	}

	m_dwRecNo = AnswerPacket.iRecNum;
	m_dwFieldNo = AnswerPacket.iFieldNum;
	return TRUE;
}

void CKDGateway::EncryptPassWd(char *pPassWd)
{
	CBlowFish oBlowFish((unsigned char *)m_szWorkKey, strlen(m_szWorkKey));
	oBlowFish.Encrypt((unsigned char*)pPassWd, (unsigned char*)pPassWd, strlen(pPassWd));
}

LPSTR CKDGateway::GetNextLine(LPSTR lpstr)
{
	LPSTR lpstrTemp = lpstr;
	DWORD i = 0;
	while(1)
	{
		lpstrTemp = strstr(lpstrTemp, "|");
		if ( lpstrTemp == NULL ) return NULL;
		lpstrTemp++;
		i++;
		
		if ( i >= this->GetFieldNum() )break;
	}
	return lpstrTemp;
}

int CKDGateway::GetFieldInfo(LPSTR lpstr, int *iPos)
{
	if(!lpstr)return 0;
	
	*iPos = 0;
	int nField = 0;
	
	LPSTR lpTemp = lpstr;
	

	return 1;
}
