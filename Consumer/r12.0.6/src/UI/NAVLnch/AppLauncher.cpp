// AppLauncher.cpp : Implementation of CAppLauncher
#include "stdafx.h"

#include "NAVTrust.h"
// #include "NAVSettingsHelperEx.h"
#include "simon.h"
#include "NAVBusinessRules.h"
#include "StahlSoft.h"
#include "SyncHelper.h"
#include "StahlSoft_EventHelper.h"
#include "NAVInfo.h"

#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#include "NAVSecurity.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepClientBase.h"
#include "cltPepConstants.h"

#include "NAVLnch.h"
#include "AVccModuleId.h"
#include "AVccLogViewerPluginId.h"

#include "AppLauncher.h"
#include "NAVInfo.h"        // Toolbox
#include "NAVHelpLauncher.h"    // NAVToolbox
#include "StartBrowser.h"   // Toolbox
#include "AVRESBranding.h"
#include "NAVErrorResource.h"
#include "Resource.h"

#include "NAVWHLP5.H"       // For resource ID's for Help for NAVW_INFO_ABOUT_NAV default
#include "optnames.h"       // Names of options
#include "GlobalEvents.h"   // Names of NAV events
#include "GlobalEvent.h"    // Special XP happy global events
#include "NAVOptions.h"
#include "NAVUIHTM_Resource.h"
#include "NavLicense_h.h"
#include "NAVLicense_i.c"

#include <htmlhelp.h>

#define LOG_VIEWER_EXE "\\ccLgView.exe"

// helper fn
BOOL ShellExecuteAndWait (LPCTSTR lpVerb, LPCTSTR lpFile)
{
    SHELLEXECUTEINFO seinfo;
    memset (&seinfo, 0, sizeof(seinfo));
    seinfo.cbSize = sizeof(SHELLEXECUTEINFO);
    seinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    seinfo.hwnd = NULL;
    seinfo.lpVerb = lpVerb;
    seinfo.lpFile = lpFile;
    seinfo.lpDirectory = NULL;
    seinfo.nShow = SW_SHOW;

    BOOL bRet = ShellExecuteEx(&seinfo);
    if (bRet)
    {
        StahlSoft::WaitForSingleObjectWithMessageLoop(seinfo.hProcess,INFINITE);
        ::CloseHandle(seinfo.hProcess);
    }

    return bRet;
}


/////////////////////////////////////////////////////////////////////////////
// CAppLauncher

CAppLauncher::CAppLauncher()
{
    CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

    // Check NAVError module for Symantec Signature...
    if( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer(_T("NAVError.NAVCOMError")) )
    {
        if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
                                                  NULL,
                                                  CLSCTX_INPROC_SERVER)))
        {
        }
    }
}

