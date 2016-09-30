// Copyright 1994 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/navaplog.cpv   1.10   21 May 1998 16:43:34   rpulint  $
//
// Description:
//      This file contains the routines that process commands for the Agent
//      of the Norton AntiVirus Auto-Protect.  The Agent gets told to do
//      various tasks for the Auto-Protect VxD via the
//      ID_COMMAND_GET_CMD_FROM_VXD command.
//
// Contains:
//      NavApLogSpoolFileProcess()      <<<=== EXPORTED
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NAVXUTIL/VCS/navaplog.cpv  $
// 
//    Rev 1.10   21 May 1998 16:43:34   rpulint
// Added Server name in ActivityLog: Modified NavAPDoCommandActivityLog()
// 
//    Rev 1.9   18 Mar 1998 16:36:40   DALLEE
// Removed remaining unknown and inoculation log code.
// Added quarantine logging.
// 
//    Rev 1.8   24 Feb 1998 16:54:04   MKEATIN
// Removed UNKNOWN_VIRUS and INOCULATION functionality.
// 
//    Rev 1.7   21 Aug 1997 14:59:12   DALLEE
// Last round of fixes to make old stuff compile for DOS again.
// 
//    Rev 1.6   21 Aug 1997 13:07:04   DALLEE
// Put back old log code alongside new for Alert Service compatibility.
// 
//    Rev 1.5   21 Aug 1997 00:03:46   DALLEE
// Re-worked NavApLogSpoolFileProcess() for the new format spool file
// written by the agent.
// 
//    Rev 1.4   21 May 1997 16:28:40   TIVANOV
// change the AnsiToOem to NavAnsi....
// 
//    Rev 1.3   20 May 1997 10:28:54   TIVANOV
// fix the a build problem with DX and DVX
// 
//    Rev 1.2   19 May 1997 14:58:02   TIVANOV
// new function to support the quarantine log file
// 
//    Rev 1.1   09 May 1997 16:59:50   TIVANOV
// new case switch for the new ALOG_FI_MOVE - supporting the quarantine
//
//    Rev 1.0   06 Feb 1997 21:05:02   RFULLER
// Initial revision
//
//    Rev 1.2   09 Sep 1996 17:05:00   DJesch
// Fixed STS #63756: Name of infected file was being translated to OEM before
// being added to the activity log.  It needs to stay ANSI.
//
//    Rev 1.1   12 Jul 1996 15:41:40   pvenkat
// #60683: Fixed Auto-Protect Service GPFs after detecting MBR virus.
// Instead of pointing to the new string, we were trying to SPRINTF to
// NULL string.
//
//    Rev 1.0   30 Jan 1996 15:56:14   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:14   BARRY
// Initial revision.
//
//    Rev 1.15   24 Oct 1995 19:49:26   JWORDEN
// Separate processing for MBR
//
//    Rev 1.14   21 Jul 1995 19:09:06   MARKL
// 2nd time I've fixed this!!!  It does no good calling BuildLogFilename() if
// you are not going to pass the results to the log function.
//
//    Rev 1.13   09 Jul 1995 17:08:28   MARKL
// Fixed defect #37815
//
//    Rev 1.12   23 Jun 1995 17:13:10   MARKL
// Now calls BuildLogFileName() to normalize the name of the logfile
// correctly.
//
//    Rev 1.11   04 May 1995 19:55:22   RSTANEV
// Now logging the user name from the AP.
//
//    Rev 1.10   17 Apr 1995 15:47:12   MFALLEN
// sym_max_path+1
//
//    Rev 1.9   08 Mar 1995 16:09:00   DALLEE
// 1. Removed include "utilstr.str" -- utilstr.str is now compiled directly
//    for DOS only (shouldn't have been included previously for VXD).
// 2. Moved DOS only LoadString() cover to DOSMISC.CPP
//
//    Rev 1.8   24 Jan 1995 12:28:12   DALLEE
// DOS_STRING structure now uses 'LPSTR lpszString' instead of
// 'char szString[256]' to save memory -- NAVBOOT's DGROUP exploded.
//
//    Rev 1.7   23 Jan 1995 13:26:58   DALLEE
// Use SPRINTF() macro instead of wsprintf() -- unresolved for DOS.
//
//    Rev 1.6   20 Jan 1995 19:09:48   MFALLEN
// All the DOS and VXD strings are accessed using the LoadString() functions
// that emulates it's Windows counterpart. All the string ID's are stored in the
// stbexts.h file that identical for all the platforms. DOS and VXD strings are
// contained in the UTILSTR.STR file.
//
//    Rev 1.5   28 Dec 1994 14:12:00   MARKL
// Due to PVCS conflicts, NAVAPLOG.STR was renamed to NAVAPLOG.TXT.
//
//    Rev 1.4   28 Dec 1994 14:09:40   MARKL
// For DOS, the strings are in NAVAPLOG.STR and the function LoadString()
// is emulated...
//
//    Rev 1.3   28 Dec 1994 10:48:04   MARKL
// Now calls symALogAddEx() since the date/time must be specified since the
// date/time of the event may be significantly different than the current
// date and time.
//
//    Rev 1.2   28 Dec 1994 09:01:48   MARKL
// NAVAP_LOG items of type UINT were changed to type WORD.
//
//    Rev 1.1   27 Dec 1994 19:26:48   MARKL
// Added routines to allow the spooling of the VxD's activity log
// entries.
//
//    Rev 1.0   27 Dec 1994 16:18:38   MARKL
// Moved the activity log processing code that the Auto-Protect Agent
// did for the VxD to NAVXUTIL.  The Agent STILL references these routines.
//
//    Rev 1.1   22 Dec 1994 13:29:02   MARKL
// Fixed some string names.
//
//    Rev 1.0   20 Dec 1994 18:35:10   MARKL
// Initial revision.
//***************************************************************************


