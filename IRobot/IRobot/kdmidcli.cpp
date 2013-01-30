#include "stdafx.h"
#include "kdmidcli.h"

/* ��֤�м���ͻ��˷�װ */

CKDMidCli::CKDMidCli()
{
  if (KCBPCLI_Init(&m_hHandle) != KCBP_MSG_OK)
  {
    m_hHandle = NULL;
  }

  m_iStatus = 0;
}

CKDMidCli::~CKDMidCli()
{
  if (m_hHandle != NULL)
  {
    KCBPCLI_Exit(m_hHandle);
  }
}

int CKDMidCli::GetHandle(KCBPCLIHANDLE &p_refhHandle)
{
  p_refhHandle = m_hHandle;

  return KCBP_MSG_OK;
}

int CKDMidCli::GetVersion(int *pnVersion)
{
  m_iErrNo = KCBPCLI_GetVersion(m_hHandle, pnVersion);

  return m_iErrNo;
}

int CKDMidCli::SetConnectOption(tagKCBPConnectOption stKCBPConnection)
{
  m_iErrNo = KCBPCLI_SetConnectOption(m_hHandle, stKCBPConnection);

  return m_iErrNo;
}

int CKDMidCli::GetConnectOption(tagKCBPConnectOption *pstKCBPConnection)
{
  m_iErrNo = KCBPCLI_GetConnectOption(m_hHandle, pstKCBPConnection);

  return m_iErrNo;
}

int CKDMidCli::ConnectServer(char *ServerName, char *UserName, char *Password)
{
  m_iErrNo = KCBPCLI_ConnectServer(m_hHandle, ServerName, UserName, Password);
  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iStatus = KCBP_CONNECT;
    memset((void *)m_szServerName, 0x00, sizeof(m_szServerName));
    memset((void *)m_szUserName,   0x00, sizeof(m_szUserName));
    memset((void *)m_szPassword,   0x00, sizeof(m_szPassword));

    strcpy_s(m_szServerName, ServerName);
    strcpy_s(m_szUserName, UserName);
    strcpy_s(m_szPassword, Password);
  }

  return m_iErrNo;
}

int CKDMidCli::ReConnectServer()
{
  m_iErrNo = KCBP_MSG_OK;

  if ((m_iStatus | KCBP_CONNECT) > 0)
  {
    m_iErrNo = KCBPCLI_DisConnect(m_hHandle);
    if (m_iErrNo == KCBP_MSG_OK)
    {
      m_iStatus = 0;
    }
  }

  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iErrNo = KCBPCLI_ConnectServer(m_hHandle, m_szServerName, m_szUserName, m_szPassword);
    if (m_iErrNo == KCBP_MSG_OK)
    {
      m_iStatus = KCBP_CONNECT;
    }
  }

  return m_iErrNo;
}

int CKDMidCli::DisConnectForce()
{
  m_iErrNo = KCBPCLI_DisConnectForce(m_hHandle);
  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iStatus = 0;
  }

  return m_iErrNo;
}

int CKDMidCli::CallProgramAndCommit(char *ProgramName)
{
  m_iErrNo = KCBPCLI_CallProgramAndCommit(m_hHandle, ProgramName);
  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iStatus |= KCBP_CALL_PROGRAM;
  }

  return m_iErrNo;
}

int CKDMidCli::CallProgram(char *ProgramName)
{
  m_iErrNo = KCBPCLI_CallProgram(m_hHandle, ProgramName);
  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iStatus |= KCBP_CALL_PROGRAM;
  }

  return m_iErrNo;
}

int CKDMidCli::Commit()
{
  return KCBPCLI_Commit(m_hHandle);
}

int CKDMidCli::RollBack()
{
  return KCBPCLI_RollBack(m_hHandle);
}


int CKDMidCli::ACallProgramAndCommit(char *ProgramName, tagCallCtrl *ptagCallCtrl)
{
  m_iErrNo = KCBPCLI_ACallProgramAndCommit(m_hHandle, ProgramName, ptagCallCtrl);
  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iStatus |= KCBP_CALL_PROGRAM;
  }

  return m_iErrNo;
}

int CKDMidCli::ACallProgram(char *ProgramName, tagCallCtrl *ptagCallCtrl)
{
  m_iErrNo = KCBPCLI_ACallProgram(m_hHandle, ProgramName, ptagCallCtrl);
  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iStatus |= KCBP_CALL_PROGRAM;
  }

  return m_iErrNo;
}

int CKDMidCli::Cancel(tagCallCtrl *ptagCallCtrl)
{
  return KCBPCLI_Cancel(m_hHandle, ptagCallCtrl);
}

