// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/INIT.CPv   1.5   29 Sep 1998 18:46:36   RStanev  $
//
// Description:
//      This file defines NAVAP's initialization routines.
//
// Contains:
//      NAVAPGlobalInitEx()
//      NAVAPGlobalInit()                       (local function)
//      UserStackGlobalInit()                   (local function)
//      NAVAPGlobalInitPlatformSpecific()       (local function)
//      NAVAPControlledInit()
//
// See Also:
//      exit.cpp, config.cpp
//
// Note:
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/INIT.CPv  $
// 
//    Rev 1.5   29 Sep 1998 18:46:36   RStanev
// Added fast clean cache.
//
//    Rev 1.4   17 Feb 1998 22:37:38   RStanev
// Updated copyright.
//
//    Rev 1.3   05 Jul 1997 20:12:08   RStanev
// New stack swapping architecture.
//
//    Rev 1.2   21 May 1997 04:52:30   RStanev
// NAVAPControlledInit() now performs startup tasks.
//
//    Rev 1.1   15 May 1997 02:57:40   RStanev
// Too many changes to describe.
//
//    Rev 1.0   09 Apr 1997 18:16:20   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "drvcomm.h"
#include "navap.h"
#include "navapi.h"

//---------------------------------------------------------------------------
//
// Prototype local functions.
//
// Note: UserStackGlobalInit() is defined right after NAVAPGlobalInit(),
// and is a stub used during stack swapping.  The function has no header
// with comments.
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalInit (
    VOID
    );

VOID UserStackGlobalInit (
    PBOOL pbResult
    );

BOOL NAVAPGlobalInitPlatformSpecific (
    VOID
    );

//---------------------------------------------------------------------------
//
// bNAVAPIsEnabled is a variable containing the global status of NAVAP.
//
//---------------------------------------------------------------------------
BOOL bNAVAPIsEnabled = TRUE;

//---------------------------------------------------------------------------
//
// prNAVAPPersistentData contains a pointer to NAVAP's persistent data area
// kept around by SymEvent.  This pointer must be valid in order for NAVAP
// to work properly.  It is initialized by NAVAPGlobalInit().
//
//---------------------------------------------------------------------------
PERSISTENTDATA *prNAVAPPersistentData = NULL;

//---------------------------------------------------------------------------
//
// bNAVTSRIsLoaded will be set to TRUE by NAVAPGlobalInitPlatformSpecific()
// if NAVTSR was loaded when NAVAP initialized.  This variable is used to
// determine the level of startup protection, and it is also used when NAVAP
// unloads to determine whether it should enable NAVTSR (if the variable was
// TRUE, then NAVTSR was disabled during initialization).
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL bNAVTSRIsLoaded = FALSE;

#endif

//---------------------------------------------------------------------------
//
// The code for NAVAPGlobalInitEx(), NAVAPGlobalInit(), and
// NAVAPGlobalInitPlatformSpecific() should be discarded after
// initialization.
//
//---------------------------------------------------------------------------
#define DRIVER_INIT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// BOOL NAVAPGlobalInitEx (
//      VOID
//      );
//
// This function is a wrap on top of NAVAPGlobalInit(), which ensures that
// a local Core instance is initialized and that the stack is large enough
// for the wrapped function to complete.  Read the header of
// NAVAPGlobalInit() for functionality description.
//
// Entry:
//      Same as in NAVAPGlobalInit().
//
// Exit:
//      Same as in NAVAPGlobalInit().
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalInitEx (
    VOID
    )
    {
    auto BOOL  bResult;
    auto PVOID pvCoreInit;

                                        // Initialize local instance of
                                        // Core.

    if ( pvCoreInit = CoreLocalInit() )
        {
                                        // Call wrapped function on our own
                                        // stack.

        if ( !CallOnNAVAPStack ( UserStackGlobalInit,
                                &bResult ) )
            {
            bResult = FALSE;
            }

                                        // Uninitialize Core.

        CoreLocalDeInit ( pvCoreInit );
        }
    else
        {
        UIOutOfMemoryError();

        bResult = FALSE;
        }

                                        // Return to caller.

    return ( bResult );
    }

