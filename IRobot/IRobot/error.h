#ifndef __CERROR_H__
#define __CERROR_H__

#define ERRMSGLEN  80

//---------------------------------------------------------------------------
struct tagErrorMessageList
{
	DWORD dwCode;
	char *pszMessage;
};

//---------------------------------------------------------------------------
class CError  ///错误处理基类,所有要处理错的类都必须继承此类
{
private:
protected:
	DWORD m_dwErrorCode; ///<错误代码
	char m_szErrorMessage[ERRMSGLEN];
public:
  CError();
  ~CError();

  char *GetLastErrorMessage();     ///得到错误信息
  DWORD GetLastErrorCode();        ///得到错误代码
  void SetLastError( DWORD p_dwErrorCode, char *p_pszErrorMessageFMT, ...); ///<设置最后的错误

  char *QueryErrorMessage(DWORD dwCode);///查询错误代码对应的错误信息
};

#endif
