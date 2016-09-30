// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "ILdvpDist.h"


#import "distinf.tlb" named_guids 
using namespace LdvpDist;


IDistribute	*pDistribute = NULL;


HRESULT GetInterfaces()
{
	HRESULT	hr;

	if (pDistribute)
		return S_OK;

	hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
		hr = CoCreateInstance(CLSID_Distribute, NULL, CLSCTX_INPROC_SERVER, IID_IDistribute, (LPVOID*)&pDistribute);

	return hr;
}


DWORD ReleaseInterfaces()
{
	HRESULT	hr;

	if (!pDistribute)
		return S_OK;

	hr = pDistribute->Release();
	CoUninitialize();

	return SUCCEEDED(hr);
}

HRESULT AddComputer(LPCTSTR sName, long dwFlags)
{
	GetInterfaces();

	HRESULT	hr;
	_bstr_t szName(sName);

	hr = pDistribute->AddComputer(szName, dwFlags);

	return hr;
}


HRESULT AddProduct(long nId, LPCTSTR sCmdFile, LPCTSTR sSrcPath, LPCTSTR sPatchFile, long dwFlags)
{
	GetInterfaces();

	HRESULT	hr;
	_bstr_t szCmd(sCmdFile),szSrcPath(sSrcPath),szPatchFile(sPatchFile);

	hr = pDistribute->AddProduct(nId, szCmd, szSrcPath, szPatchFile, dwFlags);
	return hr;
}


HRESULT AddProductVarString(LPCTSTR sVarName, LPCTSTR sVarData)
{
	GetInterfaces();

	HRESULT	hr;
	_bstr_t szVarName(sVarName),szVarData(sVarData);

	hr = pDistribute->AddProductVarString(szVarName, szVarData);
	return hr;
}

HRESULT AddProductVarValue(LPCTSTR sVarName, long dwVarData)
{
	GetInterfaces();

	HRESULT	hr;
	_bstr_t szVarName(sVarName);

	hr = pDistribute->AddProductVarValue(szVarName, dwVarData);
	return hr;
}


HRESULT RemoveProduct(long nId)
{
	GetInterfaces();

	HRESULT	hr;

	hr = pDistribute->RemoveProduct(nId);
	return hr;
}


HRESULT RemoveComputer(LPCTSTR sName)
{
	GetInterfaces();

	HRESULT	hr;
	_bstr_t szName(sName);

	hr = pDistribute->RemoveComputer(szName);

	return hr;
}


HRESULT UpdateComputers(long dwFlags, LPCTSTR sLangID)
{
	GetInterfaces();

	HRESULT	hr;
	_bstr_t szLangID(sLangID);

	hr = pDistribute->UpdateComputers(dwFlags,szLangID);
	return hr;
}


