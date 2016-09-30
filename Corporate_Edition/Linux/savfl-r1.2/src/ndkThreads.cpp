// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Thread Management functions

// ================== ndkThreads ========================================================

// Implements Thread Management related functions on Linux using Posix Threads as they
// have been implemented in the `LinuxThreads' package as part of `glibc'.
//
// The philosophy has been to follow the interface of the Windows threads management
// functions with the same function name, argument types, return type and values.
// However, to avoid making too many changes to the NetWare only source code, we
// have in some cases used the NDK interface instead.
//
// This package consists of a number of utility classes used by the `ndkThreads' API
// functions.	As the parts of RTVScan that uses these functions mostly are plain C, the
// include file for this package, "ndkThreads.h", is in C.	Therefore, the classes
// defined here are only for internal use at the moment.

#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "ndkThreads.h"
#include "ntx.h"
#include "symnts.h"

#include "NLM.H"

#include <iostream>
#include <map>

#include "Lock.hpp"
#include "Time.hpp"

extern "C"
{
    #include "dprintf.h"
    #include "dprintf_2.h"
}

static DEBUGFLAGTYPE debug = DEBUGPRINT;


//#define DBG_THREADS

// ================== Utilities =========================================================

// The utility classes are currently for internal use only. Should they be made available
// to other packages as well, they need to be provided with testing for NULL pointer
// access with proper exception or other handling when detected.

#define GO		 100
#define NIL		 101
#define TIMEOUT 102
#define dbgResult(ok) ok == GO		? "..."		  :							\
							 ok == NIL		? ""			  :							\
							 ok == TIMEOUT ? "timeout"	  :							\
							 ok				? "succeeded" : "failed"

// --------------------- Exceptions -----------------------------------------------------

// Currently, we have only one exception, `Fail'.
//
// For future expansions, we have introduced the common superclass `Exception' for all
// exceptions, with the intension of having a hierarchical model to allow for
// multileveled exception handlers from the most general handler of any exception type
// all the way down to the handling of one specific exception type only.
//
// As we have to statically link stdc++ in order to be able to build on RHAS-2.1 and use
// those executables on the other platforms, we have to avoid using exceptions.

class Exception {};

class Fail : public Exception {};

// --------------------- Call -----------------------------------------------------------

// The `Call' class facilitates the transfer of a function pointer together with its list
// of arguments as one single object that the recipient then can invoke using the `apply'
// member function. Note that a `Call' object can only be created with an actual function
// pointer and its argument -- the empty object is non-sensical.

class Call {
public:
	Call(void (*function)(void*),void* arg);

	void apply() const;
    pid_t pid() const {return _pid;}

private:
	void (*_function)(void*);
	void* _arg;
    mutable pid_t _pid;

	Call();
};

Call::Call(void (*function)(void*),void* arg) :
    _function(function),
    _arg(arg),
    _pid(0)
{}

void Call::apply() const
{
    _pid = getpid();
    _function(_arg);
}

// --------------------- ThreadData -----------------------------------------------------

// Each thread in RTVScan running on Linux has a `ThreadData' object that is used to
// implement used NetWare thread functionality:
//
//		Each thread has a name.	 At creation, the name is set to the empty string. The API
//		functions `GetThreadName' and `RenameThread' are used to change the name and to
//		extract it.
//
//		Each thread has a thread group identifier.  By default, all threads belong to the
//		thread group "0", i.e. when they are created using the `BeginThread' function.  The
//		`BeginThreadGroup' function creates a thread with a unique thread group
//		identifier. This can be read by `GetThreadGroupID' and be set in another thread
//		using this value with `SetThreadGroupID'.	 In order to ensure that each new
//		invocation generates a new thread group identifier, the static variable
//		`ThreadData::ThreadGroupID' keeps track of the last number used.	Note that even if
//		all threads in a thread group are gone, the thread group number is not reused.
//
//		A thread can suspend itself or be suspended by another thread.	 As the method to
//		resume a thread is different depending on whether it was self suspended or not, we
//		have to keep track of which case it is.
//
//		A thread can be suspended multiple times and will not be resumed until the same
//		number of resumes have been performed. A counter keeps track of this.
//
//		The call object used to start the thread is remembered in order to allow for its
//		deletion when the thread is terminated.
//
// These data for a thread need to be accessible both from its own thread as well as from
// other threads. A static datastructure `ThreadData::ThreadDataMap' maps the thread
// identifier to the `ThreadData' object for each thread.
//
// The static member functions are used to access the static variables. `select' locates
// the `ThreadData' object of a thread given its thread identifier.	`remove' will
// remove the `ThreadData' object from the map and delete it.
//
// The only public `ThreadData' constructor requires a thread identfier as input.  It
// will create the `ThreadData' object for the thread and insert it at the appropriate
// place in the static `ThreadData::ThreadDataMap' variable.
//
// The remaining public member functions handle access to the member variables.
//
// Note that the non-argument constructor is private -- a `ThreadData' object can not
// exist without being attached to a thread.	 Also, the copy constructor and the
// assignment operator are made private -- the `ThreadData' object is tightly coupled to
// its thread and there is no need to have multiple representations of the same
// `ThreadData' object.
//
// The destructor is also private. It is only called at the end of the life of the thread
// from the static member function `remove'.
//
// Note that each access to the static member variables is protected by a
// `ThreadDataMutex' that is locked for the duration of each `ndkThreads' API function.

