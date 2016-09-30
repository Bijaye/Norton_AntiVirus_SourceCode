// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/APCOMM.H_v   1.13   02 Jun 1998 01:05:36   RStanev  $
//
// Description:
//      This file defines shared structures and constants needed for
//      communication between the NAVAP driver and user-mode applications.
//
//      External communications are based on the exported functions by
//      NAVAP32.DLL.  Internal communications are based on the DriverComm
//      module in SymKrnl.
//
// Contains:
//      NAVAPGetEnabledDisabled             (SYM_WIN32)
//      NAVAPSetEnabledDisabled             (SYM_WIN32)
//      NAVAPProtectProcess                 (SYM_WIN32)
//      NAVAPUnprotectProcess               (SYM_WIN32)
//
//      NAVAPiGetEnabledDisabled            (SYM_WIN32)
//      NAVAPiSetEnabledDisabled            (SYM_WIN32)
//      NAVAPiProtectProcess                (SYM_WIN32)
//      NAVAPiUnprotectProcess              (SYM_WIN32)
//
//      NAVAPiDecodeUserInfoSubPacket()     (SYM_WIN32)
//
//      NAVAPiStart()                       (SYM_WIN32)
//      NAVAPiStop()                        (SYM_WIN32)
//      NAVAPiControlledInit()              (SYM_WIN32)
//      NAVAPiControlledDeInit()            (SYM_WIN32)
//      NAVAPiReloadConfiguration()         (SYM_WIN32)
//      NAVAPiLoadDefinitions()             (SYM_WIN32)
//      NAVAPiUnloadDefinitions()           (SYM_WIN32)
//      NAVAPiGetStatus()                   (SYM_WIN32)
//
// See Also:
//      APCOMM project
//
//***************************************************************************

#ifndef _APCOMM_H_
#define _APCOMM_H_


//---------------------------------------------------------------------------
//
// NAVAP_APCOMM_VERSION is APCOMM's version number.  
// The version number in APCommK must match the version number in APCommU
// for NAVAP to start successfully.
// THE VERSION NUMBER MUST CHANGE EVERY TIME THE APCOMM INTERFACE CHANGES!
//
// Added   04-13-2000 : MMENDON ;; started at 7 just because...
// Changed 04-20-2000 : MMENDON ;; 8: Adding dwVirusCleanable and dwVirusDeletable 
//                                    to NAVAP_EVENTPROGRESSREPORTPACKET
// Changed 07-12-2000 : MMENDON ;; 9: Adding config option for disabling network scanning
// Changed 08-03-2000 : MMENDON ;; A: Terminal Server support
// Changed 10-11-2001 : MMENDON ;; B: Backup and HSM products support
// Changed 12-06-2001 : MMENDON ;; C: Change config for extension exclusions
//---------------------------------------------------------------------------
#define NAVAP_APCOMM_VERSION    0x0000000C

//---------------------------------------------------------------------------
//
// NAVAP_COMMUNICATION_NAME is a constant which should be used by user
// mode applications when creating a DriverComm class.
//
//---------------------------------------------------------------------------
#define NAVAP_COMMUNICATION_NAME    _T("NAVAP")

//---------------------------------------------------------------------------
//
// NAVAPEL_COMMUNICATION_NAME defines the name of the device that manages
// the protect/unprotect counters.
//
//---------------------------------------------------------------------------
#define NAVAPEL_COMMUNICATION_NAME    _T("NAVAPEL")

//---------------------------------------------------------------------------
//
// Define DriverComm service request codes to be sent by user-mode
// applications to NAVAP.
//
//---------------------------------------------------------------------------
#define NAVAP_COMM_GETENABLEDDISABLED   0x01
#define NAVAP_COMM_SETENABLEDDISABLED   0x02
#define NAVAP_COMM_PROTECTPROCESS       0x03
#define NAVAP_COMM_UNPROTECTPROCESS     0x04
#define NAVAP_COMM_RELOADCONFIG         0x05
#define NAVAP_COMM_LOADAVAPI            0x06
#define NAVAP_COMM_UNLOADAVAPI          0x07
#define NAVAP_COMM_GETSTATUS            0x08
#define NAVAP_COMM_CONTROLLEDINIT       0x09
#define NAVAP_COMM_CONTROLLEDDEINIT     0x0A
#define NAVAP_COMM_WIN95AGENTINIT       0x0B
#define NAVAP_COMM_WIN95AGENTDEINIT     0x0C
#define NAVAP_COMM_RELOADCONFIGEX       0x0D
#define NAVAP_COMM_LOADAVAPIEX          0x0E
#define NAVAP_COMM_CONTROLLEDINITEX     0x0F
#define NAVAP_COMM_APCOMMVERSIONCHECK   0x10

