// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/config.cpv   1.29   01 Oct 1998 12:27:12   tcashin  $
//
// Description:
//      This file defines functions and data for managing NAVAP's
//      configuration.
//
// Contains:
//      LoadNAVAPInternalConfiguration()
//      InitNAVAPConfiguration()
//      DeInitNAVAPConfiguration()
//      ReloadNAVAPConfiguration()
//      LoadNAVAPScanConfiguration()
//      UnloadNAVAPScanConfiguration()
//      LockNAVAPConfiguration()
//      UnlockNAVAPConfiguration()
//      LockNAVAPScanConfiguration()
//      UnlockNAVAPScanConfiguration()
//
//      LoadConfiguration()                 (local function)
//      UnloadConfiguration()               (local function)
//
//      AllocateAndReadConfiguration()      (local function)
//      FreeConfiguration()                 (inline function)
//
//      ReadNAVOPTSFile()                   (local function)
//      ReadNAVSTARTFile()                  (local function, SYM_VXD only)
//      ReadCUSTOMALERTFile()               (local function, SYM_VXD only)
//
//      AllocateAndConvertOptions()         (local function)
//      FreeOptions()                       (local function)
//
//      KE2M()                              (local function)
//      KM2M()                              (local function)
//      KB2M()                              (local function)
//      BB2M()                              (local function)
//      TranslateString()                   (local function)
//      TranslateProgramFileExtensions()    (local function)
//
//      GetNAVAPConfigurationDirectory()    (local function)
//      GetNAVAPScanDirectory()             (local function)
//      GetNAVAPStartupName()               (local function)
//
//      GetNAVEXConfigInfo()                (local function)
//
// See Also:
//      init.cpp, exit.cpp
//
// Note:
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/config.cpv  $
// 
//    Rev 1.29   01 Oct 1998 12:27:12   tcashin
// Changed the check for the navopts version back to 5.0.
// 
//    Rev 1.28   29 Sep 1998 18:31:06   RStanev
// Added dwFastCacheResetTimeout to NAVAP's internal configuration.
//
//    Rev 1.27   23 Sep 1998 17:18:22   tcashin
// Bumped the navopts version number for Barstow.
//
//    Rev 1.26   31 Jul 1998 10:05:30   tcashin
// Update the NAVOPTS.DAT version number for Atomic.
//
//    Rev 1.25   08 Jun 1998 21:13:12   RStanev
// Removed references to pszBackupExtension.
//
//    Rev 1.24   02 Jun 1998 00:16:58   RStanev
// Processing the FileSharing registry key globally.
//
//    Rev 1.23   18 May 1998 17:43:26   DALLEE
// Added engine callback for setting file heuristic scanning level.
//
//    Rev 1.22   27 Apr 1998 10:39:46   DALLEE
// Oops.. Cleanup for NTK. Needed character set conversion when reading
// quarantine path.
//
//    Rev 1.21   27 Apr 1998 09:54:12   DALLEE
// Read location of quarantine location with GetQuarantinePath().
//
//    Rev 1.20   22 Apr 1998 12:54:36   DALLEE
// Flag for displaying Quarantine button is now read from
// TSROPTS.bDispQuarantine. The QUARANTINESET.bQuarantineAP is not
// used anymore.
//
//    Rev 1.19   01 Apr 1998 16:59:52   DALLEE
// Added handling for new TSR_QUAR_CANT_REP option.
// Set sequence of actions: REPAIR, QUARANTINE, LAST.
//
//    Rev 1.18   16 Mar 1998 14:44:30   DALLEE
// Default options, and conversion NAVOPTS.DAT to MAINACTION routines
// now deal with ordered arrays of actions to take.
//
//    Rev 1.17   19 Feb 1998 17:54:38   RStanev
// Removed unknown virus detection.  Added quarantine support.
//
//    Rev 1.16   17 Feb 1998 22:36:00   RStanev
// Updated copyright.
//
//    Rev 1.15   13 Aug 1997 18:26:30   MKEATIN
// We now pass the full path of NAVEX15.INF to VLRegisterVirusDB().
//
//    Rev 1.14   27 Jun 1997 18:12:56   RStanev
// FreeOptions() now releases memory allocated for pszInocPath in NAVAPCFG.
//
//    Rev 1.13   20 Jun 1997 11:31:00   DBUCHES
// Bumped version of NAVOPTS.DAT file.  Now using NAV_VERSION_97.
//
//    Rev 1.12   19 Jun 1997 16:16:18   RStanev
// Fixed call to VLScanInit().
//
//    Rev 1.11   18 Jun 1997 21:12:44   JBRENNA
// Add call to NAVSetDefsPath(). This tells our AVAPI1 callback library where
// the definitions are. Note that our AVAPI1 callback structure stores this
// in a static variable. ... As Rad noted, this eliminates multithreading
// possibilities.
//
//    Rev 1.10   17 Jun 1997 16:51:08   MKEATIN
// VLScanInit() now passes &gstNAVCallBacks.
//
//    Rev 1.9   06 Jun 1997 16:42:24   RStanev
// GetNAVAPScanDirectory() now uses szNAVAPDefPathRegSection[] and
// szNAVAPDefPathRegKeyname[].
//
//    Rev 1.8   30 May 1997 22:44:40   RStanev
// Removed an unnecessary SYM_ASSERT().
//
//    Rev 1.7   30 May 1997 18:44:22   RStanev
// Reworked the handling of configuration directories.  Added pszNAVAPCfg
// to NAVAPCFG and removed the global pszNAVAPCfg.
//
//    Rev 1.6   28 May 1997 14:16:30   RStanev
// Split uScanFileAction into uScanFileExeAction and uScanFileMacroAction.
//
//    Rev 1.5   23 May 1997 15:43:40   RStanev
// All configuration structures are now pulled out of NAVBIN.H.  This way
// NAVAP will automatically pick up any changes to the DAT files.
//
//    Rev 1.4   22 May 1997 21:57:02   RStanev
// Added long custom alert support.
//
//    Rev 1.3   21 May 1997 04:45:50   RStanev
// Added pszNAVAPStartupName and supporting code.  Enabled AVAPI boot
// caching for SYM_VXD.
//
//    Rev 1.2   17 May 1997 20:55:40   RStanev
// Renamed GetNAVAPStartupDirectory() to GetNAVAPConfigurationDirectory().
//
//    Rev 1.1   15 May 1997 02:56:22   RStanev
// Too many changes to describe.
//
//    Rev 1.0   09 Apr 1997 12:21:34   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "symsync.h"
#include "symcfg.h"
#include "xapi.h"
#include "file.h"
#include "navap.h"
#include "navapi.h"
#include "apexcl.h"
#include "apstrs.h"
#include "navbin.h"
#include "navcb.h"
#include "quaradd.h"
#include "navoptx.h"

//---------------------------------------------------------------------------
//
// prNAVAPCfg contains a pointer to the currently active NAVAP
// configuration.
//
// pszNAVAPScan points to the location of the scan engine files.
//
// pszNAVAPStartupName points to a string containing the full file spec
// of the NAVAP driver.
//
// hVGlobalContext contains a handle to a global AVAPI context.
//
// Note:
//      SYM_VXD: pszNAVAPScan and pszNAVAPStartupName are in OEM.
//
//---------------------------------------------------------------------------
PNAVAPCFG prNAVAPCfg = NULL;
PTSTR     pszNAVAPScan = NULL;
PTSTR     pszNAVAPStartupName = NULL;
HVCONTEXT hVGlobalContext = NULL;

// Global variable for the extension list removed from the NAVOPTS structure
// Note: This list is assumed to be non-DBCS compliant!
char szExtensionList[MAX_EXTENSION_LIST_SIZE+1];

//---------------------------------------------------------------------------
//
// The following options are updated upon (re)loading of NAVAP's
// configuration.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL bNAVAPCfgStartupScanMemory = TRUE;
BOOL bNAVAPCfgStartupScanUpperMemory = FALSE;
BOOL bNAVAPCfgStartupRunDOSAutoProtect = FALSE;

#endif // #if defined(SYM_VXD)

BOOL bNAVAPCanBeDisabled = TRUE;

//---------------------------------------------------------------------------
//
// Define syncrhonization objects for guarding the configuration variables
// above.
//
// rCfgLock guards the iUsageCount member of the NAVAPCFG structure, as well
// as changes to prNAVAPCfg.  This lock guards the iUsageCount members of
// both current and pending delete configurations.
//
// rCfgUsage keeps track of the total users of NAVAP configurations, both
// current and pending delete.  Every client that has locked a configuration
// will acquire shared access to this resource.
//
// rAVAPIUsage guards changes to hVGlobalContext and pszNAVAPScan.
//
//---------------------------------------------------------------------------
#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

SYMMUTEX rCfgLock = {0};

SYMRESOURCE rCfgUsage = {0};

SYMRESOURCE rAVAPIUsage = {0};

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// lReloadNAVAPConfigurationLock is used by ReloadNAVAPConfiguration() to
// control reentrancy.
//
// lNAVAPScanConfigurationLock is used by LoadNAVAPScanConfiguration() and
// UnloadNAVAPScanConfiguration() to control reentrancy.
//
// lNAVAPCfgInitDeInitSync is used to control the access to NAVAP's
// configuration APIs.  It is available only in debug and prerelease versions
// because the APIs which it protects are called internally by NAVAP -
// something over which I have full control, while reloading of the
// configuration or AVAPI context can be requested by external clients.
//
// bNAVAPCfgInitialized is also a debug-only variable to help with
// identifying synchronization problems.
//
//---------------------------------------------------------------------------
#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

LONG lReloadNAVAPConfigurationLock = 0;
LONG lNAVAPScanConfigurationLock = 0;

#if defined(SYM_DEBUG) || defined(PRERELEASE)

LONG lNAVAPCfgInitDeInitSync = -1;
BOOL bNAVAPCfgInitialized = FALSE;

#endif

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// Define default NAVAP configuration variables and structures.
//
//---------------------------------------------------------------------------
#define fDEFPROMPTACTION_SCANFILE       ( fPROMPTACTION_STOP      | \
                                          fPROMPTACTION_REPAIR    | \
                                          fPROMPTACTION_DELETE )

#define fDEFPROMPTACTION_BEHAVIORBLOCK  ( fPROMPTACTION_STOP      | \
                                          fPROMPTACTION_CONTINUE  | \
                                          fPROMPTACTION_EXCLUDE )

#define fDEFPROMPTACTION_SCANDEVICE     ( fPROMPTACTION_CONTINUE  | \
                                          fPROMPTACTION_REPAIR )

NAVAPCFG rNAVAPDefaultCfg =
    {
                                        // Structure management
    0,                                  //    iUsageCount;
                                        //
    NULL,                               //    pszNAVAPCfg;
                                        //
                                        // File scan (known virus detection)
    2,                                  //    uFileHeuristicLevel - on, medium.
                                        //
    TRUE,                               //    bScanFileOnExecute;
    FALSE,                              //    bScanFileOnOpen;
    FALSE,                              //    bScanFileOnCreate;
                                        //
    FALSE,                              //    bScanAllFiles;
                                        //
    {MAINACTION_PROMPT,MAINACTION_LAST},//    auScanFileExeActions;
    {MAINACTION_PROMPT,MAINACTION_LAST},//    auScanFileMacroActions;
                                        //
    fDEFPROMPTACTION_SCANFILE,          //    uScanFilePrompts;
                                        //
    TRUE,                               //    bDeleteInfectedOnCreate;
                                        //
                                        // Device scan
    TRUE,                               //    bScanDeviceUponAccess;
    TRUE,                               //    bScanBootDeviceUponBoot;
    FALSE,                              //    bScanNonBootDevicesUponBoot;
                                        //
    {MAINACTION_PROMPT,MAINACTION_LAST},//    auScanDeviceUponAccessActions;
    {MAINACTION_PROMPT,MAINACTION_LAST},//    auScanDeviceUponBootActions;
                                        //
    fDEFPROMPTACTION_SCANDEVICE,        //    uScanDevicePrompts;
                                        //
                                        // Behavior block
    {MAINACTION_ALLOW,MAINACTION_LAST}, //    auFormatHardDiskActions;
    {MAINACTION_ALLOW,MAINACTION_LAST}, //    auWriteToHDBootRecordsActions;
    {MAINACTION_ALLOW,MAINACTION_LAST}, //    auWriteToFDBootRecordActions;
    {MAINACTION_ALLOW,MAINACTION_LAST}, //    auWriteToProgramFilesActions;
    {MAINACTION_ALLOW,MAINACTION_LAST}, //    auROAttributeClearActions;
                                        //
    fDEFPROMPTACTION_BEHAVIORBLOCK,     //    uBehaviorBlockPrompts;
                                        //
                                        // File repair options (SYM_VXD: extension in OEM)
    TRUE,                               //    bBackupBeforeRepair;
                                        //
                                        // File quarantine options (SYM_VXD: in OEM)
    NULL,                               //    pszQuarantineLocation;
                                        //
                                        // File move options (SYM_VXD: in OEM)
    NULL,                               //    pszMoveLocation;
                                        //
                                        // File rename options (SYM_VXD: in OEM)
    NULL,                               //    pszRenameExtension;
                                        //
                                        // Workstation alerts
    FALSE,                              //    bDisplayAlert;
    TRUE,                               //    bAudibleAlert;
    100,                                //    uAudibleAlertDelay;
    0,                                  //    uAlertTimeOut;
                                        //
    NULL,                               //    pszAlertText;
                                        //
                                        // List of program extensions (SYM_VXD: in OEM)
    NULL,                               //    pszzProgramExtensionList;
                                        //
                                        // Long custom alert
    NULL,                               //    pszLongCustomAlert;
                                        //
                                        // External module handles
    {0},                                //    hExcludeN;
    };

