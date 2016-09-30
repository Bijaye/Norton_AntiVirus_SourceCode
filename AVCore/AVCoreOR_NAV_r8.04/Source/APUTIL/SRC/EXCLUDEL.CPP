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

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "avexcll.h"

// ---------------------------------------------------------------------------
//                  LOCAL FUNCTION TO THIS MODULE
// ---------------------------------------------------------------------------


#define EXCLOPEN_HAS_HEADER     0
#define EXCLOPEN_NO_HEADER      1
#define EXCLOPEN_HEADER_BAD     2

HLEXCLUDE WINAPI ExcludeLSpecifyFileContents (
    DWORD dwExcludeLItems,
    PVOID pvExcludeLItems
    )
{
    SYM_ASSERT ( pvExcludeLItems );

    auto LEXCLUDE_DAT gLExcludeDat;      // Single record from ExcludeL.Dat

    auto    LPEXCLUDELSTRUCT    lpLExclude;
    auto    BOOL                bErrorOccured = FALSE;
    auto    UINT                uiHeaderState = EXCLOPEN_NO_HEADER;
    auto    WORD                wIndex;
    auto    LPLEXCLUDE          lpEntry;

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

                                        // Initialize the number of exclude
                                        // entries (LEXCLUDE_DAT)

        lpLExclude->wNumEntries = (WORD)dwExcludeLItems;

                                        // --------------------------------
                                        // Compute bytes to allocate.
                                        // --------------------------------

        lpLExclude->dwEntriesSize  = lpLExclude->wNumEntries / ENTRIES_PER_BLOCK;
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

            for ( wIndex = 0, bErrorOccured = FALSE,
                    lpEntry = lpLExclude->lpEntries;
                    ( !bErrorOccured ) &&
                    ( wIndex < lpLExclude->wNumEntries );
                    ++wIndex, ++lpEntry )
                {
                MEMCPY ( &gLExcludeDat, pvExcludeLItems, sizeof(gLExcludeDat) );
                pvExcludeLItems = &((PBYTE)pvExcludeLItems)[sizeof(gLExcludeDat)];
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

    return ( lpLExclude );
}

STATUSEXCLUDE WINAPI ExcludeLClose (
    HLEXCLUDE hLExclude
    )
{
    if ( hLExclude )
        {
        if ( hLExclude->lpEntries )
            MemFreePtr ( hLExclude->lpEntries );

        MemFreePtr ( hLExclude );
        }

    return ( STATUSEXCLUDE_OK );
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
                   (UINT)lpThis->dwEntriesSize ) )
        {
        Status = STATUSEXCLUDE_OK;
        }

    return ( Status );
}


