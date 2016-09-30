/////////////////////////////////////////////////////////////////////////////
//
// CSymProtectControlHelperNAV.cpp : SymProtect event provider implementation
//
// PROPRIETARY/CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.  All rights reserved. 
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ccLib.h"
#include "ccProviderHelper.h"
#include "SymProtectControlHelperNAV.h"

// Event timeout after which the helper functions will return errors
static const int SYMPROTECT_EVENT_TIMEOUT_MS = 60000;
static const int SYMPROTECT_MAX_QUERIES = 30;

using namespace ccEvtMgr;
using namespace SymProtectEvt;

// SymStaticRefCount is deprecated... disable warning for clients by request
// TODO: dont use static ref count
#pragma warning(push)
#pragma warning(disable: 4996)

CSymProtectControlHelperNAV::CSymProtectControlHelperNAV()
{
}

CSymProtectControlHelperNAV::~CSymProtectControlHelperNAV()
{
    m_EventManagerHelper.Destroy();
}

bool CSymProtectControlHelperNAV::Create()
{
    // Load the event factory
    ccEvtMgr::CEventFactoryEx2Ptr pEventFactory;
    if( SYM_FAILED(m_EventFactoryLoader.CreateObject(&pEventFactory)) ||
        pEventFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectControlHelper::Create() - m_EventFactoryLoader.CreateObject() failed.\n"));
        return false;
    }

    // Load the proxy factory
    ccEvtMgr::CProxyFactoryExPtr pProxyFactory;
    if( SYM_FAILED(m_ProxyFactoryLoader.CreateObject(&pProxyFactory)) ||
        pProxyFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectControlHelper::Create() - m_ProxyFactoryLoader.CreateObject() failed.\n"));
        return false;
    }

    // Set up the event manager helper
    if( !m_EventManagerHelper.Create(pEventFactory, pProxyFactory) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::Create() - m_EventManagerHelper.Create() failed.\n"));
        return false;
    }

    return true;
}


bool CSymProtectControlHelperNAV::SetBBSystemEnabled(bool bEnable)
{
    CEventExPtr pEvent;
    m_EventManagerHelper.NewEvent(IBBSymProtectSetConfigEvent::TypeId,
                                  pEvent.m_p);
    if( !pEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelper::SetBBSystemEnabled() - m_EventManagerHelper.NewEvent(IBBSymProtectSetConfigEvent) failed.\n"));
        return false;
    }

    IBBSymProtectSetConfigEventQIPtr pSetEvent(pEvent);
    if( !pSetEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelper::SetBBSystemEnabled() - QI failed\n"));
        ASSERT(pSetEvent);
        return false;
    }

    pSetEvent->SetSymProtectComponentState(bEnable);
    CEventExPtr pReturnEvent;
    if( SendSPEvent(pSetEvent.m_p, &pReturnEvent) )
    {
        IBBSymProtectSetConfigEventQIPtr pReturnedSetEvent(pReturnEvent);
        ASSERT(pReturnedSetEvent);
        if( !pReturnedSetEvent )
        {
            CCTRACEE(_T("CSymProtectControlHelper::SetBBSystemEnabled() - QI on returned event failed\n"));
            return false;
        }

        ISymBBSettingsEvent::ErrorTypes eError;
        if( SYM_SUCCEEDED(pReturnedSetEvent->GetResult(eError)) &&
            eError != ISymBBSettingsEvent::UNRECOVERABLE_ERROR )
        {
            // Print a warning if there was any error recovery
            if( eError == ISymBBSettingsEvent::RECOVERED_FROM_ERROR )
            {
                CCTRACEW(_T("CSymProtectControlHelper::SetBBSystemEnabled() - Configuration error encountered and recovered"));
            }

            return true;
        }
        else
        {
            CCTRACEE(_T("CSymProtectControlHelper::SetBBSystemEnabled() - Event configuration unsuccessfully applied\n"));
            return false;
        }
    }
    else
    {
        CCTRACEE(_T("CSymProtectControlHelper::SetBBSystemEnabled() - SendSPEvent() failed\n"));
        return false;
    }
}


