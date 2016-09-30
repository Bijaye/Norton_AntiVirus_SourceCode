// Copyright 1992-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVW/VCS/navwmcmd.cpv   1.63   29 Oct 1998 21:28:18   CEATON  $
//
// Description:
//      This file contians those routines necessary for handling the
// WM_COMMAND message for the main dialog window for the Norton AntiVirus
// for windows.
//
// Contains (as exports):
//
// See Also:
//      Navw.cpp, NavwFile.cpp NavwDir.cpp
//************************************************************************
// $Log:   S:/NAVW/VCS/navwmcmd.cpv  $
// 
//    Rev 1.63   29 Oct 1998 21:28:18   CEATON
// Pushing the "Info" button while "New Defs Arrived" is displayed will always
// show definitions status, even if items are quarantined.
// 
// 
//    Rev 1.62   22 Oct 1998 19:41:18   CEATON
// Check to see if any folders are selected before letting user exit "Scan Folders"
// dialog box.
// 
// 
//    Rev 1.61   21 Oct 1998 17:09:54   CEATON
// Added reference to ABOUTSW.H (contains function prototype for DoSystemWorksAbout()).
// 
// 
//    Rev 1.60   20 Oct 1998 20:29:44   CEATON
// Added call to SystemWorks upsell dialog box when NSW button is pressed.
// 
// 
//    Rev 1.59   08 Oct 1998 18:21:38   tcashin
// When the IDC_STATUS_QUAR_ENUM message is processed,
// call ProcessNewDefinitions().
// 
//    Rev 1.58   22 Sep 1998 23:54:24   cnevare
// Support for SystemWorks button, uses DoStdAbout().
// 
//    Rev 1.57   22 Sep 1998 23:07:10   tcashin
// Updated the activity log templates and filters.
// 
//    Rev 1.56   17 Jul 1998 18:45:56   jtaylor
// Updated creation of the Filter options for the activity log.  Now OEM and non-Quarantine enabled installs work well.
//
//    Rev 1.55   09 Jul 1998 17:48:14   DBuches
// Removed old help links.
//
//    Rev 1.54   02 Jul 1998 15:27:02   mdunn
// Fixed breakages on help IDs for build 22.
//
//    Rev 1.53   20 Jun 1998 13:41:12   tcashin
// Make the info dialog use the same checking order as the secondary status.
//
//    Rev 1.52   11 Jun 1998 12:50:10   tcashin
// Process messages to update the quarantine values.
//
//    Rev 1.51   07 Jun 1998 15:40:42   tcashin
// Handle to Info button.
//
//    Rev 1.50   03 Jun 1998 13:29:32   DBuches
// Changed product support help file name.
//
//    Rev 1.49   29 May 1998 15:30:20   DBuches
// Fixed font problem in ScanPathDialog.
//
//    Rev 1.48   28 May 1998 15:14:30   DBuches
// Removed virus list changes from activity log filter.
//
//    Rev 1.47   27 May 1998 15:34:38   DBuches
// Fixed yet another problem with launching Quarantine from NAVW.
//
//    Rev 1.46   22 May 1998 13:07:46   DBuches
// Added path to code that launches Qconsole.
//
//    Rev 1.45   20 May 1998 14:35:52   DBuches
// Added extra validation to ScanPath dialog.
//
//    Rev 1.44   19 May 1998 14:13:32   DBuches
// Removed Unknown Virus Detection from Log filter dialog.
//
//    Rev 1.43   18 May 1998 16:08:46   DBuches
// Added ScanPath dialog.
//
//    Rev 1.42   11 May 1998 16:55:54   DALLEE
// On Enable from auto-protect status window, load AP service and agent
// if not currently loaded.
//
//    Rev 1.41   28 Apr 1998 09:18:02   DALLEE
// Added legit AP enabling/disabling.
// Placeholder messages for Quarantine and Definitions status info.
//
//    Rev 1.40   23 Apr 1998 21:09:38   DALLEE
// Added resizing to Scan Folders dialog.
// Added kluged updating of main UI real-time status displays for usability.
//
//    Rev 1.39   20 Apr 1998 23:12:56   DALLEE
// Use new multi-select folder browse for CallScanDirectory().
//
//    Rev 1.38   14 Apr 1998 23:38:28   DALLEE
// Added Quarantine toolbar button and menu item.
// Removed code related to previously nuked AP toolbar button.
//
//    Rev 1.37   10 Apr 1998 11:05:44   tcashin
// Removed to ifdefs for activity log templates and filters. Changed it
// to use run-time checks for 95/NT instead.
//
//    Rev 1.36   01 Apr 1998 14:51:30   DALLEE
// Removed old button bar code.
// Removed inoculation menu item.
//
//    Rev 1.35   31 Mar 1998 14:20:10   tcashin
// Put the quarantine filter for activity log entries inside the
// correct set of #ifdefs so the NAVWNTPR target builds right.
//
//    Rev 1.34   30 Mar 1998 21:34:44   tcashin
// Added the Quarantine filters.
//
//    Rev 1.33   18 Nov 1997 12:19:30   mdunn
// Fix for NT AP prompting -- changed GatherCommandLineNames() to not pop up
// a file-not-found error message box if NAVW was run with the /HMEM switch.
//
//    Rev 1.32   21 Aug 1997 14:29:40   DBUCHES
// Fixed problems with handling files containing double-byte
// characters in GatherCommandLineNames().
//
//    Rev 1.31   10 Aug 1997 19:53:56   DBUCHES
// Removed obsolete help topics.
//
//    Rev 1.30   31 Jul 1997 09:00:40   BGERHAR
// Move help URL's and filename to resource, shorten vlog trail
//
//    Rev 1.29   29 Jul 1997 14:59:38   DALLEE
// In NotifyNavapW(), get Navapw's window class based on platform.
//
//    Rev 1.28   21 Jul 1997 01:59:32   BGERHAR
// Implemented /NORESULTS properly, fixed /HMEM handling
//
//    Rev 1.27   18 Jul 1997 17:25:40   BGERHAR
// [WIP] Add /SHOWRESULTS, fix /NORESULTS
//
//    Rev 1.26   16 Jul 1997 16:20:06   DBUCHES
// Added menu items to Help menu
//
//    Rev 1.25   15 Jul 1997 14:12:34   DBUCHES
// Added Ctl3d for DEC Alpha
//
//    Rev 1.24   09 Jul 1997 15:12:14   DBUCHES
// Added code to support adding ApAgent for NT.
//
//    Rev 1.23   06 Jul 1997 17:01:28   DBUCHES
// Completed last fix.  Found a way where we would not correctly verify a root directory entered on the command line
//
//    Rev 1.22   03 Jul 1997 11:39:08   DBUCHES
// Fixed problem with scanning drive letters on the commmand
// line.  Now, if user specifies a drive letter only (eg A:, A:\)
// we will proceed with a scan, regardless of whether there are files
// to scan on the drive.  This way, a boot sector scan will be performed.
//
//    Rev 1.21   01 Jul 1997 13:32:42   DBUCHES
// Fixed up handling of /A switch.  Now excludes floppy drives.
//
//    Rev 1.20   17 Jun 1997 11:44:04   TIVANOV
// comment out the quarantine entries in ActLogTemplate ad ActLogFilters
//
//    Rev 1.19   11 Jun 1997 14:49:08   MKEATIN
// Only scan high memory if /HIMEM is pass on the command line.
//
//    Rev 1.18   10 Jun 1997 21:13:42   MKEATIN
// Use VirusScanGetInfFile() to get the "VIRSCAN.INF" file for VIRLIST.
//
//    Rev 1.17   09 Jun 1997 20:25:46   TIVANOV
// hooking up the new about dlg box
//
//    Rev 1.16   09 Jun 1997 16:54:48   DBUCHES
// Ifdef'd out Ctl3d stuff for non-intel platfomrs.
//
//    Rev 1.15   04 Jun 1997 15:10:14   JBRENNA
// 1. Change the IDM_TOOLS_ONRAMP processing. Because there is a new directory
//    for each new definition that is downloaded, we do not need to call
//    VirusScanEnd() or pause NAVAPSVC from looking at the definitions.
// 2. Remove the g_bOnrampIsRunning processing. NAVW no longer waits for
//    LiveUpdate to finish before it continues processing.
// 3. Added a call to LoadVirusDefs() in StartScanning(). This ensures that
//    the NAVW scan will use the newest shared definitions directory on the
//    system. If the LoadVirusDefs() is not successful, disable scanning.
//
//    Rev 1.14   03 Jun 1997 14:38:46   JBRENNA
// Moved static LaunchLiveUpdate into the NavLuUtils.LIB under the new
// name of NavLuLaunchLiveUpdate.
//
//    Rev 1.13   31 May 1997 17:19:18   KSACKIN
// Changed the LaunchScheduler function to now launch NSCHEDNT.EXE when running
// within Windows NT.
//
//    Rev 1.12   19 May 1997 01:26:44   KSACKIN
// Converted the LaunchScheduler function to launch the Norton Program Scheduler
// when operating within Windows NT instead of launching our previous dialog
// from our scheduler dll.
//
//    Rev 1.11   16 May 1997 17:24:04   DALLEE
// Removed conflicting TRACE defines now that we're define SYM_MFC.
// TRACE seemed unused here.
// Removed references to "Netw.h" NAVNET-specific header file.
//
//    Rev 1.10   10 May 1997 02:06:30   JBRENNA
// Use LaunchLiveUpdate to start the NavLu32.EXE program. Previously,
// LiveUpdate was a DLL loaded by NAVW.
//
//    Rev 1.9   09 May 1997 23:45:48   JBRENNA
// Update NUMFILTERS_A to 4 (was 3).
//
//    Rev 1.8   09 May 1997 13:46:42   TIVANOV
// new entry in the log template and new entry in the logfilters - ActLogTemplat
// ates and ActLogFilters
//
//    Rev 1.7   22 Apr 1997 15:50:28   JBRENNA
// Merge w/ Tesla: add RPC code to start and stop the NAVAPSVC watching for
// updates to the virus definitions.
//
//    Rev 1.6   09 Apr 1997 15:20:30   TIVANOV
// remove #ifdef NAVSCANW from the code
// to support the right mouse click shell integration
//
//    Rev 1.5   07 Apr 1997 19:11:10   DALLEE
// Removed a few extra tidbits of debug code.
//
//    Rev 1.4   07 Apr 1997 18:52:54   DALLEE
// Added handling for NAVUI(NAV32.EXE) launch.
//
// Added GetRegistryNames() to build list of files NAVUI asked to scan.
//
// Updated input to ScanStart() to include SCANITEM array which indexes
// old hScanList and adds per-item scan subdirectories flags.
//
// Added BuildScanListIndex() to create new SCANITEM array input from
// old style hScanList input.
//
//    Rev 1.3   27 Mar 1997 15:54:18   DALLEE
// Removed unused old NDW drag drop code.
// Added support to query setting of /UILAUNCH command line switch.
// Moved huge mass of in-line scheduler code from WM_COMMAND handler to
//     separate function LaunchScheduler().
//
//    Rev 1.2   26 Mar 1997 21:04:22   DALLEE
// Removed NAVNET and WIN16 code and #ifdefs.
// Only valid remaining targets are W32 - NAVW, NAVWNT, NAVSCANW.
// Did some manual "pretty printing" of the source as well.
//
//    Rev 1.1   06 Mar 1997 17:20:46   DALLEE
// Port from HYDRA.15.
//
//    Rev 1.0   06 Feb 1997 21:01:30   RFULLER
// Initial revision
//
//************************************************************************

#include "platform.h"                   // Platform independence
#include <shellapi.h>
#include "windowsx.h"                   // Windows control functions
#include "xapi.h"
#include "disk.h"
#include "undoc.h"

#include "stdhelp.h"
#include "ngui.h"                       // Norton look / feel
#include "ctl3d.h"                      // Microsoft 3D look
#include "commdlg.h"                    // Windows 3.1 Common dialog
#include "disk.h"                       // For drive types
#include "ndrvlist.h"                   // For Norton drive list control
#include "msgdisp.h"                    // For Quake Message Display API

#include "navutil.h"
#include "common.h"
#include "navwres.h"                // NAV resource identifiers
#include "navwstb.h"                // NAV string table entries
#include "security.h"               // Password related defines
#include "navw_hlp.h"               // Help ids
#include "cmbstr.h"
#include "nonav.h"
#include "navalog.h"
#include "options.h"
#include "virlist.h"
#include "inoc.h"
#include "navw.h"
#include "virscan.h"
#include "navwcomm.h"
#include "dbcs.h"
#include "navdoabt.h"
#include "aboutsw.h"

#include "navapcom.h"
#include "apscomm.h"                    // For NAVAPSVC commands
#include "navrpc.h"                     // For protocols, addresses and enpoints

#include "netalert.h"
#include "actilog.h"
#include "NavLuUtils.h"
#include "NavVDefs.h"                   // For NavVDefs* functions.
#include "ddeml.h"
#include "progman.h"

#include "nvbrowse.h"
#include "wndsize.h"

#ifdef NAVSCANW
    #include "navntsch.h"
    #include "navver.h"
#endif

#include "regobj.h"                     // Communication with NAVUI front end.
#include "infodlg.h"
#include "navcmdid.h"

#include "n32behav.h"                  // Personality DLL.

//************************************************************************
// External data
//************************************************************************

extern          HINSTANCE           ghInst;
extern          TCHAR               gszCmdLine[];
extern          HWND                ghWndMain;

//************************************************************************
// Static variables for this file
//************************************************************************

static s_bHMEMSwitchUsed;               // Used to suppress the "missing
                                        // filename" error msg box if NAVW
                                        // is launched with the /HMEM switch.
                                        // See comments before the NotifyUser()
                                        // call in GatherCommandLineNames()
                                        // for more info.

//************************************************************************
// Local definitions
//************************************************************************
#define SIZE_APPNAME    60
#define SIZE_VERSION    30

                                        // Command line switches
enum
    {
    MEMORY_SWITCH = 1,
    BOOT_SWITCH,
    CFG_ALLDRIVES,
    CFG_LOCALDRIVES,
    CFG_BOOTONLY,
    CFG_MEMONLY,
    CFG_NOSUBS,
    CFG_OPTIONS,                        // Show options panel(s).
    CFG_MEMORYHANDLE,                   // Norton Navigator multiple drag and drop.
    CFG_DOWNLOAD,                       // Netscape help app mode.
    CFG_CPULOAD,
    CFG_SAGERUN,
    CFG_SAGESET,
    CFG_UILAUNCH,                       // Launched by NAVUI front end.
    CFG_HIMEM,
    CFG_NORESULTS,
    CFG_VISIBLE,                        // Parsed in navw.cpp:NavMainWndWM_CREATE()
    };

//************************************************************************
// Local Typedefs
//************************************************************************

#define NAV_AP_EXE      _T("navapw32.exe")
#define NAV_DEFAULT_DIR _T("C:\\WIN32APP\\NAVWNT")

//***************************************************************************
// Local prototypes
//***************************************************************************

UINT LOCAL SwitchState          (UINT uSwitchType);
UINT LOCAL IsConfigSwitchSet    (UINT uSwitchType);

VOID LOCAL NavVirlistDlgSetContextHelpIds ( LPVIRLIST lpsVirlist );

VOID LOCAL BuildScanList (              // Builds items to scan from raw data
    LPTSTR  lpScanList,                 // [out] formatted output placed here
    LPTSTR  lpRawData                   // [in] unformatted data
    );

VOID LOCAL FillWithDrives (
    LPTSTR  lpScanList,                 // Memory to fill
    BOOL    bAllDrives                  // All the drives or fixed
    );

VOID LOCAL ProcessCommandLine (
    LPTSTR          *lplpScanList,
    LPSCANSTRUCT    lpScan,
    LPNAVMAIN       lpNavMain
    );

VOID LOCAL GetRegistryNames (           // Read files to scan passed from NAVUI
    CNavRegComm     *pRegComm,          // [in] NAVUI communication object
    LPTSTR          *lplpScanList,      // [out] Ptr to scan list pointer
    HGLOBAL         *lphScanList,       // [out] Ptr to scan list memory handle
    LPSCANSTRUCT    lpScan              // [out] Scan options
    );

VOID LOCAL ScanSelected(
    HWND        hWnd,
    LPNAVMAIN   lpNavMain
    );

STATUS LOCAL ScanDownload(
    HWND        hWnd,
    LPNAVMAIN   lpNavMain
    );

BOOL LOCAL CallVirusList (
    HWND        hWnd
    );

VOID LOCAL CallScanDirectory (
    HWND        hWnd,
    LPNAVMAIN   lpNavMain
    );

VOID LOCAL CallScanPath (
    HWND        hWnd,
    LPNAVMAIN   lpNavMain
    );

BOOL CALLBACK ScanFoldersProc(
    HWND    hDlg,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam
    );

BOOL CALLBACK ScanPathProc(
    HWND hDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    );

VOID LOCAL LaunchScheduler (
    void
    );

VOID LOCAL LaunchQuarantineConsole();

BOOL LOCAL GetInfoAla31 (
    HWND        hWnd,
    LPTSTR      lpWork
    );

BOOL LOCAL GetInfoAla30 (
    HWND        hWnd,
    LPTSTR      lpWork
    );

VOID LOCAL NavOptsErrorNotify (         // If errors notify about it
    HWND        hParent,                // [in] parent window handle
    LPALERTOPT  lpAlert,                // [in] alert options
    WORD        wStatus                 // [in] status returned from opts
    );

VOID LOCAL NotifyNavapw (               // Notify NAVAPW about something to do
    WORD    wStatus                     // [in] status from options update.
    );

VOID LOCAL DisplayInfoDeskHelp (        // Just as the name sez.
    HWND    hWnd                        // [in] parent window handle
    );

VOID SYM_EXPORT WINAPI NavTeamHookProc ( // About Box Hook Proc
    HWND        hWnd,                   // [in] Window handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] Message wParam
    LPARAM      lParam                  // [in] Message lParam
    );

BOOL SYM_EXPORT CALLBACK NavEggProc (   // Display Team Photo
    HWND        hDlg,                   // [in] Dialog Box handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] Message wParam
    LPARAM      lParam                  // [in] Message lParam
    );

void LOCAL PASCAL CenterMain(HWND hWnd);
WORD DIBNumColors (LPTSTR lpbi);
WORD PaletteSize (LPTSTR lpbi);
HPALETTE CreateDIBPalette (HANDLE hDIB);
void LOCAL PASCAL SetupAPAgentStartGroupItem();
BOOL LOCAL SIWQuoteLFN(LPTSTR pathName, int sizePathName);
BOOL LOCAL SIWSpacesPresent(LPTSTR pathName);

