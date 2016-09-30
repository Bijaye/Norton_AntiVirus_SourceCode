// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: NetWare implementation of Windows thread management functions

extern "C" {
#include <assert.h>

#if defined NLM // && 0                                  // TODO remove before shipping !
	#define EnterDebugger() asm { int 3 }
#else
	#define EnterDebugger()
#endif

#if defined LINUX
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <signal.h>
	#include <errno.h>
#elif defined SYM_LIBC_NLM
	#include <library.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>
#elif defined NLM
	#include <nwthread.h>
	#include <nwsemaph.h>
	#include <time.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <nwerrno.h>
	#include <errno.h>
	#include <stdio.h>

	typedef unsigned long pthread_mutex_t;
	typedef unsigned long pthread_cond_t;
#endif
}

#include <new>
#include <list>
#include <map>

using namespace std;

#include "winStrings.h"
#include "winThreads.h"

#include "FineTime.h"

#ifdef LINUX   
extern "C" {
	#include "dprintf.h"
   #include "dprintf_2.h"
}
#endif

//#ifdef SYM_LIBC_NLM
// #include "DebugPrint.h"
//#endif

#undef DBG
#ifdef DBG_THREADS
	#define DBG
#else
	#define DBG return
#endif

// ================== Errors ============================================================

static DWORD getError();
static void	 setError(DWORD dwErrCode);

#if defined LINUX

	// We have three kinds of threads when running Linux:
	//
	//	   1	 Created by `ndkThreads' `BeginThread' or `BeginThreadGroup'.
	//	   2	 Created by `winThreads' `_beginthread' or `_beginthreadex'.
	//	   3	 Created directly by `pthread_create'.
	//
	//	The Windows/NDK inspired thread safe `Get/SetLastError' functions are supported by
	//	directing the underlying error code handler for each respective thread type
	//	depending on the presence of the thread identifier in the two thread data maps:
	//
	//		1	 ndkThreads.cpp:ThreadData::ThreadDataMap
	//		2	 winThreads.cpp:Thread::ThreadMap
	//		3	 neither

   DWORD  ndkGetLastError(HANDLE hThreadData);
   void   ndkSetLastError(HANDLE hThreadData,DWORD dwErrCode);
	HANDLE ndkGetThreadData();

	DWORD GetLastError() {
		HANDLE hThreadData = ndkGetThreadData();
		return hThreadData ? ndkGetLastError(hThreadData) : getError();
	}
   void SetLastError(DWORD dwErrCode) {
		HANDLE hThreadData = ndkGetThreadData();
		if (hThreadData)
			ndkSetLastError(hThreadData,dwErrCode);
		else
			setError(dwErrCode);
	}
#elif defined NLM && ! SYM_LIBC_NLM
   DWORD GetLastError() {
		return getError();
	}
   void	SetLastError(DWORD dwErrCode) {
		setError(dwErrCode);
	}
#endif

#if defined SYM_LIBC_NLM
   #include "scssock.h"

	int WSAGetLastError() {
		DWORD ec = GetLastError();

		switch (ec) {
		case EWOULDBLOCK:	  ec = WSABASEERR + EWOULDBLOCK;	  break;
		case ENOTSOCK:		  ec = WSABASEERR + ENOTSOCK;		  break;
		case EAFNOSUPPORT:  ec = WSABASEERR + EAFNOSUPPORT;  break;
		case EADDRINUSE:	  ec = WSABASEERR + EADDRINUSE;	  break;
		case EADDRNOTAVAIL: ec = WSABASEERR + EADDRNOTAVAIL; break;
		case ENETDOWN:		  ec = WSABASEERR + ENETDOWN;		  break;
		case ENETUNREACH:	  ec = WSABASEERR + ENETUNREACH;	  break;
		case ENOBUFS:		  ec = WSABASEERR + ENOBUFS;		  break;
		case ETIMEDOUT:	  ec = WSABASEERR + ETIMEDOUT;	  break;
		case ECONNREFUSED:  ec = WSABASEERR + ECONNREFUSED;  break;
		}
		return (int) ec;
	}
#elif defined NLM
	int WSAGetLastError() {return (int) GetLastError();}
#endif

unsigned long noThreads = 0;

// ================== Utilities =========================================================

// --------------------- Time -----------------------------------------------------------

// Creation of a `timespec' C structure from time in milliseconds to be used by time
// dependent Posix functions. A flag indicates when the time has to be in an absolute
// form, i.e. the current wallclock plus the `msec' interval. Note that absolute time is
// as if in the UTC timezone. For use with Clib on NetWare, the NLM starting time is
// used.

static inline void mkTime(unsigned long msec,struct timespec* pTime,bool absolute) {
	pTime->tv_sec	= msec / 1000;
	pTime->tv_nsec = (msec % 1000) * 1000 * 1000;
	if (absolute) {
#if defined SYM_LIBC_NLM || defined LINUX
		struct timeval now;
		gettimeofday(&now,NULL); // Time since the Epoch (00:00:00 UTC, January 1, 1970).
		pTime->tv_sec	+= now.tv_sec;
		pTime->tv_nsec += now.tv_usec * 1000;
#elif defined NLM
		clock_t now = clock(); // 1/100th seconds since NLM started.
		pTime->tv_sec	+= now / 100;
		pTime->tv_nsec += (now % 100) * 10 * 1000 * 1000;
#endif
	}
}

// Convert the `timespec' to milliseconds. If `absolute' -- the `timespec' is an absolute
// time -- return the number of milliseconds that remains until the absolute point in
// time.

static inline unsigned long mkTime(const struct timespec* pTime,bool absolute) {
	long sec = 0,nsec = 0;
	if (absolute) {
#if defined SYM_LIBC_NLM || defined LINUX
		struct timeval now; gettimeofday(&now,NULL);
		sec  = now.tv_sec;
		nsec = now.tv_usec * 1000;
#elif defined NLM
		clock_t now = clock();
		sec  = now / 100;
		nsec = (now % 100) * 10 * 1000 * 1000;
#endif
	}
	return (pTime->tv_sec - sec) * 1000 + (pTime->tv_nsec - nsec) / 1000 / 1000;
}

// --------------------- pthread on Clib ------------------------------------------------

#if defined NLM && ! defined SYM_LIBC_NLM

// This implementation of the pthread mutexes will work as long as we follow the rules
// for using mutexes:
//
//		A thread that has acquired a mutex can not acquire it again, as it then would wait
//		for itself to release is -- deadlock -- unless a another thread illegally releases
//		the mutex even if it doesn't hold it.
//
//		A thread may only release a mutex that it holds, otherwise the mutex semaphore will
//		be oversignaled and more than one threead will be let go from the mutex wait.
//
// If one mutex lock always preceeds one mutex unlock in each thread, we are safe.
//
// If this pthread implementation ever is started to be used more widely, we have to add
// safeguards to prevent the problems described above.

static inline void wfSema(const char* op,unsigned long* semaphore) {
	DBG; if (! *semaphore) dbg(op,false);
}

static inline int pthread_mutex_lock(pthread_mutex_t* pMutex) {

	// When the mutex semaphore is 1, a thread will decrease the semaphore to 0 and
	// immediately return from this function. It has acquired the mutex. Other threads
	// trying to acquire the mutex, will each first decrease the semaphore by 1, notice
	// that the resulting value is less than 0 and thus fall asleep waiting for the mutex
	// semaphore to be signaled by the thread that holds it. When the semaphore is less
	// than 0, threads are waiting for it. The absolute value of the negative semaphore
	// value tells you how many threads are waiting.

	wfSema("pthread_mutex_lock",pMutex); WaitOnLocalSemaphore(*pMutex); return 0;
}
static inline int pthread_mutex_unlock(pthread_mutex_t* pMutex) {

	// Signal the mutex semaphore that you are ready to give it up and let some other
	// thread have it. The mutex semaphore is increased by 1 causing another thread to
	// wake up from its sleep and thus acquire the mutex.

	wfSema("pthread_mutex_unlock",pMutex); SignalLocalSemaphore(*pMutex); return 0;
}

