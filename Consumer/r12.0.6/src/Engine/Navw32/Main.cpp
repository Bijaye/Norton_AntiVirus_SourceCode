// Main.cpp

#include "Stdafx.h"

#define SIMON_INITGUID
#include "Simon.h"
#include "SymAlertInterface.h"

#define INITIIDS
#include "ScanTask.h"
#include "navtrust.h"   // we don't use this here, but someone does...
#include "ccSettingsInterface.h"
#include "NAVSettingsHelperEx.h"
//#include "ccAlertInterface.h"
#include "ScanMgrInterface.h"

#include <initguid.h>
// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepMiddleMan.h"
#include "PepClientBase.h"
#include "cltPEPConstants.h"

#include "SuiteOwnerHelper.h"

CString _g_csModuleFileName;

#include "Main.h"
#include "Navw32.h"
#include "NavInfo.h"

#include "AVccModuleId.h"
#include "NAVError.h"
#include "Resource.h"
#include "NAVWHLP5.H"
//#include "GlobalMutex.h"
#include "OptNames.h"
//#include "SSOsinfo.h"
#include "InstOptsNames.h"
#include "AVRESBranding.h"
#include "NAVOptHelperEx.h"

#include "NAVHelpLauncher.h"
#include "switches.h"

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            


////////////////////////////////////////////////////////////////////////////////
//
// Global variables.

// The following static global is for tagging the object code
// for this app as the property of Symantec, Inc. DO NOT DELETE!
static char const *gNAVCODETAG = NAVCODETAG;

