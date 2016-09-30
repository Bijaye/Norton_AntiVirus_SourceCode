/////////////////////////////////////////////////////////////////////
// File: NavMig95.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGRAT/VCS/navmig95.cpv   1.8   12 Feb 1999 17:46:44   MBROWN  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/navmig95.cpv  $
// 
//    Rev 1.8   12 Feb 1999 17:46:44   MBROWN
// Merged in Roswell migdll code and added version checking so that single migdll works for both Atomic and Roswell
// 
//    Rev 1.7   19 Nov 1998 13:14:32   MBROWN
// Changed method for getting shell folders, deleting NPS links, added entry point for unloading NAV
// 
//    Rev 1.6   11 Nov 1998 12:20:42   MBROWN
// Added more LTrace calls for debugging, checking for NULL pointers, and casting CWStrings to LPSTR
// 
//    Rev 1.5   01 Oct 1998 13:09:04   tcashin
// Moved the navopts version check back to Atomic.
// 
//    Rev 1.4   28 Sep 1998 11:36:52   MBROWN
// Removed options modification, added write to Handled section of inf, modify shell links, move prog groups, etc.
// 
//    Rev 1.3   23 Sep 1998 17:27:46   tcashin
// Bumped the navopts version number for Barstow.
// 
//    Rev 1.2   31 Jul 1998 10:16:54   tcashin
// Bumped up the NAVOPTS.DAT version number for Atomic.
// 
//    Rev 1.1   18 Jun 1998 18:21:14   SEDWARD
// Changed reference from IDS_PRODUCT_ID_NAV40 to "NAV50".
//
//    Rev 1.0   26 May 1998 14:31:26   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include    <windows.h>
#include    <windowsx.h>
#include    <shlobj.h>
#include    "global.h"
#include    "Migrant.h"
#include    "NavMig95.h"
#include    "CWString.h"
#include    "CRegEnt.h"
#include    "registry.h"
#include    "resource.h"
#include    "Utils.h"
#include    "shelllnk.h"



// initialize some static parent class members
CHAR*  CMigrant::m_szInstallDirFiles[] =
    {
    ""
    };

CHAR*   CMigrant::m_szProgramFilesSymantecFiles[] =
    {
    ""
    };

CHAR*   CMigrant::m_szCommonFilesSymantecFiles[] =
    {
    ""
    };

CHAR*   CMigrant::m_szMigInfExeFiles[] =
    {
    ""
    };

CHAR*   CMigrant::m_szSymantecSharedRegkeys[] =
    {
    "s32evnt1.dll"
    , "symevnt.386"
    , "symevnt1.dll"
    , "s32stat.dll"
    , ""
    };

INT     CMigrant::m_codePageArray[] = { CP_932
                                        , CP_936
                                        , CP_949
                                        , CP_950
                                        , CP_1250
                                        , CP_1251
                                        , CP_1252
                                        , CP_1255
                                        , END_OF_CODEPAGES };


// constructor
CNavMigrant95::CNavMigrant95()
{
    m_dllVersion = NAV_DLL_VERSION;
    if (TRUE == CheckAppInstallation())
        {
        InitAppDirs();
        }
}


// destructor
CNavMigrant95::~CNavMigrant95(void)
{

}





// ==== CheckAppInstallation ==============================================
//
//  This function checks to see if NAV is installed on the target system.
//  If so, it will return a success value and attempt to set the "version"
//  member variable to a specific version of NAV.
//
//  Input:  nothing
//  Output: a value of TRUE if the target system has NAV installed, a value
//          of FALSE if it does not; the "m_versionNav" member variable may
//          also be initialized with more specific information.
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CNavMigrant95::CheckAppInstallation(void)
{
    auto    BOOL                bResult = FALSE;
    auto    CRegistryEntry*     cReg = NULL;
    auto    CWString            szTemp(MAX_PATH*2);
    auto    DWORD               dwVersionData = 0;
    auto    DWORD               dwVersionSize = 0;
    auto    DWORD               dwVersionType = 0;

    LTrace("BEGIN CNavMigrant95::CheckAppInstallation");

    // create the regkey object
    szTemp = "SOFTWARE\\Symantec\\Norton AntiVirus";
    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, szTemp, "CurrentVersion");
    if (NULL == cReg)
        {
        goto  Exit_Function;
        }
    cReg->OpenCreateKey(KEY_ALL_ACCESS);

    // fetch the internal version number info
    dwVersionType = REG_BINARY;
    dwVersionSize = sizeof(dwVersionSize);
    if (ERROR_SUCCESS == cReg->GetValue(&dwVersionType, (LPBYTE)&dwVersionData
                                                                , &dwVersionSize))
        {
        // if the current installation of NAV is Roswell95, set a global flag so
        // we'll know later to copy an updated version of the uninstall DLL to
        // the system
        if (5 == dwVersionData)
            {
            LTrace("  m_versionNav = INSTALLATION_ROSWELL");
            m_versionNav = INSTALLATION_ROSWELL;
            }
        else if (5 < dwVersionData)
            {
            LTrace("  m_versionNav = INSTALLATION_ATOMIC");
            m_versionNav = INSTALLATION_ATOMIC;
            }
        else
            {
            LTrace("  m_versionNav != INSTALLATION_ROSWELL");
            LTrace("  m_versionNav != INSTALLATION_ATOMIC");
            // Use zero to indicate that an unknown version is installed
            m_versionNav = 0;
            }

        bResult = TRUE;
        }
    else
        {
        // we can't get a valid version number, so assign an invalid value
        m_versionNav = -1;
        }


