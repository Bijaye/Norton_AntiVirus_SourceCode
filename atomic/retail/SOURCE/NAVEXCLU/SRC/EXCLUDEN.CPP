// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVEXCLU/VCS/excluden.cpv   1.1   07 Jul 1998 14:17:24   RStanev  $
//
// Description:
//      This file defines exclusion APIs for use by NAVAP.
//
// Contains:
//      ExcludeNInit()
//      ExcludeNClose()
//      ExcludeNIdleUpdate()
//      ExcludeNSetIdleCallback()
//      ExcludeNIsFileExcluded()
//      ExcludeNExcludeFile()
//      ExcludeAppyTime()           (local function)
//
// See Also:
//
// Note:
//      This code is one big hack.  Please be careful when you modify
//      something, and please keep the functions synchronized with the other
//      exclusion APIs.
//
//***************************************************************************
// $Log:   S:/NAVEXCLU/VCS/excluden.cpv  $
// 
//    Rev 1.1   07 Jul 1998 14:17:24   RStanev
// Changed to conform to the APIs of NavExcludeCheck(), and worked around
// a problem with NavExcludeCheck() where an excluded file in the root dir
// (C:\File.ext) would match the same file if it was in a directory with
// 100+ something characters (C:\...100+chars...\File.ext).
//
//    Rev 1.0   06 Feb 1997 20:57:56   RFULLER
// Initial revision
//
//    Rev 1.4   07 Sep 1996 14:13:10   DALLEE
// Put back APPY and shell stuff for VXD (inside ifdefs...)
//
//    Rev 1.3   10 May 1996 10:58:58   RHAYES
// Unicode conversion update.
//
//    Rev 1.2   06 May 1996 16:10:54   RSTANEV
// Merged Ron's Unicode changes and cleaned up.
//
//    Rev 1.1   19 Mar 1996 13:52:00   jworden
// Double byte enable
//
//    Rev 1.0   30 Jan 1996 15:52:54   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 13:21:58   BARRY
// Initial revision.
//
//    Rev 1.4   08 Jul 1995 11:34:10   RSTANEV
// Changed such that we pass the extended flags to the async function for
// adding exclusions.
//
//    Rev 1.3   08 Jul 1995 10:37:20   RSTANEV
// Added support for EXCLADD_EXACTNAME.
//
//    Rev 1.2   28 Jun 1995 10:15:00   RSTANEV
// Oops, changed "!=" to "==".
//
//    Rev 1.1   28 Jun 1995 10:02:52   RSTANEV
// Fixed an incorrectly formed SYM_ASSERT.
//
//    Rev 1.0   28 Jun 1995 09:25:30   RSTANEV
// Initial revision.
//
//***************************************************************************

#define NOMB
#include "platform.h"
#include "exclude.h"

#if defined(SYM_VXD)
  #undef WANTVXDWRAPS
  #include <shell.h>                    // for APPY stuff.
  #define WANTVXDWRAPS
#endif

//---------------------------------------------------------------------------
//
// Define inline functions
//
//---------------------------------------------------------------------------

#if defined(SYM_VXD)
#pragma warning (disable:4035)      // turn off no return code warning

WORD VXDINLINE
_SHELL_Get_Version(VOID)
{
    VxDCall(SHELL_Get_Version);
}

#pragma warning (default:4035)      // turn on no return code warning
#endif

//---------------------------------------------------------------------------
//
// Define local function prototypes
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID __cdecl ExcludeAppyTime (
    DWORD dwRefData
    );

#endif

