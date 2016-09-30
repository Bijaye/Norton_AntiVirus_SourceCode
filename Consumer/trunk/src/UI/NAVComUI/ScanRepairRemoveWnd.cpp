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
#include "DHTMLWndCtrlview.h"
#include "ScanRepairRemoveWnd.h"
#include "commonuiinterface.h"
#include "StartBrowser.h"
#include "NAVInfo.h"
#include "ccResourceLoader.h"
#include "AnomalyHelper.h"
#include "Utils.h"

#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CScanRepairRemoveWnd::CScanRepairRemoveWnd( CScanEventsSink* pSink ) :
    CBaseDHTMLWnd( _T("ScanRepairRemove.htm") ),
    m_bTab1Visible(FALSE),
    m_bTab2Visible(FALSE),
    m_bEmailScan(false),
    m_spAxSysListView(NULL)
{
    RegisterEventSink( pSink );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CScanRepairRemoveWnd::DoDataExchange( BOOL bSaveAndValidate, UINT nCtlID )
{
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("tab1"), m_strTab1Text );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("tab1HC"), m_strTab1Text );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("tab2"), m_strTab2Text );
    DDX_DHtml_ElementInnerText( bSaveAndValidate, _T("tab2HC"), m_strTab2Text );
    return TRUE;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnMoreInfoBtnClick( IHTMLElement* pElement )
{
    Event_OnHelp( IDH );
    return S_FALSE;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnApplyBtnClick( IHTMLElement* pElement )
{
    Event_OnRepairRemoveApply();
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnCancelBtnClick( IHTMLElement* pElement )
{
    Event_OnRepairRemoveCancel();
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnTab1BtnClick( IHTMLElement* pElement )
{
    ChangeTab( TAB1 );
    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnTab2BtnClick( IHTMLElement* pElement )
{
    ChangeTab( TAB2 );
    return S_FALSE;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::ChangeTab( TABS dwTab )
{
    LPCTSTR szClassNameTab1 = _T("tab");
    LPCTSTR szClassNameTab2 = _T("tab");
    LPCTSTR szClassNameTabHC1 = _T("tabHC");
    LPCTSTR szClassNameTabHC2 = _T("tabHC");

    IAxSysListView32_Filter eFilter;
    CString strResourceTemp;

    switch( dwTab )
    {
        case TAB1:
            CCTRACEI( _T("CScanRepairRemoveWnd::ChangeTab() - Changing to the threats remaining tab.") );
            eFilter = IAxSysListView32_Filter_Infected;
            szClassNameTab1 = _T("tab tabSelected");
            szClassNameTabHC1 = _T("tabHC tabSelectedHC");

            if ( m_bEmailScan ) {
                CComBSTR bStrHtml;
                g_ResLoader.LoadString(IDS_REMAINING_RISK_DESCRIPTION_EMAIL,
                                       strResourceTemp);
                bStrHtml = strResourceTemp;
                SetElementHtml( _T("tabDescription"), bStrHtml );
                SetElementHtml( _T("tabDescriptionHC"), bStrHtml );
            }
            else {
                SetElementText( _T("tabDescription"), 
                                _S(IDS_REMAINING_RISK_DESCRIPTION) );
                SetElementText( _T("tabDescriptionHC"), 
                                _S(IDS_REMAINING_RISK_DESCRIPTION_HC) );
            }

            break;

        case TAB2:
            CCTRACEI( _T("CScanRepairRemoveWnd::ChangeTab() - Changing to the threats resolved tab.") );
            eFilter = IAxSysListView32_Filter_Repaired;
            szClassNameTab2 = _T("tab tabSelected");
            szClassNameTabHC2 = _T("tabHC tabSelectedHC");

            if ( m_bEmailScan ) {
                CComBSTR bStrHtml;
                g_ResLoader.LoadString(IDS_REPAIRED_RISK_DESCRIPTION_EMAIL,
                                       strResourceTemp);
                bStrHtml = strResourceTemp;
                //bStrHtml.LoadString(IDS_REPAIRED_RISK_DESCRIPTION_EMAIL);
                SetElementHtml( _T("tabDescription"), bStrHtml );
                SetElementHtml( _T("tabDescriptionHC"), bStrHtml );
            }
            else {
                SetElementText( _T("tabDescription"), 
                                _S(IDS_REPAIRED_RISK_DESCRIPTION) );
                SetElementText( _T("tabDescriptionHC"), 
                                _S(IDS_REPAIRED_RISK_DESCRIPTION) );
            }

            break;

        default:
            ATLASSERT( false );
            break;
    }
    
    SetElementClassName( _T("tab1"), szClassNameTab1 );
    SetElementClassName( _T("tab2"), szClassNameTab2 );
    SetElementClassName( _T("tab1HC"), szClassNameTabHC1 );
    SetElementClassName( _T("tab2HC"), szClassNameTabHC2 );

    DisplayElement( _T("GeneralNotesImage"), FALSE );
    DisplayElement( _T("GeneralNotesText"), FALSE );
    DisplayElement( _T("LaunchQuarantine"), FALSE );

    ATLASSERT( m_spAxSysListView );

    if( m_spAxSysListView ) {
        m_spAxSysListView->put_Filter( eFilter );
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
void CScanRepairRemoveWnd::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    __super::OnDocumentComplete(pDisp,szUrl);
    m_bHighContrastMode = CUtils::IsSystemHCMode();

    if ( m_bHighContrastMode ) {
        DisplayElement( _T("RegularViewTabs"), FALSE );
        DisplayElement( _T("HighContrastViewTabs"), TRUE );
        DisplayElement( _T("tabDescription"), FALSE );
        DisplayElement( _T("tabDescriptionHC"), TRUE );
    }
    else {
        DisplayElement( _T("RegularViewTabs"), TRUE );
        DisplayElement( _T("HighContrastViewTabs"), FALSE );
        DisplayElement( _T("tabDescription"), TRUE );
        DisplayElement( _T("tabDescriptionHC"), FALSE );
    }

    DisplayElement( _T("GeneralNotesImage"), FALSE );
    DisplayElement( _T("GeneralNotesText"), FALSE );
    DisplayElement( _T("LaunchQuarantine"), FALSE );

    Event_OnIsEmailScan(m_bEmailScan);

    if( m_bEmailScan )
    {
        LPCTSTR szSubject = NULL;
        LPCTSTR szSender = NULL;
        LPCTSTR szRecipient = NULL;

        Event_OnGetEmailValues( &szSubject, &szSender, &szRecipient );

        SetElementText( _T("email_subject"), szSubject);
        SetElementText( _T("email_sender"), szSender);
        SetElementText( _T("email_recipient"), szRecipient);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanRepairRemoveWnd::OnAltKey()
{
    CComPtr< IHTMLRuleStyle > spStyle;

    HRESULT hr = S_OK;
    hr = GetStyleSheetRule( _T(".hotkey"), &spStyle );

    if( SUCCEEDED(hr) )
    {
        _bstr_t bstrDecoration = _T("underline");
        hr = spStyle->put_textDecoration( bstrDecoration );
    } 

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanRepairRemoveWnd::OnGetHelpID()
{
    return IDH;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanRepairRemoveWnd::OnKillActive()
{
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanRepairRemoveWnd::OnSetActive()
{
    BOOL bHighContrastMode = CUtils::IsSystemHCMode();

    if ( bHighContrastMode != m_bHighContrastMode ) {
        m_bHighContrastMode = bHighContrastMode;

        if ( m_bHighContrastMode ) {
            DisplayElement( _T("RegularViewTabs"), FALSE );
            DisplayElement( _T("HighContrastViewTabs"), TRUE );
            DisplayElement( _T("RegularViewContent"), FALSE );
            DisplayElement( _T("HighContrastViewContent"), TRUE );
        }
        else {
            DisplayElement( _T("RegularViewTabs"), TRUE );
            DisplayElement( _T("HighContrastViewTabs"), FALSE );
            DisplayElement( _T("RegularViewContent"), TRUE );
            DisplayElement( _T("HighContrastViewContent"), FALSE );
        }
    }

    long nThreatsDetected = 0;
    Event_OnGetVirusCount( nThreatsDetected );

    long nNonViralCount = 0;
    Event_OnGetNonViralCount( nNonViralCount );

    // Get the total number of threats removed, both viral and non-viral
    long nThreatsDeletedRemoved = 0;
    Event_OnGetRemovedVirusCount( nThreatsDeletedRemoved );

    long nNonViralRemovedCount = 0;
    Event_OnGetRemovedNonViralCount( nNonViralRemovedCount );
    nThreatsDeletedRemoved += nNonViralRemovedCount;

    if( !m_spAnomalyList )
    {
        // Get the anomaly list and pass to the AX list control...
        Event_OnGetAnomalyList( m_spAnomalyList );

        if ( !m_spAxSysListView ) {
            CCTRACEE( _T("CScanRepairRemoveWnd::OnSetActive() - IAxSysListView32 interface uninitialized.  Will attempt to initialize now.") );

            HRESULT hr = 0;
            hr = GetElementInterface( _T("AxListCtrl"), 
                                      __uuidof(IAxSysListView32), 
                                      (void**)&m_spAxSysListView );
            ATLASSERT( SUCCEEDED(hr) );

            if ( m_spAxSysListView ) {
                CCTRACEI( _T("CScanRepairRemoveWnd::Initialize() - Initializing the columns of the ActiveX object.") );
                m_spAxSysListView->InitColumns(false);
            }
        }

        if( m_spAxSysListView )
        {
            CCTRACEI( _T("CScanRepairRemoveWnd::OnSetActive() - IAxSysListView32 interface is valid.  Now passing it the anomaly list") );
            _variant_t var = (long) 
                                 reinterpret_cast<DWORD>(m_spAnomalyList.m_p);
            m_spAxSysListView->put_AnomalyList( &var );
        }
        else
        {
            CCTRACEE( _T("CScanRepairRemoveWnd::OnSetActive() - Unable to get IAxSysListView32 interface.") );
        }
    }

    const long lRemainingThreats = (nThreatsDetected + nNonViralCount) - 
                                                        nThreatsDeletedRemoved;

    int nResLoad = IDS_TAB1_TITLE1;

    if( lRemainingThreats == 0 )
    {
        m_bTab1Visible = FALSE;
        DisplayElement( _T("tab1"), m_bTab1Visible );
        DisplayElement( _T("tab1HC"), m_bTab1Visible );

        DisplayElement( _T("spanApplyButton"), FALSE );
        DisplayElement( _T("spanCancelButton"), FALSE );
        DisplayElement( _T("spanBackButton"), TRUE );
    }
    else
    {
        CString strResourceTemp;
        // Tab 1 text...
        nResLoad = (1==lRemainingThreats) ? IDS_TAB1_TITLE1 : IDS_TAB1_TITLE2;
        g_ResLoader.LoadString(nResLoad,strResourceTemp);
        m_strTab1Text.Format( strResourceTemp, lRemainingThreats );

        m_bTab1Visible = TRUE;
        DisplayElement( _T("tab1"), m_bTab1Visible);
        DisplayElement( _T("tab1HC"), m_bTab1Visible);

        DisplayElement( _T("spanApplyButton"), TRUE );
        DisplayElement( _T("spanCancelButton"), TRUE );
        DisplayElement( _T("spanBackButton"), FALSE );
    }

    if( nThreatsDeletedRemoved == 0 )
    {
        m_bTab2Visible = FALSE;
        DisplayElement( _T("tab2"), m_bTab2Visible);
        DisplayElement( _T("tab2HC"), m_bTab2Visible);
    }
    else
    {
        CString strResourceTemp;
        // Tab 2 text...
        nResLoad = (1==nThreatsDeletedRemoved) ? IDS_TAB2_TITLE1 : 
                                                 IDS_TAB2_TITLE2;
        g_ResLoader.LoadString(nResLoad,strResourceTemp);

        m_strTab2Text.Format( strResourceTemp, nThreatsDeletedRemoved );

        m_bTab2Visible = TRUE;
        DisplayElement( _T("tab2"), m_bTab2Visible);
        DisplayElement( _T("tab2HC"), m_bTab2Visible);
    }

    if( m_spAxSysListView ) {
        m_spAxSysListView->Initialize(m_bEmailScan);
    }

    // Select correct tab
    if( lRemainingThreats == 0 )
    {
        ChangeTab( TAB2 );
    }
    else
    {
        ChangeTab( TAB1 );
    }

    SetFocusToElement(_T("AxListCtrl"));

    DoDataExchange();

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanRepairRemoveWnd::Initialize(BOOL bEmailScan)
{
    m_bEmailScan = bEmailScan;

    if ( !m_spAxSysListView ) {
        CCTRACEI( _T("CScanRepairRemoveWnd::Initialize() - Initializing the IAxSysListView32 interface.") );

        HRESULT hr = 0;
        hr = GetElementInterface( _T("AxListCtrl"), __uuidof(IAxSysListView32),
                                  (void**)&m_spAxSysListView );
        ATLASSERT( SUCCEEDED(hr) );
    }
    else {
        CCTRACEE( _T("CScanRepairRemoveWnd::Initialize() - IAxSysListView32 interface already initialized.") );
    }

    if ( m_spAxSysListView ) {
        CCTRACEI( _T("CScanRepairRemoveWnd::Initialize() - Initializing the columns of the ActiveX object.") );
        m_spAxSysListView->InitColumns(false);
    }
    else {
        CCTRACEE( _T("CScanRepairRemoveWnd::Initialize() - Unable ot initialize the IAxSysListview32 interface.") );
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CScanRepairRemoveWnd::OnItemChanged()
{
    HRESULT hr = S_OK;
    CString strResourceTemp;

    _variant_t var;
    hr = m_spAxSysListView->get_Anomaly( &var );

    if( SUCCEEDED(hr) )
    {
        m_EZAnomaly = reinterpret_cast<ccEraser::IAnomaly*>(var.lVal);
    }
    else
    {
        CCTRACEE(_T("CScanRepairRemoveWnd::OnItemChanged() - Failed to get the anomaly."));
        return;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CScanRepairRemoveWnd::OnDisplayQuarantineNote()
{
    DisplayElement( _T("GeneralNotesImage"), TRUE );
    DisplayElement( _T("GeneralNotesText"), TRUE );
    DisplayElement( _T("LaunchQuarantine"), TRUE );

    CString strFormat;
    CString strName;
    strFormat.LoadString(g_ResLoader.GetResourceInstance(), 
                         IDS_QUARANTINE_NAME);
    strName.Format(strFormat, m_strProductName);
    CComBSTR bstrName(strName);

    SetImgSrc( _T("GeneralNotesImage"), _S(IDS_USE_QUARANTINE_NOTE_IMAGE) );
    SetElementText( _T("GeneralNotesText"), _S(IDS_USE_QUARANTINE_NOTE) );
    SetElementText( _T("QuarantineNameText"), bstrName );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CScanRepairRemoveWnd::OnThreatNameClick()
{
    // Get the VID for the anomaly that is currently selected
    unsigned long ulVirusID = 0;
    _variant_t var;

    if( SUCCEEDED(m_spAxSysListView->get_Anomaly( &var )) )
    {
        m_EZAnomaly = reinterpret_cast<ccEraser::IAnomaly*>(var.lVal);

        // Set the action to the user selection
        ulVirusID = m_EZAnomaly.GetVID();
    }
    else
    {
        CCTRACEE(_T("CScanRepairRemoveWnd::OnThreatNameClick() - Failed to get the anomaly."));
        return;
    }

    // Format SARC URL with virus ID
    CString strURL;
    strURL.Format(m_strSarcThreatInfoUrl, ulVirusID);

    // Launch the ULR
    NAVToolbox::CStartBrowser browser;
    browser.ShowURL ( strURL );

    CCTRACEI(_T("CScanRepairRemoveWnd::OnThreatNameClick() - Launching page: %s"), (LPCTSTR)strURL);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanRepairRemoveWnd::OnThreatTypeClick()
{
    Event_OnRiskDetailsDialog( m_EZAnomaly, false );
}

void CScanRepairRemoveWnd::OnThreatRiskClick()
{
    //
    // Detailed threat risk information really only applies to
    // non-virals.  All virals are high risk, so no need to give
    // a breakdown of information if they click on this for a
    // viral.  For every viral, just bring up the same help panel
    // that explains what viral risk is.
    // 
    if ( m_EZAnomaly.IsViral() ) {
        Event_OnHelp( NAV_CSH_VIRUS_IMPACT );
    }
    else {
        Event_OnRiskDetailsDialog( m_EZAnomaly, true );
    }
}
    

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CScanRepairRemoveWnd::SetCurrentAnomalySelection( CEZAnomaly* pEZAnomaly, LPCTSTR pcszAction)
{
    if( pEZAnomaly == NULL )
    {
        CCTRACEE(_T("CScanRepairRemoveWnd::SetCurrentAnomalySelection() - Anomaly param is null."));
        return;
    }

    // Get the current status
    // Get the status since it won't change
    AnomalyAction eAction;
    CommonUIInfectionStatus eStatus;
    pEZAnomaly->GetAnomalyStatus(eAction, eStatus);

    if( _tcsicmp(pcszAction, REPAIR_SELECTION) == 0 )
    {
        eAction = ACTION_REPAIR;
    }
    else if( _tcsicmp(pcszAction, QUARANTINE_SELECTION) == 0 )
    {
        eAction = ACTION_QUARANTINE;
    }
    else if( _tcsicmp(pcszAction, DELETE_SELECTION) == 0 || _tcsicmp(pcszAction, REMOVE_SELECTION) == 0 )
    {
        eAction = ACTION_DELETE;
    }
    else if( _tcsicmp(pcszAction, EXCLUDE_SELECTION) == 0 )
    {
        eAction = ACTION_EXCLUDE;
    }
    else if( _tcsicmp(pcszAction, IGNORE_SELECTION) == 0 )
    {
        eAction = ACTION_IGNORE;
    }
    else if( _tcsicmp(pcszAction, REVIEW_SELECTION) == 0 )
    {
        eAction = ACTION_REVIEW;
    }

    pEZAnomaly->SetAnomalyStatus(eAction, eStatus);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnLaunchQuarantineClick( IHTMLElement* pElement )
{
    CCTRACEI(_T("CScanRepairRemoveWnd::OnLaunchQuarantineClick) - Detected Launch Quarantine click."));

    CComPtr< IAppLauncher > spAppLauncher;

    HRESULT hr = S_OK;
    hr = Event_OnGetAppLauncher( &spAppLauncher );

    if( SUCCEEDED(hr) && spAppLauncher )
    {
        CCTRACEI(_T("CScanRepairRemoveWnd::OnLaunchQuarantineClick) - Attempting to launch quarantine."));

        hr = spAppLauncher->LaunchQuarantine();
        ATLASSERT( SUCCEEDED(hr) );

        if( FAILED(hr) )
        {
            CCTRACEE(_T("CScanRepairRemoveWnd::OnLaunchQuarantienClick() - LaunchQuarantine returned with a failure."));
        }
    }

    return S_FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnEmailInfoClick( IHTMLElement* pElement )
{
    //
    // Do nothing
    // 
    return S_FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnMouseOverEmailInfo( IHTMLElement* pElement )
{
    DisplayElement( _T("email_tool_tip"), TRUE );
    return S_FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CScanRepairRemoveWnd::OnMouseOutEmailInfo( IHTMLElement* pElement )
{
    DisplayElement( _T("email_tool_tip"), FALSE );
    return S_FALSE;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanRepairRemoveWnd::OnCtrlTab(WORD /*wNotifyCode*/, 
                                        WORD /*wID*/, 
                                        HWND /*hWndCtl*/, 
                                        BOOL& /*bHandled*/)
{
    // This handles the Ctrl-Tab or Ctrl-Shift-Tab switching between panels
    // so if only one tab is visible there is nothing to do
    if( m_bTab1Visible && m_bTab2Visible )
    {
        // See which tab is currently active
        CComBSTR bstrTab1ClassName;
        CComBSTR bstrTab2ClassName;

        if( SUCCEEDED(GetElementClassName(_T("tab1"), &bstrTab1ClassName)) &&
            SUCCEEDED(GetElementClassName(_T("tab2"), &bstrTab2ClassName)) )
        {
            if( bstrTab1ClassName == _T("tab tabSelected") ) {
                ChangeTab(TAB2);
            }
            else if( bstrTab2ClassName == _T("tab tabSelected") ) {
                ChangeTab(TAB1);
            }
        }
    }

    return 0;
}


LRESULT CScanRepairRemoveWnd::OnPaint(UINT uMsg, WPARAM wParam, 
                                      LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    BOOL bHighContrastMode = CUtils::IsSystemHCMode();

    if (bHighContrastMode != m_bHighContrastMode) {
        m_bHighContrastMode = bHighContrastMode;

        if ( m_bHighContrastMode ) {
            DisplayElement( _T("RegularViewTabs"), FALSE );
            DisplayElement( _T("HighContrastViewTabs"), TRUE );
            DisplayElement( _T("tabDescription"), FALSE );
            DisplayElement( _T("tabDescriptionHC"), TRUE );
        }
        else {
            DisplayElement( _T("RegularViewTabs"), TRUE );
            DisplayElement( _T("HighContrastViewTabs"), FALSE );
            DisplayElement( _T("tabDescription"), TRUE );
            DisplayElement( _T("tabDescriptionHC"), FALSE );
        }
    }
	
    return 0;
}

