// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/bldlog.cpv   1.11   23 Sep 1998 15:29:02   cnevare  $
//
// Description:
//      Builds the correct log file name.
//
// Contains (as exports):
//
// See Also:
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/bldlog.cpv  $
// 
//    Rev 1.11   23 Sep 1998 15:29:02   cnevare
// Fixed type cast problem for builds on different platforms.
// 
//    Rev 1.10   22 Sep 1998 22:11:56   tcashin
// Added events for scan start, directory access denied and file access denied.
// 
//    Rev 1.9   17 Sep 1998 16:02:46   tcashin
// lpNoNav in the new LOGSTRUCTEX is passed in as LPVOID. Make it
// be the right the thing (LPNONAV).
// 
//    Rev 1.8   17 Sep 1998 14:36:52   tcashin
// Added LogEventEx(). When the event LOGEV_SCANEND it will log the
// scan results statistics.
// 
//    Rev 1.7   28 Jun 1998 15:30:48   tcashin
// Missing file status (FILESSTATUS_QUARANTINE_INFECTED_ZIP) in LogFileStatus()
// was causing odd log entries.
// 
//    Rev 1.6   01 Jun 1998 18:31:02   tcashin
// When writing quarantine records to the activity log, we need to
// check the bQuarantine flag, not bKnown. Also check for quarantined 
// zip files.
// 
//    Rev 1.5   22 May 1998 10:37:22   mdunn
// Fixed things up to compile for DOS/DVX/NCX.
// 
//    Rev 1.4   21 May 1998 16:42:26   rpulint
// Add Server name in ActivityLog: Modified LogEvent() and 
// WriteAllFileLogRecords()
// 
//    Rev 1.3   07 Apr 1998 18:41:02   tcashin
// Use the new quarantine record type.
// 
//    Rev 1.2   14 May 1997 16:25:22   TIVANOV
// remove quarantine from LogEvent
// 
//    Rev 1.1   09 May 1997 16:55:14   TIVANOV
// make hanges quaranrine cahnges in LogEvent; WriteAllFileLogRecords;LogFileSta
// s
//
//    Rev 1.0   06 Feb 1997 21:05:12   RFULLER
// Initial revision
//
//    Rev 1.3   17 Sep 1996 15:43:00   JALLEE
// Removed code causing GPF during logging of inoculation events.
//
//    Rev 1.2   07 Aug 1996 13:50:56   YVANG
// Ported changes from LuigiPlus.
//
//    Rev 1.1   04 Apr 1996 17:34:36   YVANG
// Ported from Kirin.
//
//    Rev 1.1   03 Apr 1996 15:25:36   YVANG
// DBCS enable.
//
//    Rev 1.0   30 Jan 1996 15:56:28   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:30   BARRY
// Initial revision.
//
//    Rev 1.14   09 Nov 1995 16:56:36   JWORDEN
// Special handling for log records involving MBR viruses
//
//    Rev 1.13   09 Nov 1995 11:36:36   JWORDEN
//
//    Rev 1.12   24 Oct 1995 19:46:48   JWORDEN
// Separate processing for boot record and MBR
// Process MBR file name as a single byte drive number
//
//    Rev 1.11   06 Jun 1995 20:27:52   BARRY
// Improved handling of username (malloc isn't fatal anymore)
//
//    Rev 1.10   19 Apr 1995 21:41:12   MFALLEN
// Virus name buffer was not correctly null terminated.
//
//    Rev 1.9   12 Apr 1995 13:04:58   MFALLEN
// Allocate buffers at run-time. LFNs puked.
//
//    Rev 1.8   10 Apr 1995 20:58:00   MFALLEN
// Some log events do not have any action field. For those events make sure that
// the AddLog.lpAction field is set to NULL.
//
//    Rev 1.7   23 Jan 1995 15:54:28   MFALLEN
// Some allignment problems.
//
//    Rev 1.6   20 Jan 1995 19:11:38   MFALLEN
// include correct string ID file.
//
//    Rev 1.5   20 Jan 1995 19:09:32   MFALLEN
// All the DOS and VXD strings are accessed using the LoadString() functions
// that emulates it's Windows counterpart. All the string ID's are stored in the
// stbexts.h file that identical for all the platforms. DOS and VXD strings are
// contained in the UTILSTR.STR file.
//
//    Rev 1.4   29 Dec 1994 17:40:56   DALLEE
// extern "C" around DOS external strings.
//
//    Rev 1.3   20 Dec 1994 17:59:08   MFALLEN
// User correct Win32 API to obtain user name.
//
//    Rev 1.2   20 Dec 1994 17:21:24   MFALLEN
// symineg.h was removed from nonav.h include file and had to be included here.
//
//    Rev 1.1   16 Dec 1994 13:04:30   MFALLEN
// Dos build was complaining.
//
//    Rev 1.0   16 Dec 1994 10:37:00   MFALLEN
// Initial revision.
//
//    Rev 1.23   15 Dec 1994 20:14:06   MFALLEN
// From NIRVANA
//
//    Rev 1.22   01 Jun 1994 16:26:52   MFALLEN
// We indicate if no item's were scanned.
//
//    Rev 1.21   23 Mar 1994 11:49:16   DALLEE
// Added configuration change log message for Bankers Trust version.
//
//    Rev 1.20   20 Mar 1994 12:27:32   KEITH
// Bankers Trust - Include TSR.H so that TSRIsLoaded et. al. is defined.
//
//    Rev 1.19   20 Mar 1994 09:23:14   KEITH
// Bankers Trust - Allow access to log file when evDeny is active
//
//    Rev 1.18   17 Mar 1994 14:50:30   DALLEE
// Put #if defined(SYM_WIN) around BANKERS code not included in DOS BANKERS.
//
//    Rev 1.17   14 Mar 1994 16:51:26   KEITH
// Bankers Trust - pass a drive list scanned seperate from the Message
// string for ALOG_FI_STARTEND when scan was completed so that this
// doesn't have to be parsed from the message for the "LASTSSCAN" utility.
//
//
//    Rev 1.16   11 Mar 1994 00:21:00   DALLEE
// Check vs. ACTOPTS.bRegChange before logging registration activities.
//
//    Rev 1.15   10 Mar 1994 22:56:56   DALLEE
// Log registration events as registration type, not inoculation.
//
//    Rev 1.14   08 Mar 1994 17:58:38   DALLEE
// Added file registration messages for BANKERS target.
//
//    Rev 1.13   13 Dec 1993 12:07:30   MFALLEN
// Completion of scan activity log modification.
//
//    Rev 1.12   03 Nov 1993 12:12:18   BARRY
// Not initializing buffer before call to NetGetCurrentUserName()
//
//    Rev 1.11   14 Oct 1993 20:05:08   DALLEE
// Call cover for NetGetCurrentUserName in DOS.
//
//    Rev 1.10   04 Oct 1993 10:55:16   MFALLEN
// Added NameIsDevice() to activity log file validation function.
//
//    Rev 1.9   19 Sep 1993 10:34:02   DALLEE
// Make sure szUserName is initialized for DOS in WriteAllFileLogRecords()
// before passing its address to symAlogAdd().
//
//    Rev 1.8   06 Sep 1993 18:16:22   DALLEE
// Check Activity Settings before logging events in LogEvent().
//
//    Rev 1.7   05 Sep 1993 20:16:58   DALLEE
// Added error return codes.
//************************************************************************

#include "platform.h"                   // Platform independence
#include "xapi.h"
#include "symalog.h"                    // symALogAdd()

