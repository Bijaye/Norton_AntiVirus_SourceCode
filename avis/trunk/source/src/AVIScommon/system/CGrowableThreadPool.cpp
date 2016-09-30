//
// FILE: CGrowableThreadPool.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "CMclGlobal.h"
#include "CGrowableThreadPool.h"

CGrowableThreadPool::CGrowableThreadPool( long lMinThreads, long lMaxThreads, DWORD dwThreadLifetime )
    : m_csPool(),
      m_ExitEvent(TRUE),    // Manual reset event, initially nonsignaled.
      m_dwThreadLifetime(dwThreadLifetime),
      m_ThreadIdMap(lMaxThreads),
      m_DispatchQ(),
      m_JobsPending(0, lMaxThreads),
      m_lMinThreads(lMinThreads),
      m_lMaxThreads(lMaxThreads),
      m_lCurThreads(0),
      m_ThreadsFree(0, lMaxThreads)
{
//    printf(
//        "[0x%08lx] CGrowableThreadPool::CThreadPool\n",
//        GetCurrentThreadId()
//    );

    CMclAutoLock    Lock(m_csPool);
    BOOL            fOkay = TRUE;

    while( fOkay && (m_lCurThreads < m_lMinThreads) )
    {
        CMclThread *pThread = new CMclThread(this, CREATE_SUSPENDED);

        if( pThread )
        {
            fOkay = m_ThreadsInPool.PutOnTailOfList(pThread);
            
            if( fOkay  )
            {
                m_ThreadIdMap.AddThread(pThread->GetThreadId(), pThread);
                pThread->Resume();
                m_lCurThreads++;
            }
        }
        else
        {
            fOkay = FALSE;
        }
    }

    if( fOkay )
    {
        m_ThreadsFree.Release(m_lCurThreads);
    }
    else
    {
        CMclThrowError(ERROR_OUTOFMEMORY);
    }
}

CGrowableThreadPool::~CGrowableThreadPool()
{
//    printf(
//        "[0x%08lx] CGrowableThreadPool::~CThreadPool entered\n",
//        GetCurrentThreadId()
//    );

    {
        BOOL                            fAllDone = FALSE;
        CMclLinkedList<CMclThread *>    LiveThreads;
        CMclThread                     *pThread;

        // Remove all the thread pointer objects from our list
        // of threads in the pool and keep a temporary copy
        // of "Live" threads.  This might actually be done in
        // competition with threads in the pool that have decided
        // to remove themselves from the pool.  Threads that are
        // able to remove themselves from the m_ThreadsInPool list
        // delete themselves.
        //
        {
            CMclAutoLock Lock(m_csPool);

            while( m_ThreadsInPool.GetFromHeadOfList(pThread, 0) )
            {
                LiveThreads.PutOnTailOfList(pThread);
            }

            // Signal to any remaining live threads that it's
            // time for them to exit.
            //
            m_ExitEvent.Set();
        }

        // Wait for all of the live threads that we're going to
        // terminate to exit.  This looks a little complicated because
        // we support creating more threads in the pool than can actually 
        // be waited on using one WaitForMultipleObjects operation.
        // To do this, we look through out list, assigning the live
        // thread pointers to an array of thread auto pointers.  When
        // we fill up the array or run out of threads, we (1) wait for
        // them to exit and (2) let the array of thread auto pointers
        // cleanup the thread objects.  We then repeat this until
        // no more threads are left.
        do
        {
            CMclThreadAutoPtr       m_Threads[MAXIMUM_WAIT_OBJECTS];
            CMclWaitableCollection  WaitSet;
            long                    lLiveThreads = 0;

            while( !fAllDone && (lLiveThreads < MAXIMUM_WAIT_OBJECTS) )
            {
                if( LiveThreads.GetFromHeadOfList(pThread, 0) )
                {
                    m_Threads[lLiveThreads++] = pThread;
                    WaitSet.AddObject(pThread);
                }
                else
                {
                    fAllDone = TRUE;
                }
            }

            if( lLiveThreads )
            {
                WaitSet.Wait(TRUE, INFINITE);
            }
        }
        while( !fAllDone );        
    }

//    printf(
//        "[0x%08lx] CGrowableThreadPool::~CThreadPool returning\n",
//        GetCurrentThreadId()
//    );
}

