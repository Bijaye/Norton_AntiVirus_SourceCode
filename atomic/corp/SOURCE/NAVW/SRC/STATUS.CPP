// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVW/VCS/status.cpv   1.16   29 Oct 1998 21:27:34   CEATON  $
//
// Description:
//      Routines for checking and updating main UI status info
//      on Auto-Protect, Quarantine, and Virus Definitions.
//
// Contains:
//      UpdateApStatus()
//      UpdateQuarantineStatus()
//      UpdateDefinitionsStatus()
//      DisplayApStatus()
//      DisplaySecondaryStatus()
//*************************************************************************
// $Log:   S:/NAVW/VCS/status.cpv  $
// 
//    Rev 1.16   29 Oct 1998 21:27:34   CEATON
// Added function to return value of sbNewDefs.
// 
// 
//    Rev 1.15   06 Oct 1998 17:53:32   tcashin
// Finished the status window changes. Wasn't updating correctly
// when new defs were installed.
// 
//    Rev 1.14   15 Sep 1998 19:01:44   CEATON
// Removed code bracketed by "#ifdef TESTING" and added call to GetInitialDefinitionsDate()
// when processing new virus definitions (to refresh current definitions date).
// 
//    Rev 1.13   15 Sep 1998 18:57:12   CEATON
// Modified functions that check for incoming virus definitions so they use an
// internally stored definition date instead of physically reading the informa-
// tion from disk.
// 
//    Rev 1.12   14 Jul 1998 13:48:36   jtaylor
// Changed OEM version to not display Quarantine file total even if non-zero.
//
//    Rev 1.11   10 Jul 1998 12:21:14   DBuches
// Now, cache status icons in a static array.
//
//    Rev 1.10   29 Jun 1998 14:01:30   DBuches
// Fixed problem with date formatting.
//
//    Rev 1.9   26 Jun 1998 16:10:20   jtaylor
// Removed Quarantine and AP from OEM versions.
//
//    Rev 1.8   20 Jun 1998 13:44:14   tcashin
// Added a defs-are-out-of-date icon. Made the secondary status window check
// for new defs, old defs, quarantine, then normal defs. Cleaned up some of the
// date processing.
//
//    Rev 1.7   15 Jun 1998 10:35:56   DBuches
// Disable Updating of quarantine status if we are launched with the /HMEM switch.
//
//    Rev 1.6   15 Jun 1998 10:18:24   DBuches
// Fixed problem in UpdateQuarantineZeroNewItems().
//
//    Rev 1.5   15 Jun 1998 10:16:38   DBuches
// Fixed problem in UpdateQuarantineZeroNewItems().
//
//    Rev 1.4   11 Jun 1998 12:53:22   tcashin
// Get rid of the functions to update the new quarantine items.
//
//    Rev 1.3   10 Jun 1998 22:32:30   tcashin
// Added routines to process some quarantine-related events.
//
//    Rev 1.2   28 May 1998 12:27:06   DBuches
// Changed to short date format for definition date display.
//
//    Rev 1.1   11 May 1998 20:16:48   DALLEE
// Change in UpdateApStatus() for Win95 - "Loaded" status is
// now determined by presence of both driver and W32 Agent/Service
// piece. Both must be running for "Loaded" to be TRUE.
//
//    Rev 1.0   28 Apr 1998 09:20:00   DALLEE
// Initial revision.
//*************************************************************************

#include <afxmt.h>
#include "platform.h"
#include "navw.h"

#include "drvcomm.h"
#include "apcomm.h"

#include <objbase.h>
#include "quar32.h"
#include "iquaran.h"

#include "defutils.h"
#include "navappids.h"

#include "navwstb.h"
#include "navwres.h"
#include "navcmdid.h"

#include "navapcom.h"
#include "cmbstr.h"
#include "navwstb.h"

#include "n32behav.h" // Personality DLL


//*************************************************************************
// External Data
//*************************************************************************

extern HINSTANCE    ghInst;

//*************************************************************************
// Local Data
//*************************************************************************

static HICON        s_aStatusIcons[ 5 ];
static WORD         wYear      = 0;
static WORD         wMonth     = 0;
static WORD         wDay       = 0;
static DWORD        dwRevision = 0;
static BOOL         sbNewDefs  = FALSE;

#define STATUS_IMAGE( x ) (s_aStatusIcons[x - IDI_STATUS_AP_ENABLED])

//*************************************************************************
// CODE
//*************************************************************************