/* **************************************************************************

@Name:
LRESULT WINAPI NavMainWndWM_COMMAND(    // Handles WM_COMMAND for main wnd
    HWND          hWnd,                 // [in] Window handle
    WPARAM        wParam,               // [in] wParam of WM_COMMAND message
    LPARAM        lParam,               // [in] lParam of WM_COMMAND message
    LPNAVMAIN     lpNav                 // [in/out] Main window data struct
    );

@Description:
This routine handles the WM_COMMAND message for the main dialog window
of the Norton AntiVirus for Windows.  Processing depends on the control
the user interacted with.

@Parameters:
$hWnd$            [in] Window handle of main dialog window
$wParam$          [in] wParam of WM_COMMAND message
$lParam$          [in] lParam of WM_COMMAND message
$lpNavMain$       [in/out] data structure for main window
$lpNavScanSpec$   [in/out] data structure for what to scan

@Returns:
      0L              message processed
      DefDlgProc()    message not processed.

************************************************************************** */

LRESULT WINAPI NavMainWndWM_COMMAND(    // Handles WM_COMMAND for main wnd
    HWND          hWnd,                 // [in] Window handle
    WPARAM        wParam,               // [in] wParam of WM_COMMAND message
    LPARAM        lParam,               // [in] lParam of WM_COMMAND message
    LPNAVMAIN     lpNav                 // [in/out] Main window data struct
    )
{
    extern          HELPSTRUCT          HelpInfo;
    extern          NAVMAIN             NavMain;

// Below are the tables for the content of the Activity log filter dialog.
// They are setup to dynamically create the dialog.  95 has an OEM and a non-OEM
// version.  For NT they are the same, so there is only one.
// In all cases, Quarantine is the last item in the list.  This allows for Quarantine
// to be removed by reporting one fewer items than is actually in the list.  If Quarantine
// is ever changed to not be the bottom item, then the mechanism for removing Quarantine
// must be updated.

#define NUMFILTERS_A_95     6
#define NUMFILTERS_A_95_OEM 5
#define NUMFILTERS_A_NT     4

    static ALOGFORMAT ActLogTemplates95[NUMFILTERS_A_95]=
        {
        {ALOG_FI_KNOWNVIRUS,   IDS_TEMPLATE_KNOWNVIRUS},
        {ALOG_FI_INOCHANGE,    IDS_TEMPLATE_INOCHANGE},
        {ALOG_FI_VIRUSLIKE,    IDS_TEMPLATE_VIRUSLIKE},
        {ALOG_FI_STARTEND,     IDS_TEMPLATE_STARTEND},
        {ALOG_FI_ERROR,        IDS_TEMPLATE_ACCESSDENIED},
        {ALOG_FI_QUARANTINE,   IDS_TEMPLATE_QUARANTINE},
        };

    static UINT ActLogFilters95[NUMFILTERS_A_95]=
        {
        IDS_FILTER_KNOWN,
        IDS_FILTER_INOCHANGE,
        IDS_FILTER_VIRUSLIKE,
        IDS_FILTER_STARTEND,
        IDS_FILTER_ACCESSDENIED,
        IDS_FILTER_QUARANTINE,
        };

    static ALOGFORMAT ActLogTemplates95_OEM[NUMFILTERS_A_95_OEM]=
        {
        {ALOG_FI_KNOWNVIRUS,   IDS_TEMPLATE_KNOWNVIRUS},
        {ALOG_FI_VIRUSLIKE,    IDS_TEMPLATE_VIRUSLIKE},
        {ALOG_FI_STARTEND,     IDS_TEMPLATE_STARTEND},
        {ALOG_FI_ERROR,        IDS_TEMPLATE_ACCESSDENIED},
        {ALOG_FI_QUARANTINE,   IDS_TEMPLATE_QUARANTINE},
        };

    static UINT ActLogFilters95_OEM[NUMFILTERS_A_95_OEM]=
        {
        IDS_FILTER_KNOWN,
        IDS_FILTER_VIRUSLIKE,
        IDS_FILTER_STARTEND,
        IDS_FILTER_ACCESSDENIED,
        IDS_FILTER_QUARANTINE,
        };

    static ALOGFORMAT ActLogTemplatesNT[NUMFILTERS_A_NT]=
        {
        {ALOG_FI_KNOWNVIRUS,   IDS_TEMPLATE_KNOWNVIRUS},
        {ALOG_FI_STARTEND,     IDS_TEMPLATE_STARTEND},
        {ALOG_FI_ERROR,        IDS_TEMPLATE_ACCESSDENIED},
        {ALOG_FI_QUARANTINE,   IDS_TEMPLATE_QUARANTINE},
        };

    static UINT ActLogFiltersNT[NUMFILTERS_A_NT]=
        {
        IDS_FILTER_KNOWN,
        IDS_FILTER_STARTEND,
        IDS_FILTER_ACCESSDENIED,
        IDS_FILTER_QUARANTINE,
        };

    auto            TCHAR               szAppName[SIZE_APPNAME+1];
    auto            LRESULT             lResult = 0L;
    auto            UINT                uStatus;
    auto            LPACTILOG           lpAct;
    auto            CHECKSTRUCT         PwCheck;
    auto            TCHAR               szWork [ SYM_MAX_PATH+1 ];
    auto            TCHAR               szFile [ SYM_MAX_PATH+1 ];
    auto            PASSWORDSTRUCT      Pass;
    auto            LPTSTR              lpScanList;

    extern          BOOL                bCommandLine;
    extern          BOOL                g_bNavapwOptions;
    extern          UINT                g_uiOptionNumber;

                                        // -----------------------------------
                                        // Set status line to default.
                                        // -----------------------------------
    NavMainWndStatusUpdate (hWnd, IDS_STATUS_DEFAULT);

                                        // -----------------------------------
                                        // Handle the WM_COMMAND message
                                        // based on the control that was
                                        // interacted with.
                                        // -----------------------------------
    switch ( GET_WM_COMMAND_ID(wParam, lParam) )
        {
        case IDC_STATUS_AP_BUTTON:
            if ( FALSE == lpNav->rApStatus.bLoaded )
            {
                LoadAutoProtect();
            }
            else
            {
                TalkToVxD( lpNav->rApStatus.bEnabled ? NAVAPSVC_DISABLE_DRIVER : NAVAPSVC_ENABLE_DRIVER );
            }

            if ( UpdateApStatus( &lpNav->rApStatus ) )
            {
                DisplayApStatus( hWnd, lpNav );
            }

            break;

        case IDC_STATUS_2_BUTTON:
            {
            auto INFOSTRUCT strInfo;

            if ( HasNewDefsArrived() || !UpdateCheckDefinitionAreNewest() ||
                  UpdateCheckDefinitionAreOld() )
            {
                strInfo.dwInfoType = INFOVIRUSDEFS;
            }
            else if ( lpNav->rQuarantineStatus.uTotalItems )
            {
                strInfo.dwInfoType = INFOQUARANTINE;
            }
            else
            {
                strInfo.dwInfoType = INFOVIRUSDEFS;
            }

            strInfo.lpNav = lpNav;

            InfoDlg(hWnd, &strInfo);

            UpdateDefinitionsStatus( &lpNav->rDefinitionsStatus );

            DisplaySecondaryStatus( hWnd, lpNav );

            break;
            }

        case IDC_STATUS_QUAR_ENUM:

            UpdateQuarantineStatus( &lpNav->rQuarantineStatus );
            ProcessNewDefinitions ( hWnd );

        case IDC_STATUS_QUAR_UPDATE:

            DisplaySecondaryStatus( hWnd, lpNav );
            PostMessage( hWnd, WM_PAINT, 0, 0 );

            break;

		case IDC_SYSTEM_WORKS:			// For now, same as IDM_HELP_ABOUT
			DoSystemWorksAbout(hWnd);
			break; 
                                        // -----------------------------------
                                        // IDOK & IDM_SCAN_SELECTED:  The
                                        // user wants to scan selected
                                        // drives.  Scan the specified
                                        // drives.
                                        // -----------------------------------
        case IDM_SCAN_SELECTED:
        case IDOK:
            ScanSelected (hWnd, lpNav);
            break;
                                        // -----------------------------------
                                        // IDM_SCAN_EXIT:
                                        // ID_SCANMAIN_EXIT:
                                        // Terminate the aplication.
                                        // -----------------------------------
        case IDM_SCAN_EXIT:
            // If Netscape mode and files left, don't exit.
            // Instead, launch the next scan...
            if (TRUE == NavMain.bDownloadMode)
                {
                                        // If we scanned another downloaded
                                        // file, don't exit this time around.
                                        // Let the next IDM_SCAN_EXIT come
                                        // through.
                if (NOERR == ScanDownload(hWnd, lpNav))
                    {
                    break;
                    }
                }

        case ID_SCANMAIN_EXIT:
            DestroyWindow (hWnd);
            break;
                                        // -----------------------------------
                                        // IDM_SCAN_DIRECTORY:  Present the
                                        // user with a directory browse box
                                        // and then scan that directory
                                        // selected by the user.
                                        // -----------------------------------
        case IDM_SCAN_DIRECTORY:
            CallScanDirectory (hWnd, lpNav);
            break;

        case IDM_SCAN_PATH:
            CallScanPath( hWnd, lpNav);
            break;



                                        // -----------------------------------
                                        // IDM_SCAN_FILES:  Present the
                                        // user with a files browse box and
                                        // then scan that file selected by
                                        // the user.
                                        // -----------------------------------
        case IDM_SCAN_FILE:
            if ( NavMainWndScanFile ( hWnd, lpNav, szWork, sizeof(szWork) ) )
                {
                                        // -----------------------------------
                                        // Make sure that we can see this file.
                                        // If the file name contains non-Ansi
                                        // translatable characters they will
                                        // appear here as garbage. We will get
                                        // a file name but it won't be usable.
                                        // -----------------------------------
                UINT uBogusAttr;

                if (NOERR != FileGetAttr (szWork, &uBogusAttr))
                {
                    TCHAR szMessage[SYM_MAX_PATH],
                          szBuffer [SYM_MAX_PATH];

                    LoadString ( ghInst, IDS_NAVW, szAppName,
                                 sizeof(szAppName) );
                    LoadString ( ghInst, IDS_GARBAGEFILENAME, szBuffer,
                               sizeof(szBuffer) );
                    SPRINTF ( szMessage, szBuffer, (LPSTR)szWork );
                    MessageBox( hWnd, szMessage, szAppName,
                                MB_OK | MB_ICONSTOP );

                    break;
                }

                if ( (lpNav->hScanList = MemAlloc(GHND, SYM_MAX_PATH+1)) != 0 )
                    {
                    MEMSET (&lpNav->ScanStruct, 0, sizeof(lpNav->ScanStruct));

                    lpScanList = (LPTSTR)MemLock(lpNav->hScanList);

                                        // -----------------------------------
                                        // If PIF or LNK get real file name.
                                        // -----------------------------------

                    IfSpecialFileGetTrueFile ( szWork );

                    STRCPY (lpScanList, szWork);
                    MemUnlock (lpNav->hScanList);

                    lpNav->ScanStruct.bScanSubs = TRUE;
                    StartScanning (hWnd, lpNav);
                    }
                }
            break;
                                        // ----------------------------------
                                        // ID_SCANMAIN_DRIVE_LIST:  The state
                                        // of the drive list has changed.
                                        // ----------------------------------
        case ID_SCANMAIN_DRIVE_LIST:
            if ( GET_WM_COMMAND_CMD(wParam,lParam) == LBN_SELCHANGE)
                {
                                        // ----------------------------------
                                        // The user has changed his selection
                                        // in the drive list.  Make sure our
                                        // check boxes correspond to what
                                        // is going on.
                                        // ----------------------------------
                NDL_TypeBoxCheck (hWnd,
                                    ID_SCANMAIN_DRIVE_LIST,
                                    ID_SCANMAIN_DRIVES_FLOPPY,
                                    NDRV_ALL_FLOPPIES);
                NDL_TypeBoxCheck (hWnd,
                                    ID_SCANMAIN_DRIVE_LIST,
                                    ID_SCANMAIN_DRIVES_HARD,
                                    NAV_NDRV_FIXED);
                NDL_TypeBoxCheck (hWnd,
                                    ID_SCANMAIN_DRIVE_LIST,
                                    ID_SCANMAIN_DRIVES_NETWORK,
                                    NDRV_NETWORK);

                NDL_DisplaySelectedDrives (hWnd, ID_SCANMAIN_DRIVE_LIST,
                                       ID_SCANMAIN_DRIVES_SELECTED);
                }
            break;
                                        // ----------------------------------
                                        // ID_SCANMAIN_DRIVES_FLOPPY:  User
                                        // wants all floppy drives selected.
                                        // ----------------------------------
        case ID_SCANMAIN_DRIVES_FLOPPY:
            NDL_SelectByMask (GetDlgItem (hWnd, ID_SCANMAIN_DRIVE_LIST),
                                NDRV_ALL_FLOPPIES,
                                Button_GetCheck ((HWND)LOWORD (lParam)));

            NDL_DisplaySelectedDrives (hWnd, ID_SCANMAIN_DRIVE_LIST,
                                   ID_SCANMAIN_DRIVES_SELECTED);
            break;
                                        // ----------------------------------
                                        // ID_SCANMAIN_DRIVES_HARD:  User
                                        // wants all local drives selected.
                                        // ----------------------------------
        case ID_SCANMAIN_DRIVES_HARD:
            NDL_SelectByMask ((HWND)GetDlgItem (hWnd, ID_SCANMAIN_DRIVE_LIST),
                                NAV_NDRV_FIXED,
                                Button_GetCheck ((HWND)LOWORD (lParam)));
                                        // ----------------------------------
                                        // Update our selected drive list
                                        // field.
                                        // ----------------------------------
            NDL_DisplaySelectedDrives (hWnd, ID_SCANMAIN_DRIVE_LIST,
                                        ID_SCANMAIN_DRIVES_SELECTED);
            break;
                                        // ----------------------------------
                                        // ID_SCANMAIN_DRIVES_NETWORK:  User
                                        // wants all network drives selected.
                                        // ----------------------------------
        case ID_SCANMAIN_DRIVES_NETWORK:
            NDL_SelectByMask ((HWND)GetDlgItem (hWnd, ID_SCANMAIN_DRIVE_LIST),
                            NDRV_NETWORK,
                            Button_GetCheck ((HWND)LOWORD (lParam)));
                                        // ----------------------------------
                                        // Update our selected drive list
                                        // field.
                                        // ----------------------------------
            NDL_DisplaySelectedDrives (hWnd, ID_SCANMAIN_DRIVE_LIST,
                                    ID_SCANMAIN_DRIVES_SELECTED);
            break;
                                        // ----------------------------------
                                        // IDM_TOOLS_SCHEDULER:  User wishes
                                        // to run the scheduler.
                                        // ----------------------------------
        case IDM_TOOLS_SCHEDULER:
            LaunchScheduler();
            break;

        case IDM_TOOLS_QUARANTINE:
            LaunchQuarantineConsole();
            break;

                                        // ----------------------------------
                                        // IDM_TOOLS_VIRUSLIST:  User wants
                                        // to look at the virus list.
                                        // ----------------------------------
        case IDM_TOOLS_VIRUSLIST:

                                        // Pause the auto-def update while
                                        // we change the virus list.
            if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
            {
                NavRpcSendCommand(
                   NAVAPSVC_PROTOCOL,
                   NAVAPSVC_ADDRESS,
                   NAVAPSVC_ENDPOINT,
                   NAVAPSVC_PAUSE_WATCHING_DEFS);
            }

            if (CallVirusList(hWnd))
                lpNav->bUpdateTsr = TRUE;

                                        // Tell the auto-def update guy that
                                        // we are now done.
            if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
            {
                NavRpcSendCommand(
                   NAVAPSVC_PROTOCOL,
                   NAVAPSVC_ADDRESS,
                   NAVAPSVC_ENDPOINT,
                   NAVAPSVC_CONTINUE_WATCHING_DEFS);
            }
            break;
                                        // ----------------------------------
                                        // IDM_TOOLS_ONRAMP:  User wants
                                        // to download the latest virus defs
                                        // ----------------------------------
        case IDM_TOOLS_ONRAMP:
            // Launch LiveUpdate. Note that this function launches NAVLU32.EXE
            // and returns. This means that this function returns before the
            // user actually internacts with LiveUpdate.
            NavLuLaunchLiveUpdate ();

                                        // JBRENNA 6/4/97:This COMMENT should be
                                        // removed if it is still here after
                                        // project ROSWELL ships.
#ifdef COMMENT_MAYBE_LU_BUG_FIXED
            extern HWND ghwndLastFocused;  // reset focus (fixes onramp bug)
            if ( IsWindow ( ghwndLastFocused ) )
                SetFocus(ghwndLastFocused);
            else
                SetFocus ( GetDlgItem ( hWnd, IDOK ) );
#endif

            break;

                                        // ----------------------------------
                                        // IDM_TOOLS_ACTIVITYLOG:  User
                                        // wishes to look at the activity log.
                                        // ----------------------------------
        case IDM_TOOLS_ACTIVITYLOG:
            MEMSET (&PwCheck, 0, sizeof(PwCheck));
            PwCheck.hWnd         = hWnd;
            PwCheck.lpNavOptions = &navOptions;
            PwCheck.uID          = PASSWORD_VIEWACTIVITY;

            uStatus = AskPassword (&PwCheck);

            if (uStatus == PASSWORD_ASK)
                {
                if (PasswordCheck (&PwCheck, FALSE, FALSE))
                    uStatus = TRUE;
                else
                    uStatus = FALSE;
                }
            else if (uStatus == PASSWORD_SHOWCONTROL)
                uStatus = TRUE;
            else
                uStatus = FALSE;

            if (uStatus == TRUE)
                {
                LoadString (ghInst, IDS_ACTILOG_TITLE, szWork, sizeof(szWork));

                lpAct = (LPACTILOG)MemAllocPtr (GHND, sizeof(ACTILOG));

                if (lpAct)
                    {
                    BuildLogFilename(szFile, navOptions.activity.szLogFile);

                    lpAct->szTitle       = szWork;
                    lpAct->szLogFile     = szFile;
                    lpAct->bAllowClear   = TRUE;
                    lpAct->uHelpID       = IDH_NAVW_ACTIVITY_LOG_DLG_HELP_BTN;
                //&? Needs ID!!! -DALLEE
                    lpAct->uHelpIDFilter = 0;

                                        // ----------------------------------
                                        // Required to specify filtering options.
                                        // ----------------------------------

                    lpAct->lpLogFilter   = (LPALOGFILTER)MemAllocPtr (GHND, sizeof(ALOGFILTER));

                    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
                        {
                        lpAct->nTypes        = NUMFILTERS_A_NT;
                        lpAct->pFormats      = (LPALOGFORMAT)&ActLogTemplatesNT;
                        lpAct->aFilterDesc   = ActLogFiltersNT;
                        }
                    else
                        {
                        if( !NAV_IS_SCANONLY() )
                            {
                            lpAct->nTypes        = NUMFILTERS_A_95;
                            lpAct->pFormats      = (LPALOGFORMAT)&ActLogTemplates95;
                            lpAct->aFilterDesc   = ActLogFilters95;
                            }
                        else
                            {
                            lpAct->nTypes        = NUMFILTERS_A_95_OEM;
                            lpAct->pFormats      = (LPALOGFORMAT)&ActLogTemplates95_OEM;
                            lpAct->aFilterDesc   = ActLogFilters95_OEM;
                            }
                        }

                    // If Quarantine is not ennabled then remove the Quarantine item.
                    if( !NAV_IS_QUARANTINE() )
                        {
                        lpAct->nTypes--;
                        }

                    lpAct->lpNav         = &navOptions;

                    ShowActivityLog(hWnd, lpAct);
                    }
                }
            break;

                                        // ----------------------------------
                                        // IDHELP:
                                        // Help on the main dlg
                                        // ----------------------------------
        case IDHELP:
            StdHelpFromDialog ( hWnd, IDH_NAVW_MAIN_WINDOW_HELP_BTN);
            break;

                                        // ----------------------------------
                                        // IDM_TOOLS_OPTIONS:  User wishes
                                        // to configure NAV.  Bring up the
                                        // control panel.
                                        // ----------------------------------

        case IDM_TOOLS_OPTIONS:
            {
            WORD    wOptions;

            NDL_GetSelected(GetDlgItem (hWnd, ID_SCANMAIN_DRIVE_LIST), szWork);

            if ( g_bNavapwOptions )
                {
                wOptions = NavOptsControlPanel ( hWnd,
                                                 ghInst,
                                                 g_uiOptionNumber );
                }
            else
                {
                wOptions = NavOptsControlPanel (hWnd, ghInst);
                }

            if ( OPTIONS_UPDATETSR & wOptions )
                {
                NavMain.bUpdateTsr = TRUE;

                SetupAPAgentStartGroupItem();
                SetFocus(hWnd);
                }

            NotifyNavapw ( wOptions );
                                        // ----------------------------------
                                        // If some error occured notify user.
                                        // ----------------------------------

            NavOptsErrorNotify ( hWnd, &navOptions.alert, wOptions );

            NavMainOptionsLoad (hWnd, lpNav, szWork);

            if ( g_bNavapwOptions )
                {
                bCommandLine = FALSE;

                FORWARD_WM_COMMAND (
                                hWnd,
                                IDM_SCAN_EXIT,
                                0, 0,
                                PostMessage
                                );
                }
            }
            break;

        case IDM_HELP_INFODESK:
            DisplayInfoDeskHelp ( hWnd );
            break;

	    case IDM_HELP_PSO_CONNECT:
            WinHelp( hWnd, String_Ptr(IDS_HELPFILE_INTERNET), HELP_CONTENTS, 0);
            break;

	    case IDM_HELP_PSO_ONLINESUPPORT:
            WinHelp( hWnd, String_Ptr(IDS_HELPFILE_ONLINE), HELP_CONTENTS, 0);
			break;

	    case IDM_HELP_PSO_AVSUPPORT:
            WinHelp( hWnd, String_Ptr( IDS_SUPPORT_HELP_FILE ), HELP_CONTENTS, 0);
            break;

        case IDM_HELP_PSO_SARC:
            ShellExecute( hWnd,
                          _T("open"),
                          String_Ptr(IDS_URL_SARC),
                          NULL,
                          NULL,
                          SW_SHOW);
            break;


        case IDM_HELP_ABOUT:
            DoStdAbout(hWnd);
            break;

                                        // ----------------------------------
                                        // default:  Could be a button bar
                                        // command or something we just would
                                        // rather not bother with.
                                        // ----------------------------------
        default:
            lResult = DefDlgProc( hWnd, WM_COMMAND, wParam, lParam );
            break;
        }

    return (lResult);
} // NavMainWndWM_COMMAND()


