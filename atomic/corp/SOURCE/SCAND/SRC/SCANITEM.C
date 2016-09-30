// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/scanitem.c_v   1.0   06 Feb 1997 21:08:56   RFULLER  $
//
// Description:
//      These are the functions NAV DOS calls to access the low level
//      scanning routines.
//
// Contains:
//      ScanMemory()
//      ScanMasterBoot()
//      ScanBoot()
//      ScanFiles()
//      ScanFileAll()
//      StartScanFile()
//      ScanFile()
//
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/scanitem.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:56   RFULLER
// Initial revision
// 
//    Rev 1.3   15 Oct 1996 12:46:12   JALLEE
// Do not scan inside zip files when chosen from File/Scan if we are navboot.
// 
//    Rev 1.2   08 Aug 1996 15:37:14   JBRENNA
// LuigiPlus Port:
//   JWORDEN: Pass file name in Infected.szFullOemPath to match shared
//     functions in NAVXUTIL.
// 
//   MZAREMB: Added a "throwaway" variable to the NWGetVolumeInfo call for DX.
// 
//    Rev 1.1   08 Aug 1996 13:26:06   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.104   31 Jul 1996 11:09:24   JBELDEN
// Move TSR_OFF higher up in ScanFiles so that the TSR is not triggered
// while doing a scan of a file with nav.exe
// 
//    Rev 1.103   26 Jun 1996 09:43:36   JALLEE
// Corrected Problems Found list generation for entries containing infected
// zip files.
// 
//    Rev 1.102   13 Jun 1996 14:04:36   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.101   06 May 1996 09:07:22   MZAREMB
// Initial Pegasus update: added zip and navscan support via #ifdefs.
// 
//    Rev 1.100   07 Jun 1995 10:27:28   BARRY
// Remove ZIP dependencies
// 
//    Rev 1.99   22 May 1995 14:46:26   DALLEE
// The code which handles exclusions was moved to after the
// scanning process, and resp. only if infection.  This results
// in a large speed increase. -CGRANGE
// 
// 
//    Rev 1.98   10 May 1995 01:40:26   SZIADEH
// call InocInit() to make sure the INOC PATH is updated.
// 
//    Rev 1.97   15 Mar 1995 17:45:52   DALLEE
// Replace StdDlgError() with NavDlgError() to ensure we pop up if hidden.
// 
//    Rev 1.96   14 Mar 1995 17:46:02   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.95   13 Mar 1995 13:58:52   DALLEE
// Didn't notice a change to GetBytesToScan() interface.
// Have to fill in NONAV manually w/ return value.
// 
//    Rev 1.94   06 Mar 1995 14:09:52   RSTANEV
// Checking if the filename to be scanned fits in buffers with length of
// SYM_MAX_PATH bytes.
//
//    Rev 1.93   06 Feb 1995 13:37:26   DALLEE
// SmartInocOpen() parameter change: Takes full path of file for which we're
// opening the database -- necessary to distinguish NAVINOC from NAVINOCL?
//
//    Rev 1.92   29 Dec 1994 16:46:16   DALLEE
// Include syminteg.h before nonav.h
//
//    Rev 1.91   28 Dec 1994 14:15:54   DALLEE
// Latest NAVBOOT revision.
//
    //    Rev 1.3   27 Dec 1994 19:36:48   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.2   12 Dec 1994 18:11:02   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.1   05 Dec 1994 18:10:24   DALLEE
    // CVT1 script.
    //
//    Rev 1.90   28 Dec 1994 13:53:28   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
//
    //    Rev 1.0   23 Nov 1994 15:38:46   DALLEE
    // Initial revision.
    //
    // From NIRVANA/SCAND scanitem r1.84
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "file.h"
# include "disk.h"
#include "tsr.h"

#include "ctsn.h"
#include "virscan.h"

#ifdef SYM_DOS
 #define BINDERY_OBJECT_TIME FAKE_BINDERY_OBJECT_TIME
#endif
#include "navutil.h"
#ifdef SYM_DOS
 #undef BINDERY_OBJECT_TIME
#endif

#include "options.h"
#include "syminteg.h"
#include "nonav.h"
#include "exclude.h"
#include "inoc.h"
#include "navdprot.h"
#include "tsrcomm.h"
#ifdef USE_ZIP
#include "symzip.h"
#endif
#include "scand.h"
#include "dosnet.h"

