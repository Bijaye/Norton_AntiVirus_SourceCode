#include "stdafx.h"

#define INITIIDS
#include "ccverifytrustinterface.h"
#include "navtrust.h"
#include "ccSettingsInterface.h"
#include "ccServicesConfigInterface.h"

#include "globals.h"
#include "NAVOptions.h"
#include "NAVOptions_i.c"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "OSInfo.h"         // Toolbox
#include "resource.h"
#include "NAVError.h"
#include "..\\NavOptionsRes\\resource.h"
#include "GlobalEvents.h"
#include "ccWebWnd.h"
#include "ccWebWnd_i.c"
#include "NAVLicenseNames.h"
#include "NAVOptHelperEx.h"

#include "NAVSettingsHelperEx.h"
#include "simon.h"
#include "NAVBusinessRules.h"
#include "StahlSoft.h"
#include "SyncHelper.h"
#include "StahlSoft_EventHelper.h"
#include "NAVInfo.h"
#include "NavLicense_h.h"
#include "NavLicense_i.c"

#include "InstOptsNames.h"
#include "NAVErrorResource.h"

bool g_bStandAlone = false;
bool g_bLicensingAlertRequestSent = false;
bool g_bInitLicense = true;
DJSMAR_LicenseType g_LicenseType = DJSMAR_LicenseType_Violated;
DJSMAR00_LicenseState g_LicenseState = DJSMAR00_LicenseState_Violated;
long g_LicenseZone = DJSMAR_LicenseZone_Violated;
CString g_csProductName;

#include "ccResourceLoader.h"
#include "..\navuires\resource.h"
extern cc::CResourceLoader g_ResLoader;

HRESULT GetLicensingInfo()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		if(g_bInitLicense)
		{
			// Check for a valid digital signature on the COM Server before loading it
			hrx << (( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVLicenseInfo))? E_FAIL : S_OK);

			CComPtr<INAVLicenseInfo> spNavLicense;
			CComPtr <ISymScriptSafe> spSymScriptSafe;

			hrx << spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL);
			hrx << spNavLicense.QueryInterface(&spSymScriptSafe);
			hrx << spSymScriptSafe->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
			hrx << spNavLicense->GetLicenseType(&g_LicenseType);
			hrx << spNavLicense->GetLicenseState(&g_LicenseState);
			hrx << spNavLicense->GetLicenseZone(&g_LicenseZone);

			g_bInitLicense = false;

			CCTRACEI(_T("GetLicensingInfo - Type=0X%08X, State=0X%08X, Zone=0X%08X"), 
				g_LicenseType, g_LicenseState,  g_LicenseZone);
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("GetLicensingInfo - Failed to get license data 0x%08X"), hr);
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////
//
// Functions:    ErrorMessageBox 
//
// Description:
//  Displays fatal errors to the user using the common 
//  error handler.  The version that takes in the error message
//  as a string also requires the ErrorID for the error so it can
//  pass the information to the Common Error Handler
//
//////////////////////////////////////////////////////////////////////////

void ErrorMessageBox ( HWND hMainWnd, BSTR bstrText, UINT ErrorID, UINT uModuleID )
{
    USES_CONVERSION;    // ATL needs this to do OLE2...

    // Debug Log this error first
    CCTRACEI ( OLE2T (bstrText) );

    // This variable indicates that the NAVError object could not display the
    // error and a messagebox should be used instead
    bool bDisplayBaseError = false;

    // Get a NAVError object
    CComBSTR bstrNAVErr (_T("NAVError.NAVCOMError"));
    CComPtr <INAVCOMError> spNavError;

    // Create, Populate, Log, and Display the error
    if( SUCCEEDED (spNavError.CoCreateInstance( bstrNAVErr, NULL, CLSCTX_INPROC_SERVER )) &&
        SUCCEEDED (spNavError->put_ModuleID(uModuleID)) &&   // Module ID defined in ModuleID.h
        SUCCEEDED (spNavError->put_ErrorID(ErrorID)) && // Error ID is the resource ID for the error message
        SUCCEEDED (spNavError->put_Message(bstrText)) &&
        SUCCEEDED (spNavError->LogAndDisplay(reinterpret_cast<unsigned long>(hMainWnd)))
      )
    {
        // Everything worked so we're done
        bDisplayBaseError = false;
    }
    else
        bDisplayBaseError = true;

    // This indicates that for some reason the NAVError object could not display
    // the error so a standard messagebox will be displayed instead
    if( bDisplayBaseError )
    {   
        ::MessageBox(hMainWnd, OLE2T(bstrText), g_csProductName, MB_ICONERROR);
    }
}

