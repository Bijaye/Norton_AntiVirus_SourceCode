/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/movefiledlg.cpv   1.3   21 May 1998 11:28:24   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// MoveFileDlg.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/movefiledlg.cpv  $
// 
//    Rev 1.3   21 May 1998 11:28:24   DBuches
// Added help topics.
// 
//    Rev 1.2   20 May 1998 17:13:40   DBuches
// 1st pass at help.
// 
//    Rev 1.1   28 Apr 1998 15:14:14   DBuches
// Changed Add button text.
// 
//    Rev 1.0   06 Mar 1998 21:09:34   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "MoveFileDlg.h"
#include "dlgs.h"
#include "qconhlp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveFileDlg

IMPLEMENT_DYNAMIC(CMoveFileDlg, CFileDialog)


////////////////////////////////////////////////////////////////////////////
// Function name	: CMoveFileDlg::CMoveFileDlg
//
// Description	    : Constructor - see help for CFileDialog::CFileDialog.
//
////////////////////////////////////////////////////////////////////////////
// 3/6/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////

CMoveFileDlg::CMoveFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
    // Set title
    m_sTitle.LoadString( IDS_ADD_FILE_TITLE );
    m_ofn.lpstrTitle = m_sTitle.GetBuffer(0);

    // Set up extension template.
    m_ofn.Flags |= OFN_ENABLETEMPLATE;
    m_ofn.hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( IDD_MOVE_FILE_TEMPLATE ), RT_DIALOG );
    m_ofn.lpTemplateName = MAKEINTRESOURCE( IDD_MOVE_FILE_TEMPLATE );

    // Set initial dir to root of system volume.
    TCHAR szBuff[ MAX_PATH ];                    
    GetWindowsDirectory( szBuff, MAX_PATH );
    lstrcpyn( m_szInitialDir, szBuff, 3 );
    m_ofn.lpstrInitialDir = m_szInitialDir;

    m_bDelete = TRUE;

    m_iCtrlID = -1;
}


BEGIN_MESSAGE_MAP(CMoveFileDlg, CFileDialog)
	//{{AFX_MSG_MAP(CMoveFileDlg)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_WHATS_THIS, OnWhatsThis)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



////////////////////////////////////////////////////////////////////////////
// Function name	: CMoveFileDlg::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/6/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CMoveFileDlg::OnInitDialog() 
{
    // Call base class.
    CFileDialog::OnInitDialog();

    // Set check state
    CheckDlgButton( IDC_DELETE_CHECK, m_bDelete );

    // Change Open button text to Add
    CWnd *p = GetParent()->GetDlgItem( 1 );
    ASSERT( *p );
    if( p )
        {
        CString s;
        s.LoadString( IDS_ADD_BUTTON );
        p->SetWindowText( s );
        }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CMoveFileDlg::OnDestroy
//
// Description	    : Handles Destroy message.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/6/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CMoveFileDlg::OnDestroy() 
{
    // Get state of delete checkbox.
    m_bDelete = IsDlgButtonChecked( IDC_DELETE_CHECK );

    // Call base class
    CFileDialog::OnDestroy();
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CMoveFileDlg::OnContextMenu
//
// Description	    : Handles right mouse click on control
//
// Return type		: void 
//
// Argument         : CWnd* pWnd
// Argument         : CPoint point
//
////////////////////////////////////////////////////////////////////////////
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CMoveFileDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    // Set up array of controls
    static  int aControls[] = { IDC_DELETE_CHECK, -1 };

    // Get ID of control in question
    int iCtrlID = pWnd->GetDlgCtrlID();

    // Search array looking for a match.
    int iCount = 0;
    m_iCtrlID = -1;
    while( aControls[iCount] != -1 )
        {
        if( iCtrlID == aControls[iCount] )
            {
            m_iCtrlID = iCtrlID;
            break;
            }
        iCount++;
        }

    // Did we find what we were looking for?
    if( m_iCtrlID == -1 )
        return;

    // Fire off popup menu
    CMenu	menu;
  	CMenu*	pPopup = NULL;
    if (menu.LoadMenu( IDR_VIEW_CONTEXT ))
        {
	    pPopup		= menu.GetSubMenu(3);
        pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, this );
        }    
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CMoveFileDlg::OnWhatsThis
//
// Description	    : Handles What's this menu item.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CMoveFileDlg::OnWhatsThis() 
{
	// Make sure we have something to do.
    if( m_iCtrlID < 0 )
        return;

    int iTopicID = 0;

    switch( m_iCtrlID )
        {
        case IDC_DELETE_CHECK:
            iTopicID = IDH_QUARAN_ADD_REMOVE_FILE_FROM_CHKBX;
            break;

        default:
            return;
        }

	// Invoke help system
    ::WinHelp( GetSafeHwnd(), 
               AfxGetApp()->m_pszHelpFilePath,
               HELP_CONTEXTPOPUP,
               iTopicID );
	
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CMoveFileDlg::OnHelpInfo
//
// Description	    : Handles F1 and ? button help
//
// Return type		: BOOL 
//
// Argument         : HELPINFO* pHelpInfo
//
////////////////////////////////////////////////////////////////////////////
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CMoveFileDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// Map control to topic
    int iTopicID;
    switch( pHelpInfo->iCtrlId )
        {
        case IDC_DELETE_CHECK:
            iTopicID = IDH_QUARAN_ADD_REMOVE_FILE_FROM_CHKBX;
            break;

        default:
            return CFileDialog::OnHelpInfo(pHelpInfo);
        }

	// Invoke help system
    ::WinHelp( GetSafeHwnd(), 
               AfxGetApp()->m_pszHelpFilePath,
               HELP_CONTEXTPOPUP,
               iTopicID );
	
	return CFileDialog::OnHelpInfo(pHelpInfo);
}
