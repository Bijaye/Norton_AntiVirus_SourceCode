// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwfincln.cpv   1.3   26 Jun 1998 15:13:06   jtaylor  $
//
// Description:
//      Implementation of CRepWizFinishClean. Repair Wizard final panel if all
//      infected were "cleaned" - Repaired, (Quarantined?), Deleted.
//
// Contains:
//      CRepWizFinishClean::CRepWizFinishClean()
//      CRepWizFinishClean::~CRepWizFinishClean()
//      CRepWizFinishClean::GetMoveToState()
//      CRepWizFinishClean::OnInitDialog()
//      CRepWizFinishClean::OnMoreInfoButton()
//      CRepWizFinishClean::OnSetActive()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwfincln.cpv  $
// 
//    Rev 1.3   26 Jun 1998 15:13:06   jtaylor
// Dissabled Quaranine items if Quarantine is not enabled.
//
//    Rev 1.2   07 Jun 1998 15:21:44   tcashin
// Change the text of the last panel if we put files in Quarantine (#114308).
//
//    Rev 1.1   12 May 1998 14:05:40   tcashin
// Added the number of files quarantined and deleted.
//
//    Rev 1.0   09 Mar 1998 23:45:06   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "nunavstb.h"
#include "nunavdlg.h"
#include "RWFinCln.h"

#ifndef SYM_DOS
    #include "n32behav.h" // Personality DLL
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CRepWizFinishClean, CRepWizPage)


/////////////////////////////////////////////////////////////////////////////
// CRepWizFinishClean property page

//*************************************************************************
// CRepWizFinishClean::CRepWizFinishClean()
// CRepWizFinishClean::~CRepWizFinishClean()
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

CRepWizFinishClean::CRepWizFinishClean() : CRepWizPage(CRepWizFinishClean::IDD)
{
    //{{AFX_DATA_INIT(CRepWizFinishClean)
    m_sInfected = _T("");
    m_sScanned = _T("");
    m_sCleaned = _T("");
    m_sQuarantined = _T("");
    m_sDeleted = _T("");
    //}}AFX_DATA_INIT
} // CRepWizFinishClean::CRepWizFinishClean()

CRepWizFinishClean::~CRepWizFinishClean()
{
} // CRepWizFinishClean::~CRepWizFinishClean()


//*************************************************************************
// CRepWizFinishClean::GetMoveToState()
//
// MOVETO_STATE CRepWizFinishClean::GetMoveToState( )
//
// This function is queried for all pages when GetNextPage() is called.
// A repair wizard page should override this method if it ever needs to
// be skipped.
//
// For internal use by GetNextPage()/GetPreviousPage().
//
// The final all clean page is displayed if there are no infected items
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

MOVETO_STATE CRepWizFinishClean::GetMoveToState()
{
    return ( IsStatusTypeInList( GetNoNav(), ST_LIVEVIRUS ) ? MOVETO_SKIP : MOVETO_YES );
} // CRepWizFinishClean::GetMoveToState()


//*************************************************************************
// CRepWizFinishClean::DoDataExchange()
//
// void CRepWizFinishClean::DoDataExchange(
//      CDataExchange* pDX )
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

void CRepWizFinishClean::DoDataExchange(CDataExchange* pDX)
{
    CRepWizPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRepWizFinishClean)
    DDX_Control(pDX, IDC_PICTURE, m_Picture);
    DDX_Text(pDX, IDC_ITEMS_INFECTED_STATIC, m_sInfected);
    DDX_Text(pDX, IDC_ITEMS_SCANNED_STATIC, m_sScanned);
    DDX_Text(pDX, IDC_ITEMS_REPAIRED_STATIC, m_sCleaned);
    DDX_Text(pDX, IDC_ITEMS_DELETED_STATIC, m_sDeleted);
    DDX_Text(pDX, IDC_ITEMS_QUARANTINED_STATIC, m_sQuarantined);
    DDX_Text(pDX, IDC_FINISH_TEXT, m_sFinishText);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRepWizFinishClean, CRepWizPage)
    //{{AFX_MSG_MAP(CRepWizFinishClean)
    ON_BN_CLICKED(IDC_MORE_INFO_BUTTON, OnMoreInfoButton)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*************************************************************************
// CRepWizFinishClean::OnInitDialog()
//
// BOOL CRepWizFinishClean::OnInitDialog( )
//
// Initalizes final all clean page's controls on WM_INITDIALOG.
//
// Returns:
//      TRUE to accept default focus.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
//*************************************************************************

