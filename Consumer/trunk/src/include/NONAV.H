////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/nonav.h_v   1.15   05 Oct 1998 12:33:56   tcashin  $
//
// Description:
//      Constants and structures for NAV 3.0 virus scanning.
//
// Contains:
//
// See Also:
//      OPTIONS.H                       These files must be included before
//      NAVUTIL.H                       NONAV.H
//************************************************************************
// $Log:   S:/INCLUDE/VCS/nonav.h_v  $
// 
// 2     12/30/98 10:56a Cbenson
// updated in 33g
// 
//    Rev 1.15   05 Oct 1998 12:33:56   tcashin
// Added a new struct - EXTENDEDOPTIONS.
// 
//    Rev 1.14   28 Jul 1998 19:17:04   tcashin
// Added another parameter to MarkCompressedFilesQuarantined() and changed a
// parameter in DecideWhatToDoAfterProcessingZip().
// 
//    Rev 1.13   21 Jul 1998 13:42:08   tcashin
// Added a parameter to DecideWhatToDoAfterProcessingZip(). Added new 
// function NavMoveFileInf().
// 
//    Rev 1.12   01 Jul 1998 16:17:38   tcashin
// Added new function DecideWhatToDoAfterProcessingZip().
// 
//    Rev 1.11   03 Jun 1998 18:45:02   tcashin
// Added function prototypes for stuff in n_forwrd.cpp.
// 
//    Rev 1.10   01 Jun 1998 16:35:56   tcashin
// Added support for putting infected compressed files in quarantine.
// 
//    Rev 1.9   08 May 1998 21:48:24   tcashin
// Added stats for files quarantined and deleted.
// 
//    Rev 1.8   03 Apr 1998 12:20:52   tcashin
// Updated #defines for new quarantine in the repair wizard.
// 
//    Rev 1.7   21 Jul 1997 01:42:56   BGERHAR
// Added PROGRAM_AUTOMATED mode
// 
//    Rev 1.6   19 May 1997 15:45:04   TIVANOV
// move the quarantine log function to navxutil proj
// 
//    Rev 1.5   13 May 1997 18:14:04   DALLEE
// Added szInfectedLFN and szInfectedSFN to NONAV struct to
// track full long and short filenames of currently scanned item --
// necessary to speed up exclusions.
// 
//    Rev 1.4   09 May 1997 13:32:56   TIVANOV
// function prototype for UpdateQuarantineFile - this function creates and 
// updates the file
// 
//    Rev 1.3   02 May 1997 07:41:04   TIVANOV
// new quaranrine entries - nonavstuct, new question
//
//    Rev 1.2   07 Apr 1997 16:44:36   DALLEE
// Added SCANITEM struct and pointer to array of SCANITEMs to SCANSTRUCT.
// Used to index the lpScanList file specifications and add a per item
// include subdirectories scan.
//
//    Rev 1.1   01 Apr 1997 12:47:08   DALLEE
// Added new program mode #define for scanning launched from NAV32.EXE
//
//    Rev 1.0   06 Feb 1997 20:51:36   RFULLER
// Initial revision
//
//    Rev 1.3   11 Dec 1996 09:37:44   JBRENNA
// Add GetSysIntegFileOem() prototype.
//
//    Rev 1.2   26 Jun 1996 19:27:40   DALLEE
// LuigiPlus port:
// Removed #ifdefs around web-browser /DOWNLOAD prototypes.
//
//    Rev 1.1   24 Jun 1996 21:28:00   jmillar
// see 1.0
//
//    Rev 1.0   24 Jun 1996 21:26:42   jmillar
// Initial revision - actually the vcs and the file got corrupted when
// Ron checked the last change in - I just nuked (chkold) the vcs, and just
// checked in the recovered file. Everything should be in order.
//
//    Rev 1.7   24 Jun 1996 20:12:14   RHAYES
// Add BOOL parm. to UpdateDir() prototype.
//
//    Rev 1.6   20 Jun 1996 19:17:04   GWESTER
// Added bHMemLaunch to SCANSTRUCT for PileDriver fix 48138.
//
//    Rev 1.5   24 May 1996 12:07:12   GWESTER
// Added confirmation flag on Do All dialog if user clicks Stop.
//
//    Rev 1.4   21 May 1996 11:48:32   GWESTER
// Modified ActUponFileStatus() for Repair Wizard.
//
//    Rev 1.3   09 May 1996 17:53:16   GWESTER
// Added flags and functions for the Repair Wizard.
//
//    Rev 1.2   23 Apr 1996 20:01:40   TSmith
// Added uCpuLoad to SCANSTRUCT
//
//    Rev 1.1   13 Feb 1996 17:53:56   JWORDEN
// Add szFullOemPath to INFECTEDFILE structure
//
//    Rev 1.0   30 Jan 1996 15:47:20   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:11:44   BARRY
// Initial revision.
//
//    Rev 1.35   25 Jan 1996 19:06:24   GWESTER
// Added structure components for download mode.
//
//    Rev 1.34   21 Jan 1996 13:05:08   GWESTER
// Added functions and structure members for the Netscape downloading process.
//
//    Rev 1.33   17 Jan 1996 19:20:52   DALLEE
// Added uProgramMode to SCANSTRUCT.
// Set to PROGRAM_DOWNLOAD for Netscap/Web browser helper app mode.
//
//    Rev 1.32   21 Nov 1995 12:47:30   GWESTER
// Under NT, fixed problem of GPFs that occur when NAVNT is running and the
// user logs in and logs out.
//
//
//    Rev 1.31   02 Nov 1995 16:32:44   JWORDEN
// Change MasterBootRec elements from booleans to counters
//
//    Rev 1.30   24 Oct 1995 19:35:32   JWORDEN
// Add ASKQUESTION_REPAIR_MASTER_BOOT
//
//    Rev 1.29   07 Jul 1995 11:16:22   MARKL
// SYS4_HALT is now NavHaltSystem() and is in NAVXUTIL
//
//    Rev 1.28   09 May 1995 09:53:18   KEITH
// Use a consistent method for shortening filenames on dialogs.
// Set the width dynamically relative to the control width.
// Allow long names to wrap if the control allows.
//
//
//    Rev 1.27   24 Apr 1995 08:18:26   MFALLEN
// FillVirusInfo() is a local function.
//
//    Rev 1.26   21 Apr 1995 18:05:28   MFALLEN
// NotifyInocDeleteCompletion()
//
//    Rev 1.25   19 Apr 1995 15:57:16   MFALLEN
// Moved Win95 stuff to NAVXUTIL
//
//    Rev 1.24   19 Apr 1995 11:16:16   MFALLEN
// Added inoc specific feedback message.
//
//    Rev 1.23   17 Apr 1995 14:45:46   MFALLEN
// SYM_MAX_PATH + 1
//
//    Rev 1.22   16 Mar 1995 13:02:30   MFALLEN
// Now follows PIF files also.
//
//    Rev 1.21   15 Mar 1995 19:23:56   MFALLEN
// Follow LNK files.
//
//    Rev 1.20   03 Feb 1995 12:33:18   MFALLEN
// SCANSTRUCT does not use excludeitem or whatever exclude BS structure to process
// filespecs.
//
//    Rev 1.19   01 Feb 1995 20:26:16   MFALLEN
// ScanDriveSkipped macro.
//
//    Rev 1.18   30 Jan 1995 18:31:34   MFALLEN
// System integrity checking reintroduced.
//
//    Rev 1.17   25 Jan 1995 15:03:34   MFALLEN
// Cover for SYS4_HALT
//
//    Rev 1.16   20 Dec 1994 14:45:44   MFALLEN
// Sami did fixes on CERTLIBN
//
//    Rev 1.15   16 Dec 1994 14:21:00   MFALLEN
// uMatchType for LFN
//
//    Rev 1.14   16 Dec 1994 10:25:54   MFALLEN
// DeleteFile -> NavDeleteFile
//
//    Rev 1.13   15 Dec 1994 20:31:50   MFALLEN
// Removed previous change.
//
//    Rev 1.12   15 Dec 1994 20:29:04   MFALLEN
// Commented out netobj.h since none of this stuff works.
//
//    Rev 1.11   15 Dec 1994 20:14:50   MFALLEN
// From nirvana
//
//    Rev 1.133   11 Mar 1994 03:28:50   DALLEE
// Added RegPasswordVerify() prototype.
//************************************************************************

