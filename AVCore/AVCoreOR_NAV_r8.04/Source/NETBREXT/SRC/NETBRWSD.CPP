/////////////////////////////////////////////////////////////////////
// NETBRWSD.CPP - Created - 11/01/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/netbrwsd.cpv   1.5   28 Aug 1998 16:30:02   JBRENNA  $
//
// Description:
//    Header file for the CNetBrowsDlg class.  This will be the main
//    dialog the Network Browser.  It will contain a CNetTreeCtrl
//    class for the network enumeration, and will provide extra
//    functionality for right mouse click, and for specific browser
//    features that this particular network browser requires.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/netbrwsd.cpv  $
// 
//    Rev 1.5   28 Aug 1998 16:30:02   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.4.1.2   07 Jul 1998 12:06:00   DBuches
//    Added members to deal with the help button.
// 2. Rev 1.4.1.1   26 Jun 1998 18:19:00   mdunn
//    Now #including navwhlp5.h
// 
//    Rev 1.4   20 Jan 1998 17:45:10   KSACKIN
// Updated WindowResizer with latest revisions.  Now setting parent correctly,
// and adding controls with the control ID instead of having to get the window
// handle and pass that.  Also, now we no longer use the Multiplier struct.
// This is just confusing and unneccessary.
//
//    Rev 1.3   10 Dec 1997 20:44:48   KSACKIN
// Converted to use the new CWindowResizer.  This now takes only Windows HWND's
// as arguments, and not a CWnd *.
//
//    Rev 1.2   09 Dec 1997 12:26:42   DBuches
// Commented out calls to NG_SetFontsInDialog().
//
//    Rev 1.1   21 Nov 1997 12:21:00   KSACKIN
// Removed NavUtil.h as a dependent header file.
//
//    Rev 1.0   21 Nov 1997 11:52:00   KSACKIN
// Initial revision.
//
//    Rev 1.2   12 Nov 1997 15:33:46   TSmith
// Replaced calls to NQ_SetFontsInDialog with NG_SetFontsInDialog.
//
//
//    Rev 1.1   15 Jun 1997 20:30:26   KSACKIN
// Removed call to context help function.  This function was calling into
// NavXUtil, which was giving us a nasty line of dependencies.  We will replace
// this with RoboHelp.
//
//    Rev 1.0   18 Apr 1997 14:03:00   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:16:32   RFULLER
// Initial revision
//
//    Rev 1.12   03 Feb 1997 17:27:00   ksackin
// Added a call to NQ_SetFontsInDialog() on the InitDialog member of my Network
// browser dialog.  This call is made into the UniUtil.lib library and gets
// the prototype from UniUtil.h.
//
//    Rev 1.11   30 Jan 1997 19:26:46   ksackin
// Added a how-to help ID for the network browser.
//
//    Rev 1.10   20 Jan 1997 16:49:06   ksackin
// Modified the OnSelChanged function to update the edit box and OK button every
// time that you select a new object.  If it is a valid object, then it will set
// the text and enable the OK button, but if it is not, it will clear the edit
// field and disable the OK button.
//
//    Rev 1.9   09 Jan 1997 03:51:52   ksackin
// Hacked this file just a little bit for this build until we can figure out
// how to update the ID's in the navwhlp.h file that we find with the ones
// from the AVCORE project
//
//    Rev 1.8   09 Jan 1997 03:37:28   ksackin
// Added code for the help button of the Network Browser dialog, as well as
// calls through to the OnContextHelp function for context help.
//
//    Rev 1.7   20 Dec 1996 21:43:58   ksackin
// Fixed the item type return on the Network browser to reflect the true item
// type.  Previously, it was only setting the item type if you selected a
// custom name, rather than selected it thought the browser.
//
//    Rev 1.6   20 Dec 1996 20:08:36   ksackin
// Made various changes to the browser dialog to add a text field at the top,
// and to fix some context menu stuff.
//
//    Rev 1.5   16 Dec 1996 23:16:54   ksackin
// Modified the dialog to handle the new Size Grip control.  This involved some
// magic when resizing and checking your windows version to see if we should use
// that control at all.  If we are in a windows major version less than 4.0, we
// will not use the control at all.
//
//    Rev 1.4   09 Dec 1996 23:40:06   ksackin
// Brushed up some functionality having to do with dealing with network
// resources.  Removed a dependancy on items being containers (for showing
// shares), and also now expand a provider branch at startup if there is
// only 1 branch.  It did not make much sense to have a single branch, and not
// automatically expand it.
//
//    Rev 1.3   05 Dec 1996 03:45:48   ksackin
// Restricted the Netware Network for the Forward Target Network Browser.
//
//    Rev 1.2   05 Dec 1996 02:00:04   ksackin
// Added defines for network resoruce types, and now reference a DWORD of flags
// to determine if items should be displayed, browsed beneath, or are valid
// selection items.
//
//    Rev 1.1   25 Nov 1996 00:39:38   ksackin
// Added miminum and maximum size information for the browser dialog.
//
//    Rev 1.0   20 Nov 1996 01:49:12   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Include Files

#include "stdafx.h"
#include "platform.h"
#include "NetBrStr.h"                   // String resources.
#include "NetBrwsD.h"
#include "wndsize.h"
#include "DispMsg.h"
#include "DllRCSta.h"                   // DLL Resource State Management.
#include "navwhlp5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD glWindowsVersion;                  // Windows Major Version.



