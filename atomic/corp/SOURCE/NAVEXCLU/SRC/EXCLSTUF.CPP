// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEXCLU/VCS/exclstuf.cpv   1.3   29 Aug 1997 21:12:50   JBRENNA  $
//
// Description:
//      This file contains the routines necessary for reading, modifying,
//      and checking the exclusions list.
//
// Contains:
//      GetExcludeBin()
//      ExcludeInit()
//	ExcludeInitSpecifyDir()
//      ExcludeAdd()
//      ExcludeAddSpecifyDir()
//      ExcludeDeleteIndex()
//      ExcludeIs()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVEXCLU/VCS/exclstuf.cpv  $
// 
//    Rev 1.3   29 Aug 1997 21:12:50   JBRENNA
// AP is adding an exclusion without the SFN alias (non-translating ANSI/OEM
// char ... Alt-163, 176, 178, 181, etc.). This hack is to all NAVW to delete
// the true name and the SFN alias.
// 
//    Rev 1.2   16 May 1997 10:35:18   RStanev
// Fixed major problems with ExcludeAddSpecifyDir().
//
//    Rev 1.1   13 May 1997 16:40:02   DALLEE
// Added ExcludeIsEx() which takes both LFN and SFN of file.
//
//    Rev 1.0   06 Feb 1997 20:57:48   RFULLER
// Initial revision
//
//    Rev 1.5   08 Sep 1996 16:17:58   DALLEE
// Put back #ifndef SYM_VXD around ExcludeAdd/Ex().
//
//    Rev 1.4   27 Jun 1996 16:47:58   RHAYES
// Luigi merge - Remove OEM<->Ansi conversion since all
// scanning and comparisons are now exclusively in OEM.
//
//    Rev 1.3   10 May 1996 10:58:36   RHAYES
// Unicode conversion update.
//
//    Rev 1.2   06 May 1996 16:13:16   RSTANEV
// Merged Ron's Unicode changes.
//
//    Rev 1.1   19 Mar 1996 13:55:14   jworden
// Double byte enable
//
//    Rev 1.0   30 Jan 1996 15:53:02   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 13:22:06   BARRY
// Initial revision.
//
//    Rev 1.37   08 Jul 1995 21:16:10   RHAYES
// Several changes to ExcludeAddShort() including forcing name to upper case,
// performing case-insensitve comparisons for finding a duplicate, and some
// other minor cleanup.
//
//    Rev 1.36   08 Jul 1995 11:32:26   RSTANEV
// Forgot one place where we need to look at EXCLADD_EXACTNAME.
//
//    Rev 1.35   08 Jul 1995 10:36:54   RSTANEV
// Added support for EXCLADD_EXACTNAME.
//
//    Rev 1.34   05 Jul 1995 10:29:46   BARRY
// Reworked parsing and validation logic. Should fix STS#39317, 38851, 39316
//
//    Rev 1.33   26 Jun 1995 22:49:06   RSTANEV
// Now properly converting the exclude DB location to current character set
// when initializing a new exclusion structure.
//
//    Rev 1.32   24 May 1995 10:10:18   DALLEE
// VxD reentency problem fix.  Added temp buffer to retain static
// lpExclude memory area for multi-threads.  Was flushing buffer
// causing invalid data to be present in exclude area if a thread
// needed to access this portion of static mem. -CGRANGE
//
//
//    Rev 1.30   28 Apr 1995 19:09:28   MFALLEN
// Fixed problem with deleting LFN exclusions.
//
//    Rev 1.29   17 Apr 1995 15:37:26   MFALLEN
// SYM_MAX_PATH+1
//
//
//    Rev 1.28   04 Apr 1995 16:46:24   MFALLEN
// Strict validation.
//
//    Rev 1.27   30 Jan 1995 20:36:32   DALLEE
// Commented out assertion on HINSTANCE for SYM_DOS.
// For this platform we're usually passing bogus values off the stack.
//
//    Rev 1.26   19 Jan 1995 14:17:54   mfallen
// Correctly checks return code from ExcludeAddShort().
//
//    Rev 1.25   19 Jan 1995 10:31:24   MARKL
// Added more SYM_ASSERTs.  Also set free'd memory handles and pointers
// to NULL.
//
//    Rev 1.24   17 Jan 1995 15:58:52   MFALLEN
// Added fixes.
//
//    Rev 1.23   16 Jan 1995 17:26:08   MFALLEN
// Moved index var where it belongs.
//
//    Rev 1.22   16 Jan 1995 17:09:22   MFALLEN
// Oem ifdef fix.
//
//    Rev 1.21   16 Jan 1995 15:42:06   MARKL
// Fixed link errors in VxD.
//
//    Rev 1.20   16 Jan 1995 15:20:20   MARKL
// Added ExcludeAddSpecifyDir()
//
//    Rev 1.19   13 Jan 1995 18:45:28   MFALLEN
// Spiffed up for VXD, still testing.
//
//    Rev 1.18   13 Jan 1995 17:38:06   MFALLEN
// Removed ..Ex() functions and added new exclude match checking routine.
//
//    Rev 1.17   11 Jan 1995 18:39:26   MFALLEN
// Small typo.
//
//    Rev 1.16   11 Jan 1995 16:14:26   MFALLEN
// Made sure the exclude structure is unchanged.
//
//    Rev 1.15   11 Jan 1995 15:19:32   MFALLEN
// Added EXCLUDE_DUPLICATE
//
//    Rev 1.14   10 Jan 1995 18:43:44   MARKL
// When building for VxD platform, doesn't include routines that are not
// needed.
//
//    Rev 1.13   10 Jan 1995 17:49:26   MFALLEN
// ExcludeInitSpecifyDir() function fixed.
//
//    Rev 1.12   09 Jan 1995 13:21:06   DALLEE
// Changed to use STRICMP instead of EXCLUDECHECK() in ExcludeDeleteEx() when
// determining match.  We want the exact item, not just the first exclusion
// that matches the filespec passed.
//
//
//    Rev 1.11   06 Jan 1995 10:45:28   MFALLEN
// Comment in header fixed.
//
//    Rev 1.10   06 Jan 1995 09:50:54   MFALLEN
// More fixes.
//
//    Rev 1.9   05 Jan 1995 17:24:28   MFALLEN
// Polished.
//
//    Rev 1.8   05 Jan 1995 13:47:10   MFALLEN
// Still debugging but somebody wants me to check this stuff in.
//
//    Rev 1.7   04 Jan 1995 10:58:46   MFALLEN
// Added compare function.
//
//    Rev 1.6   04 Jan 1995 09:53:26   MFALLEN
// Added create copy and delete functions.
//
//    Rev 1.5   03 Jan 1995 18:32:50   MFALLEN
// LFN's
//
//    Rev 1.4   29 Dec 1994 13:33:54   DALLEE
// MemUnlock() with single parameter only works for SYM_WIN C++.
// Changed these calls to MemRelease() for DOS compatibility.
//
//    Rev 1.3   02 Dec 1994 18:35:08   RSTANEV
// Include correct cts include file.
//
//    Rev 1.2   28 Nov 1994 19:51:24   MFALLEN
// Compiles W16
//
//    Rev 1.1   04 Nov 1994 17:39:24   MFALLEN
// Moved to W32
//
//
//    Rev 1.0   04 Nov 1994 17:27:04   MFALLEN
// Initial revision.
//
//    Rev 1.32   25 Aug 1994 21:57:22   BARRY
// Remove file integrity check for MONITOR (Bankers) and Exclusions
//
//    Rev 1.31   02 Jun 1994 16:57:10   MFALLEN
// Make sure that we do not err on exclude files that are not written in NAV 3.0
// format.
//
//    Rev 1.30   10 Feb 1994 15:30:44   DALLEE
// Added ExcludeInitSpecifyFile() so RegisterInit() can share this code.
//
//    Rev 1.29   02 Nov 1993 16:08:48   DALLEE
// #ifdef SYM_WIN around SetAnsiFlag() call.
//
//    Rev 1.28   01 Nov 1993 17:38:04   MFALLEN
// Call to SetAnsiFlag()
//
//    Rev 1.27   08 Oct 1993 10:37:32   MFALLEN
// Removed goto
//
//    Rev 1.26   15 Sep 1993 01:07:28   REESE
// Added ExcludeInitSpecifyDir().
//
//    Rev 1.25   23 Aug 1993 00:11:30   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
//
//    Rev 1.24   03 Aug 1993 19:53:46   DALLEE
// Local var i unreferenced in DOS in ExcludeInit().
//
//    Rev 1.23   03 Aug 1993 13:59:34   MFALLEN
// AnsiToOem / OemToAnsi pain
//
//
//    Rev 1.22   22 Jul 1993 09:33:34   MFALLEN
//
//    Rev 1.21   06 Jul 1993 11:06:30   DALLEE
// Removed compiler warning under DOS - unitialized var hInstance.
//
//    Rev 1.20   08 Jun 1993 00:26:08   MFALLEN
// Fixed bug with ExcludeEdit()
//
//
//    Rev 1.19   03 Jun 1993 02:13:50   DALLEE
// Changed comparison in ExcludeEdit from ExcludeCheck to STRCMP.
//
//    Rev 1.18   02 Jun 1993 15:53:56   MFALLEN
// ExcludeEdit()
//
//    Rev 1.17   27 May 1993 19:47:08   BRAD
// Not properly handling hFile = HFILE_ERROR condition.
//
//    Rev 1.16   16 May 1993 23:46:14   BRAD
//
//    Rev 1.15   16 May 1993 23:41:52   BRAD
// Extract ExcludeTerminate() into own module.
//
//    Rev 1.14   16 May 1993 23:18:10   BRAD
// Added ExcludeTerminateSpecifyDir().
//
//    Rev 1.13   10 May 1993 13:03:26   DALLEE
// (1) Added dummy local variable hInstance for DOS.
// (2) Initialize an empty exclusions array if EXCLUDE.BIN does not exist.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "exclude.h"
#include "navutil.h"
#include "file.h"

