// NavLu.cpp: implementation of the CNavLu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SIMON.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepClientBase.h"
#include "cltPepConstants.h"

#include "navtrust.h"
#include "resource.h"
#include "LiveUpdate.h"
#include "NAVDefutilsLoader.h"
#include "defutilsinterface.h"

#include "NavLu.h"
#include "strings.h"
#include "OptNames.h"
#include "AVRESBranding.h"

#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

#include "NAVSettingsHelperEx.h"
#include "NAVInfo.h"
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
                CCTRACEE("CNavLu::RegisterCmdLines - Error: Unable to initialize CNavLu");
            }
            else
			    navLu.RegisterCommandLines( DefAnnuitySuccess );
		}
		catch(...)
		{}
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
    ZeroMemory(m_szPatchDirectDir, sizeof(m_szPatchDirectDir));
    ZeroMemory(m_szBinaryPatchDir, sizeof(m_szBinaryPatchDir));
    ZeroMemory(m_szBinaryPatchVersion, sizeof(m_szBinaryPatchVersion));
    ZeroMemory(m_szPatch25DllPath, sizeof(m_szPatch25DllPath));
    ZeroMemory(&initialDefs, sizeof(DefsDateStruct));
    ZeroMemory(&postDefs, sizeof(DefsDateStruct));
}

CNavLu::~CNavLu()
{
    if( m_pDefUtils.m_p )
        m_pDefUtils.Release();

    if( m_ptrProductReg != NULL )
        m_ptrProductReg = NULL;
}

BOOL CNavLu::Initialize()
{
	// Get product name
	CBrandingRes BrandRes;
	m_strBrandingProductName = BrandRes.ProductName();

	// Get the full path to patch25d.dll
    CNAVInfo navInfo;
    _stprintf(m_szPatch25DllPath, _T("%s\\Patch25d.dll"), navInfo.GetNAVDir());

    // Create a new defutils object
    if( m_pDefUtils.m_p )
    {
        CCTRACEI("CNavLu::Initialize - A defutils object exists...releasing it and creating a new one");
        m_pDefUtils.Release();
    }

    if( SYM_FAILED(m_DefUtilsLoader.CreateObject(m_pDefUtils.m_p)) )
    {
        CCTRACEE("CNavLu::Initialize - Error: Unable to load the DefUtils interface object");
        return FALSE;
    }

    // Save off the current defs date so we know if there is a newer one installed
    // after the postmicrodef update
    if( !m_pDefUtils->GetNewestDefsDate(&initialDefs.wYear, &initialDefs.wMonth, &initialDefs.wDay, &initialDefs.dwRev) )
    {
        // If this fails set them all to 0
        CCTRACEE("CNavLu::Initialize() - GetNewestDefsDate() failed to return defs date");
        ZeroMemory(&initialDefs, sizeof(DefsDateStruct));
    }
    else
        CCTRACEI("CNavLu::Initialize() - Current defs returned by GetNewestDefsDate are: %d%02d%02d%03d", initialDefs.wYear, initialDefs.wMonth, initialDefs.wDay, initialDefs.dwRev);

    // Create a product reg com object
    if ( m_ptrProductReg == NULL )
	{
        // Check the LU dll for a valid symantec signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(__uuidof(luProductReg)) )
        {
            CCTRACEE( "CNavLu::Initialize() - Symantec digital signature check failed on product reg dll." );
            return FALSE;
        }

        HRESULT hr = S_OK;
		if ( FAILED( hr = m_ptrProductReg.CoCreateInstance( __uuidof( luProductReg ), NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER )) ) )
        {
			CCTRACEE( "CNavLu::Initialize() - Failed to create the product reg com object. Error = 0x%X", hr );
            return FALSE;
        }
	}

    return TRUE;
}

