// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_inoc.cpv   1.3   22 May 1997 16:48:36   DALLEE  $
//
// Description:
//      Routines to check for and act on file inoculation (& registration)
//      info changes.
//
// Contains:
//      FileInoculated()
//      FileRegistered()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_inoc.cpv  $
// 
//    Rev 1.3   22 May 1997 16:48:36   DALLEE
// Modified exclude check to pass LFN and SFN to new faster exclude check.
// Also cleaned up code -- broke into separate functions for new and 
// inoc change.
// 
//    Rev 1.2   13 May 1997 13:54:28   TIVANOV
// new case INOC_QUARANTINE to support quarantine a file if inoc changed
// 
//    Rev 1.1   25 Feb 1997 12:45:32   DALLEE
// Port from HYDRA.15:
//        Rev 1.14   12 Dec 1996 12:00:06   GWESTER
//     Removed password protection for File Inoc.
// 
//        Rev 1.13   09 Oct 1996 12:03:48   CCOTRIL
//     Re-enable network alerting for W32.
// 
//    Rev 1.0   06 Feb 1997 21:02:46   RFULLER
// Initial revision
// 
//    Rev 1.4   12 Sep 1996 16:19:30   JBRENNA
// Pass the OEM version of the filename to the ExcludeIs() call in
// FileInoculated().
// 
//    Rev 1.3   24 Jun 1996 21:25:38   RHAYES
// Add.'l OEM<->Ansi mod.'s (new and from Luigi).
// 
//    Rev 1.2   21 May 1996 15:05:16   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
// 
//    Rev 1.1   14 Feb 1996 17:00:24   JWORDEN
// Process internal file names in OEM mode to allow scan, and repair, and
// innoculation of file names containing certain non-ANSI translatable
// characters.
// 
//    Rev 1.0   30 Jan 1996 15:55:52   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:50:56   BARRY
// Initial revision.
// 
//    Rev 1.7   25 Oct 1995 00:58:50   DALLEE
// Move InocPasswordVerify() here from N_SYSINT.CPP --
// N_SYSINT.CPP is going to be commented out of NAVWNT (Gargoyle) builds.
// 
//    Rev 1.6   09 May 1995 10:30:54   KEITH
// Correct some nouns
// 
//    Rev 1.5   20 Dec 1994 14:46:16   MFALLEN
// Sami did fixes on CERTLIBN
// 
//    Rev 1.4   29 Nov 1994 13:37:04   MFALLEN
// Compiles for W16
// 
//    Rev 1.3   21 Nov 1994 13:07:08   MFALLEN
// Instead of commenting out all the functions that does not work, they have
// been stubbed out instead.
// 
//    Rev 1.2   08 Nov 1994 15:42:04   MFALLEN
// Compiles fine under W32.
// 
// 
//    Rev 1.41   11 Mar 1994 00:29:42   DALLEE
// Added Alert on registration changes.
// 
//    Rev 1.40   09 Mar 1994 16:26:06   DALLEE
// Fixed Register Automatically for unregistered files.
// 
//    Rev 1.39   04 Mar 1994 17:51:04   DALLEE
// Check vs. exclusions before doing file registration.
// 
//    Rev 1.38   01 Mar 1994 11:48:36   DALLEE
// Support prompt for registration items.
// Use new Registration strings not inoc. strings.
// 
//    Rev 1.37   24 Feb 1994 14:27:24   DALLEE
// File registration support.  Logs event as inoculation - fix next rev.
//************************************************************************

#include "platform.h"
#include "file.h"
#include "navutil.h"
#include "options.h"
#include "security.h"
#include "nunavdlg.h"
#include "nunavstb.h"

#include "nonav.h"
#include "inoc.h"

#include "netalert.h"


//***************************************************************************
// Externs

extern  HINSTANCE   hInstance;


//***************************************************************************
// Local prototypes

