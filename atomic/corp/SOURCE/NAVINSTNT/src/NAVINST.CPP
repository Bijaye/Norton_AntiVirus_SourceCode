/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/NAVINST.CPv   1.31   18 Jul 1998 20:15:06   mdunn  $
//
//  NavInst.CPP -   Module that holds mostly install functions for the
//                  InstallShield installation of NAVNT; this is the main
//                  point of entry for the custom DLL.
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/NAVINST.CPv  $
// 
//    Rev 1.31   18 Jul 1998 20:15:06   mdunn
// Added GetFileVersionString()
//
//    Rev 1.30   30 Jun 1998 16:34:02   mdunn
// #ifdef'ed EnableQuarService() for x86
//
//    Rev 1.29   30 Jun 1998 14:20:52   tcashin
// Removed the special processing for NAVENG files. They will be treated the
// same as the rest of the defs.
//
//    Rev 1.28   30 Jun 1998 13:59:36   mdunn
// Added EnableQuarService()
//
//    Rev 1.27   17 Jun 1998 21:58:20   mdunn
// Tim - Added SpecialDefUtils code to handle a mismatched NAVENG.
// Me - Added DisableQuarService() to stop and disable the Quar svc if it's
//      installed.  Fixes #117612.
//    - Added RemoveQConsole(), called from the install script.  Fixes #117498.
//
//    Rev 1.26   10 Jun 1998 18:51:04   mdunn
// Added CreateInternetShortcut()
//
//    Rev 1.25   03 Jun 1998 17:31:42   mdunn
// Added Quarantine and .GID cleanup (and remembered to remove all the int 3's
// this time!)
//
//    Rev 1.24   23 Apr 1998 13:03:00   mdunn
// Added StringContainsDBCSChars()
//
//    Rev 1.23   06 Apr 1998 13:37:22   mdunn
// Changed IsNT40ServicePack2() to GetNT40ServicePack().
//
//    Rev 1.22   26 Mar 1998 20:51:34   mdunn
// Added CreateNAVMutex() and ReleaseNAVMutex(), used to prevent multiple
// instances of install from running.
//
//
//    Rev 1.21   06 Mar 1998 13:39:34   mdunn
// Now deleting all product names from the LU keys (retail, trial, NEC, and
// NEC trial).
//
//    Rev 1.20   17 Feb 1998 15:09:16   mdunn
// Ported IsMachineNEC() from the ROSNEC area.
//
//    Rev 1.19   17 Dec 1997 21:55:50   jtaylor
// Added IsOemPersonalityDll function to check for OEM versions during install.
//
//    Rev 1.18   17 Aug 1997 23:47:28   SEDWARD
// Use memcmp instead of _tcscmp to perform the string compare on "Service
// Pack 2" (in case there's no null).
//
//    Rev 1.17   15 Aug 1997 13:57:42   SEDWARD
// Include "progman.h", added GetCommonStartupDisplayName and
// IsNT40ServicePack2 functions.
//
//    Rev 1.16   03 Aug 1997 20:26:14   SEDWARD
// Added global variable to work with the "Symantec Shared" path.
//
//    Rev 1.15   02 Jul 1997 21:46:42   SEDWARD
// Use 'g_szNavAppIdNavwNt' in call to InitInstallApp() instead of string
// defined in navinst.h.
//
//    Rev 1.14   30 Jun 1997 15:18:28   SEDWARD
// Use character mapping macros (ie, _tcscpy() instead of strcpy()), added
// SetNavStartInfo() and WriteNavstartBinData().
//
//    Rev 1.13   22 Jun 1997 23:01:00   SEDWARD
// ParseServiceLogString() now accepts a usage count argument; added
// IsServiceRunning() function; StopLoggedServices() now checks the usage
// count to help determine if a service is to be marked for deletion; added
// more LTrace calls to StopSingleService().
//
//    Rev 1.12   12 Jun 1997 15:16:38   SEDWARD
// Use the global scope operator on the 3rd parameter to the PreDefUpdate
// call.
//
//    Rev 1.11   11 Jun 1997 15:51:26   SEDWARD
// Now we use the App ID arrays defined in 'NavAppIds.h' to use the newest
// defs and stop using defs.
//
//    Rev 1.10   10 Jun 1997 21:48:04   SEDWARD
// Fixed argument to 'sizeof' in 'GetDefsDirectory' to use local buffer.
//
//    Rev 1.9   08 Jun 1997 15:08:28   SEDWARD
// Set up the service functions so that parameters can now be supplied from
// the installation script (instead of hard-coded).
//
//    Rev 1.8   05 Jun 1997 15:38:12   KSACKIN
// Changed the name of the Norton Program Scheduler from NPSSVC to "Norton
// Program Scheduler"
//
//    Rev 1.7   02 Jun 1997 18:54:52   SEDWARD
// Changed 'navex.sys' to be 'navex15.sys'
//
//    Rev 1.6   02 Jun 1997 15:39:52   SEDWARD
// Added call to 'UseNewestDefs' in 'CopyVirusDefinitionFiles'.
//
//    Rev 1.5   01 Jun 1997 17:19:50   SEDWARD
// Now include 'defutils.h'; added 'GetDefsDirectory' and
// 'CopyVirusDefinitionFiles' functions.
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

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include    "platform.h"
#include    "resource.h"
#include    "svcctrl.h"
#include    "cwstring.h"
#include    "navstart.h"
#include    "NavInst.h"
#include    "DefUtils.h"
#include    "IsUtil.h"
#include    "ProgMan.h"
#include    "n32behav.h"  // for scanner only flag defines
#include    "cverrsrc.h"

