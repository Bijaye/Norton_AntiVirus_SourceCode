////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// SymNavDefAlert.cpp : Implementation of CSymNavDefAlert

#include "stdafx.h"
#include "DefAlert.h"
#include "SymNavDefAlert.h"
#include "CLTProductHelper.h"
#include "NavSettingsHelperEx.h"
#include "AVccModuleId.h"
#include "ISVersion.h"
#include "DefHelper.h"
#include "NAVSecurity.h"
#include "ISDataClientLoader.h"
#include "uiNumberDataInterface.h"
#include "..\defalertres\Resource.h"
#include "SymHelpLauncher.h"
#include "ccOSInfo.h"

using namespace ui;
using namespace ISShared;

/////////////////////////////////////////////////////////////////////////////
// CSymNavDefAlert
/////////////////////////////////////////////////////////////////////////////

CSymNavDefAlert::CSymNavDefAlert()
{
	CComBSTR bstrErrorClass	("NAVError.NAVCOMError");

	// Check NAVError module for Symantec Signature...
	if(NAVToolbox::NAVTRUST_OK	== NAVToolbox::IsSymantecComServer("NAVError.NAVCOMError"))
	{
		if (FAILED(m_spError.CoCreateInstance(bstrErrorClass,
												  NULL,
												  CLSCTX_INPROC_SERVER)))
		{
			CCTRCTXI0(L"Create NavError failed");
		}
	}
}

STDMETHODIMP CSymNavDefAlert::LaunchLiveUpdate()
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


STDMETHODIMP CSymNavDefAlert::SetSubExpiredCycle(long dwDays)
{
	// Save	value of "Notify me	in x days".
	_SetAlertDword(ALERT_SubsExpiredCycle, dwDays);
	return S_OK;
}


STDMETHODIMP CSymNavDefAlert::SetDefsOutOfDateCycle(long dwDays)
{
	// Save	value of "Notify me	in x days".
	_SetAlertDword(ALERT_DefsOutOfDateCycle, dwDays);
	return S_OK;
}


STDMETHODIMP CSymNavDefAlert::get_SubExpiredOptionEnabled(BOOL *pVal)
{
	// Always show 15 days option for expired alert.
	//	Request	from Laura Garcia-Manrique
	*pVal =	TRUE;
	return S_OK;
}

STDMETHODIMP CSymNavDefAlert::get_NAVError(INAVCOMError* *pVal)
{
	if(!m_spError)
		return E_FAIL;

	*pVal =	m_spError;
	(*pVal)->AddRef(); // We get a ref and the caller gets one

	return S_OK;
}

void CSymNavDefAlert::makeError(long lMessageID, long lHResult,	long lNAVErrorResID)
{
	if(!m_spError)
		return;

	m_spError->put_ModuleID	( AV_MODULE_ID_DEF_ALERT );
	m_spError->put_ErrorID ( lMessageID	);
	m_spError->put_ErrorResourceID ( lNAVErrorResID	);
	m_spError->put_HResult ( lHResult );
}

bool CSymNavDefAlert::forceError(long lErrorID)
{
	if(!m_spError )
		return false;

	long lTempErrorID =	0;
	long lTempModuleID = 0;

	m_spError->get_ForcedModuleID (	&lTempModuleID );
	m_spError->get_ForcedErrorID ( &lTempErrorID );

	if ( lTempModuleID == AV_MODULE_ID_DEF_ALERT &&
		 lTempErrorID == lErrorID )
		 return	true;
	else
		return false;
}

STDMETHODIMP CSymNavDefAlert::SetAlertDword(BSTR bstrName, LONG	iValue)
{
	// Save	value of "Notify me	in x days".
	USES_CONVERSION;
	_SetAlertDword(OLE2CA(bstrName), iValue);

	return S_OK;
}

STDMETHODIMP CSymNavDefAlert::GetAlertDword(BSTR bstrName, LONG	iDefaultValue, LONG* iValue)
{
	// Save	value of "Notify me	in x days" in alert.dat	file.

	USES_CONVERSION;
	_GetAlertDword(OLE2CA(bstrName), iDefaultValue);

	return S_OK;
}

