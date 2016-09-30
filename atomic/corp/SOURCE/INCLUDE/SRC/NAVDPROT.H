// Copyright 1993 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/INCLUDE/VCS/navdprot.h_v   1.2   09 Jun 1997 18:35:32   BGERHAR  $
//
// Description:
//      NAV 3.0 for DOS                                 PROTO.H
//
//      This file should contain every public function (i.e. a function
//      is either declared LOCAL or is declared here).
//
//      Also contains multi-project #defines and typedefs.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/navdprot.h_v  $
// 
//    Rev 1.2   09 Jun 1997 18:35:32   BGERHAR
// Add AreDefsWild() and misc. clean-up
// 
//    Rev 1.1   01 Jun 1997 20:34:22   BGERHAR
// Update definitions/options search logic
// 
//    Rev 1.0   06 Feb 1997 20:51:24   RFULLER
// Initial revision
// 
//    Rev 1.3   31 Dec 1996 15:32:16   MKEATIN
// This is the navdprot.h from DOSNAV/TTYSCAN
// 
//    Rev 1.85   27 Nov 1996 08:28:08   JBELDEN
// fixed errorlevels for ttyscan and navd
// 
//    Rev 1.84   27 Nov 1996 08:06:32   JBELDEN
// added errorlevel codes for ttyscan
// 
//    Rev 1.83   12 Nov 1996 18:22:24   JBELDEN
// added error codes for ttyscan
// 
//    Rev 1.82   26 Mar 1996 17:05:12   MZAREMB
// Added #define to notify the stub program (NAVBOOT) to launch the TSR.
// 
//    Rev 1.81   05 Sep 1995 07:22:00   JWORDEN
// Change ifdef for NAVNET to NAVNETW
// 
//    Rev 1.80   15 Jul 1995 18:13:00   BARRY
// If /STARTUP mode and the CRC is bad, halt instead of exit
// 
//    Rev 1.79   15 Mar 1995 17:43:46   DALLEE
// Added NavDlgError() replacement for StdDlgError() that makes sure
// sreen IO is intialized.
// 
//    Rev 1.78   14 Mar 1995 17:46:52   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.77   14 Mar 1995 14:24:28   DALLEE
// Moved exported Locate Settings function prototypes here.
// 
//    Rev 1.76   08 Mar 1995 19:49:20   RSTANEV
// Added ERRORLEVEL_DOSAPERROR.
//
//    Rev 1.75   07 Mar 1995 19:53:48   DALLEE
// Oops... had to add optional lpszTitle parameter to NavDlgSelectDir().
//
//    Rev 1.74   07 Mar 1995 19:34:32   DALLEE
// Prototypes for: NavDlgSelectDir(), DatFilesExistIn(),
//     and FormatNameDateAndTime().
//
//    Rev 1.73   03 Mar 1995 15:10:34   DALLEE
// Added flag to LocateUpdateSettingsFound() to display either
// "Settings found in" or "Using settings from:" once selected.
//
//    Rev 1.72   21 Feb 1995 11:30:06   DALLEE
// Changed VirusList() to NavdVirusList() due to name conflict w/ NAVW.
//
//    Rev 1.71   14 Feb 1995 13:28:16   DALLEE
// Made LocateUpdateFoundText() public.  Now called from main dialog to
// show .dat files used.
//
//    Rev 1.70   13 Feb 1995 22:30:56   DALLEE
// SearchXXX() functions for locating settings now take lpPath param and
// return TRUE if settings found and fills lpPath with folder.
//
//    Rev 1.69   07 Feb 1995 13:52:08   ELEAR
// Added prototypes for search and locate modules.
//
//    Rev 1.68   29 Dec 1994 17:34:56   DALLEE
// Put extern "C" around funcion prototypes.
//
//    Rev 1.67   28 Dec 1994 14:39:24   DALLEE
// Use CTSN.H instead of CTS4.H.
// Remove prototype for FileCheckError().  This is published in FILE.H now.
//
//    Rev 1.66   31 Oct 1994 12:49:04   MFALLEN
// Starting of fresh from the old NAV 3.0 codebase.
//
//    Rev 1.60   25 Oct 1993 19:12:12   BARRY
// Added ERRORLEVEL_INTERRUPT
//************************************************************************

#ifndef INCL_NAVDPROT
#define INCL_NAVDPROT

#include "ctsn.h"

#ifdef NAVNETW
#include "nethelp.h"
#else
#include "navhelp.h"
#endif

//************************************************************************
// TYPEDEFS
//************************************************************************
                                        // Structure used for variable length
                                        // lists.