#ifndef INCL_NONAV
#define INCL_NONAV

#include "options.h"
#include "navutil.h"
#include "netobj.h"
#include "syminteg.h"
#include "quarset.h"

// ***************************** //

#define WM_NAVWORKUSER      (WM_USER + 150)

#define NO  ((BYTE)0)
#define YES ((BYTE)1)

#define CONTINUE            FALSE
#define STOP                TRUE
#define SIZE_MESSAGE        256
#define SIZE_WORK           256
#define NUM_DRIVES          ('Z' - 'A' + 1)
#define MAX_RESERVED_SIZE   15

                                        // ------------------------------------
                                        // Possible return statuses from the
                                        // general purpose inquiry dialog
                                        // ------------------------------------
enum
    {
    ASKQUESTION_ONLYONE = 1,
    ASKQUESTION_ALL,
    ASKQUESTION_CANCEL,
    ASKQUESTION_ERROR
    };
                                        // ------------------------------------
                                        // Type of information to present to
                                        // the user in the general purpose
                                        // dialog box.
                                        // ------------------------------------
enum
    {
    ASKQUESTION_REPAIR_INOCHANGED = 1,
    ASKQUESTION_REPAIR_INFECTED,
    ASKQUESTION_REPAIR_BOOT,

    ASKQUESTION_DELETE_INOCHANGED,
    ASKQUESTION_DELETE_INFECTED,
    ASKQUESTION_DELETE_UNINOCULATED,
    ASKQUESTION_DELETE_ERROR,
    ASKQUESTION_DELETE_NOTREPAIRED,
    ASKQUESTION_DELETE_INOCULATERROR,

    ASKQUESTION_INOCULATE_INOCHANGED,
    ASKQUESTION_INOCULATE_ALLCLEANFILES,

    ASKQUESTION_EXCLUDE_INOCHANGES,
    ASKQUESTION_EXCLUDE_NOTINOCULATED,
    ASKQUESTION_EXCLUDE_KNOWNVIRUS,
    ASKQUESTION_REPAIR_MASTER_BOOT,

    ASKQUESTION_REPAIR_ALL_INFECTED,    // Special Repair Wizard only defines
    ASKQUESTION_DELETE_ALL_INFECTED,     //  for DoAll()/ProcessItems() only.
    ASKQUESTION_QUARANTINE_ALL_INFECTED
    };

                                        // ------------------------------------
                                        // Return values for ShouldUseRepairWizard().
                                        // Handle these as DWORDs:
                                        // ------------------------------------