//***************************************************************************
// Include Files
//***************************************************************************
#include "platform.h"
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "file.h"                       // For file i/o

#ifdef SYM_WIN32
    #include "drvcomm.h"
    #include "apcomm.h"
    #include "actions.h"
#endif

#include "options.h"                    // For NAVOPTIONS
#include "symalog.h"                    // For activity log code
#include "navaplog.h"                   // Log file IDs
#include "navutil.h"
#include "stbexts.h"                    // String IDs

#ifdef SYM_DOS
  #include <stdio.h>                    // for sprintf()
#endif


//***************************************************************************
// Global Definitions and Variables
//***************************************************************************

#define         ACTIVITY_LOG_BUFF_SIZE  1024
#define         ACTIVITY_APP_BUFF_SIZE  80

#define  MAX_MESSAGE   1024
#define  MAX_ACTION    1024
#define  MAX_USER      80
#define  MAX_VIRUS     256
#define  MAX_APP_NAME  80

extern HINSTANCE    hInstance;

BOOL            bActLogKnown                    = FALSE;
BOOL            bActLogVirusLike                = FALSE;
BOOL            bActLogStartEnd                 = FALSE;
WORD            uActLogKiloBytes                = FALSE;
char            szActLogFile [SYM_MAX_PATH+1]   = {0};

#define         CFG_GET_ACT_LOG_KNOWN           (bActLogKnown)
#define         CFG_SET_ACT_LOG_KNOWN(b)        (bActLogKnown = (b))
#define         CFG_GET_ACT_LOG_VIRUS_LIKE      (bActLogVirusLike)
#define         CFG_SET_ACT_LOG_VIRUS_LIKE(b)   (bActLogVirusLike = (b))
#define         CFG_GET_ACT_LOG_START_END       (bActLogStartEnd)
#define         CFG_SET_ACT_LOG_START_END(b)    (bActLogStartEnd = (b))
#define         CFG_GET_ACT_LOG_KILOBYTES       (uActLogKiloBytes)
#define         CFG_SET_ACT_LOG_KILOBYTES(x)    (uActLogKiloBytes = (x))
#define         CFG_GET_ACT_LOG_FILE(f)         (STRCPY ((f), szActLogFile))
#define         CFG_SET_ACT_LOG_FILE(f)         (STRCPY (szActLogFile, (f)))


//***************************************************************************
// Local Prototypes
//***************************************************************************

#ifdef SYM_WIN32

BOOL PASCAL NavApLogEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo );
BOOL PASCAL NavApShouldLogEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo );
BOOL PASCAL NavApGetLogIDs ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
                             LPBYTE   lpbyALogID,
                             LPUINT   lpuMessageID,
                             LPUINT   lpuActionID );
void PASCAL NavApGetEventLogInfo ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
                            LPTSTR  szUser,         WORD    wMaxUserChars,
                            LPTSTR  szVirus,        WORD    wMaxVirusChars,
                            LPTSTR  szFilename,     WORD    wMaxFilenameChars,
                            LPTSTR  szAuxFilename,  WORD    wMaxAuxFilenameChars );

// Functions for old-style AP logging. Still in use by Alert service.

BOOL PASCAL NavApLogIsOkToWrite (       // TRUE if okay to write log entry
    BYTE        byRecType);             // [IN] Activity log record type

UINT PASCAL NavApLogSubstituteStrings ( // Formats message string
    LPSTR       pszMessage,             // [OUT] Message to write to log
    WORD        cbMessage,              // [IN] Size of buffer in bytes
    WORD        wVirMessageID,          // [IN] Virus message fmt string ID
    LPSTR       pszVirus,               // [IN] Name of virus found
    LPSTR       pszFile,                // [IN] File (or disk) with virus
    LPSTR       pszProgram);            // [IN] Program doing bad things

#endif


#ifndef SYM_VXD