STDMETHODIMP CAppLauncher::LaunchQuarantine()
{
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    TCHAR szQuarPath [MAX_PATH*2];
    CNAVInfo NAVInfo;
    
    _tcscpy ( szQuarPath, NAVInfo.GetNAVDir ());
    _tcscat ( szQuarPath, "\\QConsole.exe" );

    HINSTANCE hInst = 0;

    // Check QConsole.exe for a valid Symantec signature before launching it
    if( forceError (ERR_QCONSOLE_SIGNATURE) || 
        (NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage( szQuarPath )) )
    {
        MakeError ( ERR_QCONSOLE_SIGNATURE, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    hInst = ShellExecute ( NULL, "open", szQuarPath, NULL, NULL, SW_SHOW);

    if ( forceError (ERR_NO_QCONSOLE) || (int) hInst <= 32 )
    {
        MakeError ( ERR_NO_QCONSOLE, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_QCONSOLE );
        return E_FAIL;            
    }

	return S_OK;
}

STDMETHODIMP CAppLauncher::LaunchLiveUpdate(bool bWait)
{    
	if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    HINSTANCE hInst = 0;

    // Get the full path to LUALL.exe
    TCHAR szLuall[MAX_PATH] = {0};
    HKEY  hkey;
    DWORD dwType, dwBuffSize = MAX_PATH * sizeof(TCHAR);

    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                _T("software\\symantec\\SharedUsage"),
                                0, KEY_QUERY_VALUE, &hkey ) )
    {
        if ( ERROR_SUCCESS == RegQueryValueEx ( hkey, _T("LiveUpdate"), NULL, &dwType,
                                      LPBYTE(szLuall), &dwBuffSize ))
        {
            if ( !*szLuall )
            {
                // Failed to get the path from the registry just set
                // it to "LUALL.EXE"
                _tcscpy( szLuall, _T("LUALL.EXE") );
            }
            else
            {
                // Get the second to last char in the path.
                //
                TCHAR* pszDirNull = _tcsrchr ( szLuall, '\0');

                TCHAR* pszLastSlash = CharPrev( szLuall, pszDirNull );

                // If the ending character is a slash, null it.
                //            
                if ( 0 == _tcscmp ( pszLastSlash, "\\"))
                {
                    *pszLastSlash = '\0';
                }

                // Append LUALL.exe to the path
                _tcscat(szLuall, _T("\\LUALL.EXE"));
            }
        }

        ::RegCloseKey ( hkey );
    }

    // Check LUALL.exe for a valid Symantec signature before launching it
	if(  forceError (ERR_LUALL_SIGNATURE) || NAVToolbox::IsSymantecSignedImage( szLuall ) )
    {
        MakeError ( ERR_LUALL_SIGNATURE, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    if (bWait)
    {
        BOOL bSuccess = ShellExecuteAndWait(_T("open"), _T("LUALL.EXE"));
        if (forceError (ERR_NO_LU) || !bSuccess)
        {
            CCTRACEE(_T("ShellExecuteAndWait failed for LUALL.exe (%d)"), ::GetLastError());
		    MakeError ( ERR_NO_LU, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_LU );
            return E_FAIL;           
        }
    }
    else
    {
        hInst = ShellExecute ( NULL, _T("open"), _T("LUALL.EXE"), NULL, NULL, SW_SHOW);
        if ( forceError (ERR_NO_LU) || (int) hInst <= 32 )
        {   
		    MakeError ( ERR_NO_LU, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_LU );
            return E_FAIL;           
        }
    }

	return S_OK;
    
}

STDMETHODIMP CAppLauncher::LaunchRescueDisk()
{
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    BOOL      bRescueInstalled = FALSE;
    HKEY      hkey;
    HRESULT   hr = E_FAIL;
    HINSTANCE hinst;
    TCHAR     szPath [MAX_PATH];
    TCHAR     szStartingDir [MAX_PATH];
    DWORD     dwType;
    DWORD     dwBuffSize = sizeof(szStartingDir);

    // Read Rescue's install dir from the InstalledApps key.

    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                         _T("software\\Symantec\\InstalledApps"),
                                         0,
                                         KEY_QUERY_VALUE,
                                         &hkey ))
    {
        if ( ERROR_SUCCESS == RegQueryValueEx ( hkey,
                                                _T("Norton Rescue"),
                                                NULL,
                                                &dwType,
                                                reinterpret_cast<LPBYTE>( szStartingDir ),
                                                &dwBuffSize ))
        {
            if ( REG_SZ == dwType )
            {
                bRescueInstalled = TRUE;
                lstrcpy ( szPath, szStartingDir );
                lstrcat ( szPath, _T("\\rescue32.exe") );

                hinst = ShellExecute ( NULL,
                                       _T("open"),
                                       szPath,
                                       NULL,
                                       szStartingDir,
                                       SW_SHOW );

                if ( forceError (ERR_NO_RESCUE) || reinterpret_cast<int>(hinst) <= 32 )
                {
                    MakeError ( ERR_NO_RESCUE, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_RESCUE );
                    hr = E_FAIL;
                }
                else
                    hr = S_OK;
            }
        }

        RegCloseKey ( hkey );
    }

    // If Rescue was not installed, tell the user.

    if ( forceError (ERR_RESCUE_NOT_INSTALLED) || !bRescueInstalled )
    {
        hr = E_FAIL;
        MakeError ( ERR_RESCUE_NOT_INSTALLED, E_FAIL, IDS_NAVERROR_NAVLNCH_RESCUE_NOT_INSTALLED );
    }

	return hr;
}

