// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/exclude.h_v   1.3   10 Jun 1997 15:58:20   DBUCHES  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/exclude.h_v  $
//
//    Rev 1.3   10 Jun 1997 15:58:20   DBUCHES
// Fixed typo.
//
//    Rev 1.2   10 Jun 1997 15:56:50   DBUCHES
// Added pragmas for alpha support.
//
//    Rev 1.1   13 May 1997 16:40:18   DALLEE
// Added ExcludeIsEx() which takes both LFN and SFN of file.
//
//    Rev 1.0   06 Feb 1997 20:51:30   RFULLER
// Initial revision
//
//    Rev 1.3   06 May 1996 15:36:56   RSTANEV
// Merged Ron's Unicode changes.
//
//    Rev 1.2   01 Apr 1996 22:00:54   RSTANEV
// Added a specific piece of the H file to SYM_NTK compilation.
//
//    Rev 1.1   19 Mar 1996 13:59:32   jworden
// Double byte enable
//
//    Rev 1.0   30 Jan 1996 15:46:48   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:11:08   BARRY
// Initial revision.
//
//    Rev 1.45   09 Jul 1995 14:32:18   RHAYES
// Changed lpszItem in ExcludeIsValid from LPCSTR to LPSTR so that we can
// change all forward slashes ('/') to back slashes ('\').
//
//    Rev 1.44   08 Jul 1995 10:33:42   RSTANEV
// Added EXCLADD_EXACTNAME.
//
//    Rev 1.43   27 Jun 1995 23:33:28   RSTANEV
// Added bIdleUpdating to HEXCLUDEN.
//
//    Rev 1.42   27 Jun 1995 15:14:42   RSTANEV
// Added one more API.
//
//    Rev 1.41   27 Jun 1995 11:03:54   RSTANEV
// Added a new structure for the VxD platform.
//
//    Rev 1.40   26 Jun 1995 22:09:48   RSTANEV
// Fixed some of the new flags.
//
//    Rev 1.39   26 Jun 1995 21:34:48   RSTANEV
// Added new exclusion function prototypes for VxD platform.
//
//    Rev 1.38   22 May 1995 16:18:20   DALLEE
// check function now takes long and short paths explicitly - CGRANGE
//
//
//    Rev 1.37   17 Jan 1995 12:05:16   RSTANEV
// Added PEXCLUDE.
//
//    Rev 1.36   16 Jan 1995 15:19:50   MARKL
// Added ExcludeAddSpecifyDir()
//
//    Rev 1.35   16 Jan 1995 13:55:32   MARKL
// Added ExcludeTerminateSpecifyDirEx() based on Martin's previous
// changes so VxD can use new exclusions.
//
//    Rev 1.34   13 Jan 1995 18:45:10   MFALLEN
// Spiffed up for VXD, I'm still testing.
//
//    Rev 1.33   13 Jan 1995 17:38:32   MFALLEN
// Removed all ...Ex() functions.
//
//    Rev 1.32   11 Jan 1995 16:17:20   MFALLEN
// Made sure exclude structure remains unchanged.
//
//    Rev 1.31   11 Jan 1995 15:17:12   MFALLEN
// Defined EXCLUDE_DUPLICATE
//
//    Rev 1.30   05 Jan 1995 17:12:32   MFALLEN
// Surprise.
//
//    Rev 1.29   04 Jan 1995 10:39:30   MFALLEN
// Exclude object compare function.
//
//    Rev 1.28   04 Jan 1995 09:53:46   MFALLEN
// Added create copy and delete functions.
//
//    Rev 1.27   03 Jan 1995 18:44:48   MFALLEN
// ExcludeGet() and ExcludeGetCount()
//
//    Rev 1.26   03 Jan 1995 18:33:20   MFALLEN
// LFNs
//
//    Rev 1.25   28 Dec 1994 14:34:48   DALLEE
// DOS now using CTSN.H
//
//    Rev 1.24   28 Nov 1994 20:07:12   MFALLEN
// #define for DOS, let's hope it works.
//
//    Rev 1.23   28 Nov 1994 13:25:54   MFALLEN
//
//
//    Rev 1.22   03 Nov 1994 12:00:32   MFALLEN
// Now using CTSN.H
//
//    Rev 1.21   31 Oct 1994 12:48:16   MFALLEN
// Starting of fresh from the old NAV 3.0 codebase.
//
//    Rev 1.15   10 Feb 1994 16:15:18   DALLEE
// Added Exclude[Init/Terminate]SpecifyFile() so registration for Banker's
// could share this code.
//************************************************************************