class ThreadData {
public:
	static ThreadData* select(pthread_t tid = pthread_self());
	static void			 remove(pthread_t tid = pthread_self());

	static int			 nr();							// map size -- for debug
	static void			 dbg();							// print map

	ThreadData(pthread_t tid,Call* pCall);
	ThreadData(pthread_t tid,Call* pCall,int& iGroup);
	~ThreadData();

	const Call* call() const;
	void call(const Call* pCall);

	const char* name() const;
	void name(const char* sName);

	int  group() const;
	void group(int iGroup);

	bool isSelfSuspended() const;
	void selfSuspend();
	void selfResume();

	long suspendCount() const;
	long suspend();
	long resume();

	DWORD getLastError() const;
	void setLastError( DWORD dwErrCode );

private:
	#define tThreadDataMap std::map<pthread_t,ThreadData*,std::less<pthread_t> >

	static tThreadDataMap ThreadDataMap;
	static int				 ThreadGroupID;
	static ThreadData*	 MainThreadData;
	static char*		    BlankName;

	Call* _pCall;
	char* _sName;
	int	_iGroup;
	bool	_isSelfSuspended;
	long	_lSuspendCount;
	DWORD	_dwLastError;

	ThreadData();
	ThreadData(				 const ThreadData& that);
	ThreadData& operator=(const ThreadData& that);

	void init();
	void init(pthread_t tid,Call* pCall);
};

tThreadDataMap ThreadData::ThreadDataMap;
int				ThreadData::ThreadGroupID	= 0;	// ??? need more than 2,147,483,647 ?
ThreadData*		ThreadData::MainThreadData = new ThreadData(pthread_self(),NULL);
char* 		 ThreadData::BlankName = "";

ThreadData* ThreadData::select(pthread_t tid) {
	tThreadDataMap::iterator i = ThreadDataMap.find(tid);
	return (i != ThreadDataMap.end()) ? (*i).second : NULL;
}
void ThreadData::remove(pthread_t tid) {
	tThreadDataMap::iterator i = ThreadDataMap.find(tid);
	if (i != ThreadDataMap.end()) {
		delete (*i).second; ThreadDataMap.erase(tid);
	}
}
int ThreadData::nr() {return ThreadDataMap.size();}

void ThreadData::dbg() {
    tThreadDataMap::iterator i = ThreadDataMap.begin(); int j = 0;
    while (i != ThreadDataMap.end()) {
        std::pair<pthread_t,ThreadData*> element = *i;
        pthread_t tid = element.first;
        ThreadData* d = element.second;
        dprintf( "0x%x: %3d(%d) <0x%x[0x%x]|%p() \"%s\" 0x%x %i %d>\n",
                static_cast<uint>(pthread_self()),
                ++j,
                ThreadDataMap.size(),
                static_cast<uint>(tid),
                static_cast<uint>(d->call() ? d->call()->pid() : 0),
                d->call(),
                d->name(),
                d->group(),
                d->isSelfSuspended(),
                static_cast<int>(d->suspendCount()));
        i++;
    }
}

ThreadData::ThreadData(pthread_t tid,Call* pCall) {
	init(tid,pCall);
}

ThreadData::ThreadData(pthread_t tid,Call* pCall,int& iGroup) {
	init(tid,pCall); iGroup = _iGroup = ++ThreadGroupID;
}

