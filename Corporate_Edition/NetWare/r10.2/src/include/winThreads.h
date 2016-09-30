// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: NetWare implementation of Windows thread management functions

#ifndef _winThreads_h_
#define _winThreads_h_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SYM_LIBC_NLM
	#include <stdint.h>
	#include <pthread.h>

	#include "libcnlm.h"
#elif NLM
	#include "ClibNLM.h"

	typedef int				pthread_t;
	typedef unsigned int uintptr_t;
#endif

typedef unsigned long wthread_t;

// ================== Processes =========================================================

DWORD GetCurrentProcessId();

// ================== Threads ===========================================================

uintptr_t _beginthread(void (*pStart_address)(void*),
							  unsigned uiStack_size,void* pArglist);
uintptr_t _beginthreadex(void* pSecurity,unsigned uiStack_size,
								 unsigned (*pStart_address)(void*),void* pArglist,
								 unsigned uiInitflag,unsigned* uipThrdaddr);
void _endthread();
void _endthreadex(unsigned uiRetval);

BOOL TerminateThread(HANDLE hThread,DWORD dwExitCode);

DWORD     GetCurrentThreadId();
pthread_t GetCurrentPthreadId();
void      GetThreads(  DWORD* pnCount,HANDLE**	  ppThreads);
void      GetThreadIDs(DWORD* pnCount,wthread_t** ppThreadIDs);

BOOL GetExitCodeThread(HANDLE hThread,LPDWORD lpExitCode);

BOOL isThreadAlive(wthread_t wtid);
BOOL ExistThreads();

HANDLE GetInitializerThread();

// ================== Synchronization Objects ===========================================

HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,
						 BOOL bManualReset,BOOL bInitialState,LPTSTR lpName);
BOOL SetEvent(	 HANDLE hEvent);
BOOL ResetEvent(HANDLE hEvent);

HANDLE CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,
						 BOOL bInitialOwner,LPCTSTR lpName);
BOOL ReleaseMutex(HANDLE hMutex);

HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
							  LONG lInitialCount,LONG lMaximumCount,LPCTSTR lpName);
BOOL ReleaseSemaphore(HANDLE hSemaphore,LONG lReleaseCount,LPLONG lpPreviousCount);
LONG ExamineSemaphore(HANDLE hSemaphore);

DWORD WaitForSingleObject(HANDLE hEvent,DWORD dwMilliseconds);
DWORD WaitForMultipleObjects(DWORD nCount,HANDLE* lpHandles,
									  BOOL bWaitAll,DWORD dwMilliseconds);

BOOL CloseHandle(HANDLE hObject);

void dbgName(HANDLE hObject,const char* sName);

// ================== Critical Section ==================================================

#ifdef SYM_LIBC_NLM
	#define CRITICAL_SECTION	pthread_mutex_t
	#define LPCRITICAL_SECTION CRITICAL_SECTION*

	void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	void EnterCriticalSection(		 LPCRITICAL_SECTION lpCriticalSection);
	void LeaveCriticalSection(		 LPCRITICAL_SECTION lpCriticalSection);
	void DeleteCriticalSection(	 LPCRITICAL_SECTION lpCriticalSection);
#endif

// ================== Debug =============================================================

void dbgV(const char* format,va_list args);
void dbg( const char* format,...);

void prtV(const char* format,va_list args);
void prt( const char* format,...);

#ifdef __cplusplus
} // extern "C"
#endif

// ================== Interlocked Variable Access =======================================

// When compiled in C, only the `long' version is used.

#ifdef __cplusplus
extern "C++" { // Needed to force C++ linkage when in `extern "C"' context.

// --------------------- unsigned long --------------------------------------------------

static inline unsigned long InterlockedExchangeAdd(unsigned long* lpAddend,
																	unsigned long lIncrement) {
	unsigned long lResult;
	#ifdef LINUX
		asm volatile ("lock; xaddl %0,(%1)"
						  : "=r" (lResult)
						  : "r" (lpAddend),"0" (lIncrement)
						  : "memory");
	#elif defined NLM
		lResult = *lpAddend; *lpAddend += lIncrement;
	#endif
	return lResult;
}
static inline unsigned long InterlockedIncrement(unsigned long* lpAddend) {
	InterlockedExchangeAdd(lpAddend,1); return *lpAddend;
}
static inline unsigned long InterlockedDecrement(unsigned long* lpAddend) {
	InterlockedExchangeAdd(lpAddend,-1); return *lpAddend;
}
static inline unsigned long InterlockedExchange(unsigned long* lpTarget,
																unsigned long lValue) {
	unsigned long lResult;
	#ifdef LINUX
		asm volatile ("lock; xchgl %0,(%1)"
						  : "=r" (lResult)
						  : "r" (lpTarget),"0" (lValue)
						  : "memory");
	#elif defined NLM
		lResult = *lpTarget; *lpTarget = lValue;
	#endif
		return lResult;
}
static inline unsigned long InterlockedCompareExchange(unsigned long* lpDestination,
																		 unsigned long lExchange,
																		 unsigned long lComperand) {
	unsigned long lResult;
	#ifdef LINUX
		asm volatile ("lock; cmpxchgl %2,(%1)"
						  : "=a" (lResult)
						  : "r" (lpDestination),"r" (lExchange),"0" (lComperand)
						  : "memory");
	#elif defined NLM
		lResult = *lpDestination;
		if (*lpDestination == lComperand) *lpDestination = lExchange;
	#endif
	return lResult;
}

#endif // __cplusplus

// --------------------- long -----------------------------------------------------------

static inline long InterlockedExchangeAdd(long* lpAddend,
														long lIncrement) {
	long lResult;
	#ifdef LINUX
		asm volatile ("lock; xaddl %0,(%1)"
						  : "=r" (lResult)
						  : "r" (lpAddend),"0" (lIncrement)
						  : "memory");
	#elif defined NLM
		lResult = *lpAddend; *lpAddend += lIncrement;
	#endif
	return lResult;
}
static inline long InterlockedIncrement(long* lpAddend) {
	InterlockedExchangeAdd(lpAddend,1); return *lpAddend;
}
static inline long InterlockedDecrement(long* lpAddend) {
	InterlockedExchangeAdd(lpAddend,-1); return *lpAddend;
}
static inline long InterlockedExchange(long* lpTarget,
													long lValue) {
	long lResult;
	#ifdef LINUX
		asm volatile ("lock; xchgl %0,(%1)"
						  : "=r" (lResult)
						  : "r" (lpTarget),"0" (lValue)
						  : "memory");
	#elif defined NLM
		lResult = *lpTarget; *lpTarget = lValue;
	#endif
		return lResult;
}
static inline long InterlockedCompareExchange(long* lpDestination,
															 long lExchange,
															 long lComperand) {
	long lResult;
	#ifdef LINUX
		asm volatile ("lock; cmpxchgl %2,(%1)"
						  : "=a" (lResult)
						  : "r" (lpDestination),"r" (lExchange),"0" (lComperand)
						  : "memory");
	#elif defined NLM
		lResult = *lpDestination;
		if (*lpDestination == lComperand) *lpDestination = lExchange;
	#endif
	return lResult;
}

#ifdef __cplusplus
} // extern "C++"
#endif

#endif // _winThreads_h_