#ifndef _EXCLU_STUFF
#define _EXCLU_STUFF

#if defined(SYM_WIN16)
    #include "cts4.h"
#else
    #include "ctsn.h"
#endif

#include "excludel.h"

#define MAX_EXCLUDES            (50)

                                        // ----------------------------------
                                        // This value is returned from
                                        // ExcludeAdd() and ExcludeAddEx()
                                        // if the entry to be added already
                                        // exists.
                                        // ----------------------------------
#define EXCLUDE_DUPLICATE       2

typedef WORD  EXCLUDEACTION;

                                        // The following are bit flags with
                                        // exception of EXCLADD_ADDMEMONLY
                                        // When you call the APIs, you can
                                        // either pass only the first flag,
                                        // or a combination of any other
                                        // flags
#define EXCLADD_ADDMEMONLY      0       // Add the good ol' way
#define EXCLADD_RELOADFLUSH     1       // reaload from file, add'n flush
#define EXCLADD_EXACTNAME       2       // don't convert LFN2SFN or SFN2LFN

#define EXCLEND_DONTSAVEFREEMEM 0       // Do not save memory and free memory.
#define EXCLEND_SAVEDONTFREEMEM 1       // Save memory do not free it up.
#define EXCLEND_SAVEFREEMEM     2       // Save memory and free it up.

#ifdef SYM_WIN
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct tagEXCLUDEPRIVATE
{
   LPEXCLUDEITEM        lpExcludeMem;   // Ptr to EXCLUDE entries
   HLEXCLUDE            hLExclude;      // Handle to LFN exclude

} EXCLUDEPRIVATE, FAR * LPEXCLUDEPRIVATE;

typedef struct tagEXCLUDE
   {
   WORD                 wEntries;       // Number of EXCLUDE entries
   HGLOBAL              hExcludeMem;    // Handle to EXCLUDE entries
   LPEXCLUDEPRIVATE     lpPrivate;      // Private members. The size of this
                                        // structure can't change.
   } EXCLUDE, FAR *LPEXCLUDE, *PEXCLUDE;

#ifdef SYM_WIN
#pragma pack(pop)
#endif

