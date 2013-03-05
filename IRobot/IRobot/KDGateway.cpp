// CKDGateway.cpp : implementation file
//

#include "stdafx.h"
#include "Blowfish.h"
#include "crc.h"

#include "CMiniSocket.h"
#include "KDGateway.h"
#include "Tools.h"

#define TDX_UNUSED			(10000) /* δʹ�� */

/////////////////////////////////////////////////////////////////////////////
// CKDGateway ��֤��һ�����ؽӿ�
//---------------------------------------------------------------------------
CKDGateway::CKDGateway()
{
	//��ʼ���������б���
	memset(m_szOP_USER  , 0, DEF_SBUFLEN);   //����Ա����
	memset(m_szOP_ROLE  , 0, DEF_SBUFLEN);   //������ɫ
	memset(m_szOP_SITE  , 0, DEF_SBUFLEN);   //����վ��
	memset(m_szOP_BRANCH, 0, DEF_SBUFLEN);   //������֧
	memset(m_szCHANNEL  , 0, DEF_SBUFLEN);   //��������
	memset(m_szVersion  , 0, DEF_SBUFLEN);   //�ӿڰ汾��:"KDGATEWAY1.2"
	memset(m_szCRCCode  , 0, DEF_SBUFLEN);   //CRCУ��
	memset(m_szWorkKey  , 0, DEF_SBUFLEN);   //������Կ
	memset(m_szSession  , 0, DEF_SBUFLEN);   //ͨѶSession
	
	memset(m_szRequestBuffer, 0, DEF_LBUFLEN_8192); //���󻺳���
	
	m_dwReqPakSN = 0;
	m_pSocket = NULL;
	SetLastError(0,"OK.");
	
	
	strcpy_s(m_szVersion, "KDGATEWAY1.2");//�ӿڰ汾��
	strcpy_s(m_szCRCCode, "12345678");//CRCУ��
	strcpy_s(m_szWorkKey, "12345678");//������Կ
	
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
//��KDGateway��������
BOOL CKDGateway::Connect(const char *p_pIPAddress, WORD p_wPort)
{
	if (m_pSocket==NULL)
	{//����ͨѶ��
		m_pSocket = new CMiniSocket;
		if (m_pSocket==NULL)
		{
			SetLastError(-1,"����ͨѶ��ʧ��");
			return FALSE;
		}
	}
	
	//�رտ��ܵ�����
	if (m_pSocket->GetSocket()!= INVALID_SOCKET)
		m_pSocket->Close();
	
	if (m_pSocket->Connect("255.255.255.0",(char *)p_pIPAddress,p_wPort,TCP)!=0)
	{
        SetLastError(-2,"������Χ����ʧ�ܣ�");
		return FALSE;
	}
	return TRUE;
}

//��KDGateway�Ͽ�����
//---------------------------------------------------------------------------
BOOL CKDGateway::Disconnect()
{
	if (m_pSocket==NULL)
	{
		SetLastError(-3,"��δ�ɹ����Ӳ��� Disconnect��");
		return FALSE;
	}
	
	//�رտ��ܵ�����
	if (m_pSocket->GetSocket()!= INVALID_SOCKET)
		m_pSocket->Close();
	
	delete m_pSocket;
	m_pSocket = NULL;
	return TRUE;
}

//---------------------------------------------------------------------------
//���������
BOOL CKDGateway::SendRequest(char *p_pszDataSect)
{
	if (m_pSocket==NULL)
	{
		SetLastError(-3,"��δ�ɹ�����KDGATEWAY ���ܷ�������");
		return FALSE;
	}
	
	m_pSocket->Clear(); //��Ϊ��ͬ��ģʽ�������ڷ���ǰ�ѽ��ջ�������գ��Է�ֹ�յ����Ұ������½������ݳ���
	
	//���������а����ֶ������� �߳�ID:�������
	m_dwReqPakSN++;
	
	char szReqPakSN[DEF_SBUFLEN];
	sprintf_s(szReqPakSN, sizeof(szReqPakSN), "%x:%x", GetCurrentThreadId(), m_dwReqPakSN);
	
	sprintf_s(m_szRequestBuffer, sizeof(m_szRequestBuffer), "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
		"0000", //��ͷ��С
		"0000", //���ݴ�С
		m_szWorkKey, //CRC
		m_szVersion,
		m_szOP_USER, // ������д�ͻ����� USER_CODE������ ����Ա����
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
	
	//д���ͷ����
	sprintf_s(szTemp, sizeof(szTemp), "%04d",strlen(m_szRequestBuffer));
	memcpy(m_szRequestBuffer,szTemp,4);
	iHeadLen = atoi(szTemp);
	
	//д������γ���
	sprintf_s(szTemp, sizeof(szTemp), "%04d",strlen(p_pszDataSect)+1); //��1��Ϊ���������ݰ��ڵ����һ��'|'
	memcpy(m_szRequestBuffer+5,szTemp,4);
	iDataLen = atoi(szTemp);
	
	strcat_s(m_szRequestBuffer, sizeof(m_szRequestBuffer), p_pszDataSect);
	strcat_s(m_szRequestBuffer, sizeof(m_szRequestBuffer), "|");
	
	//����CRCУ����
	char szCRCCode[DEF_SBUFLEN];
	memset(szCRCCode,0,DEF_SBUFLEN);
	CRC_Get((unsigned char *)m_szRequestBuffer, iHeadLen+iDataLen, (unsigned char *)szCRCCode);
	szCRCCode[8] = 0;
	memcpy(m_szRequestBuffer+10,szCRCCode,8);
	
	if (m_pSocket->SendBuf(m_szRequestBuffer, strlen(m_szRequestBuffer))<=0)
	{
		SetLastError(-2,"���������ʧ��!");
		return FALSE;
	}
	
	return TRUE;
}

//---------------------------------------------------------------------------
//Recv��������
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
	AfxMessageBox("û��Ӧ����");
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
	
	//���հ�ͷ��4λ���õ���ͷ��С
	iPos = 0;
	iLen = 4;
	if (m_pSocket->ReceiveBuf(m_pSingleRecvBuffer + iPos, iLen)!=iLen)
	{
		SetLastError(-4,"����Ӧ�������No.1");
		return FALSE;
	}
	iPos+=iLen;
	
	m_pSingleRecvBuffer[iPos+iLen] = 0;
	iHeadLen = atoi(m_pSingleRecvBuffer);  
	if ( (iHeadLen>2048) || (iHeadLen<20) )
	{
		m_pSocket->Clear(); //�������ִ����������Ϊ��λ�����Ի������ڵ�����ȫ�����
		
		SetLastError(-5,"Ӧ���ͷ�ߴ�Ƿ��������Ǵ���� size=%d", iHeadLen);
		return FALSE;
	}
	
	//��ȡ������ͷ
	iLen = iHeadLen-iPos;
	if (m_pSocket->ReceiveBuf(m_pSingleRecvBuffer+iPos, iLen)!=iLen)
	{
		SetLastError(-6,"����Ӧ�������No.2");
		return FALSE;
	}
	iPos+=iLen;
	
	m_pSingleRecvBuffer[iPos+iLen] = 0;
	
	//��ͷȡ�����ж����к��ֶΣ��Ա��⴮�����
	char szReqPakSN[DEF_SBUFLEN];
	sprintf_s(szReqPakSN, sizeof(szReqPakSN), "%x:%x", GetCurrentThreadId(), m_dwReqPakSN);
	
	GetValue(m_pSingleRecvBuffer,szTemp, 11, '|', DEF_SBUFLEN);
	if (strcmp(szReqPakSN,szTemp)!=0)
	{
		m_pSocket->Clear(); //�������ִ����������Ϊ��λ�����Ի������ڵ�����ȫ�����
		SetLastError(-6,"Ӧ�������������ݡ�");
		return FALSE;
	}
	
	//�õ���������С
	GetValue(m_pSingleRecvBuffer, szTemp, 2, '|', DEF_SBUFLEN);
	dwDataLen = atol(szTemp);
	
	if (dwDataLen!=0)
	{//�������Ļ����ǲ��������ݶε�
		//�ٽ����������ݶ�
		iLen = dwDataLen;
		
		if (iPos+iLen>(m_iSingleRecvBufferSize-iHeadLen))
		{
			SetLastError(-7,"����Ӧ������󣡵���̫��! (������%d)", iLen);
			return FALSE;
		}
		
		if (m_pSocket->ReceiveBuf(m_pSingleRecvBuffer+iPos, iLen)!=iLen)
		{
			SetLastError(-7,"����Ӧ�������No.3 (������%d)", iLen);
			return FALSE;
		}
		iPos+=dwDataLen;
	}
	
	//�ж��Ƿ������쳣������ʣ�����ݣ�����о������Ϊ��һ�ν�����׼������ΪЭ������˳���ϵ
	if (m_pSocket->ReceiveLength()!=0)
	{
		m_pSocket->Clear();
		AfxMessageBox("���棺���ջ���������ʣ�����ݣ����������");
	}
	
	if (p_bCRC)
	{
		//���CRCУ����
		char szCRCCode_Answer[DEF_SBUFLEN]; 
		char szCRCCode[DEF_SBUFLEN];
		
		memset(szCRCCode_Answer,0,DEF_SBUFLEN);
		memset(szCRCCode,0,DEF_SBUFLEN);
		
		char *pCRC;
		pCRC = strchr(m_pSingleRecvBuffer,'|');
		if (pCRC==NULL)
		{ //CRCУ����ʧ�ܣ�
			SetLastError(-10,"����ʽ�Ƿ�1��ȱ��|");
			return FALSE;
		}
		pCRC++;
		pCRC = strchr(pCRC,'|');
		if (pCRC==NULL)
		{ //CRCУ����ʧ�ܣ�
			SetLastError(-10,"����ʽ�Ƿ�2��ȱ��|");
			return FALSE;
		}
		pCRC++;
		strncpy_s(szCRCCode_Answer, pCRC, 8);  //������ǰ��CRC��
		
		memcpy(pCRC, m_szWorkKey, 8); //д�빤����Կ
		CRC_Get((unsigned char *)m_pSingleRecvBuffer, iHeadLen+dwDataLen, (unsigned char *)szCRCCode);
		szCRCCode[8] = 0;
		if (strncmp(szCRCCode,szCRCCode_Answer,8)!=0)
		{ //CRCУ����ʧ�ܣ�
			SetLastError(-8,"CRCУ��ʧ�ܣ�");
			return FALSE;
		}
	}
	
	//  AfxMessageBox(m_pSingleRecvBuffer);
	
	GetValue(m_pSingleRecvBuffer, p_pAnswerPacket->szVersion, 4, '|', DEF_SBUFLEN);
	GetValue(m_pSingleRecvBuffer, p_pAnswerPacket->szRetCode, 5, '|', DEF_SBUFLEN);
	GetValue(m_pSingleRecvBuffer, p_pAnswerPacket->szRetMsg, 6, '|', DEF_SBUFLEN);
	
	GetValue(m_pSingleRecvBuffer, szTemp, 7, '|', DEF_SBUFLEN);//�Ƿ���������¼
	p_pAnswerPacket->bIsNext = atoi(szTemp);
	
	GetValue(m_pSingleRecvBuffer, szTemp, 8, '|', DEF_SBUFLEN);//�ֶ�����
	p_pAnswerPacket->iFieldNum = atoi(szTemp);
	
	GetValue(m_pSingleRecvBuffer, szTemp, 9, '|', DEF_SBUFLEN);//��¼����
	p_pAnswerPacket->iRecNum = atoi(szTemp);
	
	p_pAnswerPacket->iHeadLen = iHeadLen;
	p_pAnswerPacket->dwDataLen = dwDataLen;
	
	if (m_dwTotalDataLen<(m_dwDataBufferSize-dwDataLen))
	{//���ݿ��Էŵ���������
		m_dwRecNo += p_pAnswerPacket->iRecNum;
		
		memcpy(m_pReturnData+m_dwTotalDataLen, m_pSingleRecvBuffer+iHeadLen, dwDataLen);
		m_dwTotalDataLen+=dwDataLen;
		
		*(m_pReturnData+m_dwTotalDataLen) = 0;
	}
	
	return TRUE;
}

//---------------------------------------------------------------------------
//ǩ��KDGateway
BOOL CKDGateway::CheckIn()
{
	m_dwTotalDataLen = 0;
	
	//����ǩ���
	if (SendRequest("100")!=TRUE)
		return FALSE;
	
	tagAnswerPacket AnswerPacket;
	if (RecvAnswer(&AnswerPacket, 0)!=TRUE) //0��ʾ��У��CRC
		return FALSE;
	
	if (atol(AnswerPacket.szRetCode)!=0)
	{
		SetLastError(-8,"ǩ��ʧ��! ErrCode:%s ErrMsg:%s", AnswerPacket.szRetCode, AnswerPacket.szRetMsg);
		return FALSE;
	}
	
	char szWorkKey[256];
	memset(szWorkKey,0,256);
	GetValue(m_pSingleRecvBuffer+AnswerPacket.iHeadLen, szWorkKey, 2, '|', DEF_SBUFLEN);//��¼����  
	
	char szWorkKey_True[256];
	CBlowFish oBlowFish((unsigned char *)"SZKINGDOM", 9);
	oBlowFish.Decrypt((const unsigned char*)szWorkKey, (unsigned char*)szWorkKey_True, strlen(szWorkKey));
	
	strcpy_s(m_szWorkKey,szWorkKey_True);
	m_szWorkKey[8] = 0;
	return TRUE;
}

//---------------------------------------------------------------------------
//��������
BOOL CKDGateway::WaitAnswer(char *p_pszDataSect)
{
	m_dwTotalDataLen = 0;
	m_dwRecNo = 0;
	m_dwFieldNo = 0;	

	//��������
	if (SendRequest(p_pszDataSect)!=TRUE)
		return FALSE;
	
	//����Ӧ��
	tagAnswerPacket AnswerPacket;
	if (RecvAnswer(&AnswerPacket)!=TRUE)
		return FALSE;
	
	if (atol(AnswerPacket.szRetCode)!=0)
	{
		SetLastError(-8,"����ʧ�ܣ�ErrCode:%s ErrMsg:%s", AnswerPacket.szRetCode, AnswerPacket.szRetMsg);
		return FALSE;
	}
	
	while(AnswerPacket.bIsNext)
	{
		//��������
		if (SendRequest("99")!=TRUE) //ȡ��̰�
			return FALSE;
		
		//����Ӧ��
		if (RecvAnswer(&AnswerPacket)!=TRUE)
			return FALSE;
		
		if (atol(AnswerPacket.szRetCode)!=0)
		{
			SetLastError(-8,"����ʧ�ܣ�ErrCode:%s ErrMsg:%s", AnswerPacket.szRetCode, AnswerPacket.szRetMsg);
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
