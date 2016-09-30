/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// AvisConfig.cpp : Implementation of CAvisConfig
#include "stdafx.h"
#include "qserver.h"
#include "AvisConfig.h"
#include "const.h"
#include "util.h"
#include "SymSaferRegistry.h"




/////////////////////////////////////////////////////////////////////////////
// CAvisConfig  


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CAvisConfig::GetValue
//
// Description   : Retrieve value from the registry.
//
// Return type   : STDMETHODIMP 
//
// Argument      : BSTR bstrFieldName
// Argument      : BSTR *pbstrValue
//
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CAvisConfig::GetValue( BSTR bstrFieldName, BSTR bstrKeyName, VARIANT *v )    
{                                       
USES_CONVERSION;

    HRESULT hr = S_OK;
    DWORD   dwSize = 0;
    DWORD   dwType;
	LPTSTR  pszRegKey = NULL;

    if (bstrFieldName == NULL)
		return E_INVALIDARG;
    if (bstrKeyName == NULL)
		return E_INVALIDARG;
	if( v == NULL )
        return E_INVALIDARG;
	
	pszRegKey = new TCHAR[ lstrlen(W2T( bstrKeyName ))+1] ;
    ZeroMemory( pszRegKey, lstrlen(W2T( bstrKeyName ))+1);
	lstrcpy(pszRegKey, W2T( bstrKeyName ));

    VariantInit( v );
    
    try
    {
        // OPEN MAIN REG KEY.
        CRegKey reg;
        hr = reg.Open( HKEY_LOCAL_MACHINE, pszRegKey, KEY_READ );
        if( hr != ERROR_SUCCESS )
        {
            // IF IT DOESN'T EXIST, CREATE IT
            hr = reg.Create( HKEY_LOCAL_MACHINE, pszRegKey, REG_NONE, REG_OPTION_NON_VOLATILE,
                                                                 KEY_ALL_ACCESS, NULL, NULL ); 
            if( hr != ERROR_SUCCESS )        
                return hr;
        }

        // Figure out what type of data we are dealing with.
        hr = SymSaferRegQueryValueEx( reg, W2T( bstrFieldName ), NULL, &dwType, NULL, &dwSize );
        if( hr == ERROR_SUCCESS )
            {
            switch( dwType )
                {
                case REG_DWORD:
                    {
                    // Get DWORD value.
                    if( ERROR_SUCCESS == reg.QueryDWORDValue( W2T( bstrFieldName), v->ulVal ) )
                        {
                        v->vt = VT_UI4;
                        }
                    else
                        {
                        // Value does not exist
                        hr = S_FALSE;
                        }
                    }
                    break;

                case REG_SZ:
                    {
                    // Get String value.
                    LPTSTR pszTemp = new TCHAR[ dwSize + 1 ] ;
                    ZeroMemory( pszTemp, dwSize + 1 );
                    if( ERROR_SUCCESS == reg.QueryStringValue( W2T( bstrFieldName), pszTemp, &dwSize ) )
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
                        // Value does not exist
                        hr = S_FALSE;
                        VariantInit( v );
                        }
                    delete [] pszTemp;
                    }
                    break;

                case REG_BINARY:
                    {
                    // Get binary value.
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
                            if( ERROR_SUCCESS == SymSaferRegQueryValueEx( reg, W2T(bstrFieldName), NULL, &dwType, p, &dwSize ) )
                                {
                                v->vt = VT_VARIANT | VT_ARRAY;
                                v->parray = psa;
                                }
                            else
                                {
                                // Value does not exist.
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

	delete pszRegKey;
    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CAvisConfig::SetValue
//
// Description   : Set a value in the registry.
//
// Return type   : STDMETHODIMP 
//
// Argument      : BSTR bstrFieldName
// Argument      : VARIANT v
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CAvisConfig::SetValue( BSTR bstrFieldName, BSTR bstrKeyName, 
                                       VARIANT v )
{
USES_CONVERSION;

    HRESULT hr = S_OK;
    LPTSTR szFieldName = W2T( bstrFieldName );
	LPTSTR  pszRegKey = NULL;

    // 
    // Valdate input
    // 
  
	if( v.vt != VT_BSTR && 
        v.vt != VT_UI4 && 
        v.vt != ( VT_UI1 | VT_ARRAY ) )
        {
        return E_INVALIDARG;
        }

	if (bstrFieldName == NULL || bstrKeyName == NULL)
        {
        return E_INVALIDARG;
        }

	// set up key  if the key passed in is null then use default otherwise use the passed in one.
	pszRegKey = new TCHAR[ lstrlen(W2T( bstrKeyName))+1] ;
    ZeroMemory( pszRegKey, lstrlen(W2T( bstrKeyName))+1);
	lstrcpy(pszRegKey, W2T( bstrKeyName));

    try
        {
        // 
        // Open main reg key.
        // 
        CRegKey reg;
        if( ERROR_SUCCESS != reg.Open( HKEY_LOCAL_MACHINE, pszRegKey, KEY_READ | KEY_WRITE ) )
            return E_FAIL;

        switch( v.vt )
            {
            case VT_BSTR:
                {
                // 
                // String
                // 
                USES_CONVERSION;
                if( ERROR_SUCCESS != reg.SetStringValue( szFieldName , W2T( v.bstrVal ) ) )
                    hr = E_FAIL;
                }
                break;

            case VT_UI4:
                {
                // 
                // DWORD
                // 
                if( ERROR_SUCCESS != reg.SetDWORDValue( szFieldName, v.ulVal) )
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

	delete pszRegKey;

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
// Function name: CAvisConfig::Detach
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CAvisConfig::Detach()
{
    HRESULT hr = S_OK;

    if( !m_bAttached )
        return hr;

    // 
    // Set this so we don't do this again.
    // 
    m_bAttached = FALSE;

    // If changes in configuration were made, fire off event.
//    if( m_bChanged )
//        PulseEvent( _Module.m_hConfigEvent );

    // Decrement global count of config objects
//    InterlockedDecrement( &_Module.m_iConfigCount );


//    fPrintString("Remote console detached and Config object destroyed.");

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CAvisConfig::FinalRelease
//
// Description   : Called as the object is being destoryed.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CAvisConfig::FinalRelease()
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
