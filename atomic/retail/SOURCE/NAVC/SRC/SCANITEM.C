// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/scanitem.c_v   1.4   07 Jul 1997 18:04:28   MKEATIN  $
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
// $Log:   S:/NAVC/VCS/scanitem.c_v  $
// 
//    Rev 1.4   07 Jul 1997 18:04:28   MKEATIN
// We were calling RespondToVirus() too manny times when were in /prompt mode.
// 
//    Rev 1.3   12 Jun 1997 18:39:42   MKEATIN
// Removed the auto variable, wEvent, from ScanFilesAll.  This used to be
// initailzed by the UI Dialog which we no longer use in NAVC.
// 
//    Rev 1.2   09 Jun 1997 18:35:28   BGERHAR
// Add AreDefsWild() and misc. clean-up
// 
//    Rev 1.1   08 Jun 1997 23:24:20   BGERHAR
// Don't wrap for deep scan or def directories
// 
//    Rev 1.0   06 Feb 1997 20:56:24   RFULLER
// Initial revision
// 
//    Rev 1.4   17 Jan 1997 15:41:16   MKEATIN
// Added the /PROMPT switch for navc.
// 
//    Rev 1.3   16 Jan 1997 14:08:54   MKEATIN
// If an infected file failed to be repaired or deleted during a startup 
// scan, we now prompt the user for further action.
// 
//    Rev 1.2   14 Jan 1997 16:56:32   MKEATIN
// Test the temporary zip scanning directory with CreateFileTempory() instead
// of CreateFileTemp().
// 
//    Rev 1.1   06 Jan 1997 21:15:50   MKEATIN
// Added startup scanning functionality.
// 
//    Rev 1.0   31 Dec 1996 15:20:12   MKEATIN
// Initial revision.
// 
//    Rev 1.9   03 Dec 1996 13:22:24   JBELDEN
// added display of zip scans and fixed problem of last file
// scanned being displayed.
// 
//    Rev 1.8   02 Dec 1996 10:22:12   JBELDEN
// replaced a stddlgmessage with printf
// 
//    Rev 1.7   22 Nov 1996 13:38:00   JBELDEN
// fixed problem with file names not being cleared properly during a scan.
// 
//    Rev 1.6   12 Nov 1996 17:01:08   JBELDEN
// replaced navdlgerror with appropriate printf strings
// 
//    Rev 1.5   31 Oct 1996 18:05:48   JBELDEN
// removed more dialogbox code
// 
//    Rev 1.4   31 Oct 1996 13:32:24   JBELDEN
// removed call to fileinoc since inoculation is being removed
// 
//    Rev 1.3   15 Oct 1996 11:25:18   JBELDEN
// added printf's for infected files in compressed files
// 
//    Rev 1.2   14 Oct 1996 11:04:44   JBELDEN
// changed DOSPrint's to printf and added fprintf for logging
//
//    Rev 1.1   04 Oct 1996 16:33:24   JBELDEN
// put in DOSPrint for scanning files and removed a few forceupdatedialog();
//
//    Rev 1.0   02 Oct 1996 12:58:42   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
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
#ifndef NAVSCAN
BOOL bAllowZipScanning = TRUE;          // If we can't create temp files, disable
#endif

extern FILE    *fLogFile;
extern BOOL    bAutoExportLog;

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

#ifndef NAVSCAN
#ifdef USE_ZIP

#define TEMP_FILE_SIZE_NEEDED 65536L