//////////////////////////////////////////////////////////////////////////
//
// Functions:    ErrorMessageBox 
//
// Description:
//  Displays fatal errors to the user using the common 
//  error handler.  The version that takes in the error message ID
//  which identifies a generic error message in NAVError.dll as well as
//  the ErrorID for the error so it can
//  pass the information to the Common Error Handler
//
//////////////////////////////////////////////////////////////////////////
void ErrorMessageBox ( HWND hMainWnd, UINT uNAVErrorResID, UINT ErrorID, UINT uModuleID )
{
	// Get a NAVError object
	CComBSTR bstrNAVErr (_T("NAVError.NAVCOMError"));
	CComPtr <INAVCOMError> spNavError;

	// Create, Populate, Log, and Display the error
	if( SUCCEEDED (spNavError.CoCreateInstance( bstrNAVErr, NULL, CLSCTX_INPROC_SERVER )) &&
		SUCCEEDED (spNavError->put_ModuleID(uModuleID)) &&   // Module ID defined in ModuleID.h
		SUCCEEDED (spNavError->put_ErrorID(ErrorID)) && // Error ID is the resource ID for the error message
		SUCCEEDED (spNavError->put_ErrorResourceID(uNAVErrorResID)) &&
		SUCCEEDED (spNavError->LogAndDisplay(reinterpret_cast<unsigned long>(hMainWnd)))
		)
	{
		// Everything worked so we're done
		CCTRACEE("Display error successful- NAVErrorResID: %d, ErrorID:%d, ModID:%d", uNAVErrorResID, ErrorID, AV_MODULE_ID_MAIN_UI);
	}
	else
	{
		CCTRACEE("Display error failed - NAVErrorResID: %d, ErrorID:%d, ModID:%d", uNAVErrorResID, ErrorID, AV_MODULE_ID_MAIN_UI);
	}
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    WarningMessageBox 
//
// Description:
//  Displays warnings and non fatal errors to the user. This does
//  not use the common error handler.
//
//////////////////////////////////////////////////////////////////////////
BOOL WarningMessageBox ( HWND hMainWnd, UINT uIDText, UINT uIcon /*= MB_ICONERROR*/ )
{
	CString csText;
	CString csFormat;

	g_ResLoader.LoadString(uIDText,csFormat);
	csText = csFormat;

	// Format string with product name if needed
	switch(uIDText)
	{
	case IDS_ADMIN_OPTIONS:
		csText.Format(csFormat, g_csProductName);
		break;

	default:
		break;
	}

    ::MessageBox ( hMainWnd, csText, g_csProductName, uIcon );

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    DoOptionsScreen 
//
// Description:
//  Displays NAV's options dialog.
//
// Input:
//  hParentWnd: [in] Parent window for the dialog.
//  pPage: [in/out] On entry, this variable indicates the page that should
//         be active when the dialog appears.  On exit, the variable is
//         set to the page that was active when the user closed the dialog.
//
// Returns:
//  The return from NavDoOptionsDialog() in OptsStub.DLL.  This will be
//  -1 if an error happened and the dialog couldn't be shown.
//
//////////////////////////////////////////////////////////////////////////

BOOL DoOptionsScreen ( HWND hParentWnd )
{
	// Only admins can open options.
	//
	if ( g_OSInfo.IsAdminProcess () )
	{
		HRESULT hr;
		
		CComPtr<INAVOptions> spNAVOptions;
		CComPtr <ISymScriptSafe> spSymScriptSafe;
		
		// Check the NAV options dll for a valid symantec signature
		if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVOptions) )
		{
			// Display the error
			ErrorMessageBox( hParentWnd, IDS_NAVERROR_INTERNAL_REINSTALL, IDS_NO_TRUST_NAVOPTIONS );
			
			// Returning TRUE Since the error message was already handled
			return TRUE;
		}
		
		if (SUCCEEDED(hr = spNAVOptions.CoCreateInstance(CLSID_NAVOptions, NULL, CLSCTX_INPROC)))
		{
			if ( SUCCEEDED ( spNAVOptions.QueryInterface(&spSymScriptSafe)))
			{
				// Set appropriate access so that NAVStatus call to IsItSafe() 
				// returns successfully.
				
				long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
				long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
				spSymScriptSafe->SetAccess(dwAccess, dwKey);
			}
			
			// If NAVOPTION:FeatureEnabled is off, 
			// don't launch the option window.
            // Getting licensing and subscription properties needed to enable product features
            DJSMAR00_LicenseState state = DJSMAR00_LicenseState_Violated;
            long zone = DJSMAR_LicenseZone_Violated;

            STAHLSOFT_HRX_TRY(hr)
            {
				// Check for a valid digital signature on the COM Server before loading it
				hrx << (( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVLicenseInfo))? E_FAIL : S_OK);

				CComPtr<INAVLicenseInfo> spNavLicense;
				CComPtr <ISymScriptSafe> spSymScriptSafe;

				hrx << spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL);
				hrx << spNavLicense.QueryInterface(&spSymScriptSafe);
				hrx << spSymScriptSafe->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
				hrx << spNavLicense->GetLicenseState(&state);
				hrx << spNavLicense->GetLicenseZone(&zone);

				CCTRACEI(_T("NAVUI::DoOptionsScreen - state: %d, zone: %d"), state, zone);
            }
            STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

			BOOL bOptionsEnabled = true;
			UINT uMsgId = 0;

			switch(state)
			{
			case DJSMAR00_LicenseState_PURCHASED:
			case DJSMAR00_LicenseState_TRIAL:
				break;

			case DJSMAR00_LicenseState_EXPIRED:
				bOptionsEnabled = false;
				if((zone & DJSMAR_LicenseZone_Killed) == DJSMAR_LicenseZone_Killed)
				{
					uMsgId = IDS_TOOLBAR_FEATURE_DISABLED_ON_KILLED;
				}
				else
				{
					uMsgId = IDS_TOOLBAR_FEATURE_DISABLED_ON_EXPIRED;
				}
				break;

			case DJSMAR00_LicenseState_Violated:
				bOptionsEnabled = false;
				uMsgId = IDS_TOOLBAR_FEATURE_DISABLED_ON_VIOLATED;
				break;
			}

			if(!bOptionsEnabled)
			{
				// Show "Options feature not allowed." to users.
				TCHAR szBuff[MAX_PATH] = {0};
				TCHAR szCaption[MAX_PATH] = {0};

				g_ResLoader.LoadString(uMsgId, szBuff, sizeof(szBuff)/sizeof(TCHAR));
				::LoadString(_getPrimaryResourceInstance(), IDS_AV_PRODUCT_NAME, szCaption, sizeof(szCaption)/sizeof(TCHAR));
				::MessageBox(hParentWnd, szBuff, szCaption, MB_ICONEXCLAMATION);

				return TRUE;
			}
			
			hr = spNAVOptions->Show( reinterpret_cast<long>(hParentWnd));

			if (S_EXIST == hr && g_OSInfo.IsTerminalServicesInstalled())
			{
				TCHAR szOptionsPath [MAX_PATH] = {0};
				
				_tcscpy ( szOptionsPath, g_NAVInfo.GetNAVDir ());
				_tcscat ( szOptionsPath, "\\NAVOpts.dll" );
				
				HMODULE hOpts = ::LoadLibraryEx( szOptionsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
				
				if (hOpts)
				{
					TCHAR szLoadingTitleFormat[128] = {0};
					TCHAR szTitleFormat[128] = {0};
					TCHAR szWarning[512] = {0};

					// The dialog title used while the options dialog is loading.
					::LoadString(hOpts,     IDS_Title,              szTitleFormat,        128);
					::LoadString(hOpts,     IDS_LoadingTitle,       szLoadingTitleFormat, 128);
					g_ResLoader.LoadString(IDS_WRN_ANOTHERSESSION, szWarning, CCDIMOF(szWarning));

					// Format string with product name
					CString csLoadingTitle;
					CString csTitle;

					csTitle.Format(szTitleFormat, g_csProductName);
					csLoadingTitle.Format(szLoadingTitleFormat, g_csProductName);

					// We allow only one instance of the options dialog.
					if (NULL == ::FindWindow(ccWebWindow_ClassName, csLoadingTitle) // During loading the tile is different
						&& NULL == ::FindWindow(ccWebWindow_ClassName, csTitle))
					{
						CCTRACEI ( szWarning );
						::MessageBox(::GetDesktopWindow(), szWarning, csTitle, MB_OK);
					}
					
					FreeLibrary ( hOpts );
					CloseHandle ( hOpts );
				}
			}
			
			if ( SUCCEEDED (hr))
			{
				::SetForegroundWindow(hParentWnd);  
				return TRUE;
			}
		}
	}
	else
	{
		// This is the error, so return TRUE. Otherwise you get two error messages.
		//
		WarningMessageBox ( hParentWnd, IDS_ADMIN_OPTIONS, MB_OK | MB_ICONEXCLAMATION );
		return TRUE;
	}
	
	return FALSE;
}

