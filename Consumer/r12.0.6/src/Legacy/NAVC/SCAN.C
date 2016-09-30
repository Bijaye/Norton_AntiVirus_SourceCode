// Copyright 1992-1996 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/NAVC/VCS/scan.c_v   1.1   08 Jun 1997 23:24:22   BGERHAR  $
//
// Description:
//  This handles all items in the Scan menu.
//
// See Also:
//***********************************************************************
// $Log:   S:/NAVC/VCS/scan.c_v  $
// 
//    Rev 1.1   08 Jun 1997 23:24:22   BGERHAR
// Don't wrap for deep scan or def directories
// 
//    Rev 1.0   06 Feb 1997 20:56:26   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:22   MKEATIN
// Initial revision.
// 
//    Rev 1.4   19 Nov 1996 13:16:02   JBELDEN
// added fprintf for logging
// 
//    Rev 1.3   12 Nov 1996 18:03:46   JBELDEN
// replaced stddlgerror with printf string
// 
// 
//    Rev 1.2   31 Oct 1996 14:10:20   JBELDEN
// removed calls to inoculation code
// 
//    Rev 1.1   09 Oct 1996 14:57:54   JBELDEN
// updated to work with updated navstr.str
// 
//
//    Rev 1.0   02 Oct 1996 12:58:52   JBELDEN
// Initial revision.
//***********************************************************************

#include "platform.h"
#include <stdio.h>
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
#ifdef USE_NETWORKALERTS   //&?
 #include "netalert.h"
#endif

#include "scand.h"

MODULE_NAME;

extern   FILE  *fLogFile;
extern   BOOL  bAutoExportLog;


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
#ifndef TTYSCAN
    extern      CheckBoxRec     scanSubDirCheckBox;
    extern      ButtonsRec      buScanCancel;
#endif
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
#ifndef TTYSCAN
                                        // Get the directory to scan
        byResult = NavDlgSelectDir( szFileName,
                                    SZ_SELECT_DIR_TITLE,
                                    &scanSubDirCheckBox,
                                    &buScanCancel,
                                    FALSE );
#endif

                                        // Scan it
        if ( byResult )
            {
            NameToFull(lpScanList, szFileName);
            STRUPR(lpScanList);
            MemUnlock(rScan.hScanList, lpScanList);
#ifndef TTYSCAN
            rScan.bScanSubs = scanSubDirCheckBox.value;
#endif
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
#ifndef TTYSCAN
    extern  ButtonsRec      buScanCancel;
#endif
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

#ifndef TTYSCAN
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
#endif
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
    extern  char   SZ_FILE_NOT_FOUND [];

    if (!FileExists(lpszFile))
        {
        printf(SZ_FILE_NOT_FOUND, lpszFile);
        if (bAutoExportLog)
         fprintf(fLogFile, SZ_FILE_NOT_FOUND, lpszFile);
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
