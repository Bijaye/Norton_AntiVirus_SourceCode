////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "ceventprovider.h"
#include "ccSymDebugOutput.h"

using namespace ccEvtMgr;
using namespace SymProtectEvt;

//long g_DLLObjectCount = 0;
/*ccSym::CDebugOutput g_DebugOutput(_T("BBGTH"));
IMPLEMENT_CCTRACE(g_DebugOutput);
*/

CEventProvider::CEventProvider(void)
{
}

CEventProvider::~CEventProvider(void)
{
    Destroy();
}

bool CEventProvider::GetNewEvent(long nEventType, CEventExPtr& pEvent)
{
    if( m_EventManagerHelper.NewEvent(nEventType, pEvent.m_p) != CError::eNoError )
        return false;

    return true;
}

void CEventProvider::DeleteEvent(CEventExPtr& pEvent)
{
    m_EventManagerHelper.DeleteEvent(pEvent);
}

// Sends the event and checks that the application was successful
bool CEventProvider::SendConfigEvent(CEventExPtr pEvent)
{
    long nEventId = 0;
    CEventEx* pOutEvent;

    if( m_ProviderHelper.CreateEvent(*pEvent, false, 
                                     nEventId, NULL, 
                                     30000, TRUE, 
                                     pOutEvent) != CError::eNoError )
    {
        return false;
    }
    wchar_t * pszErrorData = NULL;
    size_t bufferSize = 0;
    ISymBBSettingsEventQIPtr (pOutEvent) -> GetErrorString(pszErrorData,bufferSize);
    pszErrorData = (wchar_t *)malloc(bufferSize * sizeof (wchar_t));
    ISymBBSettingsEventQIPtr(pOutEvent) -> GetErrorString(pszErrorData,bufferSize);

    wprintf(L"Printing error stack: %s\n", pszErrorData);

    ISymBBSettingsEventQIPtr pSettingsEvent(pEvent);
    if( pSettingsEvent == NULL )
        return false;

    ISymBBSettingsEvent::ErrorTypes eError;
    if( SYM_FAILED(pSettingsEvent->GetResult(eError)) )
        return false;

    HRESULT hr = pSettingsEvent->GetOperationResult();

    // Check that the setting was applied correctly
    if( ISymBBSettingsEvent::UNRECOVERABLE_ERROR == eError ||
        FAILED(hr) )
    {
        return false;
    }
    pEvent = pOutEvent;

    return true;
}

bool CEventProvider::Initialize(void)
{
    Destroy();

    // Load the event factory

    if( SYM_FAILED(m_EventFactoryLoader.CreateObject(&m_EventFactory)) ||
        m_EventFactory == NULL )
    {
		CCTRACEW(L"EventFactory failure \n");
        Destroy();
        return false;
    }

    // Load the proxy factory

    if( SYM_FAILED(m_ProxyFactoryLoader.CreateObject(&m_ProxyFactory)) ||
        m_ProxyFactory == NULL )
    {
		CCTRACEW(L"ProxyFactory failure \n");
        Destroy();
        return false;
    }

    // Set up the event manager helper

    if( !m_EventManagerHelper.Create(m_EventFactory, m_ProxyFactory) )
    {
		CCTRACEW(L"EventManagerHelper failure \n");
        Destroy();
        return false;
    }

    // Call the ProviderHelper's Create

    if( !m_ProviderHelper.Create(&m_EventManagerHelper, TRUE) )
    {
		CCTRACEW(L"ProviderHelper failure \n");
        Destroy();
        return false;
    }

    return true;
}

void CEventProvider::Destroy(void)
{
    m_ProviderHelper.Destroy();
    m_EventManagerHelper.Destroy();
    m_ProxyFactory.Release();
    m_EventFactory.Release();
}
