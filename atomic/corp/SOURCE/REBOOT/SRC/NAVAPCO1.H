// Description:
//      NAVAPCOM.H - This include file contains the typedefs, definitions,
//      and prototypes for the communication routines between the
//      Norton AntiVirus Auto-Protect VxD and its Agent.
//

#ifndef _NAVAPCOM_H
#define _NAVAPCOM_H

// #include "navoem.h"

#ifdef __cplusplus
extern "C" {
#endif


//***************************************************************************
// VxD Communication Name Definitions
//***************************************************************************
                                        // Name of the VxD and name of its
                                        // communications channel.
#define NAVAP_VXD_NAME          "NAVAP"
#define NAVAP_VXD_COMM_NAME     "\\\\.\\NAVAP"
#define NAVAP_WND_CLASS         "NAVAP Wnd Class"

                                        // Message to be sent by the agent
                                        // to NAVW for disabling the VxD.
                                        // This should not be translated.
#define NAVAP_ACTIVATING_MESSAGE "NAV Auto-Protect Activating"

#define VIRLIB_VIRUS_NAME_SIZE   255
#define SYM_MAX_PATH			 256

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
    unsigned long dwSize;
    unsigned long dwAgentCommandID;
    char          szData[1];
    } NAVAP_AGENT_COMMAND, *LPNAVAP_AGENT_COMMAND, *PNAVAP_AGENT_COMMAND;


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
    unsigned long	dwEncodedDate;
    unsigned long	dwEncodedTime;
    unsigned long	eNetAlert;
    unsigned long	prNAVAPCfg;
    char			szVirusName[VIRLIB_VIRUS_NAME_SIZE + 1];
    char			szFileDeviceInTrouble[SYM_MAX_PATH];
    char			szFileCausingTrouble[SYM_MAX_PATH];
    char			szUserID[SYM_MAX_PATH];
    unsigned long	bAlertUsers;
	unsigned long	bAlertConsole;
    unsigned long	bAlertServers;
    unsigned long	uUserRecordsCount;
    char			szUserRecords[1];
    } NETALERTPACKET, *PNETALERTPACKET;

#ifdef __cplusplus
}
#endif


#endif
