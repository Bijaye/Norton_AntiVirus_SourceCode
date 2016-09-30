/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/NAVINST.h_v   1.40   26 Oct 1998 12:40:16   mdunn  $
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/NAVINST.h_v  $
// 
//    Rev 1.40   26 Oct 1998 12:40:16   mdunn
// Added GetFileVersionString()
// 
//    Rev 1.39   12 Oct 1998 18:07:42   mdunn
// Fixed NAV_INSTALL_KEY to point to 5.31 instead of 5.3.  This _really_ 
// ought to be read in from version.dat....
// 
//    Rev 1.38   14 Aug 1998 10:40:22   mdunn
// Added SetUpNAVDXTrialInfo()
// 
//    Rev 1.37   17 Jun 1998 23:58:12   tcashin
// Changed the parameter for CopyVirusDefinitionFiles from void to LPSTR to
// pass in the temp directory.
// 
//    Rev 1.36   17 Jun 1998 11:46:16   mdunn
// Fixed NAV_INSTALL_KEY to point to 5.3 instead of 5.0
// 
//    Rev 1.35   16 Jun 1998 17:39:58   mdunn
// Added RemoveQConsole().
// 
//    Rev 1.34   10 Jun 1998 18:15:30   jbelden
// Ported code form Roswell project for the shellbrowse function.
// 
//    Rev 1.33   10 Jun 1998 15:52:40   mdunn
// Added CreateInternetShortcut()
// 
//    Rev 1.32   05 Jun 1998 22:44:10   SEDWARD
// Added more support for the Atomic migration (Win95 to NT5).
//
//    Rev 1.31   03 Jun 1998 16:53:04   mdunn
// Added strings for the Quarantine and corp features reg entries.
//
//    Rev 1.30   01 Jun 1998 20:39:02   mdunn
// Added ability to clean out Quarantine during uninstall.
// Removed references to Extensions directory and reg entry since that dir
// doesn't even exist any more.
//
//    Rev 1.29   07 May 1998 20:40:44   mdunn
// Added SetInocDirRegEntry()
//
//    Rev 1.28   17 Apr 1998 16:01:06   RStanev
// SmellyCat changes.
//
//    Rev 1.27   06 Apr 1998 22:49:50   mdunn
// Added NTASKMGR_EXE constant.
//
//    Rev 1.26   26 Mar 1998 20:28:28   mdunn
// Added CreateNAVMutex() and ReleaseNAVMutex(), used to prevent more than
// one instance of install from running.
//
//    Rev 1.25   05 Mar 1998 10:45:58   mdunn
// Added CreateProcessAndWait()
//
//    Rev 1.24   02 Mar 1998 14:03:18   mdunn
// Added UpdateEastEuropeanAutoexec() to correctly place the NAVDX startup
// scan command in AUTOEXEC on systems that have MODE CON CODEPAGE and
// KEYB commands in AUTOEXEC.
//
//    Rev 1.23   23 Feb 1998 18:41:04   SEDWARD
// Rolled back changes in 1.22.
//
//    Rev 1.21   17 Feb 1998 14:53:22   mdunn
// Ported NEC code from the ROSNEC area.
//
//    Rev 1.20   30 Dec 1997 19:01:14   SEDWARD
// Added support for the Win95-NT5 migration.
//
//    Rev 1.19   17 Dec 1997 21:57:44   jtaylor
// Added IsOemPersonalityDll function to check for oem version during install.
//
//    Rev 1.18   19 Aug 1997 21:50:54   jtaylor
// -- Added the registry backup function that Dave B. created.
//
//    Rev 1.17   12 Aug 1997 20:40:32   jtaylor
// -- Made the code to cleanup Haking exported.
//
//
//    Rev 1.16   11 Aug 1997 22:27:42   jtaylor
// -- Added declaration + implementation of StringContainsDBCSChars.
//
//    Rev 1.15   17 Jul 1997 21:04:18   jtaylor
// -- Added a global variable to hold the Symantec Shared directory.
// -- Added a call to call the symevent installer to uninstall symevent.
// -- Added a call to try and delete the prog files\comm files\symtantec shared
//    directory.
// -- Modified all calls and definitions of registry functions that stored the
//    paths for uninstall to include the symantec shared directory.
//
//    Rev 1.14   14 Jul 1997 23:07:58   JALLEE
// Exported new function RemoveSched(). Shuts down the NAV Program Scheduler.
//
//    Rev 1.13   10 Jul 1997 21:19:14   jtaylor
// -- Added export prototype for MyCharToOem
//
//    Rev 1.12   09 Jul 1997 09:58:20   jtaylor
// -- Modified install key to use version 5
// -- Exported two new functions
//    GetBootDir, and RemoveLine
//
//    Rev 1.11   01 Jul 1997 23:52:38   jtaylor
// -- Added the NAV_EXE define statement.
//
//    Rev 1.10   01 Jul 1997 22:42:38   jtaylor
// -- Updated to unicode.
// -- Added some registrykey string define statements.
// -- Update the function definitions
// -- Removed the old hardcoded appIDs for hawking.
//
//    Rev 1.9   19 Jun 1997 18:18:50   jtaylor
// -- Changed NAVNT_ID to NAVID and updated it to NAV95
// -- Changed the log filename to _isnav95.ulg
//
//    Rev 1.8   10 Jun 1997 21:31:26   jtaylor
// Removed the service cleanup calls.
// Updated the location of the install reg key.
//
//
//    Rev 1.7   10 Jun 1997 12:58:42   jtaylor
// Removed NT service code
//
//
//    Rev 1.6   08 Jun 1997 20:59:32   jtaylor
// Updated the virus definition usage and removal calls for Hawking.
//
//    Rev 1.5   02 Jun 1997 01:51:50   jtaylor
// Added Hawking support.
//
//    Rev 1.4   01 Jun 1997 19:43:54   jtaylor
// Removed memory only scan.  It will be placed in its own .dll.
//
//    Rev 1.3   01 Jun 1997 16:50:36   jtaylor
// Added pass-through function to CharToOem.
//
//    Rev 1.2   30 May 1997 18:42:24   jtaylor
// Added memory only virus scanning function.
//
//    Rev 1.1   27 May 1997 21:18:42   JTAYLOR
// Pulled update from navinstnt.
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

