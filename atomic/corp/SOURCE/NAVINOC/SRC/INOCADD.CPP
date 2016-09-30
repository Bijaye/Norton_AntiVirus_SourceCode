// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocadd.cpv   1.1   20 May 1997 17:34:06   BILL  $
//
// Description:
//      This file contains the routine for adding a file to the inoculation
//      database.
//
// Contains:
//      InocFile()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocadd.cpv  $
// 
//    Rev 1.1   20 May 1997 17:34:06   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
// 
//    Rev 1.0   06 Feb 1997 20:58:14   RFULLER
// Initial revision
// 
//    Rev 1.4   10 Oct 1996 15:05:02   JBRENNA
// Change NAnsiToOem() calls to NavAnsiToOemUpper() calls. Note that
// NAnsiToOem() upper cases all characters passed to it. NavAnsiToOemUpper()
// only upper cases non-DBCS characters. This prevents problems when you
// have mixed case DBCS English filenames.
// 
//    Rev 1.3   07 Aug 1996 14:02:52   YVANG
// Ported changes from LuigiPlus.
// 
//    Rev 1.2   27 Jun 1996 10:15:04   GWESTER
// Ported in fixes and changes from LuigiPlus.
// 
//    Rev 1.1   14 Feb 1996 17:05:36   JWORDEN
// Process internal file names in OEM mode to allow for the innoculation
// of file names that include certain non-ANSI translatable characters
// 
//    Rev 1.0   30 Jan 1996 15:53:14   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:18   BARRY
// Initial revision.
// 
//    Rev 1.10   06 Jun 1995 17:56:24   KEITH
// Oem the short filename too
// 
//    Rev 1.9   12 Apr 1995 15:36:12   MFALLEN
// Was not working correctly with very long LFNs. Buffer sizes were increased
// and dynamically allocated so the stack is not exhausted. This should be done
// when possible, and watch those multiple returns because functions clean
// up before exiting.
// 
//    Rev 1.8   31 Jan 1995 13:10:42   BRAD
// Cleaned up for Luigi
// 
//    Rev 1.7   30 Jan 1995 19:26:16   BRAD
// Added another param to FIL4_PROTECT
// 
//    Rev 1.6   16 Jan 1995 16:00:56   ELEAR
// Removed  || defined(SYM_DOS)
// 
//    Rev 1.5   28 Nov 1994 20:14:04   MFALLEN
// Now also works for DOS platform.
// 
//    Rev 1.4   28 Nov 1994 18:56:42   MFALLEN
// CERTLIB is alive for W16
// 
//    Rev 1.3   28 Nov 1994 14:14:30   MFALLEN
// Compiles fine for W16 and hopefully also for W32
// 
//    Rev 1.2   12 May 1994 10:44:26   MFALLEN
// cts4.h 2 ctsn.h
// 
//    Rev 1.1   25 Apr 1994 16:01:12   MFALLEN
// Just trying to make code compile. It's not working yet.
// 
//    Rev 1.0   19 Apr 1994 19:19:36   MFALLEN
// Initial revision.
// 
//    Rev 1.15   18 Mar 1994 00:15:20   DALLEE
// Changed #ifdef BANKERS to be for Windows only in most cases.
// DOS needs updated structures, but not the new functionality.
// 
//    Rev 1.14   08 Mar 1994 12:21:48   DALLEE
// Use GetRegistrationInfo() instead of duping code for doing full file CRC.
// 
//    Rev 1.13   01 Mar 1994 12:29:36   DALLEE
// RegisterFile to RegFile() to better mirror naming convention of
// inoculation functions.
// 
//    Rev 1.12   23 Feb 1994 22:48:46   DALLEE
// Support for save/check file registration.
// 
//    Rev 1.11   07 Sep 1993 01:10:24   DALLEE
// Use INOC_CHANGED #define instead of hard-coded 2 for changed inoc. info.
// 
//    Rev 1.10   30 Jul 1993 12:57:10   CGUC
// Adjusted FIL4_PROTECT 
// 
//    Rev 1.9   19 May 1993 20:45:46   BRAD
// Added call to GetInocStructGivenFile().
// 
//    Rev 1.8   05 May 1993 21:58:54   DALLEE
// 
//    Rev 1.7   05 May 1993 21:50:50   DALLEE
// Use IsInoculated to check if the file is already in the database.
// 
//    Rev 1.6   05 May 1993 20:20:54   CJKUO
// Make calls to routines in inocdb.c to track which INOCULATE structure
// is active and thus available for use.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "ctsn.h"
#include "cmbstr.h"
#include "options.h"
#include "inoc.h"
#include "navutil.h"


