////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PRODUCTPEPHELPER_H
#define PRODUCTPEPHELPER_H
#pragma once

#include "stdafx.h"
#ifdef _PEP_IMPL_
// For defining
#define _CSIMONV2AUTHORIZATION
#define _V2AUTHORIZATION_SIMON          
#define INIT_V2AUTHORIZATION_IMPL
#define INIT_RSA_PUBLIC_KEY
#define INIT_RSA_PRIVATE_KEY
#define _PEPNOTIFY_V2AUTHMAP_IMPL
#define _INIT_COSVERSIONINFO
#endif //PEP_IMPL_
#include "V2Authorization.h"

// Trace header
#include "ccTrace.h"
//============================================================================================
// Mapping DRMTRACE to CCTRACE
//============================================================================================
#define DRMTRACEE CCTRACEE
#define DRMTRACEI CCTRACEI


// generic PEP header
#define _SIMON_PEP_
#include "PolicyEnforcement_Simon.h"
#include "PepMiddleMan.h"

// Licensing constants
#include "cltLicenseConstants.h"

// Cache store implementation header
#include "cltPepCacheConstants.h"

#include "PRODINFOPEPHelper.h"
#include "cltPepConstants.h"
//#include "cltPepHandlers.h"

// Digital signature checking implementation
#include "DRMTrustHelper.h"

#include "resource.h"
#include "GlobalEvents.h"

#define _NAVOPTREFRESH_CONSTANTS
#include "NAVOptRefresh.h"

#include "NAVLicense_h.h"

#include "resource.h"
#include "ISVersion.h"
#include "build.h"
#include "NavOptHelperEx.h"

#include "PRODINFOPEPHelper.h"
#include "cltPepConstants.h"

#include "cltPepCacheConstants.h"

#ifdef _PEP_IMPL_
#include "ProdCCSettingsHelperImpl.h"
#include <PepMiddleMan.h>
#include "ALEComponentPepHelper.h"
#include "ComponentPepHelper.h"
#include "PRODINFOPEPHelper.h"
#include "cltPepConstants.h"

#include "NAVInfo.h"

//Console
#include "ConsPEPDefines.h"
#include "nscIConsoleServer.h"
#include "nscGetProductFeatures.h"
#include "nscApiLoader.h"
#include "nscModuleNames.h"
#include "nscIFeatureGUIDsCollection.h"
#include "FeatureDefines.h"
#include "FeaturePEPHelper.h"

#include "ccSymCommonClientInfo.h"

//============================================================================================
// GLOBALS
//============================================================================================
static const TCHAR *SZ_PRODUCT_SUITEOWNER_GUID = V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID;
static const TCHAR *SUBSCRIPTION_CACHE_LOCATION = _T("Norton Antivirus\\NAVOpts.dat\\DRM");
#endif

//============================================================================================
// PEP V2 Authorization MAP
//============================================================================================
#ifdef _DRM_V2AUTHMAP_IMPL
AUTHORIZATION_MAP_BEGIN(ThisObjectAuthorizationMap)
    AUTHORIZATION_MAP_ENTRY(V2LicensingAuthGuids::GUID_NAV_ProductLine_2004)
    AUTHORIZATION_MAP_ENTRY(V2LicensingAuthGuids::GUID_AntiVirus_Component)
AUTHORIZATION_MAP_END()

AUTHORIZATION_MAP_BEGIN(DRMPEPSinkMustHaveThisMap)
    AUTHORIZATION_MAP_ENTRY(V2LicensingAuthGuids::GUID_NAV_ProductLine_2004)
AUTHORIZATION_MAP_END2(DRMPEPSinkMustHaveThisMap)
#else
AUTHORIZATION_MAP_EXTERN(ThisObjectAuthorizationMap)
AUTHORIZATION_MAP_EXTERN(DRMPEPSinkMustHaveThisMap)
#endif

#ifdef _DRM_V2AUTHMAP_IMPL_HYBRID
AUTHORIZATION_MAP_BEGIN(ThisObjectAuthorizationMap)
    AUTHORIZATION_MAP_ENTRY(V2LicensingAuthGuids::GUID_NAV_ProductLine_2004)
    AUTHORIZATION_MAP_ENTRY(V2LicensingAuthGuids::GUID_AntiVirus_Component)
