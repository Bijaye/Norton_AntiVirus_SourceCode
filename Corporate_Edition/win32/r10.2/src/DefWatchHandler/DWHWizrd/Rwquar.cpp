// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwquar.cpv   1.5   08 Jul 1998 14:24:00   tcashin  $
//
// Description:
//      Implementation of CRepWizPage.
//      Quarantine all non-repaired items wizard page.
//
// Contains:
//      CRepWizQuarantine::CRepWizQuarantine()
//      CRepWizQuarantine::~CRepWizQuarantine()
//      CRepWizQuarantine::DoDataExchange()
//      CRepWizQuarantine::GetMoveToState()
//      CRepWizQuarantine::OnInitDialog()
//      CRepWizQuarantine::OnSetActive()
//      CRepWizQuarantine::OnWizardNext()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwquar.cpv  $
// 
//    Rev 1.5   08 Jul 1998 14:24:00   tcashin
// Don't need to check for ST_ZIP ... it's covered in ST_FILE.
// 
//    Rev 1.4   01 Jul 1998 16:20:18   tcashin
// Check for ST_ZIP status.
// 
//    Rev 1.3   28 Jun 1998 13:05:10   TCASHIN
// Only put up the quarantine panel if there are infected files. D
// put infected put MBR or BR infections in the quarantine panel listbox.
// 
//    Rev 1.2   01 Jun 1998 16:13:22   tcashin
// Changes to support adding infected compressed files to quarantine.
// 
//    Rev 1.1   06 May 1998 04:33:24   tcashin
// Fixed two bugs in GetMoveToState: always returned MOVETO_YES and 
// tested the wrong option (bDispDelete instead of bDispQuarantine).
// 
//    Rev 1.0   03 Apr 1998 11:17:00   tcashin
// Initial revision.
// 
//*************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "nunavstb.h"

#include "RWQuar.h"
#include "rwizard.h"
#include "dwhwiz.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CRepWizQuarantine, CRepWizPage)


/////////////////////////////////////////////////////////////////////////////
// CRepWizQuarantine property page


//*************************************************************************
// CRepWizQuarantine::CRepWizQuarantine()
// CRepWizQuarantine::~CRepWizQuarantine()
//
// Returns:
//      Nothing.
//*************************************************************************
// 04/01/1998 TCashin, header added.
//*************************************************************************

CRepWizQuarantine::CRepWizQuarantine() : CRepWizPage(CRepWizQuarantine::IDD)
{
    //{{AFX_DATA_INIT(CRepWizQuarantine)
    m_iQuarantineItems = 0;
    //}}AFX_DATA_INIT

    m_bDidQuarantine = FALSE;

} // CRepWizQuarantine::CRepWizQuarantine()

CRepWizQuarantine::~CRepWizQuarantine()
{
} // CRepWizQuarantine::~CRepWizQuarantine()


//*************************************************************************
// CRepWizQuarantine::GetMoveToState()
//
// MOVETO_STATE CRepWizQuarantine::GetMoveToState( )
//
// This function is queried for all pages when GetNextPage() is called.
// A repair wizard page should override this method if it ever needs to
// be skipped.
//
// For internal use by GetNextPage()/GetPreviousPage().
//
// The quarantine page is only displayed when the quarantine function is enabled
// and there are live viruses in the list of problems found.
//
// Returns:
//      MOVETO_YES          page should be displayed.
//      MOVETO_SKIP         should be skipped, not displayed.
//      MOVETO_NO           don't display this page or any
//                              previous pages.
//*************************************************************************
// 04/01/1998 TCashin, header added.
//*************************************************************************

MOVETO_STATE CRepWizQuarantine::GetMoveToState()
{
    auto    MOVETO_STATE    nState = MOVETO_SKIP;

    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    if ( m_pDWHWizApp->GetWizState() == DWH_WIZ_STATE_REPAIR_LATER )
        nState = MOVETO_YES;

    if ( m_pDWHWizApp->GetWizState() == DWH_WIZ_STATE_CONTINUE )
    {
        if ( FALSE == m_bDidQuarantine )
        {
            // If we haven't tried to retstore, then either show or skip
            // based on whether there are entries which can be deleted.

            // Call a function to go through the CQuarItem array and see if anyting
            // was repaired

            if ( m_pDWHWizApp->CheckFiles(QUARITEM_STATE_REPAIRABLE) )
                nState = MOVETO_YES;
            else
                nState = MOVETO_SKIP;


            // nState = IsStatusTypeInList( GetNoNav(), (ST_LIVEVIRUS | ST_FILE) ) && GetNoNav()->Opt.bDispQuarantine
            //              ? MOVETO_YES : MOVETO_SKIP;


        }
        else
        {
            // If we've tried to quarantine, then we can never back up to this
            // panel, nor any previous panels.

            nState = MOVETO_NO;
        }
    }

    return ( nState );
} // CRepWizQuarantine::GetMoveToState()