typedef struct tagNAVListData
    {
    WORD    wEntryMaxSize;
    HGLOBAL hPointers;
    HGLOBAL hEntries;
    BYTE    *lpEntries;
    } NAVListData;


//************************************************************************
// GLOBAL DEFINES
//************************************************************************

#define OPT_LIST_DELAY1             ( 5)
#define OPT_LIST_DELAY2             ( 2)
#define NAVD_PROGRAM_EXT_SIZE       ( 3)
                                              // For standard file dialog
#define MAX_SELECT_FILES            (512)

// Define for DOS filestatus fillbar width
#define LOG_FILL_BAR_WIDTH      40

#ifndef TTYSCAN
// These are the DOS ERRORLEVEL return values for NAVD:

#define ERRORLEVEL_NONE         ((BYTE)0)
#define ERRORLEVEL_VIRUSFOUND   ((BYTE)1)   // Virus found (at any time)
#define ERRORLEVEL_SELFCHECK    ((BYTE)2)   // Self-check failed on startup
#define ERRORLEVEL_DOSAPERROR   ((BYTE)3)   // Unable to execute DOS AP
                                            // (NAVBOOT only)
#define ERRORLEVEL_EXECUTETSR   ((BYTE)100) // "Launch TSR" return code
                                            // (NAVBOOT only)
#define ERRORLEVEL_INTERRUPT    ((BYTE)254) // Scan interrupted by user
                                            // (applies to last scan only)
#define ERRORLEVEL_OTHER        ((BYTE)255) // Incompatible config
                                            // (XT, DOS2, out of memory, etc.)

#else
// These are the DOS ERRORLEVEL return values for NAVC command line scanner:

#define ERRORLEVEL_NONE             ((BYTE)0)   // No errors occurred, no viruses were found.
#define ERRORLEVEL_LOADING          ((BYTE)5)   // Insufficient memory to load program or complete operation.
#define ERRORLEVEL_OUTOFMEM         ((BYTE)6)   // An internal program error has occurred - out of memory error.
#define ERRORLEVEL_MISSINGFILE      ((BYTE)8)   // A file required to run NAVC, such as VIRSCAN.DAT, is missing.
#define ERRORLEVEL_BADOPTION        ((BYTE)9)   // Incompatible or unrecognized option(s) or option argument(s) were
                                                // specified in the command line
#define ERRORLEVEL_MEMVIRUSFOUND    ((BYTE)10)  // A virus was found in memory

#define ERRORLEVEL_OTHER            ((BYTE)11)  // Internal program error.
#define ERRORLEVEL_VIRUSFOUND       ((BYTE)13)  // One or more viruses where found in the master boot record, boot
                                                // sector, or file(s)
#define ERRORLEVEL_SELFCHECK        ((BYTE)15)  // Self-check failed on startup
#define ERRORLEVEL_CLEANED          ((BYTE)19)  // Self-check failed on startup

#define ERRORLEVEL_INTERRUPT        ((BYTE)102) // Scan interrupted by user
                                                // (applies to last scan only)

#endif


//************************************************************************
// FUNCTION PROTOTYPES
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

// Functions to access DOSExit() ERRORLEVEL global

VOID PASCAL ErrorLevelSet ( BYTE byErrorLevel );
BYTE PASCAL ErrorLevelGet ( VOID );

UINT    PASCAL NAVDWriteLog (LPVOID lpNoNav);

VOID    PASCAL DisplayMessage(LPSTR lpszMessage1, LPSTR lpszMessage2);
VOID    PASCAL NavDlgError(LPSTR strings[]);

VOID    PASCAL DoScanDrives(VOID);
BYTE    PASCAL DoScanDirectory(VOID);
BYTE    PASCAL DoScanFile(VOID);
VOID    PASCAL DoInocFiles(LPSTR lpFileSpec, BOOL bSubDirs);
VOID    PASCAL UnInocFiles(LPSTR lpFileSpec, BOOL bSubDirs);

VOID    PASCAL DoOptions(VOID);
WORD    PASCAL DoVideoMouse(VOID);
VOID    PASCAL DoInoculation(VOID);
VOID    PASCAL NavdVirusList(VOID);

VOID    PASCAL ActivityLog(VOID);
VOID    PASCAL InfectionReport(VOID);


int     PASCAL InitDisplay(BOOL bForceInit);
VOID    PASCAL RedrawMenuBar(VOID);

DIALOG_RECORD * PASCAL OpenDriveSelectDialog(VOID);
VOID    PASCAL CloseDriveSelectDialog(DIALOG_RECORD *lpDr);