AUTHORIZATION_MAP_END()

AUTHORIZATION_MAP_BEGIN(DRMPEPSinkMustHaveThisMap)
    AUTHORIZATION_MAP_ENTRY(V2LicensingAuthGuids::GUID_AntiVirus_Component)
AUTHORIZATION_MAP_END2(DRMPEPSinkMustHaveThisMap)
#else
AUTHORIZATION_MAP_EXTERN(ThisObjectAuthorizationMap)
AUTHORIZATION_MAP_EXTERN(DRMPEPSinkMustHaveThisMap)
#endif


#ifdef _PEP_IMPL_
//============================================================================================
// Header for MACROS for tracing
//============================================================================================
#include "SettingsHelper.h"
//============================================================================================
// PEP Parent/Child helper header
//============================================================================================
#define _PEPNOTIFY_V2AUTHMAP_IMPL
#endif

//============================================================================================
// PEP Handler MAP for NAV
//============================================================================================
#ifdef _PEP_IMPL_
DECLARE_CLT_PEP_HANDLERS
DECLARE_PRODINFO_HANDLERS

namespace AntiVirus
{
		HRESULT HandleAboutInformation(PVOID pThis, SIMON_COLLECTION::IDataCollection* pCollection);
		HRESULT HandleLaunchUI(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl);
        HRESULT FeatureConfigure(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl);
        HRESULT FeatureUninstall(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl);
        HRESULT FeatureMarkDirty(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl);
}


PEPHANDLER_BEGIN(_PepItem)
    CLT_PEPHANDLER_ENTRIES
	PRODINFO_PEPHANDLER_ENTRIES
	PEPHANDLER_ENTRY(&V2LicensingAuthGuids::GUID_ALE_COMPONENT, ALEComponent::POLICY_ID_PAID_CONTENT_ALE_UPDATES, clt::pep::AllowAccessPaidContent)
	PEPHANDLER_ENTRY(&V2LicensingAuthGuids::GUID_AntiVirus_Component, AntiVirusComponent::POLICY_ID_PAID_CONTENT_VDEF_UPDATES, clt::pep::AllowAccessPaidContent)
	PEPHANDLER_ENTRY(&CONSOLE::CONSOLEGUID, CONSOLE::POLICY_ID_LAUNCH_UI, AntiVirus::HandleLaunchUI)
    PEPHANDLER_ENTRY(&PRODINFO::PRODINFOGUID, PRODINFO::POLICY_ID_QUERY_ABOUT_INFORMATION, AntiVirus::HandleAboutInformation)
	PEPHANDLER_ENTRY(&FeatureProxy::FEATUREGUID, FeatureProxy::POLICY_ID_ACTIVATE, clt::pep::HandleActivateLicense)
    PEPHANDLER_ENTRY(&FeatureProxy::FEATUREGUID, FeatureProxy::POLICY_ID_CONFIGURE, AntiVirus::FeatureConfigure)
    PEPHANDLER_ENTRY(&FeatureProxy::FEATUREGUID, FeatureProxy::POLICY_ID_UNINSTALL, AntiVirus::FeatureUninstall)
    PEPHANDLER_ENTRY(&FeatureProxy::FEATUREGUID, FeatureProxy::POLICY_ID_MARK_FEATURES_DIRTY, AntiVirus::FeatureMarkDirty)
PEPHANDLER_END()
#else
PEPHANDLER_EXTERN(_PepItem)
#endif