//---------------------------------------------------------------------------
//
// This structure contains NAVAP's internal configuration.  The default
// values can be changed only with registry keys.  These keys should not be
// documented; they should only be used in case of emergencies and tracking
// problems.  Do not abuse this feature!
//
// After changing this structure, please update the README.TXT file located
// in the directory where CONFIG.CPP is.
//
// Notes:
//      The value 0x28 for dwSIDLength is calculated based on the WinNT 3.51
//      SID structure, with 8 subauthorities.  Our default SID size is
//      significantly larger than that.
//
//      ulFileCacheFilterFlags is a field which is filled with a combination
//      of the following constants defined in the NT DDK:
//
//          #define FILE_REMOVABLE_MEDIA            0x00000001
//          #define FILE_READ_ONLY_DEVICE           0x00000002
//          #define FILE_FLOPPY_DISKETTE            0x00000004
//          #define FILE_WRITE_ONCE_MEDIA           0x00000008
//          #define FILE_REMOTE_DEVICE              0x00000010
//          #define FILE_DEVICE_IS_MOUNTED          0x00000020
//          #define FILE_VIRTUAL_VOLUME             0x00000040
//
//---------------------------------------------------------------------------
NAVAPINTERNALCFG rNAVAPInternalCfg =
    {
#if defined(SYM_VXD)
    TRUE,                               // BOOL  bRealModeAPCommunication;
    TRUE,                               // BOOL  bRealModeAPEmulation;
    TRUE,                               // BOOL  bRealModeCriticalErrorHandler;
    TRUE,                               // BOOL  bControlBreakHandler;
    TRUE,                               // BOOL  bRegisterAppyTimeForAgent;
    TRUE,                               // BOOL  bExecuteAgent;
    TRUE,                               // BOOL  bShowUIWhileScanningBoot;
    2000,                               // DWORD dwDeviceShutdownDelayTime;
#elif defined(SYM_NTK)
    0x44,                               // DWORD dwSIDLength;
#endif
    0x1000,                             // DWORD dwFileCacheEntries;
    0x00000014,                         // ULONG ulFileCacheFilterFlags;
    FALSE,                              // BOOL  bIgnoreInt13DeviceCount;
    { FALSE },                          // BOOL  bForcedInt13Device[256];
    { FALSE },                          // BOOL  bHostileInt13Device[256];
    TRUE,                               // BOOL  bUsePAMEngine;
    TRUE,                               // BOOL  bExclusionsEnabled;
    TRUE,                               // BOOL  bSendInfoPackets;
    TRUE,                               // BOOL  bTruncateProgramFileExtensions;
    0                                   // DWORD dwFastCacheResetTimeout;
    };

//---------------------------------------------------------------------------
//
// Local function prototypes.
//
//---------------------------------------------------------------------------
PNAVAPCFG LoadConfiguration (
    PTSTR pszPath
    );

VOID UnloadConfiguration (
    PNAVAPCFG prCfg
    );

PNAVAPCFG AllocateAndReadConfiguration (
    PTSTR pszPath
    );

BOOL ReadNAVOPTSFile (
    PCTSTR      pszCfgFile,
    PNAVOPTIONS prNAVOpts
    );

#if defined(SYM_VXD)

BOOL ReadNAVSTARTFile (
    PCTSTR      pszCfgFile,
    PNAVOPTIONS prNAVOpts
    );

PSTR ReadCUSTOMALERTFile (
    PCTSTR pszCustomAlertFile
    );

#endif // #if defined(SYM_VXD)

BOOL ReadQuarantinePath(
    PSTR    pszQuarantinePath,
    UINT    cchBufferSize
    );

PNAVAPCFG AllocateAndConvertOptions (
    PNAVOPTIONS prNAVOptions
    );

VOID FreeOptions (
    PNAVAPCFG prCfg
    );

VOID KE2M (
    PMAINACTION puActions,
    UINT        uKnownVirusAction,
    UINT        uCustomAction,
    PMAINACTION puDefaultActions
    );

VOID KM2M (
    PMAINACTION puActions,
    UINT        uKnownVirusAction,
    UINT        uCustomAction,
    PMAINACTION puDefaultActions
    );

VOID KB2M (
    PMAINACTION puActions,
    UINT        uKnownVirusAction,
    UINT        uCustomAction,
    PMAINACTION puDefaultActions
    );

VOID KBB2M (
    PMAINACTION puActions,
    UINT        uKnownVirusAction,
    UINT        uCustomAction,
    PMAINACTION puDefaultActions
    );

VOID BB2M (
    PMAINACTION puActions,
    UINT        uBlockBehaviorAction,
    PMAINACTION puDefaultActions
    );

PTSTR TranslateString (
    PSTR pszNAVFormat
    );

PTSTR TranslateProgramFileExtensions (
    PSTR pszzNAVFormat
    );

PTSTR GetNAVAPConfigurationDirectory (
    PTSTR pszEnforced
    );

PTSTR GetNAVAPScanDirectory (
    PTSTR pszEnforced
    );

PTSTR GetNAVAPStartupName (
    PTSTR pszNAVAPCfg
    );

VOID WINAPI GetNAVEXConfigInfo (
    PTSTR  lpszID,
    LPVOID lpvResult,
    UINT   uMaxResultLen,
    LPBOOL lpbImplemented,
    LPVOID lpvCookie
    );

BOOL GetPath(TCHAR * szTemp, int size);

BOOL ParseValueFromFile(char *szSection, char *szKey, PCTSTR pszCfgFile, char *pszData, DWORD nDataLen);

//---------------------------------------------------------------------------
//
// Define inline functions.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// inline VOID FreeConfiguration (
//      PNAVAPCFG prCfg
//      );
//
// This function releases configuration structures created by
// AllocateAndReadConfiguration().  External modules which might have been
// initialized for prCfg must be released by the client before calling
// FreeConfiguration().
//
// Entry:
//      prCfg - a non-NULL pointer returned by
//              AllocateAndReadConfiguration().
//
// Exit:
//      -
//      All memory associated with the configuration structure is released.
//
//---------------------------------------------------------------------------
inline VOID FreeConfiguration (
    PNAVAPCFG prCfg
    )
    {
    SYM_ASSERT ( prCfg );
    SYM_ASSERT ( !prCfg->hExcludeN );
    SYM_ASSERT ( !prCfg->pszNAVAPCfg );

    FreeOptions ( prCfg );
    }

//---------------------------------------------------------------------------
//
// VOID LoadNAVAPInternalConfiguration (
//      VOID
//      );
//
// This function reads the internal configuration of NAVAP from the
// registry.  LoadNAVAPInternalConfiguration() will never fail.  If there
// is an error reading some of the configuration items, the default values
// will be used.
//
// LoadNAVAPInternalConfiguration() is available only when the NAVAP
// driver is being loaded.
//
// Entry:
//      -
//
// Exit:
//      rNAVAPInternalCfg initialized with the internal settings.
//
// Note:
//      The functionality of LoadNAVAPInternalConfiguration() does not
//      depend on the status of InitNAVAPConfiguration() and
//      DeInitNAVAPConfiguration().
//
// Synchronization:
//      This function is fully reentrant, but who cares?
//
//---------------------------------------------------------------------------
#define DRIVER_INIT_CODE_SEGMENT
#define DRIVER_INIT_DATA_SEGMENT
#include "drvseg.h"

VOID LoadNAVAPInternalConfiguration (
    VOID
    )
    {
    auto DWORD     dwRegKeyType, dwDestinationLength;
    auto DWORD     dwRegValue;
    auto BYTE      abyRegValue[256];
    auto CFGREGKEY rRegKey = {
                             HKEY_LOCAL_MACHINE,
                             szNAVAPCFGProductName,
                             NULL,
                             szNAVAPCFGProjectName,
                             szNAVAPCFGSectionName,
                             NULL,
                             NULL,
                             NULL
                             };

#if defined(SYM_VXD)
                                        // Read bRealModeAPCommunication

    rRegKey.lpszName = szNAVAPCFGbRealModeAPCommunication;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bRealModeAPCommunication = dwRegValue;
            }
        }

                                        // Read bRealModeAPEmulation

    rRegKey.lpszName = szNAVAPCFGbRealModeAPEmulation;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bRealModeAPEmulation = dwRegValue;
            }
        }

                                        // Read bRealModeCriticalErrorHandler

    rRegKey.lpszName = szNAVAPCFGbRealModeCriticalErrorHandler;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bRealModeCriticalErrorHandler = dwRegValue;
            }
        }

                                        // Read bControlBreakHandler

    rRegKey.lpszName = szNAVAPCFGbControlBreakHandler;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bControlBreakHandler = dwRegValue;
            }
        }

                                        // Read bRegisterAppyTimeForAgent

    rRegKey.lpszName = szNAVAPCFGbRegisterAppyTimeForAgent;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bRegisterAppyTimeForAgent = dwRegValue;
            }
        }

                                        // Read bExecuteAgent

    rRegKey.lpszName = szNAVAPCFGbExecuteAgent;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bExecuteAgent = dwRegValue;
            }
        }

                                        // Read bShowUIWhileScanningBoot

    rRegKey.lpszName = szNAVAPCFGbShowUIWhileScanningBoot;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bShowUIWhileScanningBoot = dwRegValue;
            }
        }

                                        // Read dwDeviceShutdownDelayTime

    rRegKey.lpszName = szNAVAPCFGdwDeviceShutdownDelayTime;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.dwDeviceShutdownDelayTime = dwRegValue;
            }
        }

#elif defined(SYM_NTK)

                                        // Read dwSIDLength

    rRegKey.lpszName = szNAVAPCFGdwSIDLength;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.dwSIDLength = dwRegValue;
            }
        }

#endif

                                        // Read bFileSharing, and immediately
                                        // process its value.

    rRegKey.lpszName = szNAVAPCFGbFileSharing;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD && !dwRegValue )
            {
            FileEnableFileSharing ( FALSE );
            }
        }

                                        // Read dwFileCacheEntries

    rRegKey.lpszName = szNAVAPCFGdwFileCacheEntries;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.dwFileCacheEntries = dwRegValue;
            }
        }

                                        // Read ulFileCacheFilterFlags

    rRegKey.lpszName = szNAVAPCFGulFileCacheFilterFlags;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.ulFileCacheFilterFlags = dwRegValue;
            }
        }

                                        // Read bIgnoreInt13DeviceCount

    rRegKey.lpszName = szNAVAPCFGbIgnoreInt13DeviceCount;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bIgnoreInt13DeviceCount = dwRegValue;
            }
        }

                                        // Read bForcedInt13Device[256]

    rRegKey.lpszName = szNAVAPCFGabyForcedInt13DeviceList;

    dwDestinationLength = sizeof(abyRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               abyRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_BINARY )
            {
            while ( dwDestinationLength-- != 0 )
                {
                rNAVAPInternalCfg.bForcedInt13Device[abyRegValue[dwDestinationLength]] =
                    TRUE;
                }
            }
        }

                                        // Read bHostileInt13Device[256]

    rRegKey.lpszName = szNAVAPCFGabyHostileInt13DeviceList;

    dwDestinationLength = sizeof(abyRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               abyRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_BINARY )
            {
            while ( dwDestinationLength-- != 0 )
                {
                rNAVAPInternalCfg.bHostileInt13Device[abyRegValue[dwDestinationLength]] =
                    TRUE;
                }
            }
        }

                                        // Read bUsePAMEngine

    rRegKey.lpszName = szNAVAPCFGbUsePAMEngine;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bUsePAMEngine = dwRegValue;
            }
        }

                                        // Read bExclusionsEnabled

    rRegKey.lpszName = szNAVAPCFGbExclusionsEnabled;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bExclusionsEnabled = dwRegValue;
            }
        }

                                        // Read bSendInfoPackets

    rRegKey.lpszName = szNAVAPCFGbSendInfoPackets;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bSendInfoPackets = dwRegValue;
            }
        }

                                        // Read bTruncateProgramFileExtensions

    rRegKey.lpszName = szNAVAPCFGbTruncateProgramFileExtensions;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.bTruncateProgramFileExtensions = dwRegValue;
            }
        }

                                        // Read dwFastCacheResetTimeout

    rRegKey.lpszName = szNAVAPCFGdwFastCacheResetTimeout;

    dwDestinationLength = sizeof(dwRegValue);

    if ( ConfigRegQueryValue ( &rRegKey,
                               NULL,
                               &dwRegKeyType,
                               &dwRegValue,
                               &dwDestinationLength ) == ERROR_SUCCESS )
        {
        if ( dwRegKeyType == REG_DWORD )
            {
            rNAVAPInternalCfg.dwFastCacheResetTimeout = dwRegValue;
            }
        }
    }

#define DRIVER_DEFAULT_CODE_SEGMENT
#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// BOOL InitNAVAPConfiguration (
//      VOID
//      );
//
// This function initializes NAVAP's configuration synchronization, then it
// loads NAVAP's configuration, and at last it initializes external
// configuration modules (such as AVAPI).
//
// Call InitNAVAPCofiguration() during controlled initialization of NAVAP.
// If it succeeds, then the driver may be enabled.  If it fails, the driver
// should not be enabled, until InitNAVAPConfiguration() succeeds.
//
// This function will report error and warning messages when necessary.
//
// Entry:
//      -
//
// Exit:
//      TRUE  if successful
//            configuration initialized
//      FALSE if error
//            configuration is invalid
//
// Synchronization:
//      This function is non-reentrant.  No calls to other config APIs
//      should be made until this function completes successfully.
//
//---------------------------------------------------------------------------
BOOL InitNAVAPConfiguration (
    VOID
    )
    {
    auto PTSTR pszNAVAPCfg;

                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) == 0 );
    SYM_ASSERT ( !bNAVAPCfgInitialized );

                                        // Initialize configuration
                                        // synchronization.

    if ( SymCreateMutex ( &rCfgLock ) )
        {
        if ( SymCreateResource ( &rCfgUsage ) )
            {
            if ( SymCreateResource ( &rAVAPIUsage ) )
                {
                                        // Get the configuration directory.

                if ( pszNAVAPCfg = GetNAVAPConfigurationDirectory ( NULL ) )
                    {
                                        // Get the scan directory.

                    SYM_ASSERT ( !pszNAVAPScan );

                    if ( pszNAVAPScan = GetNAVAPScanDirectory ( NULL ) )
                        {
                                        // Initialize pszNAVAPStartupName.

                        SYM_ASSERT ( !pszNAVAPStartupName );

                        if ( pszNAVAPStartupName = GetNAVAPStartupName ( pszNAVAPCfg ) )
                            {
                                        // Load the configuration and make it
                                        // current.

                            SYM_ASSERT ( !prNAVAPCfg );

                            if ( prNAVAPCfg = LoadConfiguration ( pszNAVAPCfg ) )
                                {
                                        // Initialize AVAPI.

                                if ( LoadNAVAPScanConfiguration ( pszNAVAPScan ) )
                                    {
                                    SYM_ASSERT ( bNAVAPCfgInitialized = TRUE );

                                    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );

                                    MemFreePtr ( pszNAVAPCfg );

                                    return ( TRUE );
                                    }

                                UnloadConfiguration ( prNAVAPCfg );

                                prNAVAPCfg = NULL;
                                }

                            MemFreePtr ( pszNAVAPStartupName );

                            pszNAVAPStartupName = NULL;
                            }
                        else
                            {
                            UIControlledInitMessage ( MSG_Unable_To_Determine_Startup_Name );
                            }

                        MemFreePtr ( pszNAVAPScan );

                        pszNAVAPScan = NULL;
                        }
                    else
                        {
                        UIControlledInitMessage ( MSG_Unable_To_Locate_Scan_Directory );
                        }

                    MemFreePtr ( pszNAVAPCfg );
                    }
                else
                    {
                    UIControlledInitMessage ( MSG_Unable_To_Locate_Configuration );
                    }

                SymDestroyResource ( &rAVAPIUsage );
                }
            else
                {
                UIControlledInitMessage ( MSG_Unable_To_Synchronize_Configuration );
                }

            SymDestroyResource ( &rCfgUsage );
            }
        else
            {
            UIControlledInitMessage ( MSG_Unable_To_Synchronize_Configuration );
            }

        SymDestroyMutex ( &rCfgLock );
        }
    else
        {
        UIControlledInitMessage ( MSG_Unable_To_Synchronize_Configuration );
        }

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL DeInitNAVAPConfiguration (
//      VOID
//      );
//
// This function deinitializes NAVAP's configuration.  It should be called
// only if an earlier call to InitNAVAPConfiguration() successfully
// completed.
//
// Entry:
//      -
//
// Exit:
//      Always returns TRUE.
//
// Synchronization:
//      This function is non-reentrant.  No configuration APIs should be
//      called once this function starts executing.
//
//---------------------------------------------------------------------------
BOOL DeInitNAVAPConfiguration (
    VOID
    )
    {
                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) == 0 );
    SYM_ASSERT ( bNAVAPCfgInitialized );

                                        // Make sure that currently there are
                                        // no users of the configuration.
                                        // This also includes configurations
                                        // pending delete.