//***************************************************************************
// NavApLogSpoolFileProcess()
//
// Description:
//      This routine reads in all of the entries in the spool file and
//      updates the activity log.  Once this is done the spool file is
//      deleted.
// File format is:  4 bytes     DWORD   dwPacketLength;
//                  ? bytes     NAVAP_EVENTPROGRESSREPORTPACKET rPacket;
//                  ...
//
// Parameters:
//      lpszSpoolFile                   - [IN] Path of spool file to write to
//
// Return Value:
//      NONE
//***************************************************************************
// 12/16/1994 MARKL Function Created.
//***************************************************************************
VOID SYM_EXPORT WINAPI NavApLogSpoolFileProcess ( // Spool file -> ActLog
    LPSTR       lpszSpoolFile)          // [IN] Spool file to update log with
{
#ifdef SYM_WIN32
    // Crude rule of thumb for maximum event progress packet size in bytes.
    
    #define MAX_PACKET_SIZE     ( 20 * 1024 )

    // Locals

    auto    HANDLE  hSpoolFile;

    auto    NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo;
    auto    DWORD   dwPacketSize;
    auto    DWORD   dwBytesRead;
    auto    DWORD   dwMaxStringIndex;

    // Initialize locals.

    hSpoolFile  = INVALID_HANDLE_VALUE;
    pEventInfo  = NULL;

    // Open spool file if present.

    hSpoolFile = CreateFile( lpszSpoolFile,
                             GENERIC_READ,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL );

    if ( INVALID_HANDLE_VALUE == hSpoolFile )
    {
        goto BailOut;
    }

    // Process event packets.

    for (;;)
    {
        // Read packet length.

        if ( ( FALSE == ReadFile( hSpoolFile,
                                  &dwPacketSize,
                                  sizeof( dwPacketSize ),
                                  &dwBytesRead,
                                  NULL ) ) ||
             ( sizeof( dwPacketSize ) != dwBytesRead ) )
        {
            // No more packets.
            
            break;
        }

        // Rough validation of packet size.

        if ( ( sizeof( NAVAP_EVENTPROGRESSREPORTPACKET ) > dwPacketSize ) ||
             ( MAX_PACKET_SIZE < dwPacketSize ) )
        {
            // Assertion is only to signal bad spool file.
            // This is still a valid error condition which must be handled.

            SYM_ASSERT( FALSE );
            goto BailOut;
        }

        // Allocate packet buffer and read packet.

        // Allocating packet size plus sizeof( WCHAR ) of zeroed memory to
        // ensure free-form string data at end of packet is terminated
        // with WCHAR EOS.

        pEventInfo = ( NAVAP_EVENTPROGRESSREPORTPACKET * )
            GlobalAllocPtr( GHND, dwPacketSize + sizeof( WCHAR ) );

        if ( NULL == pEventInfo )
        {
            goto BailOut;
        }

        if ( ( FALSE == ReadFile( hSpoolFile,
                                  pEventInfo,
                                  dwPacketSize,
                                  &dwBytesRead,
                                  NULL ) ) ||
             ( dwPacketSize != dwBytesRead ) )
        {
            // Assertion is only to signal bad spool file.
            // This is still a valid error condition which must be handled.

            SYM_ASSERT( FALSE );
            goto BailOut;
        }

        // Validate pointers to string data at end of packet.
        // 1. Maximum index is end of packet minus beginning of string data
        //    minus one for zero-based.
        // 2. Validation of dwMovedRenamedFileIndex is not required.
        //    Part of a union with dwFileCausingTroubleIndex.


        dwMaxStringIndex = ( WCHAR * )( ( LPBYTE )pEventInfo + dwPacketSize ) -
                           pEventInfo->szStringArray - 1;

        if ( ( dwMaxStringIndex < pEventInfo->dwConfigurationPathIndex ) ||
             ( dwMaxStringIndex < pEventInfo->dwVirusNameIndex ) ||
             ( dwMaxStringIndex < pEventInfo->dwFileDeviceInTroubleIndex ) ||
             ( dwMaxStringIndex < pEventInfo->dwFileCausingTroubleIndex ) )
        {
            // Assertion is only to signal bad spool file.
            // This is still a valid error condition which must be handled.

            SYM_ASSERT( FALSE );
            goto BailOut;
        }

        // Process packet.

        NavApLogEvent( pEventInfo );

        GlobalFreePtr( pEventInfo );
        pEventInfo = NULL;
    }

BailOut:
    if ( INVALID_HANDLE_VALUE != hSpoolFile )
    {
        // Close and delete file.
        // Whether success or error, we've done as much processing with it
        // as possible.

        CloseHandle( hSpoolFile );
        DeleteFile( lpszSpoolFile );
    }

    if ( NULL != pEventInfo )
        GlobalFreePtr( pEventInfo );
#endif // SYM_WIN32
} // NavApLogSpoolFileProcess()


#ifdef SYM_WIN32
//***************************************************************************
// NavApLogEvent()
//
// Description:
//      Write log entry for event in AP's NAVAP_EVENTPROGRESSREPORTPACKET.
//
// Parameters:
//      NAVAP_EVENTPROGRESSREPORTPACKET pEventInfo
//
// Return Value:
//      TRUE    on success.
//      FALSE   otherwise.
//***************************************************************************
// 08/20/1997 DALLEE, created.
//***************************************************************************

