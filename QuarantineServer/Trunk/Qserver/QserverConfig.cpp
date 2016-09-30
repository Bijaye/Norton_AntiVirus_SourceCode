/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QserverConfig.cpp : Implementation of CQserverConfig
#include "stdafx.h"
#include "Qserver.h"
#include "QserverConfig.h"
#include "const.h"
#include "SymSaferRegistry.h"

extern	bool CheckDirectory(const _TCHAR* const CpCcDirectory);

/////////////////////////////////////////////////////////////////////////////
// CQserverConfig


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQserverConfig::GetStringValue
//
// Description   : Retrieve value from the registry.
//
// Return type   : STDMETHODIMP 
//
// Argument      : BSTR bstrFieldName
// Argument      : BSTR *pbstrValue
//
///////////////////////////////////////////////////////////////////////////////
// 1/25/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQserverConfig::GetValue( BSTR bstrFieldName, 
                                       VARIANT *v )    
{
USES_CONVERSION;

    HRESULT hr = S_OK;
    DWORD dwSize = 0;
    DWORD dwType;

    if( v == NULL )
        return E_INVALIDARG;

	_Module.m_dwTicktime = GetTickCount();

    VariantInit( v );
    
    try
        {
        // 
        // Open main reg key.
        // 
        CRegKey reg;
        if( ERROR_SUCCESS != reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, KEY_READ ) )
            return E_FAIL;

        // 
        // Figure out what type of data we are dealing with.
        // 
        if( ERROR_SUCCESS == SymSaferRegQueryValueEx( reg, bstrFieldName, NULL, &dwType, NULL, &dwSize ) )
            {
            switch( dwType )
                {
                case REG_DWORD:
                    {
                    // 
                    // Get DWORD value.
                    // 
                    if( ERROR_SUCCESS == reg.QueryDWORDValue( W2T( bstrFieldName ),v->ulVal ) )
                        {
                        v->vt = VT_UI4;
                        }
                    else
                        {
                        // 
                        // Value does not exist
                        // 
                        hr = S_FALSE;
                        }
                    }
                    break;

                case REG_SZ:
                    {
                    // 
                    // Get String value.
                    // 
                    LPTSTR pszTemp = new TCHAR[ dwSize + 1 ] ;
                    ZeroMemory( pszTemp, dwSize + 1 );
                    if( ERROR_SUCCESS == reg.QueryStringValue( W2T( bstrFieldName ), pszTemp,  &dwSize ) )
                        {
                        v->vt = VT_BSTR;
                        v->bstrVal = SysAllocStringLen( T2W( pszTemp ), lstrlen( pszTemp ) + 1 );
                        if( v->bstrVal == NULL )
                            {
                            VariantInit( v );
                            hr = E_OUTOFMEMORY;
                            }
                        }
                    else
                        {
                        // 
                        // Value does not exist
                        // 
                        hr = S_FALSE;
                        VariantInit( v );
                        }
                    delete [] pszTemp;
                    }
                    break;

                case REG_BINARY:
                    {
                    // 
                    // Get binary value.
                    // 
                    SAFEARRAY* psa;
                    SAFEARRAYBOUND bounds;
                    LPBYTE p;
                    bounds.lLbound = 0;
                    bounds.cElements = dwSize;
                    psa = SafeArrayCreate( VT_UI1, 1, &bounds );
                    if( psa == NULL )
                        hr = E_OUTOFMEMORY;
                    else
                        {
                        hr = SafeArrayAccessData( psa, (LPVOID*)&p );
                        if( SUCCEEDED( hr ) )
                            {
                            if( ERROR_SUCCESS == SymSaferRegQueryValueEx( reg, bstrFieldName, NULL, &dwType, p, &dwSize ) )
                                {
                                v->vt = VT_VARIANT | VT_ARRAY;
                                v->parray = psa;
                                }
                            else
                                {
                                // 
                                // Value does not exist.
                                // 
                                hr= S_FALSE;
                                }
                            SafeArrayUnaccessData( psa );
                            }
                        else
                            {
                            hr = E_FAIL;
                            }
                        }
                    }
                    break;

                default:
                    hr = E_INVALIDARG;
                }
            }
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }

    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQserverConfig::SetValue