VOID UserStackGlobalInit (
    PBOOL pbResult
    )
    {
    *pbResult = NAVAPGlobalInit();
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPGlobalInit (
//      VOID
//      );
//
// This function initializes global NAVAP modules.  If it fails, NAVAP will
// not be able to function properly and driver loading should be aborted.
//
// The tasks this function performs are the following:
//
// 1) Load NAVAP internal configuration.
// 2) Get a pointer to NAVAP's persistent data, or allocate it if it hasn't
//    been allocated yet.
// 3) Initialize PEL.
// 4) Initialize DriverComm.
// 5) Install DriverComm callbacks.
// 6) Initialize the file monitor.
// 7) Initialize file open/create syncrhonization.
// 8) Initialize file caching.
// 9) Call NAVAPGlobalInitPlatformSpecific().
//
// NAVAPGlobalInit() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      -
//
// Exit:
//      TRUE  if successful
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalInit (
    VOID
    )
    {
    auto BOOL bPersistentDataAllocated;

                                        // Load internal configuration.

    LoadNAVAPInternalConfiguration();

                                        // Initialize fast cache.

    InitFastCleanCache();

                                        // Make sure SymEvent has allocated
                                        // persistent data for NAVAP.

    bPersistentDataAllocated = SYMEvent_AllocPersistentData (
                                   SEPDID_NAVAP_PERSISTENTDATA_EX,
                                   sizeof ( *prNAVAPPersistentData ) );

                                        // Get a pointer to the persistent
                                        // data.

    prNAVAPPersistentData = (PERSISTENTDATA*)
                                SYMEvent_GetPersistentDataPtr (
                                    SEPDID_NAVAP_PERSISTENTDATA_EX );

    if ( prNAVAPPersistentData )
        {
                                        // If persistent data was just
                                        // allocated, then reset it.

        if ( bPersistentDataAllocated )
            {
            SYM_VERIFY_BUFFER ( prNAVAPPersistentData,
                                sizeof ( *prNAVAPPersistentData ) );

            memset ( prNAVAPPersistentData,
                     0,
                     sizeof ( *prNAVAPPersistentData ) );
            }

                                        // Initialize PEL.

        if ( PELInit() )
            {
            auto DWORD dwStatus;

                                        // Initialize DriverComm.

            while ( ( dwStatus = DriverCommInit() ) == ERROR_DRVCOMM_UNSAFE )
                {
                Sleep ( 100 );
                }

            SYM_ASSERT ( dwStatus == ERROR_DRVCOMM_SUCCESS );

            if ( dwStatus == ERROR_DRVCOMM_SUCCESS )
                {
                                        // Install DriverComm callbacks.

                if ( DriverCommRegisterServiceCallback ( NAVAPServiceCallback ) ==
                         ERROR_DRVCOMM_SUCCESS &&
                     DriverCommRegisterCancelCallback ( NAVAPCancelCallback ) ==
                         ERROR_DRVCOMM_SUCCESS
                   )
                    {
                                        // Initialize the file monitor.

                    if ( FileMonitorInit() )
                        {
                                        // Initialize the file open/create
                                        // event synchronization.  This
                                        // function should not fail.

                        if ( FileOpenSyncInit() )
                            {
                                        // Initialize file caching.

                            if ( InitCleanCache() )
                                {
                                        // Initialize platform specific
                                        // NAVAP modules.

                                if ( NAVAPGlobalInitPlatformSpecific() )
                                    {
                                        // We have successfully completed
                                        // the initialization of all global
                                        // NAVAP modules.

                                    return ( TRUE );
                                    }

                                        // Undo InitCleanCache().

                                if ( !DeInitCleanCache() )
                                    {
                                    SYM_ASSERT ( FALSE );
                                    }
                                }
                            else
                                {
                                UIInitError ( MSG_Cant_Initialize_File_Caching );
                                }

                                        // Undo FileOpenSyncInit().

                            FileOpenSyncCleanup();

                            if ( !FileOpenSyncDeInit() )
                                {
                                SYM_ASSERT ( FALSE );
                                }
                            }
                        else
                            {
                            SYM_ASSERT ( FALSE );
                            }

                                        // Undo FileMonitorInit().

                        if ( !FileMonitorDeInit() )
                            {
                            SYM_ASSERT ( FALSE );
                            }
                        }
                    else
                        {
                        UIInitError ( MSG_Cant_Allocate_Open_File_Data );
                        }
                    }
                else
                    {
                    UIInitError ( MSG_DriverComm_Register_Callbacks_Error );
                    }

                                        // Undo DriverCommInit().

                while ( ( dwStatus = DriverCommDeInit() ) ==
                        ERROR_DRVCOMM_UNSAFE )
                    {
                    Sleep ( 100 );
                    }

                SYM_ASSERT ( dwStatus == ERROR_DRVCOMM_SUCCESS );
                }
            else
                {
                UIInitError ( MSG_DriverCommInit_Error );
                }

                                        // Undo PELInit().

            if ( !PELDeInit() )
                {
                SYM_ASSERT ( FALSE );
                }
            }
        else
            {
            UIInitError ( MSG_Cant_Allocate_PEL_ThreadVM_Data );
            }
        }
    else
        {
        UIInitError ( MSG_Cant_Save_Persistent_Data );
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPGlobalInitPlatformSpecific (
//      VOID
//      );
//
// This function initializes platform specific global NAVAP modules.  If it
// fails, NAVAP will not be able to function properly and driver loading
// should be aborted.
//
// The tasks this function performs for SYM_VXD platform are the following:
//
// 1) Check if NAVTSR is loaded, and disable it if so.
// 2) Install NAVTSR emulation APIs.
// 3) Install V86 critical error handler.
// 4) Install Control+C / Control+Break handler.
// 5) Initialize CERTLIBN's lpgCallBack.
//
// The tasks this function performs for SYM_NTK platform are the following:
//
// 1) Validate pre-allocated file monitor open file data.
//
// NAVAPGlobalInitPlatformSpecific() will communicate to the user any errors
// and warnings generated during processing.
//
// Entry:
//      -
//
// Exit:
//      TRUE  if successful
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalInitPlatformSpecific (
    VOID
    )
    {
#if defined(SYM_VXD)

                                        // If communication with NAVTSR is
                                        // allowed, then check if NAVTSR is
                                        // loaded, and if so - disable it.

    SYM_ASSERT ( !bNAVTSRIsLoaded );

    if ( !rNAVAPInternalCfg.bRealModeAPCommunication ||
         !( bNAVTSRIsLoaded = IsRealModeAPRunning() ) ||
         DisableRealModeAP()
       )
        {
                                        // Initialize hooks for emulating
                                        // NAVTSR's APIs.

        if ( !rNAVAPInternalCfg.bRealModeAPEmulation || InstallInt2FAPI() )
            {
                                        // Initialize V86 critical error
                                        // handler.

            if ( !rNAVAPInternalCfg.bRealModeCriticalErrorHandler ||
                 Hook_V86_Int_Chain (
                    0x24, &NAVAPV86CriticalErrorHandlerDispatcher )
               )
                {
                                        // Install V86 Control+C /
                                        // Control+Break handler.

                if ( !rNAVAPInternalCfg.bControlBreakHandler ||
                     NAVAPInitBreakHandler()
                   )
                    {
                                        // Init callback structure for
                                        // CERTLIBN's inoc.  This is not the
                                        // place to put this, but the whole
                                        // inoc is one big hack, so it
                                        // doesn't matter.

                    lpgCallBack = &rVLCallbacksRev1;

                                        // We have successfully completed
                                        // all initialization steps.

                    return ( TRUE );
                    }
                else
                    {
                    UIInitError ( MSG_BreakHandlerInit_Error );
                    }

                                        // Uninstall V86 critical error
                                        // handler.

                if ( rNAVAPInternalCfg.bRealModeCriticalErrorHandler &&
                     !Unhook_V86_Int_Chain (
                         0x24, &NAVAPV86CriticalErrorHandlerDispatcher )
                   )
                    {
                    SYM_ASSERT ( FALSE );
                    }
                }
            else
                {
                UIInitError ( MSG_CriticalErrorHandler_Error );
                }

                                        // Undo InstallInt2FAPI().

            if ( rNAVAPInternalCfg.bRealModeAPEmulation &&
                 !UninstallInt2FAPI()
               )
                {
                SYM_ASSERT ( FALSE );
                }
            }
        else
            {
            UIInitError ( MSG_No_Scanner_APIs );
            }

                                        // Undo NAVTSR disabling.

        if ( bNAVTSRIsLoaded && !EnableRealModeAP() )
            {
            SYM_ASSERT ( FALSE );
            }
        }
    else
        {
        UIInitError ( MSG_NAVTSR_Error_Disabling );
        }

#elif defined(SYM_NTK)

                                        // If we have per-file data already
                                        // allocated and saved in our
                                        // persistent data area, make sure
                                        // it is large enough for us to
                                        // initialize the file monitor
                                        // module properly.  This is
                                        // necessary, because the per-file
                                        // data allocated includes a SID,
                                        // and the user may change the SID
                                        // length with an internal registry
                                        // key.  If the SID length has been
                                        // increased after the allocation of
                                        // our per-file data, then the user
                                        // may have to reload SymEvent, and
                                        // even restart the system.

    if ( !prNAVAPPersistentData->hOFData ||
         rNAVAPInternalCfg.dwSIDLength <=
             prNAVAPPersistentData->dwOFDataSIDLength
       )
        {
                                        // We have successfully completed
                                        // all initialization steps.

        return ( TRUE );
        }
    else
        {
        UIInitError ( MSG_SID_Length_Increased );
        }

#else

                                        // Unsupported platform.

    SYM_ASSERT ( FALSE );

#endif

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// All code from now on has to stay in memory for as long as the driver is
// loaded.
//
//---------------------------------------------------------------------------
#define DRIVER_DEFAULT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// BOOL NAVAPControlledInit (
//      VOID
//      );
//
// This function completes the initialization of NAVAP.  It may be called
// only if NAVAPGlobalInit() has succeeded, and once called, a successful
// call to NAVAPControlledDeInit() must be made before calling it again.
//
// The tasks this function performs are the following:
//
// 1) Initialize NAVAP's configuration.
// 2) Perform startup tasks.
// 3) Create device list.
// 4) Enable the file monitor
// 5) Install event hooks.
//
// NAVAPControlledInit() will communicate to the user any errors and
// warnings generated during processing.
//
// Entry:
//      -
//
// Exit:
//      TRUE  if NAVAP was successfully initialized
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL NAVAPControlledInit (
    VOID
    )
    {
                                        // Initialize NAVAP's configuration.

    if ( InitNAVAPConfiguration() )
        {
                                        // Perform startup tasks.

        if ( NAVAPStartupTasks() )
            {
                                        // Create a list of devices which
                                        // will be protected by NAVAP.

            if ( CreateDeviceList() )
                {
                                        // Enable the file monitor.  Note,
                                        // that we will do this before
                                        // installing of the event hooks,
                                        // but we will disable it before
                                        // uninstalling of the event hooks.

                FileMonitorEnable();

                                        // Install event hooks.

                if ( InstallEventHooks() )
                    {
                                        // We have successfully completed
                                        // the controlled initialization of
                                        // NAVAP.

                    return ( TRUE );
                    }
                else
                    {
                    UIControlledInitMessage ( MSG_Invalid_Event_Handles );
                    }

                                        // Disable the file monitor.

                FileMonitorDisable();

                                        // Undo CreateDeviceList().

                if ( !DestroyDeviceList() )
                    {
                    SYM_ASSERT ( FALSE );
                    }
                }
            else
                {
                UIControlledInitMessage ( MSG_Cant_Create_Device_List );
                }
            }

                                        // Undo InitNAVAPConfiguration().

        if ( !DeInitNAVAPConfiguration() )
            {
            SYM_ASSERT ( FALSE );
            }
        }

    return ( FALSE );
    }
