////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AVEvent.cpp: implementation of the CAVEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVEvent.h"
#include "AllNAVEvents.h"
#include "..\navlogvres\resource.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVEvent::CAVEvent()
{
}

CAVEvent::~CAVEvent()
{

    for ( mapEventData::iterator iter = m_mapEventData.begin ();
          iter != m_mapEventData.end ();
          iter++ )
    {
        int iNum = iter->first;
        _variant_t* pTemp = iter->second;
        if ( pTemp )
		{
            delete pTemp;
			pTemp = NULL;
    	}
    }

    m_mapEventData.erase (m_mapEventData.begin(), m_mapEventData.end());
}


HRESULT CAVEvent::GetImageListIndex(int& iImageListIndex)
{
	return E_NOTIMPL;
}
	
HRESULT CAVEvent::GetColumnData( int iColumn,
                                 DWORD dwFlags,
                                 VARIANT& varData)
{
    VariantClear ( &varData );

    // Asking for column that doesn't exist/isn't filled?
    //
    if ( m_mapEventData.find (iColumn) == m_mapEventData.end () )
    {
        _variant_t vTemp;
        TCHAR szTemp [1024] = {0};
        ::LoadString ( _Module.GetResourceInstance(), IDS_ACTION_UNKNOWN, szTemp, sizeof(szTemp)/sizeof(TCHAR));
        vTemp = szTemp;
        AddData ( iColumn, vTemp );
    }

    // Date fields
    //
    if ( V_VT(m_mapEventData[iColumn]) == VT_DATE )
    {
        // If the formats aren't the same, convert.
        //
        if ( m_mapEventFormat[iColumn] != dwFlags)
        {
            if ( LV_DATE_FORMAT_UTC == m_mapEventFormat[iColumn] )
            {
                // Convert UTC to Local
                //
                SYSTEMTIME systimeLocal;
                SYSTEMTIME systimeUTC;
                DATE dtDate;

                VariantTimeToSystemTime ( *m_mapEventData[iColumn], &systimeUTC );
                UTCSystemTimeToLocalTime ( &systimeUTC, &systimeLocal );
                SystemTimeToVariantTime (&systimeLocal, &dtDate);
        
                V_VT(m_mapEventData[iColumn]) = VT_DATE;
                V_DATE(m_mapEventData[iColumn]) = dtDate;
                
                // Change the format to local
                m_mapEventFormat[iColumn] = LV_DATE_FORMAT_LOCAL;
            }
            else
            {
                // Convert Local to UTC
                //
                SYSTEMTIME systimeLocal;
                SYSTEMTIME systimeUTC;
                DATE dtDate;

                VariantTimeToSystemTime ( *m_mapEventData[iColumn], &systimeUTC );
                UTCSystemTimeToLocalTime ( &systimeUTC, &systimeLocal );
                SystemTimeToVariantTime (&systimeLocal, &dtDate);
        
                V_VT(m_mapEventData[iColumn]) = VT_DATE;
                V_DATE(m_mapEventData[iColumn]) = dtDate;

                // Change the format to UTC
                m_mapEventFormat[iColumn] = LV_DATE_FORMAT_UTC; // add UTC
            }
        }
    }

    VariantCopyInd ( &varData, m_mapEventData[iColumn] );

    return S_OK;
}

HRESULT CAVEvent::GetDetails( cc::LVEVENT_DETAILSTYPE Type, /*in*/
                             DWORD dwFlags, 
                             LPSTR szDetailText, 
                             /* [in/out] */ DWORD& dwSize)
{
    return E_NOTIMPL;
}

HRESULT CAVEvent::GetDetailsW( cc::LVEVENT_DETAILSTYPE Type, /*in*/
                              DWORD dwFlags, 
                              LPWSTR wszDetailText, 
	 			              /* [in/out] */ DWORD& dwSize)
{
    // We don't have anything that cares about dwFlags right now.

    switch ( Type )
    {
        // Regular text for export and print
        //
        case cc::LVEDT_NORMAL :
        {
            if ( dwSize < m_strEventDetailsText.size() )
			{
				dwSize = m_strEventDetailsText.size();
				return E_MORE_DATA;
			}

			size_t lenStg = wcslen( m_strEventDetailsText.c_str());

            wcsncpy( wszDetailText, CT2W(m_strEventDetailsText.c_str()), lenStg);
			
			wszDetailText[ lenStg ] = '\0';
            
			dwSize = wcslen(wszDetailText);
            return S_OK;
        }
        break;

        // HTML text for UI
        //
        case cc::LVEDT_HTML :
        {
			
			size_t lenStg = wcslen( m_strEventDetailsHTML.c_str() );
            if ( dwSize < lenStg )
			{
				dwSize = lenStg;
				return E_MORE_DATA;
			}

            wcsncpy ( wszDetailText, CT2W(m_strEventDetailsHTML.c_str()), lenStg );
			wszDetailText[ lenStg ] = '\0';
            dwSize = wcslen(wszDetailText);
            return S_OK;
        }
        break;

        default :
            return E_FAIL;
            break;
    }
}

bool CAVEvent::AddData(int iColumn, const VARIANT& vData, DWORD dwFormat /* = 0 */)
{
    // See if it's already in the list
    //
    if ( m_mapEventData.find (iColumn) == m_mapEventData.end () )
    {
        m_mapEventData [iColumn] = new _variant_t;
    }

    *m_mapEventData [iColumn] = vData;
    m_mapEventFormat [iColumn] = dwFormat;

    return true;
}

void CAVEvent::SetDetailsText(LPCTSTR lpszDetails)
{
    m_strEventDetailsText = lpszDetails;
}

void CAVEvent::SetDetailsHTML(LPCTSTR lpszDetails)
{
    m_strEventDetailsHTML = lpszDetails;
}

// Converts UTC SYSTEMTIME to Local SYSTEMTIME
//
bool CAVEvent::UTCSystemTimeToLocalTime ( const SYSTEMTIME* psystimeUTC, SYSTEMTIME* psystimeLocal )
{
    // Log is in UTC System time. We need Variant Local Time. Ugh.
    //
    FILETIME ftimeUTC;
    FILETIME ftimeLocal;

    if ( SystemTimeToFileTime ( psystimeUTC, &ftimeUTC ) &&
         FileTimeToLocalFileTime ( &ftimeUTC, &ftimeLocal ) &&
         FileTimeToSystemTime ( &ftimeLocal, psystimeLocal ))
         return true;
    else 
        return false;
}

// Converts Local SYSTEMTIME to UTC SYSTEMTIME
//
bool CAVEvent::LocalTimeToUTCSystemTime ( const SYSTEMTIME* psystimeLocal, SYSTEMTIME* psystimeUTC )
{
    FILETIME ftimeUTC;
    FILETIME ftimeLocal;

    if ( SystemTimeToFileTime ( psystimeLocal, &ftimeLocal ) &&
         LocalFileTimeToFileTime ( &ftimeLocal, &ftimeUTC ) &&
         FileTimeToSystemTime ( &ftimeUTC, psystimeUTC ))
         return true;
    else 
        return false;
}