void ThreadData::init() {
	_pCall = NULL;
	_sName= BlankName;
	_iGroup= 0;
	_isSelfSuspended = false;
	_lSuspendCount = 0;
	_dwLastError = ERROR_SUCCESS;
}

void ThreadData::init(pthread_t tid,Call* pCall) {
	init();
	_pCall = pCall;
	tThreadDataMap::iterator i = ThreadDataMap.find(tid);
	if (i == ThreadDataMap.end()) {
		std::pair<pthread_t,ThreadData*> element(tid,this);
		ThreadDataMap.insert(element);
	}
}

ThreadData::~ThreadData()
{
	if ( _sName ) {
		if ( _sName != BlankName )
			free(_sName);
		_sName = 0;
	}
}

const Call* ThreadData::call() const	  {return _pCall;}
void			ThreadData::call(const Call* pCall) {_pCall = (Call*) pCall;}

const char* ThreadData::name() const
{
	return _sName;
}

void ThreadData::name(const char* sName)
{
	if ( _sName != 0 && _sName != BlankName ) {
		free(_sName);
	}
	_sName = (sName) ? strdup(sName) : BlankName;
}

int  ThreadData::group() const {return _iGroup;}
void ThreadData::group(int iGroup)	  {_iGroup = iGroup;}

bool ThreadData::isSelfSuspended() const {return _isSelfSuspended;}
void ThreadData::selfSuspend()						{_isSelfSuspended = true;}
void ThreadData::selfResume()							{_isSelfSuspended = false;}

long ThreadData::suspendCount() const {return _lSuspendCount;}
long ThreadData::suspend()				  {return _lSuspendCount++;}
long ThreadData::resume()				  {return _lSuspendCount--;}

DWORD ThreadData::getLastError() const {return _dwLastError;}
void  ThreadData::setLastError(DWORD dwErrCode) {_dwLastError = dwErrCode;}

// ================== Thread ============================================================

static pthread_mutex_t ThreadDataMutex = PTHREAD_MUTEX_INITIALIZER;

// --------------------- Thread Creation ------------------------------------------------

// A thread is spawned off by calling the `BeginThread' function, indicating the function
// to call in the newly created thread as well as the argument to use at this. All
// threads are created detached from the mother thread -- we have no need to join with it
// after it has exited. The stack size can also be set, a "0" size will cause the stack
// size to be set to a default value.
//
// As the thread has been successfully created, a `ThreadData' object is created for it
// and inserted in the `ThreadData::ThreadDataMap' static variable.	Note that the
// `ThreadDataMutex' is locked until after this object is created -- this prevents the
// newly created thread from accessing it's own `ThreadData' object until it really
// exists.
//
// When a thread is created, we start it's execution in the `mkThread' function by
// calling the user defined function via the `Call' object.
//
// At the end of the life of the thread, be it that it runs over the edge of its initial
// function, or calls `ExitThread' to kill itself, the `rmThread' function will remove
// the `ThreadData' object of the thread from the `ThreadData::ThreadDataMap' variable,
// delete the `ThreadData' object and the `Call' object.
//
// `GetCurrentThreadId' returns the current thread's thread identifier.

#ifdef DBG_THREADS
static void dbgThread(const char* op,pthread_t tid,int ok) {
	pthread_t self = pthread_self(); const char* r = dbgResult(ok);
	if (tid)
		dprintf("%u: %sThread <%u> %s\n",self,op,tid,r);
	else
		dprintf("%u: %sThread %s\n",self,op,r);
}
#else
	#define dbgThread(op,tid,ok)
#endif

static void rmCallThread(Call* pCall) {
    if (pCall)
        delete pCall;
    ThreadData::remove();
    dbgThread("rm", 0, NIL);
}
static void rmThread(Call* pCall) {
	Lock lock(&ThreadDataMutex); rmCallThread(pCall);
}
static void rmThread() {
	Lock lock(&ThreadDataMutex); ThreadData* pThreadData = ThreadData::select();
	rmCallThread(pThreadData ? (Call*) pThreadData->call() : NULL);
}
static void* mkThread(void* arg) {
    dbgThread("mk", 0, NIL);
    Call* pCall = (Call*) arg;
    pCall->apply();
    rmThread(pCall);

    return NULL;
}
static pthread_t beginThread(Call* pCall,size_t size) {
	pthread_t tid; pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if (size) pthread_attr_setstacksize(&attr,size);

	bool ok = pthread_create(&tid,&attr,mkThread,pCall) == 0;
	dbgThread("begin",tid,ok);

	pthread_attr_destroy(&attr);
	return ok ? tid : static_cast<pthread_t>(EFAILURE);
}

