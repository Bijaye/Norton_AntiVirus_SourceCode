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
#include "stdafx.h"

#define _WIN32_DCOM
#include "SettingsInterfaceMgr.h"
#include "ccPwdUtil.h"
#include "NAVPwdExp.h"
#include "CryptReg.h"
#include <time.h>
#include "resource.h"
#include <Rpcdce.h>
#include <comdef.h>
//#include "ccPwdSvc_h.h"
//#include "ccPwdSvc_i.c"

using namespace ccSettings;

HRESULT HashPassword(TCHAR *pszPwd, BYTE *bHash, DWORD &dwSize)
{
	HRESULT hr = S_OK;	

    // Validate Parameters
    if( (NULL == pszPwd) || (bHash == NULL))
    {
        hr = E_FAIL;
    }
    else
    {
        DWORD losize(dwSize);
        SHA_INFO sha_info;
        sha_init(&sha_info);
        sha_update(&sha_info, (BYTE*)pszPwd, losize);
        unsigned char sha_Digest[SHA_DIGESTSIZE];
        sha_final(sha_Digest, &sha_info);
        std::strstream output;
        HexStringInserter<unsigned char> inserter(output);
        std::for_each(sha_Digest,sha_Digest+sizeof(sha_Digest)/sizeof(*sha_Digest),inserter);
        output << std::ends;
        memcpy(bHash, (BYTE*)output.str(), strlen(output.str()));
        dwSize = strlen(output.str());
        hr = S_OK;				
    }

	bHash[dwSize] = '\0';

	return(hr);
}

HRESULT HashTime(BYTE *bHash, DWORD &dwSize)
{
	HRESULT hr = S_OK;
	TCHAR szTime[9] = {0};
	
    // Validate Parameters
    if(bHash == NULL)
    {
        hr = E_FAIL;
    }
    else
    {
	    _tstrtime(szTime);

	    DWORD losize(dwSize);
	    SHA_INFO sha_info;
	    sha_init(&sha_info);
	    sha_update(&sha_info, (BYTE*)szTime, losize);
	    unsigned char sha_Digest[SHA_DIGESTSIZE];
	    sha_final(sha_Digest, &sha_info);
	    std::strstream output;
	    HexStringInserter<unsigned char> inserter(output);
	    std::for_each(sha_Digest,sha_Digest+sizeof(sha_Digest)/sizeof(*sha_Digest),inserter);
	    output << std::ends;
	    memcpy(bHash, (BYTE*)output.str(), strlen(output.str()));
	    dwSize = strlen(output.str());
	    hr = S_OK;				
    }

	return(hr);
}

HRESULT GenerateRandomString(TCHAR szString[11])
{
	HRESULT hr = S_OK;
	
	TCHAR szB[] = "abcdefghijklmnopqrstuvwxyz0123456789/+";
	DWORD dwArraySize = lstrlen( szB );

	BYTE bHash[80] = {0};
	DWORD dwSize = 80;
	DWORD dwOrigSize = 80;

	// Generate 80 bytes worth of random characters
	hr = HashTime(bHash, dwSize);
	hr = HashTime(bHash + dwSize, dwOrigSize);

	for(int i = 0; i < 10; i++)
	{
		BYTE *p = bHash+(i*8);
		DWORD dw = 0;

		for(int j = 0; j < 8; j++)
		{
			int t = 0;

			if((*(p+j) >= '0') && (*(p+j) <= '9'))
			{
				t = (*(p+j)) - '0';
			}
			else
				t = 10 + ((*(p+j)) - 'a');

			dw |= (t << (4*j));
		}
		szString[i] = szB[ dw % dwArraySize ];
	}

	szString[10] = '\0';

	return hr;
}

HMODULE GetMyModule()
{
	HMODULE hMod = NULL;
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	HRESULT hr = S_OK;
	
	if( 0 == VirtualQuery(GetMyModule, &mbi, sizeof(mbi)) )
		hr = CCPW_FAIL;

	if(!FAILED(hr))
	{
		hMod = (HINSTANCE)mbi.AllocationBase;

		if(NULL == hMod)
			hr = CCPW_FAIL;
	}

	return hMod;
}

HRESULT LoadPassword(long lProductID, LPCTSTR szUserName, TCHAR *pszPwd, DWORD dwLen)
{    
    CSettingsInterfaceMgr itfMgr;

	SYMRESULT sr;

	if((NULL == szUserName) || (_T('\0') == szUserName[0]))
	{
		TCHAR szPasswordPath[MAX_PATH + 1] = {0};
		DWORD dwSize = MAX_PATH;

		GetProductPasswordPath(lProductID, szPasswordPath, dwSize);

	    CSymPtr<ISettings> pSet = itfMgr.getSettingsForPath(szPasswordPath);

		if(NULL == pSet.m_p)
			return E_FAIL;

		sr = pSet->GetString(PASSWORD_VAL, pszPwd, dwLen);
	}
	else
	{
		CSymPtr<ISettings> pSet = itfMgr.getSettingsForUser(szUserName);

		if(NULL == pSet.m_p)
	        return E_FAIL;

		TCHAR szName[21] = {0};
		DWORD dwSize = 20;
		_ltot(lProductID, szName, dwSize);

	    sr = pSet->GetString(szName, pszPwd, dwLen);

	}
    
	if ((sr != SYM_OK) || (_tcslen(pszPwd) == 0))
		return E_FAIL;
	else
		return S_OK;
}

