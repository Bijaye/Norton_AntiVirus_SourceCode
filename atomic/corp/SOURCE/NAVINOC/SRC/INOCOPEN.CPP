// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocopen.cpv   1.1   20 May 1997 17:42:40   BILL  $
//
// Description:
//      Routines to open NAV 3.0's inoculation database, and the default
//      hashing, quick lookup, and long lookup functions.
//
// Contains:
//      InocOpen()
//      InocComp()
//      InocHash()
//      InocQuick()
//
// See Also:
//      INOCLOSE.C for InocClose()
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocopen.cpv  $
// 
//    Rev 1.1   20 May 1997 17:42:40   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
// 
//    Rev 1.0   06 Feb 1997 20:58:10   RFULLER
// Initial revision
// 
//    Rev 1.1   22 Mar 1996 10:09:02   jworden
// DBCS Enable
// 
//    Rev 1.0   30 Jan 1996 15:53:10   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:14   BARRY
// Initial revision.
// 
//    Rev 1.10   19 Jun 1995 15:02:02   KEITH
// Added SIZE_INOCULATE_WORKMEM and SIZE_INOCULATE_WORKMEM2 so that VNAVAPD 
// and NAVINOC could use the same sizes for the INOCULATE struct.
// 
//    Rev 1.9   23 Mar 1995 14:48:52   SZIADEH
// added UNC support.
// 
//    Rev 1.8   08 Feb 1995 19:28:22   BRAD
// During conversion from CERTLIBN, STRCMP() done incorrectly
// 
//    Rev 1.7   03 Feb 1995 16:10:44   BRAD
// Got working and added Long File name
// 
//    Rev 1.6   31 Jan 1995 13:13:40   BRAD
// Cleaned up for Luigi
// 
//    Rev 1.5   28 Nov 1994 20:13:58   MFALLEN
// Now also works for DOS platform.
// 
//    Rev 1.4   28 Nov 1994 18:56:36   MFALLEN
// CERTLIB is alive for W16
// 
//    Rev 1.3   28 Nov 1994 14:14:20   MFALLEN
// Compiles fine for W16 and hopefully also for W32
// 
//    Rev 1.2   12 May 1994 10:44:24   MFALLEN
// cts4.h 2 ctsn.h
// 
//    Rev 1.1   25 Apr 1994 16:01:14   MFALLEN
// Just trying to make code compile. It's not working yet.
// 
//    Rev 1.0   19 Apr 1994 19:19:36   MFALLEN
// Initial revision.
// 
//    Rev 1.22   20 Mar 1994 12:29:48   KEITH
// Bankers Trust - include TSR.H so that TSRIsLoaded() et. al. is defined for DOS
// 
// 
//    Rev 1.21   20 Mar 1994 11:30:18   KEITH
// Bankers Trust - Allow access to inoc file when evDeny is active
// 
//    Rev 1.20   18 Mar 1994 00:15:58   DALLEE
// Changed #ifdef BANKERS to be for Windows only in most cases.
// DOS needs updated structures, but not the new functionality.
// 
//    Rev 1.19   23 Feb 1994 22:49:24   DALLEE
// Support for save/check file registration.
// 
//    Rev 1.18   24 Aug 1993 13:37:08   BARRY
// Removed Inoc.lpQuick.  Locks on the fly now so DOS VMM can cope better
// 
//    Rev 1.17   23 Aug 1993 21:19:18   BARRY
// Fixed memory leak and now can cope without hQuick if necessary
// 
//    Rev 1.16   23 Aug 1993 00:30:02   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
// 
//    Rev 1.15   04 Aug 1993 15:04:22   DALLEE
// Open with index of size SIZE_FLOPPY_INDEX or SIZE_INDEX depending
// on drive type.  (Small, 512 entry, index on floppies...)
// 
//    Rev 1.14   30 Jul 1993 12:57:06   CGUC
// Adjusted FIL4_PROTECT 
// 
//    Rev 1.13   07 Jul 1993 23:15:12   CJKUO
// Add RememberInocFloppy and use it
// 
//    Rev 1.12   23 Jun 1993 15:02:22   CJKUO
// Change logic of opening files
// 
//    Rev 1.11   16 Jun 1993 18:13:00   CJKUO
// Call inoc db routines with bReadWrite flag
// 
//    Rev 1.10   25 May 1993 17:34:56   DALLEE
// 
//    Rev 1.9   12 May 1993 18:50:50   CJKUO
// Revise prototypes to LPFILEPROTECT
// Make InocHash and InocComp skip drive letters
// 
//    Rev 1.8   10 May 1993 16:45:34   CJKUO
// Allow any name for inocDB
// 
//    Rev 1.7   05 May 1993 20:22:06   CJKUO
// Make calls to routines in inocdb.c to track which INOCULATE structure
// is active and thus available for use.
// 
//    Rev 1.6   30 Apr 1993 13:20:32   MFALLEN
// 
//    Rev 1.5   21 Apr 1993 14:26:24   MFALLEN
// 
//    Rev 1.4   19 Apr 1993 11:55:58   DALLEE
// Quick look up should never return 0.  This is the empty entry value.
// Fudge the value to 1 in this case.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "options.h"
#include "ctl3d.h"                  // Microsoft 3D library
#include "cmbstr.h"
#include "inoc.h"

