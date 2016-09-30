/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/navuninst.cpv   1.41   11 Nov 1998 10:39:18   rchinta  $
//
//  NavUninst.CPP - Module that holds mostly uninstall functions for the
//                  InstallShield installation of NAVNT
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/navuninst.cpv  $
// 
//    Rev 1.41   11 Nov 1998 10:39:18   rchinta
// Deleting Reg Value related to Backlog in UninstIntialize
// 
//    Rev 1.40   15 Oct 1998 21:30:46   mdunn
// Added NAVNT.* to the list of files to delete from the NAV dir, and added
// IsSilentUninstall() to suppress the "empty Q?" prompt when uninstall is
// run in silent mode.
//
//    Rev 1.39   13 Oct 1998 17:25:06   rchinta
// Modified UnregisterCOMObjects to call DllUnregisterServer
// for aboutsw.dll (new Dll for SystemWorks upsell)
//
//    Rev 1.38   24 Jul 1998 12:28:32   mdunn
// Fixed 118928 - Added UnregisterCOMObjects().
//
//    Rev 1.37   22 Jul 1998 19:58:44   mdunn
// Changed RemoveQuarantineContents() to leave all files in Quar on a corporate
// install.
//
//    Rev 1.36   20 Jul 1998 17:50:46   mdunn
// Fixed 116262 - UninstUnInitialize() will now delete the NAV group if it's
// still around and is empty.  This bug only showed up on DEC but it can't
// hurt to execute the code on Intel too.
//
//    Rev 1.35   17 Jun 1998 21:58:22   mdunn
// Tim - Added SpecialDefUtils code to handle a mismatched NAVENG.
// Me - Added DisableQuarService() to stop and disable the Quar svc if it's
//      installed.  Fixes #117612.
//    - Added RemoveQConsole(), called from the install script.  Fixes #117498.
//
//    Rev 1.34   15 Jun 1998 14:35:00   mdunn
// Now deleting 'submit a bug report' URL file
//
//    Rev 1.33   03 Jun 1998 17:31:40   mdunn
// Added Quarantine and .GID cleanup (and remembered to remove all the int 3's
// this time!)
//
//    Rev 1.32   17 Apr 1998 16:10:16   RStanev
// SmellyCat changes.
//
//    Rev 1.31   06 Mar 1998 13:40:00   mdunn
// Now deleting all product names from the LU keys (retail, trial, NEC, and
// NEC trial).
//
//    Rev 1.30   02 Mar 1998 19:40:32   mdunn
// Fixed build error in GetUsageCount().
//
//    Rev 1.29   22 Aug 1997 15:47:30   SEDWARD
// Call 'OnrampCleanupEx' with the new product IDs.
//
//    Rev 1.28   20 Aug 1997 13:23:52   SEDWARD
// Changed the data type in 'UninstallVirusDefs' function to TCHAR; updated
// logic in UninstUninitialize().
//
//    Rev 1.27   18 Aug 1997 23:33:34   SEDWARD
// Merged the updated 'UninstallVirusDefinitions' code from the Win95 DLL.
//
//    Rev 1.26   15 Aug 1997 13:58:24   SEDWARD
// Include "progman.h", added CleanupCommonStartupGroup function.
//
//    Rev 1.24   11 Aug 1997 16:02:34   SEDWARD
// Enhanced 'CleanupLiveUpdate' function to remove the 's32LuCp1.cpl' file.
//
//    Rev 1.23   10 Aug 1997 23:47:44   SEDWARD
// Mark symevent.sys for deletion during uninstall (sevinst.exe isn't doing
// this yet).
//
//    Rev 1.22   05 Aug 1997 18:30:32   SEDWARD
// Activated the calls to the SEVINST code; removed explicit handling of
// symevent.sys (SEVINST now does it for us automatically).
//
//    Rev 1.21   04 Aug 1997 18:03:20   JTAYLOR
// -- Removed the code that deletes the heavy hitter files on uninstall.
//
//    Rev 1.20   04 Aug 1997 15:54:20   KSACKIN
// Removed an _asm int 3 accidentally left in the code.
//
//    Rev 1.19   03 Aug 1997 20:28:26   SEDWARD
// Added new entries to the LiveUpdate non-shared files (the LU help files);
// modified calls to GetNavPaths, ReadNavPaths and WriteNavPaths to support
// the new "Symantec Shared" path variable; added code to call sevinst.exe
// to clean up symevent.sys stuff (disabled until it's working for NT);
// remove the "Symantec Shared" directory.
//
//    Rev 1.18   25 Jul 1997 13:31:02   SEDWARD
// Added 'LiveUpdate' to the directories we try to remove in the last fix.
//
//    Rev 1.16   23 Jul 1997 18:02:36   jtaylor
// Added code to remove the heavy hitter files.
//
//    Rev 1.15   21 Jul 1997 00:06:44   SEDWARD
// Reworked some of the logic in removing reg keys in UninstUnInitialize().
//
//    Rev 1.14   20 Jul 1997 19:06:36   SEDWARD
// Remove the 'temp' registry key in UnInstUninitialize() (used to hold
// the NAV directories temporarily).
//
//    Rev 1.13   18 Jul 1997 18:59:48   JALLEE
// Updated to use new reg key and usage count functions ported from BORG.
//
//    Rev 1.12   01 Jul 1997 20:42:02   SEDWARD
// Added 'schedule.dat' to the NAV list of files to nuke.
//
//    Rev 1.11   30 Jun 1997 15:20:02   SEDWARD
// Use character-mapping macros (ie, _tcscpy() instead of strcpy(); call
// NSPLUGIN.EXE during uninstall; added CleanupLiveUpdate(); moved
// GetUsageCount() and GetServicePath() from isutil.cpp; ReadNavPaths() now
// gets the symantec directory as well.
//
//    Rev 1.10   24 Jun 1997 16:28:44   SEDWARD
// Added 'DecrementIRPStackSize()'; this function is now called during
// uninstall.
//
//    Rev 1.9   23 Jun 1997 16:29:50   SEDWARD
// If the AP agent is running, shut it down.
//
//    Rev 1.8   22 Jun 1997 23:03:14   SEDWARD
// Added more LTrace calls to GetNavPaths().
//
//    Rev 1.7   19 Jun 1997 00:58:46   SEDWARD
// Don't manually decrement LiveUpdate usage counts anymore.
//
//    Rev 1.6   11 Jun 1997 15:52:04   SEDWARD
// Now we use the App ID arrays defined in 'NavAppIds.h' to use the newest
// defs and stop using defs.
//
//    Rev 1.5   08 Jun 1997 15:09:24   SEDWARD
// Replaced call to 'RemoveServices' with 'StopLoggedServices'.
//
//    Rev 1.4   01 Jun 1997 17:32:40   jtaylor
// no include "defutils.h"
// Added 'UninstallVirusDefinitions'
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
#include <shlobj.h>                     // for shell folder functions
#include <lmcons.h>
#include <lmshare.h>
#include <lm.h>

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
#include    "CUsgCnt.h"
#include    "ProgMan.h"

#define     INITGUID
#include    "iquaran.h"
#include    "quar32.h"



