// D:\work\source\scs\r10.2\Norton_AntiVirus\Corporate_Edition\Win32\src\Pscan\RTVScan\UIMgmt\ResultViewCOMCallback.cpp : Implementation of CResultViewCOMCallback

#include "stdafx.h"
#include "uimanager.h"
#include "ResultsViewCOMCallback.h"
#include "ResultsViewProxy.h"
#ifdef _USE_CCW
#include "symcharconvert.h"
#endif	//_USE_CCW
// CResultViewCOMCallback

STDMETHODIMP CResultsViewCOMCallback::ViewClosed(ULONG bStopScan)
{
    dprintf("Closing ResultsView for Key(%d)\n", m_dwKey);
    CResultsViewProxy objResultsViewProxy;
    if( ERROR_SUCCESS != CUIManager::GetInstance().GetResultsView(m_dwKey, m_dwType, objResultsViewProxy) )
    {
        SAVASSERT(!"No Results view for key and type");
        return E_ABORT;
    }
	objResultsViewProxy.ViewClosed(bStopScan);

    return S_OK;
}


STDMETHODIMP CResultsViewCOMCallback::TakeAction(/*[in]*/ULONG_PTR lData,
												 /*[in]*/BSTR bstrLogLine,
												 /*[in]*/ULONG dwAction,
												 /*[in]*/ULONG dwState,
												 /*[in]*/BOOL bSvcStopOrProcTerminate)
{
	// Validate parameters.
    CResultsViewProxy objResultsViewProxy;
    if( ERROR_SUCCESS != CUIManager::GetInstance().GetResultsView(m_dwKey, m_dwType, objResultsViewProxy) )
    {
        SAVASSERT(!"No Results view for key and type");
        return E_ABORT;
    }
	
    // Prepare data for the callback.
	const STATDLG& objStatDlg = objResultsViewProxy.GetStatDlg();
    TCHAR szLogLine[MAX_LOG_LINE_SIZE];

	memset(szLogLine, 0, sizeof(szLogLine));
	SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrLogLine, -1, szLogLine, sizeof(szLogLine), NULL, NULL)
				   > 0);
	// Perform the callback.
	DWORD dwResult = RVCB_TakeAction(objStatDlg.Computer,
									 lData,
									 szLogLine,
									 dwAction,
									 dwState,
									 bSvcStopOrProcTerminate ? true : false);

	return (ERROR_SUCCESS == dwResult) ? S_OK : E_FAIL;
}


STDMETHODIMP CResultsViewCOMCallback::TakeAction2(/*[in]*/ULONG_PTR lData,
												  /*[in]*/BSTR bstrLogLine,
												  /*[in]*/ULONG dwPrimaryAction,
												  /*[in]*/ULONG dwSecondaryAction,
												  /*[in]*/ULONG dwState,
												  /*[in]*/BOOL bSvcStopOrProcTerminate)
{
	// Validate parameters.
    CResultsViewProxy objResultsViewProxy;
    if( ERROR_SUCCESS != CUIManager::GetInstance().GetResultsView(m_dwKey, m_dwType, objResultsViewProxy) )
    {
        SAVASSERT(!"No Results view for key and type");
        return E_ABORT;
    }
	
    // Prepare data for the callback.
	const STATDLG& objStatDlg = objResultsViewProxy.GetStatDlg();
    TCHAR szLogLine[MAX_LOG_LINE_SIZE];

	memset(szLogLine, 0, sizeof(szLogLine));
	SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrLogLine, -1, szLogLine, sizeof(szLogLine), NULL, NULL)
				   > 0);
	// Perform the callback.
	DWORD dwResult = RVCB_TakeAction2(objStatDlg.Computer,
									  0,
									  szLogLine,
									  dwPrimaryAction,
									  dwSecondaryAction,
									  dwState,
									  bSvcStopOrProcTerminate ? true : false,
									  lData);

	return (ERROR_SUCCESS == dwResult) ? S_OK : E_FAIL;
}


