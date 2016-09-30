// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwfininf.cpv   1.2   26 Jun 1998 15:13:06   jtaylor  $
//
// Description:
//      Implementation of CRepWizFinishInfected. Repair Wizard final panel
//      if infected items still remain.
//
// Contains:
//      CRepWizFinishInfected::CRepWizFinishInfected()
//      CRepWizFinishInfected::~CRepWizFinishInfected()
//      CRepWizFinishInfected::DoDataExchange()
//      CRepWizFinishInfected::GetMoveToState()
//      CRepWizFinishInfected::OnInitDialog()
//      CRepWizFinishInfected::OnMoreInfoButton()
//      CRepWizFinishInfected::OnSetActive()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwfininf.cpv  $
// 
//    Rev 1.2   26 Jun 1998 15:13:06   jtaylor
// Dissabled Quaranine items if Quarantine is not enabled.
//
//    Rev 1.1   12 May 1998 14:06:26   tcashin
// Added the number of files quarantined and deleted.
//
//    Rev 1.0   09 Mar 1998 23:45:08   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "nunavdlg.h"
#include "RWFinInf.h"

#include "nunavstb.h"

#ifndef SYM_DOS
    #include "n32behav.h" // Personality DLL
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CRepWizFinishInfected, CRepWizPage)


/////////////////////////////////////////////////////////////////////////////
// CRepWizFinishInfected property page

//*************************************************************************
// CRepWizFinishInfected::CRepWizFinishInfected()
// CRepWizFinishInfected::~CRepWizFinishInfected()
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

CRepWizFinishInfected::CRepWizFinishInfected() : CRepWizPage(CRepWizFinishInfected::IDD)
{
    //{{AFX_DATA_INIT(CRepWizFinishInfected)
    m_sInfected = _T("");
    m_sScanned = _T("");
    m_sCleaned = _T("");
    m_sQuarantined = _T("");
    m_sDeleted = _T("");
    //}}AFX_DATA_INIT
} // CRepWizFinishInfected::CRepWizFinishInfected()

CRepWizFinishInfected::~CRepWizFinishInfected()
{
} // CRepWizFinishInfected::~CRepWizFinishInfected()


//*************************************************************************
// CRepWizFinishInfected::GetMoveToState()
//
// MOVETO_STATE CRepWizPage4::GetMoveToState( )
//
// This function is queried for all pages when GetNextPage() is called.
// A repair wizard page should override this method if it ever needs to
// be skipped.
//
// For internal use by GetNextPage()/GetPreviousPage().
//
// The final still infected page is displayed if there are any infected items
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

MOVETO_STATE CRepWizFinishInfected::GetMoveToState()
{
    return ( IsStatusTypeInList( GetNoNav(), ST_LIVEVIRUS ) ? MOVETO_YES : MOVETO_SKIP );
} // CRepWizFinishInfected::GetMoveToState()



//*************************************************************************
// CRepWizFinishInfected::DoDataExchange()
//
// void CRepWizFinishInfected::DoDataExchange(
//      CDataExchange* pDX )
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

void CRepWizFinishInfected::DoDataExchange(CDataExchange* pDX)
{
    CRepWizPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRepWizFinishInfected)
    DDX_Control(pDX, IDC_ANIMATE, m_Animation);
    DDX_Control(pDX, IDC_PICTURE, m_Picture);
    DDX_Text(pDX, IDC_ITEMS_INFECTED_STATIC, m_sInfected);
    DDX_Text(pDX, IDC_ITEMS_SCANNED_STATIC, m_sScanned);
    DDX_Text(pDX, IDC_ITEMS_REPAIRED_STATIC, m_sCleaned);
    DDX_Text(pDX, IDC_ITEMS_DELETED_STATIC, m_sDeleted);
    DDX_Text(pDX, IDC_ITEMS_QUARANTINED_STATIC, m_sQuarantined);
    //}}AFX_DATA_MAP
} // CRepWizFinishInfected::DoDataExchange()


BEGIN_MESSAGE_MAP(CRepWizFinishInfected, CRepWizPage)
    //{{AFX_MSG_MAP(CRepWizFinishInfected)
    ON_BN_CLICKED(IDC_MORE_INFO_BUTTON, OnMoreInfoButton)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*************************************************************************
// CRepWizFinishInfected::OnInitDialog()
//
// BOOL CRepWizFinishInfected::OnInitDialog( )
//
// Initalizes final still infected page's controls on WM_INITDIALOG.
//
// Returns:
//      TRUE to accept default focus.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
//*************************************************************************

BOOL CRepWizFinishInfected::OnInitDialog()
{
    CRepWizPage::OnInitDialog();

    // Set up bitmap
    int iBitmapID = DisplayableColors() > 256 ? IDB_WZ256_6 : IDB_WZ_16_6;
    m_Picture.SetBitmap( iBitmapID,
                         WIZARD_PICTURE_WIDTH,
                         WIZARD_PICTURE_HEIGHT );

    // Set up animation control
    m_Animation.Initialize( IDB_SIRENS, 16, SIREN_WIDTH, RGB( 0, 255 ,0 ) );

    // If Quarantine is not on the system, then dissable the Quarantine text.
#ifndef SYM_DOS
    if( !NAV_IS_QUARANTINE() )
        {
        GetDlgItem(IDC_ITEMS_QUARANTINED_STATIC)->EnableWindow(FALSE);
        }
#endif

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
} // CRepWizFinishInfected::OnInitDialog()


//*************************************************************************
// CRepWizFinishInfected::OnSetActive()
//
// BOOL CRepWizFinishInfected::OnSetActive( )
//
// Displays proper wizard panel buttons. Sets scan statistics text.
//
// Returns:
//      Nonzero if the page was successfully set active, otherwise 0.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
// 03/1998 DALLEE, initialize statistics/controls.
//*************************************************************************

BOOL CRepWizFinishInfected::OnSetActive()
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

    UpdateData( FALSE );

    // Set buttons to be displayed.

    dwButtons = PSWIZB_FINISH;

    if ( -1 != GetPreviousPage() )
    {
        dwButtons |= PSWIZB_BACK;
    }

    ((CPropertySheet*) GetParent())->SetWizardButtons( dwButtons );

    return CRepWizPage::OnSetActive();
} // CRepWizFinishInfected::OnSetActive()


//*************************************************************************
// CRepWizFinishInfected::OnMoreInfoButton()
//
// void CRepWizFinishInfected::OnMoreInfoButton( )
//
// Displays Details of Scan dialog when More Info button is pressed.
//
// Returns:
//      Nothing.
//*************************************************************************
// 2/12/98 DBUCHES - Function created / Header added.
// 03/05/1998 DALLEE, guts.
//*************************************************************************

void CRepWizFinishInfected::OnMoreInfoButton()
{
    ShowScanSumDlg( GetSafeHwnd(), GetNoNav() );
} // CRepWizFinishInfected::OnMoreInfoButton()