// You must acquire the mutex before you can wait on the condition semaphore. While you
// are waiting, the mutex is released. When the condition semaphore is signalled, you
// will return from the wait with the mutex acquired again. Check the guarded data. If
// the condition hasn't been fulfilled, wait on the condition semaphore. Otherwise,
// continue to execute and when ready, release the mutex.

static inline int pthread_cond_wait(pthread_cond_t* pCond,pthread_mutex_t* pMutex) {

	// Release the mutex to give other threads an opportunity to change the guarded
	// data. Wait for them to signal the condition semaphore when the change has been
	// done. Acquire the mutex again to go back and look at the data.

	pthread_mutex_unlock(pMutex); wfSema("pthread_cond_wait",pCond);
	WaitOnLocalSemaphore(*pCond);
	pthread_mutex_lock(	pMutex); return 0;
}
static inline int pthread_cond_timedwait(pthread_cond_t* pCond,pthread_mutex_t* pMutex,
													  const struct timespec* abstime) {

	// Same as `pthread_cond_wait', but timeout when the deadline has elapsed. Note that
	// we here are using an absolute time and not an interval. This allows us to wake up,
	// look at the data, and if not ready yet, fall asleep again, but now waiting only the
	// remaining time until the absolute point in time has been reached.

	pthread_mutex_unlock(pMutex); wfSema("pthread_cond_timedwait",pCond);
	int rc = TimedWaitOnLocalSemaphore(*pCond,mkTime(abstime,true));
	pthread_mutex_lock(pMutex); return rc == ERR_TIMEOUT_FAILURE ? ETIMEDOUT : 0;
}

// Acquire the mutex of the condition. manipulate the guarded data, signal the condition
// semaphore that you have changed the guarded data. Finish off your task and eventually
// release the condition mutex allowing the other threads to look at the guarded data.

static inline int pthread_cond_signal(pthread_cond_t* pCond) {

	// Signal the condition semaphore that the guarded data has been changed. Only one
	// thread will be awaken.

	wfSema("pthread_cond_signal",pCond); SignalLocalSemaphore(*pCond); return 0;
}
static inline int pthread_cond_broadcast(pthread_cond_t* pCond) {

	// Signal as many times as there are threads waiting on the condition semaphore. All
	// these threads will be awaken to take a look at the guarded data.

	wfSema("pthread_cond_broadcast",pCond);
	for (int i = ExamineLocalSemaphore(*pCond); i < 0; i++)
		SignalLocalSemaphore(*pCond);

	return 0;
}

static inline pthread_t pthread_self() {return GetThreadID();}

static inline void pthread_exit(void* status) {ExitThread(TSR_THREAD,(int) status);}

static inline pid_t getpid() {return GetNLMHandle();}

#endif

// --------------------- Thread Identifier ----------------------------------------------

static inline wthread_t wthread_self() {
	#if defined LINUX
		return reinterpret_cast<wthread_t>( pthread_self() );
	#elif defined SYM_LIBC_NLM
		return reinterpret_cast<wthread_t>( getthreadid() );
	#elif defined NLM
		return static_cast<wthread_t>( GetThreadID() );
	#endif
}

// --------------------- Mutex ----------------------------------------------------------

static inline void mkMutex(pthread_mutex_t* pMutex,bool bRecursive = false) {
#if defined SYM_LIBC_NLM || defined LINUX
	pthread_mutexattr_t attribute; pthread_mutexattr_init(&attribute);
	pthread_mutexattr_settype(
		&attribute,bRecursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(pMutex,&attribute); pthread_mutexattr_destroy(&attribute);
#elif defined NLM
	*pMutex = OpenLocalSemaphore(1); // A thread must acquire the mutex to hold it. Only
												// one thread at a time may hold it. Therefore, it's
	wfSema("mkMutex",pMutex);			// initially signaled to 1.
#endif
}
static inline void mkCond(pthread_cond_t* pCond) {
#if defined SYM_LIBC_NLM || defined LINUX
	pthread_cond_init(pCond,NULL);  // non-shared by default
#elif defined NLM
	*pCond = OpenLocalSemaphore(0); // A thread has to wait on the condition until that
											  // has been explicitly signaled by those changing the
	wfSema("mkCond",pCond);			  // guarded data. Therefore, it's initially unsignaled.
#endif
}

static inline DWORD rmMutex(pthread_mutex_t* pMutex) {
#if defined SYM_LIBC_NLM || defined LINUX
	return pthread_mutex_destroy(pMutex);
#elif defined NLM
	wfSema("rmMutex",pMutex);
	if (ExamineLocalSemaphore(*pMutex) < 1) return EBUSY;
	else {
		CloseLocalSemaphore(*pMutex); return 0;
	}
#endif
}
static inline DWORD rmCond(pthread_cond_t* pCond) {
#if defined SYM_LIBC_NLM || defined LINUX
	return pthread_cond_destroy(pCond);
#elif defined NLM
	wfSema("rmCond",pCond);
	if (ExamineLocalSemaphore(*pCond) < 0) return EBUSY;
	else {
		CloseLocalSemaphore(*pCond); return 0;
	}
#endif
}

// --------------------- Locker ---------------------------------------------------------

class Locker {
public:
	Locker(const pthread_mutex_t* pMutex);
	~Locker();

private:
	pthread_mutex_t* _pMutex;

	Locker();
	Locker(const Locker& that);
	Locker& operator=(const Locker& that);
};

Locker::Locker(const pthread_mutex_t* pMutex) : _pMutex((pthread_mutex_t*) pMutex) {
	pthread_mutex_lock(_pMutex);
}
Locker::~Locker() {
	pthread_mutex_unlock(_pMutex);
}

// --------------------- Debug ----------------------------------------------------------

#if defined LINUX
   #define initLine()
	#define print Real_dprintf
	#define vsnprint(buffer,count,format,args) vsnprintf(buffer,count,format,args)   
#elif defined SYM_LIBC_NLM
	#define initLine() Line::init()
	#define print Line::fprint
	#define vsnprint(buffer,count,format,args) vsnprintf(buffer,count,format,args)
#elif defined NLM
	#define initLine()
	#ifdef VPSTART
		extern "C" void iprintf(char* format,...);
		#define print iprintf
	#else
		#define print Real_dprintf
	#endif
	#define vsnprint(buffer,count,format,args) NWvsprintf(buffer,format,args)
#endif

class Synch;

class Line {
public:
	static const size_t BufferSize;

#ifdef SYM_LIBC_NLM
	static inline void init();
	static inline void fprint(const char* format,const char* buffer);
#endif

	Line();
	~Line();

	char*	 _buffer;
	size_t _index;

private:
#ifdef SYM_LIBC_NLM
	static pthread_mutex_t Mutex;
	static bool			     FirstLine;
	static const char*	  File;
	static const char*     Header;
	static const char*     Separator;
#endif

	Line(const Line& that);
	Line& operator=(const Line& that);
};

const size_t Line::BufferSize = 4096;

#ifdef SYM_LIBC_NLM
pthread_mutex_t Line::Mutex;
bool            Line::FirstLine = true;
const char*		 Line::File		  = "ScsComms.log";
const char*     Line::Header    = "%02d:%02d:%02d.%06d[_%lx %lu_]|";
const char*     Line::Separator = " =======================================================\n";

inline void Line::init() {
	mkMutex(&Mutex);
}
inline void Line::fprint(const char* format,const char* buffer) {
	Locker lock(&Mutex);

	FILE* pFile = fopen(File,"a");

	DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );
	DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
	DWORD dwMicros	 =
		OneDayMaxResToMicroSeconds(dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND);
	DWORD dwHours,dwMins,dwSecs;
	SecondsToHoursMinsSecs(dwSeconds,dwHours,dwMins,dwSecs);

	wthread_t wtid = wthread_self(); pthread_t ptid = pthread_self();

	if (FirstLine) {
		FirstLine = false;

		fprintf(pFile,Header,dwHours,dwMins,dwSecs,dwMicros,wtid,ptid);
		fprintf(pFile,Separator);
	}
	fprintf(pFile,Header,dwHours,dwMins,dwSecs,dwMicros,wtid,ptid);
	fprintf(pFile,format,buffer);

	fflush(pFile); fsync(fileno(pFile)); fclose(pFile);
}
//inline void Line::fprint(const char* format,const char* buffer) {
// if (FirstLine) {
//    FirstLine = false; DebugPrint(DEBUGFLAG_THREAD_SYNCH,Separator);
// }
// DebugPrint(DEBUGFLAG_THREAD_SYNCH,format,buffer);
//}
#endif

