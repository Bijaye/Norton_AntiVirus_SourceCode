// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// Password.cpp : Implementation of CCcPasswdApp and DLL registration.

#include "stdafx.h"
#define INITIIDS
#include "SettingsInterfaceMgr.h"
#include "NAVPwd_h.h"
#include "NAVPwdExp.h"
#include "Password.h"
#include "SetDlg.h"
#include "ReSetDlg.h"
#include "SecureReSetDlg.h"
#include "CheckDlg.h"
#include "ccPwdUtil.h"
#include "ccVerifyTrustLoader.h"
#include "ccVerifyTrustInterface.h"

using namespace ccSettings;

// Use ccVerifyTrust to check the loader.
bool VerifyLoader(void)
{
	static bool bVerified = false;

	if (!bVerified)
	{
		ccVerifyTrust::ccVerifyTrust_IVerifyTrust TrustLoader;
		ccVerifyTrust::CVerifyTrustPtr pVerifyTrust;

		if(SYM_SUCCEEDED(TrustLoader.CreateObject(pVerifyTrust.m_p)) && (pVerifyTrust != NULL))
		{
			if(pVerifyTrust->Create(true) == ccVerifyTrust::eNoError)
			{
				if (pVerifyTrust->VerifyCurrentProcess(ccVerifyTrust::eSymantecSignature) 
					== ccVerifyTrust::eVerifyError)
					return false; 
			}

			pVerifyTrust.Release();
		}

		bVerified = true;
	}

	return bVerified;
}


STDMETHODIMP Password::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IPassword,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

Password::Password()
{
	try
	{
		m_lProductID = 0;
		m_bsFeatureName = "";
		m_bsUserName = "";
//		m_bUseWindowsAccounts = FALSE;
	}
	catch(_com_error&)
	{
	}
}


STDMETHODIMP Password::put_ProductID(long newVal)
{
	m_lProductID = newVal;

	return S_OK;
}

