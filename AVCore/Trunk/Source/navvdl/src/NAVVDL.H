// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// Description:
//
//      This file defines the Norton AntiVirus Virus Definition
//      Library functions.
//
//************************************************************************

#ifndef __NAVDEFS_H
#define __NAVDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SYM_UNIX
  #pragma pack(push,1)
#endif

//***************************************************************************
// Typedefs
//***************************************************************************
#ifdef SYM_UNIX
  typedef int   HVIRUSDEF;
#else
  typedef void *HVIRUSDEF;
#endif

typedef HVIRUSDEF *LPHVIRUSDEF;
typedef struct tag_MICRODEFINFO   *LPMICRODEFINFO;
typedef struct tag_MICRODEFUPDATE *LPMICRODEFUPDATE;


//***************************************************************************
// Enumerated Types
//***************************************************************************
 enum VirusDefUpdateType {              // Virus definition update type
      VirusDefInstallScript,            // Updated by Install
      VirusDefLiveUpdate,               // Updated by Symantec LiveUpdate
      VirusDefAVUpdater,                // Updated by Symantec Updater utility
      VirusDefOther};                   // Other


//***************************************************************************
// Defines
//***************************************************************************
#ifdef SYM_WIN32
  #define DllExport   __declspec( dllexport )
#else
  #define DllExport
#endif



#define VIRDEF_OK                   0
#define VIRDEF_NO_APPID_CREATED     1
#define VIRDEF_PREUPDATE_FAILED     2
#define VIRDEF_POSTUPDATE_FAILED    3
#define VIRDEF_INIT_INSTALL_ERROR   4
#define VIRDEF_CANT_USE_NEWEST_DEFS 5
#define VIRDEF_ERROR_COPYING_FILES  6



//***************************************************************************
// Simplified Install/Uninstall Functions
//***************************************************************************

DWORD DllExport WINAPI VirusDefInstall (// Install virus definitions
      HVIRUSDEF hVirusDef,		        // [in]  handle
      LPSTR     pszAppID,               // [out] buffer to receive the AppID string
      int       nMaxCount,              // [in]  size of the buffer
      LPCSTR    pszAppIDPrefix,         // [in]  AppID prefix
      LPCSTR    pszDefSourceDir);       // [in]  Directory to find virus definitions

BOOL  DllExport WINAPI VirusDefUninstall ( // Uninstall virus definitions
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPSTR     pszAppID);              // [in]  buffer containing the AppID string


//***************************************************************************
// Virus Definition Utility Functions
//***************************************************************************

BOOL  DllExport WINAPI VirusDefAreNewest( // Check if the virus defintions are the newest.
      HVIRUSDEF hVirusDef);             // [in]  handle

VOID  DllExport WINAPI VirusDefClose (  // Close he Virus Definition Library
      HVIRUSDEF hVirusDef);             // [in]  handle

BOOL  DllExport WINAPI VirusDefGetCurrent( // Get the current virus definitions
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPSTR     pszDir,                 // [out] buffer to receive the current def dir.
      int       nMaxCount);             // [in]  size of buffer

BOOL  DllExport WINAPI VirusDefGetCurrentDate ( // Get the date of the current virus definitions
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPWORD    pwYear,                 // [out] buffer to receive the year
      LPWORD    pwMonth,                // [out] buffer to receive the month
      LPWORD    pwDay,                  // [out] buffer to receive the day
      LPDWORD   pdwRev);                // [out] buffer to receive the revision number

BOOL  DllExport WINAPI VirusDefGetNewestDate ( // Get the date of the newest virus definitions
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPWORD    pwYear,                 // [out] buffer to receive the year
      LPWORD    pwMonth,                // [out] buffer to receive the month
      LPWORD    pwDay,                  // [out] buffer to receive the day
      LPDWORD   pdwRev);                // [out] buffer to receive the revision number

BOOL  DllExport WINAPI VirusDefGetUniqueAppID ( // Get a unique Application ID string
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPCSTR    pszAppIDPrefix,         // [in]  buffer containing a prefix for the AppID
      LPSTR     pszAppID,               // [out] buffer to receive the generated AppID string.
      int       nMaxCount);             // [in]  size of buffer

BOOL  DllExport WINAPI VirusDefInit (   // Initialize the Virus Definition Library
      LPHVIRUSDEF  phVirusDef);

BOOL  DllExport WINAPI VirusDefInitInstallApp (  // Register the application during install
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPCSTR    pszAppID,               // [in]  application ID string
      LPCSTR    pszAppInstallDir,       // [in]  not used in Win32
      LPCSTR    pszAppDefsDir);         // [in]  shared defs directory, if not default

BOOL  DllExport WINAPI VirusDefInitWindowsApp (  // Register the application
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPCSTR    pszAppID);              // [in]  Application ID string

BOOL  DllExport WINAPI VirusDefPostMicroUpdate ( // Move new defintions to a shareable directory
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPMICRODEFINFO lpMicroDefInfo);   // [in]  info from VirusDefPreMicroUpdate

BOOL  DllExport WINAPI VirusDefPreMicroUpdate ( // Lock the virus defs for exclusive update
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPMICRODEFINFO lpMicroDefInfo,    // [in]  info from VirusDefPreMicroUpdate
      VirusDefUpdateType type);         // [in]  type of update to do. See UpdateType above

BOOL  DllExport WINAPI VirusDefPostUpdate ( // Move new defintions to a shareable directory
      HVIRUSDEF hVirusDef);             // [in]  handle

BOOL  DllExport WINAPI VirusDefPreUpdate ( // Lock the virus defs for exclusive update
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPSTR     pszDir,                 // [out] buffer to receive dir to update
      int       nMaxCount,              // [in]  size of the buffer
      VirusDefUpdateType type);         // [in]  type of update to do. See UpdateType above

BOOL  DllExport WINAPI VirusDefSetCurrent ( // Set the current virus defintions dir.
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPCSTR    pszDir);                // [in]  full path to the shareable def dir.

BOOL  DllExport WINAPI VirusDefStopUsingDefs (    // Unregister as a def user at app uninstall.
      HVIRUSDEF hVirusDef);	            // [in]  handle

BOOL  DllExport WINAPI VirusDefUseNewest (        // Use the newest virus definitions
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPSTR     pszDir,                 // [out] buffer to receive the directory
      int       nMaxCount,              // [in]  size of the buffer
      LPBOOL    pbChanged);             // [out] buffer to set to TRUE if the app
                                        //       switched to new defs.

BOOL  DllExport WINAPI VirusDefCopyDefs ( // Copy defs from Source dir to Dest dir
      HVIRUSDEF hVirusDef,              // [in]  handle
      LPCSTR    pszSource,              // [in]  source directory
      LPCSTR    pszDest);               // [in]  destination directory

#ifdef __cplusplus
}
#endif

#ifndef SYM_UNIX
#pragma pack(pop)
#endif

#endif // __NAVDEF_H