int CKDMidCli::GetReply(tagCallCtrl *ptagCallCtrl)
{
  return KCBPCLI_GetReply(m_hHandle, ptagCallCtrl);
}


int CKDMidCli::BeginWrite()
{
  m_iErrNo = KCBPCLI_BeginWrite(m_hHandle);
  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iStatus |= KCBP_BEGIN_WRITE;
  }

  return m_iErrNo;
}


int CKDMidCli::GetVal(char *szKeyName, unsigned char **pValue, long *pSize)
{
  return KCBPCLI_GetVal(m_hHandle, szKeyName, pValue, pSize);
}


int CKDMidCli::SetVal(char *szKeyName, unsigned char *pValue, long nSize)
{
  m_iErrNo = KCBP_MSG_OK;

  // ���û�������������������֮
  if ((m_iStatus & KCBP_BEGIN_WRITE) == 0)
  {
    BeginWrite();
  }

  if (m_iErrNo == KCBP_MSG_OK)
  {
    m_iErrNo = KCBPCLI_SetVal(m_hHandle, szKeyName, pValue, nSize);
  }

  return m_iErrNo;
}


int CKDMidCli::GetValue(char *KeyName, char *p_lpszVal, int Len)
{
  return KCBPCLI_GetValue(m_hHandle, KeyName, p_lpszVal, Len);
}


int CKDMidCli::GetValue(char *KeyName, char  &p_refcVal)
{
  return KCBPCLI_GetValue(m_hHandle, KeyName, &p_refcVal, sizeof(char));
}


int CKDMidCli::GetValue(char *KeyName, short &p_refsiVal)
{
  char   szTemp[128];

  memset((void *)szTemp, 0x00, sizeof(szTemp));
  m_iErrNo = KCBPCLI_GetValue(m_hHandle, KeyName, szTemp, sizeof(szTemp));

  p_refsiVal = atoi(szTemp);

  return m_iErrNo;
}


int CKDMidCli::GetValue(char *KeyName, long &p_reflVal)
{
  char   szTemp[128];

  memset((void *)szTemp, 0x00, sizeof(szTemp));
  m_iErrNo = KCBPCLI_GetValue(m_hHandle, KeyName, szTemp, sizeof(szTemp));

  p_reflVal = atol(szTemp);

  return m_iErrNo;
}

int CKDMidCli::GetValue(char *KeyName, __int64 &p_refllVal)
{
  char   szTemp[128];

  memset((void *)szTemp, 0x00, sizeof(szTemp));
  m_iErrNo = KCBPCLI_GetValue(m_hHandle, KeyName, szTemp, sizeof(szTemp));

  p_refllVal = _atoi64(szTemp);

  return m_iErrNo;
}

int CKDMidCli::GetValue(char *KeyName, double &p_refdVal)
{
  char   szTemp[128];

  memset((void *)szTemp, 0x00, sizeof(szTemp));
  m_iErrNo = KCBPCLI_GetValue(m_hHandle, KeyName, szTemp, sizeof(szTemp));

  p_refdVal = atof(szTemp);

  return m_iErrNo;
}

int CKDMidCli::SetValue(char *KeyName, char *p_lpszVal)
{
  return KCBPCLI_SetValue(m_hHandle, KeyName, p_lpszVal);
}

int CKDMidCli::SetValue(char *KeyName, char  p_cVal)
{
  char   szTemp[128];
  memset((void *)szTemp, 0x00, sizeof(szTemp));

  szTemp[0] = p_cVal;
  return KCBPCLI_SetValue(m_hHandle, KeyName, szTemp);
}

int CKDMidCli::SetValue(char *KeyName, short p_siVal)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%d", p_siVal);

  return KCBPCLI_SetValue(m_hHandle, KeyName, szTemp);
}

int CKDMidCli::SetValue(char *KeyName, long  p_lVal)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%ld", p_lVal);

  return KCBPCLI_SetValue(m_hHandle, KeyName, szTemp);
}

int CKDMidCli::SetValue(char *KeyName, __int64 p_llVal)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%lld", p_llVal);

  return KCBPCLI_SetValue(m_hHandle, KeyName, szTemp);
}

int CKDMidCli::SetValue(char *KeyName, double  p_dVal, int p_iDec)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%.*f", p_iDec, p_dVal);

  return KCBPCLI_SetValue(m_hHandle, KeyName, szTemp);
}

// ��д��ά����
// ������һ�������
int CKDMidCli::RsCreate(char *Name, int ColNum, char *pColInfo)
{
  return KCBPCLI_RsCreate(m_hHandle, Name, ColNum, pColInfo);
}

// �����ڶ��������Ժ󣩽����
int CKDMidCli::RsNewTable(char *Name, int ColNum, char *pColInfo)
{
  return KCBPCLI_RsNewTable(m_hHandle, Name, ColNum, pColInfo);
}

