// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwintro.cpv   1.1   23 Mar 1998 18:19:40   DALLEE  $
//
// Description:
//      Implementation of CRepWizIntro. Introductory Repair Wizard page
//      displays infection information and allow choice of automatic (Wizard)
//      or manual (Problems Found dlg) mode to fix problems.
//
// Contains:
//      CRepWizIntro::CRepWizIntro()
//      CRepWizIntro::~CRepWizIntro()
//      CRepWizIntro::AddVirusToList()
//      CRepWizIntro::FillVirusList()
//      CRepWizIntro::IsVirusInList()
//      CRepWizIntro::OnDblclkListViruses()
//      CRepWizIntro::OnInitDialog()
//      CRepWizIntro::OnSetActive()
//      CRepWizIntro::OnVirusInfoButton()
//      CRepWizIntro::OnWizardNext()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwintro.cpv  $
// 
//    Rev 1.1   23 Mar 1998 18:19:40   DALLEE
// On Next after selecting manual mode, Use parent->EndDialog( ID_OK ) to
// exit instead of simulating press of IDCANCEL.
// Simulated cancel was displaying "Really exit?" message from
// OnQueryCancel() handler.
// 
//    Rev 1.0   09 Mar 1998 23:45:04   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "RWIntro.h"
#include "rwizard.h"
#include "dwhwiz.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CRepWizIntro, CRepWizPage)


/////////////////////////////////////////////////////////////////////////////
// CRepWizIntro property page


//*************************************************************************
// CRepWizIntro::CRepWizIntro()
// CRepWizIntro::~CRepWizIntro()
//
// Returns:
//      Nothing
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

CRepWizIntro::CRepWizIntro() : CRepWizPage(CRepWizIntro::IDD)
{
    //{{AFX_DATA_INIT(CRepWizIntro)
    m_iRepairMode = 0;
    //}}AFX_DATA_INIT
} // CRepWizIntro::CRepWizIntro()

CRepWizIntro::~CRepWizIntro()
{
} // CRepWizIntro::~CRepWizIntro()


//*************************************************************************
// CRepWizIntro::DoDataExchange()
//
// void CRepWizIntro::DoDataExchange(
//      CDataExchange* pDX )
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, header added.
//*************************************************************************

void CRepWizIntro::DoDataExchange(CDataExchange* pDX)
{
    CRepWizPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRepWizIntro)
//    DDX_Control(pDX, IDC_LIST_VIRUSES, m_VirusListBox);
    DDX_Control(pDX, IDC_PICTURE, m_Picture);
    DDX_Radio(pDX, IDC_RADIO_AUTOMATIC, m_iRepairMode);
    //}}AFX_DATA_MAP
} // CRepWizIntro::DoDataExchange()


BEGIN_MESSAGE_MAP(CRepWizIntro, CRepWizPage)
    //{{AFX_MSG_MAP(CRepWizIntro)
    ON_BN_CLICKED(IDC_BUTTON_VIRUS_INFO, OnVirusInfoButton)
    ON_LBN_DBLCLK(IDC_LIST_VIRUSES, OnDblclkListViruses)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*************************************************************************
// CRepWizIntro::OnInitDialog()
//
// BOOL CRepWizIntro::OnInitDialog( )
//
// Initializes introductory panel's controls on WM_INITDIALOG.
//
// Returns:
//      TRUE to accecpt default focus.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
//*************************************************************************

BOOL CRepWizIntro::OnInitDialog()
{
    CRepWizPage::OnInitDialog();

    // Set up bitmap
    int iBitmapID = DisplayableColors() > 256 ? IDB_WZ256_0 : IDB_WZ_16_0;
    m_Picture.SetBitmap( iBitmapID,
                         WIZARD_PICTURE_WIDTH,
                         WIZARD_PICTURE_HEIGHT );

    FillVirusList();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
} // CRepWizIntro::OnInitDialog()


//*************************************************************************
// CRepWizIntro::OnSetActive()
//
// BOOL CRepWizIntro::OnSetActive( )
//
// Reset wizard buttons as page becomes active.
//
// Returns:
//      Nonzero if the page was successfully set active, otherwise 0.
//*************************************************************************
// 2/11/98 DBUCHES - Function created / Header added.
//*************************************************************************

BOOL CRepWizIntro::OnSetActive()
{
    // Disable the back button.
    ((CPropertySheet*) GetParent())->SetWizardButtons( PSWIZB_NEXT );

    return CRepWizPage::OnSetActive();
} // CRepWizIntro::OnSetActive()


//*************************************************************************
// CRepWizIntro::OnVirusInfoButton()
//
// void CRepWizIntro::OnVirusInfoButton( )
//
// Handler for virus info button click.
// Display information on current item in list of viruses.
// Note: this is also the called by the virus list double click handler,
// OnDblclkListViruses().
//
// Returns:
//      Nothing.
//*************************************************************************
// 2/12/98 DBUCHES - Function created / Header added.
//*************************************************************************

void CRepWizIntro::OnVirusInfoButton()
{
} // CRepWizIntro::OnVirusInfoButton()


