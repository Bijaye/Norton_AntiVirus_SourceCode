////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ControlEventProviders.h : header file
//
#pragma once

#include "ccEventManagerHelper.h"
#include "ccProviderHelper.h"
#include "SymProtectEventsLoader.h"
#include "SymProtectEventsInterfacePrivate.h"

class CSymProtectControlProvider : public ccEvtMgr::CProviderHelper
{
public:
    CSymProtectControlProvider();
    virtual ~CSymProtectControlProvider();

private:
    CSymProtectControlProvider(const CSymProtectControlProvider&);
    CSymProtectControlProvider& operator=(const CSymProtectControlProvider&);

public:
    // Must call Create() before using any of the other functions
    bool Create();
    void Destroy();

    // Asks to stop (disable) SymProtect, this has global effect.
    ccEvtMgr::CError::ErrorType StopSymProtect();

    // Asks SymProtect to start.
    ccEvtMgr::CError::ErrorType StartSymProtect();

    // Sends a notice to SymProtect that the contents of the Manifests
    // folder has been updated and should be incorporated into the active
    // protection rules
    ccEvtMgr::CError::ErrorType RefreshManifests();

    // Retrieves the current status of SymProtect. SP_STATE_UNKNOWN is a
    // possible return value from this function.
    SymProtectEvt::CQueryStateEventEx::SP_QUERY_STATE GetSymProtectStatus();

    // Sends a "fake" SymProtect reload event to all subscribers. This
    // functionality should only be used by SymProtect itself.
    ccEvtMgr::CError::ErrorType SendUpdateEvent();

private:
    ccEvtMgr::CError::ErrorType SendSymProtectEvent(long nEventType);

private:
    ccEvtMgr::CProxyFactoryExPtr    m_pProxyFactory;
    ccEvtMgr::CEventFactoryEx2Ptr   m_pEventFactory;
    SymProtectEvt::SymProtectEvents_CEventFactoryEx2  m_EventFactoryLoader;
    SymProtectEvt::SymProtectEvents_CProxyFactoryEx   m_ProxyFactoryLoader;
    CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManagerHelper;
};
