#include "stdafx.h"
#include "commonuithread.h"
#include "scanmgrinterface.h"

CCommonUIThread::CCommonUIThread(void)
{
    reset();
}

CCommonUIThread::~CCommonUIThread(void)
{
    reset();
}

void CCommonUIThread::reset()
{
    m_ulScanTime = 0;
    m_bScanCompressedFiles = true;
    m_UIType = CommonUI_Complete;
    m_lDamageControlItems = 0;
    m_bEmailScan = false;
    m_MemScanStatus = MemoryScanNotAvailable;
    m_bRepairProgress = true;
    m_bScanNonViralThreats = true;
    m_bQuickScan = false;
    m_hWndParent = NULL;

    m_spCommonUI = NULL;
    m_spAnomalyList = NULL;
    m_spSink = NULL;
}

// This is called from a client thread and it will kick off a new
// thread that the UI is running in
HRESULT CCommonUIThread::ShowCUIDlg( CommonUIType UIType, 
                                     ccEraser::IAnomalyList* pAnomalyList, 
                                     ICommonUISink* pSink, 
                                     const long lDamageControlItemsRemaining )
{
    // Validate some stuff
    if( !pSink )
    {
        CCTRACEE(_T("CCommonUIThread::ShowCUIDlg() - No sink!"));
        return E_FAIL;
    }

    // Set the thread options
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eSTAModel;
    m_Options.m_bPumpMessages = TRUE;

    // Set members
    m_UIType = UIType;
    m_spAnomalyList = pAnomalyList;
    m_spSink = pSink;
    m_lDamageControlItems = lDamageControlItemsRemaining;

    // Start the thread
    if( !Create(NULL, 0, 0) )
    {
        CCTRACEE(_T("CCommonUIThread::ShowCUIDlg() - Failed to create the UI thread."));
        return E_FAIL;
    }

    return S_OK;
}

int CCommonUIThread::Run (void)
{
    SMRESULT result = SMRESULT_OK;

    // Infections were found so we need to display the Common UI.
    if( SYM_FAILED(m_CommonUILoader.CreateObject(&m_spCommonUI)) || !m_spCommonUI )
    {
        // CommonUI creation failed, set result to this error
        CCTRACEE(_T("CCommonUIThread::Run() - Failed to create the common UI."));
        reset();
        return SMRESULT_ERROR_COMMON_UI;
    }
    else
    {
        //
        // Now we can display the UI on this thread
        //

        // Disable AP for this thread
        CAutoProtectWrapper APwrapper;
        APwrapper.DisableAPForThisThread();

        // Pass some essential information to the Common UI.
        m_spCommonUI->SetScanTime(m_ulScanTime);
        m_spCommonUI->SetMemoryScanStatus(m_MemScanStatus);
        m_spCommonUI->SetScanCompressedFiles(m_bScanCompressedFiles);
        m_spCommonUI->SetRepairProgress(m_bRepairProgress);
        m_spCommonUI->SetScanNonViralThreats(m_bScanNonViralThreats);
        m_spCommonUI->SetParentWindow(m_hWndParent);
        m_spCommonUI->SetQuickScan(m_bQuickScan);

        if (m_bEmailScan)
        {
            m_spCommonUI->SetEmailInfo(m_strSubject, m_strSender, m_strRecipient);
        }

        HRESULT hResult = m_spCommonUI->ShowCUIDlg(m_UIType,
                                                   m_spAnomalyList,
                                                   m_spSink,
                                                   m_lDamageControlItems);

        if (FAILED(hResult))
        {
            CCTRACEE(_T("CCommonUIThread::Run() - ShowCUIDlg Failed. HRESULT = 0x%X"), hResult);

            // Tell the the scanning thread to abort
            m_spSink->OnCUIAbort();

            // Start the scanning thread so it will abort
            m_spSink->OnCUIReady();

            result = SMRESULT_ERROR_COMMON_UI;
        }

        // Enable AP
        APwrapper.EnableAPForThisThread();
    }

    // Reset all members to be safe, since we don't need them anymore
    reset();

    return result;
}

