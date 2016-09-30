#pragma once

using namespace StahlSoft;

#include "SysThreadBase.h"
#include "PushPopQueWithEvents.h"
#include "ContainedSmartPtrRefCount.h"
#include "EventSourceT.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++
//++ CSSPRunnableHostThread
//++ CSSPRunnableHostThread
//++
//++  Virtually identical to StahlSoft CRunnableHostThread, but
//++  modified the scope of a couple of vars to be better inheritable
//++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CSSPRunnableHostThread:
    public CSystemThreadBase
    ,public CPushPopQueueSignalSink
{
public:
    typedef CContainedSmartPtrRefCount< CSSPRunnableHostThread >                       
        /*>>>>>>>>>>>*/CContainedRunnableHostThreadRefCount;    /*<<<<<CContainedRunnableHostThreadRefCount<<<<<<<<*/

        typedef CContainedSmartPtrRefCount< CRunnable >                                 
        /*>>>>>>>>>>>*/CContainedRunnableRefCount;              /*<<<<<CContainedRunnableRefCount<<<<<<<<*/

        typedef CPushPopQueueSignal< CSmartRefCountPtr < CContainedRunnableRefCount > > 
        /*>>>>>>>>>>>*/CRunnableQueueWithSignal;                /*<<<<<CRunnableQueueWithSignal<<<<<<<<*/

        typedef CContainedSmartPtrRefCount< CRunnableQueueWithSignal >                  
        /*>>>>>>>>>>>*/CContainedRunnableQueueWithSignalRefCount;/*<<<<<CContainedRunnableQueueWithSignalRefCount<<<<<<<<*/


        virtual ~CSSPRunnableHostThread()
    {

    }

    CSSPRunnableHostThread(bool bHostQueue = true)
    {
        m_pQueue = NULL;
        CRunnableQueueWithSignal* pRQWS = NULL;
        if(bHostQueue)
        {
            // Looks like we are going to host the Queue within ourselves
            pRQWS = new CRunnableQueueWithSignal;
            CSmartRefCountPtr< CContainedRunnableQueueWithSignalRefCount > 
                smContained(new CContainedRunnableQueueWithSignalRefCount(pRQWS));

            m_queueRunnable = smContained;
            int i = 0;
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
    void SetQueuePointer(CSSPRunnableHostThread::CRunnableQueueWithSignal* pQueue)
    {
        // hook up the queue, the queue can come from anywhere, outside or self hosted.
        if(m_pQueue)
            m_pQueue->UnregisterEventSink(this);
        m_pQueue = pQueue;
        if(m_pQueue)
            ((CSSPRunnableHostThread::CRunnableQueueWithSignal*)m_pQueue)->RegisterEventSink(this);

    }
    CSSPRunnableHostThread::CRunnableQueueWithSignal* GetQueuePointer()
    {
        return m_pQueue;
    }

protected:
    CSmartRefCountPtr< CContainedRunnableQueueWithSignalRefCount > m_queueRunnable; // only used when this thread object hosts the queue;

    void ProcessRunnableObject(); // internal method for pulling runnable objects out of a queue and running them.

    CSmartHandle m_shStop;              // signalled when RequestExit() is called
    CSmartHandle m_shProcessRunnable;   // signalled when we get a callback telling us that an item is in the Runnable queue

    CSSPRunnableHostThread::CRunnableQueueWithSignal*      m_pQueue; // the queue we are registered with.

    CSmartRefCountPtr < CContainedRunnableRefCount >    m_currentActiveRunnable; // reference to the current runnable object we are running.
    CCritSec                                            m_critCurrentActiveRunnable; // quards access to m_currentActiveRunnable 
};

inline STDMETHODIMP CSSPRunnableHostThread::RequestExit()
{
    try
    {
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        CSmartCritSection smCrit(&m_critCurrentActiveRunnable);
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

inline STDMETHODIMP CSSPRunnableHostThread::RunInstance()
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

inline void CSSPRunnableHostThread::ProcessRunnableObject()
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
                CSmartCritSection smCrit(&m_critCurrentActiveRunnable);
                //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
                if(::WaitForSingleObject(m_shStop,0) == WAIT_OBJECT_0)
                    return;
                CSmartRefCountPtr < CContainedRunnableRefCount > runnable;
                hr = m_pQueue->PopItem(runnable);
                if(FAILED(hr))
                    break;
                m_currentActiveRunnable = runnable;
            }/* scoped */

            CRunnable* pRunnable = (m_currentActiveRunnable.m_p->m_smContained);
            _ASSERTE(pRunnable != NULL && "pRunnable != NULL");
            if(!pRunnable)
                continue;
            pRunnable->Run();

            {/* scoped */
                //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
                CSmartCritSection smCrit(&m_critCurrentActiveRunnable);
                //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
                m_currentActiveRunnable.Release();
            }
        }while(true);
    }
    catch(...)
    {}
}
