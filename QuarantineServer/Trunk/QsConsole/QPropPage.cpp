/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "qscon.h"
#include "QPropPage.h"
#include "resource.h"
#include "mmc.h"
#include "macros.h"
#include "filenameparse.h"
#include "htmlhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQPropPage property page

IMPLEMENT_DYNCREATE(CQPropPage, CPropertyPage)

CQPropPage::CQPropPage(UINT uID ) : CPropertyPage(uID),
m_pHelpWhatsThisMap( NULL )
{
	//{{AFX_DATA_INIT(CQPropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CQPropPage::~CQPropPage()
{
}

void CQPropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQPropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CQPropPage)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_HELP_WHATSTHIS, OnHelpWhatsthis)
    ON_MESSAGE(WM_HELP, OnHelpMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQPropPage message handlers

BOOL CQPropPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

    // 
    // Set all controls to the correct font.
    // 
    EnumChildWindows( GetSafeHwnd(), CQPropPage::EnumProc, (LPARAM) this );

    // 
    // Fetch full path to HTML help file.
    // 
    TCHAR szFileName[ MAX_PATH ];
    TCHAR szPath[ MAX_PATH ];
    GetModuleFileName( _Module.m_hInst, szFileName, MAX_PATH );
    CFileNameParse::GetFilePath( szFileName, szPath, MAX_PATH );
    lstrcat( szPath, _T("\\") );
    lstrcat( szPath, QCONSOLE_HELPFILE );
	m_sHelpFile = szPath;
//	m_sHelpFile += _T("::/qscon.txt");

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
BOOL CQPropPage::EnumProc( HWND hWnd, LPARAM lParam )
{
    CQPropPage* pThis = (CQPropPage*) lParam;
    
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


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQPropPage::OnContextMenu
//
// Description  : 
//
// Return type  : void 
//
// Argument     : CWnd* pWnd
// Argument     : CPoint point
//
///////////////////////////////////////////////////////////////////////////////
// 5/4/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQPropPage::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	USES_CONVERSION;

    WORD wCtrlID = IDC_STATIC;
    LPOLESTR pTopic = NULL;

    m_pContextHelpWnd = pWnd;
	HWND hWnd;

    // 
    // Determine if we are on one of our controls.
    // 
    if ( pWnd == this )
        {
        CPoint pt = point;
        pWnd->ScreenToClient(&pt);
        m_pContextHelpWnd = ChildWindowFromPoint( pt, CWP_SKIPTRANSPARENT | CWP_SKIPINVISIBLE );

        // 
        // Figure out what control was clicked.
        // 
        if( m_pContextHelpWnd )
		{
			hWnd = m_pContextHelpWnd->GetSafeHwnd();
			wCtrlID = m_pContextHelpWnd->GetDlgCtrlID();
		}

        // 
        // Make sure we have work to do.
        // 
        if( m_pContextHelpWnd == this )
            return;
        }
    else if( pWnd )
        {
        wCtrlID = pWnd->GetDlgCtrlID();
        }

    // 
    // Try to get help topic.
    // 
    if( GetHelpTopic( wCtrlID ) )
        {
        CMenu menu;
        CMenu* pPopup;
        if( menu.LoadMenu( IDR_HELP_MENU ) )
            {
            pPopup = menu.GetSubMenu(0);
            pPopup->TrackPopupMenu( 
                    TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                    point.x, point.y, this );
            }
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQPropPage::GetHelpTopic
//
// Description  : 
//
// Return type  : DWORD 
//
// Argument     : WORD wID
//
///////////////////////////////////////////////////////////////////////////////
// 5/4/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CQPropPage::GetHelpTopic( DWORD wID )
{
    // 
    // Static controls don't get What's this help.
    // 
    if( wID == IDC_STATIC || m_pHelpWhatsThisMap == NULL )
        return 0;

    // 
    // Try to find our topic
    // 
    LPDWORD lpdwWT = m_pHelpWhatsThisMap;
    while( *lpdwWT != 0 )
        {
        if( *lpdwWT == wID )
            {
            return *(++lpdwWT);
            }
        lpdwWT += 2;
        }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServerPage::OnHelpWhatsthis
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 5/4/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQPropPage::OnHelpWhatsthis() 
{
	if( m_pContextHelpWnd && m_pHelpWhatsThisMap )
        {
        DWORD dwTopic = GetHelpTopic( m_pContextHelpWnd->GetDlgCtrlID() );
        if( dwTopic )
            {
			USES_CONVERSION;
			HtmlHelpA(
				m_pContextHelpWnd->GetSafeHwnd(),
				T2A(m_sHelpFile.GetBuffer(0)),
				HH_TP_HELP_CONTEXTMENU,
				(DWORD)(LPVOID) m_pHelpWhatsThisMap);
            }
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQPropPage::OnHelpMessage
//
// Description  : 
//
// Return type  : LRESULT 
//
// Argument     : WPARAM wParam
// Argument     : LPARAM lParam
//
///////////////////////////////////////////////////////////////////////////////
// 5/4/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT CQPropPage::OnHelpMessage(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER( wParam );
	USES_CONVERSION;

    // Handles F1 and ? help
    LPHELPINFO  pHelpInfo = (LPHELPINFO)lParam;
	return 0;
	if( pHelpInfo->iContextType == HELPINFO_WINDOW && m_pHelpWhatsThisMap )
		{
		HtmlHelpA(
			(HWND)pHelpInfo->hItemHandle,
			T2A(m_sHelpFile.GetBuffer(0)),
			HH_TP_HELP_WM_HELP,
			(DWORD)(LPVOID)m_pHelpWhatsThisMap);
		}

    return 0;
}