//************************************************************************
// InocFile()
//
// This routine adds the FILE_PROTECT information for a file to the
// inoculation database.
//
// Parameters:
//      LPSTR   lpFileName              Full ANSI pathname of file to add.
//
// Returns:
//      TRUE                            File added to database.
//      FALSE                           Error, file not added.
//************************************************************************
// 1/8/93 MFALLEN, Function created.
// 5/5/93 DALLEE, Use IsInoculated() to see if the file is in the database
//        rather than calls to DBC4_SEARCH_QUICK() and DBC4_SEARCH().
// 2/7/96 Expect input file name to be in OEM form
// 6/27/96  GWESTER Ported in changes from Luigi Plus. Input file name is
//          in ANSI; we convert it to OEM here.
//************************************************************************

BOOL SYM_EXPORT WINAPI InocFile (
    LPSTR       lpFileName
    )
{
#ifdef _ALPHA_
	return FALSE;
#else
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return FALSE;
	    	
    auto    FP          ProBefore;
    auto    UINT        uStatus = 0;
    auto    LPINOCULATE Inoc;
    auto    BOOL        bLongFile;
    auto    UINT        uEntrySize;
    const   UINT        uFileBufferSize =   ( SYM_MAX_PATH * 2 );
    auto    LPSTR       lpShortFile     =   NULL;
    auto    LPSTR       lpOemFile      =   NULL;

    lpShortFile     =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );
    lpOemFile      =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );

    if ( lpShortFile && lpOemFile )
        {
        Inoc = GetInocStructGivenFile (lpFileName);

        if ( Inoc == NULL ) 
            uStatus = ERR_INOC_INTERNAL;
        else
            {
            NavAnsiToOemUpper(lpFileName, lpOemFile);

            NameReturnShortName (lpFileName, lpShortFile);
            NavAnsiToOemUpper ( lpShortFile, lpShortFile );

            bLongFile = (BOOL) (STRLEN(lpShortFile) > 144);

            uEntrySize = (bLongFile) ? sizeof(FILE_PROTECTL) : sizeof(FILE_PROTECT);
   
            if (!FIL4_PROTECT (
                            lpFileName,
                            lpOemFile,
                            lpShortFile,
                            PROTECT_CREATE,
                            (LPSTR)&ProBefore,
                            uEntrySize,
                            Inoc->lpWorkMem,
                            Inoc->lpWorkMem2
                            ) )
                {
                switch (IsInoculated (lpFileName))
                    {
                    case FALSE:                    // File not in database.
                    case INOC_CHANGED:             // File is in database, but changed.
                        if (!DBC4_INSERT(Inoc->lpHeader, &ProBefore, Inoc->lpWorkMem))
                            uStatus = ERR_INOC_ADD;
                        break;

                    case TRUE:                     // File is already in the database
                        break;                      // with the correct information.
                    }
                }
            else
                uStatus = ERR_INOC_PROTECT;

            }
        }

    if ( lpShortFile )
        MemFreePtr ( lpShortFile );

    if ( lpOemFile )
        MemFreePtr ( lpOemFile );

   return ((uStatus) ? FALSE : TRUE);
#endif
}


#if defined(BANKERS) && defined(SYM_WIN)
//************************************************************************
//             Not converted for OEM file names
//************************************************************************
// RegFile()
//
// This routine adds the registration information for a file to the
// inoculation database.
//
// Parameters:
//      LPSTR   lpFileName              Full pathname of file to add.
//
// Returns:
//      TRUE                            File added to database.
//      FALSE                           Error, file not added.
//************************************************************************
//  2/21/94 DALLEE, from InocFile().
//  3/07/94 DALLEE, replace duped code with GetRegistrationInfo().
//************************************************************************

BOOL SYM_EXPORT WINAPI RegFile (LPSTR lpFileName)
{
    auto    FP              ProBefore;
    auto    UINT            uStatus = ERR_INOC_PROTECT;
    auto    LPINOCULATE     Inoc;


    if ((Inoc = GetInocStructGivenFile(lpFileName)) == NULL)
        return(FALSE);

    if (NOERR == GetRegistrationInfo(lpFileName, PROTECT_CREATE, &ProBefore,
                                     Inoc->lpWorkMem, Inoc->lpWorkMem2))
        {
        if (DBC4_INSERT(Inoc->lpHeader, &ProBefore, Inoc->lpWorkMem))
            {
            uStatus = NOERR;
            }
        else
            {
            uStatus = ERR_INOC_ADD;
            }
        }

    return ((uStatus) ? FALSE : TRUE);
} // RegFile()
#endif  // BANKERS && SYM_WIN
