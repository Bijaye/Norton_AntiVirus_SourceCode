// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//////////////////////////////////////////////////////////////////////
//
// UserDir.cpp: implementation of the CUserDir class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlbase.h>
#include "ldvpview.h"
#include "UserDir.h"
#include "clientreg.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "shfolder.h"
#include "sharedver.h"

//////////////////////////////////////////////////////////////////////
// Defines/types for EXTERNAL DLLs
//////////////////////////////////////////////////////////////////////

// NAVNTUTL
#define NAVNTUTL_DLL_NAME			_T( "NAVNTUTL.DLL" )
#define PROCESSISNORMALUSER         _T("ProcessIsNormalUser")
typedef ULONG (*PFNProcessIsNormalUser)(ULONG * lpbNormalUser);

// SHFOLDER.DLL
typedef HRESULT (WINAPI *PFNSHGetFoldPathA)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserDir::CUserDir()
{

}

CUserDir::~CUserDir()
{

}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CreateUserDirectory
//
// Description  : Finds the non-roaming application data directory (as defined
//                by CSIDL_LOCAL_APPDATA) for the current user creates the 
//                product subdirectories and the specific subdir that is passed
//                in ("Logs", "Temp", etc.) For example, the system provides
// 
//                C:\Documents and Settings\tcashin\Local Settings\Application Data
//
//                and the function creates:
//
//                Symantec\Symantec AntiVirus Corporate Edition\7.5\Logs
//
// Return Values: ERROR_SUCCESS or standard error codes
//
// Argument     : [in]  LPTSTR lpDataDirectory - Pointer to the dir to create
//                             ("Logs", "Temp", etc.)
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CUserDir::CreateUserDirectory(LPTSTR lpDataDirectory)
{
    DWORD           dwReturn = ERROR_SUCCESS;
    DWORD           dwClientType = 0;
    TCHAR           szTemp1[IMAX_PATH] = {0};
    TCHAR           szTemp2[IMAX_PATH] = {0};


    dwClientType = GetClientType();

    if ( IsWinNT() )
    {
        dwReturn = GetSystemDataDir(NAV_USER_DATA, szTemp1);

        if (dwReturn == ERROR_SUCCESS )
        {
            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, SYMANTEC_COMPANY_NAME );
            if ( !CreateDirectory(szTemp2, NULL) )
            {
                dwReturn = GetLastError();

                if ( dwReturn != ERROR_ALREADY_EXISTS )
                    goto All_Done;
            }

            _tcscpy(szTemp1, szTemp2);

            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, NAVCORP_DIRECTORY_NAME );
            if ( !CreateDirectory(szTemp2, NULL) )
            {
                dwReturn = GetLastError();

                if ( dwReturn != ERROR_ALREADY_EXISTS )
                    goto All_Done;
            }

            _tcscpy(szTemp1, szTemp2);

            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, SHARED_VERSION_DATA_DIRECTORY );
            if ( !CreateDirectory(szTemp2, NULL) )
            {
                dwReturn = GetLastError();

                if ( dwReturn != ERROR_ALREADY_EXISTS )
                    goto All_Done;            }

            _tcscpy(szTemp1, szTemp2);

            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, lpDataDirectory );
            if ( !CreateDirectory(szTemp2, NULL) )
            {
                dwReturn = GetLastError();
            }
        }
    }
    else
    {
        dwReturn = ERROR_NOT_SUPPORTED;
    }

All_Done:

    return dwReturn;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetSystemDataDir
