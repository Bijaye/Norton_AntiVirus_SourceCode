// NAVAlertDlg.cpp

#include "stdafx.h"
#include <simon.h>

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepClientBase.h"

#include "navtrust.h"
#include <initguid.h>
#include <atlsafe.h>
#include "resource.h"
#include "..\defalertres\resource.h"
#include "NavAlertDlg.h"
#include "ccWebWnd_h.h"				// Display alert dialog
#include "ccWebWnd_i.c"
#include "NavLicense_h.h"				// Resolve licensing
#include "NavLicense_i.c"	
#include "NetDetectController.h"                // For AutoLiveUpdate
#include "NAVError.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVLicenseNames.h"
#include "XLOK_UiProperties.h"
#include "StartBrowser.h"
#include "DRMNamedProperties.h"
#include "NAVSecurity.h"
#include "NAVUIHTM_Resource.h"
#include "NAVErrorResource.h"

using namespace ccSym;

typedef struct 
{
	SYMALERT_TYPE	Type;
	TCHAR			szHTML[MAX_PATH];
	long			nResIdWidth;
	long			nResIdHeight;
} NAVALERT_HTML;

NAVALERT_HTML g_ahtml[] =  
{
	SYMALERT_TYPE_OldDefs					,_T("OldDefs.htm")					,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_OLDDEFS 
	,SYMALERT_TYPE_OldDefsScan				,_T("Scan_OldDefs.htm")				,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_OLDDEFS_SCAN 
	,SYMALERT_TYPE_SubWarning				,_T("SubWarning.htm")				,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_SUBWARNING
	,SYMALERT_TYPE_SubExpired				,_T("SubExpired.htm")				,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_SUBEXPIRED
	,SYMALERT_TYPE_SubExpiredScan			,_T("Scan_SubExpired.htm")			,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_SUBEXPIRED_SCAN
	,SYMALERT_TYPE_SubWarningAutoRenew		,_T("SubWarningAutoRenew.htm")		,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_SUBWARNING_AUTORENEW
	,SYMALERT_TYPE_SubExpiredAutoRenew		,_T("SubExpiredAutoRenew.htm")		,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_SUBEXPIRED_AUTORENEW
	,SYMALERT_TYPE_SubExpiredAutoRenewScan	,_T("Scan_SubExpiredAutoRenew.htm")	,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_SUBEXPIRED_SCAN_AUTORENEW
	,SYMALERT_TYPE_LicRentalPreActivExpired	,_T("LicPreActivExpiredRental.htm")	,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_RENTAL_LICPREACTIVEEXPIRED
	,SYMALERT_TYPE_LicRentalWarning			,_T("LicWarningRental.htm")			,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_RENTAL_LICWARNING
	,SYMALERT_TYPE_LicRentalExpired			,_T("LicExpiredRental.htm")			,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_RENTAL_LICEXPIRED
	,SYMALERT_TYPE_LicRentalKilled			,_T("LicKilledRental.htm")			,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_RENTAL_LICKILLED
	,SYMALERT_TYPE_LicRentalPreActivGrace	,_T("LicPreActivGraceRental.htm")	,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_RENTAL_LICPREACTIVEGRACE
	,SYMALERT_TYPE_LicRentalPreActivGraceExpired	,_T("LicPreActivGraceExpiredRental.htm")	,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_RENTAL_LICPREACTIVEGRACEEXPIRED
	,SYMALERT_TYPE_ALUDisabled				,_T("ALUDisabled.htm")				,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_ALUDISABLED
	,SYMALERT_TYPE_INVALID					,_T("\0")							,0					,0
};

bool CAlertDlg::LookupHTML(SYMALERT_TYPE hType, int& iHTMLIndex)
{
    bool bResult = false;
    int idx = 0;

    while(g_ahtml[idx].Type != SYMALERT_TYPE_INVALID)
    {
        if(g_ahtml[idx].Type == hType)
        {
            iHTMLIndex = idx;
            bResult = true;
            break;
        }
        ++idx;
    }
    return bResult;
}

