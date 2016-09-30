// CNavExtBho.h : Declaration of the CCNavExtBho

#ifndef __CNAVEXTBHO_H_
#define __CNAVEXTBHO_H_

#include "resource.h"       // main symbols
#include "NAVShellExt.h"

/////////////////////////////////////////////////////////////////////////////
// CCNavExtBho
class ATL_NO_VTABLE CCNavExtBho : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCNavExtBho, &CLSID_CNavExtBho>,
	public IObjectWithSiteImpl<CCNavExtBho>
{
public:
	CCNavExtBho()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SHELLBHO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCNavExtBho)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

// ICNavExtBho
public:

	// IObjectWithSite overides.
	STDMETHOD(SetSite)( IUnknown* pUnkSite );

};

#endif //__CNAVEXTBHO_H_
