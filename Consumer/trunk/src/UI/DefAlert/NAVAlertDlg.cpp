////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVAlertDlg.cpp

#include "stdafx.h"
#include "navtrust.h"
#include <initguid.h>
#include <atlsafe.h>
#include "resource.h"
#include "isErrorIDs.h"
#include "isErrorResource.h"
#include "NavAlertDlg.h"
#include "ccWebWnd_h.h"				// Display alert dialog
#include "ccWebWnd_i.c"
#include "NetDetectController.h"                // For AutoLiveUpdate
#include "NAVError.h"
#include "StartBrowser.h"
#include "NAVSecurity.h"
#include "NAVUIHTM_Resource.h"
#include "cltProdInfoPEPConstants.h"
#include "Subscription.h"
#include "uiNISDataElementGuids.h"
#include "ISDataClientLoader.h"
#include "uiNumberDataInterface.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "isResource.h"
#include "ISVersion.h"
#include "isSymTheme.h"

using namespace ccSym;
using namespace ui;
using namespace ISShared;

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
		,SYMALERT_TYPE_LicRentalKilled			,_T("LicKilledRental.htm")			,IDS_ALERT_WIDTH	,IDS_ALERT_HEIGHT_RENTAL_LICKILLED
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
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (lpszHTML? S_OK : E_POINTER);

		StahlSoft::CSmartModuleHandle shResDLL;
		CNAVInfo NAVInfo;
		TCHAR szResDLL[MAX_PATH] = {0};
		DWORD dwLength = sizeof(szResDLL)/sizeof(TCHAR);				

		int idx = 0;
		hrx << (LookupHTML(Type, idx) ? S_OK : E_FAIL);	

		// Get alert HTML, width, and height from NAVUIHTM.
		wsprintf(szResDLL, _T("%s\\NAVUIHTM.dll"), NAVInfo.GetNAVDir());

		shResDLL = LoadLibraryEx(szResDLL, NULL, LOAD_LIBRARY_AS_DATAFILE);
		hrx << ((shResDLL == NULL) ? E_FAIL : S_OK);

		CString csWidth, csHeight;

		csWidth.LoadString(shResDLL, g_ahtml[idx].nResIdWidth);
		csHeight.LoadString(shResDLL, g_ahtml[idx].nResIdHeight);

		nWidth = _tstol(csWidth);
		nHeight = _tstol(csHeight);

		// Setup full URL path of the alert.
		wsprintf(lpszHTML, _T("res://%s/%s"), szResDLL, g_ahtml[idx].szHTML);

	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}

SIMON_STDMETHODIMP CAlertDlg::Show(SYMALERT_TYPE Type, HWND hwndParent, BOOL bWait)
{
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		// Do not display alert if symsetup is currently running.
		ccLib::CMutex mutexSymSetup;
		if(mutexSymSetup.Open(SYNCHRONIZE, FALSE, SYMSETUP_MUTEX, TRUE))
		{
			CCTRCTXI0(L"Skip showing alert because symsetup is currently running.");
			return S_FALSE;
		}

		// Save time displayed.
		time_t curtime = 0;
		time(&curtime);
		_SetAlertDword(_LookupALERT_TimeDisplay(Type), curtime);

		// Set skin window
		// Create SymTheme Skin window.
		// Do not bail out if failed.
		// Should continue to display Scheduler dialog
		HRESULT hr1 = S_OK;
		CISSymTheme isSymTheme;
		hr1 = isSymTheme.Initialize(_Module.GetModuleInstance());
		if(FAILED(hr1))
		{
			CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr1);
		}


		switch(Type)
		{
		case SYMALERT_TYPE_LicTryBuyPreActiv:
		case SYMALERT_TYPE_LicTryBuyPreActivExpired:
		case SYMALERT_TYPE_LicRentalPreActivTrial:
		case SYMALERT_TYPE_LicRentalPreActivTrialExpired:
			{
				// No baby nag. Go straight to Activation Wizard.
				DoTask(SYMALERT_RESULT_SIGNUP_SERVICE, hwndParent, bWait);
				return S_OK;
			}
			break;

		case SYMALERT_TYPE_LicRetailPreActiv:
		case SYMALERT_TYPE_LicRetailPreActivExpired:
		case SYMALERT_TYPE_LicTryDiePreActiv:
		case SYMALERT_TYPE_LicTryDiePreActivExpired:
		case SYMALERT_TYPE_LicRentalPreActivExpired:
		case SYMALERT_TYPE_LicRentalWarning:
		case SYMALERT_TYPE_LicRentalExpired:
		case SYMALERT_TYPE_LicRentalPreActivGrace:
		case SYMALERT_TYPE_LicRentalPreActivGraceExpired:
			{
				CCTRCTXI1(L"Calling PEP to display alert type %d", Type);
				hrx << m_CltHelper.Initialize();
				hrx << m_CltHelper.LaunchAlert(Type);
				return S_OK;
			}		
			break;

		case SYMALERT_TYPE_SubExpiredScan:
		case SYMALERT_TYPE_SubExpiredAutoRenewScan:
			{
				// Before we display subscription expired on scan alert,
				// create mutex to prevent subscription expired on timer alert appears.
				CCTRCTXE0(L"Create subscription expired on scan alert mutex");
				StahlSoft::CSmartHandle smMutex;
				smMutex = CreateMutex(NULL, FALSE, SZ_NAV_SUBEXPIRED_ONSCAN_ALERT_MUTEX);
				if((HANDLE)smMutex == (HANDLE)NULL)
				{
					CCTRCTXE0(L"Failed to create subscription expired on scan alert mutex");
				}
			}
			break;

		case SYMALERT_TYPE_OldDefsScan:
			{
				// Before we display OldDefs on scan alert,
				// create mutex to prevent OldDefs on timer alert appears.
				CCTRACEI(_T("CAlertDlg::Show - create old defs on scan alert mutex"));
				StahlSoft::CSmartHandle smMutex;
				smMutex = CreateMutex(NULL, FALSE, SZ_NAV_OLDDEFS_ONSCAN_ALERT_MUTEX);
				if((HANDLE)smMutex == (HANDLE)NULL)
				{
					CCTRCTXE0(L"Failed to create old defs on scan alert mutex");
				}
			}
			break;

		default:
			break;
		}

		// Load alert HTML dialog from NAVUIRES.DLL.
		TCHAR szURL[MAX_PATH] = {0};
		long nWidth = 0;
		long nHeight = 0;
		hrx << GetAlertHTMLInfo(Type, szURL, nWidth, nHeight);

		CComBSTR bstrLink = szURL;
		_variant_t vRetVal;
		_variant_t vInput;

		CComPtr<IccWebWindow> spWebCtrl;
		hrx << spWebCtrl.CoCreateInstance(CLSID_ccWebWindow, NULL, CLSCTX_ALL);
		hrx << spWebCtrl->SetIcon((long) _Module.GetModuleInstance(), IDI_SYM);
		hrx << spWebCtrl->put_FrameWindowStyle(WS_SYSMENU | WS_CAPTION);

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
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}