//
// Description  : Loads SHFOLDER.DLL and calls SHGetFolderPath() to get the
//                non-roaming application data directory (CSIDL_LOCAL_APPDATA)
//                for the current user.
//
//                Currently, this function requires Windows 2000 or later.
//
// Return Values: ERROR_SUCCESS or other error code.
//
// Argument     : [in]   DWORD  dwFlags - indicates whether to get app data or
//                              non-roaming user data
//                [out]  LPTSTR lpSystemDataDir - Buffer to get the directory
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CUserDir::GetSystemDataDir( DWORD dwFlags, LPTSTR lpSystemDataDir )
{

    auto PFNSHGetFoldPathA pfnSHGetFolderPathA = NULL;

    auto DWORD          dwError = ERROR_SUCCESS;
    auto DWORD          dwClientType = 0;
    auto TCHAR          szAppData[MAX_PATH] = {0};
    auto TCHAR          szSystemDir[MAX_PATH] = {0};
    auto TCHAR          szSHFolderDLL[MAX_PATH] ={0};
    auto HINSTANCE      hFolderDLL = NULL;
    auto HRESULT        hr ;
    auto int            len = 0;


    if ( !lpSystemDataDir ||
         (dwFlags != NAV_APP_DATA &&
          dwFlags != NAV_USER_DATA) )
    {
        return ERROR_INVALID_PARAMETER;
    }

    dwClientType = GetClientType();

    if ( IsWinNT() )
    {
        // Get the system directory
        if ( GetSystemDirectory(szSystemDir, sizeof(szSystemDir)) )
        {
            // Append the DLL name
            sssnprintf ( szSHFolderDLL, sizeof(szSHFolderDLL), _T("%s\\shfolder.dll"), szSystemDir );

            // Load it.
            hFolderDLL = LoadLibrary( szSHFolderDLL );

            if ( hFolderDLL )
            {
                // Get the function
                pfnSHGetFolderPathA = (PFNSHGetFoldPathA)GetProcAddress( hFolderDLL, _T("SHGetFolderPathA") );

                if ( pfnSHGetFolderPathA )
                {
                    if ( dwFlags == NAV_USER_DATA )
                    {
						if ( IsWindows2000() )
						{
							// Per machine, specific user, non-roaming, create it here
							hr = pfnSHGetFolderPathA( NULL, CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE, 
													  NULL, 0, szAppData);
						}
						else
						{
							// Per machine, specific user, non-roaming, create it here
							hr = pfnSHGetFolderPathA( NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, 
													  NULL, 0, szAppData);
						}
                    }
                    else
                    {
                        // Per machine, application
                        hr = pfnSHGetFolderPathA( NULL, CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE, 
                                                  NULL, 0, szAppData);
                    }
                    
                    if ( SUCCEEDED(hr) )
                    {
                        _tcscpy( lpSystemDataDir, szAppData ); 
                    }
                    else
                        dwError = GetLastError();
                }
                FreeLibrary( hFolderDLL );
            }
            else
                dwError = GetLastError();
        }
        else
            dwError = GetLastError();
    }
    else
    {
        // Not Windows 2000.
      	dwError = ERROR_NOT_SUPPORTED;
    }

    return dwError;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: UserDirectoryExists
//
// Description  : Checks to see if the specified data ("Logs", "Temp", etc.) 
//                directory exists under
//
//                Symantec\Symantec AntiVirus Corporate Edition\7.5 
//
//                within the non-roaming application data (CSIDL_LOCAL_APPDATA)
//                directory for the current user.
//
// Return Values: TRUE if the directory exists
//
// Argument     : [in]  LPTSTR lpDataDirectory - Pointer to the dir to create
//                             ("Logs", "Temp", etc.)
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CUserDir::UserDirectoryExists(LPTSTR lpUserDir)
{
    BOOL            bDirExists = FALSE;
    DWORD           dwError = ERROR_SUCCESS;
    TCHAR           szTemp1[IMAX_PATH] = {0};
    TCHAR           szTemp2[IMAX_PATH] = {0};
    HANDLE          hDir = NULL;
    WIN32_FIND_DATA finddata = {0};


    if ( IsWinNT() )
    {
        dwError = GetSystemDataDir( NAV_USER_DATA, szTemp1);

        if (dwError == ERROR_SUCCESS )
        {
            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s\\%s", szTemp1, SYMANTEC_COMMON_DATA_DIR, lpUserDir  );

            hDir = FindFirstFile( szTemp2, &finddata );

            if ( hDir && hDir != INVALID_HANDLE_VALUE )
            {
                bDirExists = TRUE;
                FindClose( hDir );
            }
        }
    }
    
    return bDirExists;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsWinNT
//
// Description  : Checks to see if the operating system is Windows NT.
//
// Return Values: TRUE is the OS is WinNT
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CUserDir::IsWinNT()
{
    OSVERSIONINFO   osInfo;
    BOOL            bRet = FALSE;

    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 

    if ( GetVersionEx (&osInfo) )
    {
        if ( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
        {   
            bRet = TRUE;
        }
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsWindows2000
//
// Description  : Checks to see if the operating system is Windows 2000.
//
// Return Values: TRUE is the OS is Windows 2000
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CUserDir::IsWindows2000()
{
    OSVERSIONINFO   osInfo;
    BOOL            bRet = FALSE;

    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 

    if ( GetVersionEx (&osInfo) )
    {
        if ( osInfo.dwMajorVersion >= 5 )
        {   
            bRet = TRUE;
        }
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: UserDirsNeeded
//
// Description  : User data directories are need is all the following
//                conditions are TRUE:
//
//                o  Windows NT or later
//                o  VPC32 is installed on the machine
//                o  The current user is a restricted user
//
// Return Values: TRUE if user data directories should be used.
//
// Argument     : None
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CUserDir::UserDirsNeeded(DWORD dwFlags)
{
    BOOL        bRet = FALSE;
    DWORD       dwNormalUser = 0;
    DWORD       dwSize = 0;
    DWORD       dwClientType = 0;
    CString     szLibPath;
    CRegKey     reg;
    HINSTANCE   hDLL;
    LONG        lResult;

    PFNProcessIsNormalUser pfnProcessIsNormalUser;


    // Have to be running on Windows NT
    if ( !IsWinNT() )
    {
        return bRet;
    }

    // VPC32 client or server has to be installed. See if we have a
    // valid "ClientType" reg key.
    if ( GetClientType() == 0 )
    {
        return bRet;
    }

    // Get the install directory
    if( ERROR_SUCCESS != reg.Open( HKEY_LOCAL_MACHINE, szReg_Key_Main, KEY_READ ) )
    {
       return bRet;
    }
  
    dwSize = MAX_PATH;
  
    lResult = reg.QueryStringValue( szReg_Val_HomeDir, szLibPath.GetBuffer( MAX_PATH ), &dwSize );
    szLibPath.ReleaseBuffer();
  
    reg.Close();
  
    if ( lResult != ERROR_SUCCESS )
    {
       return bRet;
    }

    // 
    // Build full path to NAVNTUTL.DLL.
    // 
    szLibPath += _T("\\");
    szLibPath += NAVNTUTL_DLL_NAME;

    // 
    // Load the DLL.
    // 
    hDLL = LoadLibrary( szLibPath );
    if( !hDLL  )
        return bRet;

    //
    // Get the function pointer
    //
    pfnProcessIsNormalUser = (PFNProcessIsNormalUser) GetProcAddress( hDLL, PROCESSISNORMALUSER );

    //
    // Call the function if we can
    //
    if ( pfnProcessIsNormalUser && dwFlags == READ_USER_LOGS)
    {
        pfnProcessIsNormalUser( &dwNormalUser );

        if ( dwNormalUser )
        {
            // Normal we so we want private 
            // data directories.
            bRet = TRUE;
        }
    }
    else
    {
        bRet = TRUE;
    }

    FreeLibrary( hDLL );

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetUserDirectory
//
// Description  : Get the users directory. This will be something like
//
//                Symantec\Symantec AntiVirus Corporate Edition\7.5
//                  
//                under the non-roaming application data directory for the
//                current user as defined by CSIDL_LOCAL_APPDATA.
//
// Return Values: ERROR_SUCCESS or other error code.
//
// Argument     : [out]  LPTSTR lpUserDir - Buffer to get the directory
//              : [in]   size_t nUserDirBytes - size of buffer in bytes
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CUserDir::GetUserDirectory(LPTSTR lpUserDir, size_t nUserDirBytes)
{
    BOOL            bDirExists = FALSE;
    DWORD           dwReturn = ERROR_NOT_SUPPORTED;
    TCHAR           szTemp1[IMAX_PATH] = {0};
    TCHAR           szTemp2[IMAX_PATH] = {0};
    HANDLE          hDir = NULL;
    WIN32_FIND_DATA finddata = {0};


    if ( IsWinNT() )
    {
        dwReturn = GetSystemDataDir( NAV_USER_DATA, szTemp1 );

        if (dwReturn == ERROR_SUCCESS )
        {
            sssnprintf( lpUserDir,nUserDirBytes,"%s\\%s\\%s", szTemp1, 
                      SYMANTEC_COMMON_DATA_DIR, 
                      SHARED_VERSION_DATA_DIRECTORY );
        }
    }
    
    return dwReturn;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetAppDataDirectory
//
// Description  : Get the common app data directory. This will be something like
//
//                Symantec\Symantec AntiVirus Corporate Edition\7.5
//                  
//                under the non-roaming per-machine application data directory 
//                as defined by CSIDL_COMMON_APPDATA.
//
// Return Values: ERROR_SUCCESS or other error code.
//
// Argument     : [out]  LPTSTR lpAppDataDir - Buffer to get the directory
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CUserDir::GetAppDataDirectory(LPTSTR lpAppDataDir, size_t nDataDirBytes)
{
    DWORD   dwReturn = ERROR_NOT_SUPPORTED;
    TCHAR   szTemp1[IMAX_PATH] = {0};

    if ( IsWinNT() )
    {
        dwReturn = GetSystemDataDir( NAV_APP_DATA, szTemp1 );

        if (dwReturn == ERROR_SUCCESS )
        {
            sssnprintf( lpAppDataDir,nDataDirBytes,"%s\\%s\\%s", szTemp1, 
                      SYMANTEC_COMMON_DATA_DIR, 
                      SHARED_VERSION_DATA_DIRECTORY );
        }
    }

    return dwReturn;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetClientType
//
// Description  : 
//
// Return Values: 
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CUserDir::GetClientType()
{
    DWORD           dwClientType = 0;
    CRegKey         reg;

    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, szReg_Key_Main, KEY_READ ) )
    {
        reg.QueryDWORDValue( szReg_Val_Client_Type, dwClientType );
        reg.Close();
    }

    return dwClientType;
}