BOOL PASCAL NavApLogEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
{
    //&? Sloppy w/ stack usage.

    auto    BYTE        byALogID;
    auto    UINT        uMessageID;
    auto    UINT        uActionID;

    auto    TCHAR       szMessage [ MAX_MESSAGE ];
    auto    TCHAR       szFormat [ MAX_MESSAGE ];
    auto    TCHAR       szAction [ MAX_ACTION ];
    auto    TCHAR       szUser [ MAX_USER ];
    auto    TCHAR       szVirus [ MAX_VIRUS ];
    auto    TCHAR       szFilename [ MAX_PATH ];
    auto    TCHAR       szAuxFilename [ MAX_PATH ];
    auto    TCHAR       szAppName [ MAX_APP_NAME ];
    const   TCHAR       szEmpty[] = _T("");

    auto    TCHAR       szActLog[ SYM_MAX_PATH+1 ];
    auto    TCHAR       szFullPathActLog[ SYM_MAX_PATH+1 ];

    auto    LPCTSTR     lpszSub1;
    auto    LPCTSTR     lpszSub2;

    auto    DWORD       dwDate;
    auto    DWORD       dwTime;

    auto    ALOGADD     rAddLog;

    auto    BOOL        bSuccess;

    SYM_ASSERT( pEventInfo );

    // Initialize locals

    *szFormat      = \
    *szAction      = \
    *szMessage     = \
    *szAction      = \
    *szUser        = \
    *szVirus       = \
    *szFilename    = \
    *szAppName     = EOS;

    lpszSub1 = \
    lpszSub2 = szEmpty;

    bSuccess = FALSE;

    // Check if we should log this type of event.

    if ( FALSE == NavApShouldLogEvent( pEventInfo ) )
    {
        bSuccess = TRUE;
        goto BailOut;
    }

    // Setup strings.

    // Get the necessary information from the pEventInfo structure.

    NavApGetEventLogInfo( pEventInfo,
                          szUser,        sizeof( szUser )        / sizeof( szUser[0] ),
                          szVirus,       sizeof( szVirus )       / sizeof( szVirus[0] ),
                          szFilename,    sizeof( szFilename )    / sizeof( szFilename[0] ),
                          szAuxFilename, sizeof( szAuxFilename ) / sizeof( szAuxFilename[0] ) );

    // Get app name.

    LoadString( hInstance,
                IDS_ALOG_APP_NAME,
                szAppName, 
                sizeof( szAppName ) / sizeof( *szAppName ) );

    // Get message IDs.

    if ( FALSE == NavApGetLogIDs( pEventInfo, &byALogID, &uMessageID, &uActionID ) )
    {
        goto BailOut;
    }

    LoadString( hInstance,
                uMessageID,
                szFormat, 
                sizeof( szFormat ) / sizeof( *szFormat ) );

    LoadString( hInstance,
                uActionID,
                szAction, 
                sizeof( szAction ) / sizeof( *szAction ) );

    // Format message.

    switch ( pEventInfo->dwAVContext )
    {
    case AVCONTEXT_SCANFILE:
    case AVCONTEXT_SCANBOOT:
    case AVCONTEXT_SCANMBR:
        lpszSub1 = szFilename;
        lpszSub2 = szVirus;
        break;

    case AVCONTEXT_HDFORMAT:
    case AVCONTEXT_HDMBRWRITE:
    case AVCONTEXT_HDBOOTWRITE:
    case AVCONTEXT_FDBOOTWRITE:
    case AVCONTEXT_WRITETOFILE:
    case AVCONTEXT_FILEROATTRCHANGE:
        lpszSub1 = szAuxFilename;
        lpszSub2 = szFilename;
        break;

    case AVCONTEXT_SCANMEMSTARTUP:
        lpszSub1 = szVirus;
        break;

    default:
        // Unknown type.

        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    if ( _tcslen( szFormat ) + _tcslen( lpszSub1 ) + _tcslen( lpszSub2 ) 
         >= MAX_MESSAGE )
    {
        goto BailOut;
    }

    SPRINTF( szMessage, szFormat,
             ( LPTSTR ) lpszSub1,
             ( LPTSTR ) lpszSub2 );

    // Set up ALOGADD struct.

    memset( &rAddLog, 0, sizeof( rAddLog ) );

    rAddLog.SSize       = sizeof( rAddLog );
    rAddLog.lpAction    = szAction;
    rAddLog.lpMessage   = szMessage;
    rAddLog.lpNetAddr   = NULL;
    rAddLog.lpPath      = szFilename;
    rAddLog.lpServer    = NULL;
    rAddLog.lpUser      = szUser;
    rAddLog.lpVirus     = szVirus;
    rAddLog.lpOS        = szAppName;
    rAddLog.wDirection  = 0;

    // Encode date.

    dwDate = max( 1980, pEventInfo->rSystemTime.wYear );
    dwDate -= 1980;

    dwDate <<= 4;
    dwDate += ( pEventInfo->rSystemTime.wMonth & 0x0F );

    dwDate <<= 5;
    dwDate += ( pEventInfo->rSystemTime.wDay & 0x1F );

    // Encode time.

    dwTime = ( pEventInfo->rSystemTime.wHour % 24 );

    dwTime <<= 6;
    dwTime += ( pEventInfo->rSystemTime.wMinute % 60 );

    dwTime <<= 5;
    dwTime += ( ( pEventInfo->rSystemTime.wSecond % 60 ) >> 1 );

    // Resolve our log file name.

    CFG_GET_ACT_LOG_FILE( szActLog );
    BuildLogFilename( szFullPathActLog, szActLog );

    // Write entry to log.

    bSuccess = ( NOERR == ALogAddEx( szFullPathActLog,
                                     byALogID,
                                     ALOG_PT_WINVXD,
                                     0,
                                     &rAddLog,
                                     CFG_GET_ACT_LOG_KILOBYTES * 1024,
                                     dwDate,
                                     dwTime ) );

BailOut:
    return ( bSuccess );
} // NavApLogEvent()


//************************************************************************
// NavApShouldLogEvent()
//
// BOOL PASCAL NavApShouldLogEvent (
//      NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
// 
// This routine determines whether the event should be logged based on
// type of activity and current options.
//
// Returns:
//      TRUE    if event should be logged.
//      FALSE   otherwise.
//************************************************************************
// 06/04/1997 DALLEE, created.
//************************************************************************

BOOL PASCAL NavApShouldLogEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
{
    auto    BOOL        bLogEvent;
    auto    NAVOPTIONS  navOptions = { 0 };

    ReadNavBin( &navOptions );

    CFG_SET_ACT_LOG_KNOWN( navOptions.activity.bKnown );
    CFG_SET_ACT_LOG_VIRUS_LIKE( navOptions.activity.bVirusLike );
    CFG_SET_ACT_LOG_START_END( navOptions.activity.bStartEnd );
    CFG_SET_ACT_LOG_KILOBYTES( navOptions.activity.uKiloBytes );
    CFG_SET_ACT_LOG_FILE( navOptions.activity.szLogFile );

    switch ( pEventInfo->dwAVContext )
    {
    case AVCONTEXT_SCANFILE:
    case AVCONTEXT_SCANBOOT:
    case AVCONTEXT_SCANMBR:
    case AVCONTEXT_SCANMEMSTARTUP:
        bLogEvent = CFG_GET_ACT_LOG_KNOWN;
        break;

    case AVCONTEXT_HDFORMAT:
    case AVCONTEXT_HDMBRWRITE:
    case AVCONTEXT_HDBOOTWRITE:
    case AVCONTEXT_FDBOOTWRITE:
    case AVCONTEXT_WRITETOFILE:
    case AVCONTEXT_FILEROATTRCHANGE:
        bLogEvent = CFG_GET_ACT_LOG_VIRUS_LIKE;
        break;

    default:
        // Unknown type.

        SYM_ASSERT( FALSE );
        bLogEvent = FALSE;
        break;
    }

    return ( bLogEvent );
} // NavApShouldLogEvent()


//************************************************************************
// NavApGetLogIDs()
//
// BOOL PASCAL NavApGetLogIDs (
//      NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
//      LPBYTE  lpbyALogID,
//      LPUINT  lpuMessageID,
//      LPUINT  lpuActionID )
// 
// This routine retrieves the string ID's to use for an event's activity log
// message and action strings and also the ALOG record type.
//
// Returns:
//      TRUE    on success.
//      FALSE   on error.
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

BOOL PASCAL NavApGetLogIDs ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
                             LPBYTE   lpbyALogID,
                             LPUINT   lpuMessageID,
                             LPUINT   lpuActionID )
{
    auto    BYTE    byALogID;
    auto    UINT    uMessageID;
    auto    UINT    uActionID;

    auto    BOOL    bSuccess;

    // Initialize locals.

    bSuccess = TRUE;

    // Get message string ID's.
    // Find out what the original event was. May need to check for 
    // successful action taken since message may change --
    // i.e. "File XXX is infected..." to "File XXX was infected..."

    switch ( pEventInfo->dwAVContext )
    {
    case AVCONTEXT_SCANFILE:
        byALogID = ALOG_FI_KNOWNVIRUS;

        if ( ( pEventInfo->dwActionCompleted ) &&
             ( ( pEventInfo->dwAction == AVACTION_REPAIR ) ||
               ( pEventInfo->dwAction == AVACTION_DELETE ) ||
               ( pEventInfo->dwAction == AVACTION_EXCLUDE )||
               ( pEventInfo->dwAction == AVACTION_QUARANTINE ) ) )
        {
            uMessageID = IDS_VIR_MSG_WAS_INFECTED;
        }
        else
        {
            uMessageID = IDS_VIR_MSG_IS_INFECTED;
        }
        break;

    case AVCONTEXT_SCANBOOT:
        byALogID = ALOG_FI_KNOWNVIRUS;

        if ( ( pEventInfo->dwActionCompleted ) &&
             ( pEventInfo->dwAction == AVACTION_REPAIR ) )
        {
            uMessageID = IDS_VIR_MSG_BOOT_WAS_INFECTED;
        }
        else 
        {
            uMessageID = IDS_VIR_MSG_BOOT_IS_INFECTED;
        }
        break;

    case AVCONTEXT_SCANMBR:
        byALogID = ALOG_FI_KNOWNVIRUS;

        if ( ( pEventInfo->dwActionCompleted ) &&
             ( pEventInfo->dwAction == AVACTION_REPAIR ) )
        {
            uMessageID = IDS_VIR_MSG_MBR_WAS_INFECTED;
        }
        else 
        {
            uMessageID = IDS_VIR_MSG_MBR_IS_INFECTED;
        }
        break;

    case AVCONTEXT_SCANMEMSTARTUP:
        byALogID = ALOG_FI_KNOWNVIRUS;
        uMessageID = IDS_VIR_MSG_VIRUS_IN_MEMORY;
        break;

    case AVCONTEXT_HDFORMAT:
        byALogID = ALOG_FI_VIRUSLIKE;
        uMessageID = IDS_VIR_MSG_LOW_FORMAT;
        break;

    case AVCONTEXT_HDMBRWRITE:
        byALogID = ALOG_FI_VIRUSLIKE;
        uMessageID = IDS_VIR_MSG_WRITE_MBR;
        break;

    case AVCONTEXT_HDBOOTWRITE:
    case AVCONTEXT_FDBOOTWRITE:
        byALogID = ALOG_FI_VIRUSLIKE;
        uMessageID = IDS_VIR_MSG_WRITE_BOOT;
        break;

    case AVCONTEXT_WRITETOFILE:
        byALogID = ALOG_FI_VIRUSLIKE;
        uMessageID = IDS_VIR_MSG_WRITEPROG;
        break;

    case AVCONTEXT_FILEROATTRCHANGE:
        byALogID = ALOG_FI_VIRUSLIKE;
        uMessageID = IDS_VIR_MSG_ATTRIB_CHANGED;
        break;

    default:
        // Assert if new types get added and we're not handling them.

        SYM_ASSERT( FALSE );
        bSuccess = FALSE;
        goto BailOut;
        break;
    }

    // Get action string ID's.
    // Action strings will depend on original event type, and action
    // taken.

    switch ( pEventInfo->dwAction )
    {
    case AVACTION_STOP:
        if ( ALOG_FI_VIRUSLIKE == byALogID ) 
        {
            if ( AVCONTEXT_FILEROATTRCHANGE == pEventInfo->dwAVContext )
            {
                uActionID = IDS_VIR_ACT_ATTRIB_NOT_ALLOWED;
            }
            else 
            {
                uActionID = IDS_VIR_ACT_ACTION_NOT_ALLOWED;
            }
        }
        else
        {
            uActionID = IDS_VIR_ACT_ACCESS_DENIED;
        }
        break;
                                       
    case AVACTION_CONTINUE:
        if ( ALOG_FI_VIRUSLIKE == byALogID ) 
        {
            if ( AVCONTEXT_FILEROATTRCHANGE == pEventInfo->dwAVContext )
            {
                uActionID = IDS_VIR_ACT_ATTRIB_ALLOWED;
            }
            else
            {
                uActionID = IDS_VIR_ACT_ACTION_ALLOWED;
            }
        }
        else
        {
            uActionID = IDS_VIR_ACT_NONE;
        }
        break;

    case AVACTION_REPAIR:
        switch ( pEventInfo->dwAVContext )
        {
        case AVCONTEXT_SCANMBR:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_MBR_REPAIRED
                                                      : IDS_VIR_ACT_MBR_NOT_REPAIRED;
            break;

        case AVCONTEXT_SCANBOOT:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_BOOTREC_REPAIRED
                                                      : IDS_VIR_ACT_BOOTREC_NOT_REPAIRED;
            break;

        default:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_REPAIRED
                                                      : IDS_VIR_ACT_NOT_REPAIRED;
        }
        break;

    case AVACTION_DELETE:
        SYM_ASSERT( AVCONTEXT_SCANFILE == pEventInfo->dwAVContext );

        uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_DELETED
                                                  : IDS_VIR_ACT_NOT_DELETED;
        break;

    case AVACTION_QUARANTINE:
        SYM_ASSERT( AVCONTEXT_SCANFILE == pEventInfo->dwAVContext );
        
        uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_QUARANTINED
                                                  : IDS_VIR_ACT_NOT_QUARANTINED;
        break;

    case AVACTION_EXCLUDE:
        switch ( pEventInfo->dwAVContext )
        {
        case AVCONTEXT_SCANFILE:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_KNOWN_EXCL
                                                      : IDS_VIR_ACT_KNOWN_NOT_EXCL;
            break;

        case AVCONTEXT_FILEROATTRCHANGE:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_ATTRIB_EXCL
                                                      : IDS_VIR_ACT_NOT_ATTRIB_EXCL;
            break;

        case AVCONTEXT_WRITETOFILE:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_WRITEPROG_EXCL
                                                      : IDS_VIR_ACT_WRITEPROG_NOT_EXCL;
            break;

        case AVCONTEXT_HDBOOTWRITE:
        case AVCONTEXT_FDBOOTWRITE:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_WRITEBOOT_EXCL
                                                      : IDS_VIR_ACT_WRITEBOOT_NOT_EXCL;
            break;

        case AVCONTEXT_HDMBRWRITE:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_WRITEMBR_EXCL
                                                      : IDS_VIR_ACT_WRITEMBR_NOT_EXCL;
            break;

        case AVCONTEXT_HDFORMAT:
            uActionID = pEventInfo->dwActionCompleted ? IDS_VIR_ACT_LOW_FMT_EXCL
                                                      : IDS_VIR_ACT_LOW_FMT_NOT_EXCL;
            break;

        case AVCONTEXT_SCANBOOT:
        case AVCONTEXT_SCANMBR:
        case AVCONTEXT_SCANMEMSTARTUP:
        default:
            // Not possible to exclude for any other type of action.

            SYM_ASSERT( FALSE );
            bSuccess = FALSE;
            goto BailOut;
            break;
        }
        break;  // End case AVACTION_EXCLUDE:

    case AVACTION_MOVE:
    case AVACTION_RENAME:
        // Leave assert here till I get log message text.

        SYM_ASSERT( FALSE );
        bSuccess = FALSE;
        goto BailOut;
        break;

    case AVACTION_SHUTDOWN:
        uActionID = IDS_VIR_ACT_COMPUTER_SHUTDOWN;
        break;

    default:
        // Assert if new actions are added and we're not handling them.

        SYM_ASSERT( FALSE );
        bSuccess = FALSE;
        goto BailOut;
        break;
    }

BailOut:
    if ( bSuccess )
    {
        *lpbyALogID   = byALogID;
        *lpuMessageID = uMessageID;
        *lpuActionID  = uActionID;
    }

    return ( bSuccess );
} // NavApGetLogIDs()


