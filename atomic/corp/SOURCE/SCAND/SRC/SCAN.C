// Copyright 1992-1993 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/SCAND/VCS/scan.c_v   1.2   15 Aug 1997 10:19:12   JBRENNA  $
//
// Description:
//  This handles all items in the Scan menu.
//
// See Also:
//***********************************************************************
// $Log:   S:/SCAND/VCS/scan.c_v  $
// 
//    Rev 1.2   15 Aug 1997 10:19:12   JBRENNA
// Add NULL to VirusScanInit call. Should we be passing <NAVDIR>\NAVEX15.INF?
// 
//    Rev 1.1   17 Jun 1997 18:06:02   MKEATIN
// Now passing &gstNAVCallBacks in VirusScanInit().
// 
//    Rev 1.0   06 Feb 1997 21:08:44   RFULLER
// Initial revision
// 
//    Rev 1.2   08 Aug 1996 15:18:54   JBRENNA
// LuigiPlus Port:
//   JWORDEN: Pass file name in Infected.szFullOemPath to match shared
//     functions in NAVXUTIL
// 
//   MZAREMB: Fixed STS#54954 in which floppy inoc was not working.  Added
//     code to turn floppy inoc "on" for an immediate inoc & "off" again
//     when finished.
// 
// 
//    Rev 1.1   08 Aug 1996 13:26:24   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.38   30 Jul 1996 12:24:22   JALLEE
// Added custom alert message to MBR / Boot , SYSINTEG, Inoculation dialogs.
// 
//    Rev 1.37   13 Jun 1996 14:04:40   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.36   29 Nov 1995 22:16:36   MKEATIN
// fixed dialog messages
// 
//    Rev 1.35   21 Jun 1995 11:41:08   SZIADEH
// activated mus4_beep()
// included ctsn.h
// 
//    Rev 1.34   30 Mar 1995 14:10:22   DALLEE
// In ScanInoculateSingleFile() need to setup global glpNoNav before
// calling ScanActionMessage().
// 
//    Rev 1.33   07 Mar 1995 20:04:02   DALLEE
// Use NavDlgSelectDir() instead of StdDlgSelectDir().
// 
//    Rev 1.32   06 Feb 1995 16:45:32   DALLEE
// Removed bUpdateTSR.
// SmartInocOpen() takes full path to file we need to open for -- necessary
// for navinoc.dat vs. navinocl.dat.
// 
//    Rev 1.31   29 Dec 1994 16:46:20   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.30   28 Dec 1994 14:16:10   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.3   27 Dec 1994 15:41:52   DALLEE
    // Commented out MUS4_BEEP.
    //
    //    Rev 1.2   12 Dec 1994 18:10:56   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.1   05 Dec 1994 18:10:14   DALLEE
    // CVT1 script.
    //
//    Rev 1.29   28 Dec 1994 13:53:46   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:14   DALLEE
    // Initial revision.
    //
    //    Rev 1.22   14 Sep 1993 22:26:06   BARRY
    // Now setting DOS Error Level when a virus is found
//***********************************************************************

#ifndef NAVNET

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "stddos.h"
#include "stddlg.h"
#include "tsr.h"
#include "tsrcomm.h"
#include "ctsn.h"
#include "virscan.h"

#include "navdprot.h"

#include "options.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "inoc.h"
#include "navcb.h"
#ifdef USE_NETWORKALERTS   //&?
 #include "netalert.h"
#endif

#include "scand.h"

MODULE_NAME;

//************************************************************************
// DEFINES
//************************************************************************

                                        // Space for 26 "C:\<EOS>" plus
                                        // a zero-length string to end it
#define SIZE_SCANLIST       (4 * 26 + 1)


//************************************************************************
// GLOBAL VARS
//************************************************************************
                                        // Keep track of whether we're
UINT guTypeScan = SCAN_VIRUSES;         // scanning for viruses or
                                        // inoculating files.
                                        // SCAN_VIRUSES or SCAN_INOCULATION

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID  PASCAL BuildScanList (LPSTR lpScanList, LPCSTR szSelectedDrives);
BOOL  STATIC PASCAL CheckFileBeforeScan (LPSTR lpszFile);
VOID  LOCAL  PASCAL ScanInoculateSingleFile (LPSTR lpszFile);