HANDLE BeginThread(void (*pFunc)(void*), void* pArg, size_t size) {
	Call* pCall = new Call(pFunc,pArg);
	Lock lock(&ThreadDataMutex);
	pthread_t tid = beginThread(pCall,size);
	if (tid != static_cast<pthread_t>(EFAILURE))
		new ThreadData(tid,pCall);
	return reinterpret_cast<HANDLE>(tid);
}

void ExitThread() {
	rmThread();
	pthread_exit(NULL);
}

// --------------------- Thread Group ---------------------------------------------------

// The `BeginThreadGroup' function will create a new thread just like the `BeginThread'
// function.  However, instead of the default "0" thread group, a new thread group is
// created and preserved in the thread's `ThreadData' object.	The thread group
// identifier is returned from the `BeginThreadGroup' call, but can also later be
// accessed through `GetThreadGroupID' for its own thread.
//
// The thread group identifier can be passed on to another thread and be used to set that
// thread's thread group identifier using `SetThreadGroupID'.
//
// Implementation follows NDK API

HANDLE BeginThreadGroup(void (*pFunc)(void*),
								void* pStack,unsigned uStackSize,void* pArg) 
{
	Call* pCall = new Call(pFunc, pArg);
	Lock lock(&ThreadDataMutex);
	pthread_t tid = beginThread(pCall, uStackSize);
	int iGroup = EFAILURE;
	if (tid != static_cast<pthread_t>(EFAILURE))
		new ThreadData(tid, pCall, iGroup);
	return reinterpret_cast<HANDLE>(iGroup);
}

HANDLE GetThreadGroupID() {
	Lock lock(&ThreadDataMutex);
	ThreadData* pThreadData = ThreadData::select();
	HANDLE result =
		reinterpret_cast<HANDLE>(pThreadData ? pThreadData->group() : EFAILURE);
	return result;
}

HANDLE SetThreadGroupID(HANDLE newThreadGroupID) {
	Lock lock(&ThreadDataMutex);
	ThreadData* pThreadData = ThreadData::select();
	HANDLE oldThreadGroupID = reinterpret_cast<HANDLE>(EFAILURE);
	if (pThreadData) {
		oldThreadGroupID = reinterpret_cast<HANDLE>(pThreadData->group());
		pThreadData->group(reinterpret_cast<int>(newThreadGroupID));
	}
	return oldThreadGroupID;
}

// --------------------- Thread Name ----------------------------------------------------

// `GetThreadName' and `RenameThread' are used to access any thread's thread name using
// the thread's thread identifier.
//
// Implementation follows NDK API

int GetThreadName(HANDLE hThread,char* sName) {
	pthread_t tid = (pthread_t) hThread;
	Lock lock(&ThreadDataMutex);
	ThreadData* pThreadData = ThreadData::select(tid);
	if (! pThreadData)
		return EBADHNDL;
	strcpy(sName,(char*) pThreadData->name());
	return ESUCCESS;
}
int RenameThread(HANDLE hThread,const char* sName) {
	pthread_t tid = (pthread_t) hThread;
	Lock lock(&ThreadDataMutex);
	ThreadData* pThreadData = ThreadData::select(tid);
	if (! pThreadData)
		return EBADHNDL;
	pThreadData->name(sName);
	return ESUCCESS;
}

// --------------------- Thread Error ---------------------------------------------------

// See `Get/SetLastError in `winThreads'.

DWORD ndkGetLastError(HANDLE hThreadData) {
	DWORD dwRet = ERROR_SUCCESS;
	
	ThreadData* pThreadData = reinterpret_cast<ThreadData*>(hThreadData);
	if (pThreadData) 
		dwRet = pThreadData->getLastError();

	return dwRet;
}
void ndkSetLastError(HANDLE hThreadData,DWORD dwErrCode) {
	ThreadData* pThreadData = reinterpret_cast<ThreadData*>(hThreadData);
	if (pThreadData) 
		pThreadData->setLastError(dwErrCode);
}

// Synchronization needed to stop the thread from usings its ThreadData until the thread
// that has started it has had a chance to create the ThreadData entry for it.

