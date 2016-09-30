// NAVUIHelp.h

#pragma once

#include <atlbase.h>
#include "Simon.h"
#include "NavSettingsHelperEx.h"
#include "AVRES.h"
#include "OptNames.h"
#include "cltLicenseConstants.h"
#include "cltPEPConstants.h"
#include "PepClientBase.h"

// PEP Headers
#ifndef PRODUCTPEPHELPER_H
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#endif

namespace NAVUIHelp
{
inline bool IsNavStandAlone()
{
	DWORD dwAggregated = 0;
	READ_SETTING_START()
		READ_SETTING_DWORD(LICENSE_Aggregated, dwAggregated, 0)
	READ_SETTING_END

	if(dwAggregated == 0)
		return true;
	else
		return false;
}

inline HRESULT GetPathToNisPlugin(LPTSTR lpszFile, DWORD dwSize, bool* pbAggregated = NULL)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((lpszFile == NULL) ? E_INVALIDARG: S_OK);

		// If NAV is standalone, 
		//  NAV UI should be launch as standalone product.
		//  Exit.

        // If the client did not pass in whether we are aggregated or not
        // we need to make the check ourselves
        if( pbAggregated == NULL )
        {
		    if(IsNavStandAlone())
			    return E_FAIL;
        }
        // Otherwize use the value passed in
        else if( *pbAggregated == FALSE )
        {
            CCTRACEI(_T("NAVUIHelp::GetPathToNisPlugin() - Client passed in a not aggregated flag so not getting NIS plug-in parameters"));
            return E_FAIL;
        }
        else
            CCTRACEI(_T("NAVUIHelp::GetPathToNisPlugin() - Client passed in a the aggregated flag so will attempt to get NIS plug-in parameters"));

		// Find path to the nisplug.nsi file from the registry
		CRegKey key;
		CString csNisPlug;
		CString csKey;
		DWORD dwSize = MAX_PATH;

		csKey.Format(_T("SOFTWARE\\Symantec\\CCPD\\SuiteOwners\\%s"), V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		hrx << HRESULT_FROM_WIN32(key.Open(HKEY_LOCAL_MACHINE, csKey, KEY_READ));
		hrx << HRESULT_FROM_WIN32(key.QueryStringValue(_T("ParentPlugin"), csNisPlug.GetBufferSetLength(dwSize), &dwSize));		
		csNisPlug.ReleaseBuffer();

		if(csNisPlug.GetLength() > dwSize)
			return E_FAIL;

		_tcscpy(lpszFile, csNisPlug);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}


inline HRESULT BuildNisPluginParams(LPTSTR lpszParam, DWORD dwSize, bool* pbAggregated = NULL)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((lpszParam == NULL) ? E_INVALIDARG: S_OK);

		// Find path to the NISPlug.nsi file.
		CString csNisPlug;
		DWORD dwLength = MAX_PATH;
		hrx << GetPathToNisPlugin(csNisPlug.GetBufferSetLength(dwLength), dwLength, pbAggregated);
		csNisPlug.ReleaseBuffer();

		// Get NAV product and status panel name so that we
		//  set up command line to launch Main UI with 
		//  NAV Status page active.
		CNAVInfo NAVInfo;
		CString csAVRES;
		CString csParam;
		CString csTitle;
		CString csStatusPanel;

		// Setup path to AVRES.DLL
		csAVRES.Format(_T("%s\\AVRES.DLL"), NAVInfo.GetNAVDir());

		StahlSoft::CSmartModuleHandle shMod(LoadLibrary(csAVRES));
		hrx << (((HANDLE) shMod != (HANDLE)NULL) ? S_OK : CO_E_DLLNOTFOUND);

		// Get names of NAV main UI title and status panel.
		csTitle.LoadString(shMod, IDS_PRODUCTNAME);
		csStatusPanel.LoadString(shMod, IDS_STATUS_CATEGORYTITLE);

		csTitle.Remove(_T('&'));
		csStatusPanel.Remove(_T('&'));

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
		DJSMAR_LicenseType type = DJSMAR_LicenseType_Retail;
		hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)type, DJSMAR_LicenseType_Violated);
		CCTRACEI(_T("NAVUIHelp::BuildNisPluginParams, LicenseType=%d"), type);

		// Build command line.
		//  If rental, add /nosysworks switch.
		if(type == DJSMAR_LicenseType_Rental)
		{
			csParam.Format(_T("/nosysworks /dat:%s /goto:%s\\%s"), csNisPlug, csTitle, csStatusPanel);		
		}
		else
		{
			csParam.Format(_T("/dat:%s /goto:%s\\%s"), csNisPlug, csTitle, csStatusPanel);
		}

		CCTRACEI(_T("NAVUIHelp::BuildNisPluginParams, main UI param=%s"), csParam);

		if(csParam.GetLength() > dwSize)
			return E_FAIL;

		_tcscpy(lpszParam, csParam);		
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}
};
