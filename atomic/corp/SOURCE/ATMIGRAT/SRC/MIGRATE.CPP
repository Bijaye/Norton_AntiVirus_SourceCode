/////////////////////////////////////////////////////////////////////
// File: Migrate.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGRAT/VCS/migrate.cpv   1.5   12 Feb 1999 17:46:46   MBROWN  $
//
// Description:
//  MIGRATE.CPP -   Module that holds mostly install functions for the
//                  InstallShield installation of NAVNT; this is the main
//                  point of entry for the custom DLL.
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/migrate.cpv  $
// 
//    Rev 1.5   12 Feb 1999 17:46:46   MBROWN
// Merged in Roswell migdll code and added version checking so that single migdll works for both Atomic and Roswell
// 
//    Rev 1.4   01 Feb 1999 18:27:06   MBROWN
// Added VENDORINFO stuff
// 
//    Rev 1.3   19 Nov 1998 13:14:30   MBROWN
// Changed method for getting shell folders, deleting NPS links, added entry point for unloading NAV
// 
//    Rev 1.2   11 Nov 1998 12:20:40   MBROWN
// Added more LTrace calls for debugging, checking for NULL pointers, and casting CWStrings to LPSTR
// 
//    Rev 1.1   28 Sep 1998 11:36:48   MBROWN
// Removed options modification, added write to Handled section of inf, modify shell links, move prog groups, etc.
// 
//    Rev 1.0   26 May 1998 14:31:18   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef SYM_WIN32
#define SYM_WIN32
#endif

#include    <windows.h>
#include    <windowsx.h>

#include    <tchar.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "setupapi.h"
#include    "global.h"
#include    "migrate.h"
#include    "migrant.h"
#include    "resource.h"
#include    "utils.h"
#include    "CreatMig.h"

#include    "CWString.h"
#include    "CRegent.h"
#include    "registry.h"
#include    "navwcomm.h"



// ==== DllMain ===========================================================
//
//
// ========================================================================
// ========================================================================

BOOL    WINAPI  DllMain(HANDLE  hInst, ULONG  ul_reason_for_call
                                                        , LPVOID  lpReserved)
{
    switch (ul_reason_for_call)
        {
        case  DLL_PROCESS_ATTACH:

            g_hInst = hInst;
            g_hHeap = GetProcessHeap();

            LTrace("DllMain, DLL_PROCESS_ATTACH");
            #ifdef  SHOW_MESSAGE_BOXES
            MessageBox(NULL, "DLL_PROCESS_ATTACH", "DLL_PROCESS_ATTACH", 0);
            #endif  // SHOW_MESSAGE_BOXES

            g_migrantWorker = CreateMigrant();
            if (TRUE == g_migrantWorker->IsAppInstalled())
                {
                g_migrantWorker->InitAppID();
                }
            break;

        case  DLL_PROCESS_DETACH:
            LTrace("DllMain, DLL_PROCESS_DETACH");
            #ifdef  SHOW_MESSAGE_BOXES
            MessageBox(NULL, "DLL_PROCESS_DETACH", "DLL_PROCESS_DETACH", 0);
            #endif  // SHOW_MESSAGE_BOXES

            if (g_migrantWorker)
                {
                delete  g_migrantWorker;
                }

            break;

        default:
            break;
        }

    return  (TRUE);

}  // end of "DllMain"



// ==== QueryVersion ======================================================
//
//
// ========================================================================
// ========================================================================

LONG
CALLBACK
QueryVersion (
    OUT  LPCSTR *ProductID,
    OUT  LPUINT DllVersion,
    OUT  LPINT *CodePageArray,  OPTIONAL
    OUT  LPCSTR *ExeNamesBuf,   OPTIONAL
    OUT  PVENDORINFO *VendorInfo
    )
{
    auto    DWORD   retVal;

    #ifdef  SHOW_MESSAGE_BOXES
    MessageBox(NULL, "QueryVersion", "QueryVersion", 0);
    #endif  // SHOW_MESSAGE_BOXES
    LTrace("BEGIN QueryVersion");

    // We need to return info regardless of whether the app is installed
    // NOTE: the appropriate fields must be localized!
    
    // InitAppID() only would have been called if app was installed (when DLL loads)
    g_migrantWorker->InitAppID();
    *ProductID = g_migrantWorker->GetAppID();
    *DllVersion = g_migrantWorker->GetDllVersion();
    LoadString(g_hInst, IDS_COMPANY_NAME, g_vendorInfo.CompanyName, 256);
    LoadString(g_hInst, IDS_SUPPORT_NUMBER, g_vendorInfo.SupportNumber, 256);
    LoadString(g_hInst, IDS_SUPPORT_URL, g_vendorInfo.SupportUrl, 256);
    LoadString(g_hInst, IDS_INSTRUCTIONS, g_vendorInfo.InstructionsToUser, 1024);
    *VendorInfo = &g_vendorInfo;

    // if we're installed, return a failure value, else give the setup program
    // what it wants
    if (FALSE == g_migrantWorker->IsAppInstalled())
        {
        retVal = ERROR_NOT_INSTALLED;
        }
    else
        {
        // NOTE: the appropriate fields must be localized!
        *CodePageArray = g_migrantWorker->m_codePageArray;
        *ExeNamesBuf = NULL;    // SCOTT
        retVal = ERROR_SUCCESS;
        }

    LTrace("END QueryVersion");
    return  (retVal);

}  // end if "QueryVersion"




