// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/scanfnd.c_v   1.1   20 Aug 1997 01:06:20   TCASHIN  $
//
// Description:
//      These are the functions for the Problems Found scan results dialog.
//
// Contains:
//      ScanResultsDialog ()
//
// See Also:
//      SCANFND.STR for strings and control structures.
//************************************************************************
// $Log:   S:/NAVC/VCS/scanfnd.c_v  $
// 
//    Rev 1.1   20 Aug 1997 01:06:20   TCASHIN
// Check if the activity log is enabled before trying to write to it.
// 
//    Rev 1.0   06 Feb 1997 20:56:20   RFULLER
// Initial revision
// 
//    Rev 1.3   20 Jan 1997 15:59:18   MKEATIN
// Removed a couple of '\n' characters.
// 
//    Rev 1.2   17 Jan 1997 15:43:20   MKEATIN
// Display a little "Type 'NAVC /?' for help" message if we find a virus.
// 
//    Rev 1.1   09 Jan 1997 14:22:52   MKEATIN
// Removed boot record scan results for the NEC at run time.
// 
//    Rev 1.0   31 Dec 1996 15:20:00   MKEATIN
// Initial revision.
// 
//    Rev 1.6   07 Nov 1996 16:57:36   JBELDEN
// removed unused code
// 
//    Rev 1.5   31 Oct 1996 17:05:22   JBELDEN
// removed some dialog code
//
//    Rev 1.4   17 Oct 1996 09:59:52   JBELDEN
// Ifdef'ed out routines not needed by ttyscan
//
//    Rev 1.3   11 Oct 1996 17:59:16   JBELDEN
// added fprintf's for log file
//
//    Rev 1.2   08 Oct 1996 14:49:14   JBELDEN
// added DOSPrint's for the summary message.
//
//    Rev 1.1   03 Oct 1996 14:57:22   JBELDEN
// removed code related to viruslist
//
//    Rev 1.0   02 Oct 1996 12:58:36   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "symintl.h"
#include "stddlg.h"
#include "file.h"

#include "navutil.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"

#include "navdprot.h"
#include "scand.h"

#ifdef SYM_DOS
 #include <stdio.h>
#endif

// For VMM debugging.
MODULE_NAME;


//************************************************************************
// STATIC VARIABLES
//************************************************************************

STATIC    char            *szSummaryLines;
STATIC    char            *szItems;
STATIC    char            *szType;
STATIC    char            *szInoculation;
STATIC    char            szTime [40];
STATIC    char            *szMemScanned;

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern LPNONAV          glpNoNav;
extern BOOL             bStartupMode;
extern FILE             *fLogFile;
extern BOOL             bAutoExportLog;   // Automatically export the log


//************************************************************************
// EXTERNAL PROTOTYPES
//************************************************************************

extern BOOL             SYM_EXPORT PASCAL IsActivityLogEnabled(VOID);

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

BOOL LOCAL PASCAL CheckJustInoc(void);

//************************************************************************
// CheckJustInoc
//
// Description - This fuction determines if
//               only inoculation messages were reported
//               Used for the startup and auto-inoc features
// Parameters:
//      None
//
// Returns:
//      BOOL - TRUE  - if only inoculation messages
//             FALSE - otherwise
//************************************************************************
//CGRANGE - 5-8-95
//************************************************************************

BOOL LOCAL PASCAL CheckJustInoc()
{

    auto        INFECTEDFILE    rInfected;
    auto        DWORD           dwEntry;


    for (dwEntry = 0;;dwEntry++)
        {
        if (!GetInfectedFile (&rInfected, glpNoNav, dwEntry))
	    break;
        if ( rInfected.wStatus != FILESTATUS_INOCULATED )
            return(FALSE);
        }
    return(TRUE);
}



//************************************************************************
// ScanResultsDialog ()
//
// This routine displays the Scan Results dialog.
//
// Parameters:
//      LPNONAV     lpNav               This scan's info.
//
// Returns:
//      Nothing
//************************************************************************
// 3/9/93 DALLEE Function created.
//************************************************************************

