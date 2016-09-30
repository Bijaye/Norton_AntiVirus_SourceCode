// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/apcfg.h_v   1.10   29 Sep 1998 18:23:22   RStanev  $
//
// Description:
//      This file exports NAVAP's configuration structures, variables,
//      and functions.
//
// Contains:
//      ReloadNAVAPConfiguration()
//      LoadNAVAPScanConfiguration()
//      UnloadNAVAPScanConfiguration()
//      LockNAVAPConfiguration()
//      UnlockNAVAPConfiguration()
//      LockNAVAPScanConfiguration()
//      UnlockNAVAPScanConfiguration()
//
// See Also:
//      config.cpp(NAVAP)
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/apcfg.h_v  $
// 
//    Rev 1.10   29 Sep 1998 18:23:22   RStanev
// Added dwFastCacheResetTime to NAVAP's internal configuration.
//
//    Rev 1.9   08 Jun 1998 21:13:44   RStanev
// Removed pszBackupExtension.
//
//    Rev 1.8   18 May 1998 17:42:44   DALLEE
// Added file heuristic scanning level settings to main options.
//
//    Rev 1.7   16 Mar 1998 14:08:54   DALLEE
// Changes to support sequences of actions to take:
// NAVAPCFG maintains arrays of action responses, no longer single response.
//
//    Rev 1.6   19 Feb 1998 16:15:26   RStanev
// Removed unknown virus detection.  Added quarantine support.
//
//    Rev 1.5   17 Feb 1998 22:57:26   RStanev
// Updated copyright.
//
//    Rev 1.4   30 May 1997 18:14:54   RStanev
// Added pszNAVAPCfg to NAVAPCFG.
//
//    Rev 1.3   28 May 1997 12:47:32   RStanev
// Split uScanFileAction into uScanFileExeAction and uScanFileMacroAction.
//
//    Rev 1.2   22 May 1997 20:16:02   RStanev
// Exported bNAVAPCanBeDisabled.  Removed bCanBeDisabled from NAVAPCFG.
//
//    Rev 1.1   15 May 1997 00:30:22   RStanev
// Cleaned up and exported a few more configuration functions.
//
//    Rev 1.0   09 Apr 1997 12:29:46   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef _APCFG_H_
#define _APCFG_H_

//---------------------------------------------------------------------------
//
// NAVAPCFG is a structure that contains the dynamic part of NAVAP's
// configuration.  This structure will be reloaded every time NAVAP is
// notified of a change in the configuration.
//
// The MAINACTION ???Actions[] members are ordered lists of actions to take in
// response to events. Lists must be terminated with MAINACTION_LAST.
// See notes in NAVGetMainAction() in APKERNEL actions.cpp.
//
// MAX_RESPONSE_ACTIONS defines the maximum number of sequential actions which
// may be listed, including the terminating non-action MAINACTION_LAST.
//
//---------------------------------------------------------------------------
#define MAX_RESPONSE_ACTIONS    3