// Pulled from DoLiveUpdate() and modified for LUAll purposes
BOOL CNavLu::PreProcess( )
{
    CString     strCmdLine;
    BOOL        bSuccess                   = FALSE;
    BOOL        bAllowContentDownload      = FALSE;
    BOOL        bVerbose                   = !IsLiveUpdateInSilentMode();

    STAHLSOFT_HRX_TRY(hr)
    {
        DWORD dwSubscriptionDisposition = SUBSCRIPTION_STATIC::DISPOSITION_FORCE_VERIFICATION;

        // Set verbose
        if(bVerbose)
            dwSubscriptionDisposition |= SUBSCRIPTION_STATIC::DISPOSITION_VERBOSE;

		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(V2LicensingAuthGuids::GUID_AntiVirus_Component);
		pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		pepBase.SetPolicyID(AntiVirusComponent::POLICY_ID_PAID_CONTENT_VDEF_UPDATES);
		pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_POSSIBLY_ONLINE);
		pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,dwSubscriptionDisposition);

        // Getting subscription properties needed to enable downloads
        hrx << pepBase.QueryPolicy();

		DWORD dwBoolAllowContent = (DWORD) FALSE;

        // Getting Subscription properties
        hrx << pepBase.GetValueFromCollection(SIMON_PEP::SZ_MANAGEMENT_PROPERTY_DISCRETE, (DWORD &)dwBoolAllowContent, 0);

		if(dwBoolAllowContent)
		{
			// 1-4NJCEJ
			// If PEP says everything's OK, we'll do an additional check to
			// make sure that NAVOPTRF is the shipping (post-beta) version.

			bool bNavoptrfOk = false;

			CString strPath;
			size_t nSize = _MAX_PATH;
			if (ccSym::CNAVPathProvider::GetPath(strPath.GetBuffer(nSize), nSize))
			{
				strPath.ReleaseBuffer();		// Necessary, not just to trim
				strPath += _T("\\NAVOPTRF.DLL");

				ccLib::CModule modNavoptrf;
				if (modNavoptrf.Load(strPath, LOAD_WITH_ALTERED_SEARCH_PATH))
				{
					GETAUTHGUID pGetAuthGuid =
						(GETAUTHGUID)modNavoptrf.GetProc( _T("GetAuthGUID"));
					if (pGetAuthGuid)
					{
						GUID g;
						pGetAuthGuid(g);
						if (g == GUID_NAVOPTRF_AUTH)
							bNavoptrfOk = true;
					}
				}
			}

			if (bNavoptrfOk)
				bAllowContentDownload = TRUE;
			else
				CCTRACEE(_T(__FUNCTION__ " : Failed to authenticate NAVOPTRF.DLL"));
		}

		CCTRACEI(_T("CNavLu::PreProcess - bAllowContentDownload: %d"), bAllowContentDownload);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

    // Allow only patches, no defs.
    m_DefAnnuityStatus = DefAnnuityUpdatePatches;
    if(bAllowContentDownload)
    {
        // unless we can also download paid content
        m_DefAnnuityStatus = DefAnnuityUpdateAll; // upgrade to all
    }
    else
    {
        // if we aren't downloading defs then we don't need the rest of this stuff
        CCTRACEW(_T("CNavLu::PreProcess - Subscription check not allowing def download. bailing out of preprocess"));
        AbortDefsCommandlines( IDS_ERROR_SUBSCRIPTION_EXPIRED );
        return TRUE;
    }

    CCTRACEI(_T("CNavLu::PreProcess - m_DefAnnuityStatus: %d"), m_DefAnnuityStatus);

    // Parameters for PreMicroDef25Update only used here
    bool bIsBinaryHubOK = false;

    // Enable the definitions authentication checking
    m_pDefUtils->SetDoAuthenticationCheck(true);

    // PreMicroDefUpdate sets up the Hawking environment
    bSuccess = m_pDefUtils->PreMicroDef25Update(m_szBinaryPatchDir, sizeof(m_szBinaryPatchDir),
                                                m_szPatchDirectDir, sizeof(m_szPatchDirectDir),
                                                m_szBinaryPatchVersion, sizeof(m_szBinaryPatchVersion),
                                                &bIsBinaryHubOK,
                                                DU_LIVEUPDATE);

    // If pre def update succeed make the appropriate modification to our commandlines
    if (bSuccess)
    {
        CCTRACEI("CNavLu::PreProcess() - PreMicroDef25Update() returned success. Binary patch version is: %s", m_szBinaryPatchVersion);
        
        // If either hub directory is "broken" (files are missing) then set it's
        // sequence number to zero to force LiveUpdate to get it.
        BOOL bSetSeqNumToZero = bIsBinaryHubOK ? FALSE : TRUE;
        if (!bIsBinaryHubOK)
        {
            CCTRACEE("CNavLu::PreProcess() - According to PreMicroDef25Update() the binary hub is NOT ok...going to set the sequence number to 0");
        }

        // Update the microdef version in the registry so that the call to RegisterCommandLines will update it
        // with LU
        EditMicroDefVersion(m_szBinaryPatchVersion);
        
        // Re-register any changed/updated/missing commandlines that we have in the registry
        CNavLu::RegisterCommandLines( m_DefAnnuityStatus );

        // Set the sequence numbers for the virus defs
        SetSequenceNumber( bSetSeqNumToZero );
    }
    else
    {
        // Log the error returned by PreMicroDef25Update()
        DURESULT duRes = m_pDefUtils->GetLastResult();
        char szMsg[255] = {0};
        m_pDefUtils->DuResultToString(duRes, szMsg, 255);
        CCTRACEE("CNavLu::PreProcess() - PreMicroDef25Update() failed with error: %s (%d).", szMsg, duRes);
    }

	return bSuccess;
}

