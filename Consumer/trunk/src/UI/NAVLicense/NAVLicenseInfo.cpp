////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVLicenseInfo.cpp : Implementation of CNAVLicenseInfo
#include "stdafx.h"
#include <ATLComTime.h>
#include "resource.h"

#include "StahlSoft.h"
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

#include "Simon.h"

#define INITIIDS

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "PepMiddleMan.h"

#include "NAVTrust.h"
#include "NavSettingsHelperEx.h"
#include "AVccModuleId.h"

#include "NAVLicenseNames.h"
#include "InstOptsNames.h"
#include "NAVLicense_h.h"
#include "NAVLicenseInfo.h"
#include "NAVInfo.h"
#include "AVRESBranding.h"
#include "NAVLicenseNames.h"
#include "NAVErrorResource.h"
#include "OptNames.h"
#include "ActRes_Static.h"				
#include "SWMisc.h"				// WMU_REFRESH_CURRENT_CATEGORY						
#include "NavOptHelperEx.h"
#include "OEMProductIntegrationDefines.h"
#include "DJSToolBoxLib.h"

#include "SuiteOwnerHelper.h"

#define _NAVOPTREFRESH_CONSTANTS
#include "NAVOptRefresh.h"
#include "NavOptionRefreshHelperInterface.h"

const TCHAR SZ_INTEGRATOR_WINDOWCLASSNAME[] = _T("SYMIntegratorWnd");

// Local Function
HWND GetAbsoluteParent( HWND &hWnd );