Line::Line() : _index(0) {
	_buffer = new char[BufferSize]; _buffer[0] = '\0';
}
Line::~Line() {delete[] _buffer;}

static inline void dbgF(Line* line) {print("%s",line->_buffer); delete line;}

static inline void dbgPv(Line* line,const char* format,va_list args) {
	if (line->_index < Line::BufferSize) {
		line->_index += vsnprint(line->_buffer		+ line->_index,
										 Line::BufferSize - line->_index,format,args);
		if (line->_index >= Line::BufferSize) {
			char* eol = "<eol>\n";
			strcpy(line->_buffer + Line::BufferSize - strlen(eol),eol);
		}
	}
}
static inline void dbgP(Line* line,const char* format,...) {
	va_list args; va_start(args,format); dbgPv(line,format,args); va_end(args);
}

static inline void dbgOv(Line* line,const char* format,va_list args) {
	dbgP(line," "); dbgPv(line,format,args);
}
static inline void dbgO(Line* line,const char* format,...) {
	va_list args; va_start(args,format); dbgOv(line,format,args); va_end(args);
}
static inline void dbgO(Line* line,const Synch* pSynch);

static inline void dbgO(Line* line,bool ok) {
	char* s; bool wf = true;
	switch (ok) {
	case true:	s = "succeeded"; break;
	case false: s = "failed";	  break;
	default:		s = "UNKNOWN";	  wf = false;
	}
	dbgO(line,"%s",s); if (! wf) dbgP(line,"<%li>",ok);
}
static inline void dbgR(Line* line,unsigned long rc) {
	char* s; bool wf = true;
	switch (rc) {
	case WAIT_OBJECT_0:				s = "WAIT_OBJECT_0";				 break;
	case WAIT_ABANDONED:				s = "WAIT_ABANDONED";			 break;
	case WAIT_TIMEOUT:				s = "WAIT_TIMEOUT";				 break;
	case WAIT_FAILED:					s = "WAIT_FAILED";				 break;
	case STILL_ACTIVE:				s = "STILL_ACTIVE";				 break;
	case ERROR_NOT_ENOUGH_MEMORY: s = "ERROR_NOT_ENOUGH_MEMORY"; break;
	case EAGAIN:						s = "EAGAIN";						 break;
	case ERROR_INVALID_HANDLE:		s = "ERROR_INVALID_HANDLE";	 break;
	case ERROR_INVALID_PARAMETER: s = "ERROR_INVALID_PARAMETER"; break;
	case ERROR_FUNCTION_FAILED:	s = "ERROR_FUNCTION_FAILED";	 break;
	case ERROR_BUFFER_OVERFLOW:	s = "ERROR_BUFFER_OVERFLOW";	 break;
	case ERROR_BUSY:					s = "ERROR_BUSY";					 break;
	default:								s = "nr";							 wf = false;
	}
	dbgO(line,"%s",s); if (! wf) dbgP(line,"<%lu>",rc);
}

static inline void dbgB(Line* line,const char* op = NULL) {
	if (op) dbgP(line,"_ %s",op);
}
static inline void dbgN(Line* line) {dbgP(line,"\n"); dbgF(line);}

static inline void dbgE(Line* line) {
	dbgO(line,"..."); dbgN(line);
}

// ........................ External ....................................................

void dbgV(const char* format,va_list args) {
	Line* line = new Line(); dbgB(line,"---"); dbgOv(line,format,args);
	if (format[strlen(format) - 1] == '\n') dbgF(line); else dbgN(line);
}
void dbg(const char* format,...) {
	va_list args; va_start(args,format); dbgV(format,args); va_end(args);
}
void prtV(const char* format,va_list args) {
	Line* line = new Line(); dbgB(line); dbgPv(line,format,args);
	if (format[strlen(format) - 1] == '\n') dbgF(line); else dbgN(line);
}
void prt(const char* format,...) {
	va_list args; va_start(args,format); prtV(format,args); va_end(args);
}

// ........................ Initial .....................................................

static inline void dbgI(const char* op) {
	DBG; Line* line = new Line(); dbgB(line,op); dbgE(line);
}
static inline void dbgI(const char* op,const Synch* pSynch) {
	DBG; Line* line = new Line(); dbgB(line,op); dbgO(line,"%#lx",pSynch); dbgE(line);
}
static inline void dbgIv(const char* op,const char* format,va_list args) {
	DBG; Line* line = new Line(); dbgB(line,op); dbgOv(line,format,args); dbgE(line);
}
static inline void dbgI(const char* op,const char* format,...) {
	DBG; va_list args; va_start(args,format); dbgIv(op,format,args); va_end(args);
}
static inline void dbgIv(const char* op,const Synch* pSynch,
								 const char* format,va_list args) {
	DBG; Line* line = new Line();
	dbgB(line,op); dbgO(line,"%#lx",pSynch); dbgOv(line,format,args); dbgE(line);
}
static inline void dbgI(const char* op,const Synch* pSynch,const char* format,...) {
	DBG; va_list args; va_start(args,format); dbgIv(op,pSynch,format,args); va_end(args);
}

static inline void dbgI(bool condition,const char* op) {
	DBG; if (condition) dbgI(op);
}
static inline void dbgI(bool condition,const char* op,const Synch* pSynch) {
	DBG; if (condition) dbgI(op,pSynch);
}
static inline void dbgI(bool condition,const char* op,
								const char* format,...) {
	DBG; if (condition) {
		va_list args; va_start(args,format); dbgIv(op,format,args); va_end(args);
	}
}
static inline void dbgI(bool condition,const char* op,const Synch* pSynch,
								const char* format,...) {
	DBG; if (condition) {
		va_list args; va_start(args,format); dbgIv(op,pSynch,format,args); va_end(args);
	}
}

// ........................ Body ........................................................

static inline void dbg(const char* op,const Synch* pSynch) {
	DBG; Line* line = new Line(); dbgB(line,op); dbgO(line,pSynch); dbgN(line);
}
static inline void dbg(const char* op,int ok) {
	DBG; Line* line = new Line(); dbgB(line,op); dbgO(line,ok); dbgN(line);
}
static inline void dbg(const char* op,const Synch* pSynch,int ok) {
	DBG; Line* line = new Line();
	dbgB(line,op); dbgO(line,pSynch); dbgO(line,ok); dbgN(line);
}
static inline void dbgv(const char* op,const char* format,va_list args) {
	DBG; Line* line = new Line(); dbgB(line,op); dbgOv(line,format,args); dbgN(line);
}
static inline void dbg(const char* op,const char* format,...) {
	DBG; va_list args; va_start(args,format); dbgv(op,format,args); va_end(args);
}

static inline void dbg(bool condition,const char* op,const Synch* pSynch) {
	DBG; if (condition) dbg(op,pSynch);
}
static inline void dbg(bool condition,const char* op,int ok) {
	DBG; if (condition) dbg(op,ok);
}
static inline void dbg(bool condition,const char* op,const Synch* pSynch,int ok) {
	DBG; if (condition) dbg(op,pSynch,ok);
}
static inline void dbg(bool condition,const char* op,const char* format,...) {
	DBG; if (condition) {
		va_list args; va_start(args,format); dbgv(op,format,args); va_end(args);
	}
}

