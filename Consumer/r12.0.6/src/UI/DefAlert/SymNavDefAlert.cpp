// SymNavDefAlert.cpp : Implementation of CSymNavDefAlert
#include "stdafx.h"
#include "simon.h"
#include "navtrust.h"
#include "DefAlert.h"
#include "SymNavDefAlert.h"
#include "NavSettingsHelperEx.h"
#include "ISWSCHelper_Loader.h"
#include "WSCHelper.h"
#include "AVccModuleId.h"
#include "NAVErrorResource.h"
#include "AVRESBranding.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepClientBase.h"

#include "NAVSecurity.h"
#include ".\symnavdefalert.h"

#include "..\defalertres\Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSymNavDefAlert
/////////////////////////////////////////////////////////////////////////////

CSymNavDefAlert::CSymNavDefAlert()
{
    CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

    // Check NAVError module for Symantec Signature...
    if( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer(_T("NAVError.NAVCOMError")) )
    {
        if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
                                                  NULL,
                                                  CLSCTX_INPROC_SERVER)))
        {
        }
    }
}

STDMETHODIMP CSymNavDefAlert::LaunchLiveUpdate()
{
	HRESULT hr = S_OK;

	// Ensure script security.
	if(forceError(ERR_SECURITY_FAILED) || !IsItSafe())
	{
		CCTRACEE(_T("SymNavDefAlert::LaunchLiveUpdate -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
		return E_ACCESSDENIED;
	}

	try
	{
		// Get path to c:\\program files\\symantec\\LiveUpdate\\LUALL.EXE
		CRegKey key;
		TCHAR szLuall[MAX_PATH] = {0};
		DWORD dwLength = sizeof(szLuall)/sizeof(TCHAR);

		if(forceError(ERR_OPEN_LU_KEY) || (ERROR_SUCCESS != key.Open(HKEY_LOCAL_MACHINE,_T("software\\symantec\\SharedUsage"), KEY_QUERY_VALUE)))
		{
			CCTRACEE(_T("SymNavDefAlert::LaunchLiveUpdate -> Unable to open SharedUsage key."));
			makeError(ERR_OPEN_LU_KEY, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_FAIL;
		}

		if(forceError(ERR_READ_LU_KEY) || (ERROR_SUCCESS != key.QueryStringValue(_T("LiveUpdate"), szLuall, &dwLength)))
		{
			CCTRACEE(_T("SymNavDefAlert::LaunchLiveUpdate -> Unable to read LU key"));
			makeError(ERR_READ_LU_KEY, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_FAIL;
		}

		// Get the second to last char in the path.
		TCHAR* pszDirNull = ::_tcsrchr(szLuall, '\0');
		TCHAR* pszLastSlash = ::CharPrev(szLuall, pszDirNull);

		// If the ending character is a slash, null it.
		if(0 == ::_tcscmp(pszLastSlash, "\\"))
			*pszLastSlash = '\0';

		// Append LUALL.exe to the path
		::_tcscat(szLuall, _T("\\LUALL.EXE"));

		// Check LUALL.exe for a valid Symantec signature before launching it
		hr = NAVToolbox::IsSymantecSignedImage(szLuall);
		if(forceError(ERR_LU_SECURITY_FAILED) || FAILED(hr))
		{
			CCTRACEE(_T("SymNavDefAlert::LaunchLiveUpdate -> LU Not safe"));
			makeError(ERR_LU_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_ACCESSDENIED;
		}

		BOOL bSuccess = FALSE;
		STARTUPINFO startupInfo = {0};
		PROCESS_INFORMATION processInfo = {0};
		startupInfo.cb = sizeof(startupInfo);

		bSuccess = ::CreateProcess(NULL, szLuall, NULL, NULL, FALSE, 
									NORMAL_PRIORITY_CLASS, NULL, NULL, 
									&startupInfo, &processInfo);
		if(forceError(ERR_FAILED_LAUNCH_LU) || !bSuccess)
		{
			CCTRACEE(_T("SymNavDefAlert::LaunchLiveUpdate -> LU does not exit."));
			makeError(ERR_FAILED_LAUNCH_LU, E_FAIL, IDS_NAVERROR_NAVLNCH_NO_LU);
			return E_FAIL;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}

	return hr;
}


STDMETHODIMP CSymNavDefAlert::SetSubExpiredCycle(long dwDays)
{
	// Make sure that it is used by an authorized Symantec application

	if(forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		CCTRACEE(_T("CSymNavDefAlert::SetSubExpiredCycle -> Not safe"));
		makeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
	}

	// Save value of "Notify me in x days".
	_SetAlertDword(ALERT_SubsExpiredCycle, dwDays);

	return S_OK;
}


STDMETHODIMP CSymNavDefAlert::SetDefsOutOfDateCycle(long dwDays)
{
	// Make sure that it is used by an authorized Symantec application
	if(forceError(ERR_SECURITY_FAILED) || !IsItSafe())
	{
		CCTRACEE(_T("CSymNavDefAlert::SetDefsOutOfDateCycle -> Not safe"));
		makeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
	}

	// Save value of "Notify me in x days".
	_SetAlertDword(ALERT_DefsOutOfDateCycle, dwDays);

	return S_OK;
}


STDMETHODIMP CSymNavDefAlert::get_SubExpiredOptionEnabled(BOOL *pVal)
{
	if (forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		CCTRACEE(_T("CSymNavDefAlert::get_SubExpiredOptionEnabled -> Not safe"));
		makeError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
	}
	
	// Always show 15 days option for expired alert.
	//  Request from Laura Garcia-Manrique
	*pVal = TRUE;

	return S_OK;
}

STDMETHODIMP CSymNavDefAlert::get_NAVError(INAVCOMError* *pVal)
{
	if(!m_spError)
        return E_FAIL;

    *pVal = m_spError;
    (*pVal)->AddRef(); // We get a ref and the caller gets one

	return S_OK;
}

void CSymNavDefAlert::makeError(long lMessageID, long lHResult, long lNAVErrorResID)
{
    if(!m_spError)
        return;

    m_spError->put_ModuleID ( AV_MODULE_ID_DEF_ALERT );
    m_spError->put_ErrorID ( lMessageID );
    m_spError->put_ErrorResourceID ( lNAVErrorResID );
    m_spError->put_HResult ( lHResult );
}

bool CSymNavDefAlert::forceError(long lErrorID)
{
    if(!m_spError )
        return false;

    long lTempErrorID = 0;
    long lTempModuleID = 0;

    m_spError->get_ForcedModuleID ( &lTempModuleID );
    m_spError->get_ForcedErrorID ( &lTempErrorID );

    if ( lTempModuleID == AV_MODULE_ID_DEF_ALERT &&
         lTempErrorID == lErrorID )
         return true;
    else
        return false;
}

STDMETHODIMP CSymNavDefAlert::SetAlertDword(BSTR bstrName, LONG iValue)
{
	// Make sure that it is used by an authorized Symantec application
	if (forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		CCTRACEE(_T("CSymNavDefAlert::SetDwordValue -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_ACCESSDENIED;
	}

	// Save value of "Notify me in x days".
	USES_CONVERSION;
	_SetAlertDword(OLE2CA(bstrName), iValue);

	return S_OK;
}

STDMETHODIMP CSymNavDefAlert::GetAlertDword(BSTR bstrName, LONG iDefaultValue, LONG* iValue)
{
	// Make sure that it is used by an authorized Symantec application

	if(forceError (ERR_SECURITY_FAILED) || !IsItSafe())
	{
		CCTRACEE(_T("CSymNavDefAlert::GetDwordValue -> Not safe"));
		makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_ACCESSDENIED;
	}

	// Save value of "Notify me in x days" in alert.dat file.

	USES_CONVERSION;
	_GetAlertDword(OLE2CA(bstrName), iDefaultValue);

	return S_OK;
}

STDMETHODIMP CSymNavDefAlert::LaunchURLFromBrandingResource(LONG iResId)
 {
	StahlSoft::HRX hrx;
	HRESULT hr = S_OK;

	try
	{
		CNAVInfo navInfo;
		TCHAR szCmd[MAX_PATH] = {0};
		STARTUPINFO startupInfo = {0};
		PROCESS_INFORMATION processInfo = {0};

		::wsprintf(szCmd, _T("%s\\SMNLnch.exe -dll %s\\NAVUI.DLL -func _FetchURL@8 -hint %d"), 
		navInfo.GetSymantecCommonDir(),
		navInfo.GetNAVDir(),
		iResId);

		startupInfo.cb = sizeof(startupInfo);
		::CreateProcess(NULL, szCmd, NULL, NULL, FALSE, 
		NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInfo);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}

	return hr;
 }

 STDMETHODIMP CSymNavDefAlert::GetStringFromBrandingResource(LONG iResId, BOOL bRemoveAmpersand, BSTR* pbstrBuffer)
 {
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		if(forceError(ERR_SECURITY_FAILED) || !IsItSafe())
		{
			CCTRACEE(_T("CSymNavDefAlert::GetResourceString -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_ACCESSDENIED;
		}

		// Load the string from AVRES.
		CBrandingRes BrandRes;
		CString csBuffer(BrandRes.GetString(iResId));
		csBuffer.Remove(_T('&'));

		CComBSTR bstrBuffer(csBuffer);
		*pbstrBuffer = bstrBuffer.Detach();
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}

	return hr;
 }

 
STDMETHODIMP CSymNavDefAlert::GetVirusDefSubDaysLeft(LONG* pnDaysLeft)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		if(forceError(ERR_SECURITY_FAILED) || !IsItSafe())
		{
			CCTRACEE(_T("CSymNavDefAlert::GetVirusDefSubDaysLeft -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_ACCESSDENIED;
		}

		if(pnDaysLeft)
		{
            *pnDaysLeft = 0;
            STAHLSOFT_HRX_TRY(hr)
            {
                // Getting licensing and subscription properties needed to enable product features
                CPEPClientBase pepBase;
                hrx << pepBase.Init();
                pepBase.SetContextGuid(clt::pep::CLTGUID);
                pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
                pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_SUBSCRIPTION_RECORD);
                pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
                pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

                hrx << pepBase.QueryPolicy();

                // Getting subscription days remaining
                DWORD dwDaysRemainaing = 0;
                hrx << pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING, (DWORD &)dwDaysRemainaing, 0);

                *pnDaysLeft = dwDaysRemainaing;
            }
            STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}

	return hr;
 }

STDMETHODIMP CSymNavDefAlert::IsHighContrast(BOOL *pRet)
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		if(forceError(ERR_SECURITY_FAILED) || !IsItSafe())
		{
			CCTRACEE(_T("CSymNavDefAlert::IsHighContrast -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_ACCESSDENIED;
		}

		if(!pRet) 
		{
			return ERROR_INVALID_PARAMETER;
		}
			
		HIGHCONTRAST hc; memset( &hc, 0, sizeof(hc) ); 
		hc.cbSize = sizeof(HIGHCONTRAST);
		SystemParametersInfo( SPI_GETHIGHCONTRAST
							,sizeof(HIGHCONTRAST)
							,&hc
							, SPIF_SENDWININICHANGE );
		if( hc.dwFlags & HCF_HIGHCONTRASTON )
		{
			(*pRet)=TRUE;
		}
		else
		{
			(*pRet)=FALSE;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}

	return hr;
}

STDMETHODIMP CSymNavDefAlert::get_VirusDefsThreatCount(LONG* pVal)
{
	try
	{
		if(forceError(ERR_SECURITY_FAILED) || !IsItSafe())
		{
			CCTRACEE(_T("CSymNavDefAlert::get_VirusDefsThreatCount -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_ACCESSDENIED;
		}

		if(!pVal) 
		{
			return ERROR_INVALID_PARAMETER;
		}
			
		*pVal = 0;

		DWORD dwCount = 0;
		READ_SETTING_START()
			READ_SETTING_DWORD(VIRUSDEFS_ThreatCount, dwCount, 1)
		READ_SETTING_END

		*pVal = dwCount;

		CCTRACEI(_T("VIRUSDEFS_ThreatCount=%d"), dwCount);
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CSymNavDefAlert::get_VirusDefsThreatCount - Error while accessing settings %d"), e.Error());
	}

	return S_OK;
}

STDMETHODIMP CSymNavDefAlert::get_ShowSSC(VARIANT_BOOL* pVal)
{
	try
	{
		if(forceError(ERR_SECURITY_FAILED) || !IsItSafe())
		{
			CCTRACEE(_T("CSymNavDefAlert::get_ShowSSC -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_ACCESSDENIED;
		}

		// Check if we should show WSC check box
        ISShared::WSCHelper_Loader WSCLoader;
		CSymPtr<IWSCHelper> pWSCHelper;
		*pVal = VARIANT_FALSE;
		
		if (SYM_SUCCEEDED(WSCLoader.CreateObject(&pWSCHelper)) && !pWSCHelper->IsPreXPSP2())
			*pVal = VARIANT_TRUE;
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CSymNavDefAlert::get_ShowSSC - Error while accessing settings %d"), e.Error());
	}

	return S_OK;
}

// pVal indicates if the disable broadcast checkbox is checked
STDMETHODIMP CSymNavDefAlert::WriteToWMI(VARIANT_BOOL pVal)
{
	try
	{
		if(forceError(ERR_SECURITY_FAILED) || !IsItSafe())
		{
			CCTRACEE(_T("CSymNavDefAlert::get_ShowSSC -> Not safe"));
			makeError(ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
			return E_ACCESSDENIED;
		}

		ISShared::WSCHelper_Loader WSCLoader;
		CSymPtr<IWSCHelper> pWSCHelper;

		if (SYM_SUCCEEDED(WSCLoader.CreateObject(&pWSCHelper)))
		{
			pWSCHelper->SetDisableBroadcast(VARIANT_TRUE == pVal);
			pWSCHelper->SetAVOverride(VARIANT_TRUE == pVal);
			pWSCHelper->UpdateSymantecAVStatus((LPGUID)&IID_ProductID_NAV_AV, 
				                                eState_Disabled, 
				                                eState_Unchanged);
		}
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CSymNavDefAlert::get_ShowSSC - Error while accessing settings %d"), e.Error());
	}

	return S_OK;
}
