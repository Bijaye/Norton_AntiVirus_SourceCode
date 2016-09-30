/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerDlg dialog


CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerDlg)
	m_sDomain = _T("");
	m_sPassword = _T("");
	m_sServer = _T("");
	m_sUser = _T("");
	//}}AFX_DATA_INIT
}


void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerDlg)
	DDX_Text(pDX, IDC_DOMAIN, m_sDomain);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDX_Text(pDX, IDC_SERVER, m_sServer);
	DDX_Text(pDX, IDC_USER, m_sUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	//{{AFX_MSG_MAP(CServerDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // 
    // Set all controls to the correct font.
    // 
    EnumChildWindows( GetSafeHwnd(), CServerDlg::EnumProc, (LPARAM) this );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: EnumProc
//
// Description  : 
//
// Return type  : BOOL CQPropPage:: 
//
// Argument     :  HWND hWnd
// Argument     : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 4/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerDlg::EnumProc( HWND hWnd, LPARAM lParam )
{
    CServerDlg* pThis = (CServerDlg*) lParam;
    
    // 
    // Set the font for this control.
    // 
    CWnd* pWnd = CWnd::FromHandle( hWnd );
    if( pWnd )
        {
        pWnd->SetFont( CFont::FromHandle( (HFONT) GetStockObject( DEFAULT_GUI_FONT ) ) );
        }

    return TRUE;
}
