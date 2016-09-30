// Copyright 1993-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_scan.cpv   1.36   01 Dec 1998 20:00:04   CEATON  $
//
// Description:
//      NAV Windows file scanning functions.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_scan.cpv  $
// 
//    Rev 1.36   01 Dec 1998 20:00:04   CEATON
// Added the extension .IMZ to the list of recognized .ZIP extension names (per
// IBM).
// 
// 
//    Rev 1.35   28 Oct 1998 18:17:56   CEATON
// Removed previous fix (redundant) due to fix in CEnumFiles class.
// 
// 
//    Rev 1.34   26 Oct 1998 16:24:20   CEATON
// Removed unreferenced variable from ScanProc().
// 
// 
//    Rev 1.33   22 Oct 1998 16:10:18   CEATON
// Fixed problem where Windows 95 returns error code 18 when attempting to enumurate
// files in a directory without sufficient rights.
// 
//    Rev 1.32   06 Oct 1998 14:28:24   tcashin
// In HandleAccessError(), test the bLogAccessDenied flag in
// the extended options structure.
// 
//    Rev 1.31   01 Oct 1998 17:28:58   tcashin
// Remove the check for the access denied flag in the activity log options.
// 
//    Rev 1.30   22 Sep 1998 22:06:52   tcashin
// Handle and log access-denied errors.
// 
//    Rev 1.29   02 Sep 1998 12:06:36   rchinta
// Checking in David Allee's fix for STS #119333
// 
//    Rev 1.28   28 Jul 1998 19:27:28   tcashin
// Changes to IfZipTheScanZip(). Find the pointer to the current InfArr before
// processing the compressed file then pass it to DecideWhatToDoAfterProcZip().
// 
//    Rev 1.27   21 Jul 1998 13:58:04   tcashin
// Save the number of infected items before the decomposer is called. If any
// infected items were added, then pass the starting index into infarr[] to
// DecideWhatToDoAfterProcessingZip().
// 
//    Rev 1.26   01 Jul 1998 16:32:54   tcashin
// Call DecideWhatToDoAfterProcessingZip() when the decomposer is done.
// 
//    Rev 1.25   20 May 1998 15:09:22   tcashin
// Make sure file APIs are OEM before the call to VirusScanFile().
// 
//    Rev 1.24   19 May 1998 07:06:34   tcashin
// Replaced a call to NameReturnShortName() with GetShortPathName() under NT.
// 
//    Rev 1.23   13 May 1998 20:20:14   DBuches
// Fixed chunky animation.
//
//    Rev 1.22   13 May 1998 12:41:24   MKEATIN
// If pgDecomposer is NULL the decomposer failed to initialize.  We now
// check for this before calling the decomposer.
//
//    Rev 1.21   06 May 1998 14:51:48   DBuches
// Added ability to start and stop animation.
//
//    Rev 1.20   22 Apr 1998 16:18:40   MKEATIN
// Add UUE to compressed file extentions.
//
//    Rev 1.19   21 Apr 1998 15:31:14   MKEATIN
// Added CAB scanning support.
//
//    Rev 1.18   06 Apr 1998 15:22:26   DBuches
// Changes for use of animation common control.
//
//    Rev 1.17   30 Mar 1998 17:31:42   MKEATIN
// Made Alpha compile time changes.
//
//    Rev 1.16   16 Mar 1998 18:58:44   MKEATIN
// Replaced BVZip integration with Decomposer integration.
//
//    Rev 1.15   17 Feb 1998 14:29:38   MKEATIN
// Roswell Port
//
//    Rev 1.15   16 Jan 1998 14:10:26   MDUNN
// Redid code that saves the last-accessed time of files.  Everything's now
// in ScanProc().  Added two functions to do the work - SaveLastAccessTime()
// and RestoreLastAccessTime().
//
//    Rev 1.14   05 Jan 1998 17:58:32   MDUNN
// Fixed IsZipFormat() so it preserves the last-accessed date of the file
// it's examining.
//
//    Rev 1.13   02 Sep 1997 18:15:58   DALLEE
// In HandleFileFound(), using lpNav->Infected.szFullOemPath() when
// calling InList(). ValidateAndSaveFileInfo() ensures we have this
// will be a valid name ANSI/OEM-wise. We'd prefer to use the LFN since
// scanning on Netware volumes will fail using SFN's with the MS Netware
// client. (InList() launches the Zip scanning w/ the filename we pass it).
//
//
//    Rev 1.12   17 Aug 1997 18:00:10   DBUCHES
// Removed unneccessary MessageBeep() call when custom response is enabled.
//
//    Rev 1.11   15 Aug 1997 17:35:42   DSACKIN
// Moved the NetAlert function call so that we can actually tell if any
// automatic (or from prompt) actions were taken instead of "No Action Taken"
//
//    Rev 1.10   15 Aug 1997 11:00:56   mdunn
// ScanTheFile() now calls NavNameIsReservedName() to check for DOS reserved
// device names before trying to scan.  Fixes defect #86464
//
//    Rev 1.9   11 Aug 1997 01:01:34   KSACKIN
// Now make a call to NavZipInit() and NavZipCleanup() contained in N_Zip.cpp.
// This is one level of indirection so that we can dynamically load the DLL
// transparently to the caller.  We keep track of this in the n_Zip.cpp file.
//
//    Rev 1.8   03 Jul 1997 15:02:20   TIVANOV
// fix custom notify responce
//
//    Rev 1.7   19 Jun 1997 09:45:36   BGERHAR
// Fix ZIP inclusion logic to keep LHA working
//
//    Rev 1.6   18 Jun 1997 20:51:22   BGERHAR
// Speed up program files scans (reduced ZIP scanning) but keep full ZIP for full scans
//
//    Rev 1.5   28 May 1997 22:30:04   DALLEE
// BIG oops! last rev: was returning unitialized bContinue from stack
// in HandleFileFound(). Was exiting scan loop prematurely.
// Now returns lpNav->bDoScan.
//
//    Rev 1.4   13 May 1997 18:15:14   DALLEE
// Replaced old file enumerator CFileAll with CEnumFiles.
// Consequently re-did much of FindAllFiles() and HandleFileFound().
// Broke directory tracking code into new func HandleDirectoryFound().
// Save full long and short paths for current file scanned to speed up
// exclusion checks.
// Moved exclude check prior to scanning.
//
//
//    Rev 1.3   24 Apr 1997 11:04:48   JBRENNA
// Add "s_" to LoadManagementInfo and bUsingLoadManagement.
// Tesla port:
// JMILLAR r1.2: fix handle leak in IsZipFormat.
//
// TSmith r1.1: Modified call to NetAlert in ScanTheFile to pass an
//   LPZIPDETECT structure in addition to the NETALERT struct so that when
//   scanning zip files, the name of the compressed file can be put into the
//   alert packet.
//
//    Rev 1.2   07 Apr 1997 18:45:10   DALLEE
// Re-worked file scan loop to accept new indexed input w/ SCANITEMs which
// include per-item scan subdirectories flags.
//
//
//    Rev 1.1   25 Feb 1997 12:32:50   DALLEE
// Removed unused headers: stack.h, navanim.h, navgraph.h
// Removed ancient log comments ('bout 100 lines of junk)
// Port from HYDRA.15
//            Rev 1.86   09 Oct 1996 12:04:32   CCOTRIL
//         Re-enable network alerting for W32.
//
//    Rev 1.0   06 Feb 1997 21:02:54   RFULLER
// Initial revision
//
//    Rev 1.32   18 Nov 1996 20:27:20   JBRENNA
// STS #74684 - Fix InList() to restore the lpNav->Infected.szFullPath and
// lpNav->Infected.szFullOemPath properly after a call to IfZipThenScanZip().
// The IfZip...() call blows away the infected paths when doing its scan.
// Previously, we were restoring the path with szSaveFileName. This does NOT
// provide the path information for the filename. As a result, Inoculate was
// breaking.
//
//    Rev 1.31   17 Oct 1996 15:37:04   JBRENNA
// Change ANSISwitchTo() to pass an ANSI filename to NamereturnShortName().
// Previously an OEM string was passed. This caused problems for directories
// with Alt+0160, 0164, 0166, and 0182 ansi characters.
//
//    Rev 1.30   13 Sep 1996 14:16:20   JBRENNA
// If a SFN is passed to HandleFileFound(), use that instead of the LFN
// when calling InList(). This ensures that ANSI/OEM problem files are
// correctly processed. Previously, only the LFN was given to InList().
// This cause the InList() (which scans .zip files) to skip over a badly
// named .zip file.
//
//    Rev 1.29   11 Sep 1996 12:07:52   DJesch
// Fixed a problem with untranslatable characters in a file selected from the
// Scan/File dialog (STS #61732).
//
//    Rev 1.28   10 Sep 1996 19:10:32   DJesch
// Fixed STS $64597:  Scan only those files specified in the "Program Files"
// configuration options instead of all files within a .ZIP.
//
//    Rev 1.27   09 Sep 1996 16:01:30   JBRENNA
// 1. In ScanProc(), only strip off the filename before a
// NameReturnShortName() call when the filename has wildcards. The "Name" func
// does not deal with fildcards well.
// 2. We were hard coding the "*?" string for ANSI chars that would not
// translate to OEM. On WinNT, these non-translating characters wind up as
// either "?" or "*" in OEM. On Win95, the ANSI character becomes "_" in
// OEM. Now, we use "*?" for WinNT and "_" for Win95 ... determined at
// run-time.
// 3. Cleaned up ANSISwitchTo(). It was manually parsing the file spec input
// parameter when there are "Name" functions to do that. Now, we use the
// "Name" functions making it simplier to read and understand.
//
//    Rev 1.26   30 Aug 1996 16:13:14   DALLEE
// Enabled internet download code by removing #ifdef NAVSCANW's.
//
//    Rev 1.25   19 Aug 1996 15:38:48   GWESTER
// Modified NAVSCANW #ifdefs for Kirin.
//
//    Rev 1.24   19 Aug 1996 14:46:22   GWESTER
// #ifdef'ed more Load Management stuff.
//
//    Rev 1.23   19 Aug 1996 14:25:46   GWESTER
// Wrapped up calls to Server Load Management API's in an NT #ifdef.
//
//    Rev 1.22   19 Aug 1996 14:03:04   GWESTER
// For Kirin, removed the "#ifdef NAVSCANW" statements around download
// and "GloballySaved_..." code, just like in LuigiPlus. This code will
// now get compiled whether you're compiling for Win95 or NT.
//
//    Rev 1.21   07 Aug 1996 15:43:46   JBRENNA
// LuigiPlus Port:
//  YVANG: Rework for Rev 1.83 to use format buffer from stack. (STS#60477)
//
//  YVANG:
//    Added 'AnsiToOem()' to convert loaded resource strings which are in ANSI
//    format into OEM format.  (The string will be converted later to ANSI
//    before display.) (STS#60477)
//
//    Rev 1.20   05 Aug 1996 23:16:50   DJesch
// Fixed STS #63095: problem with finding ZIPs in folders.
//
//    Rev 1.18   19 Jul 1996 15:25:18   djesch
// Fixed STS 60707 (again). It will now scan .ZIP files properly from
// the command line -- scanning all the contents of .ZIPs instead of
// just the files that match the command line filespec.
// Also, if the "Scan Compressed Files" checkbox is set, it will always
// scan compressed file even if they do not match a command line
// filespec.
//
//    Rev 1.17   11 Jul 1996 14:31:30   RHAYES
// Partial fix for STS defect # 58334. This allows ZIP files that
// contain individual password protected files to continue scanning
// in case other non-protected files exist.
//
//    Rev 1.16   08 Jul 1996 14:41:16   DJESCH
// NameReturnShortName() returned with a full file name instead of a
// partial filespec.  I stripped off the file name before calling this
// function to solve both STS 52909 and 52910.
//
//    Rev 1.15   02 Jul 1996 20:02:12   DJESCH
// Fixed another problem with scanning for .ZIP files.
//
//    Rev 1.14   28 Jun 1996 16:16:52   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.13   24 Jun 1996 21:31:24   RHAYES
// Add.'l OEM<->Ansi mod.'s (new and from Luigi).
// Fix for #53511.
//
//    Rev 1.12   24 Jun 1996 11:21:40   DJESCH
// Fixed problem with renamed ZIP files (STS 57128) and displaying
// the name of an infected file within a ZIP (STS 58831).  Also, we
// are now properly scanning ZIP2EXE files as ZIP files.
//
//    Rev 1.11   11 Jun 1996 12:10:24   RHAYES
// Fixes for DBCS, OEM, and Zip file scanning issues while
// in Japanese NT. In particular, this fixes STS defects
// 56415, 55141, 56204, 56422, 55538.
//
//    Rev 1.10   05 Jun 1996 15:53:22   RHAYES
// Yet even more OEM<->Ansi mod.'s to fix activity log and
// other display related issues.
//
//    Rev 1.9   29 May 1996 12:41:40   RHAYES
// Merged additional OEM<->Ansi Luigi mod.'s. Correction in FileInExeList() -
// removed prepending extensions w/ "*.".
//
//    Rev 1.8   22 May 1996 17:44:10   RHAYES
// Round # 2 of Dave Allee's OEM<->Ansi mod.'s in Luigi.
//
//    Rev 1.7   21 May 1996 15:08:04   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
//
//    Rev 1.6   15 May 1996 19:53:14   RHAYES
// Convert single file names to OEM before scanning.
// Fixes STS defect # 53516.
//
//    Rev 1.4   25 Apr 1996 15:09:22   TSmith
// Added code to 'YieldToWindows' that insures that the
// load management data only gets updated once per
// second. If it gets updated more often than that, the
// load management calculations behave incorrectly.
//
//    Rev 1.3   23 Apr 1996 22:12:04   TSmith
// Added cpu load management support. Modified 'YieldToWindows'
// conditionally use load management or message loop.
//
//    Rev 1.2   04 Apr 1996 16:44:02   YVANG
// Ported from Kirin.
//
//    Rev 1.2   03 Apr 1996 14:52:48   YVANG
// DBCS enable.
//
//    Rev 1.1   14 Feb 1996 17:03:48   JWORDEN
// Process internal file names in OEM mode to allow scan, and repair, and
// innoculation of file names containing certain non-ANSI translatable
// characters.
//
//    Rev 1.0   30 Jan 1996 15:55:56   BARRY
// Initial revision.
//
//    Rev 1.63   29 Jan 1996 23:53:54   GWESTER
// In download mode, moved check of whether or not this is a zip file. This
// corrected the problem of 0-file zips.
//************************************************************************

