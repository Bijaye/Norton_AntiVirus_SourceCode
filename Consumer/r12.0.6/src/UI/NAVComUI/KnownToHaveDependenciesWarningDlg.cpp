//******************************************************************************
// Copyright © 2004 Symantec Corporation.
// ---------------------------------------------------------------------------
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// All rights reserved.
//
// Author:  Christopher Brown 11/08/2004
//******************************************************************************

#include "stdafx.h"
#include "KnownToHaveDependenciesWarningDlg.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CKnownToHaveDependenciesWarningDlg::CKnownToHaveDependenciesWarningDlg() :
    CDHTMLWndCtrlDlg< CKnownToHaveDependenciesWarningDlg >( _T("KNOWNTOHAVEDEPENDENCIESWARNINGDLG.HTM") )
{
    m_bUseHtmlTitle = TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CKnownToHaveDependenciesWarningDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    EndDialog( IDCANCEL );
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CKnownToHaveDependenciesWarningDlg::OnInitDialog()
{
    __super::OnInitDialog();
    SetWindowPos(0,0,0,SCALEX(365),SCALEY(300),SWP_NOZORDER|SWP_NOMOVE);
    CenterWindow();

    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
void CKnownToHaveDependenciesWarningDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CKnownToHaveDependenciesWarningDlg::OnF1Help()
{
    Event_OnHelp( IDH );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CKnownToHaveDependenciesWarningDlg::OnContinueBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDYES );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CKnownToHaveDependenciesWarningDlg::OnAbortBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDNO );
    return S_OK;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CKnownToHaveDependenciesWarningDlg::OnMoreInfoBtnClick( IHTMLElement* pElement )
{
    Event_OnHelp( IDH );
    return S_FALSE;
}