Exit_Function:
    if (NULL != cReg)
        {
        delete  cReg;
        }

    LTrace("END CNavMigrant95::CheckAppInstallation");
    return  (bResult);


}  // end of "CNavMigrant95::CheckAppInstallation"



// ==== CopyAppFiles ======================================================
//
//  This function performs further file copy functionality beyond what the
//  default parent function does.  (NOTE: if you simply need files copied
//  to any of the "standard" directories, you should be able to use the
//  parent function only.)
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, a value of FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CNavMigrant95::CopyAppFiles(void)
{
    auto    BOOL        bResult = TRUE;
    auto    CWString    szDestFile;
    auto    CWString    szSourceFile;

    LTrace("BEGIN CNavMigrant95::CopyAppFiles");

    // first call the parent function, then we'll do our own thing
    CMigrant::CopyAppFiles();

    // if we have no working directory, something's very wrong!
    if (TRUE == m_szMigWorkingDir.IsEmpty())
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // If version is NAV4, we need to copy navinst95.dll to install dir
    if(m_versionNav == INSTALLATION_ROSWELL)
        {
        szSourceFile = m_szMigWorkingDir;
        szSourceFile += "\\";
        szSourceFile += NAV95_INST_DLL;
        szDestFile = m_szAppInstallDir;
        szDestFile += "\\";
        szDestFile += NAV95_INST_DLL;
        LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
        if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
            {
            LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
            bResult = FALSE;
            }
        }
    
    // copy MIGAPP_EXE over the main NAV executables, so that if they are ever
    // executed a dialog will be displayed informing the user they need to upgrade
    szSourceFile = m_szMigWorkingDir;
    szSourceFile += "\\";
    szSourceFile += MIGAPP_EXE;
    szDestFile = m_szAppInstallDir;
    szDestFile += "\\";
    szDestFile += NAV95_EXE_NAME;
    LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
    if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
        {
        LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
        bResult = FALSE;
        }

    szDestFile = m_szAppInstallDir;
    szDestFile += "\\";
    szDestFile += NSCHED32_EXE_NAME ;
    LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
    if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
        {
        LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
        bResult = FALSE;
        }

    szDestFile = m_szAppInstallDir;
    szDestFile += "\\";
    szDestFile += NAVLU32_EXE_NAME;
    LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
    if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
        {
        LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
        bResult = FALSE;
        }

    szDestFile = m_szAppInstallDir;
    szDestFile += "\\";
    szDestFile += RESCUE_EXE_NAME   ;
    LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
    if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
        {
        LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
        bResult = FALSE;
        }

    szDestFile = m_szAppInstallDir;
    szDestFile += "\\";
    szDestFile += NAVAPW_EXE_NAME;
    LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
    if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
        {
        LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
        bResult = FALSE;
        }

    szDestFile = m_szAppInstallDir;
    szDestFile += "\\";
    szDestFile += NAVDX_EXE_NAME;
    LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
    if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
        {
        LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
        bResult = FALSE;
        }



Exit_Function:
    LTrace("END CNavMigrant95::CopyNavntFiles");
    return  (bResult);

}  // end of "CNavMigrant95::CopyAppFiles"



