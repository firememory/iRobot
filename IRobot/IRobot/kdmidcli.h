#include "KCBPCli.h"

#ifndef  __KD_MID_CLI_H
#define  __KD_MID_CLI_H

#define  KCBP_MSG_OK   0
#define  KCBP_USERNAME_MAX  31
#define  KCBP_PASSOWRD_MAX  31

#define  KD_MID_ERR         -9999
#define  KD_MID_MSG_MAX     127

// m_iStatus
#define  KCBP_CONNECT        0x01
#define  KCBP_CALL_PROGRAM   0x02
#define  KCBP_BEGIN_WRITE    0x04

// ��֤�м���ͻ�����
class CKDMidCli
{
  public:
    CKDMidCli();
    ~CKDMidCli();

    // ȡ�ؾ��
    int GetHandle(KCBPCLIHANDLE &p_refhHandle);

    // ȡKCBP API�汾��
    int GetVersion(int *pnVersion);
    // �������Ӳ���
    int SetConnectOption(tagKCBPConnectOption stKCBPConnection);
    // ��ȡ���Ӳ���
    int GetConnectOption(tagKCBPConnectOption *pstKCBPConnection);
    // ����KCBP
    int ConnectServer(char *ServerName, char *UserName, char *Password);
    // ��������KCBP
    int ReConnectServer();
    // ǿ�ƶϿ�����(�������˷��ͶϿ���Ϣ)
    int DisConnectForce();

    // ͬ������
    // ����LBM�����ύ����
    int CallProgramAndCommit(char *ProgramName);
    // ����LBM�����ύ����
    int CallProgram(char *ProgramName);
    // �ύ����
    int Commit();
    // �ع�����
    int RollBack();

    // �첽����
    // ����LBM�����ύ����
    int ACallProgramAndCommit(char *ProgramName, tagCallCtrl *ptagCallCtrl);
    // ����LBM�����ύ����
    int ACallProgram(char *ProgramName, tagCallCtrl *ptagCallCtrl);
    // ȡ���첽����
    int Cancel(tagCallCtrl *ptagCallCtrl);
    // ��ѯ�첽���ý��
    int GetReply(tagCallCtrl *ptagCallCtrl);

    // �������ͨѶ������
    int BeginWrite();

    // ��д0ά����
    // ��д���������ݷ���
    int GetVal(char *szKeyName, unsigned char **pValue, long *pSize);
    int SetVal(char *szKeyName, unsigned char *pValue, long nSize);
    // ��д�ַ�������
    int GetValue(char *KeyName, char *p_lpszVal, int Len);
    int GetValue(char *KeyName, char  &p_refcVal);
    int GetValue(char *KeyName, short &p_refsiVal);
    int GetValue(char *KeyName, long &p_reflVal);
    int GetValue(char *KeyName, __int64 &p_refllVal);
    int GetValue(char *KeyName, double &p_refdVal);
    int SetValue(char *KeyName, char *p_lpszVal);
    int SetValue(char *KeyName, char  p_cVal);
    int SetValue(char *KeyName, short p_siVal);
    int SetValue(char *KeyName, long  p_lVal);
    int SetValue(char *KeyName, __int64 p_llVal);
    int SetValue(char *KeyName, double  p_dVal, int p_iDec = 2);

    // ��д��ά����
    // ������һ�������
    int RsCreate(char *Name, int ColNum, char *pColInfo);
    // �����ڶ��������Ժ󣩽����
    int RsNewTable(char *Name, int ColNum, char *pColInfo);
    // �ڵ�ǰ�����������һ��
    int RsAddRow();
    // ���浱ǰ���ӵ���
    int RsSaveRow();

    // ������ֵ
    // д������ֵ
    int RsSetVal(int nColumnIndex, unsigned char *pValue, long nSize);
    int RsSetValByName(char *szColumnName, unsigned char *pValue, long nSize);
    // дָ������ֵ
    int RsSetColByName(char *Name, char *p_lpszVal);
    int RsSetCol(int Col, char   *p_lpszVal);
    int RsSetCol(int Col, char   p_cVlu);
    int RsSetCol(int Col, short  p_siVlu);
    int RsSetCol(int Col, long   p_lVlu);
    int RsSetCol(int Col, __int64  p_llVlu);
    int RsSetCol(int Col, double p_dVlu, int p_iDec = 2);

    // ����ά��
    // �򿪽����, ����0��ʾ�ɹ�������ȷ������������100��ʾ�ɹ�������ȷ������
    int RsOpen();
    // ȡ��һ�������
    int RsMore();
    // �رս������������ռ����Դ
    int RsClose();
    // ȡ��ǰ���������
    int RsGetCursorName(char *pszCursorName, int nLen);
    // ȡ��ǰ�����ȫ��������
    int RsGetColNames(char *pszInfo, int nLen);
    // ȡ��N������
    int RsGetColName(int nCol, char *pszName, int nLen);
    // ȡһ��
    int RsFetchRow();
    // ȡ���������, RsOpen����100ʱ�������ô˺���
    int RsGetRowNum(int *pnRows);
    // ȡ����
    int RsGetColNum(int *pnCols);
    // ȡָ�����������
    int RsGetTableRowNum(int nt, int *pnRows);
    // ȡָ�����������
    int RsGetTableColNum(int nt, int *pnCols);

    // ȡһ��
    int RsGetCol(int Col, char *Vlu);
    int RsGetColByName(char *KeyName, char *Vlu);
    int RsGetVal(int nColumnIndex, unsigned char **pValue, long *pSize);
    int RsGetValByName(char *szColumnName, unsigned char **pValue, long *pSize);
    int RsGetCol(int Col, char *Vlu,   int p_iSize);
    int RsGetCol(int Col, char  &p_refcVlu);
    int RsGetCol(int Col, long  &p_reflVlu);
    int RsGetCol(int Col, __int64 &p_refllVlu);
    int RsGetCol(int Col, double &p_refdVlu);

    // ȡ������Ϣ
    int GetErr(int *pErrCode, char *ErrMsg);
    int GetErrorCode(int *pnErrno);
    int GetErrorMsg(char *szError);

    // ȡ��������������
    int GetCommLen(int *pnLen);
    // ���ó�ʱ��
    int SetCliTimeOut(int TimeOut);
    // ���ò���
    int SetOption(int nIndex, void *pValue);
    int SetOptions(int nIndex, void *pValue, int nLen);
    // ȡ��������
    int GetOption(int nIndex, void *pValue);
    int GetOptions(int nIndex, void *pValue, int *nLen);

    // ����ϵͳͨѶ����
    int SetSystemParam(int nIndex, char *szValue);
    // ȡϵͳͨѶ����
    int GetSystemParam(int nIndex, char *szValue, int nLen);

  private:
    int  m_iStatus;
    int  m_iErrNo;
    char m_szErrMsg[KD_MID_MSG_MAX+1];

  private:
    KCBPCLIHANDLE  m_hHandle;
    char m_szUserName[KCBP_USERNAME_MAX+1];
    char m_szServerName[KCBP_SERVERNAME_MAX + 1];
    char m_szPassword[KCBP_PASSOWRD_MAX+1];

};

#endif
