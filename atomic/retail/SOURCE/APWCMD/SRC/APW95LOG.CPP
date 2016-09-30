// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWCMD/VCS/Apw95log.cpv   1.8   21 May 1998 16:40:10   rpulint  $
//
// Description:
//      Activity log code for Win95 substitute service.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/APWCMD/VCS/Apw95log.cpv  $
// 
//    Rev 1.8   21 May 1998 16:40:10   rpulint
// Added code to get computer name in SvcLogEvent() - so we could report this
// in the activity log
// 
//    Rev 1.7   18 Mar 1998 16:46:42   DALLEE
// Added logging for quarantine actions.
// 
//    Rev 1.6   23 Feb 1998 14:56:14   DALLEE
// Removed file inoculation and unknown virus handling.
// This functionality and the corresponding defines have been removed 
// from the NAVAP driver.
// 
//    Rev 1.5   16 Jul 1997 16:54:04   DALLEE
// SvcGetLogIds():
//   Had accidental assignment instead of equality check causing boot log
//   actions to always say repaired.
//   Exclude action messages needed to switch on AVContext not ALogID.
// 
//    Rev 1.4   25 Jun 1997 17:04:12   DALLEE
// Initialize substitution string pointers to empty string.
// Makes length check easier and also guards against bad format strings.
// 
//    Rev 1.3   11 Jun 1997 15:52:18   JBRENNA
// Create the SvcGetEventLogInfo function. This function takes the pEventInfo
// parameter and returns the user name, virus name, filename, and aux filename.
// 
//    Rev 1.2   05 Jun 1997 20:15:02   DALLEE
// Added date and time processing for activity log entries.
// 
//    Rev 1.1   05 Jun 1997 17:34:32   DALLEE
// oops... g_hCmdInst to g_hInstance.
// 
//    Rev 1.0   05 Jun 1997 17:28:24   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>

#include "drvcomm.h"
#include "apcomm.h"
#include "linksrv.h"
#include "apscomm.h"

#include "apw.h"
#include "apwutil.h"
#include "apw95cmd.h"

#include "actions.h"

#include "symalog.h"
#include "navaplog.h"


//************************************************************************
// External data
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

extern HINSTANCE    g_hInstance;

#ifdef __cplusplus
}
#endif

//************************************************************************
// Win95 AP service activity log code
//************************************************************************

