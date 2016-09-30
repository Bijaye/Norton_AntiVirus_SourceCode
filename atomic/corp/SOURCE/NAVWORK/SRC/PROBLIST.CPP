// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// ProbList.cpp 
//
// $Header:   S:/NAVWORK/VCS/ProbList.cpv   1.0   09 Mar 1998 23:45:42   DALLEE  $
//
// Description:
//      Implementation of CProblemListCtrl. This is an auto-sorting 
//      list view control used in the Repair Wizard for displaying
//      infected items.
//
// Contains:
//      CProblemListCtrl::CProblemListCtrl()
//      CProblemListCtrl::~CProblemListCtrl()
//      CProblemListCtrl::AdjustColumnWidths()
//      CProblemListCtrl::CompareFunc()
//      CProblemListCtrl::FillList()
//      CProblemListCtrl::Init()
//      CProblemListCtrl::OnGetdispinfo()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/ProbList.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:42   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "navwork.h"
#include "ProbList.h"
#include "nunavdlg.h"

#include "nunavstb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProblemListCtrl

//*************************************************************************
// CProblemListCtrl::CProblemListCtrl()
// CProblemListCtrl::~CProblemListCtrl()
//
// Returns: 
//      Nothing
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

CProblemListCtrl::CProblemListCtrl()
{
    m_lpItemBlockList = NULL;
} // CProblemListCtrl::CProblemListCtrl()

CProblemListCtrl::~CProblemListCtrl()
{
} // CProblemListCtrl::~CProblemListCtrl()


//*************************************************************************
// CProblemListCtrl::Init()
//
// BOOL CProblemListCtrl::Init(
//      LPINFARR lpItemBlockList )
//
// Initialize the problem list.
// Creates and labels columns and stores info on problem items.
// The pointer to problem items must remain valid for the life of the 
// control - the data is used directly, not copied.
//
// Returns: 
//      TRUE on success, FALSE otherwise.
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CProblemListCtrl::Init( LPINFARR lpItemBlockList )
{
    auto    CString     s;

    // Save list of infected items.

    m_lpItemBlockList = lpItemBlockList;

    // Add problem item data.

    SetRedraw( FALSE );

    // Set up columns.

    s.LoadString( IDS_RW_NAME );
    InsertColumn( COLUMN_NAME, s, LVCFMT_LEFT, -1 );
    
    s.LoadString( IDS_RW_VIRUS_NAME );
    InsertColumn( COLUMN_VIRUS, s, LVCFMT_LEFT, -1 );

    AdjustColumnWidths();

    SetRedraw( TRUE );

    return ( TRUE );
} // CProblemListCtrl::Init()


//*************************************************************************
// CProblemListCtrl::AdjustColumnWidths()
//
// BOOL CProblemListCtrl::AdjustColumnWidths( )
//
// Adjusts the column widths. Columns will be adjusted to a percentage
// width of the list control minus scroll bar, if present.
//
// Returns: 
//      TRUE on success, FALSE otherwise.
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CProblemListCtrl::AdjustColumnWidths()
{
    #define NAME_WIDTH_PERCENT  65

    auto    CRect       rectClient;
    auto    int         nNameWidth;
    auto    int         nVirusWidth;

    GetClientRect( rectClient );

    nNameWidth   = ( rectClient.Width() * NAME_WIDTH_PERCENT ) / 100;
    nVirusWidth  = rectClient.Width() - nNameWidth;

    SetColumnWidth( COLUMN_NAME, nNameWidth );
    SetColumnWidth( COLUMN_VIRUS, nVirusWidth );

    return ( TRUE );
} // CProblemListCtrl::AdjustColumnWidths()


//*************************************************************************
// CProblemListCtrl::CompareFunc()
//
// int CProblemListCtrl::CompareFunc(
//      LPARAM lParam1,                 [in] list control item data for first item.
//      LPARAM lParam2 )                [in] list control item data for second item.
//
// Comparison function for sorting the list based on current sort column
// and direction.
// This is an override of CSortedListCtrl::CompareFunc().
// Call SortItems() w/ no parameters to perform a sort using this comparison
// function.
//
// Returns: 
//      <0 if item one should precede item two.
//      0 if items are equivalent
//      >0 if item one should follow item two.
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

