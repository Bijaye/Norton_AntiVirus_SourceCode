// NAVOptionRefresh.cpp: implementation of the CNAVOptionRefresh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "simon.h"

#include "resource.h"
#include "cltLicenseConstants.h"
#include "cltSecurityValidation.h"
#include "NAVLicenseNames.h"
#include "OptNames.h"                           // ALERT_Defs
#include "APOptNames.h"		
#include "NAVSettingsCache.h"
#include "NAVInfo.h"
#include "ccModuleId.h"
#include "AVccModuleId.h"
#include "NAVError.h"
#include "GlobalEvents.h"	// Option change events
#include "GlobalEvent.h"    
#include "NAVAPSCR_i.c"		// Disable/Enable AP
#include "NAVAPSCR.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NAVtrust.h"
#include "NAVOptions_i.c"	// to read AP options in SARV.dat
#include "NAVOptions.h"
#include "NAVSnoozeSettings.h"
#include "NAVLicenseNames.h"
#include "BusinessRules.h"
#include "NAVErrorResource.h"

// PEP Headers
#include "pepClientBase.h"

#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"

#include "NAVSecurity.h"

#define _NAVOPTREFRESH_CONSTANTS
#include "NavOptRefresh.h"
#include "NAVOptionRefresh.h"

#include "IWPPrivateSettingsInterface.h"    // For IWP
#include "IWPPrivateSettingsLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVOptionRefresh::CNAVOptionRefresh() :
m_cachedLicenseState(DJSMAR00_LicenseState_PURCHASED),
m_cachedLicenseZone(DJSMAR_LicenseZone_PostActivation | DJSMAR_LicenseZone_Purchased | DJSMAR_LicenseZone_NotStarted),
m_cachedSubState(0),
m_cachedSubDays(0)
{}

CNAVOptionRefresh::~CNAVOptionRefresh()
{

}
SIMON_STDMETHODIMP CNAVOptionRefresh::Refresh(DWORD flag)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	try
	{
		_IT_DoRefresh(flag);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVOptionRefresh::Refresh - Exception while refresh %08X"), hr);
	}
	return hr;
}

