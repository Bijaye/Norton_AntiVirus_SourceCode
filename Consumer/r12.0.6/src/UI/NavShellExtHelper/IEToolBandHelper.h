// IEToolBandHelper.h : Declaration of the CIEToolBandHelper

#pragma once
#include "resource.h"       // main symbols
#include "NavShellExtHelper.h"
#include "avresbranding.h"

// CIEToolBandHelper

class ATL_NO_VTABLE CIEToolBandHelper : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CIEToolBandHelper, &CLSID_IEToolBandHelper>,
	public IIEToolBandHelper,
	public CHelper
{
public:
	CIEToolBandHelper()
	{

	}
	DECLARE_CLASSFACTORY_SINGLETON(CIEToolBandHelper)

	DECLARE_REGISTRY_RESOURCEID(IDR_IETOOLBANDHELPER)

	BEGIN_COM_MAP(CIEToolBandHelper)
		COM_INTERFACE_ENTRY(IIEToolBandHelper)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		CBrandingRes BrandRes;
		m_csProductName = BrandRes.ProductName();
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}
	HRESULT _Error(int iId, const IID& cGuid)
	{
		return Error(iId,cGuid);
	}
	HRESULT _Error(CString csError, const IID& cGuid)
	{
		return Error(csError,cGuid);
	}
public:

	STDMETHOD(onActivityLog)(void);
	STDMETHOD(onVirusDef)(void);
	STDMETHOD(navigate2NMAINPanel)(BSTR bstrPanelName);
	STDMETHOD(onQuarantine)(void);

};

OBJECT_ENTRY_AUTO(__uuidof(IEToolBandHelper), CIEToolBandHelper)