/* **************************************************************************

@Name:
VOID LOCAL BuildScanList (              // Builds items to scan from raw data
    LPTSTR   lpScanList,                // [out] formatted output placed here
    LPTSTR   lpRawData                  // [in] unformatted data
    );

@Description:
This function formats raw data obtained from the drive listbox. Format that
this information has originally is: "ACEFQ". And this is converted to
"A:\ C:\ E:\ F:\ Q:\" Where each item is separated by a NULL character.
Note that the assumption is made here that lpScanList has been initialized
to all nulls and that it is large enough to hold the list.

@Parameters:
$lpScanList$    [out] formatted output placed here
$lpRawData$     [in] unformatted data

@Returns:       Nothing

************************************************************************** */

VOID LOCAL BuildScanList (              // Builds items to scan from raw data
    LPTSTR   lpScanList,                // [out] formatted output placed here
    LPTSTR   lpRawData                  // [in] unformatted data
    )
{
    while (*lpRawData != EOS)
        {
        *lpScanList = *lpRawData;
        STRCAT (lpScanList, _T(":\\"));
        lpScanList += (STRLEN (lpScanList) + 1);
        lpRawData = AnsiNext (lpRawData);
        }
}


/* **************************************************************************

@Name:
VOID LOCAL FillWithDrives (             // Fill buffer with drives to scan
    LPTSTR  lpScanList,                 // [out] buffer to fill
    BOOL    bAllDrives                  // [in] TRUE if use net drives also
    );

@Description:
This function fills the provided buffer with drive letters to scan. If
bAlllDrives is set to true then network drives are added to this list also.

@Parameters:
$lpScanList$    [out] buffer to fill
$bAllDrives$    [in] TRUE if use net drives also

@Returns:       Nothing

************************************************************************** */

VOID LOCAL FillWithDrives (             // Fill buffer with drives to scan
    LPTSTR  lpScanList,                 // [out] buffer to fill
    BOOL    bAllDrives                  // [in] TRUE if use net drives also
    )
{
    auto    TCHAR   tszDrive[10];
    auto    UINT    uStatus;
    auto    UINT    uMajor, uMinor, uFloppyType;

    *lpScanList = EOS;

    STRCPY (tszDrive, _T("x:\\"));

    for ( tszDrive[0] = 'A'; tszDrive[0] <= 'Z'; tszDrive[0]++ )
    	{
    	if ( !DiskExists(tszDrive[0]) )
    		continue;

        // Skip floppy drive types
		uStatus = DiskGetFloppyType(tszDrive[0], &uFloppyType);
    	if( uStatus == NOERR && IsTypeFloppy(uFloppyType) )
    		continue;

        if ( DiskExists(tszDrive[0]) )
            {
            if (bAllDrives == TRUE)
                {
                STRCAT (lpScanList, tszDrive);
                lpScanList += STRLEN (lpScanList) + 1;
                *lpScanList = 0;
                }
            else
                {
                uStatus = DiskGetType(tszDrive[0], &uMajor, &uMinor);

                                        // For a local disk we want all local
                                        // drives except for floppies.  Note
                                        // that TYPE_LOGICAL is for compressed
                                        // drives and fixes a defect.
                if (uStatus == NOERR &&
                    (uMajor == TYPE_FIXED ||
                     uMajor == TYPE_RAMDISK ||
                     uMajor == TYPE_ENCRYPTED ||
                     uMajor == TYPE_LOGICAL))
                    {
                    STRCAT (lpScanList, tszDrive);
                    lpScanList += STRLEN (lpScanList) + 1;
                    *lpScanList = 0;
                    }
                }
            }
    	}
}


/* **************************************************************************

@Name:
VOID LOCAL ScanSelected (               // Starts drive scanning
    HWND        hWnd,                   // [in] moma window
    LPNAVMAIN   lpNavMain               // [in] our structure pointer
    );

@Description:
This function start scanning the provided drives or performs the specified
command-line action.

@Parameters:
$hWnd$          [in] moma window
$lpNavMain$     [in] our structure pointer

@Returns:       Nothing

************************************************************************** */

VOID LOCAL ScanSelected (               // Starts drive scanning
    HWND        hWnd,                   // [in] moma window
    LPNAVMAIN   lpNavMain               // [in] our structure pointer
    )
{
    #define         WINVER30            0x0003

    extern          BOOL                bCommandLine;
    extern          BOOL                g_bNavapwOptions;

    auto            LPTSTR              lpScanList;
    auto            TCHAR               szWork [ SYM_MAX_PATH+1 ];

    MEMSET (&lpNavMain->ScanStruct, 0, sizeof(lpNavMain->ScanStruct));

    if ((lpNavMain->hScanList = MemAlloc(GHND, SIZE_SCANLIST)) != 0)
        {
        lpScanList = (LPTSTR)MemLock (lpNavMain->hScanList);

                                        // -----------------------------------
                                        // If the user provided arguments on
                                        // the command line process them
                                        // accordingly.
                                        // -----------------------------------
        if ( bCommandLine == TRUE )
            {
                                        // -----------------------------------
                                        // The buffer (pointed to by lpScanList)
                                        // might be resized somewhere inside
                                        // ProcessCommandLine(), so pass in
                                        // all pointers pointing to the
                                        // block. If the buffer is resized,
                                        // the address will probably change.
                                        // -----------------------------------
            ProcessCommandLine (&lpScanList, &lpNavMain->ScanStruct, lpNavMain);
            }
        else
            {
                                        // ----------------------------------
                                        // Get list of drives.  If no drives
                                        // selected, then use the item with
                                        // focus.
                                        // ----------------------------------
            if ( NDL_GetSelected (GetDlgItem (hWnd, ID_SCANMAIN_DRIVE_LIST),
                                              szWork) == 0 )
                {
                if ( LOWORD(GetVersion()) == WINVER30 )
                    GetInfoAla30(hWnd, szWork);
                else if ( !GetInfoAla31(hWnd, szWork) )
                    GetInfoAla30(hWnd, szWork);
                }

            BuildScanList (lpScanList, szWork);
            lpNavMain->ScanStruct.bScanSubs = TRUE;
            }

                                        // ----------------------------------
                                        // Only call StartScanning() if there
                                        // are items to be scanned.
                                        // ----------------------------------
        if ( *lpScanList || lpNavMain->ScanStruct.bMemOnly )
            {
                                        // ----------------------------------
                                        // Clear flag without caring if it was
                                        // set prior to this point. All command
                                        // line processing should be performed
                                        // by now.
                                        // ----------------------------------
            bCommandLine = FALSE;
                                        // ----------------------------------
                                        // hScanList is freed up in ScanStart
                                        // once the function is done with the
                                        // memory block
                                        // ----------------------------------

            MemUnlock (lpNavMain->hScanList);
            StartScanning (hWnd, lpNavMain);
            }
                                        // ----------------------------------
                                        // No scan to do
                                        // ----------------------------------
        else
            {
                                        // ----------------------------------
                                        // Free allocated scan memory
                                        // ----------------------------------
            MemUnlock ( lpNavMain->hScanList );
            MemFree   ( lpNavMain->hScanList );
            lpNavMain->hScanList = 0;

                                        // ----------------------------------
                                        // Is this an options run?
                                        // ----------------------------------
            if ( g_bNavapwOptions )
                {
                FORWARD_WM_COMMAND ( hWnd,
                                     IDM_TOOLS_OPTIONS,
                                     GetDlgItem(hWnd, IDM_TOOLS_OPTIONS),
                                     BN_CLICKED,
                                     PostMessage );
                }

            else if ( bCommandLine == TRUE )
                {
                                        // ----------------------------------
                                        // An invalid scan object will get
                                        // us here. Close down.
                                        // ----------------------------------
                bCommandLine = FALSE;

                FORWARD_WM_COMMAND (
                                hWnd,
                                IDM_SCAN_EXIT,
                                0, 0,
                                PostMessage
                                );
                }
            }
        }
}


/* **************************************************************************

@Name:
BOOL LOCAL GetInfoAla31 (               // Get item to scan Win 3.1 format
    HWND    hWnd,                       // [in] main window handle
    LPTSTR  lpWork                      // [out] place items in here
    );

@Description:
This function obtains the drives to scan depending on where the carett is.

@Parameters:
$hWnd$       [in] main window handle
$lpWork$     [out] place items in here

@Returns:    TRUE if we got the required info, FALSE otherwise.

************************************************************************** */

BOOL LOCAL GetInfoAla31 (               // Get item to scan Win 3.1 format
    HWND    hWnd,                       // [in] main window handle
    LPTSTR  lpWork                      // [out] place items in here
    )
{
    auto    BOOL        bStatus = FALSE;
    auto    UINT        uIndex;

    uIndex = ListBox_GetCaretIndex ( GetDlgItem (
                                                hWnd,
                                                ID_SCANMAIN_DRIVE_LIST
                                                )
                                   );
    if ( uIndex )
        {
        ListBox_GetText (
                        GetDlgItem ( hWnd, ID_SCANMAIN_DRIVE_LIST),
                        uIndex,
                        lpWork
                        );
        bStatus = TRUE;
        }

    return (bStatus);
}


/* **************************************************************************

@Name:
BOOL LOCAL GetInfoAla30 (               // Obtain scan info Win 3.0 way
    HWND    hWnd,                       // [in] main window handle
    LPTSTR  lpWork                      // [out] place stuff here
    );

@Description:
This function obtains all fixed drives to scan.

@Parameters:
$hWnd$      [in]    main window handle
$lpWork$    [out]   nobody is gonna read this.

@Returns:   TRUE if we got the required info, FALSE otherwise.

************************************************************************** */

BOOL LOCAL GetInfoAla30 (               // Obtain scan info Win 3.0 way
    HWND    hWnd,                       // [in] main window handle
    LPTSTR  lpWork                      // [out] place stuff here
    )
{
    auto    BOOL        bStatus = FALSE;
    auto    TCHAR       tszDrive[2];

    MEMSET (tszDrive, 0, sizeof (tszDrive));

    for (tszDrive[0] = 'A'; tszDrive[0] < 'Z'; tszDrive[0]++)
        {
        if (DiskExists (tszDrive[0]) &&
            DiskIsFixed (tszDrive[0]) &&
           !DiskIsNetwork (tszDrive[0]))
            {
            STRCPY (lpWork, tszDrive);
            bStatus   = TRUE;
            break;
            }
        }

    if (!bStatus)                       // If we didn't find anything???
        {
        for (tszDrive[0] = 'A'; tszDrive[0] < 'Z'; tszDrive[0]++)
            if (DiskExists (tszDrive[0]))
            {
            STRCPY (lpWork, tszDrive);
            bStatus   = TRUE;
            break;
            }
        }

    return (bStatus);
}


/* **************************************************************************

@Name:
VOID LOCAL ProcessCommandLine (         // Process command line arguments
    LPTSTR          *lplpScanList,      // [out] items to scan; we pass a pointer to a pointer, in case we resize the buffer and the address changes
    LPSCANSTRUCT    lpScan,             // [out] how scan should be performed
    LPNAVMAIN       lpNavMain           // [in] our structure pointer, in case we have to resize the buffer
    );

@Description:
This function parses the command line options and obtains all the items
that were specified on the command line and also the how to scan switches.

@Parameters:
$lpScanList$    [out] items to scan
$lpScan$        [out] how scan should be performed

@Returns:       Nothing

************************************************************************** */