//---------------------------------------------------------------------------
//
// Define DriverComm service request codes to be sent by user-mode
// applications to NAVAPEL.
//
//---------------------------------------------------------------------------
#define NAVAPEL_COMM_PROTECTPROCESS       0x01
#define NAVAPEL_COMM_UNPROTECTPROCESS     0x02

//---------------------------------------------------------------------------
//
// Define DriverComm service request codes to be sent by NAVAP to an
// application (its user mode SO).
//
//---------------------------------------------------------------------------
#define NAVAP_COMM_EVENTPROGRESSREPORT  0x01
#define NAVAP_COMM_SCANMOUNTEDDEVICE    0x02
#define NAVAP_COMM_SHUTDOWN             0x03

//---------------------------------------------------------------------------
//
// Define configuration structure to be used with NAVAP_COMM_CONTROLLEDINITEX
// and NAVAP_COMM_RELOADCONFIGEX.  All strings in the structure must be OEM.
//
//---------------------------------------------------------------------------

#include <pshpack1.h>

typedef struct
    {
                                        // Startup options (used only with
                                        // NAVAP_COMM_CONTROLLEDINITEX)
    DWORD       dw_bStartupScanMemory;
    DWORD       dw_bStartupScanUpperMemory;
    DWORD       dw_bStartupRunDOSAutoProtect;

                                        // Global options
    DWORD       dw_bNAVAPCanBeDisabled;

                                        // File scan (known virus detection)
    DWORD       dw_uFileHeuristicLevel;

    DWORD       dw_bScanFileOnExecute;
    DWORD       dw_bScanFileOnOpen;
    DWORD       dw_bScanFileOnCreate;

    DWORD       dw_bScanAllFiles;

    DWORD       adw_auScanFileExeActions[3];
    DWORD       adw_auScanFileMacroActions[3];

    DWORD       dw_uScanFilePrompts;

    DWORD       dw_bDeleteInfectedOnCreate;

                                        // Device scan
    DWORD       dw_bScanDeviceUponAccess;
    DWORD       dw_bScanBootDeviceUponBoot;
    DWORD       dw_bScanNonBootDevicesUponBoot;

    DWORD       adw_auScanDeviceUponAccessActions[3];
    DWORD       adw_auScanDeviceUponBootActions[3];

    DWORD       dw_uScanDevicePrompts;

                                        // Behavior block
    DWORD       adw_auFormatHardDiskActions[3];
    DWORD       adw_auWriteToHDBootRecordsActions[3];
    DWORD       adw_auWriteToFDBootRecordActions[3];
    DWORD       adw_auWriteToProgramFilesActions[3];
    DWORD       adw_auROAttributeClearActions[3];

    DWORD       dw_uBehaviorBlockPrompts;

                                        // File repair options
    DWORD       dw_bBackupBeforeRepair;

                                        // File quarantine options
    DWORD       idx_pszQuarantineLocation;

                                        // File move options
    DWORD       idx_pszMoveLocation;

                                        // File rename options
    DWORD       idx_pszRenameExtension;

                                        // Workstation alerts
    DWORD       dw_bDisplayAlert;
    DWORD       dw_bAudibleAlert;
    DWORD       dw_uAudibleAlertDelay;
    DWORD       dw_uAlertTimeOut;

    DWORD       idx_pszAlertText;

                                        // List of program extensions
    DWORD       idx_pszzProgramExtensionList;

        //******** MMENDON 7/12/00:  Change for new exclusion extensions
        //                           processing							
    DWORD       dw_bExclusionExtensions;
    DWORD       idx_pszzExclusionExtensionList;
       //******** MMENDON 7/12/00:  End change


                                        // Long custom alert
    DWORD       idx_pszLongCustomAlert;

                                        // Spool file
    DWORD       idx_pszSpoolFileLocation;

                                        // Check exclusions before scanning,
                                        // Number of exclusion and long
                                        // exclusion items, as well as arrays
                                        // of exclusion and long exclusion
                                        // items.

   //******** MMENDON 1/27/00:  Prescan exclusion change.                                         
    DWORD       dw_bCheckExcludePrescan;
    DWORD       dw_bExclusions;
   //******** MMENDON 1/27/00:  End prescan exclusion change.
    
    DWORD       dwExclusionItems;
    DWORD       dwExclusionLItems;
    DWORD       idx_arExclusionItems;
    DWORD       idx_arExclusionLItems;

                                        // Other options.  Used only with
                                        // NAVAP_COMM_CONTROLLEDINITEX)
    DWORD       idx_pszAVAPILocation;
    DWORD       idx_pszNAVAPLocation;
    DWORD       idx_pszNAVEXINFLocation;

   //******** MMENDON 7/12/00:  Enable/disable Network scanning change
    DWORD       dw_bScanNetwork;
   //******** MMENDON 7/12/00:  End enable/disable Network scanning change

    // mmendon 10/11/2001 Change for Backup and HSM products
    DWORD dw_BackupHSMFlags;
    // mmendon 10/11/2001 End change


                                        // idx_* must point to data placed
                                        // here.  When this structure is
                                        // passed around, the data must be
                                        // carried with it.
    BYTE        abyData[1];
    } NAVAP_CONFIG, *PNAVAP_CONFIG;