//
// Description   : Set a value in the registry.
//
// Return type   : STDMETHODIMP 
//
// Argument      : BSTR bstrFieldName
// Argument      : VARIANT v
//
///////////////////////////////////////////////////////////////////////////////
// 2/17/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQserverConfig::SetValue( BSTR bstrFieldName,
                                       VARIANT v )
{
USES_CONVERSION;

    HRESULT hr = S_OK;
    LPTSTR szFieldName = W2T( bstrFieldName );
	_Module.m_dwTicktime = GetTickCount();

    // 
    // Valdate input
    // 
    if( v.vt != VT_BSTR && 
        v.vt != VT_UI4 && 
        v.vt != ( VT_UI1 | VT_ARRAY ) )
        {
        return E_INVALIDARG;
        }

    try
        {
        // 
        // Open main reg key.
        // 
        CRegKey reg;
        if( ERROR_SUCCESS != reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, KEY_READ | KEY_WRITE ) )
            return E_FAIL;

        switch( v.vt )
            {
            case VT_BSTR:
                {
                // 
                // String
                // 
                USES_CONVERSION;
				if(_tcsicmp(szFieldName,REGVALUE_QUARANTINE_FOLDER) == 0)
				{
					// 
					// Q: Does this directory exist?
					// 
					DWORD dwAttr = GetFileAttributes( W2T(v.bstrVal) );
					if( dwAttr == 0xFFFFFFFF )
					{
						DWORD dwE = GetLastError();
						if (dwE == 0x05)
						{
							hr = E_INVALIDARG;
							break;
						}
						// 
						// No it doesn't, so try creating it
						// 
						if( false == CheckDirectory( W2T(v.bstrVal) ) )
						{
							hr = E_INVALIDARG;
							break;
						}
					}
				}

                if( ERROR_SUCCESS != reg.SetStringValue( szFieldName, W2T( v.bstrVal )) )
                    hr = E_FAIL;
                }
                break;

            case VT_UI4:
                {
                // 
                // DWORD
                // 
                if( ERROR_SUCCESS != reg.SetDWORDValue( szFieldName, v.ulVal ) )
                    hr = E_FAIL;
                }
                break;

            default:
                {
                // 
                // Binary.
                // 
                LPBYTE p;
                hr = SafeArrayAccessData( v.parray, (LPVOID*) &p );
                if( SUCCEEDED( hr ) )
                    {
                    LONG lSize = 0;
                    SafeArrayGetUBound( v.parray, 0, &lSize );
                    if( ERROR_SUCCESS != RegSetValueEx( reg, szFieldName, NULL, REG_BINARY, p, lSize ) )
                        {
                        SafeArrayUnaccessData( v.parray );
                        hr = E_FAIL;
                        }
                    }
                }
                break;
            }
        }
    catch(...)
        {
        hr = E_FAIL;
        }

    if( FAILED( hr ) )
        return hr;

    // 
    // All is well if we get here, so set changed bit.
    // 
    m_bChanged = TRUE;

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQserverConfig::Detach
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
//
///////////////////////////////////////////////////////////////////////////////
// 4/6/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQserverConfig::Detach()
{
    HRESULT hr = S_OK;

	_Module.m_dwTicktime = 0;
    if( !m_bAttached )
        return hr;

    // 
    // Set this so we don't do this again.
    // 
    m_bAttached = FALSE;

    // 
    // If changes in configuration were made, fire off event.
    // 
    if( m_bChanged )
        PulseEvent( _Module.m_hConfigEvent );

    // 
    // Decrement global count of config objects
    // 
	if (_Module.m_iConfigCount > 0)
		InterlockedDecrement( &_Module.m_iConfigCount );

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQserverConfig::FinalRelease
//
// Description   : Called as the object is being destoryed.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 1/25/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQserverConfig::FinalRelease()
{

    // 
    // Make sure our updates occur.
    // 
    Detach();    

    // 
    // Call base class implementation.
    // 
    CComObjectRoot::FinalRelease();
}