STDMETHODIMP CAppLauncher::get_IsLiveUpdateInstalled(BOOL *pbLUInstalled)
{
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    HKEY      hkey;
    LONG      lRet;
    TCHAR     szPath [MAX_PATH];
    DWORD     dwBuffSize = sizeof(szPath);
    DWORD     dwType;

    *pbLUInstalled = FALSE;

    // Check for LUALL's AppPath key.  This seems like a good way
    // to tell if LU is still installed.  Too bad they don't have
    // an InstalledApps key.

    lRet = RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                          _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\luall.exe"),
                          0,
                          KEY_QUERY_VALUE,
                          &hkey );

    if ( ERROR_SUCCESS == lRet )
    {
        lRet = RegQueryValueEx ( hkey,
                                 NULL,
                                 NULL,
                                 &dwType,
                                 reinterpret_cast<LPBYTE>( szPath ),
                                 &dwBuffSize );

        if ( ERROR_SUCCESS == lRet )
            *pbLUInstalled = TRUE;

        RegCloseKey ( hkey );
    }

	return S_OK;
}

STDMETHODIMP CAppLauncher::get_IsRescueDiskInstalled(BOOL *pbRDInstalled)
{
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    HKEY      hkey;
    TCHAR     szStartingDir [MAX_PATH];
    DWORD     dwType;
    DWORD     dwBuffSize = sizeof(szStartingDir);

    *pbRDInstalled = FALSE;

    // Read Rescue's install dir from the InstalledApps key.

    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                         _T("software\\Symantec\\InstalledApps"),
                                         0,
                                         KEY_QUERY_VALUE,
                                         &hkey ))
    {
        if ( ERROR_SUCCESS == RegQueryValueEx ( hkey,
                                                _T("Norton Rescue"),
                                                NULL,
                                                &dwType,
                                                reinterpret_cast<LPBYTE>( szStartingDir ),
                                                &dwBuffSize ))
        {
            if ( REG_SZ == dwType )
                *pbRDInstalled = TRUE;
        }

        RegCloseKey ( hkey );
    }

	return S_OK;
}

STDMETHODIMP CAppLauncher::LaunchHelp(DWORD dwHelpID)
{
	CCTRACEI("LaunchHelp(%d)", dwHelpID);

    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }


    NAVToolbox::CNAVHelpLauncher Help;
    bool bResult = (NULL != Help.LaunchHelp ( dwHelpID ));

	if (forceError (ERR_NO_HELP) || bResult == false)
    {        
		MakeError ( ERR_NO_HELP, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_HELP );		
        return E_FAIL;
    }

	return S_OK;
}

STDMETHODIMP CAppLauncher::LaunchEmailProxy()
{
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    // Set the email option to ON, then fire the Option Change event
    //
    HRESULT hr = S_OK;

    try
    {
        CComPtr<INAVOptions> spNAVOptions;
        CComPtr <ISymScriptSafe> spSymScriptSafe;
		CComBSTR bstrOptionsClass ("Symantec.Norton.AntiVirus.NAVOptions");

        if (SUCCEEDED(hr = spNAVOptions.CoCreateInstance(bstrOptionsClass, NULL, CLSCTX_INPROC)))
        {
            if ( SUCCEEDED ( spNAVOptions.QueryInterface(&spSymScriptSafe)))
            {
                // Set appropriate access so that NAVStatus call to IsItSafe() 
				// returns successfully.
				
				long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
				long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
				spSymScriptSafe->SetAccess(dwAccess, dwKey);
            }
			
            // Email option
            //
            CComVariant varEmailOn (1);
            CComBSTR bstrIncoming (NAVEMAIL_ScanIncoming );
            CComBSTR bstrOutgoing (NAVEMAIL_ScanOutgoing );
            CComVariant varNull (0);

            hr = spNAVOptions->Load ();
            hr = spNAVOptions->Get ( bstrIncoming, varNull, &varEmailOn );
            hr = spNAVOptions->Get ( bstrOutgoing, varNull, &varEmailOn );
            hr = spNAVOptions->Save ();
        }
        else
        {
            MakeError ( ERR_FAILED_START_EMAIL, E_FAIL, IDS_NAVERROR_NAVLNCH_FAILED_START_EMAIL );
            return E_FAIL;
        }
    }

    catch (...)
    {
        MakeError ( ERR_FAILED_START_EMAIL, E_FAIL, IDS_NAVERROR_NAVLNCH_FAILED_START_EMAIL );
        hr = E_FAIL;
    }

	return hr;
}

