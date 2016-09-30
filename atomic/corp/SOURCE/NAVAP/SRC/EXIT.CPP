// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/EXIT.CPv   1.6   29 Sep 1998 18:46:54   RStanev  $
//
// Description:
//      This file defines NAVAP's unloading routines.
//
// Contains:
//      NAVAPGlobalDeInitEx()
//      NAVAPGlobalDeInit()                         (local function)
//      UserStackGlobalDeInit()                     (local function)
//      NAVAPGlobalDeInitPlatformSpecific()         (local function)
//      NAVAPControlledDeInit()
//
// See Also:
//      init.cpp, config.cpp
//
// Note:
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/EXIT.CPv  $
// 
//    Rev 1.6   29 Sep 1998 18:46:54   RStanev
// Added fast clean cache.
//
//    Rev 1.5   17 Feb 1998 22:38:30   RStanev
// Updated copyright.
//
//    Rev 1.4   05 Jul 1997 20:11:44   RStanev
// New stack swapping architecture.
//
//    Rev 1.3   27 Jun 1997 21:32:36   RStanev
// NAVAPControlledDeInit() now calls CleanupOpenAfterExec().
//
//    Rev 1.2   17 Jun 1997 20:57:30   RStanev
// NAVAPControlledDeInit() now performs exit tasks.
//
//    Rev 1.1   15 May 1997 02:58:46   RStanev
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
// Note: UserStackGlobalDeInit() is defined right after NAVAPGlobalDeInit(),
// and is a stub used during stack swapping.  The function has no header
// with comments.
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalDeInit (
    VOID
    );

VOID UserStackGlobalDeInit (
    PBOOL pbResult
    );

BOOL NAVAPGlobalDeInitPlatformSpecific (
    VOID
    );

