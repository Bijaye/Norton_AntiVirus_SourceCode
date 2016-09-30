// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocdel.cpv   1.1   20 May 1997 17:35:00   BILL  $
//
// Description:
//      Routines to remove files from the inoculation database.
//
// Contains:
//      InocDelFile()
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocdel.cpv  $
// 
//    Rev 1.1   20 May 1997 17:35:00   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
// 
//    Rev 1.0   06 Feb 1997 20:58:20   RFULLER
// Initial revision
// 
//    Rev 1.5   10 Oct 1996 15:04:56   JBRENNA
// Change NAnsiToOem() calls to NavAnsiToOemUpper() calls. Note that
// NAnsiToOem() upper cases all characters passed to it. NavAnsiToOemUpper()
// only upper cases non-DBCS characters. This prevents problems when you
// have mixed case DBCS English filenames.
// 
//    Rev 1.4   07 Aug 1996 14:02:36   YVANG
// Ported changes from LuigiPlus.
// 
//    Rev 1.3   27 Jun 1996 10:15:06   GWESTER
// Ported in fixes and changes from LuigiPlus.
// 
//    Rev 1.2   26 Jun 1996 17:53:10   GWESTER
// Ported in changes and fixes from LuigiPlus.
// 
//    Rev 1.1   14 Feb 1996 17:07:44   JWORDEN
// Process internal file names in OEM mode to allow for the innoculation
// of file names that include certain non-ANSI translatable characters
// 
//    Rev 1.0   30 Jan 1996 15:53:20   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:24   BARRY
// Initial revision.
// 
//    Rev 1.19   30 Jun 1995 21:52:22   RSTANEV
// Fixed a problem where we wouldn't uninoculate LFNs which were in the SFN
// database.
//
//    Rev 1.18   06 Jun 1995 17:56:28   KEITH
// Oem the short filename too
//
//    Rev 1.17   24 May 1995 15:30:00   BARRY
// Fixed for new params in NavExcludeCheck
//
//    Rev 1.16   18 May 1995 11:42:16   SZIADEH
// call FileExists() to check whether a file exists.
// a call to Inocopen creates the file for u if it dont exist.
//
//    Rev 1.15   26 Apr 1995 16:36:02   MFALLEN
// Added callback capability for uninoculations.
//
//    Rev 1.14   12 Apr 1995 15:38:18   MFALLEN
// Was not working correctly with very long LFNs. Buffer sizes were increased
// and dynamically allocated so the stack is not exhausted. This should be done
// when possible, and watch those multiple returns because functions clean
// up before exiting.
//
//    Rev 1.13   31 Mar 1995 11:49:20   MFALLEN
// Placed back uninoc stuff for my buddy Dave.
//
//    Rev 1.12   31 Mar 1995 10:12:48   MFALLEN
// Moved inoc/uninoc code to NAVW
//
//    Rev 1.11   23 Mar 1995 14:48:46   SZIADEH
// added UNC support.
//
//    Rev 1.10   10 Feb 1995 09:38:34   DALLEE
// Uninoc callback now returns BOOL -- TRUE to continue, FALSE to cancel.
//
//    Rev 1.9   03 Feb 1995 16:10:40   BRAD
// Got working and added Long File name
//
//    Rev 1.8   30 Jan 1995 19:38:10   BRAD
// Added another param to FIL4_PROTECT
//
//    Rev 1.7   16 Jan 1995 16:04:12   ELEAR
// Removed  || defined(SYM_DOS)
//
//    Rev 1.6   28 Nov 1994 20:13:58   MFALLEN
// Now also works for DOS platform.
//
//    Rev 1.5   28 Nov 1994 18:56:46   MFALLEN
// CERTLIB is alive for W16
//
//    Rev 1.4   28 Nov 1994 14:14:10   MFALLEN
// Compiles fine for W16 and hopefully also for W32
//
//    Rev 1.3   12 May 1994 10:44:22   MFALLEN
// cts4.h 2 ctsn.h
//
//    Rev 1.2   26 Apr 1994 19:27:18   MFALLEN
// More W32
//
//    Rev 1.1   25 Apr 1994 16:01:10   MFALLEN
// Just trying to make code compile. It's not working yet.
//
//    Rev 1.0   19 Apr 1994 19:19:42   MFALLEN
// Initial revision.
//
//    Rev 1.13   07 Sep 1993 01:10:54   DALLEE
// Use INOC_CHANGED #define instead of hard-coded 2 for changed inoc. info.
//
//    Rev 1.12   30 Jul 1993 12:56:48   CGUC
// Adjusted FIL4_PROTECT
//
//    Rev 1.11   16 Jun 1993 19:48:26   CJKUO
// Open R/W stuff
//
//    Rev 1.10   15 Jun 1993 20:57:06   DALLEE
// Fixed Uninoc bug - did not uninoculate filespecs which were directories
// only with no filename.
//
//    Rev 1.9   31 May 1993 22:27:54   MFALLEN
// Added Uninoculation of Directories
//
//    Rev 1.8   27 May 1993 13:17:52   DALLEE
// Removed duplicate code - called IsInoculated() to check inoc database.
//************************************************************************


