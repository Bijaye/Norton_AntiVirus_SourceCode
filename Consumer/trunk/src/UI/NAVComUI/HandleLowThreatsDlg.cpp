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
#include "HandleLowThreatsDlg.h"
#include "NavSettingsHelperEx.h"
#include "optnames.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CHandleLowThreatsDlg::CHandleLowThreatsDlg(bool &bAlertAgain) :
    CDHTMLWndCtrlDlg< CHandleLowThreatsDlg >( _T("HandleLowThreatsDlg.HTM") )
{
    m_pbAlertAgain = &bAlertAgain;
    m_bUseHtmlTitle = TRUE;
    m_iNeverAlertChecked = BST_UNCHECKED;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CHandleLowThreatsDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, 
                                      LPARAM /*lParam*/, BOOL& bHandled)
{
    EndDialog( IDCANCEL );
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CHandleLowThreatsDlg::OnInitDialog()
{
    __super::OnInitDialog();
    SetWindowPos(0,0,0,SCALEX(365),SCALEY(280),SWP_NOZORDER|SWP_NOMOVE);
    CenterWindow();

    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
void CHandleLowThreatsDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    SetElementText( _T("ProductName"), m_strProductName );    
    
    DoDataExchange(FALSE);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CHandleLowThreatsDlg::OnF1Help()
{
    /*Event_OnHelp( IDH );*/
    // No help for this panel
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CHandleLowThreatsDlg::OnOkBtnClick( IHTMLElement* pElement )
{
    DoDataExchange(TRUE);

    if( m_iNeverAlertChecked & BST_CHECKED )
    {
        CCTRACEI(_T("CHandleLowThreatsDlg::OnOkBtnClick() - The never alert again checkbox is checked."));
        if( m_pbAlertAgain )
        {
            *m_pbAlertAgain = false;
        }

        // Save the don't alert option so we never alert again
        CNAVOptSettingsEx NavOpts;
        if( NavOpts.Init() )
        {
            if( FAILED(NavOpts.SetValue(SCANNER_AlertForExclude, (DWORD)0)) )
            {
                CCTRACEE( _T("CHandleLowThreatsDlg::OnOkBtnClick() - Failed to persist don't alert value to ccSettings.") );
            }
        }
        else
        {
            CCTRACEE( _T("CHandleLowThreatsDlg::OnOkBtnClick() - Failed to initialize ccSettigngs helper class.") );
        }
    }

    EndDialog( IDYES );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CHandleLowThreatsDlg::OnCancelBtnClick( IHTMLElement* pElement )
{
    DoDataExchange(TRUE);

    if( m_iNeverAlertChecked & BST_CHECKED )
    {
        CCTRACEI(_T("CHandleLowThreatsDlg::OnCancelBtnClick() - The never alert again checkbox is checked."));
        if( m_pbAlertAgain )
        {
            *m_pbAlertAgain = false;
        }

        // Save the don't alert option so we never alert again
        CNAVOptSettingsEx NavOpts;
        if( NavOpts.Init() )
        {
            if( FAILED(NavOpts.SetValue(SCANNER_AlertForExclude, (DWORD)0)) )
            {
                CCTRACEE( _T("CHandleLowThreatsDlg::OnCancelBtnClick() - Failed to persist don't alert value to ccSettings.") );
            }
        }
        else
        {
            CCTRACEE( _T("CHandleLowThreatsDlg::OnCancelBtnClick() - Failed to initialize ccSettigngs helper class.") );
        }
    }

    EndDialog( IDCANCEL );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CHandleLowThreatsDlg::DoDataExchange( BOOL bSaveAndValidate, UINT nCtlID )
{ 
    DDX_DHtml_CheckBox( bSaveAndValidate, _T("Ignore"), m_iNeverAlertChecked );
    return TRUE;
}