#ifdef BANKERS
#include "tsr.h"
#include "tsrcomm.h"
#endif

MODULE_NAME;

//************************************************************************
// InocOpen()
//
// Call this routine to open the inoculation database.  On error, or when
// finished with the database, InocClose() must be called.
//
// Allocates the following buffers:
//      REQUIRED        hMem/lpHeader           Inoc header info
//      OPTIONAL 32K    hQuick                  Quick lookup for inoc CRC data
//      REQUIRED  1K    hWorkMem/lpWorkMem      Temporary buffer used throughout
//      REQUIRED        hWorkMem2/lpWorkMem2    Temporary buffer used throughout
//
// If any of the required buffers can't be allocated, this call cleans up and
// returns failure.
//
// Parameters:
//      LPSTR   lpInocDB                Inoculation file (full path)
//      BOOL    bReadWrite              Open Read/Write?
//
// Returns:
//      TRUE                            Database is open, buffer ready.
//      FALSE                           Error occured.
//************************************************************************
// 3/\93 MFALLEN, Function created.
// 8/23/93 BarryG   Improved memory allocation and error handling
//************************************************************************

STATIC  BOOL    bStaticInocFloppy;

BOOL SYM_EXPORT WINAPI InocOpen (LPSTR lpInocDB, BOOL bReadWrite)
{
#ifdef _ALPHA_
	return FALSE;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return FALSE;
    	
    extern   TCHAR          SZ_INOCL_FILE[];
    extern   TCHAR          SZ_INOC_FILE [];
    auto     UINT           uStatus = 0;
    auto     LPINOCULATE    Inoc;
    auto     UINT           uSizeIndex;
    auto     BOOL           bIsFloppyDrive;
    auto     LPSTR          lpQuick;
    auto     UINT           uEntrySize;
    auto     BOOL           bLongFile;
    auto     TCHAR          szBuffer[SYM_MAX_PATH+1];



    NameReturnFile(lpInocDB, szBuffer);
    bLongFile = (BOOL)(STRICMP(szBuffer, SZ_INOCL_FILE) == 0);
    uEntrySize = (bLongFile) ? sizeof(FILE_PROTECTL) : sizeof(FILE_PROTECT);
    bIsFloppyDrive = DiskIsRemovable(lpInocDB[0]);
    if ((FALSE == bStaticInocFloppy) && bIsFloppyDrive)
        {
        return(FALSE);
        }

    if (NULL == (Inoc = GetUnusedInocStruct(lpInocDB, bReadWrite, bLongFile)))
        {
        return(FALSE);
        }
              
    // Try to get all required buffers
    if ( (Inoc->hMem = MemAlloc(GHND, sizeof(DBC4_HEADER))) != 0 )
        Inoc->lpHeader = (DBC4_HEADER FAR *) MemLock ( Inoc->hMem ) ;
    if ( (Inoc->hWorkMem = MemAlloc ( GHND, SIZE_INOCULATE_WORKMEM )) != 0 )
        Inoc->lpWorkMem = (LPSTR)MemLock ( Inoc->hWorkMem ) ;
    if ( (Inoc->hWorkMem2 = MemAlloc ( GHND, SIZE_INOCULATE_WORKMEM2 )) != 0 )
        Inoc->lpWorkMem2 = (LPSTR)MemLock ( Inoc->hWorkMem2 ) ;

    // If we got all required buffers, proceed
    if ( Inoc->hMem != 0 && Inoc->hWorkMem != 0 && Inoc->hWorkMem2 != 0 )
        {
        uSizeIndex = (bIsFloppyDrive ? SIZE_FLOPPY_INDEX : SIZE_INDEX);

        if ( !bReadWrite )
            {
            if (!DBC4_OPEN (lpInocDB, uSizeIndex, uEntrySize,
                       (FARPROC)InocComp, (FARPROC)InocHash,
                       (FARPROC)InocQuick, Inoc->lpHeader, FALSE))
                {
                                        // See if the other DB (long/short)
                                        // is there, and try to create this one
                                        // if it is.  Scanners believe there's
                                        // no long or short info for a drive
                                        // if the first open fails.  -DALLEE
                STRCPY(szBuffer, lpInocDB);
                NameStripFile(szBuffer);
                NameAppendFile(szBuffer, bLongFile?SZ_INOC_FILE:SZ_INOCL_FILE);

                if ( DBC4_OPEN(szBuffer, uSizeIndex,
                               bLongFile?sizeof(FILE_PROTECT):sizeof(FILE_PROTECTL),
                               (FARPROC)InocComp, (FARPROC)InocHash,
                               (FARPROC)InocQuick, Inoc->lpHeader, FALSE) )
                    {
                    DBC4_CLOSE(Inoc->lpHeader);

                                        // Set true to try to create this
                    bReadWrite = TRUE;  // database.
                    }
                else
                    {
                    uStatus = ERR_INOC_OPEN;
                    }
                }
            }
        if ( bReadWrite )
            {
            if (!DBC4_OPEN_CREATE (lpInocDB, uSizeIndex, uEntrySize,
                                  (FARPROC)InocComp, (FARPROC)InocHash,
                                  (FARPROC)InocQuick, Inoc->lpHeader))
                uStatus = ERR_INOC_OPEN;
            }

        if (!uStatus)
            {
            // Failing to Alloc hQuick will just slow down the Inoc but isn't fatal.
            Inoc->hQuick = MemAlloc(GHND, (Inoc->lpHeader->IndexSize * 2) + 16);

            if (Inoc->hQuick)
                {
                lpQuick = (LPSTR)MemLock(Inoc->hQuick);

                // &? BarryG: So, if we don't require hQuick, why is this fatal?
                if (!DBC4_LOAD_QUICK (Inoc->lpHeader, (WORD FAR *)lpQuick))
                    uStatus = ERR_INOC_OPEN;
                }

                MemRelease ( Inoc -> hQuick ) ;
            }
        }
    else
        {
        uStatus = ERR_INOC_OPEN ;       // Actually, we're out of memory.
        }

    if (uStatus)
        {
        if ( Inoc->hMem )
            {
            MemRelease ( Inoc->hMem ) ;
            MemFree   ( Inoc->hMem ) ;
            }
        if ( Inoc->hQuick )
            {
            MemFree   ( Inoc->hQuick ) ;
            }
        if ( Inoc->hWorkMem )
            {
            MemRelease ( Inoc->hWorkMem ) ;
            MemFree   ( Inoc->hWorkMem ) ;
            }
        if ( Inoc->hWorkMem2 )
            {
            MemRelease ( Inoc->hWorkMem2 ) ;
            MemFree   ( Inoc->hWorkMem2 ) ;
            }


        Inoc->lpHeader   = NULL ;
        Inoc->lpWorkMem  = NULL ;
        Inoc->lpWorkMem2 = NULL ;
        Inoc->hMem = Inoc->hQuick  = Inoc->hWorkMem  = Inoc->hWorkMem2  = 0 ;

        ReleaseInocStruct(Inoc);        // This seems somewhat redundant
                                        // but ReleaseInocStruct() only frees
                                        // memory when it's in-use or open.
                                        // Since we can't guarantee that here,
                                        // We are responsible for most of the cleanup.
        }

    return ((uStatus) ? FALSE : TRUE);
#endif
}

