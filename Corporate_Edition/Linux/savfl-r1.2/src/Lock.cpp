// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Failsafe Mutex for Thread synchronization

// ================== Lock ==============================================================

#include "Time.hpp"

#include "Lock.hpp"

Lock::Lock(const pthread_mutex_t* pMutex) :
	_pMutex((pthread_mutex_t*) pMutex),_pCond(NULL) 
{
	pthread_mutex_lock(_pMutex);
}
Lock::Lock(const pthread_mutex_t* pMutex,const pthread_cond_t* pCond) :
	_pMutex((pthread_mutex_t*) pMutex),_pCond((pthread_cond_t*) pCond)
{
	pthread_mutex_lock(_pMutex);
}
Lock::~Lock() {
	pthread_mutex_unlock(_pMutex);
}

void Lock::wait() {wait(_pCond);}
void Lock::wait(const pthread_cond_t* pCond) {
	pthread_cond_wait((pthread_cond_t*) pCond,_pMutex);
}
void Lock::wait(unsigned long msec) {wait(_pCond,msec);}
void Lock::wait(const pthread_cond_t* pCond,unsigned long msec) {
	struct timespec time; mkTime(msec,&time);
	pthread_cond_timedwait((pthread_cond_t*) pCond,_pMutex,&time);
}
void Lock::signal() {signal(_pCond);}
void Lock::signal(const pthread_cond_t* pCond) {
	pthread_cond_signal((pthread_cond_t*) pCond);
}
void Lock::broadcast() {broadcast(_pCond);}
void Lock::broadcast(const pthread_cond_t* pCond) {
	pthread_cond_broadcast((pthread_cond_t*) pCond);
}