//************************************************************************
// DoScanDrives()
//
// This routine gets the selected drives, and then launches the scanner.
//
// Parameters:
//      None
//
// Returns:
//      Nothing
//************************************************************************
// 4/5/93 DALLEE, Function created.
// 4/7/93 DALLEE, Use LPSCANSTRUCT when calling ScanStart()
//************************************************************************

VOID PASCAL DoScanDrives (VOID)
{
    extern  char        szSelectedDrives[];

    auto    SCANSTRUCT  rScan;
    auto    LPSTR       lpScanList;

    MEMSET(&rScan, '\0', sizeof(rScan));

    ScanDriveGetList();                 // Update szSelectedDrives[];

    if (NULL != (rScan.hScanList = MemAlloc(GHND, SIZE_SCANLIST)))
        {
        lpScanList = MemLock(rScan.hScanList);
        BuildScanList(lpScanList, szSelectedDrives);
        MemUnlock(rScan.hScanList, lpScanList);

        rScan.bScanSubs = TRUE;
	rScan.wCallWhenDone = PROGRAM_INTERACTIVE;

        ScanStart(&rScan);
        MemFree(rScan.hScanList);
        }
} // End DoScanDrives()


//************************************************************************
// DoScanDirectory()
//
// This routine gets the selected directory and then launches the scanner.
//
// Parameters:
//      None.
//
// Returns:
//      TRUE                            Directory was scanned.
//      FALSE                           No directory was selected.
//************************************************************************
// 3/??/93 BRAD?
// 4/7/93 DALLEE, Use LPSCANSTRUCT when calling ScanStart()
//************************************************************************

BYTE PASCAL DoScanDirectory (VOID)
{
    extern      CheckBoxRec     scanSubDirCheckBox;
    extern      ButtonsRec      buScanCancel;
    extern      char            SZ_SELECT_DIR_TITLE[];
    extern      BOOL            bScanSubDir;

    auto        BYTE            byResult;
    auto        char            szFileName[SYM_MAX_PATH];
    auto        SCANSTRUCT      rScan;
    auto        char            *lpScanList;



    HyperHelpTopicSet(HELP_DLG_SCAN_DIR);
    MEMSET(&rScan, '\0', sizeof(rScan));

                                        // Add space for 2 EOS's to make
                                        // an SZZ which ScanStart expects
    if (NULL != (rScan.hScanList = MemAlloc(GHND, SYM_MAX_PATH + 2)))
        {
        lpScanList = MemLock(rScan.hScanList);
        szFileName[0] = EOS;
                                        // Get the directory to scan
        byResult = NavDlgSelectDir( szFileName,
                                    SZ_SELECT_DIR_TITLE,
                                    &scanSubDirCheckBox,
                                    &buScanCancel,
                                    FALSE );

                                        // Scan it
        if ( byResult )
            {
            NameToFull(lpScanList, szFileName);
            STRUPR(lpScanList);
            MemUnlock(rScan.hScanList, lpScanList);

            rScan.bScanSubs = scanSubDirCheckBox.value;
	    rScan.wCallWhenDone = PROGRAM_INTERACTIVE;

            ScanStart(&rScan);
            }
        MemFree(rScan.hScanList);
        }
    else
        {
        byResult = FALSE;
        }

    return ( byResult );
}


//************************************************************************
// DoScanFile()
//
// This routine gets the selected file and then launches the scanner.
//
// Parameters:
//      None.
//
// Returns:
//      TRUE                            File was scanned.
//      FALSE                           No file was selected.
//************************************************************************
// 3/??/93 BRAD?
// 4/7/93 DALLEE, Use LPSCANSTRUCT when calling ScanStart()
//************************************************************************

