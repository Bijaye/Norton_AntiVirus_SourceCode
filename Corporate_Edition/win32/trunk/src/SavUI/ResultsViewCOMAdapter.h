// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

// ResultsViewCOMAdapter.h : Declaration of the CResultsViewCOMAdapter

#pragma once
#include "resource.h"       // main symbols
#include <string>
#include "SavUI.h"
#include "scandlgloader.h"
#include "scndlgex.h"

class CResultsView;

// CResultsViewCOMAdapter
/**
  * CResultsViewCOMAdapter is the wrapper class for the results view dlg.
  * It wraps the dlg with COM calls.
  */
class ATL_NO_VTABLE CResultsViewCOMAdapter : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CResultsViewCOMAdapter, &CLSID_ResultsViewCOMAdapter>,
	public IResultsViewCOMAdapter
{
public:
    CResultsViewCOMAdapter()
	  :	m_pResultsViewDlg(NULL),
		m_dwKey(0),
		m_dwType(0),
        m_dwCookie(0)
	{
	}

    ~CResultsViewCOMAdapter()
    {
    }

DECLARE_REGISTRY_RESOURCEID(IDR_RESULTSVIEWCOMADAPTER)

DECLARE_NOT_AGGREGATABLE(CResultsViewCOMAdapter)

BEGIN_COM_MAP(CResultsViewCOMAdapter)
	COM_INTERFACE_ENTRY(IResultsViewCOMAdapter)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

// Implementation of COM interface functions.
public:
    STDMETHOD(RegisterCallback)(IUnknown* pUnk);
    STDMETHOD(CreateDlg)(ULONG	   dwKey,
						 ULONG_PTR hRootKey,
						 BSTR	   bstrScanKey,
						 ULONG	   dwFlags,
						 ULONG	   dwDelayCount,
                         ULONG     dwType,
                         ULONG     dwStartTickCount);
	STDMETHOD(AddLogLine)(long lData, BSTR bstrLogLine);
    STDMETHOD(AddProgress)(VARIANT vProgressBlock);

// Callbacks for the scan results view dialog.
public:
    static DWORD ViewClosed(void *pobjContext, BOOL bScanStop);
	static DWORD TakeAction(void *pobjContext,
							LPARAM lData,
							char *pcLogLine,
							DWORD dwAction,
							DWORD dwState, 
							bool bSvcStopOrProcTerminate);
	static DWORD TakeAction2(void *pobjContext,
							 LPARAM lData,
							 char *pcLogLine,
							 DWORD dwPrimaryAction,
							 DWORD dwSecondaryAction,
							 DWORD dwState,
							 bool bSvcStopOrProcTerminate);
	static DWORD GetFileStateForView(void *pobjContext,
									 LPARAM lData,
									 char *pcLogLine,
									 DWORD *pdwState,
									 DWORD *pdwViewType);
	static DWORD GetCopyOfFile(void *pobjContext,
							   LPARAM lData,
							   char *pcLogLine,
							   char *pcDestFile,
							   DWORD dwState);
	static void  ExcludeItem(void *pobjContext,
							 const char *pcLogLine,
							 const DWORD dwRV_TYPE);

private:
    CResultsView *m_pResultsViewDlg;
    DWORD         m_dwCookie;
	DWORD		  m_dwKey;  // These two values are only stored
	DWORD		  m_dwType; // to help debugging.
public:
};

OBJECT_ENTRY_AUTO(__uuidof(ResultsViewCOMAdapter), CResultsViewCOMAdapter)