// Processor directives for local code
#define INOC_SCAN_FIREWALL  1

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "disk.h"
#include "symcfg.h"
#include "symnet.h"
#include "netobj.h"
#include "nonav.h"
#include "navutil.h"
#include "exclude.h"
#include "options.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "cmbstr.h"
#include "security.h"
#include "navwork.h"
#include "navwzip.h"
#include "syminteg.h"

#include "stub.h"
#include "inoc.h"
#include "virscan.h"
#include "enumfile.h"
#include "fname.h"
#include "n_decomp.h"
#include "navoptx.h"

#include "dbcs.h"

#include "netalert.h"

#ifdef SYM_WIN32
    #include "navstat.h"
    #include "symstat.h"
    #include "navver.h"
    #include "loadmgmt.h"
#endif

extern HINSTANCE hInstance;

//*****************************************************************************
// Global value to hold the extension list removed from the NAVOPTS structure
//*****************************************************************************
extern char gszExtensions[MAX_EXTENSION_LIST_SIZE+1];

#define DRIVE_UNC_NAME  12345

#ifndef NON_TRANS_OEM_CHARS_95
    // These characters appear in a OEM filename when the original ANSI character
    // cannot be translated to OEM. On Win95, the ANSI character becomes "_" in
    // OEM. On WinNT, the ANSI character becomes either "*" or "?" in OEM.
    #define NON_TRANS_OEM_CHARS_95    (_T("_"))
    #define NON_TRANS_OEM_CHARS_NT    (_T("*?"))
#endif

#ifdef SYM_DEBUG
    #define DEBUG_PROGRESS(a,b) OutputDebugString ( a ); \
    OutputDebugString ( " " ); OutputDebugString ( b ); OutputDebugString ( "\n" );
#else
    #define DEBUG_PROGRESS(a,b)
#endif

// These are for the local function HandleAccessError()
#define ACCESS_DENIED_DIR   1
#define ACCESS_DENIED_FILE  2

// **************************************************** //
// ****              Local Prototypes              **** //
// **************************************************** //

                                        // New find all start point (with
                                        // a non-bogus name).
VOID LOCAL FindAllFiles (
    LPNONAV           lpNav,
    BOOL              bScanSubs
    );

BOOL LOCAL HandleDirectoryFound (
    LPNONAV     lpNav,
    CEnumFiles  *pEnumFiles,
    BOOL        bScanSubs
    );

BOOL LOCAL HandleFileFound (
    LPNONAV    lpNav,
    CEnumFiles *pEnumFiles
    );

BOOL LOCAL InList (
    LPNONAV     lpNav,
    LPCTSTR     lpFileName
    );

BOOL LOCAL DiskInDrive (
    HWND        hDlg,
    BYTE        byDrive,
    UINT        uTimeOut
    );

DWORD LOCAL GetFileSize (
    LPSTR       lpFileName
    );

VOID LOCAL PASCAL GetClusterSize(       // Gets the cluster size
    LPNONAV     lpNav,                  // NAV pointer
    LPSTR       lpPath                  // path
    );

BOOL LOCAL ANSISwitchTo(                // Changes drive directory
    HWND    hDlg,                       // Parent handle for access error dlgs.
    LPSTR   lpAnsiFileSpec,             // Dir/File spec to scan (in ANSI)
    LPSTR   lpDir,                      // [out] Gets directory we're scanning.
    LPNONAV lpNav                       // [out] Gets wildcard mask if any.
    );

BOOL LOCAL IfZipThenScanZip (
    LPNONAV     lpNav,
    LPSTR       lpFileName
    );

VOID PASCAL ScanAnimationEnable (
    HWND        hDlg
    );

VOID PASCAL ScanAnimationDisable (
    HWND        hDlg
    );

struct LASTACCESSTIMEINFO
    {
    LPCTSTR  szFilespec;
    BOOL     bMustChangeAttrs;
    BOOL     bLastAccessTimeSaved;
    FILETIME ftimeLastAccessTime;
    UINT     uFileAttrs;
    LPNONAV  pNoNav;
    UINT     uNumCleanedPre;
    };

void SaveLastAccessTime ( LPNONAV pNoNav,
                          LPCTSTR szFilespec,
                          LASTACCESSTIMEINFO* prTimeInfo );

void RestoreLastAccessTime ( const LASTACCESSTIMEINFO* prTimeInfo );

BOOL LOCAL HandleAccessError ( LPNONAV lpNav, UINT uType );

// **************************************************** //
// ****              Local Data                    **** //
// **************************************************** //

#ifdef NAVWNT
static LOADMANAGEMENT  s_LoadManagementInfo;
static BOOL            s_bUsingLoadManagement = FALSE;
#endif

//************************************************************************
// ScanProc()
//
// (ANSI/OEM mess: internal work in OEM, except Inoc is ANSI)
//
// Parameters:
//      HWND        hDlg                [in] Parent UI dialog
//      LPNONAV     lpNav               [in] Pointer to NAV struct
//
// Returns:
//************************************************************************
// 04/01/91 Martin   Function created
//************************************************************************

