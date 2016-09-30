// Copyright 1998 Symantec Corporation.
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
#include "nunavdlg.h"
#include "nunavstb.h"

#include "RWDelete.h"

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
    auto    MOVETO_STATE    nState;

    if ( FALSE == m_bDidDelete )
    {
        // If we haven't tried deleting, then either show or skip
        // based on whether there are entries which can be deleted.

        nState = IsStatusTypeInList( GetNoNav(), ST_CAN_DELETE ) && GetNoNav()->Opt.bDispDelete
                        ? MOVETO_YES : MOVETO_SKIP;
    }
    else
    {
        // If we've tried deleting, then we can never back up to this
        // panel, nor any previous panels.

        nState = MOVETO_NO;
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

    if ( m_lpNoNav )
    {
        m_ListCtrl.Init( m_lpNoNav->lpInfArr );
    }

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

    m_ListCtrl.FillList( ST_CAN_DELETE | ST_LIVEVIRUS );

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
    auto    DOALLSTRUCT Dall;

    // Get data from controls.
    UpdateData();

    if ( DELETE_ITEMS == m_iDeleteItems )
    {
        // Attempt to delete all items.

        MEMSET( &Dall, 0, sizeof( Dall ) );

        Dall.uFlags     = DA_NO_NOTIFY | DA_CONFIRM_DELETE;
        Dall.uType      = ASKQUESTION_DELETE_ALL_INFECTED;
        Dall.hListbox   = NULL;
        Dall.lpNav      = GetNoNav();

        DoAll( GetSafeHwnd(), &Dall);

        m_bDidDelete = TRUE;

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