//---------------------------------------------------------------------------
//
// HEXCLUDEN WINAPI ExcludeNInit (
//      PTSTR pszExcludeDBPath
//      );
//
// This function creates an exclusion handle based on the exclusion database
// at the specified location.  The exclusion handle contains the regular
// exclusion structure (EXCLUDE) as well as other synchronization members
// and the path to the exclusion DB.  This allows the clients to not pass a
// whole bunch of parameters, which never change.
//
// Entry:
//      pszExcludeDBPath - location of the exclusion database (in OEM
//                         for SYM_VXD)
//
// Exit:
//      Returns a newly initialized exclusion handle, or NULL if error
//
//---------------------------------------------------------------------------
HEXCLUDEN WINAPI ExcludeNInit (
    PTSTR pszExcludeDBPath
    )
    {
    auto PEXCLUDEN prExcludeN;

                                        // Validate input parameters.

    SYM_ASSERT ( pszExcludeDBPath != NULL );
    SYM_ASSERT ( STRLEN ( pszExcludeDBPath ) > 0 );

                                        // Allocate memory for new handle.

    prExcludeN = (PEXCLUDEN) MemAllocPtr ( GHND, sizeof(*prExcludeN) );

                                        // Initialize old style exclusion
                                        // structure.

    if ( ExcludeInitSpecifyDir ( &prExcludeN->rExclude, pszExcludeDBPath ) ==
         NOERR )
        {
                                        // If successful, then copy the path
                                        // to the exclude DB into buffer.

        STRCPY ( prExcludeN->szExcludeDBPath, pszExcludeDBPath );
        }
    else
        {
                                        // If error, then release memory and
                                        // return error.

        if ( MemFreePtr ( prExcludeN ) != NULL )
            {
            SYM_ASSERT ( FALSE );
            }

        return ( NULL );
        }

                                        // Return exclusion handle

    return ( prExcludeN );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI ExcludeNClose (
//      HEXCLUDEN hExcludeN
//      );
//
// This function closes an exclusion handle.  All asynchronously added items
// should be flushed before calling this API.
//
// Entry:
//      hExcludeN - a handle to an exclusion database.
//
// Exit:
//      TRUE  if successful
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL WINAPI ExcludeNClose (
    HEXCLUDEN hExcludeN
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( hExcludeN != NULL );
    SYM_ASSERT ( hExcludeN->hIdleTimeHandle == NULL );
    SYM_ASSERT ( hExcludeN->prAsyncExcludeList == NULL );

                                        // Deinitialize old style exclusion
                                        // structure.

    if ( ExcludeTerminateSpecifyDirEx ( &hExcludeN->rExclude,
                                        hExcludeN->szExcludeDBPath,
                                        EXCLEND_DONTSAVEFREEMEM ) != NOERR )
        {
        return ( FALSE );
        }

                                        // Free memory block of the exclusion
                                        // handle.

    if ( MemFreePtr ( hExcludeN ) != NULL )
        {
        SYM_ASSERT ( FALSE );

        return ( FALSE );
        }

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI ExcludeNIdleUpdate (
//      HEXCLUDEN hExcludeN
//      );
//
// SYM_VXD: This function should be called every time the system is avaiable
// to flush the asynchronously added exclusion items to disk.  In other
// platforms, it does nothing and always returns TRUE.
//
// Entry:
//      hExcludeN - a handle to an exclusion database.
//
// Exit:
//      TRUE  if successful
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL WINAPI ExcludeNIdleUpdate (
    HEXCLUDEN hExcludeN
    )
    {
#if defined(SYM_VXD)

    auto BOOL              bResult;
    auto PASYNCEXCLUDEITEM prExcludeEntry, prNextItemInList;

                                        // Validate input parameters

    SYM_ASSERT ( hExcludeN != NULL );

                                        // Make sure that someone else has
                                        // not called ExcludeNIdleUpdate()
                                        // at the same time.

    if ( hExcludeN->bIdleUpdating != FALSE )
        {
        return ( TRUE );
        }

    hExcludeN->bIdleUpdating = TRUE;

                                        // Initialize local variables

    bResult = TRUE;

                                        // If there is an appy time callback
                                        // registered, then kill it.  This
                                        // function will not return, until
                                        // the last item has been written to
                                        // disk.

    if ( hExcludeN->hIdleTimeHandle != NULL )
        {
        auto APPY_HANDLE hAppyHandle;

        hAppyHandle = (APPY_HANDLE)hExcludeN->hIdleTimeHandle;

        hExcludeN->hIdleTimeHandle = NULL;

        SHELL_CancelAppyTimeEvent ( hAppyHandle );
        }

                                        // Flush all exclusion items.  Start
                                        // at the beginning of the list.

    prExcludeEntry = hExcludeN->prAsyncExcludeList;

    while ( prExcludeEntry != NULL )
        {
                                        // Find an item that is still in the
                                        // list and that hasn't been written
                                        // yet.

        while ( prExcludeEntry != NULL &&
                prExcludeEntry->szFileName[0] == EOS )
            {
            prExcludeEntry = prExcludeEntry->prNextItemInList;
            }

                                        // If item found, then write it to
                                        // disk.

        if ( prExcludeEntry != NULL )
            {
            auto EXCLUDEITEM rExcludeItem;
            auto UINT        uRet;

            STRCPY ( rExcludeItem.szText, prExcludeEntry->szFileName );

            rExcludeItem.bSubDirectory = FALSE;
            rExcludeItem.wBits         = (WORD)prExcludeEntry->dwFlags;

            uRet = ExcludeAddSpecifyDir (
                        &hExcludeN->rExclude,
                        hExcludeN->szExcludeDBPath,
                        &rExcludeItem,
                        ( prExcludeEntry->dwFlags & excEXACT_NAME ) ?
                            EXCLADD_RELOADFLUSH | EXCLADD_EXACTNAME :
                            EXCLADD_RELOADFLUSH );

                                        // If added successfully, then start
                                        // scanning at the beginning of the
                                        // list again.  The reason for doing
                                        // so, is because during the add
                                        // itself, another item could've been
                                        // added to the list.

            if ( uRet == TRUE || uRet == EXCLUDE_DUPLICATE )
                {
                                        // This entry is invalid.  We
                                        // shouldn't attempt to add it to the
                                        // database anymore.

                prExcludeEntry->szFileName[0] = EOS;

                prExcludeEntry = hExcludeN->prAsyncExcludeList;
                }
            else
                {
                                        // If error while adding, then don't
                                        // try to add anything else, but
                                        // still clean up all memory.

                SYM_ASSERT ( FALSE );

                bResult = FALSE;

                prExcludeEntry = NULL;
                }
            }
        }

                                        // Release all items from the list.
                                        // Watch out for the order of
                                        // updating all pointers, etc!

    prExcludeEntry = hExcludeN->prAsyncExcludeList;

    hExcludeN->prAsyncExcludeList = NULL;

    while ( prExcludeEntry != NULL )
        {
        prNextItemInList = prExcludeEntry->prNextItemInList;

        if ( MemFreePtr ( prExcludeEntry ) != NULL )
            {
            SYM_ASSERT ( FALSE );
            }

        prExcludeEntry = prNextItemInList;
        }

                                        // Enable this function to be entered
                                        // again.

    hExcludeN->bIdleUpdating = FALSE;

                                        // Return to caller.

    return ( bResult );

#else

    return ( TRUE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI ExcludeNSetIdleCallback (
//      HEXCLUDEN hExcludeN,
//      PVOID     pfnIdleCallback
//      );
//
// This function replaces the default idle callback which is used to flush
// the cached exclusion entries.  This function is used so that NAVAP can
// allocate stack before calling ExcludeNIdleUpdate().
//
// Entry:
//      hExcludeN       - a handle to an exclusion database.
//      pfnIdleCallback - a pointer to a function (APPY_CALLBACK) to be
//                        called when the system is available for calling
//                        ExcludeNIdleUpdate().  The only parameter passed
//                        to the callback will be the exclusion handle.
//                        Passing NULL here will force usage of internal
//                        callback.
//
// Exit:
//      Always returns TRUE, but maybe in the future FALSE will mean error!
//
//---------------------------------------------------------------------------
BOOL WINAPI ExcludeNSetIdleCallback (
    HEXCLUDEN hExcludeN,
    PVOID     pfnIdleCallback
    )
    {
                                        // Validate input parameters

    SYM_ASSERT ( hExcludeN != NULL );

                                        // Replace callback

    hExcludeN->pfnIdleCallback = pfnIdleCallback;

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI ExcludeNIsFileExcluded (
//      HEXCLUDEN hExcludeN,
//      PTSTR     pszFileName,
//      DWORD     dwFlags
//      );
//
// This function checks whether a file is in the exclusion database or not.
// The flags passed are the same as in ExcludeIs(), with the exception of
// the two new flags excASYNCHRONOUS and excEXACT_NAME (combined both in
// excIO_UNSAFE.)  Depending on the flags, this function will check for the
// requested filename in the asynchronous buffer and/or the standard
// exclusion database.
//
// Entry:
//      hExcludeN   - a handle to an exclusion database
//      pszFileName - OEM name of the file to examine
//      dwFlags     - flags to check against
//
// Exit:
//      TRUE  if file is excluded
//      FALSE if file is not excluded, or error
//
//---------------------------------------------------------------------------
BOOL WINAPI ExcludeNIsFileExcluded (
    HEXCLUDEN hExcludeN,
    PTSTR     pszFileName,
    DWORD     dwFlags
    )
    {
    auto TCHAR       szShortFileName[SYM_MAX_PATH];
    auto TCHAR       szLongFileName[SYM_MAX_PATH];
    auto PTSTR       pszShortFinal, pszLongFinal;
    auto UINT        uNumEntries, uIndex;
    auto BOOL        bSubDirectory;
    auto EXCLUDEITEM rItem;

                                        // Validate input parameters

    SYM_ASSERT ( hExcludeN != NULL );
    SYM_ASSERT ( pszFileName != NULL );

    if ( dwFlags & excEXACT_NAME )
        {
                                        // If the client wants us to check
                                        // the name exactly as passed, then
                                        // set up SFN buffer only.

        STRCPY ( szShortFileName, pszFileName );
        }
    else if ( ExcludeIsValidSFN ( pszFileName ) == STATUSEXCLUDE_OK )
        {
                                        // If the client wants us to check
                                        // a short filename, then set up
                                        // both SFN & LFN buffers.

        STRCPY ( szShortFileName, pszFileName );

        *szLongFileName = EOS;

        if ( ExcludeSFN2LFN ( szShortFileName,
                              szLongFileName,
                              sizeof(szLongFileName) / sizeof(TCHAR) ) == FALSE )
            {
            *szLongFileName = EOS;
            }
        }
    else if ( ExcludeIsValidLFN ( pszFileName ) == STATUSEXCLUDE_OK )
        {
                                        // If the client wants us to check
                                        // a long filename, then set up
                                        // both SFN & LFN buffers.

        STRCPY ( szLongFileName, pszFileName );

        *szShortFileName = EOS;

        if ( ExcludeLFN2SFN ( szLongFileName,
                              szShortFileName,
                              sizeof(szShortFileName) / sizeof(TCHAR) ) == FALSE )
            {
            *szShortFileName = EOS;
            }
        }
    else
        {
                                        // Can't figure out what to do

        return ( FALSE );
        }

#if defined(SYM_VXD)
                                        // If the async & exact name flags
                                        // specified, we have to first check
                                        // the list of exclusions that we
                                        // have created on our own.  This
                                        // check will do a simple string and
                                        // flag compare.  Nothing fancy.

    if ( (dwFlags & excASYNCHRONOUS) != 0 &&
         (dwFlags & excEXACT_NAME) != 0 )
        {
        auto PASYNCEXCLUDEITEM prAsyncExcludeItem;

        prAsyncExcludeItem = hExcludeN->prAsyncExcludeList;

        while ( prAsyncExcludeItem != NULL )
            {
            if ( prAsyncExcludeItem->szFileName[0] != EOS &&
                 ( (prAsyncExcludeItem->dwFlags & 0xFFFFL) ==
                   (dwFlags & 0xFFFFL) ) &&
                 STRICMP ( prAsyncExcludeItem->szFileName, pszFileName ) == 0
               )
                {
                return ( TRUE );
                }

            prAsyncExcludeItem = prAsyncExcludeItem->prNextItemInList;
            }
        }

#endif

                                        // Do a regular search through the
                                        // database.  The logic here was
                                        // pulled straight from ExcludeIs(),
                                        // with the following modification:
                                        // After all filenames have been
                                        // converted, make sure that
                                        // empty strings are treated as
                                        // missing filenames.

    pszLongFinal  = ( *szLongFileName )  ? szLongFileName : NULL;

    pszShortFinal = ( *szShortFileName ) ? szShortFileName : NULL;

    if ( dwFlags & excEXACT_NAME )
        {
        pszLongFinal = NULL;
        }

    if ( pszLongFinal || pszShortFinal )
        {
        uNumEntries = ExcludeGetCount ( &hExcludeN->rExclude );

        for ( uIndex = 0; uIndex < uNumEntries; uIndex++ )
            {
            if ( ExcludeGet ( &hExcludeN->rExclude,
                              uIndex,
                              rItem.szText,
                              &rItem.wBits,
                              &bSubDirectory ) == FALSE )
                {
                break;
                }

            rItem.bSubDirectory = bSubDirectory;

            if ( NavExcludeCheck ( &rItem,
                                   pszShortFinal,
                                   pszLongFinal,
                                   (WORD)dwFlags ) != FALSE )
                {
                return ( TRUE );
                }
            }
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI ExcludeNExcludeFile (
//      HEXCLUDEN hExcludeN,
//      PTSTR     pszFileName,
//      DWORD     dwFlags
//      );
//
// This function adds a file to the exclusion database.  The new flags
// excASYNCHRONOUS and excEXACT_NAME (combined both in excIO_UNSAFE.) will
// be examined and the proper database will be used.  If an asynchronous
// exclusion is added, an appy time will be scheduled for adding the item
// to the real database.
//
// Entry:
//      hExcludeN   - a handle to an exclusion database.
//      pszFileName - OEM name of the file to add
//      dwFlags     - flags to exclude file from
//
// Exit:
//      TRUE  if file was excluded successfully (this will be returned even
//            if there is a duplicate exclusion)
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL WINAPI ExcludeNExcludeFile (
    HEXCLUDEN hExcludeN,
    PTSTR     pszFileName,
    DWORD     dwFlags
    )
    {
    auto EXCLUDEITEM rExcludeItem;
    auto UINT        uResult;

                                        // Validate input parameters.

    SYM_ASSERT ( hExcludeN != NULL );
    SYM_ASSERT ( pszFileName != NULL );
    SYM_ASSERT ( STRLEN(pszFileName) > 0 );

#if defined(SYM_VXD)

    auto PASYNCEXCLUDEITEM prAsyncExcludeItem;

                                        // If asynchronous add, then add it
                                        // to the cache.  We will not check
                                        // for duplicate exclusions here!

    if ( dwFlags & excASYNCHRONOUS )
        {
                                        // This flag should always be passed
                                        // together with the excEXACT_NAME
                                        // flag.

        SYM_ASSERT ( (dwFlags & excEXACT_NAME) != 0 );

                                        // Allocate memory for cache entry.

        prAsyncExcludeItem = (PASYNCEXCLUDEITEM) MemAllocPtr (
                                GHND,
                                sizeof(*prAsyncExcludeItem) );

        if ( prAsyncExcludeItem == NULL )
            {
            return ( FALSE );
            }

                                        // Initialize the entry.

        prAsyncExcludeItem->dwFlags = dwFlags;
        STRCPY ( prAsyncExcludeItem->szFileName, pszFileName );

                                        // Link it to the list.  Please note
                                        // that the order of linking, and
                                        // registering appy time is very
                                        // important for correct
                                        // synchronization with
                                        // ExcludeNIdleUpdate().

        prAsyncExcludeItem->prNextItemInList = hExcludeN->prAsyncExcludeList;

        hExcludeN->prAsyncExcludeList = prAsyncExcludeItem;

                                        // If this is the first entry in the
                                        // list, then ask for appy time.

        if ( prAsyncExcludeItem->prNextItemInList == NULL )
            {
            if ( _SHELL_Get_Version() != 0 )
                {
                hExcludeN->hIdleTimeHandle =
                    SHELL_CallAtAppyTime ( &ExcludeAppyTime,
                                           (DWORD)hExcludeN,
                                           NULL );

                SYM_ASSERT ( hExcludeN->hIdleTimeHandle );
                }
            else
                {
                SYM_ASSERT ( FALSE );
                }
            }

        return ( TRUE );
        }
#endif

                                        // If not asynchronous exclusion,
                                        // then do it all right here, right
                                        // now.

    STRCPY ( rExcludeItem.szText, pszFileName );
    rExcludeItem.bSubDirectory = FALSE;
    rExcludeItem.wBits = (WORD)dwFlags;

    uResult = ExcludeAddSpecifyDir ( &hExcludeN->rExclude,
                                     hExcludeN->szExcludeDBPath,
                                     &rExcludeItem,
                                     ( dwFlags & excEXACT_NAME ) ?
                                        EXCLADD_RELOADFLUSH | EXCLADD_EXACTNAME :
                                        EXCLADD_RELOADFLUSH );

                                        // What the hell is this code???

    if ( uResult == TRUE || uResult == EXCLUDE_DUPLICATE )
        {
        return ( TRUE );
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// VOID __cdecl ExcludeAppyTime (
//     DWORD dwRefData
//     );
//
// This function is called during an appy time to process asynchrnously added
// exclusion entries.
//
// Entry:
//      dwRefData - exclusion handle.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID __cdecl ExcludeAppyTime (
    DWORD dwRefData
    )
    {
                                        // This is going to make my life
                                        // much easier

#define hExcludeN ((HEXCLUDEN)dwRefData)

                                        // Validate input parameters

    SYM_ASSERT ( hExcludeN != NULL );

                                        // Invalidate idle time callback
                                        // handle in hExcludeN

    hExcludeN->hIdleTimeHandle = NULL;

                                        // If there is a callback associated
                                        // with hExcludeN, then call it.

    if ( hExcludeN->pfnIdleCallback != NULL )
        {
        ( (APPY_CALLBACK)(hExcludeN->pfnIdleCallback) ) ( dwRefData );
        }
    else
        {
                                        // If there is no callback, then use
                                        // the default one.  This shouldn't
                                        // happen, since the stack is only 4K

        if ( ExcludeNIdleUpdate ( hExcludeN ) == FALSE )
            {
            SYM_ASSERT ( FALSE );
            }
        }

#undef hExcludeN

    }

#endif