UINT SYM_EXPORT WINAPI ScanProc(HWND hDlg, LPNONAV lpNav)
{
    auto    NETALERT        Alert;
    auto    LPSCANITEM      lpScanItem;
    auto    LPSTR           lpScanList;
    auto    BOOL            bIsFile = FALSE;
    auto    char            szDirectory [ SYM_MAX_PATH+1 ];
    auto    BYTE            byDrive;
    auto    char            szFilePath [ SYM_MAX_PATH+1 ];
    static  char            szDriveSpec [ SYM_MAX_PATH+1 ];     //didn't want to eat stack (night before GM)
    auto    char            szOemFileSpec [ SYM_MAX_PATH+1 ];
#ifdef SYM_WIN32
    auto    BOOL            bFileApisAnsi;

    bFileApisAnsi = AreFileApisANSI();

    if (bFileApisAnsi)
        SetFileApisToOEM ();
#endif

    lpNav->hMommyWindow = hDlg;

#ifdef NAVWNT
    //
    // Valid values for load management are 1 - 99,
    // anything else turns it off...
    //
    if ( lpNav->lpScan->uCpuLoad <= 0 || lpNav->lpScan->uCpuLoad >= 100 )
        {
        s_bUsingLoadManagement = FALSE;
        }
    else
        {
        s_bUsingLoadManagement = InitServerLoadManagement( &s_LoadManagementInfo,
                                                           lpNav->lpScan->uCpuLoad );
        }
#endif

#ifndef NAVSCANW                        // &? No inoc in Gargoyle
    lpNav->uInocOpenOK = SMARTOPEN_CLOSED;

    if (lpNav->Inoc.bUseInoc == TRUE &&
        (lpNav->Inoc.uActionNewInoc == INOC_AUTO ||
         lpNav->Inoc.uActionNewInoc == INOC_AUTO_FILEONLY))
        {
        if (InocPasswordVerify (hDlg, lpNav) == FALSE)
            lpNav->Inoc.uActionNewInoc = INOC_REPORT;
        }
#endif

    byDrive = DiskGet();                // Save current drive

    lpScanItem = lpNav->lpScan->lpScanItems;
    lpScanList = lpScanItem->lpszPathName;

    // ************************ //
    // Start timer for scanning //
    // ************************ //

    DiskReset();

#ifndef NAVSCANW                          // &? No inoc in Gargoyle
#ifdef SYM_WIN32
    SetFileApisToANSI();
#endif
    SmartInocClose();
#ifdef SYM_WIN32
    SetFileApisToOEM();
#endif
#endif
    ScanTime(hDlg, TRUE);

    while (lpScanList)
        {
        lpNav->lpScan->bScanSubs = lpScanItem->bScanSubDirs;

#ifdef SYM_WIN32
                                        // -----------------------------------
                                        // Convert the scan target to OEM
                                        // -----------------------------------
        NavAnsiToOem (lpScanList, szOemFileSpec);
#endif

                                        // -----------------------------------
                                        // Check if drive is available. If not
                                        // increment pointer and process next.
                                        // -----------------------------------
        if ( ScanDriveSkipped ( szOemFileSpec ) )
            {
            lpScanItem++;
            lpScanList = lpScanItem->lpszPathName;

            continue;
            }

                                        // -----------------------------------
                                        // Drive is available, so save current
                                        // working directory.
                                        // -----------------------------------
        TSR_OFF;

        if ( DirGet (*szOemFileSpec , szDirectory) != NOERR )
            *szDirectory = '\0' ;

                                        // -----------------------------------
                                        // Check if it is a file we're to scan
                                        // -----------------------------------
        if (!NameIsRoot(szOemFileSpec))
            {
            bIsFile = FileExists (szOemFileSpec);

            if (!bIsFile && !NameIsWildcard(szOemFileSpec))
                {                       // Check for non ANSI to OEM
                                        // translatable LFN...

                SET_FILE_APIS_TO_ANSI;

                NameReturnShortName(lpScanList, szFilePath);

                SET_FILE_APIS_TO_OEM;

#ifdef SYM_WIN32
                NavAnsiToOem(szFilePath, szFilePath);
#endif
                bIsFile = FileExists(szFilePath);

                if (bIsFile)
                    {
                    STRCPY(szOemFileSpec, szFilePath);
                    }
                }
            }
        else
            {
            bIsFile = FALSE;
            }
                                        // -----------------------------------
                                        // Open inocdb each time on drive we
                                        // are scanning.
                                        // -----------------------------------
#ifndef NAVSCANW                          // &? No inoc in Gargoyle
        if (lpNav->Inoc.bUseInoc == TRUE)
            {
#ifdef SYM_WIN32
            SetFileApisToANSI();
#endif
            if (*lpNav->Inoc.szNetInocPath)
                {
                                        // reset inoc path

                InocInit ( lpNav->lpScan->lpNavOptions );
                szFilePath[0] = '\0';
                NameReturnRoot ( lpScanList, szFilePath );
                if ( NameHasDriveSpec (lpScanList) &&
                     (lpScanList[2] == '.') )
                    {
                    STRNCPY ( szDriveSpec, lpScanList, 2 );
                    szDriveSpec[2] = '\\';
                    szDriveSpec[3] = EOS;
                    }
                else
                    {
                    STRCPY ( szDriveSpec, szFilePath );
                    }

                STRCPY(szFilePath, lpScanList);

                switch (GetDriveType(szDriveSpec))
                    {
                    case DRIVE_REMOVABLE:
                         if ( !lpNav->Inoc.bInocFloppies )
                             break;
                    case DRIVE_UNC_NAME:
                    case DRIVE_REMOTE:
                    case DRIVE_FIXED:
                         if (lpNav->Inoc.uActionNewInoc == INOC_AUTO ||
                             lpNav->Inoc.uActionNewInoc == INOC_AUTO_FILEONLY ||
                             bIsFile == TRUE )
                             {
                             lpNav->uInocOpenOK = SmartInocOpen (
                                                             szFilePath,
                                                             TRUE
                                                             );

                             if (lpNav->uInocOpenOK == SMARTOPEN_ERROR)
                                 {
                                 lpNav->uInocOpenOK = SmartInocOpen (
                                                             szFilePath,
                                                             FALSE
                                                             );
                                 }
                             }
                         else
                             lpNav->uInocOpenOK = SmartInocOpen (
                                                          szFilePath,
                                                          FALSE );
                         break;
                    default:
                         break;
                    }

                }
#ifdef SYM_WIN32
            SetFileApisToOEM();
#endif
            }
#endif  // !NAVSCANW

        TSR_ON;

        if ( bIsFile == TRUE )
            {
            LASTACCESSTIMEINFO rTimeInfo;

            SaveLastAccessTime ( lpNav, szOemFileSpec, &rTimeInfo );

            TSR_OFF;

            ValidateAndSaveFileInfo( lpNav,
                                     szOemFileSpec,
                                     lpScanList,
                                     TRUE );

            lpNav->szInfectedLFN[0] = \
            lpNav->szInfectedSFN[0] = EOS;
            NameReturnLongName( lpNav->Infected.szFullOemPath, lpNav->szInfectedLFN, FALSE );

        //  NameReturnShortName() is returning the wrong thing under NT!
            if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
                GetShortPathName(lpNav->Infected.szFullOemPath, lpNav->szInfectedSFN, sizeof(lpNav->szInfectedSFN));
            else
                NameReturnShortName(lpNav->Infected.szFullOemPath, lpNav->szInfectedSFN);

            GetClusterSize(lpNav, lpNav->Infected.szFullOemPath);

            NameReturnPath (lpNav->Infected.szFullOemPath, szFilePath );

                                        // If this file name contains non
                                        // convertible characters use the
                                        // short name
            if (NavTestAnsiToOem (lpScanList))
                STRCPY ( lpNav->szTheDirectory, szFilePath );
            else
                if ( NameReturnLongName ( szFilePath, lpNav->szTheDirectory,
                    FALSE ) != NOERR )
                    {
                    STRCPY ( lpNav->szTheDirectory, szFilePath );
                    }

            UpdateDir ( hDlg, lpNav->Infected.szFullPath, FALSE);

            if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
                {
                                        // When in viewer mode, we may need
                                        // to save the file when done!
                STRCPY(lpNav->lpScan->szSaveFileName,
                       lpNav->Infected.szFullPath);
                }


                                        // ------------------------------------
                                        // Need to add slack space to total
                                        // file space
                                        // ------------------------------------

            lpNav->dwScannedKBytes += ComputeKilobytesScanned (GetFileSize(szOemFileSpec),
                lpNav->dwClusterSize );

            TSR_ON;
                                    
                                        // -----------------------------------
                                        // Check for archive and scan members.
                                        // Then scan file, always.
                                        // -----------------------------------

            IfZipThenScanZip (lpNav, szOemFileSpec );

                                        // -----------------------------------
                                        // First scan the file.
                                        // Then (BANKERS) check registration.
                                        // Otherwise, check inoculation.
                                        // -----------------------------------
            if ( !ScanTheFile(lpNav) )
                {
#ifndef NAVSCANW          // No inoculation in Gargoyle
                                        // -----------------------------------
                                        // Do inoculation if program file.
                                        // -----------------------------------

                if ( (lpNav->uInocOpenOK == SMARTOPEN_RW ||
                      lpNav->uInocOpenOK == SMARTOPEN_RO)
                        &&
                     FileInExeList( gszExtensions, szOemFileSpec, 
                                    lpNav->ExtendedOpts.bScanNoExtFile ) )
                    {
                    FileInoculated ( lpNav );
                    }
#endif
                }

            RestoreLastAccessTime ( &rTimeInfo );
            }
        else
            {
            if (ANSISwitchTo(hDlg, lpScanList, lpNav->szTheDirectory, lpNav))
                {
                GetClusterSize(lpNav, lpNav->szTheDirectory);

                                        // Hah! Finally nuked the bogus
                                        // local FIL4_FIND_ALL.
                                        // -DALLEE 05/15/96
                FindAllFiles(lpNav, lpNav->lpScan->bScanSubs);
                }
            }

        lpScanItem++;
        lpScanList = lpScanItem->lpszPathName;

#ifndef NAVSCANW                          // &? No inoc in Gargoyle
        SetFileApisToANSI();
        SmartInocClose();
        SetFileApisToOEM();
#endif
        if ( *szDirectory )
            DirSet(szDirectory);        // Restore directory

        if ( lpNav->bDoScan == FALSE )
            break;
        }

    lpNav->uInocOpenOK = SMARTOPEN_CLOSED;

    // ********************** //
    // End timer for scanning //
    // ********************** //
    ScanTime(hDlg, FALSE);

    Alert.lpNet          = lpNav->lpNetUserItem;
    Alert.lpInfected     = &lpNav->Infected;
    Alert.lpAlertOpt     = &lpNav->Alert;
    Alert.uTypeOfEvent   = EV_ALERT_END_SCAN;
    NetAlert (&Alert);

    DiskSet(byDrive);               // Restore original drive


#ifdef SYM_WIN32
    if (bFileApisAnsi)
        SetFileApisToANSI();
#endif

#ifdef NAVWNT
    if ( s_bUsingLoadManagement )
		//
		// Free load management structures & close
		// Windows NT performance counters
		//
        DeInitServerLoadManagement( &s_LoadManagementInfo );
#endif

    return ((UINT)bIsFile);
}