HRESULT CAlertDlg::GetAlertHTMLInfo(SYMALERT_TYPE Type, LPTSTR lpszHTML, long& nWidth, long& nHeight)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		hrx << (lpszHTML? S_OK : E_POINTER);

		StahlSoft::CSmartModuleHandle shResDLL;
		CNAVInfo NAVInfo;
		TCHAR szResDLL[MAX_PATH] = {0};
		DWORD dwLength = sizeof(szResDLL)/sizeof(TCHAR);				

		switch(Type)
		{
		case SYMALERT_TYPE_LicRetailPreActiv:
		case SYMALERT_TYPE_LicRetailPreActivExpired:
		case SYMALERT_TYPE_LicTryDiePreActiv:
		case SYMALERT_TYPE_LicTryDiePreActivExpired:
			{
				// Get DRM alerts from DJSAlert.dll.
				//  This is required for all products.			
				wsprintf(szResDLL, _T("%s\\DJSAlert.dll"), NAVInfo.GetNAVDir());

				shResDLL = LoadLibrary(szResDLL);
				hrx << ((shResDLL == NULL) ? E_FAIL : S_OK);

				RECT rectHTML = {0};

				typedef bool (WINAPI* PFGETHTMLINFO)(SYMALERT_TYPE, LPTSTR, RECT&);
				PFGETHTMLINFO pfGetHTMLInfo = NULL;
				pfGetHTMLInfo = (PFGETHTMLINFO)(GetProcAddress(shResDLL, "_GetAlertHTMLInfo@12"));
				hrx << ((pfGetHTMLInfo == NULL) ? E_FAIL : S_OK);
				hrx << ((pfGetHTMLInfo(Type, lpszHTML, rectHTML) == false)? E_FAIL : S_OK);

				nWidth = rectHTML.right;
				nHeight = rectHTML.bottom;
			}
			break;

		default:
			{
				int idx = 0;
				hrx << (LookupHTML(Type, idx) ? S_OK : E_FAIL);	

				// Get alert HTML, width, and height from NAVUIHTM.
				wsprintf(szResDLL, _T("%s\\NAVUIHTM.dll"), NAVInfo.GetNAVDir());

				shResDLL = LoadLibrary(szResDLL);
				hrx << ((shResDLL == NULL) ? E_FAIL : S_OK);

				CString csWidth, csHeight;

				csWidth.LoadString(shResDLL, g_ahtml[idx].nResIdWidth);
				csHeight.LoadString(shResDLL, g_ahtml[idx].nResIdHeight);

				nWidth = atol(csWidth);
				nHeight = atol(csHeight);

				// Setup full URL path of the alert.
				wsprintf(lpszHTML, _T("res://%s/%s"), szResDLL, g_ahtml[idx].szHTML);
			}
			break;
		}

	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAlertDlg::GetAlertHTMLInfo - Failed to get alert html info 0x%08X"), hr);
	}

	return hr;
}

