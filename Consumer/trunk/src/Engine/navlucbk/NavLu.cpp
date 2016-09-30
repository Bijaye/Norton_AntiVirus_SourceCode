////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NavLu.cpp: implementation of the CNavLu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// PEP Headers
#include "V2LicensingAuthGuids.h"
#include "CLTProductHelper.h"

#include "navtrust.h"
#include "resource.h"
#include "LiveUpdate.h"

#include "NavLu.h"
#include "strings.h"
#include "OptNames.h"
#include "ISVersion.h"

#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

#include "NAVSettingsHelperEx.h"
#include "NAVInfo.h"
#include "ccModule.h"
#include "cctrace.h"

// AutoUpdate includes
#include "AutoUpdateDefines.h"
#include "..\navlucbkres\resource.h"

// Authenticate shipping (post-beta) NAVOPTRF.DLL
#include "NAVOptRefresh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern "C" 
{
	//&? Move this code into the pre-processing to update version after
	//&? build command line is called.
	//&? Also, extract BuildCommandLine() from CNavLu, or make it a static function.
	void __declspec(dllexport) WINAPI RegisterCmdLines()
	{

		if(FAILED(::CoInitialize(NULL)))
			return;

		_InitResources();

		try
		{
            CNavLu navLu;
            if( FALSE == navLu.Initialize() )
            {
                CCTRACEE(_T("CNavLu::RegisterCmdLines - Error: Unable to initialize CNavLu"));
            }
            else
			    navLu.RegisterCommandLines();
		}
		catch(_com_error &e)
		{
            CCTRCTXW1(_T("COM Exception while registering command lines: %s"), e.Error());
        }
		::CoUninitialize();
	}

} // extern "C"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//****************************************************************************
// CNavLu function implementations *******************************************
//****************************************************************************

CNavLu::CNavLu ( )
{
    ZeroMemory(&initialDefs, sizeof(DefsDateStruct));
    ZeroMemory(&postDefs, sizeof(DefsDateStruct));
}

CNavLu::~CNavLu()
{
    if( m_spAVDefInfo )
        m_spAVDefInfo.Release();

    if( m_ptrProductReg != NULL )
        m_ptrProductReg = NULL;
}

BOOL CNavLu::Initialize()
{
	// Get product name
	m_strBrandingProductName = CISVersion::GetProductName();

	// Get the full path to patch25d.dll
    CNAVInfo navInfo;
    m_strPatch25DllPath = navInfo.GetNAVDir();
    m_strPatch25DllPath += _T("\\Patch25d.dll");

    // Create the IAVDefInfo object
    if( !m_spAVDefInfo )
    {
        SYMRESULT sr = SYM_OK;
        if( SYM_FAILED(sr = m_AVDefInfoLoader.CreateObject(m_spAVDefInfo)) )
        {
            CCTRACEE(_T("CNavLu::Initialize - Error: Unable to load IAVDefInfo. SYMRESULT = 0x%X"), sr);
            return FALSE;
        }
    }

    // Save off the current defs date so we know if there is a newer one installed
    // after the postmicrodef update
    HRESULT hr = S_OK;
    if( FAILED(hr = m_spAVDefInfo->GetDefsDate(initialDefs.dwYear, initialDefs.dwMonth, initialDefs.dwDay, initialDefs.dwRev)) )
    {
        // If this fails set them all to 0
        CCTRACEE(_T("CNavLu::Initialize() - GetNewestDefsDate() failed to return defs date: 0x%X"), hr);
        ZeroMemory(&initialDefs, sizeof(DefsDateStruct));
    }
    else
        CCTRACEI(_T("CNavLu::Initialize() - Current defs returned by GetDefsDate are: %lu%02lu%02lu%03lu"), initialDefs.dwYear, initialDefs.dwMonth, initialDefs.dwDay, initialDefs.dwRev);

    // Create a product reg com object
    if ( m_ptrProductReg == NULL )
	{
        // Check the LU dll for a valid symantec signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(__uuidof(luProductReg)) )
        {
            CCTRACEE( _T("CNavLu::Initialize() - Symantec digital signature check failed on product reg dll.") );
            return FALSE;
        }

        HRESULT hr = S_OK;
		if ( FAILED( hr = m_ptrProductReg.CoCreateInstance( __uuidof( luProductReg ), NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER )) ) )
        {
			CCTRACEE( _T("CNavLu::Initialize() - Failed to create the product reg com object. Error = 0x%X"), hr );
            return FALSE;
        }
	}

    return TRUE;
}

