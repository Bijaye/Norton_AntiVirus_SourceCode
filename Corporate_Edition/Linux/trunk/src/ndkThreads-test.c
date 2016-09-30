// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Test Linux implementation of NDK Thread Management functions 

// ================== ndkThreads-test ===================================================

// This program tests the `ndkThreads' functionality. Notice that as `ndkThreads' is
// used from RTVScan C programs, the `ndkThreads' API is C oriented. This is why this
// test program is written in C.
//
// Each function group has a description of what the test does and what to expect from
// the outcome.

   #define BYTE unsigned char
   #define WORD unsigned short
   #define DWORD unsigned long
#include "nts.h"
   #undef BYTE
   #undef WORD
   #undef DWORD

#include "ndkThreads.h"

#include "ndkDebug.h"

//#ifdef HANDLE
//#error 'HANDLE is defined'
//#endif
//#undef HANDLE

// ================== Debug Utilities ===================================================

// --------------------- Argument Counter -----------------------------------------------

// Variable used as arbitrary argument to thread functions. It is increased at each call
// to make sure that each call is different.

static int I = 0;

// ================== Thread ============================================================

// --------------------- Thread Creation ------------------------------------------------

// Create two `start' threads, one with the default stack size and one with a specified
// stack size. Notice how the threads exit by running to the end of the `start' function.
//
// Create a `startExit' thread that immediately calls `ExitThread' to terminate itself
// before it runs over the end of the thread function.

static void start(void* arg) {
	dbgPtr("start(%p)",arg);
	dbgPtr("GetCurrentThreadId() -> %p", GetCurrentThreadId());
	dbgKeepAlive();
}

static void startExit(void* arg) {
	dbgPtr("startExit(%p)",arg);
	dbgKeepAlive();
	dbgNil("ExitThread()"); ExitThread();
}

static void threadCreation() {
	dbgPtr("BeginThread(start,		++I,0)	 -> %p", BeginThread(start, (void*)(++I), 0));
	dbgPtr("BeginThread(start,		++I,3096) -> %p", BeginThread(start, (void*)(++I), 3096));
	dbgPtr("BeginThread(startExit,++I,0)	 -> %p", BeginThread(startExit, (void*)(++I), 0));

	dbgAwaitAlive("threadCreation",3);
}

// --------------------- Thread Group ---------------------------------------------------

// A new thread group is started by spwaning off a `startGroup' thread. This will
// determine the identfier of its group and spawn off another thread to run
// `startSameGroup' with the thread group identifier as its argument. It will set its
// thread group to this value. Finally, the thread group of the main thread is set.

static void changeGroup(HANDLE iGroup) {
	HANDLE i;
	dbgPtr("GetThreadGroupID() -> %p", i = GetThreadGroupID());
	wfPtr("changeGroup","GetThreadGroupID() before", 0, i);
	dbgPtr("SetThreadGroupID(iGroup) -> %p", SetThreadGroupID(iGroup));
	dbgPtr("GetThreadGroupID() -> %p", i = GetThreadGroupID());
	wfPtr("changeGroup","GetThreadGroupID() after", iGroup, i);
}

static void startOtherGroup(void* arg) {
	dbgPtr("startOtherGroup(%p)", arg);
	changeGroup(arg);
	dbgKeepAlive();
}
static void startGroup(void* arg) {
	HANDLE iGroup;
	dbgPtr("startGroup(%p)",arg);
	dbgPtr("iGroup = GetThreadGroupID() -> %p",iGroup = GetThreadGroupID());
	wfPtr("startGroup", "GetThreadGroupID()", (HANDLE)1, iGroup);
	dbgPtr("BeginThread(startOtherGroup,iGroup + 10,0) -> %p",
			 BeginThread(startOtherGroup,iGroup + 10,0)); dbgKeepAlive();
}

static void threadGroup() {
	HANDLE iGroup;
	dbgPtr("iGroup = BeginThreadGroup(startGroup,NULL,0,++I) -> %p",
			 iGroup = BeginThreadGroup(startGroup, NULL, 0, (void*)(++I)));
	wfPtr("threadGroup", "BeginThreadGroup()", (HANDLE)1, iGroup);
	changeGroup(iGroup);

	dbgAwaitAlive("threadGroup",2);
}

// --------------------- Thread Name ----------------------------------------------------