STDMETHODIMP CAppLauncher::get_CanLaunchLiveUpdate(BOOL *pVal)
{
    *pVal = m_OSInfo.IsAdminProcess () ? TRUE : FALSE;
    return S_OK;

}

STDMETHODIMP CAppLauncher::LaunchUnEraseWizard()
{
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    // If UnErase Wizard is already open it will switch it's focus
    // automatically.

	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), _T("UE32.exe"),
		NULL, NULL, SW_SHOWNORMAL);

	if (forceError (ERR_NO_UEW) || (DWORD) hInstance <= 32)
	{
		MakeError ( ERR_NO_UEW, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_UEW );
        return E_FAIL;
	}

    return S_OK;
}

STDMETHODIMP CAppLauncher::LaunchWipeInfo()
{
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    // If WipeInfo is already open it will switch it's focus
    // automatically.

	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), _T("WipInfSE.exe"),
		NULL, NULL, SW_SHOWNORMAL);

	if (forceError (ERR_NO_WIPEINFO) || (DWORD) hInstance <= 32)
	{
        MakeError ( ERR_NO_WIPEINFO, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_WIPEINFO );
		return E_FAIL;
	}

    return S_OK;
}

// Launches the Log viewer to the Norton AntiVirus category.
//
STDMETHODIMP CAppLauncher::LaunchActivityLog ()
{
	CCTRACEI("LaunchActivityLog()");
    
    // Don't check security because this gets called by NAVShellExt.
    // That module's parent app is IExplore.exe which is not signed.
    
    ccLib::CString sCCFolder;

    if ( forceError (ERR_NO_CCINFO) || !ccSym::CCommonClientInfo::GetCCDirectory(sCCFolder))
	{
        MakeError ( ERR_NO_CCINFO, E_FAIL, IDS_NAVERROR_NO_COMPONENT );
		return E_FAIL;
	}
    sCCFolder += LOG_VIEWER_EXE;

    TCHAR szCommandLine [MAX_PATH] = {0};

    wsprintf ( szCommandLine, _T("//PLUGID:%d"), CC_NAV_PLUGIN_ID );

    // /PLUGID:200 /CATID:10 /STANDALONE

	CCTRACEI("Open '%s' with '%s'", sCCFolder, szCommandLine);

	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), sCCFolder, szCommandLine, NULL, SW_SHOWNORMAL);

	if (forceError (ERR_NO_LOGVIEWER) || (DWORD) hInstance <= 32)
	{
        MakeError ( ERR_NO_LOGVIEWER, E_FAIL, IDS_NAVERROR_NO_COMPONENT );
		return E_FAIL;
	}

    return S_OK;
}


STDMETHODIMP CAppLauncher::get_NAVError(INAVCOMError* *pVal)
{
    if ( !m_spError )
        return E_FAIL;

    *pVal = m_spError;
    (*pVal)->AddRef (); // We get a ref and the caller gets one

    return S_OK;
}

void CAppLauncher::MakeError(long lMessageID, long lHResult, long lNAVErrorResID )
{
    if ( !m_spError )
        return;

  m_spError->put_ModuleID ( AV_MODULE_ID_NAVLNCH );
    m_spError->put_ErrorID ( lMessageID );
    m_spError->put_HResult ( lHResult );
    m_spError->put_ErrorResourceID ( lNAVErrorResID );
}

bool CAppLauncher::forceError(long lErrorID)
{
    if ( !m_spError )
        return false;

    long lTempErrorID = 0;
    long lTempModuleID = 0;

    m_spError->get_ForcedModuleID ( &lTempModuleID );
    m_spError->get_ForcedErrorID ( &lTempErrorID );

    if ( lTempModuleID == AV_MODULE_ID_NAVLNCH &&
         lTempErrorID == lErrorID )
         return true;
    else
        return false;
}

