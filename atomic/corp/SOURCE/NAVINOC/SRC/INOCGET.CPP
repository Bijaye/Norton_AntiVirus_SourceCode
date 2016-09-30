// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocget.cpv   1.1   20 May 1997 17:39:10   BILL  $
//
// Description:
//      This file contains the routines to get the number of entries
//      in an inoculation database, and return an entry based on its
//      position in the database.
//
// Contains:
//      InocGetNumEntries()
//      InocGetEntry()
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocget.cpv  $
// 
//    Rev 1.1   20 May 1997 17:39:10   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
// 
//    Rev 1.0   06 Feb 1997 20:58:08   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:53:08   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:14   BARRY
// Initial revision.
// 
//    Rev 1.8   23 Mar 1995 14:48:50   SZIADEH
// added UNC support.
// 
//    Rev 1.7   03 Feb 1995 16:10:46   BRAD
// Got working and added Long File name
// 
//    Rev 1.6   16 Jan 1995 16:05:36   ELEAR
// Removed  || defined(SYM_DOS)
// 
//    Rev 1.5   28 Nov 1994 20:14:08   MFALLEN
// Now also works for DOS platform.
// 
//    Rev 1.4   28 Nov 1994 18:55:56   MFALLEN
// CERTLIB is alive for W16
// 
//    Rev 1.3   28 Nov 1994 14:14:16   MFALLEN
// Compiles fine for W16 and hopefully also for W32
// 
//    Rev 1.2   12 May 1994 10:44:26   MFALLEN
// cts4.h 2 ctsn.h
// 
//    Rev 1.1   25 Apr 1994 16:00:50   MFALLEN
// Just trying to make code compile. It's not working yet.
// 
//    Rev 1.0   19 Apr 1994 19:19:32   MFALLEN
// Initial revision.
// 
//    Rev 1.0   25 May 1993 01:42:14   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "ctsn.h"
#include "options.h"
#include "inoc.h"


//************************************************************************
// InocGetNumEntries()
//
// This routine returns the number of entries in an inoculation database,
// including deleted/blank entries.
//
// Parameters:
//      LPSTR   lpDriveName        Drive or UNC name of open inoculation database,
//
//
// Returns:
//      DWORD   Number of entries.
//              0 Either no entries, or could not get INOCULATE structure.
//************************************************************************
// 5/24/93 DALLEE, Function created.
//************************************************************************

DWORD SYM_EXPORT WINAPI InocGetNumEntries (LPSTR lpDriveName, BOOL bLongFile)
{
#ifdef _ALPHA_
	return 0;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return 0;

    LPINOCULATE     lpInoc;

    if ((lpInoc = GetInocStruct(lpDriveName, bLongFile)) == NULL)
        {
        return (0);
        }
    return (DBC4_GET_NUM_ENTRIES(lpInoc->lpHeader));
#endif    
} // End InocGetNumEntries()


//************************************************************************
// InocGetEntry()
//
// This routine retrieves the specified entry number from the inoculation
// database.
//
// Parameters:
//      LPSTR   lpDriveName           Drive or UNC name of open inoculation database.
//      DWORD   dwNum                   Number of entry to retrieve
//                                      dwNum = 1 for first entry.
//      LPVOID  lpEntryBuffer           [out] Buffer to place entry.
//
// Returns:
//      TRUE                            Retrieved entry.
//      FALSE                           Didn't get it.
//************************************************************************
// 4/24/93 DALLEE, Function created.
//************************************************************************

BOOL SYM_EXPORT WINAPI  InocGetEntry (
                        LPSTR     lpDriveName,
                        DWORD     dwNum,
                        LPVOID    lpEntryBuffer,
                        BOOL      bLongFile)
{
#ifdef _ALPHA_
	return FALSE;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return FALSE;

    LPINOCULATE     lpInoc;

    if ((lpInoc = GetInocStruct(lpDriveName, bLongFile)) == NULL)
        {
        return (0);
        }
    return (DBC4_GET_ENTRY(lpInoc->lpHeader, dwNum, lpEntryBuffer));
#endif    
} // End InocGetEntry()