// global variables
static  char*   g_szNavFiles[] =
{
    "runsched.exe"
    , "install.inf"
    , "s32aloge.dll"
    , "s32nave.dll"
    , "*.log"
    , "excludel.dat"
    , "defloc.dat"
    , "_isnavnt.ulg"
    , "navnt.*"                         // to delete backups of navnt.isu
    , "nwdata.dat"
    , "pipedlg.dat"
    , "modem.id"
    , "$flecomp.tmp"
    , "virscanm.dat"
    , "virsp*.dat"
    , "virspec.txt"
    , "??nav*.zip"
    , "navex*.*"
    , "navor32.dll"
    , "update.txt"
    , "???9*.txt"
    , "virinfo.txt"
    , "activity.txt"
    , "schedule.dat"
    , "schedule.bak"
    , "virlist.txt"
    , "results.txt"
    , "ncsacert.txt"
    , "whatsnew.txt"
	, "*.*"
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

static  TCHAR*  g_szNavSubDirs[] =
{
	"i2_ldvp.vdb"
	, "Alert"
	, "Infbak"
	, "Infected"
	, "Logs"
	, "vpmectmp"
	, "ccmtemp"
	, ""
};

static TCHAR* g_szNavCorpSrvDirs[] =
{
	"clt-inst",
	"logon",
	""
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

#define	NAVNT_LOG_FILE				_T("navnt.isu")
#define	NAVNT_BACKUP_LOG_FILE		_T("navnt.000")

typedef NET_API_STATUS (_stdcall*NETCHECK)(LPWSTR,LPWSTR,LPDWORD);
typedef NET_API_STATUS (_stdcall*NETDEL)(LPTSTR,LPTSTR,LPDWORD);

/////////////////////////////////////////////////////////////////////////////
//  Local Macros/Inlines
/////////////////////////////////////////////////////////////////////////////

VOID _inline FuncTrace( LPSTR lpFunc, BOOL bRet )
{
    LTrace( (LPSTR)"%s returns %s\n", lpFunc, bRet ? "TRUE" : "FALSE" );
}

/////////////////////////////////////////////////////////////////////////////
//  Local Functions
/////////////////////////////////////////////////////////////////////////////

static void CleanUpNAVProduct ( int nIDSProductBaseName, int nIDSVersion,
                                int nIDSLanguage, ONRAMPCLEANUPEX lpFunc );

void RemoveQuarantineContents (HWND);
void RemoveHiddenHelpFiles(HWND);
void RemoveSubmitBugReportFiles ( HWND );
void UnregisterCOMObjects();
void LNameAppendFile ( LPTSTR, LPCTSTR );
BOOL IsCorporateVersion();
BOOL IsSilentUninstall();
void DisableQuarService();
DWORD ConvertToWide(LPCSTR pMultiStr, LPWSTR &pWideStr);
DWORD RemoveNTShare( LPTSTR szShareName );
DWORD RemoveServerShares();
BOOL ClientIsServer();


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

    // If the admin tools Q service is installed, warn the use that
    // uninstalling NAV will leave the service in an unusable state.
    lstrcpy ( szTemp, g_szTarget );
    szTemp.AppendFile ( _T("quarsvc.exe") );

    if ( 0xFFFFFFFF != GetFileAttributes ( szTemp ) )
        {
                                        // The service is installed!!
                                        // Inform the user that the Q svc
                                        // will be unusable, and ask if they
                                        // want to continue with uninstall.
        TCHAR szMsg[1024], szCaption[128];

        if ( LoadString ( g_hInst, IDS_ADMINTOOLS_INSTALLED_MSG,
                          szMsg, sizeof(szMsg) )
             &&
             LoadString ( g_hInst, IDS_NAV_UNINSTALL,
                          szCaption, sizeof(szCaption) ) )
            {
            if ( IDNO == MessageBox ( hwndDlg, szMsg, szCaption,
                                      MB_ICONEXCLAMATION | MB_YESNO |
                                        MB_DEFBUTTON2 ) )
                {
                return -1;              // abort uninstall
                }

                                        // Tell the user that the Q svc
                                        // will be disabled.
            if ( LoadString ( g_hInst, IDS_ADMINTOOLS_QSVC_DISABLED,
                              szMsg, sizeof(szMsg) ) )
                {
                MessageBox ( hwndDlg, szMsg, szCaption,
                             MB_ICONINFORMATION | MB_OK );

                DisableQuarService();
                }
            }
        }


    // save the NAV path temporarily to the registry so we can access
    // it later (NOTE: for some reason, unInstallShield is blowing away
    // this value after "UninstInitialize" and before "UninstUnInitialize")
    WriteNavPaths(g_szTarget, g_szSharedDir, g_szSymSharedDir);

	// Prompt for password before continuing
	if(false == ClientIsServer())
		{
		// Prompt for password before continuing
		auto CWString szTempSrc;
		auto CWString szTempDest;
		auto bool bExit = false;
	
		int nResult = PromptUninstallPassword();

		switch(nResult)
			{
		case TM_OK:
			bExit = false;
			break;
		case TM_FALSE:
			bExit = true;
			break;
			}

		if(bExit)
			{
			// Get the path to the backup log file
			_tcscpy(szTempSrc, g_szTarget);
			szTempSrc.AppendFile(NAVNT_BACKUP_LOG_FILE);

			// Set the path/filename to restore the backup log file
			_tcscpy(szTempDest, g_szTarget);
			szTempDest.AppendFile(NAVNT_LOG_FILE);

			// Perform the copy
			CopyFile(szTempSrc, szTempDest, FALSE); 

			// Exit the uninstall routine
			HANDLE hProc = GetCurrentProcess();
			if(hProc != NULL)
				{
				DWORD dwExitCode;
				if(GetExitCodeProcess(hProc, &dwExitCode))
					ExitProcess(dwExitCode);
				}

			}
		}
	

    // create a default MIF file (one of "failure")
    CreateMifFile("Failure", "Uninstallation failed or incomplete");

	// for servers, remove shares if they exist.
	if( TRUE == ClientIsServer() )
		{
		RemoveServerShares();
		}

    // if the AP agent is running, shut it down
	RemoveAPUI();

    // Delete any leftover stuff in Quarantine
    RemoveQuarantineContents ( hwndDlg );

    // decrement usage counts on shared files handled by us (uses our
    // uninstall log file)
    ProcessUsageCountsFile("SymShared", LOGFILE_NAME );

    // if we're about to remove symevent.sys, mark it for deletion
    MarkSymevent();

    // remove the NAV services (we must call "DecrementUsageCounts" before this)
    szTemp = g_szTarget;
    szTemp.AppendFile(LOGFILE_NAME);
    StopLoggedServices(szTemp, TRUE);

    // call LiveUpdate's own cleanup function
    CleanUpOnRamp();

    // cleanup the virus definition files
    UninstallVirusDefinitions();

	// clean up the symevent stuff
    szTemp = g_szSymSharedDir;
    szTemp.AppendFile(SEVINST_EXE);
    _tcscat(szTemp, " /Q /U NAVNT");
    LaunchAndWait(NULL, szTemp);

    // Unregister Q, SND, ABOUTSW, and TRANSMAN COM objects.
    UnregisterCOMObjects();

    return(0);

}   // end of "UninstInitialize"




/////////////////////////////////////////////////////////////////////////////
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

    auto    CWString        szTemp(SYM_MAX_PATH);
    auto    CWString        szDataBuf(SYM_MAX_PATH);
	auto	CWString		szCurrDir(SYM_MAX_PATH);
    auto    DWORD           dwValues;
    auto    DWORD           dwKeys;
	auto	DWORD			dwType;
	auto	DWORD			dwSize;
	auto	DWORD			dwDataBuf = 0;
    auto    CRegistryEntry *cReg;
	auto	CRegistryEntry *cVP6Reg;
	auto	CRegistryEntry *cregQsconsole;
	auto	BOOL			bCanDeleteNow = TRUE;



    // if we're running NT 3.51, remove the program group items
    auto    BOOL                bIsExplorerShell;
    auto    OSVERSIONINFO       osInfo;

    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if  ((TRUE == GetVersionEx(&osInfo))
    &&  (3 == osInfo.dwMajorVersion)
    &&  (51 == osInfo.dwMinorVersion))
        {
        bIsExplorerShell = FALSE;
        }
    else
        {
        bIsExplorerShell = TRUE;
        }

    LTrace("UninstUnInitialize: OSVERSIONINFO");
    LTrace("    osInfo.dwOSVersionInfoSize = %d", osInfo.dwOSVersionInfoSize);
    LTrace("    osInfo.dwMajorVersion = %d", osInfo.dwMajorVersion);
    LTrace("    osInfo.dwMinorVersion = %d", osInfo.dwMinorVersion);
    LTrace("    osInfo.dwBuildNumber = %d", osInfo.dwBuildNumber);
    LTrace("    osInfo.dwPlatformId = %d", osInfo.dwPlatformId);
    LTrace("    osInfo.szCSDVersion = %s", osInfo.szCSDVersion);

    CleanupCommonStartupGroup(bIsExplorerShell);
	
	// Check for Quarantine Console 
    cregQsconsole = new CRegistryEntry( HKEY_LOCAL_MACHINE, QSCONSOLE_KEY
                                                         , QSCONSOLE_PATH_KEY);
    if (NULL == cregQsconsole)
        {
        return (false);
        }


    if (ERROR_SUCCESS == cregQsconsole->OpenKey(KEY_READ))
        {
        // Get the Qsconsole path location
        dwSize = SYM_MAX_PATH -1;
        if (ERROR_SUCCESS == cregQsconsole->GetValue( &dwType, (LPBYTE)(LPSTR)szDataBuf
                                                                    , &dwSize ))
            {
			// Quarantine Console is still installed so re-register 
			// scandlvr.dll to point to the Qsconsole directory

			// Save the current directory
			GetCurrentDirectory(MAX_PATH, szCurrDir);

			// Set the directory to the Qsconsole directory
			SetCurrentDirectory(szDataBuf);

			// Register SCANDLVR.DLL
			szDataBuf.AppendFile("scandlvr.dll");
			RegisterOCX(szDataBuf);

			// Restore the current directory
		    SetCurrentDirectory ( szCurrDir );
            }
		}

	delete(cregQsconsole);

    // fetch the path where NAV used to be
    if (TRUE == ReadNavPaths(g_szTarget, g_szSharedDir, g_szSymSharedDir))
        {
		// Get rid of the clt-inst folder if we're a server.  Do this first
		// before the INTEL reg keys get deleted.
		if( TRUE == ClientIsServer() )
		{
			int nSubDirs = GetArrayLen( g_szNavCorpSrvDirs );

			for( int nSubCnt = 0; nSubCnt < nSubDirs; nSubCnt++ )
				RemoveAll( g_szTarget, g_szNavCorpSrvDirs[nSubCnt] );
		}

		// Check to see if we can nuke the VP6 registry tree.
		cVP6Reg = new CRegistryEntry( HKEY_LOCAL_MACHINE,
										"SOFTWARE\\Symantec\\InstalledApps",
										"VP6UsageCount" );

		dwSize = sizeof( dwDataBuf );
		if( ERROR_SUCCESS == cVP6Reg->OpenKey( KEY_ALL_ACCESS ) )
		{
			if( ERROR_SUCCESS == cVP6Reg->GetValue( &dwType, (LPBYTE)&dwDataBuf, &dwSize ) )
			{
				// Perform decrement in this fashion since we can only have
				// a maximum of 3 installation(s) on one machine.  This
				// count should never reach a DWORD value in the A-F range.
				if( dwDataBuf > 0 )
					dwDataBuf--;

				if( 0 == dwDataBuf )
				{
					bCanDeleteNow = TRUE;
				}
				else
				{
					cVP6Reg->SetValue( REG_DWORD, (LPBYTE)&dwDataBuf, sizeof(dwDataBuf) );
					bCanDeleteNow = FALSE;
				}
			}
			else
			{
				// The value is not there so assume there are no other products.
				bCanDeleteNow = TRUE;
			}

			// if the value is decremented to zero (bCanDeleteNow == TRUE), then
			// get rid of the VP6UsageCount value pair.
			if( TRUE == bCanDeleteNow )
			{
				cVP6Reg->DeleteValue();
			}

			cVP6Reg->CloseKey();
		}

		delete( cVP6Reg );

        // cleanup any shared files' registry entries
        CleanupSharedSymantecRegEntries();

        // if the number of keys is greater than zero and the number of values is
        // less than or equal to two, remove the Symantec shared usage key
        cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, "Software\\Symantec\\SharedUsage" );

        dwKeys = cReg->GetSubkeyCount();
        dwValues = cReg->GetValueCount();

        if (!dwKeys && (dwValues <= 2))
            {
            cReg->DeleteKey();
            }

        // Remove the VP6TEMPID value-pair
		cReg->ResetKey(HKEY_LOCAL_MACHINE, "Software\\Symantec\\InstalledApps");
		cReg->SetValueName("VP6TEMPID");
		cReg->DeleteValue();

		// if there are no values left under the installed apps key, remove it
        cReg->SetSubkey( "Software\\Symantec\\InstalledApps" );
        dwKeys = cReg->GetSubkeyCount();
        dwValues = cReg->GetValueCount();
        if ( !dwValues )
            {
            cReg->DeleteKey();
            }

	    // get rid of the temporary reg key
        cReg->ResetKey(HKEY_LOCAL_MACHINE, TEMP_REGKEY_PATH);
        cReg->DeleteKey();

		// Remove all other NAV related reg keys
		cReg->ResetKey(HKEY_LOCAL_MACHINE, "Software\\Symantec\\Symantec AntiVirus NT");
		cReg->DeleteKey();

		cReg->ResetKey(HKEY_LOCAL_MACHINE, "Software\\Symantec\\SharedDefs");
		cReg->DeleteKey();

		// remove the INTEL LDVP keys
		if( TRUE == bCanDeleteNow )
		{
			cReg->ResetKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\INTEL\\DLLUsage\\VP6");
			cReg->DeleteKey();

			cReg->ResetKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\INTEL\\LANDESK");
			cReg->DeleteKey();
		}

		// remove the symantec key
	    cReg->SetSubkey( "Software\\Symantec" );
		dwKeys = cReg->GetSubkeyCount();
		if (!dwKeys)
			{
			cReg->DeleteKey();
			}

		// remove the NAV keys under symantec
		cReg->ResetKey( HKEY_CURRENT_USER, "Software\\Symantec\\Symantec AntiVirus" );
		cReg->DeleteKey();

		// remove the symantec key under "current user"
		cReg->ResetKey( HKEY_CURRENT_USER, "Software\\Symantec" );
		dwKeys = cReg->GetSubkeyCount();
		if (!dwKeys)
			{
			cReg->DeleteKey();
			}

		// Remove the INTEL LANDesk keys under [HKCU\Software\INTEL\LANDesk]
		// if there are no other SSC components on the client.
		if( TRUE == bCanDeleteNow )
		{
			cReg->ResetKey( HKEY_CURRENT_USER, "Software\\INTEL\\LANDesk" );
			cReg->DeleteKey();
		}

        // remove any leftover data files InstallShield doesn't know about
        RemoveLeftoverDataFiles();

        // if the LiveUpdate shared DLLs have been removed, be sure to cleanup the
        // "non-shared" files as well
        CleanupLiveUpdate(g_szSharedDir);

        // Blow away the symantec shared directory
        ISRemoveDirectory(g_szSymSharedDir);

        // Blow away the symantec directory
		auto CWString	cwstrFileReg;
		_tcscpy(cwstrFileReg, g_szSharedDir);
		cwstrFileReg.AppendFile("ProductReg.reg");
		SetFileAttributes(cwstrFileReg, FILE_ATTRIBUTE_NORMAL);
        ISDeleteFile(cwstrFileReg);
		ISRemoveDirectory(g_szSharedDir);

		// Remove the NAVNT directory and it's remaining subdirectories
		int nNumSubDirs = GetArrayLen(g_szNavSubDirs);

		for(int nCurSubDir = 0; nCurSubDir < nNumSubDirs; ++nCurSubDir)
			RemoveAll(g_szTarget, g_szNavSubDirs[nCurSubDir]); 

		// Remove any History files
		auto CWString	cwstrFileCsv;
		_tcscpy(cwstrFileCsv, g_szTarget);
		cwstrFileCsv.AppendFile("*.csv");
		SetFileAttributes(cwstrFileCsv, FILE_ATTRIBUTE_NORMAL);
		ISDeleteFile(cwstrFileCsv);
		ISRemoveDirectory(g_szTarget);

    }

    // if we succeeded in uninstallation, write out a "success" MIF file
    if (TRUE == NavUninstallSucceeded())
        {
        CreateMifFile("Success", "Uninstallation succeeded");
        }


    // Nuke the NAV group if it is empty (fix for #116262)