BOOL LOCAL HandleNotInoculated ( LPNONAV lpNav );
BOOL LOCAL HandleInoculatedChanged ( LPNONAV lpNav );


/*@API:**********************************************************************
@Declaration:
BOOL SYM_EXPORT WINAPI InocPasswordVerify ( HWND hDlg, LPNONAV lpNav )

@Description:
This routine checks whether the user has the right to inoculate files.
It calls PasswordCheck() to see if inoculation is password protected,
and if so asks for the password.

@Parameters:
$hDlg$      Parent window.
$lpNav$     Current scan info and options.

@Returns:
TRUE    if the user is allowed to inoculate files.
FALSE   otherwise.

@See:
@Include:
@Compatibility: WIN
****************************************************************************/
// 10/24/95 DALLEE, moved from n_sysint.cpp (n_sysint.obj to be removed from NT)
//              and added function header.

BOOL SYM_EXPORT WINAPI InocPasswordVerify (
    HWND    hDlg, 
    LPNONAV lpNav )
{
    auto    BOOL        bStatus = TRUE;
    auto    CHECKSTRUCT PwCheck;

    MEMSET( &PwCheck, EOS, sizeof( PwCheck ) );
    PwCheck.hWnd         = hDlg;
    PwCheck.lpNavOptions = lpNav->lpScan->lpNavOptions;
    PwCheck.uID          = PASSWORD_INOCFILES;

                                        // Let bStatus stay TRUE. It is always OK
                                        // for the user to inoculate.
    //if ( PASSWORD_ASK == AskPassword( &PwCheck ) )
    //    bStatus = PasswordCheck( &PwCheck, FALSE, FALSE );

    return ( bStatus );
} // InocPasswordVerify()


//************************************************************************
// FileInoculated()
//
// This routine checks whether a file has been inoculated and responds
// according to the settings in NAVOPTIONS.
// (ANSI/OEM mess: Internal work is ANSI)
//
// Parameters:
//      LPNONAV lpNav       Current scan info and current file.
//
// Returns:
//      TRUE                File is inoculated.
//      FALSE               File was not inoculated, or had changed.
//************************************************************************
//  ??/??/?? Martin?
//************************************************************************

BOOL SYM_EXPORT WINAPI FileInoculated (LPNONAV lpNav)
{
    auto    BOOL            bFileInoc = FALSE;
    auto    BOOL            bFileApisAnsi;
    auto    char            szOemFromAnsiName [ SYM_MAX_PATH + 1 ];
    auto    UINT            uResult;


    bFileApisAnsi = AreFileApisANSI();

    if ( !bFileApisAnsi )
        SetFileApisToANSI();

    // Determine whether or not to check for inoculation.
    // Filename must convert between Ansi and OEM.
    // File must also not be excluded.

    CharToOem( lpNav->Infected.szFullPath, szOemFromAnsiName );

    if ( ( 0 == STRICMP( szOemFromAnsiName, lpNav->Infected.szFullOemPath ) ) &&
         !ExcludeIsEx( lpNav->lpExclude, lpNav->szInfectedSFN, lpNav->szInfectedLFN, excINOC_CHANGE ) )
    {
        // Determine inoculation status of the file.

        if ( ( SMARTOPEN_RO == lpNav->uInocOpenOK ) ||
             ( SMARTOPEN_RW == lpNav->uInocOpenOK ) )
        {
            uResult = IsInoculated( lpNav->Infected.szFullPath );
            lpNav->Infected.Notes.wVirID = (WORD)-1;    // ?? Martin
        }
        else
        {
            // If we couln't open the inoculation database, assume there is
            // none for the drive and default to not inoculated.

            uResult = FALSE;
        }

        switch ( uResult )
        {
        case FALSE:

            // The file has never been inoculated

            bFileInoc = HandleNotInoculated( lpNav );
            break;

        case TRUE:

            // The file is inoculated and unchanged.

            bFileInoc = TRUE;
            break;

        case INOC_CHANGED:

            // The file was previously inoculated, but has since changed.

            bFileInoc = HandleInoculatedChanged( lpNav );
            break;
        }
    }

    if (!bFileApisAnsi)
        SetFileApisToOEM();

    return (bFileInoc);
} // FileInoculated()


