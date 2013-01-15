#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H

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
	LOG_NOTIFY,
	LOG_WARN,
	LOG_EMERGENT,
	LOG_DEBUG
};

struct LEVEL_DESCRIPTION
{
	LOGLEVEL level;
	char description[10];
};


class CLoginterface
{
	int m_nLogLevel;
	int m_nLogSize;
	int m_nLogFileCnt;
	int m_nLogIdx;

	char m_szLogFileName[MAX_FILE_PATH];
	char* m_pBuf;
	LEVEL_DESCRIPTION m_stLogLevelDescription[4];
	FILE* m_pFp;
	CRITICAL_SECTION m_lock;
	

public:
	CLoginterface(char *);
	~CLoginterface();

	void SetLogLevel(int);
	void WriteRunLog(char*, int, int, const char *, ...);
	void FlushLogBuf();
};
#endif
