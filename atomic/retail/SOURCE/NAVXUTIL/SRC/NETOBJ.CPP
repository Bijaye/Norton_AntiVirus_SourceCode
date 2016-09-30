// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navxutil/VCS/netobj.cpv   1.0   06 Feb 1997 21:05:32   RFULLER  $
//
// Description:
//      Functions to read and write the network users to notify to the
//      data file.
//
// Contains:
//      GetNetUserBin()
//      LoadNetUserList()
//      LoadNetUserListSpecifyDir()
//      UpdateNetUserList()
//      UpdateNetUserListSpecifyDir()
//
// See also:
//************************************************************************
// $Log:   S:/navxutil/VCS/netobj.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:05:32   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:56:46   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:48   BARRY
// Initial revision.
// 
//    Rev 1.1   16 Dec 1994 13:04:22   MFALLEN
// Dos build was complaining.
// 
//    Rev 1.0   16 Dec 1994 10:36:54   MFALLEN
// Initial revision.
// 
//    Rev 1.9   15 Dec 1994 20:13:58   MFALLEN
// From NIRVANA
// 
//    Rev 1.8   08 Oct 1993 10:33:40   MFALLEN
// Removed goto
// 
//    Rev 1.7   15 Sep 1993 16:14:32   MFALLEN
// Only the first network object was being ansi-ed when reading file from disk.
// 
//    Rev 1.6   15 Sep 1993 00:41:44   REESE
// Added LoadNetUserListSpecifyDir().
// 
//    Rev 1.5   09 Sep 1993 18:12:34   BARRY
// Removed some debug stuff
// 
//    Rev 1.4   09 Sep 1993 18:02:06   BARRY
// Renamed NetUserInit and NetUserTerminate to LoadNetUserList and
// UpdateNetUserList to avoid conflict with SYMNET function names.
// 
//    Rev 1.3   26 Aug 1993 22:35:24   DALLEE
// Removed compiler warnings in DOS.  Unreferenced vars.
// 
//    Rev 1.2   23 Aug 1993 00:39:34   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
// 
//    Rev 1.1   17 Aug 1993 18:58:22   MFALLEN
// Somehow things got screwy, We're now converting every single netobject
// to OEM before saving them to a file!!!
// 
//    Rev 1.0   12 Aug 1993 11:56:06   MFALLEN
// Initial Revision
// 
//    Rev 1.13   25 Jul 1993 23:44:40   BRAD
// Changed data file extension from BIN to DAT.
// 
//    Rev 1.12   22 Jul 1993 19:22:08   DALLEE
// #ifdef SYM_WIN around AnsiToOem and OemToAnsi.
// 
//    Rev 1.11   22 Jul 1993 14:09:40   MFALLEN
// AnsoToOem() OemToAnsi()
// 
//    Rev 1.10   20 Jul 1993 01:32:10   DALLEE
// Removed compiler warnings in DOS about using local hInstance without
// initializing first.
// 
//    Rev 1.9   27 May 1993 19:50:44   BRAD
// Wasn't properly handling condition when no NETUSER.DAT file
// existed.
// 
//    Rev 1.8   16 May 1993 23:31:36   BRAD
// 
//    Rev 1.7   16 May 1993 23:29:00   BRAD
// Added NetUserTerminateSpecifyDir().
// 
//    Rev 1.6   10 May 1993 22:05:42   BRAD
// Provide dummy hInstance for DOS.
// 
//    Rev 1.5   08 May 1993 17:04:48   BRAD
// New Net User routines.
// 
//    Rev 1.4   05 May 1993 11:47:04   DALLEE
// 
//    Rev 1.3   02 May 1993 12:07:34   MFALLEN
// Updated network object load/save
// 
// 
//    Rev 1.2   30 Apr 1993 13:00:00   DALLEE
// Added matching MemAllocPtr inline function for DOS.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "symnet.h"
#include "navutil.h"
#include "netobj.h"

MODULE_NAME;

LPSTR LOCAL PASCAL GetNetUserBinFilename(LPSTR lpszFilename)
{
    return (STRCPY(lpszFilename, "NETUSER.DAT"));
}


VOID LOCAL PASCAL GetNetUserBin(HINSTANCE hInstance, LPSTR lpszFile, UINT uSize)
{
    auto        char    szFilename[SYM_MAX_PATH+1];


    GetStartDir (hInstance, lpszFile, uSize);
    NameAppendFile(lpszFile, GetNetUserBinFilename(szFilename));
}


STATUS SYM_EXPORT WINAPI LoadNetUserList(LPNETUSER lpNetUser)
{
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;
#endif
    auto        char            szDir[SYM_MAX_PATH + 1];

    hInstance;                          // Kill DOS 'uninitialized' warning.

    GetStartDir (hInstance, szDir, sizeof(szDir)-1);
    return( LoadNetUserListSpecifyDir(lpNetUser, szDir) );
}

