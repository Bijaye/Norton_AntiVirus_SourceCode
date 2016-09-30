/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/NAVINST.H_v   1.27   15 Oct 1998 12:33:58   mdunn  $
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/NAVINST.H_v  $
// 
//    Rev 1.27   15 Oct 1998 12:33:58   mdunn
// Changed a couple of reg key names from 5.3 to 5.31.
// 
//    Rev 1.26   18 Jul 1998 20:15:00   mdunn
// Added GetFileVersionString()
// 
//    Rev 1.25   30 Jun 1998 16:34:02   mdunn
// #ifdef'ed EnableQuarService() for x86
// 
//    Rev 1.24   30 Jun 1998 13:59:36   mdunn
// Added EnableQuarService()
// 
//    Rev 1.23   17 Jun 1998 21:53:02   mdunn
// Tim - Added SpecialDefUtils code to handle a mismatched NAVENG.
// Me - Added DisableQuarService() to stop and disable the Quar svc if it's
//      installed.  Fixes #117612.
//    - Added RemoveQConsole(), called from the install script.  Fixes #117498.
// 
//    Rev 1.22   10 Jun 1998 18:51:04   mdunn
// Added CreateInternetShortcut()
// 
//    Rev 1.21   03 Jun 1998 17:31:40   mdunn
// Added Quarantine and .GID cleanup (and remembered to remove all the int 3's
// this time!)
// 
//    Rev 1.20   23 Apr 1998 13:03:00   mdunn
// Added StringContainsDBCSChars()
// 
//    Rev 1.19   17 Apr 1998 16:09:46   RStanev
// SmellyCat changes.
//
//    Rev 1.18   06 Apr 1998 13:38:14   mdunn
// Changed IsNT40ServicePack2() to GetNT40ServicePack().
//
//    Rev 1.17   26 Mar 1998 20:51:14   mdunn
// Added CreateNAVMutex() and ReleaseNAVMutex(), used to prevent multiple
// instances of install from running.
//
//
//    Rev 1.16   10 Mar 1998 18:18:44   mdunn
// Changed a couple of reg key names from 5.0 to 5.3 to fix uninstall.
//
//    Rev 1.15   06 Mar 1998 13:39:58   mdunn
// Now deleting all product names from the LU keys (retail, trial, NEC, and
// NEC trial).
//
//    Rev 1.14   17 Feb 1998 15:08:52   mdunn
// Ported IsMachineNEC() from the ROSNEC area.
//
//    Rev 1.13   17 Dec 1997 21:55:50   jtaylor
// Added IsOemPersonalityDll function to check for OEM versions during install.
//
//    Rev 1.12   18 Aug 1997 23:33:12   SEDWARD
// Made 'UninstallVirusDefinitions' an exported function so it could be called
// from the install script.
//
//    Rev 1.11   15 Aug 1997 13:55:46   SEDWARD
// Added SERVICE_PACK2_STRING define statement, prototypes for
// 'CleanupCommonStartupGroup', 'GetCommonStartupDisplayName' and
// 'IsNT40ServicePack2'.
//
//    Rev 1.10   10 Aug 1997 23:47:12   SEDWARD
// Added SYMEVENT_SERVICE_NAME define; added 'MarkSymevent' prototype.
//
//    Rev 1.9   03 Aug 1997 20:26:44   SEDWARD
// Added SEVINST_EXE define; added define for the "Symantec Shared" path;
// changed NAV_INSTALL_KEY to use "5.0" instead of "4.0"; added new global
// variable for the "Symantec Shared" path; changed function prototypes to
// support the "Symantec Shared" path (ie, GetNavPaths, ReadNavPaths and
// WriteNavPaths).
//
//    Rev 1.8   30 Jun 1997 15:19:18   SEDWARD
// Added new prototypes, use character-mapping macros (ie, _tcscpy() instead of
// strcpy()).
//
//    Rev 1.7   24 Jun 1997 16:28:18   SEDWARD
// Added prototype for 'DecrementIRPStackSize()'.
//
//    Rev 1.6   22 Jun 1997 23:02:34   SEDWARD
// Added SERVICES_REGKEY_PATH define statement; added prototypes for
// GetServicePath() and GetUsageCount() and IsServiceRunning().
//
//    Rev 1.5   08 Jun 1997 15:07:10   SEDWARD
// Added 'ParseServiceLogString', 'InstallSingleService', 'RemoveAPUI',
// 'IsServiceMarkedForDeletion' and 'StopSingleService'.
//
//    Rev 1.4   01 Jun 1997 17:20:34   SEDWARD
// Now include 'defutils.h'; added new function prototypes for
// 'UninstallVirusDefinitions', 'CopyVirusDefinitionFiles' and
// 'GetDefsDirectory'.
//
//    Rev 1.3   26 May 1997 15:30:20   SEDWARD
// Increased the number of services installed to accomodate the Norton
// Program Scheduler (npssvc.exe).
//
//    Rev 1.2   17 May 1997 19:12:32   SEDWARD
// New prototype for 'GetNavStartInto'; new #defines for NAVSTART_OPTIONS.
//
//    Rev 1.1   11 May 1997 21:31:52   SEDWARD
// Include 'platform.h' and 'navver.h'; removed SI_MAX_PATH #define;
// added TEMP_REG_KEY; added new function prototypes; fixed reference
// to global instance handle in 'GetString'.
//
//    Rev 1.0   05 May 1997 19:26:06   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////