SIMON_STDMETHODIMP CAlertDlg::Show(SYMALERT_TYPE Type, HWND hwndParent, BOOL bWait)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		StahlSoft::CSmartHandle smMutex;
		
		switch(Type)
		{
		case SYMALERT_TYPE_LicTryBuyPreActiv:
		case SYMALERT_TYPE_LicTryBuyPreActivExpired:
		case SYMALERT_TYPE_LicRentalPreActivTrial:
		case SYMALERT_TYPE_LicRentalPreActivTrialExpired:
			{
				// Save time displayed.
				time_t curtime = 0;
				time(&curtime);
				_SetAlertDword(_LookupALERT_TimeDisplay(Type), curtime);

				// No baby nag. Go straight to Activation Wizard.
				DoTask(SYMALERT_RESULT_SIGNUP_SERVICE, hwndParent, bWait);
				return S_OK;
			}
			break;

		case SYMALERT_TYPE_SubExpiredScan:
		case SYMALERT_TYPE_SubExpiredAutoRenewScan:
			{
				// Before we display subscription expired on scan alert,
				// create mutex to prevent subscription expired on timer alert appears.
				CCTRACEI(_T("CAlertDlg::Show - create subscription expired on scan alert mutex"));
				smMutex = CreateMutex(NULL, FALSE, SZ_NAV_SUBEXPIRED_ONSCAN_ALERT_MUTEX);
				if((HANDLE)smMutex == (HANDLE)NULL)
				{
					CCTRACEE(_T("CAlertDlg::Show - Failed to create subscription expired on scan alert mutex"));
				}
			}
			break;

		case SYMALERT_TYPE_OldDefsScan:
			{
				// Before we display OldDefs on scan alert,
				// create mutex to prevent OldDefs on timer alert appears.
				CCTRACEI(_T("CAlertDlg::Show - create old defs on scan alert mutex"));
				smMutex = CreateMutex(NULL, FALSE, SZ_NAV_OLDDEFS_ONSCAN_ALERT_MUTEX);
				if((HANDLE)smMutex == (HANDLE)NULL)
				{
					CCTRACEE(_T("CAlertDlg::Show - Failed to create old defs on scan alert mutex"));
				}
			}
			break;

		default:
			break;
		}

		// Save time displayed.
		time_t curtime = 0;
		time(&curtime);
		_SetAlertDword(_LookupALERT_TimeDisplay(Type), curtime);

		// Load alert HTML dialog from NAVUIRES.DLL.
		TCHAR szURL[MAX_PATH] = {0};
		long nWidth = 0;
		long nHeight = 0;
		hrx << GetAlertHTMLInfo(Type, szURL, nWidth, nHeight);

		// Pass NAVLicenseInfo object to the alert dialog.
		//  The alert dialog could use this object to 
		//  retrieve product name and number of days left.
		CComPtr<INAVLicenseInfo> spNavLicense;
		CComPtr <ISymScriptSafe> spSymScriptSafe;
		CComPtr<IDispatch> spDispatch;
		long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
		long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;

		hrx << spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL);
		hrx << spNavLicense.QueryInterface(&spSymScriptSafe);
		hrx << spSymScriptSafe->SetAccess(dwAccess, dwKey);
		hrx << spNavLicense.QueryInterface(&spDispatch);

		CComBSTR bstrLink = szURL;
		_variant_t varObjectArg(spDispatch);
		_variant_t vRetVal;
		_variant_t vInput;

		// Check for a valid digital signature 
		//  on the COM Server before loading it
		hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_ccWebWindow)) ? E_FAIL : S_OK);

		CComPtr<IccWebWindow> spWebCtrl;
		spWebCtrl.CoCreateInstance(CLSID_ccWebWindow, NULL, CLSCTX_ALL);
		spWebCtrl->SetIcon((long) _Module.GetModuleInstance(), NAVW_ICO);
		spWebCtrl->put_FrameWindowStyle(WS_SYSMENU | WS_CAPTION);
		spWebCtrl->put_ObjectArg(varObjectArg);

		hr = spWebCtrl->showModalDialogEx((long) hwndParent, bstrLink, 
			nWidth, nHeight, &vInput, &vRetVal);

		//  If failed because hWndParent is no longer valid,
		//  Call the same function again with hWndParent = NULL.
		if(FAILED(hr))
		{
			hwndParent = NULL;
			hrx << spWebCtrl->showModalDialogEx((long) hwndParent, bstrLink, 
				nWidth, nHeight, &vInput, &vRetVal);
		}

		if(Type == SYMALERT_TYPE_ALUDisabled)
		{
			// The ALUDisabled alert to be displayed only once and only once.
			//  Set flag to show that we have already displayed the alert.
			_SetAlertDword(ALERT_ALUDisabledShown, 1, FALSE);
		}

		// Execute users' request:
		//  1. Resolve licensing status.
		//  2. Renew subscription
		//  3. Launch LiveUpdate
		//  4. Launch Uninstall
		//  5. Enable ALU.
		//  5. Cancel.
		SYMALERT_RESULT Result = SYMALERT_RESULT_CANCEL;
		Result = (SYMALERT_RESULT) (vRetVal.lVal);

		if(Result != SYMALERT_RESULT_CANCEL)
		{
			DoTask(Result, hwndParent, bWait);
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
	}

	return hr;
}

HRESULT CAlertDlg::LaunchActivationWizard(HWND hWndParent, long nParam)
{
	StahlSoft::HRX hrx;
	BOOL bSuccess = FALSE;
	HRESULT hr = S_OK;

	try
	{
		// Check for a valid digital signature on the COM Server before
		// loading it
		hrx << (( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVLicenseInfo))? E_FAIL : S_OK);

		CComPtr<INAVLicenseInfo> spNavLicense;
		CComPtr <ISymScriptSafe> spSymScriptSafe;
		long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
		long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;

		hrx << spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL);
		hrx << spNavLicense.QueryInterface(&spSymScriptSafe);
		hrx << spSymScriptSafe->SetAccess(dwAccess, dwKey);
		hrx << spNavLicense->LaunchActivationWizardEx((long) hWndParent, nParam, HWNDPARENT_PASSED);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAlertDlg::LaunchActivationWizard - Exception while accessing License object %08X"), hr);
	}

	return hr;
}