BYTE PASCAL DoScanFile (VOID)
{
    extern  ButtonsRec      buScanCancel;
    extern  char            SZ_SELECT_FILE_TITLE[];
    extern  char            SZ_STARS[];
    auto    BYTE            byResult;
    auto    char            szFileName[SYM_MAX_PATH];

    auto    SCANSTRUCT      rScan;
    auto    char            *lpScanList;



    HyperHelpTopicSet(HELP_DLG_SCAN_FILE);
    MEMSET(&rScan, '\0', sizeof(rScan));

                                        // Add space for 2 EOS's to make
                                        // an SZZ which ScanStart expects
    if (NULL != (rScan.hScanList = MemAlloc(GHND, SYM_MAX_PATH + 2)))
        {
        lpScanList = MemLock(rScan.hScanList);

        TSR_OFF;

        STRCPY(szFileName, SZ_STARS);

        do  {
                                        // Get the file to scan
            byResult = (BYTE) StdDlgBrowse (szFileName,
                                            szFileName,
                                            STDDLG_FIND_HID_SYS,
                                            SZ_SELECT_FILE_TITLE,
                                            MAX_SELECT_FILES,
                                            NULL,
                                            &buScanCancel);
            }
            while (byResult && !CheckFileBeforeScan(szFileName));

        TSR_ON;

        if ( byResult )
            {
            NameToFull(lpScanList, szFileName);
            STRUPR(lpScanList);
            MemUnlock(rScan.hScanList, lpScanList);

            rScan.bScanSubs = FALSE ;
	    rScan.wCallWhenDone = PROGRAM_INTERACTIVE;

            ScanStart(&rScan);
            }
        MemFree(rScan.hScanList);
        }
    else
        {
        byResult = FALSE;
        }

    return ( byResult );
} // End DoScanFile()


//************************************************************************
// CheckFileBeforeScan()
//
// This routine complains to the user if the file specified for scanning
// can not be found.
//
// Parameters:
//      LPSTR  lpszFile                 File specified for scanning.
//
// Returns:
//      TRUE                            File exists.
//      FALSE                           File can't be found.
//************************************************************************
// 8/31/93 DALLEE, Function created.
//************************************************************************

BOOL STATIC PASCAL CheckFileBeforeScan (LPSTR lpszFile)
{
    extern  char    *LPSZ_FILE_NOT_FOUND [];

    if (!FileExists(lpszFile))
        {
        StdDlgMessage(LPSZ_FILE_NOT_FOUND, lpszFile);
        return (FALSE);
        }

    return (TRUE);
} // End CheckFileBeforeScan()


//************************************************************************
// BuildScanList()
//
// This routine builds an szz of selected drives in the form
//  "A:\<EOS>B:\<EOS>C:\<EOS><EOS>" from a list of drive letters.
//
// Parameters:
//      LPSTR   lpScanList              Pointer to buffer for the new list.
//      LPCSTR  szSelectedDrives        SZ of selected drive letters
//
// Returns:
//      Nothing.
//************************************************************************
// 3/24/93 DALLEE Function created.
//************************************************************************

VOID PASCAL BuildScanList (LPSTR lpScanList, LPCSTR szSelectedDrives)
{
    auto    LPSTR   lpLetters;
    auto    LPSTR   lpList = lpScanList;

                                        // For each drive letter...
    for (lpLetters = (LPSTR) szSelectedDrives; *lpLetters != EOS; lpLetters++)
        {
                                        // Get the drive letter, append
                                        // ":\" and EOS then advance the pointer
        *lpList++ = *lpLetters;
        STRCPY(lpList, ":\\");
        lpList += (STRLEN(lpList) + 1);
        }

    *lpList = EOS;                      // Terminate with a zero length string.
} // End BuildScanList()