typedef struct
    {
                                        // Structure management
    int         iUsageCount;

    PTSTR       pszNAVAPCfg;

                                        // File scan (known virus detection)
    UINT        uFileHeuristicLevel;

    BOOL        bScanFileOnExecute;
    BOOL        bScanFileOnOpen;
    BOOL        bScanFileOnCreate;

    BOOL        bScanAllFiles;

    MAINACTION  auScanFileExeActions   [MAX_RESPONSE_ACTIONS];
    MAINACTION  auScanFileMacroActions [MAX_RESPONSE_ACTIONS];

    UINT        uScanFilePrompts;

    BOOL        bDeleteInfectedOnCreate;

                                        // Device scan
    BOOL        bScanDeviceUponAccess;
    BOOL        bScanBootDeviceUponBoot;
    BOOL        bScanNonBootDevicesUponBoot;

    MAINACTION  auScanDeviceUponAccessActions [MAX_RESPONSE_ACTIONS];
    MAINACTION  auScanDeviceUponBootActions   [MAX_RESPONSE_ACTIONS];

    UINT        uScanDevicePrompts;

                                        // Behavior block
    MAINACTION  auFormatHardDiskActions       [MAX_RESPONSE_ACTIONS];
    MAINACTION  auWriteToHDBootRecordsActions [MAX_RESPONSE_ACTIONS];
    MAINACTION  auWriteToFDBootRecordActions  [MAX_RESPONSE_ACTIONS];
    MAINACTION  auWriteToProgramFilesActions  [MAX_RESPONSE_ACTIONS];
    MAINACTION  auROAttributeClearActions     [MAX_RESPONSE_ACTIONS];

    UINT        uBehaviorBlockPrompts;

                                        // File repair options (SYM_VXD: extension in OEM)
    BOOL        bBackupBeforeRepair;

                                        // File quarantine options
    PTSTR       pszQuarantineLocation;

                                        // File move options
    PTSTR       pszMoveLocation;

                                        // File rename options
    PTSTR       pszRenameExtension;

                                        // Workstation alerts
    BOOL        bDisplayAlert;
    BOOL        bAudibleAlert;
    UINT        uAudibleAlertDelay;
    UINT        uAlertTimeOut;

    PTSTR       pszAlertText;

                                        // List of program extensions (SYM_VXD: in OEM)
    PTSTR       pszzProgramExtensionList;

                                        // Long custom alert
    PTSTR       pszLongCustomAlert;

                                        // Scan files with no extension flag
    BOOL        bScanNoExtFile;

                                        // External module handles
    HEXCLUDEN   hExcludeN;
    } NAVAPCFG, *PNAVAPCFG;

//---------------------------------------------------------------------------
//
// Declare global startup and security NAVAP configuration variables.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern BOOL bNAVAPCfgStartupScanMemory;
extern BOOL bNAVAPCfgStartupScanUpperMemory;
extern BOOL bNAVAPCfgStartupRunDOSAutoProtect;

#endif // #if defined(SYM_VXD)

extern BOOL bNAVAPCanBeDisabled;

//---------------------------------------------------------------------------
//
// NAVAPINTERNALCFG is a structure representing NAVAP's internal
// configuration.
//
// rNAVAPInternalCfg is a variable defining NAVAP's internal configuration.
// The default values of rNAVAPInternalCfg can be changed only with registry
// keys.  These keys should not be available to the general public; they
// should only be used in case of emergencies and tracking problems.
//
//---------------------------------------------------------------------------
typedef struct
    {
#if defined(SYM_VXD)
    BOOL  bRealModeAPCommunication;
    BOOL  bRealModeAPEmulation;
    BOOL  bRealModeCriticalErrorHandler;
    BOOL  bControlBreakHandler;
    BOOL  bRegisterAppyTimeForAgent;
    BOOL  bExecuteAgent;
    BOOL  bShowUIWhileScanningBoot;
    DWORD dwDeviceShutdownDelayTime;
#elif defined(SYM_NTK)
    DWORD dwSIDLength;
#endif
    DWORD dwFileCacheEntries;
    ULONG ulFileCacheFilterFlags;
    BOOL  bIgnoreInt13DeviceCount;
    BOOL  bForcedInt13Device[256];
    BOOL  bHostileInt13Device[256];
    BOOL  bUsePAMEngine;
    BOOL  bExclusionsEnabled;
    BOOL  bSendInfoPackets;
    BOOL  bTruncateProgramFileExtensions;
    DWORD dwFastCacheResetTimeout;
    } NAVAPINTERNALCFG;

extern NAVAPINTERNALCFG rNAVAPInternalCfg;

//---------------------------------------------------------------------------
//
// Configuration function prototypes.
//
//---------------------------------------------------------------------------
BOOL ReloadNAVAPConfiguration (
    PTSTR pszPath
    );

BOOL LoadNAVAPScanConfiguration (
    PTSTR pszPath
    );

BOOL UnloadNAVAPScanConfiguration (
    VOID
    );

PNAVAPCFG LockNAVAPConfiguration (
    VOID
    );

VOID UnlockNAVAPConfiguration (
    PNAVAPCFG prCfg
    );

HVCONTEXT LockNAVAPScanConfiguration (
    VOID
    );

VOID UnlockNAVAPScanConfiguration (
    HVCONTEXT hVContext
    );

#endif // #ifndef _APCFG_H_