HINSTANCE g_hInstance;

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("Navw32"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Crash handler
#include "ccSymCrashHandler.h"
ccSym::CCrashHandler g_CrashHandler;

SIMON::CSimonModule _SimonModule;

static const TCHAR SYM_NAVW32_MUTEX[] = _T("SYM_NAVW32_MUTEX");

//Resource loading code
#include "ccResourceLoader.h"
#include "..\navw32res\resource.h"
#define SYMC_RESOURCE_DLL _T("navw32.loc")
CAtlBaseModule _Module;
::cc::CResourceLoader g_ResLoader(&_Module, SYMC_RESOURCE_DLL);

////////////////////////////////////////////////////////////////////////////////

void MessageBox(HINSTANCE hInstance, UINT uiIDS)
{
	CString csTitle, csWarning;
	CBrandingRes BrandRes;
	CString csTempStr;
	csTitle = BrandRes.ProductName();
	csTempStr.LoadString(hInstance, uiIDS);
	csWarning.Format(csTempStr, csTitle);
			
	::MessageBox(::GetDesktopWindow(), csWarning, csTitle, MB_OK);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, char* szCmdLine, int iCmdShow)
{
    // Initialize COM so the settings are accessible
    HRESULT hrCoinit = S_OK;
    StahlSoft::CSmartModuleHandle shModule;
    DJSMAR_LicenseType licType;
    DJSMAR00_LicenseState licState;

	if( FAILED(hrCoinit = CoInitialize(NULL) ) )
    {
        CCTRACEE(_T("WinMain() - Failed to CoInitialize. HRESULT = 0x%X"), hrCoinit);
    }
    
    CNavw32 Navw32;

    // Post-reboot processing
    if(_tcsnicmp(szCmdLine, SWITCH_REMEDIATE_AFTER_REBOOT, _tcslen(SWITCH_REMEDIATE_AFTER_REBOOT)) == 0)
    {
        Navw32.ProcessRebootRemediations();

        // Uninitialize COM
        if( SUCCEEDED(hrCoinit) )
            CoUninitialize();

        return 0;
    }

    // Need to check for office AV flag so we abort the scan if
    // the feature is disabled and so we can skip the subscription alerting
    // for Office scans
    bool bIsOfficeAVScan = false;
    if (_tcsnicmp(szCmdLine, SWITCH_OFFICEAV, _tcslen(SWITCH_OFFICEAV)) == 0)
    {
        CCTRACEI(_T("WinMain() - Performing an Office scan."));
        bIsOfficeAVScan = true;

        // Make sure office AV feature is enabled
        DWORD dwOfficeFeatureEnabled = 1;
        CNAVOptSettingsEx NavSettings;
	    if( NavSettings.Init() )
        {
            if ( SUCCEEDED(NavSettings.GetValue(OFFICEAV_FeatureEnabled, dwOfficeFeatureEnabled, 1))
                 && dwOfficeFeatureEnabled == 0 )
            {
                CCTRACEI(_T("WinMain() - Office AV feature is disabled, not performing the scan"));
                return NAVWRESULT_NOINFECTIONS;
            }
        }
    }

    // We need to skip certain functionality in safe mode
    bool bIsSafeMode = false;
    if( GetSystemMetrics(SM_CLEANBOOT) )
    {
        bIsSafeMode = true;
        CCTRACEW(_T("WinMain() - Running in safe mode."));
    }

    // Enable the crash handler
    if (g_CrashHandler.LoadOptions() == FALSE)
    {
        CCTRACEE(_T("WinMain() : g_CrashHandler.LoadOptions() == FALSE"));
    }

	// Save our instance handle.

	g_hInstance = hInstance;

	// Log the command line.

	CCTRACEI("WinMain() - Command line: %s", szCmdLine);

    if(IsCfgwizFinished() == false)			//Defect 1-49XIKE: Navw32.exe launches even when CfgWiz is not complete
    {
        CCTRACEE("WinMain - Config Wiz has not been completed. Integrator should not be launched");
        return 0;
    }

    // Bypass licensing calls in safe mode
    bool bShowCED = (!bIsOfficeAVScan && _tcsnicmp(szCmdLine, SWITCH_SILENT_ERASER_SCAN, _tcslen(SWITCH_SILENT_ERASER_SCAN)) != 0);
    if( !bIsSafeMode )
    {
	    // Check if user has accepted EULA    	
	    if(HasUserAgreedToEULA() == false)
	    {
		    CCTRACEE("User has not accepted EULA. Abort manual scan");
			if(bShowCED)
			{
			    Navw32.NAVErrorDisplay(IDS_EULA_NOT_ACCEPTED, NULL);
			}
		    return 0;
	    }

        // Acquire licensing data
        if(!SUCCEEDED(PerformPepCall(licType, licState)))
        {
            CCTRACEE(_T("WinMain - Error while performing PEP call for licensing data."));
            return 0;
        }

        shModule = GetAlertDll();
        if(!shModule)
        {
            CCTRACEE(_T("WinMain - Error while loading alert DLL."));
            return 0;
        }

		// Check if license is valid
	    if(!IsLicenseValid(bShowCED, licState, shModule))
	    {
		    CCTRACEE("License is not valid. Abort manual scan.");
		    return -1;
	    }

        CCTRACEI("WinMain() - Finished NAVLicense::IsLicenseValid()");

    }

	// If the command line is empty, run the Integrator.
	if (_tcslen(szCmdLine) == 0)
	{
		return (RunIntegrator() == true ? 0 : -1);
	}

	// Should we pop-up the command line help?
	if (_tcscmp(szCmdLine, "/?") == 0)
	{
        HWND hwndHelp = NULL;
        if( hwndHelp = ShowHelp() )
        {
            while( IsWindow(hwndHelp) )
                Sleep(500);
            return 0;
        }
        else
		    return -1;
	}

    // Bypass subscription checking in safe mode
    if( !bIsSafeMode )
    {
        // Don't check subscription for office AV scans or for silent eraser scans
        if (bShowCED)
        {
            CCTRACEI("WinMain() - Calling CheckVirusDefsAndSubscriptionStatus()");
	        // Should we pop-up subscription or definitions alert?
	        CheckVirusDefsAndSubscriptionStatus(licType, licState, shModule);
            CCTRACEI("WinMain() - Finished CheckVirusDefsAndSubscriptionStatus()");
        }
    }

	// Create/open our mutex.
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, SYM_NAVW32_MUTEX);

	if (hMutex == NULL)
	{
		hMutex = CreateMutex(NULL, TRUE, SYM_NAVW32_MUTEX);
	}

	// Now perform the scan.
	NAVWRESULT NavwResult;
	SMRESULT Result = Navw32.Go(NavwResult);

    // If the error code is def authentication failure then scan manager already
    // alerted the user of the error
    if( Result == SMRESULT_ERROR_DEF_AUTHENTICATION_FAILURE )
    {
        NavwResult = NAVWRESULT_NOINFECTIONS;
    }

	// If necessary, display the appropriate error message.
	else if ((Result != SMRESULT_OK) && (Result != SMRESULT_ABORT))
	{
		UINT uMessageID;

		switch(Result)
		{
		case SMRESULT_ERROR_MEMORY:
			uMessageID = IDS_ERROR_MEMORY;
			break;
		case SMRESULT_ERROR_SCANNER_CRITICAL:
			uMessageID = IDS_ERROR_CRITICAL;
			break;
		case SMRESULT_ERROR_VIRUS_DEFS:
			uMessageID = IDS_ERROR_VIRUS_DEFS;
			break;
        case SMRESULT_ERROR_ERASER_REBOOT_REQUIRED:
            uMessageID = IDS_ERROR_ERASER_REBOOT_REQUIRED;
            break;
		default:
			uMessageID = IDS_ERROR_SCANNER;
		}

		// Use the Common Error Display for errors 
		Navw32.NAVErrorDisplay(uMessageID, NULL);
	}

    // Uninitialize COM
    if( SUCCEEDED(hrCoinit) )
        CoUninitialize();
	
	// Close our mutex.
	if (hMutex != NULL)
	{
		CloseHandle(hMutex);
	}

	// Return the proper return code so we know what happened in the scan
	return NavwResult;
}