// #define NAVWNT
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

#define SIZE_VIRUSNAME      (21 + 1)        // This does NOT include EOS!



///////////////////////////////////////////////////////////////////////////////
//  General defines
///////////////////////////////////////////////////////////////////////////////

#define IS_FILE_DELETED                         0x00000000
#define IS_FILE_IN_USE                          0x00000003
#define IS_FILE_NOT_FOUND                       0x00000001
#define IS_DIR_NOT_EMPTY                        IS_FILE_IN_USE
#define IS_DIR_NOT_FOUND                        IS_FILE_NOT_FOUND
#define IS_DIR_REMOVED                          IS_FILE_DELETED

#define NSPLUGIN_EXE                            "nsplugin.exe"
#define SEVINST_EXE                             "sevinst.exe"
#define NTASKMGR_EXE                            "ntaskmgr.exe"
#define DEFANNTY_DLL                            "DefAnnty.dll"
#define DEFANNTY_UNINITIALIZE_PROC              "_DefAnnuityUninitialize@4"
#define NAVAP_SERVICE_INDEX                     4
#define NAVSTART_OPTION_SCAN_DOS_FILES          500
#define NAVSTART_OPTION_SCAN_WINDOWS_FILES      501
#define NAVSTART_OPTION_LOAD_TSR                502
#define NAVSTART_OPTION_LOAD_AP_AT_STARTUP      503
#define NAVSTART_OPTION_PROTECT_FEATURE         504
#define LOGFILE_NAME                            "_ISNAV95.ULG"
#define NAV_ID                                  "NAV95"
#define NPROTECT_SERVICE_NAME                   "NProtectService"
#define NUMBER_OF_SERVICES                      6
#define SI_MAXSTR                               1024


///////////////////////////////////////////////////////////////////////////////
//  Registry Keys
///////////////////////////////////////////////////////////////////////////////

// registry keys

#define MS_DLLS_KEY                 "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDlls"
#define MS_HELP_KEY                 "SOFTWARE\\Microsoft\\Windows\\Help"
#define MS_SHELL_FOLDERS_KEY        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
#define SYM_SHAREDUSAGE_KEY         "SOFTWARE\\Symantec\\SharedUsage"
#define SYM_INSTALLEDAPPS_KEY       "SOFTWARE\\Symantec\\InstalledApps"
#define NAV_INSTALL_KEY             "Software\\Symantec\\Norton AntiVirus\\Install\\5.31"
#define NAV_INSTALLDIR              "InstallDir"
#define NAV_EXE                     "navw32.exe"
#define NAV_KEY                     "SOFTWARE\\Symantec\\Norton AntiVirus"
#define QUAR_KEY                            "software\\Symantec\\Norton AntiVirus\\Quarantine"
#define QUAR_TEMP_PATH_VALUE                "TempPath"
#define CORP_FEATURES_KEY                   "software\\Symantec\\Norton AntiVirus"
#define CORP_FEATURES_VALUE                 "CorporateFeatures"
//#define NAVSCAN_INSTALLKEY          "SOFTWARE\\Symantec\\Norton AntiVirus Scanner\\Install\\4.0"
//#define IRPSTACK_KEY                "SYSTEM\\CurrentControlSet\\Services\\LanmanServer\\Parameters"
#define SERVICES_REGKEY_PATH        "SYSTEM\\CurrentControlSet\\Services"
#define TEMP_REGKEY_PATH            "SOFTWARE\\Symantec\\Temp"
#define TEMP_REGVAL_NAV_PATH        "NavPath"
#define TEMP_REGVAL_SYMANTEC_PATH   "Symantec"
#define TEMP_REGVAL_SYMANTEC_SHARED_PATH   "SymantecShared"
#define KEY_LIVE_UPDATE_DLL         "s32live1.dll"
#define STR_COMMON_PROGRAMS         "Common Programs"
#define STR_COMMON_STARTUP          "Common Startup"