// ........................ Return Code .................................................

static inline void dbgR(const char* op,const Synch* pSynch,unsigned long rc) {
	DBG; Line* line = new Line();
	dbgB(line,op); dbgO(line,pSynch); dbgR(line,rc); dbgN(line);
}
static inline void dbgR(const char* op,unsigned long rc) {
	DBG; Line* line = new Line(); dbgB(line,op); dbgR(line,rc); dbgN(line);
}

static inline void dbgR(bool condition,const char* op,const Synch* pSynch,
								unsigned long rc) {
	DBG; if (condition) dbgR(op,pSynch,rc);
}
static inline void dbgR(bool condition,const char* op,unsigned long rc) {
	DBG; if (condition) dbgR(op,rc);
}

// ================== Process ===========================================================

DWORD GetCurrentProcessId() {return (DWORD) getpid();}

// ================== Synchronization Objects ===========================================

// Note that `lpEventAttributes' and `lpName' are not used as we currently only implement
// support for threads local to a single process.

class MultiSynch;

class Synch {
public:
	Synch();
	virtual ~Synch();

	virtual void dbg(Line* line) const;

	pthread_mutex_t   _mutex;							// Data protection
	pthread_cond_t	   _cond;

	list<MultiSynch*> _lpMultiSynchs;				// WaitForMultipleObjects

	char*             _sName;                    // debugging name

	bool				  	_bThread;						// Object type
	bool				  	_bEvent;
	bool				  	_bMutex;
	bool				  	_bSemaphore;
 
	bool				  	_bManualReset;					// Singaling
	bool				  	_bSignaled;

private:
	Synch(const Synch& that);
	Synch& operator=(const Synch& that);
};

Synch::Synch() :
	_sName(NULL),
	_bThread (false),_bEvent(false),_bMutex(false),_bSemaphore(false),
	_bManualReset(false),_bSignaled(false)
{
	mkMutex(&_mutex); mkCond(&_cond);
}
Synch::~Synch() {_lpMultiSynchs.clear(); if (_sName) delete _sName;}

inline void Synch::dbg(Line* line) const {
	DBG;
	dbgO(line,"%#lx<%s| manualReset %li signaled %li multiSynchs %lu name \"%s\"",
		  this,
		  _bThread	  ? "Thread"	 :
		  _bEvent	  ? "Event"		 :
		  _bMutex	  ? "Mutex"		 :
		  _bSemaphore ? "Semaphore" : "UNKNOWN",
		  _bManualReset,_bSignaled,_lpMultiSynchs.size(),_sName ? _sName : "");
}
static inline void dbgO(Line* line,const Synch* pSynch) {
	DBG; pSynch->dbg(line);
}

class MultiSynch {
public:
	MultiSynch();

	bool signal(Synch* pSynch);

	pthread_mutex_t* _pMutex;
	pthread_cond_t*  _pCond;
	bool				  _bSignaled;
	Synch**			  _ppObject;
	wthread_t		  _waiter;

private:
	MultiSynch(const MultiSynch& that);
	MultiSynch& operator=(const MultiSynch& that);
};

MultiSynch::MultiSynch() :
	_pMutex(NULL),_pCond(NULL),_bSignaled(false),_ppObject(NULL),_waiter(0) {}

bool MultiSynch::signal(Synch* pSynch) {
	Locker lock(_pMutex); bool bSent = false;

	if (! _bSignaled) {									// Once only for this object.
		_bSignaled = true;

		if (_ppObject) {									// Waiting for any.
			if (! *_ppObject) {							// Only first signaled.
				*_ppObject = pSynch; pthread_cond_signal(_pCond); bSent = true;
			}
		} else {												// Waiting for all.
			pthread_cond_signal(_pCond); bSent = true;
		}
	}
	return bSent;
}

// --------------------- Thread ---------------------------------------------------------

class Thread : public Synch {
public:
	static pthread_mutex_t ThreadMapMutex;

	static void		  insert(wthread_t wtid,Thread* pThread);
	static Thread*	  select(wthread_t wtid = wthread_self());

	static void		  sThreads(	 DWORD* pnCount,HANDLE**	 ppThreads);
	static void		  sThreadIDs(DWORD* pnCount,wthread_t** ppThreadIDs);
	static bool		  isThreads();
	static Thread*	  sInitializerThread();

	Thread(unsigned (*fun)(void*),void* arg);
	~Thread();

	void dbg(Line* line) const;

	wthread_t wtid();			void wtid(      wthread_t wtid);
	pthread_t ptid();			void ptid(		 pthread_t ptid);
	DWORD		 errorCode();	void errorCode( DWORD	  ec);
	unsigned	 returnCode(); void returnCode(unsigned  rc);

	void mk(bool isExtended);
	void rm(unsigned rc);

private:
	#define tThreadMap map<wthread_t,Thread*,less<wthread_t> >

	static tThreadMap ThreadMap;						// Mapping `wtid' to thread handle.
	static Thread		InitializerThread;

	Thread();												// Initializer thread ONLY !
	Thread(const Thread& that);
	Thread& operator=(const Thread& that);

	void init(wthread_t wtid,pthread_t ptid,unsigned (*fun)(void*),void* arg);

	wthread_t  _wtid;
	pthread_t  _ptid;
	unsigned (*_fun)(void*);
	void*		  _arg;
	DWORD		  _ec;
	unsigned	  _rc;
};

// ........................ Static Thread Map ...........................................

tThreadMap		 Thread::ThreadMap;
Thread			 Thread::InitializerThread;
pthread_mutex_t Thread::ThreadMapMutex;

void Thread::insert(wthread_t wtid,Thread* pThread) {
	Locker lock(&ThreadMapMutex); tThreadMap::iterator i = ThreadMap.find(wtid);
	if (i == ThreadMap.end()) {
		pair<wthread_t,Thread*> element(wtid,pThread); ThreadMap.insert(element);
	}
}
Thread* Thread::select(wthread_t wtid) {
	Locker lock(&ThreadMapMutex); tThreadMap::iterator i = ThreadMap.find(wtid);
	return i != ThreadMap.end() ? i->second : NULL;
}

void Thread::sThreads(DWORD* pnCount,HANDLE** ppThreads) {
	Locker lock(&ThreadMapMutex); DWORD j,n = 0; HANDLE* pThreads = NULL;
	tThreadMap::iterator i,b = ThreadMap.begin(),e = ThreadMap.end();

	for (i = b; i != e; i++) n++; if (n) pThreads = new(nothrow) HANDLE[n];
	if (! pThreads) {*pnCount = 0; *ppThreads = NULL; return;}
	for (i = b,j = 0; i != e; i++,j++) pThreads[j] = (HANDLE) i->second;

	*pnCount = n; *ppThreads = pThreads;
}
void Thread::sThreadIDs(DWORD* pnCount,wthread_t** ppThreadIDs) {
	Locker lock(&ThreadMapMutex); DWORD j,n = 0; wthread_t* pThreadIDs = NULL;
	tThreadMap::iterator i,b = ThreadMap.begin(),e = ThreadMap.end();

	for (i = b; i != e; i++) n++; if (n) pThreadIDs = new(nothrow) wthread_t[n];
	if (! pThreadIDs) {*pnCount = 0; *ppThreadIDs = NULL; return;}
	for (i = b,j = 0; i != e; i++,j++) pThreadIDs[j] = i->first;

	*pnCount = n; *ppThreadIDs = pThreadIDs;
}
bool Thread::isThreads() {
	Locker lock(&ThreadMapMutex);
	::dbgI("Thread::isThreads","#ThreadMap %lu",ThreadMap.size());
	return ! ThreadMap.empty();
}
Thread* Thread::sInitializerThread() {return &InitializerThread;}

// ........................ Constructors ................................................