// The name of the `startName' thread is changed by itself. The name of the
// `startOtherName' thread is changed from the main thread. In both cases, the name is
// printed before and after the change. Notice that we have to synchronize the
// `startOtherName' and the main thread to make sure that the thread is still existing
// while the name change is done. Finally, the name of the main thread is changed.

static void changeName(HANDLE hThread,char* name) {
	char s[18];
	dbgInt("GetThreadName(hThread,s) -> %i",GetThreadName(hThread,s));
	dbgStr("s == \"%s\"",s); wfStr("changeName","GetThreadName() before","",s);
	dbgInt("RenameThread(hThread,name) -> %i",RenameThread(hThread,name));
	dbgInt("GetThreadName(hThread,s) -> %i",GetThreadName(hThread,s));
	dbgStr("s == \"%s\"",s); wfStr("changeName","GetThreadName() after",name,s);
}

static void startName(void* arg) {
	HANDLE hThread;
	dbgStr("startName(%s)",arg);
	dbgPtr("hThread = GetCurrentThreadId() -> %p",hThread = GetCurrentThreadId());
	changeName(hThread,arg); dbgKeepAlive();
}
static void startOtherName(void* arg) {
	dbgPtr("startOtherName(%p)", arg);
	dbgRelease();
	dbgKeepAlive();
}

static void threadName() {
	HANDLE hThread;

	dbgPtr("BeginThread(startName,\"name\",0) -> %p",BeginThread(startName,"name",0));

	dbgPtr("hThread = BeginThread(startOtherName,++I,0) -> %p",
			 hThread = BeginThread(startOtherName, (void*)(++I), 0));
	dbgWait();
	changeName(hThread,"otherName");

	dbgPtr("hThread = GetCurrentThreadId() -> %p",hThread = GetCurrentThreadId());
	changeName(hThread,"mainName");

	dbgAwaitAlive("threadName",2);
}

// ================== Suspend/Resume ====================================================

// Create a `startSelfSuspend' thread. It will suspend itself. Notice how nothing happens
// until the main thread calls `ResumeThread'.
//
// Create a `startSuspend' thread and let it wait for the semaphore. At this, the main
// thread will suspend it. Notice that even though the main thread calls
// `release', nothing happens in the thread. It is only when `ResumeThread' is
// called that the thread continues its execution by leaving the `wait'.

static void startSelfSuspend(void* arg) {
	HANDLE hThread; long l;
	dbgPtr("startSelfSuspend(%i)", arg);
	dbgPtr("hThread = GetCurrentThreadId() -> %u",hThread = GetCurrentThreadId());
	dbgNil("SuspendThread(hThread) -> ...");
	dbgLng("SuspendThread(hThread) -> %d",l = SuspendThread(hThread));
	wfLng("startSelfSuspend","SuspendThread()",0,l); dbgRelease(); dbgKeepAlive();
}
/*
static void startWait(void* arg) {
	dbgPtr("startWait(%i)", arg);
	dbgAwait(1500);
	dbgKeepAlive();
}

static void startSuspend(void* arg) {
	dbgPtr("startSuspend(%i)", arg);
	dbgWait();
	dbgRelease();
	dbgKeepAlive();
}
*/
static void suspendResume() {
	HANDLE hThread;
	long l;
// 	long long i,j = 0;
	dbgPtr("hThread = BeginThread(startSelfSuspend,++I,0) -> %u",
			 hThread = BeginThread(startSelfSuspend, (void*)(++I), 0));
	dbgAwait(1000); dbgLng("ResumeThread(hThread) -> %d",l = ResumeThread(hThread));
	wfLng("suspendResume","ResumeThread(startSelfSuspend)",1,l); dbgWait();

// Suspend of other thread doesn't work on kernel-2.4 => this test case is skipped

// dbgPtr("BeginThread(startWait,++I,0) -> %u",BeginThread(startWait,++I,0));
// dbgPtr("hThread = BeginThread(startSuspend,++I,0) -> %u",
//        hThread = BeginThread(startSuspend,++I,0));
// dbgAwait(1000); dbgLng("SuspendThread(hThread) -> %d",l = SuspendThread(hThread));
// wfLng("suspendResume","SuspendThread(startSuspend)",0,l);  dbgRelease();
// dbgAwait(500);
// dbgLng("ResumeThread(hThread) -> %d",l = ResumeThread(hThread));
// wfLng("suspendResume","ResumeThread(startSuspend)",1,l); dbgAwait(500); dbgWait();

	dbgAwaitAlive("suspendResume",1);
}

