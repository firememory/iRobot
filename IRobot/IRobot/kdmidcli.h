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

// 金证中间件客户端类
class CKDMidCli
{
  public:
    CKDMidCli();
    ~CKDMidCli();

    // 取回句柄
    int GetHandle(KCBPCLIHANDLE &p_refhHandle);

    // 取KCBP API版本号
    int GetVersion(int *pnVersion);
    // 设置连接参数
    int SetConnectOption(tagKCBPConnectOption stKCBPConnection);
    // 获取连接参数
    int GetConnectOption(tagKCBPConnectOption *pstKCBPConnection);
    // 连接KCBP
    int ConnectServer(char *ServerName, char *UserName, char *Password);
    // 重新连接KCBP
    int ReConnectServer();
    // 强制断开连接(不向服务端发送断开消息)
    int DisConnectForce();

    // 同步调用
    // 调用LBM，且提交事务
    int CallProgramAndCommit(char *ProgramName);
    // 调用LBM，不提交事务
    int CallProgram(char *ProgramName);
    // 提交事务
    int Commit();
    // 回滚事务
    int RollBack();

    // 异步调用
    // 调用LBM，且提交事务
    int ACallProgramAndCommit(char *ProgramName, tagCallCtrl *ptagCallCtrl);
    // 调用LBM，不提交事务
    int ACallProgram(char *ProgramName, tagCallCtrl *ptagCallCtrl);
    // 取消异步调用
    int Cancel(tagCallCtrl *ptagCallCtrl);
    // 查询异步调用结果
    int GetReply(tagCallCtrl *ptagCallCtrl);

    // 清除公共通讯数据区
    int BeginWrite();

    // 读写0维表方法
    // 读写二进制数据方法
    int GetVal(char *szKeyName, unsigned char **pValue, long *pSize);
    int SetVal(char *szKeyName, unsigned char *pValue, long nSize);
    // 读写字符串方法
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

    // 读写二维表方法
    // 创建第一个结果集
    int RsCreate(char *Name, int ColNum, char *pColInfo);
    // 创建第二个（或以后）结果集
    int RsNewTable(char *Name, int ColNum, char *pColInfo);
    // 在当前结果集中增加一行
    int RsAddRow();
    // 保存当前增加的行
    int RsSaveRow();

    // 设置列值
    // 写二进制值
    int RsSetVal(int nColumnIndex, unsigned char *pValue, long nSize);
    int RsSetValByName(char *szColumnName, unsigned char *pValue, long nSize);
    // 写指定类型值
    int RsSetColByName(char *Name, char *p_lpszVal);
    int RsSetCol(int Col, char   *p_lpszVal);
    int RsSetCol(int Col, char   p_cVlu);
    int RsSetCol(int Col, short  p_siVlu);
    int RsSetCol(int Col, long   p_lVlu);
    int RsSetCol(int Col, __int64  p_llVlu);
    int RsSetCol(int Col, double p_dVlu, int p_iDec = 2);

    // 读二维表
    // 打开结果集, 返回0表示成功并可以确认行数，返回100表示成功但不能确认行数
    int RsOpen();
    // 取下一个结果集
    int RsMore();
    // 关闭结果集并清算所占用资源
    int RsClose();
    // 取当前结果集名称
    int RsGetCursorName(char *pszCursorName, int nLen);
    // 取当前结果集全部列名称
    int RsGetColNames(char *pszInfo, int nLen);
    // 取第N列名称
    int RsGetColName(int nCol, char *pszName, int nLen);
    // 取一行
    int RsFetchRow();
    // 取结果集行数, RsOpen返回100时，不能用此函数
    int RsGetRowNum(int *pnRows);
    // 取列数
    int RsGetColNum(int *pnCols);
    // 取指定结果集行数
    int RsGetTableRowNum(int nt, int *pnRows);
    // 取指定结果集列数
    int RsGetTableColNum(int nt, int *pnCols);

    // 取一列
    int RsGetCol(int Col, char *Vlu);
    int RsGetColByName(char *KeyName, char *Vlu);
    int RsGetVal(int nColumnIndex, unsigned char **pValue, long *pSize);
    int RsGetValByName(char *szColumnName, unsigned char **pValue, long *pSize);
    int RsGetCol(int Col, char *Vlu,   int p_iSize);
    int RsGetCol(int Col, char  &p_refcVlu);
    int RsGetCol(int Col, long  &p_reflVlu);
    int RsGetCol(int Col, __int64 &p_refllVlu);
    int RsGetCol(int Col, double &p_refdVlu);

    // 取错误信息
    int GetErr(int *pErrCode, char *ErrMsg);
    int GetErrorCode(int *pnErrno);
    int GetErrorMsg(char *szError);

    // 取公共数据区长度
    int GetCommLen(int *pnLen);
    // 设置超时间
    int SetCliTimeOut(int TimeOut);
    // 设置参数
    int SetOption(int nIndex, void *pValue);
    int SetOptions(int nIndex, void *pValue, int nLen);
    // 取参数设置
    int GetOption(int nIndex, void *pValue);
    int GetOptions(int nIndex, void *pValue, int *nLen);

    // 设置系统通讯参数
    int SetSystemParam(int nIndex, char *szValue);
    // 取系统通讯参数
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
