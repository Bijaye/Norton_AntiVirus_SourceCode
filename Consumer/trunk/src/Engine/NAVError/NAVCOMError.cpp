////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVCOMError.cpp : Implementation of CNAVCOMError
#include "stdafx.h"

#include "isErrorLoader.h"

// SymInterface stuff. Goes before you use any SymInterface's.
//
#include "SymInterface.h"
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

#include "NAVCOMError.h"

#include "NAVSettingsHelperEx.h"

using namespace std;


/////////////////////////////////////////////////////////////////////////////
// CNAVCOMError

CNAVCOMError::CNAVCOMError()
{
    init();
}

CNAVCOMError::~CNAVCOMError()
{
}

STDMETHODIMP CNAVCOMError::get_Message(BSTR *pVal)
{
    _bstr_t bstrTemp ( m_strMessage );
    *pVal = bstrTemp.copy ();
	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_Message(BSTR newVal)
{
    try 
    {
        _bstr_t bstrTemp = newVal;
	    m_strMessage = (LPCSTR) bstrTemp;
        m_bAlreadyLogged = false;
	    return S_OK;
    }
    catch ( _com_error e )
    {
        return e.Error ();
    }
}

STDMETHODIMP CNAVCOMError::get_ErrorID(long *pVal)
{
	*pVal = m_lErrorID;
    return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ErrorID(long newVal)
{
	m_lErrorID = newVal;
    m_bAlreadyLogged = false;

	return S_OK;
}

STDMETHODIMP CNAVCOMError::get_ModuleID(long *pVal)
{
	*pVal = m_lModuleID;

	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ModuleID(long newVal)
{
	m_lModuleID = newVal;
    m_bAlreadyLogged = false;

	return S_OK;
}

STDMETHODIMP CNAVCOMError::get_HResult(long *pVal)
{
	*pVal = m_lErrorResult;

	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_HResult(long newVal)
{
	m_lErrorResult = newVal;
    m_bAlreadyLogged = false;

	return S_OK;
}

HRESULT CNAVCOMError::Log()
{
    HRESULT hr = Show(FALSE, TRUE, NULL);
    init ();
    return hr;
}


HRESULT CNAVCOMError::Display(long hwnd)
{
    HRESULT hr = Show(TRUE, FALSE, hwnd);
    init ();
    return hr;
}

STDMETHODIMP CNAVCOMError::LogAndDisplay(long hwnd)
{
    // Display and return result from the display
    HRESULT hr = Show(TRUE, TRUE, hwnd);
    init ();
    return hr;
}

STDMETHODIMP CNAVCOMError::Show(BOOL bDisplay, BOOL bLog, long hWnd)
{
	HRESULT hr = E_FAIL;

    ISShared::ISShared_IError errorLoader;
    ISShared::IErrorPtr error;
    if ( SYM_SUCCEEDED (errorLoader.CreateObject (GETMODULEMGR(), &error)))
    {
        hr = error->Show ( m_lModuleID, 
                      m_lErrorID,
                      m_lErrorResID,    // custom resource
                      m_strCaption,
                      m_strMessage,
                      m_lErrorResult,
                      bDisplay?true:false, // only show the error dialog if we can show UI
                      bLog?true:false,
                      (HANDLE)hWnd );
    }

    init ();

    return hr;
}

void CNAVCOMError::init()
{
    m_lErrorResult = E_FAIL;
    m_lErrorID = 0;
    m_lModuleID = 0;
    m_bAlreadyLogged = false;
	m_lForcedModuleID = 0;
	m_lForcedErrorID = 0;
	m_lErrorResID = 0;

	CRegKey rk;

	if (rk.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\Norton AntiVirus"), KEY_READ) == ERROR_SUCCESS)
	{
        DWORD dwValue = 0;

		if (rk.QueryDWORDValue(_T("Forced Error"), dwValue) == ERROR_SUCCESS && dwValue == 1)
		{
			CNAVOptSettingsEx NavOpts;
			if ( NavOpts.Init() )
			{
				if ( SUCCEEDED(NavOpts.GetValue(ERROR_ForcedModuleID, dwValue, 0)) )
				{
			        m_lForcedModuleID = dwValue;

					if ( SUCCEEDED(NavOpts.GetValue(ERROR_ForcedErrorID, dwValue, 0)) )
						m_lForcedErrorID = dwValue;
				}
			}
		}
	}

	CCTRACEI(_T("ForcedModuleID = %d, ForcedErrorID = %d"), m_lForcedModuleID, m_lForcedErrorID);
}

STDMETHODIMP CNAVCOMError::get_ForcedErrorID(long *pVal)
{
	*pVal = m_lForcedErrorID;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ForcedErrorID(long newVal)
{
	m_lForcedErrorID = newVal;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::get_ForcedModuleID(long *pVal)
{
	*pVal = m_lForcedModuleID;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ForcedModuleID(long newVal)
{
    m_lForcedErrorID = newVal;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::get_ErrorResourceID(long* pVal)
{
	*pVal = m_lErrorResID;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ErrorResourceID(long newVal)
{
	m_lErrorResID = newVal;
	return S_OK;
}