HRESULT CCommonUIThread::SetParentWindow( HWND hWndParent )
{
    m_hWndParent = hWndParent;

    if( m_spCommonUI )
        return m_spCommonUI->SetParentWindow(hWndParent);

    return S_OK;
}

HRESULT CCommonUIThread::SetScanComplete(ccEraser::IAnomalyList* pAnomalyList, const long lDamageControlItemsRemaining )
{
    m_spAnomalyList = pAnomalyList;
    m_lDamageControlItems = lDamageControlItemsRemaining;

    if( m_spCommonUI )
        return m_spCommonUI->SetScanComplete(pAnomalyList, lDamageControlItemsRemaining);

    return S_OK;
}

HRESULT CCommonUIThread::SetEmailInfo(LPCTSTR szSubject, LPCTSTR szSender, LPCTSTR szRecipient)
{
    m_bEmailScan = true;

    if( szSubject )
        m_strSubject = szSubject;

    if( szSender )
        m_strSender = szSender;

    if( szRecipient )
        m_strRecipient = szRecipient;

    if( m_spCommonUI )
        return m_spCommonUI->SetEmailInfo(szSubject, szSender, szRecipient);

    return S_OK;
}

HRESULT CCommonUIThread::SetMemoryScanStatus(MemoryScanStatus MemScanStatus)
{
    m_MemScanStatus = MemScanStatus;

    if( m_spCommonUI )
        return m_spCommonUI->SetMemoryScanStatus(MemScanStatus);

    return S_OK;
}

HRESULT CCommonUIThread::SetScanCompressedFiles(bool bScanCompressedFiles)
{
    m_bScanCompressedFiles = bScanCompressedFiles;

    if( m_spCommonUI )
        return m_spCommonUI->SetScanCompressedFiles(bScanCompressedFiles);

    return S_OK;
}

HRESULT CCommonUIThread::SetScanTime(unsigned long ulScanTime)
{
    m_ulScanTime = ulScanTime;

    if( m_spCommonUI )
        return m_spCommonUI->SetScanTime(ulScanTime);

    return S_OK;
}

HRESULT CCommonUIThread::SetRepairProgress(bool bRepairProgress)
{
    m_bRepairProgress = bRepairProgress;

    if( m_spCommonUI )
        return m_spCommonUI->SetRepairProgress(bRepairProgress);

    return S_OK;
}

HRESULT CCommonUIThread::SetScanNonViralThreats(bool bScanNonViralThreats)
{
    m_bScanNonViralThreats = bScanNonViralThreats;

    if( m_spCommonUI )
        return m_spCommonUI->SetScanNonViralThreats(bScanNonViralThreats);

    return S_OK;
}

HRESULT CCommonUIThread::SetQuickScan(bool bQuickScan)
{
    m_bQuickScan = bQuickScan;

    if( m_spCommonUI )
        return m_spCommonUI->SetQuickScan(bQuickScan);

    return S_OK;
}

HRESULT CCommonUIThread::UpdateRepairProgress(unsigned long nItemsRemaining, bool bFailuresOccured, bool& bAbort)
{
    if( m_spCommonUI )
        return m_spCommonUI->UpdateRepairProgress(nItemsRemaining, bFailuresOccured, bAbort);

    CCTRACEE(_T("CCommonUIThread::UpdateRepairProgress() - No common UI."));
    return E_FAIL;
}

HRESULT CCommonUIThread::ShowMemTerminationDlg(bool &bTerminate, bool bEnableTimeout)
{
    if( m_spCommonUI )
        return m_spCommonUI->ShowMemTerminationDlg(bTerminate, bEnableTimeout);

    CCTRACEE(_T("CCommonUIThread::ShowMemTerminationDlg() - No common UI."));
    return E_FAIL;
}

