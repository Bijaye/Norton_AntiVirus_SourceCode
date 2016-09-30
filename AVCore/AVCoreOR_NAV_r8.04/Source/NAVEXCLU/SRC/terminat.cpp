// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navexclu/VCS/terminat.cpv   1.0   06 Feb 1997 20:57:56   RFULLER  $
//
// See Also:
//************************************************************************
// $Log:   S:/navexclu/VCS/terminat.cpv  $
// 
//    Rev 1.0   06 Feb 1997 20:57:56   RFULLER
// Initial revision
// 
//    Rev 1.4   27 Jun 1996 16:52:58   RHAYES
// Luigi merge - Remove OEM<->Ansi conversion since all
// scanning and comparisons are now exclusively in OEM.
// 
//    Rev 1.3   10 May 1996 10:59:02   RHAYES
// Unicode conversion update.
// 
//    Rev 1.2   06 May 1996 16:32:52   RSTANEV
// Merged Ron's Unicode changes.
// 
//    Rev 1.1   19 Mar 1996 13:50:46   jworden
// Double byte enable
// 
//    Rev 1.0   30 Jan 1996 15:53:04   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:08   BARRY
// Initial revision.
// 
//    Rev 1.23   26 Jun 1995 23:09:14   RSTANEV
// Now properly converting the exclude DB location to current character set
// when deinitializing an exclusion structure.
//
//    Rev 1.22   20 Mar 1995 17:52:24   AMATIC
// Martin did this. Do additional checks before trying to use memory pointers.
//
//    Rev 1.21   19 Jan 1995 13:21:12   MFALLEN
// Structure offset was not incremented.
//
//    Rev 1.20   19 Jan 1995 10:31:42   MARKL
// Added more SYM_ASSERTs.  Also set free'd memory handles and pointers
// to NULL.
//
//    Rev 1.19   16 Jan 1995 17:09:38   MFALLEN
// OEM ifdef fix.
//
//    Rev 1.18   16 Jan 1995 16:41:36   ELEAR
//
//    Rev 1.17   16 Jan 1995 16:28:48   MFALLEN
// Fixed correct return status for terminate.
//
//    Rev 1.16   16 Jan 1995 15:42:00   MARKL
// Fixed link errors in VxD.
//
//    Rev 1.15   16 Jan 1995 15:20:56   MARKL
// ifdefed out some code not needed by VxD.
//
//    Rev 1.14   16 Jan 1995 13:56:00   MARKL
// Wrote ExcludeTerminateSpecifyDirEx() based on Martin's changes this
// weekend so VxD can have exclusion support.  Copy/paste of its
// parent function with support for the new flags that were added.
//
//    Rev 1.13   16 Jan 1995 09:02:02   MARKL
// Fixed bug where file would not compile.
//
//    Rev 1.12   13 Jan 1995 18:45:38   MFALLEN
// Spiffed up for VXD, still testing.
//
//    Rev 1.11   13 Jan 1995 17:38:24   MFALLEN
// Removed ..Ex() functions and added new exclude match checking routine.
//
//    Rev 1.10   13 Jan 1995 13:42:42   MFALLEN
// Fixed order in which memory is freed.
//
//    Rev 1.9   11 Jan 1995 16:25:34   MFALLEN
// Fixed VXD code.
//
//    Rev 1.8   11 Jan 1995 16:17:10   MFALLEN
// Made sure the exclude structure is unchanged.
//
//    Rev 1.7   10 Jan 1995 18:44:30   MARKL
// ExcludeTerminateSpecifyDir() now works correctly, supporting the old
// exclusions and the new LFN exclusions.
//
//    Rev 1.6   06 Jan 1995 15:23:52   MFALLEN
// Fixed for DOS.
//
//    Rev 1.5   05 Jan 1995 17:24:52   MFALLEN
// Polished.
//
//    Rev 1.4   05 Jan 1995 13:47:20   MFALLEN
//
//    Rev 1.3   03 Jan 1995 18:32:56   MFALLEN
// LFN's
//
//    Rev 1.2   29 Dec 1994 13:36:10   DALLEE
// MemUnlock() with single parameter only works for SYM_WIN C++.
// Changed these calls to MemRelease() for DOS compatibility.
//
//    Rev 1.1   04 Nov 1994 17:39:34   MFALLEN
// Moved to W32
//
//
//    Rev 1.0   04 Nov 1994 17:27:06   MFALLEN
// Initial revision.
//
//    Rev 1.12   10 Feb 1994 15:38:14   DALLEE
// Added new layer ExcludeTerminateSpecifyFile() so RegisterTerminate() for
// Banker's can share this code.
//
//    Rev 1.11   11 Sep 1993 13:58:30   MFALLEN
// Before deleting the exclusions file, make sure it exists outerwise we return
// with ERR and complain that we coundn't save the exclude file.
//
//    Rev 1.10   09 Sep 1993 18:28:04   DALLEE
// Delete exclude.dat if no entries.  Make sure to free mem if flag is set.
//
//    Rev 1.9   09 Sep 1993 06:56:12   DALLEE
// Removed FileDelete() so ExcludeTerminate() only causes one error on
// a write protected diskette.
//
//    Rev 1.8   23 Aug 1993 00:11:30   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
//
//    Rev 1.7   03 Aug 1993 19:58:00   DALLEE
// Removed 2 compiler warnings in DOS.  Unreferenced local var (used by WIN)
// and pointer type mismatch.
//
//    Rev 1.6   03 Aug 1993 13:59:18   MFALLEN
// AnsiToOem / OemToAnsi pain
//
//
//    Rev 1.5   28 Jul 1993 16:31:38   DALLEE
// TSR_ON and TSR_OFF macros in place of #ifdef blah blah blah for WIN and DOS.
//
//    Rev 1.4   25 Jul 1993 23:42:40   BRAD
// Changed data file extension from BIN to DAT.
//
//    Rev 1.3   09 Jul 1993 18:05:56   DALLEE
// Disable NAV TSR when writing EXCLUDE.DAT
//
//    Rev 1.2   06 Jul 1993 11:12:18   DALLEE
// Removed compiler warning - uninitialized var hInstance.
//
//    Rev 1.1   16 May 1993 23:46:22   BRAD
//
//    Rev 1.0   16 May 1993 23:42:10   BRAD
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "exclude.h"
#include "navutil.h"
#include "file.h"
#include "tsr.h"
#include "tsrcomm.h"

