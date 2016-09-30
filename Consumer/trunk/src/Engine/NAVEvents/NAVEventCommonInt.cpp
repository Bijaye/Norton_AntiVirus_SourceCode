////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// EventFSSInt.cpp : Implementation of CEventFSSInt
#include "stdafx.h"

#include "NAVEvents.h"

#include "NAVEventCommonInt.h"

using namespace ccEvtMgr;
/////////////////////////////////////////////////////////////////////////////
// CNAVEventCommonInt

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

STDMETHODIMP CNAVEventCommonInt::GetPropertyBSTR(long lPropIndex, BSTR* pbstrValue)
{
    std::wstring strTemp; 

	if ( !m_C.props.GetData ( lPropIndex, strTemp ))
        return E_FAIL;

    _bstr_t bstrTemp ( strTemp.c_str() );
    *pbstrValue = bstrTemp.copy ();

	return S_OK;
}

STDMETHODIMP CNAVEventCommonInt::SetPropertyBSTR(long lPropIndex, BSTR* pbstrValue)
{
	try
    {
        _bstr_t bstrTemp ( *pbstrValue );
        m_C.props.SetData ( lPropIndex, (wchar_t*) bstrTemp );

    	return S_OK;
    }
    catch ( _com_error e )
    {
        return e.Error ();
    }
}

STDMETHODIMP CNAVEventCommonInt::GetPropertyLong(long lPropIndex, long* plValue)
{
	if ( !m_C.props.GetData ( lPropIndex, *plValue ))
        return E_FAIL;

    return S_OK;
}

STDMETHODIMP CNAVEventCommonInt::SetPropertyLong(long lPropIndex, long lValue)
{
    m_C.props.SetData ( lPropIndex, lValue );

	return S_OK;
}