#define     SURW_YES        0x01        // Yes, use it.
#define     SURW_NO         0x02        // Don't.
#define     SURW_CANCEL     0x03        // Cancel (if we include this as a button).


                                        // ------------------------------------
                                        // IDs for the panels in the Repair Wizard.
                                        // These are handled like UINT.
                                        // ------------------------------------
enum
    {
    REPAIRWIZARD_DONTPANIC = 1,
    REPAIRWIZARD_STEPS,
    REPAIRWIZARD_REPAIR,
    REPAIRWIZARD_DELETE,
    REPAIRWIZARD_NOTDELETED,
    REPAIRWIZARD_CONCLUSION,
    REPAIRWIZARD_VIRUSFOUND,            // Unused
    REPAIRWIZARD_INOC                   // Unused
    };

                                        // ------------------------------------
                                        // Flags for the outside world to communicate
                                        // with the Repair Wizard. These are handled
                                        // like DWORD:
                                        // ------------------------------------

#define RWIZFLAG_USEADVANCED 0x01       // This flag is set by LaunchRepairWizard()
                                        // if the user selected NOT
                                        // to use the Repair Wizard in the first panel.
                                        // The outside world has to know this.

#define RWIZFLAG_CANCEL      0x02       // This flag is set by LaunchRepairWizard()
                                        // if the user hit Cancel in the Repair Wizard.
                                        // The outside world has to know this.



                                        // ------------------------------------
                                        // This structure is used as a reference
                                        // on what can be done for each possible
                                        // file status.
                                        // ------------------------------------
#ifdef SYM_WIN
  typedef   UINT    STRING_REF;
#else
  typedef   char *  STRING_REF;
#endif

typedef struct tagFILESTATUSSTRUCT
{
    STRING_REF  uFileStatusString;
    STRING_REF  uFollowHelp;
    BYTE        byCanRepair;
    BYTE        byCanDelete;
    BYTE        byCanExclude;
    BYTE        byCanInoc;
    BYTE        bycanQuarantine;
    BYTE        byHasVirusInfo;
} FILESTATUSSTRUCT, FAR * LPFILESTATUSSTRUCT;

// **************************************************** //
// Ask question structure to be filled with info to ask //
// **************************************************** //

typedef struct tagASKQUESTSTRUCT
{
    HWND        hDlg;                   // Parent window
    UINT        uType;                  // one of the ASKQUESTION_XXXXX
    LPSTR       lpFileName;             // Name of file
    LPSTR       lpVirusName;            // Infected with virus
} ASKQUESTSTRUCT, FAR * LPASKQUESTSTRUCT;

// *********************************** //
// Timer number identification numbers //
// *********************************** //

#define TIMER_SCANNED 1
#define TIMER_USRWAIT 2

// ************************************************************ //
// Define statements identifying the status of a processed file //
// DO NOT CHANGE THE ORDER OF THESE DEFINES. IF YOU DO NAV WILL //
// BE ROYALLY SCREWED UP.                                       //
// Must match the order of entries in FileStatus array.         //
// ************************************************************ //

