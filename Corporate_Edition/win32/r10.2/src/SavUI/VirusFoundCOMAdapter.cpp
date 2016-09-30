// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

// VirusFoundCOMAdapter.cpp : Implementation of CVirusFoundCOMAdapter

#include "stdafx.h"
#include "savassert.h"
#include "VirusFoundCOMAdapter.h"
#include "scandlgloader.h"
#include "vpcommon.h"
#include "gitmanager.h"
#include "rtvscan_h.h"

const IID IID_IVirusFoundCOMCallback = {0x45389BB6,0x6A0A,0x41AE,0x80,0x44,0x6B,0x0F,0xDD,0xEC,0x87,0xCC};

_COM_SMARTPTR_TYPEDEF(IVirusFoundCOMCallback, __uuidof(IVirusFoundCOMCallback));

/** Creates notification dlg and addes a new message to the dialog
  * 
  **/
STDMETHODIMP CVirusFoundCOMAdapter::AddMessage(BSTR bstrLogLine, BSTR bstrDescription)
{
    //::MessageBox(NULL, "TEST", "TEST", MB_OK);

    //load ScanDlgs if needed
    CScanDlgLoader& objLoader = CScanDlgLoader::GetInstance();
    if( NULL == m_pVirusFoundDlg )
    {    
        if( CreateDlg(m_dwSessionId, m_bstrTitle) != S_OK)
            return E_ABORT;
    }

    //add message to notification dlg
    char szLogLine[MAX_LOG_LINE_SIZE];
    char szDescription[MAX_LOG_LINE_SIZE];

	SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrLogLine, -1, szLogLine, sizeof(szLogLine), NULL, NULL)
				   > 0);
    SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrDescription,  -1, szDescription,  sizeof(szDescription),  NULL, NULL)
				   > 0);
    if( NULL != m_pVirusFoundDlg &&
        objLoader.AddLogLineToVirusFound(m_pVirusFoundDlg, szLogLine, szDescription)
			== ERROR_SUCCESS )
	{
	    return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}


/** Callback called from the dialog that notifies SAVUI that the dialog has closed
  * @param dwIdForCallback the id we specified in the VIRUSFOUNDDLG struct
  **/
DWORD CVirusFoundCOMAdapter::ViewClosed(void *pobjContext, BOOL bDoesNothing)
{
    (void)bDoesNothing;
	if (NULL == pobjContext)
	{
		SAVASSERT(pobjContext != NULL);
		return ERROR_INVALID_PARAMETER;
	}
	// dwIdForCallback is actually a CVirusFoundCOMAdapter pointer.  Check it.
	CVirusFoundCOMAdapter *pThis = reinterpret_cast<CVirusFoundCOMAdapter*>(pobjContext);

	// Now react to the dialog being closed.
    DWORD dwRet = S_OK;
	pThis->m_pVirusFoundDlg = NULL;
	// Call the COM callback.
    IVirusFoundCOMCallbackPtr pVirusFoundCOMCallbackPtr = NULL;
    CGITManager& objGITManager = CGITManager::GetInstance();
    dwRet = objGITManager.GetInterface(pThis->m_dwCookie,
									   IID_IVirusFoundCOMCallback,
									   (void**)&pVirusFoundCOMCallbackPtr);

    if(pVirusFoundCOMCallbackPtr != NULL)
	{
        dwRet = pVirusFoundCOMCallbackPtr->DlgClosed(pThis->m_dwSessionId);
	}

    if( ERROR_SUCCESS == dwRet )
        dwRet = objGITManager.RevokeInterface(pThis->m_dwCookie);

    return dwRet;
}

/** Registers the callback interface that notifies the caller
  * the dialog has closed and to clean up the memory
  *
  **/
STDMETHODIMP CVirusFoundCOMAdapter::RegisterCallback(IUnknown* pUnk)
{
    if( pUnk == NULL )
        return E_INVALIDARG;
    
    CGITManager& objGITManager = CGITManager::GetInstance();
    return objGITManager.RegisterInterface(pUnk, IID_IVirusFoundCOMCallback, m_dwCookie);
}

/** Creates the notification dlg
  *
  **/
STDMETHODIMP CVirusFoundCOMAdapter::CreateDlg(ULONG dwSessionId, BSTR bstrTitle)
{
    //save off info for class
    m_dwSessionId = dwSessionId;
    m_bstrTitle = bstrTitle;

    //::MessageBox(NULL, "TEST", "TEST", MB_OK);

    //create the VIRUSFOUNDDLG structure to pass data into the dlg
    VIRUSFOUNDDLG sVirusFoundData;

    memset(&sVirusFoundData,0,sizeof(sVirusFoundData));
    sVirusFoundData.Size		   = sizeof(sVirusFoundData);
	sVirusFoundData.ViewClosed = ViewClosed;
    sVirusFoundData.Context = reinterpret_cast<LPVOID>(this);

    char szTitle[MAX_LOG_LINE_SIZE];
    DWORD dwRet = WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrTitle, -1, szTitle, sizeof(szTitle), NULL, NULL);
    SAVASSERT(dwRet != 0);
	sVirusFoundData.Title = szTitle;


     //load ScanDlgs if needed
    CScanDlgLoader& objLoader = CScanDlgLoader::GetInstance();
    if( NULL == m_pVirusFoundDlg )
    {    
        if( ERROR_SUCCESS == objLoader.CreateVirusFoundDlg(&sVirusFoundData) )
        {
            m_pVirusFoundDlg = sVirusFoundData.pVirusFoundDlg;      
        }
        else
            return E_ABORT;
    }

    return S_OK;
}