//************************************************************************
// HandleNotInoculated()
//
// This routine is called for all files found which are not inoculated.
// It decides how to respond, displays all appropriate UI and saves
// result info.
//
// Parameters:
//      LPNONAV     lpNav           Junk bin of scan information.
//
// Returns:
//      TRUE                        If file was automatically inoculated.
//      FALSE                       otherwise.
//************************************************************************
// 05/15/1997 DALLEE, broke this functionality out of obese FileInoculated().
//************************************************************************

BOOL LOCAL HandleNotInoculated ( LPNONAV    lpNav )
{
    auto    NOTIFYSTRUCT    Notify;
    auto    BOOL            bFileInoc = FALSE;


    // Set initial file status.

    lpNav->Infected.wStatus = FILESTATUS_NOTINOCULATED;
    *lpNav->Infected.Notes.lpVirName = EOS;

    // Decide how to respond.

    switch (lpNav->Inoc.uActionNewInoc)
    {
    case INOC_PROMPT:
        if ( TRUE == lpNav->Opt.bPrestoNotify )
        {
            // Ask user what to do immediately.

            ScanTime( lpNav->hMommyWindow, FALSE );

            if ( STOP == InoculateFile( lpNav ) )
                {
                lpNav->bDoScan = FALSE;
                }

            ScanTime( lpNav->hMommyWindow, TRUE );
        }
        else
        {
            // Otherwise just save info for Problems Found at end of scan.

            AddVirusInfo( lpNav );
        }

        break;

    case INOC_AUTO:
    case INOC_AUTO_FILEONLY:
        if ( SMARTOPEN_RW == lpNav->uInocOpenOK )
        {
            bFileInoc = InocThisFile( lpNav, lpNav->hMommyWindow, FALSE );
        }

        lpNav->Infected.wStatus = UpdateFileStatus( lpNav->Infected.wStatus,
                                                    PROCESS_INOCULATE,
                                                    bFileInoc );

        AddVirusInfo( lpNav );

        if ( ( TRUE == lpNav->Opt.bPrestoNotify ) &&
             ( FALSE == bFileInoc) )
        {
            ScanTime( lpNav->hMommyWindow, FALSE );

            MEMSET( &Notify, '\0', sizeof( Notify ) );
            Notify.hParent       = lpNav->hMommyWindow;
            Notify.uMsgID        = IDS_FILENOTINOCULATED,
            Notify.uTitleID      = IDS_NAV;
            Notify.lpFileName    = lpNav->Infected.szFullPath;
            Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
            Notify.lpAlertMsg    = lpNav->Alert.bDispAlertMsg ?
                                        lpNav->Alert.szAlertMsg : NULL;
            Notify.uRemoveAfter  = lpNav->Alert.bRemoveAfter ?
                                        lpNav->Alert.uSeconds : NULL;
            Notify.hInstance     = hInstance;

            NotifyUser( &Notify );

            ScanTime( lpNav->hMommyWindow, TRUE );
        }

        break;

    case INOC_DENY:
    case INOC_REPORT:
        AddVirusInfo( lpNav );
        break;
    }

    return ( bFileInoc );
} // HandleNotInoculated()


//************************************************************************
// HandleInoculatedChanged()
//
// This routine is called for all files found which were inoculated but
// have changed since being inoculated.
// It decides how to respond, displays all appropriate UI and saves
// result info.
//
// Parameters:
//      LPNONAV     lpNav           Junk bin of scan information.
//
// Returns:
//      TRUE                        If file was automatically inoculated.
//      FALSE                       otherwise.
//************************************************************************
// 05/15/1997 DALLEE, broke this functionality out of obese FileInoculated().
//************************************************************************