HANDLE ndkGetThreadData() {
	Lock lock(&ThreadDataMutex); return reinterpret_cast<HANDLE>(ThreadData::select());
}

// --------------------- Thread Data ----------------------------------------------------

// For debug purposes only.

int nrThreadData() {Lock lock(&ThreadDataMutex); return ThreadData::nr();}

void dbgThreadData() {Lock lock(&ThreadDataMutex); ThreadData::dbg();}

// ================== Suspend/Resume ====================================================

// `Suspend' will halt the execution of the indicated thread until the `Resume' function
// is called for the thread. This implementation follows the Windows API.
//
// Note that the Posix standards committee strongly object against the inclusion of
// Suspend/Resume functionality on the grounds of the programming pitfalls it saw with a
// thread holding locked mutexes when it was suspended.	There are other ways to
// synchronize between threads.
//
// We support `Suspend' and `Resume' here to be able to run existing code.	 At the same
// time, we strongly advise against using them in newly written code.
//
// At this time, a problem in the Linux 2.4 kernel prevents us from supporting suspending
// a thread from another thread. We prevent all attempts by simply aborting execution.
//
// *	Thread Suspended by Another Thread -- currently NOT implemented !
//
//		The suspend counter of the thread to be suspended is increased by "1". If it was
//		"0" before, i.e. it is executing, the SIGSTOP signal is sent to it and it is forced
//		to fall asleep. Note that Linux is preemptive, so the execution will stop
//		immediately (almost).
//
//		Repeated `Suspend' will only cause the suspend counter to increase.
//
//		Equal numbers of `Resume' will have to be called for the thread to be allowed to
//		wake up. Each `Resume' only causes the suspend counter to decrease by "1" until it
//		becomes "1". Another `Resume' will now cause the counter to reach "0" and send the
//		SIGCONT signal that will force the thread to resume its execution from the point
//		where it was suspended.
//
//		The value of the suspend counter before the `Suspend' or 'Resume' is returned
//		unless the signal sending fails and a "-1" is returned to indicate the failure.
//
// *	Self Suspended Thread
//
//		If `Suspend' is done on its own thread, the thread increases the suspend counter
//		from "0" to "1" -- note that the counter really always is "0" in this case, as if
//		the thread was suspend by another thread first, it would never ba able to suspend
//		itself! It indicates that it is self suspended and then falls asleep waiting for
//		the suspend counter to become "0".
//
//		Other threads may still suspend the thread. The suspend counter for it will be
//		increased by "1" each time. As the suspend counter was larger that "0" at the
//		start, no SIGSTOP signal will be sent -- the thread is already asleep.
//
//		Equal numbers of `Resume' must now be called before the thread will awake.	 Of
//		course, you can't self resume!  Once the suspend counter has reached "1", a new
//		`Resume' will signal the condition variable and decrease it to "0". Note that the
//		mutex on the condition variable is locked until this is done.	Once the `Resume'
//		function has returned, the self suspended thread will be allowed to check the
//		condition variable, realize that it is "0", and resume the execution by returning
//		from the `Suspend' function.

static pthread_cond_t SuspendCond = PTHREAD_COND_INITIALIZER;

#ifdef DBG_THREADS
static void dbgSuRe(const char* op,pthread_t tid,ThreadData* pThreadData,int ok) {
	dprintf("%u: %sThread <%u|%i> %s\n",
			 pthread_self(),op,tid,pThreadData->suspendCount(),dbgResult(ok));
}
#else
	#define dbgSuRe(op,tid,pThreadData,ok)
#endif