HRESULT CNAVOptionRefresh::_IT_DoRefresh(DWORD flag)
{	
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	UINT uErrorID = 0;

	CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh - Begin"));

	try
	{
		// When the system started in the safe mode, symlcsvc.exe service 
		//  is not started, which results in all licensing calls failed.
		//  Suite child should not commit data to the cache, simply exit. 
		//  Clients should use whatever license status available in the cache.
		int nRet = GetSystemMetrics(SM_CLEANBOOT);
		bool bSafeMode = (nRet != 0);
		if(bSafeMode)
		{
			CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh: System is in safe mode, exit"));
			return S_OK;
		}

		DWORD dwSubDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;
		DJSMAR00_LicenseState state = DJSMAR00_LicenseState_Violated;
		DJSMAR_LicenseType type = DJSMAR_LicenseType_Violated;
		LONG zone = DJSMAR_LicenseZone_Violated;

		LONG  lSubDaysRemaining = 0;

		try
		{
			// Getting licensing and subscription properties needed to enable product features
			CPEPClientBase pepBase;
			hrx << pepBase.Init();
			pepBase.SetContextGuid(clt::pep::CLTGUID);
			clt::lt_helper::tstring sProductGuid = V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;
			pepBase.SetComponentGuid(sProductGuid);
			pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
			pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION, clt::pep::DISPOSITION_GET_SUBSCRIPTION_INFO);

			hrx << pepBase.QueryPolicy();

			// Getting licensing properties
			pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD &)state, DJSMAR00_LicenseState_Violated);
			pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)type, DJSMAR_LicenseType_Violated);
			pepBase.GetValueFromCollection(DRM::szDRMLicenseZone, (DWORD &)zone, DJSMAR_LicenseZone_Violated);

			// Getting Subscription properties (optional)
			pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION, (DWORD &)dwSubDisposition, SUBSCRIPTION_STATIC::DISPOSITION_NONE);
			pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING, (DWORD &)lSubDaysRemaining, 0);
		}
		catch(_com_error& e)
		{
			hr = e.Error();
			CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh: Exception Querying Licensing and Subscription %08X"), hr);
			uErrorID = ERR_QUERY_LICENSE;
		}

		if((state == DJSMAR00_LicenseState_Violated) && (uErrorID == 0))
		{
			// The licensing library is valid, but returns violated value.
			//  Must alert user after refresh.
			CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh: License returned violated."));
			uErrorID = ERR_LICENSE_RETURNED_VIOLATED;
		}

		CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh: Type=%08x, State=%08x, Zone=%08x"), type, state, zone);

		// Refresh product only if we are told to do so or license status has been change.
		bool bRefreshProduct = false;
		if((flag & REFRESH_FORCE_COMMIT) == REFRESH_FORCE_COMMIT)
			bRefreshProduct = true;

		bool bLicenseChanged = false;
		if((state != m_cachedLicenseState) 
			|| (zone != m_cachedLicenseZone))
		{
			bRefreshProduct = true;
			bLicenseChanged = true;
			m_cachedLicenseState = state;
			m_cachedLicenseZone = zone;
		}

		bool bSubscriptionChanged   = false;
		DWORD dwSubState            = dwSubDisposition & SUBSCRIPTION_STATIC::SUBSCRIPTION_STATE_BITS;

        CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh: cached SubState=%d, new SubState=%d, cached days=%d, new days=%d"),m_cachedSubState, dwSubState, m_cachedSubDays, lSubDaysRemaining);

		if((dwSubState != m_cachedSubState) || (m_cachedSubDays != lSubDaysRemaining))
		{
			bRefreshProduct = true;
			bSubscriptionChanged = true;
			m_cachedSubState = dwSubState;
			m_cachedSubDays = lSubDaysRemaining;
		}

		BOOL bFeatureEnabled = FALSE;
		DWORD dwFeatureEnabledBefore = 0;
		DWORD dwFeatureEnabledAfter = 0;
		BOOL bEnableALUProduct = FALSE;
		BOOL bEnableALUVirusDefs = FALSE;
		DWORD dwSubActivated = 0;
		DWORD dwVal = 0;

		{/* SCOPE */

			CNAVOptSettingsCache NAVFeature(m_ccSettings);
			if(NAVFeature.Init() == FALSE)
			{
				CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh: Open nav settings failed."));
				uErrorID = ERR_NAVSETTINGS;
				hrx << E_FAIL;
			}

			// Check to see if licensing files related are mismatched.
			//  If yes, assume tampered, disable product, and show error dialog.
			bool bMissmatch = CheckMismatch(&NAVFeature);
			if(bMissmatch)
			{
				CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh: License mismatched!!! Set product tampered."));
				bRefreshProduct = true;
				state = DJSMAR00_LicenseState_Violated;
				zone = DJSMAR_LicenseZone_Violated;
				uErrorID = ERR_LICENSE_MISMATCH;
			}

			NAVFeature.GetValue(LICENSE_FeatureEnabled, dwFeatureEnabledBefore, 0);
			NAVFeature.Save();   // LICENSE_FeatureEnabled is evaluated and saved in this step.
			NAVFeature.GetValue(LICENSE_FeatureEnabled, dwFeatureEnabledAfter, 0);		

			// Refresh product if LICENSE_FeatureEnabled has been changed.
			if(dwFeatureEnabledAfter != dwFeatureEnabledBefore)
			{
				bRefreshProduct = true;
			}

			// Get ALU settings if we need to refresh later.
			if(bRefreshProduct)
			{
				bFeatureEnabled = ((dwFeatureEnabledAfter == 0) ? FALSE : TRUE);

				NAVFeature.GetValue(ALU_FeatureEnabledProduct, dwVal, 0);
				bEnableALUProduct = ((dwVal == 0) ? FALSE : TRUE);

				NAVFeature.GetValue(ALU_FeatureEnabledVirusdefs, dwVal, 0);
				bEnableALUVirusDefs = ((dwVal == 0) ? FALSE : TRUE);
			}

		} /* END OF SCOPE */

		// Do refresh 
		if(bRefreshProduct)
		{
			// Pulse the Options changed event so clients
			// know that things have changed.
			CGlobalEvent OptionsEvent;
#ifdef _UNICODE
			if (OptionsEvent.CreateW(SYM_OPTIONS_CHANGED_EVENT))
#else  _UNICODE
			if (OptionsEvent.Create(SYM_OPTIONS_CHANGED_EVENT))
#endif  _UNICODE
			{
				CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh - Fire Options changed event."));
				PulseEvent(OptionsEvent);
			}
			else
			{
				CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - Unable to create option changed event."));
			}

			if((flag & REFRESH_SILENT) != REFRESH_SILENT)
			{
				StahlSoft::CSmartHandle shDefAlert;
				shDefAlert = ::CreateEvent(NULL, TRUE, FALSE, szNAVOptionRefreshEngine_DefAlertLicenseChange);
				if(shDefAlert != NULL)
				{
					CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh - Fire event to DefAlert to display license alert."));
					::PulseEvent(shDefAlert);
				}
				else
				{
					CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - Unable to create DefAlertLicenseChange."));
				}
			}

			if(bLicenseChanged)
			{
				// Notify Main UI and DefAlert that licensing has been refreshed.			
				StahlSoft::CSmartHandle shLicenseRefreshed;
				shLicenseRefreshed = ::CreateEvent(NULL, TRUE, FALSE, SYM_REFRESH_NAV_LICENSE_STATUS_EVENT);
				if(shLicenseRefreshed != NULL)
				{
					CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh - Fire NAV license change event."));
					::PulseEvent(shLicenseRefreshed);
				}
				else
				{
					CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - Unable to create licensing refreshed event."));
				}
			}

			if(bSubscriptionChanged)
			{
				// Notify Main UI and DefAlert that licensing has been refreshed.			
				StahlSoft::CSmartHandle shSubscriptionRefreshed;
				shSubscriptionRefreshed = ::CreateEvent(NULL, TRUE, FALSE, SYM_REFRESH_SUBSCRIPTION_STATUS_EVENT);
				if(shSubscriptionRefreshed != NULL)
				{
					CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh - Fire subscription change event."));
					::PulseEvent(shSubscriptionRefreshed);
				}
				else
				{
					CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - Unable to create subscription refreshed event."));
				}
			}

			CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - Get Snooze settings"));
			LONG lSnoozeAP = 0, lSnoozeALU = 0, lSnoozeIWP = 0;

			try
			{
				CComPtr<ISnoozeAlert> spSnooze;
				hrx << spSnooze.CoCreateInstance(CLSID_SnoozeAlertObj);
				hrx << spSnooze->GetSnoozePeriod(SnoozeAP, &lSnoozeAP);
				hrx << spSnooze->GetSnoozePeriod(SnoozeALU, &lSnoozeALU);
				hrx << spSnooze->GetSnoozePeriod(SnoozeIWP, &lSnoozeIWP);
				CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - lSnoozeAP=%d, lSnoozeALU=%d, lSnoozeIWP=%d"), lSnoozeAP, lSnoozeALU, lSnoozeIWP);

				// Note: SnoozeValue=0 --> No snooze.
			}
			catch(_com_error& e)
			{
				CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - Error while accessing snooze object 0x%08X"), e.Error());
			}

			// Enabled/Disable Auto-Protect
			if(lSnoozeAP && bFeatureEnabled)
			{
				CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - SnoozeAP is on. Skip enabling AP"));
			}
			else
			{
				if(FAILED(_EnableAutoProtect(bFeatureEnabled)))
				{
					CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - EnableAutoProtect failed"));
				}
			}

			// Enable/Disalbe ALU
			if(lSnoozeALU && (bEnableALUProduct || bEnableALUVirusDefs))
			{
				CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh - Snooze for ALU is on. Skip enabling ALU"));
			}
			else
			{
				if(FAILED(_EnableAutoLiveUpdate(bEnableALUProduct, bEnableALUVirusDefs)))
				{
					CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - _EnableAutoLiveUpdate failed %08x"), hr);
				}
			}

			// Enable/Disalbe IWP
			if(lSnoozeIWP && bFeatureEnabled)
			{
				CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh - Snooze for IWP is on. Skip enabling IWP"));
			}
			else
			{
				if(FAILED(_EnableIWP(bFeatureEnabled))) // eat this failure if it occurs
				{
					CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - _EnableIWP failed %08x"), hr);
				}
			}
		}
	}
	catch(_com_error& e)
	{
		if(uErrorID == 0)
			uErrorID = ERR_REFRESH_LICENSE;

		hr = e.Error();
		CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - Exception while refresh license %08X"), hr);
	}

	if(FAILED(hr) || (uErrorID != 0))
	{
		CCTRACEE(_T("CNAVOptionRefresh::_IT_DoRefresh - show error dialog %08X"), hr);
		_IT_FireTamperEvent(uErrorID);
	}

	CCTRACEI(_T("CNAVOptionRefresh::_IT_DoRefresh Exitting: %08X"), hr);
	return hr;
}