LPITEMIDLIST    pidl;
TCHAR           szScratch[MAX_PATH];
LPMALLOC        pMalloc;

    if ( SUCCEEDED ( SHGetSpecialFolderLocation ( hwndDlg, CSIDL_COMMON_PROGRAMS,
                                                  &pidl ) ) )
        {
        if ( SUCCEEDED ( SHGetPathFromIDList ( pidl, szScratch ) ) )
            {
            szTemp = szScratch;         // we'll build the path to the group in szTemp

            if ( LoadString ( g_hInst, IDS_NAV_GROUP_NAME, szScratch,
                              sizeof(szScratch) ) )
                {
                szTemp.AppendFile ( szScratch );
                ISRemoveDirectory ( szTemp );
                }
            }

        SHGetMalloc ( &pMalloc );
        pMalloc->Free ( pidl );
        pMalloc->Release();
        }

    return(0);

}   // end of "UninstUnInitialize"



// ===== CleanupCommonStartupGroup ========================================
//
//  This function removes the program group items for NAV.  It will call a
//  function to derive the system's name for the common startup group,
//  then attempt to remove the NAVAP item from the group.  If the NAVAP item
//  is the last item in the group, the group will be removed as well.
//
//  Input:  nothing
//  Output: TRUE on success, FALSE if failure
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

