// CStatusEventBridge.h : Declaration of the CCStatusEventBridge

#ifndef __CSTATUSEVENTBRIDGE_H_
#define __CSTATUSEVENTBRIDGE_H_

#include "resource.h"       // main symbols
#include "StatusCP.h"
#include "NAVStatus.h"

#include "comdef.h"

/////////////////////////////////////////////////////////////////////////////
// CCStatusEventBridge

// COM Bridge Object.
//
class ATL_NO_VTABLE CStatusEventBridge : 
	public CComObjectRootEx<CComSingleThreadModel>,
    public IStatusEventBridge
{
public:
	CStatusEventBridge()
	{
        m_spNAVStatus = 0;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_STATUSEVENTBRIDGE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CStatusEventBridge)
	COM_INTERFACE_ENTRY(IStatusEventBridge)
END_COM_MAP()

// Duplicate the events from NAVStatusEvents.h
// IStatusEventBridge
public:
	STDMETHOD(StatusChangedEvent)();
	HRESULT SetCallback ( CNAVStatus* pNAVStatus);
	CNAVStatus* m_spNAVStatus;
   
};

#endif //__CSTATUSEVENTBRIDGE_H_