enum
    {
    FILESTATUS_NOT_USED = 0,
    FILESTATUS_INFECTED,
    FILESTATUS_REPAIRED,
    FILESTATUS_NOTREPAIRED,
    FILESTATUS_DELETED,
    FILESTATUS_NOTDELETED,
    FILESTATUS_EXCLUDE_INOCULATION,
    FILESTATUS_EXCLUDE_VIRUSDETECTION,
    FILESTATUS_EXCLUDE_FAILED,
    FILESTATUS_INOCULATED,
    FILESTATUS_INOCULATIONFAILED,
    FILESTATUS_INOCULATEREPAIRED,
    FILESTATUS_INOCULATEREPAIR_FAILED,
    FILESTATUS_INOCULATEDELETED,
    FILESTATUS_INOCULATEDELETE_FAILED,
    FILESTATUS_INOC_CHANGED_DELETED,
    FILESTATUS_INOC_CHANGED_NOTDELETED,
    FILESTATUS_REINOCULATED,
    FILESTATUS_REINOCULATIONFAILED,
    FILESTATUS_NOTINOCULATED,
    FILESTATUS_INOCULATEDCHANGED,
    FILESTATUS_BOOTINFECTED,
    FILESTATUS_BOOTREPAIRED,
    FILESTATUS_BOOTNOTREPAIRED,
    FILESTATUS_MASTERBOOTINFECTED,
    FILESTATUS_MASTERBOOTREPAIRED,
    FILESTATUS_MASTERBOOTNOTREPAIRED,
    FILESTATUS_ERROR,
    FILESTATUS_ERRORDELETED,
    FILESTATUS_ERRORNOTDELETED,
    FILESTATUS_ZIPINFECTED,
    FILESTATUS_MEMORYINFECTED,
    FILESTATUS_INTEG_MBREPAIRED,
    FILESTATUS_INTEG_MBREPAIRFAILED,
    FILESTATUS_INTEG_BOOTREPAIRED,
    FILESTATUS_INTEG_BOOTREPAIRFAILED,
    FILESTATUS_INTEG_SYSFILEREPAIRED,
    FILESTATUS_INTEG_SYSFILEREPAIRFAILED,
    FILESTATUS_INTEG_MBCHANGED,
    FILESTATUS_INTEG_BOOTCHANGED,
    FILESTATUS_INTEG_SYSFILECHANGED,
    FILESTATUS_INTEG_REINOCULATED,
    FILESTATUS_INTEG_REINOCULATEFAILED,
    FILESTATUS_INTEG_CREATED,
    FILESTATUS_INTEG_CREATEFAILED,
    FILESTATUS_INTEG_NOTEXIST,
    FILESTATUS_QUARANTINE_INFECTED,
    FILESTATUS_QUARANTINE_INFECTED_FAILED,
    FILESTATUS_QUARANTINE_NOTINOCULATED,
    FILESTATUS_QUARANTINE_NOTINOCULATED_FAILED,
    FILESTATUS_QUARANTINE_INOCULATEDCHANGED,
    FILESTATUS_QUARANTINE_INOCULATEDCHANGED_FAILED,
    FILESTATUS_QUARANTINE_INFECTED_ZIP
    };


// Bit flag defines for IsMatchingStatusType() and FillProblemListBox()

#define ST_VIRUS            (1 <<  0)
#define ST_INOCULATION      (1 <<  1)
#define ST_CAN_REPAIR       (1 <<  2)
#define ST_CAN_DELETE       (1 <<  3)
#define ST_CAN_EXCLUDE      (1 <<  4)
#define ST_CAN_INOCULATE    (1 <<  5)
#define ST_HAS_INFO         (1 <<  6)
#define ST_LIVEVIRUS        (1 <<  7)
#define ST_ZIP              (1 <<  8)
#define ST_FILE             (1 <<  9)
#define ST_MBR              (1 << 10)
#define ST_BOOT             (1 << 11)

#define ST_ALL              ((DWORD)-1)


// ************************************************************** //
// **  A pointer to SCANSTRUCT must be passed to ScanStart()   ** //
// ************************************************************** //

#define SWITCH_DONTCHANGE  0xFFFF

// ********************************************************
// These two defines are used in wCallWhenDone in the DOS
// version to describe the program's operational mode
// ********************************************************
// These defines are used in uScanMode in the WIN version
// to describe the operational mode.
// wCallWhenDone is used by WIN to post IDM_SCAN_EXIT on
// command line scans.
// &? MOREWORK - standardize both on the new field.
// ********************************************************

typedef struct tagSCANITEM
{
    BOOL        bScanSubDirs;
    LPTSTR      lpszPathName;
} SCANITEM, *PSCANITEM, FAR *LPSCANITEM;

#define PROGRAM_INTERACTIVE   0         // User is at console pressing buttons.
#define PROGRAM_COMMANDLINE   1         // Mostly automated but prompts if virus.
#define PROGRAM_DOWNLOAD      2         // For web-browser helper command line.
#define PROGRAM_NAVUILAUNCH   3         // Launched by NAVUI(Nav32.exe) front end.
#define PROGRAM_AUTOMATED     4         // No user is around so automate everything.