#if defined(SYM_DEBUG) || defined(PRERELEASE)

    if ( SymAcquireResourceExclusive ( &rCfgUsage, FALSE, 0 ) )
        {
        SymReleaseResource ( &rCfgUsage );
        }
    else
        {
        SYM_ASSERT ( FALSE );
        }

#endif

                                        // Make sure that currently there are
                                        // no users of the AVAPI global
                                        // context handle.

#if defined(SYM_DEBUG) || defined(PRERELEASE)

    if ( SymAcquireResourceExclusive ( &rAVAPIUsage, FALSE, 0 ) )
        {
        SymReleaseResource ( &rAVAPIUsage );
        }
    else
        {
        SYM_ASSERT ( FALSE );
        }

#endif

                                        // Release AVAPI and unload the
                                        // configuration.

    SYM_ASSERT ( ! ( bNAVAPCfgInitialized = FALSE ) );

    SYM_ASSERT ( prNAVAPCfg );

    UnloadNAVAPScanConfiguration();

    UnloadConfiguration ( prNAVAPCfg );

    prNAVAPCfg = NULL;

                                        // Release memory for the location
                                        // of the configuration and the
                                        // startup directory.

    SYM_ASSERT ( pszNAVAPStartupName );

    MemFreePtr ( pszNAVAPStartupName );

    pszNAVAPStartupName = NULL;

    SYM_ASSERT ( pszNAVAPScan );

    MemFreePtr ( pszNAVAPScan );

    pszNAVAPScan = NULL;

                                        // Destroy the synchronization.

    SymDestroyResource ( &rAVAPIUsage );

    SymDestroyResource ( &rCfgUsage );

    SymDestroyMutex ( &rCfgLock );

                                        // Return to caller.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL ReloadNAVAPConfiguration (
//      PTSTR pszPath
//      );
//
// This function loads new NAVAP configuration (path specified at pszPath)
// and makes it current (which means that any consecutive calls to
// LockNAVAPConfiguration() will return a pointer to the new configuration).
// The old configuration is marked as pending for deletion, and is released
// as soon as its usage count becomes 0.
//
// ReloadNAVAPConfiguration() will report any warning or error messages
// that might be generated while processing.
//
// Entry:
//      pszPath - location of the configuration files.  SYM_VXD: in OEM.
//                if this parameter is NULL, the location of the currently
//                active configuration will be used.
//
// Exit:
//      TRUE  if configuration was successfully read and all related
//            external modules were initialized.
//      FALSE if the function failed to initialize the configuration.
//            In this case, the old configuration will be left intact.
//
// Note:
//      This function will fail if the length of the string pointed to by
//      pszPath is longer than SYM_MAX_PATH - 1 characters.
//
// Synchronization:
//      ReloadNAVAPConfiguration() is non-reentrant, but will quietly fail
//      if it is called simultaneously from different threads.
//
//---------------------------------------------------------------------------
BOOL ReloadNAVAPConfiguration (
    PTSTR pszPath
    )
    {
    auto PNAVAPCFG prNewCfg, prOldCfg;

                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );
    SYM_ASSERT ( bNAVAPCfgInitialized );

                                        // Quick reentrancy check...

    if ( !SymInterlockedExchange ( &lReloadNAVAPConfigurationLock, 1 ) )
        {
                                        // If the caller specified a new
                                        // path for the configuration, then
                                        // validate the location.

        if ( pszPath )
            {
            SYM_VERIFY_STRING ( pszPath );
            }

                                        // Load the new configuration.

        if ( prOldCfg = LockNAVAPConfiguration() )
            {
            SYM_ASSERT ( prOldCfg->pszNAVAPCfg );

            if ( prNewCfg = LoadConfiguration (
                                pszPath ?
                                pszPath : prNAVAPCfg->pszNAVAPCfg )
               )
                {
                                        // Mark the old configuration as
                                        // pending for deletion, and make the
                                        // new configuration current.

                SymWaitForMutex ( &rCfgLock );

                prNAVAPCfg = prNewCfg;

                SymReleaseMutex ( &rCfgLock );

                UnlockNAVAPConfiguration ( prOldCfg );

                SymInterlockedExchange ( &lReloadNAVAPConfigurationLock, 0 );

                SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );

                return ( TRUE );
                }

            UnlockNAVAPConfiguration ( prOldCfg );
            }

        SymInterlockedExchange ( &lReloadNAVAPConfigurationLock, 0 );
        }

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL LoadNAVAPScanConfiguration (
//      PTSTR pszPath
//      );
//
// This function initializes the AVAPI global context handle at
// hVGlobalContext.  The location of the AVAPI files should be specified at
// pszPath.
//
// LoadNAVAPScanConfiguration() will report any warning or error messages
// that might be generated while processing.
//
// Entry:
//      pszPath - Location of AVAPI's files.  SYM_VXD: in OEM.  If this
//                parameter is NULL, the path will be determined by a call
//                to GetNAVAPScanDirectory().
//
// Exit:
//      TRUE if hVGlobalContext successfully initialized, or FALSE if
//      error.  Upon error, hVGlobalContext will be set to NULL.
//
// Synchronization:
//      This function is non-reentrant, but will quietly fail if it is
//      called simultaneously from different threads.
//      Calling LoadNAVAPScanConfiguration() while another thread is in
//      UnloadNAVAPScanConfiguration() is illegal, and if such sutiation is
//      detected, LoadNAVAPScanConfiguration() will fail.
//
//---------------------------------------------------------------------------
BOOL LoadNAVAPScanConfiguration (
    PTSTR pszPath
    )
    {
    auto BOOL bResult;

                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );

                                        // Initialize local variables.

    bResult = FALSE;

                                        // Quick reentrancy check...

    if ( !SymInterlockedExchange ( &lNAVAPScanConfigurationLock, 1 ) )
        {
                                        // Acquire exclusive access to the
                                        // AVAPI handle and pszNAVAPScan.

        if ( SymAcquireResourceExclusive ( &rAVAPIUsage, TRUE, 0 ) )
            {
            auto PTSTR pszNewPath, pszDat, pszInf, pszTemp;
            auto PTSTR pszAppPath, pszNavexInf;

                                        // Make sure we will not destroy an
                                        // existing handle.  We should not
                                        // be called with an existing handle
                                        // to AVAPI, but since our APIs are
                                        // exported to user-mode clients, we
                                        // need to handle incorrect calls.

            SYM_ASSERT ( !hVGlobalContext );

            if ( !hVGlobalContext )
                {
                                        // Initialize helper variables.

                pszNewPath = NULL;

                pszDat = NULL;

                pszInf = NULL;

                pszAppPath = NULL;

                pszNavexInf = NULL;

                                        // When this function is called, we
                                        // should always have a base for
                                        // antivirus engine configuration
                                        // directory;

                SYM_ASSERT ( pszNAVAPScan );

                                        // Override default AVAPI VxD callbacks.

                gstNAVCallBacks.lpNAVEXCallBacks->GetConfigInfo = GetNAVEXConfigInfo;

                                        // Initialize AVAPI.

                hVGlobalContext = VLScanInit ( VL_REVISION,
                                               &gstNAVCallBacks,
                                               ( VLINIT_NO_ACCESS_NO_HANDLE |
#if defined(SYM_VXD)
                                                 VLINIT_CREATE_BOOT_INFO_CACHE |
#endif // #if defined(SYM_VXD)
                                                 ( rNAVAPInternalCfg.bUsePAMEngine ?
                                                        0 : VLINIT_NO_PAM )
                                               )
                                             );

                if ( !hVGlobalContext )
                    {
                    UIControlledInitMessage ( MSG_Config_VLScanInit_Failed );
                    goto function_fail;
                    }

                                        // Initialize the new path to the
                                        // virus databse files.

                pszNewPath = GetNAVAPScanDirectory ( pszPath );

                if ( !pszNewPath )
                    {
                    UIControlledInitMessage ( MSG_Config_Bad_Path_Specified );
                    goto function_fail;
                    }

                                        // Tell the AVAPI callbacks the
                                        // definition location.
                NAVSetDefsPath (pszNewPath);

                                        // Initialize the virus database
                                        // files.

                pszDat = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) *
                                                     ( STRLEN(pszNewPath) +
                                                       STRLEN(szVirusDatabaseFileName) + 2 )
                                             );

                if ( !pszDat )
                    {
                    UIOutOfMemoryError();
                    goto function_fail;
                    }

                pszInf = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) *
                                                     ( STRLEN(pszNewPath) +
                                                       STRLEN(szVirusInfoFileName) + 2 )
                                             );

                if ( !pszInf )
                    {
                    UIOutOfMemoryError();
                    goto function_fail;
                    }

                STRCPY ( pszDat, pszNewPath );
                STRCPY ( pszInf, pszNewPath );

                NameAppendFile ( pszDat, szVirusDatabaseFileName );
                NameAppendFile ( pszInf, szVirusInfoFileName );

                                        // Initialize the app path to the
                                        // install directory - this is
                                        // for NAVEX15.INF

                pszAppPath = GetNAVAPConfigurationDirectory ( NULL );

                if ( !pszAppPath )
                    {
                    UIControlledInitMessage ( MSG_Config_Bad_Path_Specified );
                    goto function_fail;
                    }

                pszNavexInf = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) *
                                                          ( STRLEN(pszAppPath) +
                                                            STRLEN(szNavexInfFileName) + 2 )
                                                  );

                if ( !pszNavexInf )
                    {
                    UIOutOfMemoryError();
                    goto function_fail;
                    }

                STRCPY ( pszNavexInf, pszAppPath );
                NameAppendFile ( pszNavexInf, szNavexInfFileName );

                if ( VLRegisterVirusDB ( hVGlobalContext,
                                         pszDat,
                                         pszInf,
                                         pszNavexInf ) != VS_OK )
                    {
                    UIControlledInitMessage ( MSG_Config_VLRegisterVirusDB_Failed );
                    goto function_fail;
                    }

                                        // Make sure that we exit
                                        // successfully and we release the
                                        // old buffer containing the
                                        // location of the antivirus
                                        // engine's configuration.

                bResult = TRUE;

                pszTemp = pszNAVAPScan;

                pszNAVAPScan = pszNewPath;

                pszNewPath = pszTemp;

                                        // Free allocated memory and return
                                        // to caller.

