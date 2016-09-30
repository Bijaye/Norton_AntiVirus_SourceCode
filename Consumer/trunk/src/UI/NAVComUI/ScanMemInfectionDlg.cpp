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
#include "ScanMemInfectionDlg.h"

const DWORD TIMER_ID = 101;
const DWORD TIMER_ELAPSE = 1000;
const int TIME_OUT = 60;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CScanMemInfectionDlg::CScanMemInfectionDlg( CScanEventsSink* pEvents, BOOL bEnabledTimout /*= TRUE*/ ) :
    CDHTMLWndCtrlDlg< CScanMemInfectionDlg >( _T("ScanMemInfectionDlg.htm") ),
    m_nTimeout(TIME_OUT), // Seconds...
    m_bEnableTimeout(bEnabledTimout),
    m_bInputDetected(false)
{
    CCTRACEI(_T("CScanMemInfectionDlg::CScanMemInfectionDlg() - Constructing memory infection dialog"));
    CScanEventsSource::RegisterEventSink( pEvents );
    m_bUseHtmlTitle = TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanMemInfectionDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    EndDialog( IDNO );
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CScanMemInfectionDlg::OnInitDialog()
{
    __super::OnInitDialog();

    CCTRACEI(_T("CScanMemInfectionDlg::OnInitDialog() - Initializing dialog"));

    if (m_bEnableTimeout) {
        CCTRACEI(_T("CScanMemInfectionDlg::OnInitDialog() - Timeout enabled; setting timer."));

        // Start the 60 second timer
        SetTimer( TIMER_ID, TIMER_ELAPSE, NULL );

        // Start detecting user input
        m_spInputDetector = new CInputDetection;

        if( m_spInputDetector ) {
            m_spInputDetector->Initialize( m_hWnd, _Module.get_m_hInst() );
            m_spInputDetector->
                     CInputDetectionEventSource::RegisterEventSink( this );
        }
    }

    SetWindowPos(0,0,0,SCALEX(400),SCALEY(315),SWP_NOZORDER|SWP_NOMOVE);
    CenterWindow();

    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanMemInfectionDlg::OnInputDeteted()
{
    CCTRACEI(_T("CScanMemInfectionDlg::OnInputDetected() - Detected input")); 

    m_bInputDetected = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanMemInfectionDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    __super::OnDocumentComplete(pDisp, szUrl);

    // Format text that contains product name
    CString strDesc;
    CString strRecommendation;
    CString strFormat;

    strFormat.LoadString(g_ResLoader.GetResourceInstance(), IDS_MEM_INFECTION);
    strDesc.Format(strFormat, m_strProductName);

    strFormat.LoadString(g_ResLoader.GetResourceInstance(), 
                         IDS_MEM_INFECTION_RECOMMEND);
    strRecommendation.Format(strFormat, m_strProductName);

    CCTRACEI(_T("CScanMemInfectionDlg::OnDocumentComplete() - Setting 'statement' element to %s"), strDesc);

    SetElementText(_T("statement"), strDesc);

    CCTRACEI(_T("CScanMemInfectionDlg::OnDocumentComplete() - Setting 'recommen dation' element to %s"), strRecommendation);

    SetElementText(_T("recommendation"), strRecommendation);

    HWND hwndForeground = GetForegroundWindow();    

    if ( m_hWnd != hwndForeground ) 
    {
        SwitchToThisWindow( m_hWnd, TRUE );
    }

    SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

//------------------------------------------------------------------------------
HRESULT CScanMemInfectionDlg::OnYesBtnClick( IHTMLElement* pElement)
{
    CCTRACEI(_T("CScanMemInfectionDlg::OnYesBtnClick() - Yes button clicked"));
    EndDialog( IDYES );

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanMemInfectionDlg::OnNoBtnClick( IHTMLElement* pElement)
{
    CCTRACEI(_T("CScanMemInfectionDlg::OnNoBtnClick() - No button clicked"));
    EndDialog( IDNO );

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanMemInfectionDlg::OnMoreInfoBtnClick( IHTMLElement* pElement)
{
    CCTRACEI(_T("CScanMemInfectionDlg::OnMoreInfoBtnClick() - more info link clicked"));
    Event_OnHelp( IDH );

    return S_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanMemInfectionDlg::OnF1Help()
{
    Event_OnHelp( IDH );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanMemInfectionDlg::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if ( m_bInputDetected )
    {
        CCTRACEI(_T("CScanMemInfectionDlg::OnTimer() - Input has been detected; killing timer"));

        // Cancel the timer
        KillTimer( TIMER_ID );

        // Hide the count down text
        HideCountDownText(true);

        if( m_spInputDetector )
        {
            m_spInputDetector->CInputDetectionEventSource::UnregisterEventSink( this );
            m_spInputDetector.Release();
        }
    }
    else
    {
        // Update the count down
        CString strProgress;
        strProgress.Format( _T("%d"), m_nTimeout );
        SetElementText(_T("timerCount"), strProgress);

        // If this is the first time out show the text after we have updated it
        if( m_nTimeout == TIME_OUT )
            HideCountDownText(false);

        m_nTimeout--;

        if( m_nTimeout < 0 )
            EndDialog( IDNO );
    }

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanMemInfectionDlg::OnDestory(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    KillTimer( TIMER_ID );

    bHandled = FALSE;

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanMemInfectionDlg::HideCountDownText(bool bHide)
{
    CComPtr<IHTMLElement> spElement;
    if( SUCCEEDED(GetElement(_T("timerTextDiv"), &spElement)) )
    {
        CComPtr<IHTMLStyle> spStyle;
        if( SUCCEEDED(spElement->get_style(&spStyle)) )
        {
            CComBSTR bstrHidden;
            if( bHide )
                bstrHidden = "hidden";
            else
                bstrHidden = "visible";

            spStyle->put_visibility(bstrHidden);
        }
    }
}