//************************************************************************
// GetClusterSize()
//
// Fills in the cluster size for the drive, directory, or file passed.
//
// Parameters:
//      LPNONAV     lpNav               [out] Pointer to NAV struct
//      LPSTR       lpPath              [in] Drive to get cluster size of
//
// Returns:
//      Fills in the dwClusterSize member of the lpNav struct
//************************************************************************
// 04/01/91 Martin   Function created
//************************************************************************

VOID LOCAL PASCAL GetClusterSize(LPNONAV lpNav, LPSTR lpPath)
{
    auto        BYTE            byDrive;

    TSR_OFF;

    byDrive = (lpPath[1] == ':') ? lpPath[0] : DiskGet();
    lpNav->dwClusterSize = DiskGetClusterSize(byDrive);
    if (0 == lpNav->dwClusterSize ||
        -1 == lpNav->dwClusterSize)
        lpNav->dwClusterSize = 4096;

    TSR_ON;
}

//************************************************************************
// FindAllFiles()
//
// Starts off the file/dir enumeration.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 05/13/1997 DALLEE, replace CFindAll iterator with CEnumFiles to remove
//      funky callback scheme and make full LFN and SFN paths available.
//************************************************************************

VOID LOCAL FindAllFiles ( LPNONAV   lpNav,
                          BOOL      bScanSubs )
{
    SYM_ASSERT ( lpNav );

    auto    TCHAR       szFileSpec [SYM_MAX_PATH+1];
    auto    CEnumFiles  enumFiles;

    auto    int         nNextResult;
    auto    int         nDirNesting;


    // Initialize locals.

    nDirNesting = 0;

    // Get file specification to scan.

    STRCPY( szFileSpec, lpNav->szTheDirectory );

    // Tack on the wildcard filespecification if we have one.
    // Otherwise, scan all files.

    if ( *lpNav->lpScan->szScanFileSpec )
    {
        NameAppendFile( szFileSpec, lpNav->lpScan->szScanFileSpec );
    }
    else
    {
        NameAppendFile( szFileSpec, _T("*") );
    }

    // Initialize file enumerator.

    if ( NOERROR != enumFiles.Init( szFileSpec, SYM_MAX_PATH ) )
    {
        return;
    }

    // Enumerate all files matching our spec.

    for (;;)
    {
        YieldToWindows( lpNav->hMommyWindow );

        if ( FALSE == lpNav->bDoScan )
            {
            // User pressed Stop, ESC, etc. to end scan.

            break;
            }

        nNextResult = enumFiles.Next();

        if ( ERROR_INSUFFICIENT_BUFFER == nNextResult )
        {
            // Filename is too long for our buffers.
            // Silently skip and continue to next.

            continue;
        }
        else if ( NOERROR != nNextResult )
        {
            // End of files (ERROR_NO_MORE_FILES) or something fatal.
            
            if ( ERROR_ACCESS_DENIED == nNextResult )
                HandleAccessError(lpNav, ACCESS_DENIED_DIR);

            if ( nDirNesting )
            {
                // End of this level, back up one.

                enumFiles.Leave();
                nDirNesting--;
                NameStripFile( lpNav->szTheDirectory );
            }
            else
            {
                // Otherwise, end of all files.

                break;
            }
        }
        else
        {
            // We have a valid item.

            if ( AttrIsDir( enumFiles.m_rFind.dwFileAttributes ) )
            {
                // Item is a directory.

                if ( HandleDirectoryFound( lpNav, &enumFiles, bScanSubs ) )
                {
                    // We're now enumerating the subdirectory.
                    // Track levels of nesting, so we know how
                    // many Leave()'s to call.

                    nDirNesting++;
                }
            }
            else
            {
            LASTACCESSTIMEINFO rTimeInfo;
            BOOL               bKeepGoing;

                SaveLastAccessTime ( lpNav, enumFiles.GetLongName(),
                                     &rTimeInfo );

                // Item is a file.
                // HandleFileFound() will update current file info and
                // take care of any necessary scanning and response.

                bKeepGoing = HandleFileFound ( lpNav, &enumFiles );

                RestoreLastAccessTime ( &rTimeInfo );

                if ( !bKeepGoing )
                {
                    // User decided to stop scan.

                    break;
                }
            }
        }
    }
} // FindAllFiles()


//************************************************************************
// HandleDirectoryFound()
//
// This routine is called whenever the file enumerator in FindAllFiles()
// returns a subdirectory.
// If scan subdirectories is on, this routine checks buffer lengths,
// verifies the new directory name and tells the file enumerator to
// begin enumerating files in the new subdirectory.
//
// Parameters:
//      LPNONAV     lpNav           Junkbin of scan information.
//      CEnumFiles  *pEnumFiles     Pointer to file enumerator.
//      BOOL        bScanSubs       Scan subdirectories flag.
//
// Returns:
//      FALSE       if current directory being scanned was unchanged.
//      TRUE        if we entered this subdirectory.
//************************************************************************
// 05/02/1997 DALLEE, created.
//************************************************************************

BOOL LOCAL HandleDirectoryFound ( LPNONAV     lpNav,
                                  CEnumFiles  *pEnumFiles,
                                  BOOL        bScanSubs )
{
    auto    LPCTSTR lpszNonTransOemChars;
    auto    BOOL    bEnteredSubDir;
    auto    UINT    uBogusAttr;

    bEnteredSubDir = FALSE;

    if ( ( bScanSubs ) &&
         ( STRLEN( lpNav->szTheDirectory ) + STRLEN( pEnumFiles->GetLongFile() ) < SYM_MAX_PATH ) &&
         ( STRLEN( lpNav->szTheDirectory ) + STRLEN( pEnumFiles->GetShortFile() ) < SYM_MAX_PATH ) )
    {
        // Determine the correct non-translating
        // OEM characters based on the windows platform.

        switch ( SystemGetWindowsType() )
        {
        default:
        case SYM_SYSTEM_WIN_NT:
            lpszNonTransOemChars = NON_TRANS_OEM_CHARS_NT;
            break;

        case SYM_SYSTEM_WIN_95:
            lpszNonTransOemChars = NON_TRANS_OEM_CHARS_95;
            break;
        }

        // Construct new directory name.

        NameAppendFile( lpNav->szTheDirectory, pEnumFiles->GetLongFile() );

        // Check for valid name.
        // If available, we'll use the SFN if the long ANSI
        // name doesn't convert to OEM.

        if ( *pEnumFiles->m_rFind.szAlternateFileName &&
             STRPBRK( pEnumFiles->GetLongFile(), lpszNonTransOemChars ) &&
             ( NOERR != FileGetAttr( lpNav->szTheDirectory, &uBogusAttr ) ) )
        {
            NameStripFile( lpNav->szTheDirectory );
            NameAppendFile( lpNav->szTheDirectory, pEnumFiles->GetShortFile() );
        }

        // Tell file enumerator to enter this new directory.
        // Note that we call Enter() last, since GetLong/ShortFile()
        // will return empty strings once we enter the new directory.
        // Easier than pulling the last section from GetLong/ShortDirectory().

        if ( NOERROR == pEnumFiles->Enter() )
        {
            bEnteredSubDir = TRUE;

            // Update display with current directory.

            UpdateDir( lpNav->hMommyWindow, lpNav->szTheDirectory, TRUE );
        }
        else
        {
            // Had an error - reset NONAV's stored current directory.

            NameStripFile( lpNav->szTheDirectory );
        }
    }

    return ( bEnteredSubDir );
} // HandleDirectoryFound()


//************************************************************************
// HandleFileFound()
//
// This is the callback for file enumeration.
// Replaces the old MuchTooFast() and not-quite-as-old ItemFound()
// functions.
//
// Parameters:
//      LPNONAV     lpNav               Junkbin of scan information.
//      CEnumFiles  *pEnumFiles         FileAllFiles()'s file enumerator.
//
// Returns:
//      TRUE    to continue enumerating files.
//      FALSE   to stop.
//************************************************************************
// 05/15/96 DALLEE, Took MuchTooFast()'s guts, added lpShortFile and used
//      that name if the long one looks bogus. Also included ItemFound()
//      wrapper functionality.
//      Which allowed me to... FINALLY NUKE MuchTooFast()!
// 07/02/96 DJESCH, Fixed a problem in the scanner when dealing with .ZIP
//      files.
// 09/13/96 JBRENNA: Change to call InList() with SFN when one is available.
// 05/13/1997 DALLEE, modifications to work with new CEnumFiles iterator.
//      Also moved directory handling code to new func HandleDirectoryFound().
//************************************************************************

BOOL LOCAL HandleFileFound ( LPNONAV    lpNav,
                             CEnumFiles *pEnumFiles )
{
    // Add this file's bytes to total scanned.

    lpNav->dwScannedKBytes += ComputeKilobytesScanned(
                                    pEnumFiles->m_rFind.dwFileSizeLow,
                                    lpNav->dwClusterSize );

    // Construct and save the Oem and Ansi names for this item.

    ValidateAndSaveFileInfo( lpNav,
                             pEnumFiles->GetLongFile(),
                             pEnumFiles->GetShortFile(),
                             FALSE );

    STRCPY( lpNav->szInfectedLFN, pEnumFiles->GetLongName() );
    STRCPY( lpNav->szInfectedSFN, pEnumFiles->GetShortName() );

    // Check if this file should be scanned based on extension.
    // InList() also does the ZIP checking/scanning (brilliant...).

    // Used to use SFN here since it was more reliable with problem ANSI/OEM
    // files. However, we have problems using the SFN's to access
    // files on Netware volumes using the MS Netware client.
    // ValidateAndSaveFileInfo() loads lpNav->Infected with the LFN if
    // it's legit for ANSI/OEM or otherwise with the SFN, so we'll use
    // that here instead.

    if ( InList( lpNav, lpNav->Infected.szFullOemPath ) )
    {
                        // ---------------------------------
                        // Scan file first.  If not infected,
                        // check inoculation.
                        // ---------------------------------
        if ( !ScanTheFile( lpNav ) )
        {
#ifndef NAVSCANW    // Remove file inoculation for NAVSCAN and NT products.

            // File is not infected with a virus.
            // See if we should check inoculation on it.

            // NOTE: Check vs. SYM_MAX_PATH - 1 was added only
            // for inoculation. Not for virus scanning.
            // &? Does this still break? -DALLEE

            if ( ( lpNav->Inoc.bUseInoc ) &&
#ifdef INOC_SCAN_FIREWALL
                 ( STRLEN( lpNav->Infected.szFullPath ) < ( SYM_MAX_PATH - 1 ) ) &&
#endif
                 ( FileInExeList( gszExtensions, pEnumFiles->GetShortFile(),
                                  lpNav->ExtendedOpts.bScanNoExtFile) ) )
            {
                auto    char    szRoot [ SYM_MAX_PATH + 1 ];

                *szRoot = '\0';
                NameReturnRoot( lpNav->szTheDirectory, szRoot );

                switch ( GetDriveType( szRoot ) )
                {
                case DRIVE_REMOVABLE:
                    if ( !lpNav->Inoc.bInocFloppies )
                        break;
                    // else
                        // fall through

                case DRIVE_UNC_NAME:
                case DRIVE_REMOTE:
                case DRIVE_FIXED:
                    FileInoculated( lpNav );
                    break;

                default:
                    break;
                }
            }
#endif
        }   // if !ScanTheFile()
    }   // if InList()

    return ( lpNav->bDoScan );
} // HandleFileFound()