#ifdef _PEP_IMPL_
//============================================================================================
// PEP Handler class
//============================================================================================
class CProductPEPHandler  :
    public SIMON_PEP::CPEPSinkObjectT2V2<
    CProductPEPHandler
    ,(V2Authorization::_AUTHORIZATION_GUID_ENTRY*)&(ThisObjectAuthorizationMap)
    ,(SIMON_PEP::PEPHANDLER_ITEM*)&_PepItem>  
{
public:
    CProductPEPHandler():m_hrCoInit(S_OK){}
protected:
    virtual HRESULT OnPreQueryPolicy(SIMON_COLLECTION::IDataCollection* pCollection)
    {
        m_hrCoInit = ::CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
        if(FAILED(m_hrCoInit))
        {
            if(m_hrCoInit == RPC_E_CHANGED_MODE)
            {
                m_hrCoInit = ::CoInitializeEx(NULL,COINIT_MULTITHREADED);
            }
        }
        return m_hrCoInit;
    }
    virtual HRESULT OnPostQueryPolicy(SIMON_COLLECTION::IDataCollection* pCollection)
    {
        if(SUCCEEDED(m_hrCoInit))
            ::CoUninitialize();
        return m_hrCoInit;
    }
    HRESULT m_hrCoInit;
};
//============================================================================================
// Product specific PEP handler class - Used by DRMPepImpl - Supports IProdPepHelper
// Provides product specific implementation
//============================================================================================
class CProdPepHelper : public CProdCCSettingsHelperImpl
{

private:

public: 
CProdPepHelper::CProdPepHelper(void) 
{
}

CProdPepHelper::~CProdPepHelper(void)
{
	// if settings were initialized store them just in case they change
}

HRESULT CProdPepHelper::GetLicenseDispatch( IDispatch **pDisp)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        hrx << CoCreateInstance( CLSID_NAVLicenseInfo, NULL, CLSCTX_INPROC, IID_IDispatch, (LPVOID *)&pDisp );
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}



HRESULT CProdPepHelper::GetIcon(HINSTANCE& hInstance, UINT& uIconID)
{
    HRESULT hr = S_OK;

    uIconID = NAVW_ICO;
	hInstance = _g_hInstance;
    
    if(hInstance == INVALID_HANDLE_VALUE)
        hr = E_FAIL;

    return hr;
}


HRESULT CProdPepHelper::GetString(UINT uID, LPTSTR szString, DWORD dwBuffer)
{
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    ZeroMemory(szString, dwBuffer);

    if(0< ::LoadString(_g_hInstance, uID, szString, dwBuffer))
        hr = S_OK;

    return hr;
}

private:

};

namespace clt {
namespace pep {
    TCHAR szComponentPath[] = _T("SOFTWARE\\Symantec\\CCPD\\SuiteOwners\\{4E415600-2005-0000-0000-000000000000}\\Children");
    /////////////////////////////////////////////////////////////////////////////////////////////
    // Function HandleCacheUpdate
    //
    // Description:
    // NAV specific handler for cache updates
    /////////////////////////////////////////////////////////////////////////////////////////////
    inline HRESULT HandleCacheUpdate(PVOID pThis, SIMON_COLLECTION::IDataCollection* pCollection)
    {
		AntiVirus::FeatureMarkDirty(NULL, NULL);

		// For install over NPF prior to 2006, ALECompbr.dll does not delay load
		// CCLib, so before we propogate the cache update we need to add the CC
		// directory to the PATH variable.
		// TODO: check for the directory already being in the PATH variable.

		DWORD dwRet;
		const DWORD BUFSIZE = 2049;
		CString sOldPath;
		CString sNewPath;
		CString sCCPath;

		dwRet = GetEnvironmentVariable(_T("PATH"), sOldPath.GetBufferSetLength(BUFSIZE), BUFSIZE);
		sOldPath.ReleaseBuffer();

		if(dwRet > BUFSIZE)
		{
			// If the return value is larger than our buffer size, it means the call failed and
			// we need to give it a larger buffer.
			CCTRACEI(_T("clt::pep::HandleCacheUpdate: GetEnvironmentVariable failed, resizing buffer to %i characters"), dwRet);
			dwRet = GetEnvironmentVariable(_T("PATH"), sOldPath.GetBufferSetLength(dwRet), dwRet);
		}

		if(0 == dwRet)
		{
			CCTRACEI(_T("clt::pep::HandleCacheUpdate: Failed to get PATH variable"));
		}
		else
		{
			CCTRACEI(_T("clt::pep::HandleCacheUpdate: Current PATH = %s"), (LPCTSTR)sOldPath);
	
			if(ccSym::CInstalledApps::GetCCDirectory(sCCPath))
			{
				sNewPath.Format(_T("%s;%s"), (LPCTSTR)sCCPath, (LPCTSTR)sOldPath);
				CCTRACEI(_T("clt::pep::HandleCacheUpdate: PATH after = %s"), (LPCTSTR)sNewPath);
				dwRet = SetEnvironmentVariable(_T("PATH"), (LPCTSTR)sNewPath);
				if(0 == dwRet)
				{
					CCTRACEI(_T("clt::pep::HandleCacheUpdate: Failed to set PATH variable"));
				}
			}
			else
			{
				CCTRACEW(_T("clt::pep::HandleCacheUpdate: Failed to get CC Directory"));
			}
		}

        // Not doing any processing for NAV the product and notifying children
        return BasicDRMMiddleMan::NotifyChildren<szComponentPath>( pThis, pCollection, clt::pep::POLICY_ID_CACHE_UPDATE_NOTIFY );
    }
}// namespace pep
}// namespace clt
#endif