long SuspendThread(HANDLE hThread) {
	pthread_t tid = (pthread_t) hThread; Lock lock(&ThreadDataMutex,&SuspendCond);
	ThreadData* pThreadData = ThreadData::select(tid); if (! pThreadData) return -1;

	long lCount = pThreadData->suspend();
	if (tid == pthread_self()) {
		pThreadData->selfSuspend(); dbgSuRe("Suspend",tid,pThreadData,GO);

		while (pThreadData->suspendCount() > 0) lock.wait();

		pThreadData->selfResume(); dbgSuRe("Suspend",tid,pThreadData,true);
	} else {
		std::cerr << "`SuspendThread(other thread)' is NOT supported => ABORTED !"
					 << std::endl; abort();
//		if (lCount == 0 && pthread_kill(tid,SIGSTOP) != 0) lCount = -1;
//		dbgSuRe("Suspend",tid,pThreadData,lCount == 0 ? true : lCount == -1 ? false : NIL);
	}
	return lCount;
}
long ResumeThread(HANDLE hThread) {
	pthread_t tid = (pthread_t) hThread; Lock lock(&ThreadDataMutex,&SuspendCond);
	ThreadData* pThreadData = ThreadData::select(tid); if (! pThreadData) return -1;

	long lCount = pThreadData->suspendCount();
	dbgSuRe("Resume",tid,pThreadData,lCount == 1 ? GO : NIL);
	switch (lCount) {
	case 0: break;
	case 1:
		if (pThreadData->isSelfSuspended())
			lock.signal();
		else {
			std::cerr << "`ResumeThread(thread suspended by other thread)' "
						 << "is NOT supported => ABORTED !" << std::endl; abort();
//			if (pthread_kill(tid,SIGCONT) != 0) lCount = -1;
//			dbgSuRe("Resume",tid,pThreadData,lCount == 1);
		}
	default:
		pThreadData->resume();
	}
	return lCount;
}

// ================== Semaphore =========================================================

// The Posix semaphores are much like the semaphores on NetWare and Windows. We have only
// had to make minor adjustments to the input and return values in order to make the
// Linux implementation look like NetWare on the input side and like Windows on the
// return side.  With this, the Linux semaphore implementation fits nicely into how the
// semaphores are used in RTVScan. The NetWare only code never bothers to check the finer
// details of the return values, while the Windows only code occasionally does. The input
// values on NetWare is a subset of the input values on Windows, where default values are
// used for the other input values.
//
// A semaphore is created using the `OpenSemaphore' function with the initial value as
// input.
//
// `WaitForSemaphore' will cause the current thread to fall asleep when the semaphore
// value is "0" and will only wake up when the value has become larger than "0".	 At this
// moment, the semaphore value will be decreased by "1".
//
// `ReleaseSemaphore' will increase the value of the semaphore by "1".
//
// Working with semaphores, you let a number of threads wait for the semaphore while it
// is "0".	By releasing the semaphore a number of times, you will let the same number of
// formerly waiting threads execute simultaneously.

#ifdef DBG_THREADS
static void dbgSem(const char* op, sem_t* pSemaphore, int ok) {
    dprintf("%u: %sSemaphore <%u|%i> %s\n",
             pthread_self(),
            op,
            pSemaphore,
            pSemaphore ? ExamineSemaphore((HANDLE) pSemaphore) : 0,
            dbgResult(ok));
}
#else
    #define dbgSem(op, pSemaphore, ok)
#endif

HANDLE OpenSemaphore(long lValue) {
	sem_t* pSemaphore = new sem_t;
	bool ok = sem_init(pSemaphore,false,lValue) == 0; dbgSem("Open",pSemaphore,ok);
	if (ok) return (HANDLE) pSemaphore; delete pSemaphore; return NULL;
}
BOOL CloseSemaphore(HANDLE hSemaphore) {
	sem_t* pSemaphore = (sem_t*) hSemaphore;	dbgSem("Close",pSemaphore,GO);
	bool ok = (sem_destroy(pSemaphore) == 0); dbgSem("Close",pSemaphore,ok);
	delete pSemaphore; return ok;
}
LONG ExamineSemaphore(HANDLE hSemaphore) {
	int iValue; sem_getvalue((sem_t*) hSemaphore,&iValue);
	return iValue;
}
BOOL SignalSemaphore(HANDLE hSemaphore) {
	sem_t* pSemaphore = (sem_t*) hSemaphore; dbgSem("Signal",pSemaphore,GO);
	return sem_post(pSemaphore) == 0;
}
unsigned long WaitForSemaphore(HANDLE hSemaphore,unsigned long msec) {
	sem_t* pSemaphore = (sem_t*) hSemaphore; int ok; unsigned long rc;
	const char* op = msec == INFINITE ? "WaitFor" : "TimedWaitFor";
	REF(op);
	dbgSem(op,pSemaphore,GO);
	if (msec == INFINITE) {
		ok = sem_wait(pSemaphore) == 0; rc = ok ? WAIT_OBJECT_0 : WAIT_FAILED;
	} else {
		struct timespec time; mkTime(msec,&time,true);
		ok = sem_timedwait(pSemaphore,&time) == 0; rc = ok ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
		if (! ok) ok = TIMEOUT;
	}
	dbgSem(op,pSemaphore,ok); return rc;
}

