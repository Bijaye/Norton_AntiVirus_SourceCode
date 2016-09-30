// Copyright 1997 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/SHUTDOWN.CPv   1.4   17 Feb 1998 22:08:58   RStanev  $
//
// Description:
//      This file defines functions for shutting the system down.
//
// Contains:
//      InitiateShutdown()
//      ShutdownAppyTime()              (local function, SYM_VXD only)
//      UserStackNavHaltSystem()        (local function, SYM_VXD only)
//
// See Also:
//      navhalt.cpp(NAVXUTIL)
//
// Note:
//
//***************************************************************************

#include "platform.h"
#include "symsync.h"
#include "drvcomm.h"
#include "navap.h"
#include "navapi.h"
#include "apstrs.h"

//---------------------------------------------------------------------------
//
// bNAVAPIsInShutdownMode is a global variable which flags when NAVAP has
// initiated shutdown.  During shutdown mode, NAVAP will quietly resolve any
// problems that it detects.
//
//---------------------------------------------------------------------------
BOOL bNAVAPIsInShutdownMode = FALSE;

//---------------------------------------------------------------------------
//
// Define local variables.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

APPY_HANDLE hShutdownAppyTime = NULL;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Prototype local functions.
//
// Note: UserStackNavHaltSystem() is defined right after ShutdownAppyTime(),
// and is a stub used during stack swapping.  The function has no header
// with comments.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID __cdecl ShutdownAppyTime (
    DWORD dwRefData
    );

VOID UserStackNavHaltSystem (
    VOID
    );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// VOID InitiateShutdown (
//      VOID
//      );
//
// This function starts shutting the system down.  It attempts to ask our
// agent to do the shutdown for us (because a Win32 application can force
// shutdown).  If this fails, then it (SYM_VXD only) tries to shutdown on
// its own by waiting for appy time and calling the ExitWindows() API.
//
// InitiateShutdown() will will communicate to the user any errors and
// warnings generated during processing.
//
// Entry:
//      -
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID InitiateShutdown (
    VOID
    )
    {
                                        // Make sure we don't reenter the
                                        // function.

    SymAcquireFastGlobalLock();

    if ( bNAVAPIsInShutdownMode )
        {
        SymReleaseFastGlobalLock();
        return;
        }

    bNAVAPIsInShutdownMode = TRUE;

    SymReleaseFastGlobalLock();

                                        // Try asking our agent to do the
                                        // shutdown for us.

    if ( DriverCommSend ( NAVAP_COMM_SHUTDOWN ) == ERROR_DRVCOMM_SUCCESS )
        {
        return;
        }

                                        // ~~~TBD~~~ Currently DriverComm
                                        // does not support synchronous
                                        // calls, so we will always end up
                                        // here.  Until DriverComm is
                                        // enhanced, we will do a cheap
                                        // check for whether a shutdown can
                                        // be done by our agent, and this is
                                        // pretty much guaranteed when we
                                        // are a dynamically loaded VxD
                                        // (unless our agent has crashed),
                                        // or we are a WinNT driver.

#if defined(SYM_NTK)

    if ( DriverCommAsyncSend ( NAVAP_COMM_SHUTDOWN ) ==
         ERROR_DRVCOMM_SUCCESS )
        {
        return;
        }

#elif defined(SYM_VXD)

    if ( bVxDIsDynaloaded )
        {
        if ( DriverCommAsyncSend ( NAVAP_COMM_SHUTDOWN ) ==
             ERROR_DRVCOMM_SUCCESS )
            {
            return;
            }
        }

                                        // Make sure we can proceed with
                                        // scheduling an event for calling
                                        // ExitWindows().

    if ( !bVMMIs3x && SHELL_Get_Version() )
        {
                                        // Schedule appy time for initiating
                                        // system shutdown.

        hShutdownAppyTime = SHELL_CallAtAppyTime ( &ShutdownAppyTime,
                                                   NULL,
                                                   NULL );

                                        // If appy time was scheduled, then
                                        // return success.

        if ( hShutdownAppyTime )
            {
            return;
            }
        }

#endif

                                        // We failed to initiate shutdown.

    bNAVAPIsInShutdownMode = FALSE;

    UIShutdownError ( MSG_Unable_To_Initiate_Shutdown );

    return;
    }

//---------------------------------------------------------------------------
//
// VOID __cdecl ShutdownAppyTime (
//     DWORD dwRefData
//     );
//
// This function is called during an appy time after InitiateShutdown()
// determined that it can't ask a user mode application to do the shutdown.
// ShutdownAppyTimy() will attempt to call 16-bit Windows APIs to complete
// the shutdown.
//
// Entry:
//      dwRefData - NULL.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID __cdecl ShutdownAppyTime (
    DWORD dwRefData
    )
    {
    auto PVOID pvCoreInit;
    auto struct { WORD  wReserved;
                  DWORD dwReturnCode;
                } rExitWindowsParams;


                                        // Clear global variables.

    hShutdownAppyTime = NULL;

                                        // Initialize local instance of
                                        // Core.

    if ( pvCoreInit = CoreLocalInit() )
        {
                                        // Perform pre-shutdown tasks on our
                                        // own stack.

        CallOnNAVAPStack ( UserStackNavHaltSystem );

        CoreLocalDeInit ( pvCoreInit );
        }
    else
        {
        UIOutOfMemoryError();
        }

                                        // Call the ExitWindows() API.

    rExitWindowsParams.dwReturnCode = 0;
    rExitWindowsParams.wReserved    = 0;

    SHELL_CallDll ( szExitWindowsDLL,
                    szExitWindowsName,
                    sizeof(rExitWindowsParams),
                    &rExitWindowsParams );

                                        // We failed to shut the system
                                        // down.

    bNAVAPIsInShutdownMode = FALSE;

    UIShutdownError ( MSG_Shutdown_Aborted );
    }

VOID UserStackNavHaltSystem (
    VOID
    )
    {
#if 0 // This is not supported by the Intel code.
    NavHaltSystem();
#endif
    }

#endif // #if defined(SYM_VXD)
