// CStatusEventBridge.cpp : Implementation of CCStatusEventBridge
#include "stdafx.h"
#include "Status.h"
#include "StatusEventBridge.h"

/////////////////////////////////////////////////////////////////////////////
// CCStatusEventBridge
//
// COM Bridge Object.
//
HRESULT CStatusEventBridge::SetCallback(CNAVStatus* pNAVStatus)
{
    if ( pNAVStatus )
        m_spNAVStatus = pNAVStatus;
    return S_OK;
}

STDMETHODIMP CStatusEventBridge::StatusChangedEvent()
{
	if ( m_spNAVStatus )
    {
        m_spNAVStatus -> Fire_StatusChangedEvent ();

	    return S_OK;
    }
    return E_FAIL;
}
