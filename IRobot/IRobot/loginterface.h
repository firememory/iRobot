#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H

#include "IRobotDlg.h"

#ifndef WIN32
#include <pthread.h>

struct CRITICAL_SECTION
{
	private:
		pthread_mutex_t datamp;
	public:
		CRITICAL_SECTION()
		{
			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
			pthread_mutex_init(&datamp, &attr);
			pthread_mutexattr_destroy(&attr);
			
		}

		~CRITICAL_SECTION()
		{
			pthread_mutex_destroy(&datamp);
		}

		void acquire()
		{
			pthread_mutex_lock(&datamp);

		}

		void release()
		{
			pthread_mutex_unlock(&datamp);
		}
			
};

#define InitialzeCriticalSection
#define DeleteCriticalSection
#define EnterCriticalSection(lock) {lock.acquire();}
#define LeaveCriticalSection(lock) {lock.release();}
#endif

#define MAX_LOG_SIZE 1024*1024*10  // 10 M
#define MAX_LOG_FILE_CNT 10	  // MAX 10 log files
#define MAX_FILE_PATH 100

#ifndef WIN32
#define TRACE(x)\
{\
	printf("%s\n", x);\
}
#endif

enum LOGLEVEL
{
	LOG_NOTIFY = 0, // 提示信息, 只显示测试用例的执行结果
	LOG_WARN,		// 告警信息, 在提示信息的基础上，增加显示检查点的执行情况
	LOG_DEBUG,		// 调试信息, 只有在WriteRunLogEx()函数中使用,打印函数执行的调试信息
	MAX_LOG_LEVEL = LOG_DEBUG + 1
};

enum LOG_MODE
{
	SYS_MODE = 0, // 系统日志
	MID_MODE, // MID 接口日志
	KCXP_MODE // KCXP接口日志
};

struct LEVEL_DESCRIPTION
{
	LOGLEVEL level;
	char description[10];
};

class CLoginterface
{
public:
	int m_nLogLevel;
	int m_nLogSize;
	int m_nLogFileCnt;
	int m_nLogIdx;
	int m_nShowLineCnt;

	char m_szLogFileName[MAX_FILE_PATH];
	char* m_pBuf;
	LEVEL_DESCRIPTION m_stLogLevelDescription[4];
	FILE* m_pFp;
	CRITICAL_SECTION m_lock;
	CIRobotDlg *m_pDlg;
	
public:
	CLoginterface(char *);
	virtual ~CLoginterface();

	void SetLogLevel(int);
	void WriteRunLogEx(char*, int, const char *, ...);
	void WriteRunLog(int, int, const char *, ...);
	void FlushLogBuf();

	CIRobotDlg * GetDlg() const { return m_pDlg; }
	void SetDlg(CIRobotDlg * val) { m_pDlg = val; }
};
#endif