// Pulled from DoLiveUpdate() and modified for LUAll purposes
HRESULT CNavLu::PostProcess( )
{
	BOOL      bMicroDefsDelivered = FALSE;

    CCTRACEI("CNavLu::PostProcess() - PostMicroDef25Update() being called with patch25.dll path: %s", m_szPatch25DllPath);
	if( !m_pDefUtils->PostMicroDef25Update(m_szPatch25DllPath, NULL, TRUE) )
	{
        // Check to see if definition authentication failed while integrating the defs
        DURESULT duRes = m_pDefUtils->GetLastResult();
        char szMsg[255] = {0};
        m_pDefUtils->DuResultToString(duRes, szMsg, 255);
        CCTRACEE("CNavLu::PostProcess() - PostMicroDef25Update() failed with error: %s (%d). Aborting commandline.", szMsg, duRes);
        if( duRes == DU_E_AUTH )
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
         CCTRACEI("CNavLu::PostProcess() - PostMicroDef25Update() succeeded");

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
    CCTRACEI("CNavLu::AbortDefsCommandlines - Aborting commandlines with RESID: %d",unAbortTextResID);
	try
	{
        if( m_ptrProductReg == NULL )
        {
            CCTRACEE("CNavLu::AbortDefsCommandlines() - No product reg com object.");
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
			CCTRACEE( "CNavLu::AbortDefsCommandlines - Failed to set the abort property for the debs hub" );
			return FALSE;
		}

		// Set the abort text
		if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( g_szAbortText ), bstrAbortText.m_str)) )
		{
			CCTRACEE( "CNavLu::AbortDefsCommandlines - Failed to set the abort text property." );
			return FALSE;
		}

        // Set the abort error ID
        if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( LUPR_ABORT_ERR_CODE ), _bstr_t(unAbortTextResID))) )
		{
			CCTRACEE( "CNavLu::AbortDefsCommandlines - Failed to set the abort error ID property." );
			return FALSE;
		}

        //
        // Abort the curdefs
        //
        bstrMoniker = _T(CURDEFS_GUID);

        // Set the abort flag
		if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( g_szAbort ), _bstr_t( "YES" ))) )
		{
			CCTRACEE( "CNavLu::AbortDefsCommandlines - Failed to set the abort property for the debs hub" );
			return FALSE;
		}

		// Set the abort text
		if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( g_szAbortText ), bstrAbortText.m_str)) )
		{
			CCTRACEE( "CNavLu::AbortDefsCommandlines - Failed to set the abort text property." );
			return FALSE;
		}

        // Set the abort error ID
        if( FAILED(m_ptrProductReg->SetProperty( bstrMoniker, _bstr_t( LUPR_ABORT_ERR_CODE ), _bstr_t(unAbortTextResID))) )
		{
			CCTRACEE( "CNavLu::AbortDefsCommandlines - Failed to set the abort error ID property." );
			return FALSE;
		}
	}
	catch(...)
	{
		CCTRACEE( "CNavLu::AbortDefsCommandlines - Unhandled exception attempting to abort." );
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set the LiveUpdate sequence number based on the current virus
//   definition version information.
//////////////////////////////////////////////////////////////////////////
void CNavLu::SetSequenceNumber (BOOL bSetToZero)
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CNavLu::SetSequenceNumber() - No defutils object.");
        return;
    }

    if( m_ptrProductReg == NULL )
    {
        CCTRACEE("CNavLu::SetSequenceNumber() - No product reg com object.");
        return;
    }

    /* determine the sequence number to set the LU command line to based on the defs installed */

	TCHAR szHubSequenceNumber [30];
	TCHAR szCurrdefsSequenceNumber [30];

	if (bSetToZero)
	{
		/* the hub is damaged */
		_tcscpy (szHubSequenceNumber, "0");
		_tcscpy (szCurrdefsSequenceNumber, "0");
	}
	else
	{
		WORD wYear=0, wMonth=0, wDay=0;
        DWORD dwRev=0;
		BOOL bResult;

		bResult = m_pDefUtils->GetBinHubDefsDate (&wYear,
												  &wMonth,
												  &wDay,
												  &dwRev);

		if ((!bResult) || (wYear < 2000))
		{
			CCTRACEE ("CNavLu::SetSequenceNumber() - Could not get BinHub Date. Setting to 0.");
			_tcscpy (szHubSequenceNumber, "0");
		}
		else
		{
			wYear -= 2000;

			_stprintf (szHubSequenceNumber, "%d%02d%02d%03d", wYear, wMonth, wDay, dwRev);
				/* sequence number formulation as per Jim Brennan for Microdefs 2.5 is YYYMMDDRRR. The year is NOT 
				   zero padded. Everything else must be */

			CCTRACEI ("CNavLu::SetSequenceNumber() - Defs Hub Sequence Number set to %s", szHubSequenceNumber);
		}

		bResult = m_pDefUtils->GetNewestDefsDate (&wYear,
												   &wMonth,
												   &wDay,
												   &dwRev);

		if ((!bResult) || (wYear < 2000))
		{
			CCTRACEE ("CNavLu::SetSequenceNumber() - Could not get newest Defs Date. Setting to 0.");
			_tcscpy (szCurrdefsSequenceNumber, "0");
		}
		else
		{
			wYear -= 2000;
			_stprintf (szCurrdefsSequenceNumber, "%d%02d%02d%03d", wYear, wMonth, wDay, dwRev);
				/* sequence number formulation as per Jim Brennan for Microdefs 2.5 is YYYMMDDRRR. The year is NOT 
				   zero padded. Everything else must be */

			CCTRACEE ("CNavLu::SetSequenceNumber() - CurDefs Sequence Number set to %s", szCurrdefsSequenceNumber);
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
		CCTRACEE ("CNavLu::SetSequenceNumber() - SetProperty for SEQ.HUBDEFS Failed on Hub. 0x%X", hResult);
	}

	/* set the hub command line SEQ.CURDEFS sequence number */
	bstrProperty = _T("SEQ.CURDEFS");
	hResult = m_ptrProductReg->SetProperty (bstrMoniker, bstrProperty, bstrCurDefsSeqNumber);

	if (FAILED (hResult))
	{
		CCTRACEE ("CNavLu::SetSequenceNumber() - SetProperty for SEQ.CURDEFS Failed on Hub. 0x%X", hResult);
	}

	/* finally set the SEQ.CURDEFS sequence number on the CurDefs command line */
	bstrMoniker  = _T(CURDEFS_GUID);
	bstrProperty = _T("SEQ.CURDEFS");
	
	hResult = m_ptrProductReg->SetProperty (bstrMoniker, bstrProperty , bstrCurDefsSeqNumber);

	if (FAILED (hResult))
	{
		CCTRACEE ("CNavLu::SetSequenceNumber() - SetProperty for SEQ.CURDEFS Failed on CurDefs. 0x%X", hResult);
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
	if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 )
	{
		strDescriptiveText.LoadString( _Module.GetResourceInstance(), IDS_DESC_NAV );
        return;
	}
	if ( strProductName.Find( "Engine" ) >= 0 ) 
	{
		strDescriptiveText.LoadString( _Module.GetResourceInstance(), IDS_DESC_ENGINE );
		return;
	}
	if ( strProductName.Find( "Avenge" ) >= 0 ) 
	{
		// Descriptive name for virus def commandlines
		strDescriptiveText.LoadString( _Module.GetResourceInstance(), IDS_DESC_DEFS );
		return;
	}

	if ( strProductName.Find( "Scan" ) >= 0 ) 
	{
		strDescriptiveText.LoadString( _Module.GetResourceInstance(), IDS_DESC_SCAN_DELIVER );
		return;
	}

	strDescriptiveText = strDefaultText;
}