bool RunIntegrator()
{
	CNAVInfo NAVInfo;
	char szPlugin[MAX_PATH];
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
        // Getting licensing properties
        hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)licType, DJSMAR_LicenseType_Violated);
	}
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(licType == DJSMAR_LicenseType_Rental)
		wsprintf(szPlugin, "/dat:%s\\NAVUI.nsi /nosysworks", NAVInfo.GetNAVDir());
	else
		wsprintf(szPlugin, "/dat:%s\\NAVUI.nsi", NAVInfo.GetNAVDir());

	HINSTANCE hInstance = ShellExecute(NULL, "open", "NMAIN.exe", szPlugin,
		NULL, SW_SHOW);

	if (hInstance > (HINSTANCE) 32)
	{
		return true;
	}

	return false;
}

HWND ShowHelp()
{
    HWND hwndHelp = NULL;
	NAVToolbox::CNAVHelpLauncher Help;
	if( !(hwndHelp = Help.LaunchHelp(IDH_NAVW_COMMAND_LINE_SWITCHES)) )
	{
		return NULL;
	}

	return hwndHelp;
}

HRESULT CheckVirusDefsAndSubscriptionStatus(DJSMAR_LicenseType &licType, DJSMAR00_LicenseState &licState, HMODULE hModule)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		STATUS_SUBSCRIPTION SubStatus = STATUS_SUBSCRIPTION_OK;
		SYMALERT_TYPE Type = SYMALERT_TYPE_INVALID;
		BOOL bShow = FALSE;    

		try
		{
			switch(licType)
			{
			case DJSMAR_LicenseType_Retail:
			case DJSMAR_LicenseType_Unlicensed:
			case DJSMAR_LicenseType_Trial:
			case DJSMAR_LicenseType_ESD:
			case DJSMAR_LicenseType_Beta:
            case DJSMAR_LicenseType_OEM:
                {
				    if(licState == DJSMAR00_LicenseState_PURCHASED)
				    {
                        CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - Checking subscription."));
                        SIMON::CSimonPtr<ISubscriptionAlert> spSubs;
					    hrx << SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CSubscriptionAlert, IID_ISubscriptionAlert,(void**)&spSubs);
					    hrx << spSubs->Init();
					    hrx << spSubs->ShouldShowAlert(&bShow, &Type, TRUE, SYMALERT_TYPE_SubExpiredScan|SYMALERT_TYPE_SubExpiredAutoRenewScan);
                        CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - ShouldShowAlert: %d"), bShow);
					    if(bShow)
					    {
                            CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - Showing alert"));
						    // Show alert
						    SIMON::CSimonPtr<IAlertDlg> spAlertDlg;
						    hrx << SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CAlertDlg, IID_IAlertDlg,(void**)&spAlertDlg);
						    hrx << spAlertDlg->Show(Type, NULL, TRUE);
					    }

					    // Cache subscription status for use later.
					    hrx << spSubs->GetSubscriptionStatus(&SubStatus);
                        CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - GetStatus: %x"), SubStatus);
				    }
			        else
			        {
				        CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - Suppress subscription alert for license type=%d, state=%d"), licType, licState);
			        }
                }
				break;

			default:
				{
					CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - Suppress subscription alert for license type=%d"), licType);
				}
				break;
			}
		}
		catch(_com_error& e)
		{
			CCTRACEE(_T("CheckVirusDefsAndSubscriptionStatus - Exception while accessing subscription alert 0x%08X"), e.Error());
		}

		// Check virus defs if license and subscription are valid.
		if((licState == DJSMAR00_LicenseState_PURCHASED) || 
			(licState == DJSMAR00_LicenseState_TRIAL))
		{
			if(SubStatus == STATUS_SUBSCRIPTION_EXPIRED)
			{
				CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - Suppress definitions alert because subscription has expired"));
				return S_OK;
			}
		}

        WCHAR modName[MAX_PATH];
        GetModuleFileNameW(hModule, modName, MAX_PATH);
        SIMON::CSimonPtr<ISymAlert> spDefs;
        Type = SYMALERT_TYPE_INVALID;
        bShow = FALSE;    
        HMODULE hDefsModule = NULL;

        CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - Check definitions alert"));

        if(wcsicmp(modName, L"defalert.dll") != 0)
        {
            CString csAlertDll;
            CNAVInfo NAVInfo;
            csAlertDll.Format(_T("%s\\DefAlert.dll"), NAVInfo.GetNAVDir());

            CCTRACEI(_T("CheckVirusDefsAndSubscriptionStatus - Loading %s"), csAlertDll);

            hDefsModule = LoadLibraryEx(csAlertDll, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
            hrx << ((hDefsModule == NULL) ? CO_E_DLLNOTFOUND : S_OK);
            hrx << SIMON::CreateInstanceByDLLInstance(hDefsModule, CLSID_CDefinitionsAlert, IID_ISymAlert,(void**)&spDefs);
        }
        else
        {
            hrx << SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CDefinitionsAlert, IID_ISymAlert,(void**)&spDefs);
        }

		hrx << spDefs->Init();
		hrx << spDefs->ShouldShowAlert(&bShow, &Type, TRUE, SYMALERT_TYPE_OldDefsScan);
		if(bShow)
		{
			// Show alert
			SIMON::CSimonPtr<IAlertDlg> spAlertDlg;
            if(hDefsModule == NULL)
            {
    			hrx << SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CAlertDlg, IID_IAlertDlg,(void**)&spAlertDlg);
            }
            else
            {
                hrx << SIMON::CreateInstanceByDLLInstance(hDefsModule, CLSID_CAlertDlg, IID_IAlertDlg,(void**)&spAlertDlg);
            }

			hrx << spAlertDlg->Show(SYMALERT_TYPE_OldDefsScan, NULL, TRUE);

            if(hDefsModule != NULL)
                FreeLibrary(hDefsModule);
		}
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CheckVirusDefsAndSubscriptionStatus - Exception while accessing defitions alert 0x%08X"), e.Error());
	}

	return hr;
}