//************************************************************************
// This routine copies certain pEventInfo fields into string variables.
//
//************************************************************************
// 06/11/1997 JBRENNA, created.
//************************************************************************
void PASCAL NavApGetEventLogInfo ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
                                   LPTSTR     szUser,
                                   WORD       wMaxUserChars,
                                   LPTSTR     szVirus,
                                   WORD       wMaxVirusChars,
                                   LPTSTR     szFilename,
                                   WORD       wMaxFilenameChars,
                                   LPTSTR     szAuxFilename,
                                   WORD       wMaxAuxFilenameChars )
{
    SYM_ASSERT (pEventInfo);
    SYM_ASSERT (szUser);
    SYM_ASSERT (szVirus);
    SYM_ASSERT (szFilename);
    SYM_ASSERT (szAuxFilename);

    *szUser        = \
    *szVirus       = \
    *szFilename    = \
    *szAuxFilename = EOS;

    // Get user name.

    if ( FALSE == pEventInfo->rUser.bUseAsSIDInsteadOfString )
    {
#ifndef _UNICODE    
        WideCharToMultiByte( CP_ACP, 0,
                             pEventInfo->rUser.rUserInfo.szUserID,
                             -1,
                             szUser,
                             wMaxUserChars,
                             NULL, NULL );
#else
        _tcscpy( szUser, pEventInfo->rUser.rUserInfo.szUserID );
#endif
    }

    // Get virus name.

#ifndef _UNICODE
    WideCharToMultiByte( CP_ACP, 0,
                         pEventInfo->szStringArray + pEventInfo->dwVirusNameIndex,
                         -1,
                         szVirus,
                         wMaxVirusChars,
                         NULL, NULL );
#else
    _tcscpy( szVirus, pEventInfo->szStringArray + pEventInfo->dwVirusNameIndex );
#endif

    // Get filename.

#ifndef _UNICODE
    WideCharToMultiByte( CP_ACP, 0,
                         pEventInfo->szStringArray + pEventInfo->dwFileDeviceInTroubleIndex,
                         -1,
                         szFilename,
                         wMaxFilenameChars,
                         NULL, NULL );
#else
    _tcscpy( szFilename, pEventInfo->szStringArray + pEventInfo->dwFileDeviceInTroubleIndex );
#endif

    // Get aux filename. Can be either file modifying primary file, or
    // new name of primary file if moved or renamed.

#ifndef _UNICODE
    WideCharToMultiByte( CP_ACP, 0,
                         pEventInfo->szStringArray + pEventInfo->dwFileCausingTroubleIndex,
                         -1,
                         szAuxFilename,
                         wMaxAuxFilenameChars,
                         NULL, NULL );
#else
    _tcscpy( szAuxFilename, pEventInfo->szStringArray + pEventInfo->dwFileCausingTroubleIndex );
#endif
} // NavApGetEventLogInfo()
#endif // SYM_WIN32