STDMETHODIMP CAppLauncher::LaunchNAV()
{
    DJSMAR_LicenseType licType = DJSMAR_LicenseType_Violated;

    STAHLSOFT_HRX_TRY(hr)
    {
        // Getting licensing and subscription properties needed to enable product features
        CPEPClientBase pepBase;
        hrx << pepBase.Init();
        pepBase.SetContextGuid(clt::pep::CLTGUID);
        pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
        pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
        pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
        pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

        hrx << pepBase.QueryPolicy();

        // Getting license type
        hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)licType, DJSMAR_LicenseType_Violated);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	CNAVInfo NAVInfo;
    char szPlugin[MAX_PATH] = {0};

	if(licType == DJSMAR_LicenseType_Rental)
		wsprintf(szPlugin, "/dat:%s\\NAVUI.nsi /nosysworks", NAVInfo.GetNAVDir());
	else
		wsprintf(szPlugin, "/dat:%s\\NAVUI.nsi", NAVInfo.GetNAVDir());

	HINSTANCE hInstance = ShellExecute(NULL, "open", "NMAIN.exe", szPlugin,
		NULL, SW_SHOW);

    if ( forceError (ERR_NO_NAV) || reinterpret_cast<int>(hInstance) <= 32 )
    {
		MakeError ( ERR_NO_NAV, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_NAV );
        return E_FAIL;
    }

	return S_OK;

}

STDMETHODIMP CAppLauncher::LaunchSubscriptionWizard()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		if (forceError(ERR_SECURITY_FAILED) || !IsItSafe())
		{
			MakeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_ACCESSDENIED;
		}

		// Check for a valid digital signature on the COM Server before loading it
		hrx << (( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVLicenseInfo))? E_FAIL : S_OK);

		CComPtr<INAVLicenseInfo> spNavLicense;
		CComPtr <ISymScriptSafe> spSymScriptSafe;

		hrx << spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL);
		hrx << spNavLicense.QueryInterface(&spSymScriptSafe);
		hrx << spSymScriptSafe->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
		hrx << spNavLicense->LaunchSubscriptionWizard(0);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAppLauncher::LaunchSubscriptionWizard - Error while accessing NAVLCOM object 0x%08X"), hr);
		MakeError(ERR_FAILED_LAUNCH_SUBSWIZ, hr, IDS_NAVERROR_NAVLNCH_FAILED_LAUNCH_SUBSWIZ);
	}

	return hr;
}

STDMETHODIMP CAppLauncher::LaunchURL(BSTR bstrURL)
{
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    HRESULT hr = S_OK;
    
    try
    {
        if ( ::SysStringLen ( bstrURL ) > MAX_PATH )
            return E_POINTER;

        // Launch the URL with the default browser. This may
        // not be IE, but since we run in an IE control we
        // can't use just a URL link. See defect # 405876.
        //
        NAVToolbox::CStartBrowser browser;
        TCHAR szURL [MAX_PATH] = {0};
        _bstr_t bstrTemp = bstrURL;
        if ( !browser.ShowURL ( bstrTemp ))
            hr = E_FAIL;
    }
    catch(...)
    {
        hr = E_UNEXPECTED;
    }

    return hr;
}

STDMETHODIMP CAppLauncher::GetBrandingURL(LONG nResourceId, BSTR* pVal)
{
	if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

	HRESULT hr = S_OK;

	try
	{
		CBrandingRes BrandRes;
		CComBSTR bstrURL(BrandRes.GetString(nResourceId));
		*pVal = bstrURL.Detach();
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAppLauncher::GetBrandingURL - exception 0x%08X"), hr);
	}

	return hr;
}


STDMETHODIMP CAppLauncher::LaunchVirusEncyclopedia(void)
{
	if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		MakeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
		return E_ACCESSDENIED;
	}

	HRESULT hr = S_OK;

	CBrandingRes BrandRes;
	NAVToolbox::CStartBrowser browser;
	if(!browser.ShowURL(BrandRes.GetString(IDS_SARC_VIRUS_ENCYCLOPEDIA_URL)))
		hr = E_FAIL;

	return hr;
}


STDMETHODIMP CAppLauncher::LaunchLiveUpdate()
{    
    return LaunchLiveUpdate(false);
}


STDMETHODIMP CAppLauncher::LaunchLiveUpdateAndWait()
{    
    return LaunchLiveUpdate(true);
}