function_fail:

                if ( pszNavexInf )
                    {
                    MemFreePtr ( pszNavexInf );
                    }

                if ( pszAppPath )
                    {
                    MemFreePtr ( pszAppPath );
                    }

                if ( pszNewPath )
                    {
                    MemFreePtr ( pszNewPath );
                    }

                if ( pszDat )
                    {
                    MemFreePtr ( pszDat );
                    }

                if ( pszInf )
                    {
                    MemFreePtr ( pszInf );
                    }

                if ( !bResult && hVGlobalContext )
                    {
                    if ( VLScanClose ( hVGlobalContext ) != VS_OK )
                        {
                        SYM_ASSERT ( FALSE );
                        }

                    hVGlobalContext = NULL;
                    }
                }

                                        // Release the exclusive access.

            SymReleaseResource ( &rAVAPIUsage );
            }
        else
            {
            SYM_ASSERT ( FALSE );
            }

        SymInterlockedExchange ( &lNAVAPScanConfigurationLock, 0 );
        }

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) <= 0 );

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// BOOL UnloadNAVAPScanConfiguration (
//      VOID
//      );
//
// This function unloads the AVAPI global context handle, previously
// created by a successfull call to LoadNAVAPScanConfiguration().
//
// Entry:
//      -
//
// Exit:
//      TRUE if successfull, FALSE if error.
//
// Synchronization:
//      This function is non-reentrant, but will quietly fail if it is
//      called simultaneously from different threads.
//      Calling UnloadNAVAPScanConfiguration() while another thread is in
//      LoadNAVAPScanConfiguration() is illegal, and if such sutiation is
//      detected, UnloadNAVAPScanConfiguration() will fail.
//
//---------------------------------------------------------------------------
BOOL UnloadNAVAPScanConfiguration (
    VOID
    )
    {
    auto BOOL bResult;

                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );

                                        // Initialize local variables;

    bResult = FALSE;
                                        // Quick reentrancy check...

    if ( !SymInterlockedExchange ( &lNAVAPScanConfigurationLock, 1 ) )
        {
                                        // Acquire exclusive access to the
                                        // AVAPI handle.  We will have to
                                        // wait until access is granted.

        if ( SymAcquireResourceExclusive ( &rAVAPIUsage, TRUE, 0 ) )
            {
                                        // Make sure we have a valid handle.
                                        // We will be called without an
                                        // existing handle to AVAPI only if
                                        // the previous call to initialize
                                        // the handle had failed.

            if ( hVGlobalContext )
                {
                                        // Release the AVAPI handle and
                                        // invalidate hVGlobalContext.

                if ( VLScanClose ( hVGlobalContext ) != VS_OK )
                    {
                    SYM_ASSERT ( FALSE );
                    }

                hVGlobalContext = NULL;

                                        // Complete the call successfully.

                bResult = TRUE;
                }

                                        // Release the exclusive access.

            SymReleaseResource ( &rAVAPIUsage );
            }
        else
            {
            SYM_ASSERT ( FALSE );
            }

        SymInterlockedExchange ( &lNAVAPScanConfigurationLock, 0 );
        }

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) <= 0 );

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// PNAVAPCFG LockNAVAPConfiguration (
//      VOID
//      );
//
// This function locks the currently active configuration and returns
// a pointer to it.  Once a configuration is locked, it has to be unlocked
// by calling UnlockNAVAPConfiguration().
//
// Entry:
//      -
//
// Exit:
//      Non-NULL pointer to a valid NAVAP configuration structure, or
//      NULL if there was an error or no configuration available for
//      locking.
//
// Synchronization:
//      LockNAVAPConfiguration() is reentrant from different threads.
//
//---------------------------------------------------------------------------
PNAVAPCFG LockNAVAPConfiguration (
    VOID
    )
    {
    auto PNAVAPCFG prResult;

                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );
    SYM_ASSERT ( bNAVAPCfgInitialized );

                                        // Increment the total configuration
                                        // usage count.

    if ( !SymAcquireResourceShared ( &rCfgUsage, FALSE, 0 ) )
        {
        SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );

        return ( NULL );
        }

                                        // Lock the current configuration
                                        // structure and increment its usage
                                        // count.  This usage count is used
                                        // as a "pending delete" flag.

    SymWaitForMutex ( &rCfgLock );

    SYM_ASSERT ( prNAVAPCfg );

    prNAVAPCfg->iUsageCount++;

    prResult = prNAVAPCfg;

    SymReleaseMutex ( &rCfgLock );

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );

    return ( prResult );
    }

//---------------------------------------------------------------------------
//
// VOID UnlockNAVAPConfiguration (
//      PNAVAPCFG prCfg
//      );
//
// This function unlocks the configuration at prCfg.  If the configuration
// was pending for deletion, and the caller was the last client of prCfg,
// it will be released along all resources associated with it.
//
// Entry:
//      prCfg - A non-NULL value returned by LockNAVAPConfiguration().
//
// Exit:
//      -
//
// Synchronization:
//      UnlockNAVAPConfiguration() is reentrant from different threads,
//      and it must be called only from the thread which had earlier
//      locked prCfg by calling LockNAVAPConfiguration().
//
//---------------------------------------------------------------------------
VOID UnlockNAVAPConfiguration (
    PNAVAPCFG prCfg
    )
    {
    auto BOOL bPendingDelete;

                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );
    SYM_ASSERT ( bNAVAPCfgInitialized );

                                        // Validate input parameters.

    SYM_ASSERT ( prCfg );

                                        // Lock the configuration structure.

    SymWaitForMutex ( &rCfgLock );

                                        // Update the usage count of prCfg
                                        // and unlock the configuration
                                        // structure.

                                        // If our client was the last user
                                        // of that configuration and the
                                        // configuration was marked for
                                        // deletion (it is not the same as
                                        // the current configuration), then
                                        // release it.

    SYM_ASSERT ( prNAVAPCfg );

    SYM_ASSERT ( prCfg->iUsageCount > 0 );

    bPendingDelete = --prCfg->iUsageCount == 0 && prCfg != prNAVAPCfg;

    SymReleaseMutex ( &rCfgLock );

    if ( bPendingDelete )
        {
        UnloadConfiguration ( prCfg );
        }

                                        // Update the total configuration
                                        // usage count.

    SymReleaseResource ( &rCfgUsage );

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );
    }

//---------------------------------------------------------------------------
//
// HVCONTEXT LockNAVAPScanConfiguration (
//      VOID
//      );
//
// This function locks the currently active AVAPI global context handle
// and returns it to the client for use.  If there is no AVAPI handle
// available at the moment, LockNAVAPScanConfiguration will return NULL.
// If this function returns a valid AVAPI handle, the client must call
// UnlockNAVAPScanConfiguration() to release the lock when done with
// using AVAPI.
//
// Entry:
//      -
//
// Exit:
//      Non-NULL value with a valid AVAPI global handle, or NULL if
//      the handle was unavailable or there was an error.
//
// Synchronization:
//      LockNAVAPScanConfiguration() is reentrant from different threads.
//
//---------------------------------------------------------------------------
HVCONTEXT LockNAVAPScanConfiguration (
    VOID
    )
    {
                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );
    SYM_ASSERT ( bNAVAPCfgInitialized );

                                        // Acquire access to AVAPI's global
                                        // context handle.

    if ( SymAcquireResourceShared ( &rAVAPIUsage, FALSE, 0 ) )
        {
                                        // If shared access was given to us,
                                        // then examine the status of the
                                        // global AVAPI context handle.  If
                                        // we have a valid handle, then
                                        // return it to the client, leaving
                                        // the guarding resource locked.

        if ( hVGlobalContext )
            {
            SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );

            return ( hVGlobalContext );
            }

                                        // If we don't have a valid handle,
                                        // then don't forget to restore back
                                        // the status of rAVAPIUsage.

        SymReleaseResource ( &rAVAPIUsage );
        }

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );

    return ( NULL );
    }

//---------------------------------------------------------------------------
//
// VOID UnlockNAVAPScanConfiguration (
//      HVCONTEXT hVContext
//      );
//
// This function unlocks the global AVAPI context handle locked earlier by
// LockNAVAPScanConfiguration().
//
// Entry:
//      hVContext - a valid handle returned to the client by an earlier
//                  call to LockNAVAPScanConfiguration().
//
// Exit:
//      -
//
// Synchronization:
//      UnlockNAVAPScanConfiguration() is reentrant from different threads,
//      and it must be called only from the thread which had earlier
//      locked hVContext by calling LockNAVAPScanConfiguration().
//
//---------------------------------------------------------------------------
VOID UnlockNAVAPScanConfiguration (
    HVCONTEXT hVContext
    )
    {
                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );
    SYM_ASSERT ( bNAVAPCfgInitialized );

                                        // Validate input parameters.

    SYM_ASSERT ( hVContext );
    SYM_ASSERT ( hVContext == hVGlobalContext );

                                        // Lock the configuration structure.

    SymReleaseResource ( &rAVAPIUsage );

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );
    }

//---------------------------------------------------------------------------
//
// PNAVAPCFG LoadConfiguration (
//      PTSTR pszPath
//      );
//
// This function loads NAVAP's configuration files and initializes all
// external modules related to that configuration.
//
// LoadConfiguration() will report error and warning messages generated
// during processing.
//
// Entry:
//      pszPath - location of the configuration files.
//                SYM_VXD: in OEM.
//
// Exit:
//      Non-NULL pointer to initialized configuration structure if the
//      configuration was successfully loaded, or NULL if error.
//
//---------------------------------------------------------------------------
PNAVAPCFG LoadConfiguration (
    PTSTR pszPath
    )
    {
    auto PNAVAPCFG prCfg;

                                        // Validate input parameters.

    SYM_ASSERT ( pszPath );
    SYM_VERIFY_STRING ( pszPath );

                                        // Load the configuration.

    if ( prCfg = AllocateAndReadConfiguration ( pszPath ) )
        {
                                        // Set prCfg->pszNAVAPCfg.

        SYM_ASSERT ( !prCfg->pszNAVAPCfg );

        if ( prCfg->pszNAVAPCfg = (PTSTR)
             MemAllocPtr ( GMEM_MOVEABLE,
                           sizeof(TCHAR) * ( STRLEN ( pszPath ) + 1 ) )
           )
            {
            STRCPY ( prCfg->pszNAVAPCfg, pszPath );

                                        // Initialize the exclusion library.

            if ( prCfg->hExcludeN = NAVAPInitExclusionAPI ( pszPath ) )
                {
                return ( prCfg );
                }
            else
                {
                UIControlledInitMessage ( MSG_Config_NAVAPInitExclusionAPI_Failed );
                }

            MemFreePtr ( prCfg->pszNAVAPCfg );

            prCfg->pszNAVAPCfg = NULL;
            }
        else
            {
            UIOutOfMemoryError();
            }

        FreeConfiguration ( prCfg );
        }
    else
        {
        UIControlledInitMessage ( MSG_Config_Unable_To_Read );
        }

    return ( NULL );
    }

//---------------------------------------------------------------------------
//
// VOID UnloadConfiguration (
//      PNAVAPCFG prCfg
//      );
//
// This function uninitializes external module handles associated with the
// NAVAP configuration structure pointed to by prCfg, and then unloads that
// configuration.
//
// Entry:
//      prCfg - Non-NULL pointer to configuration structure returned by
//              LoadConfiguration().
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UnloadConfiguration (
    PNAVAPCFG prCfg
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prCfg );

                                        // Release the exclusion library.

    if ( prCfg->hExcludeN &&
         ! NAVAPCloseExclusionAPI ( prCfg->hExcludeN ) )
        {
        SYM_ASSERT ( FALSE );
        }

    prCfg->hExcludeN = NULL;

                                        // Release prCfg->pszNAVAPCfg.

    if ( prCfg->pszNAVAPCfg )
        {
        MemFreePtr ( prCfg->pszNAVAPCfg );

        prCfg->pszNAVAPCfg = NULL;
        }

                                        // Release the rest of the
                                        // configuration.

    FreeConfiguration ( prCfg );
    }