// Ensure that we define and initialize the strings defined in NavAppIDs.h
#define NAVAPPIDS_BUILDING

// Defines an array of NAV AppIds that uninstall can loop through.
#define NAVAPPIDS_WANT_ARRAY
#include "NavAppIds.h"


// SCOTT
// #include "ctsn.h"        // compilation problems...
// from ctsn.h...
// #define SIZE_VIRUSNAME      (21 + 1)        // This does NOT include EOS!
// #define SYM_MAX_PATH        256

#include "navapcom.h"

#ifdef _M_IX86                          // building for x86
#define INITGUID
#include "objbase.h"
#include "intshcut.h"
#endif

// Include qconres.h so we can load strings from QCONRES.DLL
#include "qconres.h"

// local function prototypes
STATUS LOCAL ReadNavstartBinData (LPCTSTR  szNavstart, LPNAVSTARTBINDATA  lpOptions);
BOOL CALLBACK EnumWindowProc ( HWND hwnd, LPARAM lParam );

// Prototypes for functions copied locally out of Symkrnl
void        LNameAppendFile ( LPTSTR szPath, LPCTSTR szFile );

/////////////////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
extern "C"  {
#endif

BOOL            g_RunSchedule;
TCHAR           g_szTarget[SYM_MAX_PATH];
TCHAR           g_szSharedDir[SYM_MAX_PATH];
TCHAR           g_szSymSharedDir[SYM_MAX_PATH];
TCHAR           g_szStringTableString[SYM_MAX_PATH];
HINSTANCE       g_hInst;
HANDLE          g_hMutex;
HWND            g_hwndQConsole;
HINSTANCE       g_hinstQConRes;

#ifdef  __cplusplus
    }
#endif

CDefUtils*          defObjectPtr;                       // for installing virus definition files

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



// ==== ParseServiceLogString =============================================
//
//  This function parses the input log string (ie, read from the uninstall
//  log file into the service name and a boolean indicating whether or not
//  the service is to be stopped.  The other input arguments are initialized
//  with these parsed values.
//
//  NOTE: it is the responsibility of the caller to be sure that the service
//        name buffer is large enough to hold the full name of the service.
//
//  Input:
//      lpszLogString   -- a pointer to the log file string
//      lpszServiceName -- a pointer to the buffer that is to hold the name
//                         of the service
//      bStopService    -- a boolean that is to hold the flag value as to
//                         whether or not the service is to be stopped
//
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

