// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

// ResultsViewCOMAdapter.cpp : Implementation of CResultsViewCOMAdapter

#include "stdafx.h"
#include "savassert.h"
#include "ClientReg.h"
#include "vpcommon.h"
#include "RegUtils.h"
#include "ProgressBlockConfig.h"
#include "ProgressBlockSafeArrayCreatorImpl.h"
#include "safearraycreator.h"
#include "saverror.h"
#include "ResultsViewCOMAdapter.h"
#include "scandlgloader.h"
#include "gitmanager.h"
#include "rtvscan_h.h"

const IID IID_IResultsViewCOMCallback = {0x633A25E8,0x4FFE,0x4BD5,0xBC,0xBF,0x61,0xB3,0x1D,0x7F,0xDD,0xF3};

_COM_SMARTPTR_TYPEDEF(IResultsViewCOMCallback, __uuidof(IResultsViewCOMCallback));

/** DWORD ViewClosed(void *pobjContext)
  *
  * Callback called from the dialog that notifies SAVUI that the dialog has closed
  *
 **/
DWORD CResultsViewCOMAdapter::ViewClosed(void *pobjContext, BOOL bScanStop)
{
    if (NULL == pobjContext)
	{
		SAVASSERT(pobjContext != NULL);
		return ERROR_INVALID_PARAMETER;
	}
	// dwIdForCallback is actually a CVirusFoundCOMAdapter pointer.  Check it.
	CResultsViewCOMAdapter *pThis = reinterpret_cast<CResultsViewCOMAdapter*>(pobjContext);

    DWORD dwRet = S_OK;
    {
        CGuard objGuard(pThis->m_objLock);
        pThis->m_pResultsViewDlg = NULL;
    }

    IResultsViewCOMCallbackPtr pCOMCallbackPtr = NULL;
    CGITManager& objGITManager = CGITManager::GetInstance();
    dwRet = objGITManager.GetInterface(pThis->m_dwCookie, IID_IResultsViewCOMCallback, (void**)&pCOMCallbackPtr);
    
    if(pCOMCallbackPtr != NULL)
        dwRet = pCOMCallbackPtr->ViewClosed(bScanStop);

    if( SUCCEEDED(dwRet) )
        dwRet = objGITManager.RevokeInterface(pThis->m_dwCookie);

    return dwRet;
}


/** @see #TakeAction() in Rtvscan */
DWORD CResultsViewCOMAdapter::TakeAction(void *pobjContext,
										 LPARAM lData,
										 char *pcLogLine,
										 DWORD dwAction,
										 DWORD dwState, 
										 bool bSvcStopOrProcTerminate)
{
    if (NULL == pobjContext || NULL == pcLogLine)
	{
		SAVASSERT(pobjContext != NULL);
		return ERROR_INVALID_PARAMETER;
	}
	// dwIdForCallback is actually a CVirusFoundCOMAdapter pointer.  Check it.
	CResultsViewCOMAdapter *pThis = reinterpret_cast<CResultsViewCOMAdapter*>(pobjContext);

    DWORD dwRet = S_OK;

    IResultsViewCOMCallbackPtr pCOMCallbackPtr = NULL;
    CGITManager& objGITManager = CGITManager::GetInstance();
    dwRet = objGITManager.GetInterface(pThis->m_dwCookie, IID_IResultsViewCOMCallback, (void**)&pCOMCallbackPtr);
    
    if(pCOMCallbackPtr != NULL)
	{
		CComBSTR bstrLogLine(pcLogLine);
		HRESULT hRet = pCOMCallbackPtr->TakeAction(lData,
												   bstrLogLine,
												   dwAction,
												   dwState,
												   bSvcStopOrProcTerminate);
		dwRet = (S_OK == hRet) ? ERROR_SUCCESS : ERROR_GEN_FAILURE;
			// TakeAction() can return S_FALSE, which means the call failed.
	}

    return dwRet;
}


