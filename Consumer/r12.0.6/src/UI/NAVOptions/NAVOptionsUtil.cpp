// NAVOptionsObj.cpp : Implementation of CNAVOptions
#include "StdAfx.h"

#define INITIIDS
#include "EmailProxyInterface.h"  // For Email options
#include "NAVOptionsObj.h"
#include "Navtrust.h"
#include "defutilsinterface.h"
#include "IWPPrivateSettingsInterface.h"      // For IWP
#include "SymProtectControlHelper.h"
#include "ccScanInterface.h"

// This if for the CThreatsByVID class to display
// the threat details.
#include "ccEraserInterface.h"
#include "CommonUIInterface.h"

#include "NAVOptions.h"
#include "ccWebWnd.h"
#include "NAVOptionsObj.h"
#include "BrowserWindow.h"
#include "optnames.h"
#include "NAVErrorResource.h"
#include "NAVPwdExp.h"  // Contains the password ID define for NAV
#include "NAVPwd_h.h"
#include "NAVPwd_i.c"

#define _NAVOPTREFRESH_CONSTANTS
#include "NAVOptRefresh.h"
#include "NavOptionRefreshHelperInterface.h"
#include "StahlSoft.h"
//#define _INIT_COSVERSIONINFO
//#include "SSOsinfo.h"
#include "NAVSettingsCache.h"

#include "ccWebWnd_i.c"
#include "SnoozeAlert.h"

#include "ISWSCHelper_Loader.h"
#include "WSCHelper.h"
#include "..\navoptionsres\resource.h"

using namespace std;

// Object map for syminterface
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()

STDMETHODIMP CNAVOptions::Show(/*[in]*/ long hWnd)
{
	HRESULT hr;
	CSemaphore sm;

    switch(hr = sm.Create())
	{
		HWND hAppWnd;

	case S_EXIST:  // Another session is holding the semaphore
	default:       // Error
		break;

	case S_FALSE:  // An instance of the options dialog already exist
		// Wait untill we know an instance is up and running
		if (!sm.Accuire())
			break;  // If the system is very busy and the user managed to create
			        // several instences of the options dialog we may have some
			        // instances that are in limbo. Is such cases we simply abendon
			        // the whole idea after the timeout expired.

		// Now look for that instance.

		// Q: Is an instance of Options dialog is still running?
		// Allow one one instance of the options dialog
		if (hAppWnd = ::FindWindow(ccWebWindow_ClassName, m_csTitle))
		{
			// Just bring the existing instance to the forground.
			::SetForegroundWindow(hAppWnd);
			break;
		}
		// Probably closed be fore we had a chance to find it.
		/* FALL THROUGH */
	case S_OK:  // This is the first instance of the options dialog
	{
        // First check for a digital signature on the NAVWebWindow COM
        // server since it will be used to display the Options UI
		if( forceError (ERR_SECURITY_FAILED) ||
			NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_ccWebWindow) )
        {
            // Invalid signature
			MakeError (ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
            m_spError->LogAndDisplay(0);
            return E_ACCESSDENIED;
        }

        // Check for password if necessary
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_Password) )
        {
            // Invalid signature
			MakeError (ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
            m_spError->LogAndDisplay(0);
            return E_ACCESSDENIED;
        }

        CComPtr<IPassword> spNAVPass;
        if (SUCCEEDED(spNAVPass.CoCreateInstance(CLSID_Password, NULL, CLSCTX_INPROC)))
        {
            CComBSTR bstrProductName(m_csProductName);
            CComBSTR bstrFeatureName(m_csTitle);
            BOOL bEnabled = FALSE;

            if( SUCCEEDED(spNAVPass->put_ProductID(NAV_CONSUMER_PASSWORD_ID)) &&
                SUCCEEDED(spNAVPass->put_ProductName(bstrProductName)) &&
                SUCCEEDED(spNAVPass->put_FeatureName(bstrFeatureName)) &&
                SUCCEEDED(spNAVPass->get_Enabled(&bEnabled)) )
            {
                if( bEnabled )
                {
                    
                    // Reusing the bEnabled variable to check if the password
                    if( SUCCEEDED(spNAVPass->Check(&bEnabled)) )
                    {
                        // Check the result of the password check before displaying the options
                        if( !bEnabled )
                        {
                            // Password not input correctly...don't display options
                            return S_FALSE;
                        }
                    }
                }
            }
        }

		// Create a new instance
		const long W = 600, H = 460;  // Dialog size

      // STS 361588
      // When we launch the options dialog, it needs to be offset from the main UI.
      // Since this dialog is so big though, moving it down in Y on a 640x480 display
      // will hide the OK/CANCEL/DEFAULTS buttons so I will only shift right if
      // the vertical display size is less than 600 pixels.
      int iOffsetX = 20;
      int iOffsetY = 36;

      if (GetSystemMetrics(SM_CYSCREEN) < 600)
      {
         iOffsetY = 0;
      }
		// Now look for that instance.

		CComPtr<IccWebWindow> spSymWebWin;

        // Get a pointer to the IDispatch to pass into the options object
        CComPtr<IDispatch> pDispatch;
        CComVariant vNavOpts;
        if( SUCCEEDED(QueryInterface(IID_IDispatch, (void**) &pDispatch)) )
        {
            vNavOpts = pDispatch;
        }
        else
        {
            CCTRACEE(_T("CNAVOptions::Show() - Failed to QI for internal IDispatch interface"));
        }

		// Open dialog
		if (SUCCEEDED(hr = spSymWebWin.CoCreateInstance(CLSID_ccWebWindow, NULL, CLSCTX_INPROC))
		 && SUCCEEDED(hr = spSymWebWin->put_OffsetWindowX(iOffsetX))
		 && SUCCEEDED(hr = spSymWebWin->put_OffsetWindowY(iOffsetY))
		 && SUCCEEDED(hr = spSymWebWin->put_InitialTitle(CComBSTR(m_csTitle)))
		 && SUCCEEDED(hr = spSymWebWin->SetIcon(reinterpret_cast<long>(_Module.GetModuleInstance()), IDI_NAVOPTIONS))
         && SUCCEEDED(hr = spSymWebWin->put_ObjectArg(vNavOpts)))
        {
		    hr = spSymWebWin->showModalDialog(CComBSTR(url()), W
			                                                    , H
			                                                    , &CComVariant()
			                                                    , &CComVariant());
        }
        else
        {
            CCTRACEE(_T("CNAVOptions::Show() - Failed to create the ccWebWindow for the options dialog"));
        }
 
        break;

      }

	}

	return hr;
}