VOID    PASCAL InitializeSelectedDrives(VOID);
VOID    PASCAL ScanDriveGetList(VOID);
int     PASCAL ScanDriveButtons(DIALOG_RECORD *lpDr);
VOID    PASCAL ScanDrivePostProcess(DIALOG_RECORD *lpDr);

BYTE    PASCAL TunnelSelectFile (TERec *te, char *szSpec, char *szTitle);
BYTE    PASCAL SelectFile (char *szFilename, char *szSpec, char *szTitle);

BOOL PASCAL SelectPrintDevice(char *szTitle, char *szOutputDevice,HFILE * hFile);

VOID 	PASCAL DoHelp(UINT uHelpIndex);
BOOL    PASCAL ReallyExit (VOID);

// locate.c

WORD    PASCAL LocateSettings (LPSTR lpszOptionsDir);

BOOL    PASCAL SearchForSettings1 (LPSTR lpszFoundDir);
BOOL    PASCAL SearchForSettings2 (LPSTR lpszFoundDir);

VOID    PASCAL LocateUpdateFoundText (DIALOG_RECORD *lpdrDialog,
                                      LPSTR          lpszFoundDir,
                                      BOOL           bSelected);

BOOL    PASCAL FormatNameDateAndTime (LPSTR lpszStrBuffer,
                                      LPSTR lpszFilePath,
                                      LPSTR lpszFileName);

// search.c

BOOL PASCAL FileExistsInDirPath( LPSTR lpSearchPath,  LPSTR szFileName );
BOOL PASCAL FilesExistIn (LPSTR lpszPath, char *lpfileList[]);
BOOL PASCAL InitOptionsDir(VOID);
BOOL PASCAL InitDefinitionsDir(VOID);
BOOL PASCAL FindDefsSwapDisk (char * lpszDefsDir);
VOID PASCAL SetDefsWild(BOOL bAreWild);
BOOL PASCAL AreDefsWild(VOID);
int  PASCAL ReadPathSwitch (LPCFGTEXTREC lprSwitch, LPSTR lpszOptPath);

// parse.c

BOOL PASCAL FindNavbootPathInAutoExec( LPSTR lpszNavbootPath );

// navdir.c

BYTE PASCAL NavDlgSelectDir ( LPSTR         path,
                              LPSTR         lpszTitle,
                              CheckBoxRec  *checkBox,
                              ButtonsRec   *pbuAlternative,
                              BOOL          bShowNavopts );

//------------------------------------------------------------------------
// NavNet function
//------------------------------------------------------------------------
BOOL PASCAL NavNetCheckIfDirty(VOID);
BOOL PASCAL NavNetFileNew(VOID);
BOOL PASCAL NavNetFileOpen(BOOL bCmdLine);
BOOL PASCAL NavNetFileSave(VOID);
BOOL PASCAL NavNetFileSaveAs(VOID);


//------------------------------------------------------------------------
// Variable Length List Functions
//------------------------------------------------------------------------

WORD PASCAL NAVListAdd (ListRec *lplrList, LPVOID lpItem);
WORD PASCAL NAVListDelete (ListRec *lplrList, DWORD dwIndex);
VOID PASCAL NAVListMemFree (ListRec *lplrList);
WORD PASCAL NAVListDuplicateString (ListRec *lplrList, char *szString);


//------------------------------------------------------------------------
// Options CP Dialog Functions
//------------------------------------------------------------------------

VOID PASCAL NAVDOptsScannerLoad     (LPVOID lpOptions);
VOID PASCAL NAVDOptsScannerSave     (LPVOID lpOptions);
VOID PASCAL NAVDOptsMonitorLoad     (LPVOID lpOptions);
VOID PASCAL NAVDOptsMonitorSave     (LPVOID lpOptions);
VOID PASCAL NAVDOptsAlertLoad       (LPVOID lpOptions);
VOID PASCAL NAVDOptsAlertSave       (LPVOID lpOptions);
VOID PASCAL NAVDOptsActivityLoad    (LPVOID lpOptions);
VOID PASCAL NAVDOptsActivitySave    (LPVOID lpOptions);
VOID PASCAL NAVDOptsInocLoad        (LPVOID lpOptions);
VOID PASCAL NAVDOptsInocSave        (LPVOID lpOptions);
VOID PASCAL NAVDOptsPasswordLoad    (LPVOID lpOptions);
VOID PASCAL NAVDOptsPasswordSave    (LPVOID lpOptions);
VOID PASCAL NAVDOptsGeneralLoad     (LPVOID lpOptions);
VOID PASCAL NAVDOptsGeneralSave     (LPVOID lpOptions);