//*************************************************************************
// CRepWizIntro::OnDblclkListViruses()
//
// void CRepWizIntro::OnDblclkListViruses( )
//
// Display virus information on the current item in the list of viruses.
//
// Returns:
//      Nothing
//*************************************************************************
// 03/04/1998 DALLEE, created - header added.
//*************************************************************************

void CRepWizIntro::OnDblclkListViruses()
{
    OnVirusInfoButton();
} // CRepWizIntro::OnDblclkListViruses()


//*************************************************************************
// CRepWizIntro::OnWizardNext()
//
// LRESULT CRepWizIntro::OnWizardNext( )
//
// Handles Next button click. If manual mode selected, exits wizard.
// Manual mode can be checked by caller with CRepairWizard::GetManualMode().
//
// Returns:
//      ID of next page to display or -1 if not allowed to advance.
//*************************************************************************
// 2/12/98 DBUCHES - Function created / Header added.
//*************************************************************************

LRESULT CRepWizIntro::OnWizardNext()
{
    m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();

    // Get data from controls.
    UpdateData();

    if( m_iRepairMode )
    {
        m_pDWHWizApp->SetWizState(DWH_WIZ_STATE_NOT_NOW);
    }
    else
        m_pDWHWizApp->SetWizState(DWH_WIZ_STATE_CONTINUE);

    // Advance to the next page.
    return ( GetNextPage() );
} // CRepWizIntro::OnWizardNext()


//*************************************************************************
// CRepWizIntro::FillVirusList()
//
// BOOL CRepWizIntro::FillVirusList( )
//
// Loads the virus list box with the names of all viruses found during the
// scan. Sets item data in the listbox to a LPINFECTEDFILE for use by
// virus info routines.
//
// For internal use during init dialog.
//
// Returns:
//      TRUE on success, FALSE otherwise.
//*************************************************************************
// 02/18/1998 DALLEE - Function created / Header added.
//*************************************************************************

BOOL CRepWizIntro::FillVirusList()
{

    auto    BOOL            bSuccess = TRUE;

#ifdef DWTEST

    auto    LPINFARR        lpBlock;
    auto    LPINFECTEDFILE  lpItem;
    auto    LPNONAV         lpNoNav;
    auto    UINT            i;

    auto    BOOL            bSuccess;

    lpNoNav = GetNoNav();

    if ( lpNoNav )
    {
        lpBlock = lpNoNav->lpInfArr;

        // Loop through all blocks of problem items looking for live virus items.
        // Record all unique virus ID's and add virus names to list box.

        while ( lpBlock )
        {
            lpItem = ( LPINFECTEDFILE ) MemLock( lpBlock->hInf );

            if ( lpItem )
            {
                for ( i = 0; i < lpBlock->uNumUsed; i++ )
                {
                    if ( IsMatchingStatusType( lpItem[i].wStatus, ST_LIVEVIRUS ) &&
                         !IsVirusInList( lpItem[i].Notes.wVirID ) )
                    {
                        AddVirusToList( lpItem + i );
                    }
                }

                MemUnlock( lpBlock->hInf );
                lpBlock = ( LPINFARR ) lpBlock->lpNext;
            }
            else
            {
                // Unable to lock this block's entries, just exit.

                bSuccess = FALSE;
                break;
            }
        }
    }
    else
    {
        bSuccess = FALSE;
    }

    if ( bSuccess )
        m_VirusListBox.SetCurSel( 0 );

#endif

    return ( bSuccess );

} // CRepWizIntro::FillVirusList()


//*************************************************************************
// CRepWizIntro::IsVirusInList()
//
// BOOL CRepWizIntro::IsVirusInList(
//      WORD wVirID )                   [in] Virus ID
//
// This function determines whether a virus identified by wVirID is
// already in the list of viruses found during the scan.
//
// For internal use by FillVirusList().
//
// Returns:
//      TRUE if virus is already in list, FALSE otherwise
//*************************************************************************
// 02/18/1998 DALLEE - Function created / Header added.
//*************************************************************************

BOOL CRepWizIntro::IsVirusInList( WORD wVirID )
{
    auto    vector<WORD>::iterator    pID;

    auto    BOOL    bInList;

    bInList = FALSE;

    for ( pID = m_vectViruses.begin();
          ( pID != m_vectViruses.end() ) && !bInList;
          pID++ )
    {
        if ( wVirID == *pID )
            bInList = TRUE;
    }

    return ( bInList );
} // CRepWizIntro::IsVirusInList()


//*************************************************************************
// CRepWizIntro::AddVirusToList()
//
// BOOL CRepWizIntro::AddVirusToList(
//      LPINFECTEDFILE lpItem )         [in] Infected item which holds virus info.
//
// Adds the virus infecting lpItem to the viruses list box and adds its ID
// to the list of IDs already added to the list box.
//
// For internal use by FillVirusList().
//
// Returns:
//      TRUE on success, FALSE otherwise.
//*************************************************************************
// 02/18/1998 DALLEE - Function created / Header added.
//*************************************************************************

BOOL CRepWizIntro::AddVirusToList( LPVOID lpItem )
{

    auto    BOOL    bSuccess = TRUE;

    return ( bSuccess );
} // CRepWizIntro::AddVirusToList()