BOOL    ParseServiceLogString(LPTSTR  lpszLogString, LPTSTR  lpszServiceName
                                                        , BOOL&  bStopService
                                                        , BOOL&  bCheckUsageCount)
{
    auto    BOOL        bRetVal = TRUE;
    auto    TCHAR*      logStrPtr;

    // if the input log string is null, exit the function
    logStrPtr = lpszLogString;
    if (NULL == logStrPtr)
        {
        bRetVal = FALSE;
        goto  End_Function;
        }

    // get the service name
    while ((NULL != *logStrPtr)  &&  ('=' != *logStrPtr))
        {
        if ('=' != *logStrPtr)
            {
            *lpszServiceName = *logStrPtr;
            ++lpszServiceName;
            ++logStrPtr;
            }
        }
    if ('=' == *logStrPtr)
        {
        *lpszServiceName = NULL;
        }
    else        // input string is invalid
        {
        bRetVal = FALSE;
        goto  End_Function;
        }

    // get the stop service flag
    ++logStrPtr;
    if ('1' == *logStrPtr)
        {
        bStopService = TRUE;
        }
    else
        {
        bStopService = FALSE;
        }

    // get the check usage count flag
    ++logStrPtr;
    ++logStrPtr;
    if ('1' == *logStrPtr)
        {
        bCheckUsageCount = TRUE;
        }
    else
        {
        bCheckUsageCount = FALSE;
        }

End_Function:
    return  (bRetVal);


}  // end of "ParseServiceLogString"



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


/////////////////////////////////////////////////////////////////////////
//
// Exported Functions
//
/////////////////////////////////////////////////////////////////////////


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
        lpszAppIdArray = g_lpszNavAppIdNtArray;
        nNumAppIds = g_ucNavAppIdNtArrayNum;
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


#ifdef _M_IX86
//////////////////////////////////////////////////////////////////////////
//
// Function:    EnableQuarService
//
// Description:
//  Enables the Quarantine service, if it was previously disabled by
//  uninstalling NAV.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if the service wasn't installed (not an error condition), or if
//  the service was reenabled.  FALSE if the service's config couldn't
//  be changed.
//
//////////////////////////////////////////////////////////////////////////
//  6/30/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL EnableQuarService()
{
CServiceControl svc;
TCHAR           szServiceName[] = _T("QUARSVC");
BOOL            bRetVal = FALSE;

    if ( ERROR_SUCCESS == svc.Open ( szServiceName ) )
        {
        if ( ERROR_SUCCESS ==
                 svc.ChangeServiceConfig_StartType ( SERVICE_AUTO_START )
             &&
             ERROR_SUCCESS == svc.Start() )
            {
            bRetVal = TRUE;
            }
        }
    else
        {
        bRetVal = TRUE;                 // service wasn't installed
        }

    return bRetVal;
}
#endif  // def _M_IX86


// === GetCommonStartupDisplayName ========================================
//
//  This function gets the official display string to represent the common
//  startup group.  This is useful when dealing with foreign (ie, non-English)
//  systems.  We export the function so it is available to the installation
//  script.
//
//  Input:
//      lpNameBuf   -- a pointer to a buffer that will hold the string
//      groupType   -- either GROUPTYPE_COMMON or GROUPTYPE_PRIVATE
//
//  Output:
//      nothing
//
// ========================================================================
//  Function created: 8/97, SEDWARD
// ========================================================================

extern "C" DllExport    void    GetCommonStartupDisplayName(LPSTR  lpNameBuf
                                                            , int  groupType)
{
    GetProgmanGroupName(lpNameBuf, groupType);


}  // end of "GetCommonStartupDisplayName"



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
//      bIsHub              -- determines whether the virus set to install
//                             is a hub set
//
//  Output: TRUE if no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 05/97, SEDWARD
// ========================================================================

extern "C" DllExport    BOOL GetDefsDirectory(LPTSTR  pszAppInstallDir,
                                              LPTSTR  pszDefsDir,
                                              BOOL    bIsHub)
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

    defObjectPtr->InitInstallApp(g_szNavAppIdNavwNt, pszAppInstallDir);

    // fetch the path of the temporary virus defs directory
    *szBuf = NULL;

    if (bIsHub)
        defObjectPtr->PreDefUpdate(szBuf, sizeof(szBuf), CDefUtils::Install);
    else
        defObjectPtr->PreDefUpdate(szBuf, sizeof(szBuf), CDefUtils::Other);

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