// ================== Semaphore =========================================================

// Create a semaphore with value 2. Start 3 threads that all will wait on the
// semaphore. The 2 first threads will pass through the `WaitForSemaphore' right away as
// its value decreases to 0. The third thread will wait until the main thread increases
// the semaphore with a `SignalSemaphore'.
//
// The second test will create a thread that will wait on the semaphore for 5 seconds. It
// will continue its execution after these have elapsed even though the main thread
// hasn't called `SignalSemaphore'.
//
// The `NTs' functions are tested in a very similar fashion.

static void startSemaphore(void* arg) {
	long lValue;
	dbgPtr("startSemaphore(%u)",arg);
	dbgLng("lValue = ExamineSemaphore(arg) -> %d",lValue = ExamineSemaphore(arg));
	dbgNil("WaitForSemaphore(arg,INFINITE) -> ...");
	dbgPtr("WaitForSemaphore(arg,INFINITE) -> %p", (void*)WaitForSemaphore(arg,INFINITE) );
	if (lValue == 0) dbgRelease(); dbgKeepAlive();
}
static void startSemaphoreTimeoutRelease(void* arg) {
	long l; unsigned long u;
	dbgPtr("startSemaphoreTimeout(%u)",arg);
	dbgLng("ExamineSemaphore(arg) -> %d",l = ExamineSemaphore(arg));
	wfLng("startSemaphoreTimeoutRelease","ExamineSemaphore()",0,l);
	dbgNil("WaitForSemaphore(arg,5000) -> ...");
	dbgPtr("WaitForSemaphore(arg,5000) -> %p", (void*)(u = WaitForSemaphore(arg,5000)));
	wfPtr("startSemaphoreTimeoutRelease", "WaitForSemaphore()", WAIT_OBJECT_0, (void*)u);
	dbgRelease(); dbgKeepAlive();
}
static void startSemaphoreTimeout(void* arg) {
	long l; unsigned long u;
	dbgPtr("startSemaphoreTimeout(%u)",arg);
	dbgLng("ExamineSemaphore(arg) -> %d",l = ExamineSemaphore(arg));
	wfLng("startSemaphoreTimeout","ExamineSemaphore()",0,l);
	dbgNil("WaitForSemaphore(arg,5000) -> ...");
	dbgPtr("WaitForSemaphore(arg,5000) -> %u", (void*)(u = WaitForSemaphore(arg,5000)));
	wfPtr("startSemaphoreTimeout", "WaitForSemaphore()", (void*)WAIT_TIMEOUT, (void*)u);
	dbgRelease(); dbgKeepAlive();
}

static void semaphore() {
	HANDLE hSema; long l; int i;

	dbgPtr("hSema = OpenSemaphore(2) -> %u",hSema = OpenSemaphore(2));
	dbgLng("ExamineSemaphore(hSema) -> %d",l = ExamineSemaphore(hSema));
	wfLng("semaphore","ExamineSemaphore()",2,l);

	for (i = 0; i < 3; i++)
		dbgPtr("BeginThread(startSemaphore,hSema,0) -> %u",
				 BeginThread(startSemaphore,hSema,0));

	dbgAwait(1000);
	dbgInt("SignalSemaphore(hSema) -> %i",i = SignalSemaphore(hSema)); dbgWait();
	wfInt("semaphore","SignalSemaphore(startSemaphore)",1,i);

	dbgPtr("BeginThread(startSemaphoreTimeoutRelease,hSema,0) -> %u",
			 BeginThread(startSemaphoreTimeoutRelease,hSema,0));
	dbgAwait(100);
	dbgInt("SignalSemaphore(hSema) -> %i",i = SignalSemaphore(hSema)); dbgWait();
	wfInt("semaphore","SignalSemaphore(startSemaphoreTimeoutRelease)",1,i);

	dbgPtr("BeginThread(startSemaphoreTimeout,hSema,0) -> %u",
			 BeginThread(startSemaphoreTimeout,hSema,0)); dbgWait();

	dbgInt("CloseSemaphore(hSema) -> %i",i = CloseSemaphore(hSema));
	wfInt("semaphore","CloseSemaphore()",1,i);

	dbgAwaitAlive("semaphore",5);
}

// --------------------- NTS ------------------------------------------------------------