BOOL    CleanupCommonStartupGroup(BOOL  bIsExplorerShell)
{
    auto    BOOL        bResult;
    auto    int         nResult;
    auto    TCHAR       szCommand[SYM_MAX_PATH];
    auto    TCHAR       szNavapStartupName[SYM_MAX_PATH];
    auto    TCHAR       szStartupGroupName[SYM_MAX_PATH];

    LTrace("Top of CleanupCommonStartupGroup");

    bResult = TRUE;

    // get the official display string for the common startup group
    _tcscpy(szStartupGroupName, "Startup");
    GetCommonStartupDisplayName(szStartupGroupName, GROUPTYPE_COMMON);
    LTrace("    szStartupGroupName = %s", szStartupGroupName);

    // load the string used to display the NAVAP icon in the common
    // startup group
    nResult = LoadString(g_hInst, IDS_NAVAP_STARTUP_ICON_NAME, szNavapStartupName
                                                , sizeof(szNavapStartupName) - 1);
    if (nResult <= 0)
        {
        LTrace("    LoadString(IDS_NAVAP_STARTUP_ICON_NAME) failed");
        bResult = FALSE;
        goto  Exit_Function;
        }
    else
        {
        LTrace("    szNavapStartupName = %s", szNavapStartupName);
        }

    // call the appropriate manipulation function, based on the current flavor of NT
    if (FALSE == bIsExplorerShell)
        {
        // derive a command-line for the program manager
        // (ie,   "[DeleteItem(\"Symantec AntiVirus AutoProtect\")]"     )
        _tcscpy(szCommand, "[DeleteItem(\"");
        _tcscat(szCommand, szNavapStartupName);
        _tcscat(szCommand, "\")]");
        LTrace("    NT 3.51 szCommand = %s", szCommand);

        // remove the NAVAP icon (and, if nothing else is left, the group)
        bResult = ProgmanGroupManipulation(szStartupGroupName, szCommand
                                            , ACTION_DELETE_ITEM, GROUPTYPE_COMMON);
        }
    else
        {
        bResult = ProgmanGroupManipulationNT4(szStartupGroupName, szNavapStartupName
                                                                , ACTION_DELETE_ITEM
                                                                , GROUPTYPE_COMMON);
        }


Exit_Function:

    LTrace("CleanupCommonStartupGroup: bResult = %d", bResult);
    return (bResult);

}  // end of "CleanupCommonStartupGroup"



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
            // load the strings for the product version and language
            LoadString(g_hInst, IDS_VERSION_ONE_POINT_ZERO, szVersion
                                                            , SYM_MAX_PATH - 1);
            LoadString(g_hInst, IDS_ENGLISH, szLanguage, SYM_MAX_PATH - 1);

            // load the strings for the Avenge/Intel definitions
            LoadString(g_hInst, IDS_AVENGE_DEFS_INTEL_PRODUCT_NAME, szProductName
                                                            , SYM_MAX_PATH - 1);
            lResult = (lpFunc)(szProductName, szVersion, szLanguage);

            // load the strings for the Avenge/Alpha definitions
            LoadString(g_hInst, IDS_AVENGE_DEFS_ALPHA_PRODUCT_NAME, szProductName
                                                            , SYM_MAX_PATH - 1);
            lResult = (lpFunc)(szProductName, szVersion, szLanguage);

            // load the strings for the AV Engine/Intel definitions
            LoadString(g_hInst, IDS_AV_ENGINE_DEFS_INTEL_PRODUCT_NAME, szProductName
                                                            , SYM_MAX_PATH - 1);
            lResult = (lpFunc)(szProductName, szVersion, szLanguage);

            // load the strings for the AV Engine/Alpha definitions
            LoadString(g_hInst, IDS_AV_ENGINE_DEFS_ALPHA_PRODUCT_NAME, szProductName
                                                            , SYM_MAX_PATH - 1);
            lResult = (lpFunc)(szProductName, szVersion, szLanguage);

            // Clean up the various NAV products....
            CleanUpNAVProduct ( IDS_NAV_SERVER_INTEL_PRODUCT_NAME,
                                IDS_NAV_PRODUCT_VERSION,
                                IDS_ENGLISH, lpFunc);

            CleanUpNAVProduct ( IDS_NAV_SERVER_ALPHA_PRODUCT_NAME,
                                IDS_NAV_PRODUCT_VERSION,
                                IDS_ENGLISH, lpFunc);

            CleanUpNAVProduct ( IDS_NAV_WORKSTATION_INTEL_PRODUCT_NAME,
                                IDS_NAV_PRODUCT_VERSION,
                                IDS_ENGLISH, lpFunc);

            CleanUpNAVProduct ( IDS_NAV_WORKSTATION_ALPHA_PRODUCT_NAME,
                                IDS_NAV_PRODUCT_VERSION,
                                IDS_ENGLISH, lpFunc);

            bRet = TRUE;
            }

        FreeLibrary(hLib);
        }

    FuncTrace("CleanUpOnRamp", bRet);

    return(bRet);

}   // end of "CleanUpOnRamp"



