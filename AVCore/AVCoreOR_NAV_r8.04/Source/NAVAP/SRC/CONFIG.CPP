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
//      GetNAVAPStartupName()               (local function)
//
//      LoadConfiguration()                 (local function)
//      UnloadConfiguration()               (local function)
//
//      AllocateAndConvertOptions()         (local function)
//      FreeOptions()                       (local function)
//
//      GetNAVEXConfigInfo()                (local function)
//
//      TranslateOEMString()                (local function)
//      TranslateOEMStringZ()               (local function)
//
// See Also:
//      init.cpp, exit.cpp
//
// Note:
//
//***************************************************************************

#include "platform.h"
#include "symsync.h"
#include "symcfg.h"
#include "xapi.h"
#include "navap.h"
#include "navapi.h"
#include "apexcl.h"
#include "apstrs.h"
#include "navcb.h"

//---------------------------------------------------------------------------
//
// prNAVAPCfg contains a pointer to the currently active NAVAP
// configuration.
//
// pszNAVAPStartupName points to a string containing the full file spec
// of the NAVAP driver.
//
// pszNAVEXINFPath points to a string containing the full path to the
// NAVEX15.INF file.
//
// hVGlobalContext contains a handle to a global AVAPI context.
//
// Note:
//      SYM_VXD: pszNAVAPStartupName are in OEM.
//
//---------------------------------------------------------------------------
PNAVAPCFG prNAVAPCfg = NULL;
PTSTR     pszNAVAPStartupName = NULL;
PTSTR     pszNAVEXINFPath = NULL;
HVCONTEXT hVGlobalContext = NULL;

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
// rAVAPIUsage guards changes to hVGlobalContext.
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
PTSTR GetNAVAPStartupName (
    PSTR pszPath
    );

PNAVAPCFG LoadConfiguration (
    PNAVAP_CONFIG prConfig
    );

VOID UnloadConfiguration (
    PNAVAPCFG prCfg
    );

PNAVAPCFG AllocateAndConvertOptions (
    PNAVAP_CONFIG prConfig
    );

VOID FreeOptions (
    PNAVAPCFG prCfg
    );

VOID WINAPI GetNAVEXConfigInfo (
    PTSTR  lpszID,
    LPVOID lpvResult,
    UINT   uMaxResultLen,
    LPBOOL lpbImplemented,
    LPVOID lpvCookie
    );

PTSTR TranslateOEMString (
    PSTR pszString
    );

PTSTR TranslateOEMStringZ (
    PSTR pszzString
    );