/////////////////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
extern "C"  {
#endif

extern  BOOL            g_RunSchedule;
extern  TCHAR           g_szTarget[SYM_MAX_PATH];
extern  TCHAR           g_szExtensions[SYM_MAX_PATH];
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

BOOL    CleanupLiveUpdate(LPTSTR  lpSymantecPath);
BOOL    CleanupMigratedInstallation(void);
BOOL    CleanupMigratedMicrosoftSharedDLLsKeys(void);
VOID    CleanupMigratedStartMenuLinks(void);
BOOL    CleanupMigratedSymantecSharedUsageKeys(void);
BOOL    CleanupMigratedSymEventFiles(void);
BOOL    CleanupMigratedRegistry(void);
BOOL    CleanUpOnRamp();
void    CleanupSharedSymantecRegEntries(void);
BOOL    CreateMifFile(LPTSTR  lpStatus, LPTSTR  lpMessage);
BOOL    DecrementIRPStackSize(void);
DWORD   DeleteNavStartMenuTargets(TCHAR**  files, TCHAR**  folders
                                                        , TCHAR*  valueName
                                                        , BOOL  bUseFolders);
BOOL    GetNavPaths(LPTSTR lpNavPath, LPTSTR lpSharedPath,
                    LPTSTR lpSymSharedPath);
BOOL    IsMigratedUninstall(void);
BOOL    NavUninstallSucceeded(void);
int     RemoveLeftoverDataFiles(void);
BOOL    ReadNavPaths(LPTSTR  navPathStr, LPTSTR  symantecPathStr,
                     LPTSTR SymSharedPathStr);
int     RemoveTargetFiles(TCHAR*  pszTargetDir, TCHAR**  filenameArray
                                                            , BOOL  bRemoveDir);
BOOL    ValidFileTarget(DWORD  fileAttributes);
BOOL    WriteNavPaths(LPSTR  navPathStr, LPSTR  symantecPathStr,
                      LPSTR symantecSharedPathStr);
BOOL    WriteNavstartBinData(LPCTSTR  lpszFilename, LPNAVSTARTBINDATA  lpOptions);



///////////////////////////////////////////////////////////////////////////////
//  Function Prototypes (exported)
///////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
extern "C"  {
#endif

DllExport   BOOL    BackupRegistry(void);
DllExport   BOOL    CopyVirusDefinitionFiles(LPSTR lpTempUpdateDir);
DllExport   void    GetBootDir(LPTSTR lpDrive, int size);
DllExport   BOOL    GetDefsDirectory(LPTSTR  pszAppInstallDir, LPTSTR  pszDefsDir);
DllExport   int     GetNavStartInfo(LPCTSTR  filename, int  targetInfo);
DllExport   BOOL    GetFileVersionString ( LPCTSTR, LPTSTR );
DllExport   BOOL    GetUsageCount(LPTSTR  lpszItemName, DWORD&  dwCurrentUsageCount);
DllExport   BOOL    IsOEMPersonalityDLL(LPTSTR lpszFullDllName);
DllExport   BOOL    MyCharToOem(LPCSTR  szSource, LPSTR szDest );
DllExport   BOOL    RemoveAPUI(void);
DllExport   BOOL    RemoveSched(void);
DllExport   BOOL    RemoveScheduledScans(void);
DllExport   BOOL    RemoveLine(LPCTSTR filename, LPCTSTR line);
DllExport   BOOL    SetInocDirRegEntry ( LPCTSTR );
DllExport   BOOL    SetNavStartInfo(LPCTSTR  filename, int  targetInfo, BOOL  bValue);
DllExport   BOOL    SetUpNAVDXTrialInfo ( LPTSTR szNAVDir, int nTrialLength );
DllExport   BOOL    ShellBrowseFolder( HWND hWndParent, LPTSTR pszPrompt, LPTSTR pszDir );
DllExport   BOOL    StringContainsDBCSChars ( LPTSTR lpszString );
DllExport   BOOL    UninstallVirusDefinitions(void);
DllExport   LONG    UninstInitialize(HWND hwndDlg, HANDLE hInstance, LONG lRes);
DllExport   LONG    UninstUnInitialize(HWND hwndDlg, HANDLE hInstance, LONG lRes);
DllExport   BOOL    IsMachineNEC();
DllExport   BOOL    FixUpNECConfigSys ( LPCTSTR szRemark );
DllExport   BOOL    UpdateEastEuropeanAutoexec ( LPCTSTR, LPCTSTR );
DllExport   BOOL    CreateProcessAndWait ( LPCTSTR, LPINT );
DllExport   BOOL    CreateNAVMutex ( LPCTSTR );
DllExport   BOOL    ReleaseNAVMutex();
DllExport   BOOL    CreateInternetShortcut ( LPCTSTR szPath, LPCTSTR szURL );
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