HRESULT SavePassword(long lProductID, LPCTSTR szUserName, TCHAR *pszNewPwd)
{   
    CSettingsInterfaceMgr itfMgr;

	SYMRESULT sr;
	HRESULT hr;

	if((NULL == szUserName) || (_T('\0') == szUserName[0]))
	{
        TCHAR szPasswordPath[MAX_PATH + 1] = {0};
        DWORD dwSize = MAX_PATH;

        GetProductPasswordPath(lProductID, szPasswordPath, dwSize);

        CSymPtr<ISettings> pSet = itfMgr.getSettingsForPath(szPasswordPath);
	    
        if(NULL == pSet.m_p)
            return E_FAIL;
        sr = pSet->PutString(PASSWORD_VAL, pszNewPwd);

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

        SYMRESULT sr = pSet->PutString(szName, pszNewPwd);

        hr = ((sr == SYM_OK) ? S_OK : E_FAIL);
    
        if(SUCCEEDED(hr))
            hr = itfMgr.putSettingsForUser(szUserName, pSet);
    }

    return hr;
}

BOOL GetProductPasswordPath(long lProductID, TCHAR *pszPath, DWORD dwLen)
{
	TCHAR szProduct[21] = {0};
	DWORD dwSize = 20;
	_ltot(lProductID, szProduct, dwSize);

	// Check that we aren't too large here.
	if((_tcslen(szProduct) + _tcslen(KEY_PATH_FMT) + _tcslen(_T("\\"))) > dwLen)
		return FALSE;

	// Build the string.
    _stprintf(pszPath, _T("%s\\%s"), KEY_PATH_FMT, szProduct);

	return TRUE;
}

void DisplayError(HWND hWndParent, HRESULT hr)
{
	TCHAR szMsg[MAX_PATH+1] = {0};
	TCHAR szTitle[MAX_PATH+1] = {0};

	GetPasswordErrorString(hr, szMsg, MAX_PATH);
	LoadString(GetMyModule(), IDS_ERRORTITLE, szTitle, MAX_PATH);

	MessageBox(hWndParent, szMsg, szTitle, MB_OK | MB_ICONERROR);
}

void WipeBuffer(PBYTE pbBuf, DWORD dwLen)
{
	memset(pbBuf, 0xca, dwLen);
}

void SecureDelete(PBYTE pbBuf, DWORD dwLen)
{
	WipeBuffer(pbBuf, dwLen);

	delete pbBuf;
}

HRESULT ValidatePassword(LPCTSTR szPassword)
{
	HRESULT hr = S_OK;

	int nPwdLength = _tcslen(szPassword);
	if(nPwdLength < 8 || nPwdLength > 256)
		hr = CCPW_INVALID_LENGTH;

	return hr;	
}

void GetPasswordErrorString(HRESULT hr, LPTSTR szError, DWORD dwBufLen)
{
	long lResID = 0;
	switch(hr)
	{
	case CCPW_INVALID_LENGTH:
		lResID = IDS_INVALID_PWD_LENGTH;
		break;

	case CCPW_PASSWORD_MISMATCH:
		lResID = IDS_PWDMISMATCH;
		break;

	case CCPW_PASSWORD_INVALID:
		lResID = IDS_BADPWD;
		break;

	case CCPW_PASSWORD_INVALID_OLD:
		lResID = IDS_OLD_PWDMISMATCH;
		break;

	case CCPW_PASSWORD_INVALID_SAME:
		lResID = IDS_SAMEPWD;
		break;

	case CCPW_PASSWORD_INVALID_KEY:
		lResID = IDS_INVALID_KEY;
		break;
	}

	if(0 != lResID)
	{
		LoadString(GetMyModule(), lResID, szError, dwBufLen);
	}
}

/*
BOOL VerifyWindowsAccountCreds(LPCTSTR szUser, LPCTSTR szPassword)
{
	HRESULT hr = S_OK;
	VARIANT_BOOL bSuccess = FALSE;
	MULTI_QI mqi[1];

	if(FAILED(CoInitialize(NULL)))
    {
        return FALSE;
    }

	try
	{
		mqi[0].hr = S_OK;
		mqi[0].pIID = &IID_IWinPwdSvc;
		mqi[0].pItf = NULL;

		hr = CoCreateInstanceEx(CLSID_WinPwdSvc, NULL, CLSCTX_LOCAL_SERVER, NULL, 1, mqi);

		if(SUCCEEDED(hr) && SUCCEEDED(mqi[0].hr) && mqi[0].pItf)
		{
			IWinPwdSvc *pI = (IWinPwdSvc*)mqi[0].pItf;

			CoSetProxyBlanket((IUnknown*)pI, RPC_C_AUTHN_DEFAULT, RPC_C_AUTHZ_NONE, NULL, 
							  RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0);

			hr = pI->VerifyPassword(_bstr_t(szUser), _bstr_t(szPassword), &bSuccess);

			pI->Release();
		}
	}
	catch(_com_error&)
	{
		bSuccess = FALSE;
	}

	CoUninitialize();

	return bSuccess;
}*/