typedef struct tagSCANSTRUCT
{
    HWND         hDlg;                  // Parent Window Handle
    BOOL         bScanSubs;             // Scan Sub directories
    BOOL         bMemOnly;              // Scan Memory only
    BOOL         bBootOnly;             // Scan Boot only
    UINT         uScanMemory;           // Memory Should be scanned
    UINT         uScanBoot;             // Boot should be scanned
    WORD         wCallWhenDone;         // WM_COMMAND, ID to call when done
    HGLOBAL      hScanList;             // List of objects to scan
    LPNAVOPTIONS lpNavOptions;          // Required for the current settings
    LPBOOL       lpBoolScanning;        // Set to FALSE when done scanning.
    char         szScanFileSpec[SYM_MAX_PATH+1]; // When scanning specified file spec.
    UINT         uProgramMode;          // Indicate interactive vs command line
    BOOL         bFileDeleted;          // Don't save if deleted!
    BOOL         bDeleteZipFile;        // Delete when done?
                                        // Save the filename for download.
    char         szSaveFileName [SYM_MAX_PATH +1];
    BOOL         bCompressedDownload;   // Store whether or not the download was a compressed file.
    UINT         uCpuLoad;              // Target CPU load
    BOOL         bHMemLaunch;           // TRUE if we were launched in memory-mapped /HMEM
                                        // mode (i.e., from Norton File Manager)
    LPSCANITEM   lpScanItems;           // NULL/Zero terminated array of
                                        // file scan specs indexes and
                                        // supercedes info in hScanList.
} SCANSTRUCT, FAR *LPSCANSTRUCT;

// ************************************************************ //
//  Macros
// ************************************************************ //

#define ScanDriveSkipped(p) (p[1] == '#')

// ************************************************************ //
// MAX_INFECTSTRUCTS * sizeof(INFECTEDFILE) must not be greater //
// than 64K !                                                   //
// ************************************************************ //

#ifdef SYM_WIN
#define MAX_INFECTSTRUCTS  100
#else
#define MAX_INFECTSTRUCTS  50
#endif

typedef struct tagINFECTEDFILE
{
    WORD        wStatus;                // See FILESTATUS_xxx defines
    UINT        uVirusSigIndex;         // Virus signature index.
    char        szFullPath [ SYM_MAX_PATH+1 ];
    char        szFullOemPath [ SYM_MAX_PATH+1 ];
    N30NOTES    Notes;

    // Fields added for container support
    BOOL        bInContainer;           // TRUE if infection is in a container
    LPTSTR      pszContainerItem;       // Fully-qualified container item name
                                        // This field should be set using
                                        // the Rec2.lib library function
                                        // BuildContainerString...
    UINT        uGroup;                 // Group # (used internally to
                                        // group container items together)
} INFECTEDFILE, FAR *LPINFECTEDFILE;

typedef struct tagINFARR
{
    UINT        uNumUsed;               // Number of INFECTEDFILE used of the
    HGLOBAL     hInf;                   // allocated MAX_INFECTSTRUCTS. If full
    LPSTR       lpNext;                 // this pointer contains another InfArr
} INFARR, FAR *LPINFARR;

// ********************************************
// The ZIPDETECT structure is currently ONLY
// used by the SCAND project (DOS scanner).
// The Windows scanner now uses the container
// fields in the INFECTEDFILE structure for
// tracking container information.
typedef struct tagZIPDETECT
{
    BOOL        bInZip;
    LPSTR       lpZipFileName;
    char        szZipChildFile [ SYM_MAX_PATH+1 ];
} ZIPDETECT, FAR * LPZIPDETECT;
// ********************************************

typedef struct tagEXTENDEDOPTIONS
{
    BOOL           bLogAccessDenied;
    char           Reserved[MAX_RESERVED_SIZE];
} EXTENDEDOPTIONS, FAR * LPEXTENDEDOPTIONS;