// �ڵ�ǰ�����������һ��
int CKDMidCli::RsAddRow()
{
  return KCBPCLI_RsAddRow(m_hHandle);
}

// ���浱ǰ���ӵ���
int CKDMidCli::RsSaveRow()
{
  return KCBPCLI_RsSaveRow(m_hHandle);
}

// ������ֵ
// д������ֵ
int CKDMidCli::RsSetVal(int nColumnIndex, unsigned char *pValue, long nSize)
{
  return KCBPCLI_RsSetVal(m_hHandle, nColumnIndex, pValue, nSize);
}

int CKDMidCli::RsSetValByName(char *szColumnName, unsigned char *pValue, long nSize)
{
  return KCBPCLI_RsSetValByName(m_hHandle, szColumnName, pValue, nSize);
}

// дָ������ֵ
int CKDMidCli::RsSetColByName(char *Name, char *p_lpszVal)
{
  return KCBPCLI_RsSetColByName(m_hHandle, Name, p_lpszVal);
}

int CKDMidCli::RsSetCol(int Col, char   *p_lpszVal)
{
  return KCBPCLI_RsSetCol(m_hHandle, Col, p_lpszVal);
}

int CKDMidCli::RsSetCol(int Col, char   p_cVlu)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%c", p_cVlu);

  return KCBPCLI_RsSetCol(m_hHandle, Col, szTemp);
}

int CKDMidCli::RsSetCol(int Col, short  p_siVlu)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%d", p_siVlu);

  return KCBPCLI_RsSetCol(m_hHandle, Col, szTemp);
}

int CKDMidCli::RsSetCol(int Col, long   p_lVlu)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%ld", p_lVlu);

  return KCBPCLI_RsSetCol(m_hHandle, Col, szTemp);
}

int CKDMidCli::RsSetCol(int Col, __int64  p_llVlu)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%lld", p_llVlu);

  return KCBPCLI_RsSetCol(m_hHandle, Col, szTemp);
}

int CKDMidCli::RsSetCol(int Col, double p_dVlu, int p_iDec)
{
  char   szTemp[128];

  sprintf_s(szTemp, "%.*f", p_iDec, p_dVlu);

  return KCBPCLI_RsSetCol(m_hHandle, Col, szTemp);
}

// ����ά��
// �򿪽����, ����0��ʾ�ɹ�������ȷ������������100��ʾ�ɹ�������ȷ������
int CKDMidCli::RsOpen()
{
  return KCBPCLI_RsOpen(m_hHandle);
}

// ȡ��һ�������
int CKDMidCli::RsMore()
{
  return KCBPCLI_RsMore(m_hHandle);
}

// �رս������������ռ����Դ
int CKDMidCli::RsClose()
{
  return KCBPCLI_RsClose(m_hHandle);
}

// ȡ��ǰ���������
int CKDMidCli::RsGetCursorName(char *pszCursorName, int nLen)
{
  return KCBPCLI_RsGetCursorName(m_hHandle, pszCursorName, nLen);
}

// ȡ��ǰ�����ȫ��������
int CKDMidCli::RsGetColNames(char *pszInfo, int nLen)
{
  return KCBPCLI_RsGetColNames(m_hHandle, pszInfo, nLen);
}

// ȡ��N������
int CKDMidCli::RsGetColName(int nCol, char *pszName, int nLen)
{
  return KCBPCLI_RsGetColName(m_hHandle, nCol, pszName, nLen);
}

// ȡһ��
int CKDMidCli::RsFetchRow()
{
  return KCBPCLI_RsFetchRow(m_hHandle);
}

// ȡ���������, RsOpen����100ʱ�������ô˺���
int CKDMidCli::RsGetRowNum(int *pnRows)
{
  return KCBPCLI_RsGetRowNum(m_hHandle, pnRows);
}

// ȡ����
int CKDMidCli::RsGetColNum(int *pnCols)
{
  return KCBPCLI_RsGetColNum(m_hHandle, pnCols);
}

// ȡָ�����������
int CKDMidCli::RsGetTableRowNum(int nt, int *pnRows)
{
  return KCBPCLI_RsGetTableRowNum(m_hHandle, nt, pnRows);
}

// ȡָ�����������
int CKDMidCli::RsGetTableColNum(int nt, int *pnCols)
{
  return KCBPCLI_RsGetTableColNum(m_hHandle, nt, pnCols);
}

// ȡһ��
int CKDMidCli::RsGetCol(int Col, char *Vlu)
{
  return KCBPCLI_RsGetCol(m_hHandle, Col, Vlu);
}