MODULE_NAME;

//************************************************************************
//************************************************************************

LPTSTR SYM_EXPORT WINAPI GetExcludeBinFilename(LPTSTR lpszFilename)
{
    return (STRCPY(lpszFilename, _T("EXCLUDE.DAT")));
}

//************************************************************************
//************************************************************************

LPTSTR SYM_EXPORT WINAPI GetExcludeLBinFilename(LPTSTR lpszFilename)
{
    return (STRCPY(lpszFilename, _T("EXCLUDEL.DAT")));
}



//************************************************************************
//************************************************************************
#if !defined(SYM_VXD) && !defined(SYM_NTK)

STATUS SYM_EXPORT WINAPI ExcludeTerminate (
    LPEXCLUDE   lpExclude,
    BOOL        bFreeMemory )
{
    auto    EXCLUDEACTION   Action;

    SYM_ASSERT (lpExclude);

    if ( bFreeMemory == TRUE )
        Action = EXCLEND_SAVEFREEMEM;
    else
        Action = EXCLEND_SAVEDONTFREEMEM;

    return ( ExcludeTerminateEx ( lpExclude, Action ) );
}


STATUS SYM_EXPORT WINAPI ExcludeTerminateEx (
    LPEXCLUDE       lpExclude,
    EXCLUDEACTION   ExclAction
    )
{
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;
#endif
    auto        TCHAR            szFile [SYM_MAX_PATH + 1];
    auto        STATUS          Status = NOERR;

#ifndef SYM_WIN
    hInstance;                          // Compiler cries about uninitialized
#endif                                  // variable.

    SYM_ASSERT (lpExclude);

    GetExcludeBin(hInstance, szFile, (sizeof(szFile)-1) / sizeof(TCHAR) );

                                        // -----------------------------------
                                        // This function can be called before
                                        // the calling program had a chance
                                        // to initialize. That's why we must
                                        // be paranoid.
                                        // -----------------------------------

    if ( lpExclude
    &&   lpExclude->lpPrivate
    &&   lpExclude->lpPrivate->hLExclude )
        {
        EXCLUDELACTION Action;

                                        // EXCLEND_DONTSAVEFREEMEM
                                        // EXCLEND_SAVEDONTFREEMEM
                                        // EXCLEND_SAVEFREEMEM

        if ( ExclAction == EXCLEND_DONTSAVEFREEMEM )
            Action = EXCLUDELACTION_DONTSAVEFREEMEM;
        else if ( ExclAction == EXCLEND_SAVEDONTFREEMEM )
            Action = EXCLUDELACTION_SAVEDONTFREEMEM;
        else
            Action = EXCLUDELACTION_SAVEFREEMEM;

        if ( ExcludeLClose ( lpExclude->lpPrivate->hLExclude, Action )
                                                        != STATUSEXCLUDE_OK )
            {
            Status = ERR;
            }
        }

    if ( Status == NOERR )
        {
        Status = ExcludeTerminateSpecifyFile (
                                        lpExclude,
                                        szFile,
                                        ExclAction
                                        );
        }

    return ( Status );
}