STDMETHODIMP CSymNavDefAlert::LaunchURLFromBrandingResource(LONG iResId)
 {
	StahlSoft::HRX hrx;
	HRESULT	hr = S_OK;

	try
	{
		CNAVInfo navInfo;
		TCHAR szCmd[MAX_PATH] =	{0};
		STARTUPINFO	startupInfo	= {0};
		PROCESS_INFORMATION	processInfo	= {0};

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
	HRESULT	hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		// Load	the	string from	AVRES.
		ccLib::CString csBuffer;
		if(CISVersion::LoadString(csBuffer, iResId))
		{
			CComBSTR bstrBuffer(csBuffer);
			*pbstrBuffer = bstrBuffer.Detach();
		}

	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}

	return hr;
 }

 
STDMETHODIMP CSymNavDefAlert::GetVirusDefSubDaysLeft(ULONG*	pnDaysLeft)
{
	HRESULT	hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		if(pnDaysLeft)
		{
			*pnDaysLeft	= 0;
			STAHLSOFT_HRX_TRY(hr)
			{
				CCLTProductHelper cltHelper;
				hrx	<< cltHelper.Initialize();

				unsigned long lDaysRemainaing =	0;
				hrx	<< cltHelper.GetDaysRemaining(lDaysRemainaing);

				*pnDaysLeft	= lDaysRemainaing;
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
	HRESULT	hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		if(!pRet) 
		{
			return ERROR_INVALID_PARAMETER;
		}
			
		HIGHCONTRAST hc; memset( &hc, 0, sizeof(hc)	); 
		hc.cbSize =	sizeof(HIGHCONTRAST);
		SystemParametersInfo( SPI_GETHIGHCONTRAST
							,sizeof(HIGHCONTRAST)
							,&hc
							, SPIF_SENDWININICHANGE	);
		if(	hc.dwFlags & HCF_HIGHCONTRASTON	)
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
		if(!pVal) 
		{
			return ERROR_INVALID_PARAMETER;
		}
			
		*pVal =	0;

		DWORD dwCount =	0;
		READ_SETTING_START()
			READ_SETTING_DWORD(VIRUSDEFS_ThreatCount, dwCount, 1)
		READ_SETTING_END

		*pVal =	dwCount;

		CCTRCTXI1(L"VIRUSDEFS_ThreatCount=%d", dwCount);
	}
	catch(_com_error& e)
	{
		CCTRCTXE1(L"Error while	accessing settings %d",	e.Error());
	}

	return S_OK;
}

STDMETHODIMP CSymNavDefAlert::get_ShowSSC(VARIANT_BOOL*	pVal)
{
	return E_NOTIMPL;
}

// pVal	indicates if the disable broadcast checkbox	is checked
STDMETHODIMP CSymNavDefAlert::WriteToWMI(VARIANT_BOOL pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CSymNavDefAlert::get_ALUStatus(LONG* pVal)
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

		hrx << spALUState->GetNumber(lState);
		*pVal = lState;

		CCTRCTXI1(L"ALU State: %d", lState);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}

STDMETHODIMP CSymNavDefAlert::get_DefinitionsDate(BSTR*	pVal)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (!pVal? E_INVALIDARG : S_OK);

		ccLib::CStringW	sDate;
		CDefHelper::GetDefDate(sDate);

		CComBSTR bstrDate(sDate);
		*pVal =	bstrDate.Detach();

		CCTRCTXI1(L"DefinitionsDate: %s", sDate);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}

STDMETHODIMP CSymNavDefAlert::LaunchHelp(long lHelpTopic)
{
    isshared::CHelpLauncher help;
    help.LaunchHelp (lHelpTopic);
    return S_OK;
}

STDMETHODIMP CSymNavDefAlert::get_OptionsChangeAllowed(VARIANT_BOOL* pVal)
{
	if(!pVal)
		return E_INVALIDARG;

	ccLib::COSInfo OSInfo;
	*pVal = VARIANT_FALSE;

	// Always return TRUE for Windows Vista because
	// admin rights UI will pop up automatically for all user accounts. 
	if(OSInfo.IsWinVista(true) || OSInfo.IsAdministrator())
	{
		*pVal = VARIANT_TRUE;
		CCTRCTXI0(L"Allow to change options");
	}
	else
	{
		CCTRCTXI0(L"NOT Allow to change options");
	}

	return S_OK;
}
