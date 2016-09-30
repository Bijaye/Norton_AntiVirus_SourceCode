// NavExtBho.h : Declaration of the CNavExtBho

#pragma once
#include "resource.h"       // main symbols

#include "NavShellExtEx.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error Single-threaded COM objects are not properly supported on Windows CE platforms that do not include full DCOM support.  Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support them anyway. You must also change the threading model in your rgs file from 'Apartment'/'Single' to 'Free'.
#endif


// CNavExtBho

class ATL_NO_VTABLE CNavExtBho  : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNavExtBho, &CLSID_CNavExtBho>,
	public IObjectWithSiteImpl<CNavExtBho>
{
public:
	CNavExtBho()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_NAVEXTBHO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNavExtBho)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

// ICNavExtBho
public:

	// IObjectWithSite overides.
	STDMETHOD(SetSite)( IUnknown* pUnkSite );

};
OBJECT_ENTRY_AUTO(__uuidof(CNavExtBho), CNavExtBho)
