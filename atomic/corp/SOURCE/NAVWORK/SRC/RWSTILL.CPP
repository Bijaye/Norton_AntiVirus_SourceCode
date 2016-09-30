// Copyright 1998 Symantec Corporation.
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
#include "nunavdlg.h"
#include "RwStill.h"

#include "nunavstb.h"
#include "cmbstr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// External data

extern HINSTANCE    hInstance;          // Global name-mangled hInstance. Grr...


// Local prototypes

VOID LOCAL GetTextWhyStillInfected (
    LPINFECTEDFILE  lpInfected,
    LPSTR           lpszSentence );     // [in/out] Resulting sentence; should be >= 6*SYM_MAX_PATH


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
    return ( IsStatusTypeInList( GetNoNav(), ST_LIVEVIRUS ) ? MOVETO_YES : MOVETO_SKIP );
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
    int iBitmapID = DisplayableColors() > 256 ? IDB_WZ256_4 : IDB_WZ_16_4;
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

    dwButtons = PSWIZB_NEXT;

    if ( -1 != GetPreviousPage() )
    {
        dwButtons |= PSWIZB_BACK;
    }

    ((CPropertySheet*) GetParent())->SetWizardButtons( dwButtons );

    m_ListCtrl.FillList( ST_LIVEVIRUS );

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
    auto    CString             strTitle;
    auto    CString             strMessage;
    auto    LPINFECTEDFILE      lpItem;
    auto    int                 nSelectedItem;

    nSelectedItem = m_ListCtrl.GetNextItem( -1, LVNI_ALL | LVIS_SELECTED );

    if ( -1 != nSelectedItem )
    {
        lpItem = ( LPINFECTEDFILE ) m_ListCtrl.GetItemData( nSelectedItem );

        if ( NULL != lpItem )
        {
            strTitle.LoadString( IDS_WIZWHYNOT_CAPTION );

            GetTextWhyStillInfected( lpItem, strMessage.GetBuffer( 6 * SYM_MAX_PATH ) );
            strMessage.ReleaseBuffer();

            MessageBox( strMessage, strTitle );
        }
    }
} // CRepWizStillInfected::OnAdviceButton()


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

VOID LOCAL GetTextWhyStillInfected (
    LPINFECTEDFILE  lpInfected,
    LPSTR           lpszSentence )
{
    auto    char    szWork      [ SYM_MAX_PATH * 2];
    auto    char    szFormat    [ SYM_MAX_PATH + 1 ];
    auto    char    szZipPath   [ SYM_MAX_PATH + 1 ];
    auto    char    szFileName  [ SYM_MAX_PATH + 1 ];

    if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
    {
        if (String_Get (IDS_ZIPFOLLOWHELP, szFormat, sizeof(szFormat)))
        {
            char szTemp [ SYM_MAX_PATH + 1 ];

            ParseZipFileName (lpInfected->szFullPath, szTemp, szFileName);
            NameShortenFileName ( szTemp, szZipPath, 80 );
            SPRINTF (szWork, szFormat, (LPSTR)szFileName, (LPSTR)szZipPath);
        }
    }
    else if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
         ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED
         ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
    {
        SPRINTF (szWork, String_Ptr (IDS_PHYS_DRIVE), lpInfected->szFullPath[0]);
    }
    else
    {
        NameShortenFileName( lpInfected->szFullPath, szWork, SYM_MAX_PATH );
    }

                                        // szWork is full of at most 2*SYM_MAX_PATH chars

                                        // Put szWork into a sentence:

                                        // Reuse szFormat for the format of the messagebox text:
    auto    char    szBuffer1   [ SYM_MAX_PATH + 1 ];
    auto    char    szBuffer2   [ SYM_MAX_PATH + 1 ];
    auto    char    szBuffer3   [ SYM_MAX_PATH + SYM_MAX_PATH + 1 ];
    auto    char    szConsult   [ SYM_MAX_PATH + 1 ];

    if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
    {                                   // Item is in an infected zip file:
        LoadString ( hInstance, IDS_WIZWHYNOT_ZIPSENTENCE, szFormat, sizeof (szFormat));
        LoadString ( hInstance, IDS_WIZWHYNOT_ZIPLINE1, szBuffer1, sizeof (szBuffer1));
        LoadString ( hInstance, IDS_WIZWHYNOT_ZIPLINE2, szBuffer2, sizeof (szBuffer2));
        LoadString ( hInstance, IDS_WIZWHYNOT_CONSULT, szConsult, sizeof (szConsult));
        SPRINTF ( lpszSentence, szFormat, szWork, szBuffer1, szBuffer2, szConsult );
        return;
    }

    if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
        ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
    {                                   // Item is in an infected MBR:
        LoadString ( hInstance, IDS_WIZWHYNOT_MBRSENTENCE, szFormat, sizeof (szFormat));
        LoadString ( hInstance, IDS_WIZWHYNOT_MBRLINE1, szBuffer1, sizeof (szBuffer1));
        LoadString ( hInstance, IDS_WIZWHYNOT_MBRLINE2, szBuffer2, sizeof (szBuffer2));
        LoadString ( hInstance, IDS_WIZWHYNOT_CONSULT, szConsult, sizeof (szConsult));
        SPRINTF ( lpszSentence, szFormat, szBuffer1, szWork, szBuffer2, szConsult );
        return;
    }

    if (lpInfected->wStatus == FILESTATUS_BOOTINFECTED
        ||  lpInfected->wStatus == FILESTATUS_BOOTNOTREPAIRED)
    {                                   // Item is in an infected boot record:
        LoadString ( hInstance, IDS_WIZWHYNOT_BOOTSENTENCE, szFormat, sizeof (szFormat));
        LoadString ( hInstance, IDS_WIZWHYNOT_BOOTLINE1, szBuffer1, sizeof (szBuffer1));
        LoadString ( hInstance, IDS_WIZWHYNOT_BOOTLINE2, szBuffer2, sizeof (szBuffer2));
        LoadString ( hInstance, IDS_WIZWHYNOT_CONSULT, szConsult, sizeof (szConsult));
        SPRINTF ( lpszSentence, szFormat, szBuffer1, szWork, szBuffer2, szConsult );
        return;
    }

                                        // Item is an infected file:
    LoadString ( hInstance, IDS_WIZWHYNOT_FILESENTENCE, szFormat, sizeof (szFormat));
    LoadString ( hInstance, IDS_WIZWHYNOT_FILELINE1, szBuffer1, sizeof (szBuffer1));
    LoadString ( hInstance, IDS_WIZWHYNOT_FILELINE2, szBuffer2, sizeof (szBuffer2));

    if (SYM_SYSTEM_WIN_95 == SystemGetWindowsType())
    {
        LoadString ( hInstance, IDS_WIZWHYNOT_FILELINE3_95, szBuffer3, sizeof (szBuffer3));
        LoadString ( hInstance, IDS_WIZWHYNOT_FILELINE4, szConsult, sizeof (szConsult));
    }
    else
    {
        LoadString ( hInstance, IDS_WIZWHYNOT_FILELINE3_NT, szBuffer3, sizeof (szBuffer3));
        STRCPY ( szConsult, "" );
    }

    SPRINTF ( lpszSentence, szFormat, szWork, szBuffer1, szBuffer2, szBuffer3, szConsult );

} // GetTextWhyStillInfected()

