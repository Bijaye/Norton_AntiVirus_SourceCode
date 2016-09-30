// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Debug Printouts for Unit and Acceptance Test 

// ================== ndkDebug ==========================================================

// Functions to use for test programs debug printouts.

// A test program can be run in two modes, the "module test" mode with a detailed trace
// of the execution to facilitate debugging, and the "acceptance test" that will run
// quietly and submit a printout only when something goes wrong. The "acceptance test" is
// intended to be run from the system build script in order to be able to catch
// suspicious behavior after code changes.
//
//	Note that we can only use `printf' function and the '<<' operators with `cout/cerr'
//	for printing. No `wprintf' can be used, as the I/O doesn't allow mixing.

extern "C" {
	#include <pthread.h>
	#include <stdio.h>
	#include <errno.h>

	#include "ndkDebug.h"
}
#include <iostream>

#include "Time.hpp"

using namespace std;

// --------------------- Printout Mode --------------------------------------------------

// Printouts are listed when `dbgVerbose' is `TRUE'. A flag on the command line to this
// program can be used to set this. By default, the program is run in "acceptance test"
// mode without any printouts unless something goes wrong, at which the test program
// executable returns a non-zeto return code via the `dbgResult' variable.

BOOL dbgVerbose = FALSE;
int  dbgResult	 = 0;

// --------------------- Debug Session --------------------------------------------------

// Initialization of a debug session for a test program. Handling of input parameters in
// order:
//	
//	   -f          Force all wf*() functions to emit error messages.
//	   -v          Set verbose mode on, by default it is off.
//		-c "name"   Indicate which test case to run, by default all are run.

static bool isRun	  = true;
static bool isFirst = true;
static bool isForce = true;

BOOL dbgInit(int argc,char* argv[],char* name) {
	isRun = false; isFirst = true; isForce = false; dbgVerbose = FALSE; dbgResult = 0;

	char* arg;
	int i = 1;
	if ((arg = argv[i]) && strcmp(arg,"-f") == 0) {
		i++;
		isForce    = true;
	}
	if ((arg = argv[i]) && strcmp(arg,"-v") == 0) {
		i++;
		dbgVerbose = TRUE;
	}
	if ((arg = argv[i]) && strcmp(arg,"-c") == 0) {
		i++;
		isRun = true;
		if ( (arg = argv[i]) ) {
			i++;
			if (strcmp(arg,name) != 0)
				isRun = false;
		}
	}
	if (! isRun) return false;

	if (! dbgVerbose) cout << "\tTesting ... " << flush;
	return TRUE;
}
int dbgFinal() {
	if (isRun) {
		if (dbgResult == 0) {
			if (! dbgVerbose) cout << "done";
			cout << endl;
		}
	}
	return dbgResult;
}

// --------------------- Printout -------------------------------------------------------

// Each call to a function to be tested is described using formatting flags to `printf'
// in a standardized way. Each printed line is prefixed with the current thread's
// identifier. This allows us to follow the execution of the test cases even though the
// printouts from the different threads are intertwined due to the parallell thread
// execution. The underlying Linux I/O system makes sure that each invication of `printf'
// is atomic when printing to the shell -- no need for us to mutex the printout
// functions.

static void dbgHead(bool nl = false) {
	if (nl) cout << endl; cout << pthread_self() << ": ";
}
static void dbgTail() {cout << endl;}

void dbgNL(char* format) {
	if (dbgVerbose) {
		dbgHead(true); printf(format); dbgTail();
	}
}
void dbgNil(char* format) {
	if (dbgVerbose) {
		dbgHead(); printf(format); dbgTail();
	}
}
void dbgChr(char* format,char value) {
	if (dbgVerbose) {
		dbgHead(); printf(format,value); dbgTail();
	}
}
void dbgWch(char* format,wchar_t value) {
	if (dbgVerbose) {
		dbgHead(); printf(format,value); dbgTail();
	}
}
void dbgInt(char* format,int value) {
	if (dbgVerbose) {
		dbgHead(); printf(format,value); dbgTail();
	}
}
void dbgLng(char* format,long value) {
	if (dbgVerbose) {
		dbgHead(); printf(format,value); dbgTail();
	}
}
void dbgStr(char* format,char* value) {
	if (dbgVerbose) {
		dbgHead(); printf(format,value); dbgTail();
	}
}
void dbgWcs(char* format,wchar_t* value) {
	if (dbgVerbose) {
		dbgHead(); printf(format,value); dbgTail();
	}
}
void dbgPtr(char* format,void* value) {
	if (dbgVerbose) {
		dbgHead(); printf(format,value); dbgTail();
	}
}