MODULE_NAME;

//************************************************************************
// DEFINES
//************************************************************************

#define SCAN_FILES_WORK_SIZE    4000

#ifdef USE_ZIP
// &? No longer true with LFN
#ifndef MAX_EXTENSION_SIZE
#define MAX_EXTENSION_SIZE  32
#endif

// &? No longer true with LFN
#ifndef MAX_FILE_SIZE
#define MAX_FILE_SIZE  12
#endif
#endif

//************************************************************************
// GLOBAL VARIABLES
//************************************************************************

BOOL    gbContinueScan;
#if !defined(NAVSCAN) && !defined(NAVNET)
BOOL bAllowZipScanning = TRUE;          // If we can't create temp files, disable
#endif


//************************************************************************
// STATIC VARIABLES
//************************************************************************
                                        // Static to count how many dir
                                        // levels down and up we go when
                                        // the directory name exceeds
                                        // MAX_DIR_SIZE.
STATIC  UINT            uDirsTooDeep = 0;
STATIC  UINT            uDirLevel = 0;

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

UINT STATIC WINAPI LOADDS ScanFileAll (LPFINDITEM   lpFind);
BOOL LOCAL PASCAL CheckEnoughFileHandles(LPNONAV lpNoNav, BOOL * lpbHelpOpen);

VOID          PASCAL ValidateZipOverlaying(VOID);

//************************************************************************
// OTHER PROTOTYPES
//************************************************************************

/*-----------------------------------------------------------------------
 *               +                                    +
 *               +          ValidateZipOverlaying()   +
 *               +                                    +
 *               --------------------------------------
 *
 * Description:
 *      Checks for write permissions and sets the bAllowZipScanning
 *      variable based on this.
 *
 * Return Value:
 *      NONE
 *
 *-----------------------------------------------------------------------
 * 11/29/1995 DSACKING Function Created.
 *----------------------------------------------------------------------*/

#if !defined(NAVSCAN) && !defined(NAVNET)
#ifdef USE_ZIP

#define TEMP_FILE_SIZE_NEEDED 65536L

VOID PASCAL ValidateZipOverlaying(VOID)
{
    auto        char            szTempFile[MAX_PATH_SIZE + 1];
    auto        HFILE           hFile;
    auto        DWORD           dwCount;
    auto        UINT            uWrite;

    if ( ERR != (hFile = FileCreateTemp(TEMP_CLASS_TEMP,
                                 NULL,
                                 NULL,
                                 0L,
                                 szTempFile) ) )
    {
        bAllowZipScanning = TRUE;

                                        // Section added for STACKER compatibility
                                        // Write our buffer out.
        dwCount = 0;

        while (dwCount < TEMP_FILE_SIZE_NEEDED)
        {
                                        // We have a buffer to write, so only
                                        // write chunks the size of our buffer.
           if ( (TEMP_FILE_SIZE_NEEDED - dwCount) > ((DWORD)(MAX_PATH_SIZE)) )
              uWrite = MAX_PATH_SIZE;
           else                         // We will always have less than 64k by now.
              uWrite = (UINT) (TEMP_FILE_SIZE_NEEDED - dwCount);

                                        // Content doesn't matter.  We just
                                        // want to fill up the file.
           if (uWrite != FileWrite(hFile, szTempFile, uWrite))
           {
                                        // If we can't write all of the bytes
                                        // then bail out and don't allow
                                        // zip scanning.
              bAllowZipScanning = FALSE;
              break;
           }

           dwCount += uWrite;           // Add number of bytes.
        }

        FileFlush (hFile);              // Needed for STACKED drives!!!
        FileClose (hFile);

        FileDelete (szTempFile);

                                        // Now tell RTLINK.
//        OvlSetFileName(szTempFile);
    }
    else
    {
        bAllowZipScanning = FALSE;
    }
}
#endif  // USE_ZIP
#endif  // !NAVSCAN

//************************************************************************
// ScanFiles()
//
// This routine scans all drives and directories specified in the
// list of strings whose handle is lpNav->hScanList.
//
// Parameters:
//      LPNONAV lpNav                   Info on this particular scan.
//
// Returns:
//      0                               No error.
//      != 0                            ERROR code.
//************************************************************************
// 3/22/93 DALLEE function taken from Martin N_SCAN.C
// 6/21/93 DALLEE, Open inocdb on drive we're scanning, not current drive.
// 8/17/93 BarryG Support wildcards in scan target.
//************************************************************************

