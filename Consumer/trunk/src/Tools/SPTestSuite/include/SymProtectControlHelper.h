////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//
// SymProtectControlHelper.h : Shared SymProtect event provider for control
//

#pragma once

#include "SymProtectEventsInterface.h"
#include "SymProtectEventsLoader.h"
#include "ccEventManagerHelper.h"

class CSymProtectControlHelper
{
public:
    CSymProtectControlHelper();
    ~CSymProtectControlHelper();

    // Initialize the object
    bool Create();

    // Gets the current status. Errors return "false"/"true"
    bool GetStatus(bool& bEnabled, bool& bError);

    // Starts/Stops all SymProtect services
    bool SetComponentEnabled(bool bEnable = true);

    // Turns off any old version of SymProtect that is running, regardless
    // of version (it works with all versions 1.0+)
    // Most clients should use SetComponentEnabled(), but certain applications
    // (such as installers) may need to control older installations
    bool StopLegacySymProtect();

    // Internal functions
protected:
    bool SendSPEvent(ccEvtMgr::CEventExPtr pEvent, 
                     ccEvtMgr::CEventEx** ppReturnEvent = NULL);

    // Don't allow shallow copies
private:
    CSymProtectControlHelper(const CSymProtectControlHelper&);
    CSymProtectControlHelper& operator=(const CSymProtectControlHelper&);

    // Private event control data
private:
    SymProtectEvt::SymProtectEvents_CEventFactoryEx2  m_EventFactoryLoader;
    SymProtectEvt::SymProtectEvents_CProxyFactoryEx   m_ProxyFactoryLoader;
    CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManagerHelper;

    // Pre-formed event objects
    ccEvtMgr::CEventExPtr m_pQueryEvent;
};
