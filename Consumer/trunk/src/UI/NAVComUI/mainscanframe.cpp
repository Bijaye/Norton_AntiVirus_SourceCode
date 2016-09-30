////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Author:  Chirstopher Brown 11/08/2004

#include "stdafx.h"
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"

#include "ScanRepairRemoveWnd.h"
#include "ScanResultsWnd.h"
#include "MainScanFrame.h"
#include "NAVHelpLauncher.h"
#include "ccWebWnd_i.c"

#include "ScanConditionalActionDlg.h"
#include "ScanMemInfectionDlg.h"
#include "RiskDetailsDlg.h"
#include "HandleLowThreatsDlg.h"
#include "AnomalyHelper.h"

#include "NAVSettingsHelperEx.h"
#include "OptNames.h"
#include "CLTProductHelper.h"

#include <SIMON.h>

#if(WINVER < 0x0500)
    #define SPI_GETKEYBOARDCUES                 0x100A
    #define SPI_SETKEYBOARDCUES                 0x100B
    #define SPI_GETMENUUNDERLINES               SPI_GETKEYBOARDCUES
    #define SPI_SETMENUUNDERLINES               SPI_SETKEYBOARDCUES
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CMainScanFrame::CMainScanFrame() :
    m_eMemoryScanStatus(MemoryNotScanned),
    m_bScanCompressedFile(false),
    m_ulScanTime(0),
    m_bRepairProgress(false),
    m_bScanNonViralThreats(false),
    m_bScanSideEffects(false),
    m_bEmail(false),
    m_wndProgressView(this), 
    m_wndRepairRemoveView(this),
    m_wndScanRepairProgressView(this),
    m_wndScanResultsView(this),
    m_wndManualRemovalView(this),
    m_bFailureOccured(false),
    m_lIgnoreOnceCount(0),
    m_bAlertForDefaultExcludeItems(true)
{
    CNAVOptSettingsEx  NavOpts;
    DWORD dwValue;

    //
    // Check the scan response mode
    // 
    if (!(NavOpts.Init()) ||
        FAILED(NavOpts.GetValue(SCANNER_ResponseMode, dwValue, 0))) {
        CCTRACEE(_T("CMainScanFrame::CMainScanFrame() - Unable to read response mode option setting"));
        m_smrResponseMode = SCAN_MODE_AUTO_REPAIR;
    }
    else {
        m_smrResponseMode = (SMRESPONSE) dwValue;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CMainScanFrame::PreTranslateMessage(MSG* pMsg)
{
    if ( CSymFrameWindowImpl::PreTranslateMessage(pMsg) )
    {
        return TRUE;
    }

//                   TEMP HACK -- TEMP HACK -- TEMP HACK
//   The below code is causing TABbing problems...  Tabbing code was added to 
//   the DHTML windows to help fix this problem, however there's still a 
//   slight tabbing bug that needs to be resolved on the progress page.  
//   CBROWN 11/29/2004
//
//   Well, to me it looks like the code is *fixing* a tab problem that 
//   seems to occur in each window EXCEPT the one created last. 
//   Specifcally, the tabbing sequence takes you off into Neverland at
//   some point, before the next tab pulls you back to elements in the
//   window.  This happens on every window except which ever one was
//   create last.  Applying the below code to all of the other windows
//   seems to fix that problem on them, so I'm going with it for now.
//   MMW  08/02/2005
//
    if ( m_hWndClient == m_wndProgressView  ||
         m_hWndClient == m_wndRepairRemoveView ||
         m_hWndClient == m_wndScanResultsView )
    {
        if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) && 
             (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
        {
            return FALSE;
        }

        HWND hWndCtl = ::GetFocus();

        if ( IsChild(hWndCtl) )
        {
            // find a direct child of the dialog from the window that has focus
            while (::GetParent(hWndCtl) != m_hWnd)
            {
                hWndCtl = ::GetParent(hWndCtl);
            }

            // give control a chance to translate this message
            if (::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
            { 
                return TRUE;
            }
        }

    }

    return FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CMainScanFrame::OnIdle()
{
    return FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CMainScanFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, 
                                 LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CBrandingRes BrandRes;
    CString strFormat, strCaption;

    strFormat.LoadString(g_ResLoader.GetResourceInstance(), IDR_MAINFRAME);
    strCaption.Format(strFormat, BrandRes.ProductName());

    // Set title
    SetWindowText(strCaption);

    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);


    ////////////////////////////////////////////////////////////////
    ///// Creating COM objects that the other windows use
    HRESULT hr = S_OK;

    hr = m_spAppLauncher.CoCreateInstance( OLESTR("Symantec.Norton.AntiVirus.AppLauncher") );
    ATLASSERT( SUCCEEDED(hr) );

    if( FAILED(hr) ) {
        CCTRACEE( _T("CMainScanFrame::OnCreate() - Unable to create \"Symantec.Norton.AntiVirus.AppLauncher\" object.") );
        return -1;
    }

    // Find out if we should alert the user when the default action for
    // a threat is Exclude
    DWORD dwValue;
    UINT nTimeOut = 0;
    CNAVOptSettingsEx NavOpts;

    if ( NavOpts.Init() ) {
        if ( SUCCEEDED(NavOpts.GetValue(SCANNER_AlertForExclude, 
                                        dwValue, 1)) && 
             dwValue == 0 ) {
            CCTRACEI( _T("CMainScanFrame::OnCreate() - The never alert for exclude items value is set.") );
            m_bAlertForDefaultExcludeItems = false;
        }

        // Attempt to get the override progress timeout value
        if( SUCCEEDED(NavOpts.GetValue(_T("SCANNER:ProgressTimeOut"), dwValue, 0)) 
            && 
            dwValue != 0 ) {
            nTimeOut = static_cast<UINT>(dwValue);
        }
    }
    else {
        CCTRACEW( _T("CMainScanFrame::OnCreate() - Failed to initialize ccSettigngs helper class.") );
    }

    ///// Finished creating COM objects that the other windows use
    ////////////////////////////////////////////////////////////////

    const DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    HWND hWnd = 0;
    hWnd = m_wndProgressView.Create( m_hWnd, rcDefault, NULL, dwStyle, 0 );
    ATLASSERT( hWnd );

    if ( !hWnd ) {
        CCTRACEE( _T("CMainScanFrame::OnCreate() - Unable to create progress view.") );
        return -1;   
    }

    // Override the timeout value if specified to
    if ( nTimeOut > 0 ) {
        CCTRACEI( _T("CMainScanFrame::OnCreate() - The progress timeout value has been overrided to %ld."), nTimeOut );
        m_wndProgressView.OverrideProgressTimeOut(nTimeOut);
    }

    hWnd = m_wndRepairRemoveView.Create( m_hWnd, rcDefault, NULL, dwStyle, 0 );
    ATLASSERT( hWnd );

    if ( !hWnd ) {
        CCTRACEE( _T("CMainScanFame::OnCreate() - Unable to create repair and remove view.") );
        return -1;   
    }

    m_wndRepairRemoveView.Initialize(m_bEmail);

    hWnd = m_wndScanRepairProgressView.Create( m_hWnd, rcDefault, NULL, 
                                               dwStyle, 0 );
    ATLASSERT( hWnd );

    if ( !hWnd ) {
        CCTRACEE( _T("CMainScanFrame::OnCreate() - Unable to create repair progress view.") );
        return -1;   
    }

    hWnd = m_wndScanResultsView.Create( m_hWnd, rcDefault, NULL, dwStyle, 0 );
    ATLASSERT( hWnd );

    if ( !hWnd ) {
        CCTRACEE( _T("CMainScanFrame::OnCreate() - Unable to create scan results view.") );
        return -1;
    }

    hWnd = m_wndManualRemovalView.Create( m_hWnd, rcDefault, NULL, dwStyle, 
                                          0 );
    ATLASSERT( hWnd );

    if ( !hWnd ) {
        CCTRACEE( _T("CMainScanFrame::OnCreate() - Unable to create manual removal view.") );
        return -1;
    }

    m_wndManualRemovalView.Initialize(m_bEmail);

    //
    // Check if menu mnemonic's should be displayed...
    //
    BOOL bResult = FALSE;
    BOOL bShowMenuUnderline = FALSE;
    bResult = SystemParametersInfo( SPI_GETMENUUNDERLINES, 0, 
                                    (PVOID)&bShowMenuUnderline, 0 );

    if ( bResult && bShowMenuUnderline ) {
        SendMessageToDescendants( WM_COMMAND, ID_ALT_KEY );
    }

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CMainScanFrame::OnDestory(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

    
    bHandled = FALSE;
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CMainScanFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WM_CLOSE);
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CMainScanFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CMainScanFrame::OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    LPMINMAXINFO lpMinMax = reinterpret_cast<LPMINMAXINFO>( lParam );

    // Setting the scanners min and max frame size...
    lpMinMax->ptMaxTrackSize.x = lpMinMax->ptMinTrackSize.x = SCALEX(576);
    lpMinMax->ptMaxTrackSize.y = lpMinMax->ptMinTrackSize.y = SCALEY(432);

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::ShowCUIDlg( CommonUIType UIType, 
                                    ccEraser::IAnomalyList* pAnomalyList, 
                                    ICommonUISink* pSink, 
                                    const long lDamageControlItemsRemaining  )
{
    m_UIType = UIType;
    m_spCommUISink = pSink;

    ATLASSERT( m_spCommUISink );

    if( !m_spCommUISink )
        return E_FAIL;

    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    if( ! CreateEx( m_wndParent ) )
    {
        CCTRACEE(_T("CMainScanFrame::ShowCUIDlg() - Main window creation failed!"));
        return E_FAIL;
    }

    // Default to the Repair-Remove view
    CWindow wndSetActive = m_wndRepairRemoveView;

    if( m_UIType == CommonUI_RepairAndSummary ) // No progress mode
    {
        // The scan is complete at this point save off the anomaly list
        // passed in
        m_spAnomalyList = pAnomalyList;

        // If all anomalies have been fully processed show the results
        if ( false == 
              CAnomalyHelper::UnhandledAnomaliesRemain( m_spAnomalyList,
                                                        m_smrResponseMode ) )
        {
            wndSetActive = m_wndScanResultsView;
        }
    }
    else
    {
        wndSetActive = m_wndProgressView;
    }

    SetActiveView( wndSetActive );

    CenterWindow();

    ShowWindow( SW_NORMAL );

    //
    //  Since Common UI is really, really slow to load, it 
    //   is likely that user got frustrated and clicked on 
    //   another window, which would likely make the UI come
    //   up behind another window.  
    HWND hwndForeground = GetForegroundWindow();
    if(m_hWnd != hwndForeground)
    {
        SwitchToThisWindow(m_hWnd, TRUE);
    }

    // Prompt for damage control items if necessary
    if( lDamageControlItemsRemaining )
    {
        CScanConditionalActionDlg dlg( this, lDamageControlItemsRemaining, IDS_CONDITIONAL_SPYWARE, NAV_WARNING_NO_REMOVE );
        dlg.DoModal( m_hWnd );
    }

    int nRet = theLoop.Run();
    _Module.RemoveMessageLoop();

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetParentWindow( HWND hWndParent )
{
    m_wndParent = hWndParent;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetScanComplete( ccEraser::IAnomalyList* pAnomalyList, const long lDamageControlItemsRemaining  )
{
    // If the UI type is progress only then exit now
    if( m_UIType == CommonUI_ProgressOnly)
    {
        PostMessage( WM_CLOSE );
        return S_OK;
    }

    m_spAnomalyList = pAnomalyList;

    CWindow wndSetActive = m_wndRepairRemoveView;

    //
    // Determine whether all anomalies have been fully processed.  If scan 
    // mode is *not* manual (i.e. if the user has not selected to always 
    // "ask me what to do"), then exclude Review anomalies from 
    // consideration as remaining anomalies, and treat them as processed.
    // 
    if ( false == 
          CAnomalyHelper::UnhandledAnomaliesRemain( m_spAnomalyList, 
                                                    m_smrResponseMode ) ) 
    { 
        wndSetActive = m_wndScanResultsView;
    }

    SetActiveView( wndSetActive );

    // Prompt for damage control items if necessary
    if( lDamageControlItemsRemaining )
    {
        CScanConditionalActionDlg dlg( this, lDamageControlItemsRemaining, IDS_CONDITIONAL_SPYWARE, NAV_WARNING_REMOVE_RISK );
        dlg.DoModal( m_hWnd );
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetEmailInfo( LPCTSTR szSubject, LPCTSTR szSender, LPCTSTR szRecipient )
{
    m_bEmail = true;
    m_strEmailInfoSubject = szSubject;
    m_strEmailInfoSender = szSender;
    m_strEmailInfoRecipient = szRecipient;

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetMemoryScanStatus( MemoryScanStatus MemScanStatus )
{
    m_eMemoryScanStatus = MemScanStatus;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetScanCompressedFiles( bool bScanCompressedFiles )
{
    m_bScanCompressedFile = bScanCompressedFiles;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetScanTime( unsigned long ulScanTime )
{
    m_ulScanTime = ulScanTime;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetRepairProgress( bool bRepairProgress )
{
    m_bRepairProgress = bRepairProgress;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetScanNonViralThreats( bool bScanNonViralThreats )
{
    m_bScanNonViralThreats = bScanNonViralThreats;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetSideEffectsEnabled( bool bScanSideEffects )
{
    m_bScanSideEffects = bScanSideEffects;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::SetQuickScan(bool bQuickScan)
{
    m_bQuickScan = bQuickScan;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::UpdateRepairProgress(unsigned long nItemsRemaining, bool bFailuresOccured, bool& bAbort)
{
    // Check if the progress window is active...
    if( m_hWndClient != m_wndScanRepairProgressView )
        SetActiveView( m_wndScanRepairProgressView );

    // Remember the failures for this anomaly processing attempt
    m_bFailureOccured = bFailuresOccured;

    // Tell the window to update the progress, this will not return until the
    // progress has been updated
    m_wndScanRepairProgressView.OnUpdateRepairProgress(nItemsRemaining, bAbort);

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::ShowMemTerminationDlg( bool &bTerminate, bool bEnableTimeout )
{
    CScanMemInfectionDlg dlg( this, bEnableTimeout );

    m_wndProgressView.PauseKnightRiderBar();
    const int nResult = dlg.DoModal( m_hWnd );
    m_wndProgressView.ResumeKnightRiderBar();

    bTerminate = (IDYES == nResult);
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::ShowDetailsDlg(ccEraser::IAnomaly* pAnomaly, 
                                       HWND hWndParent)
{
    CRiskDetailsDlg dlg( this, pAnomaly, true, false );
    dlg.DoModal( hWndParent );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnAbortScan()
{
    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIAbort();
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnResumeScan()
{
    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIResume();
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnPauseScan()
{
    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIPause();
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnHelp( const long lHelpID )
{
    CCTRACEI(_T("CMainScanFrame::OnHelp() - Launching help with ID %ld"), 
             lHelpID);

    NAVToolbox::CNAVHelpLauncher Help;
    const HWND hWndResult = Help.LaunchHelp( lHelpID, NULL, false );

    if( !hWndResult ) {
        CCTRACEE(_T("CMainScanFrame::OnHelp() - Failed to launch Help"));
    }

    return !hWndResult ? E_FAIL : S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnReady()
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIReady();
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnFinished()
{
    PostMessage( WM_CLOSE );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnDetails()
{
    SetActiveView( m_wndRepairRemoveView );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnManualRemoval()
{
    SetActiveView( m_wndManualRemovalView );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnRepairRemoveCancel()
{
    SetActiveView( m_wndScanResultsView );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnRepairRemoveApply()
{
    ATLASSERT( m_spCommUISink );
    ATLASSERT( m_spAnomalyList );

    HRESULT hr = S_OK;

    // Reset the failure flag so we will know if failures occured during the
    // current anomaly processing
    m_bFailureOccured = false;

    if( m_bAlertForDefaultExcludeItems &&
        CAnomalyHelper::ExcludeRecommendationAccepted( m_spAnomalyList ) )
    {
        CHandleLowThreatsDlg dlg(m_bAlertForDefaultExcludeItems);

        if( IDYES != dlg.DoModal( m_hWnd ) )
        {
            return S_OK;
        }
    }

    if( CAnomalyHelper::AnomaliesNeedProcessing(m_spAnomalyList) )
    {
        hr = m_spCommUISink->OnCUIHandleAnomalies();
        ATLASSERT( SUCCEEDED(hr) );
    }
/*
    else if( lReviewItems > 0 )
    {
        // Prompt for the review items only
        long nThreatsDetected = NULL;
        long nNonViralCount = NULL;
        OnGetVirusCount( nThreatsDetected );
        OnGetNonViralCount( nNonViralCount );
        nThreatsDetected += nNonViralCount;

        CThreatsRemainingDlg dlg( this, lReviewItems, nThreatsDetected );
        dlg.DoModal( m_hWnd );

        if(CThreatsRemainingDlg::USER_ACTION_REVIEW == dlg.GetUserAction())
        {
            SetActiveView( m_wndRepairRemoveView );
        }
        else
        {
            // Process all the review and ignore items
            long lReviewItems = 0;
            long lIgnoreItems = 0;
            CAnomalyHelper::ProcessReviewAndIgnoreItems(m_spAnomalyList, true, lIgnoreItems, true, lReviewItems);

            // Add any ignore once items to our count
            m_lIgnoreOnceCount += lIgnoreItems;

            CCTRACEI(_T("CMainScanFrame::OnRepairRemoveApply() - Processed %d ignore items and %d review items. There are now a total of %d ignore items."), lIgnoreItems, lReviewItems, m_lIgnoreOnceCount);

            SetActiveView( m_wndScanResultsView );
        }
    }*/
    else
    {
        // Process all the ignore items
        long lReviewItems = 0;
        long lIgnoreItems = 0;
        CAnomalyHelper::ProcessReviewAndIgnoreItems(m_spAnomalyList, true, lIgnoreItems, false, lReviewItems);

        m_lIgnoreOnceCount += lIgnoreItems;

        CCTRACEI(_T("CMainScanFrame::OnRepairRemoveApply() - Processed %d ignore items. There are now a total of %d ignore items."), lIgnoreItems, m_lIgnoreOnceCount);

        SetActiveView( m_wndScanResultsView );
    }

    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetCurrentFolder( LPTSTR szFolder, long nLength )
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
	CString csBuffer;
	
	hr = m_spCommUISink->OnCUIGetCurrentFolder( csBuffer.GetBufferSetLength(nLength), nLength );
	csBuffer.ReleaseBuffer();

	_tcscpy(szFolder, csBuffer);

    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetScanFileCount( long& nScanFileCount )
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIScanFileCount( nScanFileCount );
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetVirusCount( long& nVirusCount )
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIVirusCount( nVirusCount );
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetRemovedVirusCount( long& nRemovedVirusCount )
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIRemovedVirusCount( nRemovedVirusCount );
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetNonViralCount( long& nNonViralCount )
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUINonViralCount( nNonViralCount );
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetRemovedNonViralCount( long& nRemovedNonViralCount )
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIRemovedNonViralCount( nRemovedNonViralCount );

    // Add the number of items that the CommonUI has set to ignored and processed
    // during this scan only
    nRemovedNonViralCount += m_lIgnoreOnceCount;

    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetAdditionalScan( bool& bAdditionalScan )
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIAdditionalScan( bAdditionalScan );
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetQuarantineCount(long& nQurantineCount)
{
    ATLASSERT( m_spCommUISink );

    HRESULT hr = S_OK;
    hr = m_spCommUISink->OnCUIGetQuarantinedCount( nQurantineCount );
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetAnomalyList( ccEraser::IAnomalyListPtr& pAnomalyList )
{
    pAnomalyList = m_spAnomalyList;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnRepairProgressComplete()
{
    //
    // If in "ask me" mode, and there are threats that are not "resolved",
    //  pop up a dialog box to ask the user if they want to continue
    //  resolving, or just ignore them.
    CWindow wndNextView = m_wndScanResultsView;

    // If a failure occured during the processing or there are anomalies that
    // are set to be processed then we will alert the user that items remain
    if ( CAnomalyHelper::UnhandledAnomaliesRemain( m_spAnomalyList, 
                                                   m_smrResponseMode ) )
    {
        wndNextView = m_wndRepairRemoveView;
    }

    SetActiveView( wndNextView );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetVirusDefSubscriptionExpired(bool& bVal)
{
    bVal = false;

    // Don't check when in safe mode
    const int nCleanBoot = GetSystemMetrics(SM_CLEANBOOT);

    if( !nCleanBoot )
    {
        DATE dateEnd = 0;
        bool bExpired = true;
        bool bShow = true;
        STAHLSOFT_HRX_TRY(hr)
        {
            CCLTProductHelper cltHelper;
            hrx << cltHelper.Initialize();

            hrx << cltHelper.IsSubscriptionExpired(bExpired);
            hrx << cltHelper.ShouldShowAlerts(bShow);

            CCTRCTXI2(_T("Subscription Expired: %d | Should Show Alerts: %d"), bExpired, bShow);

            if( bShow && bExpired )
                bVal = true;
        }
        STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
    }
    else
    {
        CCTRACEW(_T("CMainScanFrame::OnGetVirusDefSubscriptionExpired() - Not checking because we are in safe mode"));
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetAppLauncher( IAppLauncher** obj )
{
    HRESULT hr = E_FAIL;
    ATLASSERT( m_spAppLauncher );

    if( m_spAppLauncher )
    {
        *obj = m_spAppLauncher;
        (*obj)->AddRef();
        hr = S_OK;
    }
    else
    {
        CCTRACEE(_T("CMainScanFrame::OnGetAppLauncher() - No App launcher object."));
    }

    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnRiskDetailsDialog( CEZAnomaly & ezAnomaly, 
                                             bool bRiskAssessment )
{
    CRiskDetailsDlg dlg( this, ezAnomaly, bRiskAssessment, 
                         ezAnomaly.IsViral() );
    dlg.DoModal( m_hWnd );
    
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnIsEmailScan( bool& bVal )
{
    bVal = m_bEmail;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnGetEmailValues( LPCTSTR* szEmailInfoSubject, LPCTSTR* szEmailInfoSender, LPCTSTR* szEmailInfoRecipient )
{
    *szEmailInfoRecipient = m_strEmailInfoRecipient;
    *szEmailInfoSender = m_strEmailInfoSender;
    *szEmailInfoSubject = m_strEmailInfoSubject;

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CMainScanFrame::OnIsQuickScan( bool& bVal )
{
    bVal = m_bQuickScan;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CMainScanFrame::SetActiveView( const HWND hWnd )
{
    if( IsWindow() )
    {
        CWindow wndClient( m_hWndClient );
        CWindow wndGoingActive( hWnd );

        CRect rectClient;
        GetClientRect( rectClient );

        if( wndClient.IsWindow() )
            wndClient.SendMessage( UM_KILLACTIVE );

        wndGoingActive.SendMessage( UM_SETACTIVE );

        wndGoingActive.SetWindowPos( 0, rectClient, SWP_NOZORDER );

        if( wndClient.IsWindow() )
            wndClient.ShowWindow( SW_HIDE );

        wndGoingActive.ShowWindow( SW_NORMAL );
        wndGoingActive.SetFocus();

        m_hWndClient = hWnd;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CMainScanFrame::OnF1Help(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // Sending a user message to the active client window to get a help ID.
    // If not help id is sent back, default to the base help id.
    LRESULT lResult = ::SendMessage( m_hWndClient, UM_GET_HELP_ID, 0, 0 );

    // If the client returns -1, don't need to display help...
    if( -1 != lResult )
    {
        if( !lResult )
            lResult = IDH_NAVW_REPAIR_WIZARD_DLG_HELP_BTN;

        OnHelp( lResult );
    }

    return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CMainScanFrame::OnAlyKey(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    SendMessageToDescendants( WM_COMMAND, ID_ALT_KEY );
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CMainScanFrame::OnCtrlTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ::SendMessage( m_hWndClient, WM_COMMAND, ID_CTRLTAB, 0 );
    return 0;
}