bool CSymProtectControlHelperNAV::SetComponentEnabled(bool bEnable)
{
    // When turning on SP just call SetBBSystemEnabled.  This will
    // enabled all the SP protections automatically.
    if( bEnable )
    {
        return SetBBSystemEnabled(bEnable);
    }

    // Diabling SP.  In this case we'll turn off the individual SP
    // protection categories.  This will diable SP, but allow the 
    // BB System to stay loaded if needed by other BB components (such as System Monitor)
    CEventExPtr pEvent;
    m_EventManagerHelper.NewEvent(IBBSymProtectSetConfigEvent2::TypeId,
                                  pEvent.m_p);
    if( !pEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelper::SetEnabled() - m_EventManagerHelper.NewEvent(IBBSymProtectSetConfigEvent2) failed.\n"));
        return false;
    }

    IBBSymProtectSetConfigEvent2QIPtr pSetEvent(pEvent);
    if( !pSetEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelper::SetEnabled() - QI failed\n"));
        ASSERT(pSetEvent);
        return false;
    }

    // Set all the protection levels to disabled
    pSetEvent->SetFileProtectionLevel( PROTECTION_DISABLED );
    pSetEvent->SetProcessProtectionLevel( PROTECTION_DISABLED );
    pSetEvent->SetRegistryProtectionLevel( PROTECTION_DISABLED );
    pSetEvent->SetNamedObjectProtectionLevel( PROTECTION_DISABLED );

    CEventExPtr pReturnEvent;
    if( SendSPEvent(pSetEvent.m_p, &pReturnEvent) )
    {
        IBBSymProtectSetConfigEvent2QIPtr pReturnedSetEvent(pReturnEvent);
        ASSERT(pReturnedSetEvent);
        if( !pReturnedSetEvent )
        {
            CCTRACEE(_T("CSymProtectControlHelper::SetEnabled() - QI on returned event failed\n"));
            return false;
        }

        ISymBBSettingsEvent::ErrorTypes eError;
        if( SYM_SUCCEEDED(pReturnedSetEvent->GetResult(eError)) &&
            eError != ISymBBSettingsEvent::UNRECOVERABLE_ERROR )
        {
            // Print a warning if there was any error recovery
            if( eError == ISymBBSettingsEvent::RECOVERED_FROM_ERROR )
            {
                CCTRACEW(_T("CSymProtectControlHelper::SetEnabled() - Configuration error encountered and recovered"));
            }

            return true;
        }
        else
        {
            CCTRACEE(_T("CSymProtectControlHelper::SetEnabled() - Event configuration unsuccessfully applied\n"));
            return false;
        }
    }
    else
    {
        CCTRACEE(_T("CSymProtectControlHelper::SetEnabled() - SendSPEvent() failed\n"));
        return false;
    }
}

bool CSymProtectControlHelperNAV::GetBBStatus(bool& bEnabled, bool& bError)
{
    // Initialize parameters with suitably paranoid values:
    bEnabled = false;
    bError = true;

    // Create a Query event if one hasn't already been created
    if( !m_pQueryEvent )
    {
        m_EventManagerHelper.NewEvent(IBBSymProtectQueryConfigEvent::TypeId,
                                      m_pQueryEvent.m_p);
        if( !m_pQueryEvent )
        {
            CCTRACEE(_T("CSymProtectControlHelper::GetBBStatus() - m_EventManagerHelper.NewEvent(IBBSymProtectQueryConfigEvent) failed.\n"));
            return false;
        }
    }

    // Send the event and wait to examine the result
    CEventExPtr pReturnEvent;
    if( !SendSPEvent(m_pQueryEvent, &pReturnEvent) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetBBStatus() - SendSPEvent(m_pQueryEvent) failed.\n"));
        return false;
    }

    // Cast the returned event back to a CQueryStateEventEx
    IBBSymProtectQueryConfigEventQIPtr pQuery = pReturnEvent;
    if( pQuery == NULL )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetBBStatus() - QI failed"));
        return false;
    }

    // Extract the state data
    SYMRESULT symRes = pQuery->GetSymProtectComponentState( bEnabled, bError );
    if( SYM_FAILED(symRes) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetBBStatus() - pQuery->GetSymProtectComponentState() failed (0x%08X)\n"), symRes);
        return false;
    }

    return true;
}


