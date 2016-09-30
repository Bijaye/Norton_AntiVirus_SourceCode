#include "stdafx.h"
#include <time.h>
#include "resource.h"
#include "NAVOptions.h"
#include "SnoozeAlert.h"
#include "ccWebWnd.h"
#include "optnames.h"
#include "ccSettingsInterface.h"
#include "ccSettingsManagerHelper.h"
#include "NAVSnoozeSettings.h"
#include "ISWSCHelper_Loader.h"
#include "WSCHelper.h"
#include "NAVHelpLauncher.h"    // NAVToolbox
#include "AVRESBranding.h"
#include "..\navoptionsres\resource.h"

// static const int REMOVE_TURN_OFF_AP = 0x80000000;

BOOL CSnoozeAlert::Snooze(long hParentWnd, SnoozeFeature snoozeFeatures, BOOL bCanTurnOffAP)
{
	TRACEHR(h);

	// Notify tray icon that we are displaying the snooze alert dialog.
	ccLib::CMutex mutex(NULL, FALSE, NAVSnoozeAlertDialogMutex, FALSE);

	BOOL bResult = FALSE;
/*
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(_Module.GetModuleInstance(), szPath, MAX_PATH);

	CString url;
	url.Format("res://%s/SnoozeAlert.htm", szPath);

	CComPtr<IccWebWindow> spWebCtrl;
	h << spWebCtrl.CoCreateInstance(CLSID_ccWebWindow, NULL, CLSCTX_INPROC);

	_variant_t vRetVal;
	_variant_t vInputVal = ((int)snoozeFeatures) | (bCanTurnOffAP? 0 : REMOVE_TURN_OFF_AP);

	h << spWebCtrl->SetIcon((long)_Module.GetModuleInstance(), IDI_NAVOPTIONS);
	h << spWebCtrl->put_FrameWindowStyle(WS_SYSMENU | WS_CAPTION);
	h << spWebCtrl->showModalDialogEx(hParentWnd, url.AllocSysString(), 472, 360, &vInputVal, &vRetVal);

	if (vRetVal.vt != VT_EMPTY)
	{
		bResult = ((int)vRetVal) != 0;
	}
*/

	CSnoozeAlertDlg dlg((HWND)hParentWnd);
	bResult = dlg.DisplayAlert(snoozeFeatures, bCanTurnOffAP);

	/* This does not work, because main UI receives Update Status message before Options are closed.
		if (hParentWnd != 0)
			::ShowWindow((HWND)hParentWnd, SW_HIDE);	// Speed up closing the options dialog.
	*/
	return bResult;
}

void CSnoozeAlert::SetSnoozePeriod(SnoozeFeature snoozeFeatures, long lValue)
{
	TRACEHR(h);

	int features = snoozeFeatures;

	if (lValue > 0)
	{
		lValue += time(0);
	}

	h %= "Connect to ccSettings";

	ccSettings::CSettingsManagerHelper ccSettingsHelper;
	ccSettings::ISettingsManagerPtr pSettingsManager;

	h << ccSettingsHelper.Create(pSettingsManager.m_p);

	ccSettings::ISettingsPtr pSettings;
	h << pSettingsManager->GetSettings(NAVSnoozeSettingsKeyName, &pSettings);

	h %= "Set snooze settings";

	for (;;)
	{
		LPCTSTR name = NULL;

		if ((features & SnoozeAP) != 0)
		{
			features &= ~SnoozeAP;
			name = GetNAVSnoozeSettingsName(SnoozeAP);
		}
		else if ((features & SnoozeIWP) != 0)
		{
			features &= ~SnoozeIWP;
			name = GetNAVSnoozeSettingsName(SnoozeIWP);
		}
		else if ((features & SnoozeALU) != 0)
		{
			features &= ~SnoozeALU;
			name = GetNAVSnoozeSettingsName(SnoozeALU);
		}
		else if ((features & SnoozeEmailIn) != 0)
		{
			features &= ~SnoozeEmailIn;
			name = GetNAVSnoozeSettingsName(SnoozeEmailIn);
		}
		else if ((features & SnoozeEmailOut) != 0)
		{
			features &= ~SnoozeEmailOut;
			name = GetNAVSnoozeSettingsName(SnoozeEmailOut);
		}

		if (name == NULL)
			break;

		CCTRACEI("Snooze %s: %ld", name, lValue);

		h = pSettings->PutDword(name, lValue);
	}

	h = pSettingsManager->PutSettings(pSettings);

	h %= "Fire snooze alert event";

	ccLib::CEvent eventSnooze;
	if (eventSnooze.Open(EVENT_MODIFY_STATE, FALSE, NAVSnoozeSettingsChangedEvent, TRUE))
	{
		h.Verify( eventSnooze.SetEvent() );
	}
	else
	{
		h /= "Failed to open the snooze changed event";
	}
}


