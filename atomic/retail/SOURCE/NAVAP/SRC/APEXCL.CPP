// Copyright 1995 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/APEXCL.CPv   1.5   17 Feb 1998 22:05:00   RStanev  $
//
// Description:
//      This file defines wraps for the exclusion APIs which NAVAP uses.
//
// Contains:
//      NAVAPInitExclusionAPI()
//      NAVAPCloseExclusionAPI()
//      NAVAPIsFileExcluded()
//      NAVAPExcludeFile()
//      NAVAPExcludeAppyTime()        (SYM_VXD only, local function)
//      UserStackExcludeNIdleUpdate() (SYM_VXD only, local function)
//
// See Also:
//      -
//
// Note:
//      This is only a temporary solution for the real problem with the
//      exclusions.  We need a major redesign :-)
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/APEXCL.CPv  $
// 
//    Rev 1.5   17 Feb 1998 22:05:00   RStanev
// Updated copyright.
//
//    Rev 1.4   05 Jul 1997 20:11:14   RStanev
// New stack swapping architecture.
//
//    Rev 1.3   27 Jun 1997 13:44:26   RStanev
// NAVAPInitExclusionAPI() now calls ExcludeNInit() up to five times before
// giving up initialization.
//
//    Rev 1.2   15 May 1997 14:38:00   RStanev
// NAVAPExcludeAppyTime() has to initialize local instance of Core before
// swapping to our own stack.
//
//    Rev 1.1   15 May 1997 02:49:58   RStanev
// Minor changes.
//
//    Rev 1.0   09 Apr 1997 12:21:26   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apexcl.h"

//---------------------------------------------------------------------------
//
// Define local function prototypes
//
// Note: UserStackExcludeNIdleUpdate() is just a stub on top of
// ExcludeNIdleUpdate() which is used during stack swapping.  This function
// is defined immediately after NAVAPExcludeAppyTime() and has no header.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID __cdecl NAVAPExcludeAppyTime (
    HEXCLUDEN hExcludeN
    );

VOID UserStackExcludeNIdleUpdate (
    HEXCLUDEN hExcludeN
    );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// HEXCLUDEN NAVAPInitExclusionAPI (