bool CSymProtectControlHelperNAV::GetStatus(bool& bEnabled, bool& bError)
{
    // Reset the parameters with suitably paranoid values:
    bEnabled = false;
    bError = true;

    // Create a Query event if one hasn't already been created
    if( !m_pQueryEvent2 )
    {
        m_EventManagerHelper.NewEvent(IBBSymProtectQueryConfigEvent2::TypeId,
                                      m_pQueryEvent2.m_p);
        if( !m_pQueryEvent2 )
        {
            CCTRACEE(_T("CSymProtectControlHelper::GetStatus() - m_EventManagerHelper.NewEvent(IBBSymProtectQueryConfigEvent2) failed.\n"));
            return false;
        }
    }

    // Send the event and wait to examine the result
    CEventExPtr pReturnEvent;
    if( !SendSPEvent(m_pQueryEvent2, &pReturnEvent) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetStatus() - SendSPEvent(m_pQueryEvent2) failed.\n"));
        return false;
    }

    // Cast the returned event back to a CQueryStateEventEx
    IBBSymProtectQueryConfigEvent2QIPtr pQuery = pReturnEvent;
    if( pQuery == NULL )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetStatus() - QI failed"));
        return false;
    }

    // Extract the BB System state data
    SYMRESULT symRes = pQuery->GetSymProtectComponentState( bEnabled, bError );
    if( SYM_FAILED(symRes) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetStatus() - pQuery->GetSymProtectComponentState() failed (0x%08X)\n"), symRes);
        return false;
    }

    // Is the BB System off
    if( !bEnabled )
    {
        // BB is off so SP is also off.  Just return true.
        return true;
    }

    // BB is on, so we need to query the individual protection levels to see if SP is on or off.
    SP_PROTECTION_LEVEL fileLevel; 
    symRes = pQuery->GetFileProtectionLevel(fileLevel);
    if( SYM_FAILED(symRes) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetStatus() - pQuery->GetFileProtectionLevel() failed (0x%08X)\n"), symRes);
        return false;
    }

    SP_PROTECTION_LEVEL procLevel; 
    symRes = pQuery->GetProcessProtectionLevel(procLevel);
    if( SYM_FAILED(symRes) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetStatus() - pQuery->GetProcessProtectionLevel() failed (0x%08X)\n"), symRes);
        return false;
    }

    SP_PROTECTION_LEVEL regLevel; 
    symRes = pQuery->GetRegistryProtectionLevel(regLevel);
    if( SYM_FAILED(symRes) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetStatus() - pQuery->GetRegistryProtectionLevel() failed (0x%08X)\n"), symRes);
        return false;
    }

    SP_PROTECTION_LEVEL objLevel; 
    symRes = pQuery->GetNamedObjectProtectionLevel(objLevel);
    if( SYM_FAILED(symRes) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::GetStatus() - pQuery->GetNamedObjectProtectionLevel() failed (0x%08X)\n"), symRes);
        return false;
    }

    // If all the protection levels are off then SP is off.
    if( fileLevel == PROTECTION_DISABLED &&
        procLevel == PROTECTION_DISABLED &&
        regLevel == PROTECTION_DISABLED  &&
        objLevel == PROTECTION_DISABLED )
    {
        // Set the SP state to disabled
        bEnabled = false;
    }

    return true;
}

bool CSymProtectControlHelperNAV::StopLegacySymProtect()
{
    // Make a new legacy stop event
    CEventExPtr pStopEvent;
    m_EventManagerHelper.NewEvent(CStopEventEx::TypeId, pStopEvent.m_p);
    if( !pStopEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelper::StopLegacySymProtect() - NewEvent(CStartEventEx::TypeId) failed\n"));
        return false;
    }

    // Make a new legacy query event
    CEventExPtr pQueryEvent;
    m_EventManagerHelper.NewEvent(CQueryStateEventEx::TypeId, pQueryEvent.m_p);
    if( !pQueryEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelper::StopLegacySymProtect() - NewEvent(CQueryStateEventEx) failed\n"));
        return false;
    }

    // Send the stop event
    if( !SendSPEvent(pStopEvent.m_p) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::StopLegacySymProtect() - Failed to send stop event\n"));
        return false;
    }

    // Query in a loop until the state becomes "Off" (or we hit the maximum
    // number of iterations)
    bool bAlreadySeenErrorOff = false;
    for( int i = 0; i < SYMPROTECT_MAX_QUERIES; ++i )
    {
        // Send a query event
        CEventExPtr pReturnedEvent;
        if( !SendSPEvent(pQueryEvent, &pReturnedEvent) )
        {
            CCTRACEE(_T("CSymProtectControlHelper::StopLegacySymProtect() - SendSPEvent(pQueryEvent) failed\n"));
            break;
        }

        // Convert the returned event back to the type we want
        CQueryStateEventExQIPtr pReturnedQueryEvent(pReturnedEvent);
        if( !pReturnedEvent )
        {
            CCTRACEE(_T("CSymProtectControlHelper::StopLegacySymProtect() - QI(pQueryEvent) failed\n"));
            ASSERT(pReturnedEvent != NULL);
            break;
        }

        // Extract data from the event
        CQueryStateEventEx::SP_QUERY_STATE eState = CQueryStateEventEx::SP_STATE_ERROR_UNKNOWN;
        if( SYM_FAILED(pReturnedQueryEvent->GetSymProtectState( eState )) )
        {
            CCTRACEE(_T("CSymProtectControlHelper::StopLegacySymProtect() - GetSymProtectState() failed\n"));
            break;
        }

        // Check if it's off. If so, return success
        if( CQueryStateEventEx::SP_STATE_OFF == eState ||
            (bAlreadySeenErrorOff && CQueryStateEventEx::SP_STATE_ERROR_OFF == eState) )
        {
            CCTRACEI(_T("CSymProtectControlHelper::StopLegacySymProtect() - SymProtect stopped (state=%d)\n"), eState);
            return true;
        }

        // The first time we see error_off, we want to give SymProtect a little
        // extra time to quit properly
        if( CQueryStateEventEx::SP_STATE_ERROR_OFF == eState )
            bAlreadySeenErrorOff = true;

        // Wait a little bit before querying again
        CCTRACEI(_T("CSymProtectControlHelper::StopLegacySymProtect() - SymProtect state is %d: Sleeping...\n"), eState);
        Sleep(1000);
    }

    // If we got here, the state never became "Off"
    CCTRACEE(_T("CSymProtectControlHelper::StopLegacySymProtect() - Component did not turn off in time\n"));
    return false;
}

