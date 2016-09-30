// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocrep.cpv   1.3   03 Mar 1998 16:15:50   MKEATIN  $
//
// Description:
//      Function to repair an inoccuulated file.
//
// Contains:
//      InocRepair()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocrep.cpv  $
// 
//    Rev 1.3   03 Mar 1998 16:15:50   MKEATIN
// Initializee bLongFile to FALSE.
// 
//    Rev 1.2   21 May 1997 08:46:38   BGERHAR
// _X86_ not defined for DVX platform. Changed to ifndef _ALPHA_ for now.
// 
//    Rev 1.1   20 May 1997 17:42:54   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
// 
//    Rev 1.0   06 Feb 1997 20:58:06   RFULLER
// Initial revision
// 
//    Rev 1.5   10 Oct 1996 15:02:58   JBRENNA
// Change NAnsiToOem() calls to NavAnsiToOemUpper() calls. Note that
// NAnsiToOem() upper cases all characters passed to it. NavAnsiToOemUpper()
// only upper cases non-DBCS characters. This prevents problems when you
// have mixed case DBCS English filenames.
// 
//    Rev 1.4   07 Aug 1996 14:03:02   YVANG
// Ported changes from LuigiPlus.
// 
//    Rev 1.3   27 Jun 1996 10:14:40   GWESTER
// Ported in fixes and changes from LuigiPlus.
// 
//    Rev 1.2   26 Jun 1996 17:53:14   GWESTER
// Ported in changes and fixes from LuigiPlus.
// 
//    Rev 1.1   14 Feb 1996 17:08:12   JWORDEN
// Process internal file names in OEM mode to allow for the innoculation
// of file names that include certain non-ANSI translatable characters
// 
//    Rev 1.0   30 Jan 1996 15:53:06   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:10   BARRY
// Initial revision.
// 
//    Rev 1.12   06 Jun 1995 17:56:30   KEITH
// Oem the short filename too
// 
//    Rev 1.11   12 Apr 1995 15:38:20   MFALLEN
// Was not working correctly with very long LFNs. Buffer sizes were increased
// and dynamically allocated so the stack is not exhausted. This should be done
// when possible, and watch those multiple returns because functions clean
// up before exiting.
// 
//    Rev 1.10   23 Mar 1995 14:48:52   SZIADEH
// added UNC support.
// 
//    Rev 1.9   03 Feb 1995 16:10:42   BRAD
// Got working and added Long File name
// 
//    Rev 1.8   30 Jan 1995 19:37:04   BRAD
// Added another param to FIL4_PROTECT
// 
//    Rev 1.7   28 Nov 1994 20:14:00   MFALLEN
// Now also works for DOS platform.
// 
//    Rev 1.6   28 Nov 1994 18:56:46   MFALLEN
// CERTLIB is alive for W16
// 
//    Rev 1.5   28 Nov 1994 14:13:50   MFALLEN
// Compiles fine for W16 and hopefully also for W32
// 
//    Rev 1.4   14 Jul 1994 12:35:10   MFALLEN
// 
//    Rev 1.3   16 May 1994 18:31:56   MFALLEN
// Removed old log junk.
// 
//    Rev 1.2   12 May 1994 10:44:18   MFALLEN
// cts4.h 2 ctsn.h
// 
//    Rev 1.1   25 Apr 1994 16:00:54   MFALLEN
// Just trying to make code compile. It's not working yet.
// 
//    Rev 1.0   19 Apr 1994 19:19:46   MFALLEN
// Initial revision.
// 
//    Rev 1.6   11 Oct 1993 18:14:34   MFALLEN
// DBC4_SEARCH is now using FILE_PROTECT structure instead of filename.
// 
//    Rev 1.5   30 Jul 1993 12:57:18   CGUC
// Adjusted FIL4_PROTECT 
// 
//    Rev 1.4   19 May 1993 20:45:52   BRAD
// Added call to GetInocStructGivenFile().
// 
//    Rev 1.3   14 May 1993 15:30:38   CJKUO
// Strip drive letter off filename and set drive
// 
//    Rev 1.2   05 May 1993 20:22:08   CJKUO
// Make calls to routines in inocdb.c to track which INOCULATE structure
// is active and thus available for use.
// 
//    Rev 1.1   30 Apr 1993 13:20:38   MFALLEN
// 
//    Rev 1.0   21 Apr 1993 12:57:42   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "ctl3d.h"                  // Microsoft 3D library
#include "options.h"
#include "inoc.h"
#include "file.h"
#include "disk.h"
#include "navutil.h"