// ==== Initialize9x ======================================================
//
//  "WorkingDirectory" contains a path the migration DLL can use to store
//  its private temporary data.  This directory will contain MIGRATE.DLL
//  and all associated files.  Setup will provide this directory again
//  during the NT phase of upgrade, and automatically deletes it after the
//  upgrade has completed.
//
//  "SourceDirectories" points to the path of the NT installation files.
//
// ========================================================================
// ========================================================================

LONG
CALLBACK
Initialize9x (
	IN      LPCSTR WorkingDirectory,
	IN      LPCSTR SourceDirectories,
	        LPVOID Reserved
    )
{
    auto    DWORD       dwResult;

    #ifdef  SHOW_MESSAGE_BOXES
    MessageBox(NULL, "Initialize9x", "Initialize9x", 0);
    #endif  // SHOW_MESSAGE_BOXES
    LTrace("BEGIN Initialize9x");

    // copy the dirs the upgrade program passed to us into the migrant object
    dwResult = g_migrantWorker->CopyUpgradeDirs(WorkingDirectory, SourceDirectories);
    if (ERROR_SUCCESS != dwResult)
        {
        LTrace("  Initialize9x -- CopyUpgradeDirs() returned %ld", dwResult);
        return (dwResult);
        }

    // generate a fully-qualified path to the migrate.inf file
    g_migrantWorker->GetMigInfPath();

    // write out an incompatibility message so it will be displayed to the user
    
    // Need to base this message on NAV version
    if(g_migrantWorker->IsNavVersion4())
        g_migrantWorker->UpdateMigInfIncompatibleSection(IDS_SETUP_REPORT_NAV4);
    else
        g_migrantWorker->UpdateMigInfIncompatibleSection(IDS_SETUP_REPORT_NAV5);

    // write out handled paths and files so NT won't complain about them
    g_migrantWorker->UpdateMigInfHandledSection();

    LTrace("END Initialize9x");
    return  (ERROR_SUCCESS);

}  // end if "Initialize9x"



// ==== MigrateUser9x =====================================================
//
//
// ========================================================================
// ========================================================================

LONG
CALLBACK
MigrateUser9x (
	IN      HWND ParentWnd,
	IN      LPCSTR UnattendFile,
	IN      HKEY UserRegKey,
	IN      LPCSTR UserName,
	        LPVOID Reserved
	)
{
    auto    DWORD   dwReturnValue = ERROR_SUCCESS;

    #ifdef  SHOW_MESSAGE_BOXES
    MessageBox(NULL, "MigrateUser9x", "MigrateUser9x", 0);
    #endif  // SHOW_MESSAGE_BOXES

    LTrace("BEGIN MigrateUser9x");
    if(UserName)
        LTrace("  UserName = %s", UserName);
    else
        LTrace("  UserName is NULL");


    // use the UserRegKey argument to fetch user-specific data and write
    // it out to the data file (desktop as well)

    // Jim's code writes "handled" for all desktop settings

    LTrace("END MigrateUser9x");
    return  (dwReturnValue);

}  // end if "MigrateUser9x"




// ==== MigrateSystem9x ===================================================
//
//  Gathers system-wide information concerning the target application,
//  saves private data to be used during the call to MigrateSystemNT.
//
//  Return ERROR_SUCCESS if the target app is installed on system.
//  You should also return ERROR_SUCCESS if system-wide changes need to be
//  made for the target app during the NT phase of the upgrade.
//
//  Return ERROR_NOT_INSTALLED if the app is not installed or you require
//  no further processing.
//
// ========================================================================
// ========================================================================

