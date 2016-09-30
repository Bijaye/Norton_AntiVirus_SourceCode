// HPPScriptObj.h : Declaration of the CHPPScriptObj

#pragma once
#include "resource.h"       // main symbols

#include "HomePageProtectionScriptableObj.h"

// CHPPScriptObj

class ATL_NO_VTABLE CHPPScriptObj : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHPPScriptObj, &CLSID_HPPScriptObj>,
    public IObjectSafetyImpl<CHPPScriptObj, INTERFACESAFE_FOR_UNTRUSTED_DATA | INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public IDispatchImpl<IHPPScriptObj, &IID_IHPPScriptObj, &LIBID_HomePageProtectionScriptableObjLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CHPPScriptObj()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_HPPSCRIPTOBJ)


BEGIN_COM_MAP(CHPPScriptObj)
	COM_INTERFACE_ENTRY(IHPPScriptObj)
	COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
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

    STDMETHOD(SetHomePage)(BSTR bszNewHomePage, VARIANT_BOOL bCurrentUser);
    STDMETHOD(ResetOptions)(void);
    STDMETHOD(PrepHPPForUninstall)(void);
    
};

OBJECT_ENTRY_AUTO(__uuidof(HPPScriptObj), CHPPScriptObj)
