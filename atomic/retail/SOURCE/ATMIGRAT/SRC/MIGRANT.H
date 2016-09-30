/////////////////////////////////////////////////////////////////////
// File: Migrant.h
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/ATMIGRAT/VCS/migrant.h_v   1.3   12 Feb 1999 17:46:46   MBROWN  $
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/migrant.h_v  $
// 
//    Rev 1.3   12 Feb 1999 17:46:46   MBROWN
// Merged in Roswell migdll code and added version checking so that single migdll works for both Atomic and Roswell
// 
//    Rev 1.2   19 Nov 1998 13:14:34   MBROWN
// Changed method for getting shell folders, deleting NPS links, added entry point for unloading NAV
// 
//    Rev 1.1   28 Sep 1998 11:36:50   MBROWN
// Removed options modification, added write to Handled section of inf, modify shell links, move prog groups, etc.
// 
//    Rev 1.0   26 May 1998 14:36:12   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#ifndef MIGRANT_HEADER
#define MIGRANT_HEADER

#include    "CWString.h"        // Bruce's string class


// define statements
#define SYM_SHAREDUSAGE_KEY         "SOFTWARE\\Symantec\\SharedUsage"
#define MAX_MIGRATE_STRBUF_SIZE     2048
#define SYMEVENT_VXD_NAME           "symevnt.vxd"
#define SYMEVENT_DLL_NAME           "symevnt1.dll"
#define S32EVENT_DLL_NAME           "s32evnt1.dll"


// class definition
class   CMigrant
{
// methods
public:
    CMigrant(void);
    ~CMigrant(void);
    DWORD   CopyUpgradeDirs(LPCSTR  szWorkingDir, LPCSTR  szSourceDir);
    BOOL    Initialize();

    // access functions
    CHAR*   GetAppID(void);
    CHAR*   GetExeName(int  index);
    CHAR*   GetInstallDirFilename(int  index);
    CHAR*   GetCommonFileSymantecFilename(int  index);
    CHAR*   GetProgramFileSymantecFilename(int  index);
    UINT    GetDllVersion(void);
    BOOL    GetMigInfPath(void);
    BOOL    UpdateMigInfIncompatibleSection(DWORD  resID);
    BOOL    UpdateMigInfMessageObjectSection(LPSTR  szSectionName);
    BOOL    UpdateMigInfHandledSection(void);

    // virtual functions
    virtual BOOL    CheckAppInstallation(void) = 0;
    virtual BOOL    CopyAppFiles(void);
    virtual BOOL    InitAppDirs(void) = 0;
    virtual BOOL    InitAppID(void) = 0;
    virtual BOOL    InitNTRegistry(void) = 0;
    virtual BOOL    IsAppInstalled(void) = 0;
    virtual BOOL    WriteMigrationFlag(void) = 0;
    virtual BOOL    IsNavVersion4(void) = 0;
    virtual BOOL    ModifyShellLinks(TCHAR* szProgramsPath) = 0;
    virtual BOOL    MoveProgGroups(TCHAR* szProgramsPath, TCHAR* szStartUpPath) = 0;
    virtual BOOL    GetShellFolder(HKEY UserRegKey, TCHAR* lpFolder, TCHAR* lpOutput) = 0;

    // virtual MS "migrate" functions
//    virtual LONG    MigMigrateSystem9x(HWND  ParentWnd, LPCSTR  UnattendFile
//                                                        , LPVOID  Reserved) = 0;

// attributes



protected:
// methods

// attributes
    CWString        m_szAppInstallDir;
    CWString        m_szCommonFilesSymantecDir;
    CWString        m_szProgramFilesSymantecDir;
    CWString        m_szMigWorkingDir;
    CWString        m_szMigSourceDir;
    CWString        m_szMigrateInf;
    CWString        m_szProductID;
    UINT            m_dllVersion;


public:
    // static members; derived classes should initialize these
    static  CHAR*   m_szInstallDirFiles[];
    static  CHAR*   m_szProgramFilesSymantecFiles[];
    static  CHAR*   m_szCommonFilesSymantecFiles[];
    static  CHAR*   m_szMigInfExeFiles[];
    static  CHAR*   m_szSymantecSharedRegkeys[];
    static  INT     m_codePageArray[];


private:
// methods

// attributes

};


// inline function implementations
inline  CHAR*   CMigrant::GetExeName(int  index)
{
    return  m_szMigInfExeFiles[index];
}

inline  CHAR*   CMigrant::GetInstallDirFilename(int  index)
{
    return  m_szInstallDirFiles[index];
}

inline  CHAR*   CMigrant::GetCommonFileSymantecFilename(int  index)
{
    return  m_szCommonFilesSymantecFiles[index];
}

inline  CHAR*   CMigrant::GetProgramFileSymantecFilename(int  index)
{
    return  m_szProgramFilesSymantecFiles[index];
}

inline  CHAR*   CMigrant::GetAppID(void)
{
    return  (LPSTR)m_szProductID;
}

inline  UINT    CMigrant::GetDllVersion(void)
{
    return  m_dllVersion;
}


#endif  // MIGRANT_HEADER