typedef struct tagNONAVSTRUCT
{
    HGLOBAL        hNonav;
    LPINFARR       lpInfArr;
    INFECTEDFILE   Infected;            // Current item being scanned.
#ifdef SYM_WIN32
    TCHAR          szInfectedLFN [SYM_MAX_PATH+1]; // Full LFN of current item.
    TCHAR          szInfectedSFN [SYM_MAX_PATH+1]; // Full SFN of current item.
#endif

    // ******************************************** //
    // Variables used to control decomposition      //
    // ******************************************** //
    BOOL           bDecomposeAllFiles;
    LPSTR          lpszDecomposeFileTypes;

    BOOL           bIgnoreAll;          // Ignore all viruses (used only during download scan)
    BOOL           bDoScan;             // Continue scanning flag
    UINT           uInocOpenOK;         // If Inoc was opened OK
    HWND           hMommyWindow;        // Parent Window handle

    // ********************************************
    // The ZIPDETECT structure is currently ONLY
    // used by the SCAND project (DOS scanner).
    // The Windows scanner now uses the container
    // fields in the INFECTEDFILE structure for
    // tracking container information.
    ZIPDETECT      Zip;
    // ********************************************

    LPINTEG        lpInteg;             // Required for system integrity.

    // ******************************************** //
    // Scan structure passed by calling application //
    // ******************************************** //

    LPSCANSTRUCT   lpScan;

    // ***************************************************************** //
    // ****              Network Notification Stuff                 **** //
    // ***************************************************************** //

    LPNETUSERITEM        lpNetUserItem;

    // **************************** //
    // Statistics related variables //
    // **************************** //

    struct
        {
        struct
            {
            DWORD   dwScanned;
            UINT    uInfected,
                    uCleaned,
                    uQuarantined,
                    uDeleted;
            } Files;

        struct
            {
            UINT    uScanned,
                    uInfected,
                    uCleaned;
            } BootRecs;

        struct
            {
            UINT    uScanned,
                    uInfected,
                    uCleaned;
            } MasterBootRec;

        UINT    uDrivesScanned;

        long    lTimeScan,
                lScanStart;
        } Stats;

    // ************************************************************* //
    // <<<<<<<<<<<<<    Windows scanner options    >>>>>>>>>>>>>>>>> //
    // ************************************************************* //
    SCANOPT     Opt;

    // ************************************************************* //
    // <<<<<<<<<<<<<  General application options   >>>>>>>>>>>>>>>> //
    // ************************************************************* //
    GENOPT      Gen;

    // ************************************************************* //
    // <<<<<<<<<<<<<<<<<<<<   Alert Options   >>>>>>>>>>>>>>>>>>>>>> //
    // ************************************************************* //
    ALERTOPT    Alert;

    // ************************************************************* //
    // <<<<<<<<<<<<<<<<   Activity Log Options   >>>>>>>>>>>>>>>>>>> //
    // ************************************************************* //
    ACTOPT      Act;

    // ************************************************************* //
    // <<<<<<<<<<<<<<<<   Inoculation  Options   >>>>>>>>>>>>>>>>>>> //
    // ************************************************************* //
    INOCOPT     Inoc;

    // ************************************************************* //
    // <<<<<<<<<<<<<<<<   Quarantine   Options   >>>>>>>>>>>>>>>>>>> //
    // ************************************************************* //
    QUARANTINESET Quarantineset;

    // ******************************************************** //
    // It's into this marvelous place where the directory we're //
    // currently scanning is stored.                            //
    // ******************************************************** //

    char        szTheDirectory [ SYM_MAX_PATH+1 ];

    // ********************************************************** //
    // This address to a procedure is used when scanning. It will //
    // either contain the address of the function which updates   //
    // the file and directories scanned, or it will contain the   //
    // the address to the procedure which draws the warp speed.   //
    // ********************************************************** //

    FARPROC     lpTimerProc;
    DWORD       dwTotalKBytes,          // Used for progress bar //
                dwScannedKBytes;

    LPEXCLUDE   lpExclude;              // Exclude Info
    BOOL        bExcludeDirty;          // Set if exclusions modified.

    DWORD       dwClusterSize;          // Cluster Size for current drive

    BOOL        bScanWholeDrive;        // Scan whole drive flag

    UINT        uMatchType;             // Required for LFN support.

    EXTENDEDOPTIONS ExtendedOpts;       // Extended Options

} NONAV, FAR * LPNONAV;


// Defines for DOALLSTRUCT.uFlags

#define DA_NO_NOTIFY        (1 << 0)    // Don't complain on errors.
#define DA_CONFIRM_DELETE   (1 << 1)    // Yes,YesAll,No,Cancel on DeleteAll.
#define DA_CONFIRM_STOP     (1 << 2)    // Get confirm when user presses stop

typedef struct tagDOALLSTRUCT
{
    BOOL    bStop;
    UINT    uType;
    UINT    uNumItems;
    HWND    hListbox;
    LPNONAV lpNav;
    UINT    uFlags;
} DOALLSTRUCT, FAR * LPDOALLSTRUCT;

//************************************************************************
// Windows function prototypes
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SYM_WIN

// n_misc.cpp //
BOOL SYM_EXPORT WINAPI DeleteOneContainerItem (LPINFECTEDFILE lpInfected);
BOOL SYM_EXPORT WINAPI QuarantineOneContainerItem (LPINFECTEDFILE lpInfected);
BOOL SYM_EXPORT WINAPI ProcessContainer (LPTSTR pszContainerName, void *pItem);

VOID SYM_EXPORT WINAPI NavworkInit      (VOID);
VOID SYM_EXPORT WINAPI NavworkEnd       (VOID);
HWND SYM_EXPORT WINAPI NoNavScanMain    (VOID);
WORD SYM_EXPORT WINAPI GetDir           (LPSTR, UINT);
VOID SYM_EXPORT WINAPI BldTimerBuf      (DWORD, LPSTR);
VOID SYM_EXPORT WINAPI DrawBitmap       (HDC, HBITMAP);

// n_result.c //
VOID SYM_EXPORT WINAPI ActUponFileStatus    (HWND hDlg, LPNONAV lpNav, LPINFECTEDFILE, int);
VOID SYM_EXPORT WINAPI ShowScanResDlg       (HWND, LPNONAV, BOOL);
BOOL SYM_EXPORT WINAPI OkToShowFileInfo     (UINT uFileStatus);
BOOL SYM_EXPORT WINAPI IsInoculateStatus    (WORD wFileStatus);
VOID SYM_EXPORT WINAPI MarkCompressedFilesQuarantined ( 
    LPNONAV lpNav,
    LPINFARR lpInfArr,
    HWND hListbox,
    HWND hDlg );

// n_wizard.cpp //
VOID SYM_EXPORT WINAPI LaunchCleanupWizard  (HWND, LPNONAV, UINT, LPDWORD);
BOOL SYM_EXPORT WINAPI ConfirmWizardCancel  (LPNONAV, HWND);

