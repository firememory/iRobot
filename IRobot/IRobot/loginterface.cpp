#include "stdafx.h"
#include "loginterface.h"
#include "public.h"

#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <WinBase.h>


CLoginterface::CLoginterface(char *pLogPath)
{
	InitializeCriticalSection(&m_lock);
	
	m_nLogLevel = LOG_NOTIFY;
	m_nLogFileCnt = MAX_LOG_FILE_CNT;
	m_nLogSize = MAX_LOG_SIZE;
	m_nLogIdx = 0;

	ZEROMEM(m_szLogFileName, MAX_FILE_PATH);
	STRNCPY_EX(m_szLogFileName,pLogPath, MAX_FILE_PATH);

	m_pBuf = new char[MAX_LOG_SIZE];
	ZEROMEM(m_pBuf, MAX_LOG_SIZE);
	
	m_stLogLevelDescription[0].level = LOG_NOTIFY;
	STRNCPY_EX(m_stLogLevelDescription[0].description, "NOTIFY", 10);
	
	m_stLogLevelDescription[1].level = LOG_WARN;
	STRNCPY_EX(m_stLogLevelDescription[1].description, "WARN", 10);
	
	m_stLogLevelDescription[2].level = LOG_EMERGENT;
	STRNCPY_EX(m_stLogLevelDescription[2].description, "EMERGENT", 10);
		
	m_stLogLevelDescription[3].level = LOG_DEBUG;
 	STRNCPY_EX(m_stLogLevelDescription[3].description, "DEBUG", 10);
	
	// create log file
	FILE *fp = NULL;
	fopen_s(&fp, m_szLogFileName, "a+");
	if(NULL == fp)
	{
		TRACE("create file kcbp_cjis.log error!");
		m_pFp = NULL;
	}
	else
	{
		m_pFp = fp;
	}
}

CLoginterface::~CLoginterface()
{
	//DELCLS(m_pTimeBuf);
	DELARY(m_pBuf);

	if(NULL != m_pFp)
	{
		fclose(m_pFp);
		m_pFp = NULL;
	}
}

void CLoginterface::SetLogLevel(int nLevel)
{
	WriteRunLog(__FILE__, __LINE__, LOG_NOTIFY, "set log to level %s", m_stLogLevelDescription[nLevel].description);
	//printf("set log to %s\n", m_stLogLevelDescription[nLevel].description);

	m_nLogLevel = nLevel;
}

void CLoginterface::WriteRunLog(char* pszFile, int nLineNum, int nLogLevel, const char *msg, ...)
{
	if(nLogLevel < m_nLogLevel)
	{
		return;
	}
	
	const int nTmpBufSize = 1024;
	char msgStr[nTmpBufSize]={0};
	char bufStr[nTmpBufSize]={0};
	char timeStr[nTmpBufSize]={0};

	ZEROMEM(msgStr,nTmpBufSize);
	ZEROMEM(bufStr,nTmpBufSize);
	ZEROMEM(timeStr,nTmpBufSize);

	// get user input
	va_list arg_ptr;
	va_start(arg_ptr, msg);
	vsprintf_s(msgStr, msg, arg_ptr);
	va_end(arg_ptr);
	
	// get current time
	time_t 	NowTime;
  	struct timeb timebuffer;
  	tm stCurTm;
	tm *pTm= &stCurTm;

	time( &NowTime );
	ftime( &timebuffer );
  
	localtime_s(pTm, &NowTime);
	sprintf_s(timeStr, "%4d%02d%02d %02d:%02d:%02d",
		pTm->tm_year+1900, 
	    pTm->tm_mon+1,
	    pTm->tm_mday, 
	    pTm->tm_hour,
	    pTm->tm_min,
	    pTm->tm_sec); 
	
	sprintf_s( timeStr, "%s.%03hu", timeStr, timebuffer.millitm );
	
	// compose the log string
	_snprintf_s(bufStr, nTmpBufSize, "%s [%s][%s:%d][%s]\n", timeStr, m_stLogLevelDescription[nLogLevel].description, pszFile, nLineNum, msgStr);

	// if buf is larger than 10 M
	
	EnterCriticalSection(&m_lock);
	int nBufUsedSize = strlen(m_pBuf);
	int nLenBufStr = strlen(bufStr);
	printf("%s:%d $ nBufUsedSize=%d, nLenBufStr=%d, msgStr=%s\n", pszFile, nLineNum, nBufUsedSize, nLenBufStr, bufStr);

	if (nBufUsedSize >= MAX_LOG_SIZE || (nBufUsedSize+nLenBufStr) >= MAX_LOG_SIZE)
	{
		printf("=====%s:%d $ size1 = %d\n", pszFile, nLineNum, nBufUsedSize);
		FlushLogBuf();
	}

	printf("m_pBuf=%p\n"
	"%s\n", this, m_pBuf);
	
	_snprintf_s(m_pBuf, MAX_LOG_SIZE, _TRUNCATE, "%s%s", m_pBuf, bufStr);
	LeaveCriticalSection(&m_lock);
}

void CLoginterface::FlushLogBuf()
{
	if(NULL != m_pFp)
	{
		fprintf(m_pFp, "%s", m_pBuf);
	}
	else
	{
		// re-create the log file
		fopen_s(&m_pFp, m_szLogFileName, "a+");
		if(NULL == m_pFp)
		{
			printf("create log file %s failed!", m_szLogFileName);
			return;
		}
		fprintf(m_pFp, "%s", m_pBuf);
	}
	
	// Maximum 11 log files, if over 11 log files, then log will looply cover the last log file
	if(MAX_LOG_FILE_CNT < m_nLogIdx)
	{		
		return;
	}
	else
	{
		fclose(m_pFp);
		m_pFp = NULL;

		// make an new log file
		m_nLogIdx++;
		sprintf_s(m_szLogFileName, "kcbp_cjis%02d", m_nLogIdx);
		fopen_s(&m_pFp, m_szLogFileName, "a+");
		if(NULL == m_pFp)
		{
			printf("create log file %s failed!", m_szLogFileName);
		}
	}
}