//*************************************************************************
// LoadStatusIcons()
//
// void LoadStatusIcons()
//
// This routine loads all of the status icons into a static array
//
//*************************************************************************
// 07/10/1998 DBUCHES, created - header added.
//*************************************************************************
void LoadStatusIcons()
{
    //
    // Initialize static array of icons.
    //
    for( int i = 0; i < 5; i++ )
        {
        s_aStatusIcons[ i ] = ( HICON ) LoadImage( ghInst,
                                      MAKEINTRESOURCE( i + IDI_STATUS_AP_ENABLED ),
                                      IMAGE_ICON,
                                      0, 0,
                                      LR_DEFAULTCOLOR );
        }
}

//*************************************************************************
// UpdateApStatus()
//
// BOOL UpdateApStatus(
//      LPAPSTATUS lpApStatus )         [in/out] Last status of AP.
//                                          Receives current status.
//
// This routine fills the structure provided with the current Auto-Protect
// status and returns whether or not the information changed.
//
// Returns:
//      TRUE if status has changed. Otherwise, FALSE.
//*************************************************************************
// 04/27/1998 DALLEE, created - header added.
//*************************************************************************

BOOL UpdateApStatus( LPAPSTATUS lpApStatus )
{
    auto    DriverComm  commNAVAP;
    auto    DWORD       dwStatus;
    auto    DWORD       dwBytesReturned;
    auto    BOOL        bEnabled;
    auto    BOOL        bLoaded;

    auto    BOOL        bChanged;

    bEnabled = FALSE;

    if ( commNAVAP.Open( NAVAP_COMMUNICATION_NAME ) )
    {
        if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        {
            // On NT, if the driver is there, we're loaded regardless of
            // whether or not the Agent UI is around.

            bLoaded = TRUE;
        }
        else
        {
            // On 95, loaded status is determined by presence of
            // both driver and the combination W32 agent/service.

            bLoaded = ( NULL != FindWindow( NAVAP_95_WND_CLASS, NULL ) );
        }

        if ( ( commNAVAP.Send( NAVAP_COMM_GETENABLEDDISABLED,
                               NULL,
                               0,
                               &dwStatus,
                               sizeof( dwStatus ),
                               &dwBytesReturned ) ) &&
             ( sizeof( dwStatus ) == dwBytesReturned ) )
        {
            bEnabled = dwStatus ? TRUE : FALSE;
        }
    }
    else
    {
        bLoaded = FALSE;
    }

    bChanged = ( ( lpApStatus->bEnabled != bEnabled ) ||
                 ( lpApStatus->bLoaded  != bLoaded ) );

    lpApStatus->bEnabled = bEnabled;
    lpApStatus->bLoaded  = bLoaded;

    return ( bChanged );
} // UpdateApStatus()


//*************************************************************************
// UpdateQuarantineStatus()
//
// BOOL UpdateQuarantineStatus(
//      LPQUARANTINESTATUS lpQuarantineStatus ) [in/out] Last status of Quarantine.
//                                                  Receives current status.
//
// This routine fills the structure provided with the current Quarantine
// status and returns whether or not the information changed.
//
// Returns:
//      TRUE if status has changed. Otherwise, FALSE.
//*************************************************************************
// 04/27/1998 DALLEE, created - header added.
//*************************************************************************

BOOL UpdateQuarantineStatus( LPQUARANTINESTATUS lpQuarantineStatus )
{
    auto    IQuarantineDLL          *pIQuarantine;
    auto    IEnumQuarantineItems    *pIEnum;
    auto    IQuarantineItem         *pIItem;
    auto    CCriticalSection        *lpCS;

    auto    DWORD   dwStatus;
    auto    ULONG   uFetched;
    auto    UINT    uTotalItems;
    auto    BOOL    bChanged;
    auto    CFGTEXTREC rSwitch;

    if( !NAV_IS_QUARANTINE() )
        {
        return FALSE;
        }

    //
    // Do nothing if /HMEM switch is used.
    //
    rSwitch.lpSwitch = String_Ptr(IDS_MEMHANDLE);
    rSwitch.wFlags   = CFG_SW_MINIMUM;
    if ( ConfigSwitchSet( &rSwitch, NULL) )
        return FALSE;

    uTotalItems = 0;

    if ( SUCCEEDED ( CoInitialize( NULL ) ) )
    {
        if ( SUCCEEDED( CoCreateInstance( CLSID_QuarantineDLL,
                                          NULL,
                                          CLSCTX_INPROC_SERVER,
                                          IID_QuarantineDLL,
                                          ( LPVOID * ) &pIQuarantine ) ) )
        {
            if ( SUCCEEDED( pIQuarantine->Initialize() ) )
            {
                if ( SUCCEEDED( pIQuarantine->Enum( &pIEnum ) ) )
                {
                    while ( S_OK == pIEnum->Next( 1, &pIItem, &uFetched ) )
                    {
                        if( SUCCEEDED( pIItem->GetFileStatus( &dwStatus ) ) )
                        {
                            if ( QFILE_STATUS_QUARANTINED == dwStatus )
                            {
                                uTotalItems++;
                            }
                        }

                        pIItem->Release();
                    }

                    pIEnum->Release();
                }
            }

            pIQuarantine->Release();
        }

        CoUninitialize();
    }

    bChanged = ( lpQuarantineStatus->uTotalItems != uTotalItems );

    lpCS = (CCriticalSection *)lpQuarantineStatus->pQuarMonCS;

    if( lpCS )
        {
        lpCS->Lock();
        lpQuarantineStatus->uTotalItems = uTotalItems;
        lpCS->Unlock();
        }
    else
        lpQuarantineStatus->uTotalItems = uTotalItems;


    return ( bChanged );
} // UpdateQuarantineStatus()