bool CNavLu::PreProcess( )
{
    bool bAllowContentDownload = false;

    STAHLSOFT_HRX_TRY(hr)
    {
        CCLTProductHelper cltHelper;
        hrx << cltHelper.Initialize();

		bool bAllow = false;

        // Getting Subscription properties
        hrx << cltHelper.CanIAccessPaidContent(bAllowContentDownload);

		CCTRACEI(_T("CNavLu::PreProcess - bAllowContentDownload: %d"), bAllowContentDownload);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

    // Check the result of the subscription check to see if pre-processing should be attempted
    // or the virus definitions commandlines should be aborted with a subscription error
    if(bAllowContentDownload)
    {
        CCTRACEI(_T("CNavLu::PreProcess - Subscription check allowing def download."));

        // Parameters for PreMicroDef25Update only used here
        bool bIsBinaryHubOK = false;

        // PreMicroDefUpdate sets up the Hawking environment
        DURESULT duResLastResult = DU_E_GENERAL;
        cc::IStringPtr spLastResult;
        HRESULT hRes = m_spAVDefInfo->PreMicroDef25Update(m_spBinaryPatchDir, m_spPatchDirectDir, m_spBinaryPatchVersion,
            bIsBinaryHubOK, DU_LIVEUPDATE, reinterpret_cast<DWORD&>(duResLastResult), spLastResult);

        // If pre def update succeed make the appropriate modification to our commandlines
        if (DUSUCCESS(duResLastResult) && m_spBinaryPatchVersion)
        {
            CCTRACEI(_T("CNavLu::PreProcess() - PreMicroDef25Update() returned success. Binary patch version is: %s"), m_spBinaryPatchVersion->GetStringW());

            // Update the microdef hub version in the registry so that the call to RegisterCommandLines will update it with LU
            // The UpdateMicroDefHubVersion() function uses the version string from m_spBinaryPatchVersion
            UpdateMicroDefHubVersion();

            // Re-register any changed/updated/missing commandlines that we have in the registry
            CNavLu::RegisterCommandLines();

            // Set the sequence numbers for the virus defs, this will set them to 0 if the hub is damaged
            // which will trigger a full def hub and curdefs update
            SetSequenceNumber( bIsBinaryHubOK );
        }
        else
        {
            // Log the error returned by PreMicroDef25Update()
            CStringW strError = (spLastResult != NULL) ? spLastResult->GetStringW() : L"Unknown Error";
            CCTRACEE(L"CNavLu::PreProcess() - PreMicroDef25Update() failed with error: %s (%d).", strError.GetString(), duResLastResult);
        }
    }
    else
    {
        // if we aren't downloading defs then we don't need the rest of this stuff
        CCTRACEW(_T("CNavLu::PreProcess - Subscription check not allowing def download. bailing out of preprocess"));
        AbortDefsCommandlines( IDS_ERROR_SUBSCRIPTION_EXPIRED );
    }

	return bAllowContentDownload;
}

// Pulled from DoLiveUpdate() and modified for LUAll purposes
HRESULT CNavLu::PostProcess( )
{
    CCTRACEI(_T("CNavLu::PostProcess() - PostMicroDef25Update() being called with patch25.dll path: %s"), m_strPatch25DllPath.GetString());

    DURESULT duResLastResult = DU_E_GENERAL;
    cc::IStringPtr spLastResult;
    HRESULT hRes = m_spAVDefInfo->PostMicroDef25Update(m_strPatch25DllPath, reinterpret_cast<DWORD&>(duResLastResult), spLastResult);
	if( DUFAILED(duResLastResult) )
	{
        // Log the error
        CStringW strError = (spLastResult != NULL && spLastResult->GetLength() != 0) ? spLastResult->GetStringW() : L"Unknown Error";
        CCTRACEE(L"CNavLu::PostProcess() - PostMicroDef25Update() failed with error: %s (%d).", strError.GetString(), duResLastResult);

        // Check to see if definition authentication failed while integrating the defs
        if( duResLastResult == DU_E_AUTH )
        {
            AbortDefsCommandlines(IDS_ERROR_AUTHENTICATING_DEFS);
        }
        else
        {
			// Set the abort text for commandline 3
			AbortDefsCommandlines(IDS_ERROR_APPLYING_DEFS_ABORT_TEXT);
        }
	}
    else
         CCTRACEI(_T("CNavLu::PostProcess() - PostMicroDef25Update() succeeded"));

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Aborts the specified commandline
//
// Parameters:
//   dwCmdLine - Commandline in the registry to abort. Registry Location:
//               HKEY_LOCAL_MACHINE\SOFTWARE\Symantec\Norton AntiVirus\LiveUpdate\CmdLines\CmdLine[dwCmdLine]
//   nAbortTextResID - Resource ID of the text to use for the abort text
//
// Returns:
//   FALSE    - an error occured.
//   TRUE     - the commandline was successfully aborted
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::AbortDefsCommandlines(unsigned int unAbortTextResID)
{
    CCTRACEI(_T("CNavLu::AbortDefsCommandlines - Aborting commandlines with RESID: %d"),unAbortTextResID);
	try
	{
        if( m_ptrProductReg == NULL )
        {
            CCTRACEE(_T("CNavLu::AbortDefsCommandlines() - No product reg com object."));
            return FALSE;
        }

		// Get the abort text
		CString strFormat;
		CString strText;
		strFormat.LoadString(_Module.GetResourceInstance(), unAbortTextResID);
		strText = strFormat;
		
		// Format the abort text with product name
		if(unAbortTextResID == IDS_ERROR_SUBSCRIPTION_EXPIRED)
		{
			strText.Format(strFormat, m_strBrandingProductName);
		}

		CComBSTR bstrAbortText(strText);

        //
		// Abort the hub
        //
		_bstr_t bstrMoniker = _T(HUB_GUID);

		// Set the abort flag
		if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( g_szAbort ), _bstr_t( "YES" ))) )
		{
			CCTRACEE( _T("CNavLu::AbortDefsCommandlines - Failed to set the abort property for the debs hub") );
			return FALSE;
		}

		// Set the abort text
		if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( g_szAbortText ), bstrAbortText.m_str)) )
		{
			CCTRACEE( _T("CNavLu::AbortDefsCommandlines - Failed to set the abort text property.") );
			return FALSE;
		}

        // Set the abort error ID
        if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( LUPR_ABORT_ERR_CODE ), _bstr_t(unAbortTextResID))) )
		{
			CCTRACEE( _T("CNavLu::AbortDefsCommandlines - Failed to set the abort error ID property.") );
			return FALSE;
		}

        //
        // Abort the curdefs
        //
        bstrMoniker = _T(CURDEFS_GUID);

        // Set the abort flag
		if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( g_szAbort ), _bstr_t( "YES" ))) )
		{
			CCTRACEE( _T("CNavLu::AbortDefsCommandlines - Failed to set the abort property for the debs hub") );
			return FALSE;
		}

		// Set the abort text
		if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( g_szAbortText ), bstrAbortText.m_str)) )
		{
			CCTRACEE( _T("CNavLu::AbortDefsCommandlines - Failed to set the abort text property.") );
			return FALSE;
		}

        // Set the abort error ID
        if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( LUPR_ABORT_ERR_CODE ), _bstr_t(unAbortTextResID))) )
		{
			CCTRACEE( _T("CNavLu::AbortDefsCommandlines - Failed to set the abort error ID property.") );
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
        CCTRCTXE1( _T("COM Error while aborting: %s"), e.Error() );
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set the LiveUpdate sequence number based on the current virus
//   definition version information.
//////////////////////////////////////////////////////////////////////////
void CNavLu::SetSequenceNumber (bool bIsHubOK)
{
    if( !m_spAVDefInfo )
    {
        CCTRACEE(_T("CNavLu::SetSequenceNumber() - No IAVDefInfo object."));
        return;
    }

    if( m_ptrProductReg == NULL )
    {
        CCTRACEE(_T("CNavLu::SetSequenceNumber() - No product reg com object."));
        return;
    }

    /* determine the sequence number to set the LU command line to based on the defs installed */

	TCHAR szHubSequenceNumber [30];
	TCHAR szCurrdefsSequenceNumber [30];

	if (!bIsHubOK)
	{
		/* the hub is damaged */
        CCTRACEW(_T("CNavLu::SetSequenceNumber() - The hub is not OK, setting sequence numbers to 0."));
		_tcscpy (szHubSequenceNumber, _T("0"));
		_tcscpy (szCurrdefsSequenceNumber, _T("0"));
	}
	else
	{
		DWORD dwYear=0, dwMonth=0, dwDay=0, dwRev=0;

		HRESULT hRes = m_spAVDefInfo->GetBinHubDefsDate(dwYear, dwMonth, dwDay, dwRev);
		if( FAILED(hRes) || dwYear < 2000 )
		{
			CCTRACEE (_T("CNavLu::SetSequenceNumber() - Could not get BinHub Date. Setting to 0. hRes = 0x%X"), hRes);
			_tcscpy (szHubSequenceNumber, _T("0"));
		}
		else
		{
			dwYear -= 2000;

			_stprintf (szHubSequenceNumber, _T("%d%02d%02d%03d"), dwYear, dwMonth, dwDay, dwRev);
				/* sequence number formulation as per Jim Brennan for Microdefs 2.5 is YYYMMDDRRR. The year is NOT 
				   zero padded. Everything else must be */

			CCTRACEI (_T("CNavLu::SetSequenceNumber() - Defs Hub Sequence Number set to %s"), szHubSequenceNumber);
		}

        hRes = m_spAVDefInfo->GetNewestDefsDate(dwYear, dwMonth, dwDay, dwRev);

		if( FAILED(hRes) || dwYear < 2000 )
		{
			CCTRACEE (_T("CNavLu::SetSequenceNumber() - Could not get newest Defs Date. Setting to 0."));
			_tcscpy (szCurrdefsSequenceNumber, _T("0"));
		}
		else
		{
			dwYear -= 2000;
			_stprintf (szCurrdefsSequenceNumber, _T("%d%02d%02d%03d"), dwYear, dwMonth, dwDay, dwRev);
				/* sequence number formulation as per Jim Brennan for Microdefs 2.5 is YYYMMDDRRR. The year is NOT 
				   zero padded. Everything else must be */

			CCTRACEE (_T("CNavLu::SetSequenceNumber() - CurDefs Sequence Number set to %s"), szCurrdefsSequenceNumber);
		}
	}

	HRESULT hResult;
	bstr_t bstrMoniker;
	bstr_t bstrProperty;

	/* set the Hub command line SEQ.HUBDEFS sequence number */
    bstrMoniker = _T(HUB_GUID);
	bstrProperty = _T("SEQ.HUBDEFS");
	bstr_t bstrHubSequenceNumber (szHubSequenceNumber);
	bstr_t bstrCurDefsSeqNumber (szCurrdefsSequenceNumber);
	
	hResult = m_ptrProductReg->SetProperty (bstrMoniker, bstrProperty, bstrHubSequenceNumber);

	if (FAILED (hResult))
	{
		CCTRACEE (_T("CNavLu::SetSequenceNumber() - SetProperty for SEQ.HUBDEFS Failed on Hub. 0x%X"), hResult);
	}

	/* set the hub command line SEQ.CURDEFS sequence number */
	bstrProperty = _T("SEQ.CURDEFS");
	hResult = m_ptrProductReg->SetProperty (bstrMoniker, bstrProperty, bstrCurDefsSeqNumber);

	if (FAILED (hResult))
	{
		CCTRACEE (_T("CNavLu::SetSequenceNumber() - SetProperty for SEQ.CURDEFS Failed on Hub. 0x%X"), hResult);
	}

	/* finally set the SEQ.CURDEFS sequence number on the CurDefs command line */
	bstrMoniker  = _T(CURDEFS_GUID);
	bstrProperty = _T("SEQ.CURDEFS");
	
	hResult = m_ptrProductReg->SetProperty (bstrMoniker, bstrProperty , bstrCurDefsSeqNumber);

	if (FAILED (hResult))
	{
		CCTRACEE (_T("CNavLu::SetSequenceNumber() - SetProperty for SEQ.CURDEFS Failed on CurDefs. 0x%X"), hResult);
	}
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Fetch information from the CmdLine<dwCmdLineNum> registry value.
//
// Parameters:
//   dwCmdLineNum         [in]  Command line number to fetch.
//   pstrProductName      [out] If non-NULL, "ProductName"     value for dwCmdLineNum.
//   pstrProductType      [out] If non-NULL, "ProductType"     value for dwCmdLineNum.
//   pstrProductLanguage  [out] If non-NULL, "ProductLanguage" value for dwCmdLineNum.
//   pstrProductVersion   [out] If non-NULL, "ProductVersion"  value for dwCmdLineNum.
//   pstrOtherSwitches    [out] If non-NULL, "OtherSwitches"   value for dwCmdLineNum.
//
// Returns
//   TRUE  - function was successful.
//   FALSE - function failed. This is generally because a "CmdLine<dwCmdLineNum>"
//           value does not exist in the registry.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::GetCmdLineN (DWORD   dwCmdLineNum,
						  CString *pstrProductName,
						  CString *pstrProductType,
						  CString *pstrProductLanguage,
						  CString *pstrProductVersion,
						  CString *pstrOtherSwitches,
						  CString *pstrProductMoniker,
						  CString *pstrCallbacksHere,
						  CString *pstrGroupTogether,
						  CString *pstrSetSKUProperty,
						  CString *pstrSetEnvironment)
{
	typedef struct
	{
		LPCTSTR szValueName;
		CString *pstrOutput;
	} LOOP_INFO, *PLOOP_INFO;

	LPCTSTR     szKey       = g_szCmdLineNKey;
	LOOP_INFO   pLoopInfo[] = 
	{ 
		{ g_szOtherSwitchesValue,   pstrOtherSwitches },
		{ g_szProductNameValue,     pstrProductName },
		{ g_szProductTypeValue,     pstrProductType },
		{ g_szProductLanguageValue, pstrProductLanguage },
		{ g_szProductVersionValue,  pstrProductVersion },
		{ g_szProductMonikerValue,  pstrProductMoniker },
		{ g_szCallbacksHereValue,  pstrCallbacksHere },
		{ g_szGroupValue,  pstrGroupTogether },
		{ g_szSetSKUValue, pstrSetSKUProperty },
		{ g_szSetEnvironment, pstrSetEnvironment }
	};

	const DWORD      dwLoopInfoLen = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

	auto  TCHAR      szKeyBuf[MAX_PATH];
	auto  TCHAR      szValueBuf[1024];
	auto  DWORD      dwValueBufLen;
	auto  DWORD      dwValueType;
	auto  PLOOP_INFO pCurLoopInfo;

	// Put the command line number into the key name.
	_stprintf (szKeyBuf, szKey, dwCmdLineNum);

	// Open the key.
	CRegKey BaseKey;
	if (ERROR_SUCCESS == BaseKey.Open(HKEY_LOCAL_MACHINE, szKeyBuf, KEY_READ))
	{
		for (pCurLoopInfo = pLoopInfo;
			pCurLoopInfo - pLoopInfo < dwLoopInfoLen;
			++pCurLoopInfo)
		{
			// Skip all values the caller does not care about.
			if (NULL == pCurLoopInfo->pstrOutput)
				continue;

			// Clear the CString.
			pCurLoopInfo->pstrOutput->Empty();

			dwValueBufLen = sizeof (szValueBuf);

			// Fetch the value and add the value to the command line.
			if (ERROR_SUCCESS == RegQueryValueEx (BaseKey, pCurLoopInfo->szValueName, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
			{
				if (dwValueType == REG_SZ && dwValueBufLen)
					*pCurLoopInfo->pstrOutput = szValueBuf;
			}
		}
	}
	else
	{
		// Return FALSE when the CmdLine%d key does not exist.
		return FALSE;
	}


	return TRUE; // Everything processed okay.
}

void GetDescriptiveName( CString strProductName,CString& strProductVersion,CString& strCallbacksHere,CString& strDescriptiveText,CString& strDefaultText )
{
	if ( _ttoi( ( LPCTSTR ) strCallbacksHere ) > 0 )
	{
		strDescriptiveText.LoadString( _Module.GetResourceInstance(), IDS_DESC_NAV );
        return;
	}
	if ( strProductName.Find( _T("Engine") ) >= 0 ) 
	{
		strDescriptiveText.LoadString( _Module.GetResourceInstance(), IDS_DESC_ENGINE );
		return;
	}
	if ( strProductName.Find( _T("Avenge") ) >= 0 ) 
	{
		// Descriptive name for virus def commandlines
		strDescriptiveText.LoadString( _Module.GetResourceInstance(), IDS_DESC_DEFS );
		return;
	}

	strDescriptiveText = strDefaultText;
}

// This function will return TRUE if the given product is safe for use by AutoUpdate
// in silent mode.
BOOL IsProductAutoUpdateSafe( CString strProductName, CString& strCallbacksHere )
{
	if ( _ttoi( ( LPCTSTR ) strCallbacksHere ) > 0 ) 
	{
		return FALSE;
	}
	if ( strProductName.Find( _T("Engine") ) >= 0 ) 
	{
		return TRUE;
	}
	if ( strProductName.Find( _T("Avenge") ) >= 0 ) 
	{
		return TRUE;
	}

	return FALSE;
}

BOOL GetCmdLineMoniker( CString strProductName,CString& strProductVersion,CString& strCallbacksHere,CString& strGroup,_variant_t& vMoniker )
{
	if ( _ttoi( ( LPCTSTR ) strCallbacksHere ) > 0 ) {
		vMoniker = _bstr_t( SOFTWARE_GUID );

		return( TRUE );
	}
	if ( _ttoi( ( LPCTSTR ) strGroup ) > 0 ) {
		if ( strProductVersion.Find( _T("MicroDefsB.CurDefs") ) >= 0 )
			vMoniker = _bstr_t( CURDEFS_GUID );
		else
			vMoniker = _bstr_t( HUB_GUID );

		return( TRUE );
	}

	return( FALSE );
}

// Need to handle re-registration properly.
// Define one of the command lines to be the "default" - this
// will be the one that has the callback registered to it, and
// it will be the one named "Avenge 1.5 MicroDefs".
// As each command line is registered, a property will be saved in
// the registry with the command line to give the moniker associated with it.
// As we retrieve command lines from the registry, if they haven't already been
// registered, they will be registered and their monikers will be saved.
// If they have been registered, the registration will simply be updated
// with the current values.
// This scheme should work well so long as the number of command lines 
// doesn't change.
HRESULT CNavLu::RegisterCommandLines()
{
	if( m_ptrProductReg == NULL )
    {
        CCTRACEE(_T("CNavLu::RegisterCommandLines() - No product reg com object."));
        return E_FAIL;
    }

	LPTSTR lpszCmdLine = NULL;
	HRESULT hr = S_OK;

	try 
	{
		DWORD     dwCmdLineNum = 1;
		BOOL bRegisteredElements;
		CString strProductName;
		CString strProductVersion;
		CString strProductLanguage;
		CString strProductType;
		CString strProductMoniker;
		CString strCallbacksHere;
		CString strGroup;
		CString strSetSKU;
		CString strSetENV;
		CString strDescriptiveText;

		_variant_t vFullDefsMoniker( _bstr_t( FULL_DEFS_GUID ) );

		CComPtr<IluGroup> ptrGroup;
		hr = ptrGroup.CoCreateInstance( __uuidof( luGroup ), NULL, (CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );

		hr = ptrGroup->RegisterGroup( _bstr_t(LUPR_GT_COUPLED), &vFullDefsMoniker );

		while ( GetCmdLineN( dwCmdLineNum, &strProductName, &strProductType,
			&strProductLanguage, &strProductVersion, 
			NULL, &strProductMoniker, &strCallbacksHere, &strGroup, &strSetSKU, &strSetENV ) )
		{
			try 
			{
				_bstr_t strMoniker;
				_variant_t vMoniker;

				try 
				{
					m_ptrProductReg->FindMoniker( _bstr_t( strProductName ),
						_bstr_t( strProductVersion ),
						_bstr_t( strProductLanguage ),
						_bstr_t( _T("") ),
                        &vMoniker);

					bRegisteredElements = ( vMoniker.vt != VT_EMPTY );
				}
				catch ( _com_error &e )
				{
                    CCTRCTXW1(_T("COM Exception: %lu"), e.Error());
					bRegisteredElements = FALSE;
				}

				// strProductMoniker represents what NAV has saved in the registry.
				// vMoniker represents the moniker of something registered 
				// with the given command line (if any).
				// If there is a saved NAV moniker, and it doesn't match
				// the registered moniker for these command line elements, 
				// then delete the possible registration of the NAV saved
				// moniker and make the NAV saved moniker be the one for the
				// matching registered command line elements.
				// All of these conditions were hurting my brain, so 
				// I explicitly spelled out the conditions in the code below -
				// this definitely could be condensed, but it would be harder
				// to understand.
				BOOL bHaveSavedMoniker = !strProductMoniker.IsEmpty();
				BOOL bSavedMatchesRegistered = FALSE;
				BOOL bSavedIsRegistered = FALSE;

				if ( _ttoi( ( LPCTSTR ) strGroup ) > 0 ) 
				{
                    // Make sure the virus def commandlines are grouped together
					hr = ptrGroup->AddToGroup( vFullDefsMoniker.bstrVal, _bstr_t( strProductName ), _bstr_t( strProductVersion ), _bstr_t( strProductLanguage ), _bstr_t( "" ) );
				}

				if ( bHaveSavedMoniker && bRegisteredElements )
				{
					bSavedMatchesRegistered = ( _bstr_t( strProductMoniker ) == _bstr_t( vMoniker ) );
				}
				if ( bHaveSavedMoniker )
				{
					_variant_t vValue;
					//&? BEM - 12/11/98 - using try catch here, since 
					//  GetProperty() might return an error if nothing is registered
					//  at all, but it will only return S_FALSE/S_OK if the given moniker 
					//  exists, but does not have the given property.
					try
					{
						HRESULT hr;

						hr = m_ptrProductReg->GetProperty( _bstr_t( strProductMoniker ), _bstr_t( g_szProduct ), &vValue );
						bSavedIsRegistered = ( hr == S_OK );
					}
					catch ( _com_error e )
					{
						bSavedIsRegistered = FALSE;
					}
				}

				if ( bHaveSavedMoniker )
				{
					if ( bSavedIsRegistered )
					{
						if ( bRegisteredElements )
						{
							if ( !bSavedMatchesRegistered )
							{
								// The command line elements are registered and
								// so is the saved moniker, but they don't match - 
								// THIS IS BAD.
								ATLASSERT( FALSE );
							}
							else
							{
								// Saved matches registered - nothing to do.
                                CCTRACEI(_T("Commandline: %s matches the registered commandline, not re-registering"), strProductName);
								strMoniker = strProductMoniker;
							}
						}
						else
						{
							// Saved is registered, but it's elements have changed
							// so update the registration with the new PVL information.
                            CCTRACEI(_T("Re-registering commandline: \"%s\" \"%s\" \"%s\" \"\""), strProductName, strProductVersion, strProductLanguage);
							strMoniker = strProductMoniker;
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szProduct ), _bstr_t( strProductName ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szVersion ), _bstr_t( strProductVersion ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szLanguage ), _bstr_t( strProductLanguage ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szType ), _bstr_t( _T("") ) );
							GetDescriptiveName( strProductName, strProductVersion, strCallbacksHere, 
								strDescriptiveText, strProductName );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), 
								_bstr_t( strDescriptiveText ) );
						}
					}
					else
					{
						// Saved but not registered as the saved moniker
						if ( bRegisteredElements )
						{
							// Just use registered moniker and save it instead of
							// what we had as the NAV saved moniker.
							// Update the elements of this moniker.
							strProductMoniker = LPCTSTR( _bstr_t( vMoniker ) );
							strMoniker = strProductMoniker;

                            // Save off the moniker for next time
                            ATLASSERT( !strProductMoniker.IsEmpty() );
				            SetCmdLineN( dwCmdLineNum, NULL, NULL, NULL, NULL, NULL, &strProductMoniker );

							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szProduct ), _bstr_t( strProductName ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szVersion ), _bstr_t( strProductVersion ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szLanguage ), _bstr_t( strProductLanguage ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szType ), _bstr_t( "" ) );
							GetDescriptiveName( strProductName,strProductVersion,strCallbacksHere,strDescriptiveText,strProductName );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), _bstr_t( strDescriptiveText ) );

                            CCTRACEE(_T("RegisterCommandLines() - The saved moniker for PVL: \"%s\" \"%s\" \"%s\" \"\" is not the same as the registered one. Re-registering it now."), strProductName, strProductVersion, strProductLanguage);

                            // if we're supposed to set the SKU, do so
				            if ( _ttoi( ( LPCTSTR ) strSetSKU ) > 0 ) 
				            {
					            try
                                {
                                    // Get license type from navopts.dat
                                    TCHAR szSKU[MAX_PATH] = {0};
                                    CNAVOptSettingsEx NAVOptSettings;
                                    NAVOptSettings.Init();
                                    DWORD dwSize = MAX_PATH;
                                    NAVOptSettings.GetValue(_T("SKU:PAIDSKU"), szSKU, dwSize, _T(""));
                                    m_ptrProductReg->SetProperty( strMoniker, _bstr_t( _T("SKU") ), _bstr_t( szSKU ) );
                                }
                                catch ( _com_error e )
	                            {
		                            CCTRACEE( _T("CNavLu::SetSkuProp() - caught exception") );
                                    return E_FAIL;
	                            }
				            }

                            //if we're supposed to set the ENVIRONMENT to RETAIL, do so.
				            if ( _ttoi( ( LPCTSTR ) strSetENV ) > 0 ) 
				            {
                                SetEnvProp();
				            }

							// Set the AutoUpdate flag for this command line.
							if ( IsProductAutoUpdateSafe( strProductName, strCallbacksHere ) )
							{                                                
								m_ptrProductReg->SetProperty( strMoniker, 
									_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
									_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
								m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
									_bstr_t( AU_REG_PATCH_SILENT ) );
							}
							else
							{
								if ( strProductName.Find( _T("NAV") ) >= 0 )
								{
									m_ptrProductReg->SetProperty( strMoniker, 
										_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
										_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
									m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
										_bstr_t( AU_REG_PATCH_NOISY ) );								
								}

								else
								{	
									m_ptrProductReg->SetProperty( strMoniker,		
										_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
										_bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) );
									m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
										_bstr_t( AU_REG_PATCH_NOISY ) );
								}
							}
						}
						else
						{
							// Saved moniker is not registered and neither is there
							// a registration for these command line elements.
							// Register anew and save the new moniker as the
							// saved NAV moniker.

							GetCmdLineMoniker( strProductName,strProductVersion,strCallbacksHere,strGroup,vMoniker );

                            CCTRACEE(_T("RegisterCommandLines() - The saved moniker for PVL: \"%s\" \"%s\" \"%s\" \"\" is not registered. Re-registering it now"), strProductName, strProductVersion, strProductLanguage);

							hr = m_ptrProductReg->RegisterProduct( _bstr_t( strProductName ), 
								_bstr_t( strProductVersion ), 
								_bstr_t( strProductLanguage ),
								_bstr_t( _T("") ),
								&vMoniker );
							strMoniker = _bstr_t( vMoniker );                   
							strProductMoniker = LPCTSTR( strMoniker );
							GetDescriptiveName( strProductName,strProductVersion,strCallbacksHere,strDescriptiveText,strProductName );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), _bstr_t( strDescriptiveText ) );

				            // Save off the moniker for next time
                            ATLASSERT( !strProductMoniker.IsEmpty() );
				            SetCmdLineN( dwCmdLineNum, NULL, NULL, NULL, NULL, NULL, &strProductMoniker );

                            // if we're supposed to set the SKU, do so
				            if ( _ttoi( ( LPCTSTR ) strSetSKU ) > 0 ) 
				            {
					            try
                                {
                                    // Get license type from navopts.dat
                                    TCHAR szSKU[MAX_PATH] = {0};
                                    CNAVOptSettingsEx NAVOptSettings;
                                    NAVOptSettings.Init();
                                    DWORD dwSize = MAX_PATH;
                                    NAVOptSettings.GetValue(_T("SKU:PAIDSKU"), szSKU, dwSize, _T(""));
                                    m_ptrProductReg->SetProperty( strMoniker, _bstr_t( _T("SKU") ), _bstr_t( szSKU ) );
                                }
                                catch ( _com_error e )
	                            {
		                            CCTRACEE( _T("CNavLu::SetSkuProp() - caught exception") );
                                    return E_FAIL;
	                            }
				            }

                            //if we're supposed to set the ENVIRONMENT to RETAIL, do so.
				            if ( _ttoi( ( LPCTSTR ) strSetENV ) > 0 ) 
				            {
                                SetEnvProp();
				            }

							// Set the AutoUpdate flag for this command line.
							if ( IsProductAutoUpdateSafe( strProductName, strCallbacksHere ) )
							{
								m_ptrProductReg->SetProperty( strMoniker, 
									_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
									_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
								m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
									_bstr_t( AU_REG_PATCH_SILENT ) );
							}
							else
							{
								if ( strProductName.Find( _T("NAV") ) >= 0 )
								{
									m_ptrProductReg->SetProperty( strMoniker, 
										_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
										_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
									m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
										_bstr_t( AU_REG_PATCH_NOISY ) );								
								}
								else
								{		
									m_ptrProductReg->SetProperty( strMoniker,		
										_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
										_bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) );
									m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
										_bstr_t( AU_REG_PATCH_NOISY ) );
								}
							}

                            // If this is this has callbacks register it now
                            if ( _ttoi( ( LPCTSTR ) strCallbacksHere ) > 0 )
				            {
					            strMoniker = _bstr_t( strProductMoniker );
					            // Register the callback on the default command line
					            hr = m_ptrProductReg->RegisterCallback( strMoniker, _uuidof( luNavCallBack ), (tagLU_CALLBACK_TYPE)(PreSession | PostSession | WelcomeText | FinishText ) );
				            }
						}
					}
				}
				else 
				{
					// Don't have a saved moniker 
					if ( bRegisteredElements )
					{
						// Just use registered moniker and save it as the 
						// NAV saved moniker.
						// The elements are already registered, just under
						// a different moniker, so switch to that moniker
						// and save it.
						strProductMoniker = LPCTSTR( _bstr_t( vMoniker ) );
                        
				        // Save off the moniker for next time
                        ATLASSERT( !strProductMoniker.IsEmpty() );
				        SetCmdLineN( dwCmdLineNum, NULL, NULL, NULL, NULL, NULL, &strProductMoniker );
					}
					else // No saved, no registered 
					{
                        CCTRACEE(_T("RegisterCommandLines() - There is no PVL registered for commandline: \"%s\" \"%s\" \"%s\" \"\". Re-registering now"), strProductName, strProductVersion, strProductLanguage);
						// Register anew and save the new moniker as the saved
						// NAV moniker.
						GetCmdLineMoniker( strProductName,strProductVersion,strCallbacksHere,strGroup,vMoniker );
						hr = m_ptrProductReg->RegisterProduct( _bstr_t( strProductName ), 
							_bstr_t( strProductVersion ), 
							_bstr_t( strProductLanguage ),
							_bstr_t( _T("") ),
							&vMoniker );
						strMoniker = _bstr_t( vMoniker );                   
						strProductMoniker = LPCTSTR( strMoniker );
						// Register a descriptive name for final status messages
						GetDescriptiveName( strProductName,strProductVersion,strCallbacksHere,strDescriptiveText,strProductName );
						m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), _bstr_t( strDescriptiveText ) );

						// Set the AutoUpdate flag for this command line.
						if ( IsProductAutoUpdateSafe( strProductName, strCallbacksHere ) )
						{
							m_ptrProductReg->SetProperty( strMoniker, 
								_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
								_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
								_bstr_t( AU_REG_PATCH_SILENT ) );
						}
						else
						{
							if ( strProductName.Find( _T("NAV") ) >= 0 )
							{
								m_ptrProductReg->SetProperty( strMoniker, 
									_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
									_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
								m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
									_bstr_t( AU_REG_PATCH_NOISY ) );
							}
							else
							{
								m_ptrProductReg->SetProperty( strMoniker, 
									_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
									_bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) );
								m_ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
									_bstr_t( AU_REG_PATCH_NOISY ) );
							}
						}
					}

				}
			}
			catch ( _com_error e )
			{
				CCTRACEE(_T("RegisterCommandLines() - ERROR! Caught COM exception registering command line"));
			}
			++dwCmdLineNum;
			strProductMoniker.Empty();
		}

	}
	catch ( _com_error e )
	{
		CCTRACEE( _T("RegisterCommandLines() - ERROR! caught exception") );
	}

    if( lpszCmdLine )
	    delete [] lpszCmdLine;

	return hr;
}