long CSnoozeAlert::GetSnoozePeriod(SnoozeFeature snoozeFeature)
{
	TRACEHR(h);

	long lValue = 0;

	LPCTSTR name = GetNAVSnoozeSettingsName(snoozeFeature);
	h.Verify(name != NULL, E_INVALIDARG);

	ccSettings::CSettingsManagerHelper ccSettingsHelper;
	ccSettings::ISettingsManagerPtr pSettingsManager;

	h << ccSettingsHelper.Create(pSettingsManager.m_p);
	
	ccSettings::ISettingsPtr pSettings;
	h << pSettingsManager->GetSettings(NAVSnoozeSettingsKeyName, &pSettings);

	h << pSettings->GetDword(name, (DWORD&)lValue);

	return lValue;
}

BOOL CSnoozeAlertDlg::DisplayAlert(SnoozeFeature snoozeFeatures, BOOL bCanTurnOffAP)
{
	TRACEHR(h);

	CCTRACEI("Snooze requiest: 0x%X", snoozeFeatures);

	const static int choiceCount = 5;
	const static long choiceValues[choiceCount] = { 900, 3600, 18000, -1, 0 };  // 15 min, 1 hour, 5 hours, reboot, permanently

	cc::ccAlert_IAlert2 AlertFactory;
	cc::IAlert2Ptr pAlert;
	if(SYM_FAILED(AlertFactory.CreateObject(&pAlert)))
	{
		return TRUE;	// Falure: let the user to do what they want.
	}
	_Module.SetResourceInstance(g_ResModule);
	// Decription Text
	CString featureList = "\n";
	CString featureName;
	int featureCount = 0;

	if ((snoozeFeatures & SnoozeAP) != 0)
	{
		featureName.LoadString(IDS_AP_FEATURE_NAME);
		featureList += "\n  - " + featureName;
		featureCount++;
	}

	if ((snoozeFeatures & SnoozeIWP) != 0)
	{
		featureName.LoadString(IDS_IWP_FEATURE_NAME);
		featureList += "\n  - " + featureName;
		featureCount++;
	}

	if ((snoozeFeatures & (SnoozeEmailIn | SnoozeEmailOut)) != 0)
	{
		featureName.LoadString(IDS_EM_FEATURE_NAME);
		featureList += "\n  - " + featureName;
		featureCount++;
	}

	if ((snoozeFeatures & SnoozeALU) != 0)
	{
		featureName.LoadString(IDS_ALU_FEATURE_NAME);
		featureList += "\n  - " + featureName;
		featureCount++;
	}

	CString description;
	if (featureCount > 1)
		description.LoadString(IDS_SNOOZE_DESCRIPTION_MANY);
	else
		description.LoadString(IDS_SNOOZE_DESCRIPTION_ONE);

	CBrandingRes BrandRes;

	pAlert->SetWindowTitle(BrandRes.ProductName());
    pAlert->SetTitleBitmap(_Module.GetModuleInstance(), IDB_TOPBAR_BACKGROUND);
	pAlert->SetWindowIcon(_Module.GetModuleInstance(), IDI_NAVOPTIONS);
	pAlert->SetAlertTitle(_Module.m_hInstResource, IDS_SNOOZE_TITLE);
	pAlert->SetThreatLevelText(_Module.m_hInstResource, IDS_SNOOZE_MESSAGE);
	pAlert->SetHelpText(_Module.m_hInstResource, IDS_MORE_INFO);
	pAlert->SetPromptText(_Module.m_hInstResource, IDS_SNOOZE_HOW_LONG);

	pAlert->SetBriefDesc(description + featureList);

	pAlert->SetHelpCallback(this);
	pAlert->SetButtonConfig(cc::IAlert2::BC_OKCANCEL, cc::IAlert2::BC_DEFBUTTONCANCEL);

	// Check if we should show WSC check box
	bool bShowBroadcastCheckbox = false;
    ISShared::WSCHelper_Loader WSCLoader;
	CSymPtr<IWSCHelper> pWSCHelper;
	
	if ((/*bCanTurnOffAP && */((snoozeFeatures & SnoozeAP) != 0)) || ((snoozeFeatures & SnoozeIWP) != 0))
	{
		CCTRACEI("Can show broadcast?");
		if (SYM_SUCCEEDED(WSCLoader.CreateObject(&pWSCHelper)))
		{
			if (pWSCHelper->IsPreXPSP2())
			{
				CCTRACEI("WSC is not found");
			}
			else
			{
				bShowBroadcastCheckbox = true;
				pAlert->SetCheckBoxText(_Module.m_hInstResource, IDS_WSC_DISABLE, true);
			}
		}
		else
		{
			CCTRACEE("Failed to load WSCHelper");
		}
	}

	if (bShowBroadcastCheckbox)
		CCTRACEI("Show Broadcast Checkbox");

	if (bShowBroadcastCheckbox)
	{
		//pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR0, cc::IAlert2::ACTION_CB_DISABLE_CHECK);
		//pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR1, cc::IAlert2::ACTION_CB_DISABLE_CHECK);
		//pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR2, cc::IAlert2::ACTION_CB_DISABLE_CHECK);
		//pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR3, cc::IAlert2::ACTION_CB_DISABLE_CHECK);
		pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR0, cc::IAlert2::ACTION_CB_CHECK);
		pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR1, cc::IAlert2::ACTION_CB_CHECK);
		pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR2, cc::IAlert2::ACTION_CB_CHECK);
		pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR3, cc::IAlert2::ACTION_CB_CHECK);
		if (bCanTurnOffAP)
			pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR4, cc::IAlert2::ACTION_CB_CHECK);
	}
	else
	{
		pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR0);
		pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR1);
		pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR2);
		pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR3);
		if (bCanTurnOffAP)
			pAlert->AddAction(_Module.m_hInstResource, IDS_SNOOZEFOR4);
	}

	_Module.SetResourceInstance(_Module.GetModuleInstance());
	pAlert->SetRecAction(0);

	int choice = pAlert->DisplayAlert(m_hParent, NULL);

	long value = 0;

	if (0 <= choice && choice < choiceCount)
		value = choiceValues[choice];

	if (pAlert->GetButtonPressed() == cc::IAlert2::BC_IDOK)
	{
		// OK

		CCTRACEI("User's choice: %d (%ld)", choice, value);
		try
		{
			bool stateWSC = false;

			if (/* (0 == value) && */bShowBroadcastCheckbox)
			{
				stateWSC = pAlert->GetCheckBoxState();
				SYMRESULT symRes = pWSCHelper->SetDisableBroadcast(stateWSC);
			}

			if ((snoozeFeatures & SnoozeAP) != 0)
			{
				CSnoozeAlert::SetSnoozePeriod(SnoozeAP, value);
				if (bShowBroadcastCheckbox)
					pWSCHelper->SetAVOverride(stateWSC);
			}

			if ((snoozeFeatures & SnoozeIWP) != 0)
			{
				CSnoozeAlert::SetSnoozePeriod(SnoozeIWP, value);
				if (bShowBroadcastCheckbox)
					pWSCHelper->SetFWOverride(stateWSC);
			}

			if ((snoozeFeatures & SnoozeALU) != 0)
			{
				CSnoozeAlert::SetSnoozePeriod(SnoozeALU, value);
			}

			if ((snoozeFeatures & SnoozeEmailIn) != 0)
			{
				CSnoozeAlert::SetSnoozePeriod(SnoozeEmailIn, value);
			}

			if ((snoozeFeatures & SnoozeEmailOut) != 0)
			{
				CSnoozeAlert::SetSnoozePeriod(SnoozeEmailOut, value);
			}
		}
		catch (_com_error& err)
		{
			h = err;
		}

		return TRUE;
	}
	else
	{
		// Cancel
		return FALSE;
	}
}

bool CSnoozeAlertDlg::Run(HWND hWndParent, unsigned long nData, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context)
{
	if(context == cc::IAlertCallback::ALERTCALLBACK_HELP)
	{
		static const DWORD dwSnoozeHelpId = 5181;

		NAVToolbox::CNAVHelpLauncher Help;
		return Help.LaunchHelp(dwSnoozeHelpId, hWndParent) != NULL;
	}
	return true;
}