STDMETHODIMP CNAVOptions::get_InitialPageSpyware(BOOL* pbShowSpyware)
{
    *pbShowSpyware = (m_eInitialPageID == ShowPageID_SPYWARE) ? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

STDMETHODIMP CNAVOptions::ShowSpywarePage(long hWnd)
{
    return ShowPage(hWnd, ShowPageID_SPYWARE);
}

STDMETHODIMP CNAVOptions::get_InitialPage(EShowPageID *peShowPageID)
{
    *peShowPageID = m_eInitialPageID;

    return S_OK;
}

STDMETHODIMP CNAVOptions::ShowPage(/*[in]*/ long hWnd, /*[in]*/ EShowPageID eShowPageID)
{
    m_eInitialPageID = eShowPageID;
    return Show(hWnd);
}

STDMETHODIMP CNAVOptions::Help(/*[in]*/ long iID)
{
    m_Help.LaunchHelp(iID);
	return S_OK;
}

STDMETHODIMP CNAVOptions::Browse(/*[in]*/ EBrowseType eBrowseType, /*[in]*/ BSTR bstrInPath
                                                                 , /*[out, retval]*/ BSTR* pbstrOutPath)
{
	USES_CONVERSION;
    HRESULT hr = S_OK;

	if (forceError (ERR_INVALID_POINTER) ||
        !pbstrOutPath)
    {
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    if ( ::SysStringLen ( bstrInPath ) > MAX_PATH )
        return E_POINTER;

	*pbstrOutPath = NULL;

	// Q: is the input path is valid
	PWCHAR pszPath = OLE2W(bstrInPath);
	CBrowserWindow bw(0xFFFFFFFFUL != ::GetFileAttributesW(pszPath) ? pszPath : NULL, eBrowseType);

	hr = bw.Browse(bstrInPath, pbstrOutPath);

    if ( forceError (IDS_ERR_BROWSING_FOLDERS))
        hr = E_FAIL;

    if( FAILED ( hr ) )
    {
        MakeError(IDS_ERR_BROWSING_FOLDERS, hr, IDS_NAVERROR_INTERNAL);
        return hr;
    }

    return hr;
}

STDMETHODIMP CNAVOptions::ValidatePath(/*[in]*/ EBrowseType eBrowseType, /*[in]*/ BSTR bstrPath
                                                                       , /*[out, retval]*/ BOOL *pbValid)
{
	USES_CONVERSION;
	DWORD dwAttribute, dwError;

   	if (!pbValid)
		return Error(_T("ValidatePath()"), E_POINTER);
	*pbValid = VARIANT_FALSE;

	if (!bstrPath)
		return Error(_T("ValidatePath()"), E_INVALIDARG);

    if ( ::SysStringLen ( bstrPath ) > MAX_PATH )
        return E_POINTER;

	// Get attribute of the item
	if (0xFFFFFFFFUL != (dwAttribute = ::GetFileAttributes(OLE2T(bstrPath))))
		switch(eBrowseType & ~Browse_Exist)
		{
		case Browse_Directory:
			// For a directory validation we must make sure that there's no file existing with that name
			if(FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & dwAttribute))
				*pbValid = VARIANT_TRUE;
			break;
		case Browse_File:
			// For for file validation we want to make sure that there is no directory with the same name
			if (FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & dwAttribute))
				*pbValid = VARIANT_TRUE;
			break;
		case Browse_Disk:  // TODO: Handle disks
			return Error(IDS_Err_ValidatePath, _T("ValidatePath()"));
		}
	else if (ERROR_FILE_NOT_FOUND == (dwError = ::GetLastError())
	      || ERROR_PATH_NOT_FOUND == dwError)
	{
		if (Browse_Exist != (Browse_Exist & eBrowseType))
			*pbValid = VARIANT_TRUE;  // item doesn't exists
	}
	else
		return Error(IDS_Err_ValidatePath, _T("ValidatePath()"));

	return S_OK;
}

