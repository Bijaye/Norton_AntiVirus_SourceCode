////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Author:  Christopher Brown 11/08/2004

#include "stdafx.h"
#include "RiskDetailsDlg.h"

// These headers are necessary to launch the correct response web page...nasty
#include "StartBrowser.h"
#include "NAVInfo.h"
#include "AVRES.H"

#include "ThreatCatInfo.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inline int ToHighMedLow( DWORD dwAverage )
{
    int nResult = 0;

    switch( dwAverage )
    {
        case 0:
        case 1:
        case 2:
            nResult = 1;
            break;
        case 3:
            nResult = 2;
            break;
        case 4:
        case 5:
            nResult = 3;
            break;
    }

    return nResult;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CRiskDetailsDlg::CRiskDetailsDlg( CScanEventsSink* pSink,
                                  ccEraser::IAnomaly* pAnomaly,
                                  bool bRiskAssessment,
                                  bool bViral ) :
    CDHTMLWndCtrlDlg< CRiskDetailsDlg >( _T("RiskDetails.htm") ),
    m_EZAnomaly(pAnomaly),
    m_bRiskAssessment(bRiskAssessment),
    m_bTab1Visible(!bViral)
{
    RegisterEventSink( pSink );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CRiskDetailsDlg::DoDataExchange( BOOL bSaveAndValidate, UINT nCtlID )
{ 
    return TRUE;

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CRiskDetailsDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, 
                                   LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = FALSE;
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CRiskDetailsDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, 
                                 LPARAM /*lParam*/, BOOL& bHandled)
{
    EndDialog( IDCANCEL );
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CRiskDetailsDlg::OnF1Help()
{
    Event_OnHelp( IDH1 );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnMoreInfoBtnClick( IHTMLElement* pElement)
{
    Event_OnHelp( IDH1 );
    return S_FALSE;  // Prevents the IE control click sound
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnOkBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDOK );
    return S_FALSE; // Prevents the IE control click sound
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CRiskDetailsDlg::OnInitDialog()
{
    __super::OnInitDialog();

    WINDOWINFO wi = {sizeof(wi)};
    GetWindowInfo( m_hWnd, &wi );

    CRect rectBody;
    GetElementRect( _T("Body"), rectBody );

    //CRect rectNew(0,0, SCALEX(438), SCALEY(405));
    CRect rectNew(0, 0, SCALEX(438), SCALEY(525));
    ::AdjustWindowRectEx( &rectNew, wi.dwStyle, FALSE, wi.dwExStyle );
    SetWindowPos(0, 0, 0, rectNew.Width(), rectNew.Height(), 
                 SWP_NOZORDER | SWP_NOMOVE);
    CenterWindow();
    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CRiskDetailsDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    __super::OnDocumentComplete(pDisp, szUrl);

    // Use text based on whether this anomaly has been handled yet?
    CommonUIInfectionStatus eStatus;
    AnomalyAction eAction;
    m_EZAnomaly.GetAnomalyStatus(eAction, eStatus);

    UINT nResID = IDS_OTHERAFFECTEDAREAS_UNHANDLED_DESCRIPTION;

    if ( eStatus != InfectionStatus_Unhandled ) {
        nResID = IDS_OTHERAFFECTEDAREAS_HANDLED_DESCRIPTION;
    }

    cc::IStringPtr pStrName;
    CThreatCatInfo tciCategoryInfo;
    TCHAR szTypeBuffer[MAX_PATH] = {0};
    TCHAR szDescBuffer[MAX_DESC] = {0};

    if ( m_EZAnomaly.GetName(pStrName) ) {
        SetElementText( _T("matrix_risk_name"), pStrName->GetStringW() );
        SetElementText( _T("details_risk_name"), pStrName->GetStringW() );
    }

    if ( tciCategoryInfo.GetCategoryTextAndDesc(m_EZAnomaly.GetCategories(),
                                                szTypeBuffer, MAX_PATH,
                                                szDescBuffer, MAX_DESC) ) {
        SetElementText( _T("matrix_risk_type"), szTypeBuffer );
        SetElementText( _T("details_risk_type"), szTypeBuffer );
        SetElementText( _T("details_description"), szDescBuffer );
    }

    if ( m_EZAnomaly.KnownToHaveDependencies() ) {
        SetElementText( _T("matrix_risk_dependencies"), 
                        _S(IDS_HAS_DEPENDENCIES) );
        SetElementText( _T("details_risk_dependencies"), 
                        _S(IDS_HAS_DEPENDENCIES) );
    }
    else {
        SetElementText( _T("matrix_risk_dependencies"), 
                        _S(IDS_HAS_NO_DEPENDENCIES) );
        SetElementText( _T("details_risk_dependencies"), 
                        _S(IDS_HAS_NO_DEPENDENCIES) );
    }

    const DWORD dwDamnAverage = m_EZAnomaly.GetDamageFlagAverage();
    int nVal = ToHighMedLow( dwDamnAverage );

    switch( nVal )
    {
        case 1:
            SetElementText( _T("matrix_overall"), _S(IDS_THREAT_MATRIX_LOW) );
            SetElementText( _T("details_overall"), _S(IDS_THREAT_MATRIX_LOW) );
            break;

        case 2:
            SetElementText( _T("matrix_overall"), _S(IDS_THREAT_MATRIX_MED) );
            SetElementText( _T("details_overall"), _S(IDS_THREAT_MATRIX_MED) );
            break;

        case 3:
            SetElementText( _T("matrix_overall"), _S(IDS_THREAT_MATRIX_HIGH) );
            SetElementText( _T("details_overall"), _S(IDS_THREAT_MATRIX_HIGH) );
            break;

        default:
            ATLASSERT( FALSE );
            break;
    };

    CString strImage;
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal );
    SetImgSrc( _T("matrix_imageThreatOverall"), strImage );
    SetImgSrc( _T("details_imageThreatOverall"), strImage );

    // Select the overall by default
    CComPtr< IHTMLElement > spElement;
    GetElement( _T("overallTitle"), &spElement );
    OnOverallBtnClick(spElement);

    DWORD dwDamnValue = 0;
    dwDamnValue = m_EZAnomaly.GetDamageFlagValue(ccEraser::IAnomaly::Stealth);
    nVal = ToHighMedLow( dwDamnValue );
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageThreatStealth"), strImage );

    switch( nVal )
    {
       case 1:
            SetElementText( _T("stealth"), _S(IDS_THREAT_MATRIX_LOW) );
            break;

        case 2:
            SetElementText( _T("stealth"), _S(IDS_THREAT_MATRIX_MED) );
            break;

        case 3:
            SetElementText( _T("stealth"), _S(IDS_THREAT_MATRIX_HIGH) );
            break;
    };

    dwDamnValue = m_EZAnomaly.GetDamageFlagValue(ccEraser::IAnomaly::Removal);
    nVal = ToHighMedLow( dwDamnValue );
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageThreatRemoval"), strImage );

    switch( nVal )
    {
        case 1:
            SetElementText( _T("removal"), _S(IDS_THREAT_MATRIX_LOW) );
            break;

        case 2:
            SetElementText( _T("removal"), _S(IDS_THREAT_MATRIX_MED) );
            break;

        case 3:
            SetElementText( _T("removal"), _S(IDS_THREAT_MATRIX_HIGH) );
            break;
    };

    dwDamnValue = 
              m_EZAnomaly.GetDamageFlagValue(ccEraser::IAnomaly::Performance);

    nVal = ToHighMedLow( dwDamnValue );
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageThreatPerformace"), strImage );

    switch( nVal )
    {
        case 1:
            SetElementText( _T("performace"), _S(IDS_THREAT_MATRIX_LOW) );
            break;

        case 2:
            SetElementText( _T("performace"), _S(IDS_THREAT_MATRIX_MED) );
            break;

        case 3:
            SetElementText( _T("performace"), _S(IDS_THREAT_MATRIX_HIGH) );
            break;
    };

    dwDamnValue = m_EZAnomaly.GetDamageFlagValue(ccEraser::IAnomaly::Privacy);
    nVal = ToHighMedLow( dwDamnValue );
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageThreatPrivacy"), strImage );

    switch( nVal )
    {
        case 1:
            SetElementText( _T("privacy"), _S(IDS_THREAT_MATRIX_LOW) );
            break;

        case 2:
            SetElementText( _T("privacy"), _S(IDS_THREAT_MATRIX_MED) );
            break;

        case 3:
            SetElementText( _T("privacy"), _S(IDS_THREAT_MATRIX_HIGH) );
            break;
    };

    // The bottom control must be obtained first because the ItemChanged 
    // event requires it which is fired when the top control's anomaly 
    // item is set...never change this order
    if( !m_spAxSysListViewBottom )
    {
        HRESULT hr = S_OK;
        hr = GetElementInterface( _T("AxListCtrlBottom"), 
                                  __uuidof(IAxSysListView32OAA), 
                                  (void**)&m_spAxSysListViewBottom );
        ATLASSERT( SUCCEEDED(hr) );

        if( FAILED(hr) )
        {
            m_spAxSysListViewBottom = NULL;
            CCTRACEE( _T("Unable to get bottom AX control IAxSysListView32OAA interface.") );
		}
		else {
            _variant_t var = (long) reinterpret_cast<DWORD>(&(*m_EZAnomaly));
            hr = m_spAxSysListViewBottom->put_Anomaly( &var );
            ATLASSERT( SUCCEEDED(hr) );
        }
    }

    if( m_spAxSysListViewBottom && !m_spAxSysListViewTop )
    {
        HRESULT hr = S_OK;
        hr = GetElementInterface( _T("AxListCtrlTop"), 
                                  __uuidof(IAxSysListView32OAA), 
                                  (void**)&m_spAxSysListViewTop );
        ATLASSERT( SUCCEEDED(hr) );

        if( FAILED(hr) )
        {
            m_spAxSysListViewTop = NULL;
            CCTRACEE( _T("Unable to get top AX control IAxSysListView32OAA interface.") );
        }
        else {
            _variant_t var = (long) reinterpret_cast<DWORD>(&(*m_EZAnomaly));
            hr = m_spAxSysListViewTop->put_Anomaly( &var );
            ATLASSERT( SUCCEEDED(hr) );
        }
    }

    if (m_bRiskAssessment) {
       ChangeTab( TAB1 );
    }
    else {
       ChangeTab( TAB2 );
    }

    m_strTab1Text = _S(IDS_RISK_ASSESSMENT_TAB_TEXT);
    m_strTab2Text = _S(IDS_DETAILS_TAB_TEXT);

    DisplayElement( _T("tab1"), m_bTab1Visible );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnOverallBtnClick( IHTMLElement* pElement)
{
    const int nVal = ToHighMedLow( m_EZAnomaly.GetDamageFlagAverage() );

    // Set the detail window text
    SetElementText( _T("threatImpactTitle"), _S(IDS_OVERALL) );

    // Set the detail window image
    CString strImage;
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageDetails"), strImage );

    switch( nVal )
    {
    case 1:
        SetElementText( _T("ThreatDescription"), _S(IDS_THREAT_MATRIX_DESC_OVERALL_LOW) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_LOW) );
        break;

    case 2:
        SetElementText( _T("ThreatDescription"), _S(IDS_THREAT_MATRIX_DESC_OVERALL_MED) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_MED) );
        break;

    case 3:
        SetElementText( _T("ThreatDescription"), _S(IDS_THREAT_MATRIX_DESC_OVERALL_HIGH) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_HIGH) );
        break;

    default:
        ATLASSERT( FALSE );
        break;
    };

    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnPerformaceBtnClick( IHTMLElement* pElement )
{
    const int nVal = ToHighMedLow( m_EZAnomaly.GetDamageFlagValue(ccEraser::IAnomaly::Performance) );

    // Set the detail window text
    SetElementText( _T("threatImpactTitle"), _S(IDS_PERFORMANCE) );

    // Set the detail window image
    CString strImage;
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageDetails"), strImage );

    switch( nVal )
    {
    case 1:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_PERF_LOW) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_LOW) );
        break;

    case 2:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_PERF_MED) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_MED) );
        break;

    case 3:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_PERF_HIGH) );
        SetElementText( _T("threatImpactDetails"), 
                        _S(IDS_THREAT_MATRIX_HIGH) );
        break;

    default:
        ATLASSERT( FALSE );
        break;
    };

    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnPrivacyBtnClick( IHTMLElement* pElement)
{
    const int nVal = ToHighMedLow( m_EZAnomaly.GetDamageFlagValue(ccEraser::IAnomaly::Privacy) );

    // Set the detail window text
    SetElementText( _T("threatImpactTitle"), _S(IDS_PRIVACY) );

    // Set the detail window image
    CString strImage;
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageDetails"), strImage );

    switch( nVal )
    {
    case 1:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_PRIV_LOW) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_LOW) );
        break;

    case 2:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_PRIV_MED) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_MED) );
        break;

    case 3:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_PRIV_HIGH) );
        SetElementText( _T("threatImpactDetails"), 
                        _S(IDS_THREAT_MATRIX_HIGH) );
        break;

    default:
        ATLASSERT( FALSE );
        break;
    };

    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnRemovalBtnClick( IHTMLElement* pElement)
{
    const int nVal = ToHighMedLow( m_EZAnomaly.GetDamageFlagValue(ccEraser::IAnomaly::Removal) );

    // Set the detail window text
    SetElementText( _T("threatImpactTitle"), _S(IDS_REMOVAL) );

    // Set the detail window image
    CString strImage;
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageDetails"), strImage );

    switch( nVal )
    {
    case 1:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_REMOVE_LOW) );
        SetElementText( _T("threatImpactDetails"), 
                        _S(IDS_THREAT_MATRIX_LOW) );
        break;

    case 2:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_REMOVE_MED) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_MED) );
        break;

    case 3:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_REMOVE_HIGH) );
        SetElementText( _T("threatImpactDetails"), 
                        _S(IDS_THREAT_MATRIX_HIGH) );
        break;

    default:
        ATLASSERT( FALSE );
        break;
    };

    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnStealthBtnClick( IHTMLElement* pElement)
{
    const int nVal = ToHighMedLow( m_EZAnomaly.GetDamageFlagValue(ccEraser::IAnomaly::Stealth) );

    // Set the detail window text
    SetElementText( _T("threatImpactTitle"), _S(IDS_STEALTH) );

    // Set the detail window image
    CString strImage;
    strImage.Format(IDS_THREAT_MATRIX_IMAGE, nVal);
    SetImgSrc( _T("imageDetails"), strImage );

    switch( nVal )
    {
    case 1:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_STEALTH_LOW) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_LOW) );
        break;

    case 2:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_STEALTH_MED) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_MED) );
        break;

    case 3:
        SetElementText( _T("ThreatDescription"), 
                        _S(IDS_THREAT_MATRIX_DESC_STEALTH_HIGH) );
        SetElementText( _T("threatImpactDetails"), _S(IDS_THREAT_MATRIX_HIGH) );
        break;

    default:
        ATLASSERT( FALSE );
        break;
    };

    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnRiskNameClick( IHTMLElement* pElement )
{
    // Get the VID for the anomaly that is currently selected
    unsigned long ulVirusID = m_EZAnomaly.GetVID();
    _variant_t var;

    // Format URL with virus ID
    CString strURL;
    strURL.Format(m_strSarcThreatInfoUrl, ulVirusID);

    // Launch the ULR
    NAVToolbox::CStartBrowser browser;
    browser.ShowURL ( strURL );

    CCTRACEI(_T("CRiskDetailsDlg::OnThreatNameClick() - Launching page: %s"), (LPCTSTR)strURL);

    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CRiskDetailsDlg::OnItemChanged()
{
    HRESULT hr = S_OK;

    ULONG lAffectedType = 0;
    ATLASSERT( m_spAxSysListViewTop );

    if ( m_spAxSysListViewTop ) {
        hr = m_spAxSysListViewTop->get_AffectedType( &lAffectedType );
        ATLASSERT( SUCCEEDED(hr) );

        // Only switch the bottom list we are displaying if get_AffectTypes()
        // call into the top list returns S_OK
        if( hr == S_OK ) {
            ATLASSERT( m_spAxSysListViewBottom );

            if ( m_spAxSysListViewBottom ) {
                hr = m_spAxSysListViewBottom->put_AffectedType( lAffectedType );
                ATLASSERT( SUCCEEDED(hr) );
            }
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnTab1BtnClick( IHTMLElement* pElement )
{
    ChangeTab( TAB1 );
    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::OnTab2BtnClick( IHTMLElement* pElement )
{
    ChangeTab( TAB2 );
    return S_FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CRiskDetailsDlg::ChangeTab( TABS dwTab )
{
    LPCTSTR szClassNameTab1 = _T("tab");
    LPCTSTR szClassNameTab2 = _T("tab");

    switch( dwTab )
    {
        case TAB1:
            szClassNameTab1 = _T("tab tabSelected");

            DisplayElement( _T( "DetailsBody"), FALSE );
            DisplayElement( _T( "MatrixBody"), TRUE );

            break;

        case TAB2:
            szClassNameTab2 = _T("tab tabSelected");

            DisplayElement( _T( "MatrixBody"), FALSE );
            DisplayElement( _T( "DetailsBody"), TRUE );

            break;
            
        default:
            ATLASSERT( false );

            break;
    }
    
    SetElementClassName( _T("tab1"), szClassNameTab1 );
    SetElementClassName( _T("tab2"), szClassNameTab2 );

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CRiskDetailsDlg::OnCtrlTab(WORD /*wNotifyCode*/, WORD /*wID*/, 
                                   HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // This handles the Ctrl-Tab or Ctrl-Shift-Tab switching between panels
    // so if only one tab is visible there is nothing to do
    if ( m_bTab1Visible )
    {
        // See which tab is currently active
        CComBSTR bstrTab1ClassName;
        CComBSTR bstrTab2ClassName;

        if ( SUCCEEDED(GetElementClassName(_T("tab1"), &bstrTab1ClassName)) &&
             SUCCEEDED(GetElementClassName(_T("tab2"), &bstrTab2ClassName)) )
        {
            if ( bstrTab1ClassName == _T("tab tabSelected") )
            {
                ChangeTab(TAB2);
            }
            else if( bstrTab2ClassName == _T("tab tabSelected") )
            {
                ChangeTab(TAB1);
            }
        }
    }

    return 0;
}
