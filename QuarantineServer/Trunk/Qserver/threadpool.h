/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

/////////////////////////////////////////////////////////////////////////////////
// 
// Define worker thread function type.  This is the routine which
// will be invoked by a request to the thread pool.
// 
/////////////////////////////////////////////////////////////////////////////////
typedef void (*LPFN_WORKERTHREAD_FUNC) ( DWORD dwData );



/////////////////////////////////////////////////////////////////////////////////
// CThreadPoolData

class CThreadPoolData
{
public:
    CThreadPoolData();
    ~CThreadPoolData();
    
public:
    HANDLE m_dowork;
    HANDLE m_available;
    HANDLE m_die;
    HANDLE m_hthread;
    DWORD m_data;
    DWORD m_ID;
    LPFN_WORKERTHREAD_FUNC m_pUserWorkProc;
};


/////////////////////////////////////////////////////////////////////////////////
// CThreadPool

class CThreadPool
{
public:
   CThreadPool( UINT uThreadCount );
   ~CThreadPool();

   BOOL DoWork( LPFN_WORKERTHREAD_FUNC pWorkerProc, DWORD dwData, DWORD dwThreadPriority = THREAD_PRIORITY_NORMAL );
   
   static DWORD WINAPI ThreadFn( LPVOID lparam );

private:
   CThreadPoolData* m_data;
   UINT m_uThreadCount;
   CRITICAL_SECTION m_dataCS;
};

#if 0
class CScanThread
{
public:
	CScanThread ();
	~CScanThread ();

private:
	CObList myList;
   CThreadPoolData* m_data;
};

#endif

// 
// Thread timeout value.
//     
#define THREAD_TIMEOUT  1000


#endif



