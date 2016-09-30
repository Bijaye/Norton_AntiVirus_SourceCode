/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#include "stdafx.h"
#include "threadpool.h"



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CThreadPoolData::CThreadPoolData
//
// Description: Constructor
//
///////////////////////////////////////////////////////////////////////////////
// 12/13/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CThreadPoolData::CThreadPoolData() : 
    m_hthread(NULL), m_data(0)    
{
    // 
    // Set up event objects.
    // 
    m_dowork = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_available = CreateEvent( NULL, TRUE, TRUE, NULL );
    m_die = CreateEvent( NULL, TRUE, FALSE, NULL );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CThreadPoolData::~CThreadPoolData
//
// Description: Destructor
//
///////////////////////////////////////////////////////////////////////////////
// 12/13/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CThreadPoolData::~CThreadPoolData()
{
    // 
    // Set all events, since we are done here
    // 
    SetEvent(m_die);
    SetEvent(m_dowork);

    // 
    // Tell Windows to reschedule us/
    // 
    Sleep(0);
    if (WAIT_TIMEOUT == WaitForSingleObject(m_hthread, THREAD_TIMEOUT))
        {
        // 
        // Something bad has happened to this thread, so kill it.
        // 
        TerminateThread(m_hthread, 0);
        }
    
    // 
    // Tidy things up a bit.
    // 
    CloseHandle(m_hthread);
    CloseHandle(m_dowork);
    CloseHandle(m_available);
    CloseHandle(m_die);
}
   
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CThreadPool::CThreadPool
//
// Description: Constructor
//
// Argument: UINT noThreads - number of worker threads.
//
///////////////////////////////////////////////////////////////////////////////
// 12/13/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CThreadPool::CThreadPool(UINT uThreadCount) : 
    m_uThreadCount(uThreadCount)
{
    // 
    // Initialize critical section used for guarding data array
    // 
    InitializeCriticalSection( &m_dataCS );    

    // 
    // Create array of thread objects
    // 
    m_data = new CThreadPoolData[ m_uThreadCount ];
    
    DWORD dwThreadID;
    for( UINT i = 0; i < m_uThreadCount; i++ )
        {
        m_data[i].m_hthread = CreateThread( NULL, 0, ThreadFn,
            (LPVOID)&m_data[i], 0, &dwThreadID);
        m_data[i].m_ID = i;
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CThreadPool::~CThreadPool
//
// Description: Destructor
//
// Return type: 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/13/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CThreadPool::~CThreadPool()
{
    // 
    // Cleanup
    // 
    delete [] m_data;

    DeleteCriticalSection( &m_dataCS );
}

#include "util.h"

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CThreadPool::DoWork
//
// Description: Queues up a work request.
//
// Return type: BOOL TRUE if a thread was availiable to process task, 
//              FALSE if all threads are busy.
//
// Argument: DWORD dwData - DWORD value passed to worker thread.
//
///////////////////////////////////////////////////////////////////////////////
// 12/13/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CThreadPool::DoWork( LPFN_WORKERTHREAD_FUNC pWorkerProc, DWORD dwData, DWORD dwThreadPriority )
{
    
    // 
    // Assume all threads are busy.
    //     
    BOOL bRet = FALSE;

    // 
    // Claim critical section for data array.
    // 
    EnterCriticalSection( &m_dataCS );

    // 
    // Look for an availiable worker thread.
    // 
    for( UINT i = 0; i < m_uThreadCount; i++ )
        {
        if( WAIT_OBJECT_0 == WaitForSingleObject(m_data[i].m_available, 0) )
            {
            // 
            // Adjust thread priority.
            // 
            ::SetThreadPriority( m_data[i].m_hthread, dwThreadPriority );
            
            // 
            // Found one, set it in motion.
            // 
#ifdef  _DEBUG
			//void fWidePrintString(LPCSTR lpszFmt, ...);
			fWidePrintString("Handing work to thread id %d, index %d",m_data[i].m_hthread,i);
#endif
            m_data[i].m_data = dwData;
            m_data[i].m_pUserWorkProc = pWorkerProc;
            SetEvent( m_data[i].m_dowork );
            
            // 
            // Reshedule calling thread.
            // 
            Sleep(0);
        
            // 
            // Success!
            // 
            bRet = TRUE;
            break;
            }
        }

    LeaveCriticalSection( &m_dataCS );
    
    // 
    // Return Status
    // 
    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: ThreadFn
//
// Description: This is the actual thread proc for worker threads.
//
// Return type: DWORD 
//
// Argument: LPVOID lparam
//
///////////////////////////////////////////////////////////////////////////////
// 12/13/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CThreadPool::ThreadFn(LPVOID lparam)
{
    CThreadPoolData* pData = (CThreadPoolData*)lparam;
    while (TRUE)
        {
        // 
        // Wait here for the thread to be made availiable.
        // 
        WaitForSingleObject(pData->m_dowork, INFINITE);
        if (WAIT_OBJECT_0 == WaitForSingleObject(pData->m_die, 0))
            break;

        // 
        // This thread is now spoken for.
        // 
        ResetEvent(pData->m_available);

        // 
        // Get the real work done here.
        // 
        if( pData->m_pUserWorkProc )
            {
            pData->m_pUserWorkProc( pData->m_data );
            }
        else
            {
            // 
            // Bad things have happened here.
            // 
            _ASSERT( FALSE );
            }


        if (WAIT_OBJECT_0 == WaitForSingleObject(pData->m_die, 0))
            break;
        
        // 
        // Work is done, make this thread availiable to the pool 
        // once again.
        // 
        ResetEvent(pData->m_dowork);
        SetEvent(pData->m_available);
        Sleep(0);
        }
    
    return ERROR_SUCCESS;
}

#if 0
///////////////////////////////////////////////////////////////////////////////
//
// Function name: CScanThread::CScanThread
//
// Description: Constructor
//
// Argument: 
//
///////////////////////////////////////////////////////////////////////////////
// 01-1817-99 TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CScanThread::CScanThread() 
{
    
    // 
    // Create thread DATA object
    // 
    m_data = new CThreadPoolData;

    DWORD dwThreadID;
    m_data->m_hthread = CreateThread( NULL, 0, ThreadFn,
        (LPVOID)m_data, 0, &dwThreadID);
    m_data->m_ID = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CScanThread::~CScanThread
//
// Description: Destructor
//
// Return type: 
//
//
///////////////////////////////////////////////////////////////////////////////
// 01-17-00 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CScanThread::~CScanThread()
{
    // 
    // Cleanup
    // 
    delete [] m_data;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: ThreadFn
//
// Description: This is the actual thread proc for Scanning thread.
//
// Return type: DWORD 
//
// Argument: LPVOID lparam
//
///////////////////////////////////////////////////////////////////////////////
// 01-17-00 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CScanThread::ThreadFn(LPVOID lparam)
{
    CThreadPoolData* pData = (CThreadPoolData*)lparam;
    while (TRUE)
        {
        // 
        // Wait here for the thread to be made availiable.
        // 
        WaitForSingleObject(pData->m_dowork, INFINITE);
        if (WAIT_OBJECT_0 == WaitForSingleObject(pData->m_die, 0))
            break;

        // 
        // This thread is now spoken for.
        // 
        ResetEvent(pData->m_available);

        // 
        // Get the real work done here.
        // 
        if( pData->m_pUserWorkProc )
            {
            pData->m_pUserWorkProc( pData->m_data );
            }
        else
            {
            // 
            // Bad things have happened here.
            // 
            _ASSERT( FALSE );
            }


        if (WAIT_OBJECT_0 == WaitForSingleObject(pData->m_die, 0))
            break;
        
        // 
        // Work is done, make this thread availiable to the pool 
        // once again.
        // 
        ResetEvent(pData->m_dowork);
        SetEvent(pData->m_available);
        Sleep(0);
        }
    
    return ERROR_SUCCESS;
}

#endif