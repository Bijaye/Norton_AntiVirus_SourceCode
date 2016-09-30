// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.


#include "stdafx.h"
#include "tlsimpersonate.h"
#include "impersonate.h"
#include "SavAssert.h"

unsigned int CTlsImpersonate::m_uiTlsSlot = TLS_OUT_OF_INDEXES;

//constructor
CTlsImpersonate::CTlsImpersonate(bool bClearTlsSlot):m_bClearTlsSlot(bClearTlsSlot),
                                                     m_bTlsSet(false)
{
}

//destructor
CTlsImpersonate::~CTlsImpersonate()
{
    SAVASSERT( TLS_OUT_OF_INDEXES != m_uiTlsSlot );
    if( true == m_bClearTlsSlot && true == m_bTlsSet )
        ::TlsSetValue(m_uiTlsSlot, NULL);
}

//get the impersonate member so the privileges can be toggled
CImpersonate* CTlsImpersonate::GetTlsValue()
{
    SAVASSERT( TLS_OUT_OF_INDEXES != m_uiTlsSlot );
    return reinterpret_cast<CImpersonate*>(::TlsGetValue(m_uiTlsSlot));
}

//sets the TLS value for the impersonation class
bool CTlsImpersonate::SetTlsValue(CImpersonate& oImpersonate)
{
    SAVASSERT( TLS_OUT_OF_INDEXES != m_uiTlsSlot );
	// The following verify guards against possible nested calls to set the impersonation
	// on the same thread which would lead to prematurely calling RevertToSelf.
	// If for some reason, you need to over-write a previous call to CTlsImpersonate::SetTlsValue
	// then call CTlsImpersonate::ClearTlsValue first which will then allow you to call
	// SetTlsValue without asserting.
	SAVVERIFY(NULL == ::TlsGetValue(m_uiTlsSlot));
    return ::TlsSetValue(m_uiTlsSlot, (LPVOID)&oImpersonate) ? true, m_bTlsSet = true : false;
}

bool CTlsImpersonate::ClearTlsValue(void)
{
    SAVASSERT( TLS_OUT_OF_INDEXES != m_uiTlsSlot );
    return ::TlsSetValue(m_uiTlsSlot, NULL) ? true, m_bTlsSet = true : false;
}

bool CTlsImpersonate::CreateSlot()
{
    m_uiTlsSlot = ::TlsAlloc();
    //make sure we have a valid slot
    SAVASSERT( TLS_OUT_OF_INDEXES != m_uiTlsSlot);
    if( TLS_OUT_OF_INDEXES == m_uiTlsSlot )
        return false;

    return true;
}

void CTlsImpersonate::DestroySlot()
{
    ::TlsFree(m_uiTlsSlot);
}