// ==== InitAppDirs =======================================================
//
//  This function initializes the three directory member variables with the
//  three primary NAV/Symantec directory paths.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, a value of FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CNavMigrant95::InitAppDirs(void)
{
    auto    BOOL                bRetVal = TRUE;
    auto    CRegistryEntry*     cReg = NULL;
    auto    CWString            szTemp(MAX_PATH*2);
    auto    DWORD               dwBufSize;
    auto    DWORD               dwType;

    LTrace("BEGIN CNavMigrant95::InitAppDirs");

    // initialize the string object with the NAV product regkey
    szTemp = NAV95_KEY;
    szTemp += "\\Install\\";

    if(m_versionNav == INSTALLATION_ROSWELL)
        szTemp += NAV4_INTERNAL_VERSION;
    else
        szTemp += NAV5_INTERNAL_VERSION;

    LTrace("  Created registry object, szTemp = %s", (LPSTR)szTemp);

    // open the NAV registry key
    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, szTemp, "InstallDir");
    if ( NULL == cReg )
        {
        LTrace("  Failed to create registry object for InstallDir");
        bRetVal = FALSE;
        }


    // now get the directories...

    // nav install dir
    dwBufSize = (MAX_PATH*2) - 1;
    if (ERROR_SUCCESS != cReg->GetValue(&dwType, (LPBYTE)(LPSTR)m_szAppInstallDir, &dwBufSize))
        {
        LTrace("  Failed to GetValue for InstallDir");
        LTrace("  m_szAppInstallDir = %s", (LPSTR)m_szAppInstallDir);
        // Let's see if this is ver 5.01.04, which had it's internal version 
        // incremented for mysterious reasons.
        if(m_versionNav != INSTALLATION_ROSWELL)
            {
            LTrace("  Did somebody change our internal version?");
            // It's not Roswell, and the info wasn't in 5.3, so it's probably in 5.31
            if (NULL != cReg)
                {
                delete  cReg;
                }
            // initialize the string object with the NAV product regkey
            szTemp = NAV95_KEY;
            szTemp += "\\Install\\";
            szTemp += NAV5_INTERNAL_VERSION_ALT;
            LTrace("  Created registry object, szTemp = %s", (LPSTR)szTemp);

            // open the NAV registry key
            cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, szTemp, "InstallDir");
            if ( NULL == cReg )
                {
                LTrace("  Failed again to create registry object for InstallDir");
                bRetVal = FALSE;
                }
            // now get the directories...

            // nav install dir
            dwBufSize = (MAX_PATH*2) - 1;
            if (ERROR_SUCCESS != cReg->GetValue(&dwType, (LPBYTE)(LPSTR)m_szAppInstallDir, &dwBufSize))
                {
                LTrace("  Failed to GetValue for InstallDir");
                LTrace("  m_szAppInstallDir = %s", (LPSTR)m_szAppInstallDir);
                m_szAppInstallDir.Empty();
                bRetVal = FALSE;
                }
            }
        else
            {
            // It's Roswell
            m_szAppInstallDir.Empty();
            bRetVal = FALSE;
            }
        }
    else
        {
        LTrace("  m_szAppInstallDir = %s", (LPSTR)m_szAppInstallDir);
        }

    // shared components
    cReg->SetValueName("SharedComponents");
    dwBufSize = (MAX_PATH*2) - 1;
    if (ERROR_SUCCESS != cReg->GetValue(&dwType, (LPBYTE)(LPSTR)m_szProgramFilesSymantecDir, &dwBufSize))
        {
        LTrace("  Failed to GetValue for SharedComponents");
        LTrace("  m_szProgramFilesSymantecDir = %s", (LPSTR)m_szProgramFilesSymantecDir);
        m_szProgramFilesSymantecDir.Empty();
        bRetVal = FALSE;
        }
    else
        {
        LTrace("  m_szProgramFilesSymantecDir = %s", (LPSTR)m_szProgramFilesSymantecDir);
        }

    // symantec shared
    cReg->SetValueName("SymantecShared");
    dwBufSize = (MAX_PATH*2) - 1;
    if (ERROR_SUCCESS != cReg->GetValue(&dwType, (LPBYTE)(LPSTR)m_szCommonFilesSymantecDir, &dwBufSize))
        {
        LTrace("  Failed to GetValue for SymantecShared");
        LTrace("  m_szCommonFilesSymantecDir = %s", (LPSTR)m_szCommonFilesSymantecDir);
        m_szCommonFilesSymantecDir.Empty();
        bRetVal = FALSE;
        }
    else
        {
        LTrace("  m_szCommonFilesSymantecDir = %s", (LPSTR)m_szCommonFilesSymantecDir);
        }

    if (NULL != cReg)
        {
        delete  cReg;
        }

    LTrace("END CNavMigrant95::InitAppDirs()");

    return (bRetVal);


}  // end of "CNavMigrant95::InitAppDirs"



// ==== InitAppID =========================================================
//
//  This function initializes the member productID variable with the value
//  contained in the migration DLL's string table.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, a value of FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CNavMigrant95::InitAppID(void)
{
    auto    int     numBytes;

    // initialize the product ID string
    if(m_versionNav == INSTALLATION_ROSWELL)
        numBytes = LoadString(g_hInst, IDS_PRODUCT_ID_NAV40, m_szProductID, MAX_PATH);
    else
        numBytes = LoadString(g_hInst, IDS_PRODUCT_ID_NAV50, m_szProductID, MAX_PATH);

    if (numBytes > 0)
        {
        return  TRUE;
        }
    else
        {
        return  FALSE;
        }


}  // end of "CNavMigrant95::InitAppID"



