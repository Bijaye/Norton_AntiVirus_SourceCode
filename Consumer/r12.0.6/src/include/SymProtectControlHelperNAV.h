/////////////////////////////////////////////////////////////////////////////
//
// CSymProtectControlHelperNAV.h : Hijacked NAV version of CSymProtectControlHelper
//
//  Used for additional NAV specific work.
//
// PROPRIETARY/CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.  All rights reserved. 
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "SymProtectEventsInterface.h"
#include "SymProtectEventsLoader.h"
#include "ccEventManagerHelper.h"

class CSymProtectControlHelperNAV
{
public:
    CSymProtectControlHelperNAV();
    ~CSymProtectControlHelperNAV();

    // Initialize the object
    bool Create();

    // Gets the current SP status. Errors return "false"/"true"
    bool GetStatus(bool& bEnabled, bool& bError);

    // Get the current BB System status.  Returns bEnabled = true
    // if the BB Service & Driver are loaded, false otherwise.
    bool GetBBStatus(bool& bEnabled, bool& bError);

    // Starts/Stops all SymProtect services.  When disbaling (bEnable == false)
    // this may or may not cause the BB Service/Driver to unload
    // depending on the states of other BB components (such as System Monitor)
    bool SetComponentEnabled(bool bEnable = true);

    // Starts/Stops the BB System (Service & Driver).  When diabling (bEnable == false)
    // the BB System will unload regardless of the state of it's components.
    bool SetBBSystemEnabled(bool bEnable = true);

    // Turns off any old version of SymProtect that is running, regardless
    // of version (it works with all versions 1.0+)
    // Most clients should use SetComponentEnabled(), but certain applications
    // (such as installers) may need to control older installations
    bool StopLegacySymProtect();

    // Add a notification filter to SymProtect
    //
    bool AddNotificationFilter(const wchar_t* pszPath);

    // Don't allow shallow copies
private:
    CSymProtectControlHelperNAV(const CSymProtectControlHelperNAV&);
    CSymProtectControlHelperNAV& operator=(const CSymProtectControlHelperNAV&);

    // Internal functions
protected:
    bool SendSPEvent(ccEvtMgr::CEventExPtr pEvent, 
                     ccEvtMgr::CEventEx** ppReturnEvent = NULL);

    // Private event control data
    SymProtectEvt::SymProtectEvents_CEventFactoryEx2  m_EventFactoryLoader;
    SymProtectEvt::SymProtectEvents_CProxyFactoryEx   m_ProxyFactoryLoader;
    
    // SymStaticRefCount is deprecated... disable warning for clients by request
    // TODO: dont use static ref count
    #pragma warning(push)
    #pragma warning(disable: 4996)
    CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManagerHelper;
    #pragma warning(pop)

    // Pre-formed event objects
    ccEvtMgr::CEventExPtr m_pQueryEvent;
    ccEvtMgr::CEventExPtr m_pQueryEvent2;
};