HRESULT CAlertDlg::LaunchUninstall()
{
		TCHAR szCmd[MAX_PATH] = {0};
		STARTUPINFO startupInfo = {0};
		PROCESS_INFORMATION processInfo = {0};

		startupInfo.cb = sizeof(startupInfo);

		::_tcscpy(szCmd, _T("rundll32.exe shell32.dll,Control_RunDLL appwiz.cpl,,0"));

		::CreateProcess(NULL, szCmd, NULL, NULL, FALSE, 
			NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInfo);

	return S_OK;
}

HRESULT CAlertDlg::LaunchSubscriptionWizard(HWND hWndParent)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		// Check for a valid digital signature on the COM Server before loading it
		hrx << (( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVLicenseInfo))? E_FAIL : S_OK);

		CComPtr<INAVLicenseInfo> spNavLicense;
		CComPtr <ISymScriptSafe> spSymScriptSafe;

		hrx << spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL);
		hrx << spNavLicense.QueryInterface(&spSymScriptSafe);
		hrx << spSymScriptSafe->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
		hrx << spNavLicense->LaunchSubscriptionWizard((long) hWndParent);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAlertDlg::LaunchSubscriptionWizard - Exception while launching subscription wizard 0x%08X"), hr);
	}

	return hr;
}

HRESULT CAlertDlg::LaunchLiveUpdate(BOOL bWait)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		// Check to see if LU is already running.

		{ /* SCOPE shMutex */
			StahlSoft::CSmartHandle shMutex;
			shMutex = ::OpenMutex(SYNCHRONIZE, FALSE, _T("Symantec.LuComServer.Running"));				
			if(shMutex)
			{
				// LU is currently running, bring it to front.
				HWND hWndLU = FindWindow(_T("#32770"), _T("LiveUpdate"));
				if(hWndLU)
				{
					SetForegroundWindow(hWndLU);

					if(bWait)
					{
						// Wait for LU to go away.
						StahlSoft::WaitForSingleObjectWithMessageLoop(shMutex);
					}

					return S_OK;
				}
			}
		} /* END OF SCOPE shMutext */

		// Get path to c:\\program files\\symantec\\LiveUpdate\\LUALL.EXE
		CRegKey key;
		TCHAR szLuall[MAX_PATH] = {0};
		DWORD dwLength = sizeof(szLuall)/sizeof(TCHAR);

		hrx << HRESULT_FROM_WIN32(key.Open(HKEY_LOCAL_MACHINE,_T("software\\symantec\\SharedUsage"), KEY_QUERY_VALUE));
		hrx << HRESULT_FROM_WIN32(key.QueryStringValue(_T("LiveUpdate"), szLuall, &dwLength));

		// Get the second to last char in the path.
		TCHAR* pszDirNull = ::_tcsrchr(szLuall, '\0');
		TCHAR* pszLastSlash = ::CharPrev(szLuall, pszDirNull);

		// If the ending character is a slash, null it.
		if(0 == ::_tcscmp(pszLastSlash, "\\"))
			*pszLastSlash = '\0';

		GetShortPathName(szLuall, szLuall, MAX_PATH);

		// Append LUALL.exe to the path
		::_tcscat(szLuall, _T("\\LUALL.EXE"));

		// Check LUALL.exe for a valid Symantec signature before launching it
		hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szLuall)) ? E_FAIL : S_OK);

		STARTUPINFO startupInfo = {0};
		PROCESS_INFORMATION processInfo = {0};
		startupInfo.cb = sizeof(startupInfo);

		::CreateProcess(NULL, szLuall, NULL, NULL, FALSE, 
			NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInfo);

		if(bWait)
		{
			// Wait for LiveUpdate to go away.
			DWORD dwWait = StahlSoft::WaitForSingleObjectWithMessageLoop(processInfo.hThread);
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAlertDlg::LaunchLiveUpdate - Exception while launching LU %08X"), hr);
	}

	return hr;
}

