// HMIProductReg.cpp: implementation of the CHMIProductReg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMIProductReg.h"

#include "Resource.h"

//#include "simon.h"
//#include "NAVBusinessRules.h"
#include "StahlSoft.h"
//#include "SyncHelper.h"
//#include "StahlSoft_EventHelper.h"
//#include "NAVInfo.h"
#include "ISymcobj.h"
#include "Symcguid.h"

#include "StartBrowser.h"

// PEP Headers

#include "pepClientBase.h" 
#include "cltPepConstants.h"
#include "DRMNamedProperties.h"
#include "SimonCollectionImpl.h"
#include "prodinfopephelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMIProductReg::CHMIProductReg()
{
	SetTextID ( IDS_NAV_PRODUCT_REG );
}

CHMIProductReg::~CHMIProductReg()
{
}


HRESULT CHMIProductReg::DoWork (HWND hMainWnd)
{
	// Check to see if product has been activated.
	// (Need to do real time checking for license zone 
	//  since it's possible license is violated when
	//  users click this item.)
	//DWORD dwZone = DJSMAR_LicenseZone_Violated;

	//CPEPClientBase pepBase;
	//STAHLSOFT_HRX_TRY(hr)
	//{
	//	hrx << pepBase.Init();
	//	pepBase.SetContextGuid(clt::pep::CLTGUID);
	//	pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
	//	pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
	//	pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
	//	pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

	//	hrx << pepBase.QueryPolicy();

	//	// Getting licensing zone
	//	hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseZone, (DWORD &)dwZone, DJSMAR_LicenseZone_Violated);
	//}
	//STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	//bool bPostActivation = ((dwZone & DJSMAR_LicenseZone_PostActivation) == DJSMAR_LicenseZone_PostActivation);
	//bool bPurchased = ((dwZone & DJSMAR_LicenseZone_Purchased) == DJSMAR_LicenseZone_Purchased);
	//g_bProductActivated = (bPostActivation && bPurchased);

	//if(g_bProductActivated)
	//{
	// If product registration link exists in AVRES, launch it.
	TCHAR szURL[MAX_PATH] = {0};
	if(FetchURL(IDS_PRODUCT_REG_URL, szURL))
	{
		NAVToolbox::CStartBrowser browser;
		browser.ShowURL(szURL);
	}
	else
	{
		CComPtr<ISymCDialog> spProfile;
		if(SUCCEEDED(CoCreateInstance(CLSID_SymCUserProfile, NULL, CLSCTX_INPROC_SERVER , IID_ISymCDialog, (LPVOID*)&spProfile)))
		{
			spProfile->Invoke(hMainWnd, 0L);
		}
	}
	//}

	return S_OK;
}


bool CHMIProductRegFactory::ShouldShow()
{
	bool bShow = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		switch(g_LicenseType)
		{
		case DJSMAR_LicenseType_Beta:
		case DJSMAR_LicenseType_Violated:
		case DJSMAR_LicenseType_Rental:
		case DJSMAR_LicenseType_TryDie:
			// Don't show menu item
			break;

		case DJSMAR_LicenseType_Retail:
		case DJSMAR_LicenseType_Trial:
		case DJSMAR_LicenseType_ESD:
		case DJSMAR_LicenseType_Unlicensed:
		case DJSMAR_LicenseType_OEM:
		default:
			bShow = ShouldShowHelpMenuOption(HELPMENU_ShowProductReg);
			break;
		}

        // if bShow is true verify that its not being overriden by PRODINFO:SupportsRegistration
        if(bShow)
        {
            DWORD dwCanRegister = 1;
            CPEPClientBase pepBase;
            StahlSoft::CSmartDataPtr<BYTE> spData;
            pepBase.Init();
            pepBase.SetContextGuid(PRODINFO::PRODINFOGUID);
		    pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
            pepBase.SetPolicyID(PRODINFO::POLICY_ID_QUERY_PRODUCT_INFORMATION);
            pepBase.QueryPolicy();
            pepBase.GetValueFromCollection(PRODINFO::szPISupportsRegistration, dwCanRegister, 1);
                    
            if(dwCanRegister == 0)
                bShow = false;
        }
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

	return bShow;
}