#ifndef NAVINST_HEADER
#define NAVINST_HEADER


#include <windows.h>
#include <stdio.h>

#define NAVWNT
#include    "platform.h"
#include    "navstart.h"
#include    "navver.h"
#include    "DefUtils.h"





///////////////////////////////////////////////////////////////////////////////
//
//  Preprocessor
//
//  NOTE: the dllexport attribute eliminates the need for a .DEF file
//
///////////////////////////////////////////////////////////////////////////////

#define DllImport   __declspec( dllimport )
#define DllExport	__declspec( dllexport )





///////////////////////////////////////////////////////////////////////////////
//
//  Define Statements
//
///////////////////////////////////////////////////////////////////////////////

// SCOTT
// #include "ctsn.h"        // compilation problems...
// from ctsn.h...
#define SIZE_VIRUSNAME      (21 + 1)        // This does NOT include EOS!



///////////////////////////////////////////////////////////////////////////////
//  General defines
///////////////////////////////////////////////////////////////////////////////

#define DEFUTILS_APP_ID_NAVW                    "NAVNT_30_NAVW"
#define DEFUTILS_APP_ID_AP1                     "NAVNT_30_AP1"
#define DEFUTILS_APP_ID_AP2                     "NAVNT_30_AP2"
#define IS_FILE_DELETED                         0x00000000
#define IS_FILE_IN_USE                          0x00000003
#define IS_FILE_NOT_FOUND                       0x00000001
#define IS_DIR_NOT_EMPTY                        IS_FILE_IN_USE
#define IS_DIR_NOT_FOUND                        IS_FILE_NOT_FOUND
#define IS_DIR_REMOVED                          IS_FILE_DELETED
#define NSPLUGIN_EXE                            "nsplugin.exe"
#define DEFANNTY_DLL                            "DefAnnty.dll"
#define DEFANNTY_UNINITIALIZE_PROC              "_DefAnnuityUninitialize@4"
#define NAVAP_SERVICE_INDEX                     4
#define NAVSTART_OPTION_SCAN_DOS_FILES          500
#define NAVSTART_OPTION_SCAN_WINDOWS_FILES      501
#define NAVSTART_OPTION_LOAD_TSR                502
#define NAVSTART_OPTION_LOAD_AP_AT_STARTUP      503
#define NAVSTART_OPTION_PROTECT_FEATURE         504
#define LOGFILE_NAME                            "_ISNAVNT.ULG"
#define NAVNT_ID                                "NAVNT"
#define NPROTECT_SERVICE_NAME                   "NProtectService"
#define NUMBER_OF_SERVICES                      6
#define SERVICE_PACK2_STRING                    "Service Pack 2"
#define SEVINST_EXE                             "sevinst.exe"
#define SI_MAXSTR                               1024
#define SYMEVENT_SERVICE_NAME                   "SymEvent"



///////////////////////////////////////////////////////////////////////////////
//  Registry Keys
///////////////////////////////////////////////////////////////////////////////

