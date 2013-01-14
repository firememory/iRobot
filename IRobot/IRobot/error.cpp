#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "Error.h"

//-------------------------------------------------------------------------
tagErrorMessageList g_ListError[] = {
	{ 0x00000001L             , "GEN:Out of Memory!"                    },
};
//-------------------------------------------------------------------------
CError::CError()
{
  m_dwErrorCode = 0;
  memset( m_szErrorMessage, 0, ERRMSGLEN);
}
//-------------------------------------------------------------------------
CError::~CError()
{
  return;
}
//-------------------------------------------------------------------------
char *CError::GetLastErrorMessage()
{ ///�õ�������Ϣ
  return m_szErrorMessage;
}
//-------------------------------------------------------------------------
DWORD CError::GetLastErrorCode()
{ ///�õ�������Ϣ
  return m_dwErrorCode;
}
//-------------------------------------------------------------------------
void CError::SetLastError( DWORD p_dwErrorCode, char *p_pszErrorMessageFMT, ...)
{///�������Ĵ���
  va_list arg_ptr;
  char szTemp[256];
  
//  printf("CError::SetLastError [%s]\n", p_pszErrorMessageFMT);
  
  va_start(arg_ptr, p_pszErrorMessageFMT);
  vsprintf_s(szTemp, p_pszErrorMessageFMT, arg_ptr);
  va_end(arg_ptr);
  
  m_dwErrorCode = p_dwErrorCode;
  strncpy_s( m_szErrorMessage, szTemp, ERRMSGLEN);
}
//-------------------------------------------------------------------------
char *CError::QueryErrorMessage(DWORD dwCode)
{///�õ�������Ϣ
	int iCount = sizeof(g_ListError)/sizeof(tagErrorMessageList);

	for (int i=0;i<iCount;i++)
	{
		if (g_ListError[i].dwCode==dwCode)
			return g_ListError[i].pszMessage;
	}
	
	m_szErrorMessage[ERRMSGLEN-1] = 0;
	return m_szErrorMessage+ERRMSGLEN-1; ///<���ؿմ�
}
//-------------------------------------------------------------------------
