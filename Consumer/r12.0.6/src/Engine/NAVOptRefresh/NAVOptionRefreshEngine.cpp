// NAVOptionRefreshEngine.cpp: implementation of the CNAVOptionRefreshEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVOptionRefreshEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVOptionRefreshEngine::CNAVOptionRefreshEngine()
{
    m_mutex = ::CreateMutex(NULL,FALSE,NULL);
    m_mutexLock.Attach(m_mutex);
    m_rhtAboveNormalThread.SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
    m_thread.SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
    CNavOptionRefreshThreadSource* pSource = (CNavOptionRefreshThreadSource*)&m_thread;
    pSource->RegisterEventSink(this);
}

CNAVOptionRefreshEngine::~CNAVOptionRefreshEngine()
{

}
SIMON_STDMETHODIMP CNAVOptionRefreshEngine::Init()
{
    HRESULT hr = S_OK;
    StahlSoft::HRX hrx;
    try
    {
        // whatever 
    }
    catch(_com_error& e)
    {
        hr = e.Error();
    }
    return hr;
}
SIMON_STDMETHODIMP CNAVOptionRefreshEngine::Start()
{
    HRESULT hr = S_OK;
    StahlSoft::HRX hrx;
    try
    {
        if(m_thread.IsRunning() == S_FALSE)
        {
            //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
            StahlSoft::CSmartLock  smLock(&m_mutexLock);
            //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
            StahlSoft::CreateThread(m_rhtAboveNormalThread);
            StahlSoft::CreateThread(m_thread);
            /*
            StahlSoft::CSmartHandle smWait(::CreateEvent(NULL,FALSE,FALSE,NULL));
            StahlSoft::CEventSystemThreadBaseSink mySink(smWait);
            StahlSoft::CEventSourceSmartRegistration<CNavOptionRefreshThread,StahlSoft::CEventSystemThreadBaseSink> smReg(&m_thread,&mySink);
            m_thread.CreateThread();
            ::WaitForSingleObject(smWait,INFINITE);
            */

        }
    }
    catch(_com_error& e)
    {
        hr = e.Error();
		CCTRACEE(_T("CNAVOptionRefreshEngine::Start - Exception %08X"), hr);
    }
    return hr;
}

SIMON_STDMETHODIMP CNAVOptionRefreshEngine::Stop()
{
    HRESULT hr = S_OK;
    StahlSoft::HRX hrx;
    try
    {

        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        StahlSoft::CSmartLock  smLock(&m_mutexLock);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        m_thread.RequestExit();
        // wait 1 minute, max
		DWORD dwWait = StahlSoft::WaitForMultipleObjectsWithMessageLoop( m_thread.GetThreadHandle(), 60*1000 );

        switch(dwWait)
        {
        case WAIT_TIMEOUT:
            m_thread.TerminateThread(); // bye bye, this is so bad but I had no choice you see.
            CCTRACEE(_T("CNAVOptionRefreshEngine::Stop - timeout"));
            break;
        default:
            CCTRACEI(_T("CNAVOptionRefreshEngine::Stop - exiting"));
            break;
        }
    }
    catch(_com_error& e)
    {
        hr = e.Error();
		CCTRACEE(_T("CNAVOptionRefreshEngine::Stop - Exception %08X"), hr);
    }
    return hr;
}