// n_inoc.c //
BOOL SYM_EXPORT WINAPI AskInoc         (LPNONAV, BOOL);
BOOL SYM_EXPORT WINAPI FileInoculated  (LPNONAV);

// n_inocfi.c //
BOOL SYM_EXPORT WINAPI InoculateFile   (LPNONAV lpNav);
BOOL SYM_EXPORT WINAPI InocFileChanged (LPNONAV lpNav);

// n_virus.c //
BOOL SYM_EXPORT WINAPI ShowInfected    (HWND, LPNONAV);
BOOL SYM_EXPORT WINAPI ShowInfectFile  (HWND, LPINFECTEDFILE, UINT);
VOID SYM_EXPORT WINAPI FreeLinkList    (LPNONAV);
VOID SYM_EXPORT WINAPI AddVirusInfo    (LPNONAV);
BOOL WINAPI FillProblemListBox (HWND hListbox, LPNONAV lpNav, DWORD dwItemTypes);
BOOL WINAPI IsMatchingStatusType (WORD wStatus, DWORD dwTypes);
BOOL WINAPI IsStatusTypeInList (LPNONAV lpNav, DWORD dwType);
BOOL WINAPI FillVirusListBox (HWND hListbox, LPNONAV lpNav);
BOOL SYM_EXPORT WINAPI ShowVirusName (HWND, LPINFECTEDFILE, UINT);

// n_info.c //
VOID SYM_EXPORT WINAPI ShowScanInfoDlg  (HWND, HWND, LPNONAV, BOOL);
VOID SYM_EXPORT WINAPI FoundVirusInfo   (HWND, LPNONAV);

// navini.c //
VOID SYM_EXPORT WINAPI ReadIniScanner   (HINSTANCE, LPNONAV, LPNAVOPTIONS);
LRESULT WINAPI Control3D            (HWND, WPARAM, LPARAM);

// n_repkil.c //
BOOL SYM_EXPORT WINAPI VirusFound       (LPNONAV, WORD *);
WORD SYM_EXPORT WINAPI HaltCPU          (HWND, LPNONAV);
DWORD SYM_EXPORT WINAPI ShouldUseRepairWizard ( LPNONAV );

// n_questi.c //
UINT SYM_EXPORT WINAPI AskQuestion      (LPASKQUESTSTRUCT);

VOID PASCAL NameDisplayShortenedFileName (
    HWND    hCtrl,
    LPSTR   lpszName,
    int     nLines = 1 );

// n_scan.c //
UINT SYM_EXPORT WINAPI ScanTheFile      (LPNONAV);
VOID SYM_EXPORT WINAPI ScanTime         (HWND, BOOL);
UINT SYM_EXPORT WINAPI ScanProc         (HWND, LPNONAV);
VOID SYM_EXPORT WINAPI YieldToWindows   (HWND hWnd);

// n_scan2.c //
VOID SYM_EXPORT WINAPI DecideWhatToDo   (LPNONAV);
WORD SYM_EXPORT WINAPI DecideWhatToDoWithZip (LPNONAV lpNav, void *pObject, BOOL *pbDoFree);
VOID SYM_EXPORT WINAPI DecideWhatToDoAfterProcessingZip (LPNONAV lpNav, LPINFARR lpInfArr);

// n_scanvw.c //
VOID SYM_EXPORT WINAPI ViewerDecideWhatToDo   (LPNONAV);
WORD SYM_EXPORT WINAPI ViewerDecideWhatToDoWithZip (LPNONAV lpNav, BOOL *pbDoFree);

// n_ssave.c //
VOID SYM_EXPORT WINAPI TryToSaveTheFile   (HWND, LPNONAV);

// n_scanio.c //
VOID SYM_EXPORT CALLBACK DrawNavIcon    (HWND, UINT uPercent);
VOID SYM_EXPORT CALLBACK StatsTimer     (HWND, UINT, UINT, DWORD);
VOID SYM_EXPORT WINAPI UpdateDir        (HWND, LPSTR, BOOL);
VOID SYM_EXPORT WINAPI ShowVirusFound   (HWND, LPSTR, UINT);
VOID SYM_EXPORT WINAPI ScanMemBootStat  (HWND, LPNONAV);
VOID SYM_EXPORT WINAPI ScanRes_ScanMemBootStat  (HWND, LPNONAV);

// n_summary.c //
VOID SYM_EXPORT WINAPI FillSummaryBox   (HWND, LPNONAV);
VOID SYM_EXPORT WINAPI ShowScanSumDlg   (HWND, LPNONAV);

// n_questi.c //
VOID SYM_EXPORT WINAPI DoAll (HWND hDlg, LPDOALLSTRUCT lpDo);

// ****************************************************************** //
// **************           n_dowork.c          ********************* //
// ****************************************************************** //

BOOL SYM_EXPORT WINAPI RepairBoot (
    LPNONAV lpNav,
    HWND    hDlg,
    BOOL    bNotify
    );

