////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// IEContextMenuHelper.h : Declaration of the CIEContextMenuHelper

#pragma once
#include "resource.h"       // main symbols
#include "SymInterface.h"
#include "NavShellExtHelper_h.h"

// CIEContextMenuHelper

class ATL_NO_VTABLE CIEContextMenuHelper : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CIEContextMenuHelper, &CLSID_IEContextMenuHelper>,
	public IIEContextMenuHelper
{
public:
	CIEContextMenuHelper()
	{
	}
	DECLARE_CLASSFACTORY_SINGLETON(CIEContextMenuHelper)

DECLARE_REGISTRY_RESOURCEID(IDR_IECONTEXTMENUHELPER)


BEGIN_COM_MAP(CIEContextMenuHelper)
	COM_INTERFACE_ENTRY(IIEContextMenuHelper)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}	
};


class CHelper
{

protected:
	virtual HRESULT _Error(int iId, const IID& cGuid) = 0;
	virtual HRESULT _Error(CString csError, const IID& cGuid) = 0;

	HRESULT	ErrorFromResource(int iId, const IID& guid, CString& csError);
	bool CanShowUI();
	HRESULT LaunchElement(SYMGUID element);
	void	GetResourceString(UINT uiIDs,CString &csResource);

	CString m_csProductName;
};

OBJECT_ENTRY_AUTO(__uuidof(IEContextMenuHelper), CIEContextMenuHelper)
