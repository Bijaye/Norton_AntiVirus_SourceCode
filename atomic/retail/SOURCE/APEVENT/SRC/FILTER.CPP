// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/FILTER.CPv   1.6   17 Feb 1998 16:55:46   RStanev  $
//
// Description:
//      This file defines general functions for filtering files and
//      synchronizing file access during event notifications.
//
// Contains:
//      LocalUpperCase()            (local inline function)
//
//      IsObjectAFile()
//
//      IsFileNameReserved()
//
//      IsFileInListOfExtensions()
//
//      InitCleanCache()            (SYM_NTK, other platforms are stubbed)
//      DeInitCleanCache()          (SYM_NTK, other platforms are stubbed)
//      IsFileInCleanCache()        (SYM_NTK, other platforms are stubbed)
//      AddFileToCleanCache()       (SYM_NTK, other platforms are stubbed)
//      RemoveFileFromCleanCache()  (SYM_NTK, other platforms are stubbed)
//      FlushCleanCache()           (SYM_NTK, other platforms are stubbed)
//
//      FileOpenSyncInit()          (SYM_NTK, other platforms are stubbed)
//      FileOpenSyncDeInit()        (SYM_NTK, other platforms are stubbed)
//      FileOpenSyncCleanup()       (SYM_NTK, other platforms are stubbed)
//      FileOpenSyncEnter()         (SYM_NTK, other platforms are stubbed)
//      FileOpenSyncExit()          (SYM_NTK, other platforms are stubbed)
//
// See Also:
//      filter.h
//
// Note:
//      Event Manager module
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/FILTER.CPv  $
// 
//    Rev 1.6   17 Feb 1998 16:55:46   RStanev
// Updated copyright.
//
//    Rev 1.5   05 Aug 1997 13:15:10   RStanev
// Enhanced IsFileNameReserved() to filter out UNCs that don't contain
// file names.
//
//    Rev 1.4   08 Jul 1997 19:40:22   ksackin
// ASFCACHE functions can be used only on Intel platforms.
//
//    Rev 1.3   02 Jul 1997 19:04:06   RStanev
// Removed the changes from the last revision.
//
//    Rev 1.2   03 Jun 1997 09:37:52   RStanev
// Added IsThisMoveOperation().
//
//    Rev 1.1   15 May 1997 01:34:56   RStanev
// Too many changes to describe.
//
//    Rev 1.0   16 Mar 1997 12:57:50   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "dbcs.h"
#include "symsync.h"
#include "navap.h"
#include "monitor.h"
#include "filter.h"
#include "asfcache.h"

//---------------------------------------------------------------------------
//
// List of VxD reserved filenames: CON, NUL, AUX, PRN, LPT?, COM?, NETQ.
// The question marks in the strings will match any single-digit in the
// source string.
//
// Notes:
//      These strings should not be translated.
//      The code which processes the table assumes that all strings in this
//      table are capitalized, and in the range from 'A' to 'Z' (+ '?').
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

const TCHAR *apszReservedFileNames[] =
    {
    _T("CON"),
    _T("NUL"),
    _T("AUX"),
    _T("PRN"),
    _T("LPT?"),
    _T("COM?"),
    _T("NETQ"),
    NULL
    };

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// inline TCHAR LocalUpperCase (
//      TCHAR chChar
//      );
//
// This function returns the upper case value of chChar.
//
// Entry:
//      chChar - any character
//
// Exit:
//      Uppercased chChar
//
// Note:
//      This function does not need to support international characters!
//
//---------------------------------------------------------------------------
inline TCHAR LocalUpperCase (
    TCHAR chChar
    )
    {
    SYM_ASSERT ( 'a' > 'A' );

    if ( chChar >= 'a' && chChar <= 'z' )
        {
        chChar -= 'a' - 'A';
        }

    return ( chChar );
    }

