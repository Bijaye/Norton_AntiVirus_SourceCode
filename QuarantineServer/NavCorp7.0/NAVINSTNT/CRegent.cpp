///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header: CRegent.cpp  Revision:1.1  Sun Jul 13 22:43:24 1997  BRUCEH $
//
//  CREGENT.CPP   - This file contains class definitions for the CRegistryEntry
//                  Class Objects.
//
//
//
///////////////////////////////////////////////////////////////////////////


#include <afxwin.h>

#include "cregent.h"


#ifndef _WIN32
#error This implementation of CRegEnt requires Win32
#endif


///////////////////////////////////////////////////////////////////////////
// Constructors/Destructors
///////////////////////////////////////////////////////////////////////////

CRegistryEntry::CRegistryEntry()
{
    InitMembers();
    return;
}


CRegistryEntry::CRegistryEntry( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue )
{
    InitMembers();
    m_hHive = hKey;

    if ( lpSubKey != NULL )
    {
        m_lpSubKey = new TCHAR[ lstrlen( lpSubKey ) + 1 ];
        lstrcpy( m_lpSubKey, lpSubKey );
    }

    if ( lpValue != NULL )
    {
        m_lpValue = new TCHAR[ lstrlen( lpValue ) + 1 ];
        lstrcpy( m_lpValue, lpValue );
    }

    return;
}


CRegistryEntry::~CRegistryEntry()
{
    if ( IsOpen() )
        CloseKey();

    if ( m_lpSubKey != NULL )
        delete m_lpSubKey;

    if ( m_lpValue != NULL )
        delete m_lpValue;

    return;
}



///////////////////////////////////////////////////////////////////////////
// Member functions
///////////////////////////////////////////////////////////////////////////

void CRegistryEntry::SetHive( HKEY hKey )
{
    if ( IsOpen() )
        CloseKey();

    m_hHive = hKey;

    return;
}



void CRegistryEntry::SetSubkey( LPTSTR lpSubKey )
{
    if ( IsOpen() )
        CloseKey();

    if ( m_lpSubKey != NULL )
    {
        delete m_lpSubKey;
        m_lpSubKey = NULL;
    }

    if ( lpSubKey != NULL )
    {
        m_lpSubKey = new TCHAR[ lstrlen( lpSubKey ) + 1 ];
        lstrcpy( m_lpSubKey, lpSubKey );
    }

    return;
}



void CRegistryEntry::SetValueName( LPTSTR lpValueName )
{
    if ( m_lpValue != NULL )
    {
        delete m_lpValue;
        m_lpValue = NULL;
    }

    if ( lpValueName != NULL )
    {
        m_lpValue = new TCHAR[ lstrlen( lpValueName ) + 1 ];
        lstrcpy( m_lpValue, lpValueName );
    }


    return;
}



void CRegistryEntry::ResetKey( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue )
{
    if ( IsOpen() )
        CloseKey();

    m_hHive = hKey;

    if ( m_lpSubKey != NULL )
    {
        delete m_lpSubKey;
        m_lpSubKey = NULL;
    }

    if ( lpSubKey != NULL )
    {
        m_lpSubKey = new TCHAR[ lstrlen( lpSubKey ) + 1 ];
        lstrcpy( m_lpSubKey, lpSubKey );
    }

    if ( m_lpValue != NULL )
    {
        delete m_lpValue;
        m_lpValue = NULL;
    }

    if ( lpValue != NULL )
    {
        m_lpValue = new TCHAR[ lstrlen( lpValue ) + 1 ];
        lstrcpy( m_lpValue, lpValue );
    }

    return;
}



LONG CRegistryEntry::OpenKey( DWORD dwAccess )
{
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( m_hHive != NULL && m_lpSubKey != NULL )
    {
        lReturn = RegOpenKeyEx( m_hHive, m_lpSubKey, 0, dwAccess, &m_hKey );
    }

    if ( lReturn == ERROR_SUCCESS && m_hKey != NULL )
        m_bOpen = TRUE;
    else
    {
        m_hKey = NULL;
        m_bOpen = FALSE;
    }

    return( lReturn );
}



