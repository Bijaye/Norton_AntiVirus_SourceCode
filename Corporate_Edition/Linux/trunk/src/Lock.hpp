// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Failsafe Mutex for Thread synchronization

#ifndef _Lock_hpp_
#define _Lock_hpp_

// Encapsulation of mutexing functionality in a context safe fashion.  At the declaration
// of a `Lock' variable in a statement block, the mutex is locked immediately. It will be
// unlocked when the context of the statement block is left for whatever reason:
// `return', `goto', `throw', or running past the end of the statement block as the
// destructor is executed automatically.
//
// `Lock' also incorporates the concept of condition variables that allows us to `wait'
// for a variable to change values, and to `signal' or `broadcast' such a change to other
// threads lying in `wait'.

extern "C" {
	#include <pthread.h>
}

class Lock {
public:
	Lock(const pthread_mutex_t* pMutex);
	Lock(const pthread_mutex_t* pMutex,const pthread_cond_t* pCond);
	~Lock();

	void wait();
	void wait(		const pthread_cond_t* pCond);
	void wait(											 unsigned long msec);
	void wait(		const pthread_cond_t* pCond,unsigned long msec);
	void signal();
	void signal(	const pthread_cond_t* pCond);
	void broadcast();
	void broadcast(const pthread_cond_t* pCond);

private:
	pthread_mutex_t* _pMutex;
	pthread_cond_t*  _pCond;

	Lock();
	Lock(				 const Lock& that);
	Lock& operator=(const Lock& that);
};

#endif // _Lock_hpp_