HRESULT CNAVOptionRefresh::_IT_FireTamperEvent(UINT uErrorID)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	try
	{
		StahlSoft::CSmartHandle smEvent(::CreateEvent(NULL,FALSE,FALSE,ALERT_EVENT_ProductCorrupt));
		::SetEvent(smEvent);

		// Display error
		// Get a NAVError object
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr <INAVCOMError> spNavError;

		// Create, Populate, Log, and Display the error
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_NAVOPTREFRESH);		// Module ID of AV_MODULE_ID_NAVOPTREFRESH
		hrx << spNavError->put_ErrorID(uErrorID); // Error ID is the resource ID for the error message
		hrx << spNavError->put_ErrorResourceID(IDS_NAVERROR_INTERNAL_REINSTALL);
		hrx << spNavError->LogAndDisplay(0);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVOptionRefresh::_IT_FireTamperEvent - Exception while display ccError dialog %08X"), hr);
	}
	return hr;
}

HRESULT CNAVOptionRefresh::_EnableAutoProtect(BOOL bFeatureEnabled)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		// Verify Options scriptable object before using it
		hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVOptions))? E_FAIL : S_OK);

		BOOL bEnableAP = FALSE;
		BOOL bUpdateAPStatus = TRUE;
		CComPtr<INAVOptions> spNavOptions;
		CComPtr<ISymScriptSafe> spScriptSafeOptions;       
		CComBSTR bstrLoadVxD(STARTUP_LoadVxD);
		CComVariant varApOptEnabled;

		CCTRACEI(_T("CNAVOptionRefresh::_EnableAutoProtect - Get STARTUP_LoadVxD from SAVR.DAT."));

		hrx << spNavOptions.CoCreateInstance(CLSID_NAVOptions, NULL, CLSCTX_ALL);        
		hrx << spNavOptions.QueryInterface(&spScriptSafeOptions);
		hrx << spScriptSafeOptions->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
		hrx << spNavOptions->Get(bstrLoadVxD, (CComVariant) 1, &varApOptEnabled);

		bEnableAP = bFeatureEnabled && varApOptEnabled.lVal;
		CCTRACEI(_T("CNAVOptionRefresh::_EnableAutoProtect - Now enabling AP=%d"), bEnableAP);

		// Verify AP scriptable object before using it
		hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_ScriptableAutoProtect))? E_FAIL : S_OK);

		CComPtr<IScriptableAutoProtect> spAP;
		CComPtr<ISymScriptSafe> spScriptSafeAP;

		hrx << spAP.CoCreateInstance(CLSID_ScriptableAutoProtect, NULL, CLSCTX_ALL);              
		hrx << spAP.QueryInterface(&spScriptSafeAP);
		hrx << spScriptSafeAP->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
		hrx << spAP->put_Enabled(bEnableAP);        

		CCTRACEI(_T("CNAVOptionRefresh::_EnableAutoProtect - successful, Enabled=%d"), bEnableAP);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVOptionRefresh::_EnableAutoProtect - Exception while enable/disable AP %08X"), hr);
	}
	return hr;
}