namespace AntiVirus
{
	inline HRESULT HandleAboutInformation(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl)
	{
		STAHLSOFT_HRX_TRY(hr)
		{
            // almost everything needed in this collection is available in the PRODINFO cached collection
            // from when CfgWiz set it all up.  This will put some extra items in the spColl but 
            // thats ok.
            PRODINFO::HandleQueryProductInformation(NULL, spColl);

            // Needed so NAV will show up under "Norton Account" Defect 1-4ICPN9
            hrx << clt::pep::HandleQueryLicenseRecord(pThis,spColl);

            // The collection also needs a valid handle to the product icon so load it up here.
            DJSMAR_LicenseType licType = DJSMAR_LicenseType_Violated;
			DWORD dwValue;
			long lState = DJSMAR00_LicenseState_Violated;
			hrx << SIMON_COLLECTION::GetValueFromCollection(spColl, DRM::szDRMLicenseState, dwValue);
			lState = static_cast<long>(dwValue);
            
            // if we're activated then show the register button.
			if (DJSMAR00_LicenseState_PURCHASED == lState)
            {
			    SIMON_COLLECTION::SetCollectionDWORD(spColl, PRODINFO::szPICanRegisterProduct, 1);
            }
            else
            {
                SIMON_COLLECTION::SetCollectionDWORD(spColl, PRODINFO::szPICanRegisterProduct, 0);
            }

			// Get Product Icon
			if((HANDLE) shMod != (HANDLE) NULL)
			{
				HICON productIcon = NULL;
				CISVersion::GetProductIcon(productIcon);
				SIMON_COLLECTION::SetCollectionDWORD(spColl, PRODINFO::szPIProductIcon,(DWORD) productIcon);

				ccLib::CStringW csCopyRight;
				CISVersion::LoadString(csCopyRight, IDS_BRANDING_SYMANTEC_COPYRIGHT);
				SIMON_COLLECTION::SetCollectionString(spColl, PRODINFO::szPICopyright, CT2CA(csCopyRight));
			}	
        }
        STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)		
	}

	inline HRESULT HandleLaunchUI(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl)
	{

		CCTRACEI(_T("ProductPEPHelper:: Entering HandleLaunchUI()"));

		TCHAR szIntegratorCommandLine[2*MAX_PATH+100]; // integrator and plugin could each be max_path + other switches
		memset(szIntegratorCommandLine, 0, sizeof(TCHAR)*(2*MAX_PATH+100));
		const DWORD dwSize = CCDIMOF(szIntegratorCommandLine)-2;

		ccLib::CString sCCPath;
		if(!ccSym::CCommonClientInfo::GetCCDirectory(sCCPath))
		{
			CCTRACEE(_T("AntiVirus::HandleLaunchUI : Failed to get cc Path"));
			return E_FAIL;
		}

		ccLib::CString sNAVDir;

		if(!ccSym::CCommonClientInfo::GetNAVDirectory(sNAVDir))
		{
			CCTRACEE(_T("AntiVirus::HandleLaunchUI : Failed to get NAV Path"));
			return E_FAIL;
		}

		_sntprintf(szIntegratorCommandLine, dwSize, _T("\"%s\\%s\" /nosysworks /dat:%s\\%s /goto:Norton Protection Center"), sCCPath.GetBuffer(), _T("NMain.exe"), sNAVDir.GetBuffer(), _T("navui.nsi"));
		
		STARTUPINFO startupInfo;
		memset(&startupInfo, 0, sizeof(STARTUPINFO));
		startupInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION processInfo;
		if (CreateProcess(  NULL, szIntegratorCommandLine,
			NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE |  NORMAL_PRIORITY_CLASS,
			NULL, NULL, &startupInfo, &processInfo))
		{
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
		}

		CCTRACEI(_T("ProductPEPHelper:: Exiting HandleLaunchUI()"));

		return S_OK;
	}

    inline HRESULT FeatureConfigure(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl)
    {
        CCTRACEI(_T("ProductPEPHelper:: Entering FeatureConfigure()"));

		TCHAR szCfgWizCommandLine[MAX_PATH+100]; // could be max_path + other switches
		memset(szCfgWizCommandLine, 0, sizeof(TCHAR)*(MAX_PATH+100));
		const DWORD dwSize = CCDIMOF(szCfgWizCommandLine)-2;

			ccLib::CString sNAVDir;

		if(!ccSym::CCommonClientInfo::GetNAVDirectory(sNAVDir))
		{
			CCTRACEE(_T("AntiVirus::HandleLaunchUI : Failed to get NAV Path"));
			return E_FAIL;
		}

		CNAVInfo navInfo;
		_sntprintf(szCfgWizCommandLine, dwSize, _T("\"%s\\CfgWiz.exe\" /GUID {0D7956A2-5A08-4ec2-A72C-DF8495A66016} /MODE CfgWiz"), navInfo.GetOnlinePlatformClientDir());
		
		STARTUPINFO startupInfo;
		memset(&startupInfo, 0, sizeof(STARTUPINFO));
		startupInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION processInfo;
		if (CreateProcess(  NULL, szCfgWizCommandLine,
			NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE |  NORMAL_PRIORITY_CLASS,
			NULL, NULL, &startupInfo, &processInfo))
		{
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
            return S_OK;
		}
        else
        {
            return E_FAIL;
        }
    }

    inline HRESULT FeatureUninstall(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl)
    {
		CCTRACEI(_T("ProductPEPHelper:: Entering FeatureUninstall()"));

        TCHAR szUninstallCmd[MAX_PATH+100]; // could be max_path + other switches
		memset(szUninstallCmd, 0, sizeof(TCHAR)*(MAX_PATH+100));
		DWORD dwSize = CCDIMOF(szUninstallCmd)-2;
        
        ATL::CRegKey rkUninstallKey;
        LONG lRet = rkUninstallKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\SymSetup.{C6F5B6CF-609C-428E-876F-CA83176C021B}"), KEY_READ);
		if(ERROR_SUCCESS != lRet)
			CCTRACEE(_T("ProductPEPHelper::FeatureUninstall(): Failed to open uninstall reg key, lRet=%d"), lRet);

        lRet = rkUninstallKey.QueryStringValue(_T("UninstallString"), szUninstallCmd, &dwSize);
		if(ERROR_SUCCESS != lRet)
			CCTRACEE(_T("ProductPEPHelper::FeatureUninstall(): Failed to query uninstall reg value, lRet=%d"), lRet);
        
        STARTUPINFO startupInfo;
		memset(&startupInfo, 0, sizeof(STARTUPINFO));
		startupInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION processInfo;
		if (CreateProcess(  NULL, szUninstallCmd,
			NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE |  NORMAL_PRIORITY_CLASS,
			NULL, NULL, &startupInfo, &processInfo))
		{
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
            return S_OK;
		}
        else
        {
            return E_FAIL;
        }

		CCTRACEI(_T("ProductPEPHelper:: Exiting FeatureUninstall()"));
    }

    inline HRESULT FeatureMarkDirty(PVOID pThis, SIMON_COLLECTION::IDataCollection* spColl)
    {
        nsc::nscConsoleServerLoader nscLdr;
        
        STAHLSOFT_HRX_TRY(hr)
		{
            nscLib::CFeatureGUIDsCollection* pGuidCol = new nscLib::CFeatureGUIDsCollection;
            CSymPtr<nsc::IFeatureGUIDsCollection> pIGuidCol = pGuidCol;

            CFeatureHelpers::GetProductFeatures(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID, pGuidCol);
            
            nsc::IConsoleServerPtr spConsoleServer;
            nscLdr.CreateObject( &spConsoleServer);

            if(spConsoleServer)
                spConsoleServer->UpdateWrappers(pIGuidCol, 0xFFFFFFFF);
        }
        STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)	    
    }

}//namespace AntiVirus

#endif //PRODUCTPEPHELPER_H
