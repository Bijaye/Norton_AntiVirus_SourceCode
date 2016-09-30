////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVInfo.cpp: implementation of the CNAVInfo class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#ifdef UNICODE
const char* szFuncName = "SHGetFolderPathW";
#else
const char* szFuncName = "SHGetFolderPathA";
#endif

#include <shlobj.h>
#include "tchar.h"
#include "NAVInfo.h"

// Location of the NAV Pro key
//
const TCHAR cstrNAVKey[] = _T("Software\\Symantec\\Norton AntiVirus");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVInfo::CNAVInfo() 
{
    // Init vars
    //
    *m_szNAVDir       =
        *m_szNAVOptPath   =
        *m_szNAVUserDir   =
        *m_szNAVCommonDir =
        *m_szAllUsersAppsDir  =
        *m_szSymantecCommonDir = 
		*m_szOPCDir			   =_T('\0');
    m_pfnSHGetFolderPath = NULL;
    m_hSHFOLDER = NULL;
}

CNAVInfo::~CNAVInfo()
{
    if ( m_hSHFOLDER )
        FreeLibrary( m_hSHFOLDER );
    m_hSHFOLDER = NULL;
    m_pfnSHGetFolderPath = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// @function CNAVInfo::GetNAVDir
//
// Get the full path to the folder where NAV is installed.
//
// e.g. "c:\program files\norton antivirus"
//
// @return Pointer to the path buffer. NAVInfo stores the data.

const TCHAR* CNAVInfo::GetNAVDir()
{
    if ( m_szNAVDir[0] == '\0' )
        InitGetNAVDir();

    return m_szNAVDir;
}

/////////////////////////////////////////////////////////////////////////////
// @function CNAVInfo::GetSymantecCommonDir
//
// Get the full path to the folder for Common Files\Symantec Shared
//
// e.g. "c:\program files\common files\symantec shared"
//
// @return Pointer to the path buffer. NAVInfo stores the data.

const TCHAR* CNAVInfo::GetSymantecCommonDir()
{
    if ( m_szSymantecCommonDir[0] == '\0' )
        InitGetSymantecCommonDir ();

    return m_szSymantecCommonDir;
}

/////////////////////////////////////////////////////////////////////////////
// @function CNAVInfo::GetNAVCommonDir
//
// Get the full path to the folder for Common "Application Data" area
//
// e.g. "C:\Documents and Settings\All Users\Application Data\Symantec\Norton AntiVirus"
//
// @return Pointer to the path buffer. NAVInfo stores the data.

const TCHAR* CNAVInfo::GetNAVCommonDir()
{
    if ( m_szNAVCommonDir[0] == '\0' )
        InitGetNAVCommonDir ();

    return m_szNAVCommonDir;
}

/////////////////////////////////////////////////////////////////////////////
// @function CNAVInfo::GetNAVOptPath
//
// Get the full path to the folder where the main options file is installed.
//
// e.g. "c:\program files\norton antivirus\navopts.dat"
//
// @return Pointer to the path buffer or NULL if unsuccessful. NAVInfo stores the data.

const TCHAR* CNAVInfo::GetNAVOptPath()
{
    if ( m_szNAVOptPath[0] == '\0' )
        InitGetNAVOptPath ();

    return m_szNAVOptPath;
}

/////////////////////////////////////////////////////////////////////////////
// @function CNAVInfo::GetNAVUserDir
//
// Get the full path to the folder for user "Application Data" area
//
// e.g. "C:\Documents and Settings\user name\Application Data\Symantec\Norton AntiVirus"
//
// @return Pointer to the path buffer. NAVInfo stores the data.

const TCHAR* CNAVInfo::GetNAVUserDir()
{
    if ( m_szNAVUserDir[0] == '\0' )
        InitGetNAVUserDir();

    return m_szNAVUserDir;
}

bool CNAVInfo::InitGetNAVDir()
{
    HRESULT hr;
    HKEY  hkey;
    DWORD dwType, dwBuffSize = MAX_PATH * sizeof(TCHAR);

    // Read the NAV install directory.  
    //
    if ( ERROR_SUCCESS ==
        (hr = ::RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
        _T("software\\symantec\\InstalledApps"),
        0, KEY_QUERY_VALUE, &hkey )) )
    {
        if ( ERROR_SUCCESS ==
            (hr = ::RegQueryValueEx ( hkey, _T("NAV"), NULL, &dwType,
            LPBYTE(m_szNAVDir), &dwBuffSize )) )
        {
            if ( !*m_szNAVDir )
            {
                // Defect 1-3FST6U
                ::RegCloseKey ( hkey );
                return false;
            }

            // Get the second to last char in the path.
            //
            TCHAR* pszDirNull = _tcsrchr ( m_szNAVDir, '\0');

            TCHAR* pszLastSlash = CharPrev( m_szNAVDir, pszDirNull );

            // If the ending character is a slash, null it.
            //            
            if ( 0 == _tcscmp ( pszLastSlash, _T("\\")))
            {
                *pszLastSlash = '\0';
            }
        }

        ::RegCloseKey ( hkey );
    }

    return ERROR_SUCCESS == hr;
}