HRESULT CAlertDlg::EnableALU()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		// Read LiveUpdate modes from scheduler object.	
		// Check for a valid digital signature on the COM Server before loading it
		hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NDScheduler))? E_FAIL : S_OK);

		// When the user checks/unchecks "Enable automatic LiveUpdate",
		//  NavOptions no longer updates mode of the Scheduler as it did in version 2002.
		//  So we don't need to check for the mode, simply set m_bEnableALU to state
		//  of NSDScheduler.
		CComPtr<INDScheduler> spScheduler;
		hrx << spScheduler.CoCreateInstance(CLSID_NDScheduler, NULL, CLSCTX_INPROC_SERVER);
		hrx << spScheduler->Init();		
		hr = spScheduler->Enable();	// Make it go
		if(FAILED(hr))
		{
			// Enable() failed because it's not there
			hr = spScheduler->AddTask();
			if(FAILED(hr))
			{
				// Failed to add, it's probably because it exists already.
				hr = spScheduler->RestartSchedule();
			}

			// Try to make it go again.
			hrx << spScheduler->Enable();
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAlertDlg::EnableALU - Exception while accessing the Scheduler %08X"), hr);
	}

	return hr;
}

HRESULT CAlertDlg::DoTask(SYMALERT_RESULT Result, HWND hWndParent, BOOL bWait)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		switch(Result)
		{
		case SYMALERT_RESULT_RESOLVELICENSE:
		case SYMALERT_RESULT_ACTIVATE_SERVICE:
		case SYMALERT_RESULT_SIGNUP_SERVICE:
		case SYMALERT_RESULT_LAUNCH_ACTWIZ_KILL_PAGE:
		case SYMALERT_RESULT_CONFIRM_SERVICE:
			{
				long nParam = Result;
				if(Result == SYMALERT_RESULT_SIGNUP_SERVICE)
				{
					// ActWiz will figure out which panel to display
					//  for SCSS trial or trial expired.
					nParam = -1;
				}

				hr = LaunchActivationWizard(hWndParent, nParam);
				if(FAILED(hr))
				{
					_ErrorMessageBox(NULL, ERR_ACTIVATE_LICENSE, IDS_NAVERROR_INTERNAL_REINSTALL);
				}
			}
			break;

		case SYMALERT_RESULT_UNINSTALL:
			hrx << LaunchUninstall();
			break;

		case SYMALERT_RESULT_RENEWSUBS:
			hrx << LaunchSubscriptionWizard(hWndParent);
			break;

		case SYMALERT_RESULT_LAUNCHLU:
			// Pass in bWait to support async/synchronization for Manual scan.
			hrx << LaunchLiveUpdate(bWait);
			break;

		case SYMALERT_RESULT_ENABLEALU:
			hrx << EnableALU();
			break;

		case SYMALERT_RESULT_LAUNCH_SERVICE_REACTIVATION_WEBSITE:
			{
				TCHAR szURL[1024] = {0};
				hrx << GetLicenseProperty(DRM::szDRMSCSSReenableURL, szURL, sizeof(szURL)/sizeof(TCHAR));

				NAVToolbox::CStartBrowser browser;
				hrx << (browser.ShowURL(szURL) ? S_OK : E_FAIL);
			}
			break;

		case SYMALERT_RESULT_LAUNCH_UPSELL_WEBSITE:
			{
				TCHAR szURL[1024] = {0};
				hrx << GetLicenseProperty(DRM::szDRMSCSSUpsellUrl, szURL, sizeof(szURL)/sizeof(TCHAR));

				NAVToolbox::CStartBrowser browser;
				hrx << (browser.ShowURL(szURL) ? S_OK : E_FAIL);
			}
			break;

		case SYMALERT_RESULT_LAUNCHLU_AND_ENABLE_ALU:
			LaunchLiveUpdate(bWait);
			EnableALU();
			break;

		default:
			break;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAlertDlg::DoTask - Exception while executing Result=%d, Err=%08X"), Result, hr);
	}

	return hr;
}


HRESULT CAlertDlg::GetLicenseProperty(LPCTSTR lpszProperty, LPTSTR lpszBuff, DWORD dwLength)
{
	if((lpszProperty == NULL) || (lpszBuff == NULL))
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	try
	{
		StahlSoft::HRX hrx;
		StahlSoft::CSmartDataPtr<BYTE> spBuff;

		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(clt::pep::CLTGUID);
		pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
		pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
		pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

		hrx << pepBase.QueryPolicy();
        pepBase.GetCollectionData(lpszProperty, spBuff);

		_tcsncpy(lpszBuff, (LPCSTR) spBuff.m_p, dwLength);

		CCTRACEI(_T("CAlertDlg::GetLicenseProperty() - %s=%s"), lpszProperty, lpszBuff);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CAlertDlg::GetLicenseProperty() failed %s, error=0x%08X"), lpszProperty, hr);
	}

	return hr;
}