void Thread::init(wthread_t wtid,pthread_t ptid,unsigned (*fun)(void*),void* arg) {
	_bThread = true; _bManualReset = true;
	_wtid = wtid; _ptid = ptid; _fun = fun; _arg = arg; _ec = 0; _rc = STILL_ACTIVE;
}
Thread::Thread() : Synch() {							// Initializer thread ONLY !
	initLine(); init(wthread_self(),pthread_self(),NULL,NULL);
	mkMutex(&ThreadMapMutex); ::dbg("Thread::InitializerThread",this);
}
Thread::Thread(unsigned (*fun)(void*),void* arg) : Synch() {
	init(0,(pthread_t) -1L,fun,arg);
}

Thread::~Thread() {
	::dbg("Thread::~Thread",this);
	Locker lock(&ThreadMapMutex); tThreadMap::iterator i = ThreadMap.find(_wtid);
	if (i != ThreadMap.end()) ThreadMap.erase(_wtid);
}

// ........................ Members .....................................................

inline void Thread::dbg(Line* line) const {
	DBG; Synch::dbg(line);
	dbgO(line,"wtid _%lx ptid %lu_ fun %#lx arg %#lx stack %#lx",
		  _wtid,_ptid,_fun,_arg,&_arg);
	dbgO(line,"ec"); dbgR(line,_ec); dbgO(line,"rc"); dbgR(line,_rc); dbgP(line,">");
}

wthread_t Thread::wtid()						{return _wtid;}
void		 Thread::wtid(wthread_t wtid)		{_wtid = wtid;}

pthread_t Thread::ptid()						{return _ptid;}
void		 Thread::ptid(pthread_t ptid)		{_ptid = ptid;}

DWORD		 Thread::errorCode()					{return _ec;}
void		 Thread::errorCode(DWORD ec)		{_ec =   ec;}

unsigned	 Thread::returnCode()				{return _rc;}
void		 Thread::returnCode(unsigned rc) {_rc =   rc;}

void Thread::mk(bool isExtended) {					// The life of a thread.
	{
		Locker lock(&_mutex);								// Wait until starter is done.

		wthread_t wtid = wthread_self(); Thread::wtid(wtid); insert(wtid,this);
		::dbg("Thread::mk",this);

		pthread_cond_signal(&_cond);					// Tell starter that I'm ready.
	}
    InterlockedIncrement(&noThreads);
	unsigned rc = _fun(_arg); {Locker lock(&_mutex); returnCode(rc);}
    InterlockedDecrement(&noThreads);

	if (isExtended) _endthreadex(rc); else _endthread();
}
void Thread::rm(unsigned rc) {					// Tell all waiters that I'm going away.
	Locker lock(&_mutex); ::dbg("Thread::rm",this);
	_bSignaled = true; if (_rc == STILL_ACTIVE) _rc = rc;

	list<MultiSynch*>& lpMultiSynchs = _lpMultiSynchs;
	if (! lpMultiSynchs.empty()) {				// Others are multi waiting with me in set.
		for (list<MultiSynch*>::iterator
				  it = lpMultiSynchs.begin(),eol = lpMultiSynchs.end(); it != eol; it++) {
			(*it)->signal(this);
		}
	}
	pthread_cond_broadcast(&_cond); ::dbg("Thread::rm",this,true);
}

// ........................ Start Thread ................................................

class Call {
public:
	Call(Thread* pThread,bool isExtended);

	Thread* _pThread;
	bool	  _isExtended;

private:
	Call();
	Call(const Call& that);
	Call& operator=(const Call& that);
};

Call::Call(Thread* pThread,bool isExtended) :
	_pThread(pThread),_isExtended(isExtended) {}

static void* run(void* arg) {
	if (arg) {
		Call* pCall = (Call*) arg;

		Thread* pThread    = pCall->_pThread;
		bool    isExtended = pCall->_isExtended;

		delete pCall; pThread->mk(isExtended);
	}
	 return NULL;
}
static uintptr_t mkThread(unsigned (*pFun)(void*),void* pArg,
								  unsigned uiStackSize,unsigned* uipThreadId,bool isExtended)
{
	dbgI("mkThread","fun %#lx arg %#lx stackSize %lu extended %li",
		  pFun,pArg,uiStackSize,isExtended);
	Thread* pThread; Call* pCall; pthread_t ptid; int rc = 0;
	
	if (! ((pThread = new(nothrow) Thread(pFun,pArg)) &&
			 (pCall	 = new(nothrow) Call(pThread,isExtended)))) {
		setError(ERROR_NOT_ENOUGH_MEMORY); return 0;
	}

	Locker lock(&pThread->_mutex);					// Force startee to wait until I'm done.

#if defined SYM_LIBC_NLM || defined LINUX
	pthread_attr_t attr; pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if (uiStackSize) pthread_attr_setstacksize(&attr,uiStackSize);

	rc = pthread_create(&ptid,&attr,run,(void*) pCall);

	pthread_attr_destroy(&attr);
#elif defined NLM
	ptid = BeginThread((void (*)(void*)) run,NULL,uiStackSize,(void*) pCall);

	if (ptid == EFAILURE) rc = errno == ENOMEM ? EAGAIN : errno;
#endif
	if (rc) {setError(rc); return 0;}

	pThread->ptid(ptid); dbg("mkThread",pThread);
	
	wthread_t wtid;
	while (! (wtid = pThread->wtid()))				// Wait for startee to determine `wtid'.
		pthread_cond_wait(&pThread->_cond,&pThread->_mutex);

	dbg("mkThread",pThread,true);
	
	if (uipThreadId) {*uipThreadId = wtid;} return (uintptr_t) pThread;
}
uintptr_t _beginthread(void (*pStart_address)(void*),
							  unsigned uiStack_size,void* pArglist) {
	uintptr_t pThread = mkThread((unsigned (*)(void*)) pStart_address,pArglist,
										  uiStack_size,NULL,false);
	return pThread ? pThread : (uintptr_t) -1L;
}
uintptr_t _beginthreadex(void* pSecurity,unsigned uiStack_size,
								 unsigned (*pStart_address)(void*),void* pArglist,
								 unsigned uiInitflag,unsigned* uipThrdaddr) {
	return mkThread(pStart_address,pArglist,uiStack_size,uipThrdaddr,true);
}

// ..................... Exit Thread ....................................................

static void rmThread(unsigned rc,bool isExtended) {
	dbgI("rmThread","rc %lu extended %li",rc,isExtended);
	Thread* pThread = Thread::select();
	if (pThread) {
		pThread->rm(rc);
		if (! isExtended) // Prevents a `_beginthread' thread from being waited on.
			CloseHandle((HANDLE) pThread);
	}
	pthread_exit(NULL);
}
void _endthread() {rmThread(0,false);}

void _endthreadex(unsigned uiRetval) {rmThread(uiRetval,true);}

BOOL TerminateThread(HANDLE hThread,DWORD dwExitCode) {
	Thread* pThread = (Thread*) hThread;
	dbgI("TerminateThread",pThread,"ec %lu",dwExitCode);
	if (! (pThread && pThread->_bThread)) {
		setError(ERROR_INVALID_HANDLE); return false;
	}
	Locker lock(&pThread->_mutex); dbg("TerminateThread",pThread);

	pThread->returnCode(dwExitCode);

#if defined SYM_LIBC_NLM || defined LINUX
	pthread_kill(pThread->ptid(),SIGTERM);
#elif defined NLM
	print("<<<ERROR>>> Attempt to terminate thread \"%lu\" with exit code %lu !\n",
			pThread->wtid(),dwExitCode);
#endif

	dbg("TerminateThread",pThread,true); return true;
}

// ........................ Thread Data .................................................

HANDLE GetCurrentThreadId() {
	return reinterpret_cast<HANDLE>(
	#if defined LINUX
		pthread_self()
	#elif defined NLM
		wthread_self()
	#endif
		);
}

pthread_t GetCurrentPthreadId() {return pthread_self();}

void GetThreads(DWORD* pnCount,HANDLE** ppThreads) {
	Thread::sThreads(pnCount,ppThreads);
}
void GetThreadIDs(DWORD* pnCount,wthread_t** ppThreadIDs) {
	Thread::sThreadIDs(pnCount,ppThreadIDs);
}

