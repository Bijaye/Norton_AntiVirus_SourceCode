// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwPage.cpv   1.0   09 Mar 1998 23:45:12   DALLEE  $
//
// Description:
//      Implementation of CRepWizPage - base class for all Repair Wizard pages.
//
// Contains:
//      CRepWizPage::CRepWizPage()
//      CRepWizPage::~CRepWizPage()
//      CRepWizPage::DisplayableColors()
//      CRepWizPage::DoDataExchange()
//      CRepWizPage::GetMoveToState()
//      CRepWizPage::GetNextPage()
//      CRepWizPage::GetNoNav()
//      CRepWizPage::GetPreviousPage()
//      CRepWizPage::OnQueryCancel()
//      CRepWizPage::OnWizardBack()
//      CRepWizPage::OnWizardNext()
//      CRepWizPage::SetNoNav()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwPage.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:12   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
//#include "n_app.h"
#include "RWPage.h"

#include "nunavstb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRepWizPage property page

IMPLEMENT_DYNCREATE(CRepWizPage, CPropertyPage)


//*************************************************************************
// CRepWizPage::CRepWizPage()
// CRepWizPage::~CRepWizPage()
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

CRepWizPage::CRepWizPage()// : CPropertyPage(CRepWizPage::IDD)
{
    //{{AFX_DATA_INIT(CRepWizPage)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    // Remove help button.
    m_psp.dwFlags &= ~PSP_HASHELP;

    m_lpNoNav    = NULL;

    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

} // CRepWizPage::CRepWizPage()

CRepWizPage::CRepWizPage( int iDlgID ) : CPropertyPage(iDlgID)
{
    CRepWizPage();
} // CRepWizPage::CRepWizPage()

CRepWizPage::~CRepWizPage()
{
} // CRepWizPage::~CRepWizPage()


//*************************************************************************
// CRepWizPage::DoDataExchange()
//
// void CRepWizPage::DoDataExchange(
//      CDataExchange* pDX )
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

void CRepWizPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRepWizPage)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
} // CRepWizPage::DoDataExchange()


BEGIN_MESSAGE_MAP(CRepWizPage, CPropertyPage)
    //{{AFX_MSG_MAP(CRepWizPage)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRepWizPage message handlers


//*************************************************************************
// CRepWizPage::DisplayableColors()
//
// DWORD CRepWizPage::DisplayableColors( )
//
// This function determines how many colors can be displayed on screen.
// Should be used internally by Repair Wizard pages when deciding whether
// or not to display 256 color bitmaps.
//
// Returns:
//      Number of display colors for the screen.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
//*************************************************************************

DWORD CRepWizPage::DisplayableColors()
{
    HDC         hDC = ::GetDC ( ::GetDesktopWindow () );
    int         nColorDepth;
    DWORD       dwDisplayableColors;

    if ( GetDeviceCaps ( hDC, RASTERCAPS ) & RC_PALETTE )
        dwDisplayableColors = GetDeviceCaps ( hDC, SIZEPALETTE );
    else
        {
        nColorDepth = GetDeviceCaps ( hDC, BITSPIXEL ) *
                      GetDeviceCaps ( hDC, PLANES );
        if ( nColorDepth  >= 32 )
            dwDisplayableColors = 0xFFFFFFFF;
        else
            dwDisplayableColors = 1L << nColorDepth;
        }

    ::ReleaseDC ( ::GetDesktopWindow (), hDC );
    return ( dwDisplayableColors );
} // CRepWizPage::DisplayableColors()


//*************************************************************************
// CRepWizPage::SetNoNav()
//
// void CRepWizPage::SetNoNav(
//      LPNONAV lpNoNav )
//
// Stores a pointer to options and information on the current scan.
// Pointer must remain valid for the duration of the Repair Wizard.
//
// Returns:
//      Nothing.
//*************************************************************************
// 02/18/1998 DALLEE - Function created / Header added.
//*************************************************************************

void CRepWizPage::SetNoNav(LPVOID lpNoNav)
{
    m_lpNoNav = lpNoNav;
} // CRepWizPage::SetNoNav()


//*************************************************************************
// CRepWizPage::GetNoNav()
//
// LPNONAV CRepWizPage::GetNoNav( )
//
// Retrieves the stored pointer to options and current scan information.
//
// Returns:
//      LPNONAV options and scan info pointer.
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

LPVOID CRepWizPage::GetNoNav()
{
    return ( m_lpNoNav );
} // CRepWizPage::GetNoNav()


