// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwDelete.cpv   1.0   09 Mar 1998 23:45:02   DALLEE  $
//
// Description:
//      Implementation of CRepWizPage3.
//      Delete all non-repaired items wizard page.
//
// Contains:
//      CRepWizDelete::CRepWizDelete()
//      CRepWizDelete::~CRepWizDelete()
//      CRepWizDelete::DoDataExchange()
//      CRepWizDelete::GetMoveToState()
//      CRepWizDelete::OnInitDialog()
//      CRepWizDelete::OnSetActive()
//      CRepWizDelete::OnWizardNext()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwDelete.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:02   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "nunavstb.h"
#include "RWDelete.h"
#include "dwhwiz.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CRepWizDelete, CRepWizPage)


/////////////////////////////////////////////////////////////////////////////
// CRepWizDelete property page


//*************************************************************************
// CRepWizDelete::CRepWizDelete()
// CRepWizDelete::~CRepWizDelete()
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

CRepWizDelete::CRepWizDelete() : CRepWizPage(CRepWizDelete::IDD)
{
    //{{AFX_DATA_INIT(CRepWizDelete)
    m_iDeleteItems = 0;
    //}}AFX_DATA_INIT

    m_bDidDelete = FALSE;
} // CRepWizDelete::CRepWizDelete()

CRepWizDelete::~CRepWizDelete()
{
} // CRepWizDelete::~CRepWizDelete()


//*************************************************************************
// CRepWizDelete::GetMoveToState()
//
// MOVETO_STATE CRepWizDelete::GetMoveToState( )
//
// This function is queried for all pages when GetNextPage() is called.
// A repair wizard page should override this method if it ever needs to
// be skipped.
//
// For internal use by GetNextPage()/GetPreviousPage().
//
// The delete page is only displayed when the delete function is enabled
// and there are live viruses which can be deleted in the list of
// problems found.
//
// Returns:
//      MOVETO_YES          page should be displayed.
//      MOVETO_SKIP         should be skipped, not displayed.
//      MOVETO_NO           don't display this page or any
//                              previous pages.
//*************************************************************************
// 02/26/1998 DALLEE, created - header added.
//*************************************************************************

MOVETO_STATE CRepWizDelete::GetMoveToState()
{
    auto    MOVETO_STATE    nState = MOVETO_SKIP;


    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    if ( m_pDWHWizApp->GetWizState() == DWH_WIZ_STATE_CONTINUE )
    {    
        if ( FALSE == m_bDidDelete )
        {
            // If we haven't tried deleting, then either show or skip
            // based on whether there are entries which can be deleted.

            m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

            if ( m_pDWHWizApp->CheckFiles(QUARITEM_STATE_RESTORED_TO_ORIG_LOC) )
            {
                nState = MOVETO_YES;
            }
            else
                nState = MOVETO_SKIP;
        }
        else
        {
            // If we've tried deleting, then we can never back up to this
            // panel, nor any previous panels.

            nState = MOVETO_NO;
        }
    }

    return ( nState );
} // CRepWizDelete::GetMoveToState()


//*************************************************************************
// CRepWizDelete::DoDataExchange()
//
// void CRepWizDelete::DoDataExchange(
//      CDataExchange* pDX )
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

void CRepWizDelete::DoDataExchange(CDataExchange* pDX)
{
    CRepWizPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRepWizDelete)
    DDX_Control(pDX, IDC_LISTVIEW, m_ListCtrl);
    DDX_Control(pDX, IDC_PICTURE, m_Picture);
    DDX_Radio(pDX, IDC_RADIO1, m_iDeleteItems);
    //}}AFX_DATA_MAP
} // CRepWizDelete::DoDataExchange()


BEGIN_MESSAGE_MAP(CRepWizDelete, CRepWizPage)
    //{{AFX_MSG_MAP(CRepWizDelete)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*************************************************************************
