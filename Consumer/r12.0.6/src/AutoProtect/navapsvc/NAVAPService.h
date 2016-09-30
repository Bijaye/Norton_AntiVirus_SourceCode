// NAVAPService.h : Declaration of the CNAVAPService

#ifndef __NAVAPSERVICE_H_
#define __NAVAPSERVICE_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CNAVAPService
class ATL_NO_VTABLE CNAVAPService : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CNAVAPService, &CLSID_NAVAPService>,
	public INAVAPService
{
public:
	CNAVAPService()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_NAVAPSERVICE)
DECLARE_NOT_AGGREGATABLE(CNAVAPService)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVAPService)
	COM_INTERFACE_ENTRY(INAVAPService)
END_COM_MAP()

// INAVAPService
public:
	STDMETHOD(UnRegisterHandler)(INAVAPEventHandler* pHandler);
	STDMETHOD(RegisterHandler)(INAVAPEventHandler* pHandler);
	STDMETHOD(GetAutoProtectEnabled)( BOOL * pbEnabled );
	STDMETHOD(SendCommand)( BSTR pCommand, VARIANT vArrayData );
};

#endif //__NAVAPSERVICE_H_
