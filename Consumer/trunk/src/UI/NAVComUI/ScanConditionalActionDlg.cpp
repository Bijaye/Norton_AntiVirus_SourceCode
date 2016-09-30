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
#include "ScanConditionalActionDlg.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CScanConditionalActionDlg::CScanConditionalActionDlg( CScanEventsSink* pSink, const long lThreatsRemaining, const UINT nResDescription, const UINT nHelpId) :
    CDHTMLWndCtrlDlg< CScanConditionalActionDlg >( _T("ScanConditionalDlg.htm") ),
    m_lThreatsRemaining(lThreatsRemaining),
    m_nResIDDescription(nResDescription),
    m_nHelpId(nHelpId)
{
    m_bUseHtmlTitle = TRUE;

    if(pSink)
    {
        RegisterEventSink(pSink);
    }

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CScanConditionalActionDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    EndDialog( IDCANCEL );
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CScanConditionalActionDlg::OnInitDialog()
{
    __super::OnInitDialog();
    SetWindowPos(0,0,0,SCALEX(365),SCALEY(260),SWP_NOZORDER|SWP_NOMOVE);
    CenterWindow();

    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
void CScanConditionalActionDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    __super::OnDocumentComplete(pDisp, szUrl);

    if( m_lThreatsRemaining > 0)
    {
		// Format text that contains product name
		CString strDesc;
		CString strDescFormat;

		strDescFormat.LoadString(g_ResLoader.GetResourceInstance(), m_nResIDDescription);
		strDesc = strDescFormat;

		switch(m_nResIDDescription)
		{
		case IDS_CONDITIONAL_SPYWARE:			
			strDesc.Format(strDescFormat, m_strProductName);
			break;

		default:
			break;
		}

        // This is the damage control items dialog
        CComBSTR bStrHtmlDesc(strDesc);
        SetElementHtml( _T("description"), bStrHtmlDesc );
    }
    else
    {
        // Change the title and size for the reboot dialog
        if( m_nResIDDescription == IDS_REBOOT_WARNING )
        {
			CString strMessage((LPCTSTR)m_nResIDDescription);
            SetElementText( _T("title"), _S(IDS_REBOOT_TITLE) );
            SetElementText( _T("description"), strMessage );
            SetWindowPos(0,0,0,SCALEX(365),SCALEY(180),SWP_NOZORDER|SWP_NOMOVE);
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CScanConditionalActionDlg::OnF1Help()
{
    Event_OnHelp( m_nHelpId );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanConditionalActionDlg::OnOKBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDOK );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CScanConditionalActionDlg::OnMoreInfoBtnClick( IHTMLElement* pElement )
{
    Event_OnHelp( m_nHelpId );
    return S_FALSE;
}