// CRepWizDelete::OnInitDialog()
//
// BOOL CRepWizDelete::OnInitDialog( )
//
// Initalizes delete page's controls on WM_INITDIALOG.
// Note: problem list will be filled by OnSetActive, not here. We want
// the most current information when we switch to this panel.
//
// Returns:
//      TRUE to accept default focus.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
// 02/1998 DALLEE, Added list initialization.
//*************************************************************************

BOOL CRepWizDelete::OnInitDialog()
{
    CRepWizPage::OnInitDialog();

    // Set up bitmap
    int iBitmapID = DisplayableColors() > 256 ? IDB_WZ256_3 : IDB_WZ_16_3;
    m_Picture.SetBitmap( iBitmapID,
                         WIZARD_PICTURE_WIDTH,
                         WIZARD_PICTURE_HEIGHT );

    // Initialize the problem list control
    m_ListCtrl.Init( NULL, 1 );

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
} // CRepWizDelete::OnInitDialog()


//*************************************************************************
// CRepWizDelete::OnSetActive()
//
// BOOL CRepWizDelete::OnSetActive( )
//
// Called when delete page becomes active. Need to fill problem list
// with all items which can be deleted.
//
// Returns:
//      Nonzero if the page was successfully set active, otherwise 0.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
// 02/1998 DALLEE, Added list initialization.
//*************************************************************************

BOOL CRepWizDelete::OnSetActive()
{
    auto    DWORD   dwButtons;


    dwButtons = PSWIZB_NEXT;

    // Check if we can go backwards.

    if ( -1 != GetPreviousPage() )
    {
        dwButtons |= PSWIZB_BACK;
    }

    ((CPropertySheet*) GetParent())->SetWizardButtons( dwButtons );

    // Fill this with the list of replaced files
    m_ListCtrl.FillList( QUARITEM_STATE_RESTORED_TO_ORIG_LOC );

    return CRepWizPage::OnSetActive();
} // CRepWizDelete::OnSetActive()


//*************************************************************************
// CRepWizDelete::OnWizardNext()
//
// LRESULT CRepWizDelete::OnWizardNext( )
//
// Deletes all deletable items displayed in the problem item list then
// advances to next panel.
//
// Records fact that delete was attempted so we display Close instead of
// cancel and can prevent user from backing up to this panel.
//
// Returns:
//      ID of next panel to display.
//*************************************************************************
// 2/12/98 DBUCHES - Function created / Header added.
// 03/02/1998 DALLEE, added repair all guts.
//*************************************************************************

LRESULT CRepWizDelete::OnWizardNext()
{
    auto    CString     strClose;

    // Get data from controls.
    UpdateData();

    if ( DELETE_ITEMS == m_iDeleteItems )
    {
        // Attempt to delete all items.

        // Made irreversible change.
        // Need to change text of Cancel button to Close.
        // Note that in a wizard, CancelToClose() will disable the Cancel button.
        // This is not what we want.

        strClose.LoadString( IDS_RW_CLOSE );
        GetParent()->SetDlgItemText( IDCANCEL, strClose );
    }

    // Advance to the next page.

    return ( GetNextPage() );
} // CRepWizDelete::OnWizardNext()


//*************************************************************************
// CRepWizDelete::OnReplaceFiles()
//
// Calls RestoreFiles() in CDWHWizrdApp to restore what files it can. 
//
// Returns: Nothing. 
//      
//*************************************************************************
// 4/15/99 TCASHIN - Function created / Header added.
//*************************************************************************
void CRepWizDelete::ReplaceFiles()
{
    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    DisableButtons();

    m_pDWHWizApp->RestoreFiles();

    EnableButtons();
}


//*************************************************************************
// CRepWizQuarantine::OnWizardPrev()
//
// LRESULT CRepWizQuarantine::OnWizardPrev( )
//
// Quarantine all infected/deletable items displayed in the problem item list then
// advances to next panel.
//
// Records fact that quarantine was attempted so we display Close instead of
// cancel and can prevent user from backing up to this panel.
//
// Returns:
//      ID of next panel to display.
//*************************************************************************
// 04/01/1998 TCashin, header added.
//*************************************************************************

LRESULT CRepWizDelete::OnWizardPrev()
{
    return ( GetPreviousPage() );
}
