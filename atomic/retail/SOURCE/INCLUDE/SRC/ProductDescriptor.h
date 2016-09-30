// Copyright (C) 1997 Symantec Corporation
//****************************************************************************
// $Header:   S:/INCLUDE/VCS/ProductDescriptor.h_v   1.6   05 Aug 1997 13:09:18   DSACKIN  $
//
// Description:
//
//****************************************************************************
// $Log:   S:/INCLUDE/VCS/ProductDescriptor.h_v  $
// 
//    Rev 1.6   05 Aug 1997 13:09:18   DSACKIN
// Added m_dwFlags and support function
//
//    Rev 1.5   31 Jul 1997 18:26:56   DSACKIN
// Added a default parameter to WriteToIniFile that says DON'T Create it.
//
//    Rev 1.4   16 Jun 1997 06:48:44   TSmith
// Replaced references to STL objects & types with MFC objects & types.
//
//    Rev 1.3   06 Jun 1997 12:20:12   TSmith
// Added member variable 'm_szResultFile' & inline access functions.
//
//    Rev 1.2   29 May 1997 19:17:34   TSmith
// Reworked copy constructor and operator= to call the base class version
// of those functions.
//
//    Rev 1.1   24 May 1997 10:47:32   TSmith
// Changed return type of WriteToIniFile from bool to void to accomodate changes
// in base class.
//
//    Rev 1.0   21 May 1997 12:54:22   TSmith
// Initial revision.
//****************************************************************************
#if !defined( __ProductDescriptor_h__ )
#define       __ProductDescriptor_h__

#include <afx.h>
#include "Descriptor.h"

//
// Do not translate the following string defines
//
#define INFVALUE_DEINSTALLCMD _T("DeinstallCmd")
#define INFVALUE_INSTALLCMD   _T("InstallCmd")
#define INFVALUE_OTHERCMD     _T("OtherCmd")
#define INFVALUE_FILEPATH     _T("FilePath")
#define INFVALUE_VERSION      _T("Version")
#define INFVALUE_RESULTFILE   _T("ResultFile")
#define INFVALUE_FLAGS        _T("Flags")

#define INFVALUE_FLAG_OPTIONS      (0x1)
#define INFVALUE_FLAG_DEFINITIONS  (0x2)

class CProductDescriptor : public CDescriptor
    {
protected:
    CString m_szVersion;
    CString m_szFilePath;
    CString m_szInstallCmd;
    CString m_szDeinstallCmd;
    CString m_szOtherCmd;
    CString m_szResultFile;

    DWORD m_dwFlags;

public:
    CProductDescriptor( ) { };
    ~CProductDescriptor( ) { };

    CProductDescriptor( const CProductDescriptor& pObject );
    CProductDescriptor& operator=( const CProductDescriptor& pRhs );

    void FillFromIniFile( LPCTSTR pszIniFile, LPCTSTR pszSection );
    void WriteToIniFile( LPCTSTR pszIniFile, LPCTSTR pszSection, BOOL bCreate = FALSE );

    void SetName( LPCTSTR pszName )
        { m_szName = pszName; }

    LPCTSTR GetName( )
        { return m_szName; }

    void SetVersion( LPCTSTR pszVersion )
        { m_szVersion = pszVersion; }

    LPCTSTR GetVersion( )
        { return m_szVersion; }

    void SetFilePath( LPCTSTR pszFilePath )
        { m_szFilePath = pszFilePath; }

    LPCTSTR GetFilePath( )
        { return m_szFilePath; }

    void SetInstallCmd( LPCTSTR pszCmd )
        { m_szInstallCmd = pszCmd; }

    LPCTSTR GetInstallCmd( )
        { return m_szInstallCmd; }

    void SetDeinstallCmd( LPCTSTR pszCmd )
        { m_szDeinstallCmd = pszCmd; }

    LPCTSTR GetDeinstallCmd( )
        { return m_szDeinstallCmd; }

    void SetOtherCmd( LPCTSTR pszCmd )
        { m_szOtherCmd = pszCmd; }

    LPCTSTR GetOtherCmd( )
        { return m_szOtherCmd; }

    void SetResultFile( LPCTSTR pszResultFile )
        { m_szResultFile = pszResultFile; }

    LPCTSTR GetResultFile( )
        { return m_szResultFile; }

    void SetFlags (DWORD dwFlags) { m_dwFlags = dwFlags; }
    DWORD GetFlags() { return m_dwFlags; }

    };

#endif // __ProductDescriptor_h__

