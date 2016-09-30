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
#include "ScanRepairProgressWnd.h"

#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CScanRepairProgressWnd::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CScanRepairProgressWnd::CScanRepairProgressWnd( CScanEventsSink* pSink ) :
    CBaseDHTMLWnd( _T("ScanRepairProgress.htm") ),
    m_nItemsRemaining(0),
    m_nTotalItems(0),
    m_nProgress(0),
    m_bAbort(false)
{
    RegisterEventSink( pSink );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CScanRepairProgressWnd::DoDataExchange( BOOL bSaveAndValidate, UINT nCtlID )
{
    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanRepairProgressWnd::OnMoreInfoBtnClick( IHTMLElement* pElement )
{
    Event_OnHelp( IDH );
    return S_FALSE; // Prevents IE click sound...
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanRepairProgressWnd::OnAbortClick( IHTMLElement* pElement )
{
    CCTRACEI(_T(" CScanRepairProgressWnd::OnAbortClick() - Abort clicked."));

    // Disable the abort button
    SetElementButtonEnabled(_T("Abort"), FALSE);
    m_bAbort = true;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanRepairProgressWnd::OnAltKey()
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
LRESULT CScanRepairProgressWnd::OnGetHelpID()
{
    return IDH;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanRepairProgressWnd::OnKillActive()
{
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanRepairProgressWnd::OnSetActive()
{
    m_nTotalItems = 0;
    m_nProgress = 0;
    m_bAbort = false;

    // Enable the abort button
    SetElementButtonEnabled(_T("Abort"), TRUE);

    if( ! m_bstrOuterProgressHTML.length() )
    {
        m_bstrOuterProgressHTML = GetElementHtml( _T("outerprogress") );
    }
    else
    {
        // Store away the original HTML in case this page get's reactivated.
        // This is done to fix a HTML redraw bug...  [CBROWN]
        SetElementHtml( _T("outerprogress"), m_bstrOuterProgressHTML );
    }
   	CString strDesc;
	CString strFormat;

	strFormat.LoadString(g_ResLoader.GetResourceInstance(), IDS_REPAIR_PROGRESS);
	strDesc.Format(strFormat, m_strProductName);

	CComBSTR bstrhtml(strDesc);

	SetElementHtml(_T("progress_text"), bstrhtml);

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanRepairProgressWnd::OnUpdateRepairProgress(unsigned long nItemsRemaining, bool& bAbort)
{
    if( !m_nTotalItems )
    {
        m_nTotalItems = nItemsRemaining ? nItemsRemaining : 1;
    }

    m_nItemsRemaining = nItemsRemaining;

    const int nRepairProgress = (int)((float)(100.0f / (float)m_nTotalItems) * ((float)(m_nTotalItems - m_nItemsRemaining)));

    if( m_bAbort )
    {
        m_nProgress = END_PROGRESS_VALUE;
    }
    else
    {
        m_nProgress = nRepairProgress;
    }

    SendMessage(UM_PROGRESS_UPDATE);

    bAbort = m_bAbort;
}

LRESULT CScanRepairProgressWnd::OnUpdateProgress(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if( 100 == m_nProgress )
    {
        // Post 1 more message so we get to 100%
        m_nProgress = END_PROGRESS_VALUE;
        PostMessage(UM_PROGRESS_UPDATE);
    }
    else if( END_PROGRESS_VALUE == m_nProgress )
    {
        // Exit
        Event_OnRepairProgressComplete();
        return 0;
    }

    // Update the progress
    CString strProgress;
    strProgress.Format(_T("%d%%"), m_nProgress);
    SetElementWidth( _T("progress"), strProgress );
    

    return 0;
}