//---------------------------------------------------------------------------
//
// BOOL NAVAPGlobalDeInitEx (
//      VOID
//      );
//
// This function is a wrap on top of NAVAPGlobalDeInit(), which ensures that
// a local Core instance is initialized and that the stack is large enough
// for the wrapped function to complete.  Read the header of
// NAVAPGlobalDeInit() for functionality description.
//
// Entry:
//      Same as in NAVAPGlobalDeInit().
//
// Exit:
//      Same as in NAVAPGlobalDeInit().
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalDeInitEx (
    VOID
    )
    {
    auto BOOL  bResult;
    auto PVOID pvCoreInit;

                                        // Initialize local instance of
                                        // Core.  If this operation fails,
                                        // we have two choices: 1) fail the
                                        // function and let Windows deal
                                        // with an uninitialized driver,
                                        // which will eventually crash the
                                        // system, or 2) keep on
                                        // uninitializing the way we are,
                                        // which will either crash the
                                        // system, or complete successfully.
                                        // We will choose 2) and pray
                                        // everything to be ok.

    if ( !( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        }

                                        // Call wrapped function.  As with
                                        // initializing Core above, we would
                                        // be left with the same two choices
                                        // if the stack swap failed.  The
                                        // logic behind the decision on
                                        // choosing 2) is the same.

    if ( !CallOnNAVAPStack ( UserStackGlobalDeInit,
                             &bResult ) )
        {
        bResult = NAVAPGlobalDeInit();
        }

                                        // Uninitialize Core.

    CoreLocalDeInit ( pvCoreInit );

                                        // Return to caller.

    return ( bResult );
    }

VOID UserStackGlobalDeInit (
    PBOOL pbResult
    )
    {
    *pbResult = NAVAPGlobalDeInit();
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPGlobalDeInit (
//      VOID
//      );
//
// This function undoes the initialization of global NAVAP modules done by
// NAVAPGlobalInit().  The tasks NAVAPGlobalDeInit() performs are the
// following:
//
// 1) Call NAVAPGlobalDeInitPlatformSpecific().
// 2) Uninitialize file caching.
// 3) Uninitialize file open/create syncrhonization.
// 4) Uninitialize the file monitor.
// 5) Uninitialize DriverComm.
// 6) Uninitialize PEL.
//
// Note, that only certain tasks that were performed by NAVAPGlobalInit()
// need to be undone by this function.
//
// Entry:
//      -
//
// Exit:
//      This function returns always TRUE.
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalDeInit (
    VOID
    )
    {
    auto DWORD dwStatus;

                                        // Call
                                        // NAVAPGlobalDeInitPlatformSpecific().

    if ( !NAVAPGlobalDeInitPlatformSpecific() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Undo InitCleanCache().

    if ( !DeInitCleanCache() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Undo FileOpenSyncInit().

    FileOpenSyncCleanup();

    if ( !FileOpenSyncDeInit() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Undo FileMonitorInit().

    if ( !FileMonitorDeInit() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Undo DriverCommInit().

    while ( ( dwStatus = DriverCommDeInit() ) == ERROR_DRVCOMM_UNSAFE )
        {
        Sleep ( 100 );
        }

    SYM_ASSERT ( dwStatus == ERROR_DRVCOMM_SUCCESS );

                                        // Undo PELInit().

    if ( !PELDeInit() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Undo InitFastCleanCache().

    DeInitFastCleanCache();

                                        // Ignore all errors and return
                                        // always TRUE.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPGlobalDeInitPlatformSpecific (
//      VOID
//      );
//
// This function undoes the platform specific initialization that was done
// by NAVAPGlobalInitPlatformSpecific().
//
// The tasks this function performs for SYM_VXD platform are the following:
//
// 1) Invalidate CERTLIBN's lpgCallBack.
// 2) Uninstall Control+C / Control+Break handler.
// 3) Uninstall V86 critical error handler.
// 4) Uninstall NAVTSR emulation APIs.
// 5) Enable NAVTSR.
//
// The tasks this function performs for SYM_NTK platform are the following:
//
// 0) No tasks are performed for SYM_NTK platform.
//
// Note, that only certain tasks that were performed by
// NAVAPGlobalInitPlatformSpecific() need to be undone by this function.
//
// Entry:
//      -
//
// Exit:
//      This function returns always TRUE.
//
//---------------------------------------------------------------------------
BOOL NAVAPGlobalDeInitPlatformSpecific (
    VOID
    )
    {
#if defined(SYM_VXD)

                                        // Invalidate lpgCallBack.

    lpgCallBack = NULL;

                                        // Uninstall V86 Control+C /
                                        // Control+Break handler.

    if ( rNAVAPInternalCfg.bControlBreakHandler &&
         !NAVAPDeInitBreakHandler()
       )
        {
        SYM_ASSERT ( FALSE );
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

                                        // Uninstall NAVTSR emulation APIs.

    if ( rNAVAPInternalCfg.bRealModeAPEmulation && !UninstallInt2FAPI() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Enable NAVTSR.

    if ( bNAVTSRIsLoaded && !EnableRealModeAP() )
        {
        SYM_ASSERT ( FALSE );
        }

#endif // #if defined(SYM_VXD)

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPControlledDeInit (
//      VOID
//      );
//
// This function undoes whatever NAVAPControlledInit() did.  It may be
// called only if NAVAPGlobalInit() has succeeded.
//
// The tasks this function performs are the following:
//
// 1) Perform exit tasks.
// 2) Disable and clean up the file monitor.
// 3) Uninstall event hooks.
// 4) Destroy device list.
// 5) Uninitialize NAVAP's configuration.
//
// Entry:
//      -
//
// Exit:
//      Returns always TRUE.
//
//---------------------------------------------------------------------------
BOOL NAVAPControlledDeInit (
    VOID
    )
    {
                                        // Perform exit tasks.

    if ( !NAVAPExitTasks() )
        {
        SYM_ASSERT ( FALSE );
        }
                                        // Disable and clean up the file
                                        // monitor.

    FileMonitorDisable();

                                        // Uninstall event hooks.

    if ( !UnInstallEventHooks() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Invalidate the open-after-exec
                                        // database.

#if defined(SYM_VXD)

    CleanupOpenAfterExec ( TRUE, NULL, NULL );

#endif

                                        // Destroy device list.

    if ( !DestroyDeviceList() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Undo InitNAVAPConfiguration().

    if ( !DeInitNAVAPConfiguration() )
        {
        SYM_ASSERT ( FALSE );
        }

    return ( TRUE );
    }