#ifdef SYM_WIN16
    #include "cts4.h"
#else
    #include "ctsn.h"
#endif


// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

UINT LOCAL ExcludeAddShort (
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPTSTR          lpItemName,         // [in] name of item
    WORD            wFlags,             // [in] exclusions flags for this item
    BOOL            bSubDirs            // [in] flag to exclude sub dirs.
    );

// --------------------------------------------------------------------------

MODULE_NAME;

#if !defined(SYM_VXD) && !defined(SYM_NTK)

/*@API:*************************************************************************
@Declaration:

VOID SYM_EXPORT WINAPI GetExcludeBin (
    HINSTANCE   hInstance,
    LPTSTR       lpszFile,
    UINT        uSize );

@Description:
Returns the full path for the default file that should be used to store
SFN exclusion entries.

@Parameters:
$hInstance$ instance handle for this program instance
$lpszFile$  pointer to memory to hold name of file
$uSize$     max number of chars this buffer can hold.

@Returns:
Nothing

@Compatibility: Win16, Win32, DOS
*******************************************************************************/

VOID SYM_EXPORT WINAPI GetExcludeBin (
    HINSTANCE   hInstance,
    LPTSTR      lpszFile,
    UINT        uSize )
{
    auto        TCHAR   szFilename[SYM_MAX_PATH+1];

    GetStartDir (hInstance, lpszFile, uSize);
    NameAppendFile(lpszFile, GetExcludeBinFilename(szFilename));
}


