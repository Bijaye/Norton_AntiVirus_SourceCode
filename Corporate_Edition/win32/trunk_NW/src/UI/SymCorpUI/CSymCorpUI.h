// CSymCorpUI.h : Declaration of the CSymCorpUI

#pragma once
#include "resource.h"       // main symbols
#include "SymCorpUIApp.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CSymCorpUI
class ATL_NO_VTABLE CSymCorpUI :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSymCorpUI, &CLSID_SymCorpUI>,
	public ISupportErrorInfo,
	public IDispatchImpl<ISymCorpUI, &IID_ISymCorpUI, &LIBID_SymCorpUILib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CSymCorpUI()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SYMCORPUI1)


BEGIN_COM_MAP(CSymCorpUI)
	COM_INTERFACE_ENTRY(ISymCorpUI)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

    // ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

    // ISymCorpUI
    STDMETHOD(ShowUI)(DWORD userArg);
public:

};

OBJECT_ENTRY_AUTO(__uuidof(SymCorpUI), CSymCorpUI)