//*************************************************************************
// UpdateQuarantineZeroNewItems()
//
// BOOL UpdateQuarantineZeroNewItems(
//      LPQUARANTINESTATUS lpQuarantineStatus ) [in/out] Last status of Quarantine.
//                                                  Receives current status.
//
// This routine makes the number of quarantine items zero.
//
// Returns:
//      TRUE if the count has changed. Otherwise, FALSE.
//*************************************************************************
// 06/09/1998 TCashin, created - header added.
//*************************************************************************

BOOL UpdateQuarantineZeroNewItems( LPQUARANTINESTATUS lpQuarantineStatus )
{
    auto    CCriticalSection    *lpCS;

    if (lpQuarantineStatus)
    {
        lpCS = (CCriticalSection *)lpQuarantineStatus->pQuarMonCS;
        if( lpCS )
            {
            lpCS->Lock();
            lpQuarantineStatus->uNewItems = 0;
            lpCS->Unlock();
            }
        else
            lpQuarantineStatus->uNewItems = 0;

        return TRUE;
    }
    else
        return FALSE;

} // UpdateQuarantineZeroNewItems()


//*************************************************************************
// UpdateDefinitionsStatus()
//
// BOOL UpdateDefinitionsStatus(
//      LPDEFINITIONSSTATUS lpDefinitionsStatus ) [in/out] Last status of Definitions.
//                                                  Receives current status.
//
// This routine fills the structure provided with the current Defintions
// status and returns whether or not the information changed.
//
// Returns:
//      TRUE if status has changed. Otherwise, FALSE.
//*************************************************************************
// 04/27/1998 DALLEE, created - header added.
//*************************************************************************

BOOL UpdateDefinitionsStatus( LPDEFINITIONSSTATUS lpDefinitionsStatus )
{
    auto    BOOL        bChanged;

    bChanged = ( ( lpDefinitionsStatus->wYear      != wYear ) ||
                 ( lpDefinitionsStatus->wMonth     != wMonth ) ||
                 ( lpDefinitionsStatus->wDay       != wDay ) ||
                 ( lpDefinitionsStatus->dwRevision != dwRevision ) );

    lpDefinitionsStatus->wYear      = wYear;
    lpDefinitionsStatus->wMonth     = wMonth;
    lpDefinitionsStatus->wDay       = wDay;
    lpDefinitionsStatus->dwRevision = dwRevision;

    return ( bChanged );
} // UpdateDefinitionsStatus()


//*************************************************************************
// DisplayApStatus()
//
// VOID DisplayApStatus(
//      HWND          hWnd,             [in] NAVW main window handle.
//      LPNAVMAIN     lpNavMain )       [in] Contains current AP status.
//
// This routine displays the current status of AP as reported by lpNavMain
// in the NAVW main dialog.
//
// Returns:
//      Nothing.
//*************************************************************************
// 04/27/1998 DALLEE, created - header added.
//*************************************************************************

