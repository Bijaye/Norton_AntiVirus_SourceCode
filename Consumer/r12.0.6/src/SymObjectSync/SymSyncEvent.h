	
// SymSyncEvent.h : Declaration of the CSymSyncEvent

#ifndef __SYMSYNCEVENT_H_
#define __SYMSYNCEVENT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSymSyncEvent
class ATL_NO_VTABLE CSymSyncEvent : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CSymSyncEvent, &CLSID_SymSyncEvent>,
	public ISupportErrorInfo,
	public IDispatchImpl<ISymSyncEvent, &IID_ISymSyncEvent, &LIBID_SYMOBJECTSYNCLib>
{
public:
	CSymSyncEvent()
	{
		m_pUnkMarshaler = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SYMSYNCEVENT)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSymSyncEvent)
	COM_INTERFACE_ENTRY(ISymSyncEvent)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
END_COM_MAP()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(
			GetControllingUnknown(), &m_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		m_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> m_pUnkMarshaler;

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ISymSyncEvent
public:
	STDMETHOD(Wait)(/*[in]*/long timeoutInMilliSeconds,/*[in]*/BOOL bWaitWithMessageLoop,/*[out,retval]*/long* pReturn);
	STDMETHOD(ResetEvent)(/*[out,retval]*/BOOL* pReturn);
	STDMETHOD(SetEvent)(/*[out,retval]*/BOOL* pReturn);
	STDMETHOD(InitializeEvent)(/*[in]*/ BOOL bManualReset,/*[in]*/BOOL bInitialState,/*[in]*/BSTR name);

    
    
    StahlSoft::CSmartHandle m_smEvent;    
};

#endif //__SYMSYNCEVENT_H_
