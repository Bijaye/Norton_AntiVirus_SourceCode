////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// SymProtectControlAppDlg.cpp : implementation file
//

#include "stdafx.h"

#include "ccCatch.h"
#include "PolicyEventSubscriber.h"
#include "SymProtectEventsInterface.h"

#define WORK_THREAD_TERMINATE_WAIT_MS INFINITE

using namespace ccEvtMgr;
using namespace SymProtectEvt;

CSymProtectPolicyEventSubscriber::CSymProtectPolicyEventSubscriber() :
    m_WorkThread(*this)
{
}

CSymProtectPolicyEventSubscriber::~CSymProtectPolicyEventSubscriber()
{
    Destroy();
}

bool CSymProtectPolicyEventSubscriber::Create(ISymProtectPolicyEventCallback& EventCB)
{
    Destroy();

    // Check for double-initialization
    if( m_WorkThread.IsThread() )
        return false;

    // Load the event factory

    if( SYM_FAILED(m_EventFactoryLoader.CreateObject(&m_pEventFactory)) ||
        m_pEventFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectPolicyEventSubscriber::Create() - m_EventFactoryLoader.CreateObject() failed.\n"));
        Destroy();
        return false;
    }

    // Load the proxy factory

    if( SYM_FAILED(m_ProxyFactoryLoader.CreateObject(&m_pProxyFactory)) ||
        m_pProxyFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectPolicyEventSubscriber::Create() - m_ProxyFactoryLoader.CreateObject() failed.\n"));
        Destroy();
        return false;
    }
	
    // Set up the event manager helper
    if( !m_EventManagerHelper.Create(m_pEventFactory, m_pProxyFactory) )
    {
        CCTRACEE(_T("CSymProtectPolicyEventSubscriber::Create() - m_EventManagerHelper.Create() failed.\n"));
        Destroy();
        return false;
    }
    
    // Initialize the subscription list

    CSubscriberHelper::SubscriberInfo evtArray[] = {
        { CPolicyEventEx::TypeId, 0, NULL, 0, 0, true }
    };

    // Call the base class Create method

    if( !CSubscriberHelper::Create(&m_EventManagerHelper, evtArray, CCDIMOF(evtArray)) )
    {
        CCTRACEE(_T("CSymProtectPolicyEventSubscriber::Create() - CSubscriberHelper::Create() failed.\n"));
        Destroy();
        return false;
    }

    // Clean out the queue

    while(!m_queueEvents.empty())
        m_queueEvents.pop();    

    // Create the notification event that will be used when events come in

    if( !m_NotifyEvent.Create(NULL, FALSE, FALSE, NULL, false) )
    {
        CCTRACEE(_T("CSymProtectPolicyEventSubscriber::Create() - m_WorkThread.Create() failed.\n"));
        Destroy();
        return false;
    }

    // Start the worker thread

    if( !m_WorkThread.Create(&EventCB) )
    {
        CCTRACEE(_T("CSymProtectPolicyEventSubscriber::Create() - m_WorkThread.Create() failed.\n"));
        Destroy();
        return false;
    }

    return true;
}

void CSymProtectPolicyEventSubscriber::Destroy()
{
    // Destroy in reverse order

    m_WorkThread.Terminate(WORK_THREAD_TERMINATE_WAIT_MS);

    m_NotifyEvent.Destroy();

    // empty the queue
    m_csEventQueue.Lock();
    while(!m_queueEvents.empty())
        m_queueEvents.pop();
    m_csEventQueue.Unlock();

    CSubscriberHelper::Destroy();

    m_EventManagerHelper.Destroy();

    m_pProxyFactory.Release();

    m_pEventFactory.Release();
}

