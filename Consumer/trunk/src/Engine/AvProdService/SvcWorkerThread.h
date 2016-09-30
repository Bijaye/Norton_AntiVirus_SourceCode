////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ccCoInitialize.h>
#include <SysThreadBase.h>
#include <PushPopQueWithEvents.h>
#include <ContainedSmartPtrRefCount.h>
#include <EventSourceT.h>

namespace AvProdSvc
{
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++
    //++ AvProdSvc::CRunnableHostThread
    //++ AvProdSvc::CRunnableHostThread
    //++
    //++  Virtually identical to StahlSoft CRunnableHostThread, but
    //++  modified the scope of a couple of vars to be better inheritable
    //++
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    class CRunnableHostThread:
        public StahlSoft::CSystemThreadBase
        ,public StahlSoft::CPushPopQueueSignalSink
    {
    public:
        typedef StahlSoft::CContainedSmartPtrRefCount< CRunnableHostThread >                       
            /*>>>>>>>>>>>*/CContainedRunnableHostThreadRefCount;    /*<<<<<CContainedRunnableHostThreadRefCount<<<<<<<<*/

            typedef StahlSoft::CContainedSmartPtrRefCount< CRunnable >                                 
            /*>>>>>>>>>>>*/CContainedRunnableRefCount;              /*<<<<<CContainedRunnableRefCount<<<<<<<<*/

            typedef StahlSoft::CPushPopQueueSignal< StahlSoft::CSmartRefCountPtr < CContainedRunnableRefCount > > 
            /*>>>>>>>>>>>*/CRunnableQueueWithSignal;                /*<<<<<CRunnableQueueWithSignal<<<<<<<<*/

            typedef StahlSoft::CContainedSmartPtrRefCount< CRunnableQueueWithSignal >                  
            /*>>>>>>>>>>>*/CContainedRunnableQueueWithSignalRefCount;/*<<<<<CContainedRunnableQueueWithSignalRefCount<<<<<<<<*/


            virtual ~CRunnableHostThread()
        {

        }

        CRunnableHostThread(bool bHostQueue = true)
        {
            m_pQueue = NULL;
            CRunnableQueueWithSignal* pRQWS = NULL;
            if(bHostQueue)
            {
                // Looks like we are going to host the Queue within ourselves
                pRQWS = new CRunnableQueueWithSignal;
                StahlSoft::CSmartRefCountPtr< CContainedRunnableQueueWithSignalRefCount > 
                    smContained(new CContainedRunnableQueueWithSignalRefCount(pRQWS));

                m_queueRunnable = smContained;
            }
            // hook up the queue.  if we hosted that is.
            SetQueuePointer(bHostQueue
                ?((CContainedRunnableQueueWithSignalRefCount*)m_queueRunnable)->GetContained()
                :NULL);
        }
        STDMETHOD(RunInstance)();
        STDMETHOD(RequestExit)();

        //////////////////////////
        // CPushPopQueueSignalSink
        virtual void OnItemAdded()
        {
            // called everytime an item gets added to the queue we are registered with.
            if(HANDLE(m_shProcessRunnable) != HANDLE(NULL))
                ::SetEvent(m_shProcessRunnable);
        }
        void SetQueuePointer(CRunnableHostThread::CRunnableQueueWithSignal* pQueue)
        {
            // hook up the queue, the queue can come from anywhere, outside or self hosted.
            if(m_pQueue)
                m_pQueue->UnregisterEventSink(this);
            m_pQueue = pQueue;
            if(m_pQueue)
                ((CRunnableHostThread::CRunnableQueueWithSignal*)m_pQueue)->RegisterEventSink(this);

        }
        CRunnableHostThread::CRunnableQueueWithSignal* GetQueuePointer()
        {
            return m_pQueue;
        }

    protected:
        StahlSoft::CSmartRefCountPtr< CContainedRunnableQueueWithSignalRefCount > m_queueRunnable; // only used when this thread object hosts the queue;

        void ProcessRunnableObject(); // internal method for pulling runnable objects out of a queue and running them.

        StahlSoft::CSmartHandle m_shStop;              // signalled when RequestExit() is called
        StahlSoft::CSmartHandle m_shProcessRunnable;   // signalled when we get a callback telling us that an item is in the Runnable queue

        CRunnableHostThread::CRunnableQueueWithSignal*      m_pQueue; // the queue we are registered with.

        StahlSoft::CSmartRefCountPtr < CContainedRunnableRefCount >    m_currentActiveRunnable; // reference to the current runnable object we are running.
        StahlSoft::CCritSec                                            m_critCurrentActiveRunnable; // quards access to m_currentActiveRunnable 
    };

    inline STDMETHODIMP CRunnableHostThread::RequestExit()
    {
        try
        {
            //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
            StahlSoft::CSmartCritSection smCrit(&m_critCurrentActiveRunnable);
            //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
            ::SetEvent(m_shStop);
            // we have to request exit from the currently running CRunnable object.
            if(m_currentActiveRunnable.m_p)
                m_currentActiveRunnable.m_p->m_smContained->RequestExit();

        }
        catch(...)
        {
            return E_UNEXPECTED;
        }// eatit
        return S_OK;

    }

