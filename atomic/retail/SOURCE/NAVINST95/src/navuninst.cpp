/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/navuninst.cpv   1.52   14 Jan 1999 19:33:34   CEATON  $
//
//  NavUninst.CPP - Module that holds mostly uninstall functions for the
//                  InstallShield installation of NAVNT
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/navuninst.cpv  $
// 
//    Rev 1.52   14 Jan 1999 19:33:34   CEATON
// Added code to unregister the NAV "LiveUpdate" COM object.
// 
// 
//    Rev 1.51   11 Nov 1998 10:32:14   rchinta
// Deleting the Backlog Registry Value during UninstInitialize().
// 
//    Rev 1.50   15 Oct 1998 18:59:54   mdunn
// Barstow stuff.  Added IsSilentUninstall() to fix #123154.
// 
//    Rev 1.49   15 Oct 1998 17:06:26   mdunn
// Added "nav95.*" to the list of files to delete from TARGETDIR to remove
// backup copies of nav95.isu created by Backlog.
// 
//    Rev 1.48   13 Oct 1998 17:00:14   rchinta
// Modified UnregisterCOMObjects to call DllUnregisterServer
// for aboutsw.dll (new dll for SystemWorks Upsell).
// 
//    Rev 1.47   24 Jul 1998 11:55:06   mdunn
// Fixed 118928 - Added UnregisterCOMObjects().
// 
//    Rev 1.46   22 Jul 1998 19:56:54   mdunn
// Changed RemoveQuarantineContents() to leave all files in Quar on a corporate
// install.
//
//    Rev 1.45   15 Jul 1998 18:16:48   mdunn
// Fixed 118295 - it's actually caused by a bug in 95 so I worked around it.
// RemoveHiddenHelpFiles() calls SHFileOperation() with a don't-pop-up-errors
// flag but the API was showing an error anyway!  Now RHHF() checks for the
// existence of GID files and calls SHFileOp() only if there are any GIDs to
// delete.
//
//    Rev 1.44   27 Jun 1998 15:56:40   mdunn
// Fixed the RemoveHiddenHelpFiles() call.
//
//    Rev 1.43   15 Jun 1998 13:39:48   mdunn
// Fixed 117691 - now removing the 'submit a bug report' URL file
//
//    Rev 1.42   08 Jun 1998 23:38:10   SEDWARD
// IsMigratedInstall references the Atomic version instead of Roswell (ie,
// "5.3" instead of "5.0").
//
//    Rev 1.41   05 Jun 1998 22:43:10   SEDWARD
// Added more support for the Atomic migration (Win95 to NT5).
//
//    Rev 1.40   03 Jun 1998 16:53:26   mdunn
// Added IsCorporateVersion() and RemoveHiddenHelpFiles().
//
//    Rev 1.39   01 Jun 1998 20:52:38   mdunn
// Nuked a stray _asm int 3
//
//    Rev 1.38   01 Jun 1998 20:39:06   mdunn
// Added ability to clean out Quarantine during uninstall.
// Removed references to Extensions directory and reg entry since that dir
// doesn't even exist any more.
//
//    Rev 1.37   17 Apr 1998 16:04:56   RStanev
// SmellyCat changes/.
//
//    Rev 1.36   06 Apr 1998 22:50:26   mdunn
// UninstInitialize() now calling NTaskMgr to remove NAV events from the
// Win 98 Task Scheduler.
//
//    Rev 1.35   05 Mar 1998 16:44:26   mdunn
// Now cleaning up NEC and Trial versions in CleanUpOnRamp.
//
//    Rev 1.34   05 Mar 1998 12:59:50   mdunn
// Changed product name from "Norton AntiVirus ROSWELL95" to "NAV95" in
// CleanUpOnRamp().
//
//    Rev 1.33   02 Mar 1998 14:07:10   mdunn
// Fixed a build error in GetUsageCount().
//
//    Rev 1.32   30 Dec 1997 19:01:00   SEDWARD
// Added support for the Win95-NT5 migration.
//
//    Rev 1.31   22 Aug 1997 21:47:18   jtaylor
// -- Changed the calls to the OnrampCleanupEx.  Now it is called once for
//    each of the possible sequence numbers.
//
//    Rev 1.30   19 Aug 1997 13:35:00   jtaylor
// -- Fixed an access violation by changing a variable type.
// -- Fixed an order of execution bug.
//    This removed the Symevnt.386 registry value incorrectly.
//
//    Rev 1.29   18 Aug 1997 21:58:44   jtaylor
// -- Corrected a misspelling of the filename symevnt.386
//
//    Rev 1.28   15 Aug 1997 16:51:48   jtaylor
// -- Moved the code that removed the Symantec registry keys to the end of
//    uninstall.  They were being left behind.
//
//    Rev 1.27   14 Aug 1997 17:20:26   jtaylor
// -- Moved the call to CleanupSymantecSharedRegEntries to uninstuninitialize.
//    It was preventing sevinst from working correctly.
//
//    Rev 1.26   13 Aug 1997 18:12:46   jtaylor
// -- Added code to the UninstallVirusDefinitions function to make it
//    get the targetdir.
// -- Switched the call in there to InitInstallApp.  This will remove the
//    defloc.dat file in TARGETDIR.
//
//    Rev 1.25   12 Aug 1997 20:40:10   jtaylor
// -- Made the code to cleanup Haking exported.
//
//
//    Rev 1.24   11 Aug 1997 16:16:06   sedward
// Enhanced 'CleanupLiveUpdate' function to remove the 's32LuCp1.cpl' file.
//
//    Rev 1.23   06 Aug 1997 16:24:58   jtaylor
// -- Fixed a bug in the code that removes directories so that it will remove
//    the Symantec directory if empty.
//
//    Rev 1.22   04 Aug 1997 17:13:56   JTAYLOR
// -- Removed the commands to delete the heavy hitter files on uninstall.
//
//
// .
//
//    Rev 1.21   03 Aug 1997 20:35:00   SEDWARD
// Added 's32luh1.hlp' and 's32luh1.cnt' to the list of LiveUpdate files to
// be removed during uninstall.
//
//    Rev 1.20   23 Jul 1997 17:57:22   jtaylor
// -- Added code to delete the heavy hitter files.
//
//    Rev 1.19   20 Jul 1997 11:27:56   JALLEE
// Fixed up CleanupShareSymantecRegEntries().  Now delete the "LiveUpdate1" value
// correctly if necessary.
//
//    Rev 1.18   19 Jul 1997 15:29:02   jtaylor
// -- Added commands to delete the Symantec and Symantec Shared directories.
// -- Removed some commented code
// -- Added symtdat.dat to the list of files to remove on uninstall.
//
//    Rev 1.17   18 Jul 1997 16:26:08   JALLEE
// Now don't forget to delete the temporary reg key.  Bug introduced last rev.
//
//    Rev 1.16   18 Jul 1997 12:36:08   JALLEE
// Replaced call to clean up the Symevnt stuff, blown away in previous change.
//
//    Rev 1.15   18 Jul 1997 11:34:38   JALLEE
// Ported updated registry and usage count functions from BORG.  Many changes, but
// we should now be able to share updates/bug fixes with BORG much more easily.
//
//    Rev 1.14   17 Jul 1997 21:04:20   jtaylor
// -- Added a global variable to hold the Symantec Shared directory.
// -- Added a call to call the symevent installer to uninstall symevent.
// -- Added a call to try and delete the prog files\comm files\symtantec shared
//    directory.
// -- Modified all calls and definitions of registry functions that stored the
//    paths for uninstall to include the symantec shared directory.
//
//    Rev 1.13   14 Jul 1997 23:11:24   JALLEE
// Added call to RemoveSched() to shutdown the scheduler.
//
//    Rev 1.12   11 Jul 1997 13:56:10   jtaylor
// -- Removed an int 3 instruction that was there for debugging.
//
//    Rev 1.11   10 Jul 1997 21:09:54   jtaylor
// -- Replaced the RemoveLine function with a new implementation.
//
//    Rev 1.10   09 Jul 1997 09:59:12   jtaylor
// -- Moved RemoveLine and GetBootDrive to the proper locations.
// -- Added comments for them.
// -- Made RemoveLine take and uppercase string, and convert all file
//    text to uppercase.
//
//    Rev 1.9   08 Jul 1997 16:49:36   TIVANOV
// get the boot drive from the regestry
//
//    Rev 1.8   08 Jul 1997 15:09:14   TIVANOV
// remove a line from autoexec.bat
//
//    Rev 1.7   06 Jul 1997 19:32:44   jtaylor
// -- Fixed the name of the symevnt reg key in the delete call.
// -- Changed the decrement usage counts to the 95 keys.
// -- Changed the name of the .ulg file to delete to isnav95.ulg
//
//    Rev 1.6   01 Jul 1997 23:54:24   jtaylor
// -- converted to Unicode
// -- Added better removal for LiveUpdate
// -- Added removal of some more reg keys
//    navap vxd key
//    navap run key
//    symevent.386 key when appropriate.
// -- Added better file cleanup
// -- Added a list of files to cleanup on uninstall.
// -- Made the calls to uninstallvirus defs use the defined appIDS
//
//    Rev 1.5   19 Jun 1997 18:20:12   jtaylor
// -- removed the call to DecrimentUsageCounts for LiveUpdate files.
//
//    Rev 1.4   10 Jun 1997 21:30:52   jtaylor
// Removed the service cleanup calls.
// Updated the location of the install reg key.
//
//
//    Rev 1.3   08 Jun 1997 20:59:38   jtaylor
// Updated the virus definition usage and removal calls for Hawking.
//
//    Rev 1.2   02 Jun 1997 01:38:26   jtaylor
// Added Hawking support.  Cleaned up some garbage.
//
//    Rev 1.1   27 May 1997 21:15:42   JTAYLOR
// Update pulled from navinstnt
//
//    Rev 1.3   17 May 1997 19:15:00   SEDWARD
// Explicitly remove the temporary registry key.
//
//    Rev 1.2   13 May 1997 19:15:32   SEDWARD
// We're not going to use the AT scheduler, so the call to remove scheduled
// scans is disabled (until we implement an equivalent function to handle
// the Norton Program Scheduler's way of doing things).
//
//    Rev 1.1   11 May 1997 21:33:36   SEDWARD
// Changed SI_MAX_PATH to SYM_MAX_PATH; added 'RemoveScheduledScans';
// added 'CreateMifFile'; added 'NavUninstallSucceeded'; added
// 'WriteNavPath' and 'ReadNavPath'.
//
//    Rev 1.0   05 May 1997 19:24:56   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////