CError::ErrorType CSymProtectPolicyEventSubscriber::OnEvent(long nSubscriberId,
                                                            const CEventEx& Event,
                                                            EventAction& eAction)
{
    eAction = eNoAction;

    {
        // Enter the critical section to protect the queue
        ccLib::CSingleLock lock(&m_csEventQueue, INFINITE, FALSE);

        // Check that we don't overflow the queue
        if( m_queueEvents.size() > 100 )
        {
            CCTRACEW(_T("CSymProtectPolicyEventSubscriber::OnEvent() - Dropping event.\n"));
            return CError::eNoError;
        }

        // Make sure that this is indeed a policy event
        CPolicyEventExQIPtr pPolicyEvent(&Event);
        if( !pPolicyEvent )
        {
            CCTRACEE(_T("CSymProtectPolicyEventSubscriber::OnEvent() - Can't create CPolicyEventExPtr\n"));
            return CError::eFatalError;
        }

        // Make a copy of the event
        CEventEx* pEventCopy;
        if( m_pEventFactory->CopyEvent(*pPolicyEvent, pEventCopy) 
            != CError::eNoError )
        {
            CCTRACEE(_T("CSymProtectPolicyEventSubscriber::OnEvent() - m_pEventFactory->CopyEvent() failed.\n"));
            return CError::eFatalError;
        }

        CPolicyEventExQIPtr pPolicyEventCopy(pEventCopy);
        ASSERT(pPolicyEventCopy);

        // Add the new event to the event queue
        ccLib::CExceptionInfo exceptionInfo;
        try
        {
            m_queueEvents.push(pPolicyEventCopy);
            
            // The Queue is smart pointers so the event will have an extra refcount
            pPolicyEventCopy->Release();
        }
        CCCATCHMEM(exceptionInfo);
        if(exceptionInfo.IsException() != FALSE)
        {
            pPolicyEventCopy->Release();
            return CError::eFatalError;
        }
    } // -- Leave critical section

    // Notify the worker thread that there is a new event in the queue
    m_NotifyEvent.SetEvent();

    return CError::eNoError;
}

CError::ErrorType CSymProtectPolicyEventSubscriber::OnShutdown()
{
    return CError::eNoError;
}

CSymProtectPolicyEventSubscriber::
PolicyEventThread::PolicyEventThread(
    CSymProtectPolicyEventSubscriber& Subscriber) :
        m_pEventCB(NULL),
        m_Subscriber(Subscriber),
        m_NotifyEvent(Subscriber.m_NotifyEvent),
        m_csEventQueue(Subscriber.m_csEventQueue), 
        m_queueEvents(Subscriber.m_queueEvents)
{
    // Set thread options
    m_Options.m_bNoCRTThread = TRUE;
    m_Options.m_bPumpMessages = TRUE;
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
}

BOOL CSymProtectPolicyEventSubscriber::
PolicyEventThread::Create(ISymProtectPolicyEventCallback* pCallback)
{
    ASSERT(pCallback);
    if( !pCallback )
        return FALSE;

    m_pEventCB = pCallback;

    if( !m_ShutdownEvent.Create(NULL, false, false, NULL, false) )
        return FALSE;

    return CThread::Create(NULL, 0, 0);
}

void CSymProtectPolicyEventSubscriber::
PolicyEventThread::PostTerminate()
{
    m_ShutdownEvent.SetEvent();
    CThread::PostTerminate();
}

int CSymProtectPolicyEventSubscriber::
PolicyEventThread::Run()
{
    HANDLE pEvents[] = { m_NotifyEvent, m_ShutdownEvent };
    ccLib::CMessageLock lock(TRUE, FALSE);

    // Make sure we were created correctly
    ASSERT(m_pEventCB);
    if( !m_pEventCB )
        return -1;

    // Just wait for events. The shutdown event will terminate this loop
    bool bQuit = false;
    do
    {
        // Wait for one of our events to signal
        DWORD dwResult = lock.Lock(CCDIMOF(pEvents), pEvents, FALSE, INFINITE, FALSE);
        switch(dwResult)
        {
        case WAIT_OBJECT_0: // Notify the client of pending events

            // Empty all messages in the queue
            while( !m_queueEvents.empty() )
            {
                CPolicyEventExPtr pEvent = NULL;

                // Acquire the lock for queue access. There should not be any
                // way for the queue to become empty outside of this function
                m_csEventQueue.Lock();
                pEvent = m_queueEvents.front();
                m_queueEvents.pop();
                m_csEventQueue.Unlock();

                if( pEvent )
                {
                    m_pEventCB->HandleSymProtectEvent(pEvent);

                    // Only commit non-broadcast events
                    if( !pEvent->GetBroadcast() )
                        m_Subscriber.CommitEvent(*pEvent);
                }
            }
            break;

        case WAIT_OBJECT_0+1: // Shutdown
            bQuit = true;
            break;

        default:
            // Uh-oh... just quit
            bQuit = true;
            break;
        }
    } while( !bQuit );

    return 0;
}