#ifdef SYM_DOS
    #include "stddos.h"
    #include <stdio.h>
#endif

#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "stbexts.h"                    // string IDs

#ifdef SYM_WIN
    #include "symcfg.h"
    #include "options.h"
#endif

#include "dbcs.h"

MODULE_NAME;

#ifdef SYM_WIN
    extern  HINSTANCE   hInstance;
#else
    HINSTANCE       hInstance;      // bogus
#endif


#ifdef SYM_WIN32
// 6/15/99 GMW - Copied from the decomposer; don't want to link NAVXUTIL to the
// decomposer just to pick up nicely-displayed container files in the Activity Log:
#define APPEND_BUFFER_SIZE  (MAX_PATH * 2 * sizeof (TCHAR))
#define RECOMPOSER_TOKEN    "||"
TCHAR szRecToken[6] = _T(RECOMPOSER_TOKEN);
#endif

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

UINT LOCAL LogFileStatus (
    LPSTR           lpLogFile,
    LPINFECTEDFILE  lpInfected,
    LPALOGADD       lpAddLog,
    DWORD           dwSize
    );

UINT LOCAL GetNumInfected (
    LPINFARR        lpInfectedArray
    );

#ifdef SYM_WIN32

BOOL BuildContainerDisplayString (
    LPTSTR pContainerString,
    LPTSTR *lpBuffer
    );

BOOL ParseContainerString (LPTSTR pPath, LPTSTR pBuffer, long lSize, DWORD dwDepth);

LPTSTR AppendText (LPTSTR pszBuffer, DWORD *pdwSize, LPTSTR pszText);

#endif

//************************************************************************
// BuildLogFilename()
//
// This routine builds a valid activity log filename based, if possible,
// on the filename passed.
//
// Parameters:
//      LPSTR   lpLogFilename           Pointer to buffer to place new name.
//      LPCSTR  lpPartialFilename       Base filename.
//
// Returns:
//      Nothing
//************************************************************************
// 4/\93 MFALLEN, created.
//************************************************************************

VOID SYM_EXPORT WINAPI BuildLogFilename (
    LPSTR   lpLogFilename,
    LPCSTR  lpPartialFilename
    )
{
    auto        char            szFile [SYM_MAX_PATH+1];
    auto        char            szDir  [SYM_MAX_PATH+1];


    // ************************************************************** //
    // It is here where we check if the provided activity log file to //
    // use is valid. If either the directory, or the file is missing  //
    // we'll do our best to use the available information.            //
    // ************************************************************** //

                                        // Get path component and file component
    NameReturnPath (lpPartialFilename, szDir);
    NameReturnFile (lpPartialFilename, szFile);

                                // If no path, then use Start Dir
    if ( *szDir == EOS )
        GetStartDir(hInstance, szDir, sizeof(szDir));

                                // If no file, then use default
    if ( *szFile == EOS )
        {
        LoadString ( hInstance, IDS_DEF_ACTIFILE, szFile, sizeof(szFile) );
        }

                                // Append file
    NameAppendFile(szDir, szFile);

                                // If not valid, just use known directory
    if (NameIsDevice(szDir) || !NameValidatePath(szDir))
        {
        GetStartDir ( hInstance, szDir, sizeof(szDir) );
        NameAppendFile ( szDir, szFile );
        }
                                        // Save result
    STRCPY ( lpLogFilename, szDir );
}


//************************************************************************
// LogEvent()
//
// This routine writes the information on the specified event
// to the activity log.
//
// Parameters:
//      LPLOGSTRUCT     lpLog           Info on what to log.
//
// Returns:
//      NOERR                           Successful.
//      ALOGERR_OPENLOG                 Could not open log.
//      ALOGERR_MEMORY                  Out of memory.
//************************************************************************
// 4/\93 MFALLEN, created.
// 5/30/93 DALLEE, removed some dup code.  Work under DOS.
// 9/05/93 DALLEE, return errors.
//************************************************************************

