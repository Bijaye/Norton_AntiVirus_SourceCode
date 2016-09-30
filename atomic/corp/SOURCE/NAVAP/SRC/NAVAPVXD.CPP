// Copyright 1997 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/NAVAPVXD.CPv   1.11   14 Sep 1998 16:40:38   RStanev  $
//
// Description:
//      This file contains the entry/exit points of NAVAP's Win95 driver,
//      as well as NAVAP functions specific to Win95.
//
// Contains:
//      NAVAP_Init_Complete()
//      NAVAP_Dynamic_Init()
//      NAVAP_Sys_VM_Terminate()
//      NAVAP_Dynamic_Exit()
//      NAVAP_Terminate_Thread()
//      NAVAP_VM_Terminate()
//      NAVAP_Destroy_Thread()
//
//      NAVAPStartupTasks()
//      NAVAPExitTasks()
//      FirstAppyTime()                     (local function)
//
//      NAVAPV86CriticalErrorHandler()
//
// See Also:
//      navapnt.cpp, vxdentry.asm
//
// Note:
//      Supports SYM_VXD only!
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/NAVAPVXD.CPv  $
// 
//    Rev 1.11   14 Sep 1998 16:40:38   RStanev
// Added stack cleanup.
//
//    Rev 1.10   17 Feb 1998 22:15:20   RStanev
// Updated copyright.
//
//    Rev 1.9   09 Aug 1997 01:00:10   RStanev
// Patch for support of old NAVAP APIs.
//
//    Rev 1.8   27 Jun 1997 21:32:14   RStanev
// Added NAVAP_Terminate_Thread() and NAVAP_VM_Terminate().
//
//    Rev 1.7   17 Jun 1997 21:01:20   RStanev
// Added NAVAPExitTasks().
//
//    Rev 1.6   13 Jun 1997 20:20:14   DALLEE
// Changed show command when launching agent from SW_SHOWMINIMIZED to
// SW_SHOWMINNOACTIVE so focus won't shift from login prompt to agent.
//
//    Rev 1.5   02 Jun 1997 12:17:22   RStanev
// Removed an unnecessary SYM_ASSERT().  EnterControlledInit() should not
// prevent NAVAP_Init_Complete() from completing successfully.
//
//    Rev 1.4   23 May 1997 23:36:32   RStanev
// Fixed a spelling mistake.
//
//    Rev 1.3   21 May 1997 05:00:22   RStanev
// Added startup tasks.
//
//    Rev 1.2   15 May 1997 03:09:06   RStanev
// Too many changes to describe.
//
//    Rev 1.1   30 Apr 1997 18:57:26   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.0   09 Apr 1997 12:21:46   RStanev
// Initial revision.
//
//***************************************************************************

#if !defined(SYM_VXD)
#error This file supports only SYM_VXD platform!
#endif

#include "platform.h"
#include "xapi.h"
#include "navap.h"
#include "navapi.h"
#include "apstrs.h"

//---------------------------------------------------------------------------
//
// Prototype local functions.
//
//---------------------------------------------------------------------------
VOID __cdecl FirstAppyTime (
    DWORD dwRefData
    );

//---------------------------------------------------------------------------
//
// bNAVAPGlobalInitComplete protects us from trying to uninitialize NAVAP
// upon termination if NAVAP was not properly initialized.  The reason we
// have to synchronize the initialization/deinitialization on our own is
// because the Windows 95 documentation is not very clear on what messages
// come through when driver's initialization failed, or if dynamic
// initialization succeeded, but then the system had to shut down without
// the loading application getting a chance to unload the driver.
//
// Note, that bNAVAPGlobalInitComplete is not protected from multithreaded
// modification, because the only two functions that modify it and use it
// are NAVAP_Sys_VM_Terminate() and NAVAP_Dynamic_Exit().  We could at least
// rely on Windows 95 not sending the Sys_VM_Terminate and
// SYS_DYNAMIC_DEVICE_EXIT messages to drivers simultaneously.
//
//---------------------------------------------------------------------------
BOOL bNAVAPGlobalInitComplete = FALSE;

