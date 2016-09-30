// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocfind.cpv   1.1   20 May 1997 17:38:46   BILL  $
//
// Description:
//      Looks up a file in the inoculation database.
//
// Contains:
//      IsInoculated()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocfind.cpv  $
// 
//    Rev 1.1   20 May 1997 17:38:46   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
// 
//    Rev 1.0   06 Feb 1997 20:58:18   RFULLER
// Initial revision
// 
//    Rev 1.5   10 Oct 1996 15:05:00   JBRENNA
// Change NAnsiToOem() calls to NavAnsiToOemUpper() calls. Note that
// NAnsiToOem() upper cases all characters passed to it. NavAnsiToOemUpper()
// only upper cases non-DBCS characters. This prevents problems when you
// have mixed case DBCS English filenames.
// 
//    Rev 1.4   07 Aug 1996 14:03:00   YVANG
// Ported changes from LuigiPlus.
// 
//    Rev 1.3   27 Jun 1996 10:15:02   GWESTER
// Ported in fixes and changes from LuigiPlus.
// 
//    Rev 1.2   22 Mar 1996 10:08:36   jworden
// DBCS Enable
// 
//    Rev 1.1   14 Feb 1996 17:08:00   JWORDEN
// Process internal file names in OEM mode to allow for the innoculation
// of file names that include certain non-ANSI translatable characters
// 
//    Rev 1.0   30 Jan 1996 15:53:18   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:22   BARRY
// Initial revision.
// 
//    Rev 1.11   06 Jun 1995 17:56:34   KEITH
// Oem the short filename too
// 
//    Rev 1.10   04 May 1995 15:36:56   MFALLEN
// Preserve case.
// 
//    Rev 1.9   12 Apr 1995 15:38:28   MFALLEN
// Was not working correctly with very long LFNs. Buffer sizes were increased
// and dynamically allocated so the stack is not exhausted. This should be done
// when possible, and watch those multiple returns because functions clean
// up before exiting.
// 
//    Rev 1.8   03 Feb 1995 16:10:46   BRAD
// Got working and added Long File name
// 
//    Rev 1.7   30 Jan 1995 19:35:28   BRAD
// Added another param to FIL4_PROTECT
// 
//    Rev 1.6   28 Nov 1994 20:13:56   MFALLEN
// Now also works for DOS platform.
// 
//    Rev 1.5   28 Nov 1994 18:56:40   MFALLEN
// CERTLIB is alive for W16
// 
//    Rev 1.4   28 Nov 1994 14:14:26   MFALLEN
// Compiles fine for W16 and hopefully also for W32
// 
//    Rev 1.3   12 May 1994 10:44:30   MFALLEN
// cts4.h 2 ctsn.h
// 
//    Rev 1.2   25 Apr 1994 16:00:58   MFALLEN
// Just trying to make code compile. It's not working yet.
// 
//    Rev 1.1   21 Apr 1994 09:53:48   MFALLEN
// Trunk conversion.
// 
//    Rev 1.0   19 Apr 1994 19:19:44   MFALLEN
// Initial revision.
// 
//    Rev 1.25   18 Mar 1994 00:15:56   DALLEE
// Changed #ifdef BANKERS to be for Windows only in most cases.
// DOS needs updated structures, but not the new functionality.
// 
//    Rev 1.24   08 Mar 1994 12:22:36   DALLEE
// Use GetRegistrationInfo() instead of duping code for doing full file CRC.
// 
//    Rev 1.23   23 Feb 1994 22:49:26   DALLEE
// Support for save/check file registration.
// 
//    Rev 1.22   10 Sep 1993 17:09:08   CAREY
// Fixed bug which causes nav to think a file HAS been inoculated & that the
// file has changed when in fact the file hasn't been inoc'd.  This occured
// when quick look up had a colision (was able to load up correctly), and then
// the specified file was not found in the database.
// 
//    Rev 1.21   07 Sep 1993 01:10:56   DALLEE
// Use INOC_CHANGED #define instead of hard-coded 2 for changed inoc. info.
// 
//    Rev 1.20   24 Aug 1993 13:51:46   BARRY
// Fixed initialization bug for lpQuick
// 
//    Rev 1.19   24 Aug 1993 13:37:08   BARRY
// Removed Inoc.lpQuick.  Locks on the fly now so DOS VMM can cope better
// 
//    Rev 1.18   23 Aug 1993 21:19:16   BARRY
// Fixed memory leak and now can cope without hQuick if necessary
// 
//    Rev 1.17   30 Jul 1993 12:57:02   CGUC
// Adjusted FIL4_PROTECT 
// 
//    Rev 1.16   27 May 1993 13:18:52   DALLEE
// Removed fast lookup kludge and called FIL4_PROTECT with quick flag.
// 
//    Rev 1.15   25 May 1993 01:41:24   DALLEE
// Removed old definition of InocGetEntry() which had been commented out
// for ages.
// 
//    Rev 1.14   19 May 1993 20:45:48   BRAD
// Added call to GetInocStructGivenFile().
// 
//    Rev 1.13   19 May 1993 20:20:14   BRAD
// Cleaned up minor stuff.
// 
//    Rev 1.12   05 May 1993 20:21:56   CJKUO
// Make calls to routines in inocdb.c to track which INOCULATE structure
// is active and thus available for use.
// 
//    Rev 1.11   30 Apr 1993 13:20:18   MFALLEN
// 
//    Rev 1.10   27 Apr 1993 10:14:30   DALLEE
// Take CRC of 16 bytes or the number read by FIL4_GET_START, 
// whichever is less.
// 
//    Rev 1.9   21 Apr 1993 14:26:12   MFALLEN
// 
//    Rev 1.8   21 Apr 1993 12:57:14   DALLEE
// Added InocGetEntry()
// 
//    Rev 1.7   16 Apr 1993 15:27:46   DALLEE
// Changed call to FIL4_GET_START to match new prototype in CTS4.H
// 
//    Rev 1.6   16 Apr 1993 15:22:16   DALLEE
// Faster look ups, and detect changes in inoculated files.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "options.h"
#include "file.h"
#include "cmbstr.h"
#include "inoc.h"
#include "navutil.h"

