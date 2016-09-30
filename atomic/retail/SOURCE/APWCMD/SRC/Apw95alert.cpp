// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWCMD/VCS/apw95alert.cpv   1.7   01 Apr 1998 18:16:34   DALLEE  $
//
// Description:
//      Alert notification code for Win95 substitute service.
//
// Contains:
//      CApw95Cmd:: SvcAlertEvent()
//      CApw95Cmd:: SvcAlertShouldSend()
//
// See Also:
//************************************************************************
// $Log:   S:/APWCMD/VCS/apw95alert.cpv  $
// 
//    Rev 1.7   01 Apr 1998 18:16:34   DALLEE
// Okay, I admit it. I didn't compile after that last change.
// Fixed my typo this time.
// 
//    Rev 1.6   01 Apr 1998 18:14:38   DALLEE
// Added quarantine action alerting.
// 
//    Rev 1.5   23 Feb 1998 14:54:58   DALLEE
// Removed file inoculation and unknown virus handling.
// This functionality and the corresponding defines have been removed 
// from the NAVAP driver.
// 
//    Rev 1.4   17 Feb 1998 13:42:52   DALLEE
// Added call to CVirusAlert::SetActionSuccess() before sending alert.
// Was always picking up default value of failure for all actions.
// Also changed local dwAction to dwAlertAction to lessen confusion with
// AP's event info member .dwAction.
// 
//    Rev 1.3   18 Nov 1997 18:15:18   CEATON
// Fixed problem where alert was not logged in the event log by NAV NT due to missing domain name.
// 
//    Rev 1.2   25 Aug 1997 13:10:52   DSACKING
// I, Dan, called CreatePacketMachineType() to set source platform in 
// network alerts.
// 
//    Rev 1.1   23 Jun 1997 20:20:46   DALLEE
// 1. Format CVirusAlert and call ProcessCVirusAlert() to send both NLM and
//    NT Forward alerts.
// 2. Removed old-style NLM alerting.
//
//    Rev 1.0   11 Jun 1997 15:52:04   JBRENNA
// Initial revision.
//************************************************************************

#include "platform.h"
#include <windowsx.h>
#include "servenfy.h"
#include "drvcomm.h"
#include "apcomm.h"
#include "linksrv.h"
#include "apscomm.h"
#include "actions.h"

#include "apw.h"
#include "apwutil.h"
#include "apw95cmd.h"
#include "apw95res.h"

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
// Win95 AP service alerting code
//************************************************************************

//************************************************************************
// This routine handles all alerting for events from NAVAP.
// Called by SvcHandleEventProgress() in response to the
// NAVAP_COMM_EVENTPROGRESSREPORT request from NAVAP.
//
// Returns:
//      TRUE    on success.
//      FALSE   on error.
//************************************************************************
// 06/10/1997 JBRENNA, created.
//************************************************************************

