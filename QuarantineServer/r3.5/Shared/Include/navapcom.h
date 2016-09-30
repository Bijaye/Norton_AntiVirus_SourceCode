// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1994, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/NAVAPCOM.H_v   1.2   14 Jul 1997 23:06:32   JALLEE  $
//
// Description:
//      NAVAPCOM.H - This include file contains the typedefs, definitions,
//      and prototypes for the communication routines between the
//      Norton AntiVirus Auto-Protect VxD and its Agent.
//
// See Also:
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/NAVAPCOM.H_v  $
// 
//    Rev 1.2   14 Jul 1997 23:06:32   JALLEE
// Added NAVAP_95_Wnd_Class.  Removed #ifdef NAV95_CODE.
//
//    Rev 1.1   30 Apr 1997 15:52:16   ksackin
// Now using N30_SIZE_VIRUSNAME vs SIZE_VIRUSNAME because of Avenge work.
//
//    Rev 1.0   06 Feb 1997 20:51:42   RFULLER
// Initial revision
//
//    Rev 1.2   26 Jun 1996 17:38:38   DALLEE
// LuigiPlus port:
// Added #ifdef NAV95_CODE to change name of AP window class back to 95 version.
//
//    Rev 1.1   14 Jun 1996 13:36:22   DSACKIN
// Changed NAVAP_WND_CLASS to be different from the Win 95 version
//
//    Rev 1.0   30 Jan 1996 15:46:50   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:11:10   BARRY
// Initial revision.
//
//    Rev 1.19   11 Sep 1995 21:22:16   BARRY
// No change.
//
//    Rev 1.18   06 Sep 1995 10:29:10   RSTANEV
// Changed NETALERT enumerator to NAVAPNETALERT.  It was conflicting with
// a structure in netalert.h.
//
//    Rev 1.17   29 Aug 1995 18:53:18   RSTANEV
// Added structures for supporting network alerts.
//
//    Rev 1.16   14 Jun 1995 17:36:48   MARKL
// Added the ID_AGENT_COMMAND_DO_LOAD_CONFIG command so the VxD can tell the
// agent to reload the configuration file.
//
//    Rev 1.15   07 Jun 1995 17:16:50   MARKL
// Added the agent's class name.
//
//    Rev 1.14   03 Apr 1995 16:38:34   RSTANEV
// Added NAVAP_ACTIVATING_MESSAGE.
//
//    Rev 1.13   28 Mar 1995 10:35:38   RSTANEV
// Added ID_COMMAND_UNLOAD_VXD_ON_SYSSHUTDOWN.
//
//    Rev 1.12   21 Mar 1995 08:40:00   RSTANEV
// Added ID_COMMAND_NAVAP_LOAD_CONFIG.
//
//    Rev 1.11   27 Dec 1994 07:59:24   RSTANEV
// Added ID_COMMAND_ADD_PROCESS_TO_PEL and ID_COMMAND_DELETE_PROCESS_FROM_PEL.
//
//    Rev 1.10   21 Dec 1994 16:12:40   MARKL
// Cleaned up some async initialization and cleanup with the VxD by
// added ID_COMMAND_START_ASYNC_COMMANDS and ID_COMMAND_END_ASYNC_COMMANDS.
//
//    Rev 1.9   14 Dec 1994 20:26:18   MARKL
// Sizes are now DWORDs instead of WORDs.
//
//***************************************************************************
//
// Descriptions of older revisions of this file can be found in the log file.
//
//***************************************************************************

#ifndef _NAVAPCOM_H
#define _NAVAPCOM_H