/** @see #TakeAction2() in Rtvscan */
DWORD CResultsViewCOMAdapter::TakeAction2(void *pobjContext,
										  LPARAM lData,
										  char *pcLogLine,
										  DWORD dwPrimaryAction,
										  DWORD dwSecondaryAction,
										  DWORD dwState,
										  bool bSvcStopOrProcTerminate)
{
    if (NULL == pobjContext)
	{
		SAVASSERT(pobjContext != NULL);
		return ERROR_INVALID_PARAMETER;
	}
	// dwIdForCallback is actually a CVirusFoundCOMAdapter pointer.  Check it.
	CResultsViewCOMAdapter *pThis = reinterpret_cast<CResultsViewCOMAdapter*>(pobjContext);

    DWORD dwRet = S_OK;

    IResultsViewCOMCallbackPtr pCOMCallbackPtr = NULL;
    CGITManager& objGITManager = CGITManager::GetInstance();
    dwRet = objGITManager.GetInterface(pThis->m_dwCookie, IID_IResultsViewCOMCallback, (void**)&pCOMCallbackPtr);
    
    if(pCOMCallbackPtr != NULL)
	{
		CComBSTR bstrLogLine(pcLogLine);
		HRESULT hRet = pCOMCallbackPtr->TakeAction2(lData,
													bstrLogLine,
													dwPrimaryAction,
													dwSecondaryAction,
													dwState,
													bSvcStopOrProcTerminate);
		dwRet = (S_OK == hRet) ? ERROR_SUCCESS : ERROR_GEN_FAILURE;
			// TakeAction2() can return S_FALSE, which means the call failed.
	}

    return dwRet;
}


/** @see #RVCB_GetStateOfFileForView() in Rtvscan
  * @returns ERROR_SUCCESS on success or ERROR_GEN_FAILURE on any failure
  */
DWORD CResultsViewCOMAdapter::GetFileStateForView(void *pobjContext,
												  LPARAM lData,
												  char *pcLogLine,
												  DWORD *pdwState,
												  DWORD *pdwViewType)
{
    if (NULL == pobjContext || NULL == pcLogLine || NULL == pdwState || NULL == pdwViewType)
	{
		SAVASSERT(!"Invalid parameter(s) passed to CResultsViewCOMAdapter::GetFileStateForView().");
		return ERROR_INVALID_PARAMETER;
	}
	// dwIdForCallback is actually a CVirusFoundCOMAdapter pointer.  Check it.
	CResultsViewCOMAdapter *pThis = reinterpret_cast<CResultsViewCOMAdapter*>(pobjContext);

	SAVASSERT(*pdwViewType == pThis->m_dwType);
		// pdwViewType is a redundant parameter that should match this view's type.
	*pdwState = 0;

    DWORD dwRet = S_OK;

    IResultsViewCOMCallbackPtr pCOMCallbackPtr = NULL;
    CGITManager& objGITManager = CGITManager::GetInstance();
    dwRet = objGITManager.GetInterface(pThis->m_dwCookie, IID_IResultsViewCOMCallback, (void**)&pCOMCallbackPtr);

    if(pCOMCallbackPtr != NULL)
	{
		CComBSTR bstrLogLine(pcLogLine);
		HRESULT hRet = pCOMCallbackPtr->GetFileStateForView(lData,
															bstrLogLine,
															pdwState,
															pdwViewType);
		dwRet = (S_OK == hRet) ? ERROR_SUCCESS : ERROR_GEN_FAILURE;
			// RVCB_GetStateOfFileForView() returns several potentially
			// meaningful error codes (e.g. ERROR_NO_SCAN and
			// ERROR_IMPERSONATION_FAILED), but CGetStatus::FillAndGetStatus()
			// (the only caller of this function) doesn't check the return code.
	}

    return dwRet;
}


/** @see #RVCB_GetCopyOfFile() in Rtvscan
  * @returns ERROR_SUCCESS on success or another code on failure
  */
DWORD CResultsViewCOMAdapter::GetCopyOfFile(void *pobjContext,
											LPARAM lData,
											char *pcLogLine,
											char *pcDestFile,
											DWORD dwState)
{
    if (NULL == pobjContext || NULL == pcLogLine || NULL == pcDestFile)
	{
		SAVASSERT(!"Invalid parameter(s) passed to CResultsViewCOMAdapter::GetCopyOfFile().");
		return ERROR_INVALID_PARAMETER;
	}
	// dwIdForCallback is actually a CVirusFoundCOMAdapter pointer.  Check it.
	CResultsViewCOMAdapter *pThis = reinterpret_cast<CResultsViewCOMAdapter*>(pobjContext);

	DWORD dwRet = S_OK;

    IResultsViewCOMCallbackPtr pCOMCallbackPtr = NULL;
    CGITManager& objGITManager = CGITManager::GetInstance();
    dwRet = objGITManager.GetInterface(pThis->m_dwCookie, IID_IResultsViewCOMCallback, (void**)&pCOMCallbackPtr);

    if(pCOMCallbackPtr != NULL)
	{
		CComBSTR bstrLogLine(pcLogLine);
		CComBSTR bstrDestFile(pcDestFile);
		dwRet = ERROR_SUCCESS;
		HRESULT hRet = pCOMCallbackPtr->GetCopyOfFile(lData,
													  bstrLogLine,
													  bstrDestFile,
													  dwState,
													  &dwRet);
		if (FAILED(hRet))
			dwRet = ERROR_GENERAL;
	}

    return dwRet;
}