LONG CRegistryEntry::OpenCreateKey( DWORD dwAccess )
{
    LONG            lReturn = ERROR_INVALID_DATA;
    DWORD           dwDisp = 0;

    if ( m_hHive != NULL && m_lpSubKey != NULL )
    {
        lReturn = RegCreateKeyEx( m_hHive, m_lpSubKey, 0, NULL,
                                  REG_OPTION_NON_VOLATILE,
                                  dwAccess, NULL, &m_hKey, &dwDisp );
    }

    if ( lReturn == ERROR_SUCCESS && m_hKey != NULL )
        m_bOpen = TRUE;
    else
    {
        m_hKey = NULL;
        m_bOpen = FALSE;
    }

    return( lReturn );
}



LONG CRegistryEntry::GetValue( LPDWORD lpdwType, LPBYTE lpBuffer, LPDWORD lpdwSize )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ );
        bClose = TRUE;
    }

    lReturn = RegQueryValueEx( m_hKey, m_lpValue, NULL,
                               lpdwType, lpBuffer, lpdwSize );

    if ( bClose )
        CloseKey();

    return( lReturn );
}



LONG CRegistryEntry::GetValueInfo( LPDWORD lpdwType, LPDWORD lpdwSize )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ );
        bClose = TRUE;
    }

    if ( lpdwType != NULL && lpdwSize != NULL )
        lReturn = RegQueryValueEx( m_hKey, m_lpValue, NULL,
                                   lpdwType, NULL, lpdwSize );

    if ( bClose )
        CloseKey();

    return( lReturn );
}



LONG CRegistryEntry::SetValue( DWORD dwType, LPBYTE lpData, DWORD dwSize )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( !IsOpen() )
    {
        OpenCreateKey( KEY_WRITE );
        bClose = TRUE;
    }

    if ( lpData != NULL )
        lReturn = RegSetValueEx( m_hKey, m_lpValue, 0, dwType, lpData, dwSize );

    if ( bClose )
        CloseKey();

    return( lReturn );
}


LONG CRegistryEntry::DeleteValue( void )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( !IsOpen() )
    {
        OpenKey( KEY_WRITE );
        bClose = TRUE;
    }

    lReturn = RegDeleteValue( m_hKey, m_lpValue );

    if ( bClose )
        CloseKey();

    return( lReturn );
}


LONG CRegistryEntry::DeleteAllValues( void )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;
    DWORD           dwIndex = 0;
    DWORD           dwSize = REG_MAX_STR;
    TCHAR           szValueName[ REG_MAX_STR ];

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ | KEY_WRITE );
        bClose = TRUE;
    }

    while( RegEnumValue( m_hKey, dwIndex, szValueName, &dwSize, NULL,
                         NULL, NULL, NULL ) == ERROR_SUCCESS )
    {
        RegDeleteValue( m_hKey, szValueName );
        dwIndex++;
        dwSize = REG_MAX_STR;
        lReturn = ERROR_SUCCESS;
    }

    if ( bClose )
        CloseKey();

    return( lReturn );
}



LONG CRegistryEntry::IsKeyEmpty( void )
{
    LONG        lRet = FALSE;
    DWORD       dwSubKeys = 0;
    DWORD       dwValues = 0;

    lRet = GetKeyInfo( &dwSubKeys, &dwValues );

    if ( lRet == ERROR_SUCCESS )
        lRet = !dwSubKeys && !dwValues;

    return( lRet );
}



DWORD CRegistryEntry::GetSubkeyCount( void )
{
    DWORD       dwReturn = ERR_INVALID_COUNT;
    DWORD       dwSubKeys = 0;
    DWORD       dwValues = 0;

    if ( GetKeyInfo( &dwSubKeys, &dwValues ) == ERROR_SUCCESS )
        dwReturn = dwSubKeys;

    return( dwReturn );
}