UINT PASCAL ScanFiles (LPNONAV lpNoNav)
{
    extern  char        *SZ_OUT_OF_MEMORY[];
    extern  char        SZ_STARS[];
#if !defined(NAVSCAN) && !defined(NAVNET)
    extern  BOOL        bAllowZipScanning;
    extern  LPSTR       LPSZ_ZIP_CANT_WRITE[];
#endif
    extern  BOOL PASCAL InitNavHelp();  // In NAVD/NAVD.C
    extern  NAVOPTIONS  navOptions;

    auto    LPSTR       lpScanList;
    auto    BOOL        bFileHandlesAvailable = TRUE;
    auto    BOOL        bHelpOpen=TRUE; // Assume help is open
    auto    char        szFile [SYM_MAX_PATH];
#ifdef USE_ZIP
    auto    char        szExt  [MAX_EXTENSION_SIZE];
#endif
    auto    BYTE        byStartDrive;
    auto    BOOL        bSaveScanAll;   // Temporarily overridden on some scans
    auto    WORD        wThrowAway;
    auto    BOOL        bZipInitialized = FALSE;
#if !defined(NAVSCAN) && !defined(NAVNET)
    extern  BOOL            bAllowZipScanning;
#endif
                                        // Defined Navd.c,
                                        // are we navboot.exe or nav.exe
    extern  BOOL        bNavboot;

    if (NULL == (lpScanList = MemLock(lpNoNav->lpScan->hScanList)) )
        {
        NavDlgError(SZ_OUT_OF_MEMORY);
        return (ERR);
        }

    TSR_OFF;                            // Disable TSR first.
    lpNoNav->dwTotalKBytes = GetBytesToScan( lpScanList,
                                             lpNoNav->lpScan->bScanSubs,
                                             GetBytesSkipDrive,
                                             GetBytesDriveError,
                                             (DWORD) lpNoNav );

    ScanStopWatch(lpNoNav, TRUE);

    byStartDrive = DiskGet();           // Save current drive

                                        //--------------------------------
                                        // This block is all inoculation.
                                        // Unused in NAVSCAN
                                        //--------------------------------
    lpNoNav->uInocOpenOK = SMARTOPEN_CLOSED;
#ifndef NAVSCAN
    SmartInocClose();                   // Guarantee closed
                                        // update, in case the inoc dir have
                                        // changed
    InocInit ( &navOptions );

    // Ask user for password while they're still around (in case we need it)

#ifdef USE_PASSWORD    //&?
    if ( lpNoNav->Inoc.bUseInoc &&
         (lpNoNav->Inoc.uActionNewInoc == INOC_AUTO ||
          lpNoNav->Inoc.uActionNewInoc == INOC_AUTO_FILEONLY ) )
        {
        if (!AskForPassword(PASSWORD_INOCFILES))
            {
            lpNoNav->Inoc.uActionNewInoc = INOC_REPORT;
            }
        }
#endif
#endif  // !NAVSCAN



    if ( lpNoNav->Alert.bRemoveAfter )  // Turn on Critical Error timeouts
        {
        DiskErrorSetTimeout((DWORD)lpNoNav->Alert.uSeconds*TICKS_PER_SECOND,2);
        }

                                        //---------------------------------
                                        // Okay, let's start scanning files
                                        // and directories.
                                        //---------------------------------

#ifndef NAVSCAN
#ifdef USE_ZIP
#if !defined(NAVSCAN) && !defined(NAVNET)
    ValidateZipOverlaying();

    if (lpNoNav->Gen.bScanZipFiles && !bAllowZipScanning)
        {
        lpNoNav->Gen.bScanZipFiles = FALSE;
        StdDlgMessage(LPSZ_ZIP_CANT_WRITE);
        }
#endif

    if (lpNoNav->Gen.bScanZipFiles)
        {
        ZipInit();
        bZipInitialized = TRUE;
        }
#endif  // USE_ZIP
#endif  // !NAVSCAN

    DiskReset();                        // Reset disk system
    while (*lpScanList && gbContinueScan)
        {

        //----------------------------------------------------------------
        // if we have a X:\ path and bScanSubs is on, we consider this a
        // whole-drive-scan.  Otherwise it's just a normal scan.
        //----------------------------------------------------------------

        lpNoNav->bScanWholeDrive =
                (STRLEN(lpScanList) == 3 && lpNoNav->lpScan->bScanSubs == TRUE) ?
                 TRUE : FALSE;

                                        // Set uDirsTooDeep to zero for
                                        // each item to scan.
        uDirsTooDeep = 0;
        uDirLevel = 0;                  // initialize starting dir level

                                        // Open inocdb each time on drive we
                                        // are scanning.
#ifndef NAVSCAN
        if (lpNoNav->Inoc.bUseInoc == TRUE)
            {
            if (*lpNoNav->Inoc.szNetInocPath)
                {
                if ( (INOC_AUTO          == lpNoNav->Inoc.uActionNewInoc) ||
                     (INOC_AUTO_FILEONLY == lpNoNav->Inoc.uActionNewInoc) )
                    {
                    lpNoNav->uInocOpenOK = SmartInocOpen(lpScanList, TRUE);

                    if (lpNoNav->uInocOpenOK == SMARTOPEN_ERROR)
                        {
                        lpNoNav->uInocOpenOK = SmartInocOpen(lpScanList,FALSE);
                        }
                    }
                else
                    {
                    lpNoNav->uInocOpenOK = SmartInocOpen(lpScanList, FALSE);
                    }
                }
            }
#endif
                                        // Set up the cluster size
                                        // for this item in the scan list.
        lpNoNav->dwClusterSize = 0;

        if (DiskIsNetwork(*lpScanList))
            {
            auto    WORD            wNetSecPerBlock;

            if (NOERR == NWGetVolumeInfo((BYTE)(*lpScanList - 'A'),
                                         &wNetSecPerBlock,
                                         &wThrowAway,
                                         &wThrowAway,
                                         &wThrowAway,
                                         &wThrowAway,
                                         szFile,
                                         &wThrowAway) )
                {
                //------------------------------------------------------------
                // Calculate network cluster size = sectors per block * sectorsize
                //------------------------------------------------------------

                lpNoNav->dwClusterSize = (DWORD) wNetSecPerBlock * (DWORD) 512;
                }
            }
                                        // If the network call failed, or
                                        // gave us zero, try DiskGetClusterSize
                                        // may work okay, or at least give
                                        // a good default.
        if (lpNoNav->dwClusterSize == 0)
            {
            lpNoNav->dwClusterSize = DiskGetClusterSize(lpScanList[0]);
            }
                                        // Don't let a zero go by...
                                        // dwClusterSize is used in a division.
                                        // Better default to 4096.
        if (lpNoNav->dwClusterSize == 0 ||
            lpNoNav->dwClusterSize == -1)
            {
            lpNoNav->dwClusterSize = 4096;
            }

        // Split entry into path and file components

        bSaveScanAll = lpNoNav->Opt.bScanAll;

                                        //--------------------------------
                                        // If we have an explicit filename
                                        // we should turn on scan all files
                                        // to make sure we scan it.
                                        //--------------------------------
        if ( NameIsWildcard ( lpScanList ) ||
            (NameGetType    ( lpScanList ) & NGT_IS_FILE) )
            {
            NameSeparatePath(lpScanList, lpNoNav->szTheDirectory, szFile);
#ifdef USE_ZIP
            NameReturnExtension(szFile, szExt );
#ifndef NAVSCAN
            if (STRCMP(szExt, "ZIP") == 0 && !bNavboot)
                {                       // Turn on zip scanning if a Zip file
                                        // is explicitly chosen.

#ifndef NAVNET
#if !defined(NAVSCAN) && !defined(NAVNET)
                ValidateZipOverlaying();

                if (lpNoNav->Gen.bScanZipFiles && !bAllowZipScanning)
                    {
                    lpNoNav->Gen.bScanZipFiles = FALSE;
                    StdDlgMessage(LPSZ_ZIP_CANT_WRITE);
                    }
#endif // if !defined(NAVSCAN) && !defined(NAVNET)


                if (!bAllowZipScanning)
                    {
                    lpNoNav->Gen.bScanZipFiles = FALSE;
                    StdDlgMessage(LPSZ_ZIP_CANT_WRITE);
                    }
                else
                    {
#endif // ifdef NAVNET
                    lpNoNav->Gen.bScanZipFiles = TRUE;
                    if (!bZipInitialized)
                        {
                        ZipInit();
                        bZipInitialized = TRUE;
                        }
#ifndef NAVNET
                    }
#endif // ifdef NAVSCAN
                }
                                        // Only force specified files to be
            else                        // scanned if NON-ZIP
#endif  // *** WARNING!! This IS meant to be between the 'else' and next block.
#endif  // USE_ZIP                
                {
                lpNoNav->Opt.bScanAll = TRUE;
                }
            }
        else
            {
            STRCPY(lpNoNav->szTheDirectory, lpScanList);
            STRCPY(szFile, SZ_STARS);
            }


                                        // Check if we're out of file handles
        if ( FALSE == CheckEnoughFileHandles(lpNoNav, &bHelpOpen) )
            {
            bFileHandlesAvailable = FALSE;
            gbContinueScan = FALSE;
            }
        else
            {
                                        // Do the scan
            VirusScanWildcard ( lpNoNav->szTheDirectory,
                                szFile,
                                (FARPROC) ScanFileAll,
                                lpNoNav->lpScan->bScanSubs,
                                (LPARAM) lpNoNav);
            }

        lpNoNav->Opt.bScanAll = bSaveScanAll;

                                        // Move to the next entry in scan list
        lpScanList = SzzFindNext ( lpScanList ) ;
        }

#ifndef NAVSCAN
    SmartInocClose();
    lpNoNav->uInocOpenOK = SMARTOPEN_CLOSED ;
    
#ifdef USE_ZIP
    if (bZipInitialized)
        {
        ZipCleanup();
        }
#endif  // USE_ZIP
#endif  // !NAVSCAN
                                        // Turn TSR back on.
    TSR_ON;

                                        // Make sure the dialog was updated
                                        // at the end of the scan.
    if (gbContinueScan)                 // If the scan was completed, make
        {                               // sure the fillbar says 100%
        lpNoNav->dwScannedKBytes = lpNoNav->dwTotalKBytes;
        }
    else if (bFileHandlesAvailable)
        {
                                        // (fn takes care of error priority)
        ErrorLevelSet (ERRORLEVEL_INTERRUPT);
        }
    else                                // No file handles
        {

        ErrorLevelSet(ERRORLEVEL_OTHER);// Incompatible environment
        }

    if ( !bHelpOpen )
        InitNavHelp();

    ForceUpdateScanDialog(lpNoNav);

    DiskErrorClearTimeout( ) ;          // Turn off critical error timeouts

    MemUnlock(lpNoNav->lpScan->hScanList, lpScanList);

    DiskSet ( byStartDrive ) ;          // Back to start drive

    ScanStopWatch(lpNoNav, FALSE);  // We're done scanning - stop the clock.

    return (0);
} // End ScanFiles()