#include <poppack.h>

//---------------------------------------------------------------------------
//
// Define common data structures used when sending packets.
//
// NAVAP_USERINFOSUBPACKET is included in several of the packets which are
// sent by NAVAP to its SO.  When such packet is received, the SO should
// examine the value of bUseAsSIDInsteadOfString, and if it is FALSE, it
// should retrieve the user information as a string from rUserInfo.szUserID.
// The max length in characters of the string will be sizeof(rUserInfo) /
// sizeof(WCHAR) - 1.  When bUseAsSIDInsteadOfString is TRUE, then the user
// should use the LookupAccountSid() function to retrieve user information,
// with the SID located at rUserInfo.Sid.
//
// The function NAVAPiDecodeUserInfoSubPacket() should be used to decode
// NAVAP_USERINFOSUBPACKET structures.
//
// NAVAP_SYSTEMTIME is a user-to-kernel and kernel-to-user mode portable
// structure containing the same data as SYSTEMTIME.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    DWORD bUseAsSIDInsteadOfString;

    // MMENDON 08-03-2000:  Terminal Server support
    DWORD dwTerminalServerSessionID;
    // MMENDON 08-03-2000:  End Terminal Server support

    union
        {
        WCHAR szUserID[1];
        SID   Sid;
        BYTE  fixed_union_size_padding[512 * sizeof(WCHAR)];
        } rUserInfo;

    } NAVAP_USERINFOSUBPACKET, *PNAVAP_USERINFOSUBPACKET;

typedef struct
    {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
    } NAVAP_SYSTEMTIME, *PNAVAP_SYSTEMTIME;

#include <poppack.h>

//---------------------------------------------------------------------------
//
// Define NAVAP status packet.  This packet is returned by the NAVAP driver
// when an application has requested to get information on driver's status
// by requesting service NAVAP_COMM_GETSTATUS.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    DWORD dwErrorStatus;

    DWORD dwSize;

    NAVAP_SYSTEMTIME rSystemTimeAtStartup;

    DWORD dwDriverImageUnloadable;

    DWORD dwInitializedModules;

    DWORD dwFilesScannedSinceStartup;
    DWORD dwInfectedFilesFoundSinceStartup;
    NAVAP_SYSTEMTIME rSystemTimeWhenLastInfectedFileFound;
    NAVAP_USERINFOSUBPACKET rUserOfLastInfectedFileFound;

    DWORD dwLastFileScannedIndex;
    DWORD dwLastInfectedFileFoundIndex;
    DWORD dwLastVirusFoundIndex;

    WCHAR szStringArray[1];

    } NAVAP_STATUSPACKET, *PNAVAP_STATUSPACKET;

#include <poppack.h>

#define NAVAP_INITIALIZEDMODULES_AVAPI 0x00000001

//---------------------------------------------------------------------------
//
// Define NAVAP event progress report packet.  This packet is used by the
// driver to notify its SO of the actions that it is taking while processing
// events.  The service code for the notification is
// NAVAP_COMM_EVENTPROGRESSREPORT.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    NAVAP_SYSTEMTIME rSystemTime;

    NAVAP_USERINFOSUBPACKET rUser;

    DWORD dwSequenceRecognizer;
    DWORD dwLastSequencePacket;

    DWORD dwAVContext;
    DWORD dwAction;
    DWORD dwActionCompleted;

    DWORD dwMacroVirus;

    DWORD dwVirusID;
    
    // MMENDON 04-24-2000:  Fix for STS defect #333309
    //                      User mode needs to know if the virus is cleanable/deletable
    DWORD dwVirusCleanable;
    DWORD dwVirusDeletable;
    // MMENDON 04-24-2000:  End fix for STS defect #333309

    DWORD dwSpoolFilePathIndex;
    DWORD dwVirusNameIndex;
    DWORD dwFileDeviceInTroubleIndex;

    union
        {
        DWORD dwFileCausingTroubleIndex;
        DWORD dwMovedRenamedFileIndex;
        };

    WCHAR szStringArray[1];

    } NAVAP_EVENTPROGRESSREPORTPACKET, *PNAVAP_EVENTPROGRESSREPORTPACKET;