#endif

//************************************************************************

STATUS SYM_EXPORT WINAPI ExcludeTerminateSpecifyDir (
    LPEXCLUDE       lpExclude,
    LPTSTR           lpszDir,
    BOOL            bFreeMemory
    )
{
    auto        TCHAR            szPath[SYM_MAX_PATH+1];
    auto        TCHAR            szFile[SYM_MAX_PATH+1];
    auto        STATUS          Status = NOERR;

    SYM_ASSERT (lpExclude);
    SYM_ASSERT (lpszDir);

    STRCPY(szPath, lpszDir);
    NameAppendFile(szPath,GetExcludeBinFilename(szFile));

    if ( lpExclude->lpPrivate->hLExclude )
        {
        EXCLUDELACTION Action;

        if ( bFreeMemory == TRUE )
            Action = EXCLUDELACTION_SAVEFREEMEM;
        else
            Action = EXCLUDELACTION_SAVEDONTFREEMEM;

        if ( ExcludeLClose ( lpExclude->lpPrivate->hLExclude, Action )
                                                        != STATUSEXCLUDE_OK )
            {
            Status = ERR;
            }
        }

    if ( Status == NOERR )
        {
        EXCLUDEACTION Action;

        if ( bFreeMemory == TRUE )
            Action = EXCLEND_SAVEFREEMEM;
        else
            Action = EXCLEND_SAVEDONTFREEMEM;

        Status = ExcludeTerminateSpecifyFile (
                                        lpExclude,
                                        szPath,
                                        Action
                                        );
        }

    return ( Status );
}


//************************************************************************

STATUS SYM_EXPORT WINAPI ExcludeTerminateSpecifyDirEx (
    LPEXCLUDE       lpExclude,
    LPTSTR           lpszDir,
    EXCLUDEACTION   ExclAction
    )
{
    auto        TCHAR            szPath[SYM_MAX_PATH+1];
    auto        TCHAR            szFile[SYM_MAX_PATH+1];
    auto        EXCLUDELACTION  Action;
    auto        STATUS          Status = NOERR;

    SYM_ASSERT (lpExclude);
    SYM_ASSERT (lpszDir);

    STRCPY(szPath, lpszDir);
    NameAppendFile(szPath,GetExcludeBinFilename(szFile));

    if ( lpExclude->lpPrivate->hLExclude )
        {

                                        // EXCLEND_DONTSAVEFREEMEM
                                        // EXCLEND_SAVEDONTFREEMEM
                                        // EXCLEND_SAVEFREEMEM
        if ( ExclAction == EXCLEND_DONTSAVEFREEMEM )
            Action = EXCLUDELACTION_DONTSAVEFREEMEM;
        else if ( ExclAction == EXCLEND_SAVEDONTFREEMEM )
            Action = EXCLUDELACTION_SAVEDONTFREEMEM;
        else
            Action = EXCLUDELACTION_SAVEFREEMEM;

        if ( ExcludeLClose ( lpExclude->lpPrivate->hLExclude, Action )
                                                        != STATUSEXCLUDE_OK )
            {
            Status = ERR;
            }
        }

    if ( Status == NOERR )
        {
        Status = ExcludeTerminateSpecifyFile (
                                        lpExclude,
                                        szPath,
                                        ExclAction
                                        );
        }

    return ( Status );
}


//************************************************************************
// ExcludeTerminateSpecifyFile()
//
// This routine writes the exclusions to the EXCLUDE.DAT file, first disabling
// the NAV tsr if present.
// Now also used to write Register Items for Banker's File Monitor.
//
// Parameters:
//      LPEXCLUDE       lpExclude       The exclusions.
//      LPTSTR           lpszDir         Directory to write EXCLUDE.DAT. (in OEM for SYM_VXD)
//      BOOL            bFreeMemory     Free the memory used to store
//                                        exclusions?
//
// Returns:
//      NOERR                           Success.
//      ERR                             FileOpen or FileCreate error.
//                                          EXCLUDE.DAT not written.
//***************************************************************************
//  7/09/93 DALLEE, Moved TSR disabling code here from COMMON::OPTIONS.C
//  2/03/94 DALLEE, Added yet another layer so this can be used for
//                  Banker's Register Items as well.
//************************************************************************