VOID PASCAL ProgramExtDialog            (LPSTR szExt, LPCSTR szDef);
UINT        TEExtensionFilter           (TERec *te, UINT *c);
UINT        TEUpperPathWildcardFilter   (TERec *te, UINT *c);

VOID OptListWriteString     ( ListRec * lplrList,
                              LPVOID    lpvEntry,
                              int       iAttr,
                              DWORD     dwEntry);

VOID ExcludeListWriteEntry  ( ListRec * lplrList,
                              LPVOID    lpEntry,
                              int       iAttr,
                              DWORD     dwEntry);

VOID PasswordListWriteEntry ( ListRec * lplrList,
                              LPVOID    lpvEntry,
                              int       iAttr,
                              DWORD     dwEntry);

int PasswordListMouseProc   ( ListRec   *lplrList,
                              DWORD     dwEntry,
                              BYTERECT  *lpRect,
                              WORD      wButtons,
                              int       iMouseClicks);

BOOL PASCAL AskForPassword(WORD wFeature);
BOOL PASCAL NeedPassword(WORD wFeature);
VOID PASCAL ResetPassword(VOID);

#ifdef NAVNETW
UINT CPDialogProcInstall    ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );
#endif

UINT CPDialogProcScanner    ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );

UINT CPDialogProcMonitor    ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );

UINT CPDialogProcAlert      ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );

UINT CPDialogProcActivity   ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );

UINT CPDialogProcInoc       ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );

UINT CPDialogProcExclude    ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );

UINT CPDialogProcPassword   ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );

UINT CPDialogProcSchedule   ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );

UINT CPDialogProcGeneral    ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog );


void  GreyOutText(BYTE * byMessage,
		  int inRowOffset,
		  int inColOffset,
		  BOOL bNormalColor);

typedef UINT (*CPLENTERPROC)(VOID);

UINT PASCAL ProcessPassword(DIALOG_RECORD *lpDialog, WORD wFeature, CPLENTERPROC fnCPLEntering, LPBOOL lpbNormalProcess);
VOID PASCAL DialogEnableControls (DIALOG_RECORD	*dr, BYTE bEnable);
VOID PASCAL DialogClearAndDisableControls (DIALOG_RECORD *dr);


//------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------

VOID PASCAL UpdateCheckBox (CheckBoxRec *pcbCheckBox, BOOL bEnabled);
LPSTR PASCAL NAVDLoadTextEdit (TERec *lpText, LPCSTR szSource);
BOOL  PASCAL  PRPrintNCRLFs(HFILE * hFile,BYTE byNumberOfLines);
BOOL  PASCAL  PRPrintNChars(HFILE * hFile,char x,BYTE byNumberOfSpaces);
BOOL  PASCAL  PRPrintCenteredText(HFILE * hFile,char * szTextToOut);
BOOL  PASCAL  PRPrintLineOfStars(HFILE * hFile);
BOOL          PRPrintFrmtCRLFLine(HFILE * hFile,BYTE * szFrmt,...);
BOOL  PASCAL  PRReportTitle(HFILE * hFile,char * szTitle);
WORD  PASCAL  NavSelfCheck (BOOL bHalt);
VOID  PASCAL  DialogOutUsingArray(BYTE byStart,
				 BYTE byEnd,
				 OutputFieldRec ** apofRecArray,
				 char * szzString);
void  DisplayTimedMessage(char * lpszString);

//------------------------------------------------------------------------
// Virus catalog information retrieval and Virus Information printing
// (in VL.C)
//------------------------------------------------------------------------
VOID PASCAL WordWrapVirusComment(BYTE byLength, LPSTR  lpComment);
UINT PASCAL GetVirusInfo(WORD wVirusID,
				      LPSTR lpAlias,
                                      LPSTR lpComment);

VOID PASCAL PrintVirusInformation(char * szPaperTitle,
				 char * szInfectedFile,
				 char * szStatusLine,
	                         N30NOTES *hpDefInMemory,
				 char * szAlias,
				 char * szCommentLine);

VOID PASCAL VirusInfoUpdate(N30NOTES HUGE *hpDefInMemory);
VOID PASCAL DisablePrevNext(UINT uCurrentEntry, UINT uMaxEntries);


//------------------------------------------------------------------------
// Critical error control routines (not otherwise included in FILE.H)
//------------------------------------------------------------------------

VOID PASCAL     InitError(BOOL bUseErrDlg);
VOID CDECL      _CheckErrorFar(VOID);   // Use this after every INT 21, or use
                                        // FileCheckError() to get HFILE_ERROR

VOID CDECL      _NAVClearError(VOID);   // Ultimately want to avoid using this

#ifdef __cplusplus
}
#endif

#endif      // (#ifndef INCL_NAVDPROT)