// ==== InitNTRegistry ====================================================
//
//  This function is used to setup the NT5 registry.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, a value of FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CNavMigrant95::InitNTRegistry(void)
{
    auto    BOOL                bResult = TRUE;
    auto    CRegistryEntry*     cReg = NULL;
    auto    CWString            szTemp(MAX_PATH*2);

    LTrace("BEGIN CNavMigrant95::InitNTRegistry");

    // we want to remove the registry key that inserts the "Scan" item in the
    // right-click context menu; selecting this causes the NT5 window currently
    // open to crash!!

    // create the regkey object
    szTemp = "*\\shellex\\ContextMenuHandlers\\NortonAntivirus";
    cReg = new CRegistryEntry(HKEY_CLASSES_ROOT, szTemp);
    if (NULL == cReg)
        {
        LTrace("  Failed to create regkey object: %s", (LPSTR)szTemp);
        goto  Exit_Function;
        }
    LTrace("  Successfully created regkey object: %s", (LPSTR)szTemp);

    cReg->DeleteKey(TRUE);  // delete any subkeys
    delete  cReg;


Exit_Function:
    LTrace("END CNavMigrant95::InitNTRegistry");
    return (bResult);

}  // end of "CNavMigrant95::InitNTRegistry"






// ==== WriteMigrationFlag ================================================
//
//  This function writes a "migrated" value to the registry so that when
//  the system boots into NT5, we can know for sure if the version of NAV
//  on the system has been migrated.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, a value of FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CNavMigrant95::WriteMigrationFlag(void)
{
    auto    BOOL                bResult = TRUE;
    auto    CRegistryEntry*     cReg = NULL;
    auto    CWString            szTemp(MAX_PATH);
    auto    DWORD               dwData = 1;


    LTrace("BEGIN CNavMigrant95::WriteMigrationFlag");

    // create the registry object
    szTemp = NAV95_KEY;
    szTemp += "\\Install\\";

    if(m_versionNav == INSTALLATION_ROSWELL)
        szTemp += NAV4_INTERNAL_VERSION;
    else
        szTemp += NAV5_INTERNAL_VERSION;

    LTrace("  Created registry object, szTemp = %s", (LPSTR)szTemp);
    cReg = new CRegistryEntry(HKEY_LOCAL_MACHINE, szTemp, "Migrated");
    if ( NULL == cReg )
        {
        LTrace("  Failed to create registry object for InstallDir");
        bResult = FALSE;
        goto  Exit_Function;
        }

    // write the migration flag to the registry
    if (ERROR_SUCCESS == cReg->OpenKey(KEY_WRITE))
        {
        cReg->SetValue(REG_DWORD, (LPBYTE)&dwData, sizeof(dwData));
        }

    // free the object
    delete  cReg;


Exit_Function:
    LTrace("END CNavMigrant95::WriteMigrationFlag");
    return (bResult);

}  // end of "CNavMigrant95::WriteMigrationFlag"


// ==== ModifyShellLinks ====================================================
//
//  This function modifies one of the links in the NAV start menu group
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, a value of FALSE otherwise
//
// ========================================================================
//  Function created: 9/98, MBROWN
// ========================================================================
BOOL    CNavMigrant95::ModifyShellLinks(TCHAR* szProgramsPath)
{
    CWString    szFolder(2*MAX_PATH);
    CWString    szTarget(2*MAX_PATH);
    CWString    szName(2*MAX_PATH);
    CWString    szIconPath(2*MAX_PATH);
    BOOL        bRet = FALSE;

    LTrace("BEGIN CNavMigrant95::ModifyShellLinks");

    if((0 == LoadString(g_hInst, IDS_NAV_DEST_FOLDER, (LPSTR) szFolder, MAX_PATH)) ||
       (0 == LoadString(g_hInst, IDS_START_PLUG_NETSCAPE, (LPSTR) szName, MAX_PATH)))
        {
        LTrace("  Unable to load resource strings");
        bRet = FALSE;
        goto  Exit_Function;
        }

    LTrace("  Done loading resource strings");

    szIconPath = m_szAppInstallDir;
    szIconPath += "\\";
    szIconPath += NSPLUGIN_EXE_NAME;
    LTrace("  szIconPath = %s", (LPSTR) szIconPath);

    szTarget = m_szAppInstallDir;
    szTarget += "\\";
    szTarget += NAV95_EXE_NAME;
    LTrace("  szTarget = %s", (LPSTR) szTarget);


    if(!MyShellLinkCreate(szFolder,      // "Norton AntiVirus"
                      szTarget,             // full path to NAVW32.EXE which is really MIGAPP.EXE
                      NULL,
                      szName,               // "Plug into Netscape Navigator"
                      szIconPath,           // full path to NSPlugin.exe (so that icon remains correct)
                      0,
                      0,
                      m_szAppInstallDir,    // Install dir
                      szProgramsPath))      // ...\Start Menu\Programs
        {
        bRet = FALSE;
        LTrace("  MyShellLinkCreate failed.");
        }
    else
        {
        bRet = TRUE;
        LTrace("  MyShellLinkCreate succeeded.");
        }

Exit_Function:
    LTrace("END CNavMigrant95::ModifyShellLinks");
    return(bRet);
}