MODULE_NAME;

#ifdef MEMORY_TRACKING                  // ASSERT:  These guys are allocated.
extern void PASCAL	      VMMError		 (WORD wError);;
#endif

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

//************************************************************************
// IsInoculated (LPSTR lpFileName)
//
// This routine tells whether or not a file has been inoculated and also
// if it has changed since the last inoculation.
// Changed status is determined by the CRC of the first 16 bytes.
//
// Parameters:
//      LPSTR       lpFileName          Full ANSI path and filename.
//
// Returns:
//      FALSE                           Not inoculated.
//      TRUE                            Inoculated.
//      INOC_CHANGED                    Changed (2).
//************************************************************************
// 3/\93 MFALLEN, Function created.
// 4/15/93 DALLEE, Faster look up and detects changes.
// 5/26/93 DALLEE, Use FIL4_PROTECT() with PROTECT_QUICK_INOC for lookup.
// 8/23/93 BarryG, Cope better with the 'sometimes present' hQuick
// 6/27/96  GWESTER Ported in changes from Luigi Plus. Input file name is
//          in ANSI; we convert it to OEM here.
//************************************************************************

UINT SYM_EXPORT WINAPI IsInoculated (
    LPSTR       lpszFileName
    )
{
#ifdef _ALPHA_
	return FALSE;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return FALSE;

    auto    FP          ProBefore;
    auto    FP          ProAfter;
    auto    UINT        uFound = FALSE;
    auto    LPINOCULATE Inoc;
    auto    LPSTR       lpQuick = NULL ;
    auto    BOOL        bLongFile, bNotEqual;
    auto    UINT        uEntrySize;
    const   UINT        uFileBufferSize =   ( SYM_MAX_PATH * 2 );
    auto    LPSTR       lpFileName      =   NULL;
    auto    LPSTR       lpShortFile     =   NULL;
    auto    LPSTR       lpOemFile      =   NULL;

    // We're making lpFileName big enough to hold one extra byte in case
    // we're given a file name that's too big
    lpFileName      =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize + 1);
    lpShortFile     =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );
    lpOemFile      =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );

    if ( lpShortFile && lpOemFile && lpFileName )
        {
        STRNCPY ( lpFileName, lpszFileName, uFileBufferSize );

        Inoc = GetInocStructGivenFile(lpFileName);

        if ( Inoc != NULL)
            {
            NavAnsiToOemUpper(lpFileName, lpOemFile);

            NameReturnShortName(lpFileName, lpShortFile);
            NavAnsiToOemUpper ( lpShortFile, lpShortFile );

            bLongFile = (BOOL) (STRLEN(lpShortFile) > 144);
            uEntrySize = (bLongFile) ? sizeof(FILE_PROTECTL) : sizeof(FILE_PROTECT);
   
#ifdef MEMORY_TRACKING                  // ASSERT:  These guys are allocated.
    // The extreme granularity is so the line numbers will tell us what went wrong.
            if ( Inoc->lpHeader == NULL )
                VMMError ( VM_NULL_HANDLE_ACCESS - 1 ) ;
            if ( Inoc->lpWorkMem == NULL )
                VMMError ( VM_NULL_HANDLE_ACCESS - 1 ) ;
            if ( Inoc->lpWorkMem2 == NULL )
                VMMError ( VM_NULL_HANDLE_ACCESS - 1 ) ;
#endif // MEMORY_TRACKING

            if ( NOERR == FIL4_PROTECT (
                                    lpFileName,
                                    lpOemFile,
                                    lpShortFile, 
                                    PROTECT_QUICK_INOC,
                                    (LPSTR)&ProBefore,
                                    uEntrySize,
                                    Inoc->lpWorkMem,
                                    Inoc->lpWorkMem2
                                    ) )
                {
                // Try to lock hQuick if it's allocated

                if ( Inoc -> hQuick )
                    lpQuick = (LPSTR)MemLock ( Inoc -> hQuick ) ;

                if ( lpQuick )
                    {
                    uFound = DBC4_SEARCH_QUICK(Inoc->lpHeader,&ProBefore,lpQuick);
                    MemRelease ( Inoc -> hQuick ) ;
                    }
                else
                    uFound = 2 ;        // Couldn't lock buffer so search the slow way



                if ( uFound == 2 )
                    {
                    if (DBC4_SEARCH(Inoc->lpHeader, &ProBefore, 
                            uEntrySize,
                            &ProAfter) == TRUE)
                        {
                        bNotEqual = (bLongFile) ? 
                                    (ProBefore.fileProtectL.StartCRC != ProAfter.fileProtectL.StartCRC) :
                                    (ProBefore.fileProtect.StartCRC != ProAfter.fileProtect.StartCRC);
                        if (bNotEqual)
                            {
                                        // This file name is in the database,
                                        // but the CRC information has changed.
                            uFound = INOC_CHANGED;
                            }
                        else
                            {
                            uFound = TRUE;
                            }
                        }
                    else
                        uFound = FALSE;
                    }
                }
            }
        }

    if ( lpFileName )
        MemFreePtr ( lpFileName );

    if ( lpShortFile )
        MemFreePtr ( lpShortFile );

    if ( lpOemFile )
        MemFreePtr ( lpOemFile );

   return ( uFound );
