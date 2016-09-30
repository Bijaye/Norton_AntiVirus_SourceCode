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
#include "ccWebWnd_i.c"
#include "FullOrQuickScanDlg.h"
#include "NAVHelpLauncher.h"
#include "NAVSettingsHelperEx.h"
#include "OptNames.h"
#include "navopt32.h"
#include "navinfo.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CFullOrQuickScanDlg::CFullOrQuickScanDlg( ) :
    CDHTMLWndCtrlDlg< CFullOrQuickScanDlg >( _T("FullOrQuickScanDlg.htm") ),
    m_bNoSpyware(false)
{
    m_lUserAction = USER_ACTION_FULL;
    m_bUseHtmlTitle = TRUE;

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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CFullOrQuickScanDlg::DoDataExchange( BOOL bSaveAndValidate, UINT nCtlID )
{ 
    DDX_DHtml_Radio( bSaveAndValidate, _T("Action"), m_lUserAction );
    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CFullOrQuickScanDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    EndDialog( IDCANCEL );
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CFullOrQuickScanDlg::OnInitDialog()
{
    __super::OnInitDialog();
    WINDOWINFO wi = {sizeof(wi)};
    GetWindowInfo( m_hWnd, &wi );

    CRect rectBody;
    GetElementRect( _T("Body"), rectBody );

    CRect rectNew(0, 0, 471, 333);
    ::AdjustWindowRectEx( &rectNew, wi.dwStyle, FALSE, wi.dwExStyle );
    SetWindowPos(0, 0, 0, rectNew.Width(), rectNew.Height(), 
                 SWP_NOZORDER | SWP_NOMOVE);    CenterWindow();

    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
void CFullOrQuickScanDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    __super::OnDocumentComplete(pDisp, szUrl);

    if (m_bNoSpyware) {
        SetElementText(_T("Description"), 
                       _S(IDS_SCAN_NOW_DESCRIPTION_NOSPYWARE));
    }
    else {
        SetElementText(_T("Description"), _S(IDS_SCAN_NOW_DESCRIPTION));
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CFullOrQuickScanDlg::OnOKBtnClick( IHTMLElement* pElement )
{
    EndDialog(IDOK);
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CFullOrQuickScanDlg::OnCancelBtnClick( IHTMLElement* pElement )
{
    EndDialog( IDCANCEL );
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CFullOrQuickScanDlg::OnF1Help()
{
    // Launch the help

    NAVToolbox::CNAVHelpLauncher Help;
    const HWND hWndResult = Help.LaunchHelp( NIS_scan_now_popup );

    if( !hWndResult )
        CCTRACEE(_T("CFullOrQuickScanDlg::Help() - Failed to launch Help"));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CFullOrQuickScanDlg::OnMoreInfoBtnClick( IHTMLElement* pElement )
{
    OnF1Help();
    return S_FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CFullOrQuickScanDlg::OnClickActionRadio( IHTMLElement* pElement )
{
    DoDataExchange( TRUE );
    return S_OK;
}
