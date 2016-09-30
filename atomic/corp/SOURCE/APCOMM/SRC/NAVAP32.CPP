// Copyright 1997 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APCOMM/VCS/navap32.cpv   1.2   16 Mar 1998 19:44:50   DALLEE  $
//
// Description:
//      This file defines the functionality of NAVAP32.DLL
//
// Contains:
//      DllMain()
//      NAVAPGetEnabledDisabled()
//      NAVAPSetEnabledDisabled()
//      NAVAPProtectProcess()
//      NAVAPUnprotectProcess()
//
// See Also:
//      apcomm.h
//
// Note:
//      All functions marked as exported under the "Contains:" section are
//      not exported with their "real" names.  When using GetProcAddress()
//      to determine the address of these functions, use constants formed
//      by prepending "sz" to each function name:
//
//      pfnNAVAPGetEnabledDisabled = GetProcAddress (
//                                       hNAVAP32DLL,
//                                       szNAVAPGetEnabledDisabled );
//
//***************************************************************************
// $Log:   S:/APCOMM/VCS/navap32.cpv  $
// 
//    Rev 1.2   16 Mar 1998 19:44:50   DALLEE
// Roswell port: Fixed usage of lpvReserved in process attach.
// Was preventing run-time load of DLL, not static link.
// 
//    Rev 1.1   17 Feb 1998 14:27:40   RStanev
// Updated copyright.
//
//    Rev 1.0   16 Apr 1997 10:32:48   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "apcomm.h"

//---------------------------------------------------------------------------
//
// lProcessUnprotectCount contains the number of times NAVAP has been
// recursively called to unprotect a process.  If the application is
// behaving properly, this counter should always be non-negative, and
// when the application detaches from the DLL, this counter should be 0.
//
//---------------------------------------------------------------------------
static long lProcessUnprotectCount;

//---------------------------------------------------------------------------
//
// BOOL WINAPI DllMain (
//      HINSTANCE hinstDLL,
//      DWORD     fdwReason,
//      LPVOID    lpvReserved
//      );
//
// This is the entry point of NAVAP32.DLL.
//
// Entry:
//      hinstDLL    - look at Win32 SDK.
//      fdwReason   - look at Win32 SDK.
//      lpvReserved - look at Win32 SDK.
//
// Exit:
//      Always TRUE, except during DLL_PROCESS_ATTACH if the DLL was loaded
//      statically.
//
//---------------------------------------------------------------------------
BOOL WINAPI DllMain (
    HINSTANCE hinstDLL,
    DWORD     fdwReason,
    LPVOID    lpvReserved
    )
    {
    switch ( fdwReason )
        {
        case DLL_PROCESS_ATTACH:

                                        // Don't allow static linking to
                                        // that DLL.

            if ( lpvReserved )
                {
                return ( FALSE );
                }

                                        // Initialize the NAVAP process
                                        // protection counter.

            lProcessUnprotectCount = 0;

            break;

        case DLL_PROCESS_DETACH:

                                        // Clean up the process protection
                                        // counter in NAVAP.

            SYM_ASSERT ( !lProcessUnprotectCount );

            while ( lProcessUnprotectCount-- > 0 )
                {
                NAVAPiProtectProcess();
                }

            break;
        }

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL SYM_EXPORT WINAPI NAVAPGetEnabledDisabled (
//      PDWORD pdwStatus
//      );
//
// This function returns the status of NAVAP (whether it is enabled or
// disabled).
//
// Entry:
//      pdwStatus - pointer to a DWORD to receive the status of NAVAP.
//
// Exit:
//      TRUE if NAVAP was loaded and its status was placed in pdwStatus.
//      FALSE if error.
//
//      Upon successful return, *pdwStatus will be set to 0 if NAVAP is
//      disabled and set to non-0 if NAVAP is enabled.
//
//---------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI NAVAPGetEnabledDisabled (
    PDWORD pdwStatus
    )
    {
    return ( NAVAPiGetEnabledDisabled ( pdwStatus ) );
    }

//---------------------------------------------------------------------------
//
// BOOL SYM_EXPORT WINAPI NAVAPSetEnabledDisabled (
//      DWORD dwStatus
//      );
//
// This function enables or disables NAVAP.
//
// Entry:
//      dwStatus - new status of NAVAP.  0 if client wants to globally
//                 disable NAVAP and non-0 if client wants to globally
//                 enable NAVAP.
//
// Exit:
//      TRUE if NAVAP was loaded and its status was set according to
//      dwStatus.
//      FALSE if error.
//
//---------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI NAVAPSetEnabledDisabled (
    DWORD dwStatus
    )
    {
    return ( NAVAPiSetEnabledDisabled ( dwStatus ) );
    }

//---------------------------------------------------------------------------
//
// BOOL SYM_EXPORT WINAPI NAVAPProtectProcess (
//      VOID
//      );
//
// This function decrements the protection counter for the current process.
//
// By default all processes' protection counters are 0.  If a process wishes
// to tell NAVAP that it wants to perform actions which would be considered
// by NAVAP as harmful, then it should call NAVAPUnproctectProcess() before
// performing those actions.  This will increment this process' protection
// counter, making it a positive value, thus telling NAVAP to stop protecting
// this process.  After all "harm" is done, the process should call
// NAVAPProtectProcess() to decrement its counter.  NAVAP will never allow
// the counter to become a negative value, even though such condition can
// occur when NAVAP is loaded/unloaded.
//
// Entry:
//      -
//
// Exit:
//      TRUE if NAVAP was loaded and the request was serviced.
//      FALSE if error.
//
//---------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI NAVAPProtectProcess (
    VOID
    )
    {
    if ( NAVAPiProtectProcess() )
        {
        InterlockedDecrement ( &lProcessUnprotectCount );

        return ( TRUE );
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL SYM_EXPORT WINAPI NAVAPUnprotectProcess (
//      VOID
//      );
//
// This function increments the protection counter for the current process.
//
// By default all processes' protection counters are 0.  If a process wishes
// to tell NAVAP that it wants to perform actions which would be considered
// by NAVAP as harmful, then it should call NAVAPUnproctectProcess() before
// performing those actions.  This will increment this process' protection
// counter, making it a positive value, thus telling NAVAP to stop protecting
// this process.  After all "harm" is done, the process should call
// NAVAPProtectProcess() to decrement its counter.  NAVAP will never allow
// the counter to become a negative value, even though such condition can
// occur when NAVAP is loaded/unloaded.
//
// Entry:
//      -
//
// Exit:
//      TRUE if NAVAP was loaded and the request was serviced.
//      FALSE if error.
//
//---------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI NAVAPUnprotectProcess (
    VOID
    )
    {
    if ( NAVAPiUnprotectProcess() )
        {
        InterlockedIncrement ( &lProcessUnprotectCount );

        return ( TRUE );
        }

    return ( FALSE );
    }