//---------------------------------------------------------------------------
//
// Define inline functions.
//
//---------------------------------------------------------------------------

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
//      PNAVAP_CONFIG prConfig
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
//      prConfig - Pointer to NAVAP_CONFIG.
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
    PNAVAP_CONFIG prConfig
    )
    {
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
                                        // Initialize pszNAVAPStartupName.

                SYM_ASSERT ( !pszNAVAPStartupName );

                if ( pszNAVAPStartupName = GetNAVAPStartupName ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszNAVAPLocation] ) )
                    {
                                        // Initialize pszNAVEXINFPath.

                    SYM_ASSERT ( !pszNAVEXINFPath );

                    if ( pszNAVEXINFPath = TranslateOEMString ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszNAVEXINFLocation] ) )
                        {
                                        // Load the configuration and make it
                                        // current.

                        SYM_ASSERT ( !prNAVAPCfg );

                        if ( prNAVAPCfg = LoadConfiguration ( prConfig ) )
                            {
                                            // Initialize AVAPI.

                            if ( !prConfig->idx_pszAVAPILocation ||
                                 LoadNAVAPScanConfiguration ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszAVAPILocation] ) )
                                {
                                        // Transfer startup options - these
                                        // are statically allocated and are
                                        // read only when the configuration
                                        // is initialized.

#if defined(SYM_VXD)

                                bNAVAPCfgStartupScanMemory        = prConfig->dw_bStartupScanMemory;
                                bNAVAPCfgStartupScanUpperMemory   = prConfig->dw_bStartupScanUpperMemory;
                                bNAVAPCfgStartupRunDOSAutoProtect = prConfig->dw_bStartupRunDOSAutoProtect;

#endif

                                SYM_ASSERT ( bNAVAPCfgInitialized = TRUE );

                                SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );

                                return ( TRUE );
                                }

                            UnloadConfiguration ( prNAVAPCfg );

                            prNAVAPCfg = NULL;
                            }

                        MemFreePtr ( pszNAVEXINFPath );

                        pszNAVEXINFPath = NULL;
                        }
                    else
                        {
                        UIOutOfMemoryError();
                        }

                    MemFreePtr ( pszNAVAPStartupName );

                    pszNAVAPStartupName = NULL;
                    }
                else
                    {
                    UIControlledInitMessage ( MSG_Unable_To_Determine_Startup_Name );
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

                                        // Release other memory.

    SYM_ASSERT ( pszNAVEXINFPath );
    SYM_ASSERT ( pszNAVAPStartupName );

    MemFreePtr ( pszNAVEXINFPath );
    MemFreePtr ( pszNAVAPStartupName );

    pszNAVEXINFPath = NULL;
    pszNAVAPStartupName = NULL;

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
//      PNAVAP_CONFIG prConfig
//      );
//
// This function loads new NAVAP configuration specified at prConfig and
// makes it current (which means that any consecutive calls to
// LockNAVAPConfiguration() will return a pointer to the new configuration).
// The old configuration is marked as pending for deletion, and is released
// as soon as its usage count becomes 0.
//
// ReloadNAVAPConfiguration() will report any warning or error messages
// that might be generated while processing.
//
// Entry:
//      prConfig - Pointer to NAVAP_CONFIG structure.
//
// Exit:
//      TRUE  if configuration was successfully read and all related
//            external modules were initialized.
//      FALSE if the function failed to initialize the configuration.
//            In this case, the old configuration will be left intact.
//
// Synchronization:
//      ReloadNAVAPConfiguration() is non-reentrant, but will quietly fail
//      if it is called simultaneously from different threads.
//
//---------------------------------------------------------------------------
BOOL ReloadNAVAPConfiguration (
    PNAVAP_CONFIG prConfig
    )
    {
    auto PNAVAPCFG prNewCfg, prOldCfg;

                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );
    SYM_ASSERT ( bNAVAPCfgInitialized );

                                        // Quick reentrancy check...

    if ( !SymInterlockedExchange ( &lReloadNAVAPConfigurationLock, 1 ) )
        {
                                        // Load the new configuration.

        if ( prOldCfg = LockNAVAPConfiguration() )
            {
            if ( prNewCfg = LoadConfiguration ( prConfig ) )
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
//      PSTR pszPath
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
//      pszPath - Location of AVAPI's files (in OEM).
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
    PSTR pszPath
    )
    {
                                        // Convert pszPath to native string.

    SYM_ASSERT ( pszPath );

    auto PTSTR pszNAVAPScan = TranslateOEMString ( pszPath );

    if ( !pszNAVAPScan )
        {
        UIOutOfMemoryError();

        return FALSE;
        }

                                        // Make sure we are called properly.

    SYM_ASSERT ( SymInterlockedDecrement ( &lNAVAPCfgInitDeInitSync ) );

                                        // Initialize local variables.

    auto BOOL bResult = FALSE;

                                        // Quick reentrancy check...

    if ( !SymInterlockedExchange ( &lNAVAPScanConfigurationLock, 1 ) )
        {
                                        // Acquire exclusive access to the
                                        // AVAPI handle.

        if ( SymAcquireResourceExclusive ( &rAVAPIUsage, TRUE, 0 ) )
            {
            auto PTSTR pszDat, pszInf, pszNavexInf;

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

                pszDat = NULL;

                pszInf = NULL;

                pszNavexInf = NULL;

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

                                        // Tell the AVAPI callbacks the
                                        // definition location.

                NAVSetDefsPath ( pszNAVAPScan );

                                        // Initialize the virus database
                                        // files.

                pszDat = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) *
                                                     ( STRLEN(pszNAVAPScan) +
                                                       STRLEN(szVirusDatabaseFileName) + 2 )
                                             );

                if ( !pszDat )
                    {
                    UIOutOfMemoryError();
                    goto function_fail;
                    }

                pszInf = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) *
                                                     ( STRLEN(pszNAVAPScan) +
                                                       STRLEN(szVirusInfoFileName) + 2 )
                                             );

                if ( !pszInf )
                    {
                    UIOutOfMemoryError();
                    goto function_fail;
                    }

                STRCPY ( pszDat, pszNAVAPScan );
                STRCPY ( pszInf, pszNAVAPScan );

                NameAppendFile ( pszDat, szVirusDatabaseFileName );
                NameAppendFile ( pszInf, szVirusInfoFileName );

                                        // Initialize the NAVEX15.INF file.
                                        // At this point we should have
                                        // pszNAVEXInfPath.

                SYM_ASSERT ( pszNAVEXINFPath );

                pszNavexInf = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) *
                                                          ( STRLEN(pszNAVEXINFPath) +
                                                            STRLEN(szNavexInfFileName) + 2 )
                                                  );

                if ( !pszNavexInf )
                    {
                    UIOutOfMemoryError();
                    goto function_fail;
                    }

                STRCPY ( pszNavexInf, pszNAVEXINFPath );
                NameAppendFile ( pszNavexInf, szNavexInfFileName );

                if ( VLRegisterVirusDB ( hVGlobalContext,
                                         pszDat,
                                         pszInf,
                                         pszNavexInf ) != VS_OK )
                    {
                    UIControlledInitMessage ( MSG_Config_VLRegisterVirusDB_Failed );
                    goto function_fail;
                    }

                                        // We have completed the
                                        // initialization.

                bResult = TRUE;

                                        // Free allocated memory and return
                                        // to caller.