#ifdef SYM_WIN32

//************************************************************************
// Old-style NAVAP log code follows.
// Still in-use by Alert Service. Should rewrite this -- scheme is
// fairly specific to first rev of the W95 Auto-Protect.
//************************************************************************

//***************************************************************************
// NavApDoCommandActivityLog()
//
// Description:
//      The VxD has sent the Agent the ID_AGENT_COMMAND_DO_LOG
//      command.  This routine proceeds to update the activity log as the
//      agent desires.
//
// Parameters:
//      lpNavApLog                      - [IN] Log entry to write
//
// Return Value:
//      UINT    - NOERR for it worked; ERR for it didn't
//
// See Also:
//      NavApLogIsOkToWrite()
//
//***************************************************************************
// 12/16/1994 MARKL Function Created.
//***************************************************************************
UINT SYM_EXPORT WINAPI NavApDoCommandActivityLog ( // Write act log for VxD
    LPNAVAP_LOG lpNavApLog)             // [IN] Log entry to write
{
    char        szMessage[ACTIVITY_LOG_BUFF_SIZE];
    char        szAction[ACTIVITY_LOG_BUFF_SIZE];
    char        szAppName[ACTIVITY_APP_BUFF_SIZE];
    char        szActLog[SYM_MAX_PATH+1];
    char        szFullPathActLog[SYM_MAX_PATH+1];
    ALOGADD     ActLogAdd       = {0};
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;      // bogus!
    hInstance;                          // Get rid of uninitialized warning.
#endif

    SYM_ASSERT (lpNavApLog->wSize);

                                        // Check to see if it is okay to
                                        // write this activity log entry.
                                        // If not, simply exit.
    if (!NavApLogIsOkToWrite (lpNavApLog->byRecType))
        {
        return (NOERR);
        }

#if defined(SYM_WIN) && !defined(SYM_WIN32)
    if (*lpNavApLog->szVirus)           // Convert OEM Strings to ANSI
        {
        OemToAnsi (lpNavApLog->szVirus, lpNavApLog->szVirus);
        }
    if (*lpNavApLog->szFile)
        {
        OemToAnsi (lpNavApLog->szFile, lpNavApLog->szFile);
        }
    if (*lpNavApLog->szProgram)
        {
        OemToAnsi (lpNavApLog->szProgram, lpNavApLog->szProgram);
        }
#endif

                                        // Build our strings to contain
                                        // the appropriate text by doing
                                        // the correct substitutions.
    LoadString (hInstance, lpNavApLog->wVirActionID,
                szAction, sizeof (szAction));
    LoadString (hInstance, IDS_ALOG_APP_NAME,
                szAppName, sizeof (szAppName));

                                        // Resolve our log file name.
    CFG_GET_ACT_LOG_FILE (szActLog);
    BuildLogFilename (szFullPathActLog, szActLog);

                                        // Perform the correct string
                                        // substitution based on the message.
    NavApLogSubstituteStrings (szMessage,
                               sizeof (szMessage),
                               lpNavApLog->wVirMessageID,
                               lpNavApLog->szVirus,
                               lpNavApLog->szFile,
                               lpNavApLog->szProgram);

                                        // Set up the ActLogAdd structure
                                        // appropriately.
    ActLogAdd.SSize             = sizeof (ActLogAdd);
    ActLogAdd.lpAction          = szAction;
    ActLogAdd.lpMessage         = szMessage;
    ActLogAdd.lpNetAddr         = NULL;
    ActLogAdd.lpPath            = lpNavApLog->szFile;
//    ActLogAdd.lpServer          = NULL;
    ActLogAdd.lpServer          = *lpNavApLog->szSystem ? lpNavApLog->szSystem :
                                                        NULL;
    ActLogAdd.lpUser            = *lpNavApLog->szUser ? lpNavApLog->szUser :
                                                        NULL;
    ActLogAdd.lpVirus           = lpNavApLog->szVirus;
    ActLogAdd.lpOS              = szAppName;
    ActLogAdd.wDirection        = 0;

                                        // Add the entry to the log
    return (symALogAddEx (szFullPathActLog,
                          lpNavApLog->byRecType,
                          ALOG_PT_WINVXD,
                          0,
                          &ActLogAdd,
                          CFG_GET_ACT_LOG_KILOBYTES * 1024,
                          lpNavApLog->dwEncodedDate,
                          lpNavApLog->dwEncodedTime));
} // NavApDoCommandActivityLog()


