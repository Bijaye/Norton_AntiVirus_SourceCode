// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwRepair.cpv   1.0   09 Mar 1998 23:45:06   DALLEE  $
//
// Description:
//      Implementation of CRepWizRepair.
//      Repair all wizard page.
//
// Contains:
//      CRepWizRepair::CRepWizRepair()
//      CRepWizRepair::~CRepWizRepair()
//      CRepWizRepair::DoDataExchange()
//      CRepWizRepair::GetMoveToState()
//      CRepWizRepair::OnInitDialog()
//      CRepWizRepair::OnSetActive()
//      CRepWizRepair::OnWizardNext()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwRepair.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:06   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "nunavstb.h"
#include "RwRepair.h"
#include "quaritem.h"
#include "vpcommon.h"
#include "dwhwiz.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CRepWizRepair, CRepWizPage)


/////////////////////////////////////////////////////////////////////////////
// CRepWizRepair property page


//*************************************************************************
// CRepWizRepair::CRepWizRepair()
// CRepWizRepair::~CRepWizRepair()
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

CRepWizRepair::CRepWizRepair() : CRepWizPage(CRepWizRepair::IDD)
{
    //{{AFX_DATA_INIT(CRepWizRepair)
    //}}AFX_DATA_INIT

    m_bDidRepair = FALSE;
} // CRepWizRepair::CRepWizRepair()

CRepWizRepair::~CRepWizRepair()
{
} // CRepWizRepair::~CRepWizRepair()


//*************************************************************************
// CRepWizRepair::GetMoveToState()
//
// MOVETO_STATE CRepWizRepair::GetMoveToState( )
//
// This function is queried for all pages when GetNextPage() is called.
// A repair wizard page should override this method if it ever needs to
// be skipped.
// For internal use by GetNextPage()/GetPreviousPage().
//
// The repair page is only displayed when the repair function is enabled
// and there are live viruses which can be repaired in the list of
// problems found.
//
// Returns:
//      MOVETO_YES          page should be displayed.
//      MOVETO_SKIP         should be skipped, not displayed.
//      MOVETO_NO           don't display this page or any
//                              previous pages.
//*************************************************************************
// 03/02/1998 DALLEE, created - header added.
//*************************************************************************

MOVETO_STATE CRepWizRepair::GetMoveToState()
{
    auto    MOVETO_STATE    nState = MOVETO_SKIP;

    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    if ( m_pDWHWizApp->GetWizState() == DWH_WIZ_STATE_CONTINUE )
    {
        if ( FALSE == m_bDidRepair )
        {
            // If we haven't tried repairing, then either show or skip
            // based on whether there are entries which can be repaired.

            nState = MOVETO_YES;
        }
        else
        {
            // If we've tried repairing, then we can never back up to this
            // panel, nor any previous panels.

            nState = MOVETO_NO;
        }
    }

    return ( nState );
} // CRepWizRepair::GetMoveToState()


//*************************************************************************
// CRepWizRepair::DoDataExchange()
//
// void CRepWizRepair::DoDataExchange(
//      CDataExchange* pDX )
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

void CRepWizRepair::DoDataExchange(CDataExchange* pDX)
{
    CRepWizPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRepWizRepair)
    DDX_Control(pDX, IDC_LISTVIEW, m_ListCtrl);
    DDX_Control(pDX, IDC_PICTURE, m_Picture);
    //}}AFX_DATA_MAP
} // CRepWizRepair::DoDataExchange()


BEGIN_MESSAGE_MAP(CRepWizRepair, CRepWizPage)
    //{{AFX_MSG_MAP(CRepWizRepair)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*************************************************************************
// CRepWizRepair::OnInitDialog()
//
// BOOL CRepWizRepair::OnInitDialog( )
//
// Initalizes repair page's controls on WM_INITDIALOG.
// Note: problem list will be filled by OnSetActive, not here. We want
// the most current information when we switch to this panel.
//
// Returns:
//      TRUE to accept default focus.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
// 02/1998 DALLEE, Added list initialization.
//*************************************************************************

BOOL CRepWizRepair::OnInitDialog()
{
    CRepWizPage::OnInitDialog();

    // Set up bitmap
    int iBitmapID = DisplayableColors() > 256 ? IDB_WZ256_2 : IDB_WZ_16_2;
    m_Picture.SetBitmap( iBitmapID,
                         WIZARD_PICTURE_WIDTH,
                         WIZARD_PICTURE_HEIGHT );

    // Initialize the problem list control
    m_ListCtrl.Init( NULL, 2 );

    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
} // CRepWizRepair::OnInitDialog()


//*************************************************************************
// CRepWizRepair::OnSetActive()
//
// BOOL CRepWizRepair::OnSetActive( )
//
// Called when repair page becomes active. Need to fill problem list
// with all items which can be repaired.
//
// Returns:
//      Nonzero if the page was successfully set active, otherwise 0.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
// 02/1998 DALLEE, Added list initialization.
//*************************************************************************

BOOL CRepWizRepair::OnSetActive()
{
    ((CPropertySheet*) GetParent())->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );

    BOOL bRet = CRepWizPage::OnSetActive();
    
    DisableButtons();

    m_ListCtrl.FillList( QUARITEM_STATE_QUARANTINED );

    EnableButtons();

    return bRet;
} // CRepWizRepair::OnSetActive()


//*************************************************************************
// CRepWizRepair::OnWizardNext()
//
// LRESULT CRepWizRepair::OnWizardNext( )
//
// Repairs all repairable items displayed in the problem item list then
// advances to next panel.
//
// Records fact that repair was attempted so we display Close instead of
// cancel and can prevent user from backing up to this panel.
//
// Returns:
//      ID of next panel to display.
//*************************************************************************
// 2/12/98 DBUCHES - Function created / Header added.
// 03/02/1998 DALLEE, added repair all guts.
//*************************************************************************

LRESULT CRepWizRepair::OnWizardNext()
{
    auto    CString     strClose;
    auto    int         iNumItems = 0;
    auto    int         iNumRepaired = 0;
    auto    int         i = 0;
    auto    CQuarItem*  lpItem = NULL;


    DisableButtons();

    // Get the number of items
    iNumItems = m_ListCtrl.GetItemCount();

    // Scan and repair them all
    for ( i=0;i<iNumItems;i++)
    {
        lpItem = (CQuarItem*)m_ListCtrl.GetItemData(i);

        if ( lpItem )
        {
            lpItem->Repair();

            if ( lpItem->GetState() == QUARITEM_STATE_REPAIRABLE )
                iNumRepaired++;
        }
    }

    EnableButtons();

    m_bDidRepair = TRUE;

    if ( !iNumRepaired )
        m_pDWHWizApp->SetWizState( DWH_WIZ_STATE_NONE_REPAIRED );

    // Made irreversible change.
    // Need to change text of Cancel button to Close.
    // Note that in a wizard, CancelToClose() will disable the Cancel button.
    // This is not what we want.

    strClose.LoadString( IDS_RW_CLOSE );
    GetParent()->SetDlgItemText( IDCANCEL, strClose );

    // Advance to the next page.
    return ( GetNextPage() );
} // CRepWizRepair::OnWizardNext()

