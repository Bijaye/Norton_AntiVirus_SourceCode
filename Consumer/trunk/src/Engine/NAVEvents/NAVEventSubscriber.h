////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVEventSubscriber.h : Declaration of the CNAVEventSubscriber

#ifndef __NAVEVENTSUBSCRIBER_H_
#define __NAVEVENTSUBSCRIBER_H_

#include "NAVEventResource.h"       // main symbols
#include "NAVEvents.h"

#include "ccSubscriberImpl.h"

_COM_SMARTPTR_TYPEDEF(INAVEventSubscriber, __uuidof(INAVEventSubscriber));

/////////////////////////////////////////////////////////////////////////////
// CNAVEventSubscriber
class ATL_NO_VTABLE CNAVEventSubscriber : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CNAVEventSubscriber, &CLSID_NAVEventSubscriber>,
	public IDispatchImpl<ccEvtMgr::ISubscriberImpl<INAVEventSubscriber>, &IID_INAVEventSubscriber, &LIBID_NAVEVENTSLib>
{
public:
    CNAVEventSubscriber(){}
    virtual ~CNAVEventSubscriber(){}

private:
    CNAVEventSubscriber(const CNAVEventSubscriber&);
    CNAVEventSubscriber& operator =(const CNAVEventSubscriber&);

public:
DECLARE_REGISTRY_RESOURCEID(IDR_NAVEVENTSUBSCRIBER)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVEventSubscriber)
	COM_INTERFACE_ENTRY(ISubscriberEx)
	COM_INTERFACE_ENTRY(INAVEventSubscriber)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// INAVEventSubscriber
public:
    STDMETHOD(DisconnectProxy)()
    {
        Disconnect();
        return S_OK;
    }
};

#endif //__NAVEVENTSUBSCRIBER_H_