#define MS_DLLS_KEY                         "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDlls"
#define MS_HELP_KEY                         "SOFTWARE\\Microsoft\\Windows\\Help"
#define SYM_SHAREDUSAGE_KEY                 "SOFTWARE\\Symantec\\SharedUsage"
#define NAV_INSTALL_KEY                     "SOFTWARE\\Symantec\\Norton AntiVirus NT\\Install\\5.31"
#define NAV_INSTALLDIR                      "InstallDir"
#define NAV_EXE                             "navwnt.exe"
#define NAV_KEY                             "SOFTWARE\\Symantec\\Norton AntiVirus NT"
#define QUAR_KEY                            "software\\Symantec\\Norton AntiVirus\\Quarantine"
#define QUAR_TEMP_PATH_VALUE                "TempPath"
#define CORP_FEATURES_KEY                   "software\\Symantec\\Norton AntiVirus"
#define CORP_FEATURES_VALUE                 "CorporateFeatures"
#define NAVSCAN_INSTALLKEY                  "SOFTWARE\\Symantec\\Norton AntiVirus Scanner\\Install\\5.31"
#define IRPSTACK_KEY                        "SYSTEM\\CurrentControlSet\\Services\\LanmanServer\\Parameters"
#define SERVICES_REGKEY_PATH                "SYSTEM\\CurrentControlSet\\Services"
#define TEMP_REGKEY_PATH                    "SOFTWARE\\Symantec\\Temp"
#define TEMP_REGVAL_NAV_PATH                "NavPath"
#define TEMP_REGVAL_SYMANTEC_PATH           "Symantec"
#define TEMP_REGVAL_SYMANTEC_SHARED_PATH    "SymantecShared"
#define KEY_LIVE_UPDATE_DLL                 "s32live1.dll"




/////////////////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
extern "C"  {
#endif

extern  BOOL            g_RunSchedule;
extern  TCHAR           g_szTarget[SYM_MAX_PATH];
extern  TCHAR           g_szSharedDir[SYM_MAX_PATH];
extern  TCHAR           g_szSymSharedDir[SYM_MAX_PATH];
extern  TCHAR           g_szStringTableString[SYM_MAX_PATH];
extern  HINSTANCE       g_hInst;

#ifdef  __cplusplus
    }
#endif


extern  CDefUtils*      defObjectPtr;   // object to handle virus defs





///////////////////////////////////////////////////////////////////////////////
//  Function Prototypes (internal)
///////////////////////////////////////////////////////////////////////////////

BOOL    CleanupCommonStartupGroup(BOOL  bIsExplorerShell);
BOOL    CleanupLiveUpdate(LPTSTR  lpSymantecPath);
BOOL    CleanUpOnRamp();
void    CleanupSharedSymantecRegEntries(void);
BOOL    CreateMifFile(LPTSTR  lpStatus, LPTSTR  lpMessage);
BOOL    DecrementIRPStackSize(void);
BOOL    GetNavPaths(LPTSTR lpNavPath, LPTSTR lpSharedPath,
                    LPTSTR lpSymSharedPath);
BOOL    GetServicePath(LPTSTR  lpszServiceName, LPTSTR  strBuf);
BOOL    MarkSymevent(void);
BOOL    NavUninstallSucceeded(void);
BOOL    ParseServiceLogString(LPTSTR  lpszLogString, LPTSTR  lpszServiceName
                                                        , BOOL&  bStopService
                                                        , BOOL&  bCheckUsageCount);
int     RemoveLeftoverDataFiles(void);
BOOL    ReadNavPaths(LPTSTR  navPathStr, LPTSTR  symantecPathStr, LPTSTR  symantecSharedStr);
int     RemoveTargetFiles(TCHAR*  pszTargetDir, TCHAR**  filenameArray
                                                            , BOOL  bRemoveDir);
BOOL    ValidFileTarget(DWORD  fileAttributes);
BOOL    WriteNavPaths(LPSTR  navPathStr, LPSTR  symantecPathStr
                                                    , LPSTR symantecSharedPathStr);
BOOL    WriteNavstartBinData(LPCTSTR  lpszFilename, LPNAVSTARTBINDATA  lpOptions);