// This function will return TRUE if the given product is safe for use by AutoUpdate
// in silent mode.
BOOL IsProductAutoUpdateSafe( CString strProductName, CString& strCallbacksHere )
{
	if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 ) 
	{
		return FALSE;
	}
	if ( strProductName.Find( "Engine" ) >= 0 ) 
	{
		return TRUE;
	}
	if ( strProductName.Find( "Avenge" ) >= 0 ) 
	{
		return TRUE;
	}

	return FALSE;
}

BOOL GetCmdLineMoniker( CString strProductName,CString& strProductVersion,CString& strCallbacksHere,CString& strGroup,_variant_t& vMoniker )
{
	if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 ) {
		vMoniker = _bstr_t( SOFTWARE_GUID );

		return( TRUE );
	}
	if ( atoi( ( LPCSTR ) strGroup ) > 0 ) {
		if ( strProductVersion.Find( "MicroDefsB.CurDefs" ) >= 0 )
			vMoniker = _bstr_t( CURDEFS_GUID );
		else
			vMoniker = _bstr_t( HUB_GUID );

		return( TRUE );
	}
	if ( strProductName.Find( "Scan" ) >= 0 ) {
		vMoniker = _bstr_t( SCAN_DELIVER_GUID );

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
HRESULT CNavLu::RegisterCommandLines( DWORD DefAnnuityStatus )
{
	if( m_ptrProductReg == NULL )
    {
        CCTRACEE("CNavLu::RegisterCommandLines() - No product reg com object.");
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
						_bstr_t( "" ),
                        &vMoniker);

					bRegisteredElements = ( vMoniker.vt != VT_EMPTY );
				}
				catch ( ... )
				{
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

				if ( atoi( ( LPCSTR ) strGroup ) > 0 ) 
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
                                CCTRACEI("Commandline: %s matches the registered commandline, not re-registering", strProductName);
								strMoniker = strProductMoniker;
							}
						}
						else
						{
							// Saved is registered, but it's elements have changed
							// so update the registration with the new PVL information.
                            CCTRACEI("Re-registering commandline: \"%s\" \"%s\" \"%s\" \"\"", strProductName, strProductVersion, strProductLanguage);
							strMoniker = strProductMoniker;
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szProduct ), _bstr_t( strProductName ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szVersion ), _bstr_t( strProductVersion ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szLanguage ), _bstr_t( strProductLanguage ) );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szType ), _bstr_t( "" ) );
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

                            CCTRACEE("RegisterCommandLines() - The saved moniker for PVL: \"%s\" \"%s\" \"%s\" \"\" is not the same as the registered one. Re-registering it now.", strProductName, strProductVersion, strProductLanguage);

                            // if we're supposed to set the SKU, do so
				            if ( atoi( ( LPCSTR ) strSetSKU ) > 0 ) 
				            {
					            try
                                {
                                    // Get license type from navopts.dat
                                    char szSKU[MAX_PATH] = {0};
                                    CNAVOptSettingsEx NAVOptSettings;
                                    NAVOptSettings.Init();
                                    DWORD dwSize = MAX_PATH;
                                    NAVOptSettings.GetValue("SKU:PAIDSKU", szSKU, dwSize, "");
                                    m_ptrProductReg->SetProperty( strMoniker, _bstr_t( "SKU" ), _bstr_t( szSKU ) );
                                }
                                catch ( _com_error e )
	                            {
		                            CCTRACEE( _T("CNavLu::SetSkuProp() - caught exception") );
                                    return E_FAIL;
	                            }
				            }

                            //if we're supposed to set the ENVIRONMENT to RETAIL, do so.
				            if ( atoi( ( LPCSTR ) strSetENV ) > 0 ) 
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
								if ( strProductName.Find( "NAV" ) >= 0 )
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

                            CCTRACEE("RegisterCommandLines() - The saved moniker for PVL: \"%s\" \"%s\" \"%s\" \"\" is not registered. Re-registering it now", strProductName, strProductVersion, strProductLanguage);

							hr = m_ptrProductReg->RegisterProduct( _bstr_t( strProductName ), 
								_bstr_t( strProductVersion ), 
								_bstr_t( strProductLanguage ),
								_bstr_t( "" ),
								&vMoniker );
							strMoniker = _bstr_t( vMoniker );                   
							strProductMoniker = LPCTSTR( strMoniker );
							GetDescriptiveName( strProductName,strProductVersion,strCallbacksHere,strDescriptiveText,strProductName );
							m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), _bstr_t( strDescriptiveText ) );

				            // Save off the moniker for next time
                            ATLASSERT( !strProductMoniker.IsEmpty() );
				            SetCmdLineN( dwCmdLineNum, NULL, NULL, NULL, NULL, NULL, &strProductMoniker );

                            // if we're supposed to set the SKU, do so
				            if ( atoi( ( LPCSTR ) strSetSKU ) > 0 ) 
				            {
					            try
                                {
                                    // Get license type from navopts.dat
                                    char szSKU[MAX_PATH] = {0};
                                    CNAVOptSettingsEx NAVOptSettings;
                                    NAVOptSettings.Init();
                                    DWORD dwSize = MAX_PATH;
                                    NAVOptSettings.GetValue("SKU:PAIDSKU", szSKU, dwSize, "");
                                    m_ptrProductReg->SetProperty( strMoniker, _bstr_t( "SKU" ), _bstr_t( szSKU ) );
                                }
                                catch ( _com_error e )
	                            {
		                            CCTRACEE( _T("CNavLu::SetSkuProp() - caught exception") );
                                    return E_FAIL;
	                            }
				            }

                            //if we're supposed to set the ENVIRONMENT to RETAIL, do so.
				            if ( atoi( ( LPCSTR ) strSetENV ) > 0 ) 
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
								if ( strProductName.Find( "NAV" ) >= 0 )
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
                            if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 )
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
                        CCTRACEE("RegisterCommandLines() - There is no PVL registered for commandline: \"%s\" \"%s\" \"%s\" \"\". Re-registering now", strProductName, strProductVersion, strProductLanguage);
						// Register anew and save the new moniker as the saved
						// NAV moniker.
						GetCmdLineMoniker( strProductName,strProductVersion,strCallbacksHere,strGroup,vMoniker );
						hr = m_ptrProductReg->RegisterProduct( _bstr_t( strProductName ), 
							_bstr_t( strProductVersion ), 
							_bstr_t( strProductLanguage ),
							_bstr_t( "" ),
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
							if ( strProductName.Find( "NAV" ) >= 0 )
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
				CCTRACEE("RegisterCommandLines() - ERROR! Caught COM exception registering command line");
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
        CCTRACEE("Error attempting to set the ENVIRONMENT setting for LiveUpdate. Error: %s", e.Description());
        return e.Error();
	}
    catch(...)
    {
        CCTRACEE("Unknown error attempting to set the ENVIRONMENT setting for LiveUpdate.");
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
		CCTRACEE( "CNavLu::UnregisterCommandLines - Symantec digital signature check failed on product reg dll.\r\n" );
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
        CCTRACEE("CNavLu::UnregisterCommandLines() - No product reg com object.");
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
			CCTRACEE( "CNavLu::UnregisterCommandLines - Symantec digital signature check failed on product reg dll.\r\n" );
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
	catch ( ... )
	{
		CCTRACEE( _T("RegisterCommandLines() - caught exception") );
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
//   Edit the LiveUpdate command line Version.
//
// Parameters:
//   pszVersion [in]  string containing the new MicroDef Version
//
// Returns:
//   TRUE    when the Version is successfully updated.
//   FALSE
//
//////////////////////////////////////////////////////////////////////////

BOOL CNavLu::EditMicroDefVersion(LPTSTR pszVersion)
{
	DWORD     dwCmdLineNum = 1;
	CString   strProductName;
	CString   strProductType;
	CString   strProductVersion;
	CString   strProductLanguage;
	CString   strGroupTogether;
	BOOL      bFound;
	int       iCmpLen;

	CString   strMicroDefVersion(pszVersion);

	iCmpLen = strMicroDefVersion.Find('.');

	if ( iCmpLen < 0 )            
		return ( FALSE );

	// Find a command line that has a ProductName of "Avenge 1.5 MicroDefs".
	bFound = FALSE;
	while ( !bFound &&
		GetCmdLineN( dwCmdLineNum, &strProductName, &strProductType,
		&strProductLanguage, &strProductVersion, NULL, NULL, NULL,
		&strGroupTogether, NULL, NULL ) )
	{
		if ( atoi( ( LPCSTR ) strGroupTogether ) > 0 )
		{
			// If the version key is a match (either MicroDefsT or MicroDefsB)
			// with the pszVersion passed in, then this is the one to edit.
			if ( strProductVersion.Left( iCmpLen ) == strMicroDefVersion.Left( iCmpLen ) )
				bFound = TRUE;
		}

		if ( !bFound )
			++dwCmdLineNum;
	}

	if ( bFound )
	{
		// Update the Version field.   
		SetCmdLineN ( dwCmdLineNum, NULL, NULL, NULL, &strMicroDefVersion, NULL, NULL );

		return ( TRUE );
	}

	return (FALSE);
}

DWORD CNavLu::GetDefAnnuityStatus()
{
	return( m_DefAnnuityStatus );
}

//      This function will return TRUE if LiveUpdate is run in silent mode.  This function 
// can only determine this for LiveUpdate 1.6 and above.  In LiveUpdate 1.6, this function
// will only work if LuAll has already started.
//      This function will return FALSE if there is an error.
BOOL CNavLu::IsLiveUpdateInSilentMode()
{
	BOOL bResult = FALSE;
	HRESULT hr = S_OK;

	try
	{
		// Declare a Smart Pointer to the LiveUpdate engine.
		CComPtr<IstCheckForUpdates>  ptrCheckForUpdates;

		// Initialize the Smart Pointer.
		hr = ptrCheckForUpdates.CoCreateInstance( __uuidof( stCheckForUpdates ), NULL, 
			(CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER) );
		if ( FAILED(hr) )
			throw hr;

		// Load the Settings into the Settings object.  (This has probably been done
		// already, but doing it again won't hurt anything.)
		hr = ptrCheckForUpdates->LoadSettings();
		if ( FAILED(hr) )
			throw hr;

		// Get a pointer to the Settings object.
		CComPtr<IstSettings> ptrSettings;
        hr = ptrCheckForUpdates->get_Settings(&ptrSettings);
		if ( FAILED(hr) || ptrSettings == NULL )
			throw E_FAIL;   

		// Get the Silent switch out of the settings. 
		_bstr_t bstrSilentMode;
        hr = ptrSettings->get_Property( bstrTEMP_SILENT_MODE, &bstrSilentMode.GetBSTR() );

		if ( bstrSilentMode.length() )
		{
            CCTRACEI("CNavLu::IsLiveUpdateInSilentMode() - LU is in silent mode.");
			bResult = TRUE;
		}
		else
		{
			// this is used for detection with LU 1.62, where the TEMP\SILENT_MODE flag was not set
			// mutex detection code taken from NIS function DoesSomeoneElseOwnMutex in alerttracker\atrack.cpp
			HANDLE shMutex = ::CreateMutex(NULL, FALSE, SYM_AUTO_UPDATE_MUTEX);
			DWORD dwRet = ::WaitForSingleObject(shMutex, 0);
			switch(dwRet)
			{
			case WAIT_TIMEOUT:
				bResult = TRUE;
				break;
			case WAIT_OBJECT_0:
				::ReleaseMutex(shMutex);
				break;
			default://bad
				break;
			}
			CloseHandle(shMutex);           
		}
		// The Smart Pointers will take care of doing the appropriate Releases and
		// memory frees to release their associated objects.
	}
	catch(...)
	{
		// Something went wrong or LiveUpdate 1.6+ is not installed on this machine.
		ASSERT(FALSE);
        CCTRACEE("CNavLu::IsLiveUpdateInSilentMode() - Caught exception, return false");
		bResult = FALSE;
	}

    CCTRACEI("CNavLu::IsLiveUpdateInSilentMode() - Return %d", bResult);
	return bResult;
} // IsLiveUpdateInSilentMode

BOOL CNavLu::GetVirusDefsDelivered()
{
	// Check with defutils to see if there are newer defs on the system than we started
    // with
    if( !m_pDefUtils->GetNewestDefsDate(&postDefs.wYear, &postDefs.wMonth, &postDefs.wDay, &postDefs.dwRev) )
    {
        CCTRACEE("CNavLu::GetVirusDefsDelivered() - GetNewestDefsDate() failed assuming new defs were not delivered");
        return FALSE;
    }

    // Compare the initial dates with the new ones
    if( postDefs.wYear > initialDefs.wYear || 
        postDefs.wMonth > initialDefs.wMonth || 
        postDefs.wDay > initialDefs.wDay || 
        postDefs.dwRev > initialDefs.dwRev )
    {
        CCTRACEI("CNavLu::GetVirusDefsDelivered() - newer defs were delivered: %d%02d%02d%03d", postDefs.wYear, postDefs.wMonth, postDefs.wDay, postDefs.dwRev);
        return TRUE;
    }
    else
    {
        CCTRACEE("CNavLu::GetVirusDefsDelivered() - newer defs were NOT delivered. Newest defs are still: %d%02d%02d%03d", initialDefs.wYear, initialDefs.wMonth, initialDefs.wDay, initialDefs.dwRev);
        return FALSE;
    }
}

BOOL CNavLu::LaunchSideEffectScan()
{
    BOOL bRet = FALSE;

    // Kick off the side effect scan if the option is enabled
    CNAVOptSettingsEx navSettings;
    if( navSettings.Init() )
    {
        DWORD dwVal = 0;
        if( SUCCEEDED(navSettings.GetValue(SCANNER_DefUpdateScan, dwVal, 0)) && dwVal != 0 )
        {
            TCHAR szSEScan[MAX_PATH*2] = {0};
            STARTUPINFO rSI;
			PROCESS_INFORMATION rPI;

			ZeroMemory(&rSI, sizeof(rSI));
			rSI.cb = sizeof(rSI);
            ZeroMemory(&rPI, sizeof(rPI));

            CNAVInfo navInfo;
            _tcscpy(szSEScan, _T("\""));
            _tcsncat(szSEScan, navInfo.GetNAVDir(), MAX_PATH+1);
            _tcscat(szSEScan, _T("\\NAVW32.EXE\" /SESCAN"));

		    bRet = CreateProcess(NULL, szSEScan, NULL, NULL, FALSE, 
				                 CREATE_NEW_PROCESS_GROUP | IDLE_PRIORITY_CLASS, 
                                 NULL, NULL, &rSI, &rPI);

			if (bRet)
            {
                CloseHandle(rPI.hProcess);
				CloseHandle(rPI.hThread);
            }
        }
    }

    return bRet;
}