BOOL CRepWizFinishClean::OnInitDialog()
{
    CRepWizPage::OnInitDialog();

    // Set up bitmap
    int iBitmapID = DisplayableColors() > 256 ? IDB_WZ256_5 : IDB_WZ_16_5;
    m_Picture.SetBitmap( iBitmapID,
                         WIZARD_PICTURE_WIDTH,
                         WIZARD_PICTURE_HEIGHT );

#ifndef SYM_DOS
    if( !NAV_IS_QUARANTINE() )
        {
        GetDlgItem(IDC_ITEMS_QUARANTINED_STATIC)->EnableWindow(FALSE);
        }
#endif

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
} // CRepWizFinishClean::OnInitDialog()


//*************************************************************************
// CRepWizFinishClean::OnSetActive()
//
// BOOL CRepWizFinishClean::OnSetActive( )
//
// Displays proper wizard panel buttons. Sets scan statistics text.
//
// Returns:
//      Nonzero if the page was successfully set active, otherwise 0.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
// 03/1998 DALLEE, initialize statistics/controls.
//*************************************************************************

BOOL CRepWizFinishClean::OnSetActive()
{
    auto    DWORD   dwButtons;
    auto    TCHAR   szNumber [40];

    auto    DWORD   dwScanned;
    auto    DWORD   dwInfected;
    auto    DWORD   dwCleaned;
    auto    DWORD   dwQuar;
    auto    DWORD   dwDeleted;

    auto    LPNONAV lpNav;

    // Display statistics.

    lpNav = GetNoNav();

    dwScanned  = lpNav->Stats.Files.dwScanned +
                 lpNav->Stats.BootRecs.uScanned +
                 lpNav->Stats.MasterBootRec.uScanned;

    dwInfected = lpNav->Stats.Files.uInfected +
                 lpNav->Stats.BootRecs.uInfected +
                 lpNav->Stats.MasterBootRec.uInfected;

    dwCleaned  = lpNav->Stats.Files.uCleaned +
                 lpNav->Stats.MasterBootRec.uCleaned +
                 lpNav->Stats.BootRecs.uCleaned;

    dwQuar     = lpNav->Stats.Files.uQuarantined;

    dwDeleted  = lpNav->Stats.Files.uDeleted;

    // Using CORE's ConvertLongToString() to get commas.

    ConvertLongToString( dwScanned, szNumber );
    m_sScanned.Format( IDS_SUMM_SCANNED, szNumber );

    ConvertLongToString( dwInfected, szNumber );
    m_sInfected.Format( IDS_SUMM_INFECTED, szNumber );

    ConvertLongToString( dwCleaned, szNumber );
    m_sCleaned.Format( IDS_SUMM_CLEANED, szNumber );

    ConvertLongToString( dwQuar, szNumber );
    m_sQuarantined.Format( IDS_SUMM_QUARANTINED, szNumber );

    ConvertLongToString( dwDeleted, szNumber );
    m_sDeleted.Format( IDS_SUMM_DELETED, szNumber );

    if ( dwQuar > 0 )
        m_sFinishText.LoadString(IDS_FINISH_TEXT_ATTENTION);
    else
        m_sFinishText.LoadString(IDS_FINISH_TEXT_CONGRATS);

    UpdateData( FALSE );

    // Set buttons to be displayed.

    dwButtons = PSWIZB_FINISH;

    if ( -1 != GetPreviousPage() )
    {
        dwButtons |= PSWIZB_BACK;
    }

    ((CPropertySheet*) GetParent())->SetWizardButtons( dwButtons );

    return CRepWizPage::OnSetActive();
} // CRepWizFinishClean::OnSetActive()


//*************************************************************************
// CRepWizFinishClean::OnMoreInfoButton()
//
// void CRepWizFinishClean::OnMoreInfoButton( )
//
// Displays Details of Scan dialog when More Info button is pressed.
//
// Returns:
//      Nothing.
//*************************************************************************
// 2/12/98 DBUCHES - Function created / Header added.
// 03/05/1998 DALLEE, guts.
//*************************************************************************

void CRepWizFinishClean::OnMoreInfoButton()
{
    ShowScanSumDlg( GetSafeHwnd(), GetNoNav() );
} // CRepWizFinishClean::OnMoreInfoButton()