bool IsCfgwizFinished()
{
	CCTRACEI("Navw32-Main:IsCfgwizFinished() Enter");

	bool bFinished = false;	
	CString csCfgWizDat;

	if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgWizDat))
	{
		CNAVOptFileEx CfgWizOpts;
		if(CfgWizOpts.Init(csCfgWizDat, FALSE))
		{
			DWORD dwValue = 0;
			CfgWizOpts.GetValue(InstallToolBox::CFGWIZ_Finished, dwValue, 0);

			if(dwValue == 1)
			{
				bFinished = true;
			}
		}
	}
	CCTRACEI("CHelper::IsCfgwizFinished() Exit(%s)", bFinished ? "TRUE" : "FALSE");

	return bFinished;
}

bool HasUserAgreedToEULA()
{
	bool bRet = false;
	StahlSoft::HRX hrx;

	try
	{
		CString csCfgwizDat;
		CNAVInfo NAVInfo;

		csCfgwizDat.Format(_T("%s\\CfgWiz.dat"), NAVInfo.GetNAVDir());

		CNAVOptFileEx cfgWizFile;
		CCTRACEI(_T("HasUserAgreedToEULA: CfgWiz.dat = %s"), csCfgwizDat);
		if(cfgWizFile.Init(csCfgwizDat, FALSE))
		{
			DWORD dwAgreedEULA = 0;
			DWORD dwOEM = 0;

			if(S_OK != cfgWizFile.GetValue(InstallToolBox::CFGWIZ_OEM, dwOEM, 0))
			{
				CCTRACEW(_T("HasUserAgreedToEULA : Failed to read CFGWIZ_OEM"));
			}
			if(dwOEM == 1 || dwOEM == 2)
			{
				// Check to see if OEM and CTO user 
				//  has already accepted EULA during cfgwiz.
				if(S_OK != cfgWizFile.GetValue(InstallToolBox::CFGWIZ_AgreedToEULA, dwAgreedEULA, 0))
				{
						CCTRACEE(_T("HasUserAgreedToEULA : Failed to read CFGWIZ_AgreedToEULA, defaulting to FALSE"));
				}
				if(dwAgreedEULA)
					bRet = true;
			}
			else
			{
				// Retail/SCSS users already accepted EULA during install.
				bRet = true;
			}

			CCTRACEI(_T("HasUserAgreedToEULA - OEM=%d, Agreed=%d"), dwOEM, dwAgreedEULA);
		}
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("HasUserAgreedToEULA - Exception thrown 0x%08X"), e.Error());
	}

	return bRet;
}