VOID DisplayApStatus( HWND          hWnd,
                      LPNAVMAIN     lpNavMain )
{
    auto    TCHAR   szStatus [ 200 ];
    auto    TCHAR   szButton [ 100 ];

    auto    UINT    uIconID;
    auto    UINT    uStatusID;
    auto    UINT    uButtonID;

    *szStatus = EOS;
    *szButton = EOS;

    // Check to see if we are the scanner only version.  If so, AP status is Dissabled.
    if( !NAV_IS_SCANONLY() )
    {
        if ( lpNavMain->rApStatus.bEnabled )
        {
            uIconID     = IDI_STATUS_AP_ENABLED;
            uStatusID   = IDS_STATUS_AP_ENABLED;
            uButtonID   = IDS_STATUS_AP_BUTTON_DISABLE;
        }
        else
        {
            uIconID     = IDI_STATUS_AP_DISABLED;
            uStatusID   = IDS_STATUS_AP_DISABLED;
            uButtonID   = IDS_STATUS_AP_BUTTON_ENABLE;
        }
    }
    else
    {
        uIconID     = IDI_STATUS_AP_DISABLED;
        uStatusID   = IDS_STATUS_AP_UNAVAILABLE;

        // Hide the button for enabling AP.
        ShowWindow( GetDlgItem( hWnd, IDC_STATUS_AP_BUTTON ), SW_HIDE );
    }


    LoadString( ghInst, uStatusID, szStatus, sizeof( szStatus ) / sizeof( TCHAR ) );
    LoadString( ghInst, uButtonID, szButton, sizeof( szButton ) / sizeof( TCHAR ) );

    Static_SetIcon( GetDlgItem( hWnd, IDC_STATUS_AP_ICON ), STATUS_IMAGE( uIconID ) );

    Static_SetText( GetDlgItem( hWnd, IDC_STATUS_AP_TEXT ), szStatus );
    Button_SetText( GetDlgItem( hWnd, IDC_STATUS_AP_BUTTON ), szButton );
} // DisplayApStatus()


//*************************************************************************
// DisplaySecondaryStatus()
//
// VOID DisplaySecondaryStatus(
//      HWND       hWnd,                [in] NAVW main window handle.
//      LPNAVMAIN  lpNavMain )          [in] Current Quarantine and Defs status.
//
// This routine displays the current Quarantine status or Virus Definitions
// status based on the presence of unhandled items in Quarantine.
//
// Returns:
//      Nothing.
//*************************************************************************
// 04/27/1998 DALLEE, created - header added.
//*************************************************************************

VOID DisplaySecondaryStatus( HWND       hWnd,
                             LPNAVMAIN  lpNavMain )
{
    auto    UINT    uIconID;
    auto    UINT    uStatusID;

    auto    TCHAR   szFormat [ 200 ];
    auto    TCHAR   szStatus [ 200 ];
    auto    TCHAR   szDate[32];
    auto    SYSTEMTIME  st;


    *szFormat = EOS;
    *szStatus = EOS;

    st.wYear  = lpNavMain->rDefinitionsStatus.wYear;
    st.wMonth = lpNavMain->rDefinitionsStatus.wMonth;
    st.wDay   = lpNavMain->rDefinitionsStatus.wDay;


    GetDateFormat( LOCALE_SYSTEM_DEFAULT,
                   DATE_SHORTDATE,
                   &st,
                   NULL,
                   szDate,
                   32 );

    if ( sbNewDefs )
    {
        // Display "Hey! We have new defs!"

        uIconID = IDI_STATUS_DEFINITIONS;

        LoadString( ghInst, IDS_STATUS_NEW_DEFS, szStatus, sizeof( szStatus ) / sizeof( TCHAR ) );
    }
    else if ( AreDefinitionsOld() )
    {
        // Display defintions-are-old status.

        uIconID = IDI_STATUS_DEFSOLD;

        LoadString( ghInst, IDS_STATUS_DEFINITIONS, szFormat, sizeof( szFormat ) / sizeof( TCHAR ) );

        _stprintf( szStatus, szFormat, szDate );
    }
    // Do not display Quarantine status if there are no items, or Quarantine is not
    // enabled in this install.
    else if ( lpNavMain->rQuarantineStatus.uTotalItems && NAV_IS_QUARANTINE() )
    {
        // Display Quarantine status.

        uIconID = IDI_STATUS_QUARANTINE;

        uStatusID = lpNavMain->rQuarantineStatus.uTotalItems > 1 ?
                        IDS_STATUS_QUARANTINE_MULTIPLE : IDS_STATUS_QUARANTINE_ONE;

        LoadString( ghInst, uStatusID, szFormat, sizeof( szFormat ) / sizeof( TCHAR ) );

        _stprintf( szStatus, szFormat, lpNavMain->rQuarantineStatus.uTotalItems );
    }
    else
    {
        // Display defintions status.

        uIconID = IDI_STATUS_DEFINITIONS;

        LoadString( ghInst, IDS_STATUS_DEFINITIONS, szFormat, sizeof( szFormat ) / sizeof( TCHAR ) );

        _stprintf( szStatus, szFormat, szDate );
    }

    Static_SetIcon( GetDlgItem( hWnd, IDC_STATUS_2_ICON ), STATUS_IMAGE( uIconID ) );
    Static_SetText( GetDlgItem( hWnd, IDC_STATUS_2_TEXT ), szStatus );


} // DisplaySecondaryStatus()

