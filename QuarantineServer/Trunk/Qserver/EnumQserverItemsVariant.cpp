/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// EnumQserverItemsVariant.cpp : Implementation of CEnumQserverItemsVariant
#include "stdafx.h"
#include "safearray.h"
#include "Qserver.h"
#include "EnumQserverItemsVariant.h"
#include "qsfields.h"
//#include "safearray.h"




/////////////////////////////////////////////////////////////////////////////
// CEnumQserverItemsVariant


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::CEnumQserverItemsVariant
//
// Description   : Constructor
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CEnumQserverItemsVariant::CEnumQserverItemsVariant() :
m_pEnumerator( NULL ),
m_pList( NULL ),
m_iArraySize( 0 )                 
{
    ZeroMemory( m_szQuarantineDir, sizeof( TCHAR ) );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::CEnumQserverItemsVariant
//
// Description   : Constructor
//
// Argument      : LPCTSTR szQuaraninePath
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CEnumQserverItemsVariant::CEnumQserverItemsVariant( LPCTSTR szQuarantinePath ) :
m_pEnumerator( NULL ),
m_pList( NULL ),
m_iArraySize( 0 )
{
    // 
    // Save off location of quarantine folder
    // 
    lstrcpyn( m_szQuarantineDir, szQuarantinePath, MAX_PATH );

    // 
    // Create enumerator helper object
    // 
    try
        {
        m_pEnumerator = new CEnumerator( m_szQuarantineDir );
        m_pEnumerator->Enumerate();
        }
    catch(...)
        {
        }
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::Next
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : ULONG celt
// Argument      : VARIANT *aQserverItems
// Argument      : ULONG* pceltFetched
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::Next( ULONG celt, 
                                             VARIANT *aQserverItems, 
                                             ULONG* pceltFetched )
{
    ULONG ulNumReturned = 0;
         
    // 
    // Make sure caller knows what he is doing.
    // 
    if( pceltFetched == NULL && celt != 1 )
        return S_FALSE;
    else
        *pceltFetched = 0;

    // 
    // Make sure pointer is good
    // 
    if( aQserverItems == NULL )
        return E_POINTER;

    // 
    // Fetch the correct number of items.
    // 
    ULONG total = celt;
    TCHAR szItemName[ MAX_PATH ];
    HRESULT hr = S_OK;
    
    while( celt > 0 )
        {
        if( m_pEnumerator->GetNext( szItemName ) )
            {
            hr = GetVariantFields( szItemName, &aQserverItems[ total - celt ] ); 
            if( SUCCEEDED( hr ) )
                ulNumReturned++;
            else
                break;
            }
        else 
            break;
        
        celt--;
        }

    if( pceltFetched != NULL )
        *pceltFetched = ulNumReturned;

    if( ulNumReturned == 0 )
        return S_FALSE;

    // 
    // Success!
    // 
    return hr;

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::Skip
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : ULONG celt
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::Skip( ULONG celt )
{
    // 
    // Make sure all is well
    // 
    if( m_pEnumerator == NULL )
        return E_UNEXPECTED;

    // 
    // Skip down the list.
    // 
    TCHAR szItem[ MAX_PATH ];;
    while( celt > 0 && m_pEnumerator->GetNext( szItem ) )
        {
        celt --;
        }

    return S_OK;    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::Reset
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::Reset()
{
    HRESULT hr = S_OK;

    // 
    // Make sure all is well
    // 
    if( m_pEnumerator == NULL )
        return E_UNEXPECTED;

    // 
    // Remove old enumerator.
    // 
    delete m_pEnumerator;
    m_pEnumerator = NULL;

    // 
    // Create enumerator helper object
    //
    try
        {
        m_pEnumerator = new CEnumerator( m_szQuarantineDir );
        if( m_pEnumerator->Enumerate() == FALSE )
            hr = E_FAIL;
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::Clone
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : IEnumQserverItemsVariant** ppenum
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::Clone( IEnumQserverItemsVariant** ppenum )
{
    HRESULT hr = E_NOTIMPL;
    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::Initialize
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::Initialize( BSTR strQuarantineFolder,
					                               VARIANT vArray )
{
USES_CONVERSION;

    HRESULT hr = S_OK;    
    VARIANT v;
    _safearray_t< 2, VARIANT, Variant> sa;

    // 
    // Validate input
    // 
    if( vArray.vt != VT_ARRAY )
        return E_INVALIDARG;

    // 
    // Save off location of quarantine folder
    // 
    lstrcpyn( m_szQuarantineDir, W2T( strQuarantineFolder ), MAX_PATH );

    // 
    // Make sure that the array has the correct shape.  
    // 
    sa.Attach( vArray.parray );    
    if( 2 != sa.GetDim() )
        return E_INVALIDARG;

    try
        {
        // 
        // Enumerate quarantine folder.
        // 
        m_pEnumerator = new CEnumerator( m_szQuarantineDir );
        m_pEnumerator->Enumerate();
        
        // 
        // Allocate internal array.
        // 
        m_pList = new DATAFIELD[sa.GetLength( 1 )]; 

        // 
        // Add elements to our internal array.
        // 
        for( LONG i = 0; i < sa.GetLength(1) ; i++ )
            {
            // 
            // Fetch name of item
            // 
            v = sa( i, 0 );

            // 
            // Make sure this is a BSTR
            // 
            if( v.vt != VT_BSTR )
                {
                hr = E_INVALIDARG;
                break;
                }

            // 
            // Add this item to our list of parameters to fetch.
            // 
            m_pList[i].pszFieldName = new char[ strlen( W2A( v.bstrVal ) ) + 1];
            strcpy( m_pList[i].pszFieldName, W2A( v.bstrVal ) );

            // 
            // Fetch type of item.
            // 
            v = sa( i, 1 );
            
            // 
            // Make sure this is an unsigned integer.
            // 
            if( v.vt != VT_UI4 )
                {
                hr = E_INVALIDARG;
                break;
                }

            m_pList[i].vType = (VARTYPE) v.ulVal;
            m_iArraySize ++;
            }
        }
    catch( _com_error e )
        {
        hr = e.Error();
        }
    catch(...)
        {
        hr = E_OUTOFMEMORY;
        }

    sa.Detach();

    return hr;

}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Private methods.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::FinalRelease
//
// Description   : Called just before object is to be destroyed.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CEnumQserverItemsVariant::FinalRelease()
{
    // 
    // Cleanup
    // 
    if( m_pEnumerator )
        {
        delete m_pEnumerator;
        m_pEnumerator = NULL;
        }

    for(int i = 0; i < m_iArraySize; i++ )
        {
        delete [] m_pList[i].pszFieldName;
        }
    delete [] m_pList;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::GetVariantFields
//
// Description   : This routine will get the user specified fields and place
//                 them in a safe array VARIANT.
//
// Return type   : STDMETHODIMP 
//
// Argument      : LPCTSTR szFileName
// Argument      : VARIANT *pVariant
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::GetVariantFields( LPCTSTR szFileName, 
                                                 VARIANT *pVariant )
{
USES_CONVERSION;

    HRESULT hr;
    SAFEARRAYBOUND bounds;
    HQSERVERITEM hItem;
    TCHAR   szPath[ MAX_PATH + 1 ];
   
    // 
    // Open item in question.
    // 
    wsprintf( szPath, _T("%s\\%s"), m_szQuarantineDir, szFileName );
    if( QSPAKSTATUS_OK != QsPakOpenItem( T2A( szPath ), &hItem ) )
        {
        return E_UNEXPECTED;
        }

    // 
    // Create safearray for all elements.
    //     
    bounds.lLbound = 0;
    bounds.cElements = m_iArraySize;
    SAFEARRAY* psa = SafeArrayCreate( VT_VARIANT, 1, &bounds );
   
    if( psa == NULL )
        {
        pVariant->vt = VT_ERROR;
        pVariant->scode = GetScode( E_OUTOFMEMORY );
        QsPakReleaseItem( hItem );
        return E_OUTOFMEMORY;
        }

    // 
    // Allocate and populate a variant for each requested field.
    // 
    for( int i = 0; i < m_iArraySize; i++ )
        {
        switch( m_pList[i].vType )
            {
            case VT_BSTR:
                hr = GetBSTRVal( hItem, m_pList[i].pszFieldName, i, psa );
                break;

            case VT_UI4:
                hr = GetULONGVal( hItem, m_pList[i].pszFieldName, i, psa );
                break;

            case VT_ARRAY:
                hr = GetBinaryVal( hItem, m_pList[i].pszFieldName, i, psa );
                break;

            case VT_DATE:
                hr = GetDateVal( hItem, m_pList[i].pszFieldName, i, psa );
                break;

            default:
                hr = E_INVALIDARG;
                break;
            }
        
        if( FAILED( hr ) )
            break;
        }

    if( FAILED( hr ) )
        {
        SafeArrayDestroy( psa );
        pVariant->vt = VT_ERROR;
        pVariant->scode = GetScode( hr );
        QsPakReleaseItem( hItem );
        return hr;
        }

    // 
    // Save off our variant.
    // 
    pVariant->vt = VT_ARRAY | VT_VARIANT;
    pVariant->parray = psa;

    // 
    // Close item
    // 
    QsPakReleaseItem( hItem );

    // 
    // Everything looks good.
    // 
    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::GetDateVal
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : LPCTSTR szFileName
// Argument      : LPTSTR szFieldName
// Argument      : int iIndex
// Argument      : SAFEARRAY *psa
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::GetDateVal( HQSERVERITEM hItem,
                                                   LPSTR szFieldName, 
                                                   int iIndex, 
                                                   SAFEARRAY *psa )
{
    HRESULT hr = S_OK;
    VARIANT v;
    
    QSPAKDATE qsDate;
    ULONG ulSize = sizeof( qsDate );
    
    // 
    // Get DATE value.
    // 
    VariantInit( &v );
    try
        {
        if( QSPAKSTATUS_OK != QsPakQueryItemValue( hItem, 
                                                   szFieldName,
                                                   QSPAK_TYPE_DATE,
                                                   &qsDate, 
                                                   &ulSize ) )
            {
            hr = S_FALSE;
            }
        else
            {
            SYSTEMTIME st;
            ZeroMemory( &st, sizeof( SYSTEMTIME ) );
            st.wDay = qsDate.byDay;
            st.wMonth = qsDate.byMonth;
            st.wYear = qsDate.wYear;
            st.wHour = qsDate.byHour;
            st.wMinute = qsDate.byMinute;
            st.wSecond = qsDate.bySecond;
            v.vt = VT_DATE;
            SystemTimeToVariantTime( &st, &v.date );
            }
        }
    catch(...)
        {
        hr = E_FAIL;
        }

    // 
    // If we are good so far, insert this variant into array.
    // 
    if( SUCCEEDED( hr ) )
        {
        hr = SafeArrayPutElement( psa, (LONG*)&iIndex, &v );
        VariantClear( &v );
        }

    return hr;    
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::GetBSTRVal
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : LPCTSTR szFileName
// Argument      : LPTSTR szFieldName
// Argument      : int iIndex
// Argument      : SAFEARRAY *psa
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::GetBSTRVal( HQSERVERITEM hItem,
                                                   LPSTR szFieldName, 
                                                   int iIndex, 
                                                   SAFEARRAY *psa )
{
USES_CONVERSION;

    HRESULT hr = S_OK;
    ULONG ulSize = 0;
    VARIANT v;
    VariantInit( &v );
    
    // 
    // Get string value.
    // 
    try
        {
        if( QSPAKSTATUS_BUFFER_TOO_SMALL == QsPakQueryItemValue( hItem, 
                                                   szFieldName,
                                                   QSPAK_TYPE_STRING,
                                                   NULL, 
                                                   &ulSize ) )
            {
            // 
            // Allocate buffers and get data
            // 
            LPSTR pszTemp = new char[ ulSize + 1 ] ;
            ZeroMemory( pszTemp, ulSize + 1 );
            if( QSPAKSTATUS_OK == QsPakQueryItemValue( hItem, 
                                                       szFieldName,
                                                       QSPAK_TYPE_STRING,
                                                       pszTemp, 
                                                       &ulSize ) )
                {
                v.vt = VT_BSTR;
                v.bstrVal = SysAllocStringLen( A2W( pszTemp ), strlen( pszTemp ) + 1 );
                if( v.bstrVal == NULL )
                    {
                    hr = E_OUTOFMEMORY;
                    }
                }
            delete [] pszTemp;
            }
        else
            {
            // 
            // Field does not exist.
            // 
            v.vt = VT_EMPTY;
            hr = S_FALSE;
            }
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }


    // 
    // If we are good so far, insert this variant into array.
    // 
    if( SUCCEEDED( hr ) )
        {
        hr = SafeArrayPutElement( psa, (LONG*)&iIndex, &v );
        VariantClear( &v );
        }


    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::GetULONGVal
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : LPCTSTR szFileName
// Argument      : LPTSTR szFieldName
// Argument      : int iIndex
// Argument      : SAFEARRAY *psa
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::GetULONGVal( HQSERVERITEM hItem,
                                                   LPSTR szFieldName, 
                                                   int iIndex, 
                                                   SAFEARRAY *psa )
{
    HRESULT hr = S_OK;
    ULONG ulSize = sizeof( LONG );
    VARIANT v;
    QSPAKSTATUS status;
    VariantInit( &v );

    // 
    // Get string value.
    // 
    v.vt = VT_I4;
    try
        {
        status = QsPakQueryItemValue( hItem, 
                                      szFieldName,
                                      QSPAK_TYPE_DWORD,
                                      &v.ulVal,
                                      &ulSize );
        if( status != QSPAKSTATUS_OK )
            {
            if( status == QSPAKSTATUS_NO_SUCH_FIELD )
                {
                v.vt = VT_EMPTY;
                hr = S_FALSE;
                }
            else
                hr = E_FAIL;
            }
        else
            {
            v.vt = VT_UI4;
            }
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }


    // 
    // If we are good so far, insert this variant into array.
    // 
    if( SUCCEEDED( hr ) )
        {
        hr = SafeArrayPutElement( psa, (LONG*)&iIndex, &v );
        VariantClear( &v );
        }

    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumQserverItemsVariant::GetBinaryVal
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : HQSERVERITEM hItem
// Argument      : LPTSTR szFieldName
// Argument      : int iIndex
// Argument      : SAFEARRAY *psa
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQserverItemsVariant::GetBinaryVal( HQSERVERITEM hItem,
                                                   LPSTR szFieldName, 
                                                   int iIndex, 
                                                   SAFEARRAY *psa )
{
    HRESULT hr = S_OK;
    ULONG ulSize = 0;
    VARIANT v;
    SAFEARRAY* psaRet;
    SAFEARRAYBOUND bounds;
    LPBYTE p;
    LONG lIndexes = 1;
    VariantInit( &v );

    try
        {
        if( QSPAKSTATUS_BUFFER_TOO_SMALL == QsPakQueryItemValue( hItem, 
                                                   szFieldName,
                                                   QSPAK_TYPE_BINARY,
                                                   NULL, 
                                                   &ulSize ) )
            {
            // 
            // Create safearray of bytes to hold data.
            //     
            bounds.lLbound = 0;
            bounds.cElements = ulSize;
            psaRet = SafeArrayCreate( VT_UI1, 1, &bounds );
            if( psaRet != NULL )
                {
                // 
                // Get pointer to begining of array.
                // 
                if( SUCCEEDED( SafeArrayAccessData( psaRet, (LPVOID*)&p ) ) )

                    {
                    if( QSPAKSTATUS_OK == QsPakQueryItemValue( hItem, 
                                                               szFieldName,
                                                               QSPAK_TYPE_BINARY,
                                                               p, 
                                                               &ulSize ) )
                        {
                        v.vt = VT_ARRAY | VT_VARIANT;
                        v.parray = psaRet;
                        }
                    else
                        {
                        hr = E_FAIL;
                        }

                    SafeArrayUnaccessData( psaRet );
                    }
                else
                    {
                    hr = E_UNEXPECTED;
                    }

                }
            }
        else
            {
            // 
            // Field does not exist.
            // 
            v.vt = VT_EMPTY;
            hr = S_FALSE;
            }

        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }

    // 
    // If we are good so far, insert this variant into array.
    // 
    if( SUCCEEDED( hr ) )
        {
        hr = SafeArrayPutElement( psa, (LONG*)&iIndex, &v );
        VariantClear( &v );
        }

    return hr;
}