VOID PASCAL ValidateZipOverlaying(VOID)
{
    auto        char            szTempFile[MAX_PATH_SIZE + 1];
    auto        HFILE           hFile;
    auto        DWORD           dwCount;
    auto        UINT            uWrite;

    if ( HFILE_ERROR != (hFile = FileCreateTemporary (szTempFile, 0)) )
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
    extern  char        SZ_OUT_OF_MEMORY[];
    extern  char        SZ_STARS[];
#ifndef NAVSCAN
    extern  BOOL        bAllowZipScanning;
    extern  char        SZ_ZIP_CANT_WRITE[];
#endif
    extern  BOOL PASCAL InitNavHelp();  // In NAVD/NAVD.C
    extern  NAVOPTIONS  navOptions;
    extern  char        szBlankLine[];
    extern  char        szScanningFileEnd[];

    auto    LPSTR       lpScanList;
    auto    BOOL        bFileHandlesAvailable = TRUE;
    auto    BOOL        bHelpOpen=TRUE; // Assume help is open
    auto    char        szFile [SYM_MAX_PATH];
#ifdef USE_ZIP
    auto    char        szExt  [MAX_EXTENSION_SIZE];
    auto    BOOL        bInformedNoZip = FALSE;
#endif
    auto    BYTE        byStartDrive;
    auto    BOOL        bSaveScanAll;   // Temporarily overridden on some scans
    auto    BOOL        bZipInitialized = FALSE;
#ifndef NAVSCAN
    extern  BOOL            bAllowZipScanning;
#endif


    if (NULL == (lpScanList = MemLock(lpNoNav->lpScan->hScanList)) )
        {
        printf(SZ_OUT_OF_MEMORY);
        return (ERR);
        }

    TSR_OFF;                            // Disable TSR first.
#ifdef TTY_SCAN
    lpNoNav->dwTotalKBytes =0L;
#else
    lpNoNav->dwTotalKBytes = GetBytesToScan( lpScanList,
                                             lpNoNav->lpScan->bScanSubs,
                                             GetBytesSkipDrive,
                                             GetBytesDriveError,
                                             (DWORD) lpNoNav );
#endif

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
    ValidateZipOverlaying();

    if (lpNoNav->Gen.bScanZipFiles && !bAllowZipScanning)
        {
        lpNoNav->Gen.bScanZipFiles = FALSE;
        printf(SZ_ZIP_CANT_WRITE);
        bInformedNoZip = TRUE;
        }

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

#ifndef TTY_SCAN
        if (DiskIsNetwork(*lpScanList))
            {
            auto    WORD            wNetSecPerBlock;

            if (NOERR == NWGetVolumeInfo((BYTE)(*lpScanList - 'A'),
                                         &wNetSecPerBlock,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL) )
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

#endif // TTY_SCAN
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
            if (STRCMP(szExt, "ZIP") == 0)
                {                       // Turn on zip scanning if a Zip file
                                        // is explicitly chosen.

                                        // ValidateZipOverlaying called
                                        // previously.
                if (bAllowZipScanning)
                    {
                    lpNoNav->Gen.bScanZipFiles = TRUE;
                    if (!bZipInitialized)
                        {
                        ZipInit();
                        bZipInitialized = TRUE;
                        }
                    }
                else if (!bInformedNoZip)
                    // We did not already give this message above.
                    {
                    printf(SZ_ZIP_CANT_WRITE);
                    }

                }
                                        // Only force specified files to be
            else                        // scanned if NON-ZIP
#endif  // USE_ZIP This belongs after the else, before the next block.
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

    printf(szBlankLine);

                                        // Make sure the dialog was updated
                                        // at the end of the scan.
    if (gbContinueScan)                 // If the scan was completed, make
        {                               // sure the fillbar says 100%
#ifndef TTY_SCAN
        lpNoNav->dwScannedKBytes = lpNoNav->dwTotalKBytes;
#endif
        printf (szScanningFileEnd);
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

    printf("\n");

    if ( !bHelpOpen )
        InitNavHelp();


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

    auto    WORD            wEvent;
    auto    LPNONAV         lpNoNav;
    auto    UINT            uNameLen;

    lpNoNav = (LPNONAV) lpFind->dwCookie;

                                        // If this is a directory, update
                                        // the current directory string.
    if (lpFind->bIsDir)
        {                               // Going back up one dir.
        if (!STRCMP(lpFind->szLongName, ".."))
            {
            uDirLevel -= 1;             // back up one!

            if (uDirsTooDeep)
                {
                uDirsTooDeep--;
                }
            else
                {                       // Go back up a directory.
                NameStripFile(lpNoNav->szTheDirectory);
                }
            }
        else                            // We're switching down one dir.
            {
            uDirLevel += 1;             // go down one!

                //--------------------------------------------------------
                // If we are scan
                //--------------------------------------------------------

#ifndef TTY_SCAN
            if (uDirLevel == 1 &&
                (lpNoNav->bScanWholeDrive == FALSE &&
                 lpNoNav->lpScan->bScanSubs == TRUE))
                AddBytesScanned(lpNoNav, DIR_CONST_VALUE);
#endif
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
                
        }
                                        // If not a directory, it is a file
    else                                // and we should scan it.
        {
#ifndef TTY_SCAN
        if (lpNoNav->bScanWholeDrive == TRUE || uDirLevel == 0)
            {
            AddBytesScanned(lpNoNav, lpFind->dwFileSize);
            }
#endif

        
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
            
                StartScanFile(lpNoNav, lpNoNav->Infected.szFullPath);    
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

UINT PASCAL StartScanFile(LPNONAV lpNoNav, LPSTR lpszFile)
{
    extern  char        SZ_ZIP_INFECTED_FORMAT[];
    extern  char        SZ_REPAIR_DELETE_CONTINUE[];
    extern  char        IDS_RAP_ZIPINFECTED[];
    extern  char        szFileInfected[];
    extern  BOOL        bStartupMode;
    extern  BOOL        bNavcPrompt;
    extern  char        szScanningFile[];
    extern  char        szScanningZipFile[];
    extern  char        szBlankLine[];
    extern  const char  gnScanningFileWidth;
    extern  const char  gnScanningZipFileWidth;

    auto    UINT        uVirusID = 0;
    auto    int         nZipFileLen;
    auto    int         nZipChildLen;
    auto    BOOL        bIsExecutable;
    auto    BOOL        bFileIsExcludedForVirus;
    auto    char        szFileTmp[80];
    auto    WORD        ch = 0;
    auto    UINT        uSaveAction;

    bIsExecutable = FileIsDOSExecutable(lpszFile);

    printf(szBlankLine);
    
    if ( lpNoNav->Zip.bInZip == TRUE )
      {
      nZipFileLen = strlen(lpNoNav->Zip.lpZipFileName);
      nZipChildLen = strlen(lpNoNav->Zip.szZipChildFile);

      if (gnScanningZipFileWidth < nZipFileLen + nZipChildLen)
          {
          SYM_ASSERT (gnScanningZipFileWidth > nZipChildLen);
          nZipFileLen =  gnScanningZipFileWidth - nZipChildLen;
          }

      NameShortenFileName(lpNoNav->Zip.lpZipFileName, szFileTmp, nZipFileLen);
      printf(szScanningZipFile, lpNoNav->Zip.szZipChildFile, szFileTmp);
      }
    else
      {
      NameShortenFileName(lpNoNav->Infected.szFullPath, szFileTmp, gnScanningFileWidth);
      printf(szScanningFile, szFileTmp);
      }

    uVirusID = VirusScanFile(lpszFile,&lpNoNav->Infected.Notes,bIsExecutable);
    lpNoNav->Stats.Files.dwScanned++;

    if ((uVirusID) && (uVirusID!=0xFFFF))
      {
      if (FALSE == ExcludeIs(lpNoNav->lpExclude, lpszFile, excVIRUS_FOUND))
        {
        if (0 != uVirusID)
           {
           if ( lpNoNav->Zip.bInZip == TRUE )
              {
              printf(SZ_ZIP_INFECTED_FORMAT, lpNoNav->Zip.szZipChildFile,
                     lpNoNav->Zip.lpZipFileName, lpNoNav->Infected.Notes.lpVirName);
              printf(IDS_RAP_ZIPINFECTED);
              }
           else
              {
              if ( bStartupMode )
                printf("\n\n");
              printf(szFileInfected, lpNoNav->Infected.szFullPath,
                       lpNoNav->Infected.Notes.lpVirName);
              }

           if ( bAutoExportLog )
              if ( lpNoNav->Zip.bInZip == TRUE )
                 {
                 fprintf(fLogFile, SZ_ZIP_INFECTED_FORMAT, lpNoNav->Zip.szZipChildFile,
                         lpNoNav->Zip.lpZipFileName, lpNoNav->Infected.Notes.lpVirName);
                 fprintf(fLogFile, IDS_RAP_ZIPINFECTED);
                 }
              else
                 fprintf(fLogFile, szFileInfected, lpNoNav->Infected.szFullPath,
                       lpNoNav->Infected.Notes.lpVirName);
           }

        bFileIsExcludedForVirus=FALSE;
        lpNoNav->Infected.uVirusSigIndex = uVirusID;

        if (lpNoNav->Zip.bInZip == FALSE)
            {
            lpNoNav->Infected.wStatus = FILESTATUS_INFECTED;


                                    // This is where we prompt the user
                                    // when a virus is detected during 
                                    // a startup scan or a /PROMPT
            uSaveAction = lpNoNav->Opt.uAction;
            while (bStartupMode || bNavcPrompt)
                {   
                if (lpNoNav->Alert.bBeep)
                    MUS4_BEEP(BEEP_KNOWNBOX);    
                printf("\n%s ", SZ_REPAIR_DELETE_CONTINUE);    
                ch = 0;    

                while (_kbhit())    
                    getch();        // clear keyboard buffer    

                for (;;)    
                    {    
                    switch (ch)    
                        {    
                        case 'R':    
                            lpNoNav->Opt.uAction = SCAN_REPAUTO;    
                            putchar(ch);    
                            break;    

                        case 'D':    
                            lpNoNav->Opt.uAction = SCAN_DELAUTO;    
                            putchar(ch);    
                            break;    

                        default:    
                            lpNoNav->Opt.uAction = SCAN_REPORT;
                            lpNoNav->Infected.wStatus = FILESTATUS_INFECTED;
                            break;    
                        }    

                    if (ch == 'R' || ch == 'D' || ch == 'C' || ch == 27 )    
                        break;    
                    ch = getch();
                    if (islower(ch))
                        ch = toupper(ch);    
                    }   

                printf("\n\n");     

                RespondToVirus(lpNoNav);

                if (lpNoNav->Infected.wStatus != FILESTATUS_NOTREPAIRED &&
                    lpNoNav->Infected.wStatus != FILESTATUS_NOTDELETED)
                    {
                    printf("\n");
                    break;
                    }
                else
                    {
                    lpNoNav->Stats.Files.uInfected--;
                    }
                }  
            lpNoNav->Opt.uAction = uSaveAction;
            if (!bStartupMode && !bNavcPrompt)
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
    extern  char        SZ_OUT_OF_FILE_HANDLES[];

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
        printf(SZ_OUT_OF_FILE_HANDLES);
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