//---------------------------------------------------------------------------
//
// bStaticInitializationInProgress is set to TRUE only while the controlled
// initialization of NAVAP is performed under the context of a statically
// loaded NAVAP.VxD.
//
//---------------------------------------------------------------------------
BOOL bStaticInitializationInProgress = FALSE;

//---------------------------------------------------------------------------
//
// The code for NAVAP_Init_Complete() and NAVAP_Dynamic_Init() should be
// discarded after initialization.
//
//---------------------------------------------------------------------------
#define DRIVER_INIT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// BOOL WINAPI NAVAP_Init_Complete (
//      VOID
//      );
//
// This function is called by NAVAP.VXD's control procedure upon
// Init_Complete message.  If such message is sent to NAVAP, this means that
// the driver is loaded statically.  NAVAP_Init_Complete() initializes the
// driver, just as if it was loaded dynamically, but then it completes the
// full NAVAP initialization and enables the driver on its own.  This is
// done by emulating the calls that a user-mode application would have
// issued to NAVAP after it has successfully loaded the driver.  This way
// NAVAP will work without its user-mode counterpart.
//
// NAVAP_Init_Complete() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      -
//
// Exit:
//      TRUE if initialization successful, FALSE if error.
//
//---------------------------------------------------------------------------
BOOL WINAPI NAVAP_Init_Complete (
    VOID
    )
    {
    auto PVOID pvCoreInit;
                                        // First, we need to do exactly the
                                        // same processing as
                                        // NAVAP_Dynamic_Init().

    if ( NAVAP_Dynamic_Init() )
        {
                                        // Emulate initialization calls that
                                        // our user-mode counterpart would
                                        // have done to enable us when we
                                        // are dynamically loaded.

        if ( pvCoreInit = CoreLocalInit() )
            {
                                        // We need to tell
                                        // NAVAPStartupTasks(), which is
                                        // called during controlled
                                        // initialization that we are
                                        // currently initializing a
                                        // statically loaded VxD.

            bStaticInitializationInProgress = TRUE;

                                        // Perform controlled
                                        // initialization.  Ignore the error
                                        // code, since we are loading
                                        // statically.

            EnterControlledInit();

                                        // From now on, NAVAPStartupTasks()
                                        // should not be doing any tasks
                                        // specific to static loading.

            bStaticInitializationInProgress = FALSE;

                                        // Uninitialize local instance of
                                        // Core and return successful
                                        // controlled initialization.

            CoreLocalDeInit ( pvCoreInit );

            return ( TRUE );
            }
        else
            {
            UIOutOfMemoryError();

            SYM_ASSERT ( FALSE );
            }

        if ( !NAVAP_Dynamic_Exit() )
            {
            SYM_ASSERT ( FALSE );
            }
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI NAVAP_Dynamic_Init (
//      VOID
//      );
//
// This function is called by NAVAP.VXD's control procedure upon
// SYS_DYNAMIC_DEVICE_INIT message.  If such message is sent to NAVAP, this
// means that the driver is loaded dynamically.  This function makes sure
// that proper SymEvent is loaded, and then initializes global NAVAP
// modules.
//
// Upon successful completion of this function, the NAVAP driver itself will
// not be functional.  The global initialization of NAVAP only prepares the
// driver for full initialization.  Full driver initialization and enabling
// NAVAP is completed upon request by our user-mode counterpart.
//
// NAVAP_Dynamic_Init() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      -
//
// Exit:
//      TRUE if initialization successful, FALSE if error.
//
// Note:
//      NAVAP_Dynamic_Init() is also used as part of the initialization
//      process of statically loaded NAVAP.
//
//---------------------------------------------------------------------------
BOOL WINAPI NAVAP_Dynamic_Init (
    VOID
    )
    {
                                        // Validate the synchronization of
                                        // initialization/deinitialization.

    SYM_ASSERT ( !bNAVAPGlobalInitComplete );

                                        // Make sure SYMEvent is loaded.

    if ( SYMEvent_Get_Version() > 0 )
        {
                                        // Check for proper SYMEvent version.

        if ( SYMEvent_Get_Version() >= SYMEVENT_VERSION_REQUIRED_BY_NAVAP )
            {
                                        // Initialize Core.

            if ( CoreGlobalInit() )
                {
                                        // Initialize global NAVAP modules.

                if ( bNAVAPGlobalInitComplete = NAVAPGlobalInitEx() )
                    {
                    return ( TRUE );
                    }

                                        // Undo CoreGlobalInit().

                if ( !CoreGlobalDeInit() )
                    {
                    SYM_ASSERT ( FALSE );
                    }
                }
            else
                {
                UIInitError ( MSG_Core_Init_Failed );
                }
            }
        else
            {
            UIInitError ( MSG_SYMEvent_Incompatible_Version );
            }
        }
    else
        {
        UIInitError ( MSG_SYMEvent_Not_Loaded );
        }

                                        // Report failure during
                                        // initialization.

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
//  BOOL WINAPI NAVAP_Sys_VM_Terminate (
//      VOID
//      );
//
// This function is called by NAVAP.VXD's control procedure upon
// Sys_VM_Terminate message.  If such message is sent to NAVAP, this means
// that Windows 95 is shutting down.  Because there is no clear
// documentation on whether this message will ever be received by
// dynamically loaded drivers, this function takes precautionary measures to
// ensure NAVAP quits cleanly no matter how it was loaded.
//
// NAVAP_Sys_VM_Terminate() first makes sure that the driver is disabled and
// uninitialized before the OS unloads it.  This refers to the
// initialization and enabling done upon request by user-mode application -
// see the comments in the header of NAVAP_Dynamic_Init().  After ensuring
// that the driver is disabled and uninitialized, this function proceeds to
// undo everything that NAVAP_Dynamic_Init() did, which partially overlaps
// with what NAVAP_Init_Complete() does.
//
// Entry:
//      -
//
// Exit:
//      This function returns always TRUE.
//
//---------------------------------------------------------------------------
BOOL WINAPI NAVAP_Sys_VM_Terminate (
    VOID
    )
    {
    return ( NAVAP_Dynamic_Exit() );
    }

//---------------------------------------------------------------------------
//
//  BOOL WINAPI NAVAP_Dynamic_Exit (
//      VOID
//      );
//
// This function is called by NAVAP.VXD's control procedure upon
// SYS_DYNAMIC_DEVICE_EXIT message.  If such message is sent to NAVAP, this
// means that the driver is being unloaded.
//
// NAVAP_Dynamic_Exit() first makes sure that the driver is disabled and
// uninitialized before the OS unloads it.  This refers to the
// initialization and enabling done upon request by user-mode application -
// see the comments in the header of NAVAP_Dynamic_Init().  After ensuring
// that the driver is disabled and uninitialized, this function proceeds to
// undo everything that NAVAP_Dynamic_Init() did.
//
// Entry:
//      -
//
// Exit:
//      This function returns always TRUE.
//
//---------------------------------------------------------------------------
BOOL WINAPI NAVAP_Dynamic_Exit (
    VOID
    )
    {
    auto PVOID pvCoreInit;

                                        // If bNAVAPGlobalInitComplete is
                                        // FALSE, then we are guaranteed that
                                        // nothing is initialized, and we
                                        // should NOT call any "high-level"
                                        // NAVAP uninitialization functions.

    if ( bNAVAPGlobalInitComplete )
        {
                                        // From now on, we are considered as
                                        // "uninitialized".

        bNAVAPGlobalInitComplete = FALSE;

                                        // Make sure NAVAP is disabled and
                                        // uninitialized.

        if ( ! ( pvCoreInit = CoreLocalInit() ) )
            {
            UIOutOfMemoryError();

            SYM_ASSERT ( FALSE );
            }

        if ( EnterControlledDeInit() != ERROR_SUCCESS )
            {
            SYM_ASSERT ( FALSE );
            }

        CoreLocalDeInit ( pvCoreInit );

                                        // Undo NAVAPGlobalInitEx().

        if ( !NAVAPGlobalDeInitEx() )
            {
            SYM_ASSERT ( FALSE );
            }

                                        // Undo CoreGlobalInit().

        if ( !CoreGlobalDeInit() )
            {
            SYM_ASSERT ( FALSE );
            }
        }

                                        // Tell the caller that everything
                                        // was ok, even if we have to lie.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI NAVAP_Terminate_Thread (
//      PTCB pTCB
//      );
//
// This function is called during Terminate_Thread message.  It cleans up
// any data that NAVAP has allocted for the terminating thread.
//
// Entry:
//      pTCB - pointer to the TCB of the thread which is terminating.
//
// Exit:
//      Always TRUE - this is a VMM requirement.
//
//---------------------------------------------------------------------------
BOOL WINAPI NAVAP_Terminate_Thread (
    PTCB pTCB
    )
    {
                                        // Clean up any open-after-exec
                                        // data allocated for pTCB.

    CleanupOpenAfterExec ( FALSE, pTCB, NULL );

                                        // Return to VMM.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI NAVAP_VM_Terminate (
//      HVM hVM
//      );
//
// This function is called during VM_Terminate message.  It cleans up any
// data that NAVAP has allocted for the terminating VM.
//
// Entry:
//      hVM - handle to the VM which is terminating.
//
// Exit:
//      Always TRUE - this is a VMM requirement.
//
//---------------------------------------------------------------------------
BOOL WINAPI NAVAP_VM_Terminate (
    HVM hVM
    )
    {
                                        // Clean up any open-after-exec
                                        // data allocated for hVM.

    CleanupOpenAfterExec ( FALSE, NULL, hVM );

                                        // Return to VMM.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI NAVAP_Destroy_Thread (
//      PTCB pTCB
//      );
//
// This function is called during Destroy_Thread and Destroy_VM messages.
// It cleans up as much data as it can.
//
// Entry:
//      pTCB - Thread control block of the terminating thread.
//
// Exit:
//      Always TRUE - this is a VMM requirement.
//
//---------------------------------------------------------------------------
#define DRIVER_LOCKED_CODE_SEGMENT
#include "drvseg.h"

BOOL WINAPI NAVAP_Destroy_Thread (
    PTCB pTCB
    )
    {
                                        // Release any stack allocated for
                                        // this thread.

    StackCleanupForThread ( pTCB );

                                        // Return to VMM.

    return ( TRUE );
    }

#define DRIVER_DEFAULT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// BOOL NAVAPStartupTasks (
//      VOID
//      );
//
// This function is called during controlled initialization and performs the
// startup tasks that NAVAP is configured to do.  These tasks are the
// following:
//
// 1) NAVAP integrity self test.
// 2) Check if NAVTSR was present during startup (only during initialization
//    of statically loaded NAVAP.VxD).
// 3) Scan memory for known viruses.
// 4) Start NAVAP's agent (only during initialization of statically loaded
//    driver).
//
// The SYM_NTK implementation of NAVAPStartupTasks() may perform different
// tasks.
//
// NAVAPStartupTasks() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      -
//
// Exit:
//      TRUE if the NAVAP controlled initialization should continue, or
//      FALSE if the initialization should be failed.
//
//---------------------------------------------------------------------------
BOOL NAVAPStartupTasks (
    VOID
    )
    {
                                        // Integrity self test.

    if ( !CheckIfNAVAPIntact() )
        {
        return ( FALSE );
        }

                                        // Test if NAVTSR was loaded during
                                        // startup.

    if ( bStaticInitializationInProgress &&
         bNAVAPCfgStartupRunDOSAutoProtect &&
         rNAVAPInternalCfg.bRealModeAPCommunication &&
         !bNAVTSRIsLoaded
       )
        {
        UIControlledInitMessage ( MSG_NAVTSR_Not_Loaded );
        }

                                        // Scan memory for known viruses.

    if ( bNAVAPCfgStartupScanMemory && CheckIfMemoryInfected() )
        {
        return ( FALSE );
        }

                                        // Start NAVAP's agent.  At this
                                        // stage of the game, we can only
                                        // schedule this to be done later,
                                        // when applications can run.

    if ( bStaticInitializationInProgress &&
         !bVMMIs3x &&
         rNAVAPInternalCfg.bRegisterAppyTimeForAgent
       )
        {
        if ( SHELL_Get_Version() == 0 ||
             !SHELL_CallAtAppyTime ( &FirstAppyTime, NULL, NULL )
           )
            {
            UIControlledInitMessage ( MSG_Unable_To_Load_Agent );
            }
        }

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPExitTasks (
//      VOID
//      );
//
// This function is called during controlled deinitialization and performs
// the exit tasks that NAVAP is configured to do.  These tasks are the
// following:
//
// 1) Simulate reboot event.
//
// The SYM_NTK implementation of NAVAPExitTasks() may perform different
// tasks.
//
// NAVAPExitTasks() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      -
//
// Exit:
//      Always TRUE.
//
//---------------------------------------------------------------------------
BOOL NAVAPExitTasks (
    VOID
    )
    {
    auto ED rEventData;

                                        // Simulate what SYMEvent would have
                                        // done if it detected reboot.

    rEventData.SE_ED_etEventType = SE_MSC_REBOOT;

    ECBReboot ( NULL, &rEventData );

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// VOID __cdecl FirstAppyTime (
//      DWORD dwRefData
//      );
//
// This function is called during the first system appy time.  It currently
// only launches driver's user mode "significant other".
//
// FirstAppyTime() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      dwRefData - NULL.
//
// Exit:
//      -
//
// Notes:
//      FirstAppyTime() will be called only if NAVAP is statically loaded.
//      The user mode application is assumed to be located in the same
//      directory where the NAVAP driver was loaded from.
//
//---------------------------------------------------------------------------
VOID __cdecl FirstAppyTime (
    DWORD dwRefData
    )
    {
    auto SHEXPACKET *prShellPacket;
    auto PSTR       pszOffset;

                                        // Make sure the configuration
                                        // allows us to launch the
                                        // application.

    if ( rNAVAPInternalCfg.bExecuteAgent )
        {
                                        // Allocate memory for the packet.

        prShellPacket = (SHEXPACKET*) MemAllocPtr ( GHND,
                                                    sizeof(SHEXPACKET) +
                                                    ( 2 * (SYM_MAX_PATH + 1) )
                                                  );

        if ( prShellPacket )
            {
                                        // Initialize the Shell packet
                                        // structure with info about the
                                        // application.

                                        // Note: nCmdShow is initialized to
                                        // 7, which is how SW_SHOWMINNOACTIVE
                                        // is defined by the Windows SDK.

            prShellPacket->dwTotalSize = sizeof(SHEXPACKET);
            prShellPacket->dwSize      = sizeof(SHEXPACKET);
            prShellPacket->ibOp        = 0;
            prShellPacket->ibParams    = 0;
            prShellPacket->nCmdShow    = 7;

                                        // Specify the file used to launch
                                        // the application.

            prShellPacket->ibFile = prShellPacket->dwTotalSize;
            pszOffset             = (PSTR) ( (UINT) prShellPacket +
                                             prShellPacket->dwTotalSize );

            STRCPY ( pszOffset, pszNAVAPStartupName );

            NameStripFile ( pszOffset );

            NameAppendFile ( pszOffset, szNAVAPWFileName );

            SYM_ASSERT ( STRLEN ( pszOffset ) < SYM_MAX_PATH );

            prShellPacket->dwTotalSize += STRLEN (pszOffset) + 1;

                                        // Specify what directory to find
                                        // it in.

            prShellPacket->ibDir = prShellPacket->dwTotalSize;
            pszOffset            = (PSTR) ( (DWORD) pszOffset +
                                            STRLEN ( pszOffset ) + 1 );

            STRCPY ( pszOffset, pszNAVAPStartupName );

            NameStripFile ( pszOffset );

            SYM_ASSERT ( STRLEN ( pszOffset ) < SYM_MAX_PATH );

            prShellPacket->dwTotalSize += STRLEN ( pszOffset ) + 1;

                                        // Launch the application.

            if ( _SHELL_ShellExecute ( prShellPacket ) > 32 )
                {
                                        // Free resources and quit.

                MemFreePtr ( prShellPacket );

                return;
                }

                                        // Free resources and display error
                                        // message.

            MemFreePtr ( prShellPacket );
            }
        else
            {
            UIOutOfMemoryError();
            }

        UIControlledInitMessage ( MSG_Unable_To_Load_Agent );
        }
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI NAVAPV86CriticalErrorHandler (
//      PCRS prClientRegs
//      );
//
// This function provides the critical error handling for V86 VMs.
//
// Entry:
//      prClientRegs  - pointer to structure of type Client_Reg_Struct
//                      containing caller's registers when V86 Int 24h was
//                      called
//
// Exit:
//      TRUE  - event was processed by NAVAP and interrupt should not be
//              chained
//              Appropriate members of *prClientRegs modified
//      FALSE - event was not processed by NAVAP and interrupt should be
//              reflected into the V86 VM.
//
//---------------------------------------------------------------------------
BOOL WINAPI NAVAPV86CriticalErrorHandler (
    PCRS prClientRegs
    )
    {
                                        // If current process is in PEL1,
                                        // then we should service the
                                        // critical error.

    if ( IsCurrentThreadInPEL1() &&
                                        // Now that we know this is us
                                        // causing the error, see if failing
                                        // the operation is allowed.  If
                                        // failing is not allowed, we'll let
                                        // the default handler do the work.

        ( ((Client_Byte_Reg_Struc*)prClientRegs)->Client_AH & 8 ) != 0
       )
        {
                                        // Fail the operation.

        ((Client_Byte_Reg_Struc*)prClientRegs)->Client_AL = 3;

                                        // Don't reflect the interupt in the
                                        // V86 VM.

        return ( TRUE );
        }

    return ( FALSE );
    }

//------------------------------------------------------------------ CUT HERE
// This is a patch for supporting the process protection APIs
// provided by older versions of NAVAP (Luigi and Kirin).  This
// patch should be removed as soon as all products using the old
// APIs are updated and distributed.
//
#include "apcomm.h"

extern "C" DWORD WINAPI EmulateOldDIOC (
    PDIOCPARAMETERS prDIOCParameters
    );

DWORD WINAPI EmulateOldDIOC (
    PDIOCPARAMETERS prDIOCParameters
    )
    {
    switch ( prDIOCParameters->dwIoControlCode )
        {
        case 0x0F:

            NAVAPServiceCallback ( NAVAP_COMM_UNPROTECTPROCESS,
                                   NULL,
                                   0,
                                   NULL,
                                   0,
                                   NULL );

            return ( 0 );

        case 0x10:

            NAVAPServiceCallback ( NAVAP_COMM_PROTECTPROCESS,
                                   NULL,
                                   0,
                                   NULL,
                                   0,
                                   NULL );

            return ( 0 );
        }

    return ( 1 );
    }
//------------------------------------------------------------------ CUT HERE