//************************************************************************
// ValidateAndSaveFileInfo ()
//
//  Used to save the full path to the file we're about to process to our
//  internal structure members.
//  Does verification of the LFN passed. If it can't be accessed, the
//  SFN is used instead. This handles cases where the LFN has characters
//  which don't map between ANSI and OEM (all SFN's should contain only
//  characters valid in the current code page).
//
// Parameters:
//
//  LPNONAV lpNav;      Our internal structure
//  LPSTR   lpFileName; File name that we're about to process
//  LPSTR   lpShortName if (!bIsFullName) SFN of file.
//                      if (bIsFullName) Ansi file name
//  BOOL    bIsFullName TRUE if scanning a single file
//
// Returns:
//          none
//
//************************************************************************
// 04/01/91 Martin   Function created
// 05/15/96 DALLEE, Added "ValidateAnd" to the name and now checks for
//          bogus non ANSI-OEM translatable LFNS and uses the SFN instead.
//************************************************************************

VOID WINAPI ValidateAndSaveFileInfo (
    LPNONAV lpNav,
    LPCTSTR lpFileName,
    LPCTSTR lpShortName,
    BOOL    bIsFullName
    )
{
    auto    UINT    uDummyAttr;

    MEMSET (&lpNav->Infected, 0, sizeof(lpNav->Infected));

    if (!bIsFullName)
        {
        auto    LPCTSTR lpszNonTransOemChars;

        // Determine the correct non-translating
        // OEM characters based on the windows platform.

        switch ( SystemGetWindowsType() )
        {
        default:
        case SYM_SYSTEM_WIN_NT:
            lpszNonTransOemChars = NON_TRANS_OEM_CHARS_NT;
            break;

        case SYM_SYSTEM_WIN_95:
            lpszNonTransOemChars = NON_TRANS_OEM_CHARS_95;
            break;
        }

        STRCPY(lpNav->Infected.szFullOemPath,lpNav->szTheDirectory);
        NameAppendFile(lpNav->Infected.szFullOemPath,lpFileName);
#ifdef SYM_WIN32
                                        // ---------------------------------
                                        // Convert the file name to Ansi for
                                        // display, exclude, and inoc purposes
                                        // ---------------------------------
        NavOemToAnsi (lpNav->Infected.szFullOemPath, lpNav->Infected.szFullPath);
#endif

                                        // For the OEM name we use to access
                                        // the file:
                                        // Check whether this is a valid file
                                        // (and whether it's possible to
                                        // fix by SFN replacement).
        if ( (EOS   != lpShortName[0]) &&
             (NULL  != STRPBRK(lpFileName, lpszNonTransOemChars)) &&
             (NOERR != FileGetAttr(lpNav->Infected.szFullOemPath, &uDummyAttr)) )
            {
            NameStripFile(lpNav->Infected.szFullOemPath);
            NameAppendFile(lpNav->Infected.szFullOemPath, lpShortName);
            }
        }
    else
        {
        STRCPY(lpNav->Infected.szFullOemPath,lpFileName);
        STRCPY(lpNav->Infected.szFullPath,lpShortName);
        }
} // ValidateAndSaveFileInfo()


//************************************************************************
// ScanTheFile()
//
// Does all manner of thing from extension checking to scanning to
// launching zip extraction to responding to zip extraction.
// (ANSI/OEM mess: works in OEM, except for exclude check)
//
// Parameters:
//
// Returns:
//************************************************************************
// 04/01/91 Martin   Function created
// 05/14/96 DALLEE, Things haven't calmed down a notch, but I figured this
//              could use some comments, anyway. ;)
//              Also, do exclude check with ANSI name.
//************************************************************************

UINT SYM_EXPORT WINAPI ScanTheFile (LPNONAV lpNav)
{
    auto        NETALERT        Alert;
    auto        BOOL            bDummy;
    auto        BOOL            bFileApisAnsi;
    auto        BOOL            bAccessDenied = FALSE;

                                        // There's a bit of klugery in
                                        // ScanProc(), HandleFileFound(),
                                        // and OurZipFunc() to track
                                        // full LFN and SFN for the current
                                        // item.
                                        // Verify this has worked correctly.
    SYM_ASSERT( lpNav->szInfectedLFN[0] );
    SYM_ASSERT( lpNav->szInfectedSFN[0] );


    YieldToWindows (lpNav->hMommyWindow);


                                        // Check right off the bat if the
                                        // file being scanned has a reserved
                                        // DOS name, like "aux".  This can
                                        // happen if the file was created
                                        // on a Mac and resides on a Mac
                                        // volume that is being scanned from
                                        // NT.  Fixes defect #86464.
    if ( NavNameIsReservedName ( lpNav->szInfectedLFN ) )
        {
        MEMSET( &lpNav->Infected.Notes, 0, sizeof( lpNav->Infected.Notes ) );
        return 0;
        }

                                        // -----------------------------------
                                        // For archive members, take the
                                        // directory of the archive, and
                                        // tag on the member's filename for
                                        // purposes of exclusion checks.
                                        // -----------------------------------
    if (lpNav->Zip.bInZip == TRUE)
        {
        NameStripFile( lpNav->szInfectedLFN );
        NameStripFile( lpNav->szInfectedSFN );

        NameAppendFile( lpNav->szInfectedLFN, lpNav->Zip.szZipChildFile );
        NameAppendFile( lpNav->szInfectedSFN, lpNav->Zip.szZipChildFile );
        }

                                        // -----------------------------------
                                        // If file is not excluded from virus
                                        // checks then scan it.
                                        // -----------------------------------
    lpNav->Stats.Files.dwScanned++;

    if ( ExcludeIsEx( lpNav->lpExclude,
                      lpNav->szInfectedSFN,
                      lpNav->szInfectedLFN,
                      excVIRUS_FOUND ) )
        {
        MEMSET( &lpNav->Infected.Notes, 0, sizeof( lpNav->Infected.Notes ) );
        return ( 0 );
        }


    bDummy = FileIsDOSExecutable( lpNav->szInfectedSFN );

    TSR_OFF;

    // Make sure file APIs are OEM
    bFileApisAnsi = AreFileApisANSI();

    if (bFileApisAnsi)
        SetFileApisToOEM ();

    lpNav->Infected.uVirusSigIndex = VirusScanFile (lpNav->Infected.szFullOemPath,
                                                    &lpNav->Infected.Notes,
                                                    bDummy);
    // Set file APIs back to what they were
    if (bFileApisAnsi)
        SetFileApisToANSI ();

    // Check for an error in the scanner. If uVirusSigIndex = 0 and
    // the Notes structure is cleared, then the was an error.

    if (lpNav->Infected.uVirusSigIndex == 0)
        {
        auto N30NOTES Notes = {0};

        if (MEMCMP(&lpNav->Infected.Notes, &Notes, sizeof(N30NOTES)) == 0)
            {
            if (GetLastError() == ERROR_ACCESS_DENIED)
                {
                HandleAccessError(lpNav, ACCESS_DENIED_FILE);
                bAccessDenied = TRUE;
                lpNav->Stats.Files.dwScanned--;
                }
            }
        }

    TSR_ON;

    YieldToWindows (lpNav->hMommyWindow);

    if ( (lpNav->Infected.uVirusSigIndex) &&
         (0xFFFF != lpNav->Infected.uVirusSigIndex) &&
         !bAccessDenied )
        {
                                        // ------------------------------------
                                        // If a virus infected drive is scanned
                                        // NAVW seems to be hanging. With this
                                        // fix we'll update the number of
                                        // infected files found so it shows that
                                        // NAVW is doing something
                                        // ------------------------------------
        StatsTimer(lpNav->hMommyWindow, bDummy, bDummy, (DWORD)bDummy);

        //ScanTime(lpNav->hMommyWindow, FALSE);

        if (lpNav->Alert.bBeep && lpNav->Opt.bPrestoNotify )
            MessageBeep ( MB_ICONASTERISK );

        Alert.lpInfected = &lpNav->Infected;

                                        // ------------------------------------
                                        // If we are scanning a zip file and
                                        // the CPU is to be halted, make sure
                                        // to delete the temporary file while
                                        // we still have the temporary filename.
                                        // ------------------------------------

        if ( lpNav->Zip.bInZip == TRUE )
            {
            if ( lpNav->Opt.uAction == SCAN_HALTCPU )
                FileDelete (lpNav->Infected.szFullOemPath);
#ifdef STUB_FIXED
            STRCPY (Alert.lpInfected->szFullPath,
                    lpNav->Zip.lpZipFileName);
#endif
            }

                                        // Do the alert AFTER we have decided
                                        // what to do with the file.
//        Alert.lpNet         = lpNav->lpNetUserItem;
//        Alert.lpAlertOpt    = &lpNav->Alert;
//        Alert.uTypeOfEvent  = EV_ALERT_KNOWN_VIRUS;
//        NetAlert (&Alert, &lpNav->Zip);

        //ScanTime(lpNav->hMommyWindow, TRUE);

                                    // If in Viewer Mode, we want to
                                    // Behave a specific way.
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            {
            if (lpNav->Zip.bInZip == FALSE)
                ViewerDecideWhatToDo (lpNav);
            else
                ViewerDecideWhatToDoWithZip (lpNav);
            }
        else
            {
            if (lpNav->Zip.bInZip == FALSE)
                DecideWhatToDo (lpNav);
            else
                DecideWhatToDoWithZip (lpNav);
            }
                                        // Do the alert AFTER we have decided
                                        // what to do with the file.
        Alert.lpNet         = lpNav->lpNetUserItem;
        Alert.lpAlertOpt    = &lpNav->Alert;
        Alert.uTypeOfEvent  = EV_ALERT_KNOWN_VIRUS;
        NetAlert (&Alert, &lpNav->Zip);

        }


    return (lpNav->Infected.uVirusSigIndex);
}


