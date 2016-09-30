// D:\work\source\scs\r10.2\Norton_AntiVirus\Corporate_Edition\Win32\src\Pscan\RTVScan\UIMgmt\ResultViewCOMCallback.h : Declaration of the CResultViewCOMCallback

#pragma once

#include "resource.h"       // main symbols
#include <atlbase.h>
#include <atlcom.h>
#include "Rtvscan.h"
#include "ResultsViewProxy.h"

// CResultViewCOMCallback

/** This is the COM object in Rtvscan that receives scan results view callbacks
  * from SAVUI and passes them into Rtvscan.
  * You must call SetScanInfo() after creating a callback object.
  */
class ATL_NO_VTABLE CResultsViewCOMCallback : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CResultsViewCOMCallback, &CLSID_ResultsViewCOMCallback>,
	public IResultsViewCOMCallback
{
public:
    CResultsViewCOMCallback()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RESULTSVIEWCOMCALLBACK)

DECLARE_NOT_AGGREGATABLE(CResultsViewCOMCallback)

BEGIN_COM_MAP(CResultsViewCOMCallback)
	COM_INTERFACE_ENTRY(IResultsViewCOMCallback)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

    void EstablishTrust(void){ m_bTrusted = true; };
    void SetScanInfo(const DWORD dwKey, const DWORD dwType)
    {
        m_dwKey = dwKey;
        m_dwType = dwType;
    }

private:
    bool    m_bTrusted;
	DWORD   m_dwKey; //key to the proxy in the results view
    DWORD   m_dwType;

public:

    STDMETHOD(ViewClosed)(/*[in]*/ULONG bStopScan);
	STDMETHOD(TakeAction)(/*[in]*/ULONG_PTR lData,
						  /*[in]*/BSTR bstrLogLine,
						  /*[in]*/ULONG dwAction,
						  /*[in]*/ULONG dwState,
						  /*[in]*/BOOL bSvcStopOrProcTerminate);
	STDMETHOD(TakeAction2)(/*[in]*/ULONG_PTR lData,
						   /*[in]*/BSTR bstrLogLine,
						   /*[in]*/ULONG dwPrimaryAction,
						   /*[in]*/ULONG dwSecondaryAction,
						   /*[in]*/ULONG dwState,
						   /*[in]*/BOOL bSvcStopOrProcTerminate);
	STDMETHOD(GetFileStateForView)(/*[in]*/ ULONG_PTR lData,
								   /*[in]*/ BSTR bstrLogLine,
								   /*[out]*/ULONG *pdwState,
								   /*[in]*/ ULONG *pdwViewType);
	STDMETHOD(GetCopyOfFile)(/*[in]*/ ULONG_PTR lData,
							 /*[in]*/ BSTR bstrLogLine,
							 /*[in]*/ BSTR bstrDestFile,
							 /*[in]*/ ULONG dwState,
							 /*[out]*/ULONG *pdwResult);
	STDMETHOD(ExcludeItem)(/*[in]*/ BSTR bstrLogLine);
};

OBJECT_ENTRY_AUTO(__uuidof(ResultsViewCOMCallback), CResultsViewCOMCallback)