VOID LOCAL ProcessCommandLine (         // Process command line arguments
    LPTSTR          *lplpScanList,      // [out] items to scan; see above
    LPSCANSTRUCT    lpScan,             // [out] how scan should be performed
    LPNAVMAIN       lpNavMain           // [in] our structure pointer; see above
    )
{
    extern  BOOL    g_bNavapwOptions;
    extern  BOOL    g_bSageRun;
    extern  UINT    g_uiOptionNumber;
    extern  HWND    ghWndMain;

    auto    CFGTEXTREC  CfgRecord = {0};

                                        //------------------------------------
                                        // Uppercase all non-DBCS characters
                                        // that are parameters from the
                                        // command line.
                                        //------------------------------------
    ConfigParamToUpperNonDbcs();

                                        // ----------------------------------
                                        // Check to see if we are called to
                                        // do options - i.e. from Navapw.
                                        // ----------------------------------
    if ( IsConfigSwitchSet(CFG_OPTIONS) )
        {
        CfgRecord.lpSwitch = String_Ptr(IDS_O);
        CfgRecord.wFlags   = CFG_SW_EXACT;

        g_bNavapwOptions = ConfigSwitchGetValue ( &CfgRecord,
                                                  &g_uiOptionNumber );
        return;
        }

                                        // ----------------------------------
                                        // Check to see if we are called to
                                        // do options from SAGE
                                        // ----------------------------------
    if ( IsConfigSwitchSet(CFG_SAGESET) )
        {
        g_bNavapwOptions = TRUE;        // Force the option dialog
        g_uiOptionNumber = 0;           // Default to panel 0

        return;
        }
                                        // ----------------------------------
                                        // Check to see if we were launched
                                        // by NAVUI (NAV32.EXE) front end.
                                        // ----------------------------------
    //&? subfunc or move entirely?
    if ( IsConfigSwitchSet( CFG_UILAUNCH ) )
        {
        auto    CNavRegComm     *pRegComm;

        pRegComm = new CNavRegComm;

        if ( NULL != pRegComm )
            {
            if ( FALSE != pRegComm->Load() )
                {
                switch ( pRegComm->GetAction() )
                    {
                    case CNavRegComm::ACTION_SCAN:
                        GetRegistryNames( pRegComm,
                                          lplpScanList,
                                          &lpNavMain->hScanList,
                                          lpScan );

                        lpScan->uProgramMode = PROGRAM_NAVUILAUNCH;
                        break;

                    case CNavRegComm::ACTION_VIRUS_LIST:
                        break;

                    case CNavRegComm::ACTION_ACTIVITY_LOG:
                        break;

                    // case CNavRegComm::ACTION_NO_ACTION:
                    // case CNavRegComm::ACTION_LAST_ACTION:
                    default:
                        break;
                    }
                }
            else
                {
                // Load failed...
                }

            delete pRegComm;
            }
        else
            {
            // Error notification? unlikely...
            }

        return;
        }


    // default to scan subdirs, unless instructed otherwise - GAD 7/7/95
    lpScan -> bScanSubs = TRUE;

                                        // ----------------------------------
                                        // Parse the exclusive scan switches
                                        // from MOST SCANNED to LEAST SCANNED
                                        // /A -> /L -> /BOOT -> /MEM
                                        // ----------------------------------

    if ( IsConfigSwitchSet(CFG_ALLDRIVES) )
        {
                                        // ----------------------------------
                                        // /A: Scan all drives except drives
                                        //  A and B Supposed to override
                                        // others (even /L)
                                        // ----------------------------------

        FillWithDrives ((*lplpScanList), TRUE);
        }
    else if ( IsConfigSwitchSet(CFG_LOCALDRIVES) )
        {
                                        // ----------------------------------
                                        // /L: Scan all local drives except A
                                        // and B Supposed to override others
                                        // (not /A)
                                        // ----------------------------------

        FillWithDrives ((*lplpScanList), FALSE);
        }
    else if ( IsConfigSwitchSet(CFG_BOOTONLY) )
        {
                                        // ----------------------------------
                                        // /BOOT:
                                        // Scan only boot sectors of named
                                        // drives
                                        // ----------------------------------
        auto    TCHAR   szParamCurrent [ SYM_MAX_PATH + 2 ];

        MEMSET (szParamCurrent, EOS, sizeof(szParamCurrent));

        lpScan -> bBootOnly = TRUE ;
                                         //check if the user did not pass any
                                         //drive letters
        ConfigParamGet (0, szParamCurrent, SYM_MAX_PATH);
        if (!STRLEN(szParamCurrent))
            FillWithDrives ((*lplpScanList), FALSE);
        }
    else if ( IsConfigSwitchSet(CFG_MEMONLY) )
        {
                                        // ----------------------------------
                                        // /MEM:
                                        // Scan only memory
                                        // ----------------------------------

        lpScan->bMemOnly = TRUE ;
        }
    else if ( IsConfigSwitchSet(CFG_SAGERUN) )
        {
                                        // ----------------------------------
                                        // /SAGERUN:n Scan all local drives
                                        // except A and B. Same as /L
                                        // ----------------------------------
        g_bSageRun = TRUE;

        FillWithDrives ((*lplpScanList), FALSE);
        }
    if ( IsConfigSwitchSet(CFG_NOSUBS) )
        {
                                        // ----------------------------------
                                        // /S: Scan subdirectories
                                        // ----------------------------------

        lpScan -> bScanSubs = FALSE;
        }


                                        // ----------------------------------
                                        // /HIMEM: Only scan high memory if
                                        //         this switch is set.
                                        // ----------------------------------
    if (IsConfigSwitchSet(CFG_HIMEM))
        {
        lpScan->uScanMemory = TRUE ;
        navOptions.general.bScanHighMemory = TRUE;
        }
    else
        {
        navOptions.general.bScanHighMemory = FALSE;
        }

                                        // ----------------------------------
                                        // /VISIBLE: Show the scanning
                                        //           animation even when
                                        //           a command-line has been
                                        //           specified. Used during
                                        //           install automation.
                                        //     NOTE: After scan, returns to
                                        //           NAVW main window (does
                                        //           not exit).
                                        // ----------------------------------

    // NOTE: /VISIBLE is parsed in navw.cpp:NavMainWndWM_CREATE()
    //       because WM_CREATE is called before parsing is done.

                                        // ----------------------------------
                                        // /NORESULTS: Quit after scan instead
                                        //             of showing results
                                        //             (even if virus found).
                                        // ----------------------------------

    if (IsConfigSwitchSet(CFG_NORESULTS))
        lpScan->uProgramMode = PROGRAM_AUTOMATED;

                                        // ---------------------------------
                                        // Check if memory should be scanned
                                        // ---------------------------------
    switch (SwitchState(MEMORY_SWITCH))
        {
        case CFG_SWITCH_SET:
        case CFG_SWITCH_PLUS:
            lpScan->uScanMemory = TRUE ;
            break;

        case CFG_SWITCH_MINUS:
            lpScan->uScanMemory = FALSE ;
            break;

        default:
            lpScan->uScanMemory = SWITCH_DONTCHANGE;
            break;
        }
                                        // -----------------------------------
                                        // Check if the boot records should be
                                        // scanned
                                        // -----------------------------------
    switch (SwitchState (BOOT_SWITCH))
        {
        case CFG_SWITCH_SET:
        case CFG_SWITCH_PLUS:
            lpScan->uScanBoot = TRUE ;
	        break;

        case CFG_SWITCH_MINUS:
            lpScan->uScanBoot = FALSE ;
	        break;

        default:
            lpScan->uScanBoot = SWITCH_DONTCHANGE;
            break;
        }

                                        // Check for web-browser helper-app
                                        // mode.
                                        // In NAVWORK, we'll kill all options
                                        // except for single file scan for
                                        // known viruses.
    if (IsConfigSwitchSet(CFG_DOWNLOAD))
        {
        lpScan->uProgramMode = PROGRAM_DOWNLOAD;
        }

#ifdef NAVSCANW
                                        // ----------------------------------
                                        // /CPULOAD:###
                                        // Specifies target CPU load for
                                        // scheduled scans. If the switch is
                                        // not present, lpScan->uCpuLoad will
                                        // be set to 100 indicating to NAVWORK
                                        // not to perform load management.
                                        // ----------------------------------
    if (IsConfigSwitchSet(CFG_CPULOAD))
        {
        CfgRecord.lpSwitch = String_Ptr(IDS_CPULOAD);
        CfgRecord.wFlags   = CFG_SW_EXACT;

        ConfigSwitchGetValue( &CfgRecord, &lpScan->uCpuLoad );
        }
    else
        {
        lpScan->uCpuLoad = 100;
        }
#endif

                                        // ----------------------------------
                                        // Check for command-line /HMEM=,
                                        // which means that another
                                        // application filled a
                                        // memory-mapped file with file specs,
                                        // and passed us the handle.
                                        // ----------------------------------
    if (IsConfigSwitchSet(CFG_MEMORYHANDLE))
        {
        auto    LPTSTR  szHMEMArgStart = NULL;
        auto    LPTSTR  szHMEMArg = NULL;


        lpScan->bHMemLaunch = TRUE;
        s_bHMEMSwitchUsed = TRUE;

                                        // Find HMEM in the command line:
        szHMEMArgStart = STRSTR (gszCmdLine,String_Ptr(IDS_MEMHANDLE));
        if (szHMEMArgStart)
            {
            szHMEMArg = &(szHMEMArgStart[(STRLEN(String_Ptr(IDS_MEMHANDLE)))]);
                                        // Here, szHMEMArg points to the start
                                        // of the <x> in HMEM=<x>:
            if (szHMEMArg)
                {
                if (szHMEMArg[0] != '\0')
                    {
                    DWORD SizeOfData = 0;
                     LPTSTR szFileListIn = CatchMemoryFile ( szHMEMArg, &SizeOfData );

                                        // ----------------------------------
                                        // SizeOfData holds the size of the
                                        // data passed in from the memory-mapped
                                        // file. If this is larger than SIZE_SCANLIST,
                                        // we have to resize the Scan Buffer.
                                        //
                                        // For right now, we only have to worry
                                        // about resizing when the /HMEM switch
                                        // is set, because the amount of data
                                        // passed in does not have a limit.
                                        // ----------------------------------

                    if (SizeOfData > SIZE_SCANLIST)
                        {
                                        // ----------------------------------
                                        // To resize, we don't use a
                                        // MemReAlloc() call, because any
                                        // space that it adds to the buffer
                                        // will be free space directly after
                                        // the buffer. There isn't any solid
                                        // guarantee of this space being available,
                                        // so we create a new buffer, copy the
                                        // contents of the old buffer in, and
                                        // destroy the old one.
                                        // ----------------------------------

                        HGLOBAL hNewScanList;
                        if ((hNewScanList = MemAlloc(GHND, SizeOfData)) != 0)
                            {
                                        // The new buffer was successfully
                                        // allocated. Start making permanent
                                        // changes:

                                        // Copy in any old contents of the
                                        // scan buffer:
                            _fmemcpy( hNewScanList, lpNavMain->hScanList, (int)SIZE_SCANLIST );

                                        // Destroy the old buffer:
                            MemUnlock ( lpNavMain->hScanList );
                            MemFree ( lpNavMain->hScanList );

                                        // Redirect all open handles so
                                        // that they point to the
                                        // new buffer:
                            lpNavMain->hScanList = hNewScanList;

                                        // Lock the new buffer.
                            (*lplpScanList) = (LPTSTR)MemLock (lpNavMain->hScanList);

                            } // hNewScanList != 0
                        } // End buffer overflow

                    GatherCommandLineNames(*lplpScanList,
                                           SizeOfData,
                                           &lpScan->bScanSubs,
                                           szFileListIn);

                    CatchMemoryFile_Cleanup ( szFileListIn );
                    }
                }
            }
        }                               // End if (/HMEM was set) ...
    else                                // /HMEM switch was not set. Check
        {                               // for other command-line specs:

        GatherCommandLineNames(*lplpScanList,
                               SIZE_SCANLIST,
                               &lpScan->bScanSubs,
                               NULL);
        } // End if (/HMEM not set) ...
}


/* **************************************************************************

@Name:
UINT LOCAL IsConfigSwitchSet (          // Checks if a switch is set
    UINT    uSwitchType                 // [in] flag indicating switch type
    );

@Description:
This function checks if a specified command line switch is set.

@Parameters:
$uSwitchType$   [in] flag indicating switch type:
                CFG_ALLDRIVES
                CFG_LOCALDRIVES
                CFG_BOOTONLY
                CFG_MEMONLY
                CFG_NOSUBS
                CFG_WINNTOKAY
                CFG_MEMORYHANDLE
                CFG_DOWNLOAD
         	CFG_CPULOAD
                CFG_SAGERUN
                CFG_SAGESET
                CFG_UILAUNCH
                CFG_HIMEM
                CFG_NORESULTS
                CFG_VISIBLE

@Returns:       Whatever the Quake function ConfigSwitchSet() returns.

************************************************************************** */

UINT LOCAL IsConfigSwitchSet (          // Checks if a switch is set
    UINT    uSwitchType                 // [in] flag indicating switch type
    )
{
    auto    UINT        uSwitchState = 0;
    auto    CFGTEXTREC  CfgRecord;

    MEMSET (&CfgRecord, 0, sizeof(CfgRecord));

    switch ( uSwitchType )
        {
        case CFG_ALLDRIVES:
            CfgRecord.lpSwitch = String_Ptr(IDS_A);
            CfgRecord.wFlags   = CFG_SW_MINIMUM;
            break;

        case CFG_LOCALDRIVES:
            CfgRecord.lpSwitch = String_Ptr(IDS_L);
            CfgRecord.wFlags   = CFG_SW_MINIMUM;
            break;

        case CFG_BOOTONLY:
            CfgRecord.lpSwitch = String_Ptr(IDS_BO);
            CfgRecord.wFlags   = CFG_SW_MINIMUM;
            break;

        case CFG_MEMONLY:
            CfgRecord.lpSwitch = String_Ptr(IDS_MEM);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_NOSUBS:
            CfgRecord.lpSwitch = String_Ptr(IDS_NOSUBS);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_OPTIONS:
            CfgRecord.lpSwitch = String_Ptr(IDS_O);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_MEMORYHANDLE:
            CfgRecord.lpSwitch = String_Ptr(IDS_MEMHANDLE);
            CfgRecord.wFlags   = CFG_SW_MINIMUM;
            break;

        case CFG_DOWNLOAD:
            CfgRecord.lpSwitch = String_Ptr(IDS_DOWNLOAD);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_CPULOAD:
            CfgRecord.lpSwitch = String_Ptr(IDS_CPULOAD);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_SAGERUN:
            CfgRecord.lpSwitch = String_Ptr(IDS_SAGERUN);
            CfgRecord.wFlags   = CFG_SW_MINIMUM;
            break;

        case CFG_SAGESET:
            CfgRecord.lpSwitch = String_Ptr(IDS_SAGESET);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_UILAUNCH:
            CfgRecord.lpSwitch = String_Ptr(IDS_UILAUNCH);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_HIMEM:
            CfgRecord.lpSwitch = String_Ptr(IDS_HIMEM);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_VISIBLE:
            CfgRecord.lpSwitch = String_Ptr(IDS_VISIBLE);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case CFG_NORESULTS:
            CfgRecord.lpSwitch = String_Ptr(IDS_NORESULTS);
            CfgRecord.wFlags   = CFG_SW_MINIMUM;
            break;
        }

    if ( CfgRecord.lpSwitch )
        uSwitchState = ConfigSwitchSet (&CfgRecord, NULL);

    return ( uSwitchState );
}


/* **************************************************************************

@Name:
UINT LOCAL SwitchState (                // Check the state of a switch
    UINT    uSwitchType                 // [in] flag indicating switch to check
    );

@Description:
This function checks the state of a switch.

@Parameters:
$uSwitchType$   [in] flag indicating switch type:
                MEMORY_SWITCH
                BOOT_SWITCH

@Returns:       Whatever the Quake function ConfigSwitchSet() returns.

************************************************************************** */

UINT LOCAL SwitchState (                // Check the state of a switch
    UINT    uSwitchType                 // [in] flag indicating switch to check
    )
{
    auto    UINT        uSwitchState = 0;
    auto    CFGTEXTREC  CfgRecord;

    MEMSET (&CfgRecord, 0, sizeof(CfgRecord));

    switch (uSwitchType)
        {
        case MEMORY_SWITCH:
            CfgRecord.lpSwitch = String_Ptr(IDS_M);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;

        case BOOT_SWITCH:
            CfgRecord.lpSwitch = String_Ptr(IDS_B);
            CfgRecord.wFlags   = CFG_SW_EXACT;
            break;
        }

    if ( !ConfigSwitchSet (&CfgRecord, &uSwitchState) )
        uSwitchState = 0;

    return (uSwitchState);
}

//************************************************************************
// GetRegistryNames()
//
// This function reads the list of items that NAVUI(NAV32.EXE) has
// requested to be scanned, and packages in the Scan List SZZ and
// SCANSTRUCT.
// The buffer for the scan list will be reallocated in this routine.
//
// Parameters:
//      CNavRegComm     *pRegComm,      [in] NAVUI communication object
//      LPTSTR          *lplpScanList,  [out] Ptr to scan list pointer
//      HGLOBAL         *lphScanList,   [out] Ptr to scan list memory handle
//      LPSCANSTRUCT    lpScan          [out] Scan options
//
// Returns:
//      Nothing.
//************************************************************************
// 03/28/1997 DALLEE, created.
//************************************************************************

VOID LOCAL GetRegistryNames (           // Read files to scan passed from NAVUI
    CNavRegComm     *pRegComm,          // [in] NAVUI communication object
    LPTSTR          *lplpScanList,      // [out] Ptr to scan list pointer
    HGLOBAL         *lphScanList,       // [out] Ptr to scan list memory handle
    LPSCANSTRUCT    lpScan              // [out] Scan options
    )
{
    auto    LPSCANOBJECT    lpScanObject;
    auto    DWORD           dwBufferSize;
    auto    int             nEntries;

    auto    LPSCANITEM      lpScanItems;
    auto    HGLOBAL         hNewList;
    auto    LPTSTR          lpszzNewList;

                                        // -----------------------------------
                                        // Get required scan list buffer size
                                        // and number of entries.
                                        // -----------------------------------
    dwBufferSize = 0;
    nEntries     = 0;

    for ( lpScanObject = pRegComm->GetFirstScanObject();
          NULL != lpScanObject;
          lpScanObject = pRegComm->GetNextScanObject( lpScanObject ) )
        {
        SYM_ASSERT( SYM_MAX_PATH > STRLEN( lpScanObject->szPathName ) );

                                        // Add length of each filespec.
        dwBufferSize += sizeof(TCHAR) * ( 1 + STRLEN( lpScanObject->szPathName ) );
        nEntries++;
        }

                                        // -----------------------------------
                                        // Allocate and fill new scan list
                                        // and index.
                                        // -----------------------------------
    if ( dwBufferSize )
        {
        dwBufferSize += sizeof(TCHAR);  // Add one for 2nd ending EOS.

        hNewList = MemAlloc( GHND, dwBufferSize );

        if ( NULL != hNewList )
            {
            lpszzNewList = (LPTSTR) MemLock( hNewList );

            lpScanItems = (LPSCANITEM) MemAllocPtr( GHND, (nEntries + 1) * sizeof(SCANITEM) );

            if ( NULL != lpScanItems )
                {
                                        // Free old buffer.
                                        // And fill in return values for new
                                        // buffer.
                MemUnlock( *lphScanList );
                MemRelease( *lphScanList );
                *lphScanList  = hNewList;
                *lplpScanList = lpszzNewList;

                lpScan->lpScanItems = lpScanItems;

                                        // Fill in names of files to scan.
                for ( lpScanObject = pRegComm->GetFirstScanObject();
                      NULL != lpScanObject;
                      lpScanObject = pRegComm->GetNextScanObject( lpScanObject ) )
                    {
                    lpScanItems->lpszPathName = lpszzNewList;
                    lpScanItems->bScanSubDirs = lpScanObject->bScanSubDirs;
                    lpScanItems++;

                    STRCPY( lpszzNewList, lpScanObject->szPathName );
                    lpszzNewList += 1 + STRLEN( lpszzNewList );
                    }
                }
            else
                {
                MemFree( hNewList );
                }
            }
        // else memalloc error - not really expected.
        }

                                        // ----------------------------------
                                        // Read per-scan options
                                        // ----------------------------------
    lpScan->uScanMemory = pRegComm->m_bScanMemory;
    lpScan->uScanBoot   = pRegComm->m_bScanBootRecords;

    // lpScan->bScanSubs = TRUE;   //&? bogus setting

    //&? Need to save/kluge in following values as well...
    //
    // pRegComm->GetScanType()
    // pRegComm->GetScanResponse()
    // pRegComm->m_bScanMBR
    // pRegComm->m_bScanCompressed

} // GetRegistryNames()


/* **************************************************************************

@Name:
VOID WINAPI GatherCommandLineNames (
    LPTSTR  lpszzScanList,
    UINT    uListSize,
    LPBOOL  lpbScanSubs,
    LPTSTR  lpszScanFileNamesIn
    )

@Description:
Fill the passed szz buffer with filespecs from the command
line.  All filenames will be fully qualified but may
contain wildcards in the file portion.
NOTE:  This routine is different from the DOS counterpart
because NAVW already has the buffer.  NAVD's method is
preferred and NAVW should ultimately be modified.
Also missing from this version is the call to BuildScanList()

@Parameters:
$lpszzScanList$ -- szz list for filespecs
$uListSize$     -- size of list
$lpbScanSubs$   --
$lpszScanFileNamesIn$
                -- If not NULL, we are NOT processing the command line;
                   instead, we're parsing a memory-mapped file, which
                   will have a slightly different format.

@Returns:
    lpszzScanList is filled.  lpbScanSubs is possibly changed
************************************************************************** */

