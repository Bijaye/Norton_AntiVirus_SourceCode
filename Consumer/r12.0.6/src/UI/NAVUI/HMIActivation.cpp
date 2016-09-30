// HMIProducts.cpp: implementation of the CHMIActivation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "HMIActivation.h"
#include "AVRES.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NavLicense_i.c"
#include "NavLicense_h.h"
#include "SSOsinfo.h"
// #include "NAVSettingsHelperEx.h"
#include "simon.h"
#include "NAVBusinessRules.h"
#include "StahlSoft.h"
#include "SyncHelper.h"
#include "StahlSoft_EventHelper.h"
#include "NAVInfo.h"

#include "NAVLicenseNames.h"
#include "SymAlertStatic.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#include "V2Authorization.h"
#define _SIMON_PEP_
#include "pepClientBase.h" 
#include "cltPepConstants.h"
#include "DRMNamedProperties.h"
#include "SimonCollectionImpl.h"
#include "Subscription_Static.h"
#include "ccResourceLoader.h"
#include "..\navuires\resource.h"
extern cc::CResourceLoader g_ResLoader;

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMIActivation::CHMIActivation()
{
	SetTextID ( IDS_NAV_ACTIVATION_MENU_ITEM );
}

CHMIActivation::~CHMIActivation()
{

}

HRESULT CHMIActivation::DoWork (HWND hMainWnd)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// Rules:
		// For Rental, launch Actwiz only if product has not been activated. 
		// For non rental, always launch Actwiz. If product has already been activated,
		//  Actwiz will display product key.
		if(g_LicenseType == DJSMAR_LicenseType_Rental)
		{
			// Check to see if product has been activated.
			// (Need to do real time checking for license zone since
			//  it's possible that users already activated the product
			//  through nag or through activation button on hint panel.)
			CPEPClientBase pepBase;
			hrx << pepBase.Init();
			pepBase.SetContextGuid(clt::pep::CLTGUID);
			pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
			pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
			pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
			pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

			hrx << pepBase.QueryPolicy();

			// Getting licensing zone
			hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD &)g_LicenseState, DJSMAR00_LicenseState_Violated);
			if(g_LicenseState == DJSMAR00_LicenseState_PURCHASED) 
			{
				// Pop up message box to let users know that product has already been activated.
				CString csFormat;
				CString csMessage;

				g_ResLoader.LoadString(IDS_PRODUCT_ALREADY_ACTIVATED, csFormat);
				csMessage.Format(csFormat, g_csProductName);

				::MessageBox(hMainWnd, csMessage, g_csProductName, MB_OK);
				return S_OK;
			}
		}

		// Launch Actwiz.
		CComPtr<INAVLicenseInfo> spNavLicense;
		CComPtr <ISymScriptSafe> spSymScriptSafe;
		long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
		long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;

		hrx << spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL);
		hrx << spNavLicense.QueryInterface(&spSymScriptSafe);
		hrx << spSymScriptSafe->SetAccess(dwAccess, dwKey);	

		long nParam = 0;
		if(g_LicenseType == DJSMAR_LicenseType_Rental)
		{
			nParam = SYMALERT_RESULT_ACTIVATE_SERVICE;
		}
		
		hrx << spNavLicense->LaunchActivationWizardEx((long) hMainWnd, nParam, HWNDPARENT_PASSED);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

bool CHMIActivationFactory::ShouldShow()
{
	bool bShow = false;

	if(g_LicenseState == DJSMAR00_LicenseState_Violated) 
	{
		CCTRACEI(_T("Suppress Activation menu because license has been tamppered"));
		return bShow;
	}

	// Suppress item for TryDie, TryBuy, and Unlicensed
	// For others, always show item because:
	//  1. If product has not been activated, the menu will launch ActWiz with activation panel. 
	//  2. If product has been activated, the menu will launch ActWiz, which displays Activation Key.
	//     This allows user to look up activation key any time.
	switch(g_LicenseType)
	{
	case DJSMAR_LicenseType_Trial:
	case DJSMAR_LicenseType_TryDie:
	case DJSMAR_LicenseType_Unlicensed:
		{
			CCTRACEI(_T("Suppress Activation menu because license type=%d."), g_LicenseType);
			return bShow;
		}
		break;

	case DJSMAR_LicenseType_Rental:
		if((g_LicenseZone & DJSMAR_LicenseZone_PostActivation) == DJSMAR_LicenseZone_PostActivation)
		{
			CCTRACEI(_T("Suppress Activation menu because SOS license has been activated at least once."));
			return bShow;
		}
		break;

	default:
		break;
	}

	// Check on/off settings flag. 
	bShow = ShouldShowHelpMenuOption(HELPMENU_ShowActivation);

	return bShow;    
}