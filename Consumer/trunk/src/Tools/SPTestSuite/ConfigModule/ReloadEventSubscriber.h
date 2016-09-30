////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ReloadEventSubscriber.h : header file
//
#pragma once

#include "ccEventManagerHelper.h"
#include "ccSubscriberHelper.h"
#include "ccThread.h"

#include "SymProtectEventsLoader.h"

// Callback interface
class ISymProtectReloadCallback
{
public:
    // This function will be called when a reload event comes in through the
    // reload event subscriber. It's up to the implementors to correctly
    // let go of their own ties to the currently running binaries and reload
    // if necessary.
    virtual void HandleSymProtectReload() = 0;
};

class CSymProtectReloadEventSubscriber : ccEvtMgr::CSubscriberHelper
{
public:
    CSymProtectReloadEventSubscriber();
    virtual ~CSymProtectReloadEventSubscriber();

public:
    bool Create(ISymProtectReloadCallback& EventCB);
    void Destroy();

private:
    CSymProtectReloadEventSubscriber(const CSymProtectReloadEventSubscriber&);
    CSymProtectReloadEventSubscriber& operator=(const CSymProtectReloadEventSubscriber&);

    // CSubscriberEx implementation
public:
    ccEvtMgr::CError::ErrorType OnEvent(long nSubscriberId, 
                                        const ccEvtMgr::CEventEx& Event,
                                        EventAction& eAction);
    ccEvtMgr::CError::ErrorType OnShutdown();

private:
    ccLib::CEvent m_NotifyEvent;

    ccEvtMgr::CProxyFactoryExPtr    m_pProxyFactory;
    ccEvtMgr::CEventFactoryEx2Ptr   m_pEventFactory;
    SymProtectEvt::SymProtectEvents_CEventFactoryEx2  m_EventFactoryLoader;
    SymProtectEvt::SymProtectEvents_CProxyFactoryEx   m_ProxyFactoryLoader;
    CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManagerHelper;

    // Work thread and related events
    class ReloadWorkThread : public ccLib::CThread
    {
    public:
        ReloadWorkThread(ccLib::CEvent* pNotifyEvent);
        BOOL Create(ISymProtectReloadCallback* pCallback);
        int Run();
        void PostTerminate();

        // Hide the "normal" Create function
    private:
        BOOL Create(LPSECURITY_ATTRIBUTES pSecurityAttrs,
                    DWORD dwStackSize,
                    DWORD dwCreateFlags);

    private:
        ISymProtectReloadCallback* m_pEventCB;
        ccLib::CEvent m_ShutdownEvent;
        ccLib::CEvent* m_pNotifyEvent;
    } m_WorkThread;
};