//************************************************************************
// FileInExeList()
//
// This function checks if a file's extension matches any of the program file
// extensions.
//
// Parameters:
//      LPCTSTR lpExtensions        [in] pointer to array of program file extensions
//      LPCTSTR lpFile              [in] pointer to the file to check
//
// Returns:
//      TRUE if file matches any of the extensions, FALSE otherwise.
//************************************************************************

BOOL FileInExeList (                    // Checks if the file's extension is
    LPCTSTR lpExtensions,               // a one that is in the list of
    LPCTSTR lpFile,                     // program file extensions.
    BOOL    bScanNoExtFile)             // Scan files with no extensions flag.
{
    auto    BOOL    bReturn = FALSE;

    // -----------------------------------------------------------------------
    // Note that I intentionally placed this code here to avoid touching the
    // old NAV 3.0 codebase.
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // This code should be rewritten! It shouldn't rebuild the extension list
    // for every file that is scanned.
    //
    // Also, it should be DBCS-enabled.
    // -----------------------------------------------------------------------

    const   UINT    uListSize = (SYM_MAX_PATH * 3);
    auto    char    szExtensionList [ uListSize ] = { 0 };
    auto    LPSTR   lpExtensionList;
    auto    char    szFileExt [4];      // Only checking 3 char extensions.

    SYM_ASSERT ( lpExtensions );

    auto    UINT    uSourceOffset = 0;
    auto    UINT    uTargetOffset = 0;
    auto    UINT    uNameExtReturn;

                                        // Copy file extensions into szz.
    while ( lpExtensions[uSourceOffset] )
        {
        while ( lpExtensions[uSourceOffset] &&
                lpExtensions[uSourceOffset] != ' ' )
            {
            if (DBCSIsLeadByte (lpExtensions[uSourceOffset]) )
                {
                szExtensionList[uTargetOffset++] = lpExtensions[uSourceOffset++];
                }
            szExtensionList[uTargetOffset++] = lpExtensions[uSourceOffset++];
            }

        if ( lpExtensions[uSourceOffset] )
            {                          //&? If you got here,
                                       //   lpExtensions[uSourceOffset] == ' ',
                                       //   so no special pointer increment
                                       //   for DBCS is needed
            uTargetOffset++;
            uSourceOffset++;
            }
        }

                                        // Retrieve up to 3 chars of the
                                        // file's extension.
    uNameExtReturn = NameReturnExtensionEx(lpFile, szFileExt, sizeof(szFileExt));
#ifdef SYM_WIN32
    NavOemToAnsi(szFileExt, szFileExt);
#endif

    // Return with TRUE if the file has no extension and the scan files with no
    // extension option is enabled
    if (!STRCMP(szFileExt, "") && NOERR == uNameExtReturn && bScanNoExtFile == TRUE)
        bReturn = TRUE;
    else
    {
                                        // Check for matches. Note that the
                                        // extensions appear as filenames
                                        // with no extension, but that the
                                        // check is equivalent.
        for ( lpExtensionList = szExtensionList;
              *lpExtensionList && (FALSE == bReturn);
              lpExtensionList += (STRLEN(lpExtensionList) + 1) )
            {
            bReturn = FileNameMatchSection(lpExtensionList, szFileExt);
            }
    }

    return ( bReturn );
}


//************************************************************************
// InList()
//
// Checks if the file matches the extensions to scan.
// If its a .ZIP file, it sends it off to IfZipThenScanZip for processing
// and returns FALSE (because the IfZipThen..() function handled it), unless
// its a ZIP2EXE, in which case the .EXE part needs to be scanned also.
//
// Parameters:
//      LPNONAV     lpNav           [out] Gets wildcard mask if any.
//      LPCTSTR     lpFileName      [in] Name of file to check against list
//
// Returns:
//      TRUE if its in the list, otherwise FALSE
//************************************************************************
// 04/01/91 Martin  Function created
// 06/24/96 DJESCH  Modified to handle ZIP2EXE files and to save the file name
//                  when processing .ZIPs for display in the Repair Wizard.
// 09/10/96 DJESCH  When scanning .ZIP files, will now only scan those files
//                  specified in the "Program Files" configuration option.
//                  (just commented out the check for lpNav->Zip.bInZip)
// 8/17/1998 DALLEE Removed code which cancelled scanning if item was an archive.
//                  After scanning archive members, now checks container
//                  normally vs. program file extensions.
//************************************************************************

BOOL LOCAL InList ( LPNONAV lpNav, LPCTSTR lpFileName )
{
    auto    char    szSaveFileName [SYM_MAX_PATH + 1];
    auto    BOOL    bStatus = TRUE;


    STRCPY(szSaveFileName, lpFileName);

                                        // ---------------------------------
                                        // If we're coming from Tools->inoc
                                        // we have to check the file towards
                                        // our program list, no matter
                                        // what the options tell us.
                                        // ---------------------------------

    if ( lpNav->Inoc.uActionNewInoc == INOC_AUTO_FILEONLY )
        {
        bStatus = FileInExeList ( gszExtensions, szSaveFileName,
                                    lpNav->ExtendedOpts.bScanNoExtFile);
        }
    else
        {
        if (lpNav->Gen.bScanZipFiles == TRUE)
            {
            auto   char   szOrigInfectFullPath   [SYM_MAX_PATH+1];
            auto   char   szOrigInfectFullOemPath[SYM_MAX_PATH+1];
            auto   char   szOrigInfectLFN [SYM_MAX_PATH+1];
            auto   char   szOrigInfectSFN [SYM_MAX_PATH+1];

                                                    // IfZipThenScanZip() blows away the
                                                    // zip path information, so save it.
            STRCPY (szOrigInfectFullPath,    lpNav->Infected.szFullPath);
            STRCPY (szOrigInfectFullOemPath, lpNav->Infected.szFullOemPath);
            STRCPY (szOrigInfectLFN, lpNav->szInfectedLFN);
            STRCPY (szOrigInfectSFN, lpNav->szInfectedSFN);

            IfZipThenScanZip (lpNav, szSaveFileName );

                                                    // Restore the paths (what a kludge!)
            STRCPY(lpNav->Infected.szFullPath,    szOrigInfectFullPath);
            STRCPY(lpNav->Infected.szFullOemPath, szOrigInfectFullOemPath);
            STRCPY(lpNav->szInfectedLFN, szOrigInfectLFN);
            STRCPY(lpNav->szInfectedSFN, szOrigInfectSFN);
            }
        }

    if (lpNav->Opt.bScanAll == FALSE)
        {
        bStatus = FileInExeList( gszExtensions, szSaveFileName,
                                    lpNav->ExtendedOpts.bScanNoExtFile);
        }
    else
        bStatus = TRUE;

    return (bStatus);
} // InList()


//************************************************************************
// ANSISwitchTo()
//
// Takes the original ANSI filespec to scan and tries both LFN and SFN
// versions to try to find a valid directory and possible wildcard mask
// to scan.
// Expects ANSI filespec, but file APIs should be OEM on entry.
//
// Parameters:
//      HWND    hDlg                Parent handle for access error dlgs.
//      LPSTR   lpAnsiFileSpec      Dir/File spec to scan (in ANSI)
//      LPSTR   lpDir               [out] Gets directory we're scanning.
//      LPNONAV lpNav               [out] Gets wildcard mask if any.
//
// Returns:
//************************************************************************
// 04/01/91 Martin   Function created
//************************************************************************

BOOL LOCAL ANSISwitchTo (
    HWND    hDlg,
    LPSTR   lpAnsiFileSpec,
    LPSTR   lpDir,
    LPNONAV lpNav
    )
{
    auto    BOOL    bStatus = FALSE;
    auto    UINT    uTimeOut;
    auto    char    szDirectory [ SYM_MAX_PATH+1 ];
    auto    UINT    uDummyAttr;

    TSR_OFF;
                                        // ----------------------------------
                                        // Initialize the directory. If the
                                        // directory has a wildcard section
                                        // at the end, then chop it off and
                                        // save it in lpNav.
                                        // ----------------------------------
#ifdef SYM_WIN32
    NavAnsiToOem (lpAnsiFileSpec, lpDir);
#endif

    *lpNav->lpScan->szScanFileSpec = EOS;

    if (NameIsWildcard (lpDir))
        {
        NameReturnFile(lpDir, lpNav->lpScan->szScanFileSpec);
        NameStripFile (lpDir);
        }

                                        // ----------------------------------
                                        // Initialize # timeout seconds
                                        // ----------------------------------

    uTimeOut = (lpNav->Alert.bRemoveAfter) ? lpNav->Alert.uSeconds : 0;

                                        // Check for bogus LFN and try to
                                        // fix by substituting SFN.
    if (NOERR != FileGetAttr(lpDir, &uDummyAttr))
        {
        auto    char  szAnsiDir [ SYM_MAX_PATH + 1];

        STRCPY (szAnsiDir, lpAnsiFileSpec);
        if (NameIsWildcard (szAnsiDir))
            NameStripFile (szAnsiDir);

        SET_FILE_APIS_TO_ANSI;
        NameReturnShortName(szAnsiDir, szDirectory);
        SET_FILE_APIS_TO_OEM;

        NavAnsiToOem(szDirectory, szDirectory);

        if (NOERR == FileGetAttr(szDirectory, &uDummyAttr))
            {
            STRCPY(lpDir, szDirectory);
            }
        }

    if ( *lpDir )
        {
        if ( IsUNCPath ( lpDir ) == TRUE )
            bStatus = TRUE;
        else
            {
                                        // ----------------------------------
                                        // If we're not to skip this disk and
                                        // if it's possible to switch over to
                                        // the specified drive, continue.
                                        // ----------------------------------

            if ( ScanDriveSkipped( lpDir ) == FALSE &&
                 DiskInDrive ( hDlg, *lpDir, uTimeOut ) )
                {
                DiskSet ( *lpDir );

                                        // ----------------------------------
                                        // Only attempt to move up in dir
                                        // structure, if root attempted.
                                        // ----------------------------------
                if ( DirSet(lpDir) == ERR)
                    {
                    if ( GetLastError() == ERROR_ACCESS_DENIED )
                        HandleAccessError(lpNav, ACCESS_DENIED_DIR);

                    if ( NameIsRoot(lpDir) )
                        {
                                        // ----------------------------------
                                        // Try to move up from current dir,
                                        // until we can't.
                                        // ----------------------------------

                        if ( DirGet ( *lpDir, szDirectory ) != ERR )
                            {
                            do {
                                NameStripFile ( szDirectory );
                               }
                            while ( DirSet ( szDirectory ) != ERR &&
                                   !NameIsRoot ( szDirectory ) );

                                        // ----------------------------------
                                        // Whatever this is, is the starting
                                        // dir. If we can get the directory,
                                        // then we can scan the drive.
                                        // ----------------------------------

                            DirGet ( *lpDir, lpDir );
                            bStatus = TRUE;
                            }
                        }
                    }
                else
                    {
                    bStatus = TRUE;
                    }
                }
            else
                {
                lpDir[1] = '#';
                lpAnsiFileSpec[1] = '#';
                }
            }
        }

    if ( bStatus == TRUE )
        {
        UpdateDir ( lpNav->hMommyWindow, lpDir, TRUE );
        }

    TSR_ON;

    return ( bStatus );
} // ANSISwitchTo()


