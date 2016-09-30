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
#include "ConfirmDlg.h"
#include "ccResourceLoader.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CConfirmDlg::CConfirmDlg( UINT nResIDMessage ) :
    CDHTMLWndCtrlDlg< CConfirmDlg >( _T("ConfirmDlg.htm") ),
    m_nResIDMessage( nResIDMessage )
{
    m_bUseHtmlTitle = TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CConfirmDlg::DoDataExchange( BOOL bSaveAndValidate, UINT nCtlID )
{ 
    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CConfirmDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = FALSE;
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CConfirmDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    EndDialog( IDCANCEL );
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CConfirmDlg::OnInitDialog()
{
    __super::OnInitDialog();
    
    WINDOWINFO wi = {sizeof(wi)};   
    GetWindowInfo( m_hWnd, &wi );

    CRect rectNew(0,0, SCALEX(300), SCALEY(140));
    ::AdjustWindowRectEx( &rectNew, wi.dwStyle, FALSE, wi.dwExStyle );
    SetWindowPos(0,0,0,rectNew.Width(),rectNew.Height(), SWP_NOZORDER|SWP_NOMOVE);
    
    CenterWindow();
    MessageBeep( MB_OK );

    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CConfirmDlg::OnOkBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDOK );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CConfirmDlg::OnCancelBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDCANCEL );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CConfirmDlg::OnYesBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDYES );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CConfirmDlg::OnNoBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDNO );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CConfirmDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    __super::OnDocumentComplete(pDisp, szUrl);

    CString strRes;
	g_ResLoader.LoadString(m_nResIDMessage,strRes);
    SetElementText( _T("Message"), strRes );
}



