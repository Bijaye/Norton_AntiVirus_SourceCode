/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/exportdlg.cpv   1.1   21 May 1998 11:29:06   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ExportDlg.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/exportdlg.cpv  $
// 
//    Rev 1.1   21 May 1998 11:29:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.0   14 Apr 1998 00:41:54   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "ExportDlg.h"
#include "qconsoledoc.h"
#include "qconhlp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog



CExportDlg::CExportDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	//{{AFX_DATA_INIT(CExportDlg)
	m_iExport = 0;
	m_iFormat = 0;
    m_iViewMode = 0;
	//}}AFX_DATA_INIT

    // Set title
    m_sTitle.LoadString( IDS_EXPORT_FILE_TITLE );
    m_ofn.lpstrTitle = m_sTitle.GetBuffer(0);

    // Set up extension template.
    m_ofn.Flags |= OFN_ENABLETEMPLATE;
    m_ofn.hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( IDD_EXPORT_DIALOG ), RT_DIALOG );
    m_ofn.lpTemplateName = MAKEINTRESOURCE( IDD_EXPORT_DIALOG );

    // Set bogus control ID.
    m_iCtrlID = -1;
}


void CExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_iExport);
	DDX_Radio(pDX, IDC_RADIO3, m_iFormat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportDlg, CFileDialog)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_WHATS_THIS, OnWhatsThis)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportDlg message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: CExportDlg::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG
//
// Return type		: BOOL
//
//
////////////////////////////////////////////////////////////////////////////
// 4/13/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CExportDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	// Set correct button text for This view radio button
    CWnd *p = GetDlgItem( IDC_RADIO2 );

    // Get window text and append the view mode name
    CString sWindowText;
    CString sViewModeName;

    switch( m_iViewMode )
        {
        case VIEW_MODE_ALL:
            sViewModeName.LoadString(IDS_TREE_ROOT_TEXT);
            break;
        case VIEW_MODE_QUARANTINE:
            sViewModeName.LoadString(IDS_QUARANTINE_ITEMS);
            break;
        case VIEW_MODE_BACKUP:
            sViewModeName.LoadString(IDS_TREE_BACKUP_ITEMS);
            break;
        case VIEW_MODE_SUBMITTED:
            sViewModeName.LoadString(IDS_TREE_SUBMITTED_ITEMS);
            break;
        }

    sWindowText.Format( _T("(%s)"), sViewModeName );
    GetDlgItem( IDC_NAME_STATIC )->SetWindowText( sWindowText );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CExportDlg::OnOK
//
// Description	    : Hanles OK button
//
// Return type		: void
//
//
////////////////////////////////////////////////////////////////////////////
// 4/13/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnOK( )
{


}


////////////////////////////////////////////////////////////////////////////
// Function name	: CExportDlg::OnDestroy
//
// Description	    :
//
// Return type		: void
//
//
////////////////////////////////////////////////////////////////////////////
// 4/13/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnDestroy()
{
	UpdateData();

    CFileDialog::OnDestroy();

	// TODO: Add your message handler code here

}


////////////////////////////////////////////////////////////////////////////
// Function name	: CExportDlg::OnContextMenu
//
// Description	    : Handles context menu click
//
// Return type		: void 
//
// Argument         : CWnd* pWnd
// Argument         : CPoint point
//
////////////////////////////////////////////////////////////////////////////
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    // Set up array of controls
    static  int aControls[] = { IDC_RADIO1, IDC_RADIO2, IDC_RADIO3, IDC_RADIO4, -1 };

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
void CExportDlg::OnWhatsThis() 
{
	// Make sure we have something to do.
    if( m_iCtrlID < 0 )
        return;

    int iTopicID = 0;

    switch( m_iCtrlID )
        {
        case IDC_RADIO1:
            iTopicID = IDH_QUARAN_EXPORT_ALL_ITEMS_BTN;
            break;
        case IDC_RADIO2:
            iTopicID = IDH_QUARAN_EXPORT_CURRENT_VIEW_ONLY_BTN;
            break;
        case IDC_RADIO3:
            iTopicID = IDH_QUARAN_EXPORT_STANDARD_BTN;
            break;
        case IDC_RADIO4:
            iTopicID = IDH_QUARAN_EXPORT_COMMA_DELIMITED_BTN;
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
// Function name	: CExportDlg::OnHelpInfo
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
BOOL CExportDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// Map control to topic
    int iTopicID;
    switch( pHelpInfo->iCtrlId )
        {
        case IDC_RADIO1:
            iTopicID = IDH_QUARAN_EXPORT_ALL_ITEMS_BTN;
            break;
        case IDC_RADIO2:
            iTopicID = IDH_QUARAN_EXPORT_CURRENT_VIEW_ONLY_BTN;
            break;
        case IDC_RADIO3:
            iTopicID = IDH_QUARAN_EXPORT_STANDARD_BTN;
            break;
        case IDC_RADIO4:
            iTopicID = IDH_QUARAN_EXPORT_COMMA_DELIMITED_BTN;
            break;

        // Swallow clicks on static controls
        case IDC_STATIC:
            return FALSE;

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