HRESULT CNAVOptionRefresh::_EnableAutoLiveUpdate(BOOL bEnableALUProduct, BOOL bEnableALUVirusDefs)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	try
	{
		hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVOptions))? E_FAIL : S_OK);

		CComPtr <INAVOptions> spOptions;
		CComPtr <ISymScriptSafe> spSymScriptSafe;
		long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
		long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;

		hrx << spOptions.CoCreateInstance(CLSID_NAVOptions);
		hrx << spOptions.QueryInterface(&spSymScriptSafe);
		hrx << spSymScriptSafe->SetAccess(dwAccess, dwKey);

		// Enable/Disable ALU for product patch.
		CCTRACEI(_T("CNAVOptionRefresh::_EnableAutoLiveUpdate - EnableALUProduct=%d"), bEnableALUProduct);
		if(FAILED(spOptions->EnableALU(bEnableALUProduct, TRUE)))
		{
			CCTRACEE(_T("CNAVOptionRefresh::_EnableAutoLiveUpdate - EnableALUProduct failed"));
		}

		// Enable/Disable ALU for virus defs.
		CCTRACEI(_T("CNAVOptionRefresh::_EnableAutoLiveUpdate - EnableALUVirusDefs=%d"), bEnableALUVirusDefs);
		if(FAILED(spOptions->EnableALU(bEnableALUVirusDefs, FALSE)))		
		{
			CCTRACEE(_T("CNAVOptionRefresh::_EnableAutoLiveUpdate - EnableALUVirusDefs failed"));
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVOptionRefresh::_EnableAutoLiveUpdate - Exception while accessing Options object %08X"), hr);
	}
	return hr;
}


HRESULT CNAVOptionRefresh::_EnableIWP(BOOL bFeatureEnabled)
{
	IWP::IIWPPrivateSettingsPtr pIWPSettings;
    if(SYM_FAILED(IWP_IWPPrivateSettings::CreateObject(GETMODULEMGR(), &pIWPSettings)))
	{
		// No IWP installed
		//
		CCTRACEI ( "CNAVOptionRefresh::_EnableIWP - No IWP installed" );
	}
	else
	{
		if ( SYM_FAILED (pIWPSettings->SetValue (IWP::IIWPPrivateSettings::IWPFeatureAvailable, bFeatureEnabled)))
			CCTRACEE ("CNAVOptionRefresh::_EnableIWP - IWP->SetValue failed");
		else
			if ( SYM_FAILED (pIWPSettings->Save()))
				CCTRACEE ("CNAVOptionRefresh::_EnableIWP - IWP->Save failed");
	}

	return S_OK;
}