//*************************************************************************
// CRepWizPage::GetMoveToState()
//
// MOVETO_STATE CRepWizPage::GetMoveToState( )
//
// This function is queried for all pages when GetNextPage() is called.
//
// For internal use by GetNextPage()/GetPreviousPage().
//
// This default implementation will always return MOVETO_YES. A derived
// page should override this method if it ever needs to be skipped, or
// prevent the user from backing up.
//
// Returns:
//      MOVETO_YES          page should be displayed.
//      MOVETO_SKIP         should be skipped, not displayed.
//      MOVETO_NO           don't display this page or any
//                              previous pages.
//*************************************************************************
// 02/26/1998 DALLEE, created - header added.
//*************************************************************************

MOVETO_STATE CRepWizPage::GetMoveToState()
{
    return ( MOVETO_YES );
} // CRepWizPage::GetMoveToState()


//*************************************************************************
// CRepWizPage::OnWizardNext()
//
// LRESULT CRepWizPage::OnWizardNext( )
//
// This function determines the next panel to be displayed in the Repair Wizard.
// Uses GetNextPage() to query the following pages' GetMoveToState()
// functions to find the next panel.
//
// Returns:
//      ID of next page to display or
//      -1 if Next is not allowed.
//*************************************************************************
// 02/26/1998 DALLEE, created - header added.
//*************************************************************************

LRESULT CRepWizPage::OnWizardNext()
{
    return ( GetNextPage() );
} // CRepWizPage::OnWizardNext()


//*************************************************************************
// CRepWizPage::OnWizardBack()
//
// LRESULT CRepWizPage::OnWizardBack( )
//
// This function determines the previous panel to display in the Repair Wizard.
// Uses GetPreviousPage() to query the previous pages' GetMoveToState()
// functions to find the first available panel.
//
// Returns:
//      ID of previous page to display or
//      -1 if Back is not allowed.
//*************************************************************************
// 02/26/1998 DALLEE, created - header added.
//*************************************************************************

LRESULT CRepWizPage::OnWizardBack()
{
    return ( GetPreviousPage() );
} // CRepWizPage::OnWizardBack()


//*************************************************************************
// CRepWizPage::GetNextPage()
//
// LRESULT CRepWizPage::GetNextPage( )
//
// This routine returns the ID of the next page which should be displayed.
// It can be called to determine whether or not the Next button should
// be enabled.
// This routine calls the GetMoveToState() methods of the following pages
// to find the next one which can be displayed. Derived pages should
// override GetMoveToState() if they will need to be skipped.
//
// Returns:
//      ID of next page to display.
//      -1 if not allowed to advance to another page.
//*************************************************************************
// 03/02/1998 DALLEE, created - header added.
//*************************************************************************

LRESULT CRepWizPage::GetNextPage()
{
    auto    CPropertySheet  *pSheet;
    auto    CPropertyPage   *pPage;
    auto    LRESULT         lNextPage;
    auto    int             i;

    pSheet = (CPropertySheet *) GetParent();

    lNextPage = 0;

    for ( i = pSheet->GetActiveIndex() + 1;
          0 == lNextPage;
          i++ )
    {
        if ( i == pSheet->GetPageCount() )
        {
            // Stop if we're out of pages.

            lNextPage = -1;
            break;
        }

        pPage = pSheet->GetPage( i );

        ASSERT( pPage->IsKindOf( RUNTIME_CLASS( CRepWizPage ) ) );

        switch ( ((CRepWizPage *)pPage)->GetMoveToState() )
        {
        case MOVETO_YES:
            // Display this page next.

            lNextPage = ( LRESULT ) pPage->m_psp.pszTemplate;
            break;

        case MOVETO_NO:
            // Don't allow an advance to this page, or past it.

            lNextPage = ( LRESULT ) -1;
            break;

        case MOVETO_SKIP:
            // Keep looking for a valid page to display.
            break;
        }
    }

    return ( lNextPage );
} // CRepWizPage::GetNextPage()


//*************************************************************************
// CRepWizPage::GetPreviousPage()
//
// LRESULT CRepWizPage::GetPreviousPage( )
//
// This routine returns the ID of the preceding page which should be displayed.
// It can be called to determine whether or not the Back button should
// be enabled.
// This routine calls the GetMoveToState() methods of the previous pages
// to find the first one which can be displayed. Derived pages should
// override GetMoveToState() if they will need to be skipped or prevent
// moving back.
//
// Returns:
//      ID of previous page to display.
//      -1 if not allowed to go back to another page.
//*************************************************************************
// 03/02/1998 DALLEE, created - header added.
//*************************************************************************