LONG
CALLBACK
MigrateSystem9x (
	IN      HWND ParentWnd,
	IN      LPCSTR UnattendFile,
	        LPVOID Reserved
	)
{
    auto    DWORD       retVal;

    #ifdef  SHOW_MESSAGE_BOXES
    MessageBox(NULL, "MigrateSystem9x", "MigrateSystem9x", 0);
    #endif  // SHOW_MESSAGE_BOXES

    LTrace("BEGIN MigrateSystem9x");
    retVal = ERROR_SUCCESS;

    // MS objected to the fact that if a user cancelled NT setup, they would
    // be left with no boot recored virus protection as a result of calling
    // the following function. Instead, NT setup will notify a user that a 
    // antivirus app is running and suggest disabling boot record protection.
    // Alternately, MS may somehow suppress our boot record virus alert.
    // Either way, we need to do nothing.

    //g_migrantWorker->MigMigrateSystem9x(ParentWnd, UnattendFile, Reserved);

    LTrace("END MigrateSystem9x");
    return (retVal);

}  // end if "MigrateSystem9x"



// ==== InitializeNT ======================================================
//
//  This function is called during Windows NT GUI Mode Setup when the NT
//  phase of the upgrade is ready to start.
//
// ========================================================================
// ========================================================================

LONG
CALLBACK
InitializeNT (
	IN      LPCWSTR WorkingDirectory,
	IN      LPCWSTR SourceDirectories,
	        LPVOID Reserved
	)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = ERROR_SUCCESS;
    auto    int         nResult;
    auto    TCHAR       ansiWorkingDir[MAX_PATH*2];
    auto    TCHAR       ansiSourceDirs[MAX_PATH*2];

    #ifdef  SHOW_MESSAGE_BOXES
    MessageBox(NULL, "InitializeNT", "InitializeNT", 0);
    #endif  // SHOW_MESSAGE_BOXES
    LTrace("BEGIN InitializeNT");

    // we're now in NT land... convert the wide strings to ANSI for the call
    // to "CopyUpgradeDirs"
    nResult = WideCharToMultiByte(CP_ACP, 0, WorkingDirectory, -1, ansiWorkingDir, MAX_PATH*2, NULL, NULL);
    if (0 == nResult)
        {
        dwResult = GetLastError();
        LTrace("  WideCharToMultiByte(WorkingDirectory) failed, dwResult = %0x", dwResult);
        goto  Exit_Function;
        }

    nResult = WideCharToMultiByte(CP_ACP, 0, SourceDirectories, -1, ansiSourceDirs, MAX_PATH*2, NULL, NULL);
    if (0 == nResult)
        {
        dwResult = GetLastError();
        LTrace("  WideCharToMultiByte(SourceDirectories) failed, dwResult = %0x", dwResult);
        goto  Exit_Function;
        }

    LTrace("  InitializeNT -- after WideCharToMultiByte(), ansiWorkingDir = %s, ansiSourceDirs = %s"
                                                                    , ansiWorkingDir, ansiSourceDirs);

    // copy the dirs the upgrade program passed to us into the migrant object
    dwResult = g_migrantWorker->CopyUpgradeDirs(ansiWorkingDir, ansiSourceDirs);
    if (ERROR_SUCCESS != dwResult)
        {
        LTrace("  InitializeNT -- CopyUpgradeDirs() returned %ld", dwResult);
        return (dwResult);
        }

    // generate a fully-qualified path to the migrate.inf file
    g_migrantWorker->GetMigInfPath();

    // copy our files over
    g_migrantWorker->CopyAppFiles();

    // make any registry adjustments
    g_migrantWorker->InitNTRegistry();

    // write out a flag so we'll know the application has been migrated
    g_migrantWorker->WriteMigrationFlag();



Exit_Function:
    LTrace("END InitializeNT");
    return  (dwResult);

}  // end if "InitializeNT"



// ==== MigrateUserNT =====================================================
//
//
// ========================================================================
// ========================================================================

