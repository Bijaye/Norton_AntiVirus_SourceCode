//******************************************************************************
// Copyright © 2005 Symantec Corporation.
// ---------------------------------------------------------------------------
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// All rights reserved.
//
// Author:  Christopher Brown 11/08/2004
//******************************************************************************

#include "stdafx.h"
#include "ScanProgressWnd.h"
#include "ConfirmDlg.h"
#include "NAVSettingsHelperEx.h"
#include "OptNames.h"
#include "navopt32.h"
#include "navinfo.h"

const int TIMEER_ID = 101;
const int TIMER_ELAPSE = 125;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CScanProgressWnd::CScanProgressWnd( CScanEventsSink* pSink ) :
    CBaseDHTMLWnd( _T("ScanProgress.htm") ),
    m_nTotalScanned(0),
    m_nThreatsDetected(0),
    m_nOtherThreatsDetected(0),
    m_bNoSpyware(false),
    m_bPaused(false),
	m_bAdditionalScan(false),
    m_spdispOtherThreats(NULL),
    m_nTimeOut(TIMER_ELAPSE)
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

void CScanProgressWnd::OverrideProgressTimeOut(UINT nTimeOut)
{
    m_nTimeOut = nTimeOut;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanProgressWnd::UpdateProgress()
{
    if ( !m_spdispScanned ) {
        GetElement(_T("TotalScanned"), &m_spdispScanned);
    }

    if ( !m_spdispThreats ) {
        GetElement(_T("ThreatsDeteced"), &m_spdispThreats);
    }

    if ( !m_spdispOtherThreats && !m_bNoSpyware ) {
        GetElement(_T("OtherThreatsDetected"), &m_spdispOtherThreats);
    }

    if ( !m_spdispCurrentItem ) {
        GetElement(_T("Currentitem"), &m_spdispCurrentItem);
    }

    CComVariant var;
    DISPPARAMS dispparams = {NULL, NULL, 1, 1};
    dispparams.rgvarg = &var;
    DISPID dispidPut = DISPID_PROPERTYPUT;
    dispparams.rgdispidNamedArgs = &dispidPut;

    ATL::CAtlString strVal;

    if ( m_spdispScanned ) {
        strVal.Format(_T("%ld"), m_nTotalScanned);
        var = (LPCTSTR)strVal;

        m_spdispScanned->Invoke( DISPID_IHTMLELEMENT_INNERTEXT, IID_NULL, 
                                 LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, 
                                 &dispparams, NULL, NULL, NULL);
    }
    else {
        // Error
        CCTRACEE(_T("CScanProgressWnd::UpdateProgress() - Failed to get the TotalScanned disp interface"));
    }

    if ( m_spdispThreats ) {
        strVal.Format(_T("%ld"), m_nThreatsDetected);
        var = (LPCTSTR)strVal;

        m_spdispThreats->Invoke( DISPID_IHTMLELEMENT_INNERTEXT, IID_NULL, 
                                 LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, 
                                 &dispparams, NULL, NULL, NULL);
    }
    else {
        // Error
        CCTRACEE(_T("CScanProgressWnd::UpdateProgress() - Failed to get the ThreatsDetected disp interface"));
    }

    if ( m_spdispOtherThreats ) {
        strVal.Format(_T("%ld"), m_nOtherThreatsDetected);
        var = (LPCTSTR)strVal;

        m_spdispOtherThreats->Invoke( DISPID_IHTMLELEMENT_INNERTEXT, IID_NULL, 
                                      LOCALE_USER_DEFAULT, 
                                      DISPATCH_PROPERTYPUT, 
                                      &dispparams, NULL, NULL, NULL);
    }
    else if ( !m_bNoSpyware ) {
        // Error
        CCTRACEE(_T("CScanProgressWnd::UpdateProgress() - Failed to get the OtherThreatsDetected disp interface"));
    }

    if ( m_spdispCurrentItem ) {
        var = (LPCTSTR)m_strCurrentFolder;

        m_spdispCurrentItem->Invoke( DISPID_IHTMLELEMENT_INNERTEXT, IID_NULL, 
                                     LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, 
                                     &dispparams, NULL, NULL, NULL);
    }
    else {
        // Error
        CCTRACEE(_T("CScanProgressWnd::UpdateProgress() - Failed to get the Currentitem disp interface"));
    }


    /*DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("TotalScanned"), m_nTotalScanned );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("ThreatsDeteced"), m_nThreatsDetected );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("OtherThreatsDetected"), m_nOtherThreatsDetected );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("Currentitem"), m_strCurrentFolder );*/
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanProgressWnd::OnDestory(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    if ( m_bPaused ) {
        Event_OnResumeScan();
    }

    EndProgressPolling();
    Event_OnAbortScan();

    bHandled = FALSE;
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanProgressWnd::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, 
                                  LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CString strCurrentFolder;

    long nTotalScanned = 0;
    long nThreatsDetected = 0;
    long nOtherThreatsDetected = 0;
    bool bAdditionalScan = false;

    Event_OnGetCurrentFolder( strCurrentFolder.GetBufferSetLength(MAX_PATH), 
                              MAX_PATH );
    strCurrentFolder.ReleaseBuffer();

    Event_OnGetScanFileCount( nTotalScanned );
    Event_OnGetVirusCount( nThreatsDetected );

    if (!m_bNoSpyware) {
        Event_OnGetNonViralCount( nOtherThreatsDetected );
    }

    if (!m_bAdditionalScan)  {
        Event_OnGetAdditionalScan( bAdditionalScan );

        if (bAdditionalScan) {
            DisplayElement( _T("AdditionalScan"), TRUE );
            m_bAdditionalScan = bAdditionalScan;
        }
    }

    if( nTotalScanned != m_nTotalScanned || 
        nThreatsDetected != m_nThreatsDetected || 
        (!m_bNoSpyware && nOtherThreatsDetected != m_nOtherThreatsDetected) || 
        strCurrentFolder != m_strCurrentFolder )
    {
        m_strCurrentFolder = strCurrentFolder;
        m_nTotalScanned = nTotalScanned;
        m_nThreatsDetected = nThreatsDetected;

        if ( !m_bNoSpyware ) {
            m_nOtherThreatsDetected = nOtherThreatsDetected;
        }

        UpdateProgress();
    }
    
    return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanProgressWnd::OnPauseBtnClick( IHTMLElement* pElement )
{
    if ( !m_bPaused ) {
        m_FakeProgress.Pause();

        DisplayElement( _T("spanPauseScan"), FALSE );
        DisplayElement( _T("spanResumeScan"), TRUE );

        // Disable the buttons until the pause is complete
        SetElementButtonEnabled( _T("StopScan"), FALSE );
        SetElementButtonEnabled( _T("ResumeScan"), FALSE );

        m_bPaused = true;
        EndProgressPolling();

        CCTRACEI("CScanProgressWnd::OnPauseBtnClick() - Requesting scan manager to pause.");
        Event_OnPauseScan();
        CCTRACEI("CScanProgressWnd::OnPauseBtnClick() - Scan manager has paused..");

        // Re-enable the buttons
        SetElementButtonEnabled( _T("StopScan"), TRUE );
        SetElementButtonEnabled( _T("ResumeScan"), TRUE );
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanProgressWnd::OnResumeBtnClick( IHTMLElement* pElement )
{
    if ( m_bPaused ) {
        m_FakeProgress.Resume();

        DisplayElement( _T("spanPauseScan"), TRUE );
        DisplayElement( _T("spanResumeScan"), FALSE );

        m_bPaused = false;
        StartProgressPolling();

        Event_OnResumeScan();
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanProgressWnd::OnStopBtnClick( IHTMLElement* pElement )
{
    HRESULT hr = S_OK;

    // Disable the buttons
    SetElementButtonEnabled( _T("StopScan"), FALSE );
    SetElementButtonEnabled( _T("PauseScan"), FALSE );
    SetElementButtonEnabled( _T("ResumeScan"), FALSE );

    // If the scan is not already paused we want to pause it now
    if ( !m_bPaused )
    {
        // Stop the animation
        PauseKnightRiderBar();

        CCTRACEI("CScanProgressWnd::OnStopBtnClick() - Requesting scan manager to pause.");
        // Signal the scan manager to pause
        Event_OnPauseScan();
        CCTRACEI("CScanProgressWnd::OnStopBtnClick() - Scan manager has paused.");

        // Stop polling for progress
        EndProgressPolling();
    }

    // Prompt to see that they really want to stop scanning
    CConfirmDlg dlg( IDS_HTML_COMFIRM_STOP_SCAN );
    const int nResult = dlg.DoModal();

    if( IDYES == nResult )
    {
        // Signal the scan manager to abort
        Event_OnAbortScan();

        // Resume the scan so it can abort
        Event_OnResumeScan();
    }
    else
    {
        // Re-enable the buttons
        SetElementButtonEnabled( _T("StopScan"), TRUE );
        SetElementButtonEnabled( _T("PauseScan"), TRUE );
        SetElementButtonEnabled( _T("ResumeScan"), TRUE );

        // If the scan was not previously paused then we want to resume it now
        if ( !m_bPaused ) {
            // Re-start the progress
            ResumeKnightRiderBar();
            StartProgressPolling();

            // Resume the scan manager
            Event_OnResumeScan();
        }
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanProgressWnd::OnMoreInfoBtnClick( IHTMLElement* pElement )
{
    Event_OnHelp( IDH );
    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanProgressWnd::OnGetHelpID()
{
    return -1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanProgressWnd::OnAltKey()
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
LRESULT CScanProgressWnd::OnKillActive()
{
    if (m_FakeProgress.m_hWnd) {
       m_FakeProgress.DestroyWindow();
    }

    EndProgressPolling();

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanProgressWnd::OnSetActive()
{

    StartProgressPolling();

    // Format text that contains product name
    CString strText;
    CString strFormat;

    strFormat.LoadString(g_ResLoader.GetResourceInstance(), 
                         IDS_ADDITIONAL_SCAN);
    strText.Format(strFormat, m_strProductName);

    SetImgSrc( _T("AdditionalScanImage"), _S(IDS_ADDITIONAL_SCAN_IMAGE) );
    SetElementText( _T("AdditionalScanText"), strText );
    DisplayElement( _T("AdditionalScan"), FALSE );

    if (m_bNoSpyware) {
        SetElementText( _T("TitleDescription"), 
                        _S(IDS_SCAN_PROGRESS_TITLE_LABEL_NOSPYWARE) );
        DisplayElement( _T("RisksLabel"), FALSE );
        DisplayElement( _T("OtherThreatsDetected"), FALSE );
        DisplayElement( _T("RisksSpacer"), FALSE );
    }
    else {
        SetElementText( _T("TitleDescription"), 
                        _S(IDS_SCAN_PROGRESS_TITLE_LABEL) );
        SetElementText( _T("RisksLabel"), 
                        _S(IDS_SCAN_PROGRESS_RISKS_LABEL) );
    }

    CRect rect;
    GetClientRect( rect );

    rect.left += 10;
    rect.top += 170;
    rect.right -= 155;
    rect.bottom = rect.top + 20;

    m_FakeProgress.Create(m_hWnd, rect, NULL, 
                          WS_CHILD | WS_VISIBLE | WS_EX_STATICEDGE);

    Event_OnReady();

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanProgressWnd::StartProgressPolling()
{
    SetTimer( TIMEER_ID, m_nTimeOut, 0 );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanProgressWnd::EndProgressPolling()
{
    KillTimer( TIMEER_ID );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanProgressWnd::PauseKnightRiderBar()
{
    m_FakeProgress.Pause();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanProgressWnd::ResumeKnightRiderBar()
{
    m_FakeProgress.Resume();
}


