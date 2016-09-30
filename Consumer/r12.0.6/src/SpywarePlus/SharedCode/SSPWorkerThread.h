#pragma once

#include "ccCoInitialize.h"
#include "SSPRunnableHostThread.h"

class CMTAWorkerThread : 
    public CSSPRunnableHostThread
{
public:
    CMTAWorkerThread(){};
    virtual ~CMTAWorkerThread(){};

    STDMETHOD(RunInstance())
    {
        try
        {
            ccLib::CCoInitialize scInit(ccLib::CCoInitialize::eMTAModel);
            CSSPRunnableHostThread::RunInstance();
        }
        catch(...)
        {
        }

        return S_OK;
    };

};

typedef StahlSoft::CSmartRefCountPtr < CMTAWorkerThread::CContainedRunnableRefCount > CSmartRefCountRunnableJob;

class CMTAWorkerMessageThread : 
    public CSSPRunnableHostThread
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

class CSTAWorkerThread : 
    public CSSPRunnableHostThread
{
public:
    CSTAWorkerThread(){};
    virtual ~CSTAWorkerThread(){};

    STDMETHOD(RunInstance())
    {
        try
        {
            ccLib::CCoInitialize scInit(ccLib::CCoInitialize::eSTAModel);
            CSSPRunnableHostThread::RunInstance();
        }
        catch(...)
        {
        }

        return S_OK;
    };

};

class CSTAWorkerMessageThread : 
    public CSSPRunnableHostThread
{
public:
    CSTAWorkerMessageThread(){};
    virtual ~CSTAWorkerMessageThread(){};

    STDMETHOD(RunInstance());
};

inline STDMETHODIMP CSTAWorkerMessageThread::RunInstance()
{
    try
    {
        ccLib::CCoInitialize scInit(ccLib::CCoInitialize::eSTAModel);

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