#include <windows.h>
#include <windowsx.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <io.h>
#include <fstream.h>
#include <shlobj.h>                     // for shell folder functions

#pragma hdrstop

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include    "platform.h"
#include    "resource.h"
#include    "svcctrl.h"
#include    "IsUtil.h"
#include    "cwstring.h"
#include    "NavInst.h"
#include    "NavInstMIF.h"
#include    "DefUtils.h"
#include    "CRegent.h"
#include    "CUsgcnt.h"

#define     INITGUID
#include    "iquaran.h"
#include    "quar32.h"

// global variables
static  char*   g_szNavFiles[] =
{
    "runsched.exe"
    , "$flecomp.tmp"
    , "*.log"
    , "???9*.txt"
    , "??nav*.zip"
    , "_isnav95.ulg"
    , "activity.txt"
    , "defloc.dat"
    , "excludel.dat"
    , "install.inf"
    , "modem.id"
    , "nav95.*"                         // to delete backups of nav95.isu
    , "navex*.*"
    , "navor32.dll"
    , "ncsacert.txt"
    , "nwdata.dat"
    , "pipedlg.dat"
    , "results.txt"
    , "s32aloge.dll"
    , "s32nave.dll"
    , "schedule.bak"
    , "schedule.dat"
    , "symtdat.dat"
    , "update.txt"
    , "virinfo.txt"
    , "virlist.txt"
    , "virscanm.dat"
    , "virsp*.dat"
    , "virspec.txt"
    , "whatsnew.txt"
    , ""
};

static  char*   g_szNavSystemFiles[] =
{
    "navwnt.*"
    , "navwscn.fts"
    , "navwscn.gid"
    , ""
};


static  char*   g_szLiveUpdateFiles[] =
{
    "liveupdt.hct"
    , "hscript.scp"
    , "liveupdt.hst"
    , "s32luhp1.hlp"
    , "s32luhp1.cnt"
    , ""
};


static  char*   g_szLiveUpdateWinsysFiles[] =
{
    "s32LuCp1.cpl"
    , ""
};



// migration files
static  TCHAR*  g_szRoswellMigratedStartMenuLinks[] =
{
    "*.*"
    , ""
};

static  TCHAR*  g_szRoswellMigratedStartMenuLinkFolders[] =
{
    "Norton AntiVirus\\Product Support Online"
    , "Norton AntiVirus"
    , ""
};

static  TCHAR*  g_szRoswellMigratedStartMenuStartupLinks[] =
{
    "Norton Program Scheduler.lnk"
    , ""
};




// Ensure that we define and initialize the strings defined in NavAppIDs.h
// #define NAVAPPIDS_BUILDING

// Defines an array of NAV AppIds that uninstall can loop through.
#define NAVAPPIDS_WANT_ARRAY
#include "NavAppIds.h"



// SCOTT
// #include "ctsn.h"        // compilation problems...
// from ctsn.h...
// #define SIZE_VIRUSNAME      (21 + 1)        // This does NOT include EOS!
// #define SYM_MAX_PATH        256

#include "navapcom.h"

/////////////////////////////////////////////////////////////////////////////
//  Local Macros/Inlines
/////////////////////////////////////////////////////////////////////////////

VOID _inline FuncTrace( LPSTR lpFunc, BOOL bRet )
{
    LTrace( (LPSTR)"%s returns %s\n", lpFunc, bRet ? "TRUE" : "FALSE" );
}

void RemoveQuarantineContents ( HWND );
void RemoveHiddenHelpFiles ( HWND );
void RemoveSubmitBugReportFiles ( HWND );
void UnregisterCOMObjects();
void LNameAppendFile ( LPTSTR szPath, LPCTSTR szFile );
BOOL IsCorporateVersion();
BOOL IsSilentUninstall();


/////////////////////////////////////////////////////////////////////////////
//  UninstInitialize - Function that gets called by UNINST.EXE at the
//                     beginning of uninstall.
//
// Return values:
//  0 - Continue Uninstall
//  <0 - Abort Uninstall
//
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

extern "C" DllExport   LONG    UninstInitialize(HWND hwndDlg, HANDLE hInstance, LONG lRes)
{
    auto    CWString        szTemp(SYM_MAX_PATH);

#ifdef _LOGFILE_TRACE_
    GetWindowsDirectory(szTemp, SYM_MAX_PATH - 1);
    szTemp.AppendFile("SYMINST.LOG");
    DeleteFile(szTemp);
#endif

    // initialize the path variables
    GetNavPaths(g_szTarget, g_szSharedDir, g_szSymSharedDir);

    // save the NAV path temporarily to the registry so we can access
    // it later (NOTE: for some reason, unInstallShield is blowing away
    // this value after "UninstInitialize" and before "UninstUnInitialize")
    WriteNavPaths(g_szTarget, g_szSharedDir, g_szSymSharedDir);
    // create a default MIF file (one of "failure")
    CreateMifFile("Failure", "Uninstallation failed or incomplete");

    // if the AP agent is running, shut it down
    RemoveAPUI();

    // Shutdown scheduler if it is running.
    RemoveSched();

    // Delete any leftover stuff in Quarantine
    RemoveQuarantineContents ( hwndDlg );

    // Delete leftover WinHelp files;
    RemoveHiddenHelpFiles ( hwndDlg );

    // decrement usage counts on shared files handled by us (uses our
    // uninstall log file)
    ProcessUsageCountsFile("SymShared", LOGFILE_NAME );

    // call LiveUpdate's own cleanup function
    CleanUpOnRamp();

    // cleanup the virus definition files
    UninstallVirusDefinitions();

    // cleanup the autoexec.bat file
    char filename[SYM_MAX_PATH - 1];
    GetBootDir(filename, sizeof(filename));
    STRCAT(filename, _T("autoexec.bat"));

    RemoveLine(filename, _T("navdx.exe"));

    // cleanup any Netscape plugin keys
    szTemp = g_szTarget;
    szTemp.AppendFile(NSPLUGIN_EXE);
    _tcscat(szTemp, " /s /u");
    LaunchAndWait(NULL, szTemp);

    // Remove the "submit a bug report"-related stuff
    RemoveSubmitBugReportFiles ( hwndDlg );

    // Remove any NAV Scan or NAVLU tasks from the MS Task Scheduler
    szTemp = g_szTarget;
    szTemp.AppendFile(NTASKMGR_EXE);
    _tcscat(szTemp, " /d");
    LaunchAndWait(NULL, szTemp);

    // clean up the symevent stuff
    szTemp = g_szSymSharedDir;
    szTemp.AppendFile(SEVINST_EXE);
    _tcscat(szTemp, " /Q /U NAV95");
    LaunchAndWait(NULL, szTemp);

    // uninstall the def annuity
    typedef DWORD (__stdcall *PDefAnnuityUninitialize) ( HWND hWnd );
    auto HINSTANCE hDAInst;
    auto PDefAnnuityUninitialize pfnDefAnnuityUninitialize;
    szTemp = g_szTarget;
    szTemp.AppendFile(DEFANNTY_DLL);
    if ( hDAInst = LoadLibrary ( szTemp ) )
    {
        if ( pfnDefAnnuityUninitialize = PDefAnnuityUninitialize ( GetProcAddress ( hDAInst, DEFANNTY_UNINITIALIZE_PROC ) ) )
            pfnDefAnnuityUninitialize ( NULL );
        FreeLibrary ( hDAInst );
    }

    // Delete the RegKey for BACKLOG if exists
	auto LONG  lResult;
	auto HKEY  hKey;

	lResult = RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &hKey);
	if(lResult == ERROR_SUCCESS)
    {
		lResult = RegDeleteValue(hKey, _T("BACKLOG"));
    }
	
    RegCloseKey(hKey);

    // Unregister Q, SND, and ABOUTSW COM objects.
    UnregisterCOMObjects();

    return(0);

}   // end of "UninstInitialize"




/////////////////////////////////////
//  UninstUninitialize - Function that gets called by UNINST.EXE at the
//                       end of uninstall.
//
// Return values:
//  NOT IMPORTANT
//
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

extern "C" DllExport   LONG    UninstUnInitialize(HWND hwndDlg, HANDLE hInstance, LONG lRes)
{
    auto    DWORD           dwValues;
    auto    DWORD           dwKeys;
    auto    CRegistryEntry *cReg;

    // fetch the path where NAV used to be
    if (TRUE == ReadNavPaths(g_szTarget, g_szSharedDir, g_szSymSharedDir))
        {
        auto    CWString        szTemp(SYM_MAX_PATH);

        // If the number of keys is greater than zero and the number of values is
        // less than or equal to two, remove the Symantec shared usage key

        // cleanup any shared files' registry entries
        CleanupSharedSymantecRegEntries();

        // if this is a migrated installation, we need to do some extra cleanup
        if (TRUE == IsMigratedUninstall())
            {
            CleanupMigratedInstallation();
            }

        cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, "Software\\Symantec\\SharedUsage" );
        dwKeys = cReg->GetSubkeyCount();
        dwValues = cReg->GetValueCount();
        if (!dwKeys && (dwValues <= 2))
            {
            cReg->DeleteKey();
            }

        // If there are no values left under the installed apps key, remove it
        cReg->SetSubkey( "Software\\Symantec\\InstalledApps" );
        dwKeys = cReg->GetSubkeyCount();
        dwValues = cReg->GetValueCount();
        if ( !dwValues )
            {
            cReg->DeleteKey();
            }

        cReg->ResetKey( HKEY_CURRENT_USER, "Software\\Symantec\\Norton AntiVirus" );
        cReg->DeleteKey();

        // remove any leftover data files InstallShield doesn't know about
        RemoveLeftoverDataFiles();

        // if the LiveUpdate shared DLLs have been removed, be sure to cleanup the
        // "non-shared" files as well
        CleanupLiveUpdate(g_szSharedDir);

        // Blow away the symantec shared directory
        szTemp = g_szSymSharedDir;
        ISRemoveDirectory(g_szSymSharedDir);

        // Blow away the symantec directory
        szTemp = g_szSharedDir;
        ISRemoveDirectory(g_szSharedDir);

        // if we succeeded in uninstallation, write out a "success" MIF file
        if (TRUE == NavUninstallSucceeded())
            {
            CreateMifFile("Success", "Uninstallation succeeded");
            }

        // get rid of the temporary reg key
        cReg->ResetKey(HKEY_LOCAL_MACHINE, TEMP_REGKEY_PATH);
        cReg->DeleteKey();
        }

    // Get rid of the Symantec Reg keys.
    cReg->ResetKey(HKEY_LOCAL_MACHINE, "Software\\Symantec" );

    dwKeys = cReg->GetSubkeyCount();

    if ( !dwKeys )
        cReg->DeleteKey();

    cReg->ResetKey( HKEY_CURRENT_USER, "Software\\Symantec" );

    dwKeys = cReg->GetSubkeyCount();

    if ( !dwKeys )
        cReg->DeleteKey();

    delete cReg;

    return(0);

}   // end of "UninstUnInitialize"



