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

#ifndef _EXCLU_STUFF
#define _EXCLU_STUFF

#include "ctsn.h"
#include "avexcll.h"

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

#include <pshpack1.h>

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

#include <poppack.h>

#ifdef __cplusplus
extern "C" {
#endif

UINT SYM_EXPORT WINAPI ExcludeAddDirect (
    LPEXCLUDE       lpExclude,
    LPEXCLUDEITEM   lpExcludeItem,
    EXCLUDEACTION   ExcludeAddFlag
    );

STATUS SYM_EXPORT WINAPI ExcludeInitSpecifyContents (
    LPEXCLUDE lpExclude,
    DWORD     dwExcludeItems,
    PVOID     pvExcludeItems,
    DWORD     dwExcludeLItems,
    PVOID     pvExcludeLItems
    );

STATUS SYM_EXPORT WINAPI ExcludeTerminate (
    LPEXCLUDE       lpExclude
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
    DWORD dwExcludeItems,
    PVOID pvExcludeItems,
    DWORD dwExcludeLItems,
    PVOID pvExcludeLItems
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
