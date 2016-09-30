////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Main.cpp

#include "Stdafx.h"

#define INITIIDS
#define SIMON_INITGUID
#include <initguid.h>   // we don't use this here, but someone does...
#include <navtrust.h>   // we don't use this here, but someone does...
#include <ccSettingsInterface.h>
#include <NAVSettingsHelperEx.h>
#include <AVInterfaces.h>
#include <ScanUIInterfaces.h>

#include <initguid.h>
CString _g_csModuleFileName;

#include "Main.h"
#include "Navw32.h"
#include <NavInfo.h>

#include <AVccModuleId.h>
#include "Resource.h"
#include <NAVWHLP5.H>
#include <OptNames.h>
#include <InstOptsNames.h>
#include "ISVersion.h"
#include <NAVOptHelperEx.h>
#include "ScanTaskLoader.h"

#include <SymHelpLauncher.h>
#include <switches.h>
#include "SRTSPProtectThread.h"

// CLT defines and includes
#include "cltProdInfoPepConstants.h"
#include "cltproducthelper.h"
#include "SymAlertStatic.h"
#include "SymAlertInterface.h"

// Element stuff for product info
#include "uiNISDataElementGuids.h"
#include "uiProviderInterface.h"
#include "uiElementInterface.h"
#include "uiNumberDataInterface.h"
#include "uiDateDataInterface.h"
#include "ISDataClientLoader.h"
#include "uiPEPConstants.h"

// errors
#include "isErrorLoader.h"

ui::IProviderPtr g_pProvider;

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
#include <ccLibDllLink.h>
#include <ccSymDelayLoader.h>
ccSym::CDelayLoader g_DelayLoader;

