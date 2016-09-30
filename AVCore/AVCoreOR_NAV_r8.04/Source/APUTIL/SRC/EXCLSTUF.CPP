// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEXCLU/VCS/exclstuf.cpv   1.3   29 Aug 1997 21:12:50   JBRENNA  $
//
// Description:
//      This file contains the routines necessary for reading, modifying,
//      and checking the exclusions list.
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "avexcl.h"
#include "file.h"


// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

UINT LOCAL ExcludeAddShort (
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPTSTR          lpItemName,         // [in] name of item
    WORD            wFlags,             // [in] exclusions flags for this item
    BOOL            bSubDirs            // [in] flag to exclude sub dirs.
    );

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyFileContents (
    LPEXCLUDE   lpExclude,
    DWORD       dwExcludeItems,
    PVOID       pvExcludeItems );

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyContents (
    LPEXCLUDE       lpExclude,
    DWORD           dwExcludeItems,
    PVOID           pvExcludeItems, // OLDEXCLUDEITEM array
    DWORD           dwExcludeLItems,
    PVOID           pvExcludeLItems ) // LEXCLUDE_DAT array
{
    if ( ExcludeInitSpecifyFileContents(lpExclude, dwExcludeItems, pvExcludeItems) == NOERR )
        {
        if ( lpExclude->lpPrivate->hLExclude =
             ExcludeLSpecifyFileContents ( dwExcludeLItems, pvExcludeLItems )
           )
            return NOERR;

        ExcludeTerminate ( lpExclude );
        }

    return ( ERR );
}

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyFileContents (
    LPEXCLUDE   lpExclude,
    DWORD       dwExcludeItems,
    PVOID       pvExcludeItems )
{
    auto        HGLOBAL         hExcludeItem = NULL;
    auto        LPEXCLUDEITEM   lpExcludeItem = NULL;
    auto        UINT            uEntries = dwExcludeItems / sizeof(OLDEXCLUDEITEM);
    auto        STATUS          uStatus = ERR;

                                        // Clear the structure

    MEMSET(lpExclude, 0, sizeof(EXCLUDE));

                                        // -----------------------------------
                                        // Allocate memory for private data.
                                        // -----------------------------------

    lpExclude->lpPrivate = (LPEXCLUDEPRIVATE)MemAllocPtr (
                                                    GHND,
                                                    sizeof(EXCLUDEPRIVATE)
                                                    );
    if ( lpExclude->lpPrivate )
        {
        UINT uFileReadSize = uEntries * sizeof(OLDEXCLUDEITEM);

                                        // -----------------------------------
                                        // NOTE: We allocate 1 extra entry.
                                        // This insures we work correctly
                                        // when there are no exclude items.
                                        // -----------------------------------

        hExcludeItem = MemAlloc ( GHND, ( uEntries + 1 ) * sizeof(EXCLUDEITEM) );

        LPOLDEXCLUDEITEM lpOldItem = (LPOLDEXCLUDEITEM) MemAllocPtr (
                                        GHND,
                                        uFileReadSize + sizeof(OLDEXCLUDEITEM)
                                        );

        if ( hExcludeItem && lpOldItem )
            {
            if ( lpExcludeItem = (LPEXCLUDEITEM) MemLock ( hExcludeItem ) )
                {
                                        // -----------------------------------
                                        // Read in the exclusions in a block
                                        // -----------------------------------
                if ( (uEntries == 0) ||
                     MEMCPY ( lpOldItem, pvExcludeItems, uFileReadSize ) )
                    {
                    for ( UINT i = 0; i < uEntries; i++ )
                        {
#ifdef _UNICODE
                                       // Convert from OEM to Unicode
                        MultiByteToWideChar( CP_OEMCP, 0, lpOldItem[i].szText,
                                             -1, lpExcludeItem[i].szText,
                                             sizeof(lpExcludeItem->szText) /
                                             sizeof(TCHAR) );
#else
                        STRCPY ( lpExcludeItem[i].szText,
                                 lpOldItem[i].szText );
#endif
                        lpExcludeItem[i].wBits         = lpOldItem[i].wBits;
                        lpExcludeItem[i].bSubDirectory = lpOldItem[i].bSubDirectory;
                        }

                    lpExclude->hExcludeMem             = hExcludeItem;
                    lpExclude->lpPrivate->lpExcludeMem = lpExcludeItem;
                    lpExclude->wEntries                = uEntries;
                    uStatus = NOERR;
                    }
                }
            }

        if ( lpOldItem )
            MemFreePtr ( lpOldItem );
        }

    if ( uStatus == ERR )
        {
        if ( lpExclude->lpPrivate )
            MemFreePtr ( lpExclude->lpPrivate );

        if ( lpExcludeItem )
            MemRelease(hExcludeItem);

        if ( hExcludeItem )
            MemFree(hExcludeItem);
        }

    return( uStatus );
}