VOID WINAPI GatherCommandLineNames (
    LPTSTR  lpszzScanList,
    UINT    uListSize,
    LPBOOL  lpbScanSubs,
    LPTSTR  lpszScanFileNamesIn
    )
{
    auto        LPTSTR      lpszScanOne ;
    auto        UINT        uCounter;
    auto        UINT        uNewLen ;
    auto        TCHAR       szParamCurrent [ SYM_MAX_PATH + 2 ] ;
    auto        TCHAR       szFullCurrent [ SYM_MAX_PATH + 2 ] ;
    auto        TCHAR       szTestName [ SYM_MAX_PATH + 2 ] ;
    auto        BOOL        bFileFound;
    auto        DWORD       dwLastError;
    auto        UINT        uiErrMode;

    auto        LPTSTR       lpszScanFileNamesInOne = lpszScanFileNamesIn;
    auto        LPTSTR       lpszScanFileNamesInOneTerminator;
    auto        LPTSTR       lpszScanFileNamesLastChar;
    auto        DWORD       dwSizeOfParsedString;
    auto        BOOL        bItemIsQuoted;
    auto		TCHAR		szRootTestName[SYM_MAX_PATH + 2];

    FINDDATA    FindData;                 // Info on found file
    HFIND       hFind;
                                        // ------------------------------------
                                        // Note that lpszzScanList may contain
                                        // information and we can't just zap it
                                        // ------------------------------------

    lpszScanOne = lpszzScanList;
    while (*lpszScanOne)
        {
        lpszScanOne = AnsiNext (lpszScanOne);
        }
                                        // -----------------------------------
                                        // If there was info in the buffer,
                                        // increment pointer past NULL
                                        // -----------------------------------
    if (lpszScanOne != lpszzScanList)
        lpszScanOne = AnsiNext (lpszScanOne);

    for ( uCounter = 0 ; uListSize > 0 ; uCounter += 1 )
        {
        MEMSET ( szParamCurrent, EOS, sizeof(szParamCurrent) );

                                        // Get the next name to add to the
                                        // list of items to scan.
                                        // (NT ONLY) if lpszScanFileNamesIn
                                        // == NULL, we are parsing the
                                        // command line.
        if (!lpszScanFileNamesIn)
            {
            if ( ConfigParamGet ( uCounter , szParamCurrent , uListSize ) != NOERR )
               break ;
            }
        else
            {
                                        // lpszScanFileNamesIn is not NULL,
                                        // and points to a buffer full of
                                        // files, drives, directories,
                                        // etc. (probably passed from
                                        // File Manager). Get the next one;
                                        // if there are no more, break.
            bItemIsQuoted = FALSE;
            if (!lpszScanFileNamesInOne)
                break;

            if (*lpszScanFileNamesInOne == _T('\0'))
                break;

            dwSizeOfParsedString = 0;

            if (*lpszScanFileNamesInOne == _T('\"'))
                {
                                        // Item is quoted
                bItemIsQuoted = TRUE;

                lpszScanFileNamesInOneTerminator = AnsiNext (lpszScanFileNamesInOne);
                while (*lpszScanFileNamesInOneTerminator != _T('"')
                    && *lpszScanFileNamesInOneTerminator != 0)
                    {
                    lpszScanFileNamesLastChar = lpszScanFileNamesInOneTerminator;
                    lpszScanFileNamesInOneTerminator = AnsiNext (lpszScanFileNamesInOneTerminator);
                    dwSizeOfParsedString += ( lpszScanFileNamesInOneTerminator -
                                              lpszScanFileNamesLastChar );
                    }

                if (!lpszScanFileNamesInOneTerminator)
                    break;

                if (*lpszScanFileNamesInOneTerminator == _T('\0'))
                    break;
                                        // If there are any characters between the quotes
                                        // Strip the first quote
                if (dwSizeOfParsedString > 0)
                       lpszScanFileNamesInOne = AnsiNext (lpszScanFileNamesInOne);

                                        // Here, lpszScanFileNamesInOneTerminator
                                        // points to the second quotation mark:
                lpszScanFileNamesInOneTerminator = AnsiNext (lpszScanFileNamesInOneTerminator);
                                        // Here, lpszScanFileNamesInOneTerminator
                                        // points to white space or a NULL character:

                }
            else
                {
                                        // Item is not quoted
                bItemIsQuoted = FALSE;
                lpszScanFileNamesInOneTerminator = lpszScanFileNamesInOne;
                while (*lpszScanFileNamesInOneTerminator != _T(' ')
                    && *lpszScanFileNamesInOneTerminator != _T('\t')
                    && *lpszScanFileNamesInOneTerminator != 0)
                    {
                    lpszScanFileNamesLastChar = lpszScanFileNamesInOneTerminator;
                    lpszScanFileNamesInOneTerminator = AnsiNext (lpszScanFileNamesInOneTerminator);
                    dwSizeOfParsedString += ( lpszScanFileNamesInOneTerminator -
                                              lpszScanFileNamesLastChar );
                    }

                lpszScanFileNamesInOneTerminator = StringSkipToWhite (AnsiNext (lpszScanFileNamesInOne));
                if (!lpszScanFileNamesInOneTerminator)
                    break;
                                        // lpszScanFileNamesInOneTerminator
                                        // points to white space or a NULL character
                }


            if (dwSizeOfParsedString >= sizeof (szParamCurrent) )
                break;

            MEMSET (szParamCurrent, 0, sizeof (szParamCurrent));
            CopyMemory(szParamCurrent, lpszScanFileNamesInOne, dwSizeOfParsedString);

                                        // Convert to Upper Case:
            NavStrUpr ( szParamCurrent );

                                        // Set up for next:
            lpszScanFileNamesInOne = lpszScanFileNamesInOneTerminator;
            if (*lpszScanFileNamesInOne != '\0')
                {
                                        // Not EOS yet.
                                        // Move past the whitespace char
                                        // for the next char:
                lpszScanFileNamesInOne = AnsiNext (lpszScanFileNamesInOne);
                }
            }                           // End if (lpszScanFileNamesIn is not NULL) ...

                                        // ----------------------------------
                                        // If this file is a PIF or an LNK
                                        // file get whatever it points to.
                                        // ----------------------------------

        IfSpecialFileGetTrueFile ( szParamCurrent );

        if (szParamCurrent[0] != '%')       // special case for %SystemRoot% leading the file name
            {
            if ( !DBCSIsLeadByte (szParamCurrent [0])
            &&   szParamCurrent [ 1 ] == ':'
            &&   szParamCurrent [ 2 ] == '\0' )
                                        // ----------------------------------
                {                       // Special case "x:" -> "x:\"
                                        // ----------------------------------
                STRCPY ( szFullCurrent, "x:\\" ) ;
                szFullCurrent [ 0 ] = szParamCurrent [ 0 ] ;
                * lpbScanSubs = TRUE ;
                }
            else if ( NameToFull ( szFullCurrent , szParamCurrent ) == ERR )
                {
                                        // ----------------------------------
                                        // Bad filespec -- ignore
                                        // ----------------------------------
                continue;
                }

#if 0
            // ---------------------------------------------------------------
            // This code does not work with short filename equivalent of long
            // filenames. If bogus filename was entered it will be ignored
            // anyway.
            // ---------------------------------------------------------------

            if ( ! NameIsWildcard ( szFullCurrent ) &&
                (NameGetType ( szFullCurrent ) & ~NGT_IS_FULL) == 0 )
                {
                                        // ----------------------------------
                                        // Skip non-existant files
                                        // ----------------------------------
                continue;
                }
#endif

                                        // ----------------------------------
                                        // We should check to see if this item
                                        // contains wildcard characters since
                                        // NameReturnLongName() doesn't X-late
                                        // wildcard names correctly.
                                        // ----------------------------------

            if ( NameIsWildcard ( szFullCurrent ) == FALSE &&
                NameReturnLongName ( szFullCurrent, szParamCurrent, TRUE ) == ERR )
                {
                STRCPY ( szParamCurrent, szFullCurrent ) ;
                }

                                            //-----------------------------------
                                            // If the name is a wild card and
                                            // doesn't contain a drive spec, then
                                            // slap one on so we don't blow
                                            // chunks.  Defect #39992
                                            //-----------------------------------
            if ( NameIsWildcard ( szParamCurrent ) )
                {
                if ( !NameHasDriveSpec ( szParamCurrent ) &&
                    !NameIsUNC ( szParamCurrent ) )
                    {
                    MEMSET (szFullCurrent, 0, sizeof (szFullCurrent));
                    szFullCurrent[0] = DiskGet();
                    szFullCurrent[1] = ':';
                    if ( szParamCurrent[0] != '\\')
                        {
                                            //-----------------------------------
                                            // Defect #40009.  If it has a
                                            // wildcard and does not start off
                                            // with a drive spec, unc, or off
                                            // of the root, slam a ".\" in
                                            // front of the spec.
                                            //-----------------------------------
                        STRCAT ( szFullCurrent, ".\\");
                        }
                    STRCAT ( szFullCurrent, szParamCurrent );
                    STRCPY ( szParamCurrent, szFullCurrent );
                    uNewLen = STRLEN ( szParamCurrent ) + 1 ;
                    }
                else if ( NameHasDriveSpec ( szParamCurrent ) &&
                        ( szParamCurrent[2] != '\\' ) )
                    {
// it looks like its not getting to this point any more:  the NameToFull() function inserts a
// '\' at the beginning of the filespec, which might be a problem if the root is not the current
// directory and you tell the scanner to scan '*.com'

                                            //-----------------------------------
                                            // Defect #40009.  If it has a
                                            // drive spec and a wild card but
                                            // does not start off of the root,
                                            // then force it to. (i.e. c:*.com,
                                            // a:*.com, a:j*.com makes the spec
                                            // c:.\*.com, a:.\*.com, a:.\j*.com).
                                            //-----------------------------------
                    szFullCurrent[0] = szParamCurrent[0];
                    szFullCurrent[1] = szParamCurrent[1];
                    STRCPY (szFullCurrent + 2, ".\\");
                    STRCAT ( szFullCurrent, szParamCurrent + 2 );
                    STRCPY ( szParamCurrent, szFullCurrent );
                    uNewLen = STRLEN ( szParamCurrent ) + 1 ;
                    }
                }
            }   // endif [0] != '%'
        else
            szFullCurrent [ 0 ] = 0;    // MOREWORK:  we need to take the %env% and insert the
                                        // correct directory name and do the search on it.


        uNewLen = STRLEN ( szParamCurrent ) + 1 ;

        if ( uNewLen >= uListSize )
                                        // ----------------------------------
            {                           // MOREWORK:  Should resize block
                                        // For now, silently loose name
                                        // ----------------------------------
            continue;
            }

                                        // ----------------------------------
                                        // Make sure that we can see this file
                                        // or directory; if it is a directory
                                        // w/o a file spec. then tag a *.* to
                                        // the end and try accessing a file
                                        // within the directory to assure we
                                        // have access.
                                        // ----------------------------------

        uiErrMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

        STRCPY(szTestName, szFullCurrent);      // build the path w/o the file name

        bFileFound = TRUE;
        dwLastError = 0;

        // If this is a root directory, verify that the drive letter
        // actually exists.
        STRCPY( szRootTestName, szTestName );
        szRootTestName[0] = _T('x');
        if ( STRCMP( szRootTestName, _T("x:\\") ) == 0 )
            {
            if( GetDriveType(szTestName) == DRIVE_NO_ROOT_DIR )
                {
                bFileFound = FALSE;
                dwLastError = ERROR_FILE_NOT_FOUND;
                }
            }
        else if (NameIsWildcard(szTestName))
            {                           // -------------------------
                                        // if it has wildcards, look
                                        // to see if any files exist
                                        // that satisfy the wildcard
                                        // -------------------------
                                        // No check if subdirs - checking
                                        // at top level is bogus; don't bother.
            if ( FALSE == *lpbScanSubs )
                {
                hFind = FileFindFirst(szTestName, &FindData);
                dwLastError = GetLastError();
                FileFindClose(hFind);
                if (hFind == INVALID_HANDLE_VALUE)
                    {
                    bFileFound = FALSE;
                    }
                }
            }
        else
            {
                                                            // -------------------------
                                                            // no wild cards, check to see
                                                            // if we have access to this
                                                            // directory to look for files
                                                            // -------------------------
             hFind = FileFindFirst(szTestName, &FindData);
             if (hFind != INVALID_HANDLE_VALUE)             // we found something
                {
                FileFindClose(hFind);                       // close that search

                if (FindData.dwFileAttributes & FA_DIR)     // its a directory
                    {
                    NameAppendFile(szTestName, "*.*");      // check access
                    hFind = FileFindFirst(szTestName, &FindData);
                    if (hFind != INVALID_HANDLE_VALUE)
                        {
                        FileFindClose(hFind);
                        NameAppendFile(szParamCurrent, "*.*");  // add filespec to dir name
                        uNewLen = STRLEN(szParamCurrent) + 1;
                        }
                    else
                        {                                   // didn't find anything...
                        dwLastError = GetLastError();
                        bFileFound = FALSE;
                        }
                    }
                }
            else
                {                                           // failed the search, look for "*.*"
                if (szTestName[0] == 0)                      // we have the "%env%" thing again, ignore this
                    {
                    dwLastError = GetLastError();
                    bFileFound = FALSE;
                    }
                else
                    {
                    NameAppendFile(szTestName, "*.*");      // check access
                    hFind = FileFindFirst(szTestName, &FindData);
                    if (hFind != INVALID_HANDLE_VALUE)
                        {
                        FileFindClose(hFind);
                        NameAppendFile(szParamCurrent, "*.*");  // add filespec to dir name
                        uNewLen = STRLEN(szParamCurrent) + 1;
                        }
                    else
                        {
                        dwLastError = GetLastError();           // didn't find anything...
                        bFileFound = FALSE;                     // set the error code
                        }
                    }
                }
            }

        SetErrorMode(uiErrMode);


        if (!bFileFound)
            {
            UINT msg = 0;

            switch (dwLastError)
                {
                case ERROR_FILE_NOT_FOUND:
                case ERROR_PATH_NOT_FOUND:
                case ERROR_INVALID_NAME:
                    msg = IDS_GARBAGEFILENAME;
                    break;

                case ERROR_ACCESS_DENIED:
                    msg = IDS_NO_FOLDER_ACCESS;
                    break;
                }

            if (msg != 0)
                {
                auto NOTIFYSTRUCT Notify = {0};

                Notify.hInstance    = ghInst;
                Notify.hParent      = NULL;
                Notify.uMsgID       = msg;
                Notify.uTitleID     = IDS_NAVW;
                Notify.lpFileName   = szParamCurrent;

                if ( GetCustomVersion() == VERSION_NETSCAPE )
                    {           // Title is "Internet Scanner"; set at runtime.
                    Notify.uTitleID     = IDS_NAVW_INTERNET;
                    }

                                        // We won't pop up the error msg box
                                        // if we were launched with the /HMEM
                                        // switch. This prevents us from showing
                                        // errors when the AP launches NAVW
                                        // after finding a virus during a scan
                                        // on create.  The AP already denied
                                        // access and prevented the file from
                                        // being created, so there's no use in
                                        // showing a file-not-found msg here.
                if ( !s_bHMEMSwitchUsed )
                    {
                    NotifyUser (&Notify);
                    }
                }
            }
        else if ( uNewLen < (uListSize - 1))        // only add in if there is enough room
            {                                       // (account for double NULL at end)
            STRCPY ( lpszScanOne, szParamCurrent );

            lpszScanOne += uNewLen ;
            uListSize   -= uNewLen ;
            }


        } // for()

                                        // ----------------------------------
    * lpszScanOne = EOS ;              // Put second EOS at end
                                        // ----------------------------------

    SzzStripDuplicates ( lpszzScanList ) ;

} // End GatherCommandLineNames()


/* **************************************************************************

@Name:
BOOL LOCAL CallVirusList (              // Call the virus list
    HWND    hWnd                        // [in] moma window
    );

@Description:
This function calls the virus list that is inside N32VLIST.DLL

@Parameters:
$hWnd$      [in] moma window

@Returns:
    TRUE    if virus list could be displayed, FALSE othewise

************************************************************************** */

BOOL LOCAL CallVirusList (              // Call the virus list
    HWND    hWnd                        // [in] moma window
    )
{
    auto    BOOL                bReturn = FALSE;
    auto    UINT                uStatus;
    auto    PASSWORDSTRUCT      Pass;
    auto    NETALERT            Alert;
    auto    TCHAR               szWork      [ SYM_MAX_PATH+1 ];
    auto    TCHAR               szLogFile   [ SYM_MAX_PATH+1 ];
    auto    CHECKSTRUCT         PwCheck;
    auto    VIRLIST             Vir;

    MEMSET (&PwCheck, 0, sizeof(PwCheck));
    PwCheck.hWnd         = hWnd;
    PwCheck.lpNavOptions = &navOptions;
    PwCheck.uID          = PASSWORD_VIEWVIRUSLIST;

    uStatus = AskPassword (&PwCheck);

    if (uStatus == PASSWORD_ASK)
        {
        if (PasswordCheck (&PwCheck, FALSE, FALSE))
            uStatus = TRUE;
        else
            uStatus = FALSE;
        }
    else if (uStatus == PASSWORD_SHOWCONTROL)
        uStatus = TRUE;
    else
        uStatus = FALSE;

    if (uStatus == TRUE)
        {
        VirusScanGetDatFile (szWork, sizeof(szWork));
        VirusScanGetInfFile (szLogFile, sizeof(szLogFile));

                                        // ------------------------------------
                                        // This if() statement is required in
                                        // order to protect the VIRLIST.DLL
                                        // from the need of accessing even more
                                        // DLLs than it already has to today.
                                        // ------------------------------------

        MEMSET (&Pass, 0, sizeof(Pass));

        if ((navOptions.password.bProtect       == TRUE &&
             navOptions.password.bMaxProtection == TRUE) ||
            (navOptions.password.bProtect       == TRUE &&
             navOptions.password.bProtectFeature[PASSWORD_VIEWVIRUSLIST]))
            Pass.dwPassword = 0;
        else if (navOptions.password.bProtect       == TRUE &&
                 navOptions.password.bProtectFeature[PASSWORD_MODIVIRUSLIST])
            {
            Pass.dwPassword = navOptions.password.dwPassword;
            }
        else
            Pass.dwPassword = 0;

        Pass.dwSize            = sizeof(Pass);
        Pass.hParent           = hWnd;
        Pass.uMaxCharsToAccept = 16;
        Pass.bCaseSensitive    = FALSE;
        Pass.uHelpID           = 0;

        MEMSET (&Vir, 0, sizeof(Vir));
        Vir.dwSizeStruct       = sizeof(Vir);
        Vir.hDlg               = hWnd;
        Vir.dwPassword         = Pass.dwPassword;
        Vir.uMaxChars          = Pass.uMaxCharsToAccept;
        Vir.bCaseSensitive     = Pass.bCaseSensitive;
        Vir.bMacViruses        = FALSE;
        Vir.bAllowInfo         = TRUE;
        Vir.bAllowDelete       = TRUE;
                                        // ---------------------------------
                                        // Set Help Ids for Virlist Dialog
                                        // ---------------------------------
        NavVirlistDlgSetContextHelpIds( &Vir );


        Vir.bMacViruses        = FALSE;

        STRCPY ( Vir.szDatFile, szWork );
        STRCPY ( Vir.szInfFile, szLogFile );

        if (VirusList (&Vir))
            {
            bReturn = TRUE;

            LOGSTRUCT Log = { 0 };

            Log.uEvent       = LOGEV_VIRLISTCHANGE;
            Log.lpFileName   = NULL;
            Log.lpVirusName  = NULL;
            Log.lpAct        = &navOptions.activity;
            LogEvent (&Log);

            MEMSET (&Alert, 0, sizeof(Alert));
            Alert.lpNet          = navOptions.netUser.lpNetUserItem;;
            Alert.lpAlertOpt     = &navOptions.alert;
            Alert.uTypeOfEvent   = EV_ALERT_VIRLIST_CHANGE;

            NetAlert (&Alert);
            }
        }

    return (bReturn);
}