//************************************************************************
// ScanFileAll()
//
// This is the callback routine for FIL4_FIND_WILD() called by ScanFiles().
// It checks to see if the scan has been stopped.  If not it scans the
// current file, and updates the scan information.
// This function needs the __loadds modifier because FIL4_FIND_WILD
// changes DS before it calls this callback procedure.  __loadds loads
// the correct DS before the functions starts, and restores the old DS
// when it exits.
//
// Parameters:
//      LPFINDITEM  lpFind              Info on file found, plus our app. info.
//
// Returns:
//      TRUE                            Continue with scan.
//      FALSE                           End scan.
//************************************************************************
// 3/22/93 DALLEE Function taken from Martin N_SCAN.C
//************************************************************************

UINT STATIC WINAPI LOADDS ScanFileAll (LPFINDITEM   lpFind)
{
    extern  DIALOG_RECORD   *glpScanFilesDialog;
    auto    WORD            wEvent;
    auto    LPNONAV         lpNoNav;
    auto    UINT            uNameLen;

    lpNoNav = (LPNONAV) lpFind->dwCookie;

    UpdateScanDialog(lpNoNav);
                                        // Check for Enter, ESC, or button
                                        // hit to stop scan
    if (NULL != glpScanFilesDialog)
        {
        wEvent = DialogProcessEvent(glpScanFilesDialog, DialogGetEvent());
        }
    else
        {
        wEvent = 0;
        }

    if ( ((wEvent == ACCEPT_DIALOG) || (wEvent == ABORT_DIALOG))
         && (lpNoNav->Opt.bAllowScanStop) )
        {
        gbContinueScan = FALSE;
        }
    else
        {
                                        // If this is a directory, update
                                        // the current directory string.
        if (lpFind->bIsDir)
            {                           // Going back up one dir.
            if (!STRCMP(lpFind->szLongName, ".."))
                {
                uDirLevel -= 1;         // back up one!

                if (uDirsTooDeep)
                    {
                    uDirsTooDeep--;
                    }
                else
                    {                   // Go back up a directory.
                    NameStripFile(lpNoNav->szTheDirectory);
                    }
                }
            else                        // We're switching down one dir.
                {
                uDirLevel += 1;         // go down one!

                //--------------------------------------------------------
                // If we are scan
                //--------------------------------------------------------

                if (uDirLevel == 1 &&
                    (lpNoNav->bScanWholeDrive == FALSE &&
                     lpNoNav->lpScan->bScanSubs == TRUE))
                    AddBytesScanned(lpNoNav, DIR_CONST_VALUE);

                if (uDirsTooDeep)
                    {
                    uDirsTooDeep++;
                    }
                else
                    {                   // Add this directory to the old one.
                    if ( STRLEN(lpNoNav->szTheDirectory)
                         + STRLEN(lpFind->szLongName) + 1 > SYM_MAX_PATH )
                        {
                        uDirsTooDeep++;
                        }
                    else
                        {
                        NameAppendFile(lpNoNav->szTheDirectory,
                                       lpFind->szLongName);
                        }
                    }
                }
            if (!uDirsTooDeep)
                {
                ForceUpdateScanDialog(lpNoNav);
                }
            }
                                        // If not a directory, it is a file
        else                            // and we should scan it.
            {
            if (lpNoNav->bScanWholeDrive == TRUE || uDirLevel == 0)
                {
                AddBytesScanned(lpNoNav, lpFind->dwFileSize);
                }

                                        // Make sure the new filename fits
                                        // in standard length buffers.

            uNameLen = STRLEN ( lpNoNav->szTheDirectory ) +
                       STRLEN ( *lpFind->szLongName == '\\' ?
                                lpFind->szLongName + 1:
                                lpFind->szLongName );

            if ( StringGetLastChar ( lpNoNav->szTheDirectory ) != '\\' )
                {
                uNameLen++;
                }

            if ( !uDirsTooDeep &&
                 uNameLen < SYM_MAX_PATH )
                {
                if (ExtInList(lpNoNav, lpFind->szLongName))
                    {
                                        // Copy the full path of the file
                                        // we're scanning.
                    STRCPY(lpNoNav->Infected.szFullPath,
                           lpNoNav->szTheDirectory);

                    NameAppendFile(lpNoNav->Infected.szFullPath,
                                   lpFind->szLongName);

                                       // This is DOS. Everything is OEM
                    STRCPY (lpNoNav->Infected.szFullOemPath,
                            lpNoNav->Infected.szFullPath);

                    StartScanFile(lpNoNav, lpNoNav->Infected.szFullPath);
                    }
                }
            }
        }

    return (gbContinueScan);
} // End ScanFileAll()