// ************************************************************************* //
// ************************************************************************* //

BOOL LOCAL DiskInDrive (
    HWND    hDlg,
    BYTE    byDrive,
    UINT    uTimeOut
    )
{
    auto    UINT                uAction;
    auto    BOOL                bStatus = FALSE;
    auto    char                szFullPath [ SYM_MAX_PATH+1 ];
    auto    DRIVEPROBLEMSTRUCT  DriveAccess;


    MEMSET (&DriveAccess, EOS, sizeof(DriveAccess));
    DriveAccess.hParent         = hDlg;
    DriveAccess.byDrive         = byDrive;
    DriveAccess.bHideContinue   = TRUE;
    DriveAccess.uTimeOutSeconds = uTimeOut;

    do
        {
        bStatus = DirGet(byDrive, szFullPath);
                                        // -----------------------------------
                                        // If error occured than the drive is
                                        // not ready.
                                        // -----------------------------------
        if (bStatus == ERR)
            {
            bStatus = FALSE;
                                        // -----------------------------------
                                        // Ask the user to correct the problem
                                        // -----------------------------------

#ifdef SYM_WIN32
            STATUS Status;

            Status = NavStatSetDriveState ( byDrive,
                                            0,
                                            DRIVESTATUS_FOREGROUND,
                                            NAV_APP_CLINIC );
            if (Status != NOERR )
            {
                DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT;
                NavStatGetDriveState ( DriveAccess.byDrive,
                                       0,
                                       DriveAccess.szWho,
                                       sizeof (DriveAccess.szWho) );
            }
            else
#endif
                if ( VirusScanCanAccessDisk ( byDrive, DISK_READ_ONLY ) == FALSE )
                    DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_DISKLOCKED;
                else
                    DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_DRIVE;

#ifdef SYM_WIN32
                NavStatSetDriveState ( byDrive,
                                       0,
                                       DRIVESTATUS_FREE,
                                       NAV_APP_CLINIC );
#endif

            uAction = DriveAccessProblem (&DriveAccess);

                                        // -----------------------------------
                                        // User doesn't like to correct problem
                                        // -----------------------------------

            if ( uAction == DRIVEERROR_SKIP     ||
                 uAction == DRIVEERROR_CONTINUE ||
                 uAction == DRIVEERROR_ERROR )
                {
                break;
                }
            }
        else
            bStatus = TRUE;

        } while (bStatus == FALSE);


    return (bStatus);
}


//************************************************************************
// ScanTime()
//
// Initiate the timer for the scanner animation.
//
// Parameters:
//      HWND        hDlg                Dialog handle for the scanner UI
//      BOOL        bStart              TRUE to start, FALSE to stop
//
// Returns:
//      none
//************************************************************************
// 04/01/91 Martin   Function created
//************************************************************************

VOID SYM_EXPORT WINAPI ScanTime(HWND hDlg, BOOL bStart)
{
    #define NUM_SECONDS  1000

    auto    LPNONAV         lpNav;
    auto    DWORD           dwCurrTime;


#if SYM_WIN32
    if ( bStart == TRUE )
        ScanAnimationEnable ( hDlg );
    else
        ScanAnimationDisable ( hDlg );
#endif

                                        // Get time in milliseconds.
                                        // Don't blow up like time().
    dwCurrTime = GetTickCount() / 1000L;

#ifndef NAVSCANW
    if ((lpNav = (LPNONAV)LP_GET(hDlg)) != NULL)
#else
    lpNav = (LPNONAV)LP_GET(hDlg);
    if (!lpNav)
        {LP_SET (hDlg, GloballySaved_lpNav);
        lpNav = (LPNONAV)LP_GET(hDlg);
        /*SYM_ASSERT (lpNav);*/}
    if (lpNav)
#endif
       {
       if (bStart == TRUE)
          {
          lpNav->Stats.lScanStart = (dwCurrTime - lpNav->Stats.lTimeScan);

          if (lpNav->lpTimerProc)
             {
             if (!SetTimer (hDlg, TIMER_SCANNED, NUM_SECONDS,
                           (TIMERPROC)lpNav->lpTimerProc))
                {
#if 0
                MEMSET (&Notify, '\0', sizeof(Notify));
                Notify.hParent       = hDlg;
                Notify.uMsgID        = IDS_ERR_NOTIMER;
                Notify.uTitleID      = IDS_NAV;
                if ( GetCustomVersion() == VERSION_NETSCAPE )
                    {                   // Title is "Internet Scanner"; set at runtime.
                    Notify.uTitleID      = IDS_NAV_INTERNET;
                    }
                Notify.lpFileName    = NULL;
                Notify.lpVirusName   = NULL;
                Notify.lpAlertMsg    = (lpNav->Alert.bDispAlertMsg) ?
                                        lpNav->Alert.szAlertMsg : NULL;
                Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                        lpNav->Alert.uSeconds : NULL;
                Notify.hInstance     = hInstance;

                NotifyUser (&Notify);
#endif
                }
             }
          }
       else
          {
          lpNav->Stats.lTimeScan = (dwCurrTime - lpNav->Stats.lScanStart);
          lpNav->Stats.lScanStart = lpNav->Stats.lTimeScan;
          KillTimer(hDlg, TIMER_SCANNED);
          }
       }
}


//************************************************************************
// If ZipThenScanZip()
//
// Checks a file to see if its a container and if it is, it starts the
// processing of it.  (It sets the bInZip flag before it processes.)
//
// Parameters:
//      LPNONAV     lpNav       Pointer to all our structures
//      LPSTR       lpFileName  Name of file to check
//
// Returns:
//   Returns TRUE or FALSE for the result of the test.
//
// Modifications:
//   07/18/96 DJESCH If we're doing a command line scan on ZIP files only,
//                   we should not scan all ZIP-format files (ZIP2EXE), just
//                   the real .ZIP files.
//************************************************************************

BOOL LOCAL IfZipThenScanZip
(
    LPNONAV lpNav,
    LPSTR   lpFileName
)
{
#ifdef _ALPHA_
    return FALSE;
#else
    char        szFullPath   [SYM_MAX_PATH+1];
    char        szFileName   [SYM_MAX_PATH+1];
    char        szFileExt    [4];
    BOOL        bProcessFile  = FALSE;
    BOOL        bWasProcessed = FALSE;
    extern      IDecomposer* pgDecomposer;

    // we don't scan recursively

    if (lpNav->Zip.bInZip)
    {
        return FALSE;
    }

    // if the decomposer failed to initialize pgDecomposer will be NULL

    if (!pgDecomposer)
    {
        return FALSE;
    }

    // Do we want to send the file to the decomposer?  Check the file
    // extention.

    NameReturnExtension(lpFileName, szFileExt);

    STRUPR(szFileExt);

    const char szCompressedExtList[] = "EXE.ZIP.LHA.LZH.ARJ.CAB.MIM.MME.UUE.IMZ";

    if (STRLEN(szFileExt) == 3 &&
        (STRSTR(szCompressedExtList, szFileExt) ||
         szFileExt[2] == '_'))          // LZ extentions: ".??_"
    {
        bProcessFile = TRUE;
    }
    else
    {
        return FALSE;
    }

    // get the full path to the file name

    STRCPY(szFullPath, lpNav->szTheDirectory);
    NameReturnFile(lpFileName, szFileName);
    NameAppendFile(szFullPath, szFileName);

    // If the directory that we're in is soo deep that we don't have
    // space to append an infected ZIP filename, then don't scan it.
    //
    // ?? - is this possible?  I'm doing the check here to maintain
    // existing functionality.

    if ((STRLEN(lpNav->szTheDirectory) + STRLEN(szFileName)) >= SYM_MAX_PATH)
    {
        bProcessFile = FALSE;
    }

    // see if the file is in the exclusions list

    if (ExcludeIs(lpNav->lpExclude, szFullPath, excVIRUS_FOUND))
    {
        bProcessFile = FALSE;
    }

    // if bProcessFile is set pass the file to the decomposer for processing

    if (bProcessFile)
    {
        // variables for the decomposer

        IDecObject* pObject = NULL;
        HRESULT     hr;
        WORD        wDecResult;

        hr = DecNewFileObject(szFullPath, szFullPath, &pObject);

        if (SUCCEEDED(hr))
        {
            // Find the end of InfArr chain before we decopose the compressed file.
            auto  LPINFARR lpInfArr = NULL;

            if (lpNav->lpInfArr)
            {
                lpInfArr = (LPINFARR)lpNav->lpInfArr;

                while (lpInfArr->uNumUsed >= MAX_INFECTSTRUCTS)
                {
                    if (lpInfArr->lpNext)
                        lpInfArr = (LPINFARR)lpInfArr->lpNext;
                    else
                        break;
                }
            }


            // For download mode, store the fact that we have a compressed
            // file. Also, set the bInZip flag ("bInCompressed" would be a
            // more accurate name.

            lpNav->lpScan->bCompressedDownload = TRUE;
            lpNav->Zip.bInZip                  = TRUE;
            lpNav->Zip.lpZipFileName           = lpFileName;

            // set the UI text

            Static_SetText (GetDlgItem(lpNav->hMommyWindow, ID_SC_WORKINGON),
                            String_Ptr(IDS_SCANDLG_SCANINGFILES));

            // "sink" is the class where we do our scanning within the
            // decomposer - see n_decomp.cpp

            CNAVEventSink sink(lpNav);

            // decompose the file

            hr = pgDecomposer->Process(pObject,
                                       &sink,
                                       &wDecResult,
                                       szFullPath);

            // pDecomposer->Process returns failure if:
            //
            //   1. There was an internal decomposer error.
            //
            //   2. The Decomposer does not handle the file.
            //      NOTE: Our own implementation of OnNotIdentified()
            //            sets the HRESULT to error for us.

            if (SUCCEEDED(hr))
            {
                bWasProcessed = TRUE;

                DecideWhatToDoAfterProcessingZip (lpNav, lpInfArr);
            }

            DecDeleteFileObject(pObject);

            // turn off bInZip

            lpNav->Zip.bInZip = FALSE;
        }
    }
    return (bWasProcessed);
#endif // _ALPHA_
}