/*@API:*************************************************************************
@Declaration:

WORD SYM_EXPORT WINAPI ExcludeGetCount (// Get number of excluded items.
    LPEXCLUDE       lpExclude )         // [in] returned from init.

@Description:
 This function returns number of excluded items. This includes both SFN and LFN

@Parameters:
 $lpExclude$ structure that was used during init.

@Returns:
Number of excluded items.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

WORD SYM_EXPORT WINAPI ExcludeGetCount (// Get number of excluded items.
    LPEXCLUDE       lpExclude )         // [in] returned from init.
{
    SYM_ASSERT ( lpExclude );
                                        // -----------------------------------
                                        // Return the number of items in
                                        // SFN area and LFN area.
                                        // -----------------------------------

    return ( lpExclude->wEntries +
             ExcludeLGetCount ( lpExclude->lpPrivate->hLExclude ) );
}


/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeGet (     // Obtain an item by index number.
    LPEXCLUDE       lpExclude,          // [in] returned from init.
    WORD            wIndex,             // [in] item index number to get.
    LPTSTR           lpItemName,         // [out] name of item placed here.
    LPWORD          lpwFlags,           // [out] exclusions flags for this item
    LPBOOL          lpbSubDirs )        // [out] flag to exclude sub dirs.

@Description:
This function obtains an item by it's index number. The number of excluded
items must have been obtained using the ExcludeGetCount() function. Note that
the excluded item can not be returned in EXCLUDEITEM structure since this
structure's szText field can not exceed 144 characters due to backward comp.

@Parameters:
 $lpExclude$ structure that was used during init.
 $wIndex$    index of item to obtain.
 $lpItemName$ the excluded item's name is placed here.
 $lpwFlags$  pointer to a word that is filled with exclude flags.
 $lpbSubDirs$ filled with TRUE if subdirectories should be excluded.

@Returns:
TRUE if the item was obtained successfully.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeGet (     // Obtain an item by index number.
    LPEXCLUDE       lpExclude,          // [in] returned from init.
    WORD            wIndex,             // [in] item index number to get.
    LPTSTR           lpItemName,         // [out] name of item placed here.
    LPWORD          lpwFlags,           // [out] exclusions flags for this item
    LPBOOL          lpbSubDirs )        // [out] flag to exclude sub dirs.
{
    SYM_ASSERT ( lpExclude  );
    SYM_ASSERT ( lpItemName );
    SYM_ASSERT ( lpwFlags   );
    SYM_ASSERT ( lpbSubDirs );

    auto    BOOL    bReturn = FALSE;

    if ( wIndex < lpExclude->wEntries )
        {
        STRCPY ( lpItemName, lpExclude->lpPrivate->lpExcludeMem[wIndex].szText );
        *lpwFlags   = lpExclude->lpPrivate->lpExcludeMem[wIndex].wBits;
        *lpbSubDirs = lpExclude->lpPrivate->lpExcludeMem[wIndex].bSubDirectory;

        bReturn = TRUE;
        }
    else
        {
        wIndex -= lpExclude->wEntries;

        LEXCLUDE Item = { 0 };

        if ( ExcludeLGet ( lpExclude->lpPrivate->hLExclude, wIndex, &Item ) ==
                                                            STATUSEXCLUDE_OK )
            {
            STRCPY ( lpItemName, Item.szItem );
            *lpwFlags   = Item.wFlags;
            *lpbSubDirs = Item.bSubDirectory;

            bReturn = TRUE;
            }
        }

    return ( bReturn );
}

/*@API:*************************************************************************
@Declaration:

UINT SYM_EXPORT WINAPI ExcludeAddDirect (
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPEXCLUDEITEM   lpExcludeItem,      // [in] item to add
    EXCLUDEACTION   ExcludeAddFlag )    // [in] flags

@Description:
This function adds an entry to the list of excluded items.

@Parameters:
 $lpExclude$ structure that was used during init.
 $lpExcludeItem$    item to add to exclusions

@Returns:
 TRUE               if the item was added
 FALSE              if the item could not be added
 EXCLUDE_DUPLICATE  if there's a duplicate.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

UINT SYM_EXPORT WINAPI ExcludeAddDirect ( // Add an entry to the exclusions
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPEXCLUDEITEM   lpExcludeItem,      // [in] item to add
    EXCLUDEACTION   ExcludeAddFlag )    // [in] flags
{
    SYM_ASSERT ( lpExclude );
    SYM_ASSERT ( lpExcludeItem );
    SYM_ASSERT ( lpExclude->lpPrivate );

    auto UINT      uRetValue;

    if ( lpExclude->lpPrivate->lpExcludeMem )
        {
                                        // Note: the next if() ... else ...
                                        // if() ... else ... returns result
                                        // in uRetValue.

        if ( ExcludeIsValidSFN ( lpExcludeItem->szText ) == STATUSEXCLUDE_OK )
            {
            uRetValue = ExcludeAddShort (
                                        lpExclude,
                                        lpExcludeItem->szText,
                                        lpExcludeItem->wBits,
                                        lpExcludeItem->bSubDirectory
                                        );
            }
        else if ( ExcludeIsValidLFN ( lpExcludeItem->szText ) == STATUSEXCLUDE_OK )
            {
                                        // -----------------------------------
                                        // Try to find its short filename.
                                        // -----------------------------------

            TCHAR szShortName [ SYM_MAX_PATH + 1 ] ;
            szShortName[0]=EOS;

            BOOL bStatus = ( ExcludeAddFlag & EXCLADD_EXACTNAME ) ?
                           FALSE :
                           ExcludeLFN2SFN (
                                          lpExcludeItem->szText,
                                          szShortName,
                                          (sizeof(szShortName)-1) / sizeof(TCHAR)
                                          );

            if ( bStatus )
                {
                SYM_ASSERT ( *szShortName );

                uRetValue = ExcludeAddShort (
                                            lpExclude,
                                            szShortName,
                                            lpExcludeItem->wBits,
                                            lpExcludeItem->bSubDirectory
                                            );
                }
            else
                {

                                        // -----------------------------------
                                        // Long filename entry.
                                        // -----------------------------------

                LEXCLUDE    Exclude = { 0 };

                STRCPY ( Exclude.szItem, lpExcludeItem->szText );
                Exclude.wFlags          =   lpExcludeItem->wBits;
                Exclude.bSubDirectory   =   lpExcludeItem->bSubDirectory;

                STATUSEXCLUDE Status = ExcludeLAdd (
                                                   lpExclude->lpPrivate->hLExclude,
                                                   &Exclude
                                                   );

                if ( Status == STATUSEXCLUDE_OK )
                    {
                    uRetValue = TRUE;
                    }
                else if ( Status == STATUSEXCLUDE_DUPLICATE )
                    {
                    uRetValue = EXCLUDE_DUPLICATE;
                    }
                else
                    uRetValue = FALSE;
                }
            }
        else
            {
            SYM_ASSERT ( FALSE );

            uRetValue = FALSE;
            }
        }
    else
        uRetValue = FALSE;

    return ( uRetValue );
}

/*@API:*************************************************************************
@Declaration:

UINT LOCAL ExcludeAddShort (
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPTSTR          lpItemName,         // [in] name of item
    WORD            wFlags,             // [in] exclusions flags for this item
    BOOL            bSubDirs            // [in] flag to exclude sub dirs.
    );

@Description:
This function adds an entry to the list of excluded SFN items.

@Parameters:
 $lpExclude$ structure that was used during init.
 $lpItemName$   pointer to name of item to add
 $wFlags$    exclusion flags
 $bSubDirs$  TRUE to include subdirectories.

@Returns:
 TRUE               if the item was added
 FALSE              if the item could not be added
 EXCLUDE_DUPLICATE  if there's a duplicate.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

UINT LOCAL ExcludeAddShort (
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPTSTR          lpItemName,         // [in] name of item
    WORD            wFlags,             // [in] exclusions flags for this item
    BOOL            bSubDirs            // [in] flag to exclude sub dirs.
    )
{
    auto    UINT        uRetValue = FALSE;
    auto    HGLOBAL     hNewExcludeItem;

    SYM_ASSERT (lpExclude);
    SYM_ASSERT (lpItemName);

                                        // -----------------------------------
                                        // See if this entry already is
                                        // excluded.  If it is, then OR in
                                        // new settings, if any.
                                        // -----------------------------------

    for ( UINT i = 0; i < lpExclude->wEntries; i++ )
        {
        if ( !STRICMP(lpExclude->lpPrivate->lpExcludeMem[i].szText, lpItemName ) )
            {
            lpExclude->lpPrivate->lpExcludeMem[i].bSubDirectory |= bSubDirs;
            lpExclude->lpPrivate->lpExcludeMem[i].wBits         |= wFlags;
            return( EXCLUDE_DUPLICATE );
            }
        }
                                        // -----------------------------------
                                        // Is this a new entry?
                                        // -----------------------------------

    if ( i >= lpExclude->wEntries )
        {
        if (lpExclude->wEntries < MAX_EXCLUDES)
            {
                                        // -----------------------------------
                                        // Allocate space for a new entry
                                        // -----------------------------------
            MemRelease ( lpExclude->hExcludeMem );

                                        // -----------------------------------
                                        // +2 is because we are adding one
                                        // new entry and we always have 1
                                        // extra entry on the end.
                                        // -----------------------------------

            hNewExcludeItem = MemReAlloc(
                                    lpExclude->hExcludeMem,
                                    (lpExclude->wEntries + 2) * sizeof(EXCLUDEITEM),
                                    GHND
                                    );

            if ( hNewExcludeItem )
                {
                lpExclude->hExcludeMem = hNewExcludeItem;

                lpExclude->lpPrivate->lpExcludeMem = (LPEXCLUDEITEM)MemLock(
                                                    hNewExcludeItem
                                                    );

                                        // -----------------------------------
                                        // Add entry in upper case
                                        // -----------------------------------
                STRCPY (
                    lpExclude->lpPrivate->lpExcludeMem[lpExclude->wEntries].szText,
                    lpItemName );

                lpExclude->lpPrivate->lpExcludeMem[lpExclude->wEntries].wBits = wFlags;
                lpExclude->lpPrivate->lpExcludeMem[lpExclude->wEntries].bSubDirectory = bSubDirs;

                lpExclude->wEntries++;

                uRetValue = TRUE;
                }
            }
        }

    return ( uRetValue );
}

/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeIs (      // old exclude check function.
    LPEXCLUDE   lpExclude,              // [in] structure used during init.
    LPTSTR      lpFile,                 // [in] item to check
    WORD        wFlags )                // [in] exclude flags to check towards.

@Description:
This function queries if the specified item is excluded from the actions
specified in the wFlags field.

@Parameters:
 $lpExclude$ structure that was used during init.
 $lpFile$    filename of the item to check. Assumes LFN. SFN's are OK but
             don't pass mixed (e.g., "C:\Progra~1\Norton AntiVirus")
 $wFlags$    exclusion flags to check towards.

@Returns:
TRUE if this file is excluded for one or more actions described in wFlags

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeIs (      // old exclude check function.
    LPEXCLUDE   lpExclude,              // [in] structure used during init.
    LPTSTR      lpFile,                 // [in] item to check
    WORD        wFlags )                // [in] exclude flags to check towards.
{
    SYM_ASSERT ( lpExclude );
    SYM_ASSERT ( lpFile );
    SYM_ASSERT ( wFlags );

    auto    BOOL            bExcluded = FALSE;

    auto    BOOL    bCanExclude = FALSE;
    auto    TCHAR   szShortPath         [ SYM_MAX_PATH + 1 ] ;
    auto    TCHAR   szLongPath          [ SYM_MAX_PATH + 1 ] ;

    szShortPath[0] = szLongPath[0] = EOS;

                                        // ----------------------------------
                                        // Find both the short and the long
                                        // filename equivalent.
                                        // ----------------------------------

    if ( ExcludeIsValidSFN (lpFile) == STATUSEXCLUDE_OK )
        {
        STRCPY ( szShortPath, lpFile);

        if ( ExcludeSFN2LFN (lpFile, szLongPath, (sizeof(szLongPath)-1) / sizeof(TCHAR) )
                            == FALSE )
            {
            szLongPath[0] = EOS;
            }
        bCanExclude = TRUE;
        }
    else if ( ExcludeIsValidLFN (lpFile) == STATUSEXCLUDE_OK )
        {
        STRCPY ( szLongPath, lpFile);

        if ( ExcludeLFN2SFN (lpFile, szShortPath, (sizeof(szShortPath)-1) / sizeof(TCHAR) )
                            == FALSE )
            {
            szShortPath[0] = EOS;
            }
        bCanExclude = TRUE;
        }

    if (bCanExclude)
        {
        bExcluded = ExcludeIsEx( lpExclude, szLongPath, szShortPath, wFlags );
        }
    return ( bExcluded );
}


BOOL SYM_EXPORT WINAPI ExcludeIsEx (    // old exclude check function.
    LPEXCLUDE   lpExclude,              // [in] structure used during init.
    LPTSTR      lpszLFN,                // [in] LFN item to check
    LPTSTR      lpszSFN,                // [in] SFN of item to check
    WORD        wFlags )                // [in] exclude flags to check towards.
{
    auto    EXCLUDEITEM     Item;
    auto    int             nCount;
    auto    int             nIndex;
    auto    BOOL            bSubDirectory;

    auto    BOOL            bExcluded = FALSE;

    nCount = ExcludeGetCount ( lpExclude );

    for ( nIndex = 0; nIndex < nCount && bExcluded == FALSE; nIndex++ )
        {
        if ( ExcludeGet ( lpExclude, nIndex, Item.szText,
                          &Item.wBits, &bSubDirectory ) )
            {
            Item.bSubDirectory = bSubDirectory;

            bExcluded = NavExcludeCheck ( &Item, lpszSFN, lpszLFN, wFlags );
            }
        else
            break;
        }

    return ( bExcluded );
} // ExcludeIsEx()