VOID PASCAL ScanResultsDialog (LPNONAV lpNoNav)
{
    extern  char            SZ_EMPTY                [];
    extern  char            SZ_FOUND_NONE           [];
    extern  char            SZ_FOUND_FILES          [];
    extern  char            SZ_FOUND_BOOT           [];
    extern  char            SZ_FOUND_FILES_BOOT     [];
    extern  char            SZ_ALL_FILES            [];
    extern  char            SZ_PROGRAM_FILES        [];
    extern  char            SZ_ENABLED              [];
    extern  char            SZ_DISABLED             [];
    extern  char            SZ_YES                  [];
    extern  char            SZ_NO                   [];
    extern  char            SZ_DASH                 [];
    extern  char            SZ_TYPE_NAVC_HELP       [];
    extern  char            szSelectedDrives        [];

    extern  BOOL            bScreenStarted;
    BOOL            bJustInoc;

    auto    LPSTR           lpScanList;
    auto    HMS             hms;
    auto    char            szItemsScanned [SYM_MAX_PATH];

    auto    WORD            wNumEntries = 0;
    auto    LPINFARR        lpInfArr;
    auto    BYTE            szTempNum[4][16];

#ifdef  TTYSCAN
    extern  char  SZ_RESULT_SUMMARY_LINE [];
    extern  char  SZ_RESULT_ITEMS_SCANNED[];
    extern  char  SZ_RESULT_FILE_TYPE    [];
    extern  char  SZ_RESULT_INOCULATION  [];
    extern  char  SZ_RESULT_SCAN_TIME    [];
    extern  char  SZ_RESULT_TITLE_LINE   [];
    extern  char  SZ_RESULT_LINE         [];
    extern  char  SZ_RESULT_MEMORY       [];
    extern  char  SZ_RESULT_MASTER_BOOT  [];
    extern  char  SZ_RESULT_BOOT         [];
    extern  char  SZ_RESULT_FILES        [];
#endif                                     





    //listinfo
   lpInfArr = lpNoNav->lpInfArr;
   while (lpInfArr)
       {
       wNumEntries += lpInfArr->uNumUsed;
       lpInfArr = (LPINFARR) lpInfArr->lpNext;
       }

   bJustInoc=CheckJustInoc();
   if (!(bJustInoc && bStartupMode))
    {

                                        //-----------------------
                                        // Select Summary String.
                                        //-----------------------
    if ( lpNoNav->Stats.MasterBootRec.uInfected ||
         lpNoNav->Stats.BootRecs.uInfected )
        {
        szSummaryLines = (lpNoNav->Stats.Files.uInfected) ?
                        SZ_FOUND_FILES_BOOT : SZ_FOUND_BOOT;
        }
    else                                // No boot records infected.  Files?
        {
        szSummaryLines = (lpNoNav->Stats.Files.uInfected) ?
                        SZ_FOUND_FILES : SZ_FOUND_NONE;
        }

                                        //---------------------------------
                                        // Format the Items Scanned string.
                                        //---------------------------------
    if (NULL != (lpScanList = MemLock(lpNoNav->lpScan->hScanList)))
        {
        BuildWhatScanned(lpScanList, szItemsScanned, sizeof(szItemsScanned) -1);
        MemUnlock(lpNoNav->lpScan->hScanList, lpScanList);
        }
    else
        {
        szItemsScanned[0] = EOS;
        }
    szItems      = szItemsScanned;

                                        //---------------------------------
                                        // Set all the information strings.
                                        //---------------------------------
    szType       = lpNoNav->Opt.bScanAll      ? SZ_ALL_FILES : SZ_PROGRAM_FILES;
    szInoculation= lpNoNav->Inoc.bUseInoc     ? SZ_ENABLED   : SZ_DISABLED;
    szMemScanned = lpNoNav->Opt.bMemory       ? SZ_YES       : SZ_NO;


                                        // Break seconds down to hour,min,sec

                                        // Added to existing Kluge
                                        // Check for negative values.
                                        // This appears to be happening during
                                        // zip scanning occasionally.
                                        // jallee 8/15/96

    if ( lpNoNav->Stats.lTimeScan <= 0 )
        lpNoNav->Stats.lTimeScan=1;
    if  ( lpNoNav->Stats.lTimeScan > 1000000 )   // too many days, must be a
        lpNoNav->Stats.lTimeScan = 1;           //   subtraction overflow.
    HMS_Time(lpNoNav->Stats.lTimeScan, &hms);
    NIntlTimeToLongString(szTime, hms.uHour, hms.uMinute, hms.uSecond);

    if ( lpNoNav->Stats.MasterBootRec.uCleaned)
        _ConvertLongToString(lpNoNav->Stats.MasterBootRec.uCleaned, szTempNum[0], 10, 0);
    else
        STRCPY(szTempNum[0], SZ_DASH);

    if ( lpNoNav->Stats.BootRecs.uCleaned)
        _ConvertLongToString(lpNoNav->Stats.BootRecs.uCleaned, szTempNum[1], 10, 0);
    else
        STRCPY(szTempNum[1], SZ_DASH);

    if ( lpNoNav->Stats.Files.uCleaned)
        _ConvertLongToString(lpNoNav->Stats.Files.uCleaned, szTempNum[2], 10, 0);
    else
        STRCPY(szTempNum[2], SZ_DASH);

    printf(SZ_RESULT_SUMMARY_LINE,    szSummaryLines);
    printf(SZ_RESULT_ITEMS_SCANNED,   szItemsScanned);
    printf(SZ_RESULT_FILE_TYPE,       szType);

    printf(SZ_RESULT_SCAN_TIME,       szTime);
    printf(SZ_RESULT_TITLE_LINE);
    printf(SZ_RESULT_LINE);

    printf(SZ_RESULT_MEMORY,          szMemScanned, SZ_NO, SZ_DASH);

    if (!_HWIsNEC())
        {
        printf(SZ_RESULT_MASTER_BOOT, lpNoNav->Stats.MasterBootRec.uScanned,
                                      lpNoNav->Stats.MasterBootRec.uInfected,
                                      (LPSTR)&szTempNum[0]);

        printf(SZ_RESULT_BOOT,        lpNoNav->Stats.BootRecs.uScanned,
                                      lpNoNav->Stats.BootRecs.uInfected,
                                      (LPSTR)&szTempNum[1]);
        }


    printf(SZ_RESULT_FILES,            lpNoNav->Stats.Files.dwScanned,
                                        lpNoNav->Stats.Files.uInfected,
                                        (LPSTR)&szTempNum[2]);


    if (lpNoNav->Stats.MasterBootRec.uInfected ||
        lpNoNav->Stats.BootRecs.uInfected      ||
        lpNoNav->Stats.Files.uInfected)
        {
        printf("%s\n", SZ_TYPE_NAVC_HELP);
        }


    if (bAutoExportLog == TRUE && IsActivityLogEnabled())
      {
      fprintf(fLogFile, SZ_RESULT_SUMMARY_LINE,    szSummaryLines);
      fprintf(fLogFile, SZ_RESULT_ITEMS_SCANNED,   szItemsScanned);
      fprintf(fLogFile, SZ_RESULT_FILE_TYPE,       szType);
      fprintf(fLogFile, SZ_RESULT_SCAN_TIME,       szTime);
      fprintf(fLogFile, SZ_RESULT_TITLE_LINE);
      fprintf(fLogFile, SZ_RESULT_LINE);

      fprintf(fLogFile, SZ_RESULT_MEMORY,          szMemScanned, SZ_NO, SZ_DASH);

      if (!_HWIsNEC())
          {
          fprintf(fLogFile, SZ_RESULT_MASTER_BOOT, lpNoNav->Stats.MasterBootRec.uScanned,
                                                   lpNoNav->Stats.MasterBootRec.uInfected,
                                                   (LPSTR)&szTempNum[0]);

          fprintf(fLogFile, SZ_RESULT_BOOT,        lpNoNav->Stats.BootRecs.uScanned,
                                                   lpNoNav->Stats.BootRecs.uInfected,
                                                   (LPSTR)&szTempNum[1]);
          }

      fprintf(fLogFile, SZ_RESULT_FILES,           lpNoNav->Stats.Files.dwScanned,
                                        lpNoNav->Stats.Files.uInfected,
                                        (LPSTR)&szTempNum[2]);


      }

    }
} // End ScanResultsDialog ()