//************************************************************************
// YeildToWindows()
//
// Yeilds processor time to the OS.
//
// Parameters:
//      HWND        hWnd        Handle to current UI window
//
// Returns:
//   none
//************************************************************************
VOID SYM_EXPORT WINAPI YieldToWindows( HWND hWnd )
{

    auto    MSG     msg;

#ifdef NAVWNT
    DWORD dwElapsedTime;
    DWORD dwCurTime;
    static DWORD dwPrevTime;
#endif

#ifdef NAVWNT
    if ( s_bUsingLoadManagement )
		{
        //
        // Get elapsed time since last yield
        //
        dwCurTime = GetTickCount();

        //
        // Number reported by GetTickCount() rolls over
        // after Windows has been running for 49 hours.
        // If the current value is less than previous
        // one, assume the count has rolled over.
        //
        if ( dwCurTime < dwPrevTime )
            dwElapsedTime = 1000L;
        else
            dwElapsedTime = dwCurTime - dwPrevTime;

        //
        // Update server load info only if one second
        // or more has passed since last update
        //
        if ( dwElapsedTime >= 1000L )
            UpdateServerLoad( &s_LoadManagementInfo );

        ServerLoadManagementDelay( &s_LoadManagementInfo );
		}
#endif

    while (NavPeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
        if (!IsDialogMessage (hWnd, &msg))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

DWORD LOCAL GetFileSize (
    LPSTR   lpFileName
    )
{
    auto    DWORD   dwFileSize = 0;
    auto    HFILE   hFile;

    if ((hFile = FileOpen(lpFileName,READ_ONLY_FILE)) != HFILE_ERROR)
        {
        dwFileSize = FileLength(hFile);
        FileClose(hFile);
        }

    return (dwFileSize);
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    SaveLastAccessTime()
//
// Description:
//  Stores a file's last-accessed date/time stamp so it can be
//  restored after NAVW does whatever it needs to do with the file.
//
// Input:
//  pNoNav: [in] The NONAV struct for the scan in progress.
//  szFilespec: [in] Full path to the file
//  prTimeInfo: [out] Info used by RestoreLastAccessTime() later on.
//
// Returns:
//  Nothing - All meaningful info is returned in the parameters.
//
// Notes:
//  The caller can call RestoreLastAccessTime() without having to check
//  whether this fn succeeded - RLAT() will bail out if this fn couldn't
//  do what it needed to.
//
//  If the caller calls RestoreLastAccessTime(), the filespec and NONAV
//  struct *must* be at the same locations as when this fn is called.
//  If not, the caller will have to change the LASTACCESSTIMEINFO
//  struct members to the correct values.
//
//////////////////////////////////////////////////////////////////////////
// 1/14/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void SaveLastAccessTime ( LPNONAV pNoNav,
                          LPCTSTR szFilespec,
                          LASTACCESSTIMEINFO* prTimeInfo )
{
    SYM_ASSERT ( pNoNav != NULL );
    SYM_ASSERT ( szFilespec != NULL );
    SYM_ASSERT ( prTimeInfo != NULL );

                                        // Init struct values
    prTimeInfo->bMustChangeAttrs = FALSE;
    prTimeInfo->bLastAccessTimeSaved = FALSE;
    prTimeInfo->szFilespec = szFilespec;
    prTimeInfo->pNoNav = pNoNav;
    prTimeInfo->uNumCleanedPre = pNoNav->Stats.Files.uCleaned;

                                        // First get the file's last-accessed
                                        // date & time.
                                        // If this fails, then we'll bail
                                        // out silently because there are more
                                        // important things in life to worry
                                        // about, like viruses.

    if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        {
        if ( GetLastAccessDateNT ( szFilespec,
                                   &prTimeInfo->ftimeLastAccessTime ) )
            {
            prTimeInfo->bLastAccessTimeSaved = TRUE;
            }
        }
    else
        {
                                        // On 95 we need an open file handle...
        HFILE hfile;

        hfile = FileOpen ( szFilespec, READ_ONLY_FILE );

        if ( HFILE_ERROR != hfile )
            {
            prTimeInfo->bLastAccessTimeSaved =
                GetFileTime ( (HANDLE) hfile, NULL,
                              &prTimeInfo->ftimeLastAccessTime, NULL );

            FileClose ( hfile );
            }
        }

                                        // Now read in the file's attributes.
                                        // If this fails, then *pbMustChangeAttrs
                                        // remains FALSE.

    if ( prTimeInfo->bLastAccessTimeSaved )
        {
                                        // Only check the attrs if we were
                                        // able to get the last accessed
                                        // date earlier.

        if ( NOERR == FileGetAttr ( szFilespec, &prTimeInfo->uFileAttrs ) )
            {
            prTimeInfo->bMustChangeAttrs =
                ( (prTimeInfo->uFileAttrs) & FA_RDONLY ) ? TRUE : FALSE;
            }
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    RestoreLastAccessTime()
//
// Description:
//  Restores a file's last-accessed date/time stamp that had earlier been
//  saved by SaveLastAccessTime().
//
// Input:
//  prTimeInfo: [in] Info saved during SaveLastAccessTime().
//
// Returns:
//  Nothing - We can't do anything meaningful if this function fails anyway.
//
// Note:
//  It's up to the caller to make sure that the LASTACCESSTIMEINFO struct
//  was set by a matched call to SaveLastAccessTime().
//
//  This fn assumes that the caller is done using the file (for the
//  time being at least) and has NO more open handles on the file.
//
//  See also notes for SaveLastAccessTime().
//
//////////////////////////////////////////////////////////////////////////
// 1/14/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void RestoreLastAccessTime ( const LASTACCESSTIMEINFO* prTimeInfo )
{
BOOL  bAttrsChanged = FALSE;
HFILE hfile;

    SYM_ASSERT ( prTimeInfo != NULL );


    if ( !prTimeInfo->bLastAccessTimeSaved )
        {
        return;                         // See ya!  There's nothing for us
        }                               // to do here.


                                        // Check if a file was cleaned since
                                        // the call to SaveLastAccessTime().
                                        // If the user has immediate notify
                                        // on, and cleaned the file, then
                                        // the last accessed time *should*
                                        // change, and we shouldn't do
                                        // anything here.

    if ( prTimeInfo->uNumCleanedPre !=
         prTimeInfo->pNoNav->Stats.Files.uCleaned )
         {
         return;
         }

                                        // Clear the file's RO attribute
                                        // if necessary.  If this fails,
                                        // then we won't be able to set the
                                        // date so we might as well bail out.
    if ( prTimeInfo->bMustChangeAttrs )
        {
        if ( NOERR == FileSetAttr ( prTimeInfo->szFilespec,
                                    (prTimeInfo->uFileAttrs) & ~FA_RDONLY ) )
            {
            bAttrsChanged = TRUE;
            }
        else
            {
            return;
            }
        }

                                        // Now open the file and set its
                                        // last accessed date.  If this
                                        // fails, then there isn't a whole
                                        // lot we can do about it, and we'll
                                        // fail silently.

    hfile = FileOpen ( prTimeInfo->szFilespec, OF_WRITE );

    if ( HFILE_ERROR != hfile )
        {
        SetFileTime ( (HANDLE) hfile, NULL,
                      &prTimeInfo->ftimeLastAccessTime, NULL );

        FileClose ( hfile );
        }

                                        // Restore the file's attributes if
                                        // we changed 'em earlier.

    if ( bAttrsChanged )
        {
        FileSetAttr ( prTimeInfo->szFilespec, prTimeInfo->uFileAttrs );
        }
}



//************************************************************************
// HandleAccessError()
//
// Process ACCESS_DENIED failures.
//
// Parameters:
//      LPNONAV     lpNav               Junkbin of scan information.
//      UINT        uType               Directory or file. See ACCESS_DENIED_*
//
// Returns:
//      TRUE    to continue enumerating files.
//      FALSE   to stop.
//************************************************************************
// 09/18/98 TCASHIN Function created.
//************************************************************************

BOOL LOCAL HandleAccessError ( LPNONAV lpNav, UINT uType )
{
    auto LOGSTRUCTEX   Log = { 0 };

    if ( lpNav->ExtendedOpts.bLogAccessDenied )
    {
        if (uType == ACCESS_DENIED_DIR)
        {
            Log.uEvent       = LOGEV_SYSERROR_ACCESSDENIED_DIR;
            Log.lpFileName   = lpNav->szTheDirectory;
        }
        else
        {
            Log.uEvent       = LOGEV_SYSERROR_ACCESSDENIED_FILE;
            Log.lpFileName   = lpNav->Infected.szFullPath;
        }

        Log.lpVirusName  = NULL;
        Log.lpAct        = &lpNav->Act;
        Log.lpNoNav      = reinterpret_cast<LPVOID>(lpNav);

        if (LogEventEx (&Log))
            NotifyLogWriteError (lpNav->lpScan->hDlg, lpNav);
    }

    return ( TRUE );
}
