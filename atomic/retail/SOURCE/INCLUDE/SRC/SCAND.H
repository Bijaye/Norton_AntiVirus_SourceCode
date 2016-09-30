// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/scand.h_v   1.1   08 Jun 1997 23:25:24   BGERHAR  $
//
// Description:
//      This include file contains the prototypes, constants and typedefs
//      for the NAV DOS scanner.
//************************************************************************
// $Log:   S:/INCLUDE/VCS/scand.h_v  $
// 
//    Rev 1.1   08 Jun 1997 23:25:24   BGERHAR
// Minor change in naming of StartScanFile filename parameter -- no functional change
// 
//    Rev 1.0   06 Feb 1997 20:51:10   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:14   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:38   BARRY
// Initial revision.
// 
//    Rev 1.12   17 Nov 1995 19:37:40   MKEATIN
// changed SCAN_PROMPT_MESSAGE_LINES from 3 to 5
// 
//    Rev 1.11   14 Mar 1995 17:46:26   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.10   28 Dec 1994 14:46:20   DALLEE
// Removed ScanFileAll() prototype -- this is local to SCAND\SCANITEM.C
// 
//    Rev 1.9   31 Oct 1994 12:50:34   MFALLEN
// Starting of fresh from the old NAV 3.0 codebase.
// 
//    Rev 1.5   16 Sep 1993 21:19:46   DALLEE
// Add prototype for EnableStopWatch
//************************************************************************

#ifndef INCL_SCAND
#define INCL_SCAND


//************************************************************************
// DEFINES
//************************************************************************

#define SKIPDRIVE_DLG_ACCESS    0       // These defines determine the
#define SKIPDRIVE_DLG_BOOT      1       // string diplayed by
#define SKIPDRIVE_DLG_WRITE     2       // SkipDriveTimedErrDialog().

#define SKIPDRIVE_IGNORE        0       // Return values for
#define SKIPDRIVE_RETRY         1       // SkipDriveTimedErrDialog().
#define SKIPDRIVE_ABORT         2


#define SCAN_VIRUSES            1
#define SCAN_INOCULATION        2

#define INOC_FLAG                   1
#define SCAN_FOUND_NAME_WIDTH       (12)
#define SCAN_FOUND_VIRUS_WIDTH      (22)
#define SCAN_FOUND_STATUS_WIDTH     (18)
#define SCAN_FOUND_ENTRY_WIDTH      (SCAN_FOUND_NAME_WIDTH   + \
                                     SCAN_FOUND_VIRUS_WIDTH  + \
                                     SCAN_FOUND_STATUS_WIDTH + 2 + 2)

#define SCAN_PROMPT_MESSAGE_WIDTH   46  // These two #defines must match
#define SCAN_PROMPT_MESSAGE_LINES   5   // the prompt message dialogs.


// used in SCANFNDS.STR and SCANFND.C
#define GROUP_SIZE_TXT  "59"   // the numbers must be the same.
#define GROUP_SIZE_NUM  59

// directory fudge sizes used in scan-bar stuff follows:

#define DIR_CONST_VALUE         524288L     // 512k


//************************************************************************
// STRUCTS
//************************************************************************

typedef struct tagProblemsFoundDoAllRec
    {
    WORD    wActionType;
    BYTE    byaSkippedDrives[26];
    } ProblemsFoundDoAllRec;


//************************************************************************
// PROTOTYPES FOR SCAND PROJECT FUNCTIONS
//************************************************************************

WORD    PASCAL ScanFoundRepairDialog    (VOID);
WORD    PASCAL ScanFoundDeleteDialog    (VOID);
WORD    PASCAL ScanFoundExcludeDialog   (VOID);
WORD    PASCAL ScanFoundInocDialog      (VOID);

VOID    PASCAL PromptBootResponse       (LPNONAV lpNoNav);

VOID    PASCAL ScanInfoDialog           (BOOL bDuringScan);

VOID    PASCAL AddVirusInfo     (LPNONAV lpNoNav);
BOOL    PASCAL ExtInList        (LPNONAV lpNoNav, LPSTR lpFileName);

UINT    PASCAL ScanRepairFile   (LPNONAV lpNoNav, LPINFECTEDFILE lpInfected);
WORD    PASCAL DeleteFile       (LPNONAV lpNoNav, LPINFECTEDFILE lpInfected);
BOOL    PASCAL RepairBoot       (LPNONAV lpNoNav, LPINFECTEDFILE lpInfected);
BOOL    PASCAL CreateTemporaryRepairFile (LPSTR lpNewFile, LPCSTR lpOldFile);

BOOL    PASCAL GetInfectedFile  (LPINFECTEDFILE   lpInfected,
                                 LPNONAV          lpNoNav,
                                 DWORD            dwIndex);
