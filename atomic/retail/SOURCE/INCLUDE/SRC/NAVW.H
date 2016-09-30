// Copyright 1992-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navw.h_v   1.13   29 Oct 1998 21:29:20   CEATON  $
//
// Description:
//      This include file defines the constants, typdefs, prototypes for
//      the Norton AntiVirus for Windows Clinic.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/navw.h_v  $
// 
//    Rev 1.13   29 Oct 1998 21:29:20   CEATON
// Added function prototype for HasNewDefsArrived().
// 
// 
//    Rev 1.12   15 Sep 1998 18:54:56   CEATON
// Added function prototypes to process incoming virus definition notification
// (fixes problem where disk is accessed at set intervals).
//  
// 
//    Rev 1.11   10 Jul 1998 12:21:24   DBuches
// Added LoadStatusIcons().
// 
//    Rev 1.10   20 Jun 1998 14:08:48   tcashin
// Added two functions to check the age of the defs.
// 
//    Rev 1.9   11 Jun 1998 12:55:06   tcashin
// Don't include afxmt.h ... it breaks other projects.
// 
//    Rev 1.8   10 Jun 1998 22:25:34   tcashin
// Added routines for monitoring quarantine-related events.
// 
//    Rev 1.7   28 Apr 1998 09:16:08   DALLEE
// Routines for updating AP, Quar, and Def status in NAVW main dialog.
// 
//    Rev 1.6   30 Jun 1997 04:13:54   RFULLER
// Update for ROSWELL Build 13
//
//    Rev 1.5   19 Jun 1997 10:45:56   RFULLER
// Update for ROSWELL Build 11
//
//    Rev 1.4   22 Apr 1997 15:25:28   JBRENNA
// Add comment that CM does not need to update the BUILD_NUM string. It is
// automatically generated based on the info from NAVVER.H.
//
//    Rev 1.3   10 Apr 1997 13:31:42   RFULLER
// Roswell CM Test Build
//
//    Rev 1.2   07 Apr 1997 18:18:02   DALLEE
// Added BuilScanListIndex() prototype. Function used to build new-style
// input for ScanStart() includes per file item subdirs flag.
//
//    Rev 1.1   27 Mar 1997 15:50:34   DALLEE
// Removed old unused NDW drag drop info and functions.
//************************************************************************

#ifndef _NAVW_H
#define _NAVW_H

#include "navver.h"
#include "symcfg.h"
#include "options.h"
#include "nonav.h"                      // SCANSTRUCT and other defines now
                                        // used within.
                                        // ---------------------------------

#define NAV_MAIN_WND_CLASS      "nav"
#define NAV_OPENING_BMP_CLASS   "NavOpeningBmpClass"

#define NAV_QUARANTINE_INCR_EVENT    "NAV Quarantine Increment Event"
#define NAV_QUARANTINE_DECR_EVENT    "NAV Quarantine Deccrement Event"
#define NAV_QUARANTINE_ZERO_EVENT    "NAV Quarantine Zero Event"

                                        // ---------------------------------
                                        // File Ids for NMsgBox()
                                        // ---------------------------------
#define _NAVW_C                 1000
#define _NETW_C                 1001
#define _NAVWMCMD_C             1002

                                        // ---------------------------------
                                        // Defines used within the code. Set
                                        // build number based on value from
                                        // NAVVER.H.
                                        // ---------------------------------
                                        // CM, THIS IS AUTOMATICALLY UPDATED.
                                        // ---------------------------------
#define BUILD_NUM               "Build 13"NAV_BUILD_STRING

                                        // ---------------------------------
                                        // This is the initial size of the
                                        // buffer holding the filespecs
                                        // to be scanned. During runtime,
                                        // THE BUFFER MIGHT BE RESIZED TO
                                        // ADD MORE SPACE.
                                        // ---------------------------------
#define SIZE_SCANLIST           10000