// ==== MoveProgGroups ====================================================
//
//  This function is used to move the current users NAV start menu groups
//  to the All Users start menu. It does that the first time, then on sub-
//  sequent calls, it deletes the current users NAV group.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, a value of FALSE otherwise
//
// ========================================================================
//  Function created: 9/98, MBROWN
// ========================================================================
BOOL    CNavMigrant95::MoveProgGroups(TCHAR* szProgramsPath, TCHAR* szStartUpPath)
{
    CWString    szNAVFolder(2*MAX_PATH);
    CWString    szProdSupFolder(MAX_PATH);
    CWString    szNPSLink(MAX_PATH);
    CWString    szCurrentUserNAVGroup(2*MAX_PATH);
    CWString    szAllUsersNAVGroup(2*MAX_PATH);
    CWString    szCurrentUserProdSupGroup(MAX_PATH*2);
    CWString    szAllUsersProdSupGroup(2*MAX_PATH);
    CWString    szCurrentUserNPSLink(MAX_PATH*2);
    BOOL        bRet = FALSE;

    LTrace("BEGIN CNavMigrant95::MoveProgGroups");

    if((0 == LoadString(g_hInst, IDS_NAV_DEST_FOLDER, szNAVFolder, MAX_PATH)) ||
       (0 == LoadString(g_hInst, IDS_PRODSUP_FOLDER, szProdSupFolder, MAX_PATH)))
        {
        LTrace("  Unable to load resource strings");
        bRet = FALSE;
        goto  Exit_Function;
        }

    // Delete the 'Norton Program Scheduler' from the StartUp group
    if(LoadString(g_hInst, IDS_NPS_LINK, szNPSLink, MAX_PATH))
        {
        szCurrentUserNPSLink = szStartUpPath;
        szCurrentUserNPSLink += "\\";
        szCurrentUserNPSLink += szNPSLink;
        szCurrentUserNPSLink += ".lnk";
        LTrace("Deleting %s", (LPSTR) szCurrentUserNPSLink);
        DeleteFile(szCurrentUserNPSLink);
        }

    // Verify "Norton AntiVirus" program group exists. If not, bail.

    // Build the NAV programs path
    szCurrentUserNAVGroup = szProgramsPath;
    szCurrentUserNAVGroup += "\\";
    szCurrentUserNAVGroup += szNAVFolder;

    LTrace("  szCurrentUserNAVGroup = %s", (LPSTR) szCurrentUserNAVGroup);

    szCurrentUserProdSupGroup = szCurrentUserNAVGroup;
    szCurrentUserProdSupGroup += "\\";
    szCurrentUserProdSupGroup += szProdSupFolder;
    LTrace("  szCurrentUserProdSupGroup = %s", (LPSTR) szCurrentUserProdSupGroup);

    // and see if the NAV group exists    
    if(!DirExists(szCurrentUserNAVGroup))
        {
        LTrace("  No NAV group for this user");
        bRet = TRUE;
        goto  Exit_Function;
        }

    LTrace("  NAV group found for this user");

// If so, create "Norton AntiVirus" group under "All Users"
// and move *.* from current user prog group to 'All Users' prog group.

    // Get "All Users" program paths
    if(!MyShellLinkBuildPath(szNAVFolder,
                         CSIDL_COMMON_PROGRAMS,
                         NULL,
                         NULL,
                         szAllUsersNAVGroup))
        {
        LTrace("  Unable to build 'All Users' start menu NAV path");
        bRet = FALSE;
        goto  Exit_Function;
        }
    LTrace("  szAllUsersNAVGroup = %s", (LPSTR) szAllUsersNAVGroup);

    szAllUsersProdSupGroup = szAllUsersNAVGroup;
    szAllUsersProdSupGroup += "\\";
    szAllUsersProdSupGroup += szProdSupFolder;
    LTrace("  szAllUsersProdSupGroup = %s", (LPSTR) szAllUsersProdSupGroup);

    // if it already exists, just delete this users group
    if(DirExists(szAllUsersNAVGroup))
        {
        LTrace("  Found %s directory", (LPSTR) szAllUsersNAVGroup);
        RemoveFolder(szCurrentUserNAVGroup);
        if(m_versionNav == INSTALLATION_ROSWELL)
            {
            RemoveFolder(szCurrentUserProdSupGroup);
            }
        }
    // otherwise create it and move links
    else
        {
        LTrace("  %s directory not found", (LPSTR) szAllUsersNAVGroup);
        if(CreateDirectory(szAllUsersNAVGroup, NULL))
            {
            LTrace("  Created 'All Users' NAV group");
            // If there's a prod supp group, we need to handle it as well
            if((m_versionNav == INSTALLATION_ROSWELL) && DirExists(szCurrentUserProdSupGroup))
                {
                LTrace("  Found current users 'prod sup online' group");
                if(CreateDirectory(szAllUsersProdSupGroup, NULL))
                    {
                    LTrace("  Created 'All Users' 'prod sup online' directory");
                    MoveFolder(szCurrentUserProdSupGroup, szAllUsersProdSupGroup, TRUE);
                    }
                else
                    {
                    LTrace("  Unable to create 'All Users' 'prod sup online' group");
                    bRet = FALSE;
                    goto  Exit_Function;
                    }
                }
            // group creation succeeded so move folder
            MoveFolder(szCurrentUserNAVGroup, szAllUsersNAVGroup, TRUE);
            }
        else
            // group creation failed so indicate failure
            {
            LTrace("  Unable to create 'All Users' NAV group");
            bRet = FALSE;
            goto  Exit_Function;
            }
        }

    bRet = TRUE;

Exit_Function:
    LTrace("END CNavMigrant95::MoveProgGroups");
    return(bRet);
}

// ==== MoveFolder ========================================================
//
//  This function moves the .lnk files (shortcuts) from the source folder
//  to the destination folder. It can then delete the source folder.
//
//  Input:  
//      szSrcDir        -- this is the folder we're copying from.
//
//      szDestDir       -- this is the folder we're copying to.
//
//      bRemoveDir      -- TRUE to remove the folder after deleting the
//                         contents, FALSE to leave it.
//   
//  Output: nothing
//
// ========================================================================
//  Function created: 9/98, MBROWN
// ========================================================================
void CNavMigrant95::MoveFolder(TCHAR* szSrcDir, TCHAR* szDestDir, BOOL bRemoveDir)
{
    HANDLE              fileHandle;
    WIN32_FIND_DATA     fileData;
    CWString            szSpec(2*MAX_PATH);
    CWString            szSource(2*MAX_PATH);
    CWString            szDest(2*MAX_PATH);

    LTrace("BEGIN CNavMigrant95::MoveFolder");

    szDest = szDestDir;
    szSpec = szSrcDir;
    szSpec += "\\*.lnk";

    while(INVALID_HANDLE_VALUE != (fileHandle = FindFirstFile(szSpec, &fileData)))
        {
        szSource = szSrcDir;
        szSource.AppendFileName(fileData.cFileName);
        szDest = szDestDir;
        szDest.AppendFileName(fileData.cFileName);

        FindClose(fileHandle);

        // Copy the file to the dest dir
        LTrace("    Moving %s", (LPSTR) szSource);
        LTrace("     to %s", (LPSTR) szDest);
        MoveFile(szSource, szDest);
        }

    // now delete the source dir
    if(bRemoveDir)
        {
        LTrace("    Deleting the %s folder", (LPSTR) szSrcDir);
        RemoveDirectory(szSrcDir);
        }

    LTrace("END CNavMigrant95::MoveFolder");
    return;
}

// ==== MoveFolder ========================================================
//
//  This function deletes the .lnk files (shortcuts) from a folder
//  and then deletes the folder.
//
//  Input:  
//      szDir           -- this is the folder we're removing.
//
//  Output: nothing
//
// ========================================================================
//  Function created: 9/98, MBROWN
// ========================================================================
void CNavMigrant95::RemoveFolder(TCHAR* szDir)
{
    HANDLE              fileHandle;
    WIN32_FIND_DATA     fileData;
    CWString            szSpec(2*MAX_PATH);
    CWString            szSource(2*MAX_PATH);

    LTrace("BEGIN CNavMigrant95::RemoveFolder");

    szSource = szDir;
    szSpec = szDir;
    szSpec += "\\*.lnk";

    while(INVALID_HANDLE_VALUE != (fileHandle = FindFirstFile(szSpec, &fileData)))
        {
        szSource = szDir;
        szSource.AppendFileName(fileData.cFileName);
        FindClose(fileHandle);
        LTrace("Deleting %s", (LPSTR) szSource);
        DeleteFile(szSource);
        }

    // now delete the source dir
    LTrace("Deleting the %s folder", (LPSTR) szDir);
    RemoveDirectory(szDir);

    LTrace("END CNavMigrant95::RemoveFolder");
    return;
}

// ==== GetShellFolder ========================================================
//
//  This function uses the open key passed in by NT setup to query the registry
//  and find the appropriate shell folder.
//
//  Input:  
//      UserRegKey      -- open users key passed into MigrateUserNT (don't close key!)
//      lpFolder        -- name of subkey to query
//
//  Output: 
//      lpOutput        -- returned name of shell folder
//
// ========================================================================
//  Function created: 11/98, MBROWN
// ========================================================================
BOOL CNavMigrant95::GetShellFolder(HKEY UserRegKey, TCHAR* lpFolder, TCHAR* lpOutput)
{
    TCHAR       pBuffer[MAX_PATH*2];
    LPBYTE      pByte = (LPBYTE)pBuffer;
    DWORD       dwSize = sizeof(pBuffer);
    DWORD       dwType = 0;
    DWORD       dwErr = 0;
    HKEY        hkeyFolders = NULL;
    BOOL        bRet = FALSE;

    LTrace("BEGIN CNavMigrant95::GetShellFolder");

    dwErr = RegOpenKey(UserRegKey, 
                       "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders",
                       &hkeyFolders);
           
    if (dwErr == ERROR_SUCCESS)
        {
        dwErr = RegQueryValueEx(hkeyFolders, lpFolder, NULL, &dwType, pByte, &dwSize);
        if (dwErr == ERROR_SUCCESS)
            {
            if(ExpandEnvironmentStrings(pBuffer,lpOutput,MAX_PATH*2))
                bRet = TRUE;
            }
        RegCloseKey(hkeyFolders);
        }

    LTrace("END CNavMigrant95::GetShellFolder");
    return (bRet);
}

// ==== MigMigrateSystem9x ================================================
//
//  This function reads the NAVOPTS data from an open NAV options data file.
//  Since we're only supporting v4.0 of NAV in our current migration
//  effort, we don't need to pull in all the Quake stuff.
//
//  Input:
//      hFile           -- a handle to the opened NAVOPTS file
//      lpNavOptions    -- a pointer to a NAVOPTS structure
//
//  Output:
//      A value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 2/98, SEDWARD
// ========================================================================
/*
LONG    CNavMigrant95::MigMigrateSystem9x(HWND  ParentWnd, LPCSTR  UnattendFile
                                                        , LPVOID  Reserved)
{
    auto    LONG        retVal = ERROR_SUCCESS;

    LTrace("BEGIN CNavMigrant95::MigMigrateSystem9x");
    UpdateNavoptsFile();
    LTrace("END CNavMigrant95::MigMigrateSystem9x");
    return (retVal);

}  // end of "CNavMigrant95::MigMigrateSystem9x"

*/

// ==== MigReadNavBinFromOpenedFile =======================================
//
//  This function reads the NAVOPTS data from an open NAV options data file.
//  Since we're only supporting v4.0 of NAV in our current migration
//  effort, we don't need to pull in all the Quake stuff.
//
//  Input:
//      hFile           -- a handle to the opened NAVOPTS file
//      lpNavOptions    -- a pointer to a NAVOPTS structure
//
//  Output:
//      A value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 2/98, SEDWARD
// ========================================================================
/*
BOOL    CNavMigrant95::MigReadNavBinFromOpenedFile(HANDLE  hFile
                                                , LPNAVOPTIONS  lpNavOptions)
{
    auto    BOOL            bResult = FALSE;
    auto    DWORD           dwNumBytesRead = 0;
    auto    NAVBINHEADER    navBinHeader;


    LTrace("BEGIN CNavMigrant95::MigReadNavBinFromOpenedFile");

    // skip over the "navbin" header
    if (ReadFile(hFile, (void*)&navBinHeader, sizeof(navBinHeader), &dwNumBytesRead, NULL))
        {
        // read option settings data
        auto    DWORD   dwVersion = NAV_VERSION_50;
        if  ((navBinHeader.dwVersion == dwVersion)
        &&  (ReadFile(hFile, (void*)lpNavOptions, sizeof(NAVOPTIONS), &dwNumBytesRead, NULL)))
            {
            LTrace("  ReadFile succeeded");
            bResult = TRUE;
            //NavOptsOemToAnsi ( lpNavOptions );
            }
        }

    LTrace("END CNavMigrant95::MigReadNavBinFromOpenedFile");
    return (bResult);

}  // end of "MigReadNavBinFromOpenedFile"
*/


// ==== MigWriteNavBinFromOpenedFile ======================================
//
//  This function writes the "navopts" structure to the open file handle
//  (of course, this is assumed to be a valid "navopts.dat" file).
//  Since we're only supporting v4.0 of NAV in our current migration
//  effort, we don't need to pull in all the Quake stuff.
//
//  Input:
//      hFile           -- a handle to the opened NAVOPTS file
//      lpNavOptions    -- a pointer to a NAVOPTS structure
//
//  Output:
//      A value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 2/98, SEDWARD
// ========================================================================
/*
BOOL    CNavMigrant95::MigWriteNavBinFromOpenedFile(HANDLE  hFile
                                                , LPNAVOPTIONS  lpNavOptions)
{
    auto    BOOL            bResult = TRUE;
    auto    DWORD           dwResult = 0;
    auto    DWORD           dwNumBytesRead = 0;
    auto    DWORD           dwBytesWritten = 0;
    auto    DWORD           dwVersion = NAV_VERSION_50;
    auto    NAVBINHEADER    navBinHeader;


    LTrace("BEGIN CNavMigrant95::MigWriteNavBinFromOpenedFile");

    // position ourselves at the beginning of the file
    dwResult = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    if (0xFFFFFFFF == dwResult)
        {
        LTrace("  SetFilePointer FAILED");
        bResult = FALSE;
        goto  Exit_Function;
        }

    // skip over the "navbin" header
    if (ReadFile(hFile, (void*)&navBinHeader, sizeof(NAVBINHEADER)
                                                        , &dwNumBytesRead, NULL))
        {
        if  ((navBinHeader.dwVersion == dwVersion)
        &&  (WriteFile(hFile, (void*)lpNavOptions, sizeof(NAVOPTIONS)
                                                        , &dwBytesWritten, NULL)))
            {
            LTrace("  WriteFile succeeded");
            //NavOptsAnsiToOem (&NavOptions);
            bResult = TRUE;
            }
        }

Exit_Function:
    LTrace("END CNavMigrant95::MigWriteNavBinFromOpenedFile");
    return (bResult);

}  // end "MigWriteNavBinFromOpenedFile"
*/


// ==== UpdateNavoptsFile =================================================
//
//  This function modifies the "navopts.dat" file so that the upgrade
//  program can modify the boot record to boot into NT5.  Currently, if AP
//  is running and the user attempts to upgrade from Win95 to NT5, when
//  an attempt to modify the boot record is made an AP alert appears before
//  booting into NT5 (somewhat disruptive, no?).  Disabling AP for the
//  current process isn't working (MS is probably spawning another process
//  or thread to do the modification), so we've opted for the easy way out.
//
//  Input:
//      hFile           -- a handle to the opened NAVOPTS file
//      lpNavOptions    -- a pointer to a NAVOPTS structure
//
//  Output:
//      A value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 2/98, SEDWARD
// ========================================================================
/*
BOOL    CNavMigrant95::UpdateNavoptsFile(void)
{
    auto    BOOL            bResult;
    auto    DWORD           dwResult = 0;
    auto    HANDLE          hFile;
    auto    NAVOPTIONS      NavOptions;
    auto    CWString        szTemp(MAX_PATH*2);


    LTrace("BEGIN CNavMigrant95::UpdateNavoptsFile");

    // derive a complete path to the options file
    szTemp = m_szAppInstallDir;
    szTemp = szTemp + "\\NAVOPTS.DAT";

    // open the file
    hFile = CreateFile((LPSTR)szTemp, GENERIC_READ | GENERIC_WRITE
                                                , FILE_SHARE_READ
                                                , NULL
                                                , OPEN_ALWAYS
                                                , FILE_ATTRIBUTE_NORMAL
                                                , NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        {
        LTrace("  Failed to open NAVOPTS.DAT");
        bResult = FALSE;
        goto  Exit_Function;
        }


    memset((void*)&NavOptions, '\0', sizeof(NAVOPTIONS));
    LTrace("  Before MigReadNavBinFromOpenedFile, NavOptions.tsr.uWriteHard = %0x"
                                                        , NavOptions.tsr.uWriteHard);

    // read in the options
    bResult = MigReadNavBinFromOpenedFile(hFile, &NavOptions);
    LTrace("  After MigReadNavBinFromOpenedFile, NavOptions.tsr.uWriteHard = %0x"
                                                        , NavOptions.tsr.uWriteHard);

    // set the "boot" value and write back the new options
    NavOptions.tsr.uWriteHard = 0;  // 0 == allow writing to hard drive boot records
    bResult = MigWriteNavBinFromOpenedFile(hFile, &NavOptions);

    // read in the options
    bResult = MigReadNavBinFromOpenedFile(hFile, &NavOptions);
    LTrace("  After MigWriteNavBinFromOpenedFile, the new value is... NavOptions.tsr.uWriteHard = %0x"
                                                        , NavOptions.tsr.uWriteHard);



Exit_Function:
    LTrace("END CNavMigrant95::UpdateNavoptsFile");
    return (bResult);

}  // end of "UpdateNavoptsFile"
*/
