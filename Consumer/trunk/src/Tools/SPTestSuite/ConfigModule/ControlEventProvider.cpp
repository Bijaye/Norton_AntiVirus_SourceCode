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

#include "ControlEventProvider.h"
#include "SymProtectEventsInterface.h"

using namespace ccEvtMgr;
using namespace SymProtectEvt;

#define SYMPROTECT_EVENT_TIMEOUT_MS 30000

CSymProtectControlProvider::CSymProtectControlProvider()
{
}

CSymProtectControlProvider::~CSymProtectControlProvider()
{
    Destroy();
}

bool CSymProtectControlProvider::Create()
{
    Destroy();

    // Load the event factory

    if( SYM_FAILED(m_EventFactoryLoader.CreateObject(&m_pEventFactory)) ||
        m_pEventFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectControlProvider::Create() - m_EventFactoryLoader.CreateObject() failed.\n"));
        Destroy();
        return false;
    }

    // Load the proxy factory

    if( SYM_FAILED(m_ProxyFactoryLoader.CreateObject(&m_pProxyFactory)) ||
        m_pProxyFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectControlProvider::Create() - m_ProxyFactoryLoader.CreateObject() failed.\n"));
        Destroy();
        return false;
    }

    // Set up the event manager helper

    if( !m_EventManagerHelper.Create(m_pEventFactory, m_pProxyFactory) )
    {
        CCTRACEE(_T("CSymProtectControlProvider::Create() - m_EventManagerHelper.Create() failed.\n"));
        Destroy();
        return false;
    }

    // Call the ProviderHelper's Create

    if( !CProviderHelper::Create(&m_EventManagerHelper, TRUE) )
    {
        CCTRACEE(_T("CSymProtectControlProvider::Create() - CProviderHelper::Create() failed.\n"));
        Destroy();
        return false;
    }

    return true;
}

void CSymProtectControlProvider::Destroy()
{
    // Undo in reverse order...

    CProviderHelper::Destroy();

    m_EventManagerHelper.Destroy();

    m_pProxyFactory.Release();

    m_pEventFactory.Release();
}

// Since this function depends on a round-trip to get the status,
// it's much more complicated than the other event provider functions
CQueryStateEventEx::SP_QUERY_STATE
CSymProtectControlProvider::GetSymProtectStatus()
{
    CQueryStateEventEx::SP_QUERY_STATE curState = 
        CQueryStateEventEx::SP_STATE_ERROR_UNKNOWN;

    // Check that we've been properly initialized
    if( m_pEventFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectControlProvider::GetSymProtectStatus() - m_pEventFactory == NULL\n"));
        return CQueryStateEventEx::SP_STATE_ERROR_UNKNOWN;
    }

    // Create a new query state event
    CEventEx* pEvent = NULL;
    CEventEx* pOutEvent = NULL;
    CQueryStateEventExQIPtr pStateEvent;
    long nEventId;
    if( m_EventManagerHelper.NewEvent(CQueryStateEventEx::TypeId, pEvent)
        != CError::eNoError || pEvent == NULL )
    {
        CCTRACEE(_T("CSymProtectControlProvider::GetSymProtectStatus() - m_EventManagerHelper.NewEvent() != eNoError\n"));
        goto exit;
    }

    // Send out the event. Not broadcast!
    if( CProviderHelper::CreateEvent(*pEvent, false, nEventId, NULL, 
        SYMPROTECT_EVENT_TIMEOUT_MS, TRUE, pOutEvent) != CError::eNoError ||
        pOutEvent == NULL )
    {
        CCTRACEE(_T("CSymProtectControlProvider::GetSymProtectStatus() - EvtProvider.CreateEvent() != eNoError\n"));
        goto exit;
    }

    // Check out the returned query event
    pStateEvent = pOutEvent;
    if( pStateEvent == NULL )
    {
        CCTRACEE(_T("CSymProtectControlProvider::GetSymProtectStatus() - FAILED: pStateEvent = pOutEvent"));
        goto exit;
    }

    // Retrieve the current state
    if( SYM_FAILED(pStateEvent->GetSymProtectState(curState)) )
    {
        CCTRACEE(_T("CSymProtectControlProvider::GetSymProtectStatus() - pStateEvent->GetSymProtectState() failed"));
        curState = CQueryStateEventEx::SP_STATE_ERROR_UNKNOWN;
        goto exit;
    }

exit:
    if( pOutEvent )
        m_EventManagerHelper.DeleteEvent(pOutEvent);

    if( pEvent )
        m_EventManagerHelper.DeleteEvent(pEvent);

    return curState;
}


CError::ErrorType CSymProtectControlProvider::StartSymProtect()
{
    // We don't expect a reply
    return SendSymProtectEvent(CStartEventEx::TypeId);
}

CError::ErrorType CSymProtectControlProvider::StopSymProtect()
{
    return SendSymProtectEvent(CStopEventEx::TypeId);
}

CError::ErrorType CSymProtectControlProvider::RefreshManifests()
{
    return SendSymProtectEvent(CManifestChangeEventEx::TypeId);
}

CError::ErrorType CSymProtectControlProvider::SendUpdateEvent()
{
    return SendSymProtectEvent(CReloadDllEventEx::TypeId);
}

CError::ErrorType 
CSymProtectControlProvider::SendSymProtectEvent(long nEventType)
{
    CEventEx* pEvent;
    CEventEx* pOutEvent = NULL;
    long nEventId;
    ccEvtMgr::CError::ErrorType ret;

    // Validate
    if( m_pEventFactory == NULL )
    {
        CCTRACEE(_T("CSymProtectControlProvider::SendSymProtectEvent() - m_pEventFactory == NULL\n"));
        ASSERT(FALSE);
        return CError::eFatalError;
    }

    ret = m_EventManagerHelper.NewEvent(nEventType, pEvent);
    if( ret != CError::eNoError || pEvent == NULL )
        return CError::eFatalError;

    // Can't send the event as a broadcast event because we're relying on
    // synchronous delivery
    ret = CProviderHelper::CreateEvent(*pEvent, false, nEventId, NULL, 
                                       SYMPROTECT_EVENT_TIMEOUT_MS, TRUE,
                                       pOutEvent);

    // Ignore the output:
    if( pOutEvent )
        m_EventManagerHelper.DeleteEvent(pOutEvent);

    // Free the original event
    m_EventManagerHelper.DeleteEvent(pEvent);

    return ret;
}