#ifdef __cplusplus
extern "C" {
#endif

#include "n30type.h"


//***************************************************************************
// VxD Communication Name Definitions
//***************************************************************************
                                        // Name of the VxD and name of its
                                        // communications channel.
#define NAVAP_VXD_NAME          "NAVAP"
#define NAVAP_VXD_COMM_NAME     "\\\\.\\NAVAP"

#define NAVAP_95_WND_CLASS      "NAVAP Wnd Class"

#define NAVAP_WND_CLASS         "NAVAP NT Wnd Class"


                                        // Message to be sent by the agent
                                        // to NAVW for disabling the VxD.
                                        // This should not be translated.
#define NAVAP_ACTIVATING_MESSAGE "NAV Auto-Protect Activating"



//***************************************************************************
// Command defintions
//***************************************************************************
enum tagVXD_ID_COMMANDS
    {
    ID_COMMAND_NO_COMMAND,
    ID_COMMAND_GET_VXD_VERSION,
    ID_COMMAND_GET_ENABLE_DISABLE,
    ID_COMMAND_GET_CMD_FROM_VXD,
    ID_COMMAND_AGENT_STARTED,
    ID_COMMAND_AGENT_TERMINATING,
    ID_COMMAND_VXD_AUTOLOADED,
    ID_COMMAND_ENABLE_VXD,
    ID_COMMAND_DISABLE_VXD,
    ID_COMMAND_UNLOAD_VXD,
    ID_COMMAND_UNLOAD_VXD_ON_SYSSHUTDOWN,
    ID_COMMAND_SHUTDOWN_IN_PROGRESS,
    ID_COMMAND_SHUTDOWN_ABORTED,
    ID_COMMAND_START_ASYNC_COMMANDS,
    ID_COMMAND_END_ASYNC_COMMANDS,
    ID_COMMAND_ADD_PROCESS_TO_PEL,
    ID_COMMAND_DELETE_PROCESS_FROM_PEL,
    ID_COMMAND_NAVAP_LOAD_CONFIG
    };

#define ID_AGENT_COMMAND_NO_COMMAND ID_COMMAND_NO_COMMAND
enum tagAGENT_ID_COMMANDS
    {
    ID_AGENT_COMMAND_DO_LOG = 1,
    ID_AGENT_COMMAND_DO_INFO,
    ID_AGENT_COMMAND_DO_SHUTDOWN,
    ID_AGENT_COMMAND_DO_UNLOAD,
    ID_AGENT_COMMAND_DO_ALERTS,
    ID_AGENT_COMMAND_DO_SCAN,
    ID_AGENT_COMMAND_DO_LOAD_CONFIG
    };



//***************************************************************************
// Common values, typedefs, etc.
//***************************************************************************
typedef struct
    {
    DWORD       dwSize;
    DWORD       dwAgentCommandID;
    char        szData[1];
    } NAVAP_AGENT_COMMAND, FAR *LPNAVAP_AGENT_COMMAND, *PNAVAP_AGENT_COMMAND;


//***************************************************************************
// Structures & definitions for supporting network alerts.
//***************************************************************************
typedef enum tagNAVAPNETALERT
    {
    NETALERT_KNOWN_FILE_VIRUS,
    NETALERT_KNOWN_BOOTMBR_VIRUS,
    NETALERT_UNKNOWN_FILE_VIRUS,
    NETALERT_FILE_INOCULATION_MISMATCH,
    NETALERT_FILE_NOT_INOCULATED,
    NETALERT_HDFORMAT,
    NETALERT_HDMBRWRITE,
    NETALERT_HDBOOTWRITE,
    NETALERT_FDBOOTWRITE,
    NETALERT_WRITETOFILE,
    NETALERT_FILEROATTRCHANGE,
    } NAVAPNETALERT, *PNAVAPNETALERT;

typedef struct tagNETALERTPACKET
    {
    DWORD    dwEncodedDate;
    DWORD    dwEncodedTime;
    DWORD    eNetAlert;
    DWORD    prNAVAPCfg;
    char     szVirusName[N30_SIZE_VIRUSNAME + 1];
    char     szFileDeviceInTrouble[SYM_MAX_PATH];
    char     szFileCausingTrouble[SYM_MAX_PATH];
    char     szUserID[SYM_MAX_PATH];
    DWORD    bAlertUsers;
    DWORD    bAlertConsole;
    DWORD    bAlertServers;
    DWORD    uUserRecordsCount;
    char     szUserRecords[1];
    } NETALERTPACKET, *PNETALERTPACKET;

#ifdef __cplusplus
}
#endif


#endif