// Core function to send a ccEvt event and wait for it to be canceled
bool CSymProtectControlHelperNAV::SendSPEvent(CEventExPtr pEvent, 
                                           CEventEx** ppReturnEvent)
{
    // Create a provider to send events
    CSymStaticRefCount<CProviderHelper> ProviderHelper;
    if( !ProviderHelper.Create(&m_EventManagerHelper, TRUE) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::SendSPEvent() - ProviderHelper.Create() failed.\n"));
        return false;
    }

    // Send the event and wait for it to return
    long nEventId = 0;
    CError::ErrorType res;
    CEventExPtr pReturnEventPlaceholder;
    res = ProviderHelper.CreateEvent(*pEvent, false, nEventId, NULL, 
                                     SYMPROTECT_EVENT_TIMEOUT_MS, TRUE, 
                                     pReturnEventPlaceholder.m_p);
    if( CError::eNoError != res )
    {
        CCTRACEE(_T("CSymProtectControlHelper::SendSPEvent() - ProviderHelper.CreateEvent() failed, eRes=%d\n"), res);
        ProviderHelper.Destroy();
        return false;
    }

    // If the caller wants the resulting event back...
    if( ppReturnEvent )
    {
        // Make an addref'd copy
        *ppReturnEvent = pReturnEventPlaceholder;
        (*ppReturnEvent)->AddRef();
    }

    // Destroy the provider helper (relying on the destructor can be destructive)
    ProviderHelper.Destroy();
    return true;
}

bool CSymProtectControlHelperNAV::AddNotificationFilter(const wchar_t* pszPath)
{
    CEventExPtr pEvent;
    m_EventManagerHelper.NewEvent(IBBSymProtectSetConfigEvent2::TypeId,
                                  pEvent.m_p);
    if( !pEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - m_EventManagerHelper.NewEvent(IBBSymProtectSetConfigEvent) failed.\n"));
        return false;
    }

    IBBSymProtectSetConfigEvent2QIPtr pSetEvent(pEvent);
    if( !pSetEvent )
    {
        CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - QI failed\n"));
        ASSERT(pSetEvent);
        return false;
    }

    // Add the filter
    pSetEvent->AddNotificationFilter(pszPath);

    CEventExPtr pReturnEvent;
    if( SendSPEvent(pSetEvent.m_p, &pReturnEvent) )
    {
        IBBSymProtectSetConfigEventQIPtr pReturnedSetEvent(pReturnEvent);
        ASSERT(pReturnedSetEvent);
        if( !pReturnedSetEvent )
        {
            CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - QI on returned event failed\n"));
            return false;
        }

        ISymBBSettingsEvent::ErrorTypes eError;
        if( SYM_SUCCEEDED(pReturnedSetEvent->GetResult(eError)) &&
            eError != ISymBBSettingsEvent::UNRECOVERABLE_ERROR )
        {
            // Print a warning if there was any error recovery
            if( eError == ISymBBSettingsEvent::RECOVERED_FROM_ERROR )
            {
                CCTRACEW(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - Configuration error encountered and recovered"));
            }

            return true;
        }
        else
        {
            CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - Event configuration unsuccessfully applied\n"));
            return false;
        }
    }
    else
    {
        CCTRACEE(_T("CSymProtectControlHelperNAV::AddNotificationFilter() - SendSPEvent() failed\n"));
        return false;
    }

}

#pragma warning(pop) // C4996