BOOL    PASCAL PutInfectedFile  (LPINFECTEDFILE   lpSource,
                                 LPNONAV          lpNoNav,
                                 DWORD            dwIndex);

VOID    PASCAL ScanFoundCreateMessage (LPSTR            *lpszMessage,
                                       LPINFECTEDFILE   lpInfected);

VOID    PASCAL ScanFoundCreateEntry (char *szEntry, LPINFECTEDFILE lpInfected);
char *  PASCAL ScanFoundReturnStatusString (WORD wStatus);
char *  PASCAL ScanFoundReturnMessageBoxString (WORD wStatus);

VOID    PASCAL ScanFoundBuildList   (LPNONAV lpNoNav, ListRec *lplrList);

VOID    PASCAL ReadAllInfectedFiles (
                    LPNONAV lpNoNav,
                    BOOL (PASCAL *procCallback)( LPINFECTEDFILE lpInfected,
                                                 LPNONAV        lpNoNav,
                                                 WORD           wEntry,
                                                 LPARAM         lParam     ),
                    LPARAM lParam);

VOID    PASCAL ScanStart            (LPSCANSTRUCT lpScan);
WORD    PASCAL ScanDialog           (LPNONAV lpNoNav);
VOID    PASCAL ScanResultsDialog    (LPNONAV lpNav);
WORD    PASCAL ScanFoundDialog      (LPNONAV lpNav);

VOID    PASCAL LoadNoNavIni         (LPNONAV lpNoNav);

VOID    PASCAL ScanMemory           (LPNONAV lpNoNav);
VOID    PASCAL ScanMBR              (LPNONAV lpNoNav);
VOID    PASCAL ScanBootRecords      (LPNONAV lpNoNav);
UINT    PASCAL ScanFiles            (LPNONAV lpNav);
UINT    PASCAL StartScanFile        (LPNONAV lpNav, LPSTR lpszFile);

// scandlg.c
DIALOG_RECORD * PASCAL OpenScanFilesDialog (LPNONAV lpNoNav);
DIALOG_RECORD * PASCAL OpenMemoryBootDialog (LPNONAV lpNoNav);
VOID    PASCAL UpdateScanDialog     (LPNONAV lpNav);
VOID    PASCAL ForceUpdateScanDialog(LPNONAV lpNoNav);
VOID    PASCAL ScanMemoryBootFillBar(OutputFieldRec *lpFillBar, BOOL bDone);


VOID    PASCAL RespondToVirus       (LPNONAV lpNav);
VOID    PASCAL RespondToNotInoc     (LPNONAV lpNoNav);
VOID    PASCAL RespondToInocChange  (LPNONAV lpNoNav);
VOID    PASCAL RespondToBootVirus   (LPNONAV lpNoNav);

BOOL    PASCAL FileInoculated       (LPNONAV lpNav);

VOID           ScanFoundListWrite   (ListRec    *lplrList,
                                     LPVOID     lpEntry,
                                     int        iAttr,
                                     DWORD      dwEntry);

VOID    PASCAL NAVDFreeLinkList     (LPNONAV lpNoNav);

VOID    PASCAL EnableStopWatch      (BOOL bEnabled);
VOID    PASCAL ScanStopWatch        (LPNONAV lpNav, BOOL bStart);

VOID    PASCAL BuildInfectsString   (N30NOTES HUGE *hpDefInMemory,
                                     char *szBuffer);

VOID    PASCAL AddBytesScanned      (LPNONAV lpNoNav, DWORD dwBytes);

BOOL    PASCAL ScanExcludeInfectedFile(LPNONAV lpNoNav, LPINFECTEDFILE lpInfected);

VOID    PASCAL ScanActionMessage    (LPINFECTEDFILE lpInfected);
VOID    PASCAL ScanNotify           (LPNONAV lpNoNav);
VOID    PASCAL ScanHalt             (LPNONAV lpNoNav);

BOOL    PASCAL SystemInocCheck      (LPNONAV lpNoNav);

VOID    PASCAL RespondToVirusInZip  (LPNONAV lpNoNav);

UINT    PASCAL SkipDriveTimedErrDialog ( BYTE cDriveLetter,
                                         UINT uTypeErr,
                                         UINT uRvTimeout,
                                         LPNONAV lpNoNav );

UINT    PASCAL DoAllCheckWriteToFile   ( LPSTR lpszFile, LPNONAV lpNoNav );

//------------------------------------------------------------------------
// GetBytesToScan callback functions
//------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI GetBytesSkipDrive (LPSTR *lpszzScanList, DWORD dwUnused);
UINT SYM_EXPORT WINAPI GetBytesDriveError (BYTE byDrive, DWORD dwUnused);

#endif  // (#ifndef INCL_SCAND)