VOID SYM_EXPORT WINAPI RememberInocFloppy (BOOL bInocFloppy)
{
#ifdef _ALPHA_
	return;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    bStaticInocFloppy = bInocFloppy;
#endif    
}

//************************************************************************
// InocComp()
//
// This is the slow but sure comparison function to check whether or not
// a file is in the database.
//
// Parameters:
//      LPSTR   lpStr1                  Pointer to first entry.
//      LPSTR   lpStr2                  Pointer to second entry.
//
// Returns:
//      0                               Entries match.
//      Non-zero                        No match.
//************************************************************************
// 3/\93 MFALLEN, Function created.
//************************************************************************

UINT WINAPI InocComp (LPFILE_PROTECT lpEntry1,LPFILE_PROTECT lpEntry2)
{
#ifdef _ALPHA_
	return 0;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return 0;

   LPSTR    lpStr1 = lpEntry1->Filename;
   LPSTR    lpStr2 = lpEntry2->Filename;

   SYM_ASSERT (lpStr1 );
   SYM_ASSERT (lpStr2 );

   if (lpStr1[1] == ':')
       {
       lpStr1+=2;
       }
   else if (IsUNCName ( lpStr1 ) )
       {
       lpStr1 = UNC_SkipServerVolumeName ( lpStr1 );
       }

   if (lpStr2[1] == ':')
       {
       lpStr2+=2;
       }
   else if (IsUNCName ( lpStr2 ) )
       {
       lpStr2 = UNC_SkipServerVolumeName ( lpStr2 );
       }

   return ((STRCMP(lpStr1, lpStr2) == 0) ? 0 : 1);
#endif   
}