#endif   
} // End IsInoculated()


#if defined(BANKERS) && defined(SYM_WIN)
//************************************************************************
//                   Not converted for OEM file names
//************************************************************************
// IsRegistered (LPSTR lpFileName)
//
// This routine tells whether or not a file has been registered and also
// if it has changed since the last registration.
// Changed status is determined by the CRC of the entire file.
//
// Parameters:
//      LPSTR       lpFileName          Full path and filename.
//
// Returns:
//      FALSE                           Not inoculated.
//      TRUE                            Inoculated.
//      INOC_CHANGED                    Changed (2).
//************************************************************************
//  2/21/94 DALLEE, from IsInoculated.
//  3/07/94 DALLEE, replace duped code with GetRegistrationInfo().
//************************************************************************

UINT SYM_EXPORT WINAPI IsRegistered (LPSTR lpFileName)
{
    auto    FP              ProBefore;
    auto    FP              ProAfter;
    auto    UINT            uFound = FALSE;
    auto    LPINOCULATE     Inoc;
    auto    LPSTR           lpQuick = NULL;
    BOOL            bLongFile;
    UINT            uEntrySize;
    TCHAR           szShortFile[SYM_MAX_PATH];
    TCHAR           szOemFile[SYM_MAX_PATH];


    if ((Inoc = GetInocStructGivenFile(lpFileName)) == NULL)
        return(FALSE);

    NavAnsiToOem(lpFileName, szOemFile);
    NameReturnShortName(lpFileName, szShortFile);
    bLongFile = (BOOL) (STRLEN(szShortFile) > 144);
    uEntrySize = (bLongFile) ? sizeof(FILE_PROTECTL) : sizeof(FILE_PROTECT);
   
    if (NOERR == GetRegistrationInfo(lpFileName, PROTECT_QUICK_INOC, &ProBefore,
                                     Inoc->lpWorkMem, Inoc->lpWorkMem2))
        {
        // Try to lock hQuick if it's allocated

        if (Inoc->hQuick)
            lpQuick = (LPSTR)MemLock(Inoc->hQuick);

        if (lpQuick)
            {
            uFound = DBC4_SEARCH_QUICK(Inoc->lpHeader,&ProBefore,lpQuick);
            MemRelease(Inoc->hQuick);
            }
        else
            {
            uFound = 2 ;            // Couldn't lock buffer so search
            }                       // the slow way.


        if (uFound == 2)
            {
            if ( (DBC4_SEARCH(Inoc->lpHeader, &ProBefore, 
                            uEntrySize,
                            &ProAfter) == TRUE)
                 && (ProAfter.wReserved1 & bitFILE_PROTECT_FULLCRC) )
                {
                if (ProBefore.wReserved2 != ProAfter.wReserved2)
                    {
                                    // This file name is in the database,
                                    // but the CRC information has changed.
                    uFound = INOC_CHANGED;
                    }
                else
                    {
                    uFound = TRUE;
                    }
                }
            else
                {
                uFound = FALSE;
                }
            }
        }

   return (uFound);
} // IsRegistered()
#endif  // BANKERS && SYM_WIN