// --------------------- NTS ------------------------------------------------------------

// These functions have been copied from
//
//		Core_Technology/LDVP_Shared/src/Common/Misc/NTS.C
//
// and cleaned up up to function properly in the Linux environment.

void NTsSemaClose(HANDLE *pHandle) {
	HANDLE hSema = *pHandle;
	*pHandle = 0;
	if (hSema != 0)
		NTxSemaClose(hSema);
}

HANDLE NTsSemaSignal(HANDLE *pSema) {
	if (*pSema != 0) NTxSemaSignal(*pSema);
	return *pSema;
}

HANDLE NTsSemaWait(HANDLE *pSema) {
	if (*pSema != 0 && NTxSemaWait(*pSema) != WAIT_OBJECT_0) NTsSemaClose(pSema);
	return *pSema;
}

void NTsSemaTimedWait(HANDLE *pSema,unsigned long waitTime) {
	if (*pSema != 0) NTxSemaTimedWait(*pSema,waitTime);
}

void NTsSemaWaitAwhile(HANDLE *pSema,int semaClose) {
	if (*pSema != 0 && NTxSemaTimedWait(*pSema,NTX_TIME_AWHILE) != WAIT_OBJECT_0)
		NTsSemaClose(pSema);
	if (semaClose == 0) NTsSemaClose(pSema);
}

int NTSGetComputerName(char *pComputerName,wchar_t *pWchar) {
	unsigned int i;
	char computerName[NTS_MAX_COMPUTERNAME];
	memset(&computerName[0],0,NTS_MAX_COMPUTERNAME);
	i = NTS_MAX_COMPUTERNAME;
	GetComputerName(&computerName[0],&i);
	if (pComputerName != NULL)
		memcpy(pComputerName,&computerName[0],NTS_MAX_COMPUTERNAME);
	if (pWchar != NULL) {
		memset(pWchar,0,NTS_MAX_COMPUTERNAME * sizeof(wchar_t));
		mbstowcs(pWchar,&computerName[0],NTS_MAX_COMPUTERNAME);
	}
	return ++i;
}

// ================== IPC Semaphore =====================================================

// The IPC Semaphores supports semaphores between different processes as well as between
// different threads within a process. Apart from this, they behave exactly like the
// thread-only semaphores above.
//
// IPC Semaphores are opened using `ipcOpenSemaphore'. A key is created using
// "/etc/hosts" (this file always exists !) inode number and a number that is increased
// by 1 each time `ipcOpenSemaphore' is called. If the semaphore corresponding to the key
// already exists on the system, it is used, otherwise a new semaphore is created. In
// this way we make it possible to access the same semaphore from different processes
// just by calling `ipcOpenSemaphore' in the same order.
//
// A semaphore is only read and write accessible by processes owned by the user that
// owned the process that created it.
//
// Note that each operation that changes the semaphore value, i.e. `ipcReleaseSemaphore'
// and `ipcWaitForSemaphore' is performed using the `SEM_UNDO' flag. The kernel keeps
// track of operations done and in case a process crashes, it will assure that the
// semaphore is cleaned up to avoid dead-locks.

// The `semun' union if missing from <sys/sem.h>. It is found in <linux/sem.h>, but that
// causers collisions when included. Therefore, we have our own copy. Not nice!

union semun {
	int val;							/* value for SETVAL */
	struct semid_ds *buf;		/* buffer for IPC_STAT & IPC_SET */
	unsigned short *array;		/* array for GETALL & SETALL */
	struct seminfo *__buf;		/* buffer for IPC_INFO */
	void *__pad;
};

#ifdef DBG_THREADS
static void dbgIPCkey(const char* op, int iSemaphore, key_t key) {
    dprintf("%u: ipc%sSemaphore %i -> %i\n", pthread_self(), op, iSemaphore, key);
}
static void dbgIPC(const char* op, int iSemid, int ok) {
    dprintf("%u: ipc%sSemaphore <%i|%i> %s\n",
            pthread_self(), op, iSemid,
            iSemid > 0 ? ipcExamineSemaphore(reinterpret_cast<HANDLE>(iSemid)) : -99, dbgResult(ok));
}
#else
	#define dbgIPCkey(op, iSemaphore, key)
	#define dbgIPC(op, iSemid, ok)
#endif

static int ipcSemaphoreSetNumber = 0;				// ??? need more than 2,147,483,647 ?