BOOL GetExitCodeThread(HANDLE hThread,LPDWORD lpExitCode) {
	Thread* pThread = (Thread*) hThread; dbgI("GetExitCodeThread",pThread);
	if (! (pThread && pThread->_bThread && lpExitCode)) {
		setError(ERROR_INVALID_HANDLE); return false;
	}
	Locker lock(&pThread->_mutex); dbg("GetExitCodeThread",pThread,true);
	*lpExitCode = pThread->returnCode(); return true;
}

static DWORD getError() {
	Thread* pThread = Thread::select(); dbgI("getError",pThread);
	if (pThread) {
		Locker lock(&pThread->_mutex); dbg("getError",pThread,true);
		return pThread->errorCode();
	} else {
		dbgR("getError",errno); return errno;
	}
}
static void setError(DWORD dwErrCode) {
	Thread* pThread = Thread::select();
	dbgI("setError",pThread,"ec %lu",dwErrCode);
	if (pThread) {
		Locker lock(&pThread->_mutex);   dbg("setError",pThread);
		pThread->errorCode(dwErrCode); dbg("setError",pThread,true);
	} else {
		errno = dwErrCode; dbgR("setError",dwErrCode);
	}
}

BOOL isThreadAlive(wthread_t wtid) {
	bool ok = false;

	#if defined LINUX
		int policy; struct sched_param param;

		ok = pthread_getschedparam(wtid,&policy,&param) != ESRCH;
	#elif defined NLM
		char sName[18];

		#if defined SYM_LIBC_NLM
			ok = getthreadname((void*) wtid,sName,18) != EBADHNDL;
		#else
			ok = GetThreadName((int) wtid,sName)      != EBADHNDL;
		#endif
	#endif

	dbg("isThreadAlive","wtid _%lx alive %li",wtid,ok); return ok;
}
BOOL ExistThreads() {return Thread::isThreads();}

HANDLE GetInitializerThread() {return (HANDLE) Thread::sInitializerThread();}

// --------------------- Event ----------------------------------------------------------

class Event : public Synch {
public:
	Event(bool bManualReset,bool bInitialState);
	~Event();

	void dbg(Line* line) const;

private:
	Event();
	Event(const Event& that);
	Event& operator=(const Event& that);
};

Event::Event(bool bManualReset,bool bInitialState) : Synch() {
	_bEvent = true; _bManualReset = bManualReset; _bSignaled = bInitialState;
}
Event::~Event() {::dbg("Event::~Event",this);}

inline void Event::dbg(Line* line) const {DBG; Synch::dbg(line); dbgP(line,">");}

HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,
						 BOOL bManualReset,BOOL bInitialState,LPTSTR lpName) {
	dbgI("CreateEvent","manualReset %li initialState %li",bManualReset,bInitialState);
	HANDLE hEvent = (HANDLE) new(nothrow) Event(bManualReset,bInitialState);
	if (! hEvent) setError(ERROR_NOT_ENOUGH_MEMORY);
	dbg("CreateEvent",(Event*) hEvent,true); return hEvent;
}
BOOL SetEvent(HANDLE hEvent) {
	Event* pEvent = (Event*) hEvent; dbgI("SetEvent",pEvent);
	if (! (pEvent && pEvent->_bEvent)) {setError(ERROR_INVALID_HANDLE); return false;}

	Locker lock(&pEvent->_mutex); dbg("SetEvent",pEvent); pEvent->_bSignaled = true;

	list<MultiSynch*>& lpMultiSynchs = pEvent->_lpMultiSynchs;
	if (! lpMultiSynchs.empty()) { // Others are multi waiting with me in set.
		for (list<MultiSynch*>::iterator
				  it = lpMultiSynchs.begin(),eol = lpMultiSynchs.end(); it != eol; it++) {

			if ((*it)->signal(pEvent) && ! pEvent->_bManualReset) { // Change state here.
				pEvent->_bSignaled = false; break;
			}
		}
	}
	if		  (pEvent->_bManualReset) pthread_cond_broadcast(&pEvent->_cond);
	else if (pEvent->_bSignaled)	  pthread_cond_signal(	 &pEvent->_cond);

	dbg("SetEvent",pEvent,true); return true;
}
BOOL ResetEvent(HANDLE hEvent) {
	Event* pEvent = (Event*) hEvent; dbgI("ResetEvent",pEvent);
	if (! (pEvent && pEvent->_bEvent)) {setError(ERROR_INVALID_HANDLE); return false;}

	Locker lock(&pEvent->_mutex); dbg("ResetEvent",pEvent); pEvent->_bSignaled = false;
	dbg("ResetEvent",pEvent,true); return true;
}

// --------------------- Mutex ----------------------------------------------------------

class Mutex : public Synch {
public:
	Mutex(bool bInitialOwner);
	~Mutex();

	int	    _iLevel;
	wthread_t _owner;

	void dbg(Line* line) const;

private:
	Mutex();
	Mutex(const Mutex& that);
	Mutex& operator=(const Mutex& that);
};

Mutex::Mutex(bool bInitialOwner) : Synch(),_iLevel(0),_owner(0) {
	_bMutex = true; _bSignaled = ! bInitialOwner;
	if (bInitialOwner) {_iLevel = 1; _owner = wthread_self();}
}
Mutex::~Mutex() {::dbg("Mutex::~Mutex",this);}

inline void Mutex::dbg(Line* line) const {
	DBG; Synch::dbg(line); dbgO(line,"level %li owner _%lx>",_iLevel,_owner);
}

HANDLE CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,
						 BOOL bInitialOwner,LPCTSTR lpName) {
	dbgI("CreateMutex","initialOwner %li",bInitialOwner);
	HANDLE hMutex = (HANDLE) new(nothrow) Mutex(bInitialOwner);
	if (! hMutex) setError(ERROR_NOT_ENOUGH_MEMORY);
	dbg("CreateMutex",(Mutex*) hMutex,true); return hMutex;
}
BOOL ReleaseMutex(HANDLE hMutex) {
	Mutex* pMutex = (Mutex*) hMutex; dbgI("ReleaseMutex",pMutex);
	if (! (pMutex && pMutex->_bMutex)) {setError(ERROR_INVALID_HANDLE); return false;}

	Locker lock(&pMutex->_mutex); dbg("ReleaseMutex",pMutex);

	bool ok = pMutex->_owner == wthread_self();

	if (! ok) {
		prt("<ERROR> Attempt to release mutex %#lx owned by thread _%lx.\n",
			 pMutex,pMutex->_owner);
        #ifndef DEBUG
    		// asm { int 3 }
		EnterDebugger();
        #endif
	}
	
	if (ok && --pMutex->_iLevel == 0) {
		pMutex->_bSignaled = true; pMutex->_owner = 0;

		list<MultiSynch*>& lpMultiSynchs = pMutex->_lpMultiSynchs;
		if (! lpMultiSynchs.empty()) { // Others are multi waiting with me in set.
			for (list<MultiSynch*>::iterator
					  it = lpMultiSynchs.begin(),eol = lpMultiSynchs.end(); it != eol; it++) {
				MultiSynch* pMultiSynch = *it;

				if (pMultiSynch->signal(pMutex)) { // Change state here.
					pMutex->_bSignaled = false;
					pMutex->_iLevel = 1; pMutex->_owner = pMultiSynch->_waiter; break;
				}
			}
		}
		if (pMutex->_bSignaled) pthread_cond_signal(&pMutex->_cond);
	}
	dbg("ReleaseMutex",pMutex,ok); return ok;
}

// --------------------- Semaphore ------------------------------------------------------

class Semaphore : public Synch {
public:
	Semaphore(LONG lInitialCount,LONG lMaximumCount);
	~Semaphore();

	LONG _lCount;
	LONG _lCountMax;

	void dbg(Line* line) const;

private:
	Semaphore();
	Semaphore(const Semaphore& that);
	Semaphore& operator=(const Semaphore& that);
};