#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "file.h"
#include "ctsn.h"
#include "cmbstr.h"
#include "options.h"
#include "inoc.h"
#include "navutil.h"

//************************************************************************
// InocDelFile()
//
// This routine deletes an entry from the inoculation database.
//
// Parameters:
//      LPSTR   lpFileName              ANSI name of file to delete.
//                                      NOTE: Must be present on disk
//                                      since we need to make a
//                                      FILE_PROTECT record for it.
//
// Returns:
//      TRUE                            Entry deleted or was not present in
//                                      the database.
//      FALSE                           Couldn't delete entry.
//************************************************************************
// 03/02/93 MFALLEN, created.
// 05/26/93 DALLEE, removed duplicate code - use call to IsInoculated().
// 6/27/96  GWESTER Ported in changes from Luigi Plus. Input file name is
//          in ANSI; we convert it to OEM here.
//************************************************************************

BOOL SYM_EXPORT WINAPI InocDelFile (
    LPSTR       lpFileName
    )
{
#ifdef _ALPHA_
	return FALSE;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return FALSE;

    auto    FP          fProtect;
    auto    UINT        uStatus = 0;
    auto    LPINOCULATE Inoc;
    auto    BOOL        bLongFile;
    auto    UINT        uEntrySize;
    const   UINT        uFileBufferSize =   ( SYM_MAX_PATH * 2 );
    auto    LPSTR       lpShortFile     =   NULL;
    auto    LPSTR       lpOemFile       =   NULL;

    lpShortFile     =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );
    lpOemFile      =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );

    if ( lpShortFile && lpOemFile )
        {
        Inoc = GetInocStructGivenFile(lpFileName);

        if ( Inoc == NULL )
            uStatus = ERR_INOC_INTERNAL;
        else
            {
            NavAnsiToOemUpper ( lpFileName, lpOemFile );

            NameReturnShortName (lpFileName, lpShortFile);
            NavAnsiToOemUpper ( lpShortFile, lpShortFile );

            bLongFile = (BOOL) (STRLEN(lpShortFile) > 144);

            uEntrySize = (bLongFile) ? sizeof(FILE_PROTECTL) : sizeof(FILE_PROTECT);

            switch ( IsInoculated ( lpFileName ) )
                {
                case FALSE:                     // Entry not found
                    break;
                case TRUE:                     // Entry found
                case INOC_CHANGED:             // Entry found but changed
                    if ( !FIL4_PROTECT (
                                    lpFileName,
                                    lpOemFile,
                                    lpShortFile,
                                    PROTECT_CREATE,
                                    (LPSTR)&fProtect,
                                    uEntrySize,
                                    Inoc->lpWorkMem,
                                    Inoc->lpWorkMem2
                                    ) )
                        {
                        if ( !DBC4_DELETE (
                                        Inoc->lpHeader,
                                        &fProtect,
                                        Inoc->lpWorkMem
                                        ) )
                            {
                            uStatus = ERR_INOC_DEL;
                            }
                        break;
                        }
                    else
                        {
                        uStatus = ERR_INOC_PROTECT;
                        }
                    break;
                }
            }
        }

    if ( lpShortFile )
        MemFreePtr ( lpShortFile );

    if ( lpOemFile )
        MemFreePtr ( lpOemFile );

    return ( (uStatus) ? FALSE : TRUE );