BOOL CGrowableThreadPool::DispatchThread( CMclThreadHandler *pHandler )
{
    BOOL        fDispatchOk;
    CMclThread *pThread = 0;

//    printf(
//        "[0x%08lx] CGrowableThreadPool::Dispatch called\n",
//        GetCurrentThreadId()
//    );

    // Try to claim one of the free threads.
    //
    fDispatchOk = CMclWaitSucceeded(m_ThreadsFree.Wait(0), 1);

    if( !fDispatchOk )
    {
        // Couldn't grab an existing thread - check to see
        // if we're allowed to create one on the fly in order
        // to proceed.
        //
        CMclAutoLock Lock(m_csPool);

        if( m_lCurThreads < m_lMaxThreads )
        {
            // We're within limits, so go ahead and create another
            // thread on the fly to handle this dispatch request.
            //
//            printf(
//                "[0x%08lx] CGrowableThreadPool::Dispatch thread creating new thread\n",
//                GetCurrentThreadId()
//            );

            // The new thread is created in the suspended state so that
            // the thread pool's data structures that keep track of everything
            // can be created before the thread actually runs.  Once a thread
            // runs, it can return back to the pool at time.
            //
            pThread = new CMclThread(this, CREATE_SUSPENDED);

            if( pThread )
            {
                fDispatchOk = m_ThreadsInPool.PutOnTailOfList(pThread);
            
                if( fDispatchOk  )
                {
                    m_ThreadIdMap.AddThread(pThread->GetThreadId(), pThread);
                    pThread->Resume();
                    m_lCurThreads++;
                }
            }
        }
        else
        {
            printf(
                "[0x%08lx] CGrowableThreadPool::Dispatch hit max threads\n",
                GetCurrentThreadId()
            );
        }
    }

    if( fDispatchOk )
    {
        // If everything went okay, we know that either there
        // is at least one thread in the pool already that can
        // handle this request, or we know that we've just created
        // a new thread to handle the request.  So we'll enqueue
        // the job now and signal the job pending semaphore to
        // wake up one of the idle threads.
        //
        CMclAutoLock    Lock(m_csPool);
        CDispatchRecord Job(pHandler);

        fDispatchOk = m_DispatchQ.Put(Job);
        
        if( fDispatchOk )
        {
            // m_JobsPending.Release(1);
            if( m_JobsPending.Release(1) )
            {
                printf("Signaled jobs pending.\n");
            }
            else
            {
                printf("Failed to signaled jobs pending.\n");
            }
        }
        else
        {
            // Failed - cleanup.
            //
            if( m_ThreadsInPool.Remove(pThread) )
            {
                delete pThread;
            }
        }
    }

    return(fDispatchOk);
}

unsigned CGrowableThreadPool::ThreadHandlerProc( void )
{
    // This routine is the thread handler procedure for every thread
    // in the pool.  The basic operation of every thread is as
    // follows:
    //      1. Wait for one of the following to occur:
    //              - The exit event is signaled.
    //              - The job pending semaphore is signaled.
    //              - We timeout waiting for the above.
    //      2. If we've been told to exit, do so and let the
    //         class destructor clean up after us.
    //      3. If the job pending semaphore was signaled, 
    //         handle the next job in the dispatch queue.
    //      4. If we timed out waiting for things to do,
    //         see if the thread count in the pool is high enough
    //         to allow us to self terminate this thread.
    //
//    printf(
//        "[0x%08lx] CGrowableThreadPool::ThreadHandlerProc started\n",
//        GetCurrentThreadId()
//    );

    CMclWaitableCollection  WaitSet;
    CDispatchRecord         Job;
    long                    lJobsHandled = 0;
    BOOL                    fThreadAlive = TRUE;

    enum { EXIT_EVENT_INDEX = 0, JOBS_PENDING_INDEX, NUM_OBJECTS };

    WaitSet.AddObject(m_ExitEvent);     // Index 0.
    WaitSet.AddObject(m_JobsPending);   // Index 1.

    while( fThreadAlive )
    {
        // Wait for something to do, a timeout to occur, or being told
        // to exit.
        //
        DWORD dwWaitResult = WaitSet.Wait(FALSE, m_dwThreadLifetime);

        if( CMclWaitSucceeded(dwWaitResult, NUM_OBJECTS) )
        {
            if( CMclWaitSucceededIndex(dwWaitResult) == JOBS_PENDING_INDEX )
            {
 //               printf(
 //                   "[0x%08lx] CGrowableThreadPool::ThreadHandlerProc have a job to do\n",
 //                   GetCurrentThreadId()
 //               );

                // m_JobsPending was signaled.
                //
                if( m_DispatchQ.Get(Job) )
                {
                    // Do it!
                    //
                    Job.Execute();
                    lJobsHandled++;
                }

                m_ThreadsFree.Release(1);
            }
            else
            {
//                printf(
//                    "[0x%08lx] CGrowableThreadPool::ThreadHandlerProc exit signaled\n",
//                    GetCurrentThreadId()
 //               );

                // m_ExitEvent was signaled.  We won't clean up anything
                // because the destructor has already recorded that we're
                // alive and will be waiting for our thread handle to
                // become signaled.
                //
                fThreadAlive = FALSE;
            }
        }
        else if( CMclWaitTimeout(dwWaitResult) )
        {
            // This thread has been asleep a long time and has timed
            // out waiting for something to do.  See if it should die.
            //
//            printf(
//                "[0x%08lx] CGrowableThreadPool::ThreadHandlerProc timed out\n",
//                GetCurrentThreadId()
//            );

            CMclAutoLock Lock(m_csPool);

            if( m_lCurThreads > m_lMinThreads )
            {
                // The current thread count in the pool is above the minimum
                // we need to maintain, so cleanup after ourselves and exit
                // this thread.
                //
                m_lCurThreads--;
                
                fThreadAlive = FALSE;

                CMclThread *pThread = m_ThreadIdMap.GetThreadFromId(GetCurrentThreadId());
                
                if( pThread )
                {
                    m_ThreadIdMap.FreeSlot(GetCurrentThreadId());

                    if( m_ThreadsInPool.Remove(pThread) )
                    {
//                        printf(
//                            "[0x%08lx] CGrowableThreadPool::ThreadHandlerProc exiting\n",
//                            GetCurrentThreadId()
//                        );

                        delete pThread;
                    }
                }
            }
            else
            {
                // The minimum number of threads has already been reached, so 
                // continue to stay alive and wait for jobs to be queued.
                //
//                printf(
//                    "[0x%08lx] CGrowableThreadPool::ThreadHandlerProc min threads hit\n",
//                    GetCurrentThreadId()
//                );
            }
        }
        else
        {
            // The wait operation failed in general.  Cleanup as if we
            // were terminating ourselves.
            //
//            printf(
//                "[0x%08lx] CGrowableThreadPool::ThreadHandlerProc other failure\n",
//                GetCurrentThreadId()
//            );

            // Other failure.
            //
            CMclAutoLock Lock(m_csPool);
            
            m_lCurThreads--;
            
            fThreadAlive = FALSE;

            CMclThread *pThread;
            
            pThread = m_ThreadIdMap.GetThreadFromId(GetCurrentThreadId());
            
            if( pThread )
            {
                m_ThreadIdMap.FreeSlot(GetCurrentThreadId());

                if( m_ThreadsInPool.Remove(pThread) )
                {
                    delete pThread;
                }
            }
        }
    }
        
//    printf(
//        "[0x%08lx] CGrowableThreadPool::ThreadHandlerProc done, %d jobs handled\n",
//        GetCurrentThreadId(),
//        lJobsHandled
//    );

    return(0);
}