DWORD CRegistryEntry::GetValueCount( void )
{
    DWORD       dwReturn = ERR_INVALID_COUNT;
    DWORD       dwSubKeys = 0;
    DWORD       dwValues = 0;

    if ( GetKeyInfo( &dwSubKeys, &dwValues ) == ERROR_SUCCESS )
        dwReturn = dwValues;

    return( dwReturn );
}



BOOL CRegistryEntry::DeleteKey( BOOL bSubKeys )
{
    BOOL            bRet = FALSE;
    BOOL            bWinNT = FALSE;
    OSVERSIONINFO   osInfo;

    memset( &osInfo, 0, sizeof( OSVERSIONINFO ) );
    osInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

    GetVersionEx( &osInfo );

    if ( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
        bWinNT = TRUE;

    if ( m_lpSubKey && bWinNT && bSubKeys )
        DeleteSubkeys();

    if ( IsOpen() )
        CloseKey();

    RegDeleteKey( m_hHive, m_lpSubKey );
    bRet = TRUE;

    return( bRet );
}



void CRegistryEntry::CloseKey( void )
{
    if ( m_hKey != NULL )
    {
        RegCloseKey( m_hKey );
        m_hKey = NULL;
        m_bOpen = FALSE;
    }

    return;
}



BOOL CRegistryEntry::DeleteSubkeys( void )
{
    BOOL                bClose = FALSE;
    BOOL                bRet = FALSE;
    DWORD               dwIndex = 0;
    DWORD               dwKeySize = REG_MAX_STR - 1;
    LPTSTR              lpSubKey;
    FILETIME            ftWriteTime;
    CRegistryEntry     *pCRegEnt;

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ | KEY_WRITE );
        bClose = TRUE;
    }

    if ( m_lpSubKey != NULL )
    {
        lpSubKey = new TCHAR[ REG_MAX_STR ];

        while( RegEnumKeyEx( m_hKey, dwIndex, lpSubKey, &dwKeySize,
                             NULL, NULL, NULL, &ftWriteTime) == ERROR_SUCCESS)
        {
            pCRegEnt = new CRegistryEntry( m_hKey, lpSubKey );
            pCRegEnt->DeleteKey();
            delete pCRegEnt;
            dwKeySize = REG_MAX_STR - 1;
        }

        bRet = TRUE;
        delete lpSubKey;
    }

    if ( bClose )
        CloseKey();

    return( bRet );
}


LONG CRegistryEntry::GetKeyInfo( LPDWORD lpdwKeys, LPDWORD lpdwValues )
{
    BOOL        bClose = FALSE;
    LONG        lReturn = ERROR_INVALID_DATA;
    TCHAR       szClass[ REG_MAX_STR ];
    DWORD       dwClass = 0;
    DWORD       dwMaxSubkey = 0;
    DWORD       dwMaxClass = 0;
    DWORD       dwMaxValueName = 0;
    DWORD       dwMaxValueData = 0;
    DWORD       dwSecurityDescriptor = 0;
    FILETIME    ftWriteTime;

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ );
        bClose = TRUE;
    }

    dwClass = REG_MAX_STR;

    lReturn = RegQueryInfoKey( m_hKey, szClass, &dwClass,
                               NULL, lpdwKeys, &dwMaxSubkey,
                               &dwMaxClass,
                               lpdwValues, &dwMaxValueName, &dwMaxValueData,
                               &dwSecurityDescriptor, &ftWriteTime );

    if ( bClose )
        CloseKey();

    return( lReturn );
}



///////////////////////////////////////////////////////////////////////////
//  $Log: G:\QVCS\LOGS\BHUtils32\CRegent.dqq $
//  
//    
//  
//  Revision 1.1  by: BRUCEH  Rev date: Sun Jul 13 22:43:24 1997
//    1. Fixed bug in Cregent when setting value of a key that doesn't exist
//  
//  Revision 1.0  by: BRUCEH  Rev date: Sun Jun 29 01:03:18 1997
//    Initial revision.
//  
//  $Endlog$
//
//
//
///////////////////////////////////////////////////////////////////////////

