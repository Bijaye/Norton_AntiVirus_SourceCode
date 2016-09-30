// IEContextMenuHelper.h : Declaration of the CIEContextMenuHelper

#pragma once
#include "resource.h"       // main symbols

#include "NavShellExtHelper.h"

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

public:

};

OBJECT_ENTRY_AUTO(__uuidof(IEContextMenuHelper), CIEContextMenuHelper)