// **********  This should be removed as soon as we update our ************
// **********  copy of navwhlp.h.  We ar pulling an old one.   ************
//Network browser dialog ------------------------------------------------
#define IDH_NAVNT_WHATHIS_TARGET_TXTBX                    22101
#define IDH_NAVNT_WHATHIS_NETWORK_BROWSER_TREE            22102
#define IDH_NAVNT_NETWORK_BROWSER_DLG_HELP_BTN            22103
// ************************************************************************

/////////////////////////////////////////////////////////////////////////////
// CNetBrowsDlg dialog

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::CNetBrowsDlg
//
// Purpose:
//    Constructor for the CNetBrowsDlg class.  This is the dialog
//    class for the network browser dialog.
//
// Parameters:
//    DWORD dwBrowserType  - [IN] Set up the browser type.  This will default
//                                to BROWSER_TYPE_DEFAULT.  This will initialize
//                                all of the controls to this brwoser type's
//                                defaults.
//    CWnd* pParent        - [IN] Parent window for this dialog.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetBrowsDlg::CNetBrowsDlg(
    DWORD dwBrowserType, // = BROWSER_TYPE_DEFAULT
    CWnd* pParent        // = NULL
    )
    : CDialog( IDD_NETBROWS_DIALOG, pParent)
{
        //{{AFX_DATA_INIT(CNetBrowsDlg)
                // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32

                                        // Make sure that our title variable
                                        // is empty so that we know whether
                                        // to load a default title or not.
    m_cszDialogTitle.Empty();
                                        // Set the browser type to default
                                        // now, in case our client does not
                                        // set up a browser type.
    SetBrowserType( dwBrowserType );

    m_nMinTrackSizeX = 0;
    m_nMinTrackSizeY = 0;
    m_iHelpButtonTopicID = 0;

    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::~CNetBrowsDlg
//
// Purpose:
//    Destructor for the CNetBrowsDlg class.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetBrowsDlg::~CNetBrowsDlg( void )
{
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::DoDataExchange
//
// Purpose:
//    Data Exchange function for virtual data exchange.
//
// Parameters:
//    CDataExchange* pDX - [IN] Data exchange pointer.
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::DoDataExchange(
    CDataExchange* pDX
    )
{
        CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNetBrowsDlg)
        //}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////
// Message Maps

BEGIN_MESSAGE_MAP(CNetBrowsDlg, CDialog)
        //{{AFX_MSG_MAP(CNetBrowsDlg)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_NOTIFY(TVN_ITEMEXPANDING, IDC_NET_TREE, OnItemexpandingNetTree)
        ON_WM_SIZE()
        ON_NOTIFY(NM_DBLCLK, IDC_NET_TREE, OnDblclkNetTree)
        ON_NOTIFY(TVN_SELCHANGED, IDC_NET_TREE, OnSelchangedNetTree)
        ON_EN_CHANGE(IDE_SELECTED_ITEM_NAME, OnChangeServerName)
        ON_NOTIFY(NM_RCLICK, IDC_NET_TREE, OnRclickNetTree)
        ON_WM_CONTEXTMENU()
        ON_WM_CREATE()
        ON_BN_CLICKED(IDHELP, OnHelp)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNetBrowsDlg Member Functions

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::ValidResourceTypeSelected
//
// Purpose:
//    Get the currently selected item from the tree control, and determine
//    if it is a valid item to select and return from this dialog with.
//
// Parameters:
//    None.
//
// Returns:
//    BOOLEAN as to whether this item is a valid return object.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetBrowsDlg::ValidResourceTypeSelected( void )
{
BOOL bRetVal=FALSE;
                                        // Get the name of the resource
                                        // selected .
    if ( m_cNetTreeCtrl.GetSelectedItemInformation( &m_cszSelectedItemName, &m_dwSelectedItemType ) )
        {
                                        // Check it against valid return
                                        // types.
        if ( m_cNetTreeCtrl.IsValidSelectionType( m_dwSelectedItemType ) )
            {
            bRetVal = TRUE;
            }
        }
    else
        {
                                        // Display an error message that we
                                        // could not get the selected
                                        // resource name for the tree.
        DisplayErrorMessage( m_hWnd, IDS_ERROR_RETURNING_SELECTED_RESOURCE_NAME );
        }


    return bRetVal;
}


/////////////////////////////////////////////////////////////////////////////
// CNetBrowsDlg message handlers


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnInitDialog
//
// Purpose:
//    Init dialog handler.  This will initialize all of the controls
//    on this dialog.  It will also set up the resizer and the first
//    level of network enumerations, and fill up the tree control
//    with this first level.
//
// Parameters:
//    None.
//
// Returns:
//    BOOLEAN as to whether we set the focus or not.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetBrowsDlg::OnInitDialog()
{
                                        // Need to switch state to load this resource
                                        // out of the correct module.
    CDLLResourceState switchState;

                                        // Major Windows Version is low byte.
    glWindowsVersion = LOBYTE( GetVersion() );

                                        // Call default dialog's Init.
    CDialog::OnInitDialog();

                                        // Set the fonts in the dialog to a
                                        // font that will display correctly in
                                        // the current code page.
//    NG_SetFontsInDialog(m_hWnd, 0);

                                        // This function will initialize the
                                        // browser dialog in accordance to all
                                        // of the special pre-initialized data,
                                        // and/or by the type of dialog which
                                        // you requested through the
                                        // SetBrowserType() member function.
    InitializeBrowserDialog();

    m_cNetTreeCtrl.SubclassDlgItem( IDC_NET_TREE, this );

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);                 // Set big icon
        SetIcon(m_hIcon, FALSE);                // Set small icon


                                        // Set the parent for this
                                        // WindowResizer class.
    m_cWindowResizer.SetParent( m_hWnd );

                                        // Set up all of the window resizer
                                        // controls for all of the controls on
                                        // the dialog.
    m_cWindowResizer.RegisterWindow( IDT_BROWSER_INFORMATION_TEXT, (float)0, (float)0, (float)1, (float)0, SWP_NOMOVE | SWP_NOZORDER );
    m_cWindowResizer.RegisterWindow( IDC_NET_TREE,                 (float)0, (float)0, (float)1, (float)1, SWP_NOMOVE | SWP_NOZORDER );
    m_cWindowResizer.RegisterWindow( IDE_SELECTED_ITEM_NAME,       (float)0, (float)1, (float)1, (float)0, SWP_NOZORDER );
    m_cWindowResizer.RegisterWindow( IDT_SERVER_NAME_TEXT,         (float)0, (float)1, (float)0, (float)0, SWP_NOSIZE | SWP_NOZORDER );
    m_cWindowResizer.RegisterWindow( IDOK,                         (float)1, (float)1, (float)0, (float)0, SWP_NOSIZE | SWP_NOZORDER );
    m_cWindowResizer.RegisterWindow( IDCANCEL,                     (float)1, (float)1, (float)0, (float)0, SWP_NOSIZE | SWP_NOZORDER );
    m_cWindowResizer.RegisterWindow( IDHELP,                       (float)1, (float)1, (float)0, (float)0, SWP_NOSIZE | SWP_NOZORDER );

                                        // If Windows major version of 4.0 or
                                        // above...
    if ( glWindowsVersion >= 4 )
        {
                                        // Use the size grip control to size
                                        // this dialog.
        m_cWindowResizer.RegisterWindow( IDC_SIZEGRIP_SCROLL_BAR, (float)1, (float)1, (float)0, (float)0, SWP_NOSIZE |  SWP_NOZORDER );
        }
    else
        {
                                        // If we are in NT 3.51, don't even show
                                        // this control because it does not display
                                        // correctly.
        GetDlgItem( IDC_SIZEGRIP_SCROLL_BAR )->SendMessage( WM_CLOSE,0, 0 );
        }

                                        // Flip the initialized flag.
    m_cWindowResizer.SetInitialized( 1 );

                                        // Calcilate the smallest horizontal and
                                        // vertical size for this dialog.
    CalculateMinMax();

    SetClippingForSizeGrip();

        return TRUE;  // return TRUE  unless you set the focus to a control
}



/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnPaint
//
// Purpose:
//    Paint handler.  This is an MFC thing to draw the icon, among
//    other painting requsts.
//    If you add a minimize button to your dialog, you will need the
//    code below to draw the icon.  For MFC applications using the
//    document/view model, this is automatically done for you by the
//    framework.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnPaint()
{
        if (IsIconic())
        {
                CPaintDC dc(this); // device context for painting

                SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

                // Center icon in client rectangle
                int cxIcon = GetSystemMetrics(SM_CXICON);
                int cyIcon = GetSystemMetrics(SM_CYICON);
                CRect rect;
                GetClientRect(&rect);
                int x = (rect.Width() - cxIcon + 1) / 2;
                int y = (rect.Height() - cyIcon + 1) / 2;

                // Draw the icon
                dc.DrawIcon(x, y, m_hIcon);
        }
        else
        {
                CDialog::OnPaint();
        }
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnQueryDragIcon
//
// Purpose:
//    Handler for the query drag icon request.
//    The system calls this to obtain the cursor to display while the
//    user drags the minimized window.
//
// Parameters:
//    None.
//
// Returns:
//    HCURSOR for the drag icon.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
HCURSOR CNetBrowsDlg::OnQueryDragIcon()
{
        return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnItemexpandingNetTree
//
// Purpose:
//    Function to handle the expantion of each tree branch.  In this
//    tree application, we are going to enumerate each level of the
//    tree control on a demand basis.  In other words, we will only
//    enumerate a branch when it is asked for.
//
// Parameters:
//    NMHDR* pNMHDR    - [IN]  parameter reguarding the tree item that
//                             is expanding.
//    LRESULT* pResult - [OUT] Result of this branch expantion.
//
// Returns:
//    None. (Except through the pResult parameter.  This specifies
//           the return value of what happened.  If you return TRUE,
//           the tree item will not expand or collapse.  FALSE means
//           no problem.).
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnItemexpandingNetTree(
    NMHDR* pNMHDR,
    LRESULT* pResult
    )
{
NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
CNetResourceContainer *lpNetResourceContainer;

                                        // Get the item data for this branch.
                                        // This will be the
                                        // CNetResourceContainer class that has
                                        // been allocated for each branch.
    lpNetResourceContainer = (CNetResourceContainer *)m_cNetTreeCtrl.GetItemData( pNMTreeView->itemNew.hItem );

                                        // Use this class to enumerate the next
                                        // level of network resources.
    lpNetResourceContainer->EnumerateNetResourceBranch( &m_cNetTreeCtrl,
                                                        pNMTreeView->itemNew.hItem );

                                        // Return FALSE (No problem occured.)
    *pResult = 0;
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnSize
//
// Purpose:
//    Size handler for the CNetBrowsDlg dialog.
//
// Parameters:
//    UINT nType - [IN] Type of sizing event.
//    int cx     - [IN] New width for this dialog.
//    int cy     - [IN] New height for this dialog.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnSize(
    UINT nType,
    int cx,
    int cy
    )
{
                                        // Static flag to see if maximized.  This is
                                        // done to trap a restore after a maximize.
static BOOL bMaximized = 0;

                                        // Call through to their size function.
    CDialog::OnSize(nType, cx, cy);

                                        // Only mess with size grip if we are
                                        // in NT 4.0 or above.
    if ( glWindowsVersion >= 4 )
        {
        if ( m_cWindowResizer.GetInitialized() )
            {
                                        // If we are maximizing the dialog...
            if ( nType == SIZE_MAXIMIZED )
                {
                                        // Hide the size grip control.
                GetDlgItem( IDC_SIZEGRIP_SCROLL_BAR )->ShowWindow( SW_HIDE );

                bMaximized = TRUE;
                }
                                        // If we are restoring from a maximized
                                        // state...
            if ( (TRUE == bMaximized) &&
                 (nType == SIZE_RESTORED) )
                {
                                        // Hide the size grip control.
                GetDlgItem( IDC_SIZEGRIP_SCROLL_BAR )->ShowWindow( SW_SHOWNORMAL );

                bMaximized = FALSE;
                }
            }
        }

                                        // Size all of the controls on the
                                        // dialog accordingly through our
                                        // CWindowResizer class.

    m_cWindowResizer.UpdateSize( nType, cx, cy );

                                        // This code is here because the size grip
                                        // overlaps the help button.  This should
                                        // be fixed by another means so that we do
                                        // not get the slight flicker on the help
                                        // button, but this will work for now.
    if ( m_cWindowResizer.GetInitialized() )
        {
        UpdateWindow();
        GetDlgItem( IDHELP )->RedrawWindow();
        }
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnDblclkNetTree
//
// Purpose:
//    Double click handler for the network tree control.  This will
//    be used to determine if you are actually selecting a server, or
//    are simply trying to expand another container type.
//
// Parameters:
//    NMHDR* pNMHDR    - [IN]  Header describing the tree control
//                             that was double clicked upon.
//    LRESULT* pResult - [OUT] Result of our handling.
//
// Returns:
//    None. (Result of the handler through the pResult member.)
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnDblclkNetTree(
    NMHDR* pNMHDR,
    LRESULT* pResult
    )
{
NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    if ( TRUE == ValidResourceTypeSelected() )
        {
                                        // Check to see if this item has children
                                        // or at least can expand.  We only want to
                                        // select this item if it is valid and cannot
                                        // expand.  Otherwise, we are just going
                                        // to expand it and  make you press OK.
        if ( FALSE == m_cNetTreeCtrl.ItemHasChildren( m_cNetTreeCtrl.GetSelectedItem() ) )
            {
                                        // Call the OK handler (this will determine
                                        // if this is a valid resource, and if so,
                                        // return from this dialog.
            if ( TRUE == GetDlgItem( IDOK )->IsWindowEnabled() )
                {
                OnOK();
                }
            }

        }
                                        // Return no error.
    *pResult = 0;
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnOK
//
// Purpose:
//    OK Button handler.  We are going to get the current server
//    selection and free all of the network tree data if we have a
//    valid selection.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnOK()
{
CString cszBrowserSelectedItemName;
DWORD   dwSelectedItemType;

                                        // This function will determine if the
                                        // item that is selected is among those
                                        // that we wish to keep.
    GetDlgItemText( IDE_SELECTED_ITEM_NAME, m_cszSelectedItemName );

                                        // Only allow OK to go through if we
                                        // have text in our edit box.
    if ( m_cszSelectedItemName.GetLength() )
        {
                                        // Get the selected item name from the
                                        // tree control.
        m_cNetTreeCtrl.GetSelectedItemInformation( &cszBrowserSelectedItemName, &dwSelectedItemType );

                                        // See if what is selected in the browser
                                        // tree is the same as what is in the
                                        // edit field.
        if ( !_tcsicmp( cszBrowserSelectedItemName, m_cszSelectedItemName ) )
            {
                                        // Item is the same, so we will set the
                                        // item type to the selected item's
                                        // type.
            m_dwSelectedItemType = dwSelectedItemType;
            }
        else
            {
                                        // Set the item type to custom if we did
                                        // not select it though the browser, but
                                        // rather simply typed it in.
            m_dwSelectedItemType = RESOURCE_TYPE_CUSTOM;
            }

                                        // Return TRUE that we were able to get
                                        // a server successfully.
        CDialog::OnOK();
        }
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnCancel
//
// Purpose:
//    Cancel button handler.  We are going to get the current server
//    selection and free all of the network tree data if we have a
//    valid selection.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnCancel()
{
                                        // Cancel was pressed...
     CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnSelchangedNetTree
//
// Purpose:
//    This will handle the OnSelchanged message if you would like to
//    highlight a button or set a text item as to the network
//    resource selected.
//
// Parameters:
//    NMHDR* pNMHDR    - [IN]  Standard tree selection item header.
//    LRESULT* pResult - [OUT] Result.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnSelchangedNetTree(
    NMHDR* pNMHDR,
    LRESULT* pResult
    )
{
NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
CString cszItemName;
DWORD dwItemType;

    m_cNetTreeCtrl.GetSelectedItemInformation( &cszItemName, &dwItemType );

                                        // Check to see if it is a valid
                                        // resource type that we wish to keep.
        if ( m_cNetTreeCtrl.IsValidSelectionType( dwItemType ) )
        {
                                        // This is a good item.
                                        // Now, set our text string to show a
                                        // valid selection object.
        GetDlgItem( IDE_SELECTED_ITEM_NAME )->SendMessage( WM_SETTEXT,
            0,
            (LPARAM)(LPCTSTR)cszItemName );
        }
    else
        {
                                        // This is a not a valid item.
                                        // Now, reset our text string to NULL
                                        // in order to reflect an invalid
                                        // selection object.
        GetDlgItem( IDE_SELECTED_ITEM_NAME )->SendMessage( WM_SETTEXT,
            0,
            (LPARAM)(LPCTSTR)_T("") );
        }


    *pResult = 0;
}


int CNetBrowsDlg::DoModal()
{
                                        // Need to switch state to load this resource
                                        // out of the correct module.
        CDLLResourceState switchState;

        return CDialog::DoModal();
}

void CNetBrowsDlg::OnChangeServerName()
{
CString cszServerName;

    GetDlgItemText( IDE_SELECTED_ITEM_NAME, cszServerName );

                                        // If we have an item name in the
                                        // selection box...
    if ( cszServerName.GetLength() )
        {
                                        // Enable the "OK" button to allow
                                        // selection.
        GetDlgItem( IDOK )->EnableWindow( TRUE );
        }
    else
        {
                                        // Text item is empty, so disable the
                                        // "OK" button as to not allow
                                        // this selection.
        GetDlgItem( IDOK )->EnableWindow( FALSE );
        }
}



/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::SetBrowserType
//
// Purpose:
//    This function will set the browser type for this dialog.  This
//    function will set this variable up so that in our init dialog,
//    we will initialize all of the configurable items in the dialog
//    that have not been configured yet.
//
// Parameters:
//    LPCTSTR cszInitialBrowseItemName - [IN] Initial Browse Item Name.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::SetBrowserType(
    DWORD dwBrowserType
    )
{
DWORD dwDialogTitleID;
DWORD dwDialogInformationTextID;
                                        // Need to switch state to load this resource
                                        // out of the correct module.
    CDLLResourceState switchState;

                                        // This browser type will be used
                                        // during the init dialog in order to
                                        // determin such things as dialog title,
                                        // as well as static text in the
                                        // dialog.
    m_dwBrowserType = dwBrowserType;

                                        // Set the display flags according to the
                                        // browser type.
    switch( m_dwBrowserType )
        {
        case BROWSER_TYPE_NLM:
            dwDialogTitleID           = IDS_DEFAULT_BROWSER_DIALOG_TITLE_NLM;
            dwDialogInformationTextID = IDS_DEFAULT_DIALOG_INFORMATION_TEXT_NLM;
            m_dwValidSelectionFlags   = RESOURCE_TYPE_SERVER;
            m_dwBrowseBeneathFlags    = RESOURCE_TYPE_ALL & ~(RESOURCE_TYPE_SERVER);
            m_dwDisplayItemFlags      = RESOURCE_TYPE_ALL & ~(RESOURCE_TYPE_NETWORK_MICROSOFT);
            break;

        case BROWSER_TYPE_FORWARD:
            dwDialogTitleID           = IDS_DEFAULT_BROWSER_DIALOG_TITLE_FORWARD;
            dwDialogInformationTextID = IDS_DEFAULT_DIALOG_INFORMATION_TEXT_FORWARD;
            m_dwValidSelectionFlags   = RESOURCE_TYPE_SERVER;
            m_dwBrowseBeneathFlags    = RESOURCE_TYPE_ALL & ~(RESOURCE_TYPE_SERVER);
            m_dwDisplayItemFlags      = RESOURCE_TYPE_ALL & ~(RESOURCE_TYPE_NETWORK_NETWARE);
            break;

        case BROWSER_TYPE_NETWORK_MESSAGE:
            dwDialogTitleID           = IDS_DEFAULT_BROWSER_DIALOG_TITLE_NETWORK_MESSAGE;
            dwDialogInformationTextID = IDS_DEFAULT_DIALOG_INFORMATION_TEXT_NETWORK_MESSAGE;
            m_dwValidSelectionFlags   = RESOURCE_TYPE_SERVER | RESOURCE_TYPE_DOMAIN;
            m_dwBrowseBeneathFlags    = RESOURCE_TYPE_ALL & ~(RESOURCE_TYPE_SERVER);
            m_dwDisplayItemFlags      = RESOURCE_TYPE_ALL & ~(RESOURCE_TYPE_NETWORK_NETWARE);
            break;

        case BROWSER_TYPE_DEFAULT:
        default:
            dwDialogTitleID           = IDS_DEFAULT_BROWSER_DIALOG_TITLE;
            dwDialogInformationTextID = IDS_DEFAULT_DIALOG_INFORMATION_TEXT;
            m_dwValidSelectionFlags   = RESOURCE_TYPE_SERVER | RESOURCE_TYPE_DOMAIN;
            m_dwBrowseBeneathFlags    = RESOURCE_TYPE_ALL & ~(RESOURCE_TYPE_SERVER);
            m_dwDisplayItemFlags      = RESOURCE_TYPE_ALL;
            break;
        }

                                        // Load the window title string.
    m_cszDialogTitle.LoadString( dwDialogTitleID );

                                        // Load the window default information
                                        // text.
    m_cszInitialBrowserInformationText.LoadString( dwDialogInformationTextID );
}



/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::SetDialogTitle
//
// Purpose:
//    This function will provide a way that a client of this dialog
//    can set the title caption for this dialog before the dialog is
//    created.  Simply call this function will the title name that
//    you wish to appear in the caption of this dialog, and presto,
//    it shall be done.
//
// Parameters:
//    LPCTSTR cszNewTitle - [IN] New dialog title.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::SetDialogTitle(
    LPCTSTR cszNewTitle
    )
{
                                        // Set our internal title veriable
                                        // to the title that you sent up.
    m_cszDialogTitle = cszNewTitle;
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::SetInitialBrowseItemName
//
// Purpose:
//    This function will set the initial browse item name.  This
//    would be for if you are going to edit an existing item
//    through this dialog.
//
// Parameters:
//    LPCTSTR cszInitialBrowseItemName - [IN] Initial Browse Item Name.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::SetInitialBrowseItemName(
    LPCTSTR cszInitialBrowseItemName
    )
{
                                        // Set our internal initial browse
                                        // item name to the name that you sent
                                        // up.
    m_cszInitialBrowseItemName = cszInitialBrowseItemName;
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::SetInitialBrowseItemTitle
//
// Purpose:
//    Set's the initial browser item static text title.  This function
//    should be called before this dialog is invoked.  This information
//    will be used be InitializeBrowseDialog() to initialize the
//    controls within this dialog.
//
// Parameters:
//    LPCTSTR cszInitialBrowseItemTitle - [IN] Initial Browse Item Title.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::SetInitialBrowseItemTitle(
    LPCTSTR cszInitialBrowseItemTitle
    )
{
                                        // Set our internal initial browser
                                        // item title to the name that you
                                        // designated.
    m_cszInitialBrowseItemTitle = cszInitialBrowseItemTitle;
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::SetInitialBrowserInformationText
//
// Purpose:
//    Set's the initial browser information text.  This function
//    should be called before this dialog is invoked.  This information
//    will be used be InitializeBrowseDialog() to initialize the
//    controls within this dialog.
//
// Parameters:
//    LPCTSTR cszInitialBrowserInformationText - [IN] Initial browser
//                                  information text.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::SetInitialBrowserInformationText(
    LPCTSTR cszInitialBrowserInformationText
    )
{
                                        // Copy the initial browser information
                                        // text to our local variable.
    m_cszInitialBrowserInformationText = cszInitialBrowserInformationText;
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::InitializeBrowserDialog
//
// Purpose:
//    This function will initialize the browser dialog with all of
//    the data that you have set up previous to the creation of this
//    dialog.  This information will be in the form of setting the
//    window text, and the item text, or simply setting the browser
//    type, and allowing the default strings to be laoded for that
//    browser type.  This function will only be called through the
//    InitDialog() funciton.  This is because this will actually
//    initialize the items, and not only set up the members to
//    initialize.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//      These functions are used to initialize the state of this dialog
//    BEFORE we get to this function.  They will determine how this
//    function is to operate.  these functions are :
//
//      SetBrowserType() - Set the browser type for default initialization.
//      SetDialogType - Function to set the browser title before you actually
//                      call to this dialog.
//      SetInitialBrowserItemName() - Set the browser item name.(static text)
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::InitializeBrowserDialog( void )
{
                                        // Need to switch state to load this resource
                                        // out of the correct module.
    CDLLResourceState switchState;

                                        // If no custom dialog title has been
                                        // set...
    if ( 0 == m_cszDialogTitle.GetLength() )
        {
                                        // Load a default browse dialog title
                                        // from our string table.
        m_cszDialogTitle.LoadString( IDS_DEFAULT_BROWSER_DIALOG_TITLE );
        }

    if ( 0 == m_cszInitialBrowseItemTitle.GetLength() )
        {
                                        // Load the default text for the
                                        // static text title for the
                                        // selection edit box.
        m_cszInitialBrowseItemTitle.LoadString( IDS_DEFAULT_BROWSE_ITEM_TITLE );
        }

    if ( 0 == m_cszInitialBrowserInformationText.GetLength() )
        {
                                        // Load the default text for the
                                        // browser information text.
        m_cszInitialBrowserInformationText.LoadString( IDS_DEFAULT_DIALOG_INFORMATION_TEXT );
        }

                                        // Set the dialog caption with either the
                                        // default title, or a custom one, preset
                                        // before this time.
    SetWindowText( m_cszDialogTitle );

                                        // Set the initial browser information text.
    GetDlgItem( IDT_BROWSER_INFORMATION_TEXT )->SetWindowText( m_cszInitialBrowserInformationText );

                                        // Set the initial selected item title.
    GetDlgItem( IDT_BROWSE_ITEM_TITLE )->SetWindowText( m_cszInitialBrowseItemTitle );

                                        // Set the initial browse item name.
    GetDlgItem( IDE_SELECTED_ITEM_NAME )->SetWindowText( m_cszInitialBrowseItemName );

                                        // Set up the browser type.
        m_cNetTreeCtrl.SetValidSelectionFlags( m_dwValidSelectionFlags );
        m_cNetTreeCtrl.SetBrowseBeneathFlags( m_dwBrowseBeneathFlags );
        m_cNetTreeCtrl.SetDisplayItemFlags( m_dwDisplayItemFlags );
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnRclickNetTree
//
// Purpose:
//    Right click mouse handler function.  This function will track
//    right mouse button popup menus and call to the help system to
//    retrieve help.
//
// Parameters:
//    NMHDR * pNMHDR,  - [IN]  Notification message header.
//    LRESULT* pResult - [OUT] Result of our handling.
//
// Returns:
//    0 - No error occurred.
//    Anything Else an error occurred.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnRclickNetTree(
    NMHDR* pNMHDR,
    LRESULT* pResult
    )
{
//    TrackPopupMenuWithID( IDM_CONTEXT_TREE, this );

        *pResult = 0;
}

/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::TrackPopupMenuWithID
//
// Purpose:
//    This function will track a popup menu in the current location
//    of the mouse based upon the position of the last sent message.
//
// Parameters:
//    UINT uMenuID - [IN] ID of menu to track at cursor position.
//    CWnd  *lpcDialog
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void TrackPopupMenuWithID(
    UINT  uMenuID,
    CWnd  *lpcCommandWindow
    )
{
CMenu       cContextMenu;
CMenu      *lpcPopupMenu;
DWORD       dwPos;
POINT       pointScreen;

                                        // Switch to this module's resource
                                        // state before loading the resoruces.
   CDLLResourceState switchState;

                                        // get the screen coordinates
    dwPos = GetMessagePos();
    pointScreen.x = LOWORD (dwPos);
    pointScreen.y = HIWORD (dwPos);

                                        // Load the menu from our resoruce file.
    VERIFY( cContextMenu.LoadMenu( uMenuID ) );

                                        // Get's our popup Menu.
    lpcPopupMenu = cContextMenu.GetSubMenu( 0 );

    if ( lpcPopupMenu )
        {
                                        // Track this popup window.
        lpcPopupMenu->TrackPopupMenu(
            TPM_LEFTALIGN | TPM_RIGHTBUTTON
            , pointScreen.x
            , pointScreen.y
            , lpcCommandWindow);
        }
    }

void CNetBrowsDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
const MSG *lpCurrentMessage;

    static  DWORD       dwWhatsThisMap[] =
        {
        IDC_NET_TREE,                   IDH_NAVNT_WHATHIS_NETWORK_BROWSER_TREE,
        IDT_BROWSER_INFORMATION_TEXT,   IDH_NAVNT_WHATHIS_NETWORK_BROWSER_TREE,
        IDT_BROWSE_ITEM_TITLE,          IDH_NAVNT_WHATHIS_TARGET_TXTBX,
        0, 0
        };

                                        // -------------------------------------
                                        // "How To" map - empy, so far ...
                                        // -------------------------------------
    static  DWORD       dwHowToMap[] =
        {
        IDH_HT_NAVNT_TASK_USE_THE_NETWORK_BROWSER , IDH_HT_NAVNT_TASK_USE_THE_NETWORK_BROWSER,
        0,          0
        };
                                        // -------------------------------------
                                        // combo -  mappig to its items help
                                        // -------------------------------------
    static  DWORD       dwLBxItemMap[] =
        {
        0
        };

                                        // Function call to get the current
                                        // message that we are processing in
                                        // the CWnd.
    lpCurrentMessage = GetCurrentMessage();

//*************************************//
//*************************************//
//**   Temporarily removed so that   **//
//**   we will remove dependencies.  **//
//*************************************//
//*************************************//

//    NavDlgOnContextHelp(            /* Invoke Common Context Help Handling */
//                        m_hWnd,
//                        lpCurrentMessage->message,
//                        lpCurrentMessage->wParam,
//                        lpCurrentMessage->lParam,
//                        dwWhatsThisMap,
//                        dwHowToMap,
//                        dwLBxItemMap,
//                        NULL        /* ComboBox is a Default item selctor  */
//                        );
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnWindowProc
//
// Purpose:
//    This function will do all window proc function not associated
//    with the message mappings.  These will include items like the
//    Minimum and Maximum size of the dialog.
//
// Parameters:
//    UINT message  - [IN] Message sent.
//    WPARAM wParam - [IN] wParam of the message.
//    LPARAM lParam - [IN] lParam of the message.
//
// Returns:
//    Whether we handled this message or not.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
LRESULT CNetBrowsDlg::WindowProc(
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    )
{
MINMAXINFO *lpInfo;

        switch ( message )
      {
      case WM_GETMINMAXINFO :

         lpInfo = (MINMAXINFO *) lParam;
         lpInfo->ptMinTrackSize.x = m_nMinTrackSizeX;
         lpInfo->ptMinTrackSize.y = m_nMinTrackSizeY;

         break;
      }

        return CDialog::WindowProc(message, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::CalculateMinMax
//
// Purpose:
//    This function will calculate the minimum width and height of
//    this dialog and set our member variables accordingly.
//    These are m_nMinTrackSizeX and m_nMinTrackSizeY.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::CalculateMinMax( void )
{
                                        // This size calculated by setting the window
                                        // to the minimum size that looks good on the
                                        // screen on my machine, and then calculating
                                        // it to dialog units based on my machine's units.
CRect cMyRect(0, 0, 184, 110 );

                                        // Map these dialog units to the local computer's
                                        // dialog units.
    MapDialogRect( cMyRect );

                                        // Set our minimum values accordingly.
    m_nMinTrackSizeX = cMyRect.right;
    m_nMinTrackSizeY = cMyRect.bottom;
}


DWORD CNetBrowsDlg::GetDisplayItemFlags( void )
{
    return m_dwDisplayItemFlags;
}

DWORD CNetBrowsDlg::GetBrowseBeneathFlags( void )
{
    return m_dwBrowseBeneathFlags;
}

DWORD CNetBrowsDlg::GetValidSelectionFlags( void )
{
    return m_dwValidSelectionFlags;
}

void CNetBrowsDlg::SetDisplayItemFlags( DWORD dwNewDisplayItemFlags )
{
    m_dwDisplayItemFlags = dwNewDisplayItemFlags;
}

void CNetBrowsDlg::SetBrowseBeneathFlags( DWORD dwNewBrowseBeneathFlags )
{
    m_dwBrowseBeneathFlags = dwNewBrowseBeneathFlags;
}

void CNetBrowsDlg::SetValidSelectionFlags( DWORD dwNewValidSelectionFlags )
{
    m_dwValidSelectionFlags = dwNewValidSelectionFlags;
}

void CNetBrowsDlg::GetDialogTitle( CString *lpcReturnString )
{
    *lpcReturnString = m_cszDialogTitle;
}

void CNetBrowsDlg::GetInitialBrowserInformationText( CString *lpcReturnString )
{
    *lpcReturnString = m_cszInitialBrowserInformationText;
}

BOOL CNetBrowsDlg::DestroyWindow()
{
                                        // Kill all controls currently tracked
                                        // by the window resizer.
        m_cWindowResizer.RemoveAllItems();

        return CDialog::DestroyWindow();
}

void CNetBrowsDlg::SetClippingForSizeGrip( void )
{
CRect cHelpButtonRect;
CRect cSizeGripRect;
CRect cOverlapRect;
CDC   *lpcSizeGripDeviceContext;
int nReturn;

                                        // Only deal with size grip in versions
                                        // of windows later than or equal to 4.0.
    if ( glWindowsVersion >= 4 )
        {
        GetDlgItem( IDHELP )->GetWindowRect( &cHelpButtonRect );
        GetDlgItem( IDC_SIZEGRIP_SCROLL_BAR )->GetWindowRect( &cSizeGripRect );

        if ( TRUE == cOverlapRect.IntersectRect( cSizeGripRect, cHelpButtonRect ) )
            {
            cOverlapRect.right -= cOverlapRect.left;
            cOverlapRect.bottom -= cOverlapRect.top;
            cOverlapRect.left=0;
            cOverlapRect.top=0;

            cOverlapRect.right += 5;
            cOverlapRect.bottom += 5;

            lpcSizeGripDeviceContext = GetDlgItem( IDC_SIZEGRIP_SCROLL_BAR )->GetDC();

            nReturn = lpcSizeGripDeviceContext->ExcludeClipRect( cOverlapRect );
            }
        }
}

void CNetBrowsDlg::DrawOverlapForSizeGrip( void )
{
CRect cHelpButtonRect;
CRect cSizeGripRect;
CRect cOverlapRect;
CRect cDialogClientRect;

    GetDlgItem( IDHELP )->RedrawWindow();
/*
                                        // Only deal with size grip in versions
                                        // of windows later than or equal to 4.0.
    if ( glWindowsVersion >= 4 )
        {
        GetDlgItem( IDHELP )->GetWindowRect( &cHelpButtonRect );
        GetDlgItem( IDC_SIZEGRIP_SCROLL_BAR )->GetWindowRect( &cSizeGripRect );

        if ( TRUE == cOverlapRect.IntersectRect( cSizeGripRect, cHelpButtonRect ) )
            {
            GetWindowRect( &cDialogClientRect );
            cOverlapRect.right -= cDialogClientRect.left;
            cOverlapRect.left -= cDialogClientRect.left;
            cOverlapRect.top -= cDialogClientRect.top;
            cOverlapRect.bottom -= cDialogClientRect.top;

            cOverlapRect.right -= cOverlapRect.left;
            cOverlapRect.bottom -= cOverlapRect.top;
            cOverlapRect.left=0;
            cOverlapRect.top=0;

            InvalidateRect( cOverlapRect );
            UpdateWindow();
            }
        }
*/
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::SetHelpFileInfo()
//
// Purpose:
//    Sets up the help topic and file name for the help button
//
// Parameters:
//    iTopicID - Topic id for help button.
//    cszHelpFileName - Full path to help file.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07/7/98 - DBUCHES - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::SetHelpFileInfo( int iTopicID, LPCTSTR cszHelpFileName )
{
    // Save off input values.
    m_iHelpButtonTopicID = iTopicID;
    m_sHelpFileName = cszHelpFileName;
}


/////////////////////////////////////////////////////////////////////
// CNetBrowsDlg::OnHelp()
//
// Purpose:
//    Standard help for the help button on the Network Browser dialog.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/08/97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetBrowsDlg::OnHelp()
{
//    IDH_NAVW_BROWSE_DLG_HELP_BTN
    if( m_iHelpButtonTopicID == 0 )
        return;

    ::WinHelp( m_hWnd, m_sHelpFileName, HELP_CONTEXT, m_iHelpButtonTopicID );
}
