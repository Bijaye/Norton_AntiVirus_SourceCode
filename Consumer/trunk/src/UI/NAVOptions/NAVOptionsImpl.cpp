////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVOptionsObj.cpp : Implementation of CNAVOptions
#include "StdAfx.h"
#include "GlobalEvent.h"
#include "GlobalEvents.h"

#include "EmailProxyInterface.h"  // For Email options
#include "NAVOptionsObj.h"
#include "Navtrust.h"

#include "NAVOptMigrate.h"
#include "..\navoptionsres\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions::CBuffer: a cleanup utility class
CNAVOptions::CSemaphore::CSemaphore(void) : m_hSemaphore(NULL)
{
	// Compose the semaphore name according to the OS
	if(g_OSInfo.IsTerminalServicesInstalled())
		::_tcscpy(m_szSemaphore, _T("Global\\"));
	else
		*m_szSemaphore = _T('\0');

	::_tcscat(m_szSemaphore, _T("Symantec.Norton.AntiVirus.Options"));
}

CNAVOptions::CSemaphore::~CSemaphore(void)
{
	if (m_hSemaphore)
		::CloseHandle(m_hSemaphore);
}

HRESULT CNAVOptions::CSemaphore::Create(void)
{
	// Create a new semaphore to be used by the options dialog.
	if (NULL == (m_hSemaphore = ::CreateSemaphore(NULL, 0, 1, m_szSemaphore)))
		// Q: Access is denied by another session?
		return ERROR_ACCESS_DENIED == ::GetLastError()
		    && g_OSInfo.IsTerminalServicesInstalled() ? S_EXIST
		                                              : CNAVOptions::Error(IDS_Err_Semaphore, _T("CSemaphore::Create()"));

	return ERROR_ALREADY_EXISTS == ::GetLastError() ? S_FALSE : S_OK;
}

bool CNAVOptions::CSemaphore::Open(void)
{
	return NULL != (m_hSemaphore = ::OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, m_szSemaphore));
}

DWORD CNAVOptions::CSemaphore::Accuire(void)
{
	// Wait 5 minutes to be safe before giving up on the dialog
	return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hSemaphore, 30000);
}

void CNAVOptions::CSemaphore::Release(void)
{
	(void)::ReleaseSemaphore(m_hSemaphore, 1, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions
HRESULT CNAVOptions::get__TypeOf(LPCWCH bstrOption, long *plType)
{
	if (forceError (ERR_INVALID_POINTER) ||
        !bstrOption || !plType)
    {
	    MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

	// Check if the DISPID exists
	OPTYPEMAP::iterator it = m_OptionsTypes.find(bstrOption);
	if (m_OptionsTypes.end() == it)
    {
		CCTRCTXW1(_T("[%s] not in options type map! Assuming DWORD."), bstrOption);
		*plType = ccSettings::SYM_SETTING_DWORD;
		m_OptionsTypes.insert(OPTYPEMAP::value_type(bstrOption, ccSettings::SYM_SETTING_DWORD));
    }
	else
	{
		*plType = it->second;
	}

	return S_OK;
}

HRESULT CNAVOptions::Default(const wchar_t* pcszDefault, DWORD& dwDefault)
{
	if (forceError (IDS_Err_Default) ||
        FAILED(m_pNavOpts->GetValue(pcszDefault, dwDefault, 0)) )
	{
		MakeError(IDS_Err_Default, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

	return S_OK;
}

void CNAVOptions::notify(void)
{
	CCTRACEI(_T("NAVOptions changed notification"));

	// Tell everyone that the options changes so they have a chance to reload the file
	CGlobalEvent ge;
	if (ge.Create(SYM_OPTIONS_CHANGED_EVENT))
	    ::PulseEvent(ge);

    m_EmailOptions.Notify();
    m_IMOptions.Notify();
    m_ThreatCatOptions.Notify();
    m_IWPOptions.Notify();
	m_bIsDirty = false;  // No longer dirty

	if(m_bPasswordSupported)
	{
		m_bIsPasswordDirty = false;
	}
}

PTCHAR CNAVOptions::url(void)
{
	static TCHAR szURL[MAX_PATH + 6 + 16] = _T("res://");

	if (!szURL[6])  // Initialize the first time
	{
		// Compose the URL
		if(::GetModuleFileName(g_ResModule, szURL + 6, MAX_PATH) == 0)
		{
			CCTRCTXW1(_T("Error calling GetModuleFileName. Returned %d."), GetLastError());
		}
		::_tcscat(szURL, _T("/options.htm"));
	}

	return szURL;
}

HRESULT CNAVOptions::Error(UINT uiIDS, PTCHAR pszLocation, bool bReturnFalse/*= true*/)
{
	HRESULT hr;

    CCTRACEE(_T("CNAVOptions::Error() - Received error ID = %d. Location =%s"), uiIDS, pszLocation);

	if (ShowScriptErrors())
		return CComCoClass<CNAVOptions, &CLSID_NAVOptions>::Error(uiIDS, IID_INAVOptions);
	else
		return FAILED(hr = ::Error(_T("CNAVOptions"), uiIDS, pszLocation)) || bReturnFalse ? hr : S_OK;
}

HRESULT CNAVOptions::Error(PTCHAR pszLocation, HRESULT hr, bool bReturnFalse/*= true*/)
{
    CCTRACEE(_T("CNAVOptions::Error() - Received error HRESULT = 0x%X. Location =%s"), hr, pszLocation);
	return FAILED(hr = ::Error(_T("CNAVOptions"), pszLocation, hr)) || bReturnFalse ? hr : S_OK;
}