//*************************************************************************
// UpdateCheckDefinitionAreOld()
//
// BOOL UpdateCheckDefinitionAreOld( VOID )
//
// This routine determines if the definitions are greater than 30 days old.
//
// Returns:
//      TRUE - If the defs are old.
//
//*************************************************************************
// 06/19/1998 TCASHIN, created - header added.
//*************************************************************************
BOOL UpdateCheckDefinitionAreOld ( VOID )
{
    auto    CDefUtils   defUtils;
    auto    LPCTSTR     lpszAppId;
    auto    CTime *pDefDate;
    auto    CTime       Today;
    auto    CTimeSpan   DefAge;
    auto    int         days;
    auto    BOOL        bResult    = FALSE;
    auto    WORD        wYear      = 0;
    auto    WORD        wMonth     = 0;
    auto    WORD        wDay       = 0;
    auto    DWORD       dwRevision = 0;


    lpszAppId = ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() ?
                  g_szNavAppIdNavwNt : g_szNavAppIdNavw95 );

    if ( defUtils.InitWindowsApp( lpszAppId ) )
        {
        defUtils.GetCurrentDefsDate( &wYear, &wMonth, &wDay, &dwRevision );

        pDefDate = new CTime( wYear, wMonth, wDay, 0, 0, 0);
        Today  =  CTime::GetCurrentTime();

        DefAge = Today - *pDefDate;

        days = DefAge.GetDays();

        if ( abs(days) > 30 )
            {
            bResult = TRUE;
            }
        else
            {
            bResult = FALSE;
            }

        delete pDefDate;

        }
    return ( bResult );
}


//*************************************************************************
// UpdateCheckDefinitionAreNewest()
//
// BOOL UpdateCheckDefinitionAreNewest( VOID )
//
// This routine determines if there are newer definitions on the machine.
//
// Returns:
//      TRUE - If there are newer defs.
//
//*************************************************************************
// 06/19/1998 TCASHIN, created - header added.
//*************************************************************************
BOOL UpdateCheckDefinitionAreNewest ( VOID )
{
    auto    CDefUtils   defUtils;
    auto    LPCTSTR     lpszAppId;
    auto    BOOL        bResult = FALSE;

    lpszAppId = ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() ?
                  g_szNavAppIdNavwNt : g_szNavAppIdNavw95 );

    if ( defUtils.InitWindowsApp( lpszAppId ) )
        {
        if ( defUtils.DefsAreNewest() )
            {
            bResult = TRUE;
            }
        else
            {
            bResult = FALSE;
            }
        }
    return ( bResult );
}

BOOL GetInitialDefinitionsDate ( VOID )
{
    auto    CDefUtils   defUtils;
    auto    LPCTSTR     lpszAppId;


    lpszAppId = ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() ?
                  g_szNavAppIdNavwNt : g_szNavAppIdNavw95 );

    if ( defUtils.InitWindowsApp( lpszAppId ) )
        {
        defUtils.GetCurrentDefsDate( &wYear, &wMonth, &wDay, &dwRevision );

        return( TRUE );
        }

    return( FALSE );
}

BOOL ProcessNewDefinitions ( HWND hWnd )
{
    auto    CDefUtils   defUtils;
    auto    LPCTSTR     lpszAppId;
    auto    BOOL        bResult = FALSE;

    lpszAppId = ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() ?
                  g_szNavAppIdNavwNt : g_szNavAppIdNavw95 );

    if ( defUtils.InitWindowsApp( lpszAppId ) )
        {
        if ( !defUtils.DefsAreNewest() )
            {
            bResult   = TRUE;
            sbNewDefs = TRUE;
            }
        else
            {
            bResult = FALSE;
            }
        }
    return ( bResult );
}

BOOL AreDefinitionsOld ( VOID )
{
    auto    CTime      *pDefDate;
    auto    CTime       Today;
    auto    CTimeSpan   DefAge;
    auto    int         days;
    auto    BOOL        bResult    = FALSE;


    pDefDate = new CTime( wYear, wMonth, wDay, 0, 0, 0);

    Today  =  CTime::GetCurrentTime();

    DefAge = Today - *pDefDate;

    days = DefAge.GetDays();

    delete pDefDate;

    if ( abs(days) > 30 )
        {
        bResult = TRUE;
        }
    else
        {
        bResult = FALSE;
        }

    return ( bResult );
}

BOOL HasNewDefsArrived()
{
    return( sbNewDefs );
}