//---------------------------------------------------------------------------
//
// BOOL IsObjectAFile (
//      PCTSTR pszName
//      );
//
// This function determines whether pszName is an actual filename or not.
//
// Entry:
//      pszName - pointer to a string containing a general OS name.
//                SYM_VXD: the string should be in the current thread's
//                character set.
//
// Exit:
//      TRUE if pszName is an existing file, FALSE if pszName is not
//      a file.
//
// Note:
//      This function will return FALSE if there is an error while
//      getting pszName information or if pszName does not exist.
//      This function has to be updated if the OS supports other
//      types of names, besides FILE, DIRECTORY and VOLUME names.
//      ~~~TBD~~~ SYM_NTK: Need to make sure that FileGetAttr() returns
//      FA_DIR and FA_VOLUME attributes!
//
//---------------------------------------------------------------------------
BOOL IsObjectAFile (
    PCTSTR pszName
    )
    {
    auto UINT uObjectAttributes;

                                        // Validate input parameters.

    SYM_ASSERT ( pszName );
    SYM_VERIFY_STRING ( pszName );

                                        // Check if real file or not.

    if ( FileGetAttr ( pszName, &uObjectAttributes ) == NOERR &&
         ! ( uObjectAttributes & FA_DIR ) &&
         ! ( uObjectAttributes & FA_VOLUME ) )
        {
        return ( TRUE );
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL IsFileNameReserved (
//      PCTSTR pszFileSpec
//      );
//
// This function checks if the filename pointed to by pszFileSpec is a
// system reserved name.
//
// Entry:
//      pszFileSpec - pointer to a NULL terminated string with the filespec.
//                    SYM_VXD: the string should be ANSI or OEM.
//
// Exit:
//      TRUE  if the filename is reserved
//      FALSE if the filename is not reserved
//
//---------------------------------------------------------------------------
BOOL IsFileNameReserved (
    PCTSTR pszFileSpec
    )
    {
    auto PCTSTR pszCurrentFile;

                                        // Validate input parameters.

    SYM_ASSERT ( pszFileSpec );
    SYM_VERIFY_STRING ( pszFileSpec );

                                        // Filter out name pipes and other
                                        // UNCs that don't represent file
                                        // names.  Name pipes are UNCs
                                        // formed as \\server\PIPE\name.
                                        // All UNCs with their shares named
                                        // PIPE are assumed to be reserved.

    if ( pszFileSpec[0] == _T('\\') && pszFileSpec[1] == _T('\\') )
        {
                                        // Find the start of the share name.
                                        // SYM_VXD: No need of DBCS support.
                                        // As I understand, the '\\' and '/'
                                        // characters are reserved.  If this
                                        // assumption is wrong, then we have
                                        // to first make sure that the caller
                                        // passes only OEM filenames, then
                                        // replace the "++" operations with
                                        // AnsiNext() operations.

        for ( pszCurrentFile = &pszFileSpec[2];
              *pszCurrentFile && *pszCurrentFile != _T('\\');
              pszCurrentFile++ );

                                        // If at this point we are at the
                                        // end of the string, then we have
                                        // either "\\server" or "\\server\".
                                        // In both cases we will assume that
                                        // this is a reserved name.

        if ( !*pszCurrentFile )
            {
            return ( TRUE );
            }

                                        // Compare the share name to "PIPE".
                                        // Note, that the two strings below
                                        // are reserved and they should not
                                        // be translated.  To simplify the
                                        // comparison, the lentgh of "PIPE"
                                        // has been hard-coded to 4.

        pszCurrentFile++;

        if ( STRNICMP ( pszCurrentFile, _T("PIPE"), 4 ) == 0 &&
                ( pszCurrentFile[4] == _T('\\') ||
                pszCurrentFile[4] == _T('\0') )
            )
            {
            return ( TRUE );
            }

                                        // Check if after the share name we
                                        // have anything left over.  If not,
                                        // then the string is either
                                        // "\\server\share" or
                                        // "\\server\share\".  In both cases
                                        // we will assume that this is a
                                        // reserved name.

        for ( ;
              *pszCurrentFile && *pszCurrentFile != _T('\\');
              pszCurrentFile++ );

        if ( !*pszCurrentFile || !pszCurrentFile[1] )
            {
            return ( TRUE );
            }
        }

#if defined(SYM_VXD)

    auto UINT   uCurrentString;
    auto PCTSTR pszCurrentString;

                                        // Find the start of the file name
                                        // and place it at pszFileSpec.

    pszCurrentFile = pszFileSpec;

    while ( *pszCurrentFile )
        {
        if ( *pszCurrentFile == '\\' || *pszCurrentFile == '/' )
            {
            pszFileSpec = ++pszCurrentFile;
            }
        else
            {
            ++pszCurrentFile;
            }
        }

                                        // Initiate scan for reserved name.

    uCurrentString = 0;

    while ( pszCurrentString = apszReservedFileNames[uCurrentString++] )
        {
                                        // Start comparing the string at
                                        // uCurrentString.

        pszCurrentFile = pszFileSpec;

        while ( *pszCurrentString && *pszCurrentFile )
            {
                                        // Compare current characters, case
                                        // insensitive.  Pay attention to
                                        // the question marks in the current
                                        // source string.

            SYM_ASSERT ( '0' < '9' );

            if ( ( *pszCurrentString == '?' && ( *pszCurrentFile < '0' ||
                                                 *pszCurrentFile > '9' ) ) ||
                 *pszCurrentString != LocalUpperCase ( *pszCurrentFile )
               )
                {
                break;
                }

                                        // SYM_VXD: No need of DBCS support -
                                        // it's part of the requirements of
                                        // apszReservedFileNames[].

            pszCurrentString++;
            pszCurrentFile++;
            }

                                        // If we are at the end of both
                                        // strings, then we have a winner.

        if ( !*pszCurrentString && !*pszCurrentFile )
            {
            return ( TRUE );
            }
        }

#endif // #if defined(SYM_VXD)

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL IsFileInListOfExtensions (
//      PCTSTR pszFile,
//      PCTSTR pszzExtensionList
//      );
//
// This function returns TRUE if the file at pszFile has an extension that
// is in the list of extensions specified by pszzExtensionList.
//
// Entry:
//      pszFile           - Pointer to a file to check against the extension
//                          list.  No wildcards allowed in this string.
//                          SYM_VXD: pszFile should be in OEM.
//      pszzExtensionList - Pointer to an array of 0-terminated strings, each
//                          one specifying an extension.  The array is
//                          terminated by an empty string.
//
// Exit:
//      TRUE  if file's extension is in the list
//      FALSE if file's extension is not in the list
//
//---------------------------------------------------------------------------
BOOL IsFileInListOfExtensions (
    PCTSTR pszFile,
    PCTSTR pszzExtensionList
    )
    {
    auto PCTSTR pszExtension;

                                        // Validate input parameters.

    SYM_ASSERT ( pszFile );
    SYM_VERIFY_STRING ( pszFile );
    SYM_ASSERT ( pszzExtensionList );

                                        // Find the start of the file name
                                        // and place it at pszExtension.

    pszExtension = pszFile;

    while ( *pszFile )
        {
        if ( *pszFile == '\\' || *pszFile == '/' )
            {
            pszExtension = pszFile = AnsiNext ( pszFile );
            }
        else
            {
            pszFile = AnsiNext ( pszFile );
            }
        }

                                        // Find the last '.' in the file name
                                        // and place the location of the
                                        // following character at
                                        // pszExtension.  If no extension is
                                        // found, then make it point to a
                                        // _T(".").  This will currently
                                        // not match anything, but with a
                                        // small tweak in NAV, it may start
                                        // matching files with no extension.

    if ( pszExtension = STRRCHR ( pszExtension, '.' ) )
        {
        pszExtension++;
        }
    else
        {
        pszExtension = _T(".");
        }

                                        // Start a loop trhough the extension
                                        // list and match the current
                                        // extension.

    while ( *pszzExtensionList )
        {
        auto int    iCharsCompared;
        auto PCTSTR pszListChar, pszExtensionChar;
        auto int    iListChar, iExtensionChar;

                                        // Validate the extension list entry.

        SYM_VERIFY_STRING ( pszzExtensionList );

                                        // Start comparing character by
                                        // character.

        iCharsCompared   = 0;
        pszListChar      = pszzExtensionList;
        pszExtensionChar = pszExtension;

        do
            {
                                        // Get a "case insensitive" value for
                                        // the current character in the list.

#if defined(UNICODE)

            iListChar = towupper ( *pszListChar );
            pszListChar++;

#else

            if ( DBCSIsLeadByte ( *pszListChar ) )
                {
                iListChar = * (int *)pszListChar;
                pszListChar += 2;
                }
            else
                {
                iListChar = *pszListChar;
                pszListChar++;
                }

            iListChar = toupper ( iListChar );

#endif

                                        // Get a "case insensitive" value for
                                        // the current character in the
                                        // extension.

#if defined(UNICODE)

            iExtensionChar = towupper ( *pszExtensionChar );
            pszExtensionChar++;

#else

            if ( DBCSIsLeadByte ( *pszExtensionChar ) )
                {
                iExtensionChar = * (int *)pszExtensionChar;
                pszExtensionChar += 2;
                }
            else
                {
                iExtensionChar = *pszExtensionChar;
                pszExtensionChar++;
                }

            iExtensionChar = toupper ( iExtensionChar );

#endif

                                        // Perform wild-card matching.

            if ( iListChar == '*' )
                {
                return ( TRUE );
                }

            if ( iListChar == '?' || iListChar == iExtensionChar )
                {
                iCharsCompared++;

                if ( rNAVAPInternalCfg.bTruncateProgramFileExtensions &&
                     iCharsCompared == 3 )
                    {
                    return ( TRUE );
                    }
                }
            else
                {
                break;
                }

            } while ( iListChar && iExtensionChar );

                                        // We failed to match the extension
                                        // with the current list entry.  Go
                                        // on to match the next one.

        pszzExtensionList += STRLEN ( pszzExtensionList ) + 1;
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL InitCleanCache (
//      VOID
//      );
//
// This function initializes the clean cache module.
//
// Entry:
//      -
//
// Exit:
//      TRUE if successful, FALSE if error.
//
//---------------------------------------------------------------------------
BOOL InitCleanCache (
    VOID
    )
    {
#if defined(SYM_NTK) && defined(_X86_)

    if ( rNAVAPInternalCfg.dwFileCacheEntries &&
         AsfInitCache ( rNAVAPInternalCfg.dwFileCacheEntries ) ==
             STATUS_SUCCESS )
        {
        return ( TRUE );
        }

    return ( FALSE );

#else

    return ( TRUE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL DeInitCleanCache (
//      VOID
//      );
//
// This function uninitializes the clean cache module.
//
// Entry:
//      -
//
// Exit:
//      TRUE if successful, FALSE if error.
//
//---------------------------------------------------------------------------
BOOL DeInitCleanCache (
    VOID
    )
    {
#if defined(SYM_NTK) && defined(_X86_)

    if ( rNAVAPInternalCfg.dwFileCacheEntries &&
         AsfDeInitCache() == STATUS_SUCCESS )
        {
        return ( TRUE );
        }

    return ( FALSE );

#else

    return ( TRUE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL IsFileInCleanCache (
//      __int64 i64FileID
//      );
//
// This function searches the cache of clean files for the file specified
// by i64FileID.  This cache is used to speed up access to files which
// have already been scanned, and which are clean.
//
// Entry:
//      i64FileID - unique file ID
//
// Exit:
//      TRUE  if file was found in the cache
//      FALSE if file was not found in the cache
//
//---------------------------------------------------------------------------
BOOL IsFileInCleanCache (
    __int64 i64FileID
    )
    {
#if defined(SYM_NTK) && defined(_X86_)

                                        // Make sure the file caching is
                                        // enabled.

    if ( !rNAVAPInternalCfg.dwFileCacheEntries )
        {
        return ( FALSE );
        }

                                        // Make sure the file ID is valid.

    if ( i64FileID )
        {
                                        // Check if the file ID is cached.

        return ( AsfIsFileIdInCache ( (unsigned __int64 *) &i64FileID ) );
        }

    return ( FALSE );

#else

    return ( FALSE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL IsFileInCleanCache (
//      LPED lpEventData
//      );
//
// This function searches the cache of clean files for the file specified
// by lpEventData.  This cache is used to speed up access to files which
// have already been scanned, and which are clean.
//
// Entry:
//      lpEventData - event data structure created by SymEvent.
//
// Exit:
//      TRUE  if file was found in the cache
//      FALSE if file was not found in the cache
//
// Note:
//      This function will work only if the file ID has been previously
//      saved at the open-file-data area associated with the file at
//      lpEventData.
//
//---------------------------------------------------------------------------
BOOL IsFileInCleanCache (
    LPED lpEventData
    )
    {
#if defined(SYM_NTK) && defined(_X86_)

    auto __int64 i64FileID;

                                        // Validate input parameters.

    SYM_ASSERT ( lpEventData );

                                        // Make sure the file caching is
                                        // enabled.

    if ( !rNAVAPInternalCfg.dwFileCacheEntries )
        {
        return ( FALSE );
        }

                                        // Get the file ID.

    if ( FileMonitorGetFileID ( lpEventData, &i64FileID ) )
        {
                                        // Validate the file ID.

        SYM_ASSERT ( i64FileID );

                                        // Check if the file ID is cached.

        return ( AsfIsFileIdInCache ( (unsigned __int64 *) &i64FileID ) );
        }

    return ( FALSE );

#else

    return ( FALSE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL AddFileToCleanCache (
//      LPED lpEventData
//      );
//
// This function adds a file to the cache of clean files.
//
// Entry:
//      lpEventData - event data structure created by SymEvent.
//
// Exit:
//      TRUE  if file was added to the cache
//      FALSE if error
//
// Note:
//      This function will work only if the file ID has been previously
//      saved at the open-file-data area associated with the file at
//      lpEventData.
//
//---------------------------------------------------------------------------
BOOL AddFileToCleanCache (
    LPED lpEventData
    )
    {
#if defined(SYM_NTK) && defined(_X86_)

    auto __int64  i64FileID;
    auto NTSTATUS Status;

                                        // Validate input parameters.

    SYM_ASSERT ( lpEventData );

                                        // Make sure the file caching is
                                        // enabled.

    if ( !rNAVAPInternalCfg.dwFileCacheEntries )
        {
        return ( TRUE );
        }

                                        // Get the file ID.

    if ( FileMonitorGetFileID ( lpEventData, &i64FileID ) )
        {
                                        // Validate the file ID.

        SYM_ASSERT ( i64FileID );

                                        // Add the file ID to the cache.

        Status = AsfAddFileIdToCache ( (unsigned __int64 *) &i64FileID );

        return ( NT_SUCCESS(Status) ? TRUE : FALSE );
        }

    return ( FALSE );

#else

    return ( FALSE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL RemoveFileFromCleanCache (
//      LPED lpEventData
//      );
//
// This function removes a file from the cache of clean files.
//
// Entry:
//      lpEventData - event data structure created by SymEvent.
//
// Exit:
//      TRUE  if file was removed from the cache
//      FALSE if error
//
// Note:
//      This function will work only if the file ID has been previously
//      saved at the open-file-data area associated with the file at
//      lpEventData.
//
//---------------------------------------------------------------------------
BOOL RemoveFileFromCleanCache (
    LPED lpEventData
    )
    {
#if defined(SYM_NTK) && defined(_X86_)

    auto __int64  i64FileID;
    auto NTSTATUS Status;

                                        // Validate input parameters.

    SYM_ASSERT ( lpEventData );

                                        // Make sure the file caching is
                                        // enabled.

    if ( !rNAVAPInternalCfg.dwFileCacheEntries )
        {
        return ( TRUE );
        }

                                        // Get the file ID.

    if ( FileMonitorGetFileID ( lpEventData, &i64FileID ) )
        {
                                        // Validate the file ID.

        SYM_ASSERT ( i64FileID );

                                        // Remove the file ID from the cache.

        Status = AsfRemoveFileIdFromCache ( (unsigned __int64 *) &i64FileID );

        return ( NT_SUCCESS(Status) ? TRUE : FALSE );
        }

    return ( FALSE );

#else

    return ( FALSE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL FlushCleanCache (
//      VOID
//      );
//
// This function removes all files from the clean file cache.
//
// Entry:
//      -
//
// Exit:
//      TRUE  if all files were removed from the cache
//      FALSE if error
//
//---------------------------------------------------------------------------
BOOL FlushCleanCache (
    VOID
    )
    {
#if defined(SYM_NTK) && defined(_X86_)

    if ( !rNAVAPInternalCfg.dwFileCacheEntries )
        {
        return ( TRUE );
        }

    return ( NT_SUCCESS ( AsfFlushCache() ) ? TRUE : FALSE );

#else

    return ( TRUE );

#endif
    }

//---------------------------------------------------------------------------
//
// Define status variables for the FileOpenSync...() APIs.
//
//---------------------------------------------------------------------------
BOOL bFileOpenEventInitialized   = FALSE;
BOOL bFileOpenEventAddingAllowed = FALSE;

//---------------------------------------------------------------------------
//
// The FILEOPENEVENT structure is used by the FileOpenSyncEnter() and
// FileOpenSyncExit() APIs to syncrhonize access to files during file
// open events.
//
// prFileOpenEventList is the head of a linked list of FILEOPENEVENT
// structures.  Each entry in the list represents a unique file that is
// currently being processed by NAVAP.
//
//---------------------------------------------------------------------------
typedef struct tagFILEOPENEVENT
    {
    tagFILEOPENEVENT *prNext;
    tagFILEOPENEVENT *prPrev;
    PTSTR            pszFileName;
    LONG             lBlockedCount;

#if defined(SYM_NTK)

    KEVENT           FileNameUpdate;
    KMUTANT          Mutant;
  #if defined(SYM_DEBUG) || defined(PRERELEASE)
    PETHREAD         OwnerThread;
  #endif

#endif // #if defined(SYM_NTK)

    } FILEOPENEVENT, *PFILEOPENEVENT;

PFILEOPENEVENT prFileOpenEventList = NULL;

//---------------------------------------------------------------------------
//
// BOOL FileOpenSyncInit (
//      VOID
//      );
//
// This function initializes the FileOpenSync...() APIs.  It acquires
// full access to the list and the APIs, then validates that it is called
// properly (APIs not initialized, adding disabled), then initializes the
// APIs and enables adding to the list.
//
// Entry:
//      -
//
// Exit:
//      Returns always TRUE.
//
//---------------------------------------------------------------------------
BOOL FileOpenSyncInit (
    VOID
    )
    {
    SymAcquireFastGlobalLock();

    SYM_ASSERT ( !bFileOpenEventInitialized );
    SYM_ASSERT ( !bFileOpenEventAddingAllowed );

    bFileOpenEventInitialized   = \
    bFileOpenEventAddingAllowed = TRUE;

    SymReleaseFastGlobalLock();

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL FileOpenSyncDeInit (
//      VOID
//      );
//
// This function deinitializes the FileOpenSync...() APIs.  It acquires
// full access to the list and the APIs, then validates that it is called
// properly (APIs initialized, but adding disabled) and uninitializes
// the APIs.
//
// Entry:
//      -
//
// Exit:
//      Returns always TRUE.
//
//---------------------------------------------------------------------------
BOOL FileOpenSyncDeInit (
    VOID
    )
    {
    SymAcquireFastGlobalLock();

    SYM_ASSERT ( !bFileOpenEventAddingAllowed );
    SYM_ASSERT ( bFileOpenEventInitialized );

    bFileOpenEventInitialized = FALSE;

    SymReleaseFastGlobalLock();

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// VOID FileOpenSyncCleanup (
//      VOID
//      );
//
// This function disables adding to the file open sync APIs, then waits for
// all current clients to stop using the APIs.  Once this function
// completes, you should be safe calling FileOpenSyncDeInit().
//
// Entry:
//      -
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID FileOpenSyncCleanup (
    VOID
    )
    {
                                        // Get full access to the list and
                                        // the APIs.

    SymAcquireFastGlobalLock();

                                        // We should be initialized and
                                        // adding should be enabled at that
                                        // point.

    SYM_ASSERT ( bFileOpenEventInitialized );
    SYM_ASSERT ( bFileOpenEventAddingAllowed );

                                        // Disable adding new items to the
                                        // list.

    bFileOpenEventAddingAllowed = FALSE;

                                        // Release the access to the list.

    SymReleaseFastGlobalLock();

                                        // Wait for the list to become
                                        // empty.  This is a cheap way of
                                        // doing it, but it is also faster
                                        // than many other more straight-
                                        // forward methods.

    while ( prFileOpenEventList )
        {
        Sleep ( 100 );
        }

                                        // Just make sure the list is empty.

    SYM_ASSERT ( !prFileOpenEventList );
    }

//---------------------------------------------------------------------------
//
// PVOID FileOpenSyncEnter (
//      LPED lpEventData
//      );
//
// This function synchronizes access to the file at lpEventData.  If
// FileOpenSyncEnter() was already called once for that file, this function
// will block until the thread that is processing the file calls
// FileOpenSyncExit().
//
// Entry:
//      lpEventData - pointer to an event data structure, as given to us
//                    by SYMEvent.
//
// Exit:
//      Returns a pointer to a synchronization object if successful, or
//      NULL if error.  Upon successful completion, the client can use the
//      file at lpEventData and can be sure that no other clients of
//      FileOpenSync...() will be granted access to this file.  After the
//      client is done with the file, a call to FileOpenSyncExit() must be
//      made, passing it the pointer returned by FileOpenSyncEnter().
//
// Note:
//      !!! READ CAREFULLY !!!
//      This function currently relies on a specific SYMEvent implementation
//      for increased performance.  It uses the fact that SYMEvent keeps
//      the buffer pointed to by lpEventData->SE_ED_lpPathSpec in the
//      per-file data area.  This means that if the file object at
//      lpEventData is destroyed, and we still have an entry for that file
//      in our linked list, it will become invalid!
//
//---------------------------------------------------------------------------
PVOID FileOpenSyncEnter (
    LPED lpEventData
    )
    {
#if defined(SYM_NTK)

    auto PFILEOPENEVENT prFOEvent;

                                        // Validate input parameters.

    SYM_ASSERT ( lpEventData );
    SYM_ASSERT ( lpEventData->SE_ED_lpPathSpec );
    SYM_VERIFY_STRING ( lpEventData->SE_ED_lpPathSpec );

                                        // Give us full access to the list of
                                        // files in use.

    SymAcquireFastGlobalLock();

                                        // Check if we are initialized and if
                                        // we are allowed to add new items.

    if ( !bFileOpenEventAddingAllowed )
        {
        SymReleaseFastGlobalLock();

        return ( NULL );
        }

    SYM_ASSERT ( bFileOpenEventInitialized );

                                        // Search the list for the file which
                                        // we need to process.

    for ( prFOEvent = prFileOpenEventList;
          prFOEvent && STRICMP ( lpEventData->SE_ED_lpPathSpec, prFOEvent->pszFileName );
          prFOEvent = prFOEvent->prNext );

    if ( prFOEvent )
        {
                                        // We have an entry for this file,
                                        // therefore its mutant is acquired
                                        // by someone else.  Increment the
                                        // blocked thread count, so that
                                        // whenever the mutant's owner is
                                        // done with processing, it signals
                                        // the mutant instead of removing the
                                        // entry.

        prFOEvent->lBlockedCount++;

                                        // Release the access to the list of
                                        // files in use and wait for the
                                        // mutant to be signaled.  Once we
                                        // acquire the mutant in that entry,
                                        // we are guaranteed that all other
                                        // clients will block until we signal
                                        // the mutant.  Also, once the mutant
                                        // is acquired by us, the previous
                                        // owner of the mutant will have the
                                        // list locked, and it will be
                                        // waiting for us to signal the event
                                        // object in prFOEvent before
                                        // releasing the lock.

        SymReleaseFastGlobalLock();

        switch ( KeWaitForSingleObject ( &prFOEvent->Mutant,
                                         Executive,
                                         KernelMode,
                                         FALSE,
                                         NULL ) )
            {
            case STATUS_SUCCESS:
                break;
            case STATUS_ALERTED:
                SYM_ASSERT ( FALSE );
            case STATUS_USER_APC:
                SYM_ASSERT ( FALSE );
            case STATUS_TIMEOUT:
                SYM_ASSERT ( FALSE );
            default:
                SYM_ASSERT ( FALSE );
            }

                                        // Start using the filename in the
                                        // current event data structure,
                                        // since the original event has to
                                        // complete as soon as possible.

        prFOEvent->pszFileName = lpEventData->SE_ED_lpPathSpec;

                                        // Decrement the counter of blocked
                                        // threads so that we don't get
                                        // confused later.

        prFOEvent->lBlockedCount--;

                                        // Update the owner of the entry.

#if defined(SYM_NTK)

        SYM_ASSERT ( prFOEvent->OwnerThread = PsGetCurrentThread() );

#endif // #if defined(SYM_NTK)

                                        // Allow the event previously owning
                                        // the mutant to complete, thus
                                        // releasing the lock on the list.

        KeSetEvent ( &prFOEvent->FileNameUpdate, 0, FALSE );
        }
    else
        {
                                        // Not processing this file.  Create
                                        // an entry for the list.

        if ( prFOEvent = (PFILEOPENEVENT) MemAllocPtr ( GMEM_FIXED,
                                                        sizeof(*prFOEvent) )
           )
            {
                                        // Link the entry into the list.

            if ( prFOEvent->prNext = prFileOpenEventList )
                {
                prFileOpenEventList->prPrev = prFOEvent;
                }

            prFOEvent->prPrev = NULL;

            prFileOpenEventList = prFOEvent;

                                        // Set the filename.

            prFOEvent->pszFileName = lpEventData->SE_ED_lpPathSpec;

                                        // Initialize blocked thread counter.

            prFOEvent->lBlockedCount = 0;

                                        // Set the owner of the entry.

#if defined(SYM_NTK)

            SYM_ASSERT ( prFOEvent->OwnerThread = PsGetCurrentThread() );

#endif // #if defined(SYM_NTK)

                                        // Initialize the mutant, and acquire
                                        // it immediately.

            KeInitializeMutant ( &prFOEvent->Mutant, TRUE );

                                        // Initialize the event object for
                                        // synchronizing the updates of
                                        // pszFileName.  It's normal state
                                        // should be non-signaled.

            KeInitializeEvent ( &prFOEvent->FileNameUpdate,
                                NotificationEvent,
                                FALSE );
            }

        SymReleaseFastGlobalLock();
        }

    return ( prFOEvent );

#else

    return ( ~NULL );

#endif
    }

//---------------------------------------------------------------------------
//
// VOID FileOpenSyncExit (
//      PVOID pvSyncObject
//      );
//
// This function releases the lock on a file object, acquired earlier by
// FileOpenSyncEnter() for the same file object (read the note under
// FileOpenSyncEnter())
//
// Entry:
//      pvSyncObject - a valid pointer to synchronization object returned
//                     by FileOpenSyncEnter().
//
// Exit:
//      File lock released.  If other threads were waiting for access to
//      that file, then the thread which had waited the longest (that's my
//      guess) will be given access to that file.
//
//---------------------------------------------------------------------------
VOID FileOpenSyncExit (
    PVOID pvSyncObject
    )
    {
#if defined(SYM_NTK)

    auto PFILEOPENEVENT prFOEvent = (PFILEOPENEVENT) pvSyncObject;
    auto LONG           lBlockedCount;

                                        // Validate input parameters.

    SYM_ASSERT ( pvSyncObject );

                                        // Give us full access to the list of
                                        // files in use.

    SymAcquireFastGlobalLock();

                                        // Make sure we are initialized.

    if ( !bFileOpenEventInitialized )
        {
        SymReleaseFastGlobalLock();

        return;
        }

                                        // Save the counter of blocked
                                        // threads, because after releasing
                                        // the mutant this counter may change
                                        // almost immediately.

    lBlockedCount = prFOEvent->lBlockedCount;

                                        // Signal the mutant, thus releasing
                                        // a blocked thread, if one was
                                        // waiting.

    KeReleaseMutant ( &prFOEvent->Mutant, 1, FALSE, FALSE );

                                        // Check to see if a thread was in
                                        // fact released.

    if ( !lBlockedCount )
        {
                                        // If there were no threads waiting
                                        // on the mutant, this means that we
                                        // can unlink the entry and release
                                        // the allocated memory.

        if ( prFOEvent->prNext )
            {
            prFOEvent->prNext->prPrev = prFOEvent->prPrev;
            }

        if ( prFOEvent->prPrev )
            {
            prFOEvent->prPrev->prNext = prFOEvent->prNext;
            }
        else
            {
            prFileOpenEventList = prFOEvent->prNext;
            }

        MemFreePtr ( prFOEvent );
        }
    else
        {
                                        // If there was a thread waiting,
                                        // then we have to wait for it to
                                        // update pszFileName before we
                                        // release the lock on the list and
                                        // complete the event.  Otherwise, a
                                        // new client may come in, and try
                                        // to use pszFileName, which is still
                                        // pointing to the buffer of the
                                        // completed lpEventData.

        switch ( KeWaitForSingleObject ( &prFOEvent->FileNameUpdate,
                                         Executive,
                                         KernelMode,
                                         FALSE,
                                         NULL ) )
            {
            case STATUS_SUCCESS:
                break;
            case STATUS_ALERTED:
                SYM_ASSERT ( FALSE );
            case STATUS_USER_APC:
                SYM_ASSERT ( FALSE );
            case STATUS_TIMEOUT:
                SYM_ASSERT ( FALSE );
            default:
                SYM_ASSERT ( FALSE );
            }

        KeClearEvent ( &prFOEvent->FileNameUpdate );
        }

    SymReleaseFastGlobalLock();

#endif // #if defined(SYM_NTK)
    }