    inline STDMETHODIMP CRunnableHostThread::RunInstance()
    {
        try
        {
            m_shStop		    = ::CreateEvent(NULL,TRUE,FALSE,NULL); // once set always set
            m_shProcessRunnable	= ::CreateEvent(NULL,FALSE,FALSE,NULL);
            HANDLE hWait[2] = {m_shStop,m_shProcessRunnable};
            Fire_OnThreadReady(this);

            ///////////////////////////
            // Start working right away, because the queue could exist with items before we are started.
            // Hence, we will assume that we have already been signaled
            ProcessRunnableObject();

            bool bRun = true;
            DWORD dwRet = 0;
            while(bRun)
            {
                dwRet = ::WaitForMultipleObjects(
                    2,			// number of handles in handle array
                    hWait,		// pointer to an object-handle array
                    FALSE,
                    INFINITE	// time-out interval in milliseconds
                    );
                switch(dwRet)
                {
                case WAIT_OBJECT_0:
                    bRun = 0;
                    break;
                case WAIT_OBJECT_0 + 1:
                    ProcessRunnableObject();
                    break;
                default:
                    // loop around to dispatch the windows messages.
                    break;
                }

            }
            return S_OK;
        }
        catch(...)
        {
            return E_UNEXPECTED;
        }

    }

    inline void CRunnableHostThread::ProcessRunnableObject()
    {
        HRESULT hr = S_OK;
        try
        {
            _ASSERTE(m_pQueue && "m_pQueue != NULL");
            if(!m_pQueue)
                return;
            do
            {
                {/* scoped */
                    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
                    StahlSoft::CSmartCritSection smCrit(&m_critCurrentActiveRunnable);
                    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
                    if(::WaitForSingleObject(m_shStop,0) == WAIT_OBJECT_0)
                        return;
                    StahlSoft::CSmartRefCountPtr < CContainedRunnableRefCount > runnable;
                    hr = m_pQueue->PopItem(runnable);
                    if(FAILED(hr))
                        break;
                    m_currentActiveRunnable = runnable;
                }/* scoped */

                StahlSoft::CRunnable* pRunnable = (m_currentActiveRunnable.m_p->m_smContained);
                _ASSERTE(pRunnable != NULL && "pRunnable != NULL");
                if(!pRunnable)
                    continue;
                pRunnable->Run();

                {/* scoped */
                    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
                    StahlSoft::CSmartCritSection smCrit(&m_critCurrentActiveRunnable);
                    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
                    m_currentActiveRunnable.Release();
                }
            }while(true);
        }
        catch(...)
        {}
    }

    class CMTAWorkerThread : 
        public CRunnableHostThread
    {
    public:
        CMTAWorkerThread(){};
        virtual ~CMTAWorkerThread(){};

        STDMETHOD(RunInstance())
        {
            try
            {
                ccLib::CCoInitialize scInit(ccLib::CCoInitialize::eMTAModel);
                CRunnableHostThread::RunInstance();
            }
            catch(...)
            {
            }

            return S_OK;
        };

    };

    typedef StahlSoft::CSmartRefCountPtr < CMTAWorkerThread::CContainedRunnableRefCount > CSmartRefCountRunnableJob;

    class CMTAWorkerMessageThread : 
        public CRunnableHostThread
    {
    public:
        CMTAWorkerMessageThread(){};
        virtual ~CMTAWorkerMessageThread(){};

        STDMETHOD(RunInstance());
    };

    inline STDMETHODIMP CMTAWorkerMessageThread::RunInstance()
    {
        try
        {
            ccLib::CCoInitialize scInit(ccLib::CCoInitialize::eMTAModel);

            m_shStop		    = ::CreateEvent(NULL,TRUE,FALSE,NULL); // once set always set
            m_shProcessRunnable	= ::CreateEvent(NULL,FALSE,FALSE,NULL);
            HANDLE hWait[2] = {m_shStop,m_shProcessRunnable};
            Fire_OnThreadReady(this);

            ///////////////////////////
            // Start working right away, because the queue could exist with items before we are started.
            // Hence, we will assume that we have already been signaled
            ProcessRunnableObject();

            bool bRun = true;
            DWORD dwRet = 0;
            while(bRun)
            {
                dwRet = StahlSoft::WaitForMultipleObjectsWithMessageLoop(
                    2,			// number of handles in handle array
                    hWait,		// pointer to an object-handle array
                    INFINITE	// time-out interval in milliseconds
                    );
                switch(dwRet)
                {
                case WAIT_OBJECT_0:
                    bRun = 0;
                    break;
                case WAIT_OBJECT_0 + 1:
                    ProcessRunnableObject();
                    break;
                default:
                    // loop around to dispatch the windows messages.
                    break;
                }

            }
            return S_OK;
        }
        catch(...)
        {
            return E_UNEXPECTED;
        }

    }

}; // AvProdSvc