/*@API:*************************************************************************
@Declaration:

VOID SYM_EXPORT WINAPI GetExcludeLBin (
    HINSTANCE   hInstance,
    LPTSTR       lpszFile,
    UINT        uSize );

@Description:
Returns the full path for the default file that should be used to store
LFN exclusion entries.

@Parameters:
$hInstance$ instance handle for this program instance
$lpszFile$  pointer to memory to hold name of file
$uSize$     max number of chars this buffer can hold.

@Returns:
Nothing

@Compatibility: Win16, Win32, DOS
*******************************************************************************/

VOID SYM_EXPORT WINAPI GetExcludeLBin (
    HINSTANCE   hInstance,
    LPTSTR       lpszFile,
    UINT        uSize )
{
    auto        TCHAR   szFilename[SYM_MAX_PATH+1];

#ifndef SYM_DOS
    SYM_ASSERT (hInstance);
#endif
    SYM_ASSERT (lpszFile);
    SYM_ASSERT (uSize);

    GetStartDir (hInstance, lpszFile, uSize);
    NameAppendFile(lpszFile, GetExcludeLBinFilename(szFilename));
}


/*@API:*************************************************************************
@Declaration:

STATUS SYM_EXPORT WINAPI ExcludeInit (  // Initialize exclusions
    LPEXCLUDE   lpExclude )             // [out] structure to initialalize

@Description:
 This routine reads the exclusions information from the data file and
 places it in the EXCLUDE structure passed.
 If the file does not exist, the EXCLUDE structure is initialized with
 no EXCLUDEITEM's.

@Parameters:
$lpExclude$ structure to pass to family functions.

@Returns:
 NOERR Success
 ERR   MemAlloc or File error occurred.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

STATUS SYM_EXPORT WINAPI ExcludeInit (  // Initialize exclusions
    LPEXCLUDE   lpExclude )             // [out] structure to initialalize
{
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;
#endif
    auto        TCHAR           szFile [SYM_MAX_PATH + 1];
    auto        STATUS          Status = ERR;

    hInstance;                          // Kill DOS 'uninitialized' warning.

    GetExcludeBin(hInstance, szFile, (sizeof(szFile)-1) / sizeof(TCHAR) );

    if ( ExcludeInitSpecifyFile ( lpExclude, szFile) != ERR )
        {
                                        // -----------------------------------
                                        // Get the LFN version started.
                                        // -----------------------------------

        GetExcludeLBin ( hInstance, szFile, (sizeof(szFile)-1) / sizeof(TCHAR) );
        lpExclude->lpPrivate->hLExclude = ExcludeLOpen ( szFile, TRUE );

        if ( lpExclude->lpPrivate->hLExclude )
            Status = NOERR;
        }
    else
        {
        lpExclude->lpPrivate->hLExclude = NULL;
        }

    return ( Status );
}

#endif // !defined(SYM_VXD)

/*@API:*************************************************************************
@Declaration:

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyDir (
    LPEXCLUDE       lpExclude,
    LPTSTR           lpszDir );

@Description:
This function is used instead of ExcludeInit(). This function gives the flexi-
bility to initialize using a specific exclusions file.

@Parameters:
$lpExclude$ pointer to struct that holds init info.
$lpszDir$   directory where exclusion file can be found. (in OEM for SYM_VXD)

@Returns:
 NOERR Success
 ERR   MemAlloc or File error occurred.

@Compatibility: Win16, Win32, DOS
*******************************************************************************/

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyDir (
    LPEXCLUDE       lpExclude,
    LPTSTR           lpszDir )
{
    auto    TCHAR       szPath[SYM_MAX_PATH+1];
    auto    TCHAR       szFile[SYM_MAX_PATH+1];
    auto    STATUS      Status = ERR;

    SYM_ASSERT ( lpExclude );
    SYM_ASSERT ( lpszDir );

    STRCPY(szPath, lpszDir);
    NameAppendFile(szPath,GetExcludeBinFilename(szFile));

    if ( ExcludeInitSpecifyFile(lpExclude, szPath) == NOERR )
        {
        STRCPY(szPath, lpszDir);
        NameAppendFile ( szPath, GetExcludeLBinFilename(szFile) );

        lpExclude->lpPrivate->hLExclude = ExcludeLOpen ( szPath, TRUE );

        if ( lpExclude->lpPrivate->hLExclude )
            Status = NOERR;
        }

    return ( Status );
}