//************************************************************************
// ScanDownload()
//
// This routine scans a single file queued for download.
// Called from IDM_SCAN_EXIT - this is the wCallWhenDone kluge value
// that makes the scanner exit when in command line or download mode.
//
// Parameters:
//      HWND        hWnd                Handle to main window.
//      LPNAVMAIN   lpNavMain           Main nav options.
//
// Returns:
//      NOERR   if a downloaded file was scanned/is being scanned.
//      ERR     otherwise (queue empty or fatal error).
//************************************************************************
// 01/24/95 DALLEE, created
//************************************************************************

STATUS LOCAL ScanDownload (HWND hWnd, LPNAVMAIN lpNavMain)
{
    auto    TCHAR   szDownloadedFile [SYM_MAX_PATH];
    auto    LPTSTR  lpScanList;

    auto    STATUS  nReturn = ERR;

    static  BOOL    bScanningDownload = FALSE;

                                        // Just allow single passes through
                                        // here.  We're single threaded so
                                        // we don't need fancy checks.
    if (TRUE == bScanningDownload)
        {
        nReturn = NOERR;
        }
    else
        {
        bScanningDownload = TRUE;

                                        // Get downloaded filename.
        if (NOERR == QueueGetFile(&lpNavMain->lpDownloadedFiles, szDownloadedFile))
            {
                                        // If we have a downloaded file,
                                        // allocate memory for scan buffer.
            lpNavMain->hScanList = MemAlloc(GHND, SYM_MAX_PATH * sizeof(TCHAR));

            if (NULL != lpNavMain->hScanList)
                {
                lpScanList = (LPTSTR)MemLock(lpNavMain->hScanList);

                if (NULL != lpScanList)
                    {
                    MEMSET(&lpNavMain->ScanStruct,
                           0,
                           sizeof(lpNavMain->ScanStruct));

                    lpNavMain->ScanStruct.uProgramMode = PROGRAM_DOWNLOAD;

                    STRCPY(lpScanList, szDownloadedFile);

                                            // Drive letter should be upper case.
                    *lpScanList = CharToUpper(*lpScanList);

                    MemUnlock(lpNavMain->hScanList);
                    StartScanning(hWnd, lpNavMain);

                                            // This is allegedly freed in
                                            // ScanStart(). <What a mess>
                    lpNavMain->hScanList = NULL;

                    nReturn = NOERR;
                    }
                }
                                            // Only need to free on error since
                                            // hScanList is freed in ScanStart().
            if (NULL != lpNavMain->hScanList)
                {
                MemFree(lpNavMain->hScanList);
                }

            } // QueueGetFile()

        bScanningDownload = FALSE;
        }

    return (nReturn);
} // ScanDownload()


/* **************************************************************************

@Name:
VOID LOCAL CallScanDirectory (          // Call scan directory function
    HWND        hWnd,                   // [in] moma window
    LPNAVMAIN   lpNavMain               // [in] nav structure pointer
    );

@Description:
This function queries the directory AKA folder to scan and calls the scan
function.

@Parameters:
$hWnd$          [in] moma window
$lpNavMain$     [in] nav structure pointer

@Returns:       Nothing

************************************************************************** */

struct   SCANDIRCONTROLS
{
    CNVBrowse       browse;
    CWindowResizer  resizer;
    RECT            rectMin;
};

VOID LOCAL CallScanDirectory (          // Call scan directory function
    HWND        hWnd,                   // [in] moma window
    LPNAVMAIN   lpNavMain               // [in] nav structure pointer
    )
{
    SYM_ASSERT ( hWnd );
    SYM_ASSERT ( lpNavMain );

    auto    SCANDIRCONTROLS     scanDirControls;

    auto    DWORD       dwBufferSize;
    auto    DWORD       dwFolderLength;
    auto    int         nEntries;

    auto    LPSCANITEM  lpScanItems;
    auto    LPTSTR      lpszzScanList;

    auto    list<CNVBrowseItem>::iterator   itFolders;

    MEMSET( &lpNavMain->ScanStruct, 0, sizeof( lpNavMain->ScanStruct ) );
    lpNavMain->ScanStruct.bScanSubs = TRUE;

    if ( DialogBoxParam( ghInst,
                         MAKEINTRESOURCE( IDD_SCAN_FOLDERS ),
                         hWnd,
                         ScanFoldersProc,
                         ( LPARAM )&scanDirControls ) )
    {
        // Find number of selected folders and
        // size of required buffer for names.

        dwBufferSize = 0;
        nEntries     = 0;

        for ( itFolders = scanDirControls.browse.m_listSelectedItems.begin();
              itFolders != scanDirControls.browse.m_listSelectedItems.end();
              itFolders++ )
        {
            SYM_ASSERT( itFolders->GetName() );

            dwFolderLength = _tcslen( itFolders->GetName() );

            if ( dwFolderLength < SYM_MAX_PATH )
            {
                dwBufferSize += 1+ dwFolderLength;
                nEntries++;
            }
        }

        // Add one to required size for double EOS termination.

        dwBufferSize = ( 1 + dwBufferSize ) * sizeof( TCHAR );

        if ( dwBufferSize &&
             ( NULL != ( lpNavMain->hScanList = MemAlloc( GHND, dwBufferSize ) ) ) )
        {
            // Going to lock lpNavMain->hScanList and leave it locked.
            // This memory will be released and free at the end of scan.

            lpszzScanList = ( LPTSTR )MemLock( lpNavMain->hScanList );

            if ( NULL != ( lpScanItems = ( LPSCANITEM )MemAllocPtr( GHND, ( nEntries + 1 ) * sizeof( SCANITEM ) ) ) )
            {
                lpNavMain->ScanStruct.lpScanItems = lpScanItems;

                // Read in all selected folder names.

                for ( itFolders = scanDirControls.browse.m_listSelectedItems.begin();
                      itFolders != scanDirControls.browse.m_listSelectedItems.end();
                      itFolders++ )
                {
                    SYM_ASSERT( itFolders->GetName() );

                    if ( _tcslen( itFolders->GetName() ) < SYM_MAX_PATH )
                    {
                        _tcscpy( lpszzScanList, itFolders->GetName() );

                        lpScanItems->lpszPathName = lpszzScanList;
                        lpScanItems->bScanSubDirs = itFolders->GetSubDirs();

                        lpszzScanList += 1 + _tcslen( lpszzScanList );
                        lpScanItems++;
                    }
                }

                StartScanning( hWnd, lpNavMain );
            }
            else
            {
                MemUnlock( lpNavMain->hScanList );
                MemRelease( lpNavMain->hScanList );
                lpNavMain->hScanList = NULL;
            }
        }
    }
} // CallScanDirectory()

/* **************************************************************************

@Name:
VOID LOCAL CallScanPath (          // Call scan directory function
    HWND        hWnd,                   // [in] moma window
    LPNAVMAIN   lpNavMain               // [in] nav structure pointer
    );

@Description:
    This function allows a user to enter a path to scan manually.

@Parameters:
$hWnd$          [in] moma window
$lpNavMain$     [in] nav structure pointer

@Returns:       Nothing

************************************************************************** */


VOID LOCAL CallScanPath (
    HWND        hWnd,
    LPNAVMAIN   lpNavMain
    )
{
    SYM_ASSERT ( hWnd );
    SYM_ASSERT ( lpNavMain );

    MEMSET (&lpNavMain->ScanStruct, 0, sizeof(lpNavMain->ScanStruct));
    lpNavMain->ScanStruct.bScanSubs = TRUE;

    auto    TCHAR           szPathToScan[ SYM_MAX_PATH ];
    auto    int             iResult;
    auto    HINSTANCE       hInstance;
    auto    UINT            uBogusAttr;
    auto    FINDDATA        FindData;
    auto    HFIND           hFindFirst;
    auto    TCHAR           szAppName[SYM_MAX_PATH],
                            szMessage[SYM_MAX_PATH],
                            szBuffer [SYM_MAX_PATH];

    hInstance = (HINSTANCE)GetWindowLong ( hWnd, GWL_HINSTANCE );


    // Fire off dialog.
    iResult = DialogBoxParam( ghInst,
                            MAKEINTRESOURCE( IDD_SCAN_PATH_DIALOG ),
                            hWnd,
                            ScanPathProc,
                            ( LPARAM ) szPathToScan );

    if( iResult == IDCANCEL )
        return;
                            // -----------------------------------
                            // Make sure we have access to read
                            // the files contained in the directory.
                            // -----------------------------------
    hFindFirst = FileFindFirst( szPathToScan, &FindData );
    if ( ( hFindFirst == INVALID_HANDLE_VALUE ) &&
            ( GetLastError() == ERROR_ACCESS_DENIED ) )
        {
        LoadString( ghInst, IDS_NAVW, szAppName, sizeof(szAppName) );
        LoadString( ghInst, IDS_NO_FOLDER_ACCESS, szBuffer,
                    sizeof (szBuffer) );
        SPRINTF ( szMessage, szBuffer, (LPSTR)szPathToScan );
        MessageBox( hWnd, szMessage, szAppName, MB_OK | MB_ICONSTOP );
        return;
        }
    else
        FileFindClose( hFindFirst );

                            // -----------------------------------
                            // Make sure that we can see this folder
                            // If the path name contains non-Ansi
                            // translatable characters they will
                            // appear here as garbage. We will get
                            // a file name but it won't be usable.
                            // -----------------------------------
    if (NOERR != FileGetAttr (szPathToScan, &uBogusAttr))
        {
        LoadString (ghInst, IDS_NAVW, szAppName, sizeof (szAppName));

        int iCaption = IDS_NAVW;

        if (GetCustomVersion () == VERSION_NETSCAPE)
            iCaption = IDS_NAVW_INTERNET;

        LoadString (ghInst, IDS_GARBAGETHING, szBuffer, sizeof (szBuffer));
        SPRINTF (szMessage, szBuffer, szPathToScan);
        MessageBox (hWnd, szMessage, szAppName, MB_OK | MB_ICONSTOP);
        return;
        }


    if ((lpNavMain->hScanList = MemAlloc(GHND, SYM_MAX_PATH)) != 0)
        {
        lpNavMain->ScanStruct.bScanSubs = (BOOL)(iResult == IDC_SUB_CHECK);

        LPTSTR lpScanList = (LPTSTR)MemLock(lpNavMain->hScanList);

        STRCPY (lpScanList, szPathToScan );

                                    // ----------------------------------
                                    // Drive letter should be upper case.
                                    // ----------------------------------
        *lpScanList = CharToUpper ( *lpScanList );

                                    // ----------------------------------
                                    // hScanList if freed up in ScanStart
                                    // once the function is done with the
                                    // memory block
                                    // ----------------------------------

        MemUnlock ( lpNavMain->hScanList );
        StartScanning ( hWnd, lpNavMain );
        }

}


BOOL CALLBACK ScanPathProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    static  LPTSTR  pszPath;
    auto    BOOL    bProcessed;
    auto    HWND    hCtrl;

    bProcessed = TRUE;

    switch( uMsg )
        {
        case WM_INITDIALOG:
            {
            // Set the correct font for this dialog.
            NG_SetFontsInDialog( hDlg, 0 );

            // Save off pointer to path.
            pszPath = (LPTSTR) lParam;

            // Set up edit box.
            hCtrl = GetDlgItem( hDlg, IDC_PATH_EDIT );
            Edit_LimitText( hCtrl, SYM_MAX_PATH );

            // Set up icon
            hCtrl = GetDlgItem( hDlg, IDC_ICON_STATIC );
            HICON hIcon = LoadIcon( ghInst, MAKEINTRESOURCE( IDI_NAVW_APP ) );
            Static_SetIcon( hCtrl, hIcon );

            }
            break;

        case WM_COMMAND:
            {
            switch( LOWORD( wParam ) )
                {
                case IDOK:
                    {
                    // Get text from edit control
                    hCtrl = GetDlgItem( hDlg, IDC_PATH_EDIT );
                    GetWindowText( hCtrl, pszPath, SYM_MAX_PATH );

                    // Make sure path is valid
                    if( lstrlen( pszPath ) && NameValidatePath( pszPath ) )
                        {
                        // Check for path in form of "C:" and change to "C:\"
                        if( lstrlen( pszPath ) == 2 && pszPath[1] == _T(':') )
                            {
                            lstrcat( pszPath, _T("\\") );
                            }

                        // Kill dialog.  Return IDC_SUB_CHECK if checkbox is checked.
                        EndDialog( hDlg, IsDlgButtonChecked( hDlg, IDC_SUB_CHECK ) ? IDC_SUB_CHECK : IDOK );
                        }
                    else
                        {
                        // Make the user aware of the error of his ways.
                        TCHAR szMessage[SYM_MAX_PATH * 2],
                              szAppName[SYM_MAX_PATH],
                              szBuffer[SYM_MAX_PATH];

                        // Form message.
                        LoadString ( ghInst, IDS_NAVW, szAppName, SYM_MAX_PATH );
                        LoadString ( ghInst, IDS_INVALID_FOLDER_PATH, szBuffer, SYM_MAX_PATH );
                        SPRINTF ( szMessage, szBuffer, pszPath );

                        MessageBox( hDlg, szMessage, szAppName,
                                    MB_OK | MB_ICONSTOP );

                        // Set focus back to edit control.
                        hCtrl = GetDlgItem( hDlg, IDC_PATH_EDIT );
                        SetFocus( hCtrl );
                        Edit_SetSel( hCtrl, 0, -1 );
                        }
                    }
                    break;

                case IDCANCEL:
                    EndDialog( hDlg, IDCANCEL );
                    break;
                }
            }
            break;


        default:
            bProcessed = FALSE;
            break;
        }


    return bProcessed;

}


BOOL CALLBACK ScanFoldersProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    auto    SCANDIRCONTROLS     *pControls;
    auto    BOOL                bProcessed;
    auto    LPMINMAXINFO        lpMinMaxInfo;
    auto    HICON               hIcon;

    bProcessed = TRUE;

    pControls = ( SCANDIRCONTROLS * )GetWindowLong( hDlg, DWL_USER );

    switch ( uMsg )
    {
    case WM_INITDIALOG:
        pControls = ( SCANDIRCONTROLS * )lParam;
        SetWindowLong( hDlg, DWL_USER, lParam );

        // Set minimum dialog size. Mapping dialog units to screen pixels.

        pControls->rectMin.left   = 0;
        pControls->rectMin.top    = 0;
        pControls->rectMin.right  = 123;
        pControls->rectMin.bottom = 114;

        MapDialogRect( hDlg, &pControls->rectMin );

        // Since we need WS_THICKFRAME for resizing, we're stuck with
        // having an icon. Need to set it to main NAV icon.

        hIcon = LoadIcon( ghInst, MAKEINTRESOURCE( IDI_NAVW_APP ) );
        SendMessage( hDlg, WM_SETICON, ICON_BIG, ( LPARAM )hIcon );
        SendMessage( hDlg, WM_SETICON, ICON_SMALL, ( LPARAM )hIcon );

        // Add window resizer.

        pControls->resizer.SetParent( hDlg );
        pControls->resizer.UseSizeGrip( TRUE );
        pControls->resizer.RegisterWindow( IDOK,       1, 1, 0, 0 );
        pControls->resizer.RegisterWindow( IDCANCEL,   1, 1, 0, 0 );
        pControls->resizer.RegisterWindow( IDC_BROWSE, 0, 0, 1, 1 );
        pControls->resizer.RegisterWindow( IDC_SCAN_FOLDERS_TEXT, 0, 0, 1, 0 );

        pControls->resizer.SetInitialized( TRUE );

        // Set fonts and do centering. Centering has to happen after
        // saving SCANDIRCONTROLS and setting min rect since it will
        // cause a WM_GETMINMAXINFO to be sent.

        NG_SetFontsInDialog( hDlg, 0 );
        NG_CenterDialog( hDlg, GetParent( hDlg ) );

        // Initialize folder browse control.

        pControls->browse.Attach( GetDlgItem( hDlg, IDC_BROWSE ) );
        pControls->browse.Init( ghInst );
        break;

    case WM_NOTIFY:
        pControls->browse.OnNotify( wParam, lParam );
        break;

    case WM_SIZE:
        // Pass size messages to Ken's spanky resizer.
        // Need to invalidate text controls, since Windows doesn't redraw
        // correctly if the line wrapping changes.

        pControls->resizer.UpdateSize( wParam, LOWORD( lParam ), HIWORD( lParam ) );
        InvalidateRect( GetDlgItem( hDlg, IDC_SCAN_FOLDERS_TEXT ), NULL, TRUE );
        break;

    case WM_GETMINMAXINFO:
        lpMinMaxInfo = ( LPMINMAXINFO ) lParam;

        lpMinMaxInfo->ptMinTrackSize.x = pControls->rectMin.right;
        lpMinMaxInfo->ptMinTrackSize.y = pControls->rectMin.bottom;

        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) )
        {
        case IDOK:
            pControls->browse.UpdateSelectedList();

            if ( pControls->browse.GetTotalSelectedItems() == 0 )
                break;

            EndDialog( hDlg, 1 );
            break;

        case IDCANCEL:
            EndDialog( hDlg, 0 );
            break;
        }

        break;

    default:
        bProcessed = FALSE;
        break;
    }

    return ( bProcessed );
}