// ==== InstallSingleService ==============================================
//
//  This exported function installs a single service.
//
//  Input:
//      szServiceName   -- the display name of the service
//      szBinaryName    -- the name as it appears in a directory listing
//      dwServiceType   -- the type of service (kernel, interactive, etc.)
//      bAutoStart      -- if the service is to start automatically or not
//      szTarget        -- the path to the service EXE
//
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

extern "C" DllExport   BOOL    InstallSingleService(LPTSTR  lpszServiceName
                                                        , LPTSTR  lpszServiceDisplayName
                                                        , LPTSTR  lpszBinaryName
                                                        , DWORD  dwServiceType
                                                        , BOOL  bAutoStart
                                                        , LPTSTR  lpszTarget)
{
    auto    char                szBinaryName[SYM_MAX_PATH*2];
    auto    BOOL                bStatus = TRUE;
    auto    CServiceControl     cService;

    // create a full path to the service
    wsprintf(szBinaryName, "%s\\%s", lpszTarget, lpszBinaryName);
    if (ERROR_SUCCESS == cService.Open(lpszServiceName))
        {
        // service exists, change its config
        if (ERROR_SUCCESS != cService.ChangeServiceConfig(
                                dwServiceType,                  // Service type
                                bAutoStart ?                    // do we want autostart?
                                SERVICE_AUTO_START :            // attempt autostart
                                SERVICE_DEMAND_START,           // manual start
                                SERVICE_ERROR_NORMAL,           // severity if service fails
                                szBinaryName))                  // path to the program binary
            {
            bStatus = FALSE;
            }
        }
    else
        {
        // initialize our service object; must feed it at least one parameter
        // or the compiler will complain about us being ambiguous
        cService.Open((LPTSTR) NULL);
        if (ERROR_SUCCESS != cService.CreateService(
                                lpszServiceName,                // name of service
                                lpszServiceDisplayName,         // display name
                                SERVICE_ALL_ACCESS,             // type of access to service
                                dwServiceType,                  // type of service
                                bAutoStart ?                    // when to start service
                                SERVICE_AUTO_START :            // attempt autostart
                                SERVICE_DEMAND_START,           // manual start
                                SERVICE_ERROR_NORMAL,           // severity if service fails
                                szBinaryName))                  // Binary name
            {
            bStatus = FALSE;
            }
        }

    // if anything failed, exit
    if (FALSE == bStatus)
        {
        goto Cleanup;
        }
    else if (TRUE == bAutoStart)
        {
        if  ((ERROR_SUCCESS !=  cService.Open(lpszServiceName))
        ||  (ERROR_SUCCESS != cService.Start()))
            {
            bStatus = FALSE;
            }
        }


Cleanup:
    return  (bStatus);

}  // end of "InstallSingleService"



// ==== GetNT40ServicePack ================================================
//
//  This exported function reads the system's service pack number if the
//  OS is NT 4.0.
//
//  Input:  nothing
//  Output: The service pack number, or zero if no service pack has been
//          applied.  If the OS is not NT 4.0, or if an error occured when
//          reading the OS version info, -1 is returned.
//
// ========================================================================
//  Function created: 8/97, SEDWARD
// 4/5/98  Mike Dunn -- changed to return the service pack # as an int.
// ========================================================================