STATUS SYM_EXPORT WINAPI LoadNetUserListSpecifyDir ( LPNETUSER lpNetUser, LPSTR lpszDir )
{
    auto        HGLOBAL         hNetUserItem = NULL;
    auto        LPNETUSERITEM   lpNetUserItem = NULL;
    auto        HFILE           hFile = HFILE_ERROR;
    auto        DWORD           dwSize;
    auto        UINT            uEntries;
    auto        UINT            uSizeToAllocate;
    auto        STATUS          uStatus = ERR;
    auto        char            szPath[SYM_MAX_PATH+1];
    auto        char            szFile[SYM_MAX_PATH+1];

                                        // Clear the structure
    MEMSET(lpNetUser, 0, sizeof(NETUSER));

                                        // Open the existing file
    STRCPY(szPath, lpszDir);
    NameAppendFile(szPath, GetNetUserBinFilename(szFile));
    if ((hFile = FileOpen(szPath,OF_READ)) != HFILE_ERROR)
        {
                                        // Get size of file and # of entries
        dwSize = FileLength(hFile);
        uEntries = (UINT) (dwSize / sizeof(NETUSERITEM));
        }
    else
        uEntries = 0;                   // No file created yet

                                        // -----------------------------------
                                        // Allocate memory to hold the current
                                        // number of exclusions
                                        // -----------------------------------

    uSizeToAllocate = uEntries * sizeof(NETUSERITEM);

                                        // -----------------------------------
                                        // NOTE: We allocate 1 extra entry.
                                        // This insures we work correctly
                                        // when there are no netuser items.
                                        // -----------------------------------
    hNetUserItem = MemAlloc(GHND, uSizeToAllocate + sizeof(NETUSERITEM));

    if ( hNetUserItem != NULL )
        {
        lpNetUserItem = (LPNETUSERITEM) MemLock(hNetUserItem);

        if ( lpNetUserItem != NULL )
            {
                                        // -----------------------------------
                                        // Read in the exclusions in a block
                                        // -----------------------------------
            if ( (uEntries == 0) ||
                (FileRead(hFile, lpNetUserItem, uSizeToAllocate) == uSizeToAllocate) )
                {
                lpNetUser->hNetUserItem   = hNetUserItem;
                lpNetUser->lpNetUserItem  = lpNetUserItem;
                lpNetUser->uEntries       = uEntries;

#ifdef SYM_WIN
                for (uEntries = 0; uEntries < lpNetUser->uEntries; uEntries++)
                    {
                    OemToAnsi (lpNetUser->lpNetUserItem[uEntries].szServer,
                               lpNetUser->lpNetUserItem[uEntries].szServer);
                    OemToAnsi (lpNetUser->lpNetUserItem[uEntries].szObject,
                               lpNetUser->lpNetUserItem[uEntries].szObject);
                    }
#endif
                uStatus = NOERR;
                }
            }
        }

    if ( uStatus == ERR )
        {
        if ( lpNetUserItem )
            MemRelease(hNetUserItem);

        if ( hNetUserItem )
            MemFree(hNetUserItem);
        }

    if ( hFile != HFILE_ERROR )
        FileClose (hFile);

    return(uStatus);
}


STATUS SYM_EXPORT WINAPI UpdateNetUserList (LPNETUSER lpNetUser, BOOL bFreeMemory)
{
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;
#endif
    auto        char            szDir[SYM_MAX_PATH + 1];

    hInstance;                          // Kill DOS 'uninitialized' warning.

    GetStartDir (hInstance, szDir, sizeof(szDir)-1);
    return( UpdateNetUserListSpecifyDir(lpNetUser, szDir, bFreeMemory) );
}


//************************************************************************
// UpdateNetUserListSpecifyDir()
//
// This routine writes the NETUSERITEMs in memory and saves them to NETUSER.DAT
//
// Parameters:
//      LPNETUSER   lpNetUser           Net users in memory.
//      LPSTR       lpszDir             Directory in which to write NETUSER.DAT
//      BOOL        bFreeMemory         Free memory used by NETUSER items.
//                                          Call with TRUE when done.
// Returns:
//      NOERR                           OK
//      ERR                             File error.
//************************************************************************
// 8/26/93 DALLEE, removed comiler warnings - unused vars in DOS compile.
//************************************************************************

STATUS SYM_EXPORT WINAPI UpdateNetUserListSpecifyDir (LPNETUSER lpNetUser, 
                                                 LPSTR lpszDir,
                                                 BOOL bFreeMemory)
{
    auto        HFILE           hFile;
    auto        STATUS          uStatus = NOERR;
    auto        UINT            uSize;
    auto        char            szPath[SYM_MAX_PATH+1];
    auto        char            szFile[SYM_MAX_PATH+1];
#ifdef SYM_WIN
    auto        NETUSERITEM     NetItem;
    auto        UINT            i;
#endif

    STRCPY(szPath, lpszDir);
    NameAppendFile(szPath, GetNetUserBinFilename(szFile));

    FileDelete(szPath);                 // Delete the existing file

    if ( lpNetUser->lpNetUserItem && lpNetUser->uEntries )
        {
        if ((hFile = FileCreate(szPath,FA_NORMAL)) != HFILE_ERROR)
            {
            uSize = sizeof(NETUSERITEM) * lpNetUser->uEntries;

#ifdef SYM_WIN
            for (i = 0; i < lpNetUser->uEntries; i++)
                {
                MEMCPY (&NetItem, &lpNetUser->lpNetUserItem[i],
                        sizeof(NetItem));

                AnsiToOem (NetItem.szServer, NetItem.szServer);
                AnsiToOem (NetItem.szObject, NetItem.szObject);
            
                if (FileWrite(hFile, &NetItem, sizeof(NetItem)) !=
                        sizeof(NetItem))
                    {
                    uStatus == ERR;
                    break;
                    }
                }
#else
            if ( FileWrite(hFile, lpNetUser->lpNetUserItem, uSize) != uSize )
                {
                uStatus == ERR;
                }
#endif
                                
            FileClose(hFile);
            }
        else
            {
            uStatus == ERR;
            }

        if ( bFreeMemory )
            {
            MemRelease(lpNetUser->hNetUserItem);
            MemFree(lpNetUser->hNetUserItem);
            lpNetUser->lpNetUserItem = NULL;
            lpNetUser->hNetUserItem = NULL;
            }
        }

    return (uStatus);
}
