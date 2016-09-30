// luIWPCallback.h : Declaration of the CluIWPCallback

#pragma once
#include "resource.h"       // main symbols
#import "PRODUCTREGCOM.TLB" raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#include "IWPLUCallback.h"


// CluIWPCallback

class ATL_NO_VTABLE CluIWPCallback : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CluIWPCallback, &CLSID_luIWPCallback>,
	public IluCallback
{
public:
	CluIWPCallback()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LUIWPCALLBACK)

BEGIN_COM_MAP(CluIWPCallback)
	COM_INTERFACE_ENTRY(IluCallback)
END_COM_MAP()


DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
	STDMETHOD(OnLUNotify)(tagLU_CALLBACK_TYPE nType, BSTR strMoniker);
};

OBJECT_ENTRY_AUTO(__uuidof(luIWPCallback), CluIWPCallback)