LRESULT CRepWizPage::GetPreviousPage()
{
    auto    CPropertySheet  *pSheet;
    auto    CPropertyPage   *pPage;
    auto    LRESULT         lPreviousPage;
    auto    int             i;

    pSheet = (CPropertySheet *) GetParent();

    lPreviousPage = 0;

    i = pSheet->GetActiveIndex();

    while ( 0 == lPreviousPage )
    {
        if ( 0 == i )
        {
            // Stop if we're out of pages.

            lPreviousPage = -1;
            break;
        }

        i--;

        pPage = pSheet->GetPage( i );

        if ( pPage )
        {
            ASSERT( pPage->IsKindOf( RUNTIME_CLASS( CRepWizPage ) ) );

            switch ( ((CRepWizPage *)pPage)->GetMoveToState() )
            {
            case MOVETO_YES:
                // Display this page next.

                lPreviousPage = ( LRESULT ) pPage->m_psp.pszTemplate;
                break;

            case MOVETO_NO:
                // Don't allow an advance to this page, or past it.

                lPreviousPage = ( LRESULT ) -1;
                break;

            case MOVETO_SKIP:
                // Keep looking for a valid page to display.
                break;
            }
        }
        else
        {
            break;
        }
    }

    return ( lPreviousPage );
} // CRepWizPage::GetPreviousPage()


//*************************************************************************
// CRepWizPage::OnQueryCancel()
//
// BOOL CRepWizPage::OnQueryCancel( )
//
// This function prompts whether the user really want to cancel based on
// whether any items with live infections are left and there are actions
// which can still be taken to clean the infections.
//
// Returns:
//      TRUE to allow Cancel
//      FALSE to prevent Cancel.
//*************************************************************************
// 03/04/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CRepWizPage::OnQueryCancel()
{
    auto    BOOL    bAllowCancel = TRUE;

    return( bAllowCancel );
} // CRepWizPage::OnQueryCancel()


//*************************************************************************
// CRepWizPage::DisableButtons()
//
// BOOL CRepWizPage::DisableButtons()
//
// This function disables the back, next, and cancel buttons and changes 
// the cursor to an hourglass.
//
//*************************************************************************
// 09/08/1999 TCASHIN, created - header added.
//*************************************************************************
void CRepWizPage::DisableButtons()
{
    auto    CWnd*       pBackBtn = NULL;
    auto    CWnd*       pNextBtn = NULL;

    // Save the cursor and put up the hourglass
    m_hCurrentCursor = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    // Disable the Back & Next Buttons
    m_pButtonBack = GetParent()->GetDlgItem( ID_WIZBACK );
    m_pButtonNext = GetParent()->GetDlgItem( ID_WIZNEXT );
    m_pButtonCancel = GetParent()->GetDlgItem( IDCANCEL );

    if ( m_pButtonNext ) 
    {
        m_pButtonNext->EnableWindow( FALSE );
    }

    if ( m_pButtonBack )
    {
        m_pButtonBack->EnableWindow( FALSE );
    }

    if ( m_pButtonCancel )
    {
        m_pButtonCancel->EnableWindow( FALSE );
    }
}


//*************************************************************************
// CRepWizPage::EnableButtons()
//
// BOOL CRepWizPage::EnableButtons()
//
// This function enables the back, next, and cancel buttons and restores
// the cursor to its previous value.
//
// Returns:
//      TRUE to allow Cancel
//      FALSE to prevent Cancel.
//*************************************************************************
// 09/08/1999 TCASHIN, created - header added.
//*************************************************************************
void CRepWizPage::EnableButtons()
{

    // Restore the cursor
    if ( m_hCurrentCursor )
    {
        SetCursor(m_hCurrentCursor);
        m_hCurrentCursor = NULL;
    }

    if ( m_pButtonNext ) 
    {
        m_pButtonNext->EnableWindow( TRUE );
        m_pButtonNext = NULL;
    }

    if ( m_pButtonBack )
    {
        m_pButtonBack->EnableWindow( TRUE );
        m_pButtonBack = NULL;
    }

    if ( m_pButtonCancel )
    {
        m_pButtonCancel->EnableWindow( TRUE );
        m_pButtonCancel = NULL;
    }
}