bool CNAVInfo::InitGetSymantecCommonDir()
{
    if ( !m_hSHFOLDER && !InitSHGetFolderPath ())
        return false;

    bool bSuccess = (S_OK == m_pfnSHGetFolderPath( ::GetDesktopWindow()
        , CSIDL_PROGRAM_FILES_COMMON
        , NULL
        , SHGFP_TYPE_CURRENT
        , m_szSymantecCommonDir ));
    if (bSuccess)
    {
        ::_tcscat(m_szSymantecCommonDir, _T("\\Symantec Shared"));
    }

    return bSuccess;
}

bool CNAVInfo::InitGetNAVCommonDir()
{
    if ( !m_hSHFOLDER && !InitSHGetFolderPath ())
        return false;

    bool bSuccess = (S_OK == m_pfnSHGetFolderPath( ::GetDesktopWindow()
        , CSIDL_COMMON_APPDATA
        , NULL
        , SHGFP_TYPE_CURRENT
        , m_szNAVCommonDir ));

    if (bSuccess)
    {
        ::_tcscat(m_szNAVCommonDir, _T("\\Symantec\\Norton AntiVirus"));
    }

    return bSuccess;
}

bool CNAVInfo::InitGetNAVUserDir()
{
    if ( !m_hSHFOLDER && !InitSHGetFolderPath ())
        return false;

    bool bSuccess = (S_OK == m_pfnSHGetFolderPath ( ::GetDesktopWindow()
        , CSIDL_APPDATA
        , NULL
        , SHGFP_TYPE_CURRENT
        , m_szNAVUserDir ));

    if (bSuccess)
    {
        ::_tcscat(m_szNAVUserDir, _T("\\Symantec\\Norton AntiVirus"));
    }

    return bSuccess;
}

bool CNAVInfo::InitGetNAVOptPath()
{
    bool bSuccess = InitGetNAVDir();

    if (bSuccess)
    {
        ::_tcscpy(m_szNAVOptPath, GetNAVDir());
        ::_tcscat(m_szNAVOptPath, _T("\\NAVOPTS.DAT"));
    }

    return bSuccess;
}