// ===== CleanupLiveUpdate ================================================
//
//  This function removes any leftover LiveUpdate files if the shared DLLs
//  have indeed been removed from the system.
//
//  Input:
//      lpSymantecPath  -- the path to the Symantec directory (where the
//                         LiveUpdate subdirectory lives)
//
//  Output:
//      a value of TRUE if we removed the rest of the LiveUpdate files,
//      FALSE if not (ie, there are other clients of LiveUpdate still
//      using them)
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

BOOL    CleanupLiveUpdate(LPTSTR  lpSymantecPath)
{
    auto    BOOL            bResult = FALSE;
    auto    CWString        szPath(SYM_MAX_PATH);

    //  if the LiveUpdate shared DLLs have been removed, pull the other LU
    // files (those without usage counts) as well
    _tcscpy(szPath, g_szSharedDir);
    _tcscat(szPath, "\\LiveUpdate\\");
    _tcscat(szPath, KEY_LIVE_UPDATE_DLL);
    if (FALSE == FileExists(szPath))
        {
        _tcscpy(szPath, g_szSharedDir);
        _tcscat(szPath, "\\LiveUpdate");
        RemoveTargetFiles(szPath, g_szLiveUpdateFiles, TRUE);

        // remove the control panel applet
        if (GetSystemDirectory(szPath, SYM_MAX_PATH))
            {
            // "FALSE" means don't remove the subdirectory if empty
            RemoveTargetFiles(szPath, g_szLiveUpdateWinsysFiles, FALSE);
            bResult = TRUE;
            }
        }

    return (bResult);

}  // end of "CleanupLiveUpdate"



/////////////////////////////////////////////////////////////////////////////
//
//  CleanUpOnRamp - Function to call OnRampCleanUpEx in S32LIVE1.DLL to
//                  let LiveUpdate clean up itself
//
//  Params: NONE
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL    CleanUpOnRamp(void)
{
    auto    BOOL            bRet = FALSE;
    auto    LONG            lResult = 0;
    auto    LPSTR           lpOnRampDll = "S32LIVE1.DLL";
    auto    HMODULE         hLib = NULL;
    auto    CWString        szLibPath(SYM_MAX_PATH);
    auto    CWString        szVersion(SYM_MAX_PATH);
    auto    CWString        szLanguage(SYM_MAX_PATH);
    auto    CWString        szProductName(SYM_MAX_PATH);
    auto    ONRAMPCLEANUPEX lpFunc = NULL;


    // create the full path to the LiveUpdate DLL
    GetNavPaths(g_szTarget, g_szSharedDir, g_szSymSharedDir);
    _tcscpy(szLibPath, g_szSharedDir);
    szLibPath.AppendFile("LiveUpdate");
    szLibPath.AppendFile(lpOnRampDll);

    // load the DLL and call the function to clean up after LiveUpdate
    hLib = LoadLibrary(szLibPath);
    if (hLib != NULL)
        {
        lpFunc = (ONRAMPCLEANUPEX)GetProcAddress(hLib, "OnrampCleanupEx");
        if (lpFunc != NULL)
            {
            szProductName   ="Avenge1.5 Definitions";
            szVersion       ="1.0";
            szLanguage      ="English";
            lResult = (lpFunc)(szProductName, szVersion, szLanguage);

            lResult = lpFunc ( "NAV95", "5.0", "English" );
            lResult = lpFunc ( "NAV95 NEC", "5.0", "English" );
            lResult = lpFunc ( "NAV95 Trial", "5.0", "English" );
            lResult = lpFunc ( "NAV95 NEC Trial", "5.0", "English" );

            szProductName   ="AV Engine Definitions";
            szVersion       ="1.0";
            szLanguage      ="English";
            lResult = (lpFunc)(szProductName, szVersion, szLanguage);

            bRet = TRUE;
            }

        FreeLibrary(hLib);
        }

    FuncTrace("CleanUpOnRamp", bRet);

    return(bRet);

}   // end of "CleanUpOnRamp"



/////////////////////////////////////////////////////////////////////////////
//
//  CleanupSharedSymantecRegEntries
//
//  Return value:   TRUE if all the paths are found
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

void    CleanupSharedSymantecRegEntries(void)
{
    auto    BOOL            bResult = TRUE;
    auto    CWString        szTemp(SYM_MAX_PATH);
    auto    CRegistryEntry *cReg;

    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, SYM_SHAREDUSAGE_KEY, "LiveUpdate1");

    // if LiveUpdate is really gone, then remove its registry entries under the
    // symantec key
    _tcscpy(szTemp, g_szSharedDir);
    szTemp.AppendFile("LiveUpdate");
    szTemp.AppendFile("S32LIVE1.DLL");
    if (!FileExists(szTemp))
        {
        _tcscpy(szTemp, g_szSharedDir);
        szTemp.AppendFile("LiveUpdate");
        DeleteDirectoryContents(szTemp);
        cReg->DeleteValue();
        }

    // if infodesk is no longer being used, cleanup its registry entries
    _tcscpy(szTemp, g_szSharedDir);
    szTemp.AppendFile("INFODESK.DLL");
    if (!FileExists(szTemp))
        {
        cReg->ResetKey( HKEY_LOCAL_MACHINE, MS_HELP_KEY );
        cReg->SetValueName ( "INFODESK.DLL" );
        cReg->DeleteValue();
        cReg->SetValueName ( "INFODESK.HLP" );
        cReg->DeleteValue();
        cReg->SetValueName ( "INFODESK.CNT" );
        cReg->DeleteValue();
        }

    // remove symevent entries if it's gone
    _tcscpy(szTemp, g_szSharedDir);
    szTemp.AppendFile("SYMEVNT.386");
    if (!FileExists(szTemp))
        {
        cReg->ResetKey( HKEY_LOCAL_MACHINE
                            , "SYSTEM\\CurrentControlSet\\Services\\VxD\\Symevnt");
        cReg->DeleteKey();
        }

    // remove the AutoProtect entries
    cReg->ResetKey(HKEY_LOCAL_MACHINE,
                    "SYSTEM\\CurrentControlSet\\Services\\VxD\\NAVAP");
    cReg->DeleteKey();

    cReg->ResetKey(HKEY_LOCAL_MACHINE,
                    "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\Norton Auto-Protect");
    cReg->DeleteKey();

    delete cReg;

}  // end of "CleanupSharedSymantecRegEntries"




/////////////////////////////////////////////////////////////////////////////
//
//  CreateMifFile()
//
//  This routine creates the MIF file for BackOffice installations.
//
//  Params: lpStatus    - pointer to "success" or "failure" status string
//          lpMessage   - corresponding message to write
//
//  Return value:   TRUE if all the paths are found
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL    CreateMifFile(LPTSTR  lpStatus, LPTSTR  lpMessage)
{
    auto    BOOL            bRetVal = TRUE;
    auto    char            szCompany[SI_MAXSTR];
    auto    char            szDateTime[SI_MAXSTR];
    auto    char            szInstallStatusDesc[SI_MAXSTR];
    auto    char            szInstallStatus[SI_MAXSTR];
    auto    char            szPath[SYM_MAX_PATH];
    auto    char            szUserName[SI_MAXSTR];
    auto    DWORD           dwSize;
    auto    DWORD           dwType;
    auto    FILE*           filePtr;
    auto    int             nIndex = 0;
    auto    LPSTR           lpszPath = szPath;
    auto    SYSTEMTIME      SystemTime ;            // system time structure
    auto    CWString        szTemp( SYM_MAX_PATH );
    auto    CRegistryEntry *cReg;

    cReg = new CRegistryEntry();
    if ( NULL == cReg )
        goto ErrorExit;

    // get the current date and time
    GetLocalTime(&SystemTime);
    wsprintf(szDateTime, "%02u/%02u/%04u at %02u:%02u:%02u", SystemTime.wMonth
                                                            , SystemTime.wDay
                                                            , SystemTime.wYear
                                                            , SystemTime.wHour
                                                            , SystemTime.wMinute
                                                            , SystemTime.wSecond);

    // get the success/failure string, and the message string
    _tcscpy(szInstallStatus, lpStatus);
    _tcscpy(szInstallStatusDesc, lpMessage);

    // get the system's windows directory and create a full path to the MIF file
    GetWindowsDirectory(szPath, SYM_MAX_PATH);
    _tcscat(szPath, "\\");
    _tcscat(szPath, SMS_MIF_FILENAME);

    // open the MIF file for write access
    filePtr = fopen(szPath, "wt");
    if (NULL == filePtr)
        {
        LTrace("Unable to open MIF file: ( %s )", szPath);
        bRetVal = FALSE;
        goto  ErrorExit;
        }

    //  get the user name & company name
    cReg->ResetKey( HKEY_LOCAL_MACHINE, NAV_WINVERKEY, "RegisteredOwner");

    dwSize = SYM_MAX_PATH - 1;
    if (cReg->GetValue( &dwType, (LPBYTE)(LPSTR)szTemp, &dwSize ) == ERROR_SUCCESS)
        {
        _tcscpy(szUserName, szTemp);
        }

    // Get Organization
    cReg->ResetKey( HKEY_LOCAL_MACHINE, NAV_WINVERKEY, "RegisteredOrganization");

    dwSize = SYM_MAX_PATH - 1;
    if (cReg->GetValue( &dwType, (LPBYTE)(LPSTR)szTemp, &dwSize ) == ERROR_SUCCESS)
        {
        _tcscpy(szCompany, szTemp);
        }

    // now loop and fill blank buffers with product and MIF information
    nIndex = 0 ;
    while (NULL != *(g_szMIFStatements[nIndex]))
        {
        switch (nIndex)
            {
            case  IDX_MANUFACTURER :
                wsprintf(g_szMIFStatements[nIndex], "\"%s\"", "Symantec Corporation");
                break ;

            case  IDX_PRODUCT :
                wsprintf(g_szMIFStatements[nIndex], "\"%s\"", NAV_PRODUCT_NAME);
                break ;

            case  IDX_VERSION :
                wsprintf(g_szMIFStatements[nIndex], "\"%s\"", NAV_PRODUCT_VERSION);
                break ;

            case  IDX_USER :
                wsprintf(g_szMIFStatements[nIndex], "\"%s\"", szUserName);
                break ;

            case  IDX_COMPANY :
                wsprintf(g_szMIFStatements[nIndex], "\"%s\"", szCompany);
                break ;

            case  IDX_DATETIME :
                wsprintf(g_szMIFStatements[nIndex], "\"%s\"", szDateTime);
                break ;

            case  IDX_INSTSTAT :
                wsprintf(g_szMIFStatements[nIndex], "\"%s\"", szInstallStatus);
                break ;

            case  IDX_INSTSTATDESC :
                wsprintf(g_szMIFStatements[nIndex], "\"%s\"", szInstallStatusDesc);
                break ;

            default :
                break ;
            }

        // flush the current string to the MIF file
        auto    size_t  numBytes;
        auto    size_t  stringLen;
        numBytes = _ftprintf(filePtr, "%s", g_szMIFStatements[nIndex]);
        stringLen = _tcsclen(g_szMIFStatements[nIndex]);
        if (numBytes != (stringLen))
            {
            LTrace("Error writing to MIF file: ( %s )", g_szMIFStatements[nIndex]);
            bRetVal = FALSE;
            }

        ++nIndex ;

        }  // end of "while"



ErrorExit:

    // close the file
    if (NULL != filePtr)
        {
        fclose(filePtr);
        }

    // Free the reg entry

    if ( NULL != cReg )
        delete  cReg;

    return (bRetVal);

} // CreateMifFile