//************************************************************************
// InocHash()
//
// This routine is used to determine the hash value of an entry.
//
// Parameters:
//      LPVOID  lpVoid                  Pointer to entry.
//
// Returns:
//      UINT                            Hash value.
//************************************************************************
// 3/\93 MFALLEN, Function created.
//************************************************************************

UINT WINAPI InocHash (LPFILE_PROTECT lpEntry)
{
#ifdef _ALPHA_
	return 0;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return 0;

   LPSTR    lpStr = lpEntry->Filename;

   SYM_ASSERT ( lpStr );

   if (lpStr[1] == ':')
       {
       lpStr +=2;
       }
   else if (IsUNCName ( lpStr ) )
       {
       lpStr = UNC_SkipServerVolumeName ( lpStr );
       }
   return (MEM4_CRC(0, lpStr, STRLEN(lpStr )));
#endif   
}


//************************************************************************
// InocQuick()
//
// This routine returns the database quick look up value.
//
// Parameters:
//      LPFILE_PROTECT lpProt           Pointer to entry
//
// Returns:
//      UINT                            Quick look up value.
//************************************************************************
// 3/\93 MFALLEN, Function created.
// 4/17/93 DALLEE, Do not return 0!  This is the code for empty entry in
//              the quick look up table.  Fudge it to 1.
//************************************************************************

UINT WINAPI InocQuick(LPFILE_PROTECT lpProt)
{
#if defined(BANKERS) && defined(SYM_WIN)
    if (lpProt->wReserved1 & bitFILE_PROTECT_FULLCRC)
        {
        return (lpProt->wReserved2);
        }
    else
#endif  // BANKERS && SYM_WIN
    return (lpProt->StartCRC ? lpProt->StartCRC : 1);
}
