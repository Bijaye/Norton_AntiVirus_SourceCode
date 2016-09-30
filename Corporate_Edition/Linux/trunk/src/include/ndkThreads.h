// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Thread Management functions

#ifndef _ndkThreads_h_
#define _ndkThreads_h_

#include <malloc.h>

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif

HANDLE BeginThread(void (*pFunc)(void*),void* pArg,size_t size);
void	 ExitThread();
HANDLE GetCurrentThreadId();

HANDLE BeginThreadGroup(void (*pFunc)(void*), void* pStack, unsigned uStackSize, void* pArg);
HANDLE GetThreadGroupID();
HANDLE SetThreadGroupID(HANDLE newThreadGroupID);

int GetThreadName(HANDLE hThread,char* sName);
int RenameThread( HANDLE hThread,const char* sName);

int nrThreadData();
void dbgThreadData();

long SuspendThread(HANDLE hThread);
long ResumeThread( HANDLE hThread);

HANDLE		  OpenSemaphore(	 long	  iValue);
BOOL			  CloseSemaphore(	 HANDLE hSemaphore);
LONG			  ExamineSemaphore(HANDLE hSemaphore);
BOOL			  SignalSemaphore( HANDLE hSemaphore);
unsigned long WaitForSemaphore(HANDLE hSemaphore,unsigned long msec);

void	 NTsSemaClose(		 HANDLE *pHandle);
HANDLE NTsSemaSignal(	 HANDLE *pSema);
HANDLE NTsSemaWait(		 HANDLE *pSema);
void	 NTsSemaTimedWait( HANDLE *pSema,unsigned long waitTime);
void	 NTsSemaWaitAwhile(HANDLE *pSema,int semaClose);

int NTSGetComputerName(char* pComputerName,wchar_t* pWchar);

HANDLE		  ipcOpenSemaphore(	 int	  iSemaphore, long lValue);
HANDLE		  ipcAttachSemaphore( int	  iSemaphore);
BOOL			  ipcCloseSemaphore(	 HANDLE hSemaphore);
long			  ipcExamineSemaphore(HANDLE hSemaphore);
BOOL			  ipcReleaseSemaphore(HANDLE hSemaphore);
unsigned long ipcWaitForSemaphore(HANDLE hSemaphore, unsigned long msec);

void Sleep(unsigned long msec);

void Breakpoint(int iBreakFlag);

#ifdef __cplusplus
}
#endif

#endif // _ndkThreads_h_