HRESULT CNavLu::SetEnvProp()
{
    try
	{
        CComPtr<IstCheckForUpdates>		ptrLiveUpdate;

		// Create the the LiveUpdate CheckForUpdates object
		HRESULT hr = ptrLiveUpdate.CoCreateInstance( __uuidof( stCheckForUpdates ), NULL, 
										CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER );
		if ( FAILED( hr ) )
			throw hr;

     	// Load the settings file from disk.
		hr = ptrLiveUpdate->LoadSettings();
		if ( FAILED( hr ) )
			throw hr;
    
		// Get a pointer to the Settings file object.
		CComPtr<IstSettings> ptrSettings;
        hr = ptrLiveUpdate->get_Settings(&ptrSettings);
		if ( FAILED(hr) || ptrSettings == NULL )
			throw E_FAIL;

		// Set the Environment property
        ptrSettings->put_Property( bstrPREFS_ENVIRONMENT, bstrENVIROMENT_RETAIL );
	}
	catch(_com_error e)
	{
		// Handle fatal errors here.  I wouldn't do more than log an error here.  This failure will not be fatal to LiveUpdate or your product's 
		// registration, unless LiveUpdate is not correctly installed.  If LiveUpdate is not working, then your Product Registration code will
		// probably already handle this type of error correctly.
        CCTRACEE(_T("Error attempting to set the ENVIRONMENT setting for LiveUpdate. Error: %s"), e.Description());
        return e.Error();
	}
    catch(exception &ex)
    {
        CCTRCTXE1(_T("Unknown exception: %s"), ex.what());
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CNavLu::UnregisterCommandLines( void )
{
	HRESULT hr;

	// Check the LU dll's for a valid symantec signature
	if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(__uuidof(luGroup)) )
	{
		CCTRACEE( _T("CNavLu::UnregisterCommandLines - Symantec digital signature check failed on product reg dll.\r\n") );
		return E_ACCESSDENIED;
	}

	CComPtr<IluGroup> ptrGroup;
	hr = ptrGroup.CoCreateInstance( __uuidof( luGroup ), NULL, (CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );

	// If CreateInstance() failed, LU was uninstalled before NAV, so
	// bail out.
	if ( FAILED(hr) )
	{
		return E_FAIL;
	}

	hr = ptrGroup->DeleteGroup( _bstr_t( FULL_DEFS_GUID ) );

	hr = S_OK;

	if( m_ptrProductReg == NULL )
    {
        CCTRACEE(_T("CNavLu::UnregisterCommandLines() - No product reg com object."));
        return E_FAIL;
    }

	LPTSTR lpszCmdLine = NULL;

	try 
	{
		DWORD     dwCmdLineNum = 1;
		CString strProductName;
		CString strProductVersion;
		CString strProductLanguage;
		CString strProductType;
		CString strProductMoniker;
        
		// Check the LU dll for a valid symantec signature
		if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(__uuidof(luProductReg)) )
		{
			CCTRACEE( _T("CNavLu::UnregisterCommandLines - Symantec digital signature check failed on product reg dll") );
			return E_ACCESSDENIED;
		}

		while ( GetCmdLineN( dwCmdLineNum, &strProductName, &strProductType,
			&strProductLanguage, &strProductVersion, 
			NULL, &strProductMoniker, NULL, NULL, NULL, NULL ) )
		{
			m_ptrProductReg->DeleteProduct( _bstr_t( strProductMoniker ) );

			++dwCmdLineNum;
			strProductMoniker.Empty();
		}
	}
	catch ( _com_error &ex )
	{
        CCTRCTXE1( _T("COM exception: %lu"), ex.Error() );
		hr = E_FAIL;
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set information from the CmdLine<dwCmdLineNum> registry value.
//
// Parameters:
//   dwCmdLineNum         [in] Command line number to set.
//   pstrProductName      [in] If non-NULL, "ProductName"     value for dwCmdLineNum.
//   pstrProductType      [in] If non-NULL, "ProductType"     value for dwCmdLineNum.
//   pstrProductLanguage  [in] If non-NULL, "ProductLanguage" value for dwCmdLineNum.
//   pstrProductVersion   [in] If non-NULL, "ProductVersion"  value for dwCmdLineNum.
//   pstrOtherSwitches    [in] If non-NULL, "OtherSwitches"   value for dwCmdLineNum.
//
// Returns
//   TRUE  - function was successful.
//   FALSE - function failed. This is generally because a "CmdLine<dwCmdLineNum>"
//           value does not exist in the registry.
//////////////////////////////////////////////////////////////////////////
// static function
BOOL CNavLu::SetCmdLineN (DWORD   dwCmdLineNum,
						  CString *pstrProductName,
						  CString *pstrProductType,
						  CString *pstrProductLanguage,
						  CString *pstrProductVersion,
						  CString *pstrOtherSwitches,
						  CString *pstrProductMoniker)
{
	typedef struct
	{
		LPCTSTR szValueName;
		CString *pstrOutput;
	} LOOP_INFO, *PLOOP_INFO;

	LPCTSTR     szKey       = g_szCmdLineNKey;
	LOOP_INFO   pLoopInfo[] = 
	{ 
		{ g_szOtherSwitchesValue,   pstrOtherSwitches },
		{ g_szProductNameValue,     pstrProductName },
		{ g_szProductTypeValue,     pstrProductType },
		{ g_szProductLanguageValue, pstrProductLanguage },
		{ g_szProductVersionValue,  pstrProductVersion },
		{ g_szProductMonikerValue,  pstrProductMoniker }
	};

	const DWORD      dwLoopInfoLen = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

	auto  TCHAR      szKeyBuf[MAX_PATH];
	auto  DWORD      dwValueBufLen;
	auto  PLOOP_INFO pCurLoopInfo;
	auto  BOOL       bResult = FALSE;
	auto  TCHAR     *pszValueBuf;


	// Put the command line number into the key name.
	_stprintf (szKeyBuf, szKey, dwCmdLineNum);

	// Open the key.
	CRegKey BaseKey;
	if (ERROR_SUCCESS == BaseKey.Open(HKEY_LOCAL_MACHINE, szKeyBuf, KEY_WRITE))
	{
		for (pCurLoopInfo = pLoopInfo;
			pCurLoopInfo - pLoopInfo < dwLoopInfoLen;
			++pCurLoopInfo)
		{
			// Skip all values the caller does not care about.
			if (NULL == pCurLoopInfo->pstrOutput)
				continue;

			dwValueBufLen = pCurLoopInfo->pstrOutput->GetLength();
			pszValueBuf = pCurLoopInfo->pstrOutput->GetBuffer(dwValueBufLen);

			// Set the new LiveUpdate ProductType value
			if (ERROR_SUCCESS == BaseKey.SetStringValue(pCurLoopInfo->szValueName,pszValueBuf) )
			{
				bResult = TRUE;
			}
		}


	}

	return (bResult);

}

//////////////////////////////////////////////////////////////////////////
//
// Description:
//   Update the binary hub microdefs version with the one we retrieved in
//   m_spBinaryPatchVersion
//
// Parameters:
//   pszVersion [in]  string containing the new MicroDef Version
//
// Returns:
//   TRUE    when the Version is successfully updated.
//   FALSE
//
//////////////////////////////////////////////////////////////////////////

BOOL CNavLu::UpdateMicroDefHubVersion()
{
	DWORD     dwCmdLineNum = 1;
	CString   strProductMoniker;
	CString   strProductVersion;
	BOOL      bFound;

    CString strHubMonikerToFind(HUB_GUID);

	if ( m_spBinaryPatchVersion == NULL )            
    {
        CCTRACEE(_T("CNavLu::UpdateMicroDefHubVersion(): missing binary hub patch version string"));
        return FALSE;
    }

    CString strNewVersion(m_spBinaryPatchVersion->GetStringW());

	// Find the hub command line (looking for the HUB moniker match from the registry commandlines)
	bFound = FALSE;
	while ( !bFound &&
		GetCmdLineN( dwCmdLineNum, NULL, NULL, NULL, &strProductVersion, NULL, &strProductMoniker, NULL, NULL, NULL, NULL ) )
	{
		if ( 0 == strHubMonikerToFind.CompareNoCase(strProductMoniker) )
        {
            CCTRACEI(L"CNavLu::UpdateMicroDefHubVersion(): old registry microdefs HUB version: %s  New: %s", strProductVersion.GetString(), strNewVersion.GetString());
			bFound = TRUE;
        }

		if ( !bFound )
			++dwCmdLineNum;
	}

	if ( bFound )
	{
		// Update the Version field in the registry commandline
		SetCmdLineN ( dwCmdLineNum, NULL, NULL, NULL, &strNewVersion, NULL, NULL );
		return TRUE;
	}

    CCTRACEE(_T("CNavLu::UpdateMicroDefHubVersion(): Could not find viurs defs HUB moniker %s in registry"), strHubMonikerToFind.GetString());
	return FALSE;
}

BOOL CNavLu::GetVirusDefsDelivered()
{
	// Check with defutils to see if there are newer defs on the system than we started
    // with

    HRESULT hr = S_OK;
    if( FAILED(hr = m_spAVDefInfo->GetDefsDate(postDefs.dwYear, postDefs.dwMonth, postDefs.dwDay, postDefs.dwRev)) )
    {
        CCTRACEE(_T("CNavLu::GetVirusDefsDelivered() - GetNewestDefsDate() failed assuming new defs were not delivered. hr = 0x%X"), hr);
        return FALSE;
    }

    // Compare the initial dates with the new ones
    if( postDefs.dwYear > initialDefs.dwYear || 
        postDefs.dwMonth > initialDefs.dwMonth || 
        postDefs.dwDay > initialDefs.dwDay || 
        postDefs.dwRev > initialDefs.dwRev )
    {
        CCTRACEI(_T("CNavLu::GetVirusDefsDelivered() - newer defs were delivered: %d%02d%02d%03d"), postDefs.dwYear, postDefs.dwMonth, postDefs.dwDay, postDefs.dwRev);
        return TRUE;
    }
    else
    {
        CCTRACEE(_T("CNavLu::GetVirusDefsDelivered() - newer defs were NOT delivered. Newest defs are still: %d%02d%02d%03d"), initialDefs.dwYear, initialDefs.dwMonth, initialDefs.dwDay, initialDefs.dwRev);
        return FALSE;
    }
}
