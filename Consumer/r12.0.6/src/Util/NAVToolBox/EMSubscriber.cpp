// EMSubscriber.cpp: implementation of the CEMSubscriber class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EMSubscriber.h"

using namespace ccLib;
using namespace ccEvtMgr;

    // TODO : use TR from xlib.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEMSubscriber::CEMSubscriber(CEMSubscriberSink* pSink) : 
    m_pSink (pSink)
{
}

CEMSubscriber::~CEMSubscriber()
{
}

bool CEMSubscriber::Connect()
{
     CCTRACEI ( "CEMSubscriber() connecting" );
   
    if ( !m_pSink )
    {
        CCTRACEW ( "CEMSubscriber::Connect() - no sink" );
        return false;
    }

    if ( !m_EventManager.IsEventManagerActive())
    {
        CCTRACEW ("CEMSubscriber() - EM not active");
        return false;
    }

    ccEvtMgr::CEventFactoryEx2Ptr pEventFactory;
    ccEvtMgr::CProxyFactoryExPtr pProxyFactory;

    // Load the factory and proxy DLLs
    //
    if (SYM_FAILED(AV::NAVEventFactory_CEventFactoryEx2::CreateObject(GETMODULEMGR(), &pEventFactory)) ||
        pEventFactory == NULL ||
        SYM_FAILED(AV::NAVEventFactory_CProxyFactoryEx::CreateObject(GETMODULEMGR(), &pProxyFactory)) ||
        pProxyFactory == NULL ||
        m_EventManager.Create(pEventFactory, pProxyFactory) == FALSE)
    {
        Disconnect();
        CCTRACEE ( "CEMSubscriber() failed to connect - did you init COM?" );
        return FALSE;
    }

    m_EventManager.AddRef();

    // Build the subscriber info

    // Register the subscriber
    if (CSubscriberHelper::Create(&m_EventManager, &m_vecSubscriberInfo[0], (long)m_vecSubscriberInfo.size()) == FALSE)
    {
        Disconnect();
        return FALSE;
    }

    CCTRACEI ( "CEMSubscriber() connected successfully" );
    return TRUE;
}

bool CEMSubscriber::Disconnect ()
{
    if ( !m_EventManager.IsEventManagerActive())
    {
        CCTRACEW ("CEMSubscriber::Disconnect - EM not active");
        return false;
    }

    // Destroy in reverse order
    CSubscriberHelper::Destroy();
    m_EventManager.Destroy();

    return true;
}

bool CEMSubscriber::IsEMActive()
{
    return m_EventManager.IsEventManagerActive()?true:false;
}

CError::ErrorType CEMSubscriber::OnEvent( long nSubscriberId, 
                                          const CEventEx& Event,
                                          ccEvtMgr::CSubscriberEx::EventAction& eAction)
{
    // Default action for sync events is to recommit them.
    //
    if ( !Event.GetBroadcast() )
        eAction = ccEvtMgr::CSubscriberEx::EventAction::eCommitAction;

    if ( m_pSink )
        m_pSink->EMSubscriberOnEvent ( Event, eAction ); 
    return CError::eNoError;
}

// Makes a copy of the event.
//
bool CEMSubscriber::CopyEvent (const CEventEx& Event /*in*/, ccEvtMgr::CEventEx*& pEvent /*out*/)
{
    // Copy event
    //
    
    if (m_EventManager.CopyEvent(Event, pEvent) == CError::eNoError)
    {
        return true;
    }
    
    ASSERT(FALSE);
    return false;
}


CError::ErrorType CEMSubscriber::OnShutdown()
{
    CCTRACEI ("CEMSubscriber::OnShutdown() - shutting down");

    if ( m_pSink )
        m_pSink->EMSubscriberOnShutdown ();  

    m_vecSubscriberInfo.clear();

    // We may not directly unregister when shutting down!
    // Defect 1-2K0HKH

    return CError::eNoError;
}

void CEMSubscriber::DeleteEvent(ccEvtMgr::CEventEx* pEvent)
{
    m_EventManager.DeleteEvent(pEvent);
}