/////////////////////////////////////////////////////////////////////////////
//
// Function:    SimonLaunchURL
//
/////////////////////////////////////////////////////////////////////////////
BOOL CNAVLicenseInfo::SimonLaunchURL(UINT uResID, bool bSimonFunc)
{
	CNAVInfo navInfo;
	TCHAR szSMNLnch[MAX_PATH] = {0};
	TCHAR szParam[MAX_PATH] = {0};
	TCHAR szSymCmmPath[MAX_PATH] = {0};
	TCHAR szNAVPath[MAX_PATH] = {0};
	DWORD dwLength = sizeof(szNAVPath) / sizeof(TCHAR);

	// Must use short path names.
	GetShortPathName(navInfo.GetSymantecCommonDir(), szSymCmmPath, dwLength);
	GetShortPathName(navInfo.GetNAVDir(), szNAVPath, dwLength);

	wsprintf(szSMNLnch, _T("%s\\SMNLnch.exe"), szSymCmmPath);

	if(bSimonFunc)
	{
		wsprintf(szParam, _T("-dll %s\\NAVUI.DLL -func _Upsell@8 -hint %d"), szNAVPath, uResID);
	}
	else
	{
		wsprintf(szParam, _T("-dll %s\\NAVUI.DLL -func _FetchURL@8 -hint %d"), szNAVPath, uResID);
	}

	if ( HINSTANCE (32) >= ::ShellExecute(::GetDesktopWindow(), _T("open")    // Verb
		, szSMNLnch
		, szParam      // Parameters
		, NULL      // Directory
		, SW_SHOW)) // ShowCmd
	{
		return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    EnumWindowsProc
//
// Description:	After product activation complete, 
//				enumerate all NMain windows to request a UI refresh.
//
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	// Get window class name to compare.
	TCHAR szClassName[MAX_PATH] = {0}; 
	::GetClassName(hWnd, szClassName, sizeof(szClassName));

	// Is this an NMain window?
	if(::_tcscmp(szClassName, SZ_INTEGRATOR_WINDOWCLASSNAME) == 0)
	{
		// Request a UI refresh.
		::PostMessage(hWnd, WMU_REFRESH_CURRENT_CATEGORY, 0, 0);
	}

	// Continue enumeration.
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:    Constructor
//
/////////////////////////////////////////////////////////////////////////////
CNAVLicenseInfo::CNAVLicenseInfo()
{
	// Intentionally empty.
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    InternalActivateLicense
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CNAVLicenseInfo::InternalActivateLicense(HWND hWndParent, LONG nParam, BOOL* pbSuccess)
{   
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (pbSuccess? S_OK : E_POINTER);
		*pbSuccess = TRUE;

		GetLicenseInfo();

		// Rental/CTO: If CfgWiz is not finished, launch CfgWiz.
		//  Otherwise, launch Activation wizard.
		bool bLaunchCfgWiz = false;
		CString csCMDLine;
		CString csCfgWiz;

		// Set up path to CfgWiz.dat.
		CNAVInfo NAVInfo;
		CString sCfgWizDat;
		sCfgWizDat.Format(_T("%s\\CfgWiz.dat"), NAVInfo.GetNAVDir());

		// Check to see if CfGwiz is finished or if we're OEM.
		{ /* SCOPE CNAVOptFileEx */
			DWORD dwCfgWizFinished = 0;
			DWORD dwOEM = 0;
			CNAVOptFileEx cfgWizOpt;
			hrx << (cfgWizOpt.Init(sCfgWizDat, FALSE)? S_OK : E_FAIL);
			cfgWizOpt.GetValue(InstallToolBox::CFGWIZ_Finished, dwCfgWizFinished, 1);
			cfgWizOpt.GetValue(InstallToolBox::CFGWIZ_OEM, dwOEM, 0);

			if(dwCfgWizFinished == 0)
			{
				// CfgWiz is not finished, launch cfgwiz if we're CTO or rental.
				if((dwOEM == 2) || (m_Type == DJSMAR_LicenseType_Rental))
					bLaunchCfgWiz = true;
			}
		} /* END OF SCOPE CNAVOptFileEx */

        STARTUPINFO si = {0};
        si.cb = sizeof( STARTUPINFO );
        PROCESS_INFORMATION pi = {0};

        TCHAR szCmdLine[MAX_PATH] = {0};

        if (bLaunchCfgWiz)
        {
            wsprintf(szCmdLine
                ,_T("\"%s\\CfgWiz.exe\" /GUID %s /MODE CfgWiz /CMDLINE \"/RM %d\"")
                ,NAVInfo.GetOnlinePlatformClientDir()
                ,V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT
                ,nParam);
        }
        else
        {
            hWndParent = DJSToolBox::GetAbsoluteParent(hWndParent);

            if(DJSMAR00_LicenseState_PURCHASED == m_State)
			{
				if ((-1 != nParam) && (DJSMAR_LicenseType_Rental == m_Type))
				{
					wsprintf(szCmdLine, _T("\"%s\\CfgWiz.exe\" /GUID %s /MODE ActWiz /RA /ReAct /PARENT %d /CMDLINE \"/RM %d\"")
						,NAVInfo.GetOnlinePlatformClientDir(), GetSuiteOwnerGUID(), hWndParent, nParam);
				}
				else
				{
					wsprintf(szCmdLine, _T("\"%s\\CfgWiz.exe\" /GUID %s /MODE ActWiz /RA ReAct /PARENT %d")
						,NAVInfo.GetOnlinePlatformClientDir(), GetSuiteOwnerGUID(), hWndParent);
				}
			}
			else
			{
				if ((-1 != nParam) && (DJSMAR_LicenseType_Rental == m_Type))
				{
					wsprintf(szCmdLine, _T("\"%s\\CfgWiz.exe\" /GUID %s /MODE ActWiz /PARENT %d /CMDLINE \"/RM %d\"")
						,NAVInfo.GetOnlinePlatformClientDir(), GetSuiteOwnerGUID(), hWndParent, nParam);
				}
				else
				{
					wsprintf(szCmdLine, _T("\"%s\\CfgWiz.exe\" /GUID %s /MODE ActWiz /PARENT %d")
					 ,NAVInfo.GetOnlinePlatformClientDir(), GetSuiteOwnerGUID(), hWndParent);
				}
			}
        }

        CCTRACEI(_T("CLicenseWrapper::LaunchActivationWizard - CmdLine=%s"), szCmdLine);

        hrx << ((0 == ::CreateProcess(NULL, (LPTSTR)(LPCTSTR)szCmdLine, NULL,
            NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi)) ?
            E_FAIL : S_OK);

        // Wait for the activation to complete.
        DWORD dwWait = StahlSoft::WaitForSingleObjectWithMessageLoop(pi.hThread);

        if( ::IsWindow(hWndParent))
        {
            ::EnableWindow( hWndParent, TRUE);
            ::SetActiveWindow( hWndParent );
            ::SetFocus( hWndParent );
        }

		// Do licensing refresh only if we launched activation wizard.
		//  CfgWiz always does licensing refresh after activation.
		if(bLaunchCfgWiz == false)
		{
			// Activation completed. Force a licensing refresh.
			hr = InternalRefreshLicensing();
			if(forceError (ERR_NAVOPTRF_REFRESH_FAILED) || (hr != S_OK))
			{
				CCTRACEE(_T("CNAVLicenseInfo::InternalActivateLicense -> Failed to refresh licensing."));
				makeError(ERR_NAVOPTRF_REFRESH_FAILED, E_FAIL);
				return E_FAIL;
			}
		}

		// Get latest licensing info
		GetLicenseInfo(true);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

CString CNAVLicenseInfo::GetSuiteOwnerGUID()
{
    if(m_sOwnerSeed.IsEmpty())
    {
        CRegKey key;
        CString csKey( DRMREG::SZ_SUITE_OWNER_KEY );
        csKey = csKey  + "\\" + V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;

        BasicDRMMiddleMan::CPreferedOwnerEnum<NULL, NULL> poe(csKey, V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
        poe.GetOwnerGUID(0, m_sOwnerSeed);
    }

    return m_sOwnerSeed;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    RenewLicense
//
// Note:		Function obsolete. Use ActivateLicense instead.
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::RenewLicense(BSTR bstrClsWndName, BOOL *pbSuccess)
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::RenewLicense -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	return(this->ActivateLicense(bstrClsWndName, pbSuccess));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    ActivateLicense
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::ActivateLicense(BSTR bstrClsWndName, BOOL *pbSuccess)
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::ActivateLicense -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	USES_CONVERSION;
	HWND hWndParent = NULL;

	if(bstrClsWndName)
	{
		TCHAR szClsName[MAX_PATH] = {0};
		TCHAR szWndName[MAX_PATH] = {0};
		TCHAR sz[MAX_PATH] = {0};
		LPTSTR lpszTab = NULL;

		*pbSuccess = FALSE;

		// Separate window class name from window name.

		_tcscpy(sz, OLE2T(bstrClsWndName));
		lpszTab = _tcsrchr(sz, _T('\t'));

		if(lpszTab)
		{
			_tcsncpy(szClsName, sz, lpszTab - sz);
			_tcscpy(szWndName, lpszTab + sizeof(TCHAR)); 
		}

		// Find handle to the parent window and its title.
		if(_tcslen(szClsName) + _tcslen(szWndName)) 
		{
			hWndParent = FindWindow(*szClsName ? szClsName : NULL, *szWndName ? szWndName : NULL);
		}
	}

	return(InternalActivateLicense(hWndParent, SYMALERT_RESULT_RESOLVELICENSE, pbSuccess));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    UpdateLicense
//
// Note:		Function obsolete. Use ActivateLicense instead.
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::UpdateLicense(BSTR bstrClsWndName, BOOL *pbSuccess)
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::UpdateLicense -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	return(this->ActivateLicense(bstrClsWndName, pbSuccess));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    LaunchRentalMoreInfo
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::LaunchRentalMoreInfo()
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::LaunchRentalMoreInfo -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(!SimonLaunchURL(IDS_SUBSCRIPTION_MOREINFO, false))
	{
		CCTRACEE(_T("CNAVLicenseInfo::_SimonLauchURL-> failed"));
		makeError(ERR_SIMONLAUNCH_FAILED, E_FAIL);
		return E_FAIL;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    LaunchRentalTechSupport
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::LaunchRentalTechSupport()
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::LaunchRentalTechSupport -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(!SimonLaunchURL(IDS_TECHSUPP_WEBSITE, false))
	{
		CCTRACEE(_T("CNAVLicenseInfo::_SimonLauchURL-> failed"));
		makeError(ERR_SIMONLAUNCH_FAILED, E_FAIL);
		return E_FAIL;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    LaunchRentalProductReg
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::LaunchRentalProductReg()
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::LaunchRentalProductReg -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(!SimonLaunchURL(IDS_PRODUCT_REG_URL, false))
	{
		CCTRACEE(_T("CNAVLicenseInfo::_SimonLauchURL-> failed"));
		makeError(ERR_SIMONLAUNCH_FAILED, E_FAIL);
		return E_FAIL;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    get_NAVError
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::get_NAVError(INAVCOMError* *pVal)
{
	if ( !m_spError )
		return E_FAIL;

	*pVal = m_spError;
	(*pVal)->AddRef (); // We get a ref and the caller gets one

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    makeError
//
/////////////////////////////////////////////////////////////////////////////
void CNAVLicenseInfo::makeError(long lMessageID, long lHResult)
{
	if ( !m_spError )
		return;

	m_spError->put_ModuleID ( AV_MODULE_ID_NAV_LICENSE );
	m_spError->put_ErrorID ( lMessageID );
	m_spError->put_HResult ( lHResult );
	// Request to display generic error message
	//  "Norton AntiVirus has encountered an internal program error. 
	//  Uninstall and re-install Norton AntiVirus."
	m_spError->put_ErrorResourceID ( IDS_NAVERROR_INTERNAL_REINSTALL );		
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    forceError
//
/////////////////////////////////////////////////////////////////////////////
bool CNAVLicenseInfo::forceError(long lErrorID)
{
	if ( !m_spError )
		return false;

	long lTempErrorID = 0;
	long lTempModuleID = 0;

	m_spError->get_ForcedModuleID ( &lTempModuleID );
	m_spError->get_ForcedErrorID ( &lTempErrorID );

	if ( lTempModuleID == AV_MODULE_ID_NAV_LICENSE &&
		lTempErrorID == lErrorID )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    FinalConstruct
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CNAVLicenseInfo::FinalConstruct()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		m_bRefreshed = true;				
		m_Type = DJSMAR_LicenseType_Violated;
		m_State = DJSMAR00_LicenseState_Violated;
		m_VendorId = DJSMAR00_VendorID_Symantec;
		m_Zone = 0;
		m_dwRemainingDays = 0;
		m_dwTrialRemainingDays = 0;
		m_dwCancelCode = 0;
		m_dwDieDate = 0;

		m_bSubRefreshed = true;
		m_bSubCanRenew = false;
		m_dwSubRemainingDays = 0;
		m_dwSubEndDate = 0;
		m_dwSubLength = 0;
		m_bSubCanAutoRenew = FALSE;
		m_bSubAutoRenewReady = FALSE;

		CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

		// Check NAVError module for Symantec Signature...
		if(NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer("NAVError.NAVCOMError"))
		{
			HRESULT hr2 = m_spError.CoCreateInstance(bstrErrorClass, NULL, CLSCTX_INPROC_SERVER);
			if(FAILED(hr2))
			{
				CCTRACEE(_T("CNAVLicenseInfo::FinalConstruct - Failed to create NAVError 0x%08X"), hr2);
			}
		}

        hrx << m_cltHelper.Initialize();
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    FinalRelease
//
/////////////////////////////////////////////////////////////////////////////
void CNAVLicenseInfo::FinalRelease()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    InternalRefreshLicensing
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CNAVLicenseInfo::InternalRefreshLicensing()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(true) // BEGIN SCOPE	smLibNavOptRf
		{	

			// Refresh product using NAVOptRf object. 
			// Check NAVOptRf security
			TCHAR szNAVOptRf[_MAX_PATH*2] = {0};
			CNAVInfo NAVInfo;
			::wsprintf(szNAVOptRf, _T("%s\\NAVOPTRF.DLL"), NAVInfo.GetNAVDir());
			if(forceError (ERR_SECURITY_FAILED) || (NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szNAVOptRf)))
			{
				CCTRACEE(_T("CNAVLicenseInfo::InternalRefreshLicensing -> NAVOPTRF.DLL Not Safe"));
				return ERR_SECURITY_FAILED;
			}

			// Load NAVOptfRf.dll
			StahlSoft::CSmartModuleHandle smLibNavOptRf(::LoadLibrary(NULL, LOAD_WITH_ALTERED_SEARCH_PATH));
			if(forceError (ERR_NAVOPTRF_LOAD_FAILED) || ((HANDLE) smLibNavOptRf == (HANDLE) NULL))
			{
				CCTRACEE(_T("CNAVLicenseInfo::InternalRefreshLicensing -> Load NAVOptionRefresh failed"));
				return ERR_NAVOPTRF_LOAD_FAILED;
			}

			// Create NAVOptionRefresh object.
			SIMON::CSimonPtr<INAVOptionRefresh> spOptionRefresh;
			hr = SIMON::CreateInstanceByDLLInstance(smLibNavOptRf,CLSID_CNAVOptionRefresh, IID_INAVOptionRefresh,(void**)&spOptionRefresh);        
			if(forceError (ERR_NAVOPTRF_CREATEOBJECT_FAILED) || FAILED(hr))
			{
				CCTRACEE(_T("CNAVLicenseInfo::InternalRefreshLicensing -> Create option refesh object failed"));
				return ERR_NAVOPTRF_LOAD_FAILED;
			}

			// Force a refresh
			hr = spOptionRefresh->Refresh(REFRESH_COMMIT_ON_CHANGE_FULLY);
			if(forceError (ERR_NAVOPTRF_REFRESH_FAILED) || FAILED(hr))
			{
				CCTRACEE(_T("CNAVLicenseInfo::InternalRefreshLicensing -> Load NAVOptionRefresh failed to refresh"));
				// NAVOptRf already handle error when it fails to do licensing refresh
				//  (i.e. Display error dialog; Set licensing info to appropriate values; etc.). 
				//  We can return an S_OK here to avoid mulitple error dialogs displayed.
				return S_OK;
			}

			m_bRefreshed = true;
			m_bSubRefreshed = true;

		}	// END OF SCOPE

		// Refresh Main UI
		EnumWindows(EnumWindowsProc, 0);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    ActivateLicense2
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::ActivateLicense2(long hWndParent, BOOL *pbSuccess)
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::ActivateLicense2 -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	return(InternalActivateLicense(reinterpret_cast<HWND>(hWndParent), SYMALERT_RESULT_RESOLVELICENSE, pbSuccess));
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    RefreshLicensing
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::RefreshLicensing()
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::RefreshLicensing -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	InternalRefreshLicensing();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    GetLicenseType
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::GetLicenseType(DJSMAR_LicenseType *pType)
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::GetLicenseType -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(pType)
	{
		*pType = DJSMAR_LicenseType_Violated;
		GetLicenseInfo();
		*pType = m_Type;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    GetLicenseState
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::GetLicenseState(DJSMAR00_LicenseState *pState)
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::GetLicenseStatus -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(pState)
	{
		*pState = DJSMAR00_LicenseState_Violated;
		GetLicenseInfo();
		*pState = m_State;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    GetLicenseZone
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::GetLicenseZone(long *plZone)
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::GetLicenseZone -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(plZone)
	{
		*plZone = DJSMAR_LicenseZone_Violated;
		GetLicenseInfo();
		*plZone = m_Zone;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    GetLicenseSubType
// Note:		Subtype is now obsolete. We don't use this value anywhere in NAV.
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::GetLicenseSubType(long* plSubType)
{
	if(forceError (ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::GetLicenseSubType -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(plSubType)
		*plSubType = 0;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    get_LicenseDaysRemaining
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::get_LicenseDaysRemaining(LONG* pVal)
{
	if(forceError(ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::get_LicenseDaysRemaining -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(pVal)
	{
		*pVal = 0;
		GetLicenseInfo();

		if((m_Type == DJSMAR_LicenseType_Rental) 
			&& (m_State == DJSMAR00_LicenseState_TRIAL))
			*pVal = (long) m_dwTrialRemainingDays;
		else
			*pVal = (long) m_dwRemainingDays;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    get_LicenseExpirationDate
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::get_LicenseExpirationDate(BSTR* pVal)
{
	if(forceError(ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::get_LicenseExpirationDate -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(pVal)
	{
		GetLicenseInfo();

		if(m_Type == DJSMAR_LicenseType_Rental)
		{
			time_t curtime = 0;
			time(&curtime);

			COleDateTime date((time_t)(curtime + (m_dwTrialRemainingDays * 24 * 60 * 60)));
			CComBSTR bstrDate(date.Format(VAR_DATEVALUEONLY));
			*pVal = bstrDate.Detach();
		}
		else
		{
			// To be consistant with VS and FSS, re-using the code from FSS:
			SYSTEMTIME stimeDieDate;
			TCHAR szDieDate [64] = {0};
			VariantTimeToSystemTime ( m_dwDieDate, &stimeDieDate);
	        
			GetDateFormat ( LOCALE_USER_DEFAULT,
							DATE_SHORTDATE, 
							&stimeDieDate,
							NULL,
							szDieDate,
							64 );

			CComBSTR bstrDate(szDieDate);

			*pVal = bstrDate.Detach();
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    get_ProductName
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::get_ProductName(BSTR* pVal)
{
	if(forceError(ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::get_ProductName -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	// Get product name from resource string table.
	if(pVal)
	{
		if(m_csProductName.IsEmpty())
		{
			CBrandingRes BrandRes;
			m_csProductName = BrandRes.ProductName();
		}

		CComBSTR bstrProductName(m_csProductName);
		*pVal = bstrProductName.Detach();
		CCTRACEI(_T("CNAVLicenseInfo::get_ProductName -> %s"), m_csProductName);
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    GetLicenseInfo
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CNAVLicenseInfo::GetLicenseInfo(bool bForce)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(bForce || m_bRefreshed)
		{
			CCTRACEI(_T("CNAVLicenseInfo::GetLicenseInfo - Begin"));
			
            // Getting licensing and subscription properties needed to enable product features
            m_cltHelper.ClearCollection();
            hrx << m_cltHelper.InitializeCollection();

			// Getting licensing properties
			hrx << m_cltHelper.GetLicenseState(m_State);
            hrx << m_cltHelper.GetLicenseType(m_Type);
            hrx << m_cltHelper.GetLicenseZone((DJSMAR_LicenseZone&)m_Zone);
            hrx << m_cltHelper.GetSubscriptionRemaining((long&)m_dwRemainingDays);
            hrx << m_cltHelper.GetValueFromCollection(DRM::szDRMLicenseVendorId, 
                                    (DWORD &)m_VendorId, DJSMAR00_VendorID_XtreamLok);
			hrx << m_cltHelper.GetValueFromCollection(DRM::szDRMSCSSTrialRemainingDays, 
                                    (DWORD &)m_dwTrialRemainingDays, 0);
			hrx << m_cltHelper.GetValueFromCollection(DRM::szDRMDieDate, (DWORD &)m_dwDieDate, 0);
			
			if(m_Type == DJSMAR_LicenseType_Rental)
			{
				hrx << m_cltHelper.GetValueFromCollection(DRM::szDRMSCSSCancellationCode, (DWORD &)m_dwCancelCode, 0);
			}

			CCTRACEI(_T("GetLicenseInfo - type=%d, state=%d, zone=%d, vendorId=%d, daysRemaining=%d, trialDaysRemaining=%d, dieDate=%d, cancelCode=%d")
				,m_Type, m_State, m_Zone, m_VendorId, m_dwRemainingDays, m_dwTrialRemainingDays, m_dwDieDate, m_dwCancelCode);

			StahlSoft::CSmartDataPtr<BYTE> spBuff;
            hrx << m_cltHelper.GetCollectionData(DRM::szDRMSCSSVendorName, spBuff);

			if(spBuff.m_p)
			{
				m_csVendorName = (LPCTSTR) spBuff.m_p;
				CCTRACEI(_T("GetLicenseInfo - VendorName=%s"), m_csVendorName);
			}

			m_bRefreshed = false;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    get_LicenseVendorId
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::get_LicenseVendorId(LONG* pVal)
{
	if(forceError(ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::get_LicenseVendorId -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(pVal)
	{
		*pVal = 0;
		GetLicenseInfo();
		*pVal = (long) m_VendorId;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    LaunchActivationWizard
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::LaunchActivationWizard(long hWndParent, long nFlag)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::LaunchActivationWizard -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		LaunchActivationWizardEx(hWndParent, SYMALERT_RESULT_RESOLVELICENSE, nFlag);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    LaunchActivationWizardEx
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::LaunchActivationWizardEx(LONG hWndParent, LONG nParam, LONG nFlag)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError (ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::LaunchActivationWizardEx -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		HWND hWnd = NULL;

		switch(nFlag)
		{
		case HWNDPARENT_FIND:
			hWnd = GetActiveWindow();
			break;

		case HWNDPARENT_PASSED:
			hWnd = (HWND) hWndParent;

		case HWNDPARENT_NULL:
		default:
			break;
		}

		BOOL bSuccess = TRUE;
		InternalActivateLicense(hWnd, nParam, &bSuccess);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    GetLicenseStateEx
//
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVLicenseInfo::GetLicenseStateEx(DJSMAR00_QueryMode mode, DJSMAR00_LicenseState *pState)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		CCTRACEI(_T("CNAVLicenseInfo::GetLicenseStateEx - Begin"));

		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::GetLicenseStateEx -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pState)
		{
			*pState = DJSMAR00_LicenseState_Violated;

			if(mode == DJSMAR00_QueryMode_ForceOverride)
			{
//                pepBase.SetContextGuid(clt::pep::CLTGUID);
//                pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
                m_cltHelper.ClearCollection();
//                hrx << m_cltHelper.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
                hrx << m_cltHelper.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_START_LICENSE);
                hrx << m_cltHelper.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

				if(!m_sOwnerSeed.IsEmpty())
				{
                    hrx << m_cltHelper.SetCollectionString(SIMON_PEP::SZ_PROPERTY_OWNER_SEED, m_sOwnerSeed);
				}

				hrx << m_cltHelper.QueryPolicy(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);

                hrx << m_cltHelper.GetLicenseState(*pState);

                GetLicenseInfo(TRUE);
                *pState = m_State;
			}
			else if(mode == DJSMAR00_QueryMode_SetServiceToAutomatic)
			{
                m_cltHelper.ClearCollection();
//                pepBase.SetContextGuid(clt::pep::CLTGUID);
//                pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
//                pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
                hrx << m_cltHelper.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_AUTO_START_LICENSE);
                hrx << m_cltHelper.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

                if(!m_sOwnerSeed.IsEmpty())
                {
                   hrx << m_cltHelper.SetCollectionString(SIMON_PEP::SZ_PROPERTY_OWNER_SEED, m_sOwnerSeed);
                }

                hrx << m_cltHelper.QueryPolicy(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
			}
			else
			{
				GetLicenseInfo();
				*pState = m_State;
			}

		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::GetLicenseStateEx -> exception while query license date 0x%08X"), hr);
	}

	return hr;
}

STDMETHODIMP CNAVLicenseInfo::get_ProductBrandingId(LONG* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_ProductBrandingId -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		// Pull the UniqueOEM ID from the Branding.ini file

		CNAVInfo NAVInfo;
		TCHAR szFileName[_MAX_PATH * 2] = {0};
		wsprintf(szFileName, _T("%s\\BRANDING.INI"), NAVInfo.GetNAVDir());

		UINT uOEMId = ::GetPrivateProfileInt(OEMDATFILE_Integrator, OEMDATFILE_UniqueOEMID, 0, szFileName);

		*pVal = (long) uOEMId;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

STDMETHODIMP CNAVLicenseInfo::get_ShouldCheckSubscription(BOOL* pbVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_ShouldCheckSubscription -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		*pbVal = TRUE;

		GetLicenseInfo();

		if(m_State != DJSMAR00_LicenseState_PURCHASED)
		{
			*pbVal = FALSE;
		}
		else 
		{
			switch(m_Type)
			{
			case DJSMAR_LicenseType_Retail:
			case DJSMAR_LicenseType_Trial:
			case DJSMAR_LicenseType_Beta:
			case DJSMAR_LicenseType_Unlicensed:
			case DJSMAR_LicenseType_ESD:
            case DJSMAR_LicenseType_OEM:
				{
					// OK to check subscription.
				}
				break;

			case DJSMAR_LicenseType_Rental:
			case DJSMAR_LicenseType_TryDie:
			case DJSMAR_LicenseType_Violated:
			default:
				{
					*pbVal = FALSE;
				}
				break;

			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

STDMETHODIMP CNAVLicenseInfo::get_CanRenewSubscription(BOOL* pbVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_CanRenewSubscription -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pbVal) 
		{
			*pbVal = false;

			hrx << GetSubscriptionInfo();

			*pbVal = m_bSubCanRenew;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::get_CanRenewSubscription -> Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}


HWND GetAbsoluteParent( HWND &hWnd )
{
    HWND hWndParent = NULL, 
        hWndTemp = hWnd;

    while ( ::IsWindow(hWndTemp) )
    {
        hWndParent = hWndTemp;
        hWndTemp = ::GetParent( hWndParent );
    }

    return hWndParent;
}
STDMETHODIMP CNAVLicenseInfo::get_CancelCode(LONG* pVal)
{
	if(forceError(ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::get_CancelCode -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(pVal)
	{
		*pVal = 0;
		GetLicenseInfo();
		*pVal = (long) m_dwCancelCode;
	}

	return S_OK;
}
STDMETHODIMP CNAVLicenseInfo::get_SubscriptionExpirationDate(BSTR* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionExpirationDate -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = false;

			hrx << GetSubscriptionInfo();

			COleDateTime date((DATE) m_dwSubEndDate);
			CComBSTR bstrEndDate(date.Format(VAR_DATEVALUEONLY));
			*pVal = bstrEndDate.Detach();
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionExpirationDate -> Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}

STDMETHODIMP CNAVLicenseInfo::get_SubscriptionRemaining(LONG* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionRemaining -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = false;

			hrx << GetSubscriptionInfo();

			*pVal = (long) m_dwSubRemainingDays;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionRemaining -> Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}

STDMETHODIMP CNAVLicenseInfo::get_SubscriptionLength(LONG* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionLength -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = false;

			hrx << GetSubscriptionInfo();

			*pVal = (long) m_dwSubLength;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionLength -> Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}

HRESULT CNAVLicenseInfo::GetSubscriptionInfo()
{
	HRESULT hr = S_OK;

	if(m_bSubRefreshed)
	{
		STAHLSOFT_HRX_TRY(hr)
		{
			// Getting Subscription disposition
			DWORD dwSubDisposition = 0;
            hrx << m_cltHelper.GetSubscriptionRemaining((long&)m_dwSubRemainingDays);
            hrx << m_cltHelper.GetSubscriptionLength((long&)m_dwSubLength);
            hrx << m_cltHelper.GetSubscriptionEndDate((DATE&)m_dwSubEndDate);
            hrx << m_cltHelper.CanRenewSubscription(m_bSubCanRenew);
            hrx << m_cltHelper.GetSubscriptionExpired((bool&)m_bSubExpired);
            hrx << m_cltHelper.GetSubscriptionWarningPeriod((bool&)m_bSubWarning);
            hrx << m_cltHelper.CanAutoRenewSubscription((bool&)m_bSubCanAutoRenew);
			hrx << m_cltHelper.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION, (DWORD &)dwSubDisposition, SUBSCRIPTION_STATIC::DISPOSITION_NONE);

			m_bSubAutoRenewReady = DWORD_HAS_FLAG(dwSubDisposition,SUBSCRIPTION_STATIC::DISPOSITION_SUBSCRIPTION_AUTO_RENEW_READY);

			m_bSubRefreshed = false;

			CCTRACEI(CCTRCTX _T("SubLength=%d"), m_dwSubLength);
			CCTRACEI(CCTRCTX _T("DaysRemaining=%d"), m_dwSubRemainingDays);
			CCTRACEI(CCTRCTX _T("EndDate=%d"), m_dwSubEndDate);
			CCTRACEI(CCTRCTX _T("Disposition=%d"), dwSubDisposition);
			CCTRACEI(CCTRCTX _T("AutoRenew=%d"), m_bSubCanAutoRenew);
			CCTRACEI(CCTRCTX _T("AutoRenewReady=%d"), m_bSubAutoRenewReady);
		}
		catch(_com_error& e)
		{
			hr = e.Error();
			CCTRACEE(CCTRCTX _T("Exception 0x%08X"), hr);
		}
	}

	return hr;
}

STDMETHODIMP CNAVLicenseInfo::put_OwnerSeed(BSTR newVal)
{
    CRegKey key;
    CString csKey( DRMREG::SZ_SUITE_OWNER_KEY );
    csKey = csKey  + "\\" + V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;

    BasicDRMMiddleMan::CPreferedOwnerEnum<NULL, NULL> poe(csKey, LPCTSTR(_bstr_t(newVal)));
    poe.GetOwnerGUID(0, m_sOwnerSeed);

    CCTRACEI(_T("CLicenseWrapper::SetOwnerSeed(%s)"), m_sOwnerSeed);

	return S_OK;
}

STDMETHODIMP CNAVLicenseInfo::LaunchSubscriptionWizard(long hWndParent)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::LaunchSubscriptionWizard -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

        // If we're informed so, find the currently active window
        // We do not use NULL because many places that don't want modal
        // windows use NULL.
        if(hWndParent == (long)INVALID_HANDLE_VALUE)
        {
            // Get active window attached to this thread
            if((hWndParent = (long)GetActiveWindow()) == NULL)
            {   // No active window on this thread, use global foreground wnd.
                hWndParent = (long)GetForegroundWindow();
                // If this returns null, we have no recourse but to be nonmodal
            }
        }

		hrx << GetSubscriptionInfo();

        // Launch renewal wizard
        m_cltHelper.ClearCollection();
//        pepBase.SetContextGuid(clt::pep::CLTGUID);
//        pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
//        pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_SUBSCRIPTION_FORCE_RENEWAL);

		if(m_bSubCanAutoRenew)
		{
			hrx << m_cltHelper.SetCollectionDWORD(
                SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_RENEWAL_MODE,
				SUBSCRIPTION_STATIC::RENEWAL_WIZARD_MODE_AUTORENEWAL);
		}

        hrx << m_cltHelper.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,
                                              clt::pep::DISPOSITION_NONE);
        hrx << m_cltHelper.SetCollectionDWORD(
                    SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,
                    SUBSCRIPTION_STATIC::DISPOSITION_NONE);

        // Fix for defect 1-4FX4ZP
        hrx << m_cltHelper.SetCollectionDWORD(SIMON_PEP::SZ_PARENT_HWND, (DWORD)hWndParent);

        if(!m_sOwnerSeed.IsEmpty())
        {
           hrx << m_cltHelper.SetCollectionString(SIMON_PEP::SZ_PROPERTY_OWNER_SEED, m_sOwnerSeed);
        }

        hrx << m_cltHelper.QueryPolicy(clt::pep::POLICY_ID_QUERY_SUBSCRIPTION_FORCE_RENEWAL);

		// Set flag to refresh subscription data upon next call to GetSubscriptionInfo()
		CCTRACEI(_T("CNAVLicenseInfo::LaunchSubscriptionWizard - Set flag to refresh subscription data upon next call to GetSubscriptionInfo()"));
		m_bSubRefreshed = true;
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::LaunchSubscriptionWizard -> Failed to launch subscription wizard 0x%08X"), hr);
		makeError(ERR_LAUNCH_SUBSCRIPTION_WIZARD, E_FAIL);
	}

	return hr;
}


STDMETHODIMP CNAVLicenseInfo::get_VendorName(BSTR* pVal)
{
	if(forceError(ERR_SECURITY_FAILED))
	{
		CCTRACEE(_T("CNAVLicenseInfo::get_VendorName -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
		return E_ACCESSDENIED;
	}

	if(pVal)
	{
		// Get vendor name from license data collection.
		GetLicenseInfo();

		CComBSTR bstrVendorName(m_csVendorName);
		*pVal = bstrVendorName.Detach();
	}

	return S_OK;
}

STDMETHODIMP CNAVLicenseInfo::get_SubscriptionDateDW(DWORD* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionDateDW -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = false;

			hrx << GetSubscriptionInfo();

			*pVal = m_dwSubEndDate;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionDateDW -> Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}
STDMETHODIMP CNAVLicenseInfo::get_SubscriptionExpired(BOOL* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionExpired -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = false;

			hrx << GetSubscriptionInfo();

			*pVal = m_bSubExpired;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionExpired -> Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}

STDMETHODIMP CNAVLicenseInfo::get_SubscriptionWarning(BOOL* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionWarning -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = false;

			hrx << GetSubscriptionInfo();

			*pVal = m_bSubWarning;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::get_SubscriptionWarning -> Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}

//************************************************************************
// IsCfgWizFinished()
//
// BOOL IsCfgWizFinished(BOOL& bFinished)
//
// This reads the value "CFGWIZ:Finished" in cfgwiz.dat to determine
// if CfgWiz.exe has been run.
//
// Returns:
//			TRUE if successful reading the value.
//			FALSE otherwise.
//			bFinished = TRUE if CFGWIZ:Finished != 0
//			bFinished = FALSE otherwise.
//
//************************************************************************

STDMETHODIMP CNAVLicenseInfo::get_CfgWizFinished(BOOL* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(_T("CNAVLicenseInfo::get_CfgWizFinished -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = FALSE;

		    // Load the CfgWiz.dat.

		    CString csCfgwizDat;

		    if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgwizDat))
            {
                // Check to see if CfGwiz is finished or if we're OEM.
	            DWORD dwCfgWizFinished = 0;
	            DWORD dwOEM = 0;
	            CNAVOptFileEx cfgWizOpt;
	            hrx << (cfgWizOpt.Init(csCfgwizDat, FALSE)? S_OK : E_FAIL);
	            cfgWizOpt.GetValue(InstallToolBox::CFGWIZ_Finished, dwCfgWizFinished, 1);
	            //cfgWizOpt.GetValue(InstallToolBox::CFGWIZ_OEM, dwOEM, 0);

	            if(dwCfgWizFinished == 0)
	            {
		            // CfgWiz is not finished, launch cfgwiz if we're CTO or rental.
		            //if((dwOEM == 2) || (m_Type == DJSMAR_LicenseType_Rental))
			        //    bLaunchCfgWiz = true;
	            }

			    hrx << GetSubscriptionInfo();

			    *pVal = dwCfgWizFinished;
            }
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNAVLicenseInfo::get_CfgWizFinished -> Failed to get cfgwiz data"));
		makeError(ERR_QUERY_CFGWIZ_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}

STDMETHODIMP CNAVLicenseInfo::get_HasUserAgreedToEULA(BOOL* pVal)
{
    *pVal = FALSE;

    CString csCfgwizDat;

    if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgwizDat))
    {
        CNAVOptFileEx cfgWizFile;
        if(cfgWizFile.Init(csCfgwizDat, FALSE))
        {
            DWORD dwAgreedEULA = 0;
            DWORD dwOEM = 0;

            cfgWizFile.GetValue(InstallToolBox::CFGWIZ_OEM, dwOEM, 0);
            if(dwOEM == 1 || dwOEM == 2)
            {
                // Check to see if OEM and CTO user 
                //  has already accepted EULA during cfgwiz.
                cfgWizFile.GetValue(InstallToolBox::CFGWIZ_AgreedToEULA, dwAgreedEULA, 0);
                if(dwAgreedEULA)
                    *pVal = TRUE;
            }
            else
            {
                // Retail/SCSS users already accepted EULA during install.
                *pVal = TRUE;
            }

            CCTRACEI(_T("CNAVLicenseInfo::get_HasUserAgreedToEULA - OEM=%d, Agreed=%d"), dwOEM, dwAgreedEULA);
            return S_OK;
        }
    }

    return E_FAIL;
}

STDMETHODIMP CNAVLicenseInfo::get_SubscriptionCanAutoRenew(BOOL* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(CCTRCTX _T("Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = FALSE;
			hrx << GetSubscriptionInfo();
			*pVal = m_bSubCanAutoRenew;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(CCTRCTX _T("Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}

STDMETHODIMP CNAVLicenseInfo::get_SubscriptionAutoRenewReady(BOOL* pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(forceError(ERR_SECURITY_FAILED))
		{
			CCTRACEE(CCTRCTX _T("Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED);
			return E_ACCESSDENIED;
		}

		if(pVal) 
		{
			*pVal = FALSE;
			hrx << GetSubscriptionInfo();
			*pVal = m_bSubAutoRenewReady;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(CCTRCTX _T("Failed to get subscription data"));
		makeError(ERR_QUERY_SUBSCRIPTION_FAILED, E_FAIL);
		return E_FAIL;
	}

	return hr;
}