// Load the definitions alert DLL. The registry functionality has been
// absorbed from DRMPepHelper.h, since they no longer support that function.
HMODULE GetAlertDll()
{
    HMODULE hModule = NULL;
    CString csHandlerPath;
    CNAVInfo NAVInfo;

    STAHLSOFT_HRX_TRY(hr)
    {
        // Initialize csOwnerDLLPath to empty string.
        csHandlerPath.Empty();

        CRegKey rk;
        CString regPath(DRMREG::SZ_SUITE_OWNER_KEY);
        long rc = ERROR_SUCCESS;

        regPath += _T("\\");
        regPath += SZ_PRODUCT_SUITEOWNER_GUID;

        // Open the product suiteowner key in the registry
        rc = rk.Open(HKEY_LOCAL_MACHINE
            , regPath
            , KEY_READ);

        TCHAR *pszHandlerValueName =  (TCHAR *)DRMREG::SZ_ALERT_DLL_NAME;

        if(ERROR_SUCCESS == rc)
        {
            TCHAR szScratch[MAX_PATH] = {0};
            ULONG ulSize = MAX_PATH;

            // Read alert handler which is stored a full path in the registry
            rc = rk.QueryStringValue(pszHandlerValueName
                , szScratch
                , &ulSize);

            hrx << HRESULT_FROM_WIN32(rc);

            csHandlerPath = szScratch;
        }

        CCTRACEI(_T("GetAlertDll - Loading %s"), csHandlerPath);
        hModule = LoadLibraryEx(csHandlerPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

    return hModule;
}

HRESULT PerformPepCall(DJSMAR_LicenseType &licType, DJSMAR00_LicenseState &licState)
{
    CCTRACEI(_T("PerformPepCall - Executing PEP query for licensing information."));
    STAHLSOFT_HRX_TRY(hr)
    {
        // Getting licensing properties
        CPEPClientBase pepBase;
        hrx << pepBase.Init();
        pepBase.SetContextGuid(clt::pep::CLTGUID);
        pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
        pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
        pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
        pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

        hrx << pepBase.QueryPolicy();

        hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)licType, DJSMAR_LicenseType_Violated);
        hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD &)licState, DJSMAR00_LicenseState_Violated);

        CCTRACEI(_T("PerformPepCall - Type=%08X, State=%08X"), licType, licState);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

    return hr;
}

bool IsLicenseValid(bool bVerbose, DJSMAR00_LicenseState &licState, HMODULE hModule)
{
	bool bValid = false;

    if(licState == DJSMAR00_LicenseState_PURCHASED || 
	    licState == DJSMAR00_LicenseState_TRIAL)
    {
	    return true;
    }

	STAHLSOFT_HRX_TRY(hr)
	{
		// License is not valid, display an error if requested.
		if(bVerbose)
        {
            CCTRACEI(_T("IsLicenseValid - Checking for license alert."));
            SIMON::CSimonPtr<ILicensingAlert> spLicense;
            BOOL bShow;
            SYMALERT_TYPE licType = SYMALERT_TYPE_INVALID;
            hrx << SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CLicensingAlert, IID_ILicensingAlert,(void**)&spLicense);
            hrx << spLicense->ShouldShowAlert(&bShow, &licType, false, 0);
            if(bShow)
            {
                CCTRACEI(_T("IsLicenseValid - Displaying license alert."));
                SIMON::CSimonPtr<IAlertDlg> spAlertDlg;
                hrx << SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CAlertDlg, IID_IAlertDlg,(void**)&spAlertDlg);
                hrx << spAlertDlg->Show(licType, NULL, FALSE);

                // See if we're licensed now
                hrx << spLicense->Refresh(true);
                hrx << spLicense->GetLicenseState(&licState);
                bValid = (licState == DJSMAR00_LicenseState_PURCHASED);
            }
        }

	}
	catch(_com_error& e)
	{
		CCTRACEI(_T("IsLicenseValid() - Failed to display licensing nag %08X"), e.Error());
	}

    CCTRACEI(_T("IsLicenseValid - Done."));
	return bValid;
}