/////////////////////////////////////////////////////////////////////////////
//
//  GetNavPaths - Function to retrieve the various paths from the registry
//
//  Params: lpDrive - Pointer to buffer that will hold the Drive letter
//          size - The size of the buffer in bytes
//
/////////////////////////////////////////////////////////////////////////////
//  07/97 - TIVANOV, function created
/////////////////////////////////////////////////////////////////////////////
void GetBootDir(LPTSTR lpDrive, int size)
{
    HKEY hKey = 0;
    //MEMSET (szTempW, 0, size);
    memset (lpDrive, 0, size);

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SETUP"), 0, KEY_READ, &hKey))
    {
        RegQueryValueEx(hKey, _T("BootDir"), 0, 0, (LPBYTE)lpDrive, (LPDWORD)&size);
        RegCloseKey(hKey);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  GetNavPaths - Function to retrieve the various paths from the registry
//
//  Params: LPTSTR - Pointer to buffer that will hold the main NAV path
//          LPTSTR - Pointer to buffer that will hold the symantec shared path
//
//  Return value:   TRUE if all the paths are found
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
//  6/1/98  Mike Dunn -- Removed Extensions directory stuff, it's not used
//                       by Atomic.
/////////////////////////////////////////////////////////////////////////////

BOOL    GetNavPaths(LPTSTR lpNavPath,
                    LPTSTR lpSharedPath, LPTSTR lpSymSharedPath)
{
    auto    BOOL            bRet = FALSE;
    auto    CWString        szDataBuf(SYM_MAX_PATH);
    auto    DWORD           dwSize;
    auto    DWORD           dwType;
    auto    CRegistryEntry *cReg;


    if (lpNavPath != NULL && lpSharedPath != NULL)
        {
        *lpNavPath = *lpSharedPath = 0;

        cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, NAV_INSTALL_KEY, "InstallDir" );
        if ( NULL == cReg )
            return ( FALSE );

        dwSize = SYM_MAX_PATH - 1;
        if ( cReg->GetValue( &dwType, (LPBYTE)(LPSTR)szDataBuf,
                             &dwSize ) == ERROR_SUCCESS )
        {
            lstrcpy( lpNavPath, szDataBuf );
            LTrace("GetNavPaths: lpNavPath = %s", lpNavPath);
        }


        cReg->SetValueName( "SharedComponents" );
        dwSize = SYM_MAX_PATH - 1;

        if ( cReg->GetValue( &dwType, (LPBYTE)(LPSTR)szDataBuf,
                             &dwSize ) == ERROR_SUCCESS )
        {
            lstrcpy( lpSharedPath, szDataBuf );
            LTrace("GetNavPaths: lpSharedPath = %s", lpSharedPath);

        }

        cReg->SetValueName( "SymantecShared" );
        dwSize = SYM_MAX_PATH - 1;

        if ( cReg->GetValue( &dwType, (LPBYTE)(LPSTR)szDataBuf,
                             &dwSize ) == ERROR_SUCCESS )
        {
            lstrcpy( lpSymSharedPath, szDataBuf );
            LTrace("GetNavPaths: lpSymSharedPath = %s", lpSymSharedPath);

        }

        if (*lpNavPath && *lpSharedPath && *lpSymSharedPath)
            {
            bRet = TRUE;
            }
        delete cReg;
        }

    return(bRet);

}  // end of "GetNavPaths"



// ==== GetServicePath ====================================================
//
//  This function uses the service name to fetch the fully-qualified path
//  to its file from the registry.
//
//  Input:
//      lpszServiceName     -- the name of the target service
//      strBuf              -- buffer to store the fully-qualified path
//
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

BOOL    GetServicePath(LPTSTR  lpszServiceName, LPTSTR  strBuf)
{
    return (FALSE);
}  // end of "GetServicePath"



// ==== GetUsageCount =====================================================
//
//  This exported function fetches the current usage count for the target
//  item from the registry (HKLM\SW\MS\Windows\CurrentVersion\SharedDLLs).
//
//
//  Input:
//      lpszItemName        -- the display name of the service
//      dwCurrentUsageCount -- a reference to a usage count variable
//
//  Output: a value of TRUE if all goes well, FALSE if we can't get the
//          value from the registry
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

extern "C" DllExport   BOOL     GetUsageCount(LPTSTR  lpszItemName
                                                , DWORD&  dwCurrentUsageCount)
{
    auto    BOOL            bRetVal;
    auto    CWString        szTemp(SYM_MAX_PATH);
    auto    DWORD           dwSize;
    auto    DWORD           dwType;
    auto    CRegistryEntry *cReg;

    cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, MS_DLLS_KEY, lpszItemName );
    if ( NULL == cReg )
        return ( FALSE );

    dwSize = SYM_MAX_PATH -1;
        // get current MS shared usage count
    if (ERROR_SUCCESS == cReg->GetValue ( &dwType, (LPBYTE)(LPSTR)szTemp, &dwSize))
        {
        bRetVal = TRUE;

        if (REG_SZ == dwType)
            {
            dwCurrentUsageCount = strtoul(szTemp, NULL, 10);
            }
        else
            {
            dwCurrentUsageCount = *((LPDWORD)(LPSTR)szTemp);
            }
        }
    else
        {
        dwCurrentUsageCount = 0;
        bRetVal = FALSE;
        }

    delete cReg;
    return (bRetVal);

}  // end of "GetUsageCount"



// === NavUninstallSucceeded ==============================================
//
//  This function performs a quick check to see if NAV was successfully
//  uninstalled.
//
//  Input:  nothing
//  Output: TRUE if NAV was uninstalled, FALSE if not
//
// ========================================================================
//  Function created: 5/97, SEDWARD
// ========================================================================

BOOL    NavUninstallSucceeded(void)
{
    auto    BOOL        bResult = TRUE;
    auto    CWString    szPath(SYM_MAX_PATH);
    auto    HKEY        hRegKey;
    auto    LONG        lResult;

    // first see if the EXE still exists
    _tcscpy(szPath, g_szTarget);
    _tcscat(szPath, NAV_EXE);
    if (TRUE == FileExists(szPath))
        {
        bResult = FALSE;
        }

    // next, see if the main regkey still exists
   lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NAV_INSTALL_KEY, 0
                                                    , KEY_READ, &hRegKey);
    if (ERROR_SUCCESS == lResult)
        {
        // we found the key, so uninstallation is not complete
        bResult = FALSE;
        }

    return (bResult);

}  // end of "NavUninstallSucceeded"



// === ReadNavPaths =======================================================
//
//  This function reads the path to NAV from the registry.  This is
//  necessary because InstallShield is blowing away the global buffer
//  that normally holds this value between UninstInitialize and
//  UninstUninitialize; if a way around this surfaces, this function may
//  no longer be necessary.
//
//  NOTE: it is the caller's responsibility to make sure the buffer size is
//        sufficient
//
//  Input:
//      navPathStr      -- a pointer to a buffer that is to contain the
//                         NAV path
//      symantecPathStr -- a pointer to a buffer that is to contain the
//                         Symantec shared path
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 5/97, SEDWARD
// ========================================================================