Semaphore::Semaphore(LONG lInitialCount,LONG lMaximumCount) :
	Synch(),_lCount(lInitialCount),_lCountMax(lMaximumCount)
{
	_bSemaphore = true; _bSignaled = lInitialCount > 0;
}
Semaphore::~Semaphore() {::dbg("Semaphore::~Semaphore",this);}

inline void Semaphore::dbg(Line* line) const {
	DBG; Synch::dbg(line); dbgO(line,"count %li countMax %li>",_lCount,_lCountMax);
}

HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
							  LONG lInitialCount,LONG lMaximumCount,LPCTSTR lpName) {
	dbgI("CreateSemaphore","initialCount %li maximumCount %li",
		  lInitialCount,lMaximumCount);
	if (lInitialCount < 0 || lInitialCount > lMaximumCount) {
		setError(ERROR_INVALID_PARAMETER); return NULL;
	}
	HANDLE hSemaphore = (HANDLE) new(nothrow) Semaphore(lInitialCount,lMaximumCount);
	if (! hSemaphore) setError(ERROR_NOT_ENOUGH_MEMORY);
	dbg("CreateSemaphore",(Semaphore*) hSemaphore,true); return hSemaphore;
}
BOOL ReleaseSemaphore(HANDLE hSemaphore,LONG lReleaseCount,LPLONG lpPreviousCount) {
	Semaphore* pSemaphore = (Semaphore*) hSemaphore;
	dbgI("ReleaseSemaphore",pSemaphore,"releaseCount %li",lReleaseCount);
	if (! (pSemaphore && pSemaphore->_bSemaphore)) {
		setError(ERROR_INVALID_HANDLE); return false;
	}
	if (lReleaseCount < 1) {setError(ERROR_INVALID_PARAMETER); return false;}

	Locker lock(&pSemaphore->_mutex); dbg("ReleaseSemaphore",pSemaphore); bool ok = true;

	if (lpPreviousCount) *lpPreviousCount = pSemaphore->_lCount;

	if (pSemaphore->_lCount + lReleaseCount > pSemaphore->_lCountMax) {
		setError(ERROR_INVALID_PARAMETER); ok = false;
	} else {
		pSemaphore->_bSignaled = true; pSemaphore->_lCount += lReleaseCount;

		list<MultiSynch*>& lpMultiSynchs = pSemaphore->_lpMultiSynchs;
		if (! lpMultiSynchs.empty()) { // Others are multi waiting with me in set.
			for (list<MultiSynch*>::iterator
					  it = lpMultiSynchs.begin(),eol = lpMultiSynchs.end(); it != eol; it++) {
				if ((*it)->signal(pSemaphore) && --pSemaphore->_lCount == 0) {
					pSemaphore->_bSignaled = false; break; // Change state here.
				}
			}
		}
		if (pSemaphore->_bSignaled)
			if (pSemaphore->_lCount > 1) pthread_cond_broadcast(&pSemaphore->_cond);
			else								  pthread_cond_signal(	 &pSemaphore->_cond);
	}
	dbg("ReleaseSemaphore",pSemaphore,ok); return ok;
}
LONG SemaphoreValue(HANDLE hSemaphore) {
	Semaphore* pSemaphore = (Semaphore*) hSemaphore; dbgI("SemaphoreValue",pSemaphore);
	if (! (pSemaphore && pSemaphore->_bSemaphore)) {
		setError(ERROR_INVALID_HANDLE); return (LONG) -1L;
	}
	Locker lock(&pSemaphore->_mutex); dbg("SemaphoreValue",pSemaphore,true);
	return pSemaphore->_lCount;
}

// --------------------- Wait for Object ------------------------------------------------

DWORD WaitForSingleObject(HANDLE hObject,DWORD dwMilliseconds) {
	Synch* pSynch = (Synch*) hObject;
	dbgI(dwMilliseconds == INFINITE,"WaitForSingleObject",pSynch);
	dbgI(dwMilliseconds != INFINITE,"WaitForSingleObject",pSynch,
		  "msec %lu",dwMilliseconds);
	if (! pSynch) {setError(ERROR_INVALID_HANDLE); return WAIT_FAILED;}

    Mutex* pMutex = 0;
    Semaphore* pSemaphore = 0;

	if		  (pSynch->_bMutex)		pMutex	  = (Mutex*)	  pSynch;
	else if (pSynch->_bSemaphore) pSemaphore = (Semaphore*) pSynch;

	Locker lock(&pSynch->_mutex); dbg("WaitForSingleObject",pSynch);
	DWORD rc = WAIT_OBJECT_0;

	if (pSynch->_bMutex && pMutex->_owner == wthread_self()) pMutex->_iLevel++;
	else {
		if (pSynch->_bMutex && ! (pSynch->_bSignaled || isThreadAlive(pMutex->_owner))) {
			pSynch->_bSignaled = true; rc = WAIT_ABANDONED;
			prt("<ERROR> Mutex %#lx is owned by a zombie thread _%lx.\n",
				 pMutex,pMutex->_owner);
            #ifdef DEBUG
			//    asm { int 3 }
			EnterDebugger();
            #endif
		}
		
		if (dwMilliseconds == INFINITE)
			while (! pSynch->_bSignaled)
				pthread_cond_wait(&pSynch->_cond,&pSynch->_mutex);
		else {
			struct timespec time; mkTime(dwMilliseconds,&time,true); DWORD ec = 0;
			while (! (pSynch->_bSignaled ||
						 (ec = pthread_cond_timedwait(&pSynch->_cond,&pSynch->_mutex,&time))));
			switch (ec) {
			case ETIMEDOUT: rc = WAIT_TIMEOUT; break;
			case EINTR:		 rc = WAIT_FAILED;  break;
			}
		}
		if (pSynch->_bSignaled && ! pSynch->_bManualReset) { // Change state.
			if (pSynch->_bSemaphore) {
				if (--pSemaphore->_lCount == 0) pSynch->_bSignaled = false;
			} else {
				pSynch->_bSignaled = false;
				if (pSynch->_bMutex) {pMutex->_iLevel = 1; pMutex->_owner = wthread_self();}
			}
		}
	}
	if (rc == WAIT_FAILED) setError(ERROR_FUNCTION_FAILED);
	dbgR("WaitForSingleObject",pSynch,rc); return rc;
}