//      PTSTR pszExcludeDBLocation
//      );
//
// This function initializes a new exclusion handle for use by NAVAP.
//
// Entry:
//      pszExcludeDBLocation - path to the exclusion database files
//                             SYM_VXD: in OEM.
//
// Exit:
//      Returns a valid exclusion handle, or NULL if error.
//
//---------------------------------------------------------------------------
HEXCLUDEN NAVAPInitExclusionAPI (
    PTSTR pszExcludeDBLocation
    )
    {
    auto int       iAttempt;
    auto HEXCLUDEN hExcludeN;
                                        // Validate input parameters.

    SYM_ASSERT ( pszExcludeDBLocation );
    SYM_VERIFY_STRING ( pszExcludeDBLocation );
    SYM_ASSERT ( STRLEN ( pszExcludeDBLocation ) > 0 );

                                        // If exclusions are globally
                                        // disabled, then just return with
                                        // a "valid" handle.

    if ( !rNAVAPInternalCfg.bExclusionsEnabled )
        {
        return ( ~NULL );
        }

                                        // Call the exclusion API for
                                        // initializing.  We will attempt to
                                        // do this up to 5 times, with .5
                                        // second delay in between attempts,
                                        // and hope one of them succeeds.
                                        // This is a hack to resolve a
                                        // problem where we would not be
                                        // able to initialize the module if
                                        // an application, such as the
                                        // scanner, is updating the
                                        // exclusions at the same time.  We
                                        // need to implement better
                                        // synchronization for this task,
                                        // but with the current timeframe
                                        // that's the best that can be done.

    for ( iAttempt = 5;
          iAttempt && !( hExcludeN = ExcludeNInit ( pszExcludeDBLocation ) );
          iAttempt-- )
        {
        Sleep ( 500 );
        }

#if defined(SYM_VXD)
                                        // Hook the idle callback so that
                                        // we can swap stack and really
                                        // do some heavy exclusion work.
                                        // This is only necessary for SYM_VXD
                                        // where we can't add exclusions
                                        // during some device events.

    if ( hExcludeN )
        {
        if ( !ExcludeNSetIdleCallback ( hExcludeN, &NAVAPExcludeAppyTime ) )
            {
            SYM_ASSERT ( FALSE );

            if ( ! ExcludeNClose ( hExcludeN ) )
                {
                SYM_ASSERT ( FALSE );
                }

            return ( NULL );
            }
        }

#endif // #if defined(SYM_VXD)

                                        // Return to caller.

    return ( hExcludeN );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPCloseExclusionAPI (
//     HEXCLUDEN hExcludeN
//     );
//
// This function closes an exclusion handle created by calling
// NAVAPInitExclusionAPI().
//
// Entry:
//      hExcludeN - a valid handle returned by NAVAPInitExclusionAPI().
//
// Exit:
//      Returns TRUE if successful, FALSE if error.
//
//---------------------------------------------------------------------------
BOOL NAVAPCloseExclusionAPI (
    HEXCLUDEN hExcludeN
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( hExcludeN );

                                        // If exclusions are globally
                                        // disabled, then just return.

    if ( !rNAVAPInternalCfg.bExclusionsEnabled )
        {
        SYM_ASSERT ( hExcludeN == ~NULL );
        return ( TRUE );
        }

                                        // Flush all outstanding excluded
                                        // items to disk.

    if ( !ExcludeNIdleUpdate ( hExcludeN ) )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Free up memory & release handle.

    return ( ExcludeNClose ( hExcludeN ) );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPIsFileExcluded (
//     HEVENTPACKET hEventPacket,
//     PTSTR        pszFileName,
//     DWORD        dwFlags
//     );
//
// This function checks whether a file and a particular set of flags are
// excluded.  Dependnig on the flags passed, the file will be looked up into
// either the standard database (file event exclusions) or into both
// standard database and asynchronous buffer (dev event exclusions.)
//
// Entry:
//      hEventPacket - an event packet containing current configuration
//      pszFileName  - name of the file to be checked against exclusions
//                     SYM_VXD: in OEM.
//      dwFlags      - exclusion flags
//
// Exit:
//      TRUE  if file is excluded
//      FALSE if file is not excluded, or error
//
//---------------------------------------------------------------------------
BOOL NAVAPIsFileExcluded (
    HEVENTPACKET hEventPacket,
    PTSTR        pszFileName,
    DWORD        dwFlags
    )
    {
    auto HEXCLUDEN hExcludeN;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( prAPCfg(hEventPacket) );
    SYM_ASSERT ( prAPCfg(hEventPacket)->hExcludeN );
    SYM_ASSERT ( (dwFlags & 0xFFFF0000L) == 0 );

                                        // If exclusions are globally
                                        // disabled, then just return.

    if ( !rNAVAPInternalCfg.bExclusionsEnabled )
        {
        SYM_ASSERT ( prAPCfg(hEventPacket)->hExcludeN == ~NULL );
        return ( FALSE );
        }

                                        // Initialize local variables.

    hExcludeN = prAPCfg(hEventPacket)->hExcludeN;

                                        // Call appropriate exclusion APIs.

    switch ( dwFlags )
        {
        case excVIRUS_FOUND:
        case excINOC_CHANGE:
        case excFILE_WATCH:
        case excRO_CHANGE:
        case excWR_ATLIST:

                                        // If the exclusions are called with
                                        // one of the above flags, this means
                                        // that the system is avalable to
                                        // flush any cached exclusion items.

            if ( !ExcludeNIdleUpdate ( hExcludeN ) )
                {
                SYM_ASSERT ( FALSE );
                }

                                        // Check if file is excluded.

            SYM_ASSERT ( pszFileName );
            SYM_VERIFY_STRING ( pszFileName );

            return ( ExcludeNIsFileExcluded ( hExcludeN,
                                              pszFileName,
                                              dwFlags ) );

            break;

        case excLOW_FORMAT:
        case excWR_HBOOT:
        case excWR_FBOOT:

                                        // Check if file is excluded, but
                                        // do it the safe way...

            SYM_ASSERT ( pszFileName );
            SYM_VERIFY_STRING ( pszFileName );

            return ( ExcludeNIsFileExcluded ( hExcludeN,
                                              pszFileName,
                                              dwFlags | excIO_UNSAFE ) );

            break;

        default:

                                        // Unknown flags.

            SYM_ASSERT ( FALSE );

        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPExcludeFile (
//     HEVENTPACKET hEventPacket,
//     PTSTR        pszFileName,
//     DWORD        dwFlags
//     );
//
// This function adds a file to the exclusion database.  Depending on the
// flags, the file will be added directly into the standard database (file
// event exclusions) or it will be cached into an asynchronous buffer (dev
// event exclusions.)
//
// Entry:
//      hEventPacket - an event packet containing current configuration
//      pszFileName  - name of the file to be added to the database
//                     SYM_VXD: in OEM
//      dwFlags      - exclusion flags
//
// Exit:
//      TRUE  if file excluded
//      FALSE if file already excluded, or if error
//
//---------------------------------------------------------------------------
BOOL NAVAPExcludeFile (
    HEVENTPACKET hEventPacket,
    PTSTR        pszFileName,
    DWORD        dwFlags
    )
    {
    auto HEXCLUDEN hExcludeN;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( prAPCfg(hEventPacket) );
    SYM_ASSERT ( prAPCfg(hEventPacket)->hExcludeN );
    SYM_ASSERT ( (dwFlags & 0xFFFF0000L) == 0 );

                                        // If exclusions are globally
                                        // disabled, then just return.

    if ( !rNAVAPInternalCfg.bExclusionsEnabled )
        {
        SYM_ASSERT ( prAPCfg(hEventPacket)->hExcludeN == ~NULL );
        return ( FALSE );
        }

                                        // Initialize local variables.

    hExcludeN = prAPCfg(hEventPacket)->hExcludeN;

                                        // Call appropriate exclusion APIs

    switch ( dwFlags )
        {
        case excVIRUS_FOUND:
        case excINOC_CHANGE:
        case excFILE_WATCH:
        case excRO_CHANGE:
        case excWR_ATLIST:

                                        // If the exclusions are called with
                                        // one of the above flags, this means
                                        // that the system is avalable to
                                        // flush any cached exclusion items.

            if ( ExcludeNIdleUpdate ( hExcludeN ) == FALSE )
                {
                SYM_ASSERT ( FALSE );
                }

                                        // Add to the exclusion database,
                                        // synchronously.

            SYM_ASSERT ( pszFileName );
            SYM_VERIFY_STRING ( pszFileName );

            return ( ExcludeNExcludeFile ( hExcludeN,
                                           pszFileName,
                                           dwFlags ) );

            break;

        case excLOW_FORMAT:
        case excWR_HBOOT:
        case excWR_FBOOT:

                                        // Add to the exclusion database,
                                        // asynchronously.

            SYM_ASSERT ( pszFileName );
            SYM_VERIFY_STRING ( pszFileName );

            return ( ExcludeNExcludeFile ( hExcludeN,
                                           pszFileName,
                                           dwFlags | excIO_UNSAFE ) );

            break;

        default:

                                        // Unknown flags.

            SYM_ASSERT ( FALSE );

        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// VOID __cdecl NAVAPExcludeAppyTime (
//     HEXCLUDEN hExcludeN
//     );
//
// This function is called during an appy time to process asynchrnously added
// exclusion entries.  It swaps stacks, and then notifies the exclusions
// that it is safe to flush all information.
//
//
// Entry:
//      hExcludeN - exclusion handle.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID __cdecl NAVAPExcludeAppyTime (
    HEXCLUDEN hExcludeN
    )
    {
    auto PVOID pvCoreInit;

                                        // Validate input parameters.

    SYM_ASSERT ( hExcludeN );

                                        // If exclusions are globally
                                        // disabled, then just return.

    if ( rNAVAPInternalCfg.bExclusionsEnabled )
        {
                                        // Initialize local instance of
                                        // Core.

        if ( pvCoreInit = CoreLocalInit() )
            {
                                        // Notify the exclusions on our own
                                        // stack.

            CallOnNAVAPStack ( UserStackExcludeNIdleUpdate,
                               hExcludeN );

                                        // Uninitialize Core.

            CoreLocalDeInit ( pvCoreInit );
            }
        else
            {
            UIOutOfMemoryError();
            }
        }
    else
        {
        SYM_ASSERT ( hExcludeN == ~NULL );
        }
    }

VOID UserStackExcludeNIdleUpdate (
    HEXCLUDEN hExcludeN
    )
    {
    if ( !ExcludeNIdleUpdate ( hExcludeN ) )
        {
        SYM_ASSERT ( FALSE );
        }
    }

#endif // #if defined(SYM_VXD)