//************************************************************************
// DoInocFiles()
//
// This routine is called from Tools | Inoculation to perform inoculation
// of multiple files.
//
// Parameters:
//      LPSTR   lpFileSpec              Specification of files to inoculate.
//      BOOL    bSubDirs                Scan subdirectories?
//
// Returns:
//      Nothing.
//************************************************************************
// 5/25/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL DoInocFiles (LPSTR lpFileSpec, BOOL bSubDirs)
{
    extern  NAVOPTIONS  navOptions;
    extern  LPSTR       LPSZ_FILE_NOT_FOUND [];

    auto    BOOL        bAllFiles;
    auto    BOOL        bImmediateNotify;
    auto    BOOL        bOrigUseInoc;
    auto    BOOL        bScanZipFiles;
    auto    BOOL        bInocFloppies;
    auto    UINT        uOrigNewInoc;
    auto    char        szLocalPath [SYM_MAX_PATH];
    auto    SCANSTRUCT  rScan;
    auto    char        *lpScanList;
    auto    UINT        uOldTypeScan;

    MEMSET(&rScan, '\0', sizeof(rScan));

    if (ERR == NameToFull(szLocalPath, lpFileSpec))
        {
        StdDlgMessage(LPSZ_FILE_NOT_FOUND, lpFileSpec);
        }
    else if (FileExists(szLocalPath))
        {
        ScanInoculateSingleFile(szLocalPath);
        }
    else
        {
                                        // Kludges start here to force the
                                        // scanner to do the inoculation
                                        // actions that we want and save
                                        // the original options.
        bAllFiles        = navOptions.scanner.bScanAll;
        bImmediateNotify = navOptions.scanner.bPrestoNotify;
        bScanZipFiles    = navOptions.general.bScanZipFiles;
        bOrigUseInoc     = navOptions.inoc.bUseInoc;
        uOrigNewInoc     = navOptions.inoc.uActionNewInoc;
        bInocFloppies    = navOptions.inoc.bInocFloppies;

        navOptions.scanner.bScanAll      = FALSE;
        navOptions.scanner.bPrestoNotify = FALSE;
        navOptions.general.bScanZipFiles = FALSE;
        navOptions.inoc.bUseInoc         = TRUE;
        navOptions.inoc.uActionNewInoc   = INOC_AUTO_FILEONLY;
        navOptions.inoc.bInocFloppies    = TRUE;
                                        // End of Kludges for now.


        if (NULL != (rScan.hScanList = MemAlloc(GHND, SYM_MAX_PATH + 2)))
            {
            lpScanList = MemLock(rScan.hScanList);
            STRCPY(lpScanList, szLocalPath);
            MemUnlock(rScan.hScanList, lpScanList);

	    rScan.wCallWhenDone = PROGRAM_INTERACTIVE;
            rScan.bScanSubs = bSubDirs;

            uOldTypeScan = guTypeScan;
            guTypeScan = SCAN_INOCULATION;

            ScanStart(&rScan);

            guTypeScan = uOldTypeScan;
            MemFree(rScan.hScanList);
            }
                                        // Restore the options with which
                                        // we messed.
        navOptions.scanner.bScanAll      = bAllFiles;
        navOptions.scanner.bPrestoNotify = bImmediateNotify;
        navOptions.general.bScanZipFiles = bScanZipFiles;
        navOptions.inoc.bUseInoc         = bOrigUseInoc;
        navOptions.inoc.uActionNewInoc   = uOrigNewInoc;
        navOptions.inoc.bInocFloppies    = bInocFloppies;
        }

} // End DoInocFiles()


//************************************************************************
// ScanInoculateSingleFile()
//
// This routine scans a single file, inoculates it, and logs the action.
//
// Parameters:
//      LPSTR   lpszFile                Full path to file.
//
// Returns:
//      Nothing
//************************************************************************
// 9/10/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL ScanInoculateSingleFile (LPSTR lpszFile)
{
    extern  NAVOPTIONS      navOptions;
    extern  LPSTR           LPSZ_NO_DAT_FILES           [];
    extern  LPSTR           LPSZ_UNABLE_TO_UPDATE_INOC  [];
    extern  char            SZ_FILE_NOT_INOCULATED      [];
    extern  char            SZ_EMPTY [];

    extern  DIALOG          dlVirusFound;
    extern  ButtonsRec      okButton;
    extern  char            *lpszVirusFoundStrings[];

    auto    INFECTEDFILE    rInfected;
    auto    LPSTR           lpszAlertMessageNext;
    auto    char            lpAlertMessageBuffer [2][50 + 1];
    auto    char            lpMessageBuffer [5][50 + 1];
    auto    LPSTR           lpszMessage [5] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };
    auto    UINT            uResult;
    auto    LOGSTRUCT       Log;
#ifdef USE_NETWORKALERTS   //&?
    auto    NETALERT        rAlert;