//
// GetSHGetFolderPath()
// Loads SHfolder.dll and gets the the address of SHGetFolderPath()
// Calling SHGetFolderPath is Microsoft's recommended way of determining
// some of the folder paths
//
bool CNAVInfo::InitSHGetFolderPath()
{
    if ( !m_hSHFOLDER )
    {
        m_hSHFOLDER = LoadLibraryEx(_T("SHFOLDER.DLL"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

        if (m_hSHFOLDER == NULL)
            return false;

        m_pfnSHGetFolderPath = (PFNSHGETFOLDERPATH) GetProcAddress(m_hSHFOLDER, szFuncName);

        if ( !m_pfnSHGetFolderPath )
        {
            FreeLibrary( m_hSHFOLDER );
            m_hSHFOLDER = NULL;
            return false;
        }
    }

    return true;
}
/////////////////////////////////////////////////////////////////////////////
// @function CNAVInfo::MakeNAVUserDir
//
// Create the NAV User directory.
//
// e.g. "C:\Documents and Settings\user name\Application Data\Symantec\Norton AntiVirus"
//
// @return true - Directory now exists, false - error creating

bool CNAVInfo::MakeNAVUserDir()
{
    if ( !m_hSHFOLDER && !InitSHGetFolderPath ())
        return false;

    TCHAR szTempNAVUserDir [MAX_PATH] = {0};

    bool bSuccess = (S_OK == m_pfnSHGetFolderPath ( ::GetDesktopWindow()
        , CSIDL_APPDATA
        , NULL
        , SHGFP_TYPE_CURRENT
        , szTempNAVUserDir ));

    if (bSuccess)
    {
        // CreateDirectory return false if the directory already exists
        // so we don't want to exit.
        //
        ::_tcscat(szTempNAVUserDir, _T("\\Symantec"));

        ::CreateDirectory ( szTempNAVUserDir, NULL );

        ::_tcscat(szTempNAVUserDir, _T("\\Norton AntiVirus"));

        BOOL bResult = ::CreateDirectory ( szTempNAVUserDir, NULL );

        // Treat a final directory already existing as success.
        //
        if ( bResult  || ( !bResult && ::GetLastError () == ERROR_ALREADY_EXISTS ))
            bSuccess = true;
    }

    return bSuccess;
}

bool CNAVInfo::InitGetAllUsersAppsDir()
{
    if ( !m_hSHFOLDER && !InitSHGetFolderPath ())
        return false;

    bool bSuccess;

    bSuccess = (S_OK == m_pfnSHGetFolderPath( ::GetDesktopWindow()
        , CSIDL_COMMON_APPDATA
        , NULL
        , SHGFP_TYPE_CURRENT
        , m_szAllUsersAppsDir ));

    return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
// @function CNAVInfo::GetAllUsersAppsDir
//
// Get the full path to the folder for Common "Application Data" area without
// NAV specific data.
//
// e.g. "C:\Documents and Settings\All Users\Application Data\"
//
// @return Pointer to the path buffer. NAVInfo stores the data.

const TCHAR* CNAVInfo::GetAllUsersAppsDir()
{
    if ( m_szAllUsersAppsDir[0] == '\0' )
        InitGetAllUsersAppsDir();

    return m_szAllUsersAppsDir;
}

///////////////////////////////////////////////////////////////
//
// Determines if this is a NAV Pro install
//
bool CNAVInfo::IsNAVProfessional ()
{
    LONG lResult;
    HKEY hKey;
    bool bReturn = false;

    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, cstrNAVKey, 0, KEY_QUERY_VALUE, &hKey);

    if (lResult == ERROR_SUCCESS)
    {
        TCHAR szNAVInstType[2] = {0};
        DWORD dwSize = sizeof(szNAVInstType);

        lResult = RegQueryValueEx(hKey, _T("Type"), NULL, NULL, (BYTE* ) &szNAVInstType, &dwSize);

        if (lResult == ERROR_SUCCESS)
        {
            // If it's NAV Professional, Type should be set to 1
            if (_ttoi(szNAVInstType) == 1)
            {
                bReturn = true;
            }
        }

        RegCloseKey(hKey);
    }

    return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
// Get the full path to the folder where the current version of the Online 
// Platform Client (OPC) is installed.
// In NAV 2007/14.0 the version is 7.
// e.g. HKEY_LOCAL_MACHINE\SOFTWARE\Symantec\InstalledApps: 
//			OPC7 = C:\Program Files\Common Files\Symantec Shared\OPC\{31011D49-D90C-4da0-878B-78D28AD507AF}\
// @return Pointer to the path buffer. NAVInfo stores the data.
const TCHAR* CNAVInfo::GetOnlinePlatformClientDir()
{
    if ( m_szOPCDir[0] == '\0' )
        InitGetOnlinePlatformClientDir();

    return m_szOPCDir;
}

// Location for the current version of the Online Platform Client (OPC)
// In NAV 2007/14.0 the version is 7.
// e.g. HKEY_LOCAL_MACHINE\SOFTWARE\Symantec\InstalledApps: 
//			OPC7 = C:\Program Files\Common Files\Symantec Shared\OPC\{31011D49-D90C-4da0-878B-78D28AD507AF}\
//
bool CNAVInfo::InitGetOnlinePlatformClientDir()
{
    LONG lRes;
    HKEY  hkey;
    DWORD dwType, dwBuffSize = MAX_PATH * sizeof(TCHAR);

    // Read the OPC install directory.
    if ( ERROR_SUCCESS ==
        (lRes = ::RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
        _T("Software\\Symantec\\InstalledApps"),
        0, KEY_QUERY_VALUE, &hkey )) )
    {
        if ( ERROR_SUCCESS ==
            (lRes = ::RegQueryValueEx ( hkey, _T("OPC7"), NULL, &dwType,
            LPBYTE(m_szOPCDir), &dwBuffSize )) )
        {
            if ( !*m_szOPCDir )
            {
                // Defect 1-3FST6U
                ::RegCloseKey ( hkey );
                return false;
            }

            // Get the second to last char in the path.
            TCHAR* pszDirNull = _tcsrchr ( m_szOPCDir, '\0');
            TCHAR* pszLastSlash = CharPrev( m_szOPCDir, pszDirNull );

            // If the ending character is a slash, null it.
            if ( 0 == _tcscmp ( pszLastSlash, _T("\\")))
            {
                *pszLastSlash = '\0';
            }
        }

        ::RegCloseKey ( hkey );
    }

    return (ERROR_SUCCESS == lRes);
}