#ifdef __cplusplus
extern "C" {
#endif

UINT SYM_EXPORT WINAPI ExcludeAdd (
    LPEXCLUDE       lpExclude,
    LPEXCLUDEITEM   lpExcludeItem
    );

UINT SYM_EXPORT WINAPI ExcludeAddEx (
    LPEXCLUDE       lpExclude,
    LPEXCLUDEITEM   lpExcludeItem,
    EXCLUDEACTION   ExclAction
    );

UINT SYM_EXPORT WINAPI ExcludeAddSpecifyDir ( // Add an entry to the exclusions
    LPEXCLUDE       lpExclude,          // [in] structure initialized
    LPTSTR          lpszDir,            // [in] Dir spec
    LPEXCLUDEITEM   lpExcludeItem,      // [in] item to add
    EXCLUDEACTION   ExcludeAddFlag );

VOID SYM_EXPORT WINAPI GetExcludeBin (
    HINSTANCE       hInstance,
    LPTSTR          szFile,
    UINT            uSize
    );

VOID SYM_EXPORT WINAPI GetExcludeLBin (
    HINSTANCE       hInstance,
    LPTSTR          szFile,
    UINT            uSize
    );

VOID SYM_EXPORT WINAPI GetThreatExcludeBin (
    HINSTANCE       hInstance,
    LPTSTR          szFile,
    UINT            uSize
    );

VOID SYM_EXPORT WINAPI GetThreatExcludeLBin (
    HINSTANCE       hInstance,
    LPTSTR          szFile,
    UINT            uSize
    );

LPTSTR SYM_EXPORT WINAPI GetExcludeBinFilename (
    LPTSTR          lpszFilename
    );

LPTSTR SYM_EXPORT WINAPI GetExcludeLBinFilename (
    LPTSTR          lpszFilename
    );

LPTSTR SYM_EXPORT WINAPI GetThreatExcludeBinFilename (
    LPTSTR          lpszFilename
    );

LPTSTR SYM_EXPORT WINAPI GetThreatExcludeLBinFilename (
    LPTSTR          lpszFilename
    );

STATUS SYM_EXPORT WINAPI ExcludeInit (
    LPEXCLUDE       lpExclude
    );

STATUS SYM_EXPORT WINAPI ThreatExcludeInit (
    LPEXCLUDE       lpExclude
    );

STATUS SYM_EXPORT WINAPI ThreatDefaultExcludeInit (
    LPEXCLUDE       lpExclude
    );

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyDir (
    LPEXCLUDE       lpExclude,
    LPTSTR          lpszDir
    );

STATUS SYM_EXPORT WINAPI ExcludeTerminateSpecifyDirEx (
    LPEXCLUDE       lpExclude,
    LPTSTR          lpszDir,
    EXCLUDEACTION   ExclAction
    );

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyFile (
    LPEXCLUDE       lpExclude,
    LPTSTR          lpszFile
    );

STATUS SYM_EXPORT WINAPI ExcludeTerminate (
    LPEXCLUDE       lpExclude,
    BOOL            bFreeMemory
    );

STATUS SYM_EXPORT WINAPI ExcludeTerminateEx (
    LPEXCLUDE       lpExclude,
    EXCLUDEACTION   ExclAction
    );

STATUS SYM_EXPORT WINAPI ThreatExcludeTerminateEx (
    LPEXCLUDE       lpExclude,
    EXCLUDEACTION   ExclAction
    );

STATUS SYM_EXPORT WINAPI ThreatDefaultExcludeTerminateEx (
    LPEXCLUDE       lpExclude,
    EXCLUDEACTION   ExclAction
    );

STATUS SYM_EXPORT WINAPI ExcludeTerminateSpecifyDir (
    LPEXCLUDE       lpExclude,
    LPTSTR          lpszDir,
    BOOL            bFreeMemory
    );

STATUS SYM_EXPORT WINAPI ExcludeTerminateSpecifyFile(
    LPEXCLUDE       lpExclude,
    LPTSTR          lpszFile,
    EXCLUDEACTION   ExclAction
    );

BOOL SYM_EXPORT WINAPI ExcludeDeleteIndex (
    LPEXCLUDE       lpExclude,
    WORD            wIndex
    );

BOOL SYM_EXPORT WINAPI ExcludeIs (
    LPEXCLUDE       lpExclude,
    LPTSTR          lpFile,
    WORD            wFlags
    );

BOOL SYM_EXPORT WINAPI ExcludeIsEx (
    LPEXCLUDE       lpExclude,
    LPTSTR          lpszLFN,
    LPTSTR          lpszSFN,
    WORD            wFlags
    );

BOOL SYM_EXPORT WINAPI ExcludeIsDirEx (
    LPEXCLUDE   lpExclude,              // [in] structure used during init.
    LPTSTR      lpszLFN,                // [in] LFN item to check
    LPTSTR      lpszSFN                 // [in] exclude flags to check towards.
    );

BOOL SYM_EXPORT WINAPI ExcludeDelete (
    LPEXCLUDE       lpExclude,
    LPEXCLUDEITEM   lpExcludeItem
    );

BOOL SYM_EXPORT WINAPI ExcludeEdit (
    LPEXCLUDE       lpExclude,
    LPEXCLUDEITEM   lpFindItem,
    LPEXCLUDEITEM   lpReplaceWith
    );

STATUSEXCLUDE SYM_EXPORT WINAPI ExcludeIsValid (
    LPTSTR          lpItem,
    BOOL            bItemIsLFN
    );

#define ExcludeIsValidSFN(lp) (ExcludeIsValid(lp,FALSE))
#define ExcludeIsValidLFN(lp) (ExcludeIsValid(lp,TRUE))

BOOL SYM_EXPORT WINAPI ExcludeSFN2LFN (
    LPTSTR          lpShortName,
    LPTSTR          lpLongName,
    UINT            uSizeLongName
    );

BOOL SYM_EXPORT WINAPI ExcludeLFN2SFN (
    LPTSTR          lpLongName,
    LPTSTR          lpShortName,
    UINT            uSizeShortName
    );

WORD SYM_EXPORT WINAPI ExcludeGetCount (// Get number of excluded items.
    LPEXCLUDE       lpExclude           // [in] returned from init.
    );

BOOL SYM_EXPORT WINAPI ExcludeGet (     // Obtain an item by index number.
    LPEXCLUDE       lpExclude,          // [in] returned from init.
    WORD            wIndex,             // [in] item index number to get.
    LPTSTR          lpItemName,         // [out] name of item placed here.
    LPWORD          lpwFlags,           // [out] exclusions flags for this item
    LPBOOL          lpbSubDirs          // [out] flag to exclude sub dirs.
    );

BOOL SYM_EXPORT WINAPI ExcludeCreateCopy (// Create a copy of existing items.
    LPEXCLUDE       lpOriginal,         // [in] original list
    LPEXCLUDE       lpCopy              // [out] copy of original
    );

BOOL SYM_EXPORT WINAPI ExcludeKillCopy (// Zaps a temporary exclude handle
    LPEXCLUDE       lpDoomed            // [in] kill this
    );

BOOL SYM_EXPORT WINAPI ExcludeObjectsIdentical (
    LPEXCLUDE       lpThis,             // [in] compare this
    LPEXCLUDE       lpThat              // [in] with this.
    );

BOOL SYM_EXPORT NavExcludeCheck (
    LPEXCLUDEITEM   lpItem,
    LPTSTR          szShortPath,
    LPTSTR          szLongPath,
    WORD            wFlags
    );

/////////////////////////////////////////////////////////////////////////////
//
// The following are new API definitions used only by NAVAP.
//
// Sorry for the messy design and implementation, but if we want to get NAV
// out the door, I have to do this.
//
/////////////////////////////////////////////////////////////////////////////
#if defined(SYM_VXD) || defined(SYM_NTK)

#define excEXACT_NAME   0x00010000L
#define excASYNCHRONOUS 0x00020000L
#define excIO_UNSAFE    (excEXACT_NAME | excASYNCHRONOUS)

typedef struct tagASYNCEXCLUDEITEM
    {
    tagASYNCEXCLUDEITEM *prNextItemInList;
    DWORD               dwFlags;
    TCHAR               szFileName[SYM_MAX_PATH];
    } ASYNCEXCLUDEITEM, *PASYNCEXCLUDEITEM;

typedef struct tagEXCLUDEN
    {
    EXCLUDE           rExclude;
    TCHAR             szExcludeDBPath[SYM_MAX_PATH];
    PASYNCEXCLUDEITEM prAsyncExcludeList;
    DWORD             hIdleTimeHandle;
    PVOID             pfnIdleCallback;
    BOOL              bIdleUpdating;
    } EXCLUDEN, *PEXCLUDEN, *HEXCLUDEN;

HEXCLUDEN WINAPI ExcludeNInit (
    PTSTR pszExcludeDBPath
    );

BOOL WINAPI ExcludeNClose (
    HEXCLUDEN hExcludeN
    );

BOOL WINAPI ExcludeNIdleUpdate (
    HEXCLUDEN hExcludeN
    );

BOOL WINAPI ExcludeNSetIdleCallback (
    HEXCLUDEN hExcludeN,
    PVOID     pfnIdleCallback
    );

BOOL WINAPI ExcludeNIsFileExcluded (
    HEXCLUDEN hExcludeN,
    PTSTR     pszFileName,
    DWORD     dwFlags
    );

BOOL WINAPI ExcludeNExcludeFile (
    HEXCLUDEN hExcludeN,
    PTSTR     pszFileName,
    DWORD     dwFlags
    );

#endif // if defined(SYM_VXD)

#ifdef __cplusplus
}
#endif

#endif  // (#ifndef _EXLU_STUFF)
