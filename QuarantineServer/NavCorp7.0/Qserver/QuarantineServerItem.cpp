// QuarantineServerItem.cpp : Implementation of CQuarantineServerItem
#include "stdafx.h"
#include "Qserver.h"
#include "QuarantineServerItem.h"
#include "qsfields.h"
//#include "safearray.h"

/////////////////////////////////////////////////////////////////////////////
// CQuarantineServerItem


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServerItem::Commit
//
// Description  : This routine will commit any changes made to this object to disk.
//
// Return type  : STDMETHODIMP 
//
//
///////////////////////////////////////////////////////////////////////////////
// 1/14/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServerItem::Commit()
{
	if( m_hItem == NULL )
        return E_UNEXPECTED;

    if( m_bChanged == FALSE )
        return S_OK;

	//
	// Need to bump changes count.
	//
	DWORD dwChanges;
	DWORD dwSize = sizeof(DWORD);
	QSPAKSTATUS status = QsPakQueryItemValue( m_hItem, 
		QSERVER_ITEM_INFO_CHANGES,
		QSPAK_TYPE_DWORD,
		&dwChanges,
		&dwSize );

	switch( status )
		{
		case QSPAKSTATUS_OK:
			break;

		case QSPAKSTATUS_NO_SUCH_FIELD:
			dwChanges = 0;
			break;
		default:
			return E_UNEXPECTED;
		}

	dwChanges++;

	status = QsPakSetItemValue( m_hItem, 
		QSERVER_ITEM_INFO_CHANGES, 
		QSPAK_TYPE_DWORD,
		&dwChanges,
		sizeof( DWORD ) );

	if( status != QSPAKSTATUS_OK )
		return E_OUTOFMEMORY;

    switch( QsPakSaveItem( m_hItem ) )
        {
        case QSPAKSTATUS_OK: 
            m_bChanged = FALSE;
            return S_OK;
        case QSPAKSTATUS_MEMORY_ERROR: return E_OUTOFMEMORY;
        case QSPAKSTATUS_FILE_NOT_FOUND: return S_FALSE;

        default:
            return E_UNEXPECTED;
        }

	return E_UNEXPECTED;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQuarantineServerItem::GetValue
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : BSTR bstrFieldName
// Argument      : VARIANT* v
//
///////////////////////////////////////////////////////////////////////////////
// 2/17/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServerItem::GetValue( BSTR bstrFieldName, 
                                              VARIANT* v )
{
USES_CONVERSION;

    HRESULT hr = S_OK;
    UINT uNumFields = 0;
    LPFIELDINFO pFields = NULL;
    BOOL bFound = FALSE;
    SAFEARRAY* psa;
    SAFEARRAYBOUND bounds;
    ULONG ulSize;
    LPBYTE p = NULL;
    QSPAKDATE qsDate;
    SYSTEMTIME st;

    // 
    // Validate input.
    // 
    if( v == NULL )
        return E_INVALIDARG;

    // 
    // Clear contents of output buffer.
    // 
    VariantInit( v );

    // 
    // Fetch list of attributes from the quarantine server item.
    // 
    if( QSPAKSTATUS_OK != QsPakGetItemFieldInfo( m_hItem, &uNumFields, &pFields ) )
        return E_FAIL;

    // 
    // Find our value
    // 
    LPSTR szFieldName = W2A( bstrFieldName );
    for( UINT i = 0; i < uNumFields; i++ )
        {
        if( strcmp( szFieldName, pFields[i].szFieldName ) == 0 )
            {
            bFound = TRUE;
            break;
            }
        }
    
    if( bFound )
        {
        // 
        // Fetch value.
        // 
        try
            {
            switch( pFields[i].eType )
                {
                case QSPAK_TYPE_BINARY:
                    {
                    ulSize = pFields[i].uDataLength;
                    bounds.lLbound = 0;
                    bounds.cElements = ulSize;
                    psa = SafeArrayCreate( VT_UI1, 1, &bounds );
                    if( psa == NULL )
                        throw;

                    if( SUCCEEDED( SafeArrayAccessData( psa, (LPVOID*)&p ) ) )
                        {
                        QsPakQueryItemValue( m_hItem, pFields[i].szFieldName, pFields[i].eType, p, &ulSize );
                        SafeArrayUnaccessData( psa );
                        }
                    v->vt = VT_ARRAY | VT_VARIANT;
                    }
                    break;
             
                case QSPAK_TYPE_STRING:
                    {
                    ulSize = pFields[i].uDataLength + 1;
                    p = new BYTE[ ulSize ];
                    if( p == NULL )
                        throw;
                    ZeroMemory( p, ulSize );
                    QsPakQueryItemValue( m_hItem, pFields[i].szFieldName, pFields[i].eType, p, &ulSize );

                    v->bstrVal = ::SysAllocStringLen( A2W( (LPSTR) p ), strlen( (LPSTR) p ) );
                    v->vt = VT_BSTR;
                    }
                    break;
                
                case QSPAK_TYPE_DWORD:
                    ulSize = sizeof( v->ulVal );
                    QsPakQueryItemValue( m_hItem, pFields[i].szFieldName, pFields[i].eType, &v->ulVal, &ulSize );
                    v->vt = VT_UI4;
                    break;

                case QSPAK_TYPE_DATE:
                    ZeroMemory(&st, sizeof( SYSTEMTIME ) );  
                    ZeroMemory(&qsDate, sizeof( QSPAKDATE ) );
                    ulSize = sizeof( QSPAKDATE );
                    QsPakQueryItemValue( m_hItem, pFields[i].szFieldName, pFields[i].eType, &qsDate, &ulSize );
                    st.wDay = qsDate.byDay;
                    st.wMonth = qsDate.byMonth;
                    st.wYear = qsDate.wYear;
                    st.wHour = qsDate.byHour;
                    st.wMinute = qsDate.byMinute;
                    st.wSecond = qsDate.bySecond;
                    v->vt = VT_DATE;
                    SystemTimeToVariantTime( &st, &v->date );
                    break;
                }
            }
        catch(...)
            {
            hr = E_UNEXPECTED;
            }
        }
    else
        {
        // 
        // Field does not exist.
        // 
        hr = S_FALSE;
        }

    // 
    // Clean up allocations.
    // 
    QsPakFreeItemFieldInfo( pFields );

    if( p )
        delete [] p;

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQuarantineServerItem::SetValue
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : BSTR bstrFieldName
// Argument      : VARIANT v
//
///////////////////////////////////////////////////////////////////////////////
// 2/17/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServerItem::SetValue( BSTR bstrFieldName, 
                                              VARIANT v )
{
USES_CONVERSION;    
    HRESULT hr = S_OK;
    LPSTR szFieldName = W2A( bstrFieldName );
    QSPAKSTATUS status;

    // 
    // Valdate input
    // 
    if( v.vt != VT_BSTR && 
        v.vt != VT_UI4 && 
        v.vt != VT_DATE && 
        v.vt != ( VT_UI1 | VT_ARRAY ) )
        return E_INVALIDARG;

    try
        {
        switch( v.vt )
            {
            case VT_BSTR:
                {
                USES_CONVERSION;
                LPSTR szData = W2A( v.bstrVal );
                status = QsPakSetItemValue( m_hItem, szFieldName, QSPAK_TYPE_STRING, szData, strlen( szData ) );
                }
                break;

            case VT_UI4:
                status = QsPakSetItemValue( m_hItem, szFieldName, QSPAK_TYPE_DWORD, &v.ulVal, sizeof( v.ulVal ) );
                break;

            case VT_DATE:
                {
                QSPAKDATE qsDate;
                SYSTEMTIME st;
                VariantTimeToSystemTime( v.date, &st );
                qsDate.byDay = (BYTE) st.wDay;
                qsDate.byMonth = (BYTE) st.wMonth;
                qsDate.wYear = st.wYear;
                qsDate.byHour = (BYTE) st.wHour;
                qsDate.byMinute = (BYTE) st.wMinute;
                qsDate.bySecond = (BYTE) st.wSecond;
                status = QsPakSetItemValue( m_hItem, szFieldName, QSPAK_TYPE_DATE, &qsDate, sizeof( QSPAKDATE ) );
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

                    status = QsPakSetItemValue( m_hItem, szFieldName, QSPAK_TYPE_BINARY, p, lSize );
                    hr = SafeArrayUnaccessData( v.parray );
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

    if( status != QSPAKSTATUS_OK )
        return E_FAIL;

    // 
    // All is well if we get here, so set changed bit.
    // 
    m_bChanged = TRUE;

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQuarantineServerItem::GetAllValues
//
// Description   : This routine will retrieve all values from this object and 
//                 place them in a safearray stored in output variant.
//
// Return type   : STDMETHODIMP 
//
// Argument      : VARIANT v
//
///////////////////////////////////////////////////////////////////////////////
// 2/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServerItem::GetAllValues( VARIANT* v )
{
USES_CONVERSION;

    HRESULT hr = S_OK;
    UINT uNumFields = 0;
    LPFIELDINFO pFields = NULL;
    SAFEARRAY* psa;
    SAFEARRAYBOUND bounds;
    VARIANT vt;
    ULONG ulSize;
    LPBYTE p;
    SYSTEMTIME st;
    QSPAKDATE qsDate;

    if( v == NULL )
        return E_INVALIDARG;

    // 
    // Initialze output.
    // 
    VariantInit( v );
    VariantInit( &vt );

    // 
    // Fetch list of attributes from the quarantine server item.
    // 
    if( QSPAKSTATUS_OK != QsPakGetItemFieldInfo( m_hItem, &uNumFields, &pFields ) )
        return E_FAIL;

    try
        {
        // 
        // Construct safearray to hold our items.
        //  
        _safearray_t< 2 > safearray( uNumFields, uNumFields );

        // 
        // Populate array.
        // 
        for( UINT i = 0; i < uNumFields; i++ )
            {
            // 
            // Safe off field name.
            // 
            safearray( i, 0 ).vt = VT_BSTR;
            safearray( i, 0 ).bstrVal = ::SysAllocStringLen( A2W( pFields[i].szFieldName ), strlen( pFields[i].szFieldName ) );  

            // 
            // Save off data.
            // 
            switch( pFields[i].eType )
                {
                case QSPAK_TYPE_BINARY:
                    {
                    ulSize = pFields[i].uDataLength;
                    bounds.lLbound = 0;
                    bounds.cElements = ulSize;
                    psa = SafeArrayCreate( VT_UI1, 1, &bounds );
                    if( psa == NULL )
                        continue;

                    if( SUCCEEDED( SafeArrayAccessData( psa, (LPVOID*)&p ) ) )
                        {
                        QsPakQueryItemValue( m_hItem, pFields[i].szFieldName, pFields[i].eType, p, &ulSize );
                        SafeArrayUnaccessData( psa );
                        }
                    vt.vt = VT_ARRAY | VT_VARIANT;
                    vt.parray = psa;
                    }
                    break;
             
                case QSPAK_TYPE_STRING:
                    {
                    ulSize = pFields[i].uDataLength + 1;
                    p = new BYTE[ ulSize ];
                    if( p == NULL )
                        continue;
                    ZeroMemory( p, ulSize );
                    QsPakQueryItemValue( m_hItem, pFields[i].szFieldName, pFields[i].eType, p, &ulSize );

                    vt.bstrVal = ::SysAllocStringLen( A2W( (LPSTR) p ), strlen( (LPSTR) p ) );
                    vt.vt = VT_BSTR;

                    delete [] p;
                    }
                    break;
                
                case QSPAK_TYPE_DWORD:
                    ulSize = sizeof( vt.ulVal );
                    QsPakQueryItemValue( m_hItem, pFields[i].szFieldName, pFields[i].eType, &vt.ulVal, &ulSize );
                    vt.vt = VT_UI4;
                    break;

                case QSPAK_TYPE_DATE:
                    ZeroMemory(&st, sizeof( SYSTEMTIME ) );  
                    ZeroMemory(&qsDate, sizeof( QSPAKDATE ) );
                    ulSize = sizeof( QSPAKDATE );
                    QsPakQueryItemValue( m_hItem, pFields[i].szFieldName, pFields[i].eType, &qsDate, &ulSize );
                    st.wDay = qsDate.byDay;
                    st.wMonth = qsDate.byMonth;
                    st.wYear = qsDate.wYear;
                    st.wHour = qsDate.byHour;
                    st.wMinute = qsDate.byMinute;
                    st.wSecond = qsDate.bySecond;
                    vt.vt = VT_DATE;
                    SystemTimeToVariantTime( &st, &vt.date );
                    break;

                default:
                    VariantInit( &vt );
                }

            safearray( i, 1 ) = vt;
            }

        // 
        // Safe off output.
        // 
        v->vt = VT_ARRAY | VT_VARIANT;
        v->parray = safearray.Detach();
        }
    catch(...)
        {
        hr = E_FAIL;
        }

    // 
    // Clean up allocations.
    // 
    QsPakFreeItemFieldInfo( pFields );

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// Private methods

/////////////////////////////////////////////////////////////////////////////
// IQuarantineServerItemInternal


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServerItem::Initialize
//
// Description  : This private routine will initialize this object. 
//
// Return type  : STDMETHODIMP 
//
// Argument     : BSTR strFileName
//
///////////////////////////////////////////////////////////////////////////////
// 1/14/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServerItem::Initialize(BSTR strFileName, ULONG ulItemID )
{
USES_CONVERSION;

    if( QSPAKSTATUS_OK != QsPakOpenItem( W2A( strFileName ), &m_hItem ) )
        {
        return E_FAIL;
        }
    
    // 
    // Save off item ID.
    // 
    m_ulItemID = ulItemID;

	return S_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQuarantineServerItem::FinalRelease
//
// Description   : Called when object is destroyed.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/17/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQuarantineServerItem::FinalRelease()
{
    // 
    // Cleanup.
    // 
    if( m_hItem )
        {
        QsPakReleaseItem( m_hItem );
        m_hItem = NULL;
        }

    // 
    // Remove this item from the master list of open items.
    // 
    _Module.RemoveOpenItem( m_ulItemID, this );    

}