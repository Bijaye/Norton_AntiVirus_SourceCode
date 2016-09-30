////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "cconfigstates.h"

#include "SymProtectEventsInterface.h"

using namespace ccEvtMgr;
using namespace SymProtectEvt;

CConfigStates::CConfigStates()
{
}

CConfigStates::~CConfigStates()
{
}

bool CConfigStates::Initialize()
{
    return m_EventProvider.Initialize();
}

bool CConfigStates::SetSymProtectEnabled(bool bEnabled)
{
    CEventExPtr pNewEvent;
    if( !m_EventProvider.GetNewEvent(IBBSymProtectSetConfigEvent::TypeId, pNewEvent) )
        return false;

    IBBSymProtectSetConfigEventQIPtr pEvent(pNewEvent);
    if( pEvent == NULL )
        return false;

    if( SYM_FAILED(pEvent->SetSymProtectComponentState(bEnabled)) )
        return false;

    return m_EventProvider.SendConfigEvent(CEventExQIPtr(pEvent));
}

bool CConfigStates::SetFileProtection(bool bEnabled, bool bLogOnly)
{
    if( !bEnabled && bLogOnly )
        return false;

    CEventExPtr pNewEvent;
    if( !m_EventProvider.GetNewEvent(IBBSymProtectSetConfigEvent::TypeId, pNewEvent) )
        return false;

    IBBSymProtectSetConfigEventQIPtr pEvent(pNewEvent);
    if( pEvent == NULL )
        return false;

    SymProtectEvt::SP_PROTECTION_LEVEL eLevel;

    if( !bEnabled )
        eLevel = PROTECTION_DISABLED;
    else if( !bLogOnly )
        eLevel = PROTECTION_ENABLED;
    else
        eLevel = PROTECTION_LOG_ONLY;

    if( SYM_FAILED(pEvent->SetFileProtectionLevel(eLevel)) )
        return false;

    return m_EventProvider.SendConfigEvent(CEventExQIPtr(pEvent));
}

bool CConfigStates::SetRegistryProtection(bool bEnabled, bool bLogOnly)
{
    if( !bEnabled && bLogOnly )
        return false;

    CEventExPtr pNewEvent;
    if( !m_EventProvider.GetNewEvent(IBBSymProtectSetConfigEvent::TypeId, pNewEvent) )
        return false;

    IBBSymProtectSetConfigEventQIPtr pEvent(pNewEvent);
    if( pEvent == NULL )
        return false;

    SymProtectEvt::SP_PROTECTION_LEVEL eLevel;

    if( !bEnabled )
        eLevel = PROTECTION_DISABLED;
    else if( !bLogOnly )
        eLevel = PROTECTION_ENABLED;
    else
        eLevel = PROTECTION_LOG_ONLY;

    if( SYM_FAILED(pEvent->SetRegistryProtectionLevel(eLevel)) )
        return false;

    return m_EventProvider.SendConfigEvent(CEventExQIPtr(pEvent));
}

bool CConfigStates::SetProcessProtection(bool bEnabled, bool bLogOnly)
{
    if( !bEnabled && bLogOnly )
        return false;

    CEventExPtr pNewEvent;
    if( !m_EventProvider.GetNewEvent(IBBSymProtectSetConfigEvent::TypeId, pNewEvent) )
        return false;

    IBBSymProtectSetConfigEventQIPtr pEvent(pNewEvent);
    if( pEvent == NULL )
        return false;

    SymProtectEvt::SP_PROTECTION_LEVEL eLevel;

    if( !bEnabled )
        eLevel = PROTECTION_DISABLED;
    else if( !bLogOnly )
        eLevel = PROTECTION_ENABLED;
    else
        eLevel = PROTECTION_LOG_ONLY;

    if( SYM_FAILED(pEvent->SetProcessProtectionLevel(eLevel)) )
        return false;

    return m_EventProvider.SendConfigEvent(CEventExQIPtr(pEvent));
}
