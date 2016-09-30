////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// IEContextMenuHelper.cpp : Implementation of CIEContextMenuHelper

#include "StdAfx.h"
#define INITIIDS
#include <initguid.h>
#include "ScanTaskLoader.h"

#include "IEContextMenuHelper.h"
#include "InstOptsNames.h"
#include "NavSettingsHelperEx.h"
#include "ISComponentFrameworkGuids.h"
#include "SuiteOwnerHelper.h"
#include "NAVOptHelperEx.h"

// Element stuff for product info
#include "uiNISDataElementGuids.h"
#include "uiProviderInterface.h"
#include "uiElementInterface.h"
#include "uiNumberDataInterface.h"
#include "uiDateDataInterface.h"
#include "ISDataClientLoader.h"

ui::IProviderPtr g_pProvider;

// Returns formatted error string & HRESULT mapping with this error string
HRESULT	CHelper::ErrorFromResource(int iId, const IID& guid, CString& csError)
{
	GetResourceString(iId,csError);
	HRESULT hr = _Error(csError,guid);
	if(SUCCEEDED(hr))
		CCTRCTXI2(_T("ErrorFromResource(%d): %s"),iId, csError);
	else
		CCTRCTXE3(_T("ErrorFromResource(%d): returning hr(0x%08X): %s"),iId, hr, csError);

	return hr;
}

void CHelper::GetResourceString(UINT uiIDs, CString &csResource)
{
	CString csFormat;
	csFormat.LoadString(_AtlBaseModule.GetResourceInstance(), uiIDs);
	if( -1 != csFormat.Find(_T("%s")))
	{
		csResource.Format(csFormat, m_csProductName);
	}
	else
	{
		csResource = csFormat;
	}
}


HRESULT CHelper::LaunchElement(SYMGUID element)
{
	ui::IElementPtr pElement;
	HRESULT hr = g_pProvider->GetElement(element, pElement);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Failed GetElement() = 0x%08x"), hr);
		return hr;
	}

	hr = pElement->Configure(::GetDesktopWindow(), NULL);
	if(SUCCEEDED(hr))
	{
		CCTRCTXI0(_T("Success - Configured element"));
	}
	else
	{
		CCTRCTXE1(_T("Failed - Configure element 0x%08x"), hr);
	}

	return hr;
}

bool CHelper::CanShowUI()
{
    // Check business rules for displaying UI
    ISShared::ISShared_IProvider providerFactory;
    SYMRESULT sr = providerFactory.CreateObject(GETMODULEMGR(), &g_pProvider);
    if(SYM_FAILED(sr))
    {
        CCTRCTXE1(_T("Failed to create provider = 0%x80"), sr);
        return false;
    }
    
    ui::IElementPtr pElementCanShow;
	HRESULT hRet = g_pProvider->GetElement(ISShared::CLSID_NIS_CanShowUI, pElementCanShow);
	if(FAILED(hRet))
	{
		CCTRCTXE1(_T("Failed IProvider::GetElement() 0x%08x"), hRet);
		return false;
	}

	ui::IDataPtr pData;
	hRet = pElementCanShow->GetData(pData);
	if(FAILED(hRet))
	{
		CCTRCTXE1(_T("Failed ui::IElement::GetData() 0x%08x"), hRet);
		return false;
	}

	ui::INumberDataQIPtr pDataCanShow = pData;
	LONGLONG i64State = 0;
	hRet = pDataCanShow->GetNumber(i64State);
	if(FAILED(hRet))
	{
		CCTRCTXE1(_T("Failed ui::INumberDataQI::GetNumber() 0x%08x"), hRet);
		return false;
	}

	if(0 == i64State)
	{
		CCTRCTXI0(_T("User has not accepted EULA."));
		return false;
	}

    return true;
}