//*************************************************************************
// CRepWizQuarantine::DoDataExchange()
//
// void CRepWizQuarantine::DoDataExchange(
//      CDataExchange* pDX )
//
// Returns:
//      Nothing.
//*************************************************************************
// 04/01/1998 TCashin, header added.
//*************************************************************************

void CRepWizQuarantine::DoDataExchange(CDataExchange* pDX)
{
    CRepWizPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRepWizQuarantine)
    DDX_Control(pDX, IDC_LISTVIEW, m_ListCtrl);
    DDX_Control(pDX, IDC_PICTURE, m_Picture);
    DDX_Radio(pDX, IDC_RADIO1, m_iQuarantineItems);
    //}}AFX_DATA_MAP
} // CRepWizQuarantine::DoDataExchange()


BEGIN_MESSAGE_MAP(CRepWizQuarantine, CRepWizPage)
    //{{AFX_MSG_MAP(CRepWizQuarantine)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*************************************************************************
// CRepWizQuarantine::OnInitDialog()
//
// BOOL CRepWizQuarantine::OnInitDialog( )
//
// Initalizes delete page's controls on WM_INITDIALOG.
// Note: problem list will be filled by OnSetActive, not here. We want
// the most current information when we switch to this panel.
//
// Returns:
//      TRUE to accept default focus.
//*************************************************************************
// 04/01/1998 TCashin, header added.
//*************************************************************************

BOOL CRepWizQuarantine::OnInitDialog()
{
    CRepWizPage::OnInitDialog();

    // Set up bitmap
    int iBitmapID = DisplayableColors() > 256 ? IDB_WZ256_7 : IDB_WZ_16_7;
    m_Picture.SetBitmap( iBitmapID,
                         WIZARD_PICTURE_WIDTH,
                         WIZARD_PICTURE_HEIGHT );

    // Initialize the problem list control
    m_ListCtrl.Init( NULL, 2 );

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
} // CRepWizQuarantine::OnInitDialog()


//*************************************************************************
// CRepWizQuarantine::OnSetActive()
//
// BOOL CRepWizQuarantine::OnSetActive( )
//
// Called when delete page becomes active. Need to fill problem list
// with all items which can be deleted.
//
// Returns:
//      Nonzero if the page was successfully set active, otherwise 0.
//*************************************************************************
// 04/01/1998 TCashin, header added.
//*************************************************************************

BOOL CRepWizQuarantine::OnSetActive()
{
    auto    DWORD   dwButtons;

    dwButtons = PSWIZB_NEXT;

    ((CPropertySheet*) GetParent())->SetWizardButtons( dwButtons );

    m_ListCtrl.FillList( QUARITEM_STATE_REPAIRABLE );

    return CRepWizPage::OnSetActive();
} // CRepWizQuarantine::OnSetActive()


//*************************************************************************
// CRepWizQuarantine::OnWizardNext()
//
// LRESULT CRepWizQuarantine::OnWizardNext( )
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

LRESULT CRepWizQuarantine::OnWizardNext()
{
    auto    int         iNumItems = 0;
    auto    int         iNumRestored = 0;
    auto    int         i = 0;
    auto    CQuarItem*  lpItem = NULL;


    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    // Get data from controls.
    UpdateData();

    if( m_iQuarantineItems )
    {
        m_pDWHWizApp->SetWizState(DWH_WIZ_STATE_REPAIR_LATER);
    }
    else
    {
        m_pDWHWizApp->SetWizState(DWH_WIZ_STATE_CONTINUE);
        m_bDidQuarantine = TRUE;

     // m_pDWHWizApp->RestoreFiles();

        DisableButtons();

        // Get the number of items
        iNumItems = m_ListCtrl.GetItemCount();

        // Scan and repair them all
        for ( i=0;i<iNumItems;i++)
        {
            lpItem = (CQuarItem*)m_ListCtrl.GetItemData(i);

            if ( lpItem )
            {
                // Restore this item to its original location, moving
                // the quarantine record to "Backup" if successful.
                if ( lpItem->Restore( TRUE ) )
                {
                    iNumRestored++;
                }
            }
        }

        EnableButtons();

    }

    if ( !iNumRestored )
        m_pDWHWizApp->SetWizState( DWH_WIZ_STATE_NONE_RESTORED );

    // Advance to the next page.

    return ( GetNextPage() );
} // CRepWizQuarantine::OnWizardNext()


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

LRESULT CRepWizQuarantine::OnWizardPrev()
{
    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    return ( GetPreviousPage() );
}