BOOL    ReadNavPaths(LPTSTR  navPathStr, LPTSTR  symantecPathStr, LPTSTR SymSharedPathStr)
{
    auto    BOOL        bResult = FALSE;
    auto    DWORD       dwSize;
    auto    DWORD       dwType;
    auto    CRegistryEntry *cReg;
    auto    CWString    szDataBuf;
    // open the temporary registry key

    cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, TEMP_REGKEY_PATH, TEMP_REGVAL_NAV_PATH );
    if ( NULL == cReg )
        return ( FALSE );


    if (ERROR_SUCCESS == cReg->OpenKey( KEY_READ ))
        {
        // read the NAV path

        dwSize = SYM_MAX_PATH -1;

        if ( ERROR_SUCCESS == cReg->GetValue( &dwType, (LPBYTE)(LPSTR)szDataBuf,
                             &dwSize ))
            {
            // copy the value data to the input argument buffer pointer
            lstrcpy( navPathStr, szDataBuf );
            }

        // read the Symantec path
        cReg->SetValueName ( TEMP_REGVAL_SYMANTEC_PATH );

        dwSize = SYM_MAX_PATH -1;

        if (ERROR_SUCCESS == cReg->GetValue(&dwType,
                                            (LPBYTE)(LPSTR)szDataBuf,
                                            &dwSize))
            {
            // copy the value data to the input argument buffer pointer
            lstrcpy(symantecPathStr, szDataBuf);
            }

        // read the Symantec shared path
        cReg->SetValueName (  TEMP_REGVAL_SYMANTEC_SHARED_PATH );

        dwSize = SYM_MAX_PATH -1;

        if (ERROR_SUCCESS == cReg->GetValue(&dwType,
                                            (LPBYTE)(LPSTR)szDataBuf,
                                            &dwSize))
            {
            // copy the value data to the input argument buffer pointer
            lstrcpy(SymSharedPathStr, szDataBuf);
            }

        bResult = TRUE;
        }

    delete cReg;
    return (bResult);

}  // end of "ReadNavPaths"



// === RemoveLeftoverDataFiles ============================================
//
//  This function removes leftover files generated outside the awareness of
//  InstallShield (ie, either logging was disabled during the install or
//  they are files generated by NAV).  Arrays of filenames containing
//  wildcarded specifications are used to filter only the files we're
//  interested in.
//
//  Input:  nothing
//  Output: if all goes well, the total number of files deleted (or marked
//          for deletion) is returned; otherwise, a value of -1 is returned
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

int     RemoveLeftoverDataFiles(void)
{
    auto    CWString        szPath(SYM_MAX_PATH);
    auto    int             numFiles = -1;

    if (NULL != *g_szTarget)
        {
        numFiles = 0;

        // nav system files
        _tcscpy(szPath, g_szTarget);
        _tcscat(szPath, "\\System");
        numFiles += RemoveTargetFiles(szPath, g_szNavSystemFiles, TRUE);

        // nav files
        _tcscpy(szPath, g_szTarget);
        numFiles = RemoveTargetFiles(szPath, g_szNavFiles, TRUE);
        }

    return (numFiles);

}  // end of "RemoveLeftoverDataFiles"