STDMETHODIMP CResultsViewCOMCallback::GetFileStateForView(/*[in]*/ ULONG_PTR lData,
														  /*[in]*/ BSTR bstrLogLine,
														  /*[out]*/ULONG *pdwState,
														  /*[in]*/ ULONG *pdwViewType)
{
	// Validate parameters.
    CResultsViewProxy objResultsViewProxy;
    if( ERROR_SUCCESS != CUIManager::GetInstance().GetResultsView(m_dwKey, m_dwType, objResultsViewProxy) )
    {
        SAVASSERT(!"No Results view for key and type");
        return E_ABORT;
    }
	
    // Prepare data for the callback.
	const STATDLG& objStatDlg = objResultsViewProxy.GetStatDlg();
    TCHAR szLogLine[MAX_LOG_LINE_SIZE];

	memset(szLogLine, 0, sizeof(szLogLine));
	SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrLogLine, -1, szLogLine, sizeof(szLogLine), NULL, NULL)
				   > 0);
	// Perform the callback.
	DWORD dwResult = RVCB_GetStateOfFileForView(objStatDlg.Computer,
												lData, // GetStateOfFileForView() from scandlg.cpp did this.
												szLogLine,
												pdwState,
												pdwViewType);

	return (ERROR_SUCCESS == dwResult) ? S_OK : E_FAIL;
}


STDMETHODIMP CResultsViewCOMCallback::GetCopyOfFile(/*[in]*/ ULONG_PTR lData,
													/*[in]*/ BSTR bstrLogLine,
													/*[in]*/ BSTR bstrDestFile,
													/*[in]*/ ULONG dwState,
													/*[out]*/ULONG *pdwResult)
{
	// Validate parameters.
	if (NULL == pdwResult)
	{
		return E_FAIL;
	}
    CResultsViewProxy objResultsViewProxy;
    if( ERROR_SUCCESS != CUIManager::GetInstance().GetResultsView(m_dwKey, m_dwType, objResultsViewProxy) )
    {
        SAVASSERT(!"No Results view for key and type");
        return E_ABORT;
    }
	
    // Prepare data for the callback.
	const STATDLG& objStatDlg = objResultsViewProxy.GetStatDlg();
    TCHAR szLogLine[MAX_LOG_LINE_SIZE];
    TCHAR szDestFile[MAX_LOG_LINE_SIZE];

	memset(szLogLine, 0, sizeof(szLogLine));
	SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrLogLine, -1, szLogLine, sizeof(szLogLine), NULL, NULL)
				   > 0);
	memset(szDestFile, 0, sizeof(szDestFile));
	SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrDestFile, -1, szDestFile, sizeof(szDestFile), NULL, NULL)
				   > 0);
	// Perform the callback.
	*pdwResult = RVCB_GetCopyOfFile(objStatDlg.Computer,
									lData, // GetCopyOfFile() from scandlg.cpp did this.
									szLogLine,
#ifdef _USE_CCW
									(wchar_t*)bstrDestFile,
#else	//_USE_CCW
									szDestFile,
#endif	//_USE_CCW
									dwState);

	return S_OK;
}


STDMETHODIMP CResultsViewCOMCallback::ExcludeItem(/*[in]*/ BSTR bstrLogLine)
{
    CResultsViewProxy objResultsViewProxy;
    if( ERROR_SUCCESS != CUIManager::GetInstance().GetResultsView(m_dwKey, m_dwType, objResultsViewProxy) )
    {
        SAVASSERT(!"No Results view for key and type");
        return E_ABORT;
    }
	
    // Prepare data for the callback.
    TCHAR szLogLine[MAX_LOG_LINE_SIZE];

	memset(szLogLine, 0, sizeof(szLogLine));
	SAVVERIFY( WideCharToMultiByte(CP_ACP, 0, (wchar_t*)bstrLogLine, -1, szLogLine, sizeof(szLogLine), NULL, NULL)
				   > 0);
	// Perform the callback.
	RVCB_ExcludeItem(szLogLine, m_dwType);

	return S_OK;
}
