// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/FILTER.H_v   1.4   17 Feb 1998 16:55:48   RStanev  $
//
// Description:
//      This file prototypes the filtering functions used by NAVAP's event
//      manager.  Some of these APIs are exported to other NAVAP modules.
//
// Contains:
//      IsObjectAFile()
//
//      IsFileNameReserved()
//
//      IsThisMoveOperation()
//
//      IsFileInListOfExtensions()
//
//      InitCleanCache()
//      DeInitCleanCache()
//      IsFileInCleanCache()
//      AddFileToCleanCache()
//      RemoveFileFromCleanCache()
//      FlushCleanCache()
//
//      FileOpenSyncInit()
//      FileOpenSyncDeInit()
//      FileOpenSyncCleanup()
//      FileOpenSyncEnter()
//      FileOpenSyncExit()
//
//      CreateOpenAfterExecEntry()
//      IsItOpenAfterExec()
//      CleanupOpenAfterExec()
//
// See Also:
//      filter.cpp, oefilter.cpp, navap.h
//
// Note:
//      Event manager module
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/FILTER.H_v  $
// 
//    Rev 1.4   17 Feb 1998 16:55:48   RStanev
// Updated copyright.
//
//    Rev 1.3   02 Jul 1997 19:03:32   RStanev
// Removed the changes from the last revision.
//
//    Rev 1.2   03 Jun 1997 09:35:14   RStanev
// Added IsThisMoveOperation().
//
//    Rev 1.1   15 May 1997 01:36:22   RStanev
// Cleaned up.
//
//    Rev 1.0   16 Mar 1997 12:55:20   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef _FILTER_H_
#define _FILTER_H_

BOOL IsObjectAFile (
    PCTSTR pszName
    );

BOOL IsFileNameReserved (
    PCTSTR pszFileSpec
    );

BOOL IsFileInListOfExtensions (
    PCTSTR pszFile,
    PCTSTR pszzExtensionList
    );

BOOL InitCleanCache (
    VOID
    );

BOOL DeInitCleanCache (
    VOID
    );

BOOL IsFileInCleanCache (
    __int64 i64FileID
    );

BOOL IsFileInCleanCache (
    LPED lpEventData
    );

BOOL AddFileToCleanCache (
    LPED lpEventData
    );

BOOL RemoveFileFromCleanCache (
    LPED lpEventData
    );

BOOL FlushCleanCache (
    VOID
    );

BOOL FileOpenSyncInit (
    VOID
    );

BOOL FileOpenSyncDeInit (
    VOID
    );

VOID FileOpenSyncCleanup (
    VOID
    );

PVOID FileOpenSyncEnter (
    LPED lpEventData
    );

VOID FileOpenSyncExit (
    PVOID pvSyncObject
    );

#if defined(SYM_VXD)

VOID CreateOpenAfterExecEntry (
    PEDEXTRA prEDExtra
    );

BOOL IsItOpenAfterExec (
    PEDEXTRA prEDExtra
    );

VOID CleanupOpenAfterExec (
    BOOL bCleanupAll,
    PTCB hThread,
    HVM  hVM
    );

#else

#define CreateOpenAfterExecEntry(x) ((void)0)
#define IsItOpenAfterExec(x)        ((BOOL)FALSE)
#define CleanupOpenAfterExec(x,y,z) ((void)0)

#endif

#endif // #ifndef _FILTER_H_