//---------------------------------------------------------------------------
//
// PNAVAPCFG AllocateAndReadConfiguration (
//      PTSTR pszPath
//      );
//
// This function reads the configuration located at pszPath.  First,
// AllocateAndReadConfiguration() attempts to read the primary configuration
// files.  If they do not exist, then the default configuration is read.
// In this case, the default configuration files are also copied as primary
// configuration files.  If the reading of the default configuration files
// fails, then this function will fail.  If the default configuration files
// were read, AllocateAndReadConfiguration() will report a warning that the
// primary configuration was not found and that the defaults were used.
// Once this function succeeds, you need to release the configuration
// through FreeConfiguration().
//
// AllocateAndReadConfiguration() will report error and warning messages
// generated during processing.
//
// Entry:
//      pszPath - location of the configuration files.
//                SYM_VXD: in OEM.
//
// Exit:
//      This function returns a non-NULL pointer if the configuration was
//      successfully read.  In this case, the returned value will point to
//      a fully initialized NAVAP configuration structure (except external
//      engine handles, which are left uninitialized).  If this function
//      returns NULL, then the configuration was not read because of memory
//      error, or because of I/O error.
//
//---------------------------------------------------------------------------
PNAVAPCFG AllocateAndReadConfiguration (
    PTSTR pszPath
    )
    {
    auto PNAVAPCFG   prCfg;
    auto PNAVOPTIONS prNAVOpts;
    auto UINT        uLength, uLengthDef;
    auto PTSTR       pszFile, pszDef;
    auto BOOL        bReadOK, bUsedDefaults;
    auto DWORD       dwRegKeyType, dwDestinationLength;
    auto DWORD       dwRegValue;
    auto CFGREGKEY   rRegKey = {
                               HKEY_LOCAL_MACHINE,
                               szNAVAPCFGProductName,
                               NULL,
                               szNAVAPCFGProjectName,
                               NULL,
                               NULL,
                               NULL,
                               NULL
                               };

#if defined(SYM_VXD)

    auto PSTR        pszVxDFile, pszVxDDef, pszCustomAlert;

#endif // #if defined(SYM_VXD)

                                        // Validate input parameters.

    SYM_ASSERT ( pszPath );
    SYM_VERIFY_STRING ( pszPath );

                                        // Initialize local variables.

    prCfg = NULL;

    bUsedDefaults = FALSE;

    pszFile = NULL;

    pszDef = NULL;

#if defined(SYM_VXD)

    pszVxDFile = NULL;

    pszVxDDef = NULL;

    pszCustomAlert = NULL;

#endif // #if defined(SYM_VXD)

                                        // Allocate memory for old-style
                                        // configuration data structure

    prNAVOpts = (PNAVOPTIONS) MemAllocPtr ( GHND, sizeof ( *prNAVOpts ) );

    if ( !prNAVOpts )
        {
        goto function_end;
        }

                                        // Determine the length of, and
                                        // allocate buffers for playing
                                        // with filenames.  Make sure we
                                        // account for length of the largest
                                        // file appended with a '\\' to the
                                        // configuration path.

    uLength = max ( STRLEN ( szNAVOPTSFileName ),
                    STRLEN ( szNAVSTARTFileName ) );

    uLengthDef = max ( STRLEN ( szNAVDEFOPTSFileName ),
                       STRLEN ( szNAVDEFSTARTFileName ) );

    uLength = max ( SYM_MAX_PATH, uLength + STRLEN ( pszPath ) + 2 );

    uLengthDef = max ( SYM_MAX_PATH, uLengthDef + STRLEN ( pszPath ) + 2 );

    pszFile = (PTSTR) MemAllocPtr ( GHND, uLength * sizeof(TCHAR) );

    if ( !pszFile )
        {
        goto function_end;
        }

    pszDef = (PTSTR) MemAllocPtr ( GHND, uLengthDef * sizeof(TCHAR) );

    if ( !pszDef )
        {
        goto function_end;
        }

#if defined(SYM_VXD)

    pszVxDFile = (PSTR) MemAllocPtr ( GHND, uLength * sizeof(WCHAR) );

    if ( !pszVxDFile )
        {
        goto function_end;
        }

    pszVxDDef = (PSTR) MemAllocPtr ( GHND, uLengthDef * sizeof(WCHAR) );

    if ( !pszVxDDef )
        {
        goto function_end;
        }

#endif // #if defined(SYM_VXD)

                                        // Read standard configuration file.

    STRCPY ( pszFile, pszPath );
    NameAppendFile ( pszFile, szNAVOPTSFileName );

#if !defined(SYM_VXD)

    if ( !FileExists ( pszFile ) )
        {
        STRCPY ( pszDef, pszPath );
        NameAppendFile ( pszDef, szNAVDEFOPTSFileName );

        FileCopy ( pszDef, pszFile, NULL );

        bUsedDefaults = TRUE;
        }

    bReadOK = ReadNAVOPTSFile ( pszFile, prNAVOpts );

#else

    VxDGenericStringConvert ( pszFile, CHARSET_OEM,
                              pszVxDFile, CHARSET_CURRENT );

    if ( !FileExists ( pszVxDFile ) )
        {
        STRCPY ( pszDef, pszPath );
        NameAppendFile ( pszDef, szNAVDEFOPTSFileName );

        VxDGenericStringConvert ( pszDef, CHARSET_OEM,
                                  pszVxDDef, CHARSET_CURRENT );

        FileCopy ( pszVxDDef, pszVxDFile, NULL );

        bUsedDefaults = TRUE;
        }

    bReadOK = ReadNAVOPTSFile ( pszVxDFile, prNAVOpts );

#endif

    if ( !bReadOK )
        {
        goto skip_reading_startup_and_custom;
        }

                                        // Read startup configuration file.
                                        // Note, that currently only SYM_VXD
                                        // requires that, but generic read
                                        // code is implemented.

#if defined(SYM_VXD)

    STRCPY ( pszFile, pszPath );
    NameAppendFile ( pszFile, szNAVSTARTFileName );

#if !defined(SYM_VXD)

    if ( !FileExists ( pszFile ) )
        {
        STRCPY ( pszDef, pszPath );
        NameAppendFile ( pszDef, szNAVDEFSTARTFileName );

        FileCopy ( pszDef, pszFile, NULL );
        }

    bReadOK = ReadNAVSTARTFile ( pszFile, prNAVOpts );

#else

    VxDGenericStringConvert ( pszFile, CHARSET_OEM,
                              pszVxDFile, CHARSET_CURRENT );

    if ( !FileExists ( pszVxDFile ) )
        {
        STRCPY ( pszDef, pszPath );
        NameAppendFile ( pszDef, szNAVDEFSTARTFileName );

        VxDGenericStringConvert ( pszDef, CHARSET_OEM,
                                  pszVxDDef, CHARSET_CURRENT );

        FileCopy ( pszVxDDef, pszVxDFile, NULL );
        }

    bReadOK = ReadNAVSTARTFile ( pszVxDFile, prNAVOpts );

#endif

#endif // #if defined(SYM_VXD)

                                        // Read long custom alert file.
                                        // Note, that currently only SYM_VXD
                                        // requires that, but generic read
                                        // code is implemented.

#if defined(SYM_VXD)

    STRCPY ( pszFile, pszPath );
    NameAppendFile ( pszFile, szCUSTOMALERTFileName );

#if !defined(SYM_VXD)

    pszCustomAlert = ReadCUSTOMALERTFile ( pszFile );

#else

    VxDGenericStringConvert ( pszFile, CHARSET_OEM,
                              pszVxDFile, CHARSET_CURRENT );

    pszCustomAlert = ReadCUSTOMALERTFile ( pszFile );

#endif

#endif // #if defined(SYM_VXD)

skip_reading_startup_and_custom:

    if ( bReadOK )
        {
                                        // Options read successfully.  Now
                                        // gather miscellaneous settings
                                        // which are not supported by the
                                        // NAVOPTIONS record.  Place these
                                        // settings in fields of NAVOPTIONS
                                        // which are not used by NAVAP, but
                                        // which AllocateAndConvertOptions()
                                        // understands and transfers.

                                        // Read bDeleteInfectedOnCreate and
                                        // place it in
                                        // prNAVOpts->scanner.bMemory.

        prNAVOpts->scanner.bMemory =
            rNAVAPDefaultCfg.bDeleteInfectedOnCreate ? 1 : 0;

        rRegKey.lpszName = szNAVAPCFGbDeleteInfectedOnCreate;

        dwDestinationLength = sizeof(dwRegValue);

        if ( ConfigRegQueryValue ( &rRegKey,
                                   NULL,
                                   &dwRegKeyType,
                                   &dwRegValue,
                                   &dwDestinationLength ) == ERROR_SUCCESS )
            {
            if ( dwRegKeyType == REG_DWORD )
                {
                prNAVOpts->scanner.bMemory = dwRegValue ? 1 : 0;
                }
            }

        if ( FALSE == ReadQuarantinePath( prNAVOpts->quarantineset.szQuarantineDir,
                                          sizeof( prNAVOpts->quarantineset.szQuarantineDir ) ) )
            {
            *prNAVOpts->quarantineset.szQuarantineDir = 0;
            }

#if defined(SYM_VXD)
                                        // Save pointer to the long custom
                                        // alert string in
                                        // prNAVOpts->password.dwPassword.

        SYM_ASSERT ( sizeof(prNAVOpts->password.dwPassword) ==
                     sizeof(PSTR) );

        prNAVOpts->password.dwPassword = (DWORD)pszCustomAlert;

#else

        prNAVOpts->password.dwPassword = NULL;

#endif

                                        // Convert the options.

        prCfg = AllocateAndConvertOptions ( prNAVOpts );

                                        // If one of the default options
                                        // file was read, then notify the
                                        // user.

        if ( prCfg && bUsedDefaults )
            {
            UIControlledInitMessage ( MSG_Config_Load_Defaults_Used );
            }
        }

                                        // Release all allocated memory.

function_end:

#if defined(SYM_VXD)

    if ( pszCustomAlert )
        {
        MemFreePtr ( pszCustomAlert );
        }

    if ( pszVxDDef )
        {
        MemFreePtr ( pszVxDDef );
        }

    if ( pszVxDFile )
        {
        MemFreePtr ( pszVxDFile );
        }

#endif // #if defined(SYM_VXD)

    if ( pszDef )
        {
        MemFreePtr ( pszDef );
        }

    if ( pszFile )
        {
        MemFreePtr ( pszFile );
        }

    if ( prNAVOpts )
        {
        MemFreePtr ( prNAVOpts );
        }

                                        // Return to caller.

    return ( prCfg );
    }

