/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QServerSavInfo.cpp : Implementation of CQServerSavInfo
#include "stdafx.h"
#include "Qserver.h"
#include "QServerSavInfo.h"
#include "const.h"
#include "qscommon.h"

int CQServerSavInfo::m_iNoInstances = 0;
/////////////////////////////////////////////////////////////////////////////
// CQServerSavInfo

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::CQServerSavInfo
//
// Description: Constructor for the class.  It will initilaze the interface to 
//				NAVAPI
//
// Return type: none
//
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CQServerSavInfo::CQServerSavInfo()
{
	m_hrAVScannerStatus = m_AVScanner.Initialize();
	if( SUCCEEDED( m_hrAVScannerStatus ) )
	{
		memset(&m_nvVirusDBInfo,0,sizeof(m_nvVirusDBInfo));
		m_iNoInstances++;
	}
}                       

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::~CQServerSavInfo
//
// Description: Destructor
//
// Return type: none
//
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CQServerSavInfo::~CQServerSavInfo()
{
	m_iNoInstances--;
}                       


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// IQServerSavInfo
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::GetSavEngVersion
//
// Description: This function will return SAV (Symantec AntiVirus)
//				scanning engine version number from NAVAPI
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG pulVersion,                 
//
///////////////////////////////////////////////////////////////////////////////
// 06-25-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::GetSavEngVersion(VARIANT *v)
{
	HRESULT hr = ERROR_CALL_NOT_IMPLEMENTED;
	VariantClear (v);
	return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::GetSavDefVersion
//
// Description: This function will return the version number of the installed 
//				antivirus definitions
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG pulVersion,                 
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::GetSavAPIVersion(VARIANT *v)
{
	VariantClear (v);
	return m_AVScanner.GetVirusDBVersionString(v);
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::GetSavSeqNumber
//
// Description: This function will return the sequence number of the installed 
//				antivirus definitions
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG pulVersion,                 
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::GetSavSeqNumber(VARIANT *v)
{
	HRESULT hr = E_FAIL;
	DWORD dwSequenceNumber=0;
	VariantClear (v);
	if (S_OK == m_AVScanner.GetVirusDBSequenceNumber(&dwSequenceNumber))
	{
		VariantInit( v );
		v->vt = VT_UI4;
		v->ulVal = dwSequenceNumber;
		hr = S_OK;
	}

	return hr;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::GetSavDevVersion
//
// Description: This function will return the sequence number of the installed 
//				antivirus definitions
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG pulVersion,                 
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::GetSavDefVersion(VARIANT *v)
{
	HRESULT hr = E_FAIL;
	VariantClear (v);
	if (m_hrAVScannerStatus  == E_PENDING)
		hr = m_AVScanner.Initialize();
	

	return m_AVScanner.GetVirusDefVersionNumber(v);
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::GetSavDefDate
//
// Description: This function will return the date of the installed 
//				antivirus definitions
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG pulVersion,                 
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::GetSavDefDate(VARIANT *pv)
{
	HRESULT hr = E_FAIL;
	SYSTEMTIME st={0};
    TCHAR szDate[32]={0};
	TCHAR szTemp[MAX_PATH];
    DWORD dwRev;

	USES_CONVERSION;
    
	VariantClear (pv);
	if (m_hrAVScannerStatus  == E_PENDING)
		hr = m_AVScanner.Initialize();
	if (hr == E_PENDING)
		return E_PENDING;

	LoadString (_Module.GetResourceInstance(), IDS_SAV_DATE_FORMAT, szTemp, sizeof(szTemp));

	if (S_OK == m_AVScanner.GetVirusDefsDate(&st, &dwRev))
	{
//		CTime ctTime(st);
		VariantInit( pv );

        
        GetDateFormat( LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &st, NULL, szDate, 32 ); 
		CString sTemp;
		sTemp.Format(szTemp,szDate,dwRev);
		pv->vt = VT_BSTR;
		pv->bstrVal = sTemp.AllocSysString();

		//		pv->vt = VT_DATE;
//		SystemTimeToVariantTime (&st, &pv->date);
		hr = S_OK;
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::GetEnumVirusInfo
//
// Description: This function will create and return an enum used to enumerate all of 
//				the virus info in the installed definitions.
//
// Return type: STDMETHODIMP 
//
// Argument: IEnumSavVirusInfo** pEnum:  Pointer to an enumerator that will enumerate
//										virus info
//
///////////////////////////////////////////////////////////////////////////////
// 06-25-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::GetEnumVirusInfo(IEnumSavVirusInfo** pEnum)
{
	
	HRESULT hr=S_OK;
	if (pEnum == NULL)
		return E_INVALIDARG;
    
	// reset the index.
	m_iCurrentVIndex = 0;
	hr =m_AVScanner.LoadVirusTable();
	if (S_OK != hr)
	{
		hr =  E_FAIL;
	}
	// 
    // Since this object should support the IEnumQuarantineServerItems, just bump the 
    // internal reference count of this object and return.
    // 
    return SUCCEEDED( hr ) ? QueryInterface( __uuidof( IEnumSavVirusInfo), (LPVOID*) pEnum ) : hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::GetSavNoDefs
//
// Description: This function will return the sequence number of the installed 
//				antivirus definitions
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG pulVersion,                 
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::GetSavNoDefs(ULONG *pulNoDefs)
{
	if (pulNoDefs == NULL)
		return E_FAIL;

	*pulNoDefs = m_AVScanner.GetNoDefInTable();
	
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// IEnumSavVirusInfo
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::Next
//
// Description: This function will return the next block of virus info
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG celt,                 
// Argument: VARIANT *aVirusInfo
// Argument: ULONG* pceltFetched
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CQServerSavInfo::Next( ULONG celt, 
			                          VARIANT *aVirusInfo, 
			                          ULONG* pceltFetched )
{
    ULONG ulNumReturned = 0;
    HRESULT hr = S_OK;
	VariantClear (aVirusInfo);
         
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
    if( aVirusInfo == NULL )
        return E_POINTER;


    // 
    // Fetch the correct number of items.
    // 

	for (ULONG i=0; i < celt; ++i)
	{
		if (m_iCurrentVIndex+ulNumReturned >= m_AVScanner.GetNoDefInTable())
			break;
		hr = m_AVScanner.GetVirusInfobyTableIndex(m_iCurrentVIndex+ulNumReturned,&aVirusInfo[i]);
		if (SUCCEEDED(hr))
		{
			ulNumReturned ++;
		}
		else
			break;
	}


/*	
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
*/
    if( pceltFetched != NULL )
        *pceltFetched = ulNumReturned;

	m_iCurrentVIndex += ulNumReturned;

    if( ulNumReturned == 0 )
        return S_FALSE;

    // 
    // Success!
    // 
    return hr;



}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::Skip
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG celt
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::Skip( ULONG celt )
{

   	// reset the index.
	m_iCurrentVIndex += celt;

	return S_OK;    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQServerSavInfo::Reset
//
// Description: 
//
// Return type: STDMETHODIMP 
//
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQServerSavInfo::Reset()
{
    HRESULT hr = S_OK;

	// reset the index.
	m_iCurrentVIndex = 0;

    return hr;
}