void CResultsViewCOMAdapter::ExcludeItem(void *pobjContext,
										 const char *pcLogLine,
										 const DWORD dwRV_TYPE)
{
    if (NULL == pobjContext || NULL == pcLogLine)
	{
		SAVASSERT(!"Invalid parameter(s) passed to CResultsViewCOMAdapter::GetCopyOfFile().");
		return;
	}
	// dwIdForCallback is actually a CVirusFoundCOMAdapter pointer.  Check it.
	CResultsViewCOMAdapter *pThis = reinterpret_cast<CResultsViewCOMAdapter*>(pobjContext);

	SAVASSERT(dwRV_TYPE == pThis->m_dwType);

    IResultsViewCOMCallbackPtr pCOMCallbackPtr = NULL;
    CGITManager& objGITManager = CGITManager::GetInstance();
    objGITManager.GetInterface(pThis->m_dwCookie, IID_IResultsViewCOMCallback, (void**)&pCOMCallbackPtr);

    if(pCOMCallbackPtr != NULL)
	{
		CComBSTR bstrLogLine(pcLogLine);
		pCOMCallbackPtr->ExcludeItem(bstrLogLine);
	}
}


/** HRESULT RegisterCallback(IUnkown* pUnk)
  *
  * Registers the callback interface that notifies the caller
  * the dialog has closed and to clean up the memory
  *
 **/
STDMETHODIMP CResultsViewCOMAdapter::RegisterCallback(IUnknown* pUnk)
{
    if( pUnk == NULL )
        return E_INVALIDARG;
    
    CGITManager& objGITManager = CGITManager::GetInstance();
    return objGITManager.RegisterInterface(pUnk, IID_IResultsViewCOMCallback, m_dwCookie);
}

/** Creates the results view dlg.
  * @param dwKey The value used as a key in CUIManager's maps.  This is passed
  * into SAVUI for debugging purposes only.
  * @param hRootKey The root key containing info about the scan:
  * either HKEY_LOCAL_MACHINE or HKEY_USERS.
  * @param bstrScanKey Name of the key with scan info.
  * @param dwFlags Specifies extra flag values that this function can't set
  * based on data from the registry.
  * @param dwDelayCount the delay count
  * @param dwType type of the scan:  RV_TYPE_AUTOPROTECT or RV_TYPE_SCAN.
  * @see CreateResultsView() in ScanDlgs
  */