#include <poppack.h>

//---------------------------------------------------------------------------
//
// Define device mount notification packet.  This packet is used by
// NAVAP_COMM_SCANMOUNTEDDEVICE.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    NAVAP_SYSTEMTIME rSystemTime;

    NAVAP_USERINFOSUBPACKET rUser;

    WCHAR szDevice[3];

    } NAVAP_SCANMOUNTEDDEVICEPACKET, *PNAVAP_SCANMOUNTEDDEVICEPACKET;

#include <poppack.h>

#if defined(SYM_WIN32)

//---------------------------------------------------------------------------
//
// Prototype APCOMM Win32 functions exported by NAVAP32.DLL.
//
//---------------------------------------------------------------------------
#define NAVAPGetEnabledDisabled     gretz1
#define NAVAPSetEnabledDisabled     gretz2
#define NAVAPProtectProcess         gretz3
#define NAVAPUnprotectProcess       gretz4

#ifdef _M_ALPHA
    #define szNAVAPGetEnabledDisabled   "gretz1"
    #define szNAVAPSetEnabledDisabled   "gretz2"
    #define szNAVAPProtectProcess       "gretz3"
    #define szNAVAPUnprotectProcess     "gretz4"
#else
    #define szNAVAPGetEnabledDisabled   "_gretz1@4"
    #define szNAVAPSetEnabledDisabled   "_gretz2@4"
    #define szNAVAPProtectProcess       "_gretz3@0"
    #define szNAVAPUnprotectProcess     "_gretz4@0"
#endif

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

BOOL SYM_EXPORT WINAPI NAVAPGetEnabledDisabled (
    PDWORD pdwStatus
    );

BOOL SYM_EXPORT WINAPI NAVAPSetEnabledDisabled (
    DWORD dwStatus
    );

BOOL SYM_EXPORT WINAPI NAVAPProtectProcess (
    VOID
    );

BOOL SYM_EXPORT WINAPI NAVAPUnprotectProcess (
    VOID
    );

#if defined(__cplusplus)
}
#endif  // __cplusplus

//---------------------------------------------------------------------------
//
// Prototype APCOMM Win32 functions internal to NAV modules.
//
//---------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

BOOL NAVAPiGetEnabledDisabled (
    PDWORD pdwStatus
    );

BOOL NAVAPiSetEnabledDisabled (
    DWORD dwStatus
    );

BOOL NAVAPiProtectProcess (
    VOID
    );

BOOL NAVAPiUnprotectProcess (
    VOID
    );

BOOL NAVAPiDecodeUserInfoSubPacket (
    PNAVAP_USERINFOSUBPACKET prPacket,
    PTSTR                    pszUser,
    PDWORD                   pcchUser,
    PTSTR                    pszDomain,
    PDWORD                   pcchDomain
    );

#if defined(__cplusplus)
}
#endif  // __cplusplus

//---------------------------------------------------------------------------
//
// Prototype APCOMM Win32 functions internal to NAV (INTC code base only)
// modules.
//
//---------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef BOOL ( WINAPI * NT_SERVICE_START ) ( char * pszName, char * pszLocation );
typedef BOOL ( WINAPI * NT_SERVICE_STOP ) ( char * pszName );

BOOL NAVAPiInit (
    NT_SERVICE_START pfnServiceStart,
    NT_SERVICE_STOP pfnServiceStop
    );

void NAVAPiDeInit();

HANDLE NAVAPiStart (
    PSTR pszLocation
    );

BOOL NAVAPiStop (
    HANDLE hDriver
    );

BOOL NAVAPiWaitForPacket (
    PVOID pvData,
    PDWORD pdwData
    );

BOOL NAVAPiReleaseWaiters();

BOOL NAVAPiBlockWaiters();

BOOL NAVAPiControlledInit (
    HANDLE hDriver,
    PNAVAP_CONFIG prConfig
    );

BOOL NAVAPiControlledDeInit (
    HANDLE hDriver
    );

BOOL NAVAPiReloadConfiguration (
    HANDLE hDriver,
    PNAVAP_CONFIG prConfig
    );

BOOL NAVAPiLoadDefinitions (
    HANDLE hDriver,
    PTSTR pszLocation
    );

BOOL NAVAPiUnloadDefinitions (
    HANDLE hDriver
    );

BOOL NAVAPiGetStatus (
    HANDLE hDriver,
    PNAVAP_STATUSPACKET prPacket
    );

#if defined(__cplusplus)
}
#endif  // __cplusplus

#endif // #if defined(SYM_WIN32)

#endif // #ifndef _APCOMM_H_