#define NAV_MSG_TIME_TO_SCAN    (WM_USER + 100)
#define NAV_MSG_GET_NEW_INI     (WM_USER + 101)
#define NAV_MSG_STARTUP         (WM_USER + 102)
                                        // Add a downloaded file to scan queue.
#define NAV_MSG_QUEUEFILE       (WM_USER + 103)

                                        // RetVal's from QueueFileToPrevInst().
                                        // Also for uResponse field of
                                        // QUEUEDFILEREQUEST structure.
#define QUEUEFILE_NO_RESPONSE   0
#define QUEUEFILE_WILL_SCAN     1
#define QUEUEFILE_WONT_SCAN     2
#define QUEUEFILE_NOT_SENT      3

#define NAV_GETVALUE(w,f) ((BOOL)((w)&(f)))

#define NAV_BUTTONBAR_NONE           0x0000
#define NAV_BUTTONBAR_DISPLAY        0x0001
#define NAV_BUTTONBAR_TEXT           0x0002
#define NAV_BUTTONBAR_PICTURE        0x0004

                                        // ---------------------------------
                                        // Don't display substituted drives
                                        // ---------------------------------
#define NAV_NDRV_FIXED          (NDRV_FIXED & ~NDRV_SUBSTED)


                                        // ---------------------------------
                                        // Predefined "To do at startup" IDs
                                        // ---------------------------------
enum
    {
    NAVW_STARTUP_STARTSCAN = 1,
    NAVW_STARTUP_NAVBIN,
    NAVW_STARTUP_UPDATETSR,
    NAVW_STARTUP_NODEFS,
    NAVW_STARTUP_VXDADDERROR,
    NAVW_STARTUP_VXDREMOVEERROR,
    NAVW_STARTUP_CHECKWINNT
    };
                                        // The max length was determined
#define MAX_CONTROLPANEL_NAME 100       // from CTRLPANL.CPP.

                                        // Keep a queue of downloaded files
                                        // from web-browsers that we need
                                        // to scan.
typedef struct tagQUEUEDFILE
    {
    tagQUEUEDFILE FAR * lpNextFile;
    TCHAR               szFile [SYM_MAX_PATH];
    } QUEUEDFILE, FAR *LPQUEUEDFILE;

                                        // Real-time status of Auto-Protect

typedef struct tagAPSTATUS
    {
    BOOL    bEnabled;
    BOOL    bLoaded;
    } APSTATUS, FAR *LPAPSTATUS;

                                        // Current quarantine status

typedef struct tagQUARANTINESTATUS
    {
    UINT    uNewItems;
    UINT    uTotalItems;

    HANDLE  hQuarantineIncrEvent;
    HANDLE  hQuarantineDecrEvent;
    HANDLE  hQuarantineZeroEvent;
    HANDLE  hQuarantineDirChangeEvent;
    
    LPVOID  pQuarMonCS;

    } QUARANTINESTATUS, FAR *LPQUARANTINESTATUS;

                                        // Current virus definitions status

typedef struct tagDEFINITIONSSTATUS
    {
    WORD        wYear;
    WORD        wMonth;
    WORD        wDay;
    DWORD       dwRevision;
    HANDLE      hDefsUpdatedEvent;
    } DEFINITIONSSTATUS, FAR *LPDEFINITIONSSTATUS;

                                        // ---------------------------------
                                        // Internal NAVW.EXE structure
                                        // ---------------------------------
typedef struct tagNAVMAIN
{
    HWND                    hFocus;
    HWND                    hScanMain;  // Main Scan window handle
    SCANSTRUCT              ScanStruct;
    HGLOBAL                 hScanList;
    char                    szScanSpec [SYM_MAX_PATH+1];
    SCANOPT                 Scan;
    BOOL                    bUpdateTsr;
    BOOL                    bScanInProgress;
    BOOL                    bDownloadMode;
    LPQUEUEDFILE            lpDownloadedFiles;

    APSTATUS                rApStatus;
    QUARANTINESTATUS        rQuarantineStatus;
    DEFINITIONSSTATUS       rDefinitionsStatus;
} NAVMAIN, NEAR *NPNAVMAIN, FAR *LPNAVMAIN;