STDMETHODIMP CNAVOptions::ANSI2OEM(/*[in]*/ BSTR bstrANSI, /*[out, retval]*/ BSTR *pbstrOEM)
{
    // convert check boundries on the strings so this call is safe.
    return convert(bstrANSI, pbstrOEM, ::CharToOem);
}

STDMETHODIMP CNAVOptions::OEM2ANSI(/*[in]*/ BSTR bstrOEM, /*[out, retval]*/ BSTR *pbstrANSI)
{
	// convert check boundries on the strings so this call is safe.
    return convert(bstrOEM, pbstrANSI, ::OemToChar);
}

/////////////////////////////////////////////////////////////////////////////
// Properties
STDMETHODIMP CNAVOptions::get_IsTrialValid(/*[out, retval]*/ BOOL *pbIsTrialValid)
{
	if (!pbIsTrialValid)
		return Error(_T("get_IsTrialValid()"), E_POINTER);

	*pbIsTrialValid = VARIANT_FALSE;

	DWORD dwValue = 0;
    if( SUCCEEDED(m_pNavOpts->GetValue(NAVOPTION_FeatureEnabled, dwValue, 0)) )
		*pbIsTrialValid = (dwValue == 0) ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_HaveScriptBlocking(/*[out, retval]*/ BOOL *pbHaveScriptBlocking)
{
	return E_NOTIMPL;
}

STDMETHODIMP CNAVOptions::get_CanChange(/*[out, retval]*/ BOOL *pbCanChange)
{
	if (!pbCanChange)
		return Error(_T("get_CanChange()"), E_POINTER);

	*pbCanChange =  m_bCanAccessRegKey ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_SystemMode(/*[out, retval]*/ long *plSystemMode)
{
	if (!plSystemMode)
		return Error(_T("get_SystemMode()"), E_POINTER);

	*plSystemMode =  ::GetSystemMetrics(SM_CLEANBOOT);

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_NortonAntiVirusPath(/*[out, retval]*/ BSTR *pbstrNortonAntiVirusPath)
{
	USES_CONVERSION;

	if ( forceError (ERR_INVALID_POINTER) ||
        !pbstrNortonAntiVirusPath)
    {
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
		return E_POINTER;
    }

    if( forceError (IDS_ERR_GETTING_NAVPATH) )
    {
        MakeError(IDS_ERR_GETTING_NAVPATH, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
		return E_FAIL;
    }

	*pbstrNortonAntiVirusPath = ::SysAllocString(T2OLE(g_NAVInfo.GetNAVDir()));

    if( pbstrNortonAntiVirusPath )
        return S_OK;
    else
    {
        MakeError(IDS_ERR_GETTING_NAVPATH, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
		return E_FAIL;
    }
}

// Implementation
HRESULT CNAVOptions::convert(BSTR bstrRAW, BSTR *pbstrCONVERTED, BOOL(WINAPI*fnConvert)(LPCTSTR, LPSTR))
{
	USES_CONVERSION;

    // Make sure our buffer is big enough to hold the converted string
    DWORD dwSize = ::SysStringLen(bstrRAW) + 1;
	CBuffer szBuffer(dwSize);

	if (!pbstrCONVERTED)
		return Error(_T("convert()"), E_POINTER);
	*pbstrCONVERTED = NULL;

	if (!bstrRAW)
		return S_OK;

	if (!*bstrRAW)
	{
		*pbstrCONVERTED = ::SysAllocString(L"");
		return S_OK;
	}

	fnConvert(OLE2T(bstrRAW), szBuffer);

	return (*pbstrCONVERTED = ::SysAllocString(A2OLE(szBuffer))) ? S_OK : E_OUTOFMEMORY;
}

// CNAVOptions::getLUMonikerFromProductNameSubString(LPCSTR pcszSubstr, /*out*/BSTR bstrMonikers[3])
//
// Takes a substring and searches the ProductNames of registered LiveUpdate command lines
// for a match of the substring.  If the match is found then the BSTR array is populated with
// the moniker(s) for the match.  At most there can be 2 matches (in the case of the virus defs).
// After the last moniker is entered the next item in the array is set to NULL.  The caller is
// responsible for freeing the BSTR array.
BOOL CNAVOptions::GetLUMonikerFromProductNameSubString(LPCTSTR pcszSubstr, /*out*/BSTR bstrMonikers[3])
{
    // Validate input parameter
    if( NULL == pcszSubstr )
    {
        return FALSE;
    }

    // Return value
    BOOL bRet = TRUE;

    // Current BSTR index
    int nBstrIndex = 0;

    // Current CmdLine key
    CRegKey rkeyCurNAVCmdLine;

    // Name of current command line (i.e. "CmdLine1")
    TCHAR szCurCmdLine[] = _T("Software\\Symantec\\Norton AntiVirus\\LiveUpdate\\CmdLines\\CmdLine0");

    // Convert both values to lowercase
    TCHAR *pszSearchStr = new TCHAR[_tcslen(pcszSubstr)+1];
    _tcscpy(pszSearchStr, pcszSubstr);
    _tcslwr(pszSearchStr);

    for( int nCurCmdLine=1; ;nCurCmdLine++ )
    {
        // Stores the ProductName field retrieved from the commandline regkey
        TCHAR szProdName[MAX_PATH] = {0};
        DWORD dwSize = sizeof(szProdName);

        // Create key name to be opened by replacing previous cmdline number
        // with next number
        TCHAR cPrevNum = '0'+nCurCmdLine-1;
        TCHAR cNextNum = '0'+nCurCmdLine;

        // Find the old CmdLine number
        TCHAR* pszOldNum = _tcsrchr(szCurCmdLine, cPrevNum);

        if( NULL == pszOldNum )
        {
            // What!?!, Should always find the previous number at the end of the CmdLine string
            bRet = FALSE;
            break;
        }

        // Replace the old CmdLine number with the next one
        *pszOldNum = cNextNum;
        
        if( ERROR_SUCCESS != rkeyCurNAVCmdLine.Open(HKEY_LOCAL_MACHINE, szCurCmdLine,  KEY_QUERY_VALUE) )
        {
            // Could not open command line registry keys, assume done enumerating
            break;
        }

        // Check the "ProductName" value of this key for the sub string passed in
        if( ERROR_SUCCESS != rkeyCurNAVCmdLine.QueryValue(_T("ProductName"),  NULL, szProdName, &dwSize) )
        {
            // Couldn't query a ProductName key for this commandline, eh?
            // Just keep on truckin' to the next key
            continue;
        }

        _tcslwr(szProdName);

        // Now search for the sub string in the ProductName
        if( NULL != _tcsstr(szProdName, pszSearchStr) )
        {
            // Found the sub string, now put the ProductMoniker value into the
            // next available BSTR out variable.
            // NOTE: re-using szProdName as temp storage since we're done with it
            //       for this iteration so need to also reset the size.
            dwSize = sizeof(szProdName);
            if( ERROR_SUCCESS != rkeyCurNAVCmdLine.QueryValue(_T("ProductMoniker"), NULL, szProdName, &dwSize) )
            {
                // Hmmm... couldn't get the Moniker, must not be cool enough
                continue;
            }

            // Create a wide char string big enough to hold the Moniker
            size_t nWstrSize = mbstowcs(NULL, szProdName, 0);

            if( 0 == nWstrSize )
            {
                // Error: couldn't get size of Moniker
                bRet = FALSE;
                break;
            }

            // Allocate the wide char string and convert the MBCS string
            wchar_t *wszMonikerHolder = new wchar_t[nWstrSize+1];
            if( -1 == mbstowcs(wszMonikerHolder, szProdName, nWstrSize+1) )
            {
                // Error in coversion...
                bRet = FALSE;
                delete [] wszMonikerHolder;
                wszMonikerHolder = NULL;
                break;
            }

            // Store the Moniker
            bstrMonikers[nBstrIndex] = SysAllocString(wszMonikerHolder);
            nBstrIndex++;

            delete [] wszMonikerHolder;
            wszMonikerHolder = NULL;

            // If the index hits 2, then this better be the last Moniker so bust out
            if( 2 == nBstrIndex )
                break;
        }
    }

    if ( pszSearchStr )
    {
        delete [] pszSearchStr;
        pszSearchStr = NULL;
    }

    // Set the last item in the list of BSTR's to NULL
    bstrMonikers[nBstrIndex] = NULL;

    // On an error, free any already allocated strings
    if( !bRet )
    {
        for( int i=0; NULL != bstrMonikers[i]; i++ )
        {
            SysFreeString(bstrMonikers[i]);
        }
    }

    return bRet;
}

// Determines if the option passed in is an email option and if it
// is returns which one it is in pbstrOptName
bool	CNAVOptions::IsEmailOption( const BSTR bstrOptName, /*out*/BSTR* pbstrOptName )
{
	USES_CONVERSION;

    bool bRet = false;

    string strOptName = OLE2T(bstrOptName);

    if( string::npos != strOptName.find("NAVPROXY") )
    {
        // It's an email option, find out which one
        if( string::npos != strOptName.find("ShowTrayIcon") )
        {
			*pbstrOptName = T2BSTR(CCEMLPXY_TRAY_ANIMATION);
			bRet = true;
        }
        else if( string::npos != strOptName.find("ShowProgressOut") )
        {
            *pbstrOptName = T2BSTR(CCEMLPXY_OUTGOING_PROGRESS);
            bRet = true;
        }
        else if( string::npos != strOptName.find("TimeOutProtection") )
        {
			*pbstrOptName = T2BSTR(CCEMLPXY_TIMEOUT_PROTECTION);
            bRet = true;
        }
        else
        {
            CCTRACEE("IsEmailOption(): Error - Unknown Email Option name");
            return false;
        }
    }

    return bRet;
}

bool CNAVOptions::IsAPOption( LPCWCH pwcOptionGroupName )
{
    return IsAPProperty(pwcOptionGroupName);
}

// Checks the exclusion item to see if it is an extension
bool CNAVOptions::IsExtensionExclusion(LPCTSTR pcszExclusion)
{
    if( !pcszExclusion )
        return false;

    bool bRet = false;

    TCHAR* pszCur = NULL;
    // Check for a . first
    if( NULL != (pszCur = _tcschr(pcszExclusion, _TCHAR('.'))) )
    {
        // Make sure there is nothing but possible a * in front of the .
        if( pszCur == pcszExclusion || (*pcszExclusion == _TCHAR('*') && CharNext(pcszExclusion) && _TCHAR('.') == *(CharNext(pcszExclusion))) )
        {
            return true;
        }
    }

    return false;
}

HRESULT CNAVOptions::registerCommandLines(void)
{
    // Get LU Callback dll
    HINSTANCE hLUCallback = NULL;
    TCHAR szLUCBPath[MAX_PATH*2];

    _tcscpy(szLUCBPath, g_NAVInfo.GetNAVDir());
    _tcscat(szLUCBPath, _T("\\navlucbk.dll"));

    // Verify the signature on the officeAV plug in dll
    if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szLUCBPath) )
    {
        return E_FAIL;
    }

    hLUCallback = LoadLibraryEx(szLUCBPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    if( NULL != hLUCallback )
    {
        typedef void (WINAPI *pfnREGISTERCMDLINES) (void);
        pfnREGISTERCMDLINES Register = reinterpret_cast<pfnREGISTERCMDLINES>(::GetProcAddress(hLUCallback, _T("RegisterCmdLines")));

        if( NULL != Register )
        {
            // Let's attempt to register the command lines
            Register();
            return S_OK;
        }
        else
        {
            // Could not retrieve function
            return E_FAIL;
        }
    
    }
    else
    {
        // Could not retrieve callback Dll handle
        return E_FAIL;
    }
}

HRESULT CNAVOptions::restoreLicensingState(void)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        // Set up path to NAVOptRf.dll
        TCHAR szNAVOptRf[_MAX_PATH*2] = {0};
        ::wsprintf(szNAVOptRf, _T("%s\\NAVOPTRF.DLL"), g_NAVInfo.GetNAVDir());

        // Check digital signature.
        hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szNAVOptRf)) ? E_FAIL : S_OK);

        // Load NAVOptRf.dll
        StahlSoft::CSmartModuleHandle smLibNavOptRf(::LoadLibrary(szNAVOptRf));
        hrx << ( ((HANDLE) smLibNavOptRf == (HANDLE) NULL) ? E_FAIL : S_OK );

        // Create NAVOptionRefresh object.
        SIMON::CSimonPtr<INAVOptionRefresh> spOptionRefresh;
        hrx << SIMON::CreateInstanceByDLLInstance(smLibNavOptRf,CLSID_CNAVOptionRefresh, IID_INAVOptionRefresh,(void**)&spOptionRefresh); 

        // Force a refresh
        hrx << spOptionRefresh->Refresh(REFRESH_FORCE_FULLY);
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}
bool CNAVOptions::EnumerateSettings(_NAVSETTINGSMAP* pMap)
{
    bool bRet = false;

    if( pMap )
    {
        for( _NAVSETTINGSMAP::iterator it = pMap->begin(); it != pMap->end(); it++ )
        {
            if( it->second )
            {
                CSymPtr<ccSettings::IEnumValues> pEnumValues;
                if( SYM_SUCCEEDED((it->second)->EnumValues(&pEnumValues)) )
                {
                    // Get all the data for these values
                    DWORD dwCount = 0;
                    if( SYM_SUCCEEDED( pEnumValues->GetCount(dwCount) ) )
                    {
                        // If we have at least one value consider this enumeration a success
                        if( dwCount > 0 )
                            bRet = true;

                        // Allocate a buffer big enough to hold the largest possible setting name
                        TCHAR *pszItemName = new TCHAR[m_dwMaxName];
                        if( !pszItemName )
                        {
                            bRet = false;
                        }

                        pair<OPTYPEMAP::iterator, bool> pr;

                        for(DWORD i=0; i<dwCount && pszItemName; i++)
                        {
                            DWORD dwSize = m_dwMaxName;
                            DWORD dwType = 0;
                            LPTSTR pszNewVal = NULL;
                            
                            if( SYM_SUCCEEDED(pEnumValues->GetItem(i, pszItemName, dwSize, dwType)) )
                            {
                                std::string strOptName;
                                CNAVOptSettingsCache::GetNavoptName(it->first.c_str(), pszItemName, strOptName);

                                // Save the type of this option so we can coerce VARIANTs
                                USES_CONVERSION;
			                    pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(A2W(strOptName.c_str()), dwType));
                                _ASSERT(pr.second);  // Validate that such value didn't exist

			                    // Q: Does this group already exist?
                                std::wstring::size_type colonIndex = pr.first->first.find_first_of(L":");

                                if( std::wstring::npos != colonIndex )
                                {
                                    std::wstring wstrGroupName = pr.first->first.substr(0, colonIndex);
			                        if (!wstrGroupName.empty() && !PropertyExists(wstrGroupName.c_str()))
				                        // We need to add the group
				                        AddProperty(wstrGroupName.c_str());
                                }
                            }
                        }

                        if( pszItemName )
                            delete [] pszItemName;
                    }
                }
            }
        }
    }
    return bRet;
}