#endif
    auto    LPNONAV         lpNoNav;
    extern  LPNONAV         glpNoNav;

    lpNoNav = (NONAV *) MemAllocPtr(GHND, sizeof(NONAV));

    if (NULL == lpNoNav)
        {                               // Rude bail out if no memory
        return;                         // Should really never occur.
        }
                                        // Read necessary info from the .INI
    LoadNoNavIni(lpNoNav);              // file.

                                        // Need this info for
    glpNoNav = lpNoNav;                 // ScanActionMessage().


                                        // Make sure we can open the inocdb
                                        // on the specified drive.
    if (SMARTOPEN_ERROR == SmartInocOpen(lpszFile, TRUE))
        {
        StdDlgMessage(LPSZ_UNABLE_TO_UPDATE_INOC);
        }
    else                                // Initialize Virscan lib.
        {
        uResult = VirusScanInit(&gstNAVCallBacks, NULL);
        if (NOERR_DAT_CHANGED == uResult)
            {
            uResult = NOERR;
            }

        if (NOERR != uResult)
            {
            StdDlgError(LPSZ_NO_DAT_FILES);
            }
        else                            // Virscan initialized OK.
            {
            STRCPY(rInfected.szFullPath, lpszFile);
            STRCPY(rInfected.szFullOemPath, lpszFile);

            if ( VirusScanFile(lpszFile, &rInfected.Notes, TRUE) &&
                 !ExcludeIs(&navOptions.exclude, lpszFile, excVIRUS_FOUND) )
                {
                if (navOptions.alert.bBeep)
                    {
                    MUS4_BEEP(BEEP_KNOWNBOX);
                    }

#ifdef USE_NETWORKALERTS   //&?
                MEMSET(&rAlert, '\0', sizeof(rAlert));
                rAlert.lpNet        = navOptions.netUser.lpNetUserItem;
                rAlert.lpInfected   = &rInfected;
                rAlert.lpAlertOpt   = &navOptions.alert;
                rAlert.uTypeOfEvent = EV_ALERT_KNOWN_VIRUS;

                NetAlert(&rAlert);
#endif
                ErrorLevelSet ( ERRORLEVEL_VIRUSFOUND ) ;

                                        // This file is infected.
                                        // Set up the virus found dialog.
                dlVirusFound.buttons = &okButton;
                dlVirusFound.strings = lpszVirusFoundStrings;
                rInfected.wStatus = FILESTATUS_INFECTED;
                ScanFoundCreateMessage(lpszMessage, &rInfected);
                                        // Custom alert message
                if ( lpNoNav->Alert.bDispAlertMsg )
                    {
                    lpszAlertMessageNext = CutString(lpAlertMessageBuffer[0],
                                                 lpNoNav->Alert.szAlertMsg,
                                                 50);
                    CutString(lpAlertMessageBuffer[1],
                          lpszAlertMessageNext,
                          50);
                    }
                else
                    {
                    lpAlertMessageBuffer[0][0] = lpAlertMessageBuffer[1][0] = EOS;
                    }

                DialogBox2( &dlVirusFound,
                            lpszMessage[0],
                            lpszMessage[1],
                            lpszMessage[2],
                            lpszMessage[3],
                            lpszMessage[4],
                            SZ_FILE_NOT_INOCULATED,
                            SZ_EMPTY,
                            lpAlertMessageBuffer[0],
                            lpAlertMessageBuffer[1]);
                }
            else                        // Go ahead and inoculate the file.
                {
                MEMSET(&Log,    '\0', sizeof(Log));

                Log.lpFileName  = rInfected.szFullPath;
                Log.lpAct       = &navOptions.activity;

                uResult = InocFile(rInfected.szFullPath);

                if (uResult == TRUE)
                    {
                    rInfected.wStatus = FILESTATUS_INOCULATED;
                    Log.uEvent = LOGEV_INOCULATED_NEW;
                    }
                else
                    {
                    rInfected.wStatus = FILESTATUS_INOCULATIONFAILED;
                    Log.uEvent = LOGEV_INOCULATEFAILED_NEW;
                    }

                ScanActionMessage(&rInfected);

                LogEvent(&Log);
                }
            }
        VirusScanEnd();
        }
    SmartInocClose();

    MemFreePtr(lpNoNav);

} // End ScanInoculateSingleFile()

#endif

