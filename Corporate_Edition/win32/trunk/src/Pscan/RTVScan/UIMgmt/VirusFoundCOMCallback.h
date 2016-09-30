// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

// VirusFoundCOMCallback.h : Declaration of the CVirusFoundCOMCallback

#pragma once
#include "resource.h"       // main symbols
#include "atlbase.h"
#include "atlcom.h"
#include "Rtvscan.h"


// CVirusFoundCOMCallback

class ATL_NO_VTABLE CVirusFoundCOMCallback : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CVirusFoundCOMCallback, &CLSID_VirusFoundCOMCallback>,
	public IVirusFoundCOMCallback
{
public:
    CVirusFoundCOMCallback():m_bTrusted(false)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_VIRUSFOUNDCOMCALLBACK)

DECLARE_NOT_AGGREGATABLE(CVirusFoundCOMCallback)

BEGIN_COM_MAP(CVirusFoundCOMCallback)
	COM_INTERFACE_ENTRY(IVirusFoundCOMCallback)
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
private:
    bool    m_bTrusted;

public:

    STDMETHOD(DlgClosed)(ULONG ulSessionId);
};

OBJECT_ENTRY_AUTO(__uuidof(VirusFoundCOMCallback), CVirusFoundCOMCallback)