int CKDMidCli::RsGetColByName(char *KeyName, char *Vlu)
{
  return KCBPCLI_RsGetColByName(m_hHandle, KeyName, Vlu);
}

int CKDMidCli::RsGetVal(int nColumnIndex, unsigned char **pValue, long *pSize)
{
  return KCBPCLI_RsGetVal(m_hHandle, nColumnIndex, pValue, pSize);
}

int CKDMidCli::RsGetValByName(char *szColumnName, unsigned char **pValue, long *pSize)
{
  return KCBPCLI_RsGetValByName(m_hHandle, szColumnName, pValue, pSize);
}

int CKDMidCli::RsGetCol(int Col, char *Vlu,   int p_iSize)
{
  char   szTemp[8192];

  memset(szTemp, 0x00, sizeof(szTemp));
  m_iErrNo = KCBPCLI_RsGetCol(m_hHandle, Col, szTemp);
  if (m_iErrNo == KCBP_MSG_OK)
  {
    if (strlen(szTemp) > (size_t)p_iSize)
    {
      strncpy_s(Vlu, p_iSize, szTemp, p_iSize);
    }
    else
    {
      strcpy_s(Vlu, strlen(szTemp), szTemp);
    }
  }

  return m_iErrNo;
}

int CKDMidCli::RsGetCol(int Col, char  &p_refcVlu)
{
  char   szTemp[128];

  memset(szTemp, 0x00, sizeof(szTemp));

  m_iErrNo = KCBPCLI_RsGetCol(m_hHandle, Col, szTemp);
  p_refcVlu = szTemp[0];

  return m_iErrNo;
}

int CKDMidCli::RsGetCol(int Col, long  &p_reflVlu)
{
  char   szTemp[128];

  memset(szTemp, 0x00, sizeof(szTemp));

  m_iErrNo = KCBPCLI_RsGetCol(m_hHandle, Col, szTemp);
  p_reflVlu = atol(szTemp);

  return m_iErrNo;
}

int CKDMidCli::RsGetCol(int Col, __int64 &p_refllVlu)
{
  char   szTemp[128];

  memset(szTemp, 0x00, sizeof(szTemp));

  m_iErrNo = KCBPCLI_RsGetCol(m_hHandle, Col, szTemp);
  p_refllVlu = _atoi64(szTemp);

  return m_iErrNo;
}

int CKDMidCli::RsGetCol(int Col, double &p_refdVlu)
{
  char   szTemp[128];

  memset(szTemp, 0x00, sizeof(szTemp));

  m_iErrNo = KCBPCLI_RsGetCol(m_hHandle, Col, szTemp);
  p_refdVlu = atof(szTemp);

  return m_iErrNo;
}

// ȡ������Ϣ
int CKDMidCli::GetErr(int *pErrCode, char *ErrMsg)
{
  return KCBPCLI_GetErr(m_hHandle, pErrCode, ErrMsg);
}

int CKDMidCli::GetErrorCode(int *pnErrno)
{
  return KCBPCLI_GetErrorCode(m_hHandle, pnErrno);
}

int CKDMidCli::GetErrorMsg(char *szError)
{
  return KCBPCLI_GetErrorMsg(m_hHandle, szError);
}

// ȡ��������������
int CKDMidCli::GetCommLen(int *pnLen)
{
  return KCBPCLI_GetCommLen(m_hHandle, pnLen);
}

// ���ó�ʱ��
int CKDMidCli::SetCliTimeOut(int TimeOut)
{
  return KCBPCLI_SetCliTimeOut(m_hHandle, TimeOut);
}

// ���ò���
int CKDMidCli::SetOption(int nIndex, void *pValue)
{
  return KCBPCLI_SetOption(m_hHandle, nIndex, pValue);
}

int CKDMidCli::SetOptions(int nIndex, void *pValue, int nLen)
{
  return KCBPCLI_SetOptions(m_hHandle, nIndex, pValue, nLen);
}

// ȡ��������
int CKDMidCli::GetOption(int nIndex, void *pValue)
{
  return KCBPCLI_GetOption(m_hHandle, nIndex, pValue);
}

int CKDMidCli::GetOptions(int nIndex, void *pValue, int *nLen)
{
  return KCBPCLI_GetOptions(m_hHandle, nIndex, pValue, nLen);
}

// ����ϵͳͨѶ����
int CKDMidCli::SetSystemParam(int nIndex, char *szValue)
{
  return KCBPCLI_SetSystemParam(m_hHandle, nIndex, szValue);
}

// ȡϵͳͨѶ����
int CKDMidCli::GetSystemParam(int nIndex, char *szValue, int nLen)
{
  return KCBPCLI_GetSystemParam(m_hHandle, nIndex, szValue, nLen);
}

