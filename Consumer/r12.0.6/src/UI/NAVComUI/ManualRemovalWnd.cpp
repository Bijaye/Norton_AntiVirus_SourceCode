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
#include "DHTMLWndCtrlview.h"
#include "ManualRemovalWnd.h"
#include "commonuiinterface.h"
#include "StartBrowser.h"
#include "AVRES.h"

#include "resource.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CManualRemovalWnd::CManualRemovalWnd( CScanEventsSink* pSink ) :
    CBaseDHTMLWnd( _T("ManualRemoval.htm") ) 
{
    RegisterEventSink( pSink );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CManualRemovalWnd::DoDataExchange( BOOL bSaveAndValidate, UINT nCtlID )
{
    return TRUE;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CManualRemovalWnd::OnMoreInfoBtnClick( IHTMLElement* pElement )
{
    Event_OnHelp( IDH );
    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CManualRemovalWnd::OnCancelBtnClick( IHTMLElement* pElement )
{
    Event_OnRepairRemoveCancel();
    return S_OK;
}


void CManualRemovalWnd::OnThreatNameClick()
{
    // Get the VID for the anomaly that is currently selected
    unsigned long ulVirusID = 0;
    _variant_t var;

    if( SUCCEEDED(m_spAxSysListView->get_Anomaly( &var )) )
    {
        m_EZAnomaly = reinterpret_cast<ccEraser::IAnomaly*>(var.lVal);

        // Set the action to the user selection
        ulVirusID = m_EZAnomaly.GetVID();
        CCTRACEI(_T("CManualRemovalWnd::OnThreatNameClick() - Got virus id of %lu"), ulVirusID);
    }
    else
    {
        CCTRACEE(_T("CManualRemovalWnd::OnThreatNameClick() - Failed to get the anomaly."));
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

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------- 
LRESULT CManualRemovalWnd::OnAltKey()
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
LRESULT CManualRemovalWnd::OnGetHelpID()
{
    return IDH;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CManualRemovalWnd::OnKillActive()
{
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CManualRemovalWnd::Initialize(BOOL bEmailScan)
{
    m_bEmailScan = bEmailScan;

    if ( !m_spAxSysListView ) {
        CCTRACEI( _T("CSManualRemovalWnd::Initialize() - Initializing the IAxSysListView32 interface.") );

		HRESULT hr = 0;
        hr = GetElementInterface( _T("AxListCtrl"), __uuidof(IAxSysListView32),
                              (void**)&m_spAxSysListView );
        ATLASSERT( SUCCEEDED(hr) );
    }
    else {
        CCTRACEE( _T("CManualRemovalWnd::Initialize() - IAxSysListView32 interface already initialized.") );
    }

    if ( m_spAxSysListView ) {
        CCTRACEI( _T("CManualRemovalWnd::Initialize() - Initializing the columns of the ActiveX object.") );
        m_spAxSysListView->InitColumns(true);
    }
    else {
        CCTRACEE( _T("CManualRemovalWnd::Initialize() - Unable ot initialize the IAxSysListview32 interface.") );
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CManualRemovalWnd::OnSetActive()
{
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
            CCTRACEE( _T("CManualRemovalWnd::OnSetActive() - IAxSysListView32 interface uninitialized.  Will attempt to initialize now.") );

            HRESULT hr = 0;
            hr = GetElementInterface( _T("AxListCtrl"), 
                                      __uuidof(IAxSysListView32),
                                      (void**)&m_spAxSysListView );
            ATLASSERT( SUCCEEDED(hr) );

            if ( m_spAxSysListView ) {
                CCTRACEI( _T("CManualRemovalWnd::Initialize() - Initializing the columns of the ActiveX object.") );
                m_spAxSysListView->InitColumns(false);
            }
		}

        if( m_spAxSysListView )
        {
            CCTRACEI( _T("CManualRemovalWnd::OnSetActive() - IAxSysListView32 interface is valid.  Now passing it the anomaly list") );
            _variant_t var = (long) 
                                 reinterpret_cast<DWORD>(m_spAnomalyList.m_p);
            m_spAxSysListView->put_AnomalyList( &var );
        }
        else
        {
			CCTRACEE( _T("C:ManualRemovalWnd::OnSetActive() - Unable to get IAxSysListView32 interface.") );
        }
    }

    const long lRemainingThreats = (nThreatsDetected + nNonViralCount) - 
                                                       nThreatsDeletedRemoved;

    int nResLoad = IDS_TAB1_TITLE1;

    if( m_spAxSysListView ) {
        m_spAxSysListView->Initialize(m_bEmailScan);
        m_spAxSysListView->put_Filter( IAxSysListView32_Filter_Infected );
    }

    SetElementClassName( _T("ScanState"), _T("BlueScanState") );
    SetElementClassName( _T("description"), _T("QuickScanDescription") );

    SetFocusToElement(_T("AxListCtrl"));

    DoDataExchange();

    return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CManualRemovalWnd::OnThreatNameClick( IHTMLElement* pElement )
{
    CCTRACEI(_T("CManualRemovalWnd::OnThreatNameClick() - Detected threat name click."));

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
        CCTRACEE(_T("CManualRemovalWnd::OnThreatNameClick() - Failed to get the anomaly."));
        return S_FALSE;
    }

    // Format the SARC URL with virus ID
    CString strURL;
    strURL.Format(m_strSarcThreatInfoUrl, ulVirusID);

    NAVToolbox::CStartBrowser browser;
    browser.ShowURL ( strURL );

    CCTRACEI(_T("CManualRemovalWnd::OnThreatNameClick() - Launching page: %s"), strURL);

    return S_FALSE;
}

