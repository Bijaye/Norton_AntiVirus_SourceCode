// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwStill.cpv   1.0   09 Mar 1998 23:45:10   DALLEE  $
//
// Description:
//      Implementation of CRepWizStillInfected. This page displays the
//      items which are still infected after the user has passed the
//      "action" panels - Repair, (Quarantine?), Delete.
//
// Contains:
//      CRepWizStillInfected::CRepWizStillInfected()
//      CRepWizStillInfected::~CRepWizStillInfected()
//      CRepWizStillInfected::DoDataExchange()
//      CRepWizStillInfected::GetMoveToState()
//      CRepWizStillInfected::OnAdviceButton()
//      CRepWizStillInfected::OnDblclkListview()
//      CRepWizStillInfected::OnInitDialog()
//      CRepWizStillInfected::OnSetActive()
//
//      GetTextWhyStillInfected()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwStill.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:10   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "RwStill.h"
#include "nunavstb.h"
#include "dwhwiz.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// External data

extern HINSTANCE    hInstance;          // Global name-mangled hInstance. Grr...


// Local prototypes

VOID GetTextWhyStillInfected (
    LPVOID  lpInfected,
    LPSTR   lpszSentence );     // [in/out] Resulting sentence; should be >= 6*SYM_MAX_PATH


IMPLEMENT_DYNCREATE(CRepWizStillInfected, CRepWizPage)


/////////////////////////////////////////////////////////////////////////////
// CRepWizStillInfected property page


//*************************************************************************
// CRepWizStillInfected::CRepWizStillInfected()
// CRepWizStillInfected::~CRepWizStillInfected()
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

CRepWizStillInfected::CRepWizStillInfected() : CRepWizPage(CRepWizStillInfected::IDD)
{
    //{{AFX_DATA_INIT(CRepWizStillInfected)
    //}}AFX_DATA_INIT
} // CRepWizStillInfected::CRepWizStillInfected()

CRepWizStillInfected::~CRepWizStillInfected()
{
} // CRepWizStillInfected::~CRepWizStillInfected()


//*************************************************************************
// CRepWizStillInfected::GetMoveToState()
//
// MOVETO_STATE CRepWizStillInfected::GetMoveToState( )
//
// This function is queried for all pages when GetNextPage() is called.
// A repair wizard page should override this method if it ever needs to
// be skipped.
//
// For internal use by GetNextPage()/GetPreviousPage().
//
// The still infected page is displayed if there are any infected items
// remaining.
//
// Returns:
//      MOVETO_YES          page should be displayed.
//      MOVETO_SKIP         should be skipped, not displayed.
//      MOVETO_NO           don't display this page or any
//                              previous pages.
//*************************************************************************
// 03/02/1998 DALLEE, created - header added.
//*************************************************************************

MOVETO_STATE CRepWizStillInfected::GetMoveToState()
{
    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    if ( m_pDWHWizApp->GetWizState() == DWH_WIZ_STATE_NOT_NOW )
        return ( MOVETO_YES );
    else
        return ( MOVETO_SKIP );
} // CRepWizStillInfected::GetMoveToState()


//*************************************************************************
// CRepWizStillInfected::DoDataExchange()
//
// void CRepWizStillInfected::DoDataExchange(
//      CDataExchange* pDX )
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

void CRepWizStillInfected::DoDataExchange(CDataExchange* pDX)
{
    CRepWizPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRepWizStillInfected)
    DDX_Control(pDX, IDC_LISTVIEW, m_ListCtrl);
    DDX_Control(pDX, IDC_PICTURE, m_Picture);
    //}}AFX_DATA_MAP
} // CRepWizStillInfected::DoDataExchange()


BEGIN_MESSAGE_MAP(CRepWizStillInfected, CRepWizPage)
    //{{AFX_MSG_MAP(CRepWizStillInfected)
    ON_BN_CLICKED(IDC_ADVICE_BUTTON, OnAdviceButton)
    ON_NOTIFY(NM_DBLCLK, IDC_LISTVIEW, OnDblclkListview)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*************************************************************************