BOOL SYM_EXPORT WINAPI NavDeleteFile (
    LPNONAV lpNav,
    HWND    hDlg,
    BOOL    bNotify
    );

BOOL SYM_EXPORT WINAPI NavMoveFile (
    LPNONAV lpNav,
    HWND hDlg,
    BOOL bNotify
    );

BOOL SYM_EXPORT WINAPI NavMoveFileInf (
    LPNONAV         lpNav, 
    LPINFECTEDFILE  lpInfected, 
    HWND            hDlg, 
    BOOL            bNotify);

BOOL SYM_EXPORT WINAPI InocThisFile (
    LPNONAV lpNav,
    HWND    hDlg,
    BOOL    bNotify
    );

BOOL SYM_EXPORT WINAPI InocDeleteFile (
    LPNONAV lpNav,
    HWND    hDlg,
    BOOL    bNotify
    );

BOOL SYM_EXPORT WINAPI ExcludeAddFile (
    LPNONAV lpNav,
    HWND    hDlg,
    BOOL    bNotify
    );

VOID WINAPI NotifyRepairCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    );

VOID WINAPI NotifyInocRepairCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    );

VOID WINAPI NotifyDeleteCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    );

VOID WINAPI NotifyMoveCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    );

VOID WINAPI NotifyInocDeleteCompletion (
    HWND    hDlg,
    BOOL    bStatus,
    LPNONAV lpNav
    );

VOID WINAPI GetSysIntegFileOem (
    LPSTR       lpPath,
    UINT        uSize,
    LPINOCOPT   lpInoc
    );

// ****************************************************************** //

// n_scnstr.c //
HWND SYM_EXPORT WINAPI ScanStart        (LPSCANSTRUCT lpScan);
LRESULT SYM_EXPORT CALLBACK ScanDlgProc (
    HWND    hDlg,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam );

// n_memscn.c //
BOOL SYM_EXPORT WINAPI MemScanDlg       (HWND, LPNONAV);
BOOL SYM_EXPORT WINAPI DriveScanned     (LPSTR lpDriveList, BYTE byDrive);
VOID SYM_EXPORT WINAPI NAV_CenterDialog (HWND hDlg);
VOID SYM_EXPORT WINAPI RemoveDriveFromList (LPSTR lpScanList, BYTE byDrive);
BOOL SYM_EXPORT WINAPI ScanBootRecord   (HWND hDlg, LPNONAV lpNav, HWND hFillBoot);
BOOL SYM_EXPORT WINAPI ScanMasterBootRecord (HWND hDlg, LPNONAV lpNav,
                                         HWND hFillMBoot);

// n_scnboo.c //
UINT SYM_EXPORT WINAPI ScanMasterBootRec(HWND, LPNONAV);
UINT SYM_EXPORT WINAPI ScanBootRec      (HWND, LPSTR, LPNONAV);
VOID SYM_EXPORT WINAPI ReportVirusInMem (HWND, LPNONAV);
BOOL SYM_EXPORT WINAPI ReportVirusInBoot(HWND, BYTE, LPNONAV);

// n_print.c //
HGLOBAL SYM_EXPORT WINAPI BuildNavVirusInfo (HWND, BOOL);
HGLOBAL SYM_EXPORT WINAPI BuildResultInfo   (HWND, LPNONAV);
VOID    SYM_EXPORT WINAPI TrimFileName      (LPSTR lpFileName);
UINT    SYM_EXPORT WINAPI FileStatusString  (WORD wFileStatus, LPSTR lpMem);

// n_sysint.c //
BOOL SYM_EXPORT WINAPI Integrity            (HWND hDlg, LPNONAV lpNav);
BOOL SYM_EXPORT WINAPI SysIntegPrompt       (HWND hDlg, LPNONAV lpNav);
UINT SYM_EXPORT WINAPI DoWholeIntegScenario (HWND hDlg, LPNONAV lpNav, BOOL bPrompt);
BOOL SYM_EXPORT WINAPI InocPasswordVerify   (HWND hDlg, LPNONAV lpNav);

// n_wrilog.c //
UINT SYM_EXPORT WINAPI WriteNavLogs         (HWND hDlg, LPNONAV lpNav);
VOID SYM_EXPORT WINAPI NotifyLogWriteError  (HWND hDlg, LPNONAV lpNav);

// n_forward.cpp
UINT SYM_EXPORT WINAPI ForwardFiles(HWND hDlg, LPNONAV lpNav);

// This is a globally saved pointer to the Scan dialog's NONAV object.
// All of the child dialogs store it using LP_GET and LP_SET (actually
// GetProp() and SetProp() ), but NT won't retain these handles during
// logins and logouts when we are launched from NT's Schedule service.
// (Physical storage for this is in n_scnstr.c) //
extern LPNONAV GloballySaved_lpNav;

#endif // (#ifdef SYM_WIN)

#ifdef __cplusplus
}
#endif

#endif  // (#ifndef INCL_NONAV)