BOOL LOCAL HandleInoculatedChanged ( LPNONAV    lpNav )
{
    auto    NETALERT        Alert;
    auto    NOTIFYSTRUCT    Notify;
    auto    BOOL            bFileInoc = FALSE;
    auto    BOOL            bMoved = FALSE;
    auto    UINT            uTempOpt;


    // Set initial file status.

    lpNav->Infected.wStatus = FILESTATUS_INOCULATEDCHANGED;
    *lpNav->Infected.Notes.lpVirName = EOS;

    // Decide how to respond.

    if ( INOC_AUTO_FILEONLY == lpNav->Inoc.uActionNewInoc )
    {
        // Kluge for Tools/Inoculation where the user wants to update
        // inoculation information for a group of files.
        // In this case, silently reinoculate these changed files.

        uTempOpt = INOC_AUTO_FILEONLY;
    }
    else
    {
        Alert.lpNet          = lpNav->lpNetUserItem;
        Alert.lpInfected     = &lpNav->Infected;
        Alert.lpAlertOpt     = &lpNav->Alert;
        Alert.uTypeOfEvent   = EV_ALERT_INOC_CHANGE;
        NetAlert( &Alert );

        uTempOpt = lpNav->Inoc.uActionInocChg;
    }

    switch (uTempOpt)
    {
    case INOC_PROMPT:
        if ( TRUE == lpNav->Opt.bPrestoNotify )
        {
            ScanTime( lpNav->hMommyWindow, FALSE );

            if ( STOP == InocFileChanged( lpNav ) )
            {
                lpNav->bDoScan = FALSE;
            }

            ScanTime( lpNav->hMommyWindow, TRUE );
        }
        else
        {
            AddVirusInfo( lpNav );
        }

        break;

    case INOC_DENY:
    case INOC_REPORT:
        if ( lpNav->Opt.bPrestoNotify == TRUE )
        {
            ScanTime( lpNav->hMommyWindow, FALSE );

            MEMSET( &Notify, '\0', sizeof( Notify ) );
            Notify.hParent       = lpNav->hMommyWindow;
            Notify.uMsgID        = IDS_FILEINOCCHANGED;
            Notify.uTitleID      = IDS_NAV;
            Notify.lpFileName    = lpNav->Infected.szFullPath;
            Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
            Notify.lpAlertMsg    = lpNav->Alert.bDispAlertMsg ?
                                        lpNav->Alert.szAlertMsg : NULL;
            Notify.uRemoveAfter  = lpNav->Alert.bRemoveAfter ?
                                        lpNav->Alert.uSeconds : NULL;
            Notify.hInstance     = hInstance;

            NotifyUser( &Notify );

            ScanTime( lpNav->hMommyWindow, TRUE );
        }

        AddVirusInfo( lpNav );
        break;

    case INOC_AUTO_FILEONLY:
        lpNav->Infected.wStatus = FILESTATUS_INOCULATEDCHANGED;

        if ( SMARTOPEN_RW == lpNav->uInocOpenOK )
        {
            bFileInoc = InocThisFile( lpNav, lpNav->hMommyWindow, FALSE );
        }

        lpNav->Infected.wStatus = UpdateFileStatus( lpNav->Infected.wStatus,
                                                    PROCESS_INOCULATE,
                                                    bFileInoc );

        AddVirusInfo( lpNav );
        break;

    case INOC_QUARANTINE:
        bMoved = NavMoveFile( lpNav, lpNav->hMommyWindow, TRUE );

        lpNav->Infected.wStatus = UpdateFileStatus( lpNav->Infected.wStatus,
                                                    PROCESS_QUARANTINE,
                                                    bMoved );

        AddVirusInfo( lpNav );
        break;
    }

    return ( bFileInoc );
} // HandleInoculatedChanged()