#include <ccTrace.h>
#include <ccSymDebugOutput.h>
ccSym::CDebugOutput g_DebugOutput(_T("Navw32"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Crash handler
#include <ccSymCrashHandler.h>
ccSym::CCrashHandler g_CrashHandler;

static const TCHAR SYM_NAVW32_MUTEX[] = _T("SYM_NAVW32_MUTEX");

//Resource loading code
#include <ccResourceLoader.h>
#include <..\navw32res\resource.h>
#define SYMC_RESOURCE_DLL _T("navw32.loc")
CAtlBaseModule _Module;
::cc::CResourceLoader g_ResLoader(&_Module, SYMC_RESOURCE_DLL);

////////////////////////////////////////////////////////////////////////////////
void MessageBox(HINSTANCE hInstance, UINT uiIDS)
{
	CString csTitle, csWarning;
	CString csTempStr;
	csTitle = CISVersion::GetProductName();
	csTempStr.LoadString(hInstance, uiIDS);
	csWarning.Format(csTempStr, csTitle);

	::MessageBox(::GetDesktopWindow(), csWarning, csTitle, MB_OK);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, wchar_t* szCmdLine, int iCmdShow)
{
    // Initialize COM so the settings are accessible
    HRESULT hrCoinit = S_OK;
    StahlSoft::CSmartModuleHandle shModule;
	if(FAILED(hrCoinit = CoInitialize(NULL)))
    {
        CCTRACEE(_T("WinMain() - Failed to CoInitialize. HRESULT = 0x%X"), hrCoinit);
    }

    CNavw32 Navw32;

    // Need to check for office AV flag so we abort the scan if
    // the feature is disabled and so we can skip the subscription alerting
    // for Office scans
    bool bIsOfficeAVScan = false;
    if(_tcsnicmp(szCmdLine, SWITCH_OFFICEAV, _tcslen(SWITCH_OFFICEAV)) == 0)
    {
        CCTRACEI(_T("WinMain() - Performing an Office scan."));
        bIsOfficeAVScan = true;
#pragma message("CLT: this double check looks like overkill - Garret Polk")
        // Make sure office AV feature is enabled
        DWORD dwOfficeFeatureEnabled = 1;
        CNAVOptSettingsEx NavSettings;
	    if(NavSettings.Init())
        {
            if(SUCCEEDED(NavSettings.GetValue(OFFICEAV_FeatureEnabled, dwOfficeFeatureEnabled, 1))
                 && dwOfficeFeatureEnabled == 0)
            {
                CCTRACEI(_T("WinMain() - Office AV feature is disabled, not performing the scan"));
                return NAVWRESULT_NOINFECTIONS;
            }
        }
    }

    // We need to skip certain functionality in safe mode
    bool bIsSafeMode = false;
    if(GetSystemMetrics(SM_CLEANBOOT))
    {
        bIsSafeMode = true;
        CCTRACEW(_T("WinMain() - Running in safe mode."));
    }

    // Enable the crash handler
    if(g_CrashHandler.LoadOptions() == FALSE)
    {
        CCTRACEE(_T("WinMain() : g_CrashHandler.LoadOptions() == FALSE"));
    }

	// Save our instance handle.
	g_hInstance = hInstance;

	// Log the command line.
	CCTRACEI(_T("WinMain() - Command line: %s"), szCmdLine);

    // Figure out if we should display the CED
    bool bShowCED =(!bIsOfficeAVScan && _tcsnicmp(szCmdLine, SWITCH_SILENT_ERASER_SCAN, _tcslen(SWITCH_SILENT_ERASER_SCAN)) != 0);

    // Bypass licensing calls in safe mode (and debug)
#ifdef  _DEBUG
    bIsSafeMode = true;
#endif

    // Can show UI - CfgWiz and EULA
    // Bypass EULA/CFGWIZ calls in safe mode
    if(!bIsSafeMode && !CanShowUI(bShowCED, Navw32))
        return 0;

#if 0   // Per Javed and Herb - navw32 will no longer check license/trial/subscription states
    if(!bIsSafeMode)
    {
        CCLTProductHelper cltHelper;
        if(!SUCCEEDED(cltHelper.Initialize()))
        {
            CCTRCTXE0(_T("Error while initializing CLT Helper."));
            // CLT7 TODO: What error string to display here?
            Navw32.NAVErrorDisplay(IDS_LICENSE_VIOLATED, NULL);
            return 0;
        }

        shModule = GetAlertDll();

        if(!shModule)
        {
            CCTRCTXE0(_T("Error while loading alert DLL."));
            return 0;
        }


        bool bCanRun = true;
        if(!SUCCEEDED(cltHelper.CanIRun(bCanRun)) || !bCanRun)
        {
            // No prompts from NAVW32 ... it just exits
			CCTRCTXE0(_T("CanIRun failed or returned false. Silently terminating."));
			return 0;
        }

		bool bSubExpired;
		if(FAILED(cltHelper.IsSubscriptionExpired(bSubExpired)))
		{
			CCTRCTXE0(_T("Error querying for expired subscription. Assuming good."));
			bSubExpired = false;
		}
		Navw32.SetSubscriptionCOHOverride(bSubExpired);
    }
#endif

	// If the command line is empty, run the Integrator.
	if(_tcslen(szCmdLine) == 0)
	{
		return(RunIntegrator() == true ? 0 : -1);
	}

	// Should we pop-up the command line help?
	if(_tcscmp(szCmdLine, _T("/?")) == 0)
	{
        HWND hwndHelp = NULL;
        if(hwndHelp = ShowHelp())
        {
            while(IsWindow(hwndHelp))
                Sleep(500);
        return 0;
	}
        else
		    return -1;
	}

#if 0   // Per Javed and Herb - navw32 will no longer check license/trial/subscription states
    // Bypass subscription checking in safe mode
    if(!bIsSafeMode)
    {
        // Don't check subscription for office AV scans or for silent eraser scans
        if(bShowCED)
        {
			HRESULT hr;
            CCTRACEI(_T("WinMain() - Calling CheckVirusDefsAndSubscriptionStatus()"));
	        // Should we pop-up subscription or definitions alert?
	        if(FAILED(hr = CheckVirusDefsAndSubscriptionStatus(shModule)))
			{
				CCTRCTXE1(_T("Error calling CheckVirusDefsAndSubscriptionStatus. HR = 0x%08x"), hr);
				Navw32.NAVErrorDisplay(IDS_LICENSE_VIOLATED, NULL);
				return 0;
			}
            CCTRACEI(_T("WinMain() - Finished CheckVirusDefsAndSubscriptionStatus()"));
        }
    }
#endif

	// Create/open our mutex.
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, SYM_NAVW32_MUTEX);

	if(hMutex == NULL)
	{
		hMutex = CreateMutex(NULL, TRUE, SYM_NAVW32_MUTEX);
	}

	// Now perform the scan.
	NAVWRESULT NavwResult;
    avScanUI::eScanResult eResult = Navw32.Go(NavwResult);

	// If necessary, display the appropriate error message.
    if(eResult >= avScanUI::SRESULT_CED_NEEDED_FIRST && eResult < avScanUI::SRESULT_CED_DONE_FIRST)
	{
		UINT uMessageID;
		switch(eResult)
		{
            case avScanUI::SRESULT_ERROR_MEMORY:
			    uMessageID = IDS_ERROR_MEMORY;
			    break;
            case avScanUI::SRESULT_ERROR_CAV:
            case avScanUI::SRESULT_ERROR_SCANUI:
			    uMessageID = IDS_ERROR_SCANNER;
			    break;
		    default:
			    uMessageID = IDS_ERROR_CRITICAL;
		}

		// Use the Common Error Display for errors
		Navw32.NAVErrorDisplay(uMessageID, NULL);
	}

    // Uninitialize COM
    if(SUCCEEDED(hrCoinit))
        CoUninitialize();

	// Close our mutex.
	if(hMutex != NULL)
	{
		CloseHandle(hMutex);
	}

	// Return the proper return code so we know what happened in the scan
	return NavwResult;
}

bool LaunchElement(SYMGUID element)
{
    if(!g_pProvider)
        return false;

    ui::IElementPtr pElement;
    SYMRESULT sr = g_pProvider->GetElement(element, pElement);

    if(SYM_SUCCEEDED(sr))
    {
        sr = pElement->Configure(::GetDesktopWindow(), NULL);

        if(SYM_SUCCEEDED(sr))
        {
            CCTRCTXI0(_T("Launched element OK"));
            return true;
        }
        else
        {
            CCTRCTXE1(_T("Failed to launch 0x%x"), sr);
        }
    }
    else
    {
        CCTRCTXE1(_T("Failed to getelement 0x%x"), sr);
    }

    return false;
}

bool RunIntegrator()
{
    return LaunchElement(ISShared::CLSID_NIS_MainUI);
}

HWND ShowHelp()
{
    HWND hwndHelp = NULL;
	isshared::CHelpLauncher Help;
	if(!(hwndHelp = Help.LaunchHelp(IDH_NAVW_COMMAND_LINE_SWITCHES)))
	{
		return NULL;
	}

	return hwndHelp;
}

HRESULT CheckVirusDefsAndSubscriptionStatus(HMODULE hModule)
{
	HRESULT hr;
	CCLTProductHelper cltHelper;
	BOOL bShow;
	clt::cltLicenseAttribs licAttribs;
	clt::cltLicenseType licType;
	// Assume expired, since this will only be set if sub is active.
	STATUS_SUBSCRIPTION subStatus = STATUS_SUBSCRIPTION_EXPIRED;
	SYMALERT_TYPE symType;


	if(FAILED(hr = cltHelper.Initialize()))
	{
		CCTRCTXE1(_T("Error initializing CLT Helper object. HR = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = cltHelper.GetLicenseType(licType)))
	{
		CCTRCTXE1(_T("Error retrieving license type. HR = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = cltHelper.GetLicenseAttributes(licAttribs)))
	{
		CCTRCTXE1(_T("Error retrieving license attributes. HR = 0x%08x"), hr);
		return hr;
	}

    try
	{
		switch(licType)
		{
		case clt::cltLicenseType_Activation:
		case clt::cltLicenseType_Unlicensed:
		case clt::cltLicenseType_TryBuy:
		case clt::cltLicenseType_TryDie:
		case clt::cltLicenseType_ESD:
		case clt::cltLicenseType_Beta:
		case clt::cltLicenseType_OEM:
            {
				// Check subscription if license is active
				if((licAttribs & clt::cltLicenseAttrib_Active) == clt::cltLicenseAttrib_Active)
				{
                    CCTRCTXI0(_T("Checking subscription."));
                    SIMON::CSimonPtr<ISubscriptionAlert> spSubs;
					if(FAILED(hr = SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CSubscriptionAlert, IID_ISubscriptionAlert,(void**)&spSubs)))
					{
						CCTRCTXE1(_T("Error creating SIMON instance. HR = 0x%08x"), hr);
						return hr;
					}

					if(FAILED(hr = spSubs->Init()))
					{
						CCTRCTXE1(_T("Error initializing ISubscriptionAlert. HR = 0x%08x"), hr);
						return hr;
					}

					if(FAILED(hr = spSubs->ShouldShowAlert(&bShow, &symType, TRUE, SYMALERT_TYPE_SubExpiredScan|SYMALERT_TYPE_SubExpiredAutoRenewScan)))
					{
						CCTRCTXE1(_T("Error calling ShouldShowAlert. HR = 0x%08x"), hr);
						return hr;
					}

                    CCTRCTXI1(_T("ShouldShowAlert: %d"), bShow);
					if(bShow)
					{
                        CCTRCTXI0(_T("Showing alert"));
						// Show alert
						SIMON::CSimonPtr<IAlertDlg> spAlertDlg;
						if(FAILED(hr = SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CAlertDlg, IID_IAlertDlg,(void**)&spAlertDlg)))
						{
							CCTRCTXE1(_T("Error creating Simon CAlertDlg instance. HR = 0x%08x"), hr);
							return hr;
						}

						if(FAILED(hr = spAlertDlg->Show(symType, NULL, TRUE)))
						{
							CCTRCTXE1(_T("Error calling CAlertDlg::Show. HR = 0x%08x"), hr);
							return hr;
						}
					}

					// Cache subscription status for use later.
					if(FAILED(hr =  spSubs->GetSubscriptionStatus(&subStatus)))
					{
						CCTRCTXE1(_T("Error getting subscription status. HR = 0x%08x"), hr);
						return hr;
					}

                    CCTRCTXI1(_T("GetStatus: %x"), subStatus);
				}
			    else
			    {
				    CCTRCTXI2(_T("Suppress subscription alert for license type=%d, attribs=%d"), licType, licAttribs);
			    }
            }
			break;

		default:
			{
				CCTRCTXI1(_T("Suppress subscription alert for license type=%d"), licType);
			}
			break;
		}
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CheckVirusDefsAndSubscriptionStatus - Exception while accessing subscription alert 0x%08X"), e.Error());
	}

	// Check subscription only if license has been activated.
	if((licAttribs & clt::cltLicenseAttrib_Active) == clt::cltLicenseAttrib_Active)
	{
		if(subStatus == STATUS_SUBSCRIPTION_EXPIRED)
		{
			CCTRCTXI0(L"Suppress definitions alert because subscription has expired");
			return S_OK;
		}
	}

    WCHAR modName[MAX_PATH];
    GetModuleFileNameW(hModule, modName, MAX_PATH);
    SIMON::CSimonPtr<ISymAlert> spDefs;
    symType = SYMALERT_TYPE_INVALID;
    bShow = FALSE;
    HMODULE hDefsModule = NULL;

    CCTRCTXI0(_T("Check definitions alert"));

    if(wcsicmp(modName, L"defalert.dll") != 0)
    {
        CString csAlertDll;
        CNAVInfo NAVInfo;
        csAlertDll.Format(_T("%s\\DefAlert.dll"), NAVInfo.GetNAVDir());

        CCTRCTXI1(_T("Loading %s"), csAlertDll);

        hDefsModule = LoadLibraryEx(csAlertDll, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
        if(hDefsModule == NULL)
		{
			CCTRCTXE1(_T("DLL %s not found."), csAlertDll);
			return CO_E_DLLNOTFOUND;
		}

        if(FAILED(hr = SIMON::CreateInstanceByDLLInstance(hDefsModule, CLSID_CDefinitionsAlert, IID_ISymAlert,(void**)&spDefs)))
		{
			CCTRCTXE1(_T("Error creating SIMON CDefinitionsAlert instance. HR = 0x%08x"), hr);
			return hr;
		}
    }
    else
    {
        if(FAILED(hr = SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CDefinitionsAlert, IID_ISymAlert,(void**)&spDefs)))
		{
			CCTRCTXE1(_T("Error creating SIMON CDefinitionsAlert instance. HR = 0x%08x"), hr);
			return hr;
		}
    }

	if(FAILED(hr = spDefs->Init()))
	{
		CCTRCTXE1(_T("Error calling CDefinitionsAlert::Init. HR = 0x%08x"), hr);
		return hr;
	}

	if(FAILED(hr = spDefs->ShouldShowAlert(&bShow, &symType, TRUE, SYMALERT_TYPE_OldDefsScan)))
	{
		CCTRCTXE1(_T("Error calling ShouldShowAlert. HR = 0x%08x"), hr);
		// Do not bail out just because we cannot show definitions out-of-date alert
	}

	if(bShow)
	{
		// Show alert
		SIMON::CSimonPtr<IAlertDlg> spAlertDlg;
        if(hDefsModule == NULL)
        {
    		if(FAILED(hr = SIMON::CreateInstanceByDLLInstance(hModule, CLSID_CAlertDlg, IID_IAlertDlg,(void**)&spAlertDlg)))
			{
				CCTRCTXE1(_T("Error creating SIMON CAlertDlg instance. hr = 0x%08x"), hr);
				return hr;
			}
        }
        else
        {
			if(FAILED(hr = SIMON::CreateInstanceByDLLInstance(hDefsModule, CLSID_CAlertDlg, IID_IAlertDlg,(void**)&spAlertDlg)))
			{
				CCTRCTXE1(_T("Error creating SIMON CAlertDlg instance. HR = 0x%08x"), hr);
				return hr;
			}
        }

		if(FAILED(hr = spAlertDlg->Show(SYMALERT_TYPE_OldDefsScan, NULL, TRUE)))
		{
			CCTRCTXE1(_T("Error calling CAlertDlg::Show. Hr = 0x%08x"), hr);
			return hr;
		}

        if(hDefsModule != NULL)
            FreeLibrary(hDefsModule);
	}

	return S_OK;
}

bool CanShowUI(bool bShowCED, CNavw32& Navw32)
{
    // Check business rules for displaying UI
    ISShared::ISShared_IProvider providerFactory;
    SYMRESULT sr = providerFactory.CreateObject(GETMODULEMGR(), &g_pProvider);
    if(SYM_FAILED(sr))
    {
        CCTRCTXE1(_T("Failed to create provider = 0%x80"), sr);
        return 0;
    }

    // Can show UI - CfgWiz and EULA
    //Defect 1-49XIKE: Navw32.exe launches even when CfgWiz is not complete
    bool bRet = false;
    ui::IElementPtr pElementCanShow;
    ui::INumberDataQIPtr pDataCanShow;
    if(SYM_SUCCEEDED(g_pProvider->GetElement(ISShared::CLSID_NIS_CanShowUI, pElementCanShow)))
    {
        ui::IDataPtr pData;
        if(SYM_SUCCEEDED(pElementCanShow->GetData(pData)))
        {
            pDataCanShow = pData;

            LONGLONG i64State = 0;
            if(SYM_SUCCEEDED(pDataCanShow->GetNumber(i64State)))
            {
                if(0 == i64State)
                {
		            CCTRCTXE0(_T("User has not accepted EULA. Abort manual scan"));
			        if(bShowCED)
			        {
			            Navw32.NAVErrorDisplay(IDS_CFGWIZ_NOT_COMPLETE, NULL);
			        }

                    bRet = false;
                }
                else
                    bRet = true;
            }

        }
    }

    return bRet;
}

// Load the definitions alert DLL. The registry functionality has been
// absorbed from DRMPepHelper.h, since they no longer support that function.
HMODULE GetAlertDll()
{
    HMODULE hModule = NULL;
    CString csHandlerPath;
	CNAVInfo navInfo;

    STAHLSOFT_HRX_TRY(hr)
    {
        // Initialize csOwnerDLLPath to empty string.
        csHandlerPath.Empty();
		csHandlerPath = navInfo.GetNAVDir();
		csHandlerPath += _T("\\defalert.dll"); // yay for hardcoding
        CCTRACEI(_T("GetAlertDll - Loading %s"), csHandlerPath);
        hModule = LoadLibraryEx(csHandlerPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

    return hModule;
}