//************************************************************************
// Global Prototypes
//************************************************************************

VOID WINAPI StartScanning (
    HWND        hParent,                // [in] Handle of parent
    LPNAVMAIN   lpNavMain               // [in/out] Main window data struct
    );

HWND WINAPI NavOpeningBmpCreate (       // Creates opening credits wnd
    HWND        hWndParent              // [in] Handle of parent
    );

LRESULT SYM_EXPORT WINAPI NavOpeningBmpWndProc ( // Opening bitmap window proc
    HWND        hWnd,                   // [in] Window handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] wParam to process
    LPARAM      lParam                  // [in] lParam to process
    );

LRESULT WINAPI NavMainWndWM_COMMAND(    // Handles WM_COMMAND for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // [in] wParam of WM_COMMAND message
    LPARAM      lParam,                 // [in] lParam of WM_COMMAND message
    LPNAVMAIN   lpNavMain               // [in/out] Main window data struct
    );

BOOL WINAPI NavMainWndScanFile (        // Scans file specified by user
    HWND        hWnd,                   // [in] Main dialog window handle
    LPNAVMAIN   lpNav,                  // [in] pointer to nav structure
    LPSTR       lpFilePath,             // [out] full path to the file
    UINT        uFilePathLength         // [in] length of above
    );

VOID WINAPI NavMainWndStatusUpdate(     // Updates status line of main wnd
    HWND        hWnd,                   // [in] Main dialog window handle
    WORD        wIDStatus               // [in] ID of string to display
    );

VOID WINAPI NavMainOptionsLoad (        // Initializes Main Window Params
    HWND        hWnd,                   // [in] Handle to main window
    LPNAVMAIN   lpNavMainWnd,           // [in/out] Buffer to options record
    LPSTR       lpDrives                // [in] drives to preselect
    );

BOOL SYM_EXPORT WINAPI NavOptsProgramFilesDlgProc ( // Configure Program Files
    HWND        hDlg,                   // Dialog box handle
    UINT        uMessage,               // Message to process
    WPARAM      wParam,                 // wParam to process
    LPARAM      lParam                  // lParam to process
    );

VOID WINAPI DragDrop (                  // Used for drag/dropt
    HWND        hWnd,                   // Parent
    HDROP       hDrop,                  // HDROP
    LPNAVMAIN   lpNavMain               // [in/out] Buffer to options record
    );

WORD SYM_EXPORT WINAPI TranslateRegisteredMessage(
    WORD        msg
    );

VOID SYM_EXPORT WINAPI InocUninoc (
    HWND            hDlg,
    LPNAVOPTIONS    lpNavOptions
    );

VOID WINAPI NavScanFileFilterList (     // Builds filter list for scan dlg
    BOOL        bScanAll,               // [in] 'All files' should go first.
    LPSTR       lpszzFilterList         // [out] buffer to receive filter lst
    );

LPSTR WINAPI CatchMemoryFile (          // navwhmem.cpp
    LPSTR       szHandleID,
    LPDWORD     lpdwSizeOfData
    );

BOOL WINAPI CatchMemoryFile_Cleanup (   // navwhmem.cpp
    LPSTR       lpStr
    );

VOID WINAPI GatherCommandLineNames (
    LPSTR       lpszzScanList,
    UINT        uListSize,
    LPBOOL      lpbScanSubs,
    LPSTR       lpszScanFileNamesIn
    );

STATUS WINAPI QueueAddFile (            // navw.cpp Add a download for scanning.
    LPQUEUEDFILE FAR *lpNodePtr,        // Address of pointer to first node.
    LPCTSTR           lpszFilename      // File we want to scan.
    );