//////////////////////////////////////////////////////////////////////////
//
// Function:    CleanUpNAVProduct
//
// Description:
//  Called from CleanUpOnramp() to remove the LU keys for a family of
//  NAV products.  The "family" is the retail version, NEC version, trial
//  version, and NEC trial version.
//
// Input:
//  nIDSProductBaseName: [in] String ID of the base product name.
//  nIDSVersion: [in] String ID of the version number.
//  nIDSLanguage: [in] String ID of the product language.
//  lpFunc: [in] Pointer to the Onramp cleanup function to call.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 3/5/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void CleanUpNAVProduct ( int nIDSProductBaseName, int nIDSVersion,
                         int nIDSLanguage, ONRAMPCLEANUPEX lpFunc )
{
TCHAR  szProductName [SYM_MAX_PATH];
TCHAR  szVersion [SYM_MAX_PATH];
TCHAR  szLanguage [SYM_MAX_PATH];
TCHAR* pInsertLocation;
int    nRet;

    nRet = LoadString ( g_hInst, nIDSProductBaseName, szProductName,
                        SYM_MAX_PATH );

    SYM_ASSERT ( nRet != 0 );

    nRet = LoadString ( g_hInst, nIDSVersion, szVersion, SYM_MAX_PATH );

    SYM_ASSERT ( nRet != 0 );

    nRet = LoadString ( g_hInst, nIDSLanguage, szLanguage, SYM_MAX_PATH );

    SYM_ASSERT ( nRet != 0 );


    pInsertLocation = _tcschr ( szProductName, '\0' );


                                        // Clean up the retail product.
    lpFunc ( szProductName, szVersion, szLanguage );


                                        // Clean up the NEC product.
    _tcscpy ( pInsertLocation, _T(" NEC") );

    lpFunc ( szProductName, szVersion, szLanguage );


                                        // Clean up the trial product.
    _tcscpy ( pInsertLocation, _T(" Trial") );

    lpFunc ( szProductName, szVersion, szLanguage );


                                        // Clean up the NEC trial product.
    _tcscpy ( pInsertLocation, _T(" NEC Trial") );

    lpFunc ( szProductName, szVersion, szLanguage );
}

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

    cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, SYM_SHAREDUSAGE_KEY, "LiveUpdate1" );
    if ( NULL != cReg )
        {
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

        delete cReg;
        }

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



// === DecrementIRPStackSize ==============================================
//
//  This function decrements the IRP stack size using symevent.sys as a guide.
//  If there is no usage count for symevent, then it's assumed that we must
//  be the last Norton product uninstalling from the system, so it's okay
//  to then decrement the IRP stack value.
//
//  NOTE: this function assumes that the shared usage counts under
//        HKLM\SW\MS\Windows\CurrentVersion\SharedDLLs key are up to date!
//
//  Input:  nothing
//  Output: a value of TRUE if the IRP value is actually decremented,
//          FALSE if it is not (not necessarily an error)
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

BOOL    DecrementIRPStackSize(void)
{
    auto    BOOL            bRet = FALSE;
    auto    DWORD           dwTemp = 0;
    auto    DWORD           dwData = 0;
    auto    DWORD           dwSize = 0;
    auto    DWORD           dwType = 0;
    auto    LPCTSTR         lpSymeventDriver = "SYMEVENT.SYS";
    auto    CWString        szFile(SYM_MAX_PATH);
    auto    CRegistryEntry *cReg;

    LTrace( "BEGIN DecrementIRPStackSize" );

    // build a full path and filename to symevent.sys
    GetSystemDirectory((LPSTR)szFile, SYM_MAX_PATH);
    szFile.AppendFile( (LPSTR)lpSymeventDriver );

    // get the current usage count on symevent.sys; if this function returns FALSE,
    // there is no usage count available
    if (FALSE == GetUsageCount(szFile, dwTemp))
        {
        auto    DWORD       dwIrpStackCount;

        cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, IRPSTACK_KEY, "IRPStackSize");
        if (NULL == cReg)
            return ( FALSE );

        // there is no usage count, so we must be the last ones out; decrement the
        // IRP stack size...

        // get the current stack value
        dwSize = sizeof (DWORD);

        if (cReg->GetValue(&dwType, (LPBYTE)&dwTemp, &dwSize))
            {
            // if it's greater than or equal to six, decrement and save the value
            dwIrpStackCount = dwTemp;
            if (dwIrpStackCount >= 6)
                {
                dwIrpStackCount -= 2;
                cReg->SetValue(REG_DWORD, (LPBYTE)&dwIrpStackCount, sizeof(DWORD));
                bRet = TRUE;
                }
            }
        delete cReg;
        }


    LTrace( "END DecrementIRPStackSize( %d )\n", bRet );

    return(bRet);

}  // end of "DecrementIRPStackSize"


//////////////////////////////////////////////////////////////////////////
//
// Function:    DisableQuarService
//
// Description:
//  Stops and disables the Quarantine service.
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
//  6/17/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void DisableQuarService()
{
CServiceControl svc;
TCHAR           szServiceName[] = _T("QUARSVC");

                                        // Stop the service...
    if ( StopSingleService ( szServiceName, TRUE, FALSE ) )
        {
        if ( ERROR_SUCCESS == svc.Open ( szServiceName ) )
            {
                                        // and disable it.
            svc.ChangeServiceConfig ( SERVICE_NO_CHANGE,
                                      SERVICE_DISABLED );
            }
        }
}