static pthread_mutex_t ipcOpenSemaphoreMutex = PTHREAD_MUTEX_INITIALIZER;

HANDLE ipcOpenSemaphore(int iSemaphore,long lValue) {
	Lock lock(&ipcOpenSemaphoreMutex);
	key_t key = ftok("/etc/hosts", iSemaphore ? iSemaphore : ++ipcSemaphoreSetNumber);
	dbgIPCkey("Open", iSemaphore, key);
	int iSemid = semget(key, 1, IPC_CREAT | S_IRUSR | S_IWUSR);
	if (iSemid <= 0) {
		dbgIPC("Open", iSemid, false);
		return 0;
	}
	semun ctlInfo = {lValue};
	bool ok = (semctl(iSemid, 0, SETVAL, ctlInfo) == 0);
	dbgIPC("Open", iSemid, ok);
	if (ok)
		return reinterpret_cast<HANDLE>(iSemid);
	semctl(iSemid, 0, IPC_RMID, 0);
	return 0;
}

HANDLE ipcAttachSemaphore(int iSemaphore) {
	key_t key = ftok("/etc/hosts", iSemaphore);
	dbgIPCkey("Attach", iSemaphore, key);
	int iSemid = semget(key, 1, 0);
	if (iSemid <= 0) {
		dbgIPC("Attach", iSemid, false);
		return 0;
	}
	HANDLE hSema = reinterpret_cast<HANDLE>(iSemid);
	dbgIPC("Attach", iSemid, true);
	return hSema;
}
BOOL ipcCloseSemaphore(HANDLE hSemaphore) {
	int iSemid = reinterpret_cast<int>(hSemaphore);
	dbgIPC("Close", iSemid, GO);
	bool ok = (semctl(iSemid, 0, IPC_RMID, 0) == 0);
	dbgIPC("Close", iSemid, ok);
	return ok;
}
long ipcExamineSemaphore(HANDLE hSemaphore) {
	int iSemid = reinterpret_cast<int>(hSemaphore);
	return semctl(iSemid, 0, GETVAL, 0);
}
BOOL ipcReleaseSemaphore(HANDLE hSemaphore) {
	int iSemid = reinterpret_cast<int>(hSemaphore);
	dbgIPC("Release", iSemid, GO);
	struct sembuf opInfo[] = {{0, 1, SEM_UNDO}};
	bool rc = (semop(iSemid, opInfo, 1) == 0);
	return rc;
}
unsigned long ipcWaitForSemaphore(HANDLE hSemaphore, unsigned long msec) {
	int iSemid = reinterpret_cast<int>(hSemaphore);
	int ok;
	unsigned long rc;
	const char* op = msec == INFINITE ? "WaitFor" : "TimedWaitFor";
	REF(op);
	dbgIPC(op,iSemid,GO);
	struct sembuf opInfo[] = {{0,-1,SEM_UNDO}};
	if (msec == INFINITE) {
		ok = semop(iSemid,opInfo,1) == 0;
		rc = ok ? WAIT_OBJECT_0 : WAIT_FAILED;
	} else {
		std::cerr << "`semtimedop()' is NOT supported as it doesn't exist on RHAS-2.1"
					 << " => ABORTED !" << std::endl;
		abort();
//		struct timespec time;
//		mkTime(msec,&time,true);
//		ok = semtimedop(iSemid,opInfo,1,&time) == 0;
//		rc = ok ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
//		if (! ok)
//			ok = TIMEOUT;
	}
	dbgIPC(op,iSemid,ok);
	return rc;
}

// ================== Sleep =============================================================

// Both the NetWare `delay' and the Windows `Sleep' use an integer representing the
// number of milliseconds. The `mkTime' creates the proper Posix `timespec' structure for
// this and calls the Posix function `nanosleep'.

void Sleep(unsigned long msec) {
	struct timespec time;
	mkTime(msec, &time);
	nanosleep(&time, NULL);
}

// ================== Breakpoint ========================================================

// There is no way, AFAIK, to get `gdb' invoked from inside a program stopping in a
// breakpoint at the point of invocation. Instead, `gdb' as to be started, a breakpoint
// be set on this function, and then program under scrutiny be started. Execution will
// stop here and then its just to navigate with the regular stack inspections and
// stepping instructions to investigate the state at this point.

void Breakpoint(int iBreakFlag) {
	std::cout << "Break with flag = " << iBreakFlag << std::endl;
}
