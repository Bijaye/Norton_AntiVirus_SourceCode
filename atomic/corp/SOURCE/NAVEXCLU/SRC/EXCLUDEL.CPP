// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEXCLU/VCS/excludel.cpv   1.2   20 Aug 1997 18:14:46   MKEATIN  $
//
// Description:
//      Routines to handle the long filename version of the exclude
//      list.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/NAVEXCLU/VCS/excludel.cpv  $
// 
//    Rev 1.2   20 Aug 1997 18:14:46   MKEATIN
// In ExcludeLValidHeader() use EXCLUDEL_DAT to calculate the filesize - not
// EXCLUDEL as it is a different size in memory under Unicode.
// 
//    Rev 1.1   16 May 1997 06:49:46   RStanev
// Removed a static used to read/write excludels.
//
//    Rev 1.0   06 Feb 1997 20:57:50   RFULLER
// Initial revision
//
//    Rev 1.9   12 Sep 1996 16:55:02   JBRENNA
// When on Win95 (i.e. non-Unicode), the exclusion list is already in OEM
// characters. Therefore, when writing the exclusion list out to file and
// reading it in, we do not need to convert between Ansi and OEM. Removed
// the translation code.
//
//    Rev 1.8   18 Jul 1996 14:40:20   djesch
// Added 'address-of' operator as per TCASHIN for compiler error under
// DX.
//
//    Rev 1.7   17 Jul 1996 10:45:54   jworden
// Fix call to NAnsiToOem. Parameters were backwards causing us to save
// old stuff
//
//    Rev 1.6   01 Jul 1996 14:35:42   RSTANEV
// When we are dealing with a string of CHAR or WCHAR, not TCHAR, we cannot
// use the capitalized STR functions.  Instead, we should use the lower-case
// versions of the RTL (str... and wcs...)
//
//    Rev 1.5   27 Jun 1996 16:45:00   RHAYES
// Luigi merge - update strcmp & strcpy to MACRO equiv.'s.
//
//    Rev 1.3   10 May 1996 10:58:50   RHAYES
// Unicode conversion update.
//
//    Rev 1.2   06 May 1996 16:42:50   RSTANEV
// Merged Ron's Unicode changes.
//
//    Rev 1.1   19 Mar 1996 13:52:42   jworden
// Double byte enable
//
//    Rev 1.0   30 Jan 1996 15:52:56   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 13:22:00   BARRY
// Initial revision.
//
//    Rev 1.12   08 Jul 1995 21:19:50   RHAYES
// Added code to ExcludeLAdd() to look for an existing entry in our long file
// list before blindly adding a new entry. If an existing entry is found then
// the settings are updated with new flags (if any).
//
//    Rev 1.11   28 Jun 1995 19:31:56   RHAYES
// Before the LFN entries are written out to disk, each entry is converted to OEM.
// The problem was that conversion was done on the active, in memory, copy; now
// the conversion is reversed immediately after the write to assure that our
// active copy remains in native Windows Ansi format.
//
//
//    Rev 1.10   26 Jun 1995 23:54:18   RSTANEV
// Now properly converting the exclude DB location to current character set
// when initializing a new LFN exclusion structure.
//
//    Rev 1.9   23 Jun 1995 13:37:16   KEITH
// Changed the functionaly of how we verify the EXCLUDEL.DAT file so
// that a 0-byte file will cause a create rather than problems STS# 38396.
// Now, any file less than the header size will be treated as no file and
// the create mechanism will be called.
//
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "excludel.h"

// ---------------------------------------------------------------------------
//                  LOCAL FUNCTION TO THIS MODULE
// ---------------------------------------------------------------------------


#define EXCLOPEN_HAS_HEADER     0
#define EXCLOPEN_NO_HEADER      1
#define EXCLOPEN_HEADER_BAD     2


UINT LOCAL ExcludeLValidHeader (        // Validates file header.
    HFILE   hFile,                      // [in] File handle
    LPWORD  lpwNumEntries               // [out] number of records in file.
    );

BOOL LOCAL ExcludeLWriteHeader (        // Write header to file.
    HFILE               hFile,          // [in] file handle to write to
    LPEXCLUDELSTRUCT    lpLExclude      // [in] info about the file
    );

// ---------------------------------------------------------------------------