/////////////////////////////////////////////////////////////////////////////
//
//  GetNavPaths - Function to retrieve the various paths from the registry
//
//  Params: LPTSTR - Pointer to buffer that will hold the main NAV path
//          LPTSTR - Pointer to buffer that will hold the symantec shared path
//          LPTSTR - Pointer to buffer that will hold the symantec shared path
//
//  Return value:   TRUE if all the paths are found
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL    GetNavPaths(LPTSTR lpNavPath, LPTSTR lpSharedPath,
                    LPTSTR lpSymSharedPath)
{
    auto    BOOL            bRet = FALSE;
    auto    CRegistryEntry *cReg;
    auto    CWString        szDataBuf(SYM_MAX_PATH);
    auto    DWORD           dwSize;
    auto    DWORD           dwType;


    if (lpNavPath != NULL && lpSharedPath != NULL)
        {
        *lpNavPath = *lpSharedPath = 0;

        cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, NAV_INSTALL_KEY, NAV_INSTALLDIR);
        if ( NULL == cReg )
            {
            return ( FALSE );
            }

        dwSize = SYM_MAX_PATH - 1;
        if (ERROR_SUCCESS == cReg->GetValue(&dwType, (LPBYTE)(LPSTR)szDataBuf
                                                                    , &dwSize))
            {
            lstrcpy( lpNavPath, szDataBuf );
            LTrace("GetNavPaths: lpNavPath = %s", lpNavPath);
            }

        cReg->SetValueName("SharedComponents");
        dwSize = SYM_MAX_PATH - 1;
        if (ERROR_SUCCESS == cReg->GetValue(&dwType, (LPBYTE)(LPSTR)szDataBuf
                                                                    , &dwSize))
            {
            lstrcpy(lpSharedPath, szDataBuf);
            LTrace("GetNavPaths: lpSharedPath = %s", lpSharedPath);
            }

        cReg->SetValueName( "SymantecShared" );
        dwSize = SYM_MAX_PATH - 1;
        if (ERROR_SUCCESS == cReg->GetValue(&dwType, (LPBYTE)(LPSTR)szDataBuf
                                                                    , &dwSize))
            {
            lstrcpy(lpSymSharedPath, szDataBuf);
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
    auto    BOOL            bRetVal = FALSE;
    auto    CWString        szTemp(SYM_MAX_PATH);
    auto    DWORD           dwSize;
    auto    DWORD           dwType;
    auto    LPTSTR          dataPtr;
    auto    CRegistryEntry *cReg;



    szTemp = SERVICES_REGKEY_PATH;
    _tcscat(szTemp, "\\");
    _tcscat(szTemp, lpszServiceName);

    cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, szTemp, "ImagePath" );
    if ( NULL == cReg )
        return ( FALSE );

    dwSize = SYM_MAX_PATH - 1;


    if (ERROR_SUCCESS == cReg->GetValue ( &dwType, (LPBYTE)(LPSTR)szTemp, &dwSize))
        {
        dataPtr = (LPTSTR)szTemp;

        // if this is a kernel-mode driver, skip over the leading garbage chars
        while (0 == isalpha(*dataPtr))
            {
            dataPtr = CharNext(dataPtr);
            }

        if (NULL != *dataPtr)
            {
            _tcscpy(strBuf, dataPtr);
            bRetVal = TRUE;
            }
        }
    delete cReg;
    return (bRetVal);

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



// ==== MarkSymevent ======================================================
//
//  This function will handle the case where "symevent.sys" is about to be
//  removed from the system by "sevinst.exe".  If so, we need to tell the
//  service control manager to mark the service for deletion (in case the
//  user attempts to reinstall, we can inform them that the system must be
//  rebooted to install so we can unload symevent).  If the current usage
//  count for symevent.sys == 1, we can assume that it is about to be
//  removed and go ahead and mark the service for deletion.  Otherwise, we
//  leave it be.
//
//  NOTE: this function is to be called before "sevinst.exe".
//
//
//  Input:
//      lpszServiceName -- the display name of the service
//      bStopService    -- TRUE means stop the service, FALSE means not to
//      bDeleteService  -- if TRUE, the service is deleted
//
//  Output: a value of TRUE if symevent.sys is marked for deletion, FALSE
//          if it is not.
//
// ========================================================================
//  Function created: 8/97, SEDWARD
// ========================================================================

BOOL    MarkSymevent(void)
{
    auto    BOOL            bResult;
    auto    CWString        szTemp(SYM_MAX_PATH);
    auto    DWORD           dwCurrentUsageCount;

    // get the current usage count on symevent.sys
    GetServicePath(SYMEVENT_SERVICE_NAME, szTemp);
    GetUsageCount(szTemp, dwCurrentUsageCount);

    // if the current usage count equals one, we should mark it for deletion; else,
    // leave it alone
    if (dwCurrentUsageCount <= 1)
        {
        // params == service name, stop service flag, delete service flag
        StopSingleService(SYMEVENT_SERVICE_NAME, TRUE, TRUE);
        bResult = TRUE;
        }
    else
        {
        bResult = FALSE;
        }

    return (bResult);

}  // end of "MarkSymevent"



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

BOOL    ReadNavPaths(LPTSTR  navPathStr, LPTSTR  symantecPathStr
                                                    , LPTSTR  symSharedPathStr)
{
    auto    BOOL            bResult = FALSE;
    auto    DWORD           dwSize;
    auto    DWORD           dwType;
    auto    CRegistryEntry  *cReg;
	auto	CWString		szDataBuf;


    // open the temporary registry key
    cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, TEMP_REGKEY_PATH
                                                        , TEMP_REGVAL_NAV_PATH);
    if (NULL == cReg)
        {
        return (FALSE);
        }


    if (ERROR_SUCCESS == cReg->OpenKey(KEY_READ))
        {
        // read the NAV path
        dwSize = SYM_MAX_PATH -1;
        if (ERROR_SUCCESS == cReg->GetValue( &dwType, (LPBYTE)(LPSTR)szDataBuf
                                                                    , &dwSize ))
            {
            // copy the value data to the input argument buffer pointer
            lstrcpy(navPathStr, szDataBuf);
            }

        // read the Symantec path
        cReg->SetValueName(TEMP_REGVAL_SYMANTEC_PATH);
        dwSize = SYM_MAX_PATH -1;
        if (ERROR_SUCCESS == cReg->GetValue(&dwType, (LPBYTE)(LPSTR)szDataBuf
                                                                    , &dwSize))
            {
            // copy the value data to the input argument buffer pointer
            lstrcpy(symantecPathStr, szDataBuf);
            }

        // read the Symantec shared path
        cReg->SetValueName(TEMP_REGVAL_SYMANTEC_SHARED_PATH);
        dwSize = SYM_MAX_PATH -1;
        if (ERROR_SUCCESS == cReg->GetValue(&dwType, (LPBYTE)(LPSTR)szDataBuf
                                                                    , &dwSize))
            {
            // copy the value data to the input argument buffer pointer
            lstrcpy(symSharedPathStr, szDataBuf);
            }

        bResult = TRUE;
        cReg->CloseKey();
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


    // loop and process each string in the array
    while (NULL != **filenameArray)
        {
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
                    dwResult = ISDeleteFile(szBuf);
                    if ((IS_FILE_DELETED == dwResult)  ||  (IS_FILE_IN_USE == dwResult))
                        {
                        ++numDeletedFiles;
                        }
                    }

                // try to get another match from this string
                }  while (TRUE == FindNextFile(fileHandle, &fileData));
            }
        ++filenameArray;
        }

    // if we're supposed to remove the directory, do so
    if (TRUE == bRemoveDir)
        {
        ISRemoveDirectory(pszTargetDir);
        }

    return (numDeletedFiles);

}  // end of "RemoveTargetFiles"


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
// 8/09/99 Jacob Taylor -- rewritten to work for Beluga VirusBin.
//////////////////////////////////////////////////////////////////////////

