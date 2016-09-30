////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVEventProvider.h : Declaration of the CNAVEventProvider

#ifndef __NAVEVENTPROVIDER_H_
#define __NAVEVENTPROVIDER_H_

#include "NAVEventResource.h"       // main symbols
#include "NAVEvents.h"

#include "ccProviderImpl.h"

_COM_SMARTPTR_TYPEDEF(INAVEventProvider, __uuidof(INAVEventProvider));

/////////////////////////////////////////////////////////////////////////////
// CNAVEventProvider
class ATL_NO_VTABLE CNAVEventProvider : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CNAVEventProvider, &CLSID_NAVEventProvider>,
	public IDispatchImpl<ccEvtMgr::IProviderImpl<INAVEventProvider>, &IID_INAVEventProvider, &LIBID_NAVEVENTSLib>
{
public:
	CNAVEventProvider(){}
	virtual ~CNAVEventProvider() {}

DECLARE_REGISTRY_RESOURCEID(IDR_NAVEVENTPROVIDER)
DECLARE_GET_CONTROLLING_UNKNOWN()
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVEventProvider)
	COM_INTERFACE_ENTRY(IProviderEx)
	COM_INTERFACE_ENTRY(INAVEventProvider)
    COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// INAVProvider
public:
    STDMETHOD(DisconnectProxy)()
    {
        Disconnect();
        return S_OK;
    }
};
/*
#if _ATL_VER >= 0x0700
    OBJECT_ENTRY_AUTO(__uuidof(NAVEventProvider), CNAVEventProvider)
#endif
*/
#endif //__NAVEVENTPROVIDER_H_
