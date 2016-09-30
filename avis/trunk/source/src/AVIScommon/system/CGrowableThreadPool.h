//
// FILE: CGrowableThreadPool.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////

#ifndef __CGrowableThreadPool_H__
#define __CGrowableThreadPool_H__

#include "CMclGlobal.h"
#include "CMclAutoLock.h"
#include "CMclLinkedLists.h"
#include "CMclThread.h"
#include "CMclCritSec.h"
#include "CMclSemaphore.h"
#include "CMclEvent.h"
#include "CMclWaitableCollection.h"
#include "CMclAutoPtr.h"
#include "CThreadPool.h"

// CLinkedListRemovable
//
// This class provides a Remove() operation in addition to the
// regulare operations provided by CMclLinkedList.  To use
// this class, objects of type T must provide an equality (==)
// operator.
//
template <class T>
class AVISCOMMON_API CLinkedListRemovable : public CMclLinkedList<T>
{
    public:
        BOOL Remove( T & rData )
        {
            BOOL fFoundAndRemoved = FALSE;

            // decrease the semaphore count...
            // if the wait fails, there are no jobs in the queue...
            if (CMclWaitTimeout(m_csNotEmpty.Wait(0))) 
            {
                return fFoundAndRemoved;
            }

            CMclAutoLock        Lock(m_cCritSec);
            CMclLinkedListNode *pTargetNode = 0;
            CMclLinkedListNode *pNode = m_MasterNode.m_pNext;
            T                   NodeData;
        
            // Search for a node with matchin data.
            //
            while( !fFoundAndRemoved && (pNode != &m_MasterNode) )
            {
                pNode->GetData(NodeData);

                if( NodeData == rData )
                {
                    // Found it!  Make a note and terminate the
                    // search.
                    //
                    fFoundAndRemoved = TRUE;
                    pTargetNode = pNode;
                }
                else
                {
                    pNode = pNode->m_pNext;
                }
            }

            if( fFoundAndRemoved )
            {
                // Link the node before the target node to the node
                // following the target node.
                //
                pTargetNode->m_pPrev->m_pNext = pTargetNode->m_pNext;
                pTargetNode->m_pNext->m_pPrev = pTargetNode->m_pPrev;

                // Toss the removed node back on the free list of nodes.
                //
                AddToFreeList(pTargetNode);

            }
            else 
            {
                // no node to remove, we need to bump the semaphore
                // count back up...
                m_csNotEmpty.Release(1);
            }

            return(fFoundAndRemoved);
        }
};

#pragma warning(disable: 4251)	// warns that CmclQueue<classCGrowableThreadPool> and
								// CLinkedListRemovable<CmClThread*>  need to be
								// exported to dll users.  Since it seems to work anyway
								// I'm getting rid of the warning message (it bothers me)

class AVISCOMMON_API CGrowableThreadPool : public CThreadPool, private CMclThreadHandler
{
    public:
        CGrowableThreadPool( long lMinThreads, long lMaxThreads, DWORD dwThreadLifetime );
        virtual ~CGrowableThreadPool();

        // CThreadPool implementation.
        //
        virtual BOOL DispatchThread( CMclThreadHandler *pHandler );

    private:
        // CMclThreadHandler implementation.
        //
        virtual unsigned ThreadHandlerProc( void );

    private:
        // CDispatchRecords are enqueued to signal a pending
        // DispatchThread operation that needs to be serviced.
        //
        class CDispatchRecord
        {
            public:
                CDispatchRecord();
                CDispatchRecord( CMclThreadHandler *pThreadHandler );

                unsigned Execute( void );
                int operator == ( const CDispatchRecord& rhs );

            private:
                CMclThreadHandler  *m_pUserThreadHandler;
        };

        // Because threads in this pool all execute the same
        // thread handler in the CGrowableThreadPool class, and
        // because threads may terminate themselves if they
        // sit idle too long in the pool, the thread handler proce
        // needs a way to recover the pointer to the CMclThread
        // object that embodies the thread that is executing.
        // To do this, we maintain a map between thread ids and
        // CMclThread * pointers.
        //
        class AVISCOMMON_API CThreadIdToPtrMap
        {
            public:
                CThreadIdToPtrMap( long lMaxThreads );
                ~CThreadIdToPtrMap();

                void        AddThread( DWORD dwThreadId, CMclThread *pThread );
                CMclThread *GetThreadFromId( DWORD dwThreadId );
                void        FreeSlot( DWORD dwThreadId );
                void        FreeSlot( CMclThread *pThread );

            private:
                struct THREADINFO
                {
                    BOOL        fSlotUsed;
                    DWORD       dwThreadId;
                    CMclThread *pThread;
                };

                THREADINFO *m_pThreadIdMap;
                long        m_lMaxThreads;
        };

    private:
        // Thread pool synchronization and management.
        //
        CMclCritSec                         m_csPool;
        CMclEvent                           m_ExitEvent;
        DWORD                               m_dwThreadLifetime;
        CThreadIdToPtrMap                   m_ThreadIdMap;

        // Dispatch queue and thread pool.
        //
        CMclQueue<CDispatchRecord>          m_DispatchQ;
        CMclSemaphore                       m_JobsPending;
        
        long                                m_lMinThreads;  // Numbers of alive threads; not
        long                                m_lMaxThreads;  // counting who's busy and who's
        long                                m_lCurThreads;  // not busy.

        // List of threads in the pool and a semaphore that
        // indicates how many are actually free (idle).
        //
        CLinkedListRemovable<CMclThread *>  m_ThreadsInPool;
        CMclSemaphore                       m_ThreadsFree;
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // __CGrowableThreadPool_H__