function_fail:

                if ( pszNavexInf )
                    {
                    MemFreePtr ( pszNavexInf );
                    }

                if ( pszInf )
                    {
                    MemFreePtr ( pszInf );
                    }

                if ( pszDat )
                    {
                    MemFreePtr ( pszDat );
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

    MemFreePtr ( pszNAVAPScan );

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

                                        // Unlock the configuration
                                        // structure.

    SymReleaseResource ( &rAVAPIUsage );

    SYM_ASSERT ( SymInterlockedIncrement ( &lNAVAPCfgInitDeInitSync ) );
    }

//---------------------------------------------------------------------------
//
// PTSTR GetNAVAPStartupName (
//      PSTR pszPath
//      );
//
// This function retrieves the full name of NAVAP's executable.  In SYM_VXD,
// the executable is assumed to be szNAVAPFileName[] located in the path
// specifed at pszPath.  In SYM_NTK, the startup executable is derived from
// the RegistryPath paramter to DriverEntry().
//
// Entry:
//      pszPath - pointer to NAVAP's location (used only in SYM_VXD),
//                in OEM.
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
    PSTR pszPath
    )
    {
    auto PTSTR pszResult;

                                        // Allocate memory for result.  We
                                        // will currently allow up to
                                        // SYM_MAX_PATH characters.

    if ( pszResult = (PTSTR)
         MemAllocPtr ( GMEM_MOVEABLE, sizeof(TCHAR) * SYM_MAX_PATH )
       )
        {

#if defined(SYM_VXD)

                                        // Validate input parameters.

        SYM_ASSERT ( pszPath );
        SYM_VERIFY_STRING ( pszPath );

                                        // Copy pszPath to the destination
                                        // buffer.

        if ( STRLEN ( pszPath ) &&
             ( STRLEN ( pszPath ) +
               STRLEN ( szNAVAPFileName ) +
               STRLEN ( _T("\\") ) ) < SYM_MAX_PATH
           )
            {
            STRCPY ( pszResult, pszPath );

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
// PNAVAPCFG LoadConfiguration (
//      PNAVAP_CONFIG prConfig
//      );
//
// This function loads NAVAP's configuration files and initializes all
// external modules related to that configuration.
//
// LoadConfiguration() will report error and warning messages generated
// during processing.
//
// Entry:
//      prConfig - Pointer to NAVAP_CONFIG structure.
//
// Exit:
//      Non-NULL pointer to initialized configuration structure if the
//      configuration was successfully loaded, or NULL if error.
//
//---------------------------------------------------------------------------
PNAVAPCFG LoadConfiguration (
    PNAVAP_CONFIG prConfig
    )
    {
    auto PNAVAPCFG prCfg;

                                        // Validate input parameters.

    SYM_ASSERT ( prConfig );

                                        // Load the configuration.

    if ( prCfg = AllocateAndConvertOptions ( prConfig ) )
        {
                                        // Initialize the exclusion library.

            if ( prCfg->hExcludeN = NAVAPInitExclusionAPI ( prConfig->dwExclusionItems,
                                                            &((PBYTE)prConfig)[prConfig->idx_arExclusionItems],
                                                            prConfig->dwExclusionLItems,
                                                            &((PBYTE)prConfig)[prConfig->idx_arExclusionLItems] ) )
                {
                return ( prCfg );
                }
            else
                {
                UIControlledInitMessage ( MSG_Config_NAVAPInitExclusionAPI_Failed );
                }

        FreeOptions ( prCfg );
        }
    else
        {
                                        // ~~~TBD~~~ Display proper message.

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

                                        // Release the rest of the
                                        // configuration.

    FreeOptions ( prCfg );
    }

//---------------------------------------------------------------------------
//
// PNAVAPCFG AllocateAndConvertOptions (
//      PNAVAP_CONFIG prConfig
//      );
//
// This function converts the options from NAVAP_CONFIG structure to internal
// NAVAP format.  The returned value will be a pointer to a structure
// containing the options in NAVAPCFG format.  Invalid options at
// prConfig will not fail this function.  Instead, default values will be
// used.  To release the configuration, use FreeOptions().
//
// Entry:
//      prConfig - Pointer to NAVAP_CONFIG structure.
//
// Exit:
//      Returns a pointer to NAVAPCFG structure, if successful.  NULL
//      result indicates out of memory error.
//
//---------------------------------------------------------------------------
PNAVAPCFG AllocateAndConvertOptions (
    PNAVAP_CONFIG prConfig
    )
    {
    auto PNAVAPCFG prCfg;

                                        // Validate input parameters.

    SYM_ASSERT ( prConfig );

                                        // Transfer security configuration
                                        // of NAVAP.

    bNAVAPCanBeDisabled = prConfig->dw_bNAVAPCanBeDisabled;

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

    prCfg->uFileHeuristicLevel     = prConfig->dw_uFileHeuristicLevel;

    prCfg->bScanFileOnExecute      = prConfig->dw_bScanFileOnExecute;
    prCfg->bScanFileOnOpen         = prConfig->dw_bScanFileOnOpen;
    prCfg->bScanFileOnCreate       = prConfig->dw_bScanFileOnCreate;

    prCfg->bScanAllFiles           = prConfig->dw_bScanAllFiles;

    MEMCPY ( prCfg->auScanFileExeActions, prConfig->adw_auScanFileExeActions, sizeof(prCfg->auScanFileExeActions) );
    MEMCPY ( prCfg->auScanFileMacroActions, prConfig->adw_auScanFileMacroActions, sizeof(prCfg->auScanFileMacroActions) );

    prCfg->uScanFilePrompts = prConfig->dw_uScanFilePrompts;

    prCfg->bDeleteInfectedOnCreate = prConfig->dw_bDeleteInfectedOnCreate;

                                        // Transfer device scan options

    prCfg->bScanDeviceUponAccess       = prConfig->dw_bScanDeviceUponAccess;
    prCfg->bScanBootDeviceUponBoot     = prConfig->dw_bScanBootDeviceUponBoot;
    prCfg->bScanNonBootDevicesUponBoot = prConfig->dw_bScanNonBootDevicesUponBoot;

    MEMCPY ( prCfg->auScanDeviceUponAccessActions, prConfig->adw_auScanDeviceUponAccessActions, sizeof(prCfg->auScanDeviceUponAccessActions) );
    MEMCPY ( prCfg->auScanDeviceUponBootActions, prConfig->adw_auScanDeviceUponBootActions, sizeof(prCfg->auScanDeviceUponBootActions) );

    prCfg->uScanDevicePrompts = prConfig->dw_uScanDevicePrompts;

                                        // Transfer behavior block options

    MEMCPY ( prCfg->auFormatHardDiskActions, prConfig->adw_auFormatHardDiskActions, sizeof(prCfg->auFormatHardDiskActions) );
    MEMCPY ( prCfg->auWriteToHDBootRecordsActions, prConfig->adw_auWriteToHDBootRecordsActions, sizeof(prCfg->auWriteToHDBootRecordsActions) );
    MEMCPY ( prCfg->auWriteToFDBootRecordActions, prConfig->adw_auWriteToFDBootRecordActions, sizeof(prCfg->auWriteToFDBootRecordActions) );
    MEMCPY ( prCfg->auWriteToProgramFilesActions, prConfig->adw_auWriteToProgramFilesActions, sizeof(prCfg->auWriteToProgramFilesActions) );
    MEMCPY ( prCfg->auROAttributeClearActions, prConfig->adw_auROAttributeClearActions, sizeof(prCfg->auROAttributeClearActions) );

    prCfg->uBehaviorBlockPrompts = prConfig->dw_uBehaviorBlockPrompts;

                                        // Transfer file repair options

    prCfg->bBackupBeforeRepair = prConfig->dw_bBackupBeforeRepair;

                                        // Transfer quarantine directory.

    if ( ! ( prCfg->pszQuarantineLocation =
             TranslateOEMString ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszQuarantineLocation]) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

                                        // Transfer move directory.

    if ( ! ( prCfg->pszMoveLocation =
             TranslateOEMString ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszMoveLocation]) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

                                        // Transfer the file extension used
                                        // when rename action is performed.

    if ( ! ( prCfg->pszRenameExtension =
             TranslateOEMString ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszRenameExtension]) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

                                        // Transfer workstation alert options

    prCfg->bDisplayAlert      = prConfig->dw_bDisplayAlert;
    prCfg->bAudibleAlert      = prConfig->dw_bAudibleAlert;
    prCfg->uAudibleAlertDelay = prConfig->dw_uAudibleAlertDelay;
    prCfg->uAlertTimeOut      = prConfig->dw_uAlertTimeOut;

    if ( ! ( prCfg->pszAlertText =
             TranslateOEMString ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszAlertText]) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

                                        // Transfer list of program
                                        // extensions.

    if ( ! ( prCfg->pszzProgramExtensionList =
             TranslateOEMStringZ ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszzProgramExtensionList]) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
        }

                                        // Transfer long custom alert.

    if ( prConfig->idx_pszLongCustomAlert )
        {
        if ( ! ( prCfg->pszLongCustomAlert =
                 TranslateOEMString ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszLongCustomAlert]) )
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

                                        // Transfer spool file location.

    if ( ! ( prCfg->pszSpoolFileLocation =
             TranslateOEMString ( (PSTR) &((PBYTE)prConfig)[prConfig->idx_pszSpoolFileLocation]) )
       )
        {
        SYM_ASSERT ( FALSE );

        FreeOptions ( prCfg );

        return ( NULL );
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

    if ( prCfg->pszSpoolFileLocation )
        {
        MemFreePtr ( prCfg->pszSpoolFileLocation );
        }

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

//---------------------------------------------------------------------------
//
// PTSTR TranslateOEMString (
//      PSTR pszString
//      );
//
// This function converts an OEM string to native character set.
//
// Entry:
//      pszString - ASCIIZ string to convert.
//
// Exit:
//      Pointer to buffer with converted string.  This buffer is allocated
//      through MemAllocPtr(), and once the client is done with it, should
//      free the memory.  If the memory allocation or the string conversion
//      failed, the returned value will be NULL.
//
//---------------------------------------------------------------------------
PTSTR TranslateOEMString (
    PSTR pszString
    )
    {
    auto PTSTR pszResult;

                                        // Validate input parameters.

    SYM_ASSERT ( pszString );
    SYM_VERIFY_STRING ( pszString );

                                        // Allocate memory for converted
                                        // string.

    if ( pszResult = (PTSTR) MemAllocPtr ( GHND, sizeof(TCHAR) *
                                                 ( strlen(pszString) + 1 ) )
       )
        {

                                        // Convert the string.
#if defined(UNICODE)

        if ( MultiByteToWideChar ( CP_OEMCP,
                                   0,
                                   pszString,
                                   -1,
                                   pszResult,
                                   strlen(pszString) + 1 ) == 0 )
            {
            MemFreePtr ( pszResult );

            return ( NULL );
            }

#else

        strcpy ( pszResult, pszString );

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
// PTSTR TranslateOEMStringZ (
//      PSTR pszzString
//      );
//
// This function converts an ASCIIZ OEM string to native character set.
//
// Entry:
//      pszzString - Pointer to ASCIIZ string.
//
// Exit:
//      Pointer to buffer with converted string.  This buffer is allocated
//      through MemAllocPtr(), and once the client is done with it, should
//      free the memory.  If the memory allocation or the string conversion
//      failed, the returned value will be NULL.
//
//---------------------------------------------------------------------------
PTSTR TranslateOEMStringZ (
    PSTR pszzString
    )
    {
    SYM_ASSERT ( pszzString );

                                        // Determine the total length of the
                                        // string.

    auto PSTR pszParse = pszzString;
    auto UINT uLength = 2;

    while ( *pszParse )
        {
        auto UINT uSegmentLength = strlen ( pszParse ) + 1;

        uLength += uSegmentLength;
        pszParse = &pszParse[uSegmentLength];
        }

                                        // Allocate memory for converted
                                        // string.

    auto PTSTR pszzResult;

    if ( pszzResult = (PTSTR) MemAllocPtr ( GHND, sizeof(WCHAR) * uLength ) )
        {
                                        // Convert each string and append it
                                        // to the result.

        auto PTSTR pszAppend = pszzResult;

        while ( *pszzString )
            {
#if defined(UNICODE)

                if ( MultiByteToWideChar ( CP_OEMCP,
                                           0,
                                           pszzString,
                                           -1,
                                           pszAppend,
                                           strlen(pszzString) + 1 ) == 0 )
                    {
                    MemFreePtr ( pszzResult );

                    return ( NULL );
                    }

#else

                strcpy ( pszAppend, pszzString );

#endif
                                        // Go to the next string.

            pszAppend = &pszAppend[STRLEN(pszAppend) + 1];
            pszzString = &pszzString[strlen(pszzString) + 1];
            }
        }
    else
        {
        UIOutOfMemoryError();
        }

    return ( pszzResult );
    }
