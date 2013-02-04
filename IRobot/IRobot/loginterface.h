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
	LOG_NOTIFY = 0, // ��ʾ��Ϣ, ֻ��ʾ����������ִ�н��
	LOG_WARN,		// �澯��Ϣ, ����ʾ��Ϣ�Ļ����ϣ�������ʾ�����ִ�����
	LOG_DEBUG,		// ������Ϣ, ֻ����WriteRunLogEx()������ʹ��,��ӡ����ִ�еĵ�����Ϣ
	MAX_LOG_LEVEL = LOG_DEBUG + 1
};

enum LOG_MODE
{
	SYS_MODE = 0, // ϵͳ��־
	MID_MODE, // MID �ӿ���־
	KCXP_MODE // KCXP�ӿ���־
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