/* **************************************************************************

@Name:
VOID WINAPI StartScanning (             // Start scanning the gathered stuff
    HWND        hParent,                // [in] moma
    LPNAVMAIN   lpNavMain               // [in] nav structure pointer
    );

@Description:
This function starts scanning the required items gathered by helped funcs.

@Parameters:
$hParent$           [in] moma
$lpNavMain$         [in] nav structure pointer

@Returns:       Nothing

************************************************************************** */

VOID WINAPI StartScanning (             // Start scanning the gathered stuff
    HWND        hParent,                // [in] moma
    LPNAVMAIN   lpNavMain               // [in] nav structure pointer
    )
{
    auto    LPSCANSTRUCT    lpScan;
    auto    LPTSTR          lpszScanList;

    if ( lpNavMain->bScanInProgress == FALSE )
        {
                                        // ----------------------------------
                                        // Allocate chunk of memory that
                                        // will be freed in NAVWORK upon scan
                                        // completion.
                                        // ----------------------------------

        lpScan = (LPSCANSTRUCT)MemAllocPtr(GHND, sizeof(SCANSTRUCT));

        if ( lpScan )
            {
            lpScan->uProgramMode = lpNavMain->ScanStruct.uProgramMode;
            lpScan->bHMemLaunch  = lpNavMain->ScanStruct.bHMemLaunch;

            if (IsMinimized(hParent))
                {
                lpScan->wCallWhenDone = IDM_SCAN_EXIT;

                // Klugey, but least dangerous way to add this to the code...
                // Setting wCallWhenDone here used to be the only way to
                // indicate command line mode.
                // Note: uProgramMode may already be set to PROGRAM_DOWNLOAD
                // if running as a web-browser plug-in.

                if (PROGRAM_INTERACTIVE == lpScan->uProgramMode)
                    lpScan->uProgramMode = PROGRAM_COMMANDLINE;
                }

            lpScan->bScanSubs       = lpNavMain->ScanStruct.bScanSubs;
            lpScan->hDlg            = hParent;
            lpScan->hScanList       = lpNavMain->hScanList;
            lpScan->lpNavOptions    = &navOptions;
            lpScan->lpBoolScanning  = &lpNavMain->bScanInProgress;

            lpScan->bBootOnly       = lpNavMain->ScanStruct.bBootOnly;
            lpScan->bMemOnly        = lpNavMain->ScanStruct.bMemOnly;
            lpScan->uScanMemory     = lpNavMain->ScanStruct.uScanMemory;
            lpScan->uScanBoot       = lpNavMain->ScanStruct.uScanBoot;
            lpScan->uCpuLoad        = lpNavMain->ScanStruct.uCpuLoad;

            // Get ScanStart()'s new input format:
            // lpScanItems will be filled in already for NAVUI launched
            // scans. Old-style input will be translated here with a
            // call to BuildScanListIndex() (see notes there).

            if ( NULL != lpNavMain->ScanStruct.lpScanItems )
                {
                lpScan->lpScanItems = lpNavMain->ScanStruct.lpScanItems;
                }
            else
                {
                lpszScanList = (LPTSTR) MemLock( lpScan->hScanList );

                if ( NULL != lpszScanList )
                    {
                    lpScan->lpScanItems = BuildScanListIndex( lpszScanList,
                                                              lpScan->bScanSubs );

                    // Memory will be released and freed at the end of the scan.
                    // We need to keep this buffer locked since the new
                    // lpScanItems has pointers into the current buffer.
                    //MemRelease( lpScan->hScanList );
                    }
                }

            // If we got our scan data, go and scan it.

            if ( NULL != lpScan->lpScanItems )
                {
                                            // Try to load new defs from the
                                            // shared definitions directory.
                if (FALSE == LoadVirusDefs())
                    {
                    extern WORD g_wStartupTask;
                                            // Prevent scans from happening if
                                            // defs didn't reload.
                                            // Also display no defs message.
                    lpNavMain->bScanInProgress = TRUE;
                    PostMessage (hParent, NAV_MSG_STARTUP, NAVW_STARTUP_NODEFS, 0L);
                    g_wStartupTask = 0;
                    }
                else
                    {
                    lpNavMain->bScanInProgress = TRUE;
                    lpNavMain->hScanMain       = ScanStart (lpScan);

                    // lpScan, hScanList and lpScanItems will be freed at the end
                    // of the scan. Make sure our BailOut doesn't catch 'em.

                    lpScan = NULL;
                    lpNavMain->hScanList = NULL;
                    lpNavMain->ScanStruct.lpScanItems = NULL;
                    }
                }
            }
        }

//BailOut:
    // If we bailed out and didn't call ScanStart(), we need to free these
    // manually.
    // The buffers are normally freed at the end of the scan initiated by
    // calling ScanStart(). It's not possible to free them here, as
    // ScanStart() launches a modeless scan dialog and immediately returns.
    // DALLEE 04/1997

    if ( lpScan )
        MemFreePtr( lpScan );

    if ( lpNavMain->hScanList )
        {
        MemFree( lpNavMain->hScanList );
        lpNavMain->hScanList = NULL;
        }

    if ( lpNavMain->ScanStruct.lpScanItems )
        {
        MemFreePtr( lpNavMain->ScanStruct.lpScanItems );
        lpNavMain->ScanStruct.lpScanItems = NULL;
        }

} // StartScanning();


/* **************************************************************************
@Name: NavVirlistDlgSetContextHelpIds()

@Description:
Does NAV Common VirusList dialog context sensitive help string settings.

$lpVirslist$ - address of struct which describes Virlist dialog properties.

@Returns:
NOTHING

***************************************************************************** */

VOID LOCAL NavVirlistDlgSetContextHelpIds( LPVIRLIST lpsVirlist )
{


    lpsVirlist->uHelpVirusList         = IDH_NAVW_VIRUS_LIST_DLG_HELP_BTN;

//&^    lpsVirlist->uHelpRightClickThisDlg = IDH_NAVW_WHATHIS_VIRUS_LIST_DLG;
    lpsVirlist->uHelpRightClickThisDlg = 0;//&^
    lpsVirlist->uHelpRightClickCombo   = IDH_NAVW_WHATHIS_VIRUS_LIST_DISPLAY_LSTBX;
    lpsVirlist->uHelpRightClickList    = IDH_NAVW_WHATHIS_VIRUS_LIST_BOX;
    lpsVirlist->uHelpRightClickPrint   = IDH_NAVW_WHATHIS_VIRUS_LIST_PRINT_BTN;
//&^    lpsVirlist->uHelpRightClickCustom  = IDH_NAVW_WHATHIS_VIRUS_LIST_DLG;
    lpsVirlist->uHelpRightClickCustom  = 0;//&^
    lpsVirlist->uHelpRightClickDelete  = 0;
    lpsVirlist->uHelpRightClickInfo    = IDH_NAVW_WHATHIS_VIRUS_LIST_INFO_BTN;


    lpsVirlist->uHelpVirdefDelete     =  // Help ID for Delete Virus Dlg
//&^                IDH_NAVW_VIRUS_DEFINITION_DLG_HELP_BTN;
                0;//&^


    lpsVirlist->uHelpVirusInfo        = // Help ID for Virus Info Dlg
                IDH_NAVW_VIRUS_LIST_INFO_DLG_HELP_BTN;
    lpsVirlist->uHelpVirusInfoRightClickThisDlg =
//&^                IDH_NAVW_WHATHIS_VIRUS_LIST_VIRUS_INFORMATION_DLG;
                0;//&^
    lpsVirlist->uHelpVirusInfoRightClickComments =
                IDH_NAVW_WHATHIS_VIRUS_LIST_INFO_DLG_COMMENTS_OPTION;

                                        // New ones added with expansion
                                        // of LPVIRLIST structure.
    lpsVirlist->uHelpVirusInfoRightClickCloseBtn =
                IDH_NAVW_WHATHIS_VIRUS_FOUND_INFORMATION_CLOSE_BTN;
    lpsVirlist->uHelpVirusInfoRightClickPrintBtn =
                IDH_NAVW_WHATHIS_SCAN_RESULTS_PRINT_BTN;
    lpsVirlist->uHelpVirusInfoRightClickCharacteristics =
                IDH_NAVW_WHATHIS_VIRUS_FOUND_CHARACTERISTICS;

/*
    lpsVirlist->uHelpPrint            = // Help IDs for Print Dlg
                IDH_NAVW_PRINT_DLG_HELP_BTN;

    lpsVirlist->uHelpPrintRightClickThisDlg = \
                IDH_NAVW_WHATHIS_PRINT_DLG;

    lpsVirlist->uHelpPrintBrowse      = // Help ID for Print Browse  Dlg
                IDH_NAVW_PRINT_TO_FILE_DLG_HELP_BTN;

                                        // Help IDs for Print Dlg controls:
    lpsVirlist->uHelpPrintRightClickPrintPrinterButton     =  \
	    lpsVirlist->uHelpPrintRightClickPrintPrinterTunnel =  \
	                     IDH_NAVW_WHATHIS_PRINT_DLG_PRINTER_BTN;

    lpsVirlist-> uHelpPrintRightClickPrintFileButton     =  \
	    lpsVirlist->uHelpPrintRightClickPrintFileTunnel  =  \
			       IDH_NAVW_WHATHIS_PRINT_DLG_PRINT_TO_FILE_BTN;

    lpsVirlist->uHelpPrintRightClickSendUserButton       =  \
			IDH_NAVW_WHATHIS_WHATHIS_PRINT_DLG_MAIL_TO_USER_BTN;
*/

    lpsVirlist->uHelpPrintOverwrite   = // Help ID for Print Overwrite Dlg
                IDH_NAVW_OVERWRITE_DLG_HELP_BTN;



    lpsVirlist->lpdwHelpHowToArray           = NULL;
    lpsVirlist->lpdwHelpHowToInfoVirDefArray = NULL;


//
//    LPDWORD     lpdwHelpHowToDelVirDefArray;

}


/* **************************************************************************

@Name:
VOID LOCAL NavOptsErrorNotify (         // If errors notify about it
    HWND        hParent,                // [in] parent window handle
    LPALERTOPT  lpAlert,                // [in] alert options
    WORD        wStatus                 // [in] status returned from opts
    );

@Description:
This function alerts user about errors occured while updating NAVW options.

@Parameters:
$hParent$       [in] parent window handle
$lpAlert$       [in] alert options
$uStatus$       [in] status returned from opts

@Returns:       Nothing

************************************************************************** */

VOID LOCAL NavOptsErrorNotify (         // If errors notify about it
    HWND        hParent,                // [in] parent window handle
    LPALERTOPT  lpAlert,                // [in] alert options
    WORD        wStatus                 // [in] status returned from opts
    )
{
    SYM_ASSERT ( hParent );
    SYM_ASSERT ( lpAlert );

    auto    NOTIFYSTRUCT    Notify      = { 0 };
    auto    UINT            uMessageID  = 0;

    if ( OPTIONS_ADDVXD_ERROR & wStatus )
        {
        uMessageID = IDS_NAVWOPTS_VXDADD_ERROR;
        }
    else if ( OPTIONS_REMOVEVXD_ERROR & wStatus )
        {
        uMessageID = IDS_NAVWOPTS_VXDREMOVE_ERROR;
        }

    if ( uMessageID )
        {
        Notify.hParent      = hParent;
        Notify.uMsgID       = uMessageID;

        if ( GetCustomVersion()  == VERSION_NETSCAPE )
            Notify.uTitleID = IDS_NAVW_INTERNET;
        else
            Notify.uTitleID = IDS_NAVW;

        Notify.uRemoveAfter = (lpAlert->bRemoveAfter) ?
                               lpAlert->uSeconds : NULL;
        Notify.hInstance    = ghInst;

        Notify.lpAlertMsg   = (lpAlert->bDispAlertMsg) ?
                              lpAlert->szAlertMsg : NULL;

        NotifyUser (&Notify);
        }
}


/* **************************************************************************

@Name:
VOID LOCAL NotifyNavapw (               // Notify NAVAPW about something to do
    WORD    wStatus                     // [in] status from options update.
    );

@Description:
This function attempts to notify NAVAPW to take some action. This function
is called after the options were changed and some of the options affect
NAVAPW.

@Parameters:
$wStatus$       [in] status returned from options update

@Returns:       Nothing

@Update:
   07/16/96 - DSACKIN - NAV for NT SHOULD check to see if there is an APW
                        running and post the messages to it.  If there is
                        no APW running, this code is not going to hurt anything.
************************************************************************** */

VOID LOCAL NotifyNavapw (               // Notify NAVAPW about something to do
    WORD    wStatus                     // [in] status from options update.
    )
{

    auto    HWND    hNavapW;
    static  UINT    uNavApWMessage = 0;

    hNavapW = FindWindow( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() ?
                              NAVAP_WND_CLASS : NAVAP_95_WND_CLASS,
                          NULL );

    if ( hNavapW &&
       ( OPTIONS_HIDEVXDICON      & wStatus ||
         OPTIONS_SHOWVXDICON      & wStatus ||
         OPTIONS_VXDUNLOADABLE    & wStatus ||
         OPTIONS_VXDNOTUNLOADABLE & wStatus ) )
        {
                                        // ----------------------------------
                                        // Try to establish communication with
                                        // the VXD's agent so it's UI can be
                                        // updated.
                                        // ----------------------------------
        if ( !uNavApWMessage )
            {
            uNavApWMessage = RegisterWindowMessage (NAVW_NAVAPW_COMM);
            }

        if ( uNavApWMessage )
            {
            if ( OPTIONS_HIDEVXDICON & wStatus ||
                 OPTIONS_SHOWVXDICON & wStatus )
                {
                PostMessage ( hNavapW, uNavApWMessage, NAVW_CMD_SHOWHIDEICON,
                            (OPTIONS_SHOWVXDICON & wStatus) ? TRUE : FALSE );
                }

            if ( OPTIONS_VXDUNLOADABLE    & wStatus ||
                 OPTIONS_VXDNOTUNLOADABLE & wStatus )
                {
                PostMessage ( hNavapW, uNavApWMessage, NAVW_CMD_CANBEDISABLED,
                            (OPTIONS_VXDUNLOADABLE & wStatus) ? TRUE : FALSE );
                }
            }
        }
}


//************************************************************************
// LaunchScheduler()
//
// This routine lauches the scheduler for NAV.
// This may be either N32SCHED.EXE, or the NAVNTSCH.DLL.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/27/1997 DALLEE, moved code here from huge in-line mess in
//      NavMainWndWM_COMMAND().
//************************************************************************

VOID LOCAL LaunchScheduler (
    void
    )
{
    auto    TCHAR       szFile [ SYM_MAX_PATH + 1 ];
    auto    TCHAR       szWork [ SYM_MAX_PATH + 1 ];

                                        // ------------------------------------
                                        // Get the startup directory for the
                                        // scheduler executables.
                                        // ------------------------------------
    GetStartDir( ghInst, szFile, sizeof(szFile) );


                                        // ------------------------------------
                                        // Load scheduler executable name.
                                        // This now works for NT and 95, since
                                        // our scheduling solutions are now
                                        // both executables.
                                        //
                                        // Windows95: NSCHED32.EXE
                                        // WindowsNT: NSCHEDNT.EXE
                                        // ------------------------------------
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
        {
                                        // Running under Windows 95
        LoadString( ghInst, IDS_SCHEDULER, szWork, sizeof(szWork) );
        }
    else
        {
                                        // Running under NT
        LoadString( ghInst, IDS_SCHEDULERNT, szWork, sizeof(szWork) );
        }


    NameAppendFile( szFile, szWork );

    SetCursor( LoadCursor( NULL, IDC_WAIT ) );

                                        // ------------------------------------
                                        // Execute the respective scheduler for
                                        // the OS currently running on.
                                        // ------------------------------------
    WinExec( szFile, SW_SHOW );

    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
} // LaunchScheduler()


//************************************************************************
// LaunchQuarantineConsole()
//
// This routine lauches the Quarantine console.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 04/14/1998 DALLEE, created.
//************************************************************************

VOID LOCAL LaunchQuarantineConsole()
{
    auto    TCHAR       szConsolePath [ SYM_MAX_PATH + 1 ];
    auto    TCHAR       szConsoleFileName [ SYM_MAX_PATH + 1 ];
    auto    TCHAR       szFullPathName [ SYM_MAX_PATH + 1 ];

    // Get console name.

    GetStartDir( ghInst, szConsolePath, sizeof( szConsolePath ) );

    // Set current directory here.
    SetCurrentDirectory( szConsolePath );

    lstrcpy( szFullPathName, szConsolePath );

    LoadString( ghInst, IDS_QUARANTINE_CONSOLE, szConsoleFileName, sizeof( szConsoleFileName ) );
    NameAppendFile( szFullPathName, szConsoleFileName );

    // Launch console with busy cursor (might be waiting a bit).

    SetCursor( LoadCursor( NULL, IDC_WAIT ) );

    WinExec( szFullPathName, SW_SHOWNORMAL );

    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
} // LaunchQuarantineConsole()


//***************************************************************************
// NAVTeamHookProc()
//
// Description:
//      Hook proc in the about box that decides when it is time to displays
//      a photo of The Norton AntiVirus for Windows '95 Team.
//
// Parameters:
//      hWnd,                           [in] Window handle
//      uMessage,                       [in] Message to process
//      wParam,                         [in] Message wParam
//      lParam                          [in] Message lParam
//
//***************************************************************************
// 07/04/1995 MARKL Function Created.
//***************************************************************************

VOID SYM_EXPORT WINAPI NavTeamHookProc ( // About Box Hook Proc
    HWND        hWnd,                   // [in] Window handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] Message wParam
    LPARAM      lParam                  // [in] Message lParam
    )
{
    static      BOOL    bBusy   = FALSE;
    auto        BYTE    byKeys[256];

    if ( (uMessage == WM_KEYDOWN) && !bBusy )
        {
        GetKeyboardState ( byKeys );

        if ( (byKeys['N'] & 0x80) &&
             (byKeys['A'] & 0x80) &&
             (byKeys['V'] & 0x80) )
            {
            bBusy = TRUE;
            DialogBox ( ghInst, MAKEINTRESOURCE(IDD_NAVEGG), hWnd, NavEggProc );
            bBusy = FALSE;
            }
        }
} // NavTeamHookProc()


//***************************************************************************
// NavEggProc()
//
// Description:
//      Displays The Norton AntiVirus for Windows '95 Team Photo!!!  This is
//      a simple dialog box proc that handles things for us.  Source stolen
//      directly from NU Team and pasted in to save time.
//
// Parameters:
//      hWnd,                           [in] Dialog Box handle
//      uMessage,                       [in] Message to process
//      wParam,                         [in] Message wParam
//      lParam                          [in] Message lParam
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/04/1995 MARKL Function Created.
//***************************************************************************