UINT SYM_EXPORT WINAPI LogEvent (LPLOGSTRUCT lpLog)
{
    auto    char        szOS      [ SYM_MAX_PATH+1 ];
    auto    char        szLogFile [ SYM_MAX_PATH+1 ];
    auto    char        szMessage [ SYM_MAX_PATH+1 ];
    auto    char        szAction  [ SYM_MAX_PATH+1 ];
    auto    char        szFormat  [ SYM_MAX_PATH*2 ];
    auto    LPSTR       lpServer = NULL;
    auto    LPSTR       lpUser;
    auto    ALOGADD     AddLog;
    auto    BYTE        byRecType = 0;
    auto    UINT        uStatus = NOERR;
    auto    BOOL        bMBR = FALSE;

    *szMessage = EOS;

    // ************************************************************** //
    // Make a copy of the provided log file name so we don't screw up //
    // the option settings when the user hits Options, and make sure  //
    // that the activity log file name is valid.                      //
    // ************************************************************** //

    BuildLogFilename (szLogFile, lpLog->lpAct->szLogFile);

#ifdef SYM_WIN32
    lpServer = (LPSTR)MemAllocPtr(GHND, MAX_COMPUTERNAME_LENGTH+2);
#endif

    lpUser = (LPSTR)MemAllocPtr(GHND, SIZE_NET_USER_LIST+1);

    if ( lpUser )
        {
        lpUser[0]=EOS;                  // Initialize

#ifdef SYM_WIN32
        DWORD   dwSize = SIZE_NET_USER_LIST;
        GetUserName ( lpUser, &dwSize );

        if (lpServer)
            {
            lpServer[0]=EOS;                  // Initialize
            dwSize = MAX_COMPUTERNAME_LENGTH + 1;
            GetComputerName ( lpServer, &dwSize );
            }
#else
        NetGetCurrentUserName(NULL, NULL, lpUser, SIZE_NET_USER_LIST);
#endif
        }

    LoadString ( hInstance, IDS_LOG_SCANNER, szOS, sizeof(szOS) );

                                    // Initialize the ALOGADD structure
    MEMSET (&AddLog, '\0', sizeof(AddLog));
    AddLog.SSize    = sizeof(AddLog);
    AddLog.lpOS     = szOS;

    if ( lpUser && *lpUser )
        AddLog.lpUser   = lpUser;
    else
        AddLog.lpUser   = NULL;

    if ( lpServer && *lpServer )
        AddLog.lpServer   = lpServer;
    else
        AddLog.lpServer   = NULL;

    AddLog.lpAction = szAction;

    switch (lpLog->uEvent)
        {
        case LOGEV_HALTCPU:
            if (lpLog->lpAct->bStartEnd || lpLog->lpAct->bKnown)
                {
                if (lpLog->lpFileName
                &&  lpLog->lpFileName[0] < 'A'
                &&  lpLog->lpFileName[1] == ':')
                    {
                    bMBR = TRUE;
                    LoadString ( hInstance,
                                IDS_LOG_MBRISINFECTED, szMessage,
                                sizeof(szMessage));
                    }
                else
                    if (lpLog->lpFileName && lpLog->lpFileName[1] == ':' &&
                        STRLEN(lpLog->lpFileName) == 2)
                        {
                        LoadString ( hInstance,
                                    IDS_LOG_BOOTISINFECTED, szMessage,
                                    sizeof(szMessage));
                        }
                    else
                        if (lpLog->lpFileName && *lpLog->lpFileName)
                        {
                        LoadString ( hInstance,
                                    IDS_LOG_FILEISINFECTED, szMessage,
                                    sizeof(szMessage));
                        }
                    else
                        {
                        LoadString ( hInstance,
                                    IDS_LOG_MEMORYISINFECTED, szMessage,
                                    sizeof(szMessage));
                        }

                LoadString ( hInstance, IDS_LOGACTION_HALTCPU,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_KNOWNVIRUS;
                }
            break;

        case LOGEV_SCANEND:
            if (lpLog->lpAct->bStartEnd)
                {
                                    // ----------------------------------
                                    // If no items were scanned, indicate
                                    // ----------------------------------
                if (lpLog->lpFileName
                &&  STRLEN(lpLog->lpFileName) > 0)
                    {
                    LoadString (hInstance, IDS_LOG_SCANEND, szMessage,
                                sizeof(szMessage));
                    }
                else
                    {
                    LoadString (hInstance, IDS_LOG_SCANEND_NONE, szMessage,
                                sizeof(szMessage));
                    }

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_STARTEND;
                }
            break;

        case LOGEV_SCANINTERRUPTED:
            if (lpLog->lpAct->bStartEnd)
                {
                                    // ----------------------------------
                                    // If no items were scanned, indicate
                                    // ----------------------------------
                if (lpLog->lpFileName
                &&  STRLEN(lpLog->lpFileName) > 0)
                    {
                    LoadString (hInstance, IDS_LOG_SCANINTERRUPTED,
                                szMessage,
                                sizeof(szMessage));
                    }
                else
                    {
                    LoadString (hInstance,IDS_LOG_SCANINTERRUPTED_NONE,
                                szMessage,
                                sizeof(szMessage));
                    }

                AddLog.lpAction   = NULL;
                byRecType = ALOG_FI_STARTEND;
                }
            break;

        case LOGEV_VIRLISTCHANGE:
            if (lpLog->lpAct->bVirusList)
                {
                LoadString (hInstance, IDS_LOG_VIRLISTCHANGED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_VIRLISTCHANGE;
                }
            break;

        case LOGEV_INOCULATED_NEW:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_FILEINOC_NEW, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INOCULATED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INOCULATEFAILED_NEW:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_FILEINOC_NEW, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INOCULATEFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INOCULATED_CHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_FILEINOC_CHANGE, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INOCULATED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INOCULATEFAILED_CHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_FILEINOC_CHANGE, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INOCULATEFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_UNINOCULATED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_UNINOCULATED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_UNINOCULATEFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_UNINOCULATEFAILED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_CREATEFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_CREATEFAILED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_REINOCULATED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_REINOCULATED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_REINOCULATEFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_CREATEFAILED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_MBREPAIRED:
            if (lpLog->lpAct->bInocChange)
                {
                bMBR = TRUE;
                LoadString (hInstance,IDS_LOG_INTEG_MBCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_MBREPAIRED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_MBREPAIRFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                bMBR = TRUE;
                LoadString (hInstance,IDS_LOG_INTEG_MBCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_MBREPAIRFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_BOOTREPAIRED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_INTEG_BOOTCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_BOOTREPAIRED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_BOOTREPAIRFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_BOOTCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_BOOTREPAIRFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_SYSFILEREPAIRED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_SYSFILECHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_SYSFILEREPAIRED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_SYSFILEREPAIRFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_SYSFILECHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_SYSFILEREPAIRFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_MBCHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                bMBR = TRUE;
                LoadString (hInstance,IDS_LOG_INTEG_MBCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_BOOTCHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_BOOTCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_SYSFILECHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_INTEG_SYSFILECHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_NOTEXIST:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_NOTEXIST, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;
        }

    if ( byRecType )
        {
        if (*szMessage)
            {
            if ((lpLog->lpFileName && lpLog->lpFileName[1] == ':')
            ||  (lpLog->lpVirusName && *lpLog->lpVirusName))
                {
                if (bMBR)
                    SPRINTF (szFormat, szMessage, lpLog->lpFileName[0],
                                                  (LPSTR)lpLog->lpVirusName);
                else
                    SPRINTF (szFormat, szMessage, (LPSTR)lpLog->lpFileName,
                                                  (LPSTR)lpLog->lpVirusName);
                AddLog.lpMessage = szFormat;
                }
            else if (lpLog->lpFileName && *lpLog->lpFileName)
                {
                if (bMBR)
                    SPRINTF (szFormat, szMessage, lpLog->lpFileName[0]);
                else
                    SPRINTF (szFormat, szMessage, (LPSTR)lpLog->lpFileName);
                AddLog.lpMessage = szFormat;
                }
            else
                AddLog.lpMessage = szMessage;
            }


        uStatus = symALogAdd(szLogFile,
                             byRecType,
                             ALOG_PT_WINDOWS,
                             0,
                             &AddLog,
                             (DWORD)lpLog->lpAct->uKiloBytes * 1024);
        }

    if ( lpUser )
        MemFreePtr(lpUser);

    if ( lpServer )
        MemFreePtr(lpServer);

    return (uStatus);
} // End LogEvent()


//************************************************************************
// LogEventEx()
//
// This routine is a superset of LogEvent() that writes the information on
// the specified event to the activity log.
//
// Parameters:
//      LPLOGSTRUCTEX   lpLog           Info on what to log.
//
// Returns:
//      NOERR                           Successful.
//      ALOGERR_OPENLOG                 Could not open log.
//      ALOGERR_MEMORY                  Out of memory.
//************************************************************************
// 9/12/98 TCASHIN Created
//************************************************************************

UINT SYM_EXPORT WINAPI LogEventEx (LPLOGSTRUCTEX lpLog)
{
    auto    char        szOS      [ SYM_MAX_PATH+1 ];
    auto    char        szLogFile [ SYM_MAX_PATH+1 ];
    auto    char        szMessage [ SYM_MAX_PATH*2 ];
    auto    char        szAction  [ SYM_MAX_PATH+1 ];
    auto    char        szFormat  [ SYM_MAX_PATH*2 ];
    auto    char        szTempBuffer [ SYM_MAX_PATH*2 ];
    auto    LPSTR       lpServer = NULL;
    auto    LPSTR       lpUser;
    auto    ALOGADD     AddLog;
    auto    BYTE        byRecType = 0;
    auto    UINT        uStatus = NOERR;
    auto    BOOL        bMBR = FALSE;
    auto    LPNONAV     lpNoNav = NULL;

    *szMessage = EOS;

    // ************************************************************** //
    // Make a copy of the provided log file name so we don't screw up //
    // the option settings when the user hits Options, and make sure  //
    // that the activity log file name is valid.                      //
    // ************************************************************** //

    BuildLogFilename (szLogFile, lpLog->lpAct->szLogFile);

#ifdef SYM_WIN32
    lpServer = (LPSTR)MemAllocPtr(GHND, MAX_COMPUTERNAME_LENGTH+2);
#endif

    lpUser = (LPSTR)MemAllocPtr(GHND, SIZE_NET_USER_LIST+1);

	if (lpLog->lpNoNav)
		{
#ifdef SYM_WIN
		lpNoNav = reinterpret_cast<LPNONAV>(lpLog->lpNoNav);
#else
		lpNoNav = (LPNONAV) (lpLog->lpNoNav);
#endif
		}

    if ( lpUser )
        {
        lpUser[0]=EOS;                  // Initialize

#ifdef SYM_WIN32
        DWORD   dwSize = SIZE_NET_USER_LIST;
        GetUserName ( lpUser, &dwSize );

        if (lpServer)
            {
            lpServer[0]=EOS;                  // Initialize
            dwSize = MAX_COMPUTERNAME_LENGTH + 1;
            GetComputerName ( lpServer, &dwSize );
            }
#else
        NetGetCurrentUserName(NULL, NULL, lpUser, SIZE_NET_USER_LIST);
#endif
        }

    LoadString ( hInstance, IDS_LOG_SCANNER, szOS, sizeof(szOS) );

                                    // Initialize the ALOGADD structure
    MEMSET (&AddLog, '\0', sizeof(AddLog));
    AddLog.SSize    = sizeof(AddLog);
    AddLog.lpOS     = szOS;

    if ( lpUser && *lpUser )
        AddLog.lpUser   = lpUser;
    else
        AddLog.lpUser   = NULL;

    if ( lpServer && *lpServer )
        AddLog.lpServer   = lpServer;
    else
        AddLog.lpServer   = NULL;

    AddLog.lpAction = szAction;

    switch (lpLog->uEvent)
        {
        case LOGEV_HALTCPU:
            if (lpLog->lpAct->bStartEnd || lpLog->lpAct->bKnown)
                {
                if (lpLog->lpFileName
                &&  lpLog->lpFileName[0] < 'A'
                &&  lpLog->lpFileName[1] == ':')
                    {
                    bMBR = TRUE;
                    LoadString ( hInstance,
                                IDS_LOG_MBRISINFECTED, szMessage,
                                sizeof(szMessage));
                    }
                else
                    if (lpLog->lpFileName && lpLog->lpFileName[1] == ':' &&
                        STRLEN(lpLog->lpFileName) == 2)
                        {
                        LoadString ( hInstance,
                                    IDS_LOG_BOOTISINFECTED, szMessage,
                                    sizeof(szMessage));
                        }
                    else
                        if (lpLog->lpFileName && *lpLog->lpFileName)
                        {
                        LoadString ( hInstance,
                                    IDS_LOG_FILEISINFECTED, szMessage,
                                    sizeof(szMessage));
                        }
                    else
                        {
                        LoadString ( hInstance,
                                    IDS_LOG_MEMORYISINFECTED, szMessage,
                                    sizeof(szMessage));
                        }

                LoadString ( hInstance, IDS_LOGACTION_HALTCPU,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_KNOWNVIRUS;
                }
            break;

        case LOGEV_SCANEND:
            if (lpLog->lpAct->bStartEnd)
                {
                                    // ----------------------------------
                                    // If no items were scanned, indicate
                                    // ----------------------------------
                if (lpLog->lpFileName
                &&  STRLEN(lpLog->lpFileName) > 0)
                    {
                    if ( lpLog->lpNoNav )
                    {
                        auto    char  szTempBuffer [ SYM_MAX_PATH*2 ];

                        LoadString (hInstance, IDS_LOG_SCANEND_STATS, szTempBuffer,
                                    sizeof(szTempBuffer));

                        SPRINTF (szMessage, szTempBuffer,
                                            lpLog->lpFileName,
                                            lpNoNav->Stats.MasterBootRec.uScanned,
                                            lpNoNav->Stats.MasterBootRec.uInfected,
                                            lpNoNav->Stats.MasterBootRec.uCleaned,
                                            lpNoNav->Stats.BootRecs.uScanned,
                                            lpNoNav->Stats.BootRecs.uInfected,
                                            lpNoNav->Stats.BootRecs.uCleaned,                                            
                                            lpNoNav->Stats.Files.dwScanned,
                                            lpNoNav->Stats.Files.uInfected,
                                            lpNoNav->Stats.Files.uCleaned,
                                            lpNoNav->Stats.Files.uQuarantined,
                                            lpNoNav->Stats.Files.uDeleted);
                    }

                    else
                        LoadString (hInstance, IDS_LOG_SCANEND, szMessage,
                                    sizeof(szMessage));
                    }
                else
                    {
                    LoadString (hInstance, IDS_LOG_SCANEND_NONE, szMessage,
                                sizeof(szMessage));
                    }

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_STARTEND;
                }
            break;

        case LOGEV_SCANINTERRUPTED:
            if (lpLog->lpAct->bStartEnd)
                {
                                    // ----------------------------------
                                    // If no items were scanned, indicate
                                    // ----------------------------------
                if (lpLog->lpFileName
                &&  STRLEN(lpLog->lpFileName) > 0)
                    {
                    LoadString (hInstance, IDS_LOG_SCANINTERRUPTED,
                                szMessage,
                                sizeof(szMessage));
                    }
                else
                    {
                    LoadString (hInstance,IDS_LOG_SCANINTERRUPTED_NONE,
                                szMessage,
                                sizeof(szMessage));
                    }

                AddLog.lpAction   = NULL;
                byRecType = ALOG_FI_STARTEND;
                }
            break;

        case LOGEV_VIRLISTCHANGE:
            if (lpLog->lpAct->bVirusList)
                {
                LoadString (hInstance, IDS_LOG_VIRLISTCHANGED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_VIRLISTCHANGE;
                }
            break;

        case LOGEV_INOCULATED_NEW:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_FILEINOC_NEW, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INOCULATED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INOCULATEFAILED_NEW:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_FILEINOC_NEW, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INOCULATEFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INOCULATED_CHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_FILEINOC_CHANGE, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INOCULATED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INOCULATEFAILED_CHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_FILEINOC_CHANGE, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INOCULATEFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_UNINOCULATED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_UNINOCULATED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_UNINOCULATEFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_UNINOCULATEFAILED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_CREATEFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_CREATEFAILED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_REINOCULATED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_REINOCULATED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_REINOCULATEFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_CREATEFAILED, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_MBREPAIRED:
            if (lpLog->lpAct->bInocChange)
                {
                bMBR = TRUE;
                LoadString (hInstance,IDS_LOG_INTEG_MBCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_MBREPAIRED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_MBREPAIRFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                bMBR = TRUE;
                LoadString (hInstance,IDS_LOG_INTEG_MBCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_MBREPAIRFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_BOOTREPAIRED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_INTEG_BOOTCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_BOOTREPAIRED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_BOOTREPAIRFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_BOOTCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_BOOTREPAIRFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_SYSFILEREPAIRED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_SYSFILECHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_SYSFILEREPAIRED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_SYSFILEREPAIRFAILED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_SYSFILECHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_INTEG_SYSFILEREPAIRFAILED,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_MBCHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                bMBR = TRUE;
                LoadString (hInstance,IDS_LOG_INTEG_MBCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_BOOTCHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_BOOTCHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_SYSFILECHANGED:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance, IDS_LOG_INTEG_SYSFILECHANGED, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_INTEG_NOTEXIST:
            if (lpLog->lpAct->bInocChange)
                {
                LoadString (hInstance,IDS_LOG_INTEG_NOTEXIST, szMessage,
                            sizeof(szMessage));

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, sizeof(szAction) );

                byRecType = ALOG_FI_INOCHANGE;
                }
            break;

        case LOGEV_SCANSTART:
            if (lpLog->lpAct->bStartEnd)
                {
                LoadString (hInstance, IDS_LOG_SCANSTART, szMessage,
                            sizeof(szMessage));

                AddLog.lpAction = NULL;
                byRecType = ALOG_FI_STARTEND;
                }

            break;

        case LOGEV_SYSERROR_ACCESSDENIED_DIR:

			if (lpNoNav)
			    {
				LoadString (hInstance, IDS_LOG_ACCESSDENIED_DIR, szTempBuffer,
										sizeof(szTempBuffer));

    			SPRINTF (szMessage, szTempBuffer, lpNoNav->szTheDirectory);

				AddLog.lpAction = NULL;

				byRecType = ALOG_FI_ERROR;
			    }

            break;

        case LOGEV_SYSERROR_ACCESSDENIED_FILE:


			if (lpNoNav)
			    {
				LoadString (hInstance, IDS_LOG_ACCESSDENIED_FILE, szTempBuffer,
										sizeof(szTempBuffer));

				SPRINTF (szMessage, szTempBuffer, lpNoNav->Infected.szFullPath);

				AddLog.lpAction = NULL;

				byRecType = ALOG_FI_ERROR;
			    }
            break;

        }

    if ( byRecType )
        {
        if (*szMessage)
            {
            if ((lpLog->lpFileName && lpLog->lpFileName[1] == ':')
            ||  (lpLog->lpVirusName && *lpLog->lpVirusName))
                {
                if (bMBR)
                    SPRINTF (szFormat, szMessage, lpLog->lpFileName[0],
                                                  (LPSTR)lpLog->lpVirusName);
                else
                    SPRINTF (szFormat, szMessage, (LPSTR)lpLog->lpFileName,
                                                  (LPSTR)lpLog->lpVirusName);
                AddLog.lpMessage = szFormat;
                }
            else if (lpLog->lpFileName && *lpLog->lpFileName)
                {
                if (bMBR)
                    SPRINTF (szFormat, szMessage, lpLog->lpFileName[0]);
                else
                    SPRINTF (szFormat, szMessage, (LPSTR)lpLog->lpFileName);
                AddLog.lpMessage = szFormat;
                }
            else
                AddLog.lpMessage = szMessage;
            }


        uStatus = symALogAdd(szLogFile,
                             byRecType,
                             ALOG_PT_WINDOWS,
                             0,
                             &AddLog,
                             (DWORD)lpLog->lpAct->uKiloBytes * 1024);
        }

    if ( lpUser )
        MemFreePtr(lpUser);

    if ( lpServer )
        MemFreePtr(lpServer);

    return (uStatus);
} // End LogEventEx()



//************************************************************************
// WriteAllFileLogRecords()
//
// This routine goes through the list of problems found and logs
// any entries specified in the Activity Log Settings.
//
// Parameters:
//      LPACTOPT    lpAct               Activity Log settings.
//      LPVOID      lpvProblems         Infected array pointer to problem items.
//      LOGCALLBACK lpfnCallback        Callback function (for progress bar).
//      DWORD       dwUserData          Optional data (passed to callback).
//
// Returns:
//      NOERR                           Successful.
//      ALOGERR_MEMORY                  Out of memory.
//      ALOGERR_OPENFILE                Could not open log file.
//************************************************************************
// 9/05/93 DALLEE, added spiffy-header, comments, and return error values.
//************************************************************************

UINT SYM_EXPORT WINAPI WriteAllFileLogRecords (
    LPACTOPT    lpAct,
    LPVOID      lpvProblems,
    LOGCALLBACK lpfnCallback,
    DWORD       dwUserData
    )
{
    auto    char            szOS       [SYM_MAX_PATH + 1];
    auto    char            szLogFile  [SYM_MAX_PATH + 1];
    auto    char            szUserName [SIZE_NET_USER_LIST+1];
    auto    ALOGADD         AddLog;
    auto    LPINFARR        lpInfArr;
    auto    LPINFECTEDFILE  lpInfected;
    auto    UINT            uIndex;
    auto    BOOL            bLogThis;
    auto    BOOL            bContinue = TRUE;
    auto    UINT            uWritten = 0;
    auto    UINT            uToWrite;
    auto    LPINFARR        lpInfectedArray = (LPINFARR)lpvProblems;
    auto    UINT            uStatus = NOERR;
#ifdef SYM_WIN32
    auto    char            szServerName [MAX_COMPUTERNAME_LENGTH+2];
#endif

    // --------------------------------------------------------------------- //
                                        // All this information is common to
                                        // every activity log record. All the
                                        // common fields are initialized here
                                        // since it would be silly to do it
                                        // for every record.
                                        // --------------------------------- //
    szUserName[0]=EOS;                      // Initialize

#ifdef SYM_WIN32
    DWORD   dwSize = sizeof(szUserName);
    GetUserName ( szUserName, &dwSize );

    szServerName[0]=EOS;                      // Initialize
    dwSize = sizeof(szServerName);
    GetComputerName ( szServerName, &dwSize );

#else
    NetGetCurrentUserName (NULL, NULL, szUserName, SIZE_NET_USER_LIST);
#endif


    LoadString ( hInstance, IDS_LOG_SCANNER, szOS, sizeof(szOS) );

    uToWrite = GetNumInfected (lpInfectedArray);

                                        // Initialize the ALOGADD structure
    MEMSET (&AddLog, '\0', sizeof(AddLog));
    AddLog.SSize    = sizeof(AddLog);
    AddLog.lpOS     = szOS;
    AddLog.lpUser   = (*szUserName) ? (LPSTR)szUserName : NULL;

#ifdef SYM_WIN32
    AddLog.lpServer   = (*szServerName) ? (LPSTR)szServerName : NULL;
#else
    AddLog.lpServer   = NULL;
#endif

    BuildLogFilename (szLogFile, lpAct->szLogFile);

    // --------------------------------------------------------------------- //
                                        // We loop as many times as we have
                                        // files to be logged.
    while (lpInfectedArray && bContinue == TRUE)
        {
        lpInfArr = lpInfectedArray;

        if (lpInfArr->hInf)
            {
            lpInfected = (LPINFECTEDFILE)MemLock (lpInfArr->hInf);

            for (uIndex = 0;
                 uIndex < lpInfArr->uNumUsed && bContinue == TRUE;
                 uIndex ++)
                {
                bLogThis = FALSE;
                switch (lpInfected[uIndex].wStatus)
                    {                   // ----------------------------------
                                        // File statuses that depend on the
                                        // log known virus activity flag
                                        // ----------------------------------
                    case FILESTATUS_INFECTED:
                    case FILESTATUS_REPAIRED:
                    case FILESTATUS_NOTREPAIRED:
                    case FILESTATUS_DELETED:
                    case FILESTATUS_NOTDELETED:
                    case FILESTATUS_EXCLUDE_VIRUSDETECTION:
                    case FILESTATUS_BOOTINFECTED:
                    case FILESTATUS_BOOTREPAIRED:
                    case FILESTATUS_BOOTNOTREPAIRED:
                    case FILESTATUS_MASTERBOOTINFECTED:
                    case FILESTATUS_MASTERBOOTREPAIRED:
                    case FILESTATUS_MASTERBOOTNOTREPAIRED:
                    case FILESTATUS_ERROR:
                    case FILESTATUS_ZIPINFECTED:
                        if (lpAct->bKnown == TRUE)
                            bLogThis = TRUE;
                        break;
                                        // -----------------------------------
                                        // File statuses that depend on the
                                        // log inoculation activity flag.
                                        // -----------------------------------
                    case FILESTATUS_EXCLUDE_INOCULATION:
                    case FILESTATUS_INOCULATED:
                    case FILESTATUS_INOCULATIONFAILED:
                    case FILESTATUS_NOTINOCULATED:
                    case FILESTATUS_INOCULATEREPAIRED:
                    case FILESTATUS_INOCULATEDELETED:
                    case FILESTATUS_INOCULATEDELETE_FAILED:
                    case FILESTATUS_INOCULATEREPAIR_FAILED:
                    case FILESTATUS_REINOCULATED:
                    case FILESTATUS_REINOCULATIONFAILED:
                    case FILESTATUS_INOCULATEDCHANGED:
                    case FILESTATUS_INOC_CHANGED_DELETED:
                    case FILESTATUS_INOC_CHANGED_NOTDELETED:
                        if (lpAct->bInocChange == TRUE)
                            bLogThis = TRUE;
                        break;

#ifdef BANKERS
                    case FILESTATUS_EXCLUDE_REGISTRATION:
                    case FILESTATUS_REGISTERED:
                    case FILESTATUS_REGISTRATIONFAILED:
                    case FILESTATUS_NOTREGISTERED:
                    case FILESTATUS_REREGISTERED:
                    case FILESTATUS_REREGISTRATIONFAILED:
                    case FILESTATUS_REGISTEREDCHANGED:
                        if (TRUE == lpAct->bRegChange)
                            {
                            bLogThis = TRUE;
                            }
                        break;
#endif  // BANKERS

                    case FILESTATUS_QUARANTINE_INFECTED:
                    case FILESTATUS_QUARANTINE_INFECTED_ZIP:
                    case FILESTATUS_QUARANTINE_INFECTED_FAILED:
                    case FILESTATUS_QUARANTINE_NOTINOCULATED:
                    case FILESTATUS_QUARANTINE_NOTINOCULATED_FAILED:
                    case FILESTATUS_QUARANTINE_INOCULATEDCHANGED:
                    case FILESTATUS_QUARANTINE_INOCULATEDCHANGED_FAILED:
                        if (lpAct->bQuarantine == TRUE)
                            bLogThis = TRUE;
                        break;
                    }

                if (bLogThis == TRUE)
                    {
                    uStatus = LogFileStatus (szLogFile,
                                             &lpInfected[uIndex],
                                             &AddLog,
                                             (DWORD)lpAct->uKiloBytes * 1024);
                    if (uStatus != NOERR)
                        {               // Error occured writing to log.
                                        // Stop now, don't cause 50 million
                                        // errors, one for each record.
                        bContinue = FALSE;
                        }
                    }

                if ( (lpfnCallback != NULL) && bContinue)
                    {
                    bContinue = (*lpfnCallback) (++uWritten, uToWrite,
                                                dwUserData);
                    }
                }

            MemRelease (lpInfArr->hInf);
            }

        lpInfectedArray = (LPINFARR)lpInfArr->lpNext;
        }
    return (uStatus);
}

// These are static variables used in the LogFileStatus. (Made them statics to
// take them off the stack.)
#define LOG_FORMATEDSTRSIZE 1024
#define LOG_ACTIONSTRSIZE    256

static    TCHAR   szFormated  [ LOG_FORMATEDSTRSIZE ];
static    TCHAR   szAction    [ LOG_ACTIONSTRSIZE ];
static    TCHAR   szMessage   [ LOG_ACTIONSTRSIZE ];
static    TCHAR   szVirusName [ SIZE_VIRUSNAME  + 1 ];

#define CONTAINERLOG_SIZE SYM_MAX_PATH

static    TCHAR   szZipPath           [ CONTAINERLOG_SIZE * 2];
static    TCHAR   szContainerCutoff   [ 50 ];

//************************************************************************
// LogFileStatus()
//
// Logs a problem found event based on the item's status.
//
// Parameters:
//      LPSTR           lpLogFile       Name of log file to write to.
//      LPINFECTEDFILE  lpInfected      Problem item.
//      LPALOGADD       lpAddLog        symalog structure to add to log.
//      DWORD           dwMaxSize       Maximum size of the log file.
//
// Returns:
//      NOERR                           Successful.
//      ALOGERR_MEMORY                  Out of memory.
//      ALOGERR_OPENLOG                 Could not open logfile.
//************************************************************************
// 8/18/93 DALLEE, use local buffer and STRNCPY for virus name.
//************************************************************************

UINT LOCAL LogFileStatus (
    LPSTR           lpLogFile,
    LPINFECTEDFILE  lpInfected,
    LPALOGADD       lpAddLog,
    DWORD           dwMaxSize
    )
{
    const   UINT    uFormatedBufferSize     =   LOG_FORMATEDSTRSIZE;
    const   UINT    uMessageBufferSize      =   LOG_ACTIONSTRSIZE;

    auto    BOOL    bAdd            =   TRUE;
    auto    UINT    uStatus         =   NOERR;
    auto    BYTE    byLogType;

        lpAddLog->lpAction = szAction;

        switch ( lpInfected->wStatus )
            {
            case FILESTATUS_INFECTED:
                if ( lpInfected->bInContainer )
                {
                LoadString ( hInstance, IDS_LOG_CONTAINERFILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }
                else
                {
                LoadString ( hInstance, IDS_LOG_FILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_REPAIRED:
                if ( lpInfected->bInContainer )
                {
                LoadString ( hInstance, IDS_LOG_CONTAINERFILEWASINFECTED,
                             szMessage, uMessageBufferSize );
                }
                else
                {
                LoadString ( hInstance, IDS_LOG_FILEWASINFECTED,
                             szMessage, uMessageBufferSize );
                }

                LoadString ( hInstance, IDS_LOGACTION_FILEREPAIRED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_NOTREPAIRED:
                if ( lpInfected->bInContainer )
                {
                LoadString ( hInstance, IDS_LOG_CONTAINERFILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }
                else
                {
                LoadString ( hInstance, IDS_LOG_FILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }

                LoadString ( hInstance, IDS_LOGACTION_REPAIRFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_INOCULATEDELETED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_NEW,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_DELETED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_INOCULATEDELETE_FAILED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_NEW,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_DELETEFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_INOC_CHANGED_DELETED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_DELETED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_INOC_CHANGED_NOTDELETED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_DELETEFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_DELETED:
                if ( lpInfected->bInContainer )
                {
                LoadString ( hInstance, IDS_LOG_CONTAINERFILEWASINFECTED,
                             szMessage, uMessageBufferSize );
                }
                else
                {
                LoadString ( hInstance, IDS_LOG_FILEWASINFECTED,
                             szMessage, uMessageBufferSize );
                }

                LoadString ( hInstance, IDS_LOGACTION_DELETED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_NOTDELETED:
                if ( lpInfected->bInContainer )
                {
                LoadString ( hInstance, IDS_LOG_CONTAINERFILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }
                else
                {
                LoadString ( hInstance, IDS_LOG_FILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }

                LoadString ( hInstance, IDS_LOGACTION_DELETEFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_BOOTINFECTED:
                LoadString ( hInstance, IDS_LOG_BOOTISINFECTED,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_BOOTISINFECTED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_MASTERBOOTINFECTED:
                LoadString ( hInstance, IDS_LOG_MBRISINFECTED,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_MBRISINFECTED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_BOOTREPAIRED:
                LoadString ( hInstance, IDS_LOG_BOOTWASINFECTED,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_BOOTREPAIRED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
            break;

            case FILESTATUS_MASTERBOOTREPAIRED:
                LoadString ( hInstance, IDS_LOG_MBRWASINFECTED,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_MBRREPAIRED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
            break;

            case FILESTATUS_BOOTNOTREPAIRED:
                LoadString ( hInstance, IDS_LOG_BOOTISINFECTED,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_BOOTNOTREPAIRED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_MASTERBOOTNOTREPAIRED:
                LoadString ( hInstance, IDS_LOG_MBRISINFECTED,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_MBRNOTREPAIRED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_ERROR:
                bAdd = FALSE;
                break;

            case FILESTATUS_ZIPINFECTED:
                LoadString ( hInstance, IDS_LOG_CONTAINERFILEISINFECTED,
                             szMessage, uMessageBufferSize);

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_EXCLUDE_VIRUSDETECTION:
                LoadString ( hInstance, IDS_LOG_FILEEXCLUDE,
                             szMessage, uMessageBufferSize);

                LoadString ( hInstance, IDS_LOGACTION_EXCLUDE_VIRUS,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_KNOWNVIRUS;
                break;

            case FILESTATUS_EXCLUDE_INOCULATION:
                LoadString ( hInstance, IDS_LOG_FILEEXCLUDE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_EXCLUDE_INOC,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_INOCULATED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_NEW,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_INOCULATED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_INOCULATIONFAILED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_NEW,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_INOCULATEFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_INOCULATEREPAIRED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_INOCULATEREPAIRED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_INOCULATEREPAIR_FAILED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize);

                LoadString ( hInstance, IDS_LOGACTION_INOCULATEREPAIRFAILED,
                            szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_REINOCULATED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_REINOCULATED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_REINOCULATIONFAILED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_REINOCULATEFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_NOTINOCULATED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_NEW,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_NOTINOCULATED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;
                break;

            case FILESTATUS_INOCULATEDCHANGED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_NOACTION,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_INOCHANGE;

                break;

            case FILESTATUS_QUARANTINE_INFECTED:
            case FILESTATUS_QUARANTINE_INFECTED_ZIP:
                if ( lpInfected->bInContainer )
                {
                LoadString ( hInstance, IDS_LOG_CONTAINERFILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }
                else
                {
                LoadString ( hInstance, IDS_LOG_FILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }

                LoadString ( hInstance, IDS_LOGACTION_QUARANTINED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_QUARANTINE;
                break;

            case FILESTATUS_QUARANTINE_INFECTED_FAILED:
                if ( lpInfected->bInContainer )
                {
                LoadString ( hInstance, IDS_LOG_CONTAINERFILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }
                else
                {
                LoadString ( hInstance, IDS_LOG_FILEISINFECTED,
                             szMessage, uMessageBufferSize );
                }

                LoadString ( hInstance, IDS_LOGACTION_QUARANTINEFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_MOVE;
                break;

            case FILESTATUS_QUARANTINE_NOTINOCULATED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_NEW,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_QUARANTINED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_QUARANTINE;
                break;

            case FILESTATUS_QUARANTINE_NOTINOCULATED_FAILED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_NEW,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_QUARANTINEFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_MOVE;
                break;

            case FILESTATUS_QUARANTINE_INOCULATEDCHANGED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_QUARANTINED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_QUARANTINE;
                break;

            case FILESTATUS_QUARANTINE_INOCULATEDCHANGED_FAILED:
                LoadString ( hInstance, IDS_LOG_FILEINOC_CHANGE,
                             szMessage, uMessageBufferSize );

                LoadString ( hInstance, IDS_LOGACTION_QUARANTINEFAILED,
                             szAction, uMessageBufferSize );

                byLogType = ALOG_FI_MOVE;
                break;
            }

        if (bAdd == TRUE)
            {
            STRNCPY(szVirusName,
                    (LPSTR)lpInfected->Notes.lpVirName,
                    SIZE_VIRUSNAME - 1);

            szVirusName[ STRLEN ( szVirusName ) ] = EOS;

            if ( lpInfected->bInContainer )
                {
                
                // OK, we have an infection within a container file.
                // lpInfected->pszContainerItem should be pointing to a fully-qualified
                // location of the infected container item.
                // Use BuildContainerDisplayString to construct a more
                // human-readable description of the container item's location.
                LPTSTR lpBuffer = NULL;
#ifdef SYM_WIN32
                if (BuildContainerDisplayString (lpInfected->pszContainerItem, &lpBuffer))
                    {
                        // We only want to log CONTAINERLOG_SIZE number of characters 
                        // to the activity log (since the activity log is 
                        // "fixed structure"-based. CONTAINERLOG_SIZE is defined above at
                        // SYM_MAX_PATH or ~256. If the container is longer than that,
                        // use IDS_LOG_CONTAINERCUTOFF, or "..." at the end.

                        LoadString ( hInstance, IDS_LOG_CONTAINERCUTOFF,
                             szContainerCutoff, sizeof (szContainerCutoff) );

                        // iMaxZipChars will contain the number of characters we will cap off
                        // the zip string at, not counting the '...'. (It will be approx. 253.)
                        UINT uiMaxZipChars = CONTAINERLOG_SIZE - (STRLEN (szContainerCutoff) + 4);
                        // (Added in the +4 to be on the safe side [we are dealing with 
                        //  legacy code here...])
                        
                        _tcsncpy (szZipPath, lpBuffer, uiMaxZipChars );

                        szZipPath[uiMaxZipChars] = EOS;

                        if ( STRLEN ( lpBuffer ) >= uiMaxZipChars )
                        {
                            // We truncated the string early.
                            // Add "..." to the container name:
                            STRCAT ( szZipPath , szContainerCutoff );
                        }

                        // Free the buffer allocated by BuildContainerDisplayString.
                        MemFreePtr (lpBuffer);

                        //lpZipPath is the string   
                        SPRINTF (szFormated, szMessage,
                            (LPSTR)szZipPath,
                            szVirusName);
                    }       
#endif
                }
            else
                if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
                ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED
                ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
                    {
                    SPRINTF ((LPSTR)szFormated,
                             (LPSTR)szMessage,
                             (BYTE)lpInfected->szFullPath[0],
                             (LPSTR)szVirusName);
                    }
                else
                    {
                    SPRINTF ((LPSTR)szFormated,
                             (LPSTR)szMessage,
                             (LPSTR)lpInfected->szFullPath,
                             (LPSTR)szVirusName);
                    }

            lpAddLog->lpMessage = (LPSTR)szFormated;
            lpAddLog->lpVirus   = (LPSTR)szVirusName;
            lpAddLog->lpPath    = lpInfected->szFullPath;

            uStatus = symALogAdd (
                                lpLogFile,
                                byLogType,
                                ALOG_PT_WINDOWS,
                                0,
                                lpAddLog,
                                dwMaxSize
                                );
            }

    return (uStatus);
}


UINT LOCAL GetNumInfected (
    LPINFARR    lpInfectedArray
    )
{
    auto    LPINFARR    lpInfArr;
    auto    UINT        uNum = 0;

    while (lpInfectedArray)
        {
        lpInfArr = lpInfectedArray;

        uNum += lpInfArr->uNumUsed;

        lpInfectedArray = (LPINFARR)lpInfArr->lpNext;
        }

    return (uNum);
}


#ifdef SYM_WIN32
//************************************************************************
//
// BOOL BuildContainerDisplayString (LPTSTR pContainerString, LPTSTR *lpBuffer)
//
// Purpose:
//   Converts the 'ugly' (i.e., internal) string into a human-readable one.
// Returns:
//   TRUE if successfully copied desired sub-string to *ppBuffer.
//   FALSE otherwise.
//
//************************************************************************
// 6/15/99 GMW - Copied from NAVWORK; don't want to create a circular dependency 
// just to pick up nicely-displayed container files in the Activity Log.
BOOL BuildContainerDisplayString (LPTSTR pContainerString, LPTSTR *lpBuffer)
{
    LPTSTR lpTempBuffer1;
    BOOL  bError = TRUE; // Assume an error
    BOOL  rc;
    long  lDepth;
    DWORD dwSize = SYM_MAX_PATH * 2;

    TCHAR tchBeginString[20], tchMidString[50];
    LoadString ( hInstance, IDS_PRINTCONTAINERFILE_BEGIN,
                             tchBeginString, sizeof(tchBeginString) );
    LoadString ( hInstance, IDS_PRINTCONTAINERFILE_MID,
                             tchMidString, sizeof(tchMidString) );


    // Allocate some buffers (these will get dynamically re-sized
    // as needed via the AppendText function).
    *lpBuffer = (LPTSTR)MemAllocPtr (GHND, dwSize);
    lpTempBuffer1 = (LPTSTR)MemAllocPtr (GHND, SYM_MAX_PATH * 2 + 1);
    if (!*lpBuffer || !lpTempBuffer1)
        goto error_fillinfo;

    // Get the name of the infected item (without container info.)
    // This will always be the first part of the fully-qualified
    // location string (thus the 0 input below to return the
    // 1st string in lpFullPath).
    rc = ParseContainerString (pContainerString,
                               lpTempBuffer1,
                               SYM_MAX_PATH * 2,
                               0);
    if (!rc)
        goto error_fillinfo;

    // Build the first part of the output string:
    // "The file d:\xxx.yyy"
    *lpBuffer = AppendText (*lpBuffer,
                            &dwSize,
                            tchBeginString);
    *lpBuffer = AppendText (*lpBuffer,
                            &dwSize,
                            lpTempBuffer1);

    // Now loop through the rest of the container path,
    // appending as we go...
    lDepth = 1;
    while (TRUE)
    {
        rc = ParseContainerString (pContainerString,
                                   lpTempBuffer1,
                                   SYM_MAX_PATH * 2,
                                   lDepth);
        if (!rc)
            break;  // We have reached the end of the string.

        // Build the next piece to add to the output string.
        // " in compressed file xxx.yyy"
        *lpBuffer = AppendText (*lpBuffer,
                                &dwSize,
                                tchMidString);
        *lpBuffer = AppendText (*lpBuffer,
                                &dwSize,
                                lpTempBuffer1);
        lDepth++;
    }

//  Don't append a period at the end of the text in the activity log:
//    *lpBuffer = AppendText (*lpBuffer,
//                            &dwSize,
//                            String_Ptr (IDS_PRINTCONTAINERFILE_END));
    
    bError = FALSE;

error_fillinfo:
    if (lpTempBuffer1)
        MemFreePtr (lpTempBuffer1);

    if (bError)
    {
        if (*lpBuffer)
            MemFreePtr (*lpBuffer);
    }

    return (!bError);
}
#endif

#ifdef SYM_WIN32
//************************************************************************
//
// BOOL ParseContainerString (LPTSTR pPath, LPTSTR pBuffer, long lSize, DWORD dwDepth)
//
// Purpose:
//   Returns the sub-string within the given string (pPath) at the
//   specified depth (dwDepth) in the given buffer (*ppBuffer).
// Returns:
//   TRUE if successfully copied desired sub-string to *ppBuffer.
//   FALSE otherwise.
//
//************************************************************************
// 6/15/99 GMW - Copied from the decomposer; don't want to link NAVXUTIL to the
// decomposer just to pick up nicely-displayed container files in the Activity Log.
BOOL ParseContainerString (LPTSTR pPath, LPTSTR pBuffer, long lSize, DWORD dwDepth)
{
    LPTSTR  pSubString;
    LPTSTR  pToken;
    long    lLength;

    SYM_ASSERT (pPath);
    SYM_ASSERT (pBuffer);

    pSubString = pPath;
    pToken = _tcsstr (pSubString, szRecToken);
    if (pToken)
        lLength = pToken - pSubString;
    else
        lLength = _tcslen (pSubString);
    
    while (dwDepth)
    {
        if (pToken)
        {
            // Found separator token, point to the
            // sub-string immediately following the token.
            pSubString = pToken + _tcslen (szRecToken);

            // Find the next token and compute the length
            // of the sub-string from its position.
            pToken = _tcsstr (pSubString, szRecToken);
            if (pToken)
                lLength = pToken - pSubString;
            else
                lLength = _tcslen (pSubString);
        }
        else
        {
            // No more tokens so stop right here.
            break;
        }

        dwDepth--;
    }
    
    if (dwDepth == 0)
    {
        // We reached the desired depth.  pSubString and lLength
        // are set properly for copying out the sub-string.
        if (lLength > lSize)
            lLength = lSize;

        _tcsncpy (pBuffer, pSubString, lLength);
        _tcscpy (pBuffer + lLength, _T(""));  // Make sure to add NULL terminator
        return (TRUE);
    }

    return (FALSE);
}
#endif

#ifdef SYM_WIN32
//************************************************************************
//
// LPTSTR AppendText (LPTSTR pszBuffer, DWORD *pdwSize, LPTSTR pszText)
//
// Purpose:
//   Concatenates the string at pszText to the string at pszBuffer.
//   Adjusts the size of the input pszBuffer buffer if necessary to
//   accomodate the size of pszText.
// Returns:
//   Pointer to concatenated string.  Pointer may or may not be
//   the same as the input pszBuffer (will be the same if the buffer did not
//   need to be extended, will not otherwise).
//
//************************************************************************
// 6/15/99 GMW - Copied from the decomposer; don't want to link NAVXUTIL to the
// decomposer just to pick up nicely-displayed container files in the Activity Log.
LPTSTR AppendText (LPTSTR pszBuffer, DWORD *pdwSize, LPTSTR pszText)
{
    LPTSTR  pszNewBuffer;
    DWORD   dwBytes;

    // Get size of additional text.
    dwBytes = _tcslen (pszText) * sizeof (TCHAR);

    // Limit size of additional text to APPEND_BUFFER_SIZE characters.
    // This just makes it easier below to make sure we allocate enough
    // additional space...
    SYM_ASSERT (dwBytes < APPEND_BUFFER_SIZE);

    // Compute size of concatenated string.
    dwBytes += (_tcslen (pszBuffer) + sizeof (TCHAR)) * sizeof (TCHAR);

    // Is this new size larger than the size of the input buffer?
    if (dwBytes > *pdwSize)
    {
        // Yes, it is.  So, allocate a new buffer that is large enough to
        // accomodate the additional text.
        dwBytes = (*pdwSize) + APPEND_BUFFER_SIZE;
        pszNewBuffer = (LPTSTR)malloc (dwBytes);
        if (!pszNewBuffer)
        {
            // Failed to allocate additional space.  Just return original
            // unmodified string.
            pszNewBuffer = pszBuffer;
            goto done_append;
        }

        // OK, we now have a buffer that is large enough.  Do the append
        // in the new buffer and update the return parameters.  Also do
        // not forget to free the original buffer.
        _tcscpy (pszNewBuffer, pszBuffer);
        _tcscat (pszNewBuffer, pszText);
        *pdwSize = dwBytes;

        // Free the original buffer.
        free (pszBuffer);
    }
    else
    {
        // OK, the input buffer is already large enough.  Just do the
        // append in the input buffer.
        pszNewBuffer = pszBuffer;
        _tcscat (pszNewBuffer, pszText);
    }

done_append:
    return (pszNewBuffer);
}
#endif