extern "C" DllExport   int      GetNT40ServicePack()
{
    auto    int                 nResult = -1;
    auto    OSVERSIONINFO       osInfo;

    auto    TCHAR               szSrvcPackVer[] = { _T("Service Pack ") };

    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if ( GetVersionEx (&osInfo) )
        {
        if ( 4 == osInfo.dwMajorVersion &&
             0 == osInfo.dwMinorVersion   )
            {
                                        // We're running on NT 4.  Read
                                        // the add'l version info string and
                                        // if it's a service pack ID, read
                                        // in the SP number.

            if ( '\0' == osInfo.szCSDVersion[0] )
                {
                nResult = 0;            // No SP on the system.
                }
            else if ( 0 == memcmp ( osInfo.szCSDVersion,
                                    szSrvcPackVer,
                                    sizeof(szSrvcPackVer)-sizeof(TCHAR) ))
                {
                nResult =
                    atoi ( &osInfo.szCSDVersion [lstrlen(szSrvcPackVer)] );
                }
            }
        }


    LTrace("UninstUnInitialize: OSVERSIONINFO");
    LTrace("    osInfo.dwOSVersionInfoSize = %d", osInfo.dwOSVersionInfoSize);
    LTrace("    osInfo.dwMajorVersion = %d", osInfo.dwMajorVersion);
    LTrace("    osInfo.dwMinorVersion = %d", osInfo.dwMinorVersion);
    LTrace("    osInfo.dwBuildNumber = %d", osInfo.dwBuildNumber);
    LTrace("    osInfo.dwPlatformId = %d", osInfo.dwPlatformId);
    LTrace("    osInfo.szCSDVersion = %s", osInfo.szCSDVersion);

    return (nResult);

}  // end of "GetNT40ServicePack"


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


// ==== IsServiceMarkedForDeletetion ======================================
//
//  This function tests to see if a target service is marked for deletion.
//
//  Input:
//      pszServiceName  -- the name of the service
//
//  Output: TRUE if the service is marked for deletion, FALSE if it is not
//
// ========================================================================
//  Function created: 06/97, SEDWARD
// ========================================================================

extern "C" DllExport    BOOL    IsServiceMarkedForDeletetion(LPTSTR  pszServiceName)
{
    auto    BOOL            bRetValue;
    auto    CServiceControl cServiceControl;
    auto    DWORD           dwReturn;

    bRetValue = FALSE;
    cServiceControl.Open(pszServiceName);
    if (ERROR_SUCCESS != cServiceControl.ChangeServiceConfig())
        {
        dwReturn = GetLastError();

        // now test for the deletion flag
        if (ERROR_SERVICE_MARKED_FOR_DELETE == dwReturn)
            {
            bRetValue = TRUE;
            }
        }

    return (bRetValue);

}  // end of "IsServiceMarkedForDeletetion"



// ==== IsServiceRunning ==================================================
//
//  This function tests to see if a target service is currently running.
//
//  Input:
//      pszServiceName  -- the name of the service
//
//  Output: TRUE if the service is running, FALSE if it is not
//
// ========================================================================
//  Function created: 06/97, SEDWARD
// ========================================================================

extern "C" DllExport    BOOL    IsServiceRunning(LPTSTR  pszServiceName)
{
    auto    BOOL                bRetValue;
    auto    CServiceControl     cService;
    auto    DWORD               dwStatus;

    bRetValue = FALSE;
    if (ERROR_SUCCESS == cService.Open(pszServiceName))
        {
        if  ((cService.QueryServiceStatus(&dwStatus))
        &&  (SERVICE_RUNNING == dwStatus))
            {
            bRetValue = TRUE;
            }
        }

    return (bRetValue);

}  // end of "IsServiceRunning"



// ==== RemoveAPUI ========================================================
//
//  This function attempts to close the AP window, if it is running.
//
//  Input:  nothing
//  Output: a value of TRUE if the window was found, FALSE if it was not
//
// ========================================================================
//  Function created: 06/97, SEDWARD
// ========================================================================