// --------------------------------------------------------------------------
//                  LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

#ifndef _ALPHA_
BOOL LOCAL _InocRepairFile (
    LPSTR lpFileName );
#endif    

//************************************************************************
// InocRepairFile()
//
// This routine attempts to repair a file using inoculation technology.
//
// Parameters:
//      LPSTR       lpFileName          ANSI name of file to repair
//
// Returns:
//      TRUE                            File successfully repaired.
//      FALSE                           Repair failed.  (or file not in
//                                          database)
//************************************************************************
// 4/20/93 DALLEE, Function created.
// 6/27/96  GWESTER Ported in changes from Luigi Plus. Input file name is
//          in ANSI; we convert it to OEM here.
//************************************************************************

BOOL SYM_EXPORT WINAPI InocRepairFile (
    LPSTR   lpFile )
{
#ifdef _ALPHA_
	return FALSE;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return FALSE;

    auto    BOOL    bSuccess = FALSE;

    SmartInocOpen ( lpFile, FALSE );

    if ( IsInoculated  ( lpFile ) == INOC_CHANGED &&
         _InocRepairFile( lpFile ) == TRUE )
        {
        bSuccess = TRUE;
        }

    return ( bSuccess );
#endif
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// 6/27/96  GWESTER Ported in changes from Luigi Plus. Input file name is
//          in ANSI; we convert it to OEM here.

#ifndef _ALPHA_
BOOL LOCAL _InocRepairFile (
    LPSTR lpFileName)
{
    auto    FP          fpInfoBefore;
    auto    FP          fpInfoAfter;
    auto    BOOL        bStatus = FALSE;
    auto    LPINOCULATE Inoc;
    auto    BOOL        bLongFile = FALSE;
    auto    UINT        uEntrySize;

    const   UINT        uFileBufferSize =   ( SYM_MAX_PATH * 2 );
    auto    LPSTR       lpShortFile     =   NULL;
    auto    LPSTR       lpOemFile      =   NULL;

    SYM_ASSERT (lpFileName );

    lpShortFile     =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );
    lpOemFile      =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );

    if ( lpShortFile && lpOemFile )
        {
        if ( lpFileName[1] == ':')
            {
            Inoc = GetInocStructGivenFile(lpFileName);

            if ( Inoc != NULL) 
                bStatus = TRUE;

            DiskSet(lpFileName[0]);
            }
        else if ( IsUNCName (lpFileName) )
            {
            Inoc = GetInocStructGivenFile(lpFileName);

            if ( Inoc != NULL) 
                bStatus = TRUE;
            }
        else
            {
            Inoc = GetInocStruct(NULL, bLongFile);

            if ( Inoc != NULL)
                bStatus = TRUE;
            }

        NavAnsiToOemUpper(lpFileName, lpOemFile);

        NameReturnShortName(lpFileName, lpShortFile);
        NavAnsiToOemUpper ( lpShortFile, lpShortFile );

        bLongFile = (BOOL) (STRLEN(lpShortFile) > 144);

        uEntrySize = (bLongFile) ? sizeof(FILE_PROTECTL) : sizeof(FILE_PROTECT);
   
        if ( bStatus == TRUE )
            {
            bStatus = !FIL4_PROTECT (
                                    lpFileName,
                                    lpOemFile,
                                    lpShortFile,
                                    PROTECT_CREATE,
                                    (LPSTR) &fpInfoBefore,
                                    uEntrySize,
                                    Inoc->lpWorkMem,
                                    Inoc->lpWorkMem2
                                    );

            if ( bStatus == TRUE &&
                 DBC4_SEARCH ( Inoc->lpHeader, &fpInfoBefore, 
                               uEntrySize,
                               &fpInfoAfter) )
                {
                bStatus = !FIL4_PROTECT(
                                    lpFileName,
                                    lpOemFile,
                                    lpShortFile,
                                    PROTECT_REPAIR,
                                    (LPSTR) &fpInfoAfter,
                                    uEntrySize,
                                    Inoc->lpWorkMem,
                                    Inoc->lpWorkMem2
                                    );
                }
            else
                {
                bStatus = FALSE;
                }
            }
        }

    if ( lpShortFile )
        MemFreePtr ( lpShortFile );

    if ( lpOemFile )
        MemFreePtr ( lpOemFile );

    return (bStatus);
} // End InocRepairFile()
#endif