//***************************************************************************
// NavApLogIsOkToWrite()
//
// Description:
//      This routine checks to see if this type of activity log item
//      should be written based on the configuration.
//
// Parameters:
//      byRecType                       - [IN] Activity log record type
//
// Return Value:
//      BOOL    - TRUE if okay to write this entry; FALSE otherwise.
//
//***************************************************************************
// 12/16/1994 MARKL Function Created.
//***************************************************************************
BOOL PASCAL NavApLogIsOkToWrite (       // TRUE if okay to write log entry
    BYTE        byRecType)              // [IN] Activity log record type
{
    BOOL        bLogIt          = TRUE;
    NAVOPTIONS  navOptions      = {0};

    ReadNavBin (&navOptions);
    CFG_SET_ACT_LOG_KNOWN (navOptions.activity.bKnown);
    CFG_SET_ACT_LOG_VIRUS_LIKE (navOptions.activity.bVirusLike);
    CFG_SET_ACT_LOG_START_END (navOptions.activity.bStartEnd);
    CFG_SET_ACT_LOG_KILOBYTES (navOptions.activity.uKiloBytes);
    CFG_SET_ACT_LOG_FILE (navOptions.activity.szLogFile);

                                        // Check to see if we should even
                                        // bother logging this event based
                                        // on the configuration of the
                                        // activity log.
    switch (byRecType)
        {
        case ALOG_FI_KNOWNVIRUS:
        case ALOG_FI_MOVE:
            if (!CFG_GET_ACT_LOG_KNOWN)
                {
                bLogIt = FALSE;
                }
            break;

        case ALOG_FI_VIRUSLIKE:
            if (!CFG_GET_ACT_LOG_VIRUS_LIKE)
                {
                bLogIt = FALSE;
                }
            break;

        case ALOG_FI_LOADUNLOAD:
            if (!CFG_GET_ACT_LOG_START_END)
                {
                bLogIt = FALSE;
                }
            break;

        case ALOG_FI_STARTEND:
            if (!CFG_GET_ACT_LOG_START_END)
                {
                bLogIt = FALSE;
                }
            break;

                                        // We do all of these regardless
                                        // of the configuration settings.
        case ALOG_FI_ERROR:
        case ALOG_FI_STATUS:
        case ALOG_FI_FROMWRK:
        case ALOG_FI_REGCHANGE:
        case ALOG_FI_CONFIGCHANGE:
            break;

                                        // What the ?
        default:
            SYM_ASSERT (FALSE);
            bLogIt = FALSE;
        }

                                        // Return to caller letting him
                                        // know if it is okay to write this
                                        // entry.
    return (bLogIt);
} // NavApLogIsOkToWrite()


