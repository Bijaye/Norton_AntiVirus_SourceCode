#include "stdafx.h"
#include <simon.h>
#include "cltLicenseConstants.h"
#include "cltSecurityValidation.h"
#include "cltLicenseHelper.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "pepClientBase.h"

#include "NAVLicenseNames.h"
#include "NAVSettingsCache.h"
#include "businessrules.h"
#include "OptNames.h"       
#include "NAVOPTHelperEx.H"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "InstOptsNames.h"


bool g_bMismatchChecked = false;
bool g_bIsMismatch		= false;

typedef struct NAV_FEATUREMAP
{
	LPCTSTR lpszFeatureEnabled;
	LPCTSTR lpszFeatureSetting;
} NAV_FEATUREMAP;

NAV_FEATUREMAP _g_aFeatureMap[] =
{
	 NAVSHEXT_FeatureEnabled				,_T("\0")
	,OFFICEAV_FeatureEnabled				,_T("\0")
	,NAVOPTION_FeatureEnabled				,_T("\0")
	,IMSCANSTUB_FeatureEnabled				,_T("\0")
	,IMSCAN_FeatureEnabledScanAIM			,IMSCAN_ScanAIM
	,IMSCAN_FeatureEnabledScanMIM			,IMSCAN_ScanMIM
	,IMSCAN_FeatureEnabledScanYIM			,IMSCAN_ScanYIM
	,IMSCAN_FeatureEnabledScanTOL			,IMSCAN_ScanTOL
	,NAVEMAIL_FeatureEnabledScanIn			,NAVEMAIL_ScanIncoming
	,NAVEMAIL_FeatureEnabledScanOut			,NAVEMAIL_ScanOutgoing
	,NAVEMAIL_FeatureEnabledOEH				,NAVEMAIL_OEH
	,ALU_FeatureEnabledProduct				,ALU_Product
	,ALU_FeatureEnabledVirusdefs			,ALU_Virusdefs
	,NULL									,NULL
};

bool CheckMismatch(CNAVOptSettingsCache* pOptFile)
{
	if(g_bMismatchChecked)
	{
		return g_bIsMismatch;
	}
	g_bMismatchChecked	= true;
	g_bIsMismatch		= true;

	STAHLSOFT_HRX_TRY(hr)
	{
		DWORD dwLicensed = 1;
		DWORD dwType = (DWORD) DJSMAR_LicenseType_Violated;

		pOptFile->GetValue(LICENSE_Licensed, dwLicensed, 1);

		if(dwLicensed != 1)
		{
			g_bIsMismatch = false;
			return g_bIsMismatch;
		}

		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(clt::pep::CLTGUID);
		clt::lt_helper::tstring sProductGuid = V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;
		pepBase.SetComponentGuid(sProductGuid);
		pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
		hrx << pepBase.QueryPolicy();

        // Getting licensing properties
        pepBase.GetValueFromCollection(DRM::szDRMLicenseType, dwType, DJSMAR_LicenseType_Violated);

        // If the license type isn't unlicensed, then don't worry about a license mismatch
		if( dwType != static_cast<DWORD>(DJSMAR_LicenseType_Unlicensed ))
		{
			g_bIsMismatch = false;
			return g_bIsMismatch;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return g_bIsMismatch;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusinessRules::CBusinessRules(void)
{
}

CBusinessRules::~CBusinessRules(void)
{
}

HRESULT CBusinessRules::_InitializeVariables()
{
	return S_OK;
}
HRESULT CBusinessRules::FinalConstruct()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	try
	{
		hrx << _InitializeVariables();
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}
	return hr;
}

SIMON_STDMETHODIMP CBusinessRules::Calc(CNAVOptSettingsCache* pOptFile)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	try
	{
		hrx << _InitializeVariables();

		// Check licensing status
		BOOL bEnable = FALSE;
		BOOL bEnableAP = FALSE;

		STAHLSOFT_HRX_TRY(hr)
		{
			CPEPClientBase pepBase;
			hrx << pepBase.Init();
			pepBase.SetContextGuid(clt::pep::CLTGUID);
			clt::lt_helper::tstring sProductGuid = V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;
			pepBase.SetComponentGuid(sProductGuid);
			pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
			hrx << pepBase.QueryPolicy();

			DJSMAR00_LicenseState state = DJSMAR00_LicenseState_Violated;
			long zone = DJSMAR_LicenseZone_Violated;

            // Getting licensing properties
            pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD &)state, DJSMAR00_LicenseState_Violated);
            pepBase.GetValueFromCollection(DRM::szDRMLicenseZone, (DWORD &)zone, DJSMAR_LicenseZone_Violated);

			if(state == DJSMAR00_LicenseState_PURCHASED 
				|| state == DJSMAR00_LicenseState_TRIAL)
			{
				bEnable = TRUE;
				bEnableAP = TRUE;
			}
			else if((zone & DJSMAR_LicenseZone_NotStarted) == DJSMAR_LicenseZone_NotStarted)
			{
				// If CTO and trial period has not been started,
				//  allow AP to run.
				if(IsCTO())
				{
					bEnableAP = TRUE;
				}
			}
		}
		STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

		DWORD dwSetting = 1;
		DWORD dwFeatureEnabled = 1;

		for(int i=0; _g_aFeatureMap[i].lpszFeatureEnabled != NULL; ++i)
		{
			// Assume users enable this feature.
			dwSetting = 1;

			// Get users' setting for this feature if there is one.
			if(*_g_aFeatureMap[i].lpszFeatureSetting)
			{
				pOptFile->GetValue(_g_aFeatureMap[i].lpszFeatureSetting, dwSetting, 1);
			}

			// Enable/disable product features based on 
			//  licensing state and users' setting.
			dwFeatureEnabled = (dwSetting && bEnable);
			pOptFile->SetValue(_g_aFeatureMap[i].lpszFeatureEnabled, dwFeatureEnabled);
		}          

		pOptFile->SetValue(LICENSE_FeatureEnabled, bEnableAP ? 1 : 0);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}
	return hr;
}

bool CBusinessRules::IsCTO()
{
	bool bCTO = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		CNAVInfo NAVInfo;
		TCHAR szCfgWizDat[MAX_PATH] = {0};
		CNAVOptFileEx CfgWizDatFile;
		DWORD dwVal = 0;
		
		wsprintf(szCfgWizDat, _T("%s\\Cfgwiz.dat"), NAVInfo.GetNAVDir());

		hrx << (CfgWizDatFile.Init(szCfgWizDat, FALSE) ? S_OK : E_FAIL);
		hrx << CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_OEM, dwVal, 0);
		bCTO = (dwVal == 2) ? true : false;
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

	return bCTO;
}