///////////////////////////////////////////////////////////////////////////////
//  Function Prototypes (exported)
///////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
extern "C"  {
#endif

DllExport   BOOL    CopyVirusDefinitionFiles(LPSTR lpTempUpdateDir);
DllExport   void    GetCommonStartupDisplayName(LPSTR  lpNameBuf, int  groupType);
DllExport   BOOL    GetDefsDirectory(LPTSTR  pszAppInstallDir, LPTSTR  pszDefsDir);
DllExport   BOOL    GetFileVersionString ( LPCTSTR, LPTSTR );
DllExport   int     GetNavStartInfo(LPCTSTR  filename, int  targetInfo);
DllExport   BOOL    GetUsageCount(LPTSTR  lpszItemName, DWORD&  dwCurrentUsageCount);
DllExport   BOOL    InstallSingleService(LPTSTR  lpszServiceName
                                                        , LPTSTR  lpszServiceDisplayName
                                                        , LPTSTR  lpszBinaryName
                                                        , DWORD  dwServiceType
                                                        , BOOL  bAutoStart
                                                        , LPTSTR  lpszTarget);
DllExport   int     GetNT40ServicePack(void);
DllExport   BOOL    IsOEMPersonalityDLL(LPTSTR lpszFullDllName);
DllExport   BOOL    IsServiceMarkedForDeletetion(LPTSTR  pszServiceName);
DllExport   BOOL    IsServiceRunning(LPTSTR  pszServiceName);
DllExport   BOOL    StopLoggedServices(LPTSTR  szLogFile, BOOL  bDeleteService);
DllExport   BOOL    RemoveAPUI(void);
DllExport   BOOL    RemoveScheduledScans(void);
DllExport   BOOL    SetNavStartInfo(LPCTSTR  filename, int  targetInfo, BOOL  bValue);
DllExport   BOOL    ServicesMarkedForDeletion(void);
DllExport   BOOL    StopSingleService(LPTSTR  lpszServiceName, BOOL  bStopService
                                                            , BOOL  bDeleteService);
DllExport   BOOL    StringContainsDBCSChars ( LPTSTR lpszString );
DllExport   BOOL    UninstallVirusDefinitions(void);
DllExport   LONG    UninstInitialize(HWND hwndDlg, HANDLE hInstance, LONG lRes);
DllExport   LONG    UninstUnInitialize(HWND hwndDlg, HANDLE hInstance, LONG lRes);
DllExport   BOOL    MachineIsNEC();
DllExport   BOOL    CreateProcessAndWait ( LPCTSTR, LPINT );
DllExport   BOOL    CreateNAVMutex ( LPCTSTR );
DllExport   BOOL    ReleaseNAVMutex();
DllExport   BOOL    CreateInternetShortcut ( LPCTSTR szPath, LPCTSTR szURL );

#ifdef _M_IX86
DllExport   BOOL    EnableQuarService();
#endif

#ifdef  __cplusplus
    }
#endif


/////////////////////////////////////////////////////////////////////////////
//  Typedefs
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern"C"
{
#endif

typedef LONG (*ONRAMPCLEANUPEX)(LPCTSTR productName,
                                LPCTSTR productVersion,
                                LPCTSTR productLanguage);
#ifdef __cplusplus
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Structures and devices used in controlling the services
/////////////////////////////////////////////////////////////////////////////

typedef struct  tagSERVICE_CREATE_STRUCT
    {
    char*   lpszServiceName;
    char*   lpszBinaryName;
    DWORD   dwServiceType;
    BOOL    bAutoStart;
    char*   lpszTarget;
    } SERVICE_CREATE_STRUCT;

typedef struct  tagSERVICE_DELETE_STRUCT
    {
    char*   lpszServiceName;
    BOOL    bStopService;
    } SERVICE_DELETE_STRUCT;



/////////////////////////////////////////////////////////////////////////////
//  Macro/Inline definitions
/////////////////////////////////////////////////////////////////////////////

LPSTR _inline GetString( UINT uId )
{
    extern  char        g_szStringTableString[];
    extern  HINSTANCE   g_hInst;

    *g_szStringTableString = 0;
    LoadString( g_hInst, uId, g_szStringTableString, SYM_MAX_PATH - 1 );

    return( *g_szStringTableString ? g_szStringTableString : NULL );
}


#endif  // NAVINST_HEADER