static inline void releaseObjects(DWORD nCount,HANDLE* lpHandles,
											 MultiSynch** pMultiSynchs,bool ok) {
	for (DWORD i = nCount; i > 0;) {					// Release mutexes backwards.
		Synch* pSynch = (Synch*) lpHandles[--i]; pthread_mutex_unlock(&pSynch->_mutex);
		dbg(! pMultiSynchs[i],"WaitForMultipleObjects",pSynch,ok);
	}
}
static inline void removeMultiSynchs(DWORD nCount,HANDLE* lpHandles,
												 MultiSynch** pMultiSynchs,bool isLocked,bool ok) {
	for (DWORD i = nCount; i > 0;) {
		Synch* pSynch = (Synch*) lpHandles[--i]; MultiSynch* pMultiSynch = pMultiSynchs[i];
		if (pMultiSynch) {
			if (! isLocked) pthread_mutex_lock(&pSynch->_mutex);
			dbg("WaitForMultipleObjects",pSynch);

			list<MultiSynch*>& lpMultiSynchs = pSynch->_lpMultiSynchs;
			for (list<MultiSynch*>::iterator
					  it = lpMultiSynchs.begin(),eol = lpMultiSynchs.end(); it != eol; it++)
				if (*it == pMultiSynch) {lpMultiSynchs.erase(it); break;}

			dbg("WaitForMultipleObjects",pSynch,ok);
			pthread_mutex_unlock(&pSynch->_mutex); delete pMultiSynch;
		} else
			if (isLocked) pthread_mutex_unlock(&pSynch->_mutex);
	}
}
DWORD WaitForMultipleObjects(DWORD nCount,HANDLE* lpHandles,
									  BOOL bWaitAll,DWORD dwMilliseconds) {
	dbgI(dwMilliseconds == INFINITE,"WaitForMultipleObjects",
		  "count %lu waitAll %li",nCount,bWaitAll);
	dbgI(dwMilliseconds != INFINITE,"WaitForMultipleObjects",
		  "count %lu waitAll %li msec %lu",nCount,bWaitAll,dwMilliseconds);

	if (! nCount) {setError(ERROR_INVALID_PARAMETER); return WAIT_FAILED;}

	MultiSynch** pMultiSynchs = new(nothrow) MultiSynch*[nCount];
	if (! pMultiSynchs) {setError(ERROR_NOT_ENOUGH_MEMORY); return WAIT_FAILED;}

    DWORD i = 0;
    DWORD j = 0;
    DWORD n = 0;
    DWORD rc = WAIT_OBJECT_0;
    Synch* pSynch = 0;
    Mutex* pMutex = 0;
    Semaphore* pSemaphore = 0;
    MultiSynch* pMultiSynch = 0;

	pthread_mutex_t mutex;
	pthread_cond_t	 cond;
	Synch*			 pObject = NULL;

	for (i = 0; i < nCount; i++) {
		pSynch = (Synch*) lpHandles[i]; pMultiSynchs[i] = NULL;
		if (! pSynch) {
			releaseObjects(i,lpHandles,pMultiSynchs,false);
			setError(ERROR_INVALID_HANDLE); return WAIT_FAILED;
		}
		pthread_mutex_lock(&pSynch->_mutex); dbg("WaitForMultipleObjects",pSynch);
	}

	// Look for already signaled objects or Mutexes that I own.

	for (j = 0; j < nCount; j++) {
		pSynch = (Synch*) lpHandles[j];
		if		  (pSynch->_bMutex)		pMutex	  = (Mutex*)	  pSynch;
		else if (pSynch->_bSemaphore) pSemaphore = (Semaphore*) pSynch;

		if (pSynch->_bMutex && ! (pSynch->_bSignaled || isThreadAlive(pMutex->_owner))) {
			pSynch->_bSignaled = true; rc = WAIT_ABANDONED;
			prt("<ERROR> Mutex %#lx is owned by a zombie thread _%lx.\n",
				 pMutex,pMutex->_owner);
            #ifdef DEBUG
			//    asm { int 3 }
			EnterDebugger();
            #endif
		}
		
		if (pSynch->_bMutex && pMutex->_owner == wthread_self()) {
			pMutex->_iLevel++; if (bWaitAll) n++; else {rc += j; break;}
		} else if (pSynch->_bSignaled) {
			if (! pSynch->_bManualReset) { // Change state.
				if (pSynch->_bSemaphore) {
					if (--pSemaphore->_lCount == 0) pSynch->_bSignaled = false;
				} else {
					pSynch->_bSignaled = false;
					if (pSynch->_bMutex) {
						pMutex->_iLevel = 1; pMutex->_owner = wthread_self();
					}
				}
			}
			if (bWaitAll) n++; else {rc += j; break;}
		}
	}
	if (bWaitAll ? n == nCount : j < nCount)
		releaseObjects(nCount,lpHandles,pMultiSynchs,true);
	else {
		// Didn't find any/all, I'll setup objects to tell me when signaled.

		mkMutex(&mutex); mkCond(&cond); pthread_mutex_lock(&mutex);

		for (i = 0; i < nCount; i++) {
			pSynch = (Synch*) lpHandles[i];

			if (pSynch->_bSignaled || // Already handled above.
				 pSynch->_bMutex && ((Mutex*) pSynch)->_owner == wthread_self()) continue;

			if (! (pMultiSynch = new(nothrow) MultiSynch)) {
				pthread_mutex_unlock(&mutex); rmMutex(&mutex); rmCond(&cond);
				removeMultiSynchs(nCount,lpHandles,pMultiSynchs,true,false);
				delete[] pMultiSynchs; setError(ERROR_NOT_ENOUGH_MEMORY); return WAIT_FAILED;
			}
			pMultiSynchs[i] = pMultiSynch;

			pSynch->_lpMultiSynchs.push_back(pMultiSynch);	// Ordered by time.

			pMultiSynch->_pMutex		= &mutex;							// Exclusive access.
			pMultiSynch->_pCond		= &cond;								// For object to signal on
			pMultiSynch->_bSignaled = false;								// once only
			pMultiSynch->_ppObject	= bWaitAll ? NULL : &pObject; // telling me who he is,
			pMultiSynch->_waiter		= wthread_self();					// telling him who I am.
		}

		// The released objects will still be on hold until the `mutex' is released in
		// the wait, preventing them from signaling me before I'm ready for it.

		releaseObjects(nCount,lpHandles,pMultiSynchs,true);

		// Wait for any/all objects to signal me when signaled.

		if (dwMilliseconds == INFINITE)
			while (! (bWaitAll ? n++ == nCount : pObject != NULL))
				pthread_cond_wait(&cond,&mutex);
		else {
			struct timespec time; mkTime(dwMilliseconds,&time,true); DWORD ec = 0;
			while (! ((bWaitAll ? n++ == nCount : pObject != NULL) ||
						 (ec = pthread_cond_timedwait(&cond,&mutex,&time))));
			switch (ec) {
			case ETIMEDOUT: rc = WAIT_TIMEOUT; break;
			case EINTR:		 rc = WAIT_FAILED;  break;
			}
		}

		// State changes to waited for objects have to be done on the signaling side, as,
		// when waiting for all, multiple objects could be signaled before we get a chance
		// to return from our cond_wait, thus getting access only to the last value of
		// `pObject'. Therefore, `pObject' is of no use when `bWaitAll' is true.

		pthread_mutex_unlock(&mutex); rmMutex(&mutex); rmCond(&cond);

		// I'm done, clean up and leave.

		removeMultiSynchs(nCount,lpHandles,pMultiSynchs,false,true);

		if (pObject) { // Determine index of first signaled object.
			for (j = 0; j < nCount && (Synch*) lpHandles[j] != pObject; j++);
			if (j < nCount) rc += j;
		}
	}
	delete[] pMultiSynchs;

	if (rc == WAIT_FAILED) setError(ERROR_FUNCTION_FAILED);

	dbg(pObject,"WaitForMultipleObjects","objects[%lu] == %#lx signaled",j,pObject);
	dbgR(! pObject,"WaitForMultipleObjects",rc);

	return rc;
}

// --------------------- Close Object ---------------------------------------------------

BOOL CloseHandle(HANDLE hObject) {
	Synch* pSynch = (Synch*) hObject; dbgI("CloseHandle",pSynch); bool ok;
	if (pSynch) {
		DWORD ec1 = rmMutex(&pSynch->_mutex);
		DWORD ec2 = rmCond( &pSynch->_cond);

		if (ec1 == EBUSY || ec2 == EBUSY) {
			setError(ERROR_BUSY); ok = false;
		} else {
			delete pSynch; ok = true;
		}
	} else {
		setError(ERROR_INVALID_HANDLE); ok = false;
	}
	dbg("CloseHandle",ok); return ok;
}

// --------------------- Debug Name -----------------------------------------------------

void dbgName(HANDLE hObject,const char* sName) {
	if (hObject) {
		Synch* pSynch = (Synch*) hObject;
		pSynch->_sName = new char[strlen(sName) + 1]; strcpy(pSynch->_sName,sName);
	}
}

// ================== Critical Section ==================================================

#if defined SYM_LIBC_NLM || defined LINUX
void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
	dbg("InitializeCriticalSection","%#lx",lpCriticalSection);
	mkMutex(lpCriticalSection,true);
}
void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
	pthread_mutex_lock(lpCriticalSection);
}
void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
	pthread_mutex_unlock(lpCriticalSection);
}
void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
	dbg("DeleteCriticalSection","%#lx",lpCriticalSection);
	rmMutex(lpCriticalSection);
}
#endif
