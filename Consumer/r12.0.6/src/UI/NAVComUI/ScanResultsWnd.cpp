//******************************************************************************
// Copyright © 2004 Symantec Corporation.
// ---------------------------------------------------------------------------
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// All rights reserved.
//
// Author:  Chirstopher Brown 11/08/2004
//******************************************************************************

#include "stdafx.h"
#include "ScanResultsWnd.h"
#include "AnomalyHelper.h"
#include "ScanConditionalActionDlg.h"
#include "NAVDefutilsLoader.h"
#include "atltime.h"
#include "NAVSettingsHelperEx.h"
#include "OptNames.h"
#include "navopt32.h"
#include "navinfo.h"
#include "EZEraserObjects.h"

#define NAVAPPIDS_BUILDING
#include "NAVAppIDs.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CScanResultsWnd::CScanResultsWnd( CScanEventsSink* pSink ) :
    CBaseDHTMLWnd( _T("ScanResults.htm") ),
    m_nTotalFilesScanned(0),
    m_nThreatsDetected(0),
    m_nThreatsDeletedRemoved(0),
    m_nThreatsRemaining(0),
    m_nThreatsQuarantined(0),
    m_bLaunchLiveUpdate(false),
    m_bDefsOldOrExpired(false),
    m_bQuarantineNoteApplies(false),
    m_bNoSpyware(false),
    m_pSink(pSink)
{
    RegisterEventSink( pSink );

    DWORD  dwValue = 0;
    CNAVOptSettingsEx  NavOpts;

    if (NavOpts.Init()) {
        if (SUCCEEDED(NavOpts.GetValue(THREAT_NoThreatCat, dwValue, 0)) && 
            (0 != dwValue)) {
            m_bNoSpyware = true;
        }
    }
    else {
        CCTRACEE("Unable to initialize the options library.");

        // If in safe mode, ccSettings is not available, so get 
        // THREAT_NoThreatCat from NAVOpts.dat.
        if (0 != GetSystemMetrics(SM_CLEANBOOT)) {
            CCTRACEI(_T("%s - Running in safe mode."), __FUNCTION__);

            TCHAR szNAVOpts[MAX_PATH] = {0};
            HNAVOPTS32 hOptions = NULL;
            DWORD dwSize = MAX_PATH;
            DWORD dwValue = 0;
            HKEY hKey = NULL;

            CNAVInfo NAVInfo;
            _tcscpy(szNAVOpts, NAVInfo.GetNAVOptPath());

            NAVOPTS32_STATUS Status = NavOpts32_Allocate(&hOptions);

            if (NAVOPTS32_OK == Status) {
                // initialize the NAVOpts.dat option file.
                Status = NavOpts32_Load(szNAVOpts, hOptions, true);

                if (NAVOPTS32_OK == Status) {
                    Status = NavOpts32_GetDwordValue(hOptions, 
                                                     THREAT_NoThreatCat, 
                                                     &dwValue, 0);

                    if (NAVOPTS32_OK == Status) {
                        m_bNoSpyware = (0 != dwValue);
                        CCTRACEI(_T("NoThreatCat value=%s"), __FUNCTION__, 
                                 dwValue);
                    }
                }
                else {
                    CCTRACEE(_T("%s - Unable to load the NAVOpts.dat option file."), __FUNCTION__);
                }
            }
            else {
                CCTRACEE(_T("%s - Unable to initialize options library."), 
                         __FUNCTION__);
            }

            // Clean up
            if (hOptions) {
                NavOpts32_Free(hOptions);
            }
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CScanResultsWnd::DoDataExchange(BOOL bSaveAndValidate, UINT nCtlID )
{
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("TotalFilesScanned"), 
                                m_nTotalFilesScanned );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("ThreatsDetected"), 
                                m_nThreatsDetected );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("ThreatsRepairedRemoved"),
                                m_nThreatsDeletedRemoved );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("ThreatsRemaining"), 
                                m_nThreatsRemaining );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("ThreatsQuarantined"), 
                                m_nThreatsQuarantined );

    return TRUE;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanResultsWnd::OnDetailsBtnClick( IHTMLElement* pElement )
{
    Event_OnDetails();
    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanResultsWnd::OnManualRemovalBtnClick( IHTMLElement* pElement )
{
    Event_OnManualRemoval();
    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanResultsWnd::OnQuarantineBtnClick( IHTMLElement* pElement )
{
    CComPtr< IAppLauncher > spAppLauncher;

    HRESULT hr = S_OK;
    hr = Event_OnGetAppLauncher( &spAppLauncher );

    if( SUCCEEDED(hr) && spAppLauncher )
    {
        hr = spAppLauncher->LaunchQuarantine();
        ATLASSERT( SUCCEEDED(hr) );

        if( FAILED(hr) )
        {
            CCTRACEE(_T("LaunchQuarantine returned with a failure."));
        }
    }

    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanResultsWnd::OnFinishedBtnClick( IHTMLElement* pElement )
{
    // Do we need to prompt for reboot required?
    if ( CAnomalyHelper::NeedReboot(m_spAnomalyList) )
    {
        CScanConditionalActionDlg dlg( m_pSink, 0, IDS_REBOOT_WARNING, 
                                       IDH_NAVW_REPAIR_WIZARD_DLG_HELP_BTN );
        dlg.DoModal( m_hWnd );
    }

    Event_OnFinished();
    return S_OK;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanResultsWnd::OnAltKey()
{
    CComPtr< IHTMLRuleStyle > spStyle;

    HRESULT hr = S_OK;
    hr = GetStyleSheetRule( ".hotkey", &spStyle );

    if( SUCCEEDED(hr) )
    {
        _bstr_t bstrDecoration = _T("underline");
        hr = spStyle->put_textDecoration( bstrDecoration );
    } 

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanResultsWnd::OnGetHelpID()
{
    return IDH;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanResultsWnd::OnKillActive()
{
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanResultsWnd::OnSetActive()
{
    // Get the anomaly list
    if( !m_spAnomalyList ) {
        Event_OnGetAnomalyList( m_spAnomalyList );
    }

    long nNonViralCount = 0;
    long nNonViralRemovedCount = 0;

    size_t nRemainingManualRemovalCount = 0;

    
    // Get the total number of files scanned
    Event_OnGetScanFileCount( m_nTotalFilesScanned );

    // Get the total number of threats detected, both viral and non-viral
    Event_OnGetVirusCount( m_nThreatsDetected );
    Event_OnGetNonViralCount( nNonViralCount );
    m_nThreatsDetected += nNonViralCount;

    // Get the total number of threats removed, both viral and non-viral
    Event_OnGetRemovedVirusCount( m_nThreatsDeletedRemoved );
    Event_OnGetRemovedNonViralCount( nNonViralRemovedCount );
    m_nThreatsDeletedRemoved += nNonViralRemovedCount;

    // Compute the number of remaining threats
    m_nThreatsRemaining = (m_nThreatsDetected - m_nThreatsDeletedRemoved);

    
    if ( m_nThreatsRemaining ) {
        // Get number of manual removal threats
        nRemainingManualRemovalCount = 
             CAnomalyHelper::GetRemainingManualRemovalCount( m_spAnomalyList );
    } 

    // Get the number of viral threats that were quarantined
    Event_OnGetQuarantineCount( m_nThreatsQuarantined );

    // Get the anomaly count
    size_t nCount = CAnomalyHelper::GetAnomalyCount(m_spAnomalyList);

    bool bQuickScan = false;
    Event_OnIsQuickScan(bQuickScan);

    ANOMALYLISTSTATE eListState = 
                             CAnomalyHelper::AnomalyListState(m_spAnomalyList);

    // Set focus to the finish button
    SetFocusToElement(_T("Finished"));

    // No items detected hide the details button
    if ( nCount == 0 ) {
        // Hide the details button and span if nothing was detected
        //SetElementClassName( _T("spanDetailsButton"), _T("hiddenButton") );
        //SetElementClassName( _T("Details"), _T("hiddenButton") );
        DisplayElement(_T("ViewDetailsText"), FALSE);
        
        if ( !m_bDefsOldOrExpired ) {
            // Set to green state
            SetElementClassName( _T("ScanState"), _T("GreenScanState") );

            if ( bQuickScan ) {
                SetElementClassName( _T("description"), 
                                     _T("QuickScanDescription") );

                if (m_bNoSpyware) {
                    SetElementText( 
                                 _T("description"), 
                                 _S(IDS_QUICKSCANGREENTEXTNOITEMS_NOSPYWARE) );
                }
                else {
                    SetElementText( _T("description"), 
                                    _S(IDS_QUICKSCANGREENTEXTNOITEMS) );
                }
            }
            else {
                if (m_bNoSpyware) {
                    SetElementText( _T("description"), 
                                    _S(IDS_GREENTEXTNOITEMS_NOSPYWARE) );
                }
                else {
                    SetElementText( _T("description"), 
                                    _S(IDS_GREENTEXTNOITEMS) );
                }
            }
        }
    }
    else if ( eListState == LISTSTATE_GREEN  ) {
        if( !m_bDefsOldOrExpired ) {
            // We have processed all items successfully
            SetElementClassName( _T("ScanState"), _T("GreenScanState") );

            if( bQuickScan ) {
                SetElementClassName( _T("description"), 
                                     _T("QuickScanDescription") );
                SetElementText( _T("description"), 
                                _S(IDS_PROCESSED_ALL_SUCCESSFULLY_QUICKSCAN) );
            }
            else {
                SetElementText( _T("description"), 
                                _S(IDS_PROCESSED_ALL_SUCCESSFULLY) );
            }
        }
    }
    else if ( eListState == LISTSTATE_BLUE_REMAINING || 
              eListState == LISTSTATE_BLUE_RESOLVED )
    {
        if( !m_bDefsOldOrExpired ) {
            // Only non-virals are remaining
            SetElementClassName( _T("ScanState"), _T("BlueScanState") );
            
            if( eListState == LISTSTATE_BLUE_REMAINING ) {
                SetElementText( _T("description"), _S(IDS_RED_TEXT) );
            }
            else if( bQuickScan ) {
                SetElementClassName( _T("description"), 
                                     _T("QuickScanDescription") );
                SetElementText( _T("description"), 
                                _S(IDS_PROCESSED_ALL_SUCCESSFULLY_QUICKSCAN) );
            }
            else {
                SetElementText( _T("description"), 
                                _S(IDS_PROCESSED_ALL_SUCCESSFULLY) );
            }
        }
    }
    else { //if( eListState == LISTSTATE_RED )
        if ( !m_bDefsOldOrExpired ) {
            // There are items left to process or items left that we are
            // unable to process
            SetElementClassName( _T("ScanState"), _T("RedScanState") );
            SetElementText( _T("description"), _S(IDS_RED_TEXT) );
        }
    }

    if ( nRemainingManualRemovalCount > 0 ) {
        //
        // There were manual removal items.  The manual reomval text
        // gets priority of quarantine text.
        //
        DisplayElement(_T("tGeneralNotes"), TRUE);
        DisplayElement(_T("ManualRemoval"), TRUE);
        DisplayElement(_T("ManualRemovalButton"), TRUE);
        DisplayElement(_T("LaunchQuarantine"), FALSE);

        if ( nRemainingManualRemovalCount > 1 ) {
            SetElementText( _T("GeneralNotesText"), 
                            _S(IDS_RESULTS_NOTE_ITEMS_REMAINING) );
        }
        else {
            SetElementText( _T("GeneralNotesText"), 
                            _S(IDS_RESULTS_NOTE_ITEMS_REMAINING_SINGULAR) );
        }

        SetElementText( _T("ManualRemovalText"), _S(IDS_RESULTS_NOTE_FIX_NOW) );
        SetElementText( _T("QuarantineNameText"), "");
    }
    else {
        //
        // No manual removal items, so Definitely not going to display 
        // manual removal message.  Need to decide whether to display 
        // quarantine message.
        //
        DisplayElement(_T("ManualRemoval"), FALSE);
        DisplayElement(_T("ManualRemovalButton"), FALSE);
        SetElementText( _T("ManualRemovalText"), "");

        if ( m_bQuarantineNoteApplies ) {
            //
            // Quarantine note was already displayed on a previous viewing
            // of this screen
            //
            CString strFormat;
            CString strName;
            strFormat.LoadString(g_ResLoader.GetResourceInstance(), 
                                 IDS_QUARANTINE_NAME);
            strName.Format( strFormat, m_strProductName );
            CComBSTR bstrName( strName );      

            DisplayElement( _T("tGeneralNotes"), TRUE );
            DisplayElement( _T("LaunchQuarantine"), TRUE );
            SetElementText( _T("GeneralNotesText"), 
                            _S(IDS_USE_QUARANTINE_NOTE) );
            SetElementText( _T("QuarantineNameText"), bstrName);
        }
        else {
            //
            // Default to not displaying quarantine message
            //
            DisplayElement(_T("tGeneralNotes"), FALSE);
            DisplayElement(_T("LaunchQuarantine"), FALSE);

            // 
            // Check each anomaly in the anomaly list to see if it's a 
            // quarantined (removed) non-viral.
            //
            for ( size_t nCur = 0; nCur < nCount; nCur++ ) {
                ccEraser::IAnomalyPtr pAnomaly;
                ccEraser::eResult eRes = m_spAnomalyList->GetItem(nCur, 
                                                                  pAnomaly);

                if ( ccEraser::Failed(eRes)) {
                    CCTRACEE( _T("CScanResultsWnd::OnSetActive() - Failed to get anomaly %d from the list; eResult = %d"), nCur, eRes);
                    continue;
                }

                CEZAnomaly ezAnomaly(pAnomaly);

                if ( !(ezAnomaly.IsViral()) ) {
                    AnomalyAction eAction;
                    CommonUIInfectionStatus eStatus;

                    if (!ezAnomaly.GetAnomalyStatus(eAction, eStatus) ) {
                        CCTRACEE( _T("CScanResultsWnd::OnSetActive() - Failed to get the status anomaly %d"), nCur);
                        continue;
                    }

                    if (InfectionStatus_Deleted == eStatus) {
                        //
                        // Found a quarantined (removed) non-viral.  
                        // Display the quarantine mesasge
                        // 
                        CString strFormat;
                        CString strName;
                        strFormat.LoadString(g_ResLoader.GetResourceInstance(), 
                                             IDS_QUARANTINE_NAME);
                        strName.Format( strFormat, m_strProductName );
                        CComBSTR bstrName( strName );      

                        DisplayElement( _T("tGeneralNotes"), TRUE );
                        DisplayElement( _T("LaunchQuarantine"), TRUE );
                        SetElementText( _T("GeneralNotesText"), 
                                        _S(IDS_USE_QUARANTINE_NOTE) );
                        SetElementText( _T("QuarantineNameText"), bstrName);

                        m_bQuarantineNoteApplies = true;
  
                        break;
                    } // anomaly was deleted
                } // anomaly was non-viral.
            } // for each anomaly in the list

            if ( !m_bQuarantineNoteApplies ) {
                if ( bQuickScan ) {
                    DisplayElement( _T("tGeneralNotes"), TRUE );
                    SetElementText( _T("GeneralNotesText"), 
                                    _S(IDS_RESULTS_NOTE_QUICKSCAN) );
                }
                else {
                    // No items in quarantine. Don't show anything, 
                    DisplayElement(_T("tGeneralNotesImage"), FALSE);
                    DisplayElement(_T("tGeneralNotes"), FALSE);
                }
            }
        } // Quarantine note was not previously displayed
    } 

    if (m_bNoSpyware) {
        SetElementText( _T("RisksLabel"), 
                        _S(IDS_SCAN_RESULTS_RISKS_LABEL_NOSPYWARE) );
    }
    else {
        SetElementText( _T("RisksLabel"), _S(IDS_SCAN_RESULTS_RISKS_LABEL) );
    }

    //
    // Display quarantine button only in safe mode.
    //
    if (::GetSystemMetrics(SM_CLEANBOOT)) {
        CCTRACEI("CScanResultsWnd::OnSetActive() - Detected system is in safe mode");
        DisplayElement(_T("QuarantineButton"), TRUE);
    }
    else {
        DisplayElement(_T("QuarantineButton"), FALSE);
    }

    DoDataExchange( FALSE );

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanResultsWnd::OnMoreInfoBtnClick( IHTMLElement* pElement )
{
    Event_OnHelp( IDH );
    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
void CScanResultsWnd::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    __super::OnDocumentComplete(pDisp,szUrl);

    HRESULT hr = S_OK;

    // Set the correct title text
    bool bEmail = false;
    Event_OnIsEmailScan( bEmail );

    bool bQuickScan = false;
    Event_OnIsQuickScan( bQuickScan );

    UINT nTitleRes = IDS_SCAN_RESULTS_NORMAL_TITLE;
    if ( bEmail )
    {
        nTitleRes = IDS_SCAN_RESULT_EMAIL_TITLE;
    }
    else if ( bQuickScan )
    {
        nTitleRes = IDS_QUICKSCAN_TITLE;
    }

    SetElementText( _T("title"), _S(nTitleRes) );

    // Check for expired subscription
    bool bExpired = false;
    hr = Event_OnGetVirusDefSubscriptionExpired( bExpired );
    ATLASSERT( SUCCEEDED(hr) );

    if ( FAILED(hr) )
    {
        CCTRACEE(_T("CScanResultsWnd::OnDocumentComplete() - Failed to get subscription expired state."));
    }

    if ( SUCCEEDED(hr) && bExpired )
    {
        CCTRACEI(_T("CScanResultsWnd::OnDocumentComplete() - Subscription is in expired state."));
        CComPtr< INAVLicenseInfo > spNAVLicenseInfo;
        hr = Event_OnGetNAVLicenseInfo( &spNAVLicenseInfo );
        ATLASSERT( SUCCEEDED(hr) );

        if ( FAILED(hr) )
        {
            CCTRACEE(_T("CScanResultsWnd::OnDocumentComplete() - Failed to get license info object."));
        }

        if ( spNAVLicenseInfo )
        {
            int nResID = IDS_RENEW_SUBCRIPTION_NONADMIN;

            BOOL bCanRenew = FALSE;
            hr = spNAVLicenseInfo->get_CanRenewSubscription( &bCanRenew );
            ATLASSERT( SUCCEEDED(hr) );

            if ( FAILED(hr) )
            {
                CCTRACEE(_T("CScanResultsWnd::OnDocumentComplete() - Failed spNAVLicenseInfo->get_CanRenewSubscription( &bCanRenew )."));
            }

            if ( SUCCEEDED(hr) && bCanRenew )
            {
                nResID = IDS_RENEW_SUBCRIPTION_ADMIN;
            }

            // Format text that contains product name
            CString strFormat;
            CString strDesc;
            strFormat.LoadString(g_ResLoader.GetResourceInstance(), nResID);
            strDesc.Format(strFormat, m_strProductName);
            CComBSTR bstrHTML(strDesc);

            SetElementClassName( _T("description"), _T("DefsOldDescription") );
            SetElementHtml( _T("description"), bstrHTML );

            // Make the top panel red
            SetElementClassName( _T("ScanState"), _T("RedScanState") );

            m_bLaunchLiveUpdate = false;
            m_bDefsOldOrExpired = true;
        }

        return;
    }

    // Check for old definitions
    DefUtilsLoader DefUtilsLoader;
    IDefUtilsPtr pIDefUtils;
    if (SYM_SUCCEEDED(DefUtilsLoader.CreateObject(pIDefUtils.m_p)) )
    {
        if ( pIDefUtils->InitWindowsApp(g_szNavAppIdNAVW32) )
        {
            // Try to use the newest defs.  If this fails, it's probably 
            // because there is a new set of defs on the machine that has 
            // not been integrated yet, but the current process doesn't 
            // have rights to the hawkings tree.
            // In this case, just use the current defs.
            bool bNewer = true;
            WORD wYear = 0, wMonth = 0, wDay = 0;
            DWORD dwRev = 0;
            char szDefsDir[MAX_PATH] = {0};

            if ( pIDefUtils->UseNewestDefs( &bNewer ) == FALSE )
            {
                pIDefUtils->GetCurrentDefs( szDefsDir, MAX_PATH );
            }

            // get the definitions date
            if ( pIDefUtils->GetCurrentDefsDate( &wYear, &wMonth, &wDay, 
                                                 &dwRev ) )
            {
                ATL::CTime timeDefTime( wYear, wMonth, wDay, 0, 0, 0 );

                // get the current time
                ATL::CTime timeCurrentTime = CTime::GetCurrentTime();

                // get the difference in time
                ATL::CTimeSpan span = timeCurrentTime - timeDefTime;

                CCTRACEI(_T("CScanResultsWnd::OnDocumentComplete() - Defs age %d."), span.GetDays());

                // Are the defs older than 14 days?
                if ( span.GetDays() > 14 )
                {
                    SetElementClassName( _T("description"), 
                                         _T("DefsOldDescription") );

                    _bstr_t bstrHTML;

                    if (m_bNoSpyware) 
                    {
                        bstrHTML = _S(IDS_DEFS_OLD_NOSPYWARE);
                    }
                    else
                    {
                        bstrHTML = _S(IDS_DEFS_OLD);
                    }
                    SetElementHtml( _T("description"), bstrHTML );

                    // Make the top panel red
                    SetElementClassName( _T("ScanState"), _T("RedScanState") );

                    m_bLaunchLiveUpdate = true;
                    m_bDefsOldOrExpired = true;
                }
            }
            else
            {
                DURESULT dRes = pIDefUtils->GetLastResult();
                CCTRACEE(_T("CScanResultsWnd::OnDocumentComplete() - IDefUtils::GetCurrentDefsDate() failed. Result = %d"), dRes);
            }
        }
        else
        {
            CCTRACEE(_T("CScanResultsWnd::OnDocumentComplete() - IDefUtils::InitWindowsApp() failed."));
        }
    }

    return;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
HRESULT CScanResultsWnd::OnRenewSubscriptionBtnClick( IHTMLElement* pElement )
{
    CComPtr< IAppLauncher > spAppLauncher;
    
    HRESULT hr = S_OK;
    hr = Event_OnGetAppLauncher( &spAppLauncher );

    if( SUCCEEDED(hr) && spAppLauncher )
    {
        if( m_bLaunchLiveUpdate )
            hr = spAppLauncher->LaunchLiveUpdate();
        else
            hr = spAppLauncher->LaunchSubscriptionWizard();

        ATLASSERT( SUCCEEDED(hr) );

        if( FAILED(hr) )
        {
            CCTRACEE(_T("CScanResultsWnd::OnRenewSubscriptionBtnClick() - Launch %s returned with a failure."), m_bLaunchLiveUpdate ? _T("LiveUpdate") : _T("Subscription Wizard") );
        }
    }

    return S_FALSE; // don't need the IE clicking sound...
}