int CProblemListCtrl::CompareFunc( LPARAM lParam1, LPARAM lParam2 )
{
    auto    LPINFECTEDFILE  pItem1;
    auto    LPINFECTEDFILE  pItem2;

    auto    int     nResult;

    nResult = 0;

    pItem1 = reinterpret_cast<LPINFECTEDFILE>( lParam1 );
    pItem2 = reinterpret_cast<LPINFECTEDFILE>( lParam2 );

    if ( pItem1 && pItem2 )
    {
        switch ( m_nSortColumn )
        {
        case COLUMN_NAME:
            nResult = _tcsicmp( pItem1->szFullPath, pItem2->szFullPath );
            break;

        case COLUMN_VIRUS:
            nResult = _tcsicmp( pItem1->Notes.lpVirName, pItem2->Notes.lpVirName );
            break;

        default:
            // Why is there some new column we don't know about?

            SYM_ASSERT( FALSE );
            break;
        }
    }

    if ( FALSE == m_bSortAscending )
        nResult = -nResult;

    return ( nResult );
} // CProblemListCtrl::CompareFunc()


//*************************************************************************
// CProblemListCtrl::FillList()
//
// BOOL CProblemListCtrl::FillList(
//      DWORD dwItemTypes )             [in] Flags defining problems to display.
//
// This routine will fill the problem list with all items matching the
// status types passed in dwItemTypes. See IsMatchingStatusType() for details
// on flags. Flags are ST_??? defined in NONAV.H.
//
// Returns: 
//      TRUE on success, FALSE otherwise.
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CProblemListCtrl::FillList( DWORD dwItemTypes )
{
    auto    LPINFECTEDFILE  lpItem;
    auto    LPINFARR        lpBlockList;

    auto    LV_ITEM         listItem;
    auto    int             nIndex;
    auto    UINT            i;

    auto    BOOL            bSuccess;

    bSuccess = TRUE;

    // Loop through all blocks of problem items and add each item.

    SetRedraw( FALSE );

    DeleteAllItems();

    for ( lpBlockList = m_lpItemBlockList;
          lpBlockList;
          lpBlockList = ( LPINFARR ) lpBlockList->lpNext )
    {
        lpItem = ( LPINFECTEDFILE ) MemLock( lpBlockList->hInf );

        if ( lpItem )
        {
            for ( i = 0; i < lpBlockList->uNumUsed; i++, lpItem++ )
            {
                if ( IsMatchingStatusType( lpItem->wStatus, dwItemTypes ) )
                {
                    listItem.mask       = LVIF_TEXT | LVIF_PARAM;
                    listItem.iItem      = GetItemCount();
                    listItem.iSubItem   = COLUMN_NAME;
                    listItem.stateMask  = 0;
                    listItem.pszText    = LPSTR_TEXTCALLBACK;
                    listItem.lParam     = reinterpret_cast<LPARAM>( lpItem );

                    nIndex = InsertItem( &listItem );
                }
            }

            MemUnlock( lpBlockList->hInf );
        }
        else
        {
            // Unable to lock this block's entries, just exit.

            bSuccess = FALSE;
            break;
        }
    }

    m_bSortAscending = TRUE;
    m_nSortColumn    = COLUMN_NAME;

    SortItems();

    if ( i )
    {
        SetItemState( 0 , LVIS_SELECTED, LVIS_SELECTED );
        Update( 0 );
    }

    SetRedraw( TRUE );

    // Reset columns widths in case we added enough items to create
    // a vertical scroll bar.

    AdjustColumnWidths();

    return ( bSuccess );
} // CProblemListCtrl::FillList()


BEGIN_MESSAGE_MAP(CProblemListCtrl, CSortedListCtrl)
	//{{AFX_MSG_MAP(CProblemListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProblemListCtrl message handlers


//*************************************************************************
// CProblemListCtrl::OnGetdispinfo()
//
// void CProblemListCtrl::OnGetdispinfo(
//      NMHDR*   pNMHDR                 [in] Actually LV_DISPINFO*
//      LRESULT* pResult )              [out] 
//
// Gets text to be displayed in the problem items list control.
//
// Returns: 
//      Nothing.
//      *pResult set to 0 to indicate success.
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

void CProblemListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

    auto    LPINFECTEDFILE  pItem;

    pItem = reinterpret_cast<LPINFECTEDFILE>( pDispInfo->item.lParam );

    if ( NULL != pItem )
    {
        if ( LVIF_TEXT & pDispInfo->item.mask )
        {
            switch ( pDispInfo->item.iSubItem )
            {
            case COLUMN_NAME:
                if ( FALSE == GetDisplayName( pDispInfo ) )
                {
                    pDispInfo->item.pszText = pItem->szFullPath;
                }

                break;

            case COLUMN_VIRUS:
                pDispInfo->item.pszText = pItem->Notes.lpVirName;
                break;

            default:
                // Some new unknown column. We should know about it.

                SYM_ASSERT( FALSE );
                break;
            }
        }
    }

	*pResult = 0;
} // CProblemListCtrl::OnGetdispinfo()


//*************************************************************************
// CProblemListCtrl::GetDisplayName()
//
// BOOL CProblemListCtrl::GetDisplayName(
//      LV_DISPINFO *pDispInfo )
// 
// This function constructs and copies the Name column text to be displayed
// the list view display info struct passed.
//
// For internal use by OnGetdispinfo().
//
// Returns: 
//      TRUE on success, FALSE otherwise.
//*************************************************************************
// 03/06/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CProblemListCtrl::GetDisplayName( LV_DISPINFO *pDispInfo )
{
    auto    LPINFECTEDFILE  pItem;
    auto    CString         strText;
    auto    BOOL            bSuccess;

    bSuccess = TRUE;

    pItem = reinterpret_cast<LPINFECTEDFILE>( pDispInfo->item.lParam );

    if ( IsMatchingStatusType( pItem->wStatus, ST_ZIP ) )
    {
        // Format to "<<file.ext>> in the compressed file <<path\archive.ext>>."

        auto    TCHAR   szArchiveName [ SYM_MAX_PATH ];
        auto    TCHAR   szMemberName [ SYM_MAX_PATH ];

        if ( pDispInfo->item.cchTextMax )
        {
            ParseZipFileName( pItem->szFullPath, szArchiveName, szMemberName );
            strText.Format( IDS_ZIPFOLLOWHELP, szMemberName, szArchiveName );

            _tcsncpy( pDispInfo->item.pszText, strText, pDispInfo->item.cchTextMax );
            pDispInfo->item.pszText[ pDispInfo->item.cchTextMax - 1 ] = EOS;
        }
        else
        {
            bSuccess = FALSE;
        }
    }
    else if ( IsMatchingStatusType( pItem->wStatus, ST_MBR ) )
    {
        // Format to "Harddrive #%d"

        if ( pDispInfo->item.cchTextMax )
        {
            strText.Format( IDS_PHYS_DRIVE, static_cast<int>( pItem->szFullPath[0] ) );

            _tcsncpy( pDispInfo->item.pszText, strText, pDispInfo->item.cchTextMax );
            pDispInfo->item.pszText[ pDispInfo->item.cchTextMax - 1 ] = EOS;
        }
        else
        {
            bSuccess = FALSE;
        }
    }
    else
    {
        // Other types (boot, file), simply use the name as is.

        pDispInfo->item.pszText = pItem->szFullPath;
    }

    return ( TRUE );
} // CProblemListCtrl::GetDisplayName()

#if 0
    if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
        {
        if (String_Get (IDS_ZIPFOLLOWHELP, szFormat, sizeof(szFormat)))
            {
            char szTemp [ SYM_MAX_PATH + 1 ];

            ParseZipFileName (lpInfected->szFullPath, szTemp, szFileName);
                                        // Shorten the text going into the static to
                                        // parameter iStaticMaxChars :
            NameShortenFileName ( szTemp, szZipPath, iStaticMaxChars );
            SPRINTF (szWork, szFormat, (LPSTR)szFileName, (LPSTR)szZipPath);
            Static_SetText (GetDlgItem(hDlg, ID_SR_RAP), szWork);
            }
        }
    else
        if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
        ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED
        ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
            {
            SPRINTF (szWork, String_Ptr (IDS_PHYS_DRIVE), lpInfected->szFullPath[0]);
            Static_SetText (GetDlgItem(hDlg, ID_SR_RAP), szWork);
            }
        else
            {
            NameDisplayShortenedFileName ( GetDlgItem(hDlg, ID_SR_RAP),
                                           lpInfected->szFullPath,
                                           2 );
            }

    Static_SetText (GetDlgItem(hDlg, ID_SR_RAP2),
                    FileStatus[lpInfected->wStatus].uFollowHelp ?
                     String_Ptr(FileStatus[lpInfected->wStatus].uFollowHelp) :
                     "" );
#endif
