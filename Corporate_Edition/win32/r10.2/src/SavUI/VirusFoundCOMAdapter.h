// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

// VirusFoundCOMAdapter.h : Declaration of the CVirusFoundCOMAdapter

#pragma once
#include "resource.h"       // main symbols
#include <string>
#include "SavUI.h"
#include "scandlgloader.h"
#include "scndlgex.h"

/** CVirusFoundCOMAdapter is the wrapper class for the notification dlg that
  * wraps the Virus found dlg with COM calls.
  **/
class ATL_NO_VTABLE CVirusFoundCOMAdapter : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CVirusFoundCOMAdapter, &CLSID_VirusFoundCOMAdapter>,
	public IVirusFoundCOMAdapter
{
public:
    CVirusFoundCOMAdapter(): m_pVirusFoundDlg(NULL),
                        m_dwSessionId(0),
                        m_dwCookie(0)
	{
	}

    ~CVirusFoundCOMAdapter()
    {
    }

DECLARE_REGISTRY_RESOURCEID(IDR_VIRUSFOUNDCOMADAPTER)

DECLARE_NOT_AGGREGATABLE(CVirusFoundCOMAdapter)

BEGIN_COM_MAP(CVirusFoundCOMAdapter)
	COM_INTERFACE_ENTRY(IVirusFoundCOMAdapter)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

// COM interface functions
public:
    STDMETHOD(AddMessage)(BSTR bstrLogLine, BSTR bstrDescription);
    STDMETHOD(RegisterCallback)(IUnknown* pUnk);
    STDMETHOD(CreateDlg)(ULONG dwSessionId, BSTR bstrTitle);

// Callback for ScanDlgs
public:
    static DWORD ViewClosed(void *pobjContext, BOOL bDoesNothing);

private:
    CVirusFound*    m_pVirusFoundDlg;
    CComBSTR        m_bstrTitle;
    DWORD           m_dwSessionId;
    DWORD           m_dwCookie;
};

OBJECT_ENTRY_AUTO(__uuidof(VirusFoundCOMAdapter), CVirusFoundCOMAdapter)