/*@API:*************************************************************************
@Declaration:

HLEXCLUDE WINAPI ExcludeLOpen (         // To open/create and excludel.dat file
    LPCTSTR      lpExcludeFile,         // [in] name of file to open/create.
    BOOL        bCreate                 // [in] flag to create file
    );

@Description:
This function opens or creates a file that will store long filename exclusion
entries.

@Parameters:
$lpExcludeFile$ path to the file to open or create. (in OEM for SYM_VXD)
$bCreate$ flag is set to TRUE if the file should be created if it doesn't exist

@Returns:
A handle to the file that should be passed to the other family functions to
do some action upon the file. NULL if dudu happened.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

HLEXCLUDE WINAPI ExcludeLOpen (         // To open/create an excludel.dat file
    LPCTSTR      lpExcludeFile,         // [in] name of file to open/create.
    BOOL        bCreate                 // [in] flag to create file
    )
{
    SYM_ASSERT ( lpExcludeFile );
    SYM_ASSERT ( *lpExcludeFile );

    auto LEXCLUDE_DAT gLExcludeDat;      // Single record from ExcludeL.Dat

    auto    HLEXCLUDE           hLExclude = NULL;
    auto    LPEXCLUDELSTRUCT    lpLExclude;
    auto    BOOL                bErrorOccured = FALSE;
    auto    UINT                uiHeaderState = EXCLOPEN_NO_HEADER;
    auto    WORD                wIndex;
    auto    LPLEXCLUDE          lpEntry;
#if defined(SYM_VXD)
    auto    TCHAR               szExcludeFile[SYM_MAX_PATH * 2];

    VxDGenericStringConvert (
        (LPTSTR)lpExcludeFile,
        THFLAG_OEM,
        szExcludeFile,
        ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK );
#endif


                                        // --------------------------------
                                        // Allocate mem for structure that
                                        // contains info to manage file.
                                        // --------------------------------

    lpLExclude = (LPEXCLUDELSTRUCT)MemAllocPtr ( GHND, sizeof(EXCLUDELSTRUCT) );

    if ( lpLExclude )
        {
                                        // --------------------------------
                                        // Initialize stucture contents to
                                        // default values.
                                        // --------------------------------

        lpLExclude->wNumEntries     =   0;
        lpLExclude->lpEntries       =   NULL;
        lpLExclude->bIsDirty        =   FALSE;
        lpLExclude->dwEntriesSize   =   0;
        *lpLExclude->szFilePath     =   EOS;

                                        // --------------------------------
                                        // Try opening the file.
                                        // --------------------------------

#if defined(SYM_VXD)
        HFILE hFile = FileOpen ( szExcludeFile, READ_ONLY_FILE );
#else
        HFILE hFile = FileOpen ( lpExcludeFile, READ_ONLY_FILE );
#endif

                                        // --------------------------------
                                        // Make sure file header is OK.
                                        // --------------------------------

        if ( hFile != HFILE_ERROR )
            {
            uiHeaderState = ExcludeLValidHeader ( hFile,
                                                  &lpLExclude->wNumEntries );
            if ( uiHeaderState != EXCLOPEN_HAS_HEADER )
                {
                FileClose ( hFile );
                hFile = HFILE_ERROR;
                }
            }
                                        // --------------------------------
                                        // On error try to create it if
                                        // the file is too small to have
                                        // a header or if the file doesn't
                                        // Exist. A file header that can
                                        // be read but has invalid data
                                        // is explicitly excluded from the
                                        // re-create process.
                                        // --------------------------------

        if ( hFile == HFILE_ERROR
                && bCreate == TRUE
                    && uiHeaderState != EXCLOPEN_HEADER_BAD )
            {
#if defined(SYM_VXD)
            hFile = FileCreate ( szExcludeFile, FA_NORMAL );
#else
            hFile = FileCreate ( lpExcludeFile, FA_NORMAL );
#endif

            if ( ExcludeLWriteHeader ( hFile, lpLExclude ) != TRUE )
                {
                FileClose ( hFile );
                hFile = HFILE_ERROR;
                }
            }

                                        // --------------------------------
                                        // If we have a valid handle ...
                                        // --------------------------------

        if ( hFile != HFILE_ERROR )
            {
                {
                                    // --------------------------------
                                    // Compute bytes to allocate.
                                    // --------------------------------

                lpLExclude->dwEntriesSize  = lpLExclude->wNumEntries /
                                             ENTRIES_PER_BLOCK;
                lpLExclude->dwEntriesSize += 1;
                lpLExclude->dwEntriesSize *= ENTRIES_PER_BLOCK;
                lpLExclude->dwEntriesSize *= sizeof(LEXCLUDE);

                lpLExclude->lpEntries = (LPLEXCLUDE)MemAllocPtr (
                                                GHND,
                                                lpLExclude->dwEntriesSize
                                                );
                if ( lpLExclude->lpEntries )
                    {
                                    // --------------------------------
                                    // Seek past header to 1st record
                                    // Read in file contents one record
                                    // at a time
                                    // --------------------------------
                    FileSeek ( hFile, sizeof(EXCLUDELHEADER), SEEK_SET );

                    for ( wIndex = 0, bErrorOccured = FALSE,
                          lpEntry = lpLExclude->lpEntries;
                          ( !bErrorOccured ) &&
                          ( wIndex < lpLExclude->wNumEntries );
                          ++wIndex, ++lpEntry )
                       {
                       if ( FileRead ( hFile, &gLExcludeDat,
                                       sizeof(gLExcludeDat) ) !=
                            sizeof(gLExcludeDat) )
                           {
                            bErrorOccured = TRUE;
                           }
                         else
                           {
#ifdef _UNICODE
                           MultiByteToWideChar( CP_OEMCP, 0,
                                                gLExcludeDat.szItem,
                                                -1, lpEntry->szItem,
                                                sizeof(lpEntry->szItem) /
                                                sizeof(TCHAR) );
#else
                           STRCPY( lpEntry->szItem, gLExcludeDat.szItem );
#endif
                           lpEntry->bSubDirectory = gLExcludeDat.bSubDirectory;
                           lpEntry->wFlags        = gLExcludeDat.wFlags;
                           }
                       }
                    }
                }
                                        // --------------------------------
                                        // If the file contained some info
                                        // it was sucked in memory. We're
                                        // done with it.
                                        // --------------------------------
            FileClose ( hFile );
            }
        else
            {
                                        // At this point, the file could not
                                        // be created either due to a physical
                                        // create problem (R/O?, disk error?)
                                        // or it is not valid and we cannot
                                        // recreate it.
            bErrorOccured = TRUE;
            }

                                        // --------------------------------
                                        // If error occured clean up othewise
                                        // return a valid handle.
                                        // --------------------------------
        if ( bErrorOccured == TRUE )
            {
            if ( lpLExclude->lpEntries )
                MemFreePtr ( lpLExclude->lpEntries );

            MemFreePtr ( lpLExclude );
            }
        else
            {
            STRCPY ( lpLExclude->szFilePath, lpExcludeFile );
            hLExclude = lpLExclude;
            }
        }

    return ( hLExclude );
}

/*@API:*************************************************************************
@Declaration:

STATUSEXCLUDE WINAPI ExcludeLClose (    // Closes a file opened ExcludeLOpen()
    HLEXCLUDE   hLExclude               // [in] handle to file to close.
    );

@Description:
This function closes the file that was created/opened with the ExcludeLOpen()
function.

@Parameters:
$hLExclude$ handle that was returned by the ExcludeLOpen() function

@Returns:
STATUSEXCLUDE_OK    if the file was successfully closed.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

STATUSEXCLUDE WINAPI ExcludeLClose (    // Closes a file opened ExcludeLOpen()
    HLEXCLUDE       hLExclude,          // [in] handle to file to close.
    EXCLUDELACTION  Action              // [in] flag indicating to free memory
    )
{
    SYM_ASSERT ( hLExclude );

    auto LEXCLUDE_DAT gLExcludeDat;      // Single record from ExcludeL.Dat

    auto    STATUSEXCLUDE   Status = STATUSEXCLUDE_ERR;
    auto    WORD            wIndex;
    auto    LPLEXCLUDE      lpEntry;

    if ( hLExclude )
        {
        BOOL bErrorOccured = FALSE;
        LPEXCLUDELSTRUCT lpLExclude = hLExclude;

#if defined(SYM_VXD)
        auto TCHAR szFilePath[SYM_MAX_PATH * 2];

        VxDGenericStringConvert (
            lpLExclude->szFilePath,
            THFLAG_OEM,
            szFilePath,
            ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK );
#endif

                                        // -----------------------------------
                                        // If dirty flag is set then the
                                        // file must be updated.
                                        // -----------------------------------

        if ( lpLExclude->bIsDirty == TRUE &&
             ( Action == EXCLUDELACTION_SAVEDONTFREEMEM ||
               Action == EXCLUDELACTION_SAVEFREEMEM ) )
            {
            SYM_ASSERT ( STRLEN ( lpLExclude->szFilePath ) );

                                        // -----------------------------------
                                        // Zap original file.
                                        // -----------------------------------

#if defined(SYM_VXD)
            FileDelete ( szFilePath );
#else
            FileDelete ( lpLExclude->szFilePath );
#endif

                                        // -----------------------------------
                                        // So it can be re-created.
                                        // -----------------------------------

#if defined(SYM_VXD)
            HFILE hFile = FileCreate ( szFilePath, FA_NORMAL );
#else
            HFILE hFile = FileCreate ( lpLExclude->szFilePath, FA_NORMAL );
#endif

            if ( hFile != HFILE_ERROR )
                {
                if ( ExcludeLWriteHeader ( hFile, lpLExclude ) == TRUE )
                    {
                                       // -----------------------------------
                                       // RLH: Convert/write out one record
                                       // at a time
                                       // -----------------------------------
                    for ( wIndex = 0, bErrorOccured = FALSE,
                          lpEntry = lpLExclude->lpEntries;
                          ( !bErrorOccured ) &&
                          ( wIndex < lpLExclude->wNumEntries );
                          ++wIndex, ++lpEntry )
                       {
#ifdef _UNICODE
                       WideCharToMultiByte( CP_OEMCP, 0,
                                            lpEntry->szItem, -1,
                                            gLExcludeDat.szItem,
                                            sizeof(gLExcludeDat.szItem),
                                            NULL,
                                            NULL );
                       gLExcludeDat.bSubDirectory = lpEntry->bSubDirectory;
                       gLExcludeDat.wFlags        = lpEntry->wFlags;
#else
                       MEMCPY( &gLExcludeDat, lpEntry, sizeof(gLExcludeDat) );
#endif
                                        // -----------------------------------
                                        // Write out current entry
                                        // -----------------------------------
                       if ( FileWrite( hFile, &gLExcludeDat,
                                       sizeof(gLExcludeDat) ) !=
                                       sizeof(gLExcludeDat) )
                           {
                           bErrorOccured = TRUE;
                           }
                       }
                    }
                else
                    {
                    bErrorOccured = TRUE;
                    }

                FileClose ( hFile );
                }
            else
                {
                bErrorOccured = TRUE;
                }
            }
                                        // -----------------------------------
                                        // If error occured clean up otherwise
                                        // indicate success.
                                        // -----------------------------------
        if ( bErrorOccured == TRUE )
            {
#if defined(SYM_VXD)
            FileDelete ( szFilePath );
#else
            FileDelete ( lpLExclude->szFilePath );
#endif
            }
        else
            {
            Status = STATUSEXCLUDE_OK;
            }

        if ( Action == EXCLUDELACTION_DONTSAVEFREEMEM ||
             Action == EXCLUDELACTION_SAVEFREEMEM )
            {
            if ( lpLExclude->lpEntries )
                MemFreePtr ( lpLExclude->lpEntries );

            MemFreePtr ( lpLExclude );
            }
        }

    return ( Status );
}


/*@API:*************************************************************************
@Declaration:

WORD WINAPI ExcludeLGetCount (          // Obtain number of entries in file
    HLEXCLUDE   hLExclude               // [in] handle to file to query
    );

@Description:
This function obtains the number of entries in the file that was opened with
the ExcludeLOpen() function.

@Parameters:
$hLExclude$ handle that was returned by the ExcludeLOpen() function

@Returns:
Number of items in the file.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

WORD WINAPI ExcludeLGetCount (          // Obtain number of entries in file
    HLEXCLUDE   hLExclude               // [in] handle to file to query
    )
{
    SYM_ASSERT ( hLExclude );

    auto    WORD    wCount = 0;

    if ( hLExclude )
        {
        LPEXCLUDELSTRUCT lpLExclude = hLExclude;

        wCount = lpLExclude->wNumEntries;
        }

    return ( wCount );
}

/*@API:*************************************************************************
@Declaration:

STATUSEXCLUDE WINAPI ExcludeLAdd (      // Adds an entry to the file
    HLEXCLUDE   hLExclude,              // [in] handle to the file to add
    LPLEXCLUDE  lpItem                  // [in] pointer to item to add
    );

@Description:
This function adds an entry to the file that was opened with the
ExcludeLOpen() function.

@Parameters:
$hLExclude$ handle that was returned by the ExcludeLOpen() function
$lpItem$ pointer to the item to add.

@Returns:
STATUSEXCLUDE_OK         if the item was successfully added.
STATUSEXCLUDE_DUPLICATE  if there's a duplicate.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

STATUSEXCLUDE WINAPI ExcludeLAdd (      // Adds an entry to the file
    HLEXCLUDE   hLExclude,              // [in] handle to the file to add
    LPLEXCLUDE  lpItem                  // [in] pointer to item to add
    )
{
    STATUSEXCLUDE    Status = STATUSEXCLUDE_ERR;
    WORD             wCount;
    BOOL             bErrorOccured = FALSE;
    LPEXCLUDELSTRUCT lpLExclude = hLExclude;
    LPLEXCLUDE       lpLEntry;

    SYM_ASSERT ( hLExclude );
    SYM_ASSERT ( lpItem );
    if ( hLExclude )
        {
                                        // First make sure that we don't
                                        // have a duplicate entry. If one
                                        // is found then simply OR in the
                                        // new settings, if any.
        for ( wCount = 0, lpLEntry = lpLExclude->lpEntries;
              wCount < lpLExclude->wNumEntries; ++wCount, ++lpLEntry )
            {
            if ( !STRICMP( lpLEntry->szItem, lpItem->szItem ) )
                {
                lpLEntry->bSubDirectory |= lpItem->bSubDirectory;
                lpLEntry->wFlags        |= lpItem->wFlags;
                return( STATUSEXCLUDE_DUPLICATE );
                }
            }

                                        // --------------------------------
                                        // Compute bytes to needed.
                                        // --------------------------------
        DWORD dwNeed = ( lpLExclude->wNumEntries + 1 ) * sizeof(LEXCLUDE);

                                        // --------------------------------
                                        // If we need more than what we have
                                        // then reallocate buffer.
                                        // --------------------------------
        if ( dwNeed >= lpLExclude->dwEntriesSize )
            {
                                        // --------------------------------
                                        // Compute new size needed.
                                        // --------------------------------
            DWORD dwNewSize = lpLExclude->dwEntriesSize;
            dwNewSize += (ENTRIES_PER_BLOCK * sizeof(LEXCLUDE));

            LPLEXCLUDE lpTemp = (LPLEXCLUDE)MemReAllocPtr (
                                            lpLExclude->lpEntries,
                                            dwNewSize,
                                            GHND
                                            );
            if ( lpTemp )
                {
                                        // --------------------------------
                                        // If realloc OK fixup variables.
                                        // --------------------------------

                lpLExclude->lpEntries     = lpTemp;
                lpLExclude->dwEntriesSize = dwNewSize;
                }
            else
                {
                bErrorOccured = TRUE;
                }
            }
                                        // --------------------------------
                                        // If no errors add new item to list.
                                        // --------------------------------
        if ( bErrorOccured == FALSE )
            {
            lpLExclude->bIsDirty = TRUE;

                                        // Force all new names to upper case
            MEMCPY ( &lpLExclude->lpEntries[lpLExclude->wNumEntries],
                    lpItem, sizeof(LEXCLUDE) );

            lpLExclude->wNumEntries++;

            Status = STATUSEXCLUDE_OK;
            }
        }

    return ( Status );
}


/*@API:*************************************************************************
@Declaration:

STATUSEXCLUDE WINAPI ExcludeLGet (      // Obtain an item from the file
    HLEXCLUDE   hLExclude,              // [in] handle to file
    DWORD       dwIndex,                // [in] record number to get
    LPLEXCLUDE  lpItem                  // [out] info placed here
    );

@Description:
This function obtains an entry from the file that was opened with the
ExcludeLOpen() function.

@Parameters:
$hLExclude$ handle that was returned by the ExcludeLOpen() function
$dwIndex$ zero based record number that should be obtained.
$lpItem$ pointer to the item.

@Returns:
STATUSEXCLUDE_OK    if the item was successfully obtained.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

STATUSEXCLUDE WINAPI ExcludeLGet (      // Obtain an item from the file
    HLEXCLUDE   hLExclude,              // [in] handle to file
    WORD        wIndex,                 // [in] record number to get
    LPLEXCLUDE  lpItem                  // [out] info placed here
    )
{
    SYM_ASSERT ( hLExclude );
    SYM_ASSERT ( lpItem );
    SYM_ASSERT ( wIndex < hLExclude->wNumEntries );

    auto    STATUSEXCLUDE   Status = STATUSEXCLUDE_ERR;

    if ( hLExclude )
        {
        LPEXCLUDELSTRUCT lpLExclude = hLExclude;

        if ( wIndex < lpLExclude->wNumEntries )
            {
            MEMCPY ( lpItem, &lpLExclude->lpEntries[wIndex], sizeof(LEXCLUDE) );
            Status = STATUSEXCLUDE_OK;
            }
        }

    return ( Status );
}

/*@API:*************************************************************************
@Declaration:

STATUSEXCLUDE WINAPI ExcludeLSet (      // Set an item in the file
    HLEXCLUDE   hLExclude,              // [in] handle to file
    DWORD       dwIndex,                // [in] record number to get
    LPLEXCLUDE  lpItem                  // [in] info to place
    );

@Description:
This function sets an entry in the file that was opened with the
ExcludeLOpen() function.

@Parameters:
$hLExclude$ handle that was returned by the ExcludeLOpen() function
$dwIndex$ zero based record number that should be obtained.
$lpItem$ pointer to the item.

@Returns:
STATUSEXCLUDE_OK    if the item was successfully obtained.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

STATUSEXCLUDE WINAPI ExcludeLSet (      // Set an item in the file
    HLEXCLUDE   hLExclude,              // [in] handle to file
    WORD        wIndex,                 // [in] record number to get
    LPLEXCLUDE  lpItem                  // [in] info to place
    )
{
    SYM_ASSERT ( hLExclude );
    SYM_ASSERT ( lpItem );
    SYM_ASSERT ( wIndex < hLExclude->wNumEntries );

    auto    STATUSEXCLUDE   Status = STATUSEXCLUDE_ERR;

    if ( hLExclude )
        {
        LPEXCLUDELSTRUCT lpLExclude = hLExclude;

        if ( wIndex < lpLExclude->wNumEntries )
            {
            MEMCPY ( &lpLExclude->lpEntries[wIndex], lpItem, sizeof(LEXCLUDE) );
            Status = STATUSEXCLUDE_OK;
            }
        }

    return ( Status );
}


/*@API:*************************************************************************
@Declaration:

STATUSEXCLUDE WINAPI ExcludeLDelete (   // Deletes an item from the file
    HLEXCLUDE   hLExclude,              // [in] handle to the file
    DWORD       dwIndex                 // [in] record number to delete
    );

@Description:
This function deletes an entry from the file that was opened with the
ExcludeLOpen() function.

@Parameters:
$hLExclude$ handle that was returned by the ExcludeLOpen() function
$dwIndex$ zero based record number that should be deleted.

@Returns:
STATUSEXCLUDE_OK    if the item was successfully deleted.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

STATUSEXCLUDE WINAPI ExcludeLDelete (   // Deletes an item from the file
    HLEXCLUDE   hLExclude,              // [in] handle to the file
    WORD        wIndex                  // [in] record number to delete
    )
{
    SYM_ASSERT ( hLExclude );
    SYM_ASSERT ( wIndex < hLExclude->wNumEntries );

    auto    STATUSEXCLUDE   Status = STATUSEXCLUDE_ERR;

    if ( hLExclude )
        {
        LPEXCLUDELSTRUCT lpLExclude = hLExclude;

                                        // ----------------------------------
                                        // Make sure index to delete is valid
                                        // ----------------------------------

        if ( wIndex < lpLExclude->wNumEntries )
            {
                                        // ----------------------------------
                                        // Compute bytes to move
                                        // ----------------------------------

            DWORD dwBytes = lpLExclude->wNumEntries - wIndex + 1;
            dwBytes *= sizeof(LEXCLUDE);

                                        // ----------------------------------
                                        // Move info
                                        // ----------------------------------
            MEMMOVE (
                    &lpLExclude->lpEntries[wIndex],     // To
                    &lpLExclude->lpEntries[wIndex+1],   // From
                    (UINT)dwBytes                             // Bytes
                    );

            lpLExclude->wNumEntries--;
            lpLExclude->bIsDirty = TRUE;

                                        // ----------------------------------
                                        // Zap last entry just to make sure.
                                        // ----------------------------------

            MEMSET ( &lpLExclude->lpEntries[lpLExclude->wNumEntries], EOS,
                     sizeof(LEXCLUDE) );

            Status = STATUSEXCLUDE_OK;
            }
        }

    return ( Status );
}

// ---------------------------------------------------------------------------
//                  LOCAL FUNCTION TO THIS MODULE
// ---------------------------------------------------------------------------


/*@API:*************************************************************************
@Declaration:

BOOL LOCAL ExcludeLValidHeader (        // Validates file header.
    HFILE   hFile,                      // [in]  File handle
    LPWORD  lpwNumEntries )             // [out] Number of records in file.

@Description:
This function validates the header of the EXCLUDEL.DAT file.

@Parameters:
$hFile$ handle to the file to validate.

@Returns:
    EXCLOPEN_HEADER_BAD - header exists but the actual number of records
                          is different than that stored in the header.
    EXCLOPEN_HAS_HEADER - everything tested appears fine.
    EXCLOPEN_NO_HEADER  - there was not even enough data read from the file
                          for a header.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

UINT LOCAL ExcludeLValidHeader (        // Validates file header.
    HFILE   hFile,                      // [in]  File handle
    LPWORD  lpwNumEntries               // [out] number of records in file.
    )
{
    auto    UINT            uiHeaderState = EXCLOPEN_HEADER_BAD;
    auto    EXCLUDELHEADER  Header = { 0 };
    auto    DWORD           dwActualFileSize;
    auto    DWORD           dwCalculatedFileSize;

    SYM_ASSERT ( hFile && hFile != HFILE_ERROR );
    SYM_ASSERT ( lpwNumEntries );

    *lpwNumEntries = 0;

    FileSeek ( hFile, 0L, SEEK_SET );
    UINT uRead = FileRead ( hFile, &Header, sizeof(Header) );

    if ( uRead == sizeof(Header) )
        {
        if ( Header.wSize == sizeof(Header) &&
            !strcmp ( Header.szSignature, NAV_W95_SIGNATURE ) )
            {
            *lpwNumEntries = Header.wNumRecords;

            dwActualFileSize = FileLength ( hFile );

                                    // --------------------------------
                                    // Confirm the information from the
                                    // file header with the information
                                    // that we compute.
                                    // --------------------------------

            dwCalculatedFileSize = sizeof(EXCLUDELHEADER) +
                         ( sizeof(LEXCLUDE_DAT) * Header.wNumRecords );

                                    // Final test before reporting success.
            if ( dwActualFileSize == dwCalculatedFileSize )
                uiHeaderState = EXCLOPEN_HAS_HEADER;
            }
        }
    else
        {
        uiHeaderState = EXCLOPEN_NO_HEADER;
        }

    return ( uiHeaderState );
}

/*@API:*************************************************************************
@Declaration:

HLEXCLUDE WINAPI ExcludeLCreateCopy (   // Create a copy of
    HLEXCLUDE   hLExclude               // [in] this handle
    )

@Description:
This function creates a copy of the provided hLExclude handle.

@Parameters:
$hLExclude$ handle to make a copy of.

@Returns:
If succeeded a HLEXCLUDE handle that is a copy of the provided hLExclude
handle. On error NULL is returned.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

HLEXCLUDE WINAPI ExcludeLCreateCopy (   // Create a copy of
    HLEXCLUDE   hLExclude               // [in] this handle
    )
{
    SYM_ASSERT ( hLExclude );

    auto    LPEXCLUDELSTRUCT    lpLExclude;
    const   LPEXCLUDELSTRUCT    lpLOriginal = hLExclude;

    lpLExclude = (LPEXCLUDELSTRUCT)MemAllocPtr ( GHND, sizeof(EXCLUDELSTRUCT) );

    if ( lpLExclude )
        {
        lpLExclude->lpEntries = (LPLEXCLUDE)MemAllocPtr (
                                        GHND,
                                        lpLOriginal->dwEntriesSize
                                        );

        if ( lpLExclude->lpEntries )
            {
            MEMCPY ( lpLExclude->lpEntries,
                     lpLOriginal->lpEntries,
                     (UINT)lpLOriginal->dwEntriesSize );

            lpLExclude->dwEntriesSize = lpLOriginal->dwEntriesSize;
            lpLExclude->wNumEntries   = lpLOriginal->wNumEntries;
            lpLExclude->bIsDirty      = lpLOriginal->bIsDirty;

            STRCPY ( lpLExclude->szFilePath, lpLOriginal->szFilePath );
            }
        else
            {
            MemFreePtr ( lpLExclude );
            lpLExclude = NULL;
            }
        }

    return ( lpLExclude );
}


/*@API:*************************************************************************
@Declaration:

STATUSEXCLUDE WINAPI ExcludeLKillCopy ( // Kill a previously created copy
    HLEXCLUDE   hLExclude               // [in] this handle
    );

@Description:
This function deletes a handle that was created by the ExcludeLCreateCopy()
function.

@Parameters:
$hLExclude$ handle to delete

@Returns:
If succeeded STATUSEXCLUDE_OK is returned otherwise STATUSEXCLUDE_ERR

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

STATUSEXCLUDE WINAPI ExcludeLKillCopy ( // Kill a previously created copy
    HLEXCLUDE   hLExclude               // [in] this handle
    )
{
    SYM_ASSERT ( hLExclude );

    auto    STATUSEXCLUDE       Status = STATUSEXCLUDE_ERR;
    auto    LPEXCLUDELSTRUCT    lpLExclude = hLExclude;

    if ( lpLExclude )
        {
        if ( lpLExclude->lpEntries )
            MemFreePtr ( lpLExclude->lpEntries );

        MemFreePtr ( lpLExclude );

        Status = STATUSEXCLUDE_OK;
        }

    return ( Status );
}

/*@API:*************************************************************************
@Declaration:

BOOL LOCAL ExcludeLWriteHeader (        // Write header to file.
    HFILE               hFile,          // [in] file handle to write to
    LPEXCLUDELSTRUCT    lpLExclude      // [in] info about the file
    );

@Description:
This function writes the EXCLUDEL.DAT file header.

@Parameters:
$hFile$ handle to file to write to
$lpLExclude$ pointer to file info that will be used to write header.

@Returns:
If succeeded TRUE is returned otherwise FALSE.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL LOCAL ExcludeLWriteHeader (        // Write header to file.
    HFILE               hFile,          // [in] file handle to write to
    LPEXCLUDELSTRUCT    lpLExclude      // [in] info about the file
    )
{
    SYM_ASSERT ( hFile );
    SYM_ASSERT ( lpLExclude );
                                        // -----------------------------------
                                        // Initialize file header contents.
                                        // -----------------------------------

    auto    EXCLUDELHEADER  Header = { 0 };

    Header.wSize        = sizeof(Header);
    Header.dwVersion    = NAV_W95_VERSION;
    Header.wNumRecords  = lpLExclude->wNumEntries;
    strcpy( Header.szSignature, NAV_W95_SIGNATURE );

                                        // -----------------------------------
                                        // and write it to file.
                                        // -----------------------------------

    FileSeek ( hFile, 0L, SEEK_SET );
    DWORD dwSize = FileWrite ( hFile, &Header, sizeof(Header) );

    return ( dwSize == sizeof(Header) ? TRUE : FALSE );
}


STATUSEXCLUDE WINAPI ExcludeLObjectsIdentical (
    HLEXCLUDE   hThis,                  // [in] this handle
    HLEXCLUDE   hThat                   // [in] this handle
    )
{
    SYM_ASSERT ( hThis );
    SYM_ASSERT ( hThat );

    auto    STATUSEXCLUDE       Status = STATUSEXCLUDE_ERR;
    const   LPEXCLUDELSTRUCT    lpThis = hThis;
    const   LPEXCLUDELSTRUCT    lpThat = hThat;

    if ( lpThis->wNumEntries    ==  lpThat->wNumEntries     &&
         lpThis->bIsDirty       ==  lpThat->bIsDirty        &&
         lpThis->dwEntriesSize  ==  lpThat->dwEntriesSize   &&
         !MEMCMP ( lpThis->lpEntries, lpThat->lpEntries,
                   (UINT)lpThis->dwEntriesSize )            &&
         !STRCMP ( lpThis->szFilePath, lpThat->szFilePath ) )
        {
        Status = STATUSEXCLUDE_OK;
        }

    return ( Status );
}