STATUS SYM_EXPORT WINAPI ExcludeTerminateSpecifyFile (
    LPEXCLUDE       lpExclude,
    LPTSTR           lpszFile,
    EXCLUDEACTION   ExclAction
    )
{
    auto    UINT            i;
    auto    HFILE           hFile;
    auto    STATUS          uStatus = ERR;
    auto    UINT            uSize;
#if defined(SYM_VXD)
    auto    TCHAR           szFile[SYM_MAX_PATH * 2];

    VxDGenericStringConvert (
        lpszFile,
        THFLAG_OEM,
        szFile,
        ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK );
#endif

    SYM_ASSERT (lpExclude);
    SYM_ASSERT (lpszFile);

                                        // ----------------------------------
                                        // Possible ExclActions at the time
                                        // of writing this.
                                        //
                                        // EXCLEND_DONTSAVEFREEMEM
                                        // EXCLEND_SAVEDONTFREEMEM
                                        // EXCLEND_SAVEFREEMEM
                                        // ----------------------------------

    if ( ExclAction == EXCLEND_SAVEFREEMEM ||
         ExclAction == EXCLEND_SAVEDONTFREEMEM )
        {
        if (0 == lpExclude->wEntries)  // Nothing in the EXCLUDE list
            {
#if defined(SYM_VXD)
            if (FileExists(szFile))
                uStatus = FileDelete(szFile);
#else
            if (FileExists(lpszFile))
                uStatus = FileDelete(lpszFile);
#endif
            else
                uStatus = NOERR;
            }
        else if ( lpExclude->lpPrivate && lpExclude->lpPrivate->lpExcludeMem )
            {
#if defined(SYM_VXD)
            if ((hFile = FileCreate(szFile,FA_NORMAL)) != HFILE_ERROR)
#else
            if ((hFile = FileCreate(lpszFile,FA_NORMAL)) != HFILE_ERROR)
#endif
                {
                uSize = sizeof(OLDEXCLUDEITEM) * lpExclude->wEntries;

                LPOLDEXCLUDEITEM lpOldExcludeItem = (LPOLDEXCLUDEITEM)MemAllocPtr (
                                                                GHND,
                                                                uSize
                                                                );
                if ( lpOldExcludeItem )
                    {
                    for ( i = 0; i < lpExclude->wEntries; i++ )
                        {
#ifdef _UNICODE
                                       //----------------------------------
                                       // Convert from Unicode back to OEM
                                       // before storing back on disk
                                       //----------------------------------
                        WideCharToMultiByte( 
                                CP_OEMCP, 
                                0, 
                                lpExclude->lpPrivate->lpExcludeMem[i].szText,
                                -1, 
                                lpOldExcludeItem[i].szText,
                                sizeof(lpOldExcludeItem->szText),
                                NULL,
                                NULL );
#else
//#ifdef SYM_WIN
                                        // -----------------------------------
                                        // RLH - All file scanning and name
                                        // comparisions are now done in OEM
                                        // -----------------------------------
//                        NAnsiToOem (
//                                lpExclude->lpPrivate->lpExcludeMem[i].szText,
//                                lpOldExcludeItem[i].szText
//                                );
//#else
                        STRCPY (
                               lpOldExcludeItem[i].szText,
                               lpExclude->lpPrivate->lpExcludeMem[i].szText
                               );
//#endif
#endif
                        lpOldExcludeItem[i].wBits =
                                lpExclude->lpPrivate->lpExcludeMem[i].wBits;

                        lpOldExcludeItem[i].bSubDirectory =
                            lpExclude->lpPrivate->lpExcludeMem[i].bSubDirectory;
                        }

                    if ( FileWrite ( hFile, lpOldExcludeItem, uSize ) == uSize )
                        {
                        uStatus = NOERR;
                        }

                    MemFreePtr ( lpOldExcludeItem );
                    lpOldExcludeItem = NULL;
                    }

                FileWrite ( hFile, NULL, 0 );  // Terminate file.
                FileClose ( hFile );
                }
            }
        }
    else
        {
        uStatus = NOERR;
        }


    if ( lpExclude->lpPrivate &&
         ExclAction == EXCLEND_DONTSAVEFREEMEM ||
         ExclAction == EXCLEND_SAVEFREEMEM )
        {
        if ( lpExclude->hExcludeMem )
            {
            MemRelease ( lpExclude->hExcludeMem );
            MemFree ( lpExclude->hExcludeMem );
            lpExclude->hExcludeMem = NULL;

            lpExclude->lpPrivate->lpExcludeMem = NULL;

            if ( lpExclude->lpPrivate )
                {
                MemFreePtr ( lpExclude->lpPrivate );
                lpExclude->lpPrivate = NULL;
                }

            lpExclude->hExcludeMem = NULL;
            }
        }

    return ( uStatus );
}