STATUS WINAPI QueueGetFile (            // navw.cpp Get first queued file.
    LPQUEUEDFILE FAR *lpNodePtr,        // Address of pointer to first node.
    LPTSTR            lpszFilename      // [out] Buffer for filename.
    );

VOID WINAPI QueueRemoveAll (            // navw.cpp Clear all files from queue.
    LPQUEUEDFILE FAR *lpNodePtr         // Address of pointer to first node.
    );

BOOL WINAPI LoadVirusDefs (             // Initialize virus scanning engine
    VOID                                // Nothing
    );

VOID WINAPI NavStartRPCServer (         // Initializes our RPC server for
    VOID                                // NAVAPSVC starting commands.
    );

VOID WINAPI NavStopRPCServer (          // DeInitializes our RPC server
    VOID
    );

LPSCANITEM BuildScanListIndex (         // Build new ScanStart() input index.
    LPTSTR  lpszzScanList,
    BOOL    bScanSubDirs
    );

BOOL UpdateApStatus(                    // Get current status of Auto-Protect.
    LPAPSTATUS lpApStatus
    );

BOOL UpdateQuarantineStatus(            // Get current status of Quarantine.
    LPQUARANTINESTATUS lpQuarantineStatus
    );

BOOL UpdateQuarantineIncrementCount(    // Increment the number of items in Quarantine.
    LPQUARANTINESTATUS lpQuarantineStatus 
    );

BOOL UpdateQuarantineDecrementCount(    // Increment the number of items in Quarantine.
    LPQUARANTINESTATUS lpQuarantineStatus 
    );

BOOL UpdateQuarantineZeroNewItems(      // Set the number of new items in Quarantine to 0.
    LPQUARANTINESTATUS lpQuarantineStatus 
    );


BOOL UpdateDefinitionsStatus(           // Get current status of definitions.
    LPDEFINITIONSSTATUS lpDefinitionsStatus
    );

BOOL UpdateCheckDefinitionAreOld(       // Check of the definitions are more than 30 days old.
    VOID 
    );

BOOL UpdateCheckDefinitionAreNewest(    // Check if newer definitions are on the machine.
    VOID 
    );

BOOL GetInitialDefinitionsDate(         // Get initial definitions date.
    VOID
    );

BOOL ProcessNewDefinitions( 
    HWND hWnd
    );

VOID ContinueDefinitionNotifications( 
    LPNAVMAIN lpNavMain 
    );

BOOL AreDefinitionsOld ( 
    VOID
    );

BOOL HasNewDefsArrived(
    VOID
    );

VOID DisplayApStatus(                   // Update main UI AP status controls.
    HWND        hWnd,
    LPNAVMAIN   lpNavMain
    );

VOID DisplaySecondaryStatus(            // Update secondary (Quarantine or Def)
    HWND        hWnd,                   // status controls.
    LPNAVMAIN   lpNavMain
    );

VOID StartStatusMonitor(                  // Start the Secondary Status Monitor
    LPNAVMAIN   lpNavMain
    );

VOID StopStatusMonitor(                   // Stop the Secondary Status Monitor
    LPNAVMAIN   lpNavMain
    );

DWORD QuarantineMonitorIncrEvent(       // Quarantine Monitor for Increment Events
    HANDLE      hQuarEvent, 
    LPNAVMAIN   lpNavMain
    );

DWORD QuarantineMonitorDecrEvent(       // Quarantine Monitor for Increment Events
    HANDLE      hQuarEvent, 
    LPNAVMAIN   lpNavMain
    );

DWORD QuarantineMonitorZeroEvent(       // Quarantine Monitor for Increment Events
    HANDLE      hQuarEvent, 
    LPNAVMAIN   lpNavMain
    );

DWORD WINAPI QuarantineMonitorDirChange(// Quarantine Monitor for directory change events
    LPVOID      lpvParam
    );

void LoadStatusIcons();                // Initializes status icons.


#endif  // _NAVW_H