#endif    
} // End InocDelFile()


//************************************************************************
// DoUnInocFiles()
//
// This routine goes through the entire inoculation database, deleting
// those entries which match the filespec passed.
//
// Parameters:
//      LPSTR   lpFileSpec              OEM files to uninoculate
//      BOOL    bSubDirs                Match subdirectories.
//
// Returns:
//      Nothing.
//************************************************************************
// 5/25/93 DALLEE, Function created.
// 5/31/93 Martin, Function enhanced.
//************************************************************************

VOID SYM_EXPORT WINAPI DoUnInocFiles (  // Do uninoc stuff
    LPUNINOCSTRUCT  lpUninoc
    )
{
#ifdef _ALPHA_
	return;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    auto    FP              fProtect;
    auto    EXCLUDEITEM     rExcludeDir;
    auto    EXCLUDEITEM     rExcludeFile;
    auto    LPINOCULATE     lpInoc;
    auto    DWORD           dwNumEntries;
    auto    DWORD           dwIndex;
    auto    UINT            uPercent;
    auto    BOOL            bLongFile;
    auto    UINT            i;
    auto    BOOL            bUNC_OK=FALSE;

    const   UINT            uFileBufferSize =   ( SYM_MAX_PATH * 2 );
    auto    LPSTR           lpInocFile      =   NULL;
    auto    LPSTR           lpTemp          =   NULL;
    auto    LPSTR           lpLFNOfInoced   =   NULL;
    auto    BOOL            bLFNOfInocedIsValid;

    lpInocFile  =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );
    lpTemp      =   (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );
    lpLFNOfInoced = (LPSTR)MemAllocPtr ( GHND, uFileBufferSize );

    if ( lpInocFile && lpTemp && lpLFNOfInoced )
        {
        // ******************************************************************** //
        // Set up the EXCLUDEITEMS.  These are used to check if the inoculation //
        // entry matches our file specification.                                //
        // ******************************************************************** //

        MEMSET ( &rExcludeDir,   EOS, sizeof(rExcludeDir) );
        MEMSET ( &rExcludeFile,  EOS, sizeof(rExcludeFile) );

        rExcludeDir.bSubDirectory   = lpUninoc->bSubDirs;
        rExcludeFile.bSubDirectory  = FALSE;
        rExcludeDir.wBits           = 1;
        rExcludeFile.wBits          = 1;

        SYM_ASSERT (lpUninoc->lpFileSpec );
        bUNC_OK = IsUNCName ( lpUninoc->lpFileSpec );

        // -------------------------------------------------------------------- //
        // "drive:\path" or UNC path must be specified.                         //
        // -------------------------------------------------------------------- //

        if  ( bUNC_OK || (lpUninoc->lpFileSpec [1] == ':') )
            {
            STRCPY ( rExcludeDir.szText, lpUninoc->lpFileSpec );
            }
        else
            {
            goto CleanUp;
            }

        // -------------------------------------------------------------- //
        // If we have a wildcard filename at the end of our spec, we need //
        // to strip it and use our second EXCLUDEITEM for the filemask.   //
        // -------------------------------------------------------------- //

        if (NameIsWildcard(rExcludeDir.szText))
            {
            NameParsePath(rExcludeDir.szText, rExcludeFile.szText);
            }
        else
            {
            rExcludeFile.szText[0] = EOS;
            }

                                        // Do this once for the short list,
                                        // then for the long list.
        bLongFile = FALSE;
        for (i = 0; i < 2; i++, bLongFile = TRUE)
            {
            BuildInocDBName(lpTemp, bLongFile);

            if ( bUNC_OK )
                {
                UNC_GetServerVolumeName (rExcludeDir.szText, lpInocFile );
                STRCAT ( lpInocFile, lpTemp+2 );
                }
            else
                {
                STRCPY ( lpInocFile, lpTemp );
                lpInocFile[0] = rExcludeDir.szText[0];
                }

                                        // Do not create it if it is not there.
            //if (FALSE == InocOpen(lpInocFile, FALSE))
            if (!FileExists (lpInocFile) )
                goto CleanUp;

                                        // Now that we know it's there, open
                                        // for write.
            InocClose();
            if (TRUE == InocOpen(lpInocFile, TRUE))
                {
                                        // Search through all files in the
                                        // database for the drive we want,
                                        // and delete any entries that match
                                        // our file spec.

                dwNumEntries = InocGetNumEntries(lpInocFile, bLongFile);

                for (dwIndex = 1; dwIndex <= dwNumEntries; dwIndex++)
                    {
                // ********************************************************* //
                // Calculate the percentage, and call the provided function. //
                // Check for user abort.
                // ********************************************************* //

                    uPercent = (UINT)((dwIndex * 100)/dwNumEntries);

                    if (FALSE == (*lpUninoc->lpfn)(uPercent, lpUninoc->lParam))
                        {
                        break;
                        }


                    InocGetEntry(lpInocFile, dwIndex, &fProtect, bLongFile);

                                        // szFileName has the drive letter
                                        // we want plus ":\" so append
                                        // 'driveless' filename we retrieved
                                        // to it.
                    if (bUNC_OK)
                        {
                                        // the +1 is to preserve the '\'
                        STRCPY ( UNC_SkipServerVolumeName ( lpInocFile ) + 1,
                                 fProtect.fileProtect.Filename);
                        }
                    else
                        STRCPY(lpInocFile+2, fProtect.fileProtect.Filename);

                                        // If this record matches our filespec,
                                        // delete it.

                    bLFNOfInocedIsValid = ExcludeSFN2LFN (
                                                lpInocFile,
                                                lpLFNOfInoced,
                                                uFileBufferSize );

                                        // Don't ask me how the following if
                                        // works.  It started as an innocent
                                        // copy, paste and hack...

                    if (( bLFNOfInocedIsValid &&
                        ( NavExcludeCheck (&rExcludeDir, NULL, lpLFNOfInoced, 1 ) &&
                         ( !rExcludeFile.szText[0] ||
                            NavExcludeCheck ( &rExcludeFile, NULL, lpLFNOfInoced, 1 )))
                        )
                        ||
                        ( NavExcludeCheck (&rExcludeDir, lpInocFile, NULL, 1 ) &&
                         ( !rExcludeFile.szText[0] ||
                            NavExcludeCheck ( &rExcludeFile, lpInocFile, NULL, 1 )))
                       )
                        {
                        lpInoc = GetInocStruct ( lpInocFile, bLongFile );

                        if ( lpInoc )
                            {
                            UINT uStatus = DBC4_DELETE (
                                                    lpInoc->lpHeader,
                                                    &fProtect,
                                                    lpInoc->lpWorkMem
                                                    );

                                        // -----------------------------------
                                        // If the calling app provided a
                                        // callback function pointer to notify
                                        // about uninoc completion status.
                                        // -----------------------------------

                            if ( lpUninoc->lpfnUnInoc )
                                {
                                        // -----------------------------------
                                        // If the return status is something
                                        // else than zero and error occured.
                                        // -----------------------------------

                                uStatus = (*lpUninoc->lpfnUnInoc) (
                                                    lpInocFile,
                                                    (uStatus) ? TRUE : FALSE,
                                                    lpUninoc->dwUnInocCookie
                                                    );

                                        // -----------------------------------
                                        // Check if we should drop whatever
                                        // we're doing.
                                        // -----------------------------------

                                if ( !uStatus )
                                    {
                                    break;
                                    }
                                }
                            }
                        }
                    }
                InocClose();
                }
            }
        }

CleanUp:

    if ( lpLFNOfInoced )
        MemFreePtr ( lpLFNOfInoced );

    if ( lpInocFile )
        MemFreePtr ( lpInocFile );

    if ( lpTemp )
        MemFreePtr ( lpTemp );
#endif
} // End DoUnInocFiles()