// --------------------- Verification ---------------------------------------------------

// The "well-formedness" function are spread throughtout the test cases to assert that
// expected values are received from function calls to the `ndkThreads' API. If not,
// and error printout is submitted and the return code for the test program executable is
// set to a non-zero value.

static void wfHead(char* sCase,char* sDescription) {
	if (! dbgVerbose && isFirst) {cout << endl; isFirst = false;}
	if (dbgVerbose) cout << pthread_self() << ": "; else cout << "\t";
	cout << "\t<Error> " << sCase << "() -- " << sDescription << ": Expected \"";
}
static void wfBody() {cout << "\" but got \"";}
static void wfTail() {cout << "\" !" << endl; if (! isForce) dbgResult = 1;}

void wfChr(char* sCase,char* sDescription,char cExpected,char cActual) {
	if (isForce || cExpected != cActual) {
		wfHead(sCase,sDescription);
		cout << cExpected; wfBody(); cout << cActual; wfTail();
	}
}
void wfWch(char* sCase,char* sDescription,wchar_t wcExpected,wchar_t wcActual) {
	if (isForce || wcExpected != wcActual) {
		wfHead(sCase,sDescription);
		printf("%lc",wcExpected); wfBody(); printf("%lc",wcActual); wfTail();
	}
}
void wfInt(char* sCase,char* sDescription,int iExpected,int iActual) {
	if (isForce || iExpected != iActual) {
		wfHead(sCase,sDescription);
		cout << iExpected; wfBody(); cout << iActual; wfTail();
	}
}
void wfLng(char* sCase,char* sDescription,long lExpected,long lActual) {
	if (isForce || lExpected != lActual) {
		wfHead(sCase,sDescription);
		cout << lExpected; wfBody(); cout << lActual; wfTail();
	}
}

// Strings are equal when they both are NULL, or both are non-NULL and the pointers are
// equal or all the characters in them are equal. Note that `printf' emitts a "(null)"
// when the character pointer is `NULL'.

void wfStr(char* sCase,char* sDescription,char* sExpected,char* sActual) {
	if (isForce || ! (! (sExpected || sActual) ||
							sExpected && sActual && (sExpected == sActual ||
															 strcmp(sExpected,sActual) == 0))) {
		wfHead(sCase,sDescription);
		printf("%s",sExpected); wfBody(); printf("%s",sActual); wfTail();
	}
}
void wfWcs(char* sCase,char* sDescription,wchar_t* wsExpected,wchar_t* wsActual) {
	if (isForce || ! (! (wsExpected || wsActual) ||
							wsExpected && wsActual && (wsExpected == wsActual ||
																wcscmp(wsExpected,wsActual) == 0))) {
		wfHead(sCase,sDescription);
		printf("%ls",wsExpected); wfBody(); printf("%ls",wsActual); wfTail();
	}
}

void wfPtr(char* sCase,char* sDescription,
			  void* uExpected, void* uActual) {
	if (isForce || uExpected != uActual) {
		wfHead(sCase,sDescription);
		cout << uExpected; wfBody(); cout << uActual; wfTail();
	}
}

// --------------------- Single Synchronization -----------------------------------------

// Two threads are synchronized by calling `wait' in one causing it to wait until the
// other thread calls `release'. Should the `release' happen to be called first, the
// `wait' will cause no delay, just be passed through. This is accomplished by the
// `ThreadWait' variable with three states:
//
//		Originally, its has the value "-1". A `wait' will change the value to "1" and then
//		fall into a mutex cond wait loop until its value has been changed back to "-1"
//
//		When the value is "1", a `release' will change the value back to "-1" and the
//		changed will be signalled to the thread lying in `wait'.
//
//		Should the `release' be called first when the value is "-1", the value is changed
//		to "0". When the `wait' comes, this means that there is no need to wait. Just
//		change the value back to "-1".
//
// The intension of this simple schema is to make sure that threads in the test cases are
// available and ready to be tested.

static pthread_mutex_t ThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  ThreadCond  = PTHREAD_COND_INITIALIZER;
static int				  ThreadWait  = -1;

static bool dbgCondWait(pthread_cond_t* pCond,pthread_mutex_t* pMutex,
								const char* sWhere) {
	struct timespec time;
	mkTime(2 * 60 * 1000, &time, true);
	int rc = pthread_cond_timedwait(pCond, pMutex, &time);
	bool ok = (rc == 0);
	if (! ok) {
		const char* sCause = rc == ETIMEDOUT ? "timeout" : "interrupt";
		if (dbgVerbose)
			printf("%u: %s() %s\n", static_cast<uint>(pthread_self()), sWhere, sCause);
		cout << "\t<Error> \"" << sWhere << "\" " << sCause << " !" << endl;
		if (! isForce)
			dbgResult = 1;
	}
	return ok;
}