STDMETHODIMP CNAVOptions::Snooze(/*[in]*/ long hParentWnd, /*[in]*/ SnoozeFeature snoozeFeatures, /*[in]*/ BOOL bCanTurnOffAP, /*[out, retval]*/ BOOL *pResult)
{
	// We do not have to verify the trust here: the object will fail to load if the trust is not OK.
	TRACEHR(h);
	try
	{
		*pResult = CSnoozeAlert::Snooze(hParentWnd, snoozeFeatures, bCanTurnOffAP);
	}
	catch (_com_error& e)
	{
		h = e;
	}

	return h;
}

STDMETHODIMP CNAVOptions::SetSnoozePeriod(/*[in]*/ SnoozeFeature snoozeFeatures, /*[in]*/ long lValue)
{
	// We do not have to verify the trust here: the object will fail to load if the trust is not OK.
	TRACEHR(h);

	try
	{
		CSnoozeAlert::SetSnoozePeriod(snoozeFeatures, lValue);
	}
	catch (_com_error& e)
	{
		h = e;
	}
	return h;
}

STDMETHODIMP CNAVOptions::GetSnoozePeriod(/*[in]*/ SnoozeFeature snoozeFeature, /*[out, retval]*/ long* pValue)
{
	TRACEHR(h);

	try
	{
		*pValue = CSnoozeAlert::GetSnoozePeriod(snoozeFeature);
	}
	catch (_com_error& e)
	{
		h = e;
	}
	return h;
}

