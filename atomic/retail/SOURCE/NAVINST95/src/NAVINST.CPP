/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/NAVINST.CPv   1.35   26 Oct 1998 12:40:16   mdunn  $
//
//  NavInst.CPP -   Module that holds mostly install functions for the
//                  InstallShield installation of NAVNT; this is the main
//                  point of entry for the custom DLL.
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/NAVINST.CPv  $
// 
//    Rev 1.35   26 Oct 1998 12:40:16   mdunn
// Added GetFileVersionString()
// 
//    Rev 1.34   01 Oct 1998 12:56:00   tcashin
// Put back the check for Atomics navopts.dat version.
// 
//    Rev 1.33   23 Sep 1998 17:39:16   tcashin
// Bumped the navopts version number for Barstow.
// 
//    Rev 1.32   14 Aug 1998 10:40:20   mdunn
// Added SetUpNAVDXTrialInfo()
// 
//    Rev 1.31   30 Jun 1998 12:04:16   tcashin
// Removed all the code for special handling for NAVENG files. They are
// going to be treated as part of the standard def set.
// 
//    Rev 1.30   17 Jun 1998 23:59:44   tcashin
// Added special defutils processing for NAVENG files.
// 
//    Rev 1.29   16 Jun 1998 17:39:40   mdunn
// Added RemoveQConsole().
// 
//    Rev 1.28   10 Jun 1998 18:15:50   jbelden
// Ported code form Roswell project for the shellbrowse function.
// 
//    Rev 1.27   10 Jun 1998 15:52:40   mdunn
// Added CreateInternetShortcut()
// 
//    Rev 1.26   01 Jun 1998 20:39:04   mdunn
// Added ability to clean out Quarantine during uninstall.
// Removed references to Extensions directory and reg entry since that dir
// doesn't even exist any more.
// 
//    Rev 1.25   07 May 1998 20:40:44   mdunn
// Added SetInocDirRegEntry()
// 
//    Rev 1.24   21 Apr 1998 14:39:48   mdunn
// Disabled optimization in IsOEMPersonalityDLL(), as it was crashing in the
// OEM version.
// 
//    Rev 1.23   26 Mar 1998 20:28:02   mdunn
// Added CreateNAVMutex() and ReleaseNAVMutex(), used to prevent more than
// one instance of install from running.
// 
//    Rev 1.22   05 Mar 1998 10:45:56   mdunn
// Added CreateProcessAndWait()
// 
//    Rev 1.21   02 Mar 1998 13:58:34   mdunn
// Added UpdateEastEuropeanAutoexec() to correctly place the NAVDX startup
// scan command in AUTOEXEC on systems that have MODE CON CODEPAGE and
// KEYB commands in AUTOEXEC.
// 
//    Rev 1.20   19 Feb 1998 17:04:48   mdunn
// This time I _really_ ported all the NEC stuff over!
//
//    Rev 1.19   17 Feb 1998 14:53:32   mdunn
// Ported NEC code from the ROSNEC area.
// 
//    Rev 1.18   17 Dec 1997 21:57:44   jtaylor
// Added IsOemPersonalityDll function to check for oem version during install.
//
//    Rev 1.17   19 Aug 1997 21:51:12   jtaylor
// -- Added the registry backup function that Dave B. created.
//
//    Rev 1.16   11 Aug 1997 22:28:10   jtaylor
// -- Added declaration + implementation of StringContainsDBCSChars.
//
//    Rev 1.15   17 Jul 1997 20:56:32   jtaylor
// -- Added a global variable to hold the Symantec Shared directory.
// -- Added a call to call the symevent installer to uninstall symevent.
// -- Added a call to try and delete the prog files\comm files\symtantec shared
//    directory.
// -- Modified all calls and definitions of registry functions that stored the
//    paths for uninstall to include the symantec shared directory.
//
//    Rev 1.14   14 Jul 1997 23:10:04   JALLEE
// Modified RemoveAPUI() to shutdown agent under 95.
// Added RemoveSched() to shutdown the scheduler.
//
//    Rev 1.13   01 Jul 1997 23:53:22   jtaylor
// -- Removed the use of a hardcoded appID and used the Defined one instead.
//
//    Rev 1.12   01 Jul 1997 22:18:52   jtaylor
// -- Added WriteNavStartBinData.
// -- Added SetNavStartInfo.
// -- Upgraded to be unicode enabled.
// -- Reordered the functions.
// -- Changed to the array based method for registering hawking users.
// -- Added RemoveAPUI.
//
//
//    Rev 1.11   12 Jun 1997 12:25:56   jtaylor
// Updated one of the arguments to the hawking predefutils call.
//
//    Rev 1.10   10 Jun 1997 12:58:58   jtaylor
// Removed NT service code
//
//
//    Rev 1.9   08 Jun 1997 20:59:14   jtaylor
// Updated the virus definition usage and removal calls for Hawking.
//
//    Rev 1.8   02 Jun 1997 15:38:26   jtaylor
// Added call to add a usage count for the virus definitions.
//
//    Rev 1.7   02 Jun 1997 01:33:52   jtaylor
// Added Hawking support functions.
//
//    Rev 1.6   01 Jun 1997 19:43:34   jtaylor
// Removed memory only scan.  It will be placed in its own .dll.
//
//    Rev 1.5   01 Jun 1997 16:49:58   jtaylor
// Added pass-through function to CharToOem.
//
//    Rev 1.4   31 May 1997 23:05:06   jtaylor
// Added message box warning about memory resident virus before shutdown.
//
//    Rev 1.3   31 May 1997 18:09:02   jtaylor
// Set memory scan for low memory only.
// Removed the debug message box.
//
//    Rev 1.2   30 May 1997 18:41:56   jtaylor
// Added memory only virus scan to the file.
//
//    Rev 1.1   27 May 1997 21:18:20   JTAYLOR
// Pulled update from navinstnt.
//
//    Rev 1.3   26 May 1997 15:29:52   SEDWARD
// Added support for the Norton Program Scheduler service (npssvc.exe).
//
//    Rev 1.2   17 May 1997 19:11:54   SEDWARD
// Enhanced switch statement in 'GetNavStartInfo'.
//
//    Rev 1.1   11 May 1997 21:31:18   SEDWARD
// Changed SI_MAX_PATH to SYM_MAX_PATH.
//
//    Rev 1.0   05 May 1997 19:24:58   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////



#ifndef SYM_WIN32
#define SYM_WIN32
#endif

#include <windows.h>
#include <windowsx.h>
#include <winnls.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <shlobj.h>

#define INITGUID
#include "objbase.h"

#include "platform.h"
#include "resource.h"
#include "svcctrl.h"
#include "cwstring.h"
#include "navstart.h"
#include "NavInst.h"
#include "DefUtils.h"
#include "IsUtil.h"
#include "n32behav.h"  // for scanner only flag defines
#include "cverrsrc.h"  // for CVersionResourec
#include "cregent.h"

#define CREATE_NAVBIN
#include "navbnhdr.h"
#include "navopts.h"
#include "options.h"


// Ensure that we define and initialize the strings defined in NavAppIDs.h
#define NAVAPPIDS_BUILDING

// Defines an array of NAV AppIds that uninstall can loop through.
#define NAVAPPIDS_WANT_ARRAY
#include "NavAppIds.h"


#include "navapcom.h"
#include "navwcomm.h"

// Internet shortcut stuff
#include "intshcut.h"

// Include qconres.h so we can load strings from QCONRES.DLL
#include "qconres.h"

// local function prototypes
STATUS LOCAL ReadNavstartBinData (LPCTSTR  szNavstart, LPNAVSTARTBINDATA  lpOptions);
BOOL WINAPI ReadNavBinFromOpenedFile ( HANDLE, LPNAVOPTIONS );
BOOL CALLBACK EnumWindowProc ( HWND hwnd, LPARAM lParam );

// DOS trial setup functions
int DosTrialInitialize ( LPCTSTR pszCompany, LPCTSTR pszProduct,
                         UINT nMajorRev, UINT nMinorRev );

int DosTrialSetup ( LPTSTR szNAVDir, DWORD dwTrialDays, DWORD dwExpires );

// Prototypes for functions copied locally out of Symkrnl
static BOOL LHWIsNEC();
void        LNameAppendFile ( LPTSTR szPath, LPCTSTR szFile );
static BYTE LDiskGetBootDrive();

/////////////////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
extern "C"  {
#endif

BOOL            g_RunSchedule;
TCHAR           g_szTarget[SYM_MAX_PATH];
TCHAR           g_szExtensions[SYM_MAX_PATH];
TCHAR           g_szSharedDir[SYM_MAX_PATH];
TCHAR           g_szSymSharedDir[SYM_MAX_PATH];
TCHAR           g_szStringTableString[SYM_MAX_PATH];
HINSTANCE       g_hInst;
HANDLE          g_hMutex;

    // Variables used when searching for QConsole
HINSTANCE       g_hinstQConRes;
HWND            g_hwndQConsole;
#ifdef  __cplusplus
    }
#endif


CDefUtils*      defObjectPtr;                           // for installing virus definition files

static  TCHAR       s_szNewestDefDir[SYM_MAX_PATH];


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

            // SCOTT
            #if 0   // IFDEF_DISABLE
            RegisterCWStrings(NULL, hinstDLL, SYM_MAX_PATH, IDS_STRINGFAIL,
                              IDS_MSG_TITLE);
            #endif  // IFDEF_DISABLE

            break;

        case  DLL_THREAD_ATTACH:
            break;

        case  DLL_THREAD_DETACH:
            break;

        case  DLL_PROCESS_DETACH:
            break;

        default:
            break;
        }

    return  (TRUE);

}  // end of "DllMain"



/////////////////////////////////////////////////////////////////////////
//
// Local Functions
//
/////////////////////////////////////////////////////////////////////////


//************************************************************************
// ReadNavstartBinData()
//
// This function reads the binary options from NAVSTART.DAT
//
// Parameters:
//  LPCTSTR             szNavstart      Full path to NAVSTART.DAT
//  LPNAVSTARTBINDATA   lpOptions       [out] Buffer for binary options.
//
// Returns:
//  NOERR       on success
//  ERR         if invalid NAVSTART.DAT file or file IO error
//************************************************************************
// 04/26/95 DALLEE created.
//************************************************************************

STATUS LOCAL ReadNavstartBinData(LPCTSTR  szNavstart, LPNAVSTARTBINDATA  lpOptions)
{
    auto    HFILE           hFile;
    auto    STATUS          nStatus = ERR;
    auto    NAVSTARTHEADER  rHeader;

    hFile = _lopen(szNavstart, OF_READ);
    if (HFILE_ERROR != hFile)
        {
        auto    BOOL        validHeaderSize;
        auto    int         signatureResult;
        auto    BOOL        validVersion;

        // check for valid header
        validHeaderSize = (sizeof(rHeader) == _lread(hFile, &rHeader, sizeof(rHeader)));
        signatureResult = _tcscmp(NAVSTART_SIGNATURE, rHeader.szSignature);
        validVersion = (NAVSTART_VERSION == rHeader.dwVersion);

        if  ((TRUE == validHeaderSize)
        &&  (0 == signatureResult)
        &&  (TRUE == validVersion))
            {
            // seek to binary data
            _llseek(hFile, rHeader.dwSize, FILE_BEGIN);

            // read in data
            if (sizeof(NAVSTARTBINDATA) == _lread(hFile, lpOptions
                                                    , sizeof(NAVSTARTBINDATA)))
                {
                nStatus = NOERR;
                }
            }
        _lclose(hFile);
        }
    return (nStatus);

} // ReadNavstartBinData()

// ==== WriteNavstartBinData ==============================================
//
//  This function writes the "navstart" structure argument to the navstart.dat
//  file.
//
//  Input:
//      lpszFilename    -- the fully-qualified path to the navstart.dat file
//      lpOptions       -- a pointer to the NAVSTARTBINDATA structure
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

BOOL    WriteNavstartBinData(LPCTSTR  lpszFilename, LPNAVSTARTBINDATA  lpOptions)
{
    auto    BOOL            bResult = TRUE;
    auto    DWORD           dwNumBytes = 0;
    auto    DWORD           dwResult = 0;
    auto    HANDLE          fileHandle = 0;
    auto    NAVSTARTHEADER  rHeader;

    // open the file
    fileHandle = CreateFile(lpszFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL
                                                        , OPEN_EXISTING
                                                        , FILE_ATTRIBUTE_NORMAL
                                                        , NULL);
    if (INVALID_HANDLE_VALUE == fileHandle)
        {
        bResult = FALSE;
        goto  Exit_function;
        }

    // set the file pointer to point to the beginning of the file
    dwResult = SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN);
    if (0xFFFFFFFF == dwResult)
        {
        bResult = FALSE;
        goto  Exit_function;
        }

    // read in the header and perform some validation checks first
    if  ((TRUE == ReadFile(fileHandle, (LPVOID)&rHeader, sizeof(rHeader)
                                                                , &dwNumBytes
                                                                , NULL))
    &&  (sizeof(rHeader) == dwNumBytes)
    &&  (0 == _tcscmp(NAVSTART_SIGNATURE, rHeader.szSignature))
    &&  (NAVSTART_VERSION == rHeader.dwVersion))
        {
        // seek to binary data
        dwResult = SetFilePointer(fileHandle, dwNumBytes, NULL, FILE_BEGIN);
        if (0xFFFFFFFF == dwResult)
            {
            bResult = FALSE;
            goto  Exit_function;
            }

        // write the binary data to the file
        if (FALSE == WriteFile(fileHandle, (LPCVOID)lpOptions, sizeof(NAVSTARTBINDATA)
                                                                , &dwNumBytes
                                                                , NULL))
            {
            bResult = FALSE;
            goto  Exit_function;
            }

        // close the file
        bResult = CloseHandle(fileHandle);
        }


Exit_function:

    return (bResult);

}  // end of "WriteNavstartBinData"


//***************************************************************************
//
// BOOL WINAPI ReadNavBinFromOpenedFile (
//      HFILE        hFile,
//      LPNAVOPTIONS lpNavOptions
//      );
//
// Reads the NAV configuration from an opened configuration file.  The
// current file position should be 0.  If you make any changes for backwards
// compatibility, please make them in this function, or any function called
// from here on...  This is because NAVAP.VXD calls here to read the
// configuration...
//
// Entry:
//      hFile        - handle to an opened configuration file ( usually
//                     NAVOPTS.DAT or NAVDEF.DAT )
//      lpNavOptions - pointer to NAVOPTIONS structure where to place the
//                     configuration.
//
// Exit:
//      TRUE  if function succeeded
//            *lpNavOptions contains configuration
//      FALSE if function failed
//            *lpNavOptions is undefined
//
//***************************************************************************
BOOL WINAPI ReadNavBinFromOpenedFile (
    HANDLE       hFile,
    LPNAVOPTIONS lpNavOptions
    )
{
BOOL         bResult = FALSE;
NAVBINHEADER navBinHeader;
DWORD        dwBytesRead;

                                        // Skip over header

    if ( ReadFile ( hFile, &navBinHeader, sizeof(navBinHeader),
                    &dwBytesRead, NULL ) &&
         sizeof(navBinHeader) == dwBytesRead )
        {
#ifdef BANKERS
        DWORD dwVersion = NAV_VERSION_30_BANKERS;
#else
        DWORD dwVersion = NAV_VERSION_50;
#endif
                                        // Read option settings

        if ( navBinHeader.dwVersion == dwVersion  &&
             ReadFile ( hFile, lpNavOptions, sizeof ( NAVOPTIONS ),
                        &dwBytesRead, NULL )  &&
             sizeof(NAVOPTIONS) == dwBytesRead )
            {
            bResult = TRUE;
            }
        }

    return (bResult);
}


/////////////////////////////////////////////////////////////////////////
//
// Exported Functions
//
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// BOOL BackupRegistry()
//
//  This routine copies SYSTEM.DAT and USER.DAT to SYSTEM.NAV and
//  USER.NAV in the Windows directory.
//
//  Parameters: None
//
//  Returns: TRUE if successful
//           FALSE otherwise
//
///////////////////////////////////////////////////////////////////////////
// 8/19/97 DBUCHES - Function created.
///////////////////////////////////////////////////////////////////////////
extern "C" DllExport    BOOL BackupRegistry(void)
{
    TCHAR szWinDir[MAX_PATH];
    TCHAR szSystemDat[MAX_PATH];
    TCHAR szUserDat[MAX_PATH];
    TCHAR szDest[MAX_PATH];

    // Get windows directory
    if( GetWindowsDirectory( szWinDir, MAX_PATH ) == FALSE)
        return FALSE;

    // Get paths to registry files.
    lstrcpy( szSystemDat, szWinDir );
    lstrcpy( szUserDat, szWinDir );
    lstrcat( szSystemDat, _T("\\SYSTEM.DAT") );
    lstrcat( szUserDat, _T("\\USER.DAT") );

    // Copy SYSTEM.DAT
    lstrcpy( szDest, szWinDir );
    lstrcat( szDest, _T("\\SYSTEM.NAV"));
    if( CopyFile( szSystemDat, szDest, FALSE ) == FALSE )
        return FALSE;

    // Clear attributes of destination file
    SetFileAttributes( szDest, FILE_ATTRIBUTE_NORMAL );

    // Copy USER.DAT
    lstrcpy( szDest, szWinDir );
    lstrcat( szDest, _T("\\USER.NAV"));
    if( CopyFile( szUserDat, szDest, FALSE ) == FALSE )
        return FALSE;

    // Clear attributes of destination file
    SetFileAttributes( szDest, FILE_ATTRIBUTE_NORMAL );

    return TRUE;
}

// ==== CopyVirusDefinitionFiles ==========================================
//
//  This function uses the CDefUtils object to copy the virus definition
//  files to their initial, temporary location.  If all goes well, a value
//  of TRUE is returned; else, a value of FALSE is returned.
//
//  NOTE: you must call "GetDefsDirectory" before this function so that the
//        the CDefUtils object is properly initialized with the path to the
//        definition files.
//
//  Input:  nothing
//
//  Output: TRUE if no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 05/97, SEDWARD
// ========================================================================

extern "C" DllExport    BOOL CopyVirusDefinitionFiles(LPSTR lpTempUpdateDir)
{
    auto    BOOL            bRetValue;
    auto    int             nCurAppId;
    auto    int             nNumAppIds;
    auto    LPCTSTR*        lpszAppIdArray;

    if (NULL == defObjectPtr)
        {
         bRetValue = FALSE;
        }
    else
        {
        defObjectPtr->PostDefUpdate();

        // loop through the app IDs (defined in "NavAppIds.h") and use the
        // newest virus definitions
        lpszAppIdArray = g_lpszNavAppId95Array;
        nNumAppIds = g_ucNavAppId95ArrayNum;
        for (nCurAppId = 0; nCurAppId < nNumAppIds; ++nCurAppId)
            {
            defObjectPtr->InitWindowsApp(lpszAppIdArray[nCurAppId]);
            defObjectPtr->UseNewestDefs();
            }

        delete  defObjectPtr;

        bRetValue = TRUE;
        }

    return (bRetValue);

}  // end of "CopyVirusDefinitionFiles"


// ===== IsFileLoaded =====================================================
//
//  This function uses the "poor man's method" of checking a loaded list
//  for NAVNT (see "siwstat.cpp / CheckForNortonApps()" in the "instnt"
//  project).  It attempts to open the file specified by the argument with
//  exclusive access.
//
//
//  Input:
//      fullFilename    -- the full path to the target file
//
//  Output: the handle of the new child item, NULL otherwise
//
// ========================================================================
//  Function created: 4/97, SEDWARD
// ========================================================================

extern "C" DllExport   BOOL    IsFileLoaded(LPCSTR  fullFilename)
{
    auto    BOOL    fileLoaded = FALSE;
    auto    HANDLE  hFile;

    // try to open the file with exclusive access
    hFile = CreateFile(fullFilename, GENERIC_READ | GENERIC_WRITE
                                            , FILE_SHARE_READ | FILE_SHARE_WRITE
                                            , NULL
                                            , OPEN_EXISTING
                                            , FILE_ATTRIBUTE_NORMAL
                                            , NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        {
        // if sharing violation, assume it's because the DLL is in use
        if (ERROR_SHARING_VIOLATION == GetLastError())
            {
            fileLoaded = TRUE;
            }
        }
    else
        {
        CloseHandle(hFile);
        }

    return (fileLoaded);

}  // end of "IsFileLoaded"


#pragma optimize("", off)

// ==== IsOEMPersonalityDLL ==================================================
//
//  This function links to the specified Personality dll and determines if
//  it has the scanner only attribute set.  If it does it returns TRUE, in all
//  other cases, it returns FALSE.
//
//  Input:
//      pszAppInstallDir    -- the target location of the main application
//      pszDefsDir          -- a pointer to a buffer that is to receive
//                             the path of the temporary location for the
//                             virus definition files (NOTE: it's the
//                             responsibility of the caller to insure that
//                             this buffer is of reasonable size to
//                             accomodate a path specification)
//
//  Output: TRUE if no errors occur and scanner only attribute, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, JTAYLOR
// ========================================================================
extern "C" DllExport   BOOL    IsOEMPersonalityDLL(LPTSTR lpszFullDllName)
{
    // function prototype typedef
    typedef BOOL (WINAPI *FunctionPointer) (DWORD dwBehavior, DWORD dwExtra);
    FunctionPointer    fpInitNt = NULL;
    auto    HINSTANCE       hDll;
    auto    BOOL            veryTemp = FALSE;

    hDll = LoadLibrary(lpszFullDllName);
    if (hDll)
        {
		fpInitNt = (FunctionPointer)GetProcAddress(hDll, "?GetBehavior@@YAHKK@Z");  // function name
		if (fpInitNt)
			{
            veryTemp = fpInitNt(BEHAVIOR_SCAN_ONLY,0);
			}

        FreeLibrary(hDll);
        }

    return veryTemp;
}

#pragma optimize("", on)


// ==== GetDefsDirectory ==================================================
//
//  This function uses the CDefUtils object to copy the virus definition
//  files to their initial, temporary location.  If all goes well, a value
//  of TRUE is returned; else, a value of FALSE is returned.
//
//  Input:
//      pszAppInstallDir    -- the target location of the main application
//      pszDefsDir          -- a pointer to a buffer that is to receive
//                             the path of the temporary location for the
//                             virus definition files (NOTE: it's the
//                             responsibility of the caller to insure that
//                             this buffer is of reasonable size to
//                             accomodate a path specification)
//
//  Output: TRUE if no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 05/97, SEDWARD
// ========================================================================

extern "C" DllExport    BOOL GetDefsDirectory(LPTSTR  pszAppInstallDir
                                                        , LPTSTR  pszDefsDir)
{
    auto    BOOL            bRetValue;
    auto    TCHAR           szBuf[SYM_MAX_PATH];

    // create and initialize a def utils object (if successfully created, this
    // object will be destroyed in the "CopyVirusDefinitionFiles" function
    defObjectPtr = new  CDefUtils;
    if (NULL == defObjectPtr)
        {
         bRetValue = FALSE;
         goto  Exit_Function;
        }

    defObjectPtr->InitInstallApp(g_szNavAppIdNavw95, pszAppInstallDir);

    // fetch the path of the temporary virus defs directory
    *szBuf = NULL;
    defObjectPtr->PreDefUpdate(szBuf, sizeof(szBuf), CDefUtils::Install);
    if (NULL != *szBuf)
        {
        bRetValue = TRUE;
        _tcscpy(pszDefsDir, szBuf);
        }
    else
        {
        bRetValue = FALSE;
        }


Exit_Function:

    return (bRetValue);

}  // end of "GetDefsDirectory"


//////////////////////////////////////////////////////////////////////////
//
// Function:    GetFileVersionString
//
// Description:
//  Reads a file's version string from its version resource.
//
// Input:
//  szPath: [in] Full path to the file.
//  szVersion: [out] Pointer to a buffer that receives the version string.
//
// Returns:
//  TRUE if successful, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
//  7/17/98  Mike Dunn -- function created
// 10/26/98  Mike Dunn -- copied here from NAVINSTNT.
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL GetFileVersionString ( LPCTSTR szPath, LPTSTR szVersion )
{
BOOL             bRet = FALSE;
CVersionResource ver ( szPath );
DWORD            dwHighDword, dwLowDword;

    if ( 0 == ver.InitializeData() &&
         0 == ver.GetBinaryFileVer ( &dwHighDword, &dwLowDword ) )
        {
        wsprintf ( szVersion, _T("%u.%u.%u.%u"),
                   HIWORD(dwHighDword), LOWORD(dwHighDword),
                   HIWORD(dwLowDword), LOWORD(dwLowDword) );

        bRet = TRUE;
        }

    return bRet;
}


// ==== GetNavStartInfo ===================================================
//
//  This function reads the "navstart.dat" file to get some information
//  specified by the integer argument.
//
//  Input:
//      filename    -- the fully-qualified path to the "navstart.dat" file
//      targetInfo  -- specifies the information to target (ie, load navap
//                     at startup)
//
//  Output:
//      A value of TRUE or FALSE if the data is successfully read, a value
//      of negative one (-1) if not
//
// ========================================================================
//  Function created: 4/97, SEDWARD
// ========================================================================

extern "C" DllExport   int  GetNavStartInfo(LPCTSTR  filename, int  targetInfo)
{
    auto    int                 nResult;
    auto    NAVSTARTBINDATA     rNavstart;

    if (NOERR == ReadNavstartBinData(filename, &rNavstart))
        {
        switch (targetInfo)
            {
            case  NAVSTART_OPTION_SCAN_DOS_FILES:
                nResult = rNavstart.startup.bScanDOSFiles;
                break;

            case  NAVSTART_OPTION_SCAN_WINDOWS_FILES:
                nResult = rNavstart.startup.bScanWindowsFiles;
                break;

            case  NAVSTART_OPTION_LOAD_TSR:
                nResult = rNavstart.startup.bLoadTSR;
                break;

            case  NAVSTART_OPTION_LOAD_AP_AT_STARTUP:
                nResult = rNavstart.startup.bLoadVxD;
                break;

            case  NAVSTART_OPTION_PROTECT_FEATURE:
                nResult = rNavstart.startup.bProtectFeature;
                break;

            default:
                nResult = -1;
                break;
            }
        }
    else // error reading file
        {
        nResult = -1;
        }

    return (nResult);

}  // end of "GetNavStartInfo"



//************************************************************************
// MyCharToOem()
//
// This function calls the OS CharToOem function.
//
// Parameters:
//  LPCSTR szSource    A string
//  LPSTR  szDest      [Out]Buffer in which to place OEM version of the string.
//
// Returns:
//      TRUE;
//************************************************************************
// 06/01/97 JTAYLOR created.
//************************************************************************

extern "C" DllExport   BOOL  MyCharToOem(LPCSTR  szSource, LPSTR szDest )
{
    return CharToOem( szSource, szDest );
}

// ==== RemoveAPUI ========================================================
//
//  This function attempts to close the AP window, if it is running.
//
//  Input:  nothing
//  Output: a value of TRUE if the window was found, FALSE if it was not
//
// ========================================================================
//  Function created: 06/97, SEDWARD
//  Updated            7/97, jallee
// ========================================================================

extern "C"  DllExport   BOOL    RemoveAPUI(void)
{
    auto    HWND            hNavAp;
    auto    BOOL            bReturnValue = FALSE;
    auto    UINT            uNavApWMessage = 0L;


	hNavAp = FindWindow(NAVAP_95_WND_CLASS, NULL);
    if (hNavAp)
        // Found the window, send a message to close it.
        {
        // Get the custom message that NAVW uses to talk to the agent.
        uNavApWMessage = RegisterWindowMessage (NAVW_NAVAPW_COMM);

        // Send unload.
        if ( uNavApWMessage != NULL )
            {
            PostMessage ( hNavAp,
                          uNavApWMessage,
                          NAVW_CMD_UNLOADNAVAPW,
                          0L);

            bReturnValue = TRUE;
            }
        }
    else
        {
        bReturnValue = FALSE;
        }

    return (bReturnValue);

}  // end of "RemoveAPUI"

// ==== RemoveSched ========================================================
//
//  This function attempts to close the Scheduler window, if it is running.
//
//  Input:  nothing
//  Output: a value of TRUE if the window was found, FALSE if it was not
//
// ========================================================================
//  Function stolen: 07/97, jallee
// ========================================================================

extern "C"  DllExport   BOOL    RemoveSched(void)
{
    auto    HWND            hSched;

    hSched = FindWindow( "npsWClass", NULL );
    if ( hSched )
        // Found the window, send a message to close it
        {
        // Scheduler will shutdown properly on WM_DESTROY.
        PostMessage ( hSched,
                WM_DESTROY,
                (WPARAM) 0,
                (LPARAM) 0);

        return ( TRUE );
        }
    else
        {
        return ( FALSE );
        }
}  // End of "RemoveSched"


//////////////////////////////////////////////////////////////////////////
//
// Function:    RemoveQConsole()
//
// Description:
//  Searches for QConsole and closes it if found.
//
// Input:
//  Nothing.
//
// Returns:
//  1 always.
//
//////////////////////////////////////////////////////////////////////////
//  6/15/98  Mike Dunn -- function created.
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL RemoveQConsole()
{
HANDLE         hMutex = NULL;
TCHAR          szQResDLLPath [MAX_PATH];
CRegistryEntry reg ( HKEY_LOCAL_MACHINE, SYM_INSTALLEDAPPS_KEY, _T("NAV95") );
DWORD          dwType;
DWORD          dwBuffSize = sizeof(szQResDLLPath);


    g_hwndQConsole = NULL;

                                        // The mutex name *MUST* match the
                                        // one in QCONSOLE/qconsole.cpp.

    hMutex = CreateMutex ( NULL, FALSE, _T("Quarantine Console Mutex") );

    if ( NULL != hMutex  &&  ERROR_ALREADY_EXISTS == GetLastError() )
        {
                                        // QConsole is open!  Load the QConsole
                                        // resource DLL to get the window
                                        // title string.
        if ( ERROR_SUCCESS ==
                 reg.GetValue ( &dwType, (LPBYTE) szQResDLLPath, &dwBuffSize ) )
            {
            LNameAppendFile ( szQResDLLPath, _T("qconres.dll") );

            g_hinstQConRes = LoadLibrary ( szQResDLLPath );

            if ( NULL == g_hinstQConRes )
                {
                goto bailout;           // Fail silently if we can't load
                                        // the DLL.
                }

            if ( LoadString ( g_hinstQConRes, IDS_MAINFRM_TITLE,
                              g_szStringTableString,
                              sizeof(g_szStringTableString ) ) )
                {
                EnumWindows ( (WNDENUMPROC) EnumWindowProc, 0 );

                if ( NULL != g_hwndQConsole )
                    {
                                        //&^ Should this be PostMessage??
                    SendMessage ( g_hwndQConsole, WM_CLOSE, 0, 0 );
                    }
                }

            FreeLibrary ( g_hinstQConRes );
            }
        }

bailout:
    if ( NULL != hMutex )
        CloseHandle ( hMutex );

    return 1;
}


BOOL CALLBACK EnumWindowProc ( HWND hwnd, LPARAM lParam )
{
TCHAR szWindowText[256];

    GetWindowText ( hwnd, szWindowText, 256 );

    if ( 0 == _tcsnicmp ( szWindowText, g_szStringTableString,
                          _tcslen ( g_szStringTableString ) ) )
        {
        g_hwndQConsole = hwnd;
        return FALSE;
        }

    return TRUE;
}


// ==== SetNavStartInfo ===================================================
//
//  This function writes a boolean value to the "navstart.dat" file; the
//  field to set is specified by the integer argument.
//
//  Input:
//      filename    -- the fully-qualified path to the "navstart.dat" file
//      targetInfo  -- specifies the information to target (ie, load navap
//                     at startup)
//      bValue      -- the value to write (ie, TRUE or FALSE)
//
//  Output:
//      A value of TRUE or FALSE if the data is successfully written, a value
//      of negative one (-1) if not
//
// ========================================================================
//  Function created: 4/97, SEDWARD
// ========================================================================

extern "C" DllExport    BOOL    SetNavStartInfo(LPCTSTR  filename, int  targetInfo
                                                                    , BOOL  bValue)
{
    auto    int                 nResult = TRUE;
    auto    NAVSTARTBINDATA     rNavstart;

    // read in the data and set the target structure member
    if (NOERR == ReadNavstartBinData(filename, &rNavstart))
        {
        switch (targetInfo)
            {
            case  NAVSTART_OPTION_SCAN_DOS_FILES:
                rNavstart.startup.bScanDOSFiles = bValue;
                break;

            case  NAVSTART_OPTION_SCAN_WINDOWS_FILES:
                rNavstart.startup.bScanWindowsFiles = bValue;
                break;

            case  NAVSTART_OPTION_LOAD_TSR:
                rNavstart.startup.bLoadTSR = bValue;
                break;

            case  NAVSTART_OPTION_LOAD_AP_AT_STARTUP:
                rNavstart.startup.bLoadVxD = bValue;
                break;

            case  NAVSTART_OPTION_PROTECT_FEATURE:
                rNavstart.startup.bProtectFeature = bValue;
                break;

            default:
                nResult = -1;
                break;
            }
        }
    else // error reading file
        {
        nResult = -1;
        }

    // now write the structure back to the file
    if (-1 != nResult)
        {
        nResult = WriteNavstartBinData(filename, &rNavstart);
        }

    return (nResult);

}  // end of "SetNavStartInfo"


//////////////////////////////////////////////////////////////////////////
//
// Function:    SetUpNAVDXTrialInfo()
//
// Description:
//  Writes the DOS trial info file to the NAV directory.
//
// Input:
//  szNAVDir: [in] The NAV install dir (the trial info file is put here)
//  nTrialLength: [in] # of days in the trial period.
//
// Returns:
//  TRUE if the info was written, FALSE if not.  On failure, the info file
//  is deleted so that NAVDX will recreate it (albeit always with a 30-day
//  trial period) the next time it is run.
//
//////////////////////////////////////////////////////////////////////////
//  8/13/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL SetUpNAVDXTrialInfo ( LPTSTR szNAVDir, 
                                                int nTrialLength )
{
BOOL     bRetVal = FALSE;
CWString cstrDatFile;

    if ( DosTrialInitialize ( _T("Symantec"), _T("Norton AntiVirus"),
                              2, 0 ) )
        {
        if ( DosTrialSetup ( szNAVDir, nTrialLength, 0 ) )
            {
            bRetVal = TRUE;
            }
        }


    if ( !bRetVal )
        {
        cstrDatFile = g_szTarget;
        cstrDatFile.AppendFile ( _T("symtdat.dat") );
        DeleteFile ( cstrDatFile );
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////////
// StringContainsDBCSChars
//
// Description: Check a string to see if has any DBCS characters
//
// Parameters: LPTSTR - The string to check
//
// Return Value: TRUE if any DBCS characters are in the string
//
// Author: GWESTER
// Date:   10/27/96
//
/////////////////////////////////////////////////////////////////////////
extern "C" DllExport    BOOL StringContainsDBCSChars ( LPTSTR lpszString )
{
    int                 nLen;
    LPTSTR              lpszCurrent;

    nLen = STRLEN ( lpszString );

    if (nLen)
    {
        for (lpszCurrent = lpszString; *lpszCurrent != '\0';
             lpszCurrent = AnsiNext ( lpszCurrent ) )
        {
            if (IsDBCSLeadByte ( *lpszCurrent ) ) return TRUE;
        }
    }

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    MachineIsNEC()
//
// Description:
//  A cover for the Quake function HWIsNEC().
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if the system is a NEC, or false if not.
//
//////////////////////////////////////////////////////////////////////////
// 9/15/97 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL MachineIsNEC()
{
    return LHWIsNEC();                  // now calling local copy of HWIsNEC()
}

//***************************************************************************
// GetBootDriveLetter()
// 
// Description:
//     Return the drive letter that this system booted from. Generally, this
//     is 'C' on normal computers, but on NEC PC-98xx machines it could be
//     anything.
//
// Return Value: Drive Letter
// 
//***************************************************************************
// 01/10/1997 JBRENNA Function Created.
// 01/23/1997 JBRENNA Changed to find boot drive rather than first hard drive.
// 9/17/97 Mike Dunn -- ported from Kirin NEC
//***************************************************************************

BYTE GetBootDriveLetter (void)
{
    static BYTE DriveLetter = 0;
    
                                        // Get the boot drive ltr and cache it.
    if (DriveLetter == 0)
    {
        DriveLetter = LDiskGetBootDrive();
    
        if (DriveLetter == 0)
            DriveLetter = 'C';
    }
    return DriveLetter;
}


//***************************************************************************
// UpdateBootDriveLetter()
// 
// Description:
//     Updates the drive letter of the specified string to the first hard
//     drive. Generally, this is 'C' on normal computers, but on NEC PC-98xx
//     machines it could be anything.
//
//     Note this only updates the string if the first character is a letter
//     and the second character is a ':'.
// 
// Parameters:
//     LPSTR lpPath -- path to update.
// 
// Return Value: NONE
// 
//***************************************************************************
// 01/10/1997 JBRENNA Function Created.                                   
// 9/17/97  Mike Dunn -- ported from Kirin NEC
//***************************************************************************

void UpdateBootDriveLetter (LPSTR lpPath)
{
    const  char DriveLtr = lpPath[0];

                                        // We require at least 2 characters.
    if (lpPath[0] == EOS || lpPath[1] == EOS)
        return;

    if ((DriveLtr >= 'a' && DriveLtr <= 'z' && lpPath[1] == ':') ||
        (DriveLtr >= 'A' && DriveLtr <= 'Z' && lpPath[1] == ':'))
    {
        lpPath[0] = GetBootDriveLetter();
    }
}


//****************************************************************************
//  LineIsRemark()
//
//  Description:
//      Determines if the specified string begins with "REM" or a 
//      semicolon. All leading "@" characters are ignored.
//
//  Parameters:
//      lpLine       [in]   Line to look at.
//
//  Returns:
//      TRUE   - when REM leads the string.
//      FALSE  - when REM does not lead the string.
//****************************************************************************
// 01/20/1997 JBRENNA Function Created.                                   
// 5/1/97 Mike Dunn -- Now looks for a leading semicolon as well. Also rewrote
//                     to be just one block of code, not a loop.
// 9/17/97 Mike Dunn -- ported from Kirin NEC
//****************************************************************************

static BOOL LineIsRemark (LPSTR lpLine)
{
    LPCTSTR     szRemString         = _T("REM");
    const WORD  szRemStringLen      = STRLEN (szRemString);
    BOOL        bIsRem              = FALSE;
    LPTSTR      pFirstNonspaceChar;

    // Find the first non-whitespace character in the line.

    pFirstNonspaceChar = _strspnp ( lpLine, _T(" \t\n") );

    if ( NULL != pFirstNonspaceChar )
        {
        // A non-whitespace character was found.

        // Skip leading '@' characters.

        while (*lpLine == '@')
            {
            lpLine++;                       // This is cool for DBCS since 
            }                               // '@' is always a SB char.

        // At this point, lpLine points at the first significant character.
        // Compare it against the remark markers.

        if ( *lpLine == ';' )
            {
            bIsRem = TRUE;
            }
        else if ( 0 == _tcsnicmp ( lpLine, szRemString, szRemStringLen ) )
            {
            bIsRem = TRUE;
            }
        }   // end if

    return bIsRem;

    // Note that if the whole line is whitespace, this fn returns FALSE.
    // I see no harm in that, since that's a degenerate case and the
    // other fns in this file that search the line will find nothing
    // anyway.
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    FixUpNECConfigSys()
//
// Description:
//  Searches CONFIG.SYS for an EMM386.EXE line, and remarks it out if
//  it exists.
//
// Input:
//  szRemark: The text to prepend to the EMM386 line to remark it out.
//            This must start with either "rem" or ";" in order to really
//            be a remark!  A space is added after szRemark and before
//            the start of the original line.
//
// Returns:
//  TRUE if everything went OK, or FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 5/1/97  Mike Dunn -- function created
// 9/15/97 Mike Dunn -- ported from Kirin NEC
// 10/13/97 Mike Dunn -- changed to remark out EMM386.EXE if it's there,
//                       and ignore HIMEM entirely (since Windows 95 loads
//                       it on it's own if it isn't in CONFIG.SYS).  This
//                       works around bugs in the DOS extender.
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL FixUpNECConfigSys ( LPCTSTR szRemark )
{
const WORD wMaxLines = 128;
TCHAR      szConfigSys[] = _T("a:\\config.sys");
TCHAR      szLines  [wMaxLines][SYM_MAX_PATH];
FILE*      pFile;
WORD       wLineCnt;
WORD       wNumLines;
LPTSTR     lpTmp;
LPTSTR     lpKeyWordMatch;
BOOL       bNeedToChangeFile = FALSE;

    // Change the drive letter in "a:\config.sys" if necessary.
    UpdateBootDriveLetter ( szConfigSys );

    // Read the CONFIG.SYS file into our buffer.

    if ( NULL == ( pFile = fopen ( szConfigSys, "r" ) ) )
        {
        // There was no config.sys on the system, so we don't have to
        // do anything.

        return TRUE;
        }

    // Read in the file byte by bitty byte.

    for ( wLineCnt = 0;
          !feof (pFile) && wLineCnt < wMaxLines; 
          ++wLineCnt)
        {
        int    ch         = ' ';
        WORD   wCurCh     = 0;
        LPTSTR lpCurLine  = szLines[wLineCnt];

        while ( !feof (pFile) && 
                ch != '\n'    &&
                ch != EOF     &&
                wCurCh < SI_MAXSTR )
            {
            ch = fgetc (pFile);

            if (ch != '\n')
                {
                *lpCurLine = ch;
                }
            
            ++lpCurLine;
            ++wCurCh;
            }

        if (ch == EOF)
            {
            --lpCurLine;
            *lpCurLine = EOS;
            }

        wNumLines = wLineCnt + 1;
        }   // end for

    fclose (pFile);

    // Walk through the file and rem out any EMM386 lines.

    for (wLineCnt = 0; wLineCnt < wNumLines; ++wLineCnt)
        {
        const WORD wMaxTmpLine      = 1024;
        TCHAR      szTmpLine[wMaxTmpLine];

        // Skip all commented out lines.
        
        if (LineIsRemark (szLines[wLineCnt]))
            continue;

        lstrcpy (szTmpLine, szLines[wLineCnt]);

        // conver the line to upper case
        for (lpTmp = szTmpLine; lpTmp && *lpTmp; ++lpTmp)
            {
            if (IsDBCSLeadByte (*lpTmp))
                ++lpTmp;
            else
                *lpTmp = toupper (*lpTmp);
            }

        // Search for "EMM386.EXE" in the line.
        lpKeyWordMatch = _tcsstr ( szTmpLine, _T("EMM386.EXE") );

        // If there's a match, prepend the remark string that was 
        // passed in to the fn.

        if ( lpKeyWordMatch )
            {
            lstrcpy ( szLines[wLineCnt], szRemark );
            lstrcat ( szLines[wLineCnt], _T(" ") );
            lstrcat ( szLines[wLineCnt], szTmpLine );

            bNeedToChangeFile = TRUE;
            }
        }   // end for


    // If we don't have to change CONFIG.SYS as it currently stands,
    // then don't mess!

    if ( bNeedToChangeFile )
        {
        // Now, write the lines back out to the file.

        if ( NULL == ( pFile = fopen ( szConfigSys, "w" ) ) )
            {
            return FALSE;
            }


        for (wLineCnt = 0; wLineCnt < wNumLines; ++wLineCnt)
            {
            fprintf (pFile, "%s\n", szLines[wLineCnt]);
            }

        fclose (pFile);
        }

    return TRUE;                                            
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    UpdateEastEuropeanAutoexec
//
// Description:
//  Adds the passed-in NAVDX command line to the system's AUTOEXEC.BAT,
//  but places the command after the first KEYB command.  East European
//  systems have two MODE CON CODEPAGE commands and a KEYB command in
//  AUTOEXEC, and NAVDX must go after those commands in order to have
//  the screen display look right.
//
// Input:
//  szAutoexecPath: [in] Fully-qualified path to the system's AUTOEXEC.
//  szCommand: [in] The NAVDX command line to write to the file.
//
// Returns:
//  TRUE if the line was added OK, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 3/2/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL UpdateEastEuropeanAutoexec
    ( LPCTSTR szAutoexecPath,
      LPCTSTR szCommand )
{
const WORD wMaxLines = 128;
TCHAR      szLines  [wMaxLines][SYM_MAX_PATH];
FILE*      pFile;
WORD       wLineCnt;
WORD       wNumLines;
int        nInsertAfterLine = -1;
LPTSTR     lpTmp;

    // Read the AUTOEXEC.BAT file into our buffer.

    pFile = fopen ( szAutoexecPath, "r" );

    if ( NULL == pFile )
        {
                                        // We couldn't open AUTOEXEC. First
                                        // try to create a new file.
        pFile = fopen ( szAutoexecPath, "w" );

        if ( NULL == pFile )
            {
                                        // We couldn't create a new file,
                                        // so there's nothing more we can
                                        // do here.
            return FALSE;
            }

                                        // Otherwise, write the NAVDX
                                        // command to the file.
        fprintf ( pFile, "%s\n", szCommand );

        fclose ( pFile );

        return TRUE;
        }
    else
        {
        // Read in the file byte by bitty byte.

        for ( wLineCnt = 0;
              !feof (pFile) && wLineCnt < wMaxLines; 
              ++wLineCnt)
            {
            int    ch         = ' ';
            WORD   wCurCh     = 0;
            LPTSTR lpCurLine  = szLines[wLineCnt];

            while ( !feof (pFile) && 
                    ch != '\n'    &&
                    ch != EOF     &&
                    wCurCh < SI_MAXSTR )
                {
                ch = fgetc (pFile);

                if (ch != '\n')
                    {
                    *lpCurLine = ch;
                    }
        
                ++lpCurLine;
                ++wCurCh;
                }

            if (ch == EOF)
                {
                --lpCurLine;
                *lpCurLine = EOS;
                }

            wNumLines = wLineCnt + 1;
            }   // end for

        fclose (pFile);

        // Walk through the file and find the first instance of "keyb " or
        // "keyb."

        for (wLineCnt = 0; wLineCnt < wNumLines; ++wLineCnt)
            {
            const WORD wMaxTmpLine      = 1024;
            TCHAR      szTmpLine[wMaxTmpLine];

            // Skip all commented out lines.
        
            if (LineIsRemark (szLines[wLineCnt]))
                continue;

            lstrcpy (szTmpLine, szLines[wLineCnt]);

            // conver the line to upper case
            for (lpTmp = szTmpLine; lpTmp && *lpTmp; ++lpTmp)
                {
                if (IsDBCSLeadByte (*lpTmp))
                    ++lpTmp;
                else
                    *lpTmp = toupper (*lpTmp);
                }

            // Search for "KEYB." and "KEYB " in the line.  If there's a match,
            // note the line number.

            if (  _tcsstr ( szTmpLine, _T("KEYB ") ) ||
                _tcsstr ( szTmpLine, _T("KEYB.") ) )
                {
                nInsertAfterLine = wLineCnt;
                }
            }   // end for


        // Now, write the lines back out to the file.

        if ( NULL == ( pFile = fopen ( szAutoexecPath, "w" ) ) )
            {
            return FALSE;
            }


        // If nInsertAfterLine == -1 here, then we never found a KEYB
        // line, so go back to the default behavior of putting NAVDX
        // at the beginning of the file.

        if ( -1 == nInsertAfterLine )
            {
            fprintf ( pFile, "%s\n", szCommand );
            }

        for (wLineCnt = 0; wLineCnt < wNumLines; ++wLineCnt)
            {
            fprintf (pFile, "%s\n", szLines[wLineCnt]);

            if ( wLineCnt == nInsertAfterLine )
                {
                fprintf ( pFile, "%s\n", szCommand );
                }
            }

        fclose (pFile);
        }   // end else

    return TRUE;                                            
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CreateProcessAndWait
//
// Description:
//  Launches an app and waits for it to terminate.  Similar to the
//  InstallShield LaunchAppAndWait(), but this fn reads the app's
//  return value.
//
// Input:
//  szCmd: [in]  The command to execute.
//  pnExitCode: [out] Pointer to an int that receives the process's
//                    exit code.
//
// Returns:
//  TRUE if the command was executed, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 3/5/98  Mike Dunn -- function ported from..... somwhere.
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL CreateProcessAndWait (
                             LPCTSTR szCmd,
                             LPINT   pnExitCode )
{
PROCESS_INFORMATION	ProcessInfo;
STARTUPINFO			StartUpInfo;

	ZeroMemory(&StartUpInfo,sizeof(STARTUPINFO));
	StartUpInfo.cb = sizeof(STARTUPINFO); 			// size of structure

	BOOL bSuccess = CreateProcess (
                        NULL, (LPSTR)szCmd, NULL, NULL, FALSE,
                        HIGH_PRIORITY_CLASS, NULL, NULL, &StartUpInfo,
                        &ProcessInfo );

	if (bSuccess)
	{
		while (WaitForSingleObject(ProcessInfo.hProcess, 50) == WAIT_TIMEOUT)
		{
			MSG		msg;
		    while (PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
			{
				TranslateMessage(&msg);
			    DispatchMessage(&msg);
			}
		}
		DWORD dwExitCode;
		if (GetExitCodeProcess(ProcessInfo.hProcess, &dwExitCode))
			*pnExitCode = (int)dwExitCode;
		else
			bSuccess = FALSE;
		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
	}
	return bSuccess;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CreateNAVMutex
//
// Description:
//  Creates a named mutex, used to keep more than one instance of
//  install running at once.
//
// Input:
//  szMutexName: [in] Name to use for the mutex.
//
// Returns:
//  TRUE if the installer may continue, or FALSE if the mutex couldn't
//  be created or if it already existed, in which case the installer should
//  exit.
//
//////////////////////////////////////////////////////////////////////////
// 3/26/98  Mike Dunn -- Function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL CreateNAVMutex ( LPCTSTR szMutexName )
{
HANDLE hMutex;

    hMutex = CreateMutex ( NULL, TRUE, szMutexName );

                                        // Succeed only if CreateMutex()
                                        // returned a valid handle AND the
                                        // mutex didn't already exist.
    if ( NULL != hMutex  &&
         ERROR_ALREADY_EXISTS != GetLastError() )
        {
        g_hMutex = hMutex;
        return TRUE;
        }
    else
        {
        g_hMutex = NULL;
        return FALSE;
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    ReleaseNAVMutex()
//
// Description:
//  Releases a mutex created by CreateNAVMutex().
//
// Input:
//  Nothing.
//
// Returns:
//  Always 1.
//
//////////////////////////////////////////////////////////////////////////
// 3/26/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL ReleaseNAVMutex()
{
    if ( NULL != g_hMutex )
        {
        ReleaseMutex ( g_hMutex );
        g_hMutex = NULL;
        }

    return 1;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    SetInocDirRegEntry()
//
// Description:
//  Writes the current inoculation path to the registry.  The entry goes
//  under HLKM\SW\Sym\NAV, string value "InocDir".
//
// Input:
//  szNAVDir: [in] The NAV install dir.
//
// Returns:
//  TRUE if successful, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 5/7/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL SetInocDirRegEntry ( LPCTSTR szNAVDir )
{
CRegistryEntry reg ( HKEY_LOCAL_MACHINE, NAV_KEY, _T("InocDir") );
TCHAR          szOptsFile [SYM_MAX_PATH];
TCHAR          szInocPath [SYM_MAX_PATH];
HANDLE         hFile;
NAVOPTIONS     rOptions;
LONG           lRet;

    lstrcpy ( szOptsFile, szNAVDir );
    LNameAppendFile ( szOptsFile, _T("navopts.dat") );

    hFile = CreateFile ( szOptsFile, GENERIC_READ, FILE_SHARE_READ,
                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                         NULL );

    if ( INVALID_HANDLE_VALUE != hFile &&
         ReadNavBinFromOpenedFile ( hFile, &rOptions ) )
        {
        OemToChar ( rOptions.inoc.szNetInocPath, szInocPath );
        CloseHandle ( hFile );
        }
    else
        {
                                        // If the navopts file couldn't be
                                        // read, use our default path.
        lstrcpy ( szInocPath, _T("\\NCDTREE") );
        }


    if ( INVALID_HANDLE_VALUE != hFile )
        {
        CloseHandle ( hFile );
        }


    lRet = reg.SetValue ( REG_SZ, (LPBYTE) szInocPath,
                          sizeof(TCHAR) * (1 + lstrlen(szInocPath)) );

    return  ERROR_SUCCESS == lRet ? TRUE : FALSE;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CreateInternetShortcut()
//
// Description:
//  Creates a URL file if IE3+ is installed (URL files are an IE
//  invention).
//
// Input:
//  szPath: The fully-qualified path specifying the filename to give the
//          URL file.  This should be constructed with IS predefined
//          variables, such as FOLDER_PROGRAMS.
//  szURL:  The absolute URL.
//
// Returns:
//  TRUE if the URL file was created, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 6/10/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL CreateInternetShortcut ( LPCTSTR szPath,
                                                   LPCTSTR szURL )
{
BOOL                     bRetVal = FALSE;
HRESULT                  hr;
IUniformResourceLocator* pIURL = NULL;
IPersistFile*            pIFile = NULL;
WCHAR                    wszPath [ MAX_PATH ];

    if ( FAILED(CoInitialize(NULL)) )
        return FALSE;

                                        // Create an IURL interface.

    hr = CoCreateInstance ( CLSID_InternetShortcut, NULL,
                            CLSCTX_INPROC_SERVER,
                            IID_IUniformResourceLocator,
                            (void**) &pIURL );

    if ( FAILED(hr) )
        goto bailout;

    hr = pIURL->SetURL ( szURL, IURL_SETURL_FL_GUESS_PROTOCOL );

    if ( FAILED(hr) )
        goto bailout;

                                        // Get an IPersistFile interface so
                                        // we can save the URL as a file.

    hr = pIURL->QueryInterface ( IID_IPersistFile, (void**) &pIFile );

    if ( FAILED(hr) )
        goto bailout;

                                        // IPersistFile::Save() requires
                                        // the filespec to be in Unicode.

    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, szPath, -1,
                          wszPath, MAX_PATH );

    hr = pIFile->Save ( wszPath, TRUE );

    if ( SUCCEEDED(hr) )
        bRetVal = TRUE;

bailout:
    if ( pIFile )
        pIFile->Release();

    if ( pIURL )
        pIURL->Release();

    return bRetVal;
}


// ****************************************************************
//
// Following are functions pulled locally from Symkrnl so that
// NAVINST95.DLL doesn't have to link to S32KRNLK.DLL.  The function
// names start with 'L' for "local".
//
// ****************************************************************

BOOL LHWIsNEC()
{
#ifdef _M_ALPHA // WES No (known) NEC Alpha machines
	return FALSE;
#else	
    static      BOOL bIsNEC = FALSE;
    static      BOOL bTested = FALSE;

    if (!bTested)
        {
        UINT    uType;


        bTested = TRUE;
                                        // Get main KB type
        uType = GetKeyboardType(0);
        if (uType == 7)                 // '7' is japanese keyboard type
            {
                                        // Get the sub-type.
            uType = GetKeyboardType(1);
            if (uType >= 0x0D01)
                bIsNEC = TRUE;
            }

        }
    return(bIsNEC);
#endif    
}

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

BYTE LDiskGetBootDrive()
{
   HKEY          RegKey;
   LONG          lRet           = 0;
   DWORD         dwSize         = 0;
   DWORD         dwValueType    = 0;
   unsigned long nDirNameLength = 0;
   TCHAR         szDir[SYM_MAX_PATH];
                                        // In NT 4 and Win95, the registry will work.
                                        // In NT 3.5x, the SystemRoot environment variable
                                        // will be used.
                                        // If all else fails, we'll try thunks (but I doubt
                                        // that code path ever gets taken).
   memset( szDir, 0x00, sizeof( szDir ));
   nDirNameLength = sizeof( szDir );

                                        // For NT 4....
   lRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                        _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"),
                        0,
                        KEY_READ,
                        &RegKey);

   if (lRet == ERROR_SUCCESS)
       {
       lRet = RegQueryValueEx (RegKey,
                               "BootDir",
                               NULL,
                               &dwValueType,
                               (LPBYTE)szDir,
                               &nDirNameLength);

       RegCloseKey( RegKey );

                                        //&? BEM - Until we convert this function to return a TCHAR, we'll hack it to
                                        //&? return a byte.  This WILL break under Unicode!!
       return(((LPCSTR)szDir)[0] );
       }
   else if ( GetVersion() < 0x8000000 ) // we're on NT
       {                                // For NT 3.5x, use the SystemRoot environment varialbe
       TCHAR szSystemRoot[SYM_MAX_PATH];
       DWORD dwSize;

       ZeroMemory(szSystemRoot, sizeof(szSystemRoot));
       dwSize = GetEnvironmentVariable("SystemRoot", szSystemRoot, sizeof(szSystemRoot));

                                        //&? BEM - Until we convert this function to return a TCHAR, we'll hack it to
                                        //&? return a byte.  This WILL break under Unicode!!
       return ((LPCSTR)szSystemRoot)[0];
       }
   else
       {
       return LHWIsNEC() ? 'A' : 'C';   // reasonable defaults
       }
}


// ==== ShellBrowseFolder ===================================================
//
// Description: Display a dialog asking the user to select the directory
//  In which they would like to install Norton AntiVirus.
//
//  Input:
//          hWndParent - the HWND for the parent window
//          pszPrompt - the string that should be displayed to the user.
//          pszDir - [out] the directory for the selected path to be placed in.
//  Output:
//      A value of TRUE if a directory was selected succesfully
//
// ========================================================================
//  5/12/98 - JTAYLOR - Function created.
// ========================================================================
extern "C" DllExport  BOOL ShellBrowseFolder( HWND hWndParent, LPTSTR pszPrompt, LPTSTR pszDir )
{
    BOOL            bRet = FALSE;
    LPTSTR          lpBuffer;
    LPTSTR          lpPrompt;
    IMalloc        *pMalloc;
    BROWSEINFO      bi;
    LPITEMIDLIST    pidlPrograms;       // PIDL for My Computer
    LPITEMIDLIST    pidlBrowse;         // PIDL selected by user

                                        // Get pointer to shell's IMalloc iface

    if ( SHGetMalloc( &pMalloc ) == NOERROR )
    {

                                        // Allocate a buffer to receive browse
                                        // information.
        lpBuffer = (LPTSTR)pMalloc->Alloc( MAX_PATH );
        lpPrompt = (LPTSTR)pMalloc->Alloc( strlen(pszPrompt) + 2 );
        lstrcpy( lpPrompt, pszPrompt );

        if ( lpBuffer != NULL )
        {
                                        // Get the PIDL for the My Computer folder

            HRESULT hRes = SHGetSpecialFolderLocation( hWndParent,
                                                       CSIDL_DRIVES,
                                                       &pidlPrograms );
            if ( hRes == NOERROR )
            {
                                        // Fill in the BROWSEINFO structure.
                bi.hwndOwner = hWndParent;
                bi.pidlRoot = pidlPrograms;
                bi.pszDisplayName = lpBuffer;
                bi.lpszTitle = lpPrompt;
                bi.ulFlags = BIF_RETURNONLYFSDIRS;
                bi.lpfn = NULL;
                bi.lParam = 0;

                                        // Browse for a folder and return its PIDL.

                pidlBrowse = SHBrowseForFolder( &bi );

                if ( pidlBrowse != NULL )
                {
                    if ( SHGetPathFromIDList( pidlBrowse, lpBuffer ) )
                    {
                                        // this code assumes that the lpBuffer will be
                                        // null terminated and not be larger than MAX_PATH
                                        // as the help claims
                        lstrcpy( pszDir, lpBuffer );
                        bRet = TRUE;
                    }

                    pMalloc->Free( pidlBrowse );
                }

                pMalloc->Free( pidlPrograms);
            }

            pMalloc->Free( lpBuffer);
        }
    }

    return( bRet );
}