//***************************************************************************
// NavApLogSubstituteStrings()
//
// Description:
//      This routine takes the various bits of information that need to go
//      in to the activity log and formats the lpszMessage buffer to
//      contain a human readable string.
//
// Parameters:
//      pszMessage                      - [OUT] Message to write to log
//      cbMessage                       - [IN] Size of buffer in bytes
//      wVirMessageID                   - [IN] Virus message fmt string ID
//      pszVirus                        - [IN] Name of virus found
//      pszFile                         - [IN] File (or disk) with virus
//      pszProgram                      - [IN] Program doing bad things
//
// Return Value:
//      UINT                            - Number of bytes in returned
//                                        message string
//
//***************************************************************************
// 12/19/1994 MARKL Function Created.
//***************************************************************************
UINT PASCAL NavApLogSubstituteStrings ( // Formats message string
    LPSTR       pszMessage,             // [OUT] Message to write to log
    WORD        cbMessage,              // [IN] Size of buffer in bytes
    WORD        wVirMessageID,          // [IN] Virus message fmt string ID
    LPSTR       pszVirus,               // [IN] Name of virus found
    LPSTR       pszFile,                // [IN] File (or disk) with virus
    LPSTR       pszProgram)             // [IN] Program doing bad things
{
    char        szMessageFmt[ACTIVITY_LOG_BUFF_SIZE + SYM_MAX_PATH * 2];
    BYTE        cbTest;
    LPSTR       pszSubstitute1  = NULL;
    LPSTR       pszSubstitute2  = NULL;
    extern      HINSTANCE       hInstance;

    SYM_ASSERT (pszMessage);
    SYM_ASSERT (cbMessage);
    SYM_ASSERT (wVirMessageID);

                                        // Get the format of the message
                                        // string.
    *pszMessage = EOS;
    LoadString (hInstance, wVirMessageID,
                szMessageFmt, sizeof (szMessageFmt));

                                        // Do the required string
                                        // substitutions based on the
                                        // message ID.
    switch (wVirMessageID)
        {
                                        // Messages where the string is
                                        // FILE only.
        case IDS_VIR_MSG_BOOT_INOC_CHANGE:
        case IDS_VIR_MSG_MBR_INOC_CHANGE:
            pszSubstitute1 = pszFile;
            break;

                                        // Messages where the string is
                                        // FILE then VIRUS.
        case IDS_VIR_MSG_IS_INFECTED:
        case IDS_VIR_MSG_WAS_INFECTED:
        case IDS_VIR_MSG_BOOT_IS_INFECTED:
        case IDS_VIR_MSG_BOOT_WAS_INFECTED:
            pszSubstitute1 = pszFile;
            pszSubstitute2 = pszVirus;
            break;

                                        // Messages where the string is
                                        // DRIVE # then VIRUS.
        case IDS_VIR_MSG_MBR_IS_INFECTED:
        case IDS_VIR_MSG_MBR_WAS_INFECTED:
			pszSubstitute1 = pszFile;
            pszSubstitute2 = pszVirus;
            break;

                                        // Messages where the string is
                                        // PROGRAM then FILE.
        case IDS_VIR_MSG_ATTRIB_CHANGED:
        case IDS_VIR_MSG_WRITEPROG:
        case IDS_VIR_MSG_WRITE_BOOT:
        case IDS_VIR_MSG_WRITE_MBR:
        case IDS_VIR_MSG_LOW_FORMAT:
            pszSubstitute1 = pszProgram;
            pszSubstitute2 = pszFile;
            break;

                                        // Messages where the string is
                                        // VIRUS only.
        case IDS_VIR_MSG_VIRUS_IN_MEMORY:
            pszSubstitute1 = pszVirus;
            break;

                                        // What's this?
        default:
            SYM_ASSERT (FALSE);
        }

                                        // Do the required string
                                        // substitutions, checking to
                                        // see if it will all fit.  We
                                        // don't bother to count %s
                                        // characters in this calculation...
    cbTest = STRLEN (szMessageFmt);
    if (pszSubstitute1)
        {
        cbTest += STRLEN (pszSubstitute1);
        }
    if (pszSubstitute2)
        {
        cbTest += STRLEN (pszSubstitute2);
        }
    if (cbTest < cbMessage)
        {
        SPRINTF (pszMessage, szMessageFmt,
                  (LPSTR)pszSubstitute1, (LPSTR)pszSubstitute2);
        }
    else
        {
        SYM_ASSERT (FALSE);
        }

                                        // Return to caller
    return (STRLEN (pszMessage));
} // NavApLogSubstituteStrings()


#endif


#endif // !SYM_VXD