BOOL SYM_EXPORT CALLBACK NavEggProc (   // Display Team Photo
    HWND        hDlg,                   // [in] Dialog Box handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] Message wParam
    LPARAM      lParam                  // [in] Message lParam
    )
{
    static      TCHAR           szGreet[100];
    static      HBITMAP         hBmp = NULL;
    auto        HPALETTE        hPal = NULL;
    auto        HGLOBAL         hDIB;
    auto        HDC             hDC;
    auto        HDC             hMemDC;
    auto        HRSRC           hRes;
    auto        HPALETTE        hOldPal;
    auto        HBITMAP         hOldBmp;
    auto        PAINTSTRUCT     ps;
    auto        LPBITMAPINFOHEADER lpbmInfoHdr;
    auto        LPTSTR          lpBits;
    static      int             nWhich;
    auto        BOOL            rtn = TRUE;
    auto	RECT		rect;

    switch (uMessage)
    {
    case WM_INITDIALOG:
        NG_SetFontsInDialog (hDlg, 0);
        if (hBmp)
            {
            DeleteObject( hBmp );
            hBmp = NULL;
            }
        hRes = FindResource( ghInst, MAKEINTRESOURCE(IDB_NAVTEAM), RT_BITMAP );
        if (!hRes)
            {
            EndDialog( hDlg, IDOK );
            break;
            }
        hDIB = LoadResource( ghInst, hRes );
        if (!hDIB)
            {
            EndDialog( hDlg, IDOK );
            break;
            }

        lpbmInfoHdr = (LPBITMAPINFOHEADER) LockResource (hDIB);
        if (!lpbmInfoHdr)
            {
            EndDialog( hDlg, IDOK );
            break;
            }

        lpBits = (LPTSTR)lpbmInfoHdr + *(LPDWORD)lpbmInfoHdr +
                            PaletteSize((LPTSTR)lpbmInfoHdr);

        if (!lParam)
            {
            nWhich = -1;
            GetWindowText( hDlg, szGreet, sizeof(szGreet) );
            SetWindowPos( hDlg, NULL, 0, 0,
                (int)lpbmInfoHdr->biWidth + 2 * GetSystemMetrics( SM_CXBORDER ),
                (int)lpbmInfoHdr->biHeight + GetSystemMetrics( SM_CYCAPTION ) +
                              GetSystemMetrics( SM_CYBORDER ),
                SWP_NOMOVE | SWP_NOZORDER );
            CenterMain( hDlg );
            }
        hPal = CreateDIBPalette( hDIB );
        hDC = GetDC( hDlg );
        if (!hDC)
            {
            EndDialog( hDlg, IDOK );
            break;
            }
        hBmp = CreateCompatibleBitmap( hDC,
                (WORD)lpbmInfoHdr->biWidth, (WORD)lpbmInfoHdr->biHeight );
        if (hPal)
            {
            hOldPal = SelectPalette( hDC, hPal, FALSE);
            RealizePalette( hDC );
            }

        SetDIBits( hDC, hBmp, 0, (WORD)lpbmInfoHdr->biHeight,
                    lpBits, (LPBITMAPINFO)lpbmInfoHdr, DIB_RGB_COLORS );

        if (hPal)
            {
            SelectPalette( hDC, hOldPal, FALSE );
            DeleteObject( hPal );
            }
        ReleaseDC( hDlg, hDC );
        FreeResource( hDIB );
        break;  // WM_INITDIALOG

    case WM_PAINT:
        BeginPaint( hDlg, &ps );
        GetClientRect( hDlg, &rect );
        hMemDC = CreateCompatibleDC( ps.hdc );
        if (hMemDC)
            {
            hOldBmp = (HBITMAP)SelectObject( hMemDC, hBmp );
            BitBlt( ps.hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY );
            SelectObject( hMemDC, hOldBmp );
            DeleteDC( hMemDC );
            }
        EndPaint( hDlg, &ps );
        break;

    case WM_SIZE:
        InvalidateRect( hDlg, NULL, FALSE );
        break;

    case WM_KILLFOCUS:
        if ((HWND)wParam == hDlg)
            break;
        // fall thru

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
        wParam = IDOK;
        // fall thru

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog( hDlg, wParam );
            break;

        default:
            break;
        }
        break;  // WM_COMMAND

    case WM_NCDESTROY:
        if (hBmp)
            {
            DeleteObject( hBmp );
            hBmp = NULL;
            }
        break;

    default:
        rtn = FALSE;
        break;
    }   // switch uMsg
    return rtn;
} // NavEggProc()


void LOCAL PASCAL CenterMain(HWND hWnd)
{
    auto        int             xPopup;
    auto        int             yPopup;
    auto        int             cxPopup;
    auto        int             cyPopup;
    auto        int             cxScreen;
    auto        int             cyScreen;
    auto        RECT            rcWindow;


                                        /* retrieve main display dimensions */
    cxScreen = GetSystemMetrics(SM_CXSCREEN);
    cyScreen = GetSystemMetrics(SM_CYSCREEN);

                                        /* retrieve popup rectangle */
    GetWindowRect(hWnd, (LPRECT) & rcWindow);

                                        /* calculate popup size */
    cxPopup = rcWindow.right - rcWindow.left;
    cyPopup = rcWindow.bottom - rcWindow.top;

                                        /* center within entire screen */
    xPopup = (cxScreen - cxPopup) / 2;
    yPopup = (cyScreen - cyPopup) / 2;

                                        /* move window to new location and display */
    MoveWindow(hWnd, (xPopup > 0) ? xPopup : 0, (yPopup > 0) ? yPopup : 0, cxPopup, cyPopup, TRUE);
}


//---------------------------------------------------------------------
//
// Function:   DIBNumColors
//
// Purpose:    Given a pointer to a DIB, returns a number of colors in
//             the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD DIBNumColors (LPTSTR lpbi)
{
   WORD wBitCount;


      // If this is a Windows style DIB, the number of colors in the
      //  color table can be less than the number of bits per pixel
      //  allows for (i.e. lpbi->biClrUsed can be set to some value).
      //  If this is the case, return the appropriate value.

   DWORD dwClrUsed;

   dwClrUsed = ((LPBITMAPINFOHEADER) lpbi)->biClrUsed;

   if (dwClrUsed)
      return (WORD) dwClrUsed;


      // Calculate the number of colors in the color table based on
      //  the number of bits per pixel for the DIB.

   wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;

   switch (wBitCount)
      {
      case 1:
         return 2;

      case 4:
         return 16;

      case 8:
         return 256;

      default:
         return 0;
      }
}


//---------------------------------------------------------------------
//
// Function:   PaletteSize
//
// Purpose:    Given a pointer to a DIB, returns number of bytes
//             in the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD PaletteSize (LPTSTR lpbi)
{
   return (DIBNumColors (lpbi) * sizeof (RGBQUAD));
}


//---------------------------------------------------------------------
//
// Function:   CreateDIBPalette
//
// Purpose:    Given a handle to a DIB, constructs a logical palette,
//             and returns a handle to this palette.
//
//             Stolen almost verbatim from ShowDIB.
//
// Parms:      hDIB == HANDLE to global memory with a DIB header
//                     (either BITMAPINFOHEADER or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

HPALETTE CreateDIBPalette (HANDLE hDIB)
{
   LPLOGPALETTE     lpPal;
   HANDLE           hLogPal;
   HPALETTE         hPal = NULL;
   int              i, wNumColors;
   LPTSTR           lpbi;
   LPBITMAPINFO     lpbmi;
   LPBITMAPCOREINFO lpbmc;

   if (!hDIB)
      return NULL;

   lpbi         = (LPTSTR) LockResource (hDIB);
   lpbmi        = (LPBITMAPINFO) lpbi;
   lpbmc        = (LPBITMAPCOREINFO) lpbi;
   wNumColors   = DIBNumColors (lpbi);

   if (wNumColors)
      {
      hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
                             sizeof (PALETTEENTRY) * wNumColors);

      if (!hLogPal)
         return NULL;

      lpPal = (LPLOGPALETTE) GlobalLock (hLogPal);

      lpPal->palVersion    = 0x300;
      lpPal->palNumEntries = wNumColors;

      for (i = 0;  i < wNumColors;  i++)
         {
         lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
         lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
         lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
         lpPal->palPalEntry[i].peFlags = 0;
         }

      hPal = CreatePalette (lpPal);
      GlobalFree   (hLogPal);
      }
   return hPal;
}   // CreateDIBPalette



/* **************************************************************************

@Name:
VOID LOCAL DisplayInfoDeskHelp (        // Just as the name sez.
    HWND    hWnd                        // [in] parent window handle
    );

@Description:
This function calls WinHelp() with the path to our info desk help file to
display the Symantec standard help.

@Parameters:
$hWnd$       [in] handle to our parent window

@Returns:       Nothing

************************************************************************** */

VOID LOCAL DisplayInfoDeskHelp (        // Just as the name sez.
    HWND    hWnd                        // [in] parent window handle
    )
{
    auto    TCHAR    szInfoPath [ SYM_MAX_PATH + 1 ] = { 0 };
    auto    TCHAR    szFileName [ SYM_MAX_PATH + 1 ] = { 0 };

    GetModuleFileName ( ghInst, szInfoPath, sizeof(szInfoPath) );
    NameStripFile ( szInfoPath );
    SYM_ASSERT ( STRLEN(szInfoPath) );

    LoadString ( ghInst, IDS_INFODESK, szFileName, sizeof(szFileName) );
    SYM_ASSERT ( *szFileName );

    NameAppendFile ( szInfoPath, szFileName );

    WinHelp ( hWnd, szInfoPath, HELP_FINDER, 0 );
}

//************************************************************************
// BuildScanListIndex()
//
// This routine builds ScanStart()'s new input - an array of SCANITEM's -
// from the old input - an SZZ of file specifications.
//
// The new input format is to support per-item subdirs flags as passed
// by NAVUI(NAV32.EXE). Old input routines remain unchanged, but call this
// routine for translation.
//
// Note that the old SZZ buffer must be maintained as the new data is
// an array of pointers into the old buffer.
//
// Parameters:
//      LPTSTR      lpszzScanList       [in] Old-format SZZ of file specs.
//      BOOL        bScanSubDirs        [in] Global scan subdirectories flag.
//
// Returns:
//      LPSCANITEM      On success - pointer to array of SCANITEM's. Array
//                          is terminated by NULL/Zero entry. Must be
//                          freed by caller with MemFreePtr() when done.
//      NULL            On error (bad input, or mem alloc fail)
//************************************************************************
// 04/01/1997 DALLEE, created.
//************************************************************************

LPSCANITEM BuildScanListIndex ( LPTSTR  lpszzScanList,
                                BOOL    bScanSubDirs )
{
    auto    LPSCANITEM  lpFirstItem;
    auto    LPSCANITEM  lpScanItem;
    auto    LPTSTR      lpszScanSpec;
    auto    int         nNumItems;

    SYM_ASSERT( lpszzScanList );
    SYM_ASSERT( EOS != *lpszzScanList );

    lpFirstItem = NULL;
    nNumItems   = 0;

                                        // Find number of items.
    for ( lpszScanSpec = lpszzScanList;
          EOS != *lpszScanSpec;
          lpszScanSpec = SzzFindNext( lpszScanSpec ) )
        {
            nNumItems++;
        }

                                        // Allocate array with empty
                                        // terminating entry.
    lpFirstItem = (LPSCANITEM)MemAllocPtr( GHND, (nNumItems + 1) * sizeof(SCANITEM) );

    if ( NULL != lpFirstItem )
        {
                                        // Fill in file specs and subdirs flag.
        for ( lpszScanSpec = lpszzScanList, lpScanItem = lpFirstItem;
              EOS != *lpszScanSpec;
              lpszScanSpec = SzzFindNext( lpszScanSpec ), lpScanItem++ )
            {
            lpScanItem->lpszPathName = lpszScanSpec;
            lpScanItem->bScanSubDirs = bScanSubDirs;
            }
        }

    return ( lpFirstItem );
} // BuildScanListIndex()


//************************************************************************
// SetupAPAgentStartGroupItem()
//
//
// Parameters:
//
// Returns:
//      nothing
//************************************************************************
// 7/8/97 DBUCHES function copied from NAVOPTS project.
//************************************************************************

void LOCAL PASCAL SetupAPAgentStartGroupItem()
{
    // Only valid for NT platforms.
    if ( SYM_SYSTEM_WIN_NT != SystemGetWindowsType() )
        return;

    char        szCommand[SI_MAXCOMMAND] ;
    char        szPath   [SI_MAXCOMMAND] ;
    char        szCmdLine[SI_MAXCOMMAND] ;
    char        szIconFile[SI_MAXCOMMAND] ;
    DWORD       dwType;
    DWORD       dwDataSize;
    HKEY        hKey ;
    long        lResult = 0L;
    TCHAR       szStartup[20] ;
    TCHAR       szNAVAP[50] ;
    int         verHi, verLow;
    DWORD       dwVersion;
    BOOL        bProgmanIconic = IsProgmanIconic();


    if ( navOptions.startup.bLoadVxD )
    {
        LoadString (ghInst, IDS_NAVOPTS_STR_STARTUP, szStartup, sizeof(szStartup));
        LoadString (ghInst, IDS_NAVOPTS_STR_NAVAP, szNAVAP, sizeof(szNAVAP));

        // Get the version number
        dwVersion = GetVersion();
        verHi  = LOBYTE(LOWORD(dwVersion));
        verLow = HIBYTE(LOWORD(dwVersion));

        if ( verHi < 4 )
        {
            // Delete AP from StartUp group (NT3.51)
            wsprintf ( szCommand, "[DeleteItem(\"%s\")]", szNAVAP ) ;
            bProgmanIconic ? RestoreProgman( ) : NULL;
            ProgmanGroupManipulation ( szStartup, szCommand, ACTION_DELETE_ITEM, GROUPTYPE_COMMON ) ;
            bProgmanIconic ? MinimizeProgman( ) : NULL;
        }

        // Add AP into StartUp group only if its not already present (NT4.0)
        if ( ( verHi < 4 ) ||
            ( (verHi == 4) && (verLow == 0) &&
            ( ITEM_NOTFOUND == ProgmanQueryItemInGroupNT4 (  szStartup, szNAVAP, GROUPTYPE_COMMON  ) ) ) )
        {
            if ( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, NAV_INSTALLKEY,  0, KEY_QUERY_VALUE, &hKey ) )
            {
                lResult = RegQueryValueEx( hKey, "InstallDir", 0, &dwType, NULL, &dwDataSize) ;
                if ( lResult == ERROR_SUCCESS && dwType == REG_SZ )
                {
                    RegQueryValueEx( hKey, "InstallDir", 0, &dwType, (LPBYTE)szPath, &dwDataSize) ;
                }
                else
                {
                    wsprintf ( szPath , NAV_DEFAULT_DIR ) ;
                }
                RegCloseKey( hKey ) ;
            }
            else
            {
                wsprintf ( szPath , NAV_DEFAULT_DIR ) ;
            }

            wsprintf ( szCmdLine, "%s\\%s", szPath, NAV_AP_EXE ) ;
            wsprintf ( szIconFile, "%s\\%s", szPath, NAV_AP_EXE ) ;

            // for NT4.0
            if (verHi >= 4)
            {
                ProgmanGroupAddItemNT4 ( szStartup, szNAVAP, szCmdLine, " ",
                                            szIconFile, 0, szPath, GROUPTYPE_COMMON  ) ;
            }
            else
            {

                SIWQuoteLFN(szCmdLine,SI_MAXCOMMAND - 1);
                SIWQuoteLFN(szPath,SI_MAXCOMMAND - 1);
                GetShortPathName(szIconFile,szIconFile,SI_MAXCOMMAND - 1);

                wsprintf ( szCommand, "[AddItem(%s,\"%s\",%s,%d,-1,-1,%s)]", szCmdLine,
                                        szNAVAP, szIconFile , 0, szPath ) ;
                bProgmanIconic ? RestoreProgman( ) : NULL;
                ProgmanGroupManipulation ( szStartup, szCommand, ACTION_ADD_ITEM, GROUPTYPE_COMMON ) ;
                bProgmanIconic ? MinimizeProgman( ) : NULL;
            }
        }
    }
    else
    {
        LoadString (ghInst, IDS_NAVOPTS_STR_STARTUP, szStartup, sizeof(szStartup));
        LoadString (ghInst, IDS_NAVOPTS_STR_NAVAP, szNAVAP, sizeof(szNAVAP));

        // Get the version number
        dwVersion = GetVersion();
        verHi  = LOBYTE(LOWORD(dwVersion));
        verLow = HIBYTE(LOWORD(dwVersion));

        if ( (verHi == 4) && (verLow == 0) )
        {
            ProgmanGroupManipulationNT4 ( szStartup, szNAVAP, ACTION_DELETE_ITEM, GROUPTYPE_COMMON ) ;
        }
        else
        {
            // Delete AP from StartUp group
            wsprintf ( szCommand, "[DeleteItem(\"%s\")]", szNAVAP ) ;
            bProgmanIconic ? RestoreProgman( ) : NULL;
            ProgmanGroupManipulation ( szStartup, szCommand, ACTION_DELETE_ITEM, GROUPTYPE_COMMON ) ;
            bProgmanIconic ? MinimizeProgman( ) : NULL;
        }
    }
}


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*                                                                      */
/* Params:                                                              */
/*                                                                      */
/* Return values:                                                       */
/*  Success/Failure                                                     */
/*                                                                      */
/************************************************************************/
BOOL LOCAL SIWQuoteLFN(LPTSTR pathName, int sizePathName)
{
	BOOL ret = TRUE;
	char   temp[SI_MAXCOMMAND];

	if (*pathName != '\0' && SIWSpacesPresent(pathName) && *pathName != '"')
	{
		if (lstrlen(pathName) + 2 <= sizePathName)
		{
			lstrcpy(temp, pathName);
			lstrcpy(pathName, "\"");
			lstrcat(pathName, temp);
			lstrcat(pathName, "\"");
		}
		else
			ret = FALSE;
	}
	return ret;

}

/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*                                                                      */
/* Params:                                                              */
/*                                                                      */
/* Return values:                                                       */
/*  Success/Failure                                                     */
/*                                                                      */
/************************************************************************/
BOOL LOCAL SIWSpacesPresent(LPTSTR pathName)
{
	BOOL ret = FALSE;
	while (*pathName != '\0')
	{
		if (*pathName == ' ')
		{
			ret = TRUE;
			break;
		}
		pathName = AnsiNext(pathName);
	}
	return ret;
}