//---------------------------------------------------------------------------
//
// BOOL ReadNAVOPTSFile (
//      PCTSTR      pszCfgFile,
//      PNAVOPTIONS prNAVOpts
//      );
//
// This function reads NAV configuration file (NAVOPTS.DAT) and updates
// the fields of prNAVOpts.
//
// Entry:
//      pszCfgFile - pointer to configuration file
//                   SYM_VXD: in current character set
//      prNAVOpts  - pointer to NAVOPTIONS structure
//
// Exit:
//      TRUE  if successful
//            prNAVOpts contains the configuration
//      FALSE if error
//            prNAVOpts may contain invalid information
//
//---------------------------------------------------------------------------
BOOL ReadNAVOPTSFile (
    PCTSTR      pszCfgFile,
    PNAVOPTIONS prNAVOpts
    )
    {
    auto HFILE        hFile;
    auto NAVBINHEADER rHeader;

                                        // Validate input parameters

    SYM_ASSERT ( pszCfgFile );

#if !defined(SYM_VXD)

    SYM_VERIFY_STRING ( pszCfgFile );

#endif

    SYM_ASSERT ( prNAVOpts );
    SYM_VERIFY_BUFFER ( prNAVOpts, sizeof(*prNAVOpts) );

                                        // Open configuration file

    if ( ( hFile = FileOpen ( pszCfgFile, OF_READ ) ) == HFILE_ERROR )
        {
        return ( FALSE );
        }

                                        // Read file header

    if ( FileRead ( hFile, &rHeader, sizeof(rHeader) ) != sizeof(rHeader) ||
         rHeader.dwVersion != NAV_VERSION_50 ||
         FileRead ( hFile, prNAVOpts, sizeof(_NAVOPTIONS) ) != sizeof(_NAVOPTIONS)
       )
        {
        if ( FileClose ( hFile ) == HFILE_ERROR )
            {
            SYM_ASSERT ( FALSE );
            }

        return ( FALSE );
        }

    if ( FileClose ( hFile ) == HFILE_ERROR )
        {
        SYM_ASSERT ( FALSE );
        }

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL ReadNAVSTARTFile (
//      PCTSTR      pszCfgFile,
//      PNAVOPTIONS prNAVOpts
//      );
//
// This function reads NAV startup configuration file (NAVSTART.DAT) and
// updates the fields of prNAVOpts.
//
// Entry:
//      pszCfgFile - pointer to startup configuration file
//                   SYM_VXD: in current character set
//      prNAVOpts  - pointer to NAVOPTIONS structure
//
// Exit:
//      TRUE  if successful
//            prNAVOpts contains the startup configuration
//      FALSE if error
//            prNAVOpts may contain invalid information
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL ReadNAVSTARTFile (
    PCTSTR      pszCfgFile,
    PNAVOPTIONS prNAVOpts
    )
    {
    auto HFILE          hFile;
    auto NAVSTARTHEADER rHeader;

                                        // Validate input parameters

    SYM_ASSERT ( pszCfgFile );

#if !defined(SYM_VXD)

    SYM_VERIFY_STRING ( pszCfgFile );

#endif

    SYM_ASSERT ( prNAVOpts );
    SYM_VERIFY_BUFFER ( prNAVOpts, sizeof(*prNAVOpts) );

                                        // Open configuration file

    if ( ( hFile = FileOpen ( pszCfgFile, OF_READ ) ) == HFILE_ERROR )
        {
        return ( FALSE );
        }

                                        // Read file header

    if ( FileRead ( hFile, &rHeader, sizeof(rHeader) ) != sizeof(rHeader) ||
         rHeader.dwVersion != NAVSTART_VERSION ||
         FileRead ( hFile, &prNAVOpts->startup, sizeof(STARTOPT) ) != sizeof(STARTOPT)
       )
        {
        if ( FileClose ( hFile ) == HFILE_ERROR )
            {
            SYM_ASSERT ( FALSE );
            }

        return ( FALSE );
        }

    if ( FileClose ( hFile ) == HFILE_ERROR )
        {
        SYM_ASSERT ( FALSE );
        }

    return ( TRUE );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// PSTR ReadCUSTOMALERTFile (
//      PCTSTR pszCustomAlertFile
//      );
//
// This function reads the NAV custom alert file.
//
// Entry:
//      pszCustomAlertFile - pointer to a string containing the name of
//                           the custom alert file.
//                           SYM_VXD: in current character set
//
// Exit:
//      Returns a pointer to a MemAllocPtr()-ed string containing the long
//      custom alert.  NULL if file does not exist, or if error.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

PSTR ReadCUSTOMALERTFile (
    PCTSTR pszCustomAlertFile
    )
    {
    auto HFILE hFile;
    auto DWORD dwFileSize;
    auto PSTR  pszAlert;

                                        // Validate input parameters.

    SYM_ASSERT ( pszCustomAlertFile );

#if !defined(SYM_VXD)

    SYM_VERIFY_STRING ( pszCustomAlertFile );

#endif

                                        // Open the file with custom
                                        // alert.

    if ( ( hFile = FileOpen ( pszCustomAlertFile, OF_READ ) ) !=
           HFILE_ERROR )
        {
                                        // Get the length of the alert.

        if ( ( dwFileSize = FileLength ( hFile ) ) != (DWORD)-1 )
            {
                                        // Allocate buffer for alert.

            if ( pszAlert = (PSTR) MemAllocPtr ( GHND,
                                                 dwFileSize + sizeof(char) )
               )
                {
                                        // Read the alert.

                if ( FileRead ( hFile, pszAlert, dwFileSize ) == dwFileSize )
                    {
                    if ( FileClose ( hFile ) == HFILE_ERROR )
                        {
                        SYM_ASSERT ( FALSE );
                        }

                    return ( pszAlert );
                    }

                MemFreePtr ( pszAlert );
                }
            else
                {
                UIOutOfMemoryError();
                }
            }

        if ( FileClose ( hFile ) == HFILE_ERROR )
            {
            SYM_ASSERT ( FALSE );
            }
        }

    return ( NULL );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// BOOL ReadQuarantinePath(
//      PSTR    pszQuarantinePath,
//      UINT    cchBufferSize
//      );
//
// This function reads the quarantine path.
//
// Entry:
//      pszQuarantinePath  - Buffer to receive quarantine path.
//      cchBufferSize      - Buffer size in characters.
//
// Exit:
//      TRUE on success. Otherwise, FALSE and pszQuarntinePath contents
//      undefined.
//
//---------------------------------------------------------------------------

BOOL ReadQuarantinePath(
    PSTR    pszQuarantinePath,
    UINT    cchBufferSize
    )
    {
#if defined(SYM_VXD)
    return ( GetQuarantinePath ( pszQuarantinePath,
                                 cchBufferSize,
                                 TARGET_QUARANTINE_INCOMING_PATH ) );
#else
    auto    TCHAR   szQPath [ MAX_QUARANTINE_FILENAME_LEN ];
    auto    BOOL    bSuccess;

    // Note: GetQuarantinePath takes size of buffer in BYTES, not characters.

    if ( GetQuarantinePath( szQPath, sizeof( szQPath ), TARGET_QUARANTINE_INCOMING_PATH ) )
        {
        bSuccess = WideCharToMultiByte ( CP_OEMCP,
                                         WC_COMPOSITECHECK,
                                         szQPath,
                                         -1,
                                         pszQuarantinePath,
                                         cchBufferSize,
                                         NULL,
                                         NULL ) == 0 ? FALSE : TRUE;
        }
    else
        {
        bSuccess = FALSE;
        }

    return ( bSuccess );
#endif
    }

//---------------------------------------------------------------------------
//
// PNAVAPCFG AllocateAndConvertOptions (
//      PNAVOPTIONS prNAVOptions
//      );
//
// This function converts the options from NAVOPTIONS structure to internal
// NAVAP format.  The returned value will be a pointer to a structure
// containing the options in NAVAPCFG format.  Invalid options at
// prNAVOptions will not fail this function.  Instead, default values will
// be used.  To release the configuration, use FreeOptions().
//
// Entry:
//      prNAVOptions - pointer to NAVOPTIONS structure
//
// Exit:
//      Returns a pointer to NAVAPCFG structure, if successful.  NULL
//      result indicates out of memory error.
//
// Note:
//      Only SYM_VXD is, and should be utilizing the startup options in the
//      input structure.
//
//---------------------------------------------------------------------------
PNAVAPCFG AllocateAndConvertOptions (
    PNAVOPTIONS prNAVOptions
    )
    {
    auto PNAVAPCFG prCfg;
    char sNoExt[2];
    BOOL bTempScanAllFiles;


                                        // Validate input parameters.

    SYM_ASSERT ( prNAVOptions );
    SYM_VERIFY_BUFFER ( prNAVOptions, sizeof(*prNAVOptions) );

                                        // Transfer startup options - these
                                        // are statically allocated.

#if defined(SYM_VXD)

    bNAVAPCfgStartupScanMemory        = prNAVOptions->tsr.bScanMem;
    bNAVAPCfgStartupScanUpperMemory   = prNAVOptions->general.bScanHighMemory;
    bNAVAPCfgStartupRunDOSAutoProtect = prNAVOptions->startup.bLoadTSR;
#endif

#if defined(UNICODE)
    TCHAR   szTempPath[SYM_MAX_PATH*2] = {0};
#else
    char    szTempPath[SYM_MAX_PATH*2] = {0};
#endif

                                        // Transfer security configuration
                                        // of NAVAP.

    bNAVAPCanBeDisabled = prNAVOptions->tsr.bUnloadable;

                                        // Allocate memory for main
                                        // configuration structure.

    prCfg = (PNAVAPCFG) MemAllocPtr ( GHND, sizeof(*prCfg) );

    if ( !prCfg )
        {
        UIOutOfMemoryError();

        return ( NULL );
        }

                                        // Transfer file scan options (known
                                        // virus detection)

    prCfg->uFileHeuristicLevel     = prNAVOptions->tsr.bEnableHeuristicScan ?
                                        prNAVOptions->tsr.uHeuristicLevel : 0;

    prCfg->bScanFileOnExecute      = prNAVOptions->tsr.bScanRun;
    prCfg->bScanFileOnOpen         = prNAVOptions->tsr.bScanOpen;
    prCfg->bScanFileOnCreate       = prNAVOptions->tsr.bScanCreate;

    prCfg->bScanAllFiles           = prNAVOptions->tsr.bScanAll;

    KE2M ( prCfg->auScanFileExeActions,
           prNAVOptions->tsr.uActionKnown,
           prNAVOptions->customAP.uchFile,
           rNAVAPDefaultCfg.auScanFileExeActions );


    KM2M ( prCfg->auScanFileMacroActions,
           prNAVOptions->tsr.uActionKnown,
           prNAVOptions->customAP.uchMacro,
           rNAVAPDefaultCfg.auScanFileMacroActions );

    prCfg->uScanFilePrompts        = ( prNAVOptions->tsr.bDispStop ?
                                          fPROMPTACTION_STOP : 0 )          |
                                     ( prNAVOptions->tsr.bDispContinue ?
                                          fPROMPTACTION_CONTINUE : 0 )      |
                                     ( prNAVOptions->tsr.bDispDelete ?
                                          fPROMPTACTION_DELETE : 0 )        |
                                     ( prNAVOptions->tsr.bDispRepair ?
                                          fPROMPTACTION_REPAIR : 0 )        |
                                     ( prNAVOptions->tsr.bDispExclude ?
                                          fPROMPTACTION_EXCLUDE : 0 )       |
                                     ( prNAVOptions->tsr.bDispQuarantine ?
                                          fPROMPTACTION_QUARANTINE : 0 );

    prCfg->bDeleteInfectedOnCreate = prNAVOptions->scanner.bMemory;

                                        // Transfer device scan options

    prCfg->bScanDeviceUponAccess       = prNAVOptions->tsr.bChkFlopAccess;
    prCfg->bScanBootDeviceUponBoot     = prNAVOptions->tsr.bChkFlopOnBoot;
    prCfg->bScanNonBootDevicesUponBoot = prNAVOptions->tsr.bChkBothFlops;

    KB2M ( prCfg->auScanDeviceUponAccessActions,
           prNAVOptions->tsr.uActionKnown,
           prNAVOptions->customAP.uchBoot,
           rNAVAPDefaultCfg.auScanDeviceUponAccessActions );

    KBB2M ( prCfg->auScanDeviceUponBootActions,
            prNAVOptions->tsr.uActionKnown,
            prNAVOptions->customAP.uchBoot,
            rNAVAPDefaultCfg.auScanDeviceUponBootActions );

    prCfg->uScanDevicePrompts = ( prNAVOptions->tsr.bDispContinue ?
                                     fPROMPTACTION_CONTINUE : 0 )      |
                                ( prNAVOptions->tsr.bDispRepair ?
                                     fPROMPTACTION_REPAIR : 0 );

                                        // Transfer behavior block options

    BB2M ( prCfg->auFormatHardDiskActions,
           prNAVOptions->tsr.uFormat,
           rNAVAPDefaultCfg.auFormatHardDiskActions );

    BB2M ( prCfg->auWriteToHDBootRecordsActions,
           prNAVOptions->tsr.uWriteHard,
           rNAVAPDefaultCfg.auWriteToHDBootRecordsActions );

    BB2M ( prCfg->auWriteToFDBootRecordActions,
           prNAVOptions->tsr.uWriteFlop,
           rNAVAPDefaultCfg.auWriteToFDBootRecordActions );

    BB2M ( prCfg->auWriteToProgramFilesActions,
           prNAVOptions->tsr.uWriteExe,
           rNAVAPDefaultCfg.auWriteToProgramFilesActions );

    BB2M ( prCfg->auROAttributeClearActions,
           prNAVOptions->tsr.uROAttrib,
           rNAVAPDefaultCfg.auROAttributeClearActions );

    prCfg->uBehaviorBlockPrompts = rNAVAPDefaultCfg.uBehaviorBlockPrompts;

                                        // Transfer file repair options

    prCfg->bBackupBeforeRepair = prNAVOptions->general.bBackupRep;

                                        // Transfer quarantine directory.

    if ( ! ( prCfg->pszQuarantineLocation =
             TranslateString ( prNAVOptions->quarantineset.szQuarantineDir ) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

                                        // Transfer move directory.
                                        // Currently we are going to use the
                                        // quarantine directory, since a
                                        // field in the configuration file is
                                        // not reserved.

    if ( ! ( prCfg->pszMoveLocation =
             TranslateString ( prNAVOptions->quarantineset.szQuarantineDir ) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

                                        // Transfer the file extension used
                                        // when rename action is performed.
                                        // Currently we are going to use the
                                        // backup extension, since a field in
                                        // the configuration file is not
                                        // reserved.

    if ( ! ( prCfg->pszRenameExtension =
             TranslateString ( prNAVOptions->general.szBackExt ) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

                                        // Transfer workstation alert options

    prCfg->bDisplayAlert      = prNAVOptions->alert.bDispAlertMsg;

    prCfg->bAudibleAlert      = prNAVOptions->alert.bBeep;

    prCfg->uAudibleAlertDelay = rNAVAPDefaultCfg.uAudibleAlertDelay;

    prCfg->uAlertTimeOut      = prNAVOptions->alert.bRemoveAfter ?
                                prNAVOptions->alert.uSeconds : 0;

    if ( ! ( prCfg->pszAlertText =
             TranslateString ( prNAVOptions->alert.szAlertMsg ) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

    // Scan files with no extension flag
    // Set default to scan no extension files in case the following code fails
    prCfg->bScanNoExtFile = TRUE;

    if ( GetPath(szTempPath, sizeof(szTempPath) / sizeof(szTempPath[0])) )
    {
#if defined(UNICODE)
           wcscat(szTempPath, _T(NAVOPTX_FILE));
#else
           strcat(szTempPath, NAVOPTX_FILE);
#endif

        if (TRUE == ParseValueFromFile(NAVOPTX_SECT_SCANOPTS,
                                  NAVOPTX_SCAN_NOEXT_FILES,
                                  szTempPath,
                                  sNoExt,
                                  1))
        {
            if ('0' == sNoExt[0])
                prCfg->bScanNoExtFile = FALSE;
        }

    }

    // User-defined extensions
    // Save off current setting and set scan all files to TRUE in case the following code fails
    bTempScanAllFiles = prCfg->bScanAllFiles;
    prCfg->bScanAllFiles = TRUE;

    if ( GetPath(szTempPath, sizeof(szTempPath) / sizeof(szTempPath[0])) )
    {
#if defined(UNICODE)
           wcscat(szTempPath, _T(NAVOPTX_FILE));
#else
           strcat(szTempPath, NAVOPTX_FILE);
#endif

        if (TRUE == ParseValueFromFile(NAVOPTX_SECT_EXTENSIONS,
                                  NAVOPTX_USER_DEFINED_EXTENSIONS,
                                  szTempPath,
                                  szExtensionList,
                                  MAX_EXTENSION_LIST_SIZE))
        {
            // If this succeeds, recover the original scan all files setting
            prCfg->bScanAllFiles = bTempScanAllFiles;
        }
        else
        {
            strcpy(szExtensionList, "");
        }
    }

                                            // Transfer list of program
                                        // extensions.
    if ( ! ( prCfg->pszzProgramExtensionList =
             TranslateProgramFileExtensions ( szExtensionList ) )
       )
    {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
    }

                                        // Transfer long custom alert.

    SYM_ASSERT ( sizeof(PSTR) == sizeof(prNAVOptions->password.dwPassword) );

    if ( prNAVOptions->password.dwPassword )
        {
        if ( ! ( prCfg->pszLongCustomAlert =
                 TranslateString ( (PSTR) prNAVOptions->password.dwPassword ) )
           )
            {
            SYM_ASSERT ( FALSE );

            FreeOptions ( prCfg );

            return ( NULL );
            }
        }
    else
        {
        prCfg->pszLongCustomAlert = NULL;
        }

    return ( prCfg );
    }

//---------------------------------------------------------------------------
//
// VOID FreeOptions (
//      PNAVAPCFG prCfg
//      );
//
// This function releases the configuration structure returned by
// AllocateAndConvertOptions().  After this function is called, the pointer
// passed at prCfg can no longer be used.
//
// Entry:
//      prCfg - a non-NULL result from AllocateAndConvertOptions()
//
// Exit:
//      All memory allocated for prCfg freed up.
//
//---------------------------------------------------------------------------
VOID FreeOptions (
    PNAVAPCFG prCfg
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prCfg );

                                        // Release memory allocated for
                                        // different buffers needed by
                                        // the configuration.

    if ( prCfg->pszLongCustomAlert )
        {
        MemFreePtr ( prCfg->pszLongCustomAlert );
        }

    if ( prCfg->pszzProgramExtensionList )
        {
        MemFreePtr ( prCfg->pszzProgramExtensionList );
        }

    if ( prCfg->pszAlertText )
        {
        MemFreePtr ( prCfg->pszAlertText );
        }

    if ( prCfg->pszRenameExtension )
        {
        MemFreePtr ( prCfg->pszRenameExtension );
        }

    if ( prCfg->pszMoveLocation )
        {
        MemFreePtr ( prCfg->pszMoveLocation );
        }

    if ( prCfg->pszQuarantineLocation )
        {
        MemFreePtr ( prCfg->pszQuarantineLocation );
        }

                                        // Release the configuration
                                        // structure itself.

    MemFreePtr ( prCfg );
    }

//---------------------------------------------------------------------------
//
// VOID KE2M (
//      PMAINACTION puActions,
//      UINT        uKnownVirusAction,
//      UINT        uCustomAction,
//      PMAINACTION puDefaultActions
//      );
//
// This function converts the known exe-virus action enumerators from
// NAVOPTS.DAT to MAINACTION format.
//
// Entry:
//      puActions         - pointer to an array of MAINACTIONs. On exit, this
//                          will be the sequence of actions to take.
//                          See notes in apcfg.h.
//      uKnownVirusAction - known virus action enumerator in NAVOPTS.DAT
//                          standard.
//      uCustomAction     - known virus action enumerator in NAVOPTS.DAT
//                          standard (specific to exe virus types), used
//                          when uKnownVirusAction specifies custom action.
//      puDefaultActions  - default sequence of actions if uKnownVirusAction
//                          and/or uCustomAction is invalid.
//
// Exit:
//      puActions has sequence of actions to take in MAINACTION format.
//
//---------------------------------------------------------------------------
VOID KE2M (
    PMAINACTION puActions,
    UINT        uKnownVirusAction,
    UINT        uCustomAction,
    PMAINACTION puDefaultActions
    )
    {
    auto    int     i;

    puActions[1] = MAINACTION_LAST;

    switch ( uKnownVirusAction )
        {
        case TSR_PROMPT:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_PROMPT;
#else
            puActions[0] = MAINACTION_DENYACCESS;
#endif
            break;

        case TSR_DENY:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_DENYACCESS_NOTIFY;
#else
            puActions[0] = MAINACTION_DENYACCESS;
#endif
            break;

        case TSR_DENYQUIET:

            puActions[0] = MAINACTION_DENYACCESS;
            break;

        case TSR_REPAIR:

            puActions[0] = MAINACTION_REPAIR;
            break;

        case TSR_DELETE:

            puActions[0] = MAINACTION_DELETE;
            break;

        case TSR_HALTCPU:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_SHUTDOWN_NOTIFY;
#else
            puActions[0] = MAINACTION_DENYACCESS;
#endif
            break;

        case TSR_QUARANTINEQUIET:

            puActions[0] = MAINACTION_QUARANTINE;
            break;

        case TSR_QUAR_CANT_REP:

            puActions[0] = MAINACTION_REPAIR;
            puActions[1] = MAINACTION_QUARANTINE;
            puActions[2] = MAINACTION_LAST;
            break;

        case TSR_MOVEQUIET:

            puActions[0] = MAINACTION_MOVE;
            break;

        case TSR_RENAMEQUIET:

            puActions[0] = MAINACTION_RENAME;
            break;

        case TSR_CUSTOM_RESPONSE:

            SYM_ASSERT ( uCustomAction != TSR_CUSTOM_RESPONSE );

            if ( uCustomAction != TSR_CUSTOM_RESPONSE )
                {
                KE2M( puActions, uCustomAction, 0, puDefaultActions );
                break;
                }
            // else fall through...

        default:

            SYM_ASSERT ( FALSE );

                                        // Copy default sequence of actions.

            for ( i = 0; i < MAX_RESPONSE_ACTIONS; i++ )
                {
                puActions[i] = puDefaultActions[i];
                }
            break;
        }
    }

//---------------------------------------------------------------------------
//
// VOID KM2M (
//      PMAINACTION puActions,
//      UINT        uKnownVirusAction,
//      UINT        uCustomAction,
//      PMAINACTION puDefaultActions
//      );
//
// This function converts the known macro-virus action enumerators from
// NAVOPTS.DAT to MAINACTION format.
//
// Entry:
//      puActions         - pointer to an array of MAINACTIONs. On exit, this
//                          will be the sequence of actions to take.
//                          See notes in apcfg.h.
//      uKnownVirusAction - known virus action enumerator in NAVOPTS.DAT
//                          standard.
//      uCustomAction     - known virus action enumerator in NAVOPTS.DAT
//                          standard (specific to macro virus types), used
//                          when uKnownVirusAction specifies custom action.
//      puDefaultActions  - default sequence of actions if uKnownVirusAction
//                          and/or uCustomAction is invalid.
//
// Exit:
//      puActions has sequence of actions to take in MAINACTION format.
//
//---------------------------------------------------------------------------
VOID KM2M (
    PMAINACTION puActions,
    UINT        uKnownVirusAction,
    UINT        uCustomAction,
    PMAINACTION puDefaultActions
    )
    {
    auto    int     i;

    puActions[1] = MAINACTION_LAST;

    switch ( uKnownVirusAction )
        {
        case TSR_PROMPT:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_PROMPT;
#else
            puActions[0] = MAINACTION_DENYACCESS;
#endif
            break;

        case TSR_DENY:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_DENYACCESS_NOTIFY;
#else
            puActions[0] = MAINACTION_DENYACCESS;
#endif
            break;

        case TSR_DENYQUIET:

            puActions[0] = MAINACTION_DENYACCESS;
            break;

        case TSR_REPAIR:

            puActions[0] = MAINACTION_REPAIR;
            break;

        case TSR_DELETE:

            puActions[0] = MAINACTION_DELETE;
            break;

        case TSR_HALTCPU:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_SHUTDOWN_NOTIFY;
#else
            puActions[0] = MAINACTION_DENYACCESS;
#endif
            break;

        case TSR_QUARANTINEQUIET:

            puActions[0] = MAINACTION_QUARANTINE;
            break;

        case TSR_QUAR_CANT_REP:

            puActions[0] = MAINACTION_REPAIR;
            puActions[1] = MAINACTION_QUARANTINE;
            puActions[2] = MAINACTION_LAST;
            break;

        case TSR_MOVEQUIET:

            puActions[0] = MAINACTION_MOVE;
            break;

        case TSR_RENAMEQUIET:

            puActions[0] = MAINACTION_RENAME;
            break;

        case TSR_CUSTOM_RESPONSE:

            SYM_ASSERT ( uCustomAction != TSR_CUSTOM_RESPONSE );

            if ( uCustomAction != TSR_CUSTOM_RESPONSE )
                {
                KM2M ( puActions, uCustomAction, 0, puDefaultActions );
                break;
                }
            // else fall through...

        default:

            SYM_ASSERT ( FALSE );
                                        // Copy default sequence of actions.

            for ( i = 0; i < MAX_RESPONSE_ACTIONS; i++ )
                {
                puActions[i] = puDefaultActions[i];
                }
            break;
        }
    }

//---------------------------------------------------------------------------
//
// VOID KB2M (
//      PMAINACTION puActions,
//      UINT        uKnownVirusAction,
//      UINT        uCustomAction,
//      PMAINACTION puDefaultActions
//      );
//
// This function converts the known boot-virus action enumerators from
// NAVOPTS.DAT to MAINACTION format.
//
// Entry:
//      puActions         - pointer to an array of MAINACTIONs. On exit, this
//                          will be the sequence of actions to take.
//                          See notes in apcfg.h.
//      uKnownVirusAction - known virus action enumerator in NAVOPTS.DAT
//                          standard.
//      uCustomAction     - known virus action enumerator in NAVOPTS.DAT
//                          standard (specific to booy virus types), used
//                          when uKnownVirusAction specifies custom action.
//      puDefaultActions  - default sequence of actions if uKnownVirusAction
//                          and/or uCustomAction is invalid.
//
// Exit:
//      puActions has sequence of actions to take in MAINACTION format.
//
//---------------------------------------------------------------------------
VOID KB2M (
    PMAINACTION puActions,
    UINT        uKnownVirusAction,
    UINT        uCustomAction,
    PMAINACTION puDefaultActions
    )
    {
    auto    int     i;

    puActions[1] = MAINACTION_LAST;

    switch ( uKnownVirusAction )
        {
        case TSR_PROMPT:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_PROMPT;
#else
            puActions[0] = MAINACTION_CONTINUE;
#endif
            break;

        case TSR_DENY:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_CONTINUE_NOTIFY;
#else
            puActions[0] = MAINACTION_CONTINUE;
#endif
            break;

        case TSR_DENYQUIET:

            puActions[0] = MAINACTION_CONTINUE;
            break;

        case TSR_REPAIR:

            puActions[0] = MAINACTION_REPAIR;
            break;

        case TSR_DELETE:

            puActions[0] = MAINACTION_REPAIR;
            break;

        case TSR_HALTCPU:

#if defined(SYM_VXD)
            puActions[0] = MAINACTION_SHUTDOWN_NOTIFY;
#else
            puActions[0] = MAINACTION_CONTINUE;
#endif
            break;

        case TSR_QUARANTINEQUIET:

            puActions[0] = MAINACTION_REPAIR;
            break;

        case TSR_QUAR_CANT_REP:

            puActions[0] = MAINACTION_REPAIR;
            break;

        case TSR_MOVEQUIET:

            puActions[0] = MAINACTION_CONTINUE_NOTIFY;
            break;

        case TSR_RENAMEQUIET:

            puActions[0] = MAINACTION_CONTINUE_NOTIFY;
            break;

        case TSR_CUSTOM_RESPONSE:

            SYM_ASSERT ( uCustomAction != TSR_CUSTOM_RESPONSE );

            if ( uCustomAction != TSR_CUSTOM_RESPONSE )
                {
                KB2M ( puActions, uCustomAction, 0, puDefaultActions );
                break;
                }
            // else fall through...

        default:

            SYM_ASSERT ( FALSE );

                                        // Copy default sequence of actions.

            for ( i = 0; i < MAX_RESPONSE_ACTIONS; i++ )
                {
                puActions[i] = puDefaultActions[i];
                }
            break;
        }
    }

//---------------------------------------------------------------------------
//
// VOID KBB2M (
//      PMAINACTION puActions,
//      UINT        uKnownVirusAction,
//      UINT        uCustomAction,
//      PMAINACTION puDefaultActions
//      );
//
// This function converts the known boot-virus found on boot action
// enumerators from NAVOPTS.DAT to MAINACTION format.
//
// Entry:
//      puActions         - pointer to an array of MAINACTIONs. On exit, this
//                          will be the sequence of actions to take.
//                          See notes in apcfg.h.
//      uKnownVirusAction - known virus action enumerator in NAVOPTS.DAT
//                          standard.
//      uCustomAction     - known virus action enumerator in NAVOPTS.DAT
//                          standard (specific to booy virus types), used
//                          when uKnownVirusAction specifies custom action.
//      puDefaultActions  - default sequence of actions if uKnownVirusAction
//                          and/or uCustomAction is invalid.
//
// Exit:
//      puActions has sequence of actions to take in MAINACTION format.
//
//---------------------------------------------------------------------------
VOID KBB2M (
    PMAINACTION puActions,
    UINT        uKnownVirusAction,
    UINT        uCustomAction,
    PMAINACTION puDefaultActions
    )
    {
    auto    int     i;

                                        // Get known boot actions, but change
                                        // shutdown to notify, since shutdown
                                        // is not available during device scan
                                        // on boot.

    KB2M ( puActions, uKnownVirusAction, uCustomAction, puDefaultActions );

    for ( i = 0; i < MAX_RESPONSE_ACTIONS; i++ )
        {
        if ( MAINACTION_SHUTDOWN_NOTIFY == puActions[i] )
            {
            puActions[i] = MAINACTION_CONTINUE_NOTIFY;
            }
        }
    }


//---------------------------------------------------------------------------
//
// VOID BB2M (
//      PMAINACTION puActions,
//      UINT        uBlockBehaviorAction,
//      PMAINACTION puDefaultActions
//      );
//
// This function converts the block behavior action enumerators from
// NAVOPTS.DAT to MAINACTION format.
//
// Entry:
//      uBlockBehaviorAction - block behavior action enumerator in
//                             NAVOPTS.DAT standard.
//      uDefaultAction       - default action if uBlockBehaviorAction is
//                             unknown
//
// Exit:
//      Returns uBlockBehaviorAction in MAINACTION format.
//
//---------------------------------------------------------------------------
VOID BB2M (
    PMAINACTION puActions,
    UINT        uBlockBehaviorAction,
    PMAINACTION puDefaultActions
    )
    {
    auto    int     i;

    puActions[1] = MAINACTION_LAST;

    switch ( uBlockBehaviorAction )
        {
        case TSR_ADV_ALLOW:

            puActions[0] = MAINACTION_ALLOW;
            break;

        case TSR_ADV_PROMPT:

            puActions[0] = MAINACTION_PROMPT;
            break;

        case TSR_ADV_DONT:

            puActions[0] = MAINACTION_DONOTALLOW_NOTIFY;
            break;

        default:

            SYM_ASSERT ( FALSE );
                                        // Copy default sequence of actions.

            for ( i = 0; i < MAX_RESPONSE_ACTIONS; i++ )
                {
                puActions[i] = puDefaultActions[i];
                }
            break;
        }
    }

//---------------------------------------------------------------------------
//
// PTSTR TranslateString (
//      PSTR pszNAVFormat
//      );
//
// This function converts a string from NAVOPTS format to one useable
// by NAVAP.  The useable format when TCHAR == WCHAR is Unicode, and
// when TCHAR == char is OEM.
//
// Entry:
//      pszNAVFormat - ASCIIZ string from NAVOPTs, in OEM.
//
// Exit:
//      Pointer to buffer with converted string.  This buffer is allocated
//      through MemAllocPtr(), and once the client is done with it, should
//      free the memory.  If the memory allocation or the string conversion
//      failed, the returned value will be NULL.
//
//---------------------------------------------------------------------------
PTSTR TranslateString (
    PSTR pszNAVFormat
    )
    {
    auto PTSTR pszResult;

                                        // Validate input parameters.

    SYM_ASSERT ( pszNAVFormat );
    SYM_VERIFY_STRING ( pszNAVFormat );

                                        // Allocate memory for converted
                                        // string.

    if ( pszResult = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) *
                                                 ( strlen(pszNAVFormat) + 1 ) )
       )
        {

                                        // Convert the string.
#if defined(UNICODE)

        if ( MultiByteToWideChar ( CP_OEMCP,
                                   0,
                                   pszNAVFormat,
                                   -1,
                                   pszResult,
                                   strlen(pszNAVFormat) + 1 ) == 0 )
            {
            MemFreePtr ( pszResult );

            return ( NULL );
            }

#else

        strcpy ( pszResult, pszNAVFormat );

#endif
        }
    else
        {
        UIOutOfMemoryError();
        }

    return ( pszResult );
    }

//---------------------------------------------------------------------------
//
// PTSTR TranslateProgramFileExtensions (
//      PSTR pszzNAVFormat
//      );
//
// This function converts the list of program extensions from NAVOPTS
// format to one useable by IsFileInListOfExtensions() in the event manager
// of NAVAP.  The returned strings will be in Unicode if TCHAR == WCHAR,
// or OEM if TCHAR == char.
//
// Entry:
//      pszzNAVFormat - list of extensions from NAVOPTs, in OEM.
//
// Exit:
//      Pointer to the beginning of an array of ASCIIZ strings, formatted
//      for the event manager.  The buffer with the strings is allocated
//      through MemAllocPtr(), and once the client is done with it, should
//      free the memory.  If the memory allocation or the string conversion
//      failed, the returned value will be NULL.
//
//---------------------------------------------------------------------------
PTSTR TranslateProgramFileExtensions (
    PSTR pszzNAVFormat
    )
    {
    auto PTSTR pszzResult, pszTerminate;
    auto UINT  uLength;

                                        // Allocate memory for extensions.

    uLength = strlen ( pszzNAVFormat ) + 2;

    if ( pszzResult = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) * uLength ) )
        {
#if defined(UNICODE)

        if ( MultiByteToWideChar ( CP_OEMCP,
                                   0,
                                   pszzNAVFormat,
                                   -1,
                                   pszzResult,
                                   uLength - 1 ) == 0 )
            {
            MemFreePtr ( pszzResult );

            return ( NULL );
            }

#else

        strcpy ( pszzResult, pszzNAVFormat );

#endif

                                        // Replace all spaces with zeros.

        pszTerminate = pszzResult;

        while ( pszTerminate = STRCHR ( pszTerminate, ' ' ) )
            {
            *pszTerminate = '\0';
            pszTerminate++;
            }
        }
    else
        {
        UIOutOfMemoryError();
        }

    return ( pszzResult );
    }

//---------------------------------------------------------------------------
//
// PTSTR GetNAVAPConfigurationDirectory (
//      pszEnforced
//      );
//
// This function returns NAVAP's configuration directory.  This directory
// is retrieved from the registry and is assumed to be the same as the NAV
// installation directory.
//
// Entry:
//      pszEnforced - pointer to a string containing a path to override
//                    the registry settings.  NULL if client wants this
//                    function to determine the configuration on its own.
//                    SYM_VXD: in OEM.
//
// Exit:
//      Pointer to a MemAllocPtr-ed string containing the path to NAVAP's
//      configuration files.  NULL if error.  SYM_VXD: result is in OEM.
//
// Synchronization:
//      This function is fully reentrant.
//
//---------------------------------------------------------------------------
PTSTR GetNAVAPConfigurationDirectory (
    PTSTR pszEnforced
    )
    {
    auto   PTSTR     pszResult;
    auto   DWORD     dwRegKeyType, dwDestinationLength;

#if defined(SYM_VXD)
    auto   char      szRegEntry[SYM_MAX_PATH * sizeof(WCHAR)];
#else
    auto   TCHAR     szRegEntry[SYM_MAX_PATH];
#endif

    static CFGREGKEY rNAVAPRegistryLocation = {
                                              HKEY_LOCAL_MACHINE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              szNAVAPInstallPathRegSection,
                                              szNAVAPInstallPathRegKeyname,
                                              NULL
                                              };

                                        // If path is enforced on us, then
                                        // return it.

    if ( pszEnforced )
        {
        SYM_VERIFY_STRING ( pszEnforced );

        if ( pszResult = (PTSTR)
             MemAllocPtr ( GMEM_MOVEABLE,
                           sizeof(TCHAR) * ( STRLEN ( pszEnforced ) + 1 ) )
           )
            {
            STRCPY ( pszResult, pszEnforced );

            return ( pszResult );
            }

        UIOutOfMemoryError();

        return ( NULL );
        }

                                        // Allocate memory for result.  We
                                        // will currently allow up to
                                        // SYM_MAX_PATH characters.

    if ( pszResult = (PTSTR)
         MemAllocPtr ( GMEM_MOVEABLE, sizeof(TCHAR) * SYM_MAX_PATH )
       )
        {
                                        // Query the Registry.  This entry
                                        // will be in ANSI (SYM_VXD) or
                                        // Unicode (SYM_NTK).

        dwDestinationLength = sizeof(szRegEntry);

        if ( ConfigRegQueryValue ( &rNAVAPRegistryLocation,
                                   NULL,
                                   &dwRegKeyType,
                                   szRegEntry,
                                   &dwDestinationLength ) == ERROR_SUCCESS )
            {

#if defined(SYM_VXD)

            auto char szRegConvert[SYM_MAX_PATH * sizeof(WCHAR)];

            VxDGenericStringConvert ( szRegEntry, CHARSET_ANSI,
                                      szRegConvert, CHARSET_CURRENT );

            TrueNameGet ( szRegConvert, szRegEntry );

            VxDGenericStringConvert ( szRegEntry, CHARSET_CURRENT,
                                      pszResult, CHARSET_OEM );

            if ( STRLEN ( pszResult ) < SYM_MAX_PATH )
                {
                return ( pszResult );
                }

#else

            if ( STRLEN ( szRegEntry ) < SYM_MAX_PATH )
                {
                STRCPY ( pszResult, szRegEntry );

                return ( pszResult );
                }

#endif
            }

        MemFreePtr ( pszResult );
        }
    else
        {
        UIOutOfMemoryError();
        }

    return ( NULL );
    }

//---------------------------------------------------------------------------
//
// BOOL GetNAVAPScanDirectory (
//      PTSTR pszEnforced
//      );
//
// This function returns antivirus engine's configuration directory.  This
// directory is retrieved from the registry and is assumed to be the same
// as the NAV installation directory.
//
// Entry:
//      pszEnforced - pointer to a string containing a path to override
//                    the registry settings.  NULL if client wants this
//                    function to determine the configuration on its own.
//                    SYM_VXD: in OEM.
//
// Exit:
//      Pointer to a MemAllocPtr-ed string containing the path to antivirus
//      engine's configuration files.  NULL if error.  SYM_VXD: result is
//      in OEM.
//
// Synchronization:
//      This function is fully reentrant.
//
// Note:
//      Currently this function queries the registry for the NAVAP's
//      configuration directory.
//
//---------------------------------------------------------------------------
PTSTR GetNAVAPScanDirectory (
    PTSTR pszEnforced
    )
    {
    auto   PTSTR     pszResult;
    auto   DWORD     dwRegKeyType, dwDestinationLength;

#if defined(SYM_VXD)
    auto   char      szRegEntry[SYM_MAX_PATH * sizeof(WCHAR)];
#else
    auto   TCHAR     szRegEntry[SYM_MAX_PATH];
#endif

    static CFGREGKEY rNAVAPRegistryLocation = {
                                              HKEY_LOCAL_MACHINE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              szNAVAPDefPathRegSection,
                                              szNAVAPDefPathRegKeyname,
                                              NULL
                                              };

                                        // If path is enforced on us, then
                                        // return it.

    if ( pszEnforced )
        {
        SYM_VERIFY_STRING ( pszEnforced );

        if ( pszResult = (PTSTR)
             MemAllocPtr ( GMEM_MOVEABLE,
                           sizeof(TCHAR) * ( STRLEN ( pszEnforced ) + 1 ) )
           )
            {
            STRCPY ( pszResult, pszEnforced );

            return ( pszResult );
            }

        UIOutOfMemoryError();

        return ( NULL );
        }

                                        // Allocate memory for result.  We
                                        // will currently allow up to
                                        // SYM_MAX_PATH characters.

    if ( pszResult = (PTSTR)
         MemAllocPtr ( GMEM_MOVEABLE, sizeof(TCHAR) * SYM_MAX_PATH )
       )
        {
                                        // Query the Registry.  This entry
                                        // will be in ANSI (SYM_VXD) or
                                        // Unicode (SYM_NTK).

        dwDestinationLength = sizeof(szRegEntry);

        if ( ConfigRegQueryValue ( &rNAVAPRegistryLocation,
                                   NULL,
                                   &dwRegKeyType,
                                   szRegEntry,
                                   &dwDestinationLength ) == ERROR_SUCCESS )
            {

#if defined(SYM_VXD)

            auto char szRegConvert[SYM_MAX_PATH * sizeof(WCHAR)];

            VxDGenericStringConvert ( szRegEntry, CHARSET_ANSI,
                                      szRegConvert, CHARSET_CURRENT );

            TrueNameGet ( szRegConvert, szRegEntry );

            VxDGenericStringConvert ( szRegEntry, CHARSET_CURRENT,
                                      pszResult, CHARSET_OEM );

            if ( STRLEN ( pszResult ) < SYM_MAX_PATH )
                {
                return ( pszResult );
                }

#else

            if ( STRLEN ( szRegEntry ) < SYM_MAX_PATH )
                {
                STRCPY ( pszResult, szRegEntry );

                return ( pszResult );
                }

#endif
            }

        MemFreePtr ( pszResult );
        }
    else
        {
        UIOutOfMemoryError();
        }

    return ( NULL );
    }

//---------------------------------------------------------------------------
//
// PTSTR GetNAVAPStartupName (
//      PTSTR pszNAVAPCfg
//      );
//
// This function retrieves the full name of NAVAP's executable.  In SYM_VXD,
// the executable is assumed to be szNAVAPFileName[] located in NAVAP's
// configuration directory.  In SYM_NTK, the startup executable is derived
// from the RegistryPath paramter to DriverEntry().
//
// Entry:
//      pszNAVAPCfg - pointer to NAVAP's configuration.
//
// Exit:
//      Pointer to a MemAllocPtr-ed string containing the full name of
//      NAVAP's executable.  NULL if error.  SYM_VXD: result is in OEM.
//
// Synchronization:
//      This function is fully reentrant.
//
//---------------------------------------------------------------------------
PTSTR GetNAVAPStartupName (
    PTSTR pszNAVAPCfg
    )
    {
    auto PTSTR pszResult;

                                        // Validate input parameters.

    SYM_ASSERT ( pszNAVAPCfg );
    SYM_VERIFY_STRING ( pszNAVAPCfg );

                                        // Allocate memory for result.  We
                                        // will currently allow up to
                                        // SYM_MAX_PATH characters.

    if ( pszResult = (PTSTR)
         MemAllocPtr ( GMEM_MOVEABLE, sizeof(TCHAR) * SYM_MAX_PATH )
       )
        {

#if defined(SYM_VXD)

                                        // Copy pszNAVAPCfg to the
                                        // destination buffer.

        if ( STRLEN ( pszNAVAPCfg ) &&
             ( STRLEN ( pszNAVAPCfg ) +
               STRLEN ( szNAVAPFileName ) +
               STRLEN ( _T("\\") ) ) < SYM_MAX_PATH
           )
            {
            STRCPY ( pszResult, pszNAVAPCfg );

            NameAppendFile ( pszResult, szNAVAPFileName );

            return ( pszResult );
            }

#elif defined(SYM_NTK)

        if ( STRLEN ( pszNAVAPSYSStartupName ) &&
             STRLEN ( pszNAVAPSYSStartupName ) < SYM_MAX_PATH )
            {
            STRCPY ( pszResult, pszNAVAPSYSStartupName );

            return ( pszResult );
            }

#endif

        MemFreePtr ( pszResult );
        }
    else
        {
        UIOutOfMemoryError();
        }

    return ( NULL );
    }

//---------------------------------------------------------------------------
//
// VOID WINAPI GetNAVEXConfigInfo(
//      PTSTR  lpszID,
//      LPVOID lpvResult,
//      UINT   uMaxResultLen,
//      LPBOOL lpbImplemented,
//      LPVOID lpvCookie
//      );
//
// This function is an override of the default NAVEX configuration callback
// for AVAPI. Currently, it implemented only for retrieving the file heuristic
// scanning level.
// The default callbacks are overridden in LoadNAVAPScanConfiguration().
//
// Entry:
//      lpszID         - Text identifier of configuration option being queried.
//      lpvResult      - Pointer to result buffer, format depends on option queried.
//      uMaxResultLen  - Size of result buffer in bytes.
//      lpbImplemented - Pointer to value to set if option implemented.
//      lpvCookie      - Pointer to application data (unused - unable to pass
//                       application data through from AVAPI top layer).
//
// Exit:
//      If querying _T("heur") value for file heuristic level,
//          *lpszImplemented is TRUE. lpvResult has single-byte string
//          indicating scanning level - "0" (off) to "3" (highest).
//
//      Otherwise, *lpszImpemented is FALSE.
//
// Synchronization:
//      This function is fully reentrant.
//
//---------------------------------------------------------------------------
VOID WINAPI GetNAVEXConfigInfo (
    PTSTR  lpszID,
    LPVOID lpvResult,
    UINT   uMaxResultLen,
    LPBOOL lpbImplemented,
    LPVOID lpvCookie
    )
    {
    PNAVAPCFG prCurrentNAVAPCfg;

                                        // Default to option not implemented.

    *lpbImplemented = FALSE;

                                        // Check "heur" for file heuristic scanning
                                        // level. lpvResult expects single-byte (char)
                                        // string for level.
                                        // "0" (off) to "3" (highest).

    if ( 0 == STRICMP ( lpszID, _T("heur") ) )
        {
        if ( prCurrentNAVAPCfg = LockNAVAPConfiguration() )
            {
            ((PSTR) lpvResult)[0] = '0' + prCurrentNAVAPCfg->uFileHeuristicLevel;
            ((PSTR) lpvResult)[1] = 0;

            *lpbImplemented = TRUE;

            UnlockNAVAPConfiguration ( prCurrentNAVAPCfg );
            }
        }

    }


//************************************************************************
// GetPath()
//
// Get the path of the NAV directory.
//
// Returns:
//      TRUE        on success.
//      FALSE       failure
//************************************************************************
BOOL GetPath(TCHAR * szTemp, int size)
{
    auto HKEY   hKey = 0;
    auto BOOL   bRet = FALSE;

    MEMSET (szTemp, 0, size);


    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Symantec\\InstalledApps"), 0, KEY_READ, &hKey))
    {
#if defined(SYM_VXD)
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAV95"), 0, 0, (LPBYTE)szTemp, (LPDWORD)&size))
#else
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAVNT"), 0, 0, (LPBYTE)szTemp, (LPDWORD)&size))
#endif
        {
            bRet = TRUE;
            RegCloseKey(hKey);
        }
    }


    return ( bRet );
}


//************************************************************************
// ParseValueFromFile()
//
// Gets a value from an INI-type file.
//
// Returns:
//      TRUE        on success.
//      FALSE       failure
//
// NOTES:
//      This code assumes that all strings passed to it are single-byte!
//************************************************************************
BOOL ParseValueFromFile(char *szSection, char *szKey, PCTSTR pszCfgFile, char *pszData, DWORD nDataLen)
{
    HFILE        hFile;
    char         *pszFile, *pszDataStart, *pszKeyPtr, *pszDataEnd, *pszSectionStart, *pszTempSection;
    DWORD        nFileLength;


    // Validate input parameters
    SYM_ASSERT ( pszCfgFile );

#if !defined(SYM_VXD)
    SYM_VERIFY_STRING ( pszCfgFile );
#endif

    if ( ( hFile = FileOpen ( pszCfgFile, OF_READ ) ) == HFILE_ERROR )
        return ( FALSE );

    if ( ( nFileLength = FileLength(hFile) ) == -1)
        return ( FALSE );

    // Allocate memory to store the file to parse
    pszFile = (char *) MemAllocPtr ( GHND, nFileLength * sizeof(char) );

    if ((char *)NULL == pszFile)
        return ( FALSE );

    if ( FileRead ( hFile, pszFile, nFileLength ) != nFileLength)
    {
        if ( FileClose ( hFile ) == HFILE_ERROR )
            SYM_ASSERT ( FALSE );

        MemFreePtr ( (void *)pszFile );
        return ( FALSE );
    }

    if ( FileClose ( hFile ) == HFILE_ERROR )
        SYM_ASSERT ( FALSE );

    // Find the section name
    pszSectionStart = strstr(pszFile, szSection);

    if (pszSectionStart != (char *)NULL)
    {
        // Advance to the beginning of the next section and plug a NULL in
        pszTempSection = pszSectionStart;

        while (*pszTempSection != '\0' && *pszTempSection != '[')
            pszTempSection++;

        *pszTempSection = '\0';

        // Find the key within the section
        pszDataStart = pszSectionStart;

        pszKeyPtr = strstr(pszDataStart, szKey);

        if (pszKeyPtr != (char *)NULL)
        {
            pszDataStart = pszKeyPtr;

            // Advance past the value name and the equals sign to get to the data
            pszDataStart += (strlen(szKey) + 1);

            pszDataEnd = pszDataStart;

            // Copy the data to the passed in buffer
            while (*pszDataEnd != 0xD && *pszDataEnd != '\0' && (pszDataEnd - pszDataStart) < nDataLen)
            {
                *pszData++ = *pszDataEnd++;
            }
            *pszData = '\0';
        }
        else
        {
            // Couldn't find the key within the file
            MemFreePtr( (void *)pszFile );
            return (FALSE);
        }
    }
    else
    {
        // Couldn't find the section within the file
        MemFreePtr( (void *)pszFile );
        return (FALSE);
    }


    MemFreePtr ( (void *)pszFile );

    return ( TRUE );
}
