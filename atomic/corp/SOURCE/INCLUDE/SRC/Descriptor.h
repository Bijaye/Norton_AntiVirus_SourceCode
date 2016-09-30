// Copyright (C) 1997 Symantec Corporation
//****************************************************************************
// $Header:   S:/INCLUDE/VCS/Descriptor.h_v   1.8   31 Jul 1997 16:31:18   DSACKIN  $
//
// Description:
//
//****************************************************************************
// $Log:   S:/INCLUDE/VCS/Descriptor.h_v  $
// 
//    Rev 1.8   31 Jul 1997 16:31:18   DSACKIN
// Added a function to check for the existance of the INI file before
// we access it.  (This was creating bogus jobs for us).
//
//    Rev 1.7   22 Jul 1997 14:18:58   TSmith
// Added string variables to class to store the name of the ini file and
// file section that the descriptor came from.
//
//    Rev 1.6   16 Jun 1997 06:38:02   TSmith
// Replaced references to STL objects & types with MFC objects & types.
//
//    Rev 1.5   31 May 1997 17:19:06   TSmith
// Removed job status string defines (moved to JobFile.h)
//
//    Rev 1.4   31 May 1997 15:08:00   TSmith
// Defined four status strings for output to job description files.
//
//    Rev 1.3   29 May 1997 19:15:56   TSmith
// Added two string member variables and access functions.
//
//    Rev 1.2   25 May 1997 19:33:58   TSmith
// Increased JOBFILE_MAX_STRING_LENGTH from 1024 to 4096.
//
//    Rev 1.1   24 May 1997 10:07:44   TSmith
// Changed pure virtual function declarations return types from bool to void.
//
//    Rev 1.0   22 May 1997 07:06:28   TSmith
// Initial revision.
//****************************************************************************
#if !defined( __Descriptor_h__ )
#define       __Descriptor_h__

#include <afx.h>
#include <afxtempl.h>

//
// Following are the ini file section names
// common to all descriptor objects.
//
// *** DO NOT TRANSLATE ***
//
#define INFVALUE_NAME           TEXT( "Name"        )
#define INFVALUE_OS             TEXT( "OS"          )
#define INFVALUE_OSVER          TEXT( "OSVer"       )
#define INFVALUE_CPU            TEXT( "CPU"         )
#define INFVALUE_LANGUAGE       TEXT( "Language"    )
#define INFVALUE_STATUS         TEXT( "Status"      )
#define INFVALUE_STATUSTEXT     TEXT( "Status Text" )

class CDescriptor
    {
private:
    CMap<DWORD, DWORD, CString, LPCTSTR> m_StatusValToStr;
    CMap<CString, LPCTSTR, DWORD, DWORD> m_StatusStrToVal;
    void CreateStatusValueMaps( );

public:
    CDescriptor( );
    ~CDescriptor( ) { };

    CDescriptor( const CDescriptor& pObject );
    CDescriptor& operator=( const CDescriptor& pRhs );

    virtual void FillFromIniFile( LPCTSTR pszIniFile, LPCTSTR pszSection );
    virtual void WriteToIniFile( LPCTSTR pszIniFile, LPCTSTR pszSection );

    void SetName( LPCTSTR pszName ) { m_szName = pszName; }
    LPCTSTR GetName( ) { return m_szName; }

    void SetOs( LPCTSTR pszOs) { m_szOs = pszOs; }
    LPCTSTR GetOs( ) { return m_szOs; }

    void SetOsVer( LPCTSTR pszOsVer ) { m_szOsVer = pszOsVer; }
    LPCTSTR GetOsVer( ) { return m_szOsVer; }

    void SetCpu ( LPCTSTR pszCpu ) { m_szCpu = pszCpu; }
    LPCTSTR GetCpu( ) { return m_szCpu; }

    void SetLanguage( LPCTSTR pszLanguage ) { m_szLanguage = pszLanguage; }
    LPCTSTR GetLanguage( ) { return m_szLanguage; }

    void SetStatus( LPCTSTR pszStatus ) { m_szStatus = pszStatus; }
    LPCTSTR GetStatus( ) { return m_szStatus; }
    DWORD GetStatusValue( );

    void SetStatusText( LPCTSTR pszStatusText ) { m_szStatusText = pszStatusText; }
    LPCTSTR GetStatusText( ) { return m_szStatusText; }

protected:
    void GetIniValue(
        LPCTSTR pszIniFile,
        LPCTSTR pszSection,
        LPCTSTR pszValueName,
        LPCTSTR pszDefaultValue,
        CString& pszValue
        );

    BOOL PutIniValue(
        LPCTSTR pszIniFile,
        LPCTSTR pszSection,
        LPCTSTR pszValueName,
        LPCTSTR pszValue
        );

    CString m_szCpu;
    CString m_szLanguage;
    CString m_szName;
    CString m_szOs;
    CString m_szOsVer;
    CString m_szStatus;
    CString m_szStatusText;

public:
    CString m_szFileName;               // File that this descriptor came from.
    CString m_szSectionName;            // File section this descriptor came from.

private:
                                        // Utility function to check to see if
                                        // the INI file exists.
    BOOL DoesIniFileExist(
        LPCTSTR pszIniFile);
    };

#endif // __Descriptor_h__