STDMETHODIMP CNAVOptions::get_ProductName(/*[out, retval]*/ BSTR *pVal)
{
	if(pVal)
	{
		CComBSTR bstrProductName(m_csProductName);
		*pVal = bstrProductName.Detach();
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_THREATSBYVID(/*[out, retval]*/ IThreatsByVID **ppThreatsByVID)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    HRESULT hr = E_FAIL;

    if (forceError (ERR_INVALID_POINTER) || !ppThreatsByVID)
    {
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *ppThreatsByVID = NULL;

    if( m_spThreatsByVID )
    {
        (*ppThreatsByVID = m_spThreatsByVID)->AddRef();
        return S_OK;
    }

    // Need to create the IThreatsByVID object
    if( FAILED(hr = m_spThreatsByVID.CoCreateInstance(CLSID_ThreatsByVID)) )
    {
        MakeError(IDS_ERR_CREATE_THEATSBYVID, hr, IDS_NAVERROR_INTERNAL);
        return hr;
    }

    // Set scriptable safety for this object
    hr = m_spThreatsByVID.QueryInterface(&m_spSymScriptSafeThreatsByVID);

    if( FAILED(hr) )
    {
        CCTRACEE( _T("CNAVOptions::get_THREATSBYVID() - Unable to query for the script safe object from the threats by VID object. HRESULT = 0x%X"), hr );
        MakeError(IDS_ERR_CREATE_THEATSBYVID, hr, IDS_NAVERROR_INTERNAL);
        return hr;
    }

    // Set appropriate access so that IsItSafe() returns successfully.
    long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
    long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
    m_spSymScriptSafeThreatsByVID->SetAccess(dwAccess, dwKey);

    (*ppThreatsByVID = m_spThreatsByVID)->AddRef();

    return S_OK;
}