//************************************************************************
// StartScanFile()
//
// This routine calls ScanFile() to scan a file for viruses, then updates
// the scan info in *lpNoNav.
//
// Parameters:
//      SCAN    *lpScanData             Info on viruses and signitures, etc.
//
//      LPNONAV lpNav                   This particular scan's info.
//
//      LPSTR   szFile                  Full path of file to scan.
//
// Returns:
//      0xFFFF                          File couldn't be opened or DOS error
//      0                               File not infected.
//      Non-zero                        File infected - this is the Virus ID
//************************************************************************
// 3/22/93 DALLEE Function lifted from Martin N_SCAN.C
// 8/03/93 DALLEE Fewer conditionals.
// 9/11/93 DALLEE Check exclusions BEFORE scan.
//************************************************************************

UINT PASCAL StartScanFile(LPNONAV lpNoNav, LPSTR szFile)
{
    auto    UINT        uVirusID = 0;
    auto    BOOL        bIsExecutable;
    auto    LPSTR       lpStr;
    auto    BOOL        bFileIsExcludedForVirus;

    lpStr = szFile;

//moved Exclude to after scan -CGRANGE
        bIsExecutable = FileIsDOSExecutable(lpStr);

        uVirusID = VirusScanFile(szFile,&lpNoNav->Infected.Notes,bIsExecutable);
        lpNoNav->Stats.Files.dwScanned++;

        if ((uVirusID) && (uVirusID!=0xFFFF))
          {
          if (FALSE == ExcludeIs(lpNoNav->lpExclude, lpStr, excVIRUS_FOUND))
            {
            bFileIsExcludedForVirus=FALSE;
            lpNoNav->Infected.uVirusSigIndex = uVirusID;

            //lpNoNav->Infected.wStatus = FILESTATUS_INFECTED;
            //RespondToVirus(lpNoNav);
            if (lpNoNav->Zip.bInZip == FALSE)
                {
                lpNoNav->Infected.wStatus = FILESTATUS_INFECTED;
                RespondToVirus(lpNoNav);
                }
            else                            // Special handling for zip files...
                {
                lpNoNav->Infected.wStatus = FILESTATUS_ZIPINFECTED;
                RespondToVirusInZip(lpNoNav);
                }

            }
          else
            {
            bFileIsExcludedForVirus=TRUE;
            }
          }

                                        // If the file is uninfected,
                                        // and inoculation technology is
                                        // turned on, check the inoculation
                                        // state of the file and act on it.
#ifndef NAVSCAN
          if ( ((0 == uVirusID) || (bFileIsExcludedForVirus)) &&
               (lpNoNav->Inoc.bUseInoc) )
              {
              FileInoculated(lpNoNav);
              }
#endif

    return (uVirusID);
} // End StartScanFile()