// === RemoveLine =======================================================
//
//	This function removes all lines in [filename] that contain the string
//  [string]
//
//  NOTE: This function is NOT case sensitive.
//
//  Input:
//      string		-- This is a pointer to the string to search for
//      filename    -- This is the complete path and filename of the file
//						to remove lines from.
//
//  Output:
//      TRUE if all lines containing [string] were removed, FALSE otherwise
//
// ========================================================================
//  Function created: 7/97, JTAYLOR
// ========================================================================
BOOL RemoveLine(LPCTSTR filename, LPCTSTR string)
{
	BOOL	bFirstRun = TRUE;
	BOOL	bNotAtEnd = TRUE;
	BOOL	bReadSuccess = FALSE;
	DWORD	dwOldFileSize = 0;
	DWORD	dwReadSize = 0;
	DWORD	WrittenBytes = 0;
	HANDLE	hFile;
	LPBYTE	NewFile = NULL;
	LPBYTE	OldFile = NULL;
	LPSTR	pSubStr = NULL;

	// Create a list of the seperating characters
	char seps[] = "\n  ";
	seps[1] = EOF;
	seps[2] = -3;

	hFile = CreateFile(	filename,	// pointer to name of the file
						GENERIC_READ | GENERIC_WRITE,	// access (read-write) mode
						FILE_SHARE_READ,	// share mode
						NULL,	// pointer to security descriptor
						OPEN_EXISTING,	// how to create
						0,	// file attributes
						NULL 	// handle to file with attributes to copy
						);

	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	dwOldFileSize = GetFileSize(hFile, NULL);

	// Allocate the buffers for files.
	OldFile = new BYTE[dwOldFileSize+10];
	if(NULL==OldFile)
		{
		return FALSE;
		}

	// Create a little extra space since it will add a '\n'
	// If the EOF occurs at the end of a line.
	NewFile = new BYTE[dwOldFileSize+10];
	if(NULL==NewFile)
		{
		delete[] OldFile;
		return FALSE;
		}

	// Clear the buffers.
	memset(NewFile, 0, dwOldFileSize+10);
	memset(OldFile, 0, dwOldFileSize+10);

	// Read in the file
	bReadSuccess = ReadFile(	hFile,	// handle of file to read
								OldFile,	// address of buffer that receives data
								dwOldFileSize,	// number of bytes to read
								&dwReadSize,	// address of number of bytes read
								NULL 	// address of structure for data
								);

	// Make sure that all the data was read in.
	if(FALSE == bReadSuccess || dwOldFileSize!=dwReadSize)
		{
		// Delete the buffers and close the file
		delete[] NewFile;
		delete[] OldFile;
		CloseHandle(hFile);
		return FALSE;
		}

	// Copy and conver string over to uppercase.
	pSubStr = _tcsdup(string);
	if(NULL == pSubStr)
		{
		// Delete the buffers and close the file
		delete[] NewFile;
		delete[] OldFile;
		CloseHandle(hFile);
		return FALSE;
		}
	_tcsupr(pSubStr);

	// Loop through the file analysing it one line at a time.
	while( bNotAtEnd )
		{
		LPSTR pLine = NULL;

		if(bFirstRun)
			{
			pLine = _tcstok( (LPTSTR)OldFile, seps );
			}
		else
			pLine = _tcstok( NULL, seps );

		// Check to see if it was allocated ok.
		if( pLine == NULL )
			{
			bNotAtEnd = FALSE;
			}
		else
			{
			// Make a duplicate
			LPSTR pCompare = _tcsdup(pLine);
			// Convert to upper.
			_tcsupr(pCompare);
			// Check to see if the substring exists.
			LPSTR pSearch = _tcsstr(pCompare,pSubStr);
			if( pSearch == NULL )
				{
				// Append the string to the new file.
				_tcscat( (LPTSTR)NewFile, pLine);
				_tcscat( (LPTSTR)NewFile, "\n");
				}

			bFirstRun = FALSE;

			free(pCompare);
			}

		}

	SetFilePointer(	hFile,		// handle of file
					0,			// number of bytes to move file pointer
					NULL,		// address of high-order word of distance to move
					FILE_BEGIN	// how to move
				    );

	BOOL bSuccess = WriteFile(	hFile,
								NewFile,
								_tcslen((LPTSTR)NewFile),
								&WrittenBytes,
								NULL
								);

	// Check to see if the whole file was written to disk.
	if(_tcslen((LPTSTR)NewFile)!=WrittenBytes)
		bSuccess = FALSE;
	if(FALSE == SetEndOfFile(hFile))
		bSuccess = FALSE;
	if(FALSE == FlushFileBuffers(hFile))
		bSuccess = FALSE;

	// Delete the buffers and close the file
	free(pSubStr);
	delete[] NewFile;
	delete[] OldFile;
	CloseHandle(hFile);

	return bSuccess;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    RemoveQuarantineContents()
//
// Description:
//  Checks the Quarantine directory, and if any items are there, asks
//  the user if he wants to delete them.  The Quarantine temp directory
//  is also cleaned out (regardless of whether main Q is cleaned out)
//  since it may contain live viruses.
//
// Input:
//  hDlg: [in] Uninstaller window handle.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 6/1/98  Mike Dunn -- function created
// 7/22/98 Mike Dunn -- changed behavior on corp installs to leave all
//                      files in the Quarantine dir
//////////////////////////////////////////////////////////////////////////

void RemoveQuarantineContents ( HWND hDlg )
{
IQuarantineDLL*         pIQuar;
IEnumQuarantineItems*   pIEnum;
IQuarantineItem*        pIQuarItem;
ULONG                   ulFetched;
BOOL                    bQuarEmpty = TRUE;
BOOL                    bCleanUpQuarDir = FALSE;
TCHAR                   szPrompt [MAX_PATH];
TCHAR                   szCaption[MAX_PATH];
TCHAR                   szTempDir[MAX_PATH];
DWORD                   dwBufSize = sizeof(szTempDir);
HKEY                    hQuarKey;
SHFILEOPSTRUCT          rFileOp;

    ZeroMemory ( szTempDir, sizeof(szTempDir) );

                                        // Clean out the Q temp dir
    if ( ERROR_SUCCESS == RegOpenKeyEx (
                              HKEY_LOCAL_MACHINE,
                              QUAR_KEY, 0, KEY_QUERY_VALUE, &hQuarKey ) )
        {
        if ( ERROR_SUCCESS == RegQueryValueEx (
                                  hQuarKey, QUAR_TEMP_PATH_VALUE, NULL,
                                  NULL, (LPBYTE) szTempDir, &dwBufSize ) )
            {
                                        // Add *.* wildcard to the temp path
            LNameAppendFile ( szTempDir, _T("*.*") );

            ZeroMemory ( &rFileOp, sizeof(SHFILEOPSTRUCT) );

            rFileOp.hwnd   = hDlg;
            rFileOp.wFunc  = FO_DELETE;
            rFileOp.pFrom  = szTempDir;
            rFileOp.fFlags = FOF_FILESONLY | FOF_NOCONFIRMATION |
                             FOF_NOERRORUI | FOF_SILENT;

            SHFileOperation ( &rFileOp );
            }
        }

    // On corporate installs, we leave all files in Quarantine and don't
    // prompt.  Brian says this is The Right Thing To Do!
    // New for Barstow - leave Quarantine alone if the uninstall is being
    // run silently.
    if ( IsCorporateVersion() || IsSilentUninstall() )
        return;


    if ( SUCCEEDED ( CoInitialize(NULL) ))
        {
        if ( SUCCEEDED ( CoCreateInstance (
                             CLSID_QuarantineDLL,
                             NULL,
                             CLSCTX_INPROC_SERVER,
                             IID_QuarantineDLL,
                             (void**) &pIQuar ) ) )
            {
            if ( SUCCEEDED ( pIQuar->Initialize() ) )
                {
                if ( SUCCEEDED ( pIQuar->Enum( &pIEnum ) ) )
                    {
                    if ( S_OK == pIEnum->Next ( 1, &pIQuarItem, &ulFetched ) )
                        {
                                        // Something's in Quarantine.
                        bQuarEmpty = FALSE;
                        pIQuarItem->Release();
                        }

                    pIEnum->Release();
                    }
                }
            }
        else
            {
                                        // Couldn't get IQuarDLL pointer
            pIQuar = NULL;
            }
        }

                                        // If there is anything in Quar,
                                        // prompt the user to delete them.
                                        // If the user wants to, we enumerate
                                        // all Q items and delete them one
                                        // by one.  (The IQuarDll function
                                        // DeleteAllItems() isn't
                                        // implemented.) :(
    if ( !bQuarEmpty )
        {
        bCleanUpQuarDir = TRUE;

        if ( LoadString ( g_hInst, IDS_CLEAN_QUAR_PROMPT,
                            szPrompt, sizeof(szPrompt) ) &&
             LoadString ( g_hInst, IDS_CLEAN_QUAR_CAPTION,
                            szCaption, sizeof(szCaption) ))
            {
            if ( IDNO == MessageBox ( hDlg, szPrompt, szCaption,
                                      MB_ICONQUESTION | MB_YESNO ) )
                {
                bCleanUpQuarDir = FALSE;
                }
            }
        }


    if ( bCleanUpQuarDir )
        {
        if ( SUCCEEDED ( pIQuar->Enum ( &pIEnum ) ) )
            {
            while ( S_OK == pIEnum->Next ( 1, &pIQuarItem,
                                           &ulFetched ) )
                {
                pIQuarItem->DeleteItem();   // nuke it
                pIQuarItem->Release();      // free the interface
                }

            pIEnum->Release();
            }
        }

    if ( NULL != pIQuar )
        pIQuar->Release();

    CoUninitialize();
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    IsCorporateVersion()
//
// Description:
//  Tests whether the installed NAV has the corp version registry entry
//  set.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if NAV is the corp version, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 6/2/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL IsCorporateVersion()
{
BOOL    bIsCorp = FALSE;
HKEY    hkey;
DWORD   dwValue;
DWORD   dwBufSize = sizeof(DWORD);
DWORD   dwType;

    if ( ERROR_SUCCESS == RegOpenKeyEx (
                              HKEY_LOCAL_MACHINE,
                              CORP_FEATURES_KEY, 0, KEY_QUERY_VALUE,
                              &hkey ) )
        {
        if ( ERROR_SUCCESS == RegQueryValueEx (
                                  hkey, CORP_FEATURES_VALUE,
                                  NULL, &dwType, (LPBYTE) &dwValue,
                                  &dwBufSize ) )
            {
            if ( REG_DWORD == dwType  &&  1 == dwValue )
                bIsCorp = TRUE;
            }

        RegCloseKey ( hkey );
        }

    return bIsCorp;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    IsSilentUninstall()
//
// Description:
//  Checks a reg entry put in by NAVUSTUB.EXE to determine whether uninstall
//  is being run in silent mode (-a -y switches).
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if the uninstall is silent, FALSE if not.
//
// Note:
//   The reg value name is hard-coded here and in NavUStub.
//
//////////////////////////////////////////////////////////////////////////
// 10/15/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL IsSilentUninstall()
{
BOOL  bSilent = FALSE;
HKEY  hkey;
DWORD dwData, dwType, dwSize = sizeof(DWORD);

    if ( ERROR_SUCCESS ==
             RegOpenKeyEx ( HKEY_LOCAL_MACHINE, NAV_KEY, 0, 
                            KEY_QUERY_VALUE, &hkey ) )
        {
        if ( ERROR_SUCCESS ==
                 RegQueryValueEx ( hkey, _T("Silent Uninstall"), NULL,
                                   &dwType, (LPBYTE) &dwData, &dwSize ) )
            {
            if ( REG_DWORD == dwType && 0 != dwData )
                bSilent = TRUE;
            }

        RegCloseKey ( hkey );
        }

    return bSilent;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    RemoveHiddenHelpFiles
//
// Description:
//  Deletes .GID files from the NAV install dir, which are hidden and
//  created by WinHelp, and as such don't get deleted automagically by
//  InstallShield.
//
// Input:
//  hDlg: [in] Handle of uninstaller window.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
//  6/3/98  Mike Dunn -- function created
//  7/15/98 Mike Dunn -- fixed to work around apparent bug in Win 95
//                       SHFileOperation() API (see comments)
//////////////////////////////////////////////////////////////////////////

void RemoveHiddenHelpFiles ( HWND hDlg )
{
SHFILEOPSTRUCT rFileOp;
TCHAR          szFiles[MAX_PATH];

    ZeroMemory ( &rFileOp, sizeof(SHFILEOPSTRUCT) );
    ZeroMemory ( szFiles, sizeof(szFiles) );

    lstrcpy ( szFiles, g_szTarget );
    LNameAppendFile ( szFiles, _T("*.gid") );

    // First check if there are any .GID files around. It seems
    // that Win 95 isn't adhering to the FOF_NOERRORUI flag, cuz if there
    // are no GIDs, then you get an ugly error message box from
    // SHFileOperation().

HANDLE          hFind;
WIN32_FIND_DATA rFind;

    hFind = FindFirstFile ( szFiles, &rFind );

    if ( INVALID_HANDLE_VALUE != hFind )
        {
                                        // There's at least one GID file, so
                                        // go ahead and do the delete.
        FindClose ( hFind );

        rFileOp.hwnd   = hDlg;
        rFileOp.wFunc  = FO_DELETE;
        rFileOp.pFrom  = szFiles;
        rFileOp.fFlags = FOF_FILESONLY | FOF_NOCONFIRMATION |
                         FOF_NOERRORUI | FOF_SILENT;

        SHFileOperation ( &rFileOp );
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    RemoveSubmitBugReportFiles
//
// Description:
//  Deletes the .URL file generated by install in the NAV group (the
//  "submit a bug report" link).
//
// Input:
//  hwnd: [in] Uninstaller window handle.
//
// Returns:
//  Nothing.
//
// Notes:
//  This fn relies on two strings which MUST match the corresponding
//  strings in install!  Namely, the NAV group name and the text for
//  "Submit a Bug Report".
//
//  This fn only takes care of deleting the URL file, since if the dummy
//  HTML file and shortcut were created in the script, InstallShield will
//  remove those itself.
//
//////////////////////////////////////////////////////////////////////////
//  6/15/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void RemoveSubmitBugReportFiles ( HWND hwnd )
{
TCHAR        szPath [MAX_PATH];         // Path to the URL file
HRESULT      hr;
LPITEMIDLIST pidl = NULL;
LPMALLOC     pMalloc = NULL;

    hr = SHGetSpecialFolderLocation ( hwnd, CSIDL_PROGRAMS, &pidl );

    if ( NOERROR != hr )                // Couldn't get the Programs menu
        return;                         // location.

    if ( !SHGetPathFromIDList ( pidl, szPath ) )
        return;                         // Couldn't convert to a path.

                                        // Construct the path to the URL file.
    if ( NULL == GetString ( IDS_NAV_GROUP_NAME ) )
        goto bailout;

    LNameAppendFile ( szPath, g_szStringTableString );

    if ( NULL == GetString ( IDS_SUBMIT_A_BUG_REPORT ) )
        goto bailout;

    LNameAppendFile ( szPath, g_szStringTableString );

    DeleteFile ( szPath );

bailout:
    if ( NULL != pidl )
        {
        if ( NOERROR == SHGetMalloc ( &pMalloc ) )
            {
            pMalloc->Free ( pidl );
            pMalloc->Release();
            }
        }
}


// ===== RemoveTargetFiles ================================================
//
//  This function removes files from a target directory.  The input array
//  of filenames can contain wildcard characters.
//
//  Input:
//      pszTargetDir    -- directory where the target files live
//      filenameArray   -- an array of null-terminated strings (the last
//                         entry must contain a null string)
//
//  Output:
//      the number of files deleted (or marked for deletion)
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

int     RemoveTargetFiles(TCHAR*  pszTargetDir, TCHAR**  filenameArray
                                                            , BOOL  bRemoveDir)
{
    auto    DWORD               dwResult;
    auto    HANDLE              fileHandle;
    auto    int                 numDeletedFiles = 0;
    auto    TCHAR               szBuf[MAX_PATH];
    auto    WIN32_FIND_DATA     fileData;

    LTrace("BEGIN RemoveTargetFiles");

    // loop and process each string in the array
    while (NULL != **filenameArray)
        {
        LTrace("  *filenameArray = <%s>", *filenameArray);

        // assemble the file specification
        _tcscpy(szBuf, pszTargetDir);
        _tcscat(szBuf, "\\");
        _tcscat(szBuf, *filenameArray);

        // see if we get a hit
        fileHandle = FindFirstFile(szBuf, &fileData);
        if (INVALID_HANDLE_VALUE != fileHandle)
            {
            do  {
                // if we have a valid file, delete it
                if (TRUE == ValidFileTarget(fileData.dwFileAttributes))
                    {
                    // construct a string specific to the current filename
                    _tcscpy(szBuf, pszTargetDir);
                    _tcscat(szBuf, "\\");
                    _tcscat(szBuf, fileData.cFileName);

                    LTrace("  about to remove <%s>", szBuf);

                    dwResult = ISDeleteFile(szBuf);
                    if ((IS_FILE_DELETED == dwResult)  ||  (IS_FILE_IN_USE == dwResult))
                        {
                        ++numDeletedFiles;
                        LTrace("  succeeded in removing  <%s>, numDeletedFiles = %d", szBuf, numDeletedFiles);
                        }
                    else
                        {
                        LTrace("  failed to remove  <%s>", szBuf);
                        }
                    }

                // try to get another match from this string
                }  while (TRUE == FindNextFile(fileHandle, &fileData));
            }
        else
            {
            LTrace("  INVALID_HANDLE_VALUE == fileHandle");
            }
        ++filenameArray;
        }

    // if we're supposed to remove the directory, do so
    if (TRUE == bRemoveDir)
        {
        ISRemoveDirectory(pszTargetDir);
        }

    LTrace("  final count of numDeletedFiles = %d", numDeletedFiles);
    LTrace("END RemoveTargetFiles");
    return (numDeletedFiles);

}  // end of "RemoveTargetFiles"



// ==== UninstallVirusDefinitions =========================================
//
//  This function calls on a 'CDefUtils' object to uninstall the virus
//  definition files.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 5/97, SEDWARD
// ========================================================================

extern "C" DllExport    BOOL    UninstallVirusDefinitions(void)
{
    auto    BOOL            bRetValue = TRUE;
    auto    int             nCurAppId;
    auto    int             nNumAppIds;
    auto    LPCTSTR*        lpszAppIdArray;
    auto    TCHAR           szDataBuf[SYM_MAX_PATH];
    auto    DWORD           dwSize;
    auto    DWORD           dwType;
    auto    CRegistryEntry *cReg;

    cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, NAV_INSTALL_KEY, "InstallDir" );
    if (NULL==cReg)
        return (FALSE);

    dwSize = SYM_MAX_PATH - 1;
    if (cReg->GetValue( &dwType,
                        (LPBYTE)(LPSTR)szDataBuf,
                        &dwSize ) == ERROR_SUCCESS )
        {
        LTrace("UninstallVirusDefinitions: lpNavPath = %s", szDataBuf);
        }
    else
        return (FALSE);

    delete cReg;


    // create and initialize a def utils object (if successfully created, this
    // object will be destroyed in the "CopyVirusDefinitionFiles" function
    defObjectPtr = new  CDefUtils;
    if (NULL == defObjectPtr)
        {
         bRetValue = FALSE;
         goto  Exit_Function;
        }

    // loop through the app IDs (defined in "NavAppIds.h") and have the object
    // remove the virus definition files
    lpszAppIdArray = g_lpszNavAppId95Array;
    nNumAppIds = g_ucNavAppId95ArrayNum;
    for (nCurAppId = 0; nCurAppId < nNumAppIds; ++nCurAppId)
        {
        if( defObjectPtr->InitInstallApp(lpszAppIdArray[nCurAppId],szDataBuf) )
            defObjectPtr->StopUsingDefs();
        }

    // destroy the object
    delete  defObjectPtr;


Exit_Function:

    return (bRetValue);

}  // end of "UninstallVirusDefinitions"


//////////////////////////////////////////////////////////////////////////
//
// Function:    UnregisterCOMObjects()
//
// Description:
//  Calls DllUnregisterServer() in the Q and SND DLLs so they can remove
//  their GUIDs from the registry.  InstallShield is _supposed_ to do
//  this for us, but it just ain't happenin'.  A bug in InstallShield
//  maybe?  Nah.....
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 7/24/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

typedef HRESULT (STDAPICALLTYPE* LPFN_DLLUNREG)(void);

void UnregisterCOMObjects()
{
HINSTANCE     hInst;
TCHAR         szCurrDir [MAX_PATH];
LPFN_DLLUNREG lpfnUnregister;

    // Save the current dir, and change to the NAV dir so we can load
    // up the DLLs.
    GetCurrentDirectory ( MAX_PATH, szCurrDir );
    SetCurrentDirectory ( g_szTarget );

                                        // Load up QUAR32.DLL and unregister it.

    hInst = LoadLibrary ( _T(".\\quar32.dll") );

    if ( NULL != hInst )
        {
        lpfnUnregister = (LPFN_DLLUNREG) GetProcAddress (
                                             hInst,
                                             _T("DllUnregisterServer") );

        if ( NULL != lpfnUnregister )
            lpfnUnregister();

        FreeLibrary ( hInst );
        }

                                        // Load up SCANDLVR.DLL and unregister it.

    hInst = LoadLibrary ( _T(".\\scandlvr.dll") );

    if ( NULL != hInst )
        {
        lpfnUnregister = (LPFN_DLLUNREG) GetProcAddress (
                                             hInst,
                                             _T("DllUnregisterServer") );

        if ( NULL != lpfnUnregister )
            lpfnUnregister();

        FreeLibrary ( hInst );
        }

                                            // Load up ABOUTSW.DLL and unregister it.

    hInst = LoadLibrary ( _T(".\\aboutsw.dll") );

    if ( NULL != hInst )
        {
        lpfnUnregister = (LPFN_DLLUNREG) GetProcAddress (
                                             hInst,
                                             _T("DllUnregisterServer") );

        if ( NULL != lpfnUnregister )
            lpfnUnregister();

        FreeLibrary ( hInst );
        }


                                        // Restore original working directory.
    SetCurrentDirectory ( szCurrDir );
}


// ===== ValidFileTarget ==================================================
//
//  This function uses the input file attributes (returned from a call to
//  FindFirstFile or FindNextFile) to determine if the target file is one
//  we want to remove from the system.
//
//  Input:
//      fileAttributes  -- a DWORD of file attributes (returned from a call
//                         to FindFirstFile or FindNextFile)
//
//  Output:
//      a value of TRUE if the file is one we want to remove from the
//      system, FALSE if it is not
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

BOOL    ValidFileTarget(DWORD  fileAttributes)
{
    auto    BOOL        bValidFile;

    if  ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    ||  (fileAttributes & FILE_ATTRIBUTE_READONLY))
        {
        bValidFile = FALSE;
        }
    else
        {
        bValidFile = TRUE;
        }

    return (bValidFile);

}  // end of "ValidFileTarget"



// === WriteNavPaths ======================================================
//
//  This function saves the path to NAV temporarily to the registry.  This
//  is necessary because InstallShield is blowing away the global buffer
//  that normally holds this value between UninstInitialize and
//  UninstUninitialize; if a way around this surfaces, this function may
//  no longer be necessary.
//
//  Input:
//      navPathStr      -- a pointer to the full NAV path
//      symantecPathStr -- a pointer to the full Symantec path
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 5/97, SEDWARD
// ========================================================================

BOOL    WriteNavPaths(LPSTR  navPathStr, LPSTR  symantecPathStr,
                      LPSTR symantecSharedPathStr)
{
    auto    BOOL        bResult = TRUE;
    auto    CRegistryEntry *cReg;

    // create the temporary registry key
    cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, TEMP_REGKEY_PATH, TEMP_REGVAL_NAV_PATH );
    if ( NULL == cReg )
        return ( FALSE );

    if (ERROR_SUCCESS == cReg->OpenCreateKey( KEY_ALL_ACCESS ))
        {
        // write the value data
        cReg->SetValue(REG_SZ, (LPBYTE)navPathStr, _tcsclen(navPathStr) + 1);

        cReg->SetValueName( TEMP_REGVAL_SYMANTEC_PATH );
        cReg->SetValue(REG_SZ, (LPBYTE)symantecPathStr, _tcsclen(symantecPathStr) + 1);

        cReg->SetValueName( TEMP_REGVAL_SYMANTEC_SHARED_PATH );
        cReg->SetValue(REG_SZ, (LPBYTE)symantecSharedPathStr, _tcsclen(symantecSharedPathStr) + 1);

        cReg->CloseKey();
        }
    else
        bResult = FALSE;
    delete cReg;

    return (bResult);

}  // end of "WriteNavPaths"






// -=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-
// MIGRATION FUNCTIONS
// -=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-




// ==== IsMigratedUninstall ===============================================
//
//  This function checks to see if the current installation of NAV is a
//  "migrated" installation (meaning, it was installed on a Windows9x
//  system that has since been upgrade to Windows NT5).
//
//  Input:  nothing
//  Output: a value of TRUE if the installation of NAV has been migrated,
//          FALSE if it has not
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    IsMigratedUninstall(void)
{
    auto    BOOL            bResult = TRUE;;
    auto    CRegistryEntry* cReg;
    auto    CWString        szTemp(SYM_MAX_PATH);
    auto    DWORD           dwSize;
    auto    DWORD           dwType;


    LTrace("BEGIN IsMigratedUninstall");

    // if this installation has been migrated from Windows9x to NT5.0, then
    // we should find some bread crumbs in the registry
    szTemp = "Software\\Symantec\\Norton AntiVirus\\Install";
    szTemp += "\\5.3";  // our migration DLL only works with Atomic...

    LTrace("  About to create regkey object with <%s>", (LPSTR)szTemp);

    // create a registry object, referencing the "migrated" value; if the value
    // exists then we assume the installation has been migrated
    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, szTemp, "Migrated");
    dwSize = SYM_MAX_PATH - 1;
    if (cReg->GetValueInfo(&dwType, &dwSize ) == ERROR_SUCCESS)
        {
        LTrace("  bResult = TRUE");
        bResult = TRUE;
        }
    else
        {
        LTrace("  bResult = FALSE");
        bResult = FALSE;
        }

    LTrace("END IsMigratedUninstall");
    return  (bResult);

}  // end of "IsMigratedUninstall"



// ==== CleanupMigratedInstallation =======================================
//
//  This function cleans up a migrated installation of NAV ("migrated" meaning
//  a Windows9x upgrade to NT5).  If it is determined that the current
//  installation of NAV has been migrated from Windows9x, then this function
//  is called to do extra cleanup for the Win95 uninstall in an NT5
//  environment.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CleanupMigratedInstallation(void)
{
    auto    BOOL        bResult = TRUE;

    LTrace("BEGIN CleanupMigratedInstallation");

    // cleanup the registry
    if (FALSE == CleanupMigratedRegistry())
        {
        bResult = FALSE;
        }

    // cleanup files
    if (FALSE == CleanupMigratedSymEventFiles())
        {
        bResult = FALSE;
        }

    // cleanup the start menu
    CleanupMigratedStartMenuLinks();

    LTrace("END CleanupMigratedInstallation");
    return  (bResult);

}  // end of "CleanupMigratedInstallation"



// ==== CleanupMigratedRegistry ===========================================
//
//  This function calls other functions needed to cleanup the registry.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CleanupMigratedRegistry(void)
{
    auto    BOOL        bResult = TRUE;

    if (FALSE == CleanupMigratedMicrosoftSharedDLLsKeys())
        {
        bResult = FALSE;
        }

    if (FALSE == CleanupMigratedSymantecSharedUsageKeys())
        {
        bResult = FALSE;
        }

    return  (bResult);

}  // end of "CleanupMigratedRegistry"



// ==== CleanupMigratedMicrosoftSharedDLLsKeys ============================
//
//  This function will look in the Microsoft SharedDLLs key to see if we can
//  remove some migrated SymEvent files.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CleanupMigratedMicrosoftSharedDLLsKeys(void)
{
    auto    BOOL                bResult = TRUE;
    auto    CRegistryEntry*     cReg = NULL;
    auto    CWString            szTemp(SYM_MAX_PATH);
    auto    DWORD               dwVersionType;
    auto    DWORD               dwVersionSize;
    auto    DWORD               dwVersionData;


    LTrace("BEGIN CleanupMigratedMicrosoftSharedDLLsKeys");

    // SymEvnt.386
    GetWindowsDirectory(szTemp, SYM_MAX_PATH - 1);
    szTemp += "\\System";
    szTemp += "\\symevnt.386";

    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, MS_DLLS_KEY, (LPSTR)szTemp);
    dwVersionType = REG_DWORD;
    dwVersionSize = sizeof(dwVersionSize);
    if (ERROR_SUCCESS == cReg->GetValue(&dwVersionType, (LPBYTE)&dwVersionData
                                                                , &dwVersionSize))
        {
        if (1 == dwVersionData)
            {
            LTrace("  About to DeleteFile(%s)", (LPSTR)szTemp);
            DeleteFile(szTemp);
            cReg->DeleteValue();
            }
        }
    else
        {
        LTrace("  Failed to get registry value for %s", (LPSTR)szTemp);
        }

    // SymEvnt1.dll
    GetWindowsDirectory(szTemp, SYM_MAX_PATH - 1);
    szTemp += "\\System";
    szTemp += "\\symevnt1.dll";

    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, MS_DLLS_KEY, (LPSTR)szTemp);
    dwVersionType = REG_DWORD;
    dwVersionSize = sizeof(dwVersionSize);
    if (ERROR_SUCCESS == cReg->GetValue(&dwVersionType, (LPBYTE)&dwVersionData
                                                                , &dwVersionSize))
        {
        if (1 == dwVersionData)
            {
            LTrace("  About to DeleteFile(%s)", (LPSTR)szTemp);
            DeleteFile(szTemp);
            cReg->DeleteValue();
            }
        }
    else
        {
        LTrace("  Failed to get registry value for %s", (LPSTR)szTemp);
        }

    if (NULL != cReg)
        {
        delete  cReg;
        }

    LTrace("END CleanupMigratedMicrosoftSharedDLLsKeys");
    return  (bResult);

}  // end of "CleanupMigratedMicrosoftSharedDLLsKeys"