STDMETHODIMP Password::put_FeatureName(BSTR newVal)
{
	try
	{
		m_bsFeatureName = newVal;
	}
	catch(_com_error&)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP Password::Set(BOOL bPromptForOldPwd, BOOL *bSuccess)
{
	HRESULT hr = S_OK;

	hr = ccpw_SetPassword(m_lProductID, (TCHAR*)m_bsProductName, (TCHAR*)m_bsFeatureName, (TCHAR*)m_bsUserName, bPromptForOldPwd);
	*bSuccess = SUCCEEDED(hr);

	return CheckReturnCode(hr);
}

STDMETHODIMP Password::Check(BOOL *bSuccess)
{	
	HRESULT hr = S_OK;

	hr = ccpw_CheckPassword(m_lProductID, (TCHAR*)m_bsProductName, (TCHAR*)m_bsFeatureName, (TCHAR*)m_bsUserName);
	*bSuccess = SUCCEEDED(hr);

	return CheckReturnCode(hr);
}

STDMETHODIMP Password::Reset(BOOL *bSuccess)
{
	HRESULT hr = S_OK;

	hr = ccpw_ResetPassword(m_lProductID, (TCHAR*)m_bsProductName, (TCHAR*)m_bsFeatureName, (TCHAR*)m_bsUserName);
	*bSuccess = SUCCEEDED(hr);

	return CheckReturnCode(hr);
}

STDMETHODIMP Password::Clear(BOOL *bSuccess)
{
	HRESULT hr = S_OK;

	hr = ccpw_ClearPassword(m_lProductID, (TCHAR*)m_bsProductName, (TCHAR*)m_bsFeatureName, (TCHAR*)m_bsUserName);
	*bSuccess = SUCCEEDED(hr);

	return CheckReturnCode(hr);
}

STDMETHODIMP Password::AddUser(BSTR bsPassword, BOOL *bSuccess)
{
	HRESULT hr = S_OK;
	try
	{
		_bstr_t bstrPwd = bsPassword;

		hr = ccpw_AddUserAndPassword(m_lProductID, (TCHAR*)m_bsUserName, (TCHAR*)bstrPwd);
		*bSuccess = SUCCEEDED(hr);
	}
	catch(_com_error&)
	{
		hr = E_FAIL;
	}

	return CheckReturnCode(hr);
}

STDMETHODIMP Password::get_UserName(BSTR *pVal)
{
	try
	{
		*pVal = m_bsUserName.copy();
	}
	catch(_com_error&)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP Password::put_UserName(BSTR newVal)
{
	try
	{
		m_bsUserName = newVal;
	}
	catch(_com_error&)
	{
		return E_FAIL;
	}

	return S_OK;
}

/*STDMETHODIMP Password::get_UseWindowsAccounts(BOOL *pVal)
{
	*pVal = m_bUseWindowsAccounts;

	return S_OK;
}

STDMETHODIMP Password::put_UseWindowsAccounts(BOOL newVal)
{
	m_bUseWindowsAccounts = newVal;

	return S_OK;
}*/

STDMETHODIMP Password::get_ProductName(BSTR *pVal)
{
	try
	{
		*pVal = m_bsProductName.copy();
	}
	catch(_com_error&)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP Password::put_ProductName(BSTR newVal)
{
	try
	{
		m_bsProductName = newVal;
	}
	catch(_com_error&)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP Password::get_Enabled(BOOL *pVal)
{
	*pVal = ccpw_IsPasswordCheckEnabled(m_lProductID);

	return S_OK;
}

STDMETHODIMP Password::put_Enabled(BOOL newVal)
{
	return ccpw_SetPasswordCheckEnabled(m_lProductID, newVal);
}

STDMETHODIMP Password::get_UserPasswordSet(BOOL *pVal)
{
	*pVal = ccpw_IsUserPasswordSet(m_lProductID, (TCHAR*)m_bsUserName);

	return S_OK;
}


CCPASSWD_API HRESULT ccpw_SetPassword(long lProductID, LPCTSTR szProductName, LPCTSTR szFeatureName, LPCTSTR szUserName, BOOL bPromptForOldPwd)
{
	HRESULT hr = S_OK;
	TCHAR pszNewPwd[MAX_PATH+1] = {0};
	BOOL bCommit = FALSE;

	if (!VerifyLoader())
		return CCPW_FAIL;

	if(bPromptForOldPwd)
	{
		CReSetDlg dlg;
		TCHAR pszOldPwd[MAX_PATH+1] = {0};

		hr = LoadPassword(lProductID, szUserName, pszOldPwd, MAX_PATH);

		dlg.SetProductName((TCHAR*)szProductName);
		dlg.SetFeatureName((TCHAR*)szFeatureName);
		dlg.SetOldPwd(pszOldPwd);

		if(ID_SET_PWD == dlg.DoModal())
		{
			dlg.GetNewPwd(pszNewPwd, MAX_PATH);
			bCommit = TRUE;
		}
		else
			hr = CCPW_USER_CANCEL;
	}
	else
	{
		CSetDlg dlg;

		dlg.SetProductName((TCHAR*)szProductName);
		dlg.SetFeatureName((TCHAR*)szFeatureName);

		if(ID_SET_PWD == dlg.DoModal())
		{
			dlg.GetNewPwd(pszNewPwd, MAX_PATH);
			bCommit = TRUE;
		}
		else
			hr = CCPW_USER_CANCEL;
	}

	if(bCommit)
	{
		hr = SavePassword(lProductID, szUserName, pszNewPwd);
		WipeBuffer((PBYTE)pszNewPwd, (MAX_PATH+1)*sizeof(TCHAR));
	}

	return hr;
}

CCPASSWD_API HRESULT ccpw_CheckPassword(long lProductID, LPCTSTR szProductName, LPCTSTR szFeatureName, LPCTSTR szUserName)
{
	HRESULT hr = S_OK;
	TCHAR pszOldPwd[MAX_PATH+1] = {0};

	if (!VerifyLoader())
		return CCPW_FAIL;

    hr = LoadPassword(lProductID, szUserName, pszOldPwd, MAX_PATH);

	CCheckDlg dlg;

	dlg.SetProductName((TCHAR*)szProductName);
	dlg.SetFeatureName((TCHAR*)szFeatureName);
	dlg.SetCompPwd(pszOldPwd);
	dlg.SetUserName((TCHAR*)szUserName);

	WipeBuffer((PBYTE)pszOldPwd, (MAX_PATH+1)*sizeof(TCHAR));

	if(ID_SET_PWD != dlg.DoModal())
	{
		hr = CCPW_USER_CANCEL;
	}

	return hr;
}

CCPASSWD_API HRESULT ccpw_CheckPasswordSilent(long lProductID, LPCTSTR szUserName, LPCTSTR szPassword)
{
	HRESULT hr = S_OK;
	TCHAR pszOldPwd[MAX_PATH+1] = {0};
	TCHAR szHashPwd[MAX_PATH+1] = {0};

	if (!VerifyLoader())
		return CCPW_FAIL;

    hr = LoadPassword(lProductID, szUserName, pszOldPwd, MAX_PATH);

	DWORD dwSize = MAX_PATH;
	
    dwSize = _tcslen(szPassword);
    hr = HashPassword((TCHAR*)szPassword, (BYTE*)szHashPwd, dwSize);

    if(!FAILED(hr))
    {
        if(_tcscmp(szHashPwd, pszOldPwd) != 0)
        {
            hr = E_FAIL;
        }
    }	

	WipeBuffer((PBYTE)pszOldPwd, (MAX_PATH+1)*sizeof(TCHAR));
	WipeBuffer((PBYTE)szHashPwd, (MAX_PATH+1)*sizeof(TCHAR));

	return hr;
}


CCPASSWD_API HRESULT ccpw_ResetPassword(long lProductID, LPCTSTR szProductName, LPCTSTR szFeatureName, LPCTSTR szUserName)
{
	HRESULT hr = S_OK;
	TCHAR pszNewPwd[MAX_PATH+1] = {0};
	BOOL bCommit = FALSE;

	if (!VerifyLoader())
		return CCPW_FAIL;

	CSecureReSetDlg dlg;

	dlg.SetProductName((TCHAR*)szProductName);
	dlg.SetFeatureName((TCHAR*)szFeatureName);

	if(ID_SET_PWD == dlg.DoModal())
	{
		dlg.GetNewPwd(pszNewPwd, MAX_PATH);
		bCommit = TRUE;
	}
	else
		hr = CCPW_USER_CANCEL;

	if(bCommit)
	{
		hr = SavePassword(lProductID, szUserName, pszNewPwd);
		WipeBuffer((PBYTE)pszNewPwd, (MAX_PATH+1)*sizeof(TCHAR));
	}

	return hr;
}


CCPASSWD_API HRESULT ccpw_ClearPassword(long lProductID, LPCTSTR szProductName, LPCTSTR szFeatureName, LPCTSTR szUserName)
{
	HRESULT hr = S_OK;

	if (!VerifyLoader())
		return CCPW_FAIL;

    CSettingsInterfaceMgr itfMgr;

	if((NULL == szUserName) || (_T('\0') == szUserName[0]))
	{
	    TCHAR szPasswordPath[MAX_PATH + 1] = {0};
	    DWORD dwSize = MAX_PATH;
        GetProductPasswordPath(lProductID, szPasswordPath, dwSize);

		CSymPtr<ISettings> pSet = itfMgr.getSettingsForPath(szPasswordPath);

		// Clear the enabled value first.
		SYMRESULT sr = pSet->DeleteValue(ENABLED_VAL);
		if(sr == SYM_OK) // Then clear the password value.
			sr = pSet->DeleteValue(PASSWORD_VAL);
		
		hr = ((sr == SYM_OK) ? S_OK : E_FAIL);
    
		if(SUCCEEDED(hr))
			hr = itfMgr.putSettingsNoUser(pSet);
	}
	else
	{
		CSymPtr<ISettings> pSet = itfMgr.getSettingsForUser(szUserName);

		if(NULL == pSet.m_p)
			return E_FAIL;

		TCHAR szName[21] = {0};
		DWORD dwSize = 20;
		_ltot(lProductID, szName, dwSize);

		SYMRESULT sr = pSet->DeleteValue(szName);
		hr = ((sr == SYM_OK) ? S_OK : E_FAIL);
	    
		if(SUCCEEDED(hr))
			hr = itfMgr.putSettingsForUser(szUserName, pSet);
	}

    return hr;
}

CCPASSWD_API HRESULT ccpw_AddUserAndPassword(long lProductID, LPCTSTR szUserName, LPCTSTR szPassword)
{
	ASSERT(szUserName && szPassword);
	if (!szUserName || !szPassword) return E_INVALIDARG;

	HRESULT hr = S_OK;
	TCHAR szHashed[MAX_PATH+1] = {0};
	DWORD dwSize = _tcslen(szPassword);

	if(dwSize == 0)
	{
		// Instead of entering this, let's clear the password.
		TCHAR szProd[] = {0};    // This value is ignored by the ccpw_ClearPassword method.
		TCHAR szFeature[] = {0}; // This value is ignored by the ccpw_ClearPassword method.
		return ccpw_ClearPassword(lProductID, szProd, szFeature, szUserName);
	}

	if (!VerifyLoader())
		return CCPW_FAIL;

	hr = HashPassword((TCHAR*)szPassword, (BYTE*)szHashed, dwSize);

	if(!FAILED(hr))
		hr = SavePassword(lProductID, szUserName, szHashed);

	WipeBuffer((PBYTE)szHashed, (MAX_PATH+1)*sizeof(TCHAR));

	return hr;
}

CCPASSWD_API BOOL ccpw_IsPasswordCheckEnabled(long lProductID)
{
	BOOL bRet = FALSE;
	cc::ccSet_ISettingsManager loader;

	ISettingsManagerPtr pMgr;
    loader.CreateObject(&pMgr);

    // Append the Product ID to the path.
    TCHAR szPasswordPath[MAX_PATH + 1] = {0};
    DWORD dwSize = MAX_PATH;
    
    GetProductPasswordPath(lProductID, szPasswordPath, dwSize);

    CSymPtr<ISettings> pSet;
    pMgr->GetSettings(szPasswordPath, &pSet);

    if(NULL == pSet.m_p)
        return bRet;

    pSet->GetDword(ENABLED_VAL, (DWORD&)bRet);

	return bRet;
}

CCPASSWD_API HRESULT ccpw_SetPasswordCheckEnabled(long lProductID, BOOL bEnabled)
{
	cc::ccSet_ISettingsManager loader;

	ISettingsManagerPtr pMgr;
    loader.CreateObject(&pMgr);

    CSymPtr<ISettings> pSet;

    TCHAR szPasswordPath[MAX_PATH + 1] = {0};
    DWORD dwSize = MAX_PATH;
    GetProductPasswordPath(lProductID, szPasswordPath, dwSize);

    SYMRESULT sr = pMgr->GetSettings(szPasswordPath, &pSet);

	// If CommonClient\Passwords\[PRODUCTID] doesn't exist, create it.
	if (sr == SYM_SETTINGS_NOT_FOUND)
		pMgr->CreateSettings(szPasswordPath, &pSet);

    if(NULL == pSet.m_p)
        return CCPW_REG_ERROR;

    sr = pSet->PutDword(ENABLED_VAL, bEnabled);

    if(SYM_OK == sr)
        sr = pMgr->PutSettings(pSet);

    return ((SYM_OK == sr) ? S_OK : E_FAIL);
}

CCPASSWD_API BOOL ccpw_IsUserPasswordSet(long lProductID, LPCTSTR szUserName)
{
	BOOL bRet = FALSE;
	TCHAR szPwd[MAX_PATH+1] = {0};

	bRet = SUCCEEDED(LoadPassword(lProductID, szUserName, szPwd, MAX_PATH));

	WipeBuffer((PBYTE)szPwd, (MAX_PATH+1)*sizeof(TCHAR));

	return bRet;
}

