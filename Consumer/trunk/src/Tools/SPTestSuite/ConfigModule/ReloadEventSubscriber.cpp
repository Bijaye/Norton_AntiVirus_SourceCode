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

#include "ReloadEventSubscriber.h"
#include "SymProtectEventsInterface.h"

#define WORK_THREAD_TERMINATE_WAIT_MS INFINITE

using namespace ccEvtMgr;
using namespace SymProtectEvt;

CSymProtectReloadEventSubscriber::CSymProtectReloadEventSubscriber() :
    m_WorkThread(&m_NotifyEvent)
{
}

CSymProtectReloadEventSubscriber::~CSymProtectReloadEventSubscriber()
{
    Destroy();
}

bool CSymProtectReloadEventSubscriber::Create(ISymProtectReloadCallback& EventCB)
{
    Destroy();

    // Check for double-initialization
    if( m_WorkThread.IsThread() )
        return false;

    // Load the event factory

    if( SYM_FAILED(m_EventFactoryLoader.CreateObject(&m_pEventFactory)) ||
        m_pEventFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectReloadEventSubscriber::Create() - m_EventFactoryLoader.CreateObject() failed.\n"));
        Destroy();
        return false;
    }

    // Load the proxy factory

    if( SYM_FAILED(m_ProxyFactoryLoader.CreateObject(&m_pProxyFactory)) ||
        m_pProxyFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectReloadEventSubscriber::Create() - m_ProxyFactoryLoader.CreateObject() failed.\n"));
        Destroy();
        return false;
    }

    // Set up the event manager helper

    if( !m_EventManagerHelper.Create(m_pEventFactory, m_pProxyFactory) )
    {
        CCTRACEE(_T("CSymProtectReloadEventSubscriber::Create() - m_EventManagerHelper.Create() failed.\n"));
        Destroy();
        return false;
    }
    
    // Initialize the subscription list

    CSubscriberHelper::SubscriberInfo evtArray[] = {
        { CReloadDllEventEx::TypeId, 0, NULL, 0, 0, false }
    };

    // Call the base class Create method

    if( !CSubscriberHelper::Create(&m_EventManagerHelper, evtArray, CCDIMOF(evtArray)) )
    {
        CCTRACEE(_T("CSymProtectReloadEventSubscriber::Create() - CSubscriberHelper::Create() failed.\n"));
        Destroy();
        return false;
    }

    // Create the notification event that will be used when events come in

    if( !m_NotifyEvent.Create(NULL, FALSE, FALSE, NULL, false) )
    {
        CCTRACEE(_T("CSymProtectReloadEventSubscriber::Create() - m_WorkThread.Create() failed.\n"));
        Destroy();
        return false;
    }

    // Start the worker thread

    if( !m_WorkThread.Create(&EventCB) )
    {
        CCTRACEE(_T("CSymProtectReloadEventSubscriber::Create() - m_WorkThread.Create() failed.\n"));
        Destroy();
        return false;
    }

    return true;
}

void CSymProtectReloadEventSubscriber::Destroy()
{
    // Destroy in reverse order

    m_WorkThread.Terminate(WORK_THREAD_TERMINATE_WAIT_MS);

    m_NotifyEvent.Destroy();

    CSubscriberHelper::Destroy();

    m_EventManagerHelper.Destroy();

    m_pProxyFactory.Release();

    m_pEventFactory.Release();
}

CError::ErrorType CSymProtectReloadEventSubscriber::OnEvent(long nSubscriberId,
                                                            const CEventEx& Event,
                                                            EventAction& eAction)
{
    eAction = eNoAction;

    // Make sure that this is indeed a reload event
    CReloadDllEventExQIPtr pReloadEvent(&Event);
    if( pReloadEvent == NULL )
    {
        CCTRACEE(_T("CSymProtectReloadEventSubscriber::OnEvent() - Can't create CReloadDllEventEx\n"));
        return CError::eFatalError;
    }

    // Notify the worker thread that we got a reload event.
    // The client callback will be called to disconnect/reconnect any
    // providers and subscribers.
    m_NotifyEvent.SetEvent();

    if( !Event.GetBroadcast() )
        eAction = eCommitAction;

    return CError::eNoError;
}

CError::ErrorType CSymProtectReloadEventSubscriber::OnShutdown()
{
    return CError::eNoError;
}

CSymProtectReloadEventSubscriber::
ReloadWorkThread::ReloadWorkThread(ccLib::CEvent* pNotifyEvent) :
    m_pNotifyEvent(pNotifyEvent), m_pEventCB(NULL)
{
    // Set thread options
    m_Options.m_bNoCRTThread = TRUE;
    m_Options.m_bPumpMessages = TRUE;
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
}

BOOL CSymProtectReloadEventSubscriber::
ReloadWorkThread::Create(ISymProtectReloadCallback* pCallback)
{
    ASSERT(pCallback);
    if( !pCallback )
        return FALSE;

    m_pEventCB = pCallback;

    if( !m_ShutdownEvent.Create(NULL, false, false, NULL, false) )
        return FALSE;

    return CThread::Create(NULL, NULL, 0);
}

void CSymProtectReloadEventSubscriber::
ReloadWorkThread::PostTerminate()
{
    m_ShutdownEvent.SetEvent();
    CThread::PostTerminate();
}

int CSymProtectReloadEventSubscriber::
ReloadWorkThread::Run()
{
    HANDLE pEvents[] = { *m_pNotifyEvent, m_ShutdownEvent };
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
        case WAIT_OBJECT_0: // Notify
            m_pEventCB->HandleSymProtectReload();
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
