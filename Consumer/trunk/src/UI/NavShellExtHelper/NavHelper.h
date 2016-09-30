////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NavHelper.h : Declaration of the CNavHelper

#pragma once

#include "NavShellExtHelper_h.h"
#include "ISVersion.h"
#include "IEContextMenuHelper.h"

// CNavHelper

typedef std::vector<TSTRING> VTSTR;
typedef VTSTR::iterator ITVTSTR;

class ATL_NO_VTABLE CNavHelper : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CNavHelper, &CLSID_NavHelper>,
	public INavHelper,
	public CHelper
{
protected:

	VTSTR m_vItems;
	VTSTR m_vDenials;

private:

	DWORD_PTR generateTempScanFile( LPTSTR szOut, int nBufferSize );
	bool ShowDenials(HWND hWnd);
	int Add2List(VARIANTARG vt,VTSTR &vStr);
public:
	CNavHelper()
	{
		m_csProductName = CISVersion::GetProductName();
	}
	HRESULT _Error(int iId, const IID& cGuid)
	{
		return Error(iId,cGuid);
	}
	HRESULT _Error(CString csError, const IID& cGuid)
	{
		return Error(csError,cGuid);
	}
	DECLARE_CLASSFACTORY_SINGLETON(CNavHelper)

DECLARE_REGISTRY_RESOURCEID(IDR_NAVHELPER)


BEGIN_COM_MAP(CNavHelper)
	COM_INTERFACE_ENTRY(INavHelper)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:
	STDMETHOD(RunNavW)(VARIANTARG,VARIANTARG);
};

OBJECT_ENTRY_AUTO(__uuidof(NavHelper), CNavHelper)