void dbgWait() {
	pthread_mutex_lock(&ThreadMutex);
	if (dbgVerbose)
		printf("%u: dbgWait() %i\n", static_cast<uint>(pthread_self()), ThreadWait);
	if (ThreadWait < 0) {
		ThreadWait = 1;
		while (ThreadWait > 0 && dbgCondWait(&ThreadCond, &ThreadMutex, "dbgWait")) {
			;
		}
		if (dbgVerbose)
			printf("%u: dbgWait() -\n", static_cast<uint>(pthread_self()));
	} else ThreadWait = -1;
	pthread_mutex_unlock(&ThreadMutex);
}
void dbgRelease() {
	pthread_mutex_lock(&ThreadMutex);
	if (dbgVerbose)
		printf("%u: dbgRelease() %i\n", static_cast<uint>(pthread_self()), ThreadWait);
	if (ThreadWait > 0) {
		ThreadWait = -1;
		pthread_cond_signal(&ThreadCond);
	}
	else
		ThreadWait = 0;
	pthread_mutex_unlock(&ThreadMutex);
}

// --------------------- Keep Alive -----------------------------------------------------

// At the end of the test of each spawned thread, the thread is left to run until the end
// of the total test case by calling `keepAlive'. When the main thread is done with the
// current test case, it is laid to wait in `awaitAlive' for the tested threads in the
// case to come to rest in the `keepAlive' call. When all are there, as counted by the
// `AliveNumber' variable, the main thread is signalled and the `awaitAlive' function
// ends the test by printing the `ThreadData' object of each thread, the threads are
// signalled to alow them to terminate. Once all are gone, except the main thread, of
// course, the remaining 'ThreadData' objects are listed to make sure that no thread is
// stuck -- only the 'ThreadData' object for the main thread should be listed.

static pthread_mutex_t AliveMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  AliveCond	 = PTHREAD_COND_INITIALIZER;

static int AliveNumber = 0;

static void releaseAlive() {
	pthread_mutex_lock(&AliveMutex);
	if (dbgVerbose)
		printf("%u: releaseAlive()\n", static_cast<uint>(pthread_self()));
	AliveNumber = 0;
	pthread_cond_broadcast(&AliveCond);
	pthread_mutex_unlock(&AliveMutex);
}

void dbgKeepAlive() {
	pthread_mutex_lock(&AliveMutex);
	++AliveNumber;
	pthread_cond_broadcast(&AliveCond);
	if (dbgVerbose)
		printf("%u: dbgKeepAlive() %i\n", static_cast<uint>(pthread_self()), AliveNumber);
	while (AliveNumber > 0 && dbgCondWait(&AliveCond,&AliveMutex,"dbgKeepAlive")) {
		;
	}
	if (dbgVerbose)
		printf("%u: dbgKeepAlive() %i\n", static_cast<uint>(pthread_self()), AliveNumber);
	pthread_mutex_unlock(&AliveMutex);
}
void dbgAwaitAlive(char* sCase,int iNumber) {
	pthread_mutex_lock(&AliveMutex);
	if (dbgVerbose)
		printf("%u: dbgAwaitAlive(%i) %i\n", static_cast<uint>(pthread_self()), iNumber, AliveNumber);
	while (AliveNumber < iNumber && dbgCondWait(&AliveCond, &AliveMutex, "dbgAwaitAlive")) {
		;
	}
	pthread_mutex_unlock(&AliveMutex);

	wfInt(sCase, "Number of tested threads", iNumber + 1, nrThreadData());

	if (dbgVerbose)
		dbgThreadData();
	releaseAlive();
	dbgAwait(500);
	if (dbgVerbose)
		dbgThreadData();

	wfInt(sCase, "Number of remaining threads", 1, nrThreadData());
}

// --------------------- Await ----------------------------------------------------------

// In some cases, it is impossible to do a proper synchronization using mutexes when we
// are calling a function in the `ndkThreads' API and the test program cannot influence
// it internal mutexes. In these cases, we use a simple `Sleep' to wait for the right
// moment.

void dbgAwait(unsigned long msec) {
	if (dbgVerbose)
		printf("%u: dbgAwait(%u)\n", static_cast<uint>(pthread_self()),
			static_cast<uint>(msec));
	struct timespec time;
	mkTime(msec, &time);
	nanosleep(&time, NULL);
}