STDMETHODIMP CResultsViewCOMAdapter::CreateDlg(ULONG	 dwKey,
											   ULONG_PTR hRootKey,
											   BSTR		 bstrScanKey,
											   ULONG	 dwFlags,
											   ULONG	 dwDelayCount,
                                               ULONG     dwType,
                                               ULONG     dwStartTickCount)
{
    {
        CGuard objGuard(m_objLock);
    	// Validate that we're in a good state for this function call.
        SAVASSERT(NULL == m_pResultsViewDlg); // The dialog has already been created!
        if (m_pResultsViewDlg != NULL)
	        return E_FAIL;
    }

    // Save identifying info for debugging purposes.
    m_dwKey = dwKey;
    m_dwType = dwType;
    //create the RESULTSVIEW structure to pass data into the dlg
    RESULTSVIEW sResultsViewData;

	memset(&sResultsViewData, 0, sizeof(sResultsViewData));
	// Set hard-coded values.
    sResultsViewData.Size		= sizeof(sResultsViewData);
	sResultsViewData.dwMaxDelay = 2;
    sResultsViewData.Type		= dwType;
	// Open the registry key.
    TCHAR szScanKey[MAX_LOG_LINE_SIZE];
	HKEY  hScanKey = NULL;

	SAVVERIFY(WideCharToMultiByte(CP_ACP,
								  0,
								  (wchar_t*)bstrScanKey,
								  -1,
								  szScanKey,
								  sizeof(szScanKey),
								  NULL,
								  NULL)
				  > 0);
	RegOpenKeyEx((HKEY)hRootKey, szScanKey, 0, KEY_READ, &hScanKey);
	// Get values from the registry.
	if (hScanKey != NULL)
	{
#pragma warning (disable : 4312) // Disable the warning about casting from DWORD to HWND.
		sResultsViewData.hWndParent = (HWND)(GetVal(hScanKey, szReg_Val_StatusHwnd, 0));
#pragma warning (default : 4312)
		PutVal(hScanKey,"StatusHWND",0);
			// Comment for the code review:  I don't see any place where StatusHWND is
			// set to a non-zero value.  Can we take this out, or do customers use it?
		if (sResultsViewData.hWndParent)
			sResultsViewData.Flags |= RV_FLAGS_CHILD;
		if (GetVal(hScanKey,"ScanLocked",0))
			sResultsViewData.Flags |= RV_FLAGS_LOCK_SCAN;
		if (GetVal(hScanKey, "CloseScan",0))
			sResultsViewData.Flags |= RV_FLAGS_CLOSE_SCAN;
		if (GetVal(hScanKey, "EnableDelay",0))
			sResultsViewData.Flags |= RV_FLAGS_DELAYABLE;
		sResultsViewData.dwMaxDelay   = GetVal(hScanKey, "Schedule\\MaxDelay", 2);
		sResultsViewData.dwMaxPause   = GetVal(hScanKey, "Schedule\\MaxPause", 0);
		sResultsViewData.dwAllow4hour = GetVal(hScanKey, "Schedule\\Allow4Hour", 0);
        sResultsViewData.dwTime = dwStartTickCount;
	    TCHAR acScanNameBuffer[MAX_SCAN_NAME_LENGTH];

        sResultsViewData.Title = GetStr(hScanKey,
										szReg_Value_ScanTitle,
										acScanNameBuffer,
										sizeof(acScanNameBuffer),
										_T(""));
		//Getting the value of "show scan progress if threat detected" key
		sResultsViewData.bShowOnFirstThreat = GetVal(hScanKey, szReg_Value_DisplayStatusDlg_Thrt, 0)? true : false;
	}
	else
	{
		// Is the code path that sets TmpTitle still used?
        //sResultsViewData.Title = block->TmpTitle;
	}

	// Set data from parameters.
    sResultsViewData.Flags		 |= dwFlags;
    sResultsViewData.dwDelayCount = dwDelayCount;
	// TODO:  A param from Transman should set sResultsViewData.bViewOpenedRemotely.

	// Callbacks
    sResultsViewData.Context			 = this;
		// Context is an identifying value passed back to callbacks.
    sResultsViewData.ViewClosed          = ViewClosed;
    sResultsViewData.TakeAction          = TakeAction;
    sResultsViewData.TakeAction2         = TakeAction2;
	sResultsViewData.GetFileStateForView = GetFileStateForView;
	sResultsViewData.GetCopyOfFile       = GetCopyOfFile;
	sResultsViewData.pfnExcludeItem         = (EXCLUDEITEM)ExcludeItem;
    // Create the dialog!
    CScanDlgLoader &objLoader = CScanDlgLoader::GetInstance();

    if (objLoader.CreateResultsView(&sResultsViewData) != ERROR_SUCCESS)
        return E_ABORT;
    m_pResultsViewDlg = sResultsViewData.pResultsViewDlg;

    return S_OK;
}


/** Adds info on a threat to a scan results dialog.
  * @see AddLogLineToResultsView() in ScanDlgs
  */
STDMETHODIMP CResultsViewCOMAdapter::AddLogLine(long lData, BSTR bstrLogLine)
{
    //load ScanDlgs if needed
    CScanDlgLoader& objLoader = CScanDlgLoader::GetInstance();
    {
        CGuard objGuard(m_objLock);
        if( NULL == m_pResultsViewDlg )
        {    
            return E_ABORT;
        }
    }

    //add message to notification dlg
    TCHAR szLogLine[MAX_LOG_LINE_SIZE];

	SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrLogLine, -1, szLogLine, sizeof(szLogLine), NULL, NULL)
				   > 0);
    if (objLoader.AddLogLineToResultsView(m_pResultsViewDlg, lData, szLogLine)
			== ERROR_SUCCESS)
	{
	    return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

STDMETHODIMP CResultsViewCOMAdapter::AddProgress(VARIANT vProgressBlock)
{
     //load ScanDlgs if needed
    CScanDlgLoader& objLoader = CScanDlgLoader::GetInstance();
    {
        CGuard objGuard(m_objLock);
        if( NULL == m_pResultsViewDlg )
        {    
            return E_ABORT;
        }
    }

	CProgressBlockConfig objProgressConfig;
	CProgressBlockSafeArrayCreatorImpl oSACreatorImpl;
	CSafeArrayCreator oSACreator(&oSACreatorImpl);

    HRESULT hr = E_FAIL;
	if( true == oSACreator.GetDataBlock(vProgressBlock.parray, &objProgressConfig) )
    {
        PROGRESSBLOCK tProgressBlock;
        if( ERROR_SUCCESS == objProgressConfig.GetProgressBlock(&tProgressBlock) )
        {
            if(objLoader.AddProgressToResultsView(m_pResultsViewDlg, &tProgressBlock) == ERROR_SUCCESS )
                hr = S_OK;
            else
                hr = ERROR_STOP_SCAN; //returning a DWORD error code as an HRESULT is not the best thing todo
                                      //but we need to work with legacy RTVSCAN stuff
        }
    }

    return hr;
}
