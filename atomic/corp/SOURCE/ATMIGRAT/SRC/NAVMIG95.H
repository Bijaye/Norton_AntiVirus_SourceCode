/////////////////////////////////////////////////////////////////////
// File: NavMig95.h
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/ATMIGRAT/VCS/navmig95.h_v   1.4   12 Feb 1999 17:46:44   MBROWN  $
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/navmig95.h_v  $
// 
//    Rev 1.4   12 Feb 1999 17:46:44   MBROWN
// Merged in Roswell migdll code and added version checking so that single migdll works for both Atomic and Roswell
// 
//    Rev 1.3   19 Nov 1998 13:14:30   MBROWN
// Changed method for getting shell folders, deleting NPS links, added entry point for unloading NAV
// 
//    Rev 1.2   28 Sep 1998 11:36:52   MBROWN
// Removed options modification, added write to Handled section of inf, modify shell links, move prog groups, etc.
// 
//    Rev 1.1   08 Jun 1998 23:42:08   SEDWARD
// NAV_INTERNAL_VERSION changed to "5.3" for Atomic.
//
//    Rev 1.0   26 May 1998 14:36:18   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////



#ifndef NAV95_MIGRANT_HEADER
#define NAV95_MIGRANT_HEADER

#ifndef SYM_WIN
#define SYM_WIN
#endif

#ifndef SYM_WIN32
#define SYM_WIN32
#endif

#include    "Migrant.h"
#include    "options.h"
#include    "navbnhdr.h"
#include    "ApComm.h"


// define statements

// these code page IDs represent all the languages NAV currently ships in:
//  CP_932  -- japanese
//  CP_936  -- chinese (simplified)
//  CP_949  -- korean
//  CP_950  -- chinese (traditional)
//  CP_1250 -- czech, polish
//  CP_1251 -- russian
//  CP_1252 -- german, italian, norwegian, portugese, danish, spanish, swedish, french, english, dutch
//  CP_1255 -- hungarian
#define CP_932                              932
#define CP_936                              936
#define CP_949                              949
#define CP_950                              950
#define CP_1250                             1250
#define CP_1251                             1251
#define CP_1252                             1252
#define CP_1255                             1255
#define END_OF_CODEPAGES                    -1

#define INSTALLATION_ROSWELL                5               // Used to indicate installed version is NAV4 (Roswell)
#define INSTALLATION_ATOMIC                 53              // Used to indicate installed version is NAV5 (Atomic)
#define MIGAPP_EXE                          "MigApp.exe"
#define NAV_DLL_VERSION                     501             // Returned as MigDLL version to QueryVersion
#define NAV4_INTERNAL_VERSION               "5.0"           // Used to build reg key ...Norton AntiVirus\Install\5.0
#define NAV5_INTERNAL_VERSION               "5.3"           // Used to build reg key ...Norton AntiVirus\Install\5.3
#define NAV95_EXE_NAME                      "navw32.exe"
#define NAV95_INST_DLL                      "NAVInst95.dll"
#define NAVAPW_EXE_NAME                     "navapw32.exe"
#define NAVDX_EXE_NAME                      "navdx.exe"
#define NAVLU32_EXE_NAME                    "navlu32.exe"
#define NSCHED32_EXE_NAME                   "nsched32.exe"
#define NSPLUGIN_EXE_NAME                   "nsplugin.exe"
#define RESCUE_EXE_NAME                     "nresq32.exe"


class   CNavMigrant95 : public  CMigrant
{
public:
    CNavMigrant95(void);
    ~CNavMigrant95(void);

    // virtual functions
    virtual BOOL    CheckAppInstallation(void);
    virtual BOOL    CopyAppFiles(void);
    virtual BOOL    InitAppDirs(void);
    virtual BOOL    InitAppID(void);
    virtual BOOL    InitNTRegistry(void);
    virtual BOOL    IsAppInstalled(void);
    virtual BOOL    WriteMigrationFlag(void);
    virtual BOOL    IsNavVersion4(void);
    virtual BOOL    ModifyShellLinks(TCHAR* szProgramsPath);
    virtual BOOL    MoveProgGroups(TCHAR* szProgramsPath, TCHAR* szStartUpPath);
    virtual BOOL    GetShellFolder(HKEY UserRegKey, TCHAR* lpFolder, TCHAR* lpOutput);

    // virtual MS "migrate" functions
//    virtual LONG    MigMigrateSystem9x(HWND  ParentWnd, LPCSTR  UnattendFile
//                                                        , LPVOID  Reserved);


protected:
//    BOOL    MigReadNavBinFromOpenedFile(HANDLE  hFile, LPNAVOPTIONS  lpNavOptions);
//    BOOL    MigWriteNavBinFromOpenedFile(HANDLE  hFile, LPNAVOPTIONS  lpNavOptions);
//    BOOL    UpdateNavoptsFile(void);
    void    MoveFolder(TCHAR* szSrcDir, TCHAR* szDestDir, BOOL bRemoveDir);
    void    RemoveFolder(TCHAR* szDir);





private:
    int     m_versionNav;

};


inline  BOOL    CNavMigrant95::IsAppInstalled(void)
{
    return  (-1 != m_versionNav);
}

inline  BOOL    CNavMigrant95::IsNavVersion4(void)
{
    return  (m_versionNav == INSTALLATION_ROSWELL);
}

#endif  // NAV95_MIGRANT_HEADER