bool forceError(long lErrorID)
{
	CComBSTR bstrErrorClass ("NAVError.NAVCOMError");
	CComPtr <INAVCOMError> spNavError;

	// Check NAVError module for Symantec Signature...
	//
	if(FAILED(spNavError.CoCreateInstance(bstrErrorClass, NULL,CLSCTX_INPROC_SERVER)))
	{		
		return false;
	}

	long lTempErrorID = 0;
	long lTempModuleID = 0;

	spNavError->get_ForcedModuleID ( &lTempModuleID );
	spNavError->get_ForcedErrorID ( &lTempErrorID );

	if ( lTempModuleID == AV_MODULE_ID_MAIN_UI &&
		lTempErrorID == lErrorID )
		return true;
	else
		return false;
}

void SetNamedEvent(LPCTSTR lpcEvent)
{
    StahlSoft::CSmartHandle shEventLicense;
    shEventLicense = ::CreateEvent(NULL, FALSE, FALSE, lpcEvent);
    if(shEventLicense)
    {
        SetEvent(shEventLicense);
    }
}

void RequestLicensingAlert(HWND hWndParent, LONG nFlag)
{
	if(g_bLicensingAlertRequestSent)
		return;

	if(g_LicenseType == DJSMAR_LicenseType_Unlicensed)
	{
		// No nag for unlicensed type.
		g_bLicensingAlertRequestSent = true;
		return;
	}


	// Check to see if we need to suppress nag 
	//  on first launch of main UI.
	CNAVInfo NAVInfo;
	TCHAR szCfgWizDat[MAX_PATH] = {0};
	DWORD dwNoNag = 0;
	CNAVOptFileEx cfgWizFile;

	::wsprintf(szCfgWizDat, _T("%s\\CfgWiz.dat"), NAVInfo.GetNAVDir());

	if(cfgWizFile.Init(szCfgWizDat, FALSE))
	{
		cfgWizFile.GetValue(InstallToolBox::CFGWIZ_NoNagOnFirstMainUI, dwNoNag, 0);
		if(dwNoNag == 1)
		{
			// Reset the flag so that main UI
			//  will display nag in subsequent launch.
			if(SUCCEEDED(cfgWizFile.SetValue(InstallToolBox::CFGWIZ_NoNagOnFirstMainUI, (DWORD) 0)))
			{
				cfgWizFile.Save();
			}

			g_bLicensingAlertRequestSent = true;
			return;
		}
	}

	// For Rental, alert on main UI only if license state
	//  or zone changes or on timer since some SCSS alerts 
	//  allow users to select "Notify me in x days".
	//  For other license types, always force an alert.

	long nAlertFlag = REQUEST_LICENSING_ALERT_ON_DEMAND;

	switch(nFlag)
	{
	case HWNDPARENT_NULL:
		hWndParent = NULL;
		break;

	case HWNDPARENT_FIND:
		hWndParent = GetActiveWindow();
		break;

	case HWNDPARENT_PASSED:
	default:
		break;
	}

	// Find defalert window.
	HWND hWndDefAlert = FindWindow(SZ_DEFALERT_WNDCLASSNAME, SZ_DEFALERT_WNDNAME);

	if(hWndDefAlert)
	{
		// Send a message to defalert to display a licensing alert.
		UINT uLicenseAlertMsgID = ::RegisterWindowMessage(MSG_NAV_LICENSING_ALERT);
		if(0 != PostMessage(hWndDefAlert, uLicenseAlertMsgID, nAlertFlag, (LPARAM) hWndParent))
		{
			g_bLicensingAlertRequestSent = true;
		}
	}
}


bool ShouldShowHelpMenuOption(LPCTSTR lpcszMenuOptName)
{
	bool bShow = false;

	try
	{
		// Check flag in the Settings.
		DWORD dwShow = 1;

		READ_SETTING_START()
			READ_SETTING_DWORD(lpcszMenuOptName, dwShow, 1)
		READ_SETTING_END

		if(dwShow == 1)
			bShow = true;
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("Failed to read %s from the Settings - 0x%08X"), lpcszMenuOptName, e.Error());
	}

	return bShow;    
}

bool ShouldShowRenewalCenterHelpMenu()
{
	// Do we want to display this menu item?
	return ShouldShowHelpMenuOption(HELPMENU_ShowRenewalCenter);
}