//************************************************************************
// CApw95Cmd::SvcLogEvent()
//
// BOOL CApw95Cmd::SvcLogEvent (
//      NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
// 
// This routine handles all logging for events from NAVAP.
// Called by SvcHandleEventProgress() in response to the 
// NAVAP_COMM_EVENTPROGRESSREPORT request from NAVAP.
//
// Returns:
//      TRUE    on success.
//      FALSE   on error.
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::SvcLogEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
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

    auto    LPCTSTR     lpszSub1;
    auto    LPCTSTR     lpszSub2;

    auto    DWORD       dwDate;
    auto    DWORD       dwTime;

    auto    ALOGADD     rAddLog;

    auto    BOOL        bSuccess;

    TCHAR szSystem[MAX_COMPUTERNAME_LENGTH+2] ;
    DWORD dwSystemLen ;

    SYM_ASSERT( pEventInfo );

    // Initialize locals

    *szMessage     = \
    *szFormat      = \
    *szAction      = \
    *szUser        = \
    *szSystem      = \
    *szVirus       = \
    *szFilename    = \
    *szAppName     = EOS;

    lpszSub1 = \
    lpszSub2 = szEmpty;

    bSuccess = FALSE;

    // Check if we should log this type of event.

    if ( FALSE == SvcShouldLogEvent( pEventInfo ) )
    {
        bSuccess = TRUE;
        goto BailOut;
    }

    // Setup strings.

    // Verify activity log name.

    if ( EOS == *m_szLogFile )
        goto BailOut;

    // Get the necessary information from the pEventInfo structure.

    SvcGetEventLogInfo( pEventInfo,
                        szUser,        sizeof( szUser )        / sizeof( szUser[0] ),
                        szVirus,       sizeof( szVirus )       / sizeof( szVirus[0] ),
                        szFilename,    sizeof( szFilename )    / sizeof( szFilename[0] ),
                        szAuxFilename, sizeof( szAuxFilename ) / sizeof( szAuxFilename[0] ) );

    // Get app name.

    LoadString( g_hInstance,
                IDS_ALOG_APP_NAME,
                szAppName, 
                sizeof( szAppName ) / sizeof( *szAppName ) );

    // Get message IDs.

    if ( FALSE == SvcGetLogIDs( pEventInfo, &byALogID, &uMessageID, &uActionID ) )
    {
        goto BailOut;
    }

    LoadString( g_hInstance,
                uMessageID,
                szFormat, 
                sizeof( szFormat ) / sizeof( *szFormat ) );

    LoadString( g_hInstance,
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

    _stprintf( szMessage, szFormat, 
               ( LPTSTR ) lpszSub1, 
               ( LPTSTR ) lpszSub2 );

    // Get the machine Name
    dwSystemLen = MAX_COMPUTERNAME_LENGTH+1 ;
    szSystem[0] = 0 ;
    GetComputerName(szSystem, &dwSystemLen);

    // Set up ALOGADD struct.

    memset( &rAddLog, 0, sizeof( rAddLog ) );

    rAddLog.SSize       = sizeof( rAddLog );
    rAddLog.lpAction    = szAction;
    rAddLog.lpMessage   = szMessage;
    rAddLog.lpNetAddr   = NULL;
    rAddLog.lpPath      = szFilename;
//    rAddLog.lpServer    = NULL;
    rAddLog.lpServer    = (szSystem[0] == 0) ? NULL : szSystem ;
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

    // Write entry to log.

    bSuccess = ( NOERR == ALogAddEx( m_szLogFile,
                                     byALogID,
                                     ALOG_PT_WINVXD,
                                     0,
                                     &rAddLog,
                                     m_uLogKiloBytes * 1024,
                                     dwDate,
                                     dwTime ) );

BailOut:
    return ( bSuccess );
} // CApw95Cmd::SvcLogEvent()


//************************************************************************
// CApw95Cmd::SvcShouldLogEvent()
//
// BOOL CApw95Cmd::SvcShouldLogEvent (
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

BOOL CApw95Cmd::SvcShouldLogEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
{
    auto    BOOL    bLogEvent;

    switch ( pEventInfo->dwAVContext )
    {
    case AVCONTEXT_SCANFILE:
    case AVCONTEXT_SCANBOOT:
    case AVCONTEXT_SCANMBR:
    case AVCONTEXT_SCANMEMSTARTUP:
        bLogEvent = m_bLogKnown;
        break;

    case AVCONTEXT_HDFORMAT:
    case AVCONTEXT_HDMBRWRITE:
    case AVCONTEXT_HDBOOTWRITE:
    case AVCONTEXT_FDBOOTWRITE:
    case AVCONTEXT_WRITETOFILE:
    case AVCONTEXT_FILEROATTRCHANGE:
        bLogEvent = m_bLogVirusLike;
        break;

    default:
        // Unknown type.

        SYM_ASSERT( FALSE );
        bLogEvent = FALSE;
        break;
    }

    return ( bLogEvent );
} // SvcShouldLogEvent()


//************************************************************************
// CApw95Cmd::SvcGetLogIDs()
//
// BOOL CApw95Cmd::SvcGetLogIDs (
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

BOOL CApw95Cmd::SvcGetLogIDs ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
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
} // CApw95Cmd::SvcGetLogIDs()


//************************************************************************
// CApw95Cmd::SvcGetEventLogInfo()
//
// void CApw95Cmd::SvcGetEventLogInfo ( 
//      NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
//      LPTSTR     szUser,
//      WORD       wMaxUserChars,
//      LPTSTR     szVirus,
//      WORD       wMaxVirusChars,
//      LPTSTR     szFilename,
//      WORD       wMaxFilenameChars,
//      LPTSTR     szAuxFilename,
//      WORD       wMaxAuxFilenameChars )
//
// This routine extracts pEventInfo fields into string variables.
//
//************************************************************************
// 06/11/1997 JBRENNA, created.
//************************************************************************

void CApw95Cmd::SvcGetEventLogInfo ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
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
} // CApw95Cmd::SvcGetEventLogInfo()