/*@API:*************************************************************************
@Declaration:

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyFile (
    LPEXCLUDE   lpExclude,
    LPTSTR       lpszFile );

@Description:
 This routine reads the exclusions from the named file.  Initializes
 to empty list if there is no file.
 Now also used for loading Register Items for Banker's File Monitor.

@Parameters:
 $lpExclude$ [out] fill with exclude items.
 $lpszFile$  Filename from which to read exclusions. (in OEM for SYM_VXD)

@Returns:
 NOERR Success
 ERR   MemAlloc or File error occurred.

@Compatibility: Win16, Win32, DOS
*******************************************************************************/

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyFile (
    LPEXCLUDE   lpExclude,
    LPTSTR       lpszFile )
{
    auto        HGLOBAL         hExcludeItem = NULL;
    auto        LPEXCLUDEITEM   lpExcludeItem = NULL;
    auto        HFILE           hFile = HFILE_ERROR;
    auto        UINT            uEntries;
    auto        STATUS          uStatus = ERR;
#if defined(SYM_VXD)
    auto        TCHAR           szDBFilename[SYM_MAX_PATH * 2];

    VxDGenericStringConvert (
        lpszFile,
        THFLAG_OEM,
        szDBFilename,
        ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK );
#endif

    SYM_ASSERT (lpszFile);

                                        // Clear the structure
    MEMSET(lpExclude, 0, sizeof(EXCLUDE));

                                        // Open the existing file
#if defined(SYM_VXD)
    if ((hFile = FileOpen(szDBFilename,OF_READ)) != HFILE_ERROR)
#else
    if ((hFile = FileOpen(lpszFile,OF_READ)) != HFILE_ERROR)
#endif
        {
                                        // Get size of file and # of entries
        DWORD dwSize = FileLength(hFile);
        uEntries = (UINT) (dwSize / sizeof(OLDEXCLUDEITEM));
        }
    else
        uEntries = 0;                   // No file created yet

                                        // -----------------------------------
                                        // Allocate memory for private data.
                                        // -----------------------------------

    lpExclude->lpPrivate = (LPEXCLUDEPRIVATE)MemAllocPtr (
                                                    GHND,
                                                    sizeof(EXCLUDEPRIVATE)
                                                    );
    if ( lpExclude->lpPrivate )
        uStatus = NOERR;
                                        // -----------------------------------
                                        // Allocate memory to hold the current
                                        // number of exclusions
                                        // -----------------------------------

    UINT uFileReadSize = uEntries * sizeof(OLDEXCLUDEITEM);
                                        // -----------------------------------
                                        // NOTE: We allocate 1 extra entry.
                                        // This insures we work correctly
                                        // when there are no exclude items.
                                        // -----------------------------------
    UINT uMemorySize   = ( uEntries + 1 ) * sizeof(EXCLUDEITEM);

    if ( uStatus == NOERR )
        {
        uStatus = ERR;

        hExcludeItem = MemAlloc ( GHND, uMemorySize );

        LPOLDEXCLUDEITEM lpOldItem = (LPOLDEXCLUDEITEM) MemAllocPtr (
                                        GHND,
                                        uFileReadSize + sizeof(OLDEXCLUDEITEM)
                                        );

        if ( hExcludeItem && lpOldItem )
            {
            lpExcludeItem = (LPEXCLUDEITEM) MemLock ( hExcludeItem );

            if ( lpExcludeItem != NULL )
                {
                                        // -----------------------------------
                                        // Read in the exclusions in a block
                                        // -----------------------------------
                if ( (uEntries == 0) ||
                     (FileRead(hFile, lpOldItem, uFileReadSize) == uFileReadSize ) )
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
                                        // -----------------------------------
                                        // RLH - All file scanning and name
                                        // comparisions are now done in OEM
                                        // -----------------------------------
//#ifdef SYM_WIN
//                        NOemToAnsi ( lpOldItem[i].szText,
//                                     lpExcludeItem[i].szText);
//#else
                        STRCPY ( lpExcludeItem[i].szText,
                                 lpOldItem[i].szText );
//#endif
#endif
                        lpExcludeItem[i].wBits         = lpOldItem[i].wBits;
                        lpExcludeItem[i].bSubDirectory =
                                                    lpOldItem[i].bSubDirectory;
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

    if ( hFile != HFILE_ERROR )
        FileClose (hFile);

    return( uStatus );
} // ExcludeInitSpecifyFile()


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

#if !defined(SYM_VXD)

/*@API:*************************************************************************
@Declaration:

UINT SYM_EXPORT WINAPI ExcludeAdd (     // Add an entry to the exclusions
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPEXCLUDEITEM   lpExcludeItem )     // [in] item to add

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

UINT SYM_EXPORT WINAPI ExcludeAdd (     // Add an entry to the exclusions
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPEXCLUDEITEM   lpExcludeItem )     // [in] item to add
{
#ifdef SYM_NTK
    return( FALSE );
#else
    SYM_ASSERT ( lpExclude );
    SYM_ASSERT ( lpExcludeItem );

    return ( ExcludeAddEx ( lpExclude, lpExcludeItem, EXCLADD_ADDMEMONLY ) );
#endif
} // ExcludeAdd()


/*@API:*************************************************************************
@Declaration:

UINT SYM_EXPORT WINAPI ExcludeAddEx (   // Add an entry to the exclusions
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPEXCLUDEITEM   lpExcludeItem )     // [in] item to add

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

UINT SYM_EXPORT WINAPI ExcludeAddEx (   // Add an entry to the exclusions
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPEXCLUDEITEM   lpExcludeItem,      // [in] item to add
    EXCLUDEACTION   ExcludeAddFlag )
{
#if defined(SYM_NTK)
    return( FALSE );
#else
    auto    UINT       uRetValue = FALSE;
    auto    STATUS     Status;
    auto    LPEXCLUDE  lpUseExclude;


    SYM_ASSERT ( lpExclude );
    SYM_ASSERT ( lpExcludeItem );
    SYM_ASSERT ( lpExclude->lpPrivate );

                                        // -----------------------------------
                                        // Check if file should be reloaded.
                                        // -----------------------------------
    lpUseExclude=lpExclude;

    if ( ExcludeAddFlag & EXCLADD_RELOADFLUSH )
        {
        Status = ExcludeTerminate ( lpUseExclude,
                                           EXCLEND_DONTSAVEFREEMEM );

        if ( Status == NOERR )
            {
            Status = ExcludeInit ( lpUseExclude );
            }
        }
    else
        {
        Status = NOERR;
        }

    if ( Status == NOERR && lpUseExclude->lpPrivate->lpExcludeMem )
        {
        if ( ExcludeIsValidSFN ( lpExcludeItem->szText ) == STATUSEXCLUDE_OK )
            {
            uRetValue = ExcludeAddShort (
                                        lpUseExclude,
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
            szShortName[0] = EOS;

            BOOL bStatus = ( ExcludeAddFlag & EXCLADD_EXACTNAME ) ?
                           FALSE :
                           ExcludeLFN2SFN (
                                        lpExcludeItem->szText,
                                        szShortName,
                                        (sizeof(szShortName)-1) / sizeof(TCHAR)
                                        );

            if ( bStatus == TRUE )
                {
                SYM_ASSERT ( *szShortName );

                uRetValue = ExcludeAddShort (
                                        lpUseExclude,
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
                                            lpUseExclude->lpPrivate->hLExclude,
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
                }
            }
                                        // -----------------------------------
                                        // If item was added with no problemo
                                        // check if memory should be flushed.
                                        // -----------------------------------

        if ( ( ExcludeAddFlag & EXCLADD_RELOADFLUSH ) && uRetValue != FALSE )
            {
            STATUS Status = ExcludeTerminate (
                                            lpUseExclude,
                                            EXCLEND_SAVEDONTFREEMEM
                                            );
            if ( Status == ERR )
                uRetValue = FALSE;
            }
        }

    return ( uRetValue );
#endif
} // ExcludeAddEx()

#endif  // !defined(SYM_VXD)


/*@API:*************************************************************************
@Declaration:

UINT SYM_EXPORT WINAPI ExcludeAddSpecifyDir (     // Add an entry to the exclusions
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPTSTR          lpszDir,            // [in] Dir spec for exclude files
    LPEXCLUDEITEM   lpExcludeItem )     // [in] item to add

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

UINT SYM_EXPORT WINAPI ExcludeAddSpecifyDir ( // Add an entry to the exclusions
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPTSTR          lpszDir,            // [in] Dir spec for exclude files
    LPEXCLUDEITEM   lpExcludeItem,      // [in] item to add
    EXCLUDEACTION   ExcludeAddFlag )
{
#ifdef SYM_NTK

   return( FALSE );

#else

    SYM_ASSERT ( lpExclude );
    SYM_ASSERT ( lpszDir );
    SYM_ASSERT ( lpExcludeItem );
    SYM_ASSERT ( lpExclude->lpPrivate );

    auto UINT      uRetValue;
    auto STATUS    Status;
    auto LPEXCLUDE lpUseExclude;

#if defined(SYM_VXD)

    auto EXCLUDE rUseExclude;

    if ( ! ExcludeCreateCopy ( lpExclude, &rUseExclude ) )
        {
        return ( FALSE );
        }

    lpUseExclude = &rUseExclude;

#else

    lpUseExclude = lpExclude;

#endif

                                        // -----------------------------------
                                        // Check if file should be reloaded.
                                        // -----------------------------------

    if ( ExcludeAddFlag & EXCLADD_RELOADFLUSH )
        {
        Status = ExcludeTerminateSpecifyDirEx ( lpUseExclude,
                                                lpszDir,
                                                EXCLEND_DONTSAVEFREEMEM );

        if ( Status == NOERR )
            {
            Status = ExcludeInitSpecifyDir ( lpUseExclude, lpszDir );
            }
        }
    else
        {
        Status = NOERR;
        }

    if ( Status == NOERR && lpUseExclude->lpPrivate->lpExcludeMem )
        {
                                        // Note: the next if() ... else ...
                                        // if() ... else ... returns result
                                        // in uRetValue.

        if ( ExcludeIsValidSFN ( lpExcludeItem->szText ) == STATUSEXCLUDE_OK )
            {
            uRetValue = ExcludeAddShort (
                                        lpUseExclude,
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
                                        lpUseExclude,
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
                                            lpUseExclude->lpPrivate->hLExclude,
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
                }
            }
        else
            {
            SYM_ASSERT ( FALSE );

            uRetValue = FALSE;
            }
                                        // -----------------------------------
                                        // If item was added with no problemo
                                        // check if memory should be flushed.
                                        // -----------------------------------

        if ( ( ExcludeAddFlag & EXCLADD_RELOADFLUSH ) && uRetValue != FALSE )
            {
            STATUS Status = ExcludeTerminateSpecifyDirEx (
                                            lpUseExclude,
                                            lpszDir,
                                            EXCLEND_SAVEDONTFREEMEM
                                            );
            if ( Status == ERR )
                uRetValue = FALSE;
            }
        }

#if defined(SYM_VXD)

    if ( uRetValue != FALSE )
        {
        auto EXCLUDE rNukeExclude;

        memcpy ( &rNukeExclude, lpExclude, sizeof(EXCLUDE) );
        memcpy ( lpExclude, lpUseExclude, sizeof(EXCLUDE) );

        ExcludeKillCopy ( &rNukeExclude );
        }

#endif

    return ( uRetValue );

#endif
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
#ifdef SYM_NTK
    return( FALSE );
#else
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
#endif
}

/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeDeleteIndex (// Delete an excluded entry
    LPEXCLUDE   lpExclude,              // [in] structure used during init.
    WORD        wIndex )                // [in] item index number to delete

@Description:
This function deletes an entry using it's index number.

@Parameters:
 $lpExclude$ structure that was used during init.
 $wIndex$    index number of the item to delete.

@Returns:
 TRUE    if the item was deleted
 FALSE   if the item could not be deleted

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeDeleteIndex (// Delete an excluded entry
    LPEXCLUDE   lpExclude,              // [in] structure used during init.
    WORD        wIndex )                // [in] item index number to delete
{
    auto    BOOL     bFound = FALSE;

    SYM_ASSERT ( lpExclude );

    if ( wIndex < lpExclude->wEntries )
        {
        if ( lpExclude->lpPrivate->lpExcludeMem && lpExclude->wEntries )
            {
                                        // -----------------------------------
                                        // Copy items later in the list up
                                        // -----------------------------------

            if ( wIndex + 1 < lpExclude->wEntries )
                MEMCPY (&lpExclude->lpPrivate->lpExcludeMem[wIndex],
                        &lpExclude->lpPrivate->lpExcludeMem[wIndex+1],
                        sizeof(EXCLUDEITEM) * (lpExclude->wEntries - wIndex - 1));

                                        // -----------------------------------
                                        // One less entry
                                        // -----------------------------------
            lpExclude->wEntries--;
                                        // -----------------------------------
                                        // Free up an entry
                                        // We always keep 1 extra entry on the end
                                        // -----------------------------------
            MemRelease(lpExclude->hExcludeMem);
            lpExclude->hExcludeMem = MemReAlloc(
                                        lpExclude->hExcludeMem,
                                        (lpExclude->wEntries + 1) *
                                        sizeof(EXCLUDEITEM), GHND
                                        );

            lpExclude->lpPrivate->lpExcludeMem = (LPEXCLUDEITEM)MemLock(
                                        lpExclude->hExcludeMem
                                        );
            bFound = TRUE;
            }
        }
    else
        {
                                        // -----------------------------------
                                        // Easyer when object model based eh?
                                        // -----------------------------------
        wIndex -= lpExclude->wEntries;

        STATUSEXCLUDE Status = ExcludeLDelete (
                                    lpExclude->lpPrivate->hLExclude,
                                    wIndex
                                    );

        if ( Status == STATUSEXCLUDE_OK )
            {
            bFound = TRUE;
            }
        }

    return ( bFound );
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


#if !defined(SYM_VXD) && !defined(SYM_NTK)

/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeDelete (  // Delete an exclusion item
    LPEXCLUDE       lpExclude,          // [in] returned from init.
    LPEXCLUDEITEM   lpExcludeItem )     // [in] item to delete

@Description:
This function deletes an excluded item from the list of excluded items. Use
ExcludeDeleteEx() instead.

@Parameters:
 $lpExclude$ structure that was used during init.
 $lpExcludeItem$ pointer to LPEXCLUDEITEM structure to delete.

@Returns:
TRUE if the item was successfully deleted.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeDelete (  // Delete an exclusion item
    LPEXCLUDE       lpExclude,          // [in] returned from init.
    LPEXCLUDEITEM   lpExcludeItem )     // [in] item to delete
{
    SYM_ASSERT ( lpExclude );
    SYM_ASSERT ( lpExcludeItem );

    auto    BOOL    bFound = FALSE;
    auto    TCHAR   szShortSpec [ SYM_MAX_PATH + 1 ] ;

    szShortSpec[0] = EOS;

                                    // -----------------------------------
                                    // Make sure we're comparing apples
                                    // with apples.
                                    // -----------------------------------

    if ( ExcludeLFN2SFN ( lpExcludeItem->szText, szShortSpec,
                          (sizeof(szShortSpec)-1) / sizeof(TCHAR) ) == FALSE )
        {
        STRNCPY ( szShortSpec, lpExcludeItem->szText,
                  sizeof(lpExcludeItem->szText) / sizeof(TCHAR) );
        }

                                    // -----------------------------------
                                    // And now try to find the item within
                                    // the short exclusions.
                                    // -----------------------------------

    if ( lpExclude->lpPrivate->lpExcludeMem )
        {
        for ( UINT i = 0; i < lpExclude->wEntries; i++ )
            {
            // Compare both the original filename and SFN alias. Either one
            // counts as a match.
            bFound = (!STRICMP(lpExclude->lpPrivate->lpExcludeMem[i].szText, lpExcludeItem->szText) ||
                      !STRICMP(lpExclude->lpPrivate->lpExcludeMem[i].szText, szShortSpec));
            if (bFound)
                {
                                        // Delete this item
                ExcludeDeleteIndex ( lpExclude, i );
                break;
                }
            }
        }

    if ( bFound == FALSE )
        {
                                        // -----------------------------------
                                        // Item was not found. Check LFN
                                        // exclusions for the item.
                                        // -----------------------------------

        WORD wEntries = ExcludeLGetCount ( lpExclude->lpPrivate->hLExclude );
        LEXCLUDE    ExcludeItem;

        for ( WORD wIndex = 0; wIndex < wEntries; wIndex++ )
            {
                                        // -----------------------------------
                                        // Get each item.
                                        // -----------------------------------

            STATUSEXCLUDE Status = ExcludeLGet (
                                        lpExclude->lpPrivate->hLExclude,
                                        wIndex,
                                        &ExcludeItem
                                        );

            if ( Status == STATUSEXCLUDE_OK )
                {
                                        // -----------------------------------
                                        // Check if this is the one.
                                        // -----------------------------------

                if ( !STRICMP ( ExcludeItem.szItem, lpExcludeItem->szText ) )
                    {
                                        // -----------------------------------
                                        // Yes so delete it.
                                        // -----------------------------------

                    Status = ExcludeLDelete (
                                        lpExclude->lpPrivate->hLExclude,
                                        wIndex
                                        );

                    if ( Status == STATUSEXCLUDE_OK )
                        {
                                        // -----------------------------------
                                        // If deleted return success.
                                        // -----------------------------------
                        bFound = TRUE;
                        }

                    break;
                    }
                }
            else
                {
                break;
                }
            }
        }

    return ( bFound );
}


/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeEdit (    // Search and replace an entry
    LPEXCLUDE       lpExclude,          // [in] returned from init.
    LPEXCLUDEITEM   lpFindItem,         // [in] item to find
    LPEXCLUDEITEM   lpReplaceWith )     // [in] item to replace with

@Description:
This function finds and replaces and entry. Use ExcludeEditEx() instead.

@Parameters:
 $lpExclude$ structure that was used during init.
 $lpFIndItem$ item to find.
 $lpReplaceWith$ replace with this.

@Returns:
TRUE if the item was successfully replaced.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeEdit (    // Search and replace an entry
    LPEXCLUDE       lpExclude,          // [in] returned from init.
    LPEXCLUDEITEM   lpFindItem,         // [in] item to find
    LPEXCLUDEITEM   lpReplaceWith )     // [in] item to replace with
{
    auto    BOOL     bFound = FALSE;

    SYM_ASSERT (lpExclude);
    SYM_ASSERT (lpFindItem);
    SYM_ASSERT (lpReplaceWith);

                                        // -----------------------------------
                                        // It is very important that editing
                                        // is being done this way. ExcludeAdd
                                        // will ensure that the new item will
                                        // be placed in the correct exclude
                                        // area.
                                        // -----------------------------------

    if ( ExcludeIsValidSFN ( lpReplaceWith->szText ) == STATUSEXCLUDE_OK ||
         ExcludeIsValidLFN ( lpReplaceWith->szText ) == STATUSEXCLUDE_OK )
        {
        if ( ExcludeDelete ( lpExclude, lpFindItem ) == TRUE &&
             ExcludeAdd ( lpExclude, lpReplaceWith ) == TRUE )
            {
            bFound = TRUE;
            }
        }

    return ( bFound );
}

#endif