// CRepWizStillInfected::OnInitDialog()
//
// BOOL CRepWizStillInfected::OnInitDialog( )
//
// Initalizes still infected page's controls on WM_INITDIALOG.
// Note: problem list will be filled by OnSetActive, not here. We want
// the most current information when we switch to this panel.
//
// Returns:
//      TRUE to accept default focus.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
// 02/1998 DALLEE, Added list initialization.
//*************************************************************************

BOOL CRepWizStillInfected::OnInitDialog()
{
    CRepWizPage::OnInitDialog();

    // Set up bitmap
    int iBitmapID = DisplayableColors() > 256 ? IDB_WZ256_6 : IDB_WZ_16_6;
    m_Picture.SetBitmap( iBitmapID,
                         WIZARD_PICTURE_WIDTH,
                         WIZARD_PICTURE_HEIGHT );

    // Initialize the problem list control
    m_ListCtrl.Init( NULL, 2 );

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
} // CRepWizStillInfected::OnInitDialog()


//*************************************************************************
// CRepWizStillInfected::OnSetActive()
//
// BOOL CRepWizStillInfected::OnSetActive( )
//
// Called when still infected page becomes active. Need to fill problem list
// with all items which are still infected.
//
// Returns:
//      Nonzero if the page was successfully set active, otherwise 0.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
// 02/1998 DALLEE, Added list initialization.
//*************************************************************************

BOOL CRepWizStillInfected::OnSetActive()
{
    auto    DWORD   dwButtons;

    dwButtons = PSWIZB_NEXT | PSWIZB_FINISH;

    if ( -1 != GetPreviousPage() )
    {
        dwButtons |= PSWIZB_BACK;
    }

    ((CPropertySheet*) GetParent())->SetWizardButtons( dwButtons );

    m_ListCtrl.FillList( QUARITEM_STATE_QUARANTINED );

    return CRepWizPage::OnSetActive();
} // CRepWizStillInfected::OnSetActive()


//*************************************************************************
// CRepWizStillInfected::OnDblclkListview()
//
// void CRepWizStillInfected::OnDblclkListview(
//      NMHDR*   pNMHDR
//      LRESULT* pResult )
//
// Displays advice for the current item in the list of items still infected.
// Uses OnAdviceButton() for displaying advice.
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/02/1998 DALLEE, created - header added.
//*************************************************************************

void CRepWizStillInfected::OnDblclkListview(NMHDR* pNMHDR, LRESULT* pResult)
{
    OnAdviceButton();

    *pResult = 0;
} // CRepWizStillInfected::OnDblclkListview()


//*************************************************************************
// CRepWizStillInfected::OnAdviceButton()
//
// void CRepWizStillInfected::OnAdviceButton( )
//
// Displays why still infected advice for the current item in the
// problem list control when the Advice button is cliked.
// Note: this routine is also called by OnDblclkListview().
//
// Returns:
//      Nothing.
//*************************************************************************
// 2/12/98 DBUCHES - Function created / Header added.
// 03/02/1998 DALLEE, guts.
//*************************************************************************

void CRepWizStillInfected::OnAdviceButton()
{
} // CRepWizStillInfected::OnAdviceButton()


//*************************************************************************
// CRepWizDelete::OnWizardBack()
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

LRESULT CRepWizStillInfected::OnWizardBack()
{
    return ( GetPreviousPage() );
}


//*************************************************************************
// GetTextWhyStillInfected()
//
// VOID LOCAL GetTextWhyStillInfected (
//      LPINFECTEDFILE  lpInfected
//      LPSTR           lpszSentence )  [out] Buffer should be >= 6 * SYM_MAX_PATH
//
// Creates the text message for the Advice dialog.
//
// Returns: VOID
//
//*************************************************************************
// 02/24/1998 DALLEE, pulled GWESTER's SetText_WhyStillInfected() from n_wizard.cpp 1.7
//*************************************************************************

VOID GetTextWhyStillInfected (
    LPVOID  lpInfected,
    LPSTR   lpszSentence )
{

} // GetTextWhyStillInfected()