void RemoveQuarantineContents ( HWND hDlg )
{
    HKEY                    hIntelCurrentVersionKey;
    TCHAR                   szHomeDir[MAX_PATH];
    TCHAR                   szPrompt [MAX_PATH];
    TCHAR                   szCaption[MAX_PATH];
    DWORD                   dwBufSize;

    // On corporate installs, we leave all files in Quarantine and don't
    // prompt.  Brian says this is The Right Thing To Do!
    if ( IsSilentUninstall() )
        return;

    ZeroMemory ( szHomeDir, sizeof(szHomeDir) );
    dwBufSize = sizeof(szHomeDir);

    if ( ERROR_SUCCESS == RegOpenKeyEx (
                              HKEY_LOCAL_MACHINE,
                              SERVER_CHECK_KEY, 0, KEY_QUERY_VALUE, &hIntelCurrentVersionKey ) )
        {
        if ( ERROR_SUCCESS == RegQueryValueEx (
                                  hIntelCurrentVersionKey, HOME_DIRECTORY_VALUE, NULL,
                                  NULL, (LPBYTE) szHomeDir, &dwBufSize ) )
            {
            char path[MAX_PATH];
            WIN32_FIND_DATA fd;
            HANDLE han;
            TCHAR szQuarantineFilename[MAX_PATH];

            _stprintf(path,"%s\\%s\\*%s", szHomeDir, QUARANTINE_FOLDERNAME, QUARANTINE_EXTENSION);
            han = FindFirstFile(path,&fd);
            if (han != INVALID_HANDLE_VALUE) 
                {
                // Check to make sure the user would like to delete the files.
                if ( LoadString ( g_hInst, IDS_CLEAN_QUAR_PROMPT,
                                    szPrompt, sizeof(szPrompt) ) &&
                     LoadString ( g_hInst, IDS_NAV_UNINSTALL,
                                    szCaption, sizeof(szCaption) ))
                    {
                    if ( IDNO == MessageBox ( hDlg, szPrompt, szCaption,
                                              MB_ICONQUESTION | MB_YESNO ) )
                        {
                        FindClose(han);
                        return;
                        }
                    }

                do  
                    {
                    // append filename to path and call delete
                    _stprintf(szQuarantineFilename, "%s\\%s\\%s", szHomeDir, QUARANTINE_FOLDERNAME, fd.cFileName);
                    DeleteFile(szQuarantineFilename);
                    } while (FindNextFile(han,&fd));

                FindClose(han);
                }
            }        
        }
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
//  Nothing.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
//  6/3/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void RemoveHiddenHelpFiles ( HWND hDlg )
{
SHFILEOPSTRUCT rFileOp;
TCHAR          szFiles[MAX_PATH];

    ZeroMemory ( &rFileOp, sizeof(SHFILEOPSTRUCT) );
    ZeroMemory ( szFiles, sizeof(szFiles) );

    lstrcpy ( szFiles, g_szTarget );
    LNameAppendFile ( szFiles, _T("*.gid") );

    rFileOp.hwnd   = hDlg;
    rFileOp.wFunc  = FO_DELETE;
    rFileOp.pFrom  = szFiles;
    rFileOp.fFlags = FOF_FILESONLY | FOF_NOCONFIRMATION |
                     FOF_NOERRORUI | FOF_SILENT;

    SHFileOperation ( &rFileOp );
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

    hr = SHGetSpecialFolderLocation ( hwnd, CSIDL_COMMON_PROGRAMS, &pidl );

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

extern "C" DllExport   BOOL    UninstallVirusDefinitions(void)
{
    auto    BOOL                bRetValue = TRUE;
    auto    CRegistryEntry*     cReg;
    auto    DWORD               dwSize;
    auto    DWORD               dwType;
    auto    int                 nCurAppId;
    auto    int                 nNumAppIds;
    auto    LPCTSTR*            lpszAppIdArray;
    auto    TCHAR               szDataBuf[SYM_MAX_PATH];


    cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, NAV_INSTALL_KEY, "InstallDir" );
    if (NULL==cReg)
        {
        return (FALSE);
        }

    dwSize = SYM_MAX_PATH - 1;
    if (ERROR_SUCCESS == cReg->GetValue(&dwType, (LPBYTE)(LPSTR)szDataBuf, &dwSize ))
        {
        LTrace("UninstallVirusDefinitions: lpNavPath = %s", szDataBuf);
        }
    else
        {
        return (FALSE);
        }

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
    lpszAppIdArray = g_lpszNavAppIdNtArray;
    nNumAppIds = g_ucNavAppIdNtArrayNum;
    for (nCurAppId = 0; nCurAppId < nNumAppIds; ++nCurAppId)
        {
        if (defObjectPtr->InitInstallApp(lpszAppIdArray[nCurAppId], szDataBuf))
            {
            defObjectPtr->StopUsingDefs();
            }
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
/*
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

										// UnRegister TRANSMAN.DLL
	hInst = LoadLibrary ( _T(".\\transman.dll") );

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
*/
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

BOOL    WriteNavPaths(LPSTR  navPathStr, LPSTR  symantecPathStr
                                                    , LPSTR symantecSharedPathStr)
{
    auto    BOOL        bResult = TRUE;
    auto    CRegistryEntry *cReg;

    // create the temporary registry key
    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, TEMP_REGKEY_PATH
                                                        , TEMP_REGVAL_NAV_PATH);
    if (NULL == cReg)
        {
        return ( FALSE );
        }

    if (ERROR_SUCCESS == cReg->OpenCreateKey(KEY_ALL_ACCESS))
        {
        // write the value data
        cReg->SetValue(REG_SZ, (LPBYTE)navPathStr, _tcsclen(navPathStr) + 1);

        cReg->SetValueName( TEMP_REGVAL_SYMANTEC_PATH );
        cReg->SetValue(REG_SZ, (LPBYTE)symantecPathStr
                                            , _tcsclen(symantecPathStr) + 1);

        cReg->SetValueName( TEMP_REGVAL_SYMANTEC_SHARED_PATH );
        cReg->SetValue(REG_SZ, (LPBYTE)symantecSharedPathStr
                                            , _tcsclen(symantecSharedPathStr) + 1);

        cReg->CloseKey();
        }
    else
        {
        bResult = FALSE;
        }
    delete cReg;

    return (bResult);

}  // end of "WriteNavPaths"


// Quake's NameAppendFile(), pulled locally so the install DLL isn't
// dependent on S32NAVx.DLL.

void LNameAppendFile ( LPTSTR szPath, LPCTSTR szFile )
{
    LPTSTR szPathStart = szPath;
    TCHAR  wChar;

    if (*szPath == '\0')
        {
        lstrcpy(szPath, szFile);
        }
    else
        {
                                        // Append a slash if the last character
                                        // is neither '\' nor ':'.
        szPath = _tcschr ( szPath, '\0' );
        szPath = _tcsdec ( szPathStart, szPath );

        wChar = *szPath;

        szPath = _tcschr ( szPath, '\0' );

        if (*szFile != '\\' && wChar != '\\' && wChar != ':')
            {
            *szPath++ = '\\';
            }

        if ( wChar == '\\' && *szFile == '\\' )
            {
            szFile = _tcsinc ( szFile );
            }

        lstrcpy(szPath, szFile);
        }
}

// ===== RemoveAll =======================================================
//
//  This function removes all files from a target including it's 
//  subdirectories and the files they contain. 
//
//  Input:
//      lpRootDir       -- directory where the target files live
//      lpSubDir        -- the root's subdirectory(ies) to remove
//
//  Output:
//      none, since this is a recursive function removing all files and
//      directories from the root directory
//
// ========================================================================
//  Function created: 5/99, CNEVARE
// ========================================================================

void RemoveAll(const TCHAR* lpRootDir, const TCHAR* lpSubDir)
{

	TCHAR			szDir[MAX_PATH];
	TCHAR			szFileSpec[MAX_PATH];
	WIN32_FIND_DATA l_win32_find_data;
	HANDLE			hFile = NULL;


	// Get the directories to remove
	wsprintf(szDir, "%s\\%s", lpRootDir, lpSubDir);
	wsprintf(szFileSpec, "%s\\%s\\*.*", lpRootDir, lpSubDir);

	// Begin the search
	hFile = ::FindFirstFile(szFileSpec, &l_win32_find_data);

	if(INVALID_HANDLE_VALUE == hFile)
		return;

	do
		{
		TCHAR	szFile[MAX_PATH];

		wsprintf(szFile, "%s\\%s\\%s", lpRootDir, lpSubDir, l_win32_find_data.cFileName);

		// Check for read-only attribute. if so, set it to writable.
		if (l_win32_find_data.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			{
			DWORD dwFileAttributes = l_win32_find_data.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes(szFile, dwFileAttributes);
			}

		// Check for a file or directory
		if(l_win32_find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	
			{	// directory
				if((0 != ::_tcscmp( l_win32_find_data.cFileName, ".")) && (0 != ::_tcscmp(l_win32_find_data.cFileName, "..")))
					{
					RemoveAll(szDir, l_win32_find_data.cFileName);
					}
			}
		else	// file
			{
			::DeleteFile(szFile);
			}
		}
	while(::FindNextFile(hFile, &l_win32_find_data));

	::FindClose(hFile);

	::RemoveDirectory(szDir);
}


// ===== GetArrayLen =======================================================
//
//  This function determines the number of subdirectory(ies) to process
//  
//
//  Input:
//      lpArrayIn     -- An array of pointers to pointers to subdirectories
//
//  Output:
//      The number of subdirectories in the root
//
// ========================================================================
//  Function created: 5/99, CNEVARE
// ========================================================================

int GetArrayLen(TCHAR **lpArrayIn)
{
	TCHAR **lpArray = lpArrayIn;

	while(*lpArray != "")
		*lpArray++;

	return (lpArray - lpArrayIn);
}

///////////////////////////////////////////////////////////////////////////
//
// Function: RemoveServerShares
//
// Description: This function will determine if the current machine is
// sharing predefined folders, and if so, they will get removed.  The
// following shares will be searched:
//
// <TARGETDIR>			"VPHOME"
// <TARGETDIR>\LOGON	"VPLOGON"
// <TARGETDIR>\ALERT	"VPALERT$"
//
///////////////////////////////////////////////////////////////////////////
// 7/7/99 MHOTTA Function Created.
///////////////////////////////////////////////////////////////////////////

DWORD RemoveServerShares()
{
	HINSTANCE hInst;
	NETCHECK pNetCheckProc;
	LPWSTR wTarget;
	TCHAR szTarget[SYM_MAX_PATH];
	DWORD dwType = 0, dwRtn = 0;

	wTarget = 0;
	// make a local copy of the Target directory
	lstrcpy( szTarget, g_szTarget );

	try
	{
		// Get a pointer to the NetShareCheck() function within netapi32.dll
		hInst = LoadLibrary( "netapi32.dll" );

		if( !hInst )
			throw(0);

		pNetCheckProc = (NETCHECK)GetProcAddress( hInst, "NetShareCheck" );

		// first, check and remove the ~\$vphome share.
		CharUpper( szTarget );

		if( ConvertToWide( szTarget, wTarget ) )
			throw(0);

		if( NERR_Success == (*pNetCheckProc)(NULL, wTarget, &dwType) )
			RemoveNTShare( "VPHOME" );

		// reset the target so we can check and remove the ~\$vphome\logon share.
		wTarget = 0;
		lstrcpy( szTarget, g_szTarget );
		lstrcat( szTarget, "logon" );
		CharUpper( szTarget );

		if( ConvertToWide( szTarget, wTarget ) )
			throw(0);

		if( NERR_Success == (*pNetCheckProc)(NULL, wTarget, &dwType) )
			RemoveNTShare( "VPLOGON" );

		// reset the target so we can check and remove the ~\vphome\alert$ share.
		wTarget = 0;
		lstrcpy( szTarget, g_szTarget );
		lstrcat( szTarget, "alert" );
		CharUpper( szTarget );
		
		if( ConvertToWide( szTarget, wTarget ) )
			throw(0);

		if( NERR_Success == (*pNetCheckProc)(NULL, wTarget, &dwType) )
			RemoveNTShare( "VPALERT$" );

	}
	catch( DWORD dwError )
	{
		if( !dwError )
			dwRtn = GetLastError();
		else
			dwRtn = dwError;
	}

	return( dwRtn );

}

///////////////////////////////////////////////////////////////////////////
//
// Function: RemoveNTShare
//
// Description: Will remove the specified NT share.
//
// IN: Name of the NT share
//
///////////////////////////////////////////////////////////////////////////
// 7/7/99 MHOTTA Function Created.
///////////////////////////////////////////////////////////////////////////

DWORD RemoveNTShare( LPTSTR szShareName )
{
	HINSTANCE hLib;
	DWORD dwRet = 0;
	NETDEL pNetDelProc;
	LPWSTR wName=0;

	try
	{
		hLib = LoadLibrary("netapi32.dll");
		if (!hLib)
			throw(0);

		// Reference Procedure Addresses
		pNetDelProc = (NETDEL)GetProcAddress(hLib,"NetShareDel");

		if (!pNetDelProc)
			throw(0);

		if (ConvertToWide(szShareName,wName))
			throw(0);
			
		// Call NetShareDel Function
		(*pNetDelProc)(NULL,(LPTSTR)wName,0);
		// Notify the shell that share information has changed.
		SHChangeNotify(SHCNE_NETUNSHARE,0,0,0);
	}
	catch (DWORD dwError)
	{
		if (!dwError)
			dwRet = GetLastError();
		else
			dwRet = dwError;
	}

	if (wName) delete[] wName;
	if (hLib) FreeLibrary(hLib);

	return dwRet;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: ConvertToWide
//
// Description: Convert string to Unicode
//
///////////////////////////////////////////////////////////////////////////
// 7/7/99 MHOTTA Function lifted from Beluga source.
///////////////////////////////////////////////////////////////////////////

DWORD ConvertToWide(LPCSTR pMultiStr, LPWSTR &pWideStr)
{
	int size;

	if (pWideStr) 
		delete[] pWideStr;

	size = MultiByteToWideChar(CP_ACP,0,pMultiStr, -1,pWideStr,0);
	pWideStr = new wchar_t[size];

	if(!MultiByteToWideChar(CP_ACP,0,pMultiStr, -1,pWideStr,size))
		return GetLastError();

	return 0;
}


////////////////////////////////////////////////////////////////////////////
//
//  PromptUninstallPassword - Function to call Uninstallit in NAVCUST2.DLL
//							  prompt for Uninstall password.
//
//  Params: NONE
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  7/11/99 - CNEVARE, function created
/////////////////////////////////////////////////////////////////////////////

int PromptUninstallPassword()
{
	auto    LONG				lResult = 0;
    auto    CWString			szLibPath(SYM_MAX_PATH);
	auto	CWString			szCurrDir(SYM_MAX_PATH);
	auto	CWString			szCommonFiles(SYM_MAX_PATH);
	auto	HMODULE				hLib = NULL;
	auto	ONUNINSTALLPASSWORD lpFunc = NULL;


	GetNavPaths(g_szTarget, g_szSharedDir, g_szSymSharedDir);

	_tcscpy(szLibPath, g_szTarget);
	szLibPath.AppendFile("navcust2.dll");

	// Load the DLL and prompt for the password
	hLib = LoadLibrary(szLibPath);
	if(hLib != NULL)
		{
		lpFunc = (ONUNINSTALLPASSWORD)GetProcAddress(hLib, _T("Uninstallit"));
		if(lpFunc != NULL)
			{
			// Save the current directory
			GetCurrentDirectory(MAX_PATH, szCurrDir);

			// Set the directory to where TRANSMAN.DLL lives
			_tcscpy(szCommonFiles, g_szSymSharedDir);
			szCommonFiles.AppendFile("ssc");
		    SetCurrentDirectory(szCommonFiles);

			// Call the uninstall dialog function 
			lResult = (lpFunc)();

			// Restore the original directory
			SetCurrentDirectory(szCurrDir);
			}
		}

	if(hLib) FreeLibrary(hLib);

	return lResult;
}

////////////////////////////////////////////////////////////////////////////
//
// Function: ClientIsServer
//
// Description: Checks to see if the NAVCORP client is a server.  This
// function will look at the following value pair in the registry
// to determine the client type:
//
// [HKEY_LOCAL_MACHINE\Software\INTEL\LANDesk\VirusProtect6\CurrentVersion]
// "ClientType"=REG_DWORD
// 
// 1 = Connected Client; 2 = Standalone Client; 4 = Server
//
// Returns: TRUE if Server
//          FALSE otherwise
//
/////////////////////////////////////////////////////////////////////////////
// 7/16/99 MHOTTA Function Created
/////////////////////////////////////////////////////////////////////////////

BOOL ClientIsServer()
{
	CRegistryEntry* cReg;
	DWORD dwType;
	DWORD dwSize;
	DWORD dwDataBuf = 0;
	BOOL bReturn = FALSE;

	cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, 
								SERVER_CHECK_KEY, 
								CLIENTTYPE_KEY );

	if( NULL == cReg )
	{
		return( FALSE );
	}

	if( ERROR_SUCCESS == cReg->OpenKey(KEY_READ) )
	{
		dwSize = sizeof( dwDataBuf );
		if( ERROR_SUCCESS == cReg->GetValue( &dwType, 
											(LPBYTE)&dwDataBuf,
											&dwSize ) )
		{
			switch( dwDataBuf )
			{
			case CLIENTTYPE_CONNECTED:
				bReturn = FALSE;
				break;
			case CLIENTTYPE_STANDALONE:
				bReturn = FALSE;
				break;
			case CLIENTTYPE_SERVER:
				bReturn = TRUE;
				break;
			default:
				break;
			}
		}

		cReg->CloseKey();
	}

    delete cReg;

    return( bReturn );
}

