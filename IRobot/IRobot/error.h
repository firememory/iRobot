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
class CError  ///���������,����Ҫ�������඼����̳д���
{
private:
protected:
	DWORD m_dwErrorCode; ///<�������
	char m_szErrorMessage[ERRMSGLEN];
public:
  CError();
  ~CError();

  char *GetLastErrorMessage();     ///�õ�������Ϣ
  DWORD GetLastErrorCode();        ///�õ��������
  void SetLastError( DWORD p_dwErrorCode, char *p_pszErrorMessageFMT, ...); ///<�������Ĵ���

  char *QueryErrorMessage(DWORD dwCode);///��ѯ��������Ӧ�Ĵ�����Ϣ
};

#endif