static void startNTsSema(void* arg) {
	dbgPtr("startNTsSema(%u)",arg);
	dbgNil("NTsSemaWait(&arg) -> ...");
	dbgPtr("NTsSemaWait(&arg) -> %u",NTsSemaWait(&arg)); dbgRelease(); dbgKeepAlive();
}
static void startNTsSemaTimed(void* arg) {
	dbgPtr("startNTsSemaTimed(%u)",arg);
	dbgNil("NTsSemaTimedWait(&arg,5000) ..."); NTsSemaTimedWait(&arg,5000);
	dbgNil("NTsSemaTimedWait(&arg,5000) ... done"); dbgRelease(); dbgKeepAlive();
}
static void startNTsSemaAwhile(void* arg) {
	dbgPtr("startNTsSemaAwhile(%u",arg);
	dbgNil("NTsSemaWaitAwhile(&arg,1) ..."); NTsSemaWaitAwhile(&arg,1);
	dbgNil("NTsSemaWaitAwhile(&arg,1) ... done"); dbgRelease(); dbgKeepAlive();
}

static void nts() {
	HANDLE hSema;
	dbgPtr("hSema = OpenSemaphore(0) -> %u",hSema = OpenSemaphore(0));
	dbgPtr("BeginThread(startNTsSema,hSema,0) -> %u",
			 BeginThread(startNTsSema,hSema,0));
	dbgAwait(1000);
	dbgPtr("NTsSemaSignal(&hSema) -> %u",NTsSemaSignal(&hSema)); dbgWait();

	dbgPtr("hSema = OpenSemaphore(0) -> %u",hSema = OpenSemaphore(0));
	dbgPtr("BeginThread(startNTsSemaTimed,hSema,0) -> %u",
			 BeginThread(startNTsSemaTimed,hSema,0));
	dbgAwait(100);
	dbgPtr("NTsSemaSignal(&hSema) -> %u",NTsSemaSignal(&hSema)); dbgWait();

	dbgPtr("hSema = OpenSemaphore(0) -> %u",hSema = OpenSemaphore(0));
	dbgPtr("BeginThread(startNTsSemaTimed,hSema,0) -> %u",
			 BeginThread(startNTsSemaTimed,hSema,0)); dbgWait();

	dbgPtr("BeginThread(startNTsSemaAwhile,hSema,0) -> %u",
			 BeginThread(startNTsSemaAwhile,hSema,0));
	dbgAwait(100);
	dbgPtr("NTsSemaSignal(&hSema) -> %u",NTsSemaSignal(&hSema)); dbgWait();

	dbgPtr("BeginThread(startNTsSemaAwhile,hSema,0) -> %u",
			 BeginThread(startNTsSemaAwhile,hSema,0)); dbgWait();

	dbgAwaitAlive("nts",5);
}

// ================== Sleep =============================================================

// Let the main thread sleep for 5 seconds.

static void sleep() {
	dbgNil("Sleep(5000)"); Sleep(5000);
}

// ================== Breakpoint ========================================================

// When this is run in the regular non-debug runtime system, we will only see a printout
// with the break flag value displayed. Start `gdb', set a breakpoint in `Breakpoint' and
// run. `gdb' will now stop in the `Breakpoint' function to allow you to inspect the
// stack and other things. Note that this test is intended to be performed interactively,
// therefore it will not be run in `acceptance test' mode.

static void breakpoint() {
	dbgNil("Breakpoint(4711)"); if (dbgVerbose) Breakpoint(4711);
}

// ================== Main ==============================================================

// For ease of readablity of the printout, we have added a synchronization between each
// test case using the `awaitAlive' function as described above. Otherwise, the printouts
// from the different test cases run the risk of being intertwined. A fail-safe way of
// doing this would require us to use non-detached threads and join them in the main
// thread after their exit. However, our threads are detached for optimal use by RTVScan.

int main(int argc,char* argv[]) {
	if (dbgInit(argc,argv,"Threads")) {
		dbgNL("threadCreation()"); threadCreation();
		dbgNL("threadGroup()");		threadGroup();		
		dbgNL("threadName()");		threadName();		
		dbgNL("suspendResume()");	suspendResume();	
		dbgNL("semaphore()");		semaphore();		
		dbgNL("nts()");				nts();				
		dbgNL("sleep()");				sleep();
		dbgNL("breakpoint()");		breakpoint();
	}
	return dbgFinal();
}
