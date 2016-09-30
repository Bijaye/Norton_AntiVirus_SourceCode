// Copyright 1998 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/ATMIGRAT/VCS/shelllnk.h_v   1.1   19 Nov 1998 13:14:32   MBROWN  $
//
// Description:
//    Shortcut detection/creation functions pulled from Symutil
//
// Contains:
//
//***************************************************************************
// $Log:   S:/ATMIGRAT/VCS/shelllnk.h_v  $
// 
//    Rev 1.1   19 Nov 1998 13:14:32   MBROWN
// Changed method for getting shell folders, deleting NPS links, added entry point for unloading NAV
// 
//    Rev 1.0   28 Sep 1998 11:42:44   MBROWN
// Initial revision.
// 
// 
//***************************************************************************
#ifndef _SHELLLNK_H
#define _SHELLLNK_H


BOOL SYM_EXPORT WINAPI MyShellLinkCreate(
    LPCSTR lpFolder,                    // Folder to append to 'special' folder.
    LPCSTR lpCmdLine,                   // Required
    LPCSTR lpArguments,                 // Optional
    LPCSTR lpName,                      // Optional
    LPCSTR lpIconPath,                  // Optional
    int nIconIndex,                     // Optional
    int nShowCommand,                   // Required
    LPCSTR lpDefDir,                    // Optional
    LPSTR lpSpecialFolder);             // Required


BOOL SYM_EXPORT WINAPI MyShellLinkExists (
    LPCSTR lpFolder,                    // Folder to append to 'special' folder.
    int    nFolder,                     // Passed to SHGetSpecialFolderLocation
    LPCSTR lpCmdLine,                   // Required
    LPCSTR lpName);                     // Optional

BOOL SYM_EXPORT WINAPI MyShellLinkBuildPath (
    LPCSTR lpFolder,                    // Folder to append to 'special' folder.
    int    nFolder,                     // Passed to SHGetSpecialFolderLocation
    LPCSTR lpCmdLine,
    LPCSTR lpName,
    LPSTR  lpOutput);

BOOL SYM_EXPORT WINAPI DirExists(LPCTSTR lpFileName);

#endif _SHELLLNK_H