BOOL CApw95Cmd::SvcAlertEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
{
    auto    HKEY        hKey;
    auto    TCHAR       szUser [ MAX_USER ];
    auto    TCHAR       szVirus [ MAX_VIRUS ];
    auto    TCHAR       szFilename [ MAX_PATH ];
    auto    TCHAR       szAuxFilename [ MAX_PATH ];
    auto    TCHAR       szObject [ MAX_PATH ];
    auto    TCHAR       szComputer [ MAX_COMPUTERNAME_LENGTH + 1 ];
    auto    TCHAR       szDomainName [ MAX_PATH ];
    auto    DWORD       dwComputerLen;
    auto    DWORD       dwAlertAction;
    auto    CVirusAlert cVirusAlert;

    auto    BOOL        bSuccess;

    SYM_ASSERT( pEventInfo );

    // Initialize locals.

    *szUser         = \
    *szVirus        = \
    *szFilename     = \
    *szAuxFilename  = \
    *szObject       = \
    *szComputer     = EOS;

    dwComputerLen   = sizeof( szComputer ) / sizeof( szComputer[0] );

    bSuccess        = FALSE;

    // Check if we should log this type of event.

    if ( FALSE == SvcAlertShouldSend( pEventInfo ) )
    {
        bSuccess = TRUE;
        goto BailOut;
    }

    // Get the necessary information from the pEventInfo structure.

    SvcGetEventLogInfo( pEventInfo,
                        szUser,        sizeof( szUser )        / sizeof( szUser[0] ),
                        szVirus,       sizeof( szVirus )       / sizeof( szVirus[0] ),
                        szFilename,    sizeof( szFilename )    / sizeof( szFilename[0] ),
                        szAuxFilename, sizeof( szAuxFilename ) / sizeof( szAuxFilename[0] ) );

    GetComputerName( szComputer, &dwComputerLen );

    lstrcpy( szDomainName,"(domain N/A)" );

    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                "System\\CurrentControlSet\\Services\\MSNP32\\NetworkProvider",
                0,
                0,
                &hKey ) == ERROR_SUCCESS ) {
        auto    LONG        lErrorCode;
        auto    DWORD       dwDomainLen = MAX_PATH;

        lErrorCode = RegQueryValueEx( hKey,
                              "AuthenticatingAgent",
                              NULL,
                              NULL,
                              ( LPBYTE ) szDomainName,
                              &dwDomainLen );

        if ( lErrorCode != ERROR_SUCCESS || *szDomainName == 0 )
            lstrcpy( szDomainName,"" );

        RegCloseKey( hKey );
        }
                   
    cVirusAlert.SetUserName( szUser );
    cVirusAlert.SetVirusName( szVirus );
    cVirusAlert.SetSystemName( szComputer );
    cVirusAlert.SetDomainName( szDomainName );

    // Tell alert service who sent alert.

    cVirusAlert.SetPacketSource( PACKET_SOURCE_FORWARD | PACKET_SOURCE_NAVAP );
    cVirusAlert.CreatePacketMachineType();

    // Create a unique ID for this alert and stamp current time.

    cVirusAlert.CreateUniqueID();
    cVirusAlert.StampTime();

    // Get type of action and related problem item text.

    switch ( pEventInfo->dwAVContext )
    {
    case AVCONTEXT_SCANFILE:
        cVirusAlert.SetAlertType( FILE_KNOWN_VIRUS );
        _tcscpy( szObject, szFilename );
        break;

    case AVCONTEXT_SCANBOOT:
        cVirusAlert.SetAlertType( BOOT_RECORD_KNOWN_VIRUS );
        _tcscpy( szObject, szFilename );
        break;

    case AVCONTEXT_SCANMEMSTARTUP:
        //&? Update alert type when specific Memory type of alert is added.

        cVirusAlert.SetAlertType( FILE_KNOWN_VIRUS );
        LoadString( g_hInstance,
                    IDS_MEMORY_ALERT,
                    szObject,
                    sizeof( szObject ) / sizeof( szObject[0] ) );
        break;

    case AVCONTEXT_SCANMBR:
        // NAVAP doesn't scan MBR's on W95. We shouldn't get this event.

        SYM_ASSERT( FALSE );
        break;

        // cVirusAlert.SetAlertType( MBR_KNOWN_VIRUS );
        // tcscpy( szObject, szFilename );
        // break;

    default:
        // Unsupported type.

        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    cVirusAlert.SetObjectName( szObject );

    // Get action taken.

    switch ( pEventInfo->dwAction )
    {
    case AVACTION_STOP:
        dwAlertAction = ACTION_DENY_ACCESS;
        break;

    case AVACTION_CONTINUE:
        dwAlertAction = ACTION_NONE;
        break;

    case AVACTION_REPAIR:
        dwAlertAction = ACTION_REPAIR;
        break;

    case AVACTION_QUARANTINE:
        dwAlertAction = ACTION_QUARANTINE;
        break;

    case AVACTION_DELETE:
        dwAlertAction = ACTION_DELETE;
        break;

    case AVACTION_EXCLUDE:
        dwAlertAction = ACTION_EXCLUDE;
        break;

    case AVACTION_MOVE:
        dwAlertAction = ACTION_MOVE;
        break;

    case AVACTION_RENAME:
        dwAlertAction = ACTION_RENAME;
        break;

    case AVACTION_SHUTDOWN:
        dwAlertAction = ACTION_SHUTDOWN;
        break;

    default:
        // Bad action type.

        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    cVirusAlert.SetAction( dwAlertAction );
    cVirusAlert.SetActionSuccess( pEventInfo->dwActionCompleted ? TRUE : FALSE );

    // Send alert.

    bSuccess = ProcessCVirusAlert( &cVirusAlert );

BailOut:
    return ( bSuccess );
} // CApw95Cmd::SvcAlertEvent()


//************************************************************************
// This routine determines when an alert should actually occur.
//
// Returns:
//      TRUE    - alert.
//      FALSE   - do not alert.
//************************************************************************
// 06/10/1997 JBRENNA, created.
//************************************************************************

BOOL CApw95Cmd::SvcAlertShouldSend ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
{
// Always return TRUE.
// Old code used to track and query alert options.
// Now calling ProcessCVirusAlert() which reads options and decides what
// to send and where to send it.
// However, doesn't seem quite legit to read/query options there.

    return ( TRUE );

//    auto  BOOL bAlertEvent;
//
//    switch (pEventInfo->dwAVContext)
//    {
//    case AVCONTEXT_SCANFILE:
//    case AVCONTEXT_SCANBOOT:
//    case AVCONTEXT_SCANMBR:
//    case AVCONTEXT_SCANMEMSTARTUP:
//        bAlertEvent = m_bAlertKnown;
//        break;
//
//    case AVCONTEXT_UNKNOWNVIRUS:
//        bAlertEvent = m_bAlertUnknown;
//        break;
//
//    case AVCONTEXT_INOCFILE:
//        bAlertEvent = m_bAlertInocChange;
//        break;
//
//    case AVCONTEXT_HDFORMAT:
//    case AVCONTEXT_HDMBRWRITE:
//    case AVCONTEXT_HDBOOTWRITE:
//    case AVCONTEXT_FDBOOTWRITE:
//    case AVCONTEXT_WRITETOFILE:
//    case AVCONTEXT_FILEROATTRCHANGE:
//        bAlertEvent = m_bAlertVirusLike;
//        break;
//
//    default:
//        // Unknown type.
//
//        SYM_ASSERT( FALSE );
//        bAlertEvent = FALSE;
//        break;
//    }
//
//
//    return ( bAlertEvent );
} // CApw95Cmd::SvcAlertShouldSend()