// CDispatchRecord class implementation.
//
CGrowableThreadPool::CDispatchRecord::CDispatchRecord()
    : m_pUserThreadHandler(0)
{
}

CGrowableThreadPool::CDispatchRecord::CDispatchRecord
(
    CMclThreadHandler *pThreadHandler
)
    : m_pUserThreadHandler(pThreadHandler)
{
}

unsigned CGrowableThreadPool::CDispatchRecord::Execute( void )
{
    unsigned wThreadExitCode = 0;

    if( m_pUserThreadHandler )
    {
        wThreadExitCode = m_pUserThreadHandler->ThreadHandlerProc();
    }

    return(wThreadExitCode);
}

int CGrowableThreadPool::CDispatchRecord::operator ==
(
    const CDispatchRecord& rhs
)
{
    return int(m_pUserThreadHandler == rhs.m_pUserThreadHandler);
}

// CThreadIdToPtrMap class implementation.
//
CGrowableThreadPool::CThreadIdToPtrMap::CThreadIdToPtrMap( long lMaxThreads )
    : m_lMaxThreads(lMaxThreads)
{
    m_pThreadIdMap = new THREADINFO[lMaxThreads];
    memset(m_pThreadIdMap, 0, sizeof(THREADINFO) * lMaxThreads);
}

CGrowableThreadPool::CThreadIdToPtrMap::~CThreadIdToPtrMap()
{
    delete [] m_pThreadIdMap;
}

void CGrowableThreadPool::CThreadIdToPtrMap::AddThread
(
    DWORD       dwThreadId,
    CMclThread *pThread
)
{
    for( long lThread = 0; lThread < m_lMaxThreads; lThread++ )
    {
        if( !m_pThreadIdMap[lThread].fSlotUsed )
        {
            m_pThreadIdMap[lThread].fSlotUsed  = TRUE;
            m_pThreadIdMap[lThread].dwThreadId = dwThreadId;
            m_pThreadIdMap[lThread].pThread    = pThread;
            return;
        }
    }
}

CMclThread *CGrowableThreadPool::CThreadIdToPtrMap::GetThreadFromId
(
    DWORD   dwThreadId 
)
{
    for( long lThread = 0; lThread < m_lMaxThreads; lThread++ )
    {
        if(
            m_pThreadIdMap[lThread].fSlotUsed &&
            (m_pThreadIdMap[lThread].dwThreadId == dwThreadId)
        )
        {
            return(m_pThreadIdMap[lThread].pThread);
        }
    }

    return(0);
}

void CGrowableThreadPool::CThreadIdToPtrMap::FreeSlot( DWORD dwThreadId )
{
    for( long lThread = 0; lThread < m_lMaxThreads; lThread++ )
    {
        if(
            m_pThreadIdMap[lThread].fSlotUsed &&
            (m_pThreadIdMap[lThread].dwThreadId == dwThreadId)
        )
        {
            m_pThreadIdMap[lThread].fSlotUsed = FALSE;
        }
    }
}

void CGrowableThreadPool::CThreadIdToPtrMap::FreeSlot( CMclThread *pThread )
{
    for( long lThread = 0; lThread < m_lMaxThreads; lThread++ )
    {
        if(
            m_pThreadIdMap[lThread].fSlotUsed &&
            (m_pThreadIdMap[lThread].pThread == pThread)
        )
        {
            m_pThreadIdMap[lThread].fSlotUsed = FALSE;
        }
    }
}