HRESULT CAlertDlg::LaunchActivationWizard(HWND hWndParent, long nParam)
{
	BOOL bSuccess = FALSE;
	CCLTProductHelper cltHelper;
	HRESULT hr = S_OK;

	hr = cltHelper.Initialize();
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Error while initializing CLT wrapper. HR = 0x%08x"), hr);
		return hr;
	}

	hr = cltHelper.LaunchActivationWizard(nParam);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Error while launching ActWiz. HR = 0x%08x"), hr);
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
	CSubscription subhelper;
	hr = subhelper.LaunchSubscriptionWizard();
	return hr;
}

HRESULT CAlertDlg::LaunchLiveUpdate(BOOL bWait)
{
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		// Create the data provider object
		ISShared_IProvider loader;
		IProviderPtr spDataProvider;
		hrx << (SYM_SUCCEEDED(loader.CreateObject(GETMODULEMGR(), &spDataProvider))? S_OK : E_FAIL);

		// Get LU element object.
		IElementPtr spLUElement;
		hrx << spDataProvider->GetElement(CLSID_NIS_LiveUpdate, spLUElement);

		// Launch LU
		hrx << spLUElement->Configure(GetDesktopWindow(), NULL);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}

HRESULT CAlertDlg::EnableALU()
{
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		// Create the data provider object
		ISShared_IProvider loader;
		IProviderPtr spDataProvider;
		hrx << (SYM_SUCCEEDED(loader.CreateObject(GETMODULEMGR(), &spDataProvider))? S_OK : E_FAIL);

		// Get ALU element object.
		IElementPtr spALUElement;
		hrx << spDataProvider->GetElement(CLSID_NIS_ALU, spALUElement);

		// Get data object
		IDataPtr spData;
		hrx << spALUElement->GetData(spData);

		// QI for ALU state object
		INumberDataQIPtr spALUState(spData);
		LONGLONG lState = 0;

		// Enable ALU
		hrx << spALUState->SetNumber(1);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}

HRESULT CAlertDlg::DoTask(SYMALERT_RESULT Result, HWND hWndParent, BOOL bWait)
{
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << m_CltHelper.Initialize();
		CCTRCTXI0(L"Initialize PEP successfully");

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
					_ErrorMessageBox(NULL, ERR_ACTIVATE_LICENSE, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
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
				size_t stUrlLength = 1024;
				hrx << m_CltHelper.GetLicenseStringProperty(clt::szSCSSReenableURL, 
					szURL, stUrlLength);

				NAVToolbox::CStartBrowser browser;
				hrx << (browser.ShowURL(szURL) ? S_OK : E_FAIL);
			}
			break;

		case SYMALERT_RESULT_LAUNCH_UPSELL_WEBSITE:
			{
				TCHAR szURL[1024] = {0};
				size_t stUrlLength = 1024;

				// CLT7 TODO Replace with Upsell URL
				hrx << m_CltHelper.GetLicenseStringProperty(clt::szSCSSUpsellURL, 
					szURL, stUrlLength);

				NAVToolbox::CStartBrowser browser;
				hrx << (browser.ShowURL(szURL) ? S_OK : E_FAIL);
			}
			break;

		case SYMALERT_RESULT_LAUNCHLU_AND_ENABLE_ALU:
			{
				hrx << LaunchLiveUpdate(bWait);
				hrx << EnableALU();
			}
			break;

		default:
			break;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}