// ==== CleanupMigratedStartMenuLinks =====================================
//
//  This function removes any straggling start menu links.
//
//  Input:  nothing (uses global variables)
//  Output: nothing
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

VOID    CleanupMigratedStartMenuLinks(void)
{
    LTrace("BEGIN CleanupMigratedStartMenuLinks");

    DeleteNavStartMenuTargets(g_szRoswellMigratedStartMenuLinks
                                , g_szRoswellMigratedStartMenuLinkFolders
                                , STR_COMMON_PROGRAMS
                                , TRUE);

    // remove the Norton Program Scheduler from the "startup" group
    DeleteNavStartMenuTargets(g_szRoswellMigratedStartMenuStartupLinks
                                , NULL
                                , STR_COMMON_STARTUP
                                , FALSE);

    LTrace("END CleanupMigratedStartMenuLinks");

}  // end of "CleanupMigratedStartMenuLinks"



// ==== CleanupMigratedSymantecSharedUsageKeys ============================
//
//  This function cleans up any shared registry keys that have been left
//  behind from a migrated installation of NAV.  The keys in question are
//  under HKLM\SW\Symantec\SharedUsage.  Additionally, a cursory glimpse up
//  the branch will tell us if a little more cleanup is possible (ie, we're
//  the last Symantec application on the system).
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CleanupMigratedSymantecSharedUsageKeys(void)
{
    auto    BOOL                bResult = TRUE;
    auto    CRegistryEntry*     cReg;
    auto    CWString            szTemp;
    auto    DWORD               dwKeys;
    auto    DWORD               dwValues;


    LTrace("BEGIN CleanupMigratedSymantecSharedUsageKeys");

    szTemp = SYM_SHAREDUSAGE_KEY;
    szTemp += "\\s32evnt1.dll";
    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, szTemp, NULL);
    dwValues = cReg->GetValueCount();
    LTrace("  szTemp = %s, dwValue = %ld", (LPSTR)szTemp, dwValues);
    if (dwValues <= 1)
        {
        cReg->DeleteKey();
        LTrace("  deleted regkey %s", (LPSTR)szTemp);
        }

    szTemp = SYM_SHAREDUSAGE_KEY;
    szTemp += "\\s32stat.dll";
    cReg->ResetKey(HKEY_LOCAL_MACHINE, szTemp);
    dwValues = cReg->GetValueCount();
    LTrace("  szTemp = %s, dwValue = %ld", (LPSTR)szTemp, dwValues);
    if (dwValues <= 1)
        {
        cReg->DeleteKey();
        LTrace("  deleted regkey %s", (LPSTR)szTemp);
        }

    szTemp = SYM_SHAREDUSAGE_KEY;
    szTemp += "\\symevnt.386";
    cReg->ResetKey(HKEY_LOCAL_MACHINE, szTemp);
    dwValues = cReg->GetValueCount();
    LTrace("  szTemp = %s, dwValue = %ld", (LPSTR)szTemp, dwValues);
    if (dwValues <= 1)
        {
        cReg->DeleteKey();
        LTrace("  deleted regkey %s", (LPSTR)szTemp);
        }

    szTemp = SYM_SHAREDUSAGE_KEY;
    szTemp += "\\symevnt1.dll";
    cReg->ResetKey(HKEY_LOCAL_MACHINE, szTemp);
    dwValues = cReg->GetValueCount();
    LTrace("  szTemp = %s, dwValue = %ld", (LPSTR)szTemp, dwValues);
    if (dwValues <= 1)
        {
        cReg->DeleteKey();
        LTrace("  deleted regkey %s", (LPSTR)szTemp);
        }

    // remove the shared usage key if we're the last to use it
    cReg->ResetKey(HKEY_LOCAL_MACHINE, SYM_SHAREDUSAGE_KEY);
    dwKeys = cReg->GetSubkeyCount();
    dwValues = cReg->GetValueCount();
    LTrace("  SYM_SHARED_USAGE_KEY, dwKeys = %ld, dwValues = %ld", dwKeys, dwValues);
    if ((0 == dwKeys)  &&  (dwValues <= 2))
        {
        cReg->DeleteKey();
        LTrace("  deleted regkey %s", (LPSTR)SYM_SHAREDUSAGE_KEY);
        }

    // finally, remove the symantec key if there's nothing else under it
    cReg->ResetKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Symantec");
    dwKeys = cReg->GetSubkeyCount();
    LTrace("  SOFTWARE\\Symantec, dwValues = %ld", dwValues);
    if (0 == dwKeys)
        {
        cReg->DeleteKey();
        LTrace("  deleted regkey SOFTWARE\\Symantec");
        }

    LTrace("END CleanupMigratedSymantecSharedUsageKeys");
    return  (bResult);

}  // end of "CleanupMigratedSymantecSharedUsageKeys"



// ==== CleanupMigratedSymEventFiles ======================================
//
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CleanupMigratedSymEventFiles(void)
{
    auto    BOOL        bResult = TRUE;

    LTrace("BEGIN CleanupMigratedSymEventFiles");
    LTrace("END CleanupMigratedSymEventFiles");

    return  (bResult);

}  // end of "CleanupMigratedSymEventFiles"







// ==== DeleteNavStartMenuTargets =========================================
//
//  This function removes target items and folders from the Start Menu.  It
//  meant to be called to do extra cleanup for a migrated installation.
//
//  Input:
//      files       -- a pointer to an array of null-terminated strings
//                     containing the start menu files to be removed
//
//      folders     -- a pointer to an array of null-terminated strings
//                     containing the start menu folders to be removed
//
//      valueName   -- the name of the registry value used to fetch the
//                     target directory from the registry
//
//      bUseFolders -- a BOOL flag indicating whether or not the "folders"
//                     argument is to be used (ie, if the registry path
//                     we fetch already contains the folder we want to
//                     access, or we supply our own with the "folders"
//                     argument)
//
//  Output:
//      The total number of links and folders removed from the start menu
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

DWORD   DeleteNavStartMenuTargets(TCHAR**  files, TCHAR**  folders
                                                        , TCHAR*  valueName
                                                        , BOOL  bUseFolders)
{
    auto    CRegistryEntry      *cReg = NULL;
    auto    CWString            szFilename(SYM_MAX_PATH);
    auto    CWString            szLinksDir(SYM_MAX_PATH);
    auto    DWORD               dwSize = 0;
    auto    DWORD               dwTotalDeleted = 0;
    auto    DWORD               dwType = 0;
    auto    INT                 index = 0;


    LTrace("BEGIN DeleteNavStartMenuTargets()");

    // get the path for start menu links and remove all link files
    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, MS_SHELL_FOLDERS_KEY, valueName);
    dwSize = SYM_MAX_PATH - 1;
    if (ERROR_SUCCESS == cReg->GetValue(&dwType, (LPBYTE)(LPSTR)szLinksDir, &dwSize ))
        {
        auto    int     numFiles = 0;
        index = 0;
        if (TRUE == bUseFolders)
            {
            szLinksDir += "\\";
            while (NULL != *folders[index])
                {
                szFilename = szLinksDir;
                szFilename += folders[index];
                numFiles = RemoveTargetFiles((LPSTR)szFilename, files, TRUE);
                LTrace("  szFilename = %s, numFiles = %d, index = %d"
                                                , (LPSTR)szFilename, numFiles, index);
                ++index;
                }
            }
        else
            {
            numFiles = RemoveTargetFiles((LPSTR)szLinksDir, files, TRUE);
            LTrace("  szLinksDir = %s, numFiles = %d", (LPSTR)szLinksDir, numFiles);
            }

        }
    else
        {
        LTrace("  FAILED to get common programs directory!!!");
        goto  Cleanup;
        }

    // now attempt to remove the folders
    if (TRUE == bUseFolders)
        {
        index = 0;
        while (NULL != *folders[index])
            {
            szFilename = szLinksDir;
            szFilename += folders[index];
            LTrace("  index = %d, szFilename = %s", index, (LPSTR)szFilename);
            ISRemoveDirectory(szFilename);
            ++index;
            }
        }



Cleanup:
    if (NULL != cReg)
        {
        delete  cReg;
        }

    LTrace("END DeleteNavStartMenuTargets()");
    return (dwTotalDeleted);

}  // end of "DeleteNavStartMenuTargets"