//***************************************************************************
// CheckEnoughFileHandles -- Returns TRUE if there are enough handles to scan
//
// Description:
//      Tries to get enough handles for scanning.
//      First, it will try to close the help file to get enough handles
//      If that fails, you can't scan.
//
// Parameters:
//      lpbHelpOpen  [IN/OUT] Sets to FALSE if necessary to enable scan
//                            (and actually closed help)
//      NOTE: Both parameters must be initialized.
//
// Return Value:
//      TRUE  -- Enough file handles to scan
//      FALSE -- Stop scan (not enough handles)
//
//***************************************************************************
// 09/01/1994 BARRY Function Created.
//***************************************************************************

BOOL LOCAL PASCAL CheckEnoughFileHandles(LPNONAV lpNoNav, BOOL * lpbHelpOpen)
{
    extern  char        *SZ_OUT_OF_FILE_HANDLES[];

    auto    char        szNavExe [SYM_MAX_PATH];
    auto    HFILE       hTestFile1 = HFILE_ERROR;
    auto    HFILE       hTestFile2 = HFILE_ERROR;
    auto    HFILE       hTestFile3 = HFILE_ERROR;
    auto    BOOL        bRv = TRUE;

    DOSGetProgramName(szNavExe);

    hTestFile1 = FileOpen(szNavExe,OF_SHARE_DENY_NONE | READ_ONLY_FILE);
    if (hTestFile1 == HFILE_ERROR && (0x04 == DOSGetExtendedError()))
        {
                                        // If first fails, we're sunk
        NavDlgError(SZ_OUT_OF_FILE_HANDLES);
        bRv = FALSE;
        }

    if ( bRv )
        {
        hTestFile2 = FileOpen(szNavExe,OF_SHARE_DENY_NONE | READ_ONLY_FILE);
        if (hTestFile2 == HFILE_ERROR && (0x04 == DOSGetExtendedError()))
            {
            HelpDone();                     // See if closing help helps

                                        // Try opening second file again
            hTestFile2 = FileOpen(szNavExe,OF_SHARE_DENY_NONE | READ_ONLY_FILE);
            if (hTestFile2 == HFILE_ERROR && (0x04 == DOSGetExtendedError()))
                {
                if ( *lpbHelpOpen )
                    InitNavHelp();          // Still didn't work, reopen help

                bRv = FALSE;
                }
            else
                {
                *lpbHelpOpen = FALSE;       // We've closed help to get success
                }
            }
        }

    if ( hTestFile3 != HFILE_ERROR )
        FileClose(hTestFile3);

    if ( hTestFile2 != HFILE_ERROR )
        FileClose(hTestFile2);

    if ( hTestFile1 != HFILE_ERROR )
        FileClose(hTestFile1);

    return bRv;
}