LONG
CALLBACK
MigrateUserNT (
	IN      HINF UnattendInfHandle,
	IN      HKEY UserRegKey,
	IN      LPCWSTR UserName,
            LPVOID Reserved
	)
{
    auto    TCHAR       ansiUserName[MAX_PATH*2];
    auto    int         nResult;
    auto    DWORD       dwResult = ERROR_SUCCESS;
    auto    CWString    szProgramsPath(MAX_PATH*2);
    auto    CWString    szStartUpPath(MAX_PATH*2);

    #ifdef  SHOW_MESSAGE_BOXES
    MessageBox(NULL, "MigrateUserNT", "MigrateUserNT", 0);
    #endif  // SHOW_MESSAGE_BOXES
    LTrace("BEGIN MigrateUserNT");
    // Make sure we don't access a NULL pointer
    if(UserName)
        {
        // we're now in NT land... convert the wide strings to ANSI for LTrace
        nResult = WideCharToMultiByte(CP_ACP, 0, UserName, -1, ansiUserName, MAX_PATH*2, NULL, NULL);
        if (0 == nResult)
            {
            dwResult = GetLastError();
            LTrace("  WideCharToMultiByte(UserName) failed, dwResult = %0x", dwResult);
            LTrace("  UserName not known yet");
            }
        else
            {
            LTrace("  UserName = %s", (LPSTR) ansiUserName);
            }
        
                // Ensure we're starting with an empty buffer
        if(szProgramsPath.IsEmpty())
            {
            LTrace("   szProgramsPath buffer is empty. Calling GetShellFolder.");
            }
        else
            {
            LTrace("  szProgramsPath = %s", (LPSTR) szProgramsPath);
            LTrace("   zeroing out szProgramsPath before calling GetShellFolder.");
            szProgramsPath.Empty();
            }

                // Ensure we're starting with an empty buffer
        if(szStartUpPath.IsEmpty())
            {
            LTrace("   szStartUpPath buffer is empty. Calling GetShellFolder.");
            }
        else
            {
            LTrace("  szStartUpPath = %s", (LPSTR) szStartUpPath);
            LTrace("   zeroing out szStartUpPath before calling GetShellFolder.");
            szStartUpPath.Empty();
            }

        // get this user's program group
        if(g_migrantWorker->GetShellFolder(UserRegKey, "Programs", szProgramsPath))
            {
            LTrace("  GetShellFolder succeeded");
            LTrace("  szProgramsPath = %s", (LPSTR) szProgramsPath);
            // modify start menu link to nsplugin.exe
            g_migrantWorker->ModifyShellLinks(szProgramsPath);

            g_migrantWorker->GetShellFolder(UserRegKey, "Startup", szStartUpPath);
            // move and/or delete users start menu groups
            g_migrantWorker->MoveProgGroups(szProgramsPath, szStartUpPath);
            }
        else
            {
            LTrace("  GetShellFolder failed");
            }

        }
    else
        {
        LTrace("  UserName is NULL");
        }

    LTrace("END MigrateUserNT");

    return ERROR_SUCCESS;

}  // end if "MigrateUserNT"



// ==== MigrateSystemNT ===================================================
//
//
// ========================================================================
// ========================================================================

LONG
CALLBACK
MigrateSystemNT (
	IN      HINF UnattendInfHandle,
            LPVOID Reserved
	)
{
    #ifdef  SHOW_MESSAGE_BOXES
    MessageBox(NULL, "MigrateSystemNT", "MigrateSystemNT", 0);
    #endif  // SHOW_MESSAGE_BOXES
    LTrace("BEGIN MigrateSystemNT");
    LTrace("END MigrateSystemNT");

    return ERROR_SUCCESS;

}  // end if "MigrateSystemNT"


// ==== MigrateNAV9x ===================================================
//
//      This function is called to unload NAVAP. It is called by NT setup 
//      before the actual migration if it detects NAV is loaded. This 
//      ensures that AP doesn't complain when NT setup writes to the boot
//      record.
//
// ========================================================================
//  Function created: 11/98, MBROWN
// ========================================================================
BOOL
CALLBACK
MigrateNAV9x(
    IN      LPCSTR  InputString
    )
{
    UINT            uCustomMessageID;
    HINSTANCE       hInst = NULL;
    HWND            hWnd = NULL;
    LPFNNAVMigCfg   pNAVMigCfg = NULL;
    BOOL            bRet = FALSE;

    // ensure the correct caller is calling us
    if(0 != strcmp(InputString, "microsoft"))
        return (TRUE);

    // first we want to see if a newer version of NAV may have
    // installed a DLL with a new method of unloading NAV.

    // try finding dll in default loadlib locations 
    // (windows dir, system dir, etc.)
    hInst = LoadLibrary("navmig.dll");
    if(hInst)
        {
        // we loaded the DLL so there must be a newer implementation
        pNAVMigCfg = (LPFNNAVMigCfg) GetProcAddress(hInst, "_NAVMigCfg@4");
        if(pNAVMigCfg)
            {
            // call new method and store result
            bRet = pNAVMigCfg("symantec");
            }
        FreeLibrary(hInst);
        }
    else
        {
        // unable to load the library, so use regular method
        // for unloading AP
        uCustomMessageID = RegisterWindowMessage( NAVW_NAVAPW_COMM );

        hWnd = FindWindow("NAVAP Wnd Class", NULL);

        if(hWnd)
            {
            SendMessage(hWnd, uCustomMessageID, NAVW_CMD_UNLOADNAVAPW, 0L);
            // Now see if the window is still around
            hWnd = FindWindow("NAVAP Wnd Class", NULL);
            // if we succeeded, FindWindow should now return NULL
            bRet = (hWnd == NULL);
            }
        else
            {
            // if the AP window doesn't exist, it must not be
            // loaded, so set return value to TRUE
            bRet = TRUE;
            }
    
        }

    return (bRet);
}