extern "C"  DllExport   BOOL    RemoveAPUI(void)
{
    auto    BOOL            bReturnValue;
    auto    HWND            hNavAp;

    hNavAp = FindWindow(NAVAP_WND_CLASS, NULL);
    if (hNavAp)
        {
        // found the windows, send a message to close it
        PostMessage(hNavAp, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
        bReturnValue = TRUE;
        }
    else
        {
        bReturnValue = FALSE;
        }

    return (bReturnValue);

}  // end of "RemoveAPUI"


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
HKEY           hkey = NULL;
DWORD          dwBuffSize = sizeof(szQResDLLPath);


    g_hwndQConsole = NULL;

                                        // Get the NAV install directory.

    if ( ERROR_SUCCESS != RegOpenKeyEx (
                              HKEY_LOCAL_MACHINE,
                              _T("Software\\Symantec\\InstalledApps"),
                              0, KEY_QUERY_VALUE, &hkey ) )
        {
        goto bailout;
        }

    if ( ERROR_SUCCESS != RegQueryValueEx (
                              hkey, _T("NAVNT"),
                              NULL, NULL,
                              (LPBYTE) szQResDLLPath, &dwBuffSize ) )
        {
        goto bailout;
        }


                                        // The mutex name *MUST* match the
                                        // one in QCONSOLE/qconsole.cpp.

    hMutex = CreateMutex ( NULL, FALSE, _T("Quarantine Console Mutex") );

    if ( NULL != hMutex  &&  ERROR_ALREADY_EXISTS == GetLastError() )
        {
                                        // QConsole is open!  Load the QConsole
                                        // resource DLL to get the window
                                        // title string.
        LNameAppendFile ( szQResDLLPath, _T("qconres.dll") );

        g_hinstQConRes = LoadLibrary ( szQResDLLPath );

        if ( NULL == g_hinstQConRes )
            {
            goto bailout;               // Fail silently if we can't load
                                        // the DLL.
            }

        if ( LoadString ( g_hinstQConRes, IDS_MAINFRM_TITLE,
                          g_szStringTableString,
                          sizeof(g_szStringTableString) ) )
            {
            EnumWindows ( (WNDENUMPROC) EnumWindowProc, 0 );

            if ( NULL != g_hwndQConsole )
                {
                SendMessage ( g_hwndQConsole, WM_CLOSE, 0, 0 );
                }
            }

        FreeLibrary ( g_hinstQConRes );
        }

bailout:
    if ( NULL != hMutex )
        CloseHandle ( hMutex );

    if ( NULL != hkey )
        RegCloseKey ( hkey );

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



// ==== StopLoggedServices ================================================
//
//  This function stops any services that were recorded in the uninstall
//  log file during installation.  If the "delete" argument is set to a
//  value of TRUE, then the services are actually removed; otherwise,
//  they're simply stopped.
//
//  Input:
//      lptstrLogFilename   -- a null-terminated string containing the full
//                             path to the uninstall log file created during
//                             install
//
//      bDeleteService      -- TRUE means delete the services, FALSE means
//                             just stop them
//
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

extern "C" DllExport   BOOL    StopLoggedServices(LPTSTR  lptstrLogFilename
                                                        , BOOL  bDeleteAllServices)
{
    auto    BOOL            bReturn = TRUE;
    auto    CWString        szLogFile(SYM_MAX_PATH);
    auto    CWString        szTemp(SYM_MAX_PATH);
    auto    CWString        szValue(SYM_MAX_PATH);
    auto    FILE            *filePtr = NULL;
    auto    int             nLen;
    auto    LPTSTR          lpLine;
    auto    LPTSTR          lpLogSect = "Services";

    LTrace("BEGIN StopLoggedServices(%s)", lpLogSect);

    // open the uninstall log file
    _tcscpy(szLogFile, lptstrLogFilename);
    filePtr = fopen(szLogFile, "rt");

    LTrace("filePtr = %x", filePtr);

    // if the file was opened and we have a section to target...
    if ((filePtr != NULL)  &&  (lpLogSect != NULL))
        {
        // ...go to the "services" section in the log file
        lpLine = LogFileFindSection(filePtr, lpLogSect);
        if (lpLine != NULL)
            {
            auto    BOOL            bDeleteService;
            auto    BOOL            bCheckUsageCount;
            auto    BOOL            bStopService;
            auto    CWString        szServiceName(SYM_MAX_PATH);
            auto    DWORD           dwCurrentUsageCount;

            do  {
                // fetch a line of text from the logfile
                _tcscpy(szTemp, lpLine);
                nLen = _tcsclen(szTemp);
                if  ((nLen)
                &&  (ParseServiceLogString(szTemp, szServiceName, bStopService
                                                            , bCheckUsageCount)))
                    {
                    if (TRUE == bCheckUsageCount)
                        {
                        // get the current usage count to see if we should mark
                        // the current service for deletion (NOTE: this assumes
                        // that the current usage count is up to date)
                        GetServicePath(szServiceName, szTemp);
                        GetUsageCount(szTemp, dwCurrentUsageCount);
                        if (dwCurrentUsageCount > 0)
                            {
                            bDeleteService = FALSE;
                            }
                        else
                            {
                            bDeleteService = TRUE;
                            }
                        }
                    else    // use whatever was passed to this function
                        {
                        bDeleteService = bDeleteAllServices;
                        }

                    // send params to the stop service function
                    StopSingleService(szServiceName, bStopService, bDeleteService);
                    }
                else
                    {
                    break;
                    }

                lpLine = LogFileGetNextLine(filePtr);

                }  while(lpLine != NULL);
            }

        fclose(filePtr);
        }
    else
        {
        LTrace("   Logfile open error");
        bReturn = FALSE;
        }

    LTrace("END StopLoggedServices(%d)\n", bReturn);

    return  (bReturn);

}  // end of "StopLoggedServices"



// ==== StopSingleService =================================================
//
//  This exported function stops a single service.  If the "delete" argument
//  is set to TRUE, then the service is actually deleted, otherwise it is
//  simply stopped.
//
//  Input:
//      lpszServiceName -- the display name of the service
//      bStopService    -- TRUE means stop the service, FALSE means not to
//      bDeleteService  -- if TRUE, the service is deleted
//
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

extern "C" DllExport   BOOL    StopSingleService(LPTSTR  lpszServiceName
                                                            , BOOL  bStopService
                                                            , BOOL  bDeleteService)
{
    auto    BOOL                    bReturn = TRUE;
    auto    CServiceControl         cService;
    auto    DWORD                   dwStatus = TRUE;

    LTrace("BEGIN StopSingleService(%s)", lpszServiceName);

    if (ERROR_SUCCESS == cService.Open(lpszServiceName))
        {

        LTrace("   %s successfully opened", lpszServiceName);

        if  ((bStopService)
        &&  (cService.QueryServiceStatus(&dwStatus))
        &&  (SERVICE_STOPPED != dwStatus))
            {

            LTrace("      about to stop %s", lpszServiceName);

            // if it is not stopped, try to stop it
            if (ERROR_SUCCESS != cService.Stop())
                {
                LTrace("      stop on %s FAILED", lpszServiceName);
                bReturn = FALSE;
                }
            }

        if (bDeleteService)
            {
            // don't do error checking here because there are times when delete will
            // fail, but the services are set up correctly
            LTrace("      about to delete %s", lpszServiceName);
            cService.Delete();
            }
        }
    else    // failed to open it?
        {

        LTrace("   %s FAILED to open", lpszServiceName);

        bReturn = FALSE;
        }

    LTrace("END StopSingleService(%s), bReturn = %d", lpszServiceName, bReturn);

    return  (bReturn);

}  // end of "StopSingleService"


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
//  This is a copy of the Quake function HWIsNEC(), copied here locally
//  so that the install custom DLL won't be dependant on s32navk.dll.
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
static BOOL bTested = FALSE;
static BOOL bIsNEC  = FALSE;

    if ( !bTested )
        {
        UINT uType;

        bTested = TRUE;
                                        // Get main KB type
        uType = GetKeyboardType(0);

        if ( uType == 7 )               // '7' is japanese keyboard type
            {
                                        // Get the sub-type.
            uType = GetKeyboardType(1);

            if ( uType >= 0x0D01 )
                {
                bIsNEC = TRUE;
                }
            }
        }

    return bIsNEC;
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
//  szCmd: [in]
//  szCommand: [in] The NAVDX command line to write to the file.
//
// Returns:
//  TRUE if the line was added OK, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 3/2/98  Mike Dunn -- function created
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
// Note:
//  This is a stub for Alpha due to a bug in MSVC 5.  The
//  IID_IUniformResourceLocator symbol is a macro that resolves to
//  IID_IUniformResourceLocatorA, but in MSVC's SHELL32.LIB, the symbol
//  doesn't have the A, even though the header files do!  (Good one MS!)
//  It works on x86 becuase the INetSDK has a fixed LIB.
//
//////////////////////////////////////////////////////////////////////////
// 6/10/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

extern "C" DllExport BOOL CreateInternetShortcut ( LPCTSTR szPath,
                                                   LPCTSTR szURL )
{
#ifdef _M_ALPHA
    return FALSE;
#else

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
#endif  // ndef _M_ALPHA
}



