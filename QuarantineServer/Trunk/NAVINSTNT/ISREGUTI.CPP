/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/Isreguti.cpv   1.1   11 May 1997 21:30:08   SEDWARD  $
//
//  ISRegUtil.CPP - Module that holds the registry utilities for the uninstall of
//                  the NAVNT.
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/Isreguti.cpv  $
// 
//    Rev 1.1   11 May 1997 21:30:08   SEDWARD
// Changed SI_MAX_PATH to SYM_MAX_PATH.
//
//    Rev 1.0   05 May 1997 19:24:54   sedward
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef SYM_WIN32
#define SYM_WIN32
#endif

#include <windows.h>
#include <windowsx.h>

#pragma hdrstop

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include "NavInst.h"
#include "IsRegUti.h"
#include "IsUtil.h"
#include "resource.h"
#include "cwstring.h"


/////////////////////////////////////////////////////////////////////////////
//  Global variables
/////////////////////////////////////////////////////////////////////////////

static char         g_szBuf[40];


/////////////////////////////////////////////////////////////////////////////
//  Local inline functions
/////////////////////////////////////////////////////////////////////////////

LPTSTR _inline MapHiveToStr( HKEY hKey )
{
    DWORD       dwValue = (DWORD)hKey;
    LPTSTR      lpRet = NULL;
    LPTSTR      lpHives[] = { "HKEY_CLASSES_ROOT", "HKEY_LOCAL_MACHINE",
                              "HKEY_CURRENT_USER", "HKEY_USERS", NULL };

    switch( dwValue )
    {
        case (DWORD)HKEY_CLASSES_ROOT:
            lstrcpy( g_szBuf, lpHives[0] );
            lpRet = g_szBuf;
            break;

        case (DWORD)HKEY_LOCAL_MACHINE:
            lstrcpy( g_szBuf, lpHives[1] );
            lpRet = g_szBuf;
            break;

        case (DWORD)HKEY_CURRENT_USER:
            lstrcpy( g_szBuf, lpHives[2] );
            lpRet = g_szBuf;
            break;

        case (DWORD)HKEY_USERS:
            lstrcpy( g_szBuf, lpHives[3] );
            lpRet = g_szBuf;
            break;

        default:
            wsprintf( (LPSTR)g_szBuf, "%lu", hKey );
            lpRet = g_szBuf;
            break;
    }

    return( lpRet );
}


/////////////////////////////////////////////////////////////////////////////
//
//  IRegDBDeleteKey - Function to delete a registry key and all subkeys
//                    directory
//
//  Params: LPTSTR - Key to delete
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI IRegDBDeleteKey( HKEY hHive, LPTSTR lpSubKey )
{
    BOOL                bRet = FALSE;
    HKEY                hKey = NULL;
    LONG                lResult = 0;
    DWORD               dwIndex = 0;
    DWORD               dwKeySize = SYM_MAX_PATH - 1;
    FILETIME            ftWriteTime;
    CWString            szSubKey(SYM_MAX_PATH);

    LTrace( "BEGIN IRegDBDeleteKey( %s, %s )", MapHiveToStr( hHive ), lpSubKey );

    if ( lpSubKey != NULL && *lpSubKey )
    {
        if ( RegOpenKeyEx( hHive, lpSubKey, 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS &&
            hKey != NULL )
        {
            if ( RegEnumKeyEx( hKey, dwIndex, szSubKey, &dwKeySize,
                               NULL, NULL, NULL, &ftWriteTime ) == ERROR_SUCCESS)
            {
                do
                {
                    IRegDBDeleteKey( hKey, szSubKey );
                    RegDeleteKey( hKey, szSubKey );
                    dwKeySize = SYM_MAX_PATH - 1;
                }
                while( RegEnumKeyEx( hKey, dwIndex, szSubKey, &dwKeySize,
                                     NULL, NULL, NULL, &ftWriteTime ) == ERROR_SUCCESS);
            }
            else
                LTrace( "   No subkeys" );

            RegCloseKey( hKey );
            hKey = NULL;
            bRet = TRUE;

            lResult = RegDeleteKey( hHive, lpSubKey );

            if ( lResult == ERROR_SUCCESS )
                LTrace( "   Key deleted successfully" );
            else
                LTrace( "   RegDeleteKey failed ( %lu )", lResult );
        }
        else
            LTrace( "   RegOpenKeyEx failed" );
    }

    LTrace( "END IRegDBDeleteKey( %d )\n", bRet );

    return( bRet );
}



/////////////////////////////////////////////////////////////////////////////
//
//  IRegDBDeleteValue - Function to delete a value from a registry key
//
//  Params: HKEY - Registry key
//          LPTSTR - Subkey to query
//          LPTSTR - Value to delete
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI IRegDBDeleteValue( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue )
{
    HKEY            hOpenKey = NULL;
    BOOL            bRet = FALSE;

    LTrace( "BEGIN IRegDBDeleteValue( %s, %s, %s )", MapHiveToStr( hKey ), lpSubKey,
            lpValue == NULL ? "NULL" : lpValue );

    if ((RegOpenKeyEx( hKey, lpSubKey, 0, KEY_SET_VALUE, &hOpenKey ) == ERROR_SUCCESS) &&
         hOpenKey != NULL)
    {
        if ( RegDeleteValue( hOpenKey, lpValue ) == ERROR_SUCCESS )
        {
            bRet = TRUE;
            LTrace( "   Value deleted successfully" );
        }
        else
            LTrace( "   Delete value failed" );

        RegCloseKey( hOpenKey );
    }
    else
        LTrace( "   RegOpenKeyEx failed" );

    LTrace( "END IRegDBDeleteValue( %d )\n", bRet );

    return( bRet );
}


/////////////////////////////////////////////////////////////////////////////
//
//  IRegDBSetValue - Function to set a value in a registry key
//
//  Params: HKEY - Registry key
//          LPTSTR - Subkey to query
//          LPTSTR - Value to set
//          DWORD - Type of data
//          LPBYTE - Data to set
//          int - Size of data
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI IRegDBSetValue( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue,
                            DWORD dwType, LPBYTE lpData, int nDataSize )
{
    HKEY            hOpenKey = NULL;
    BOOL            bRet = FALSE;

    LTrace( "BEGIN IRegDBSetValue( %s, %s, %s )", MapHiveToStr( hKey ), lpSubKey,
            lpValue == NULL ? "NULL" : lpValue );

    if ((RegOpenKeyEx( hKey, lpSubKey, 0, KEY_SET_VALUE, &hOpenKey ) == ERROR_SUCCESS) &&
         hOpenKey != NULL)
    {
        if ( RegSetValueEx( hOpenKey, lpValue, 0, dwType,
                            lpData, nDataSize ) == ERROR_SUCCESS )
        {
            LTrace( "   Value set successfully" );
            bRet = TRUE;
        }

        RegCloseKey( hOpenKey );
    }
    else
        LTrace( "   RegOpenKeyEx failed" );

    LTrace( "END IRegDBSetValue( %d )\n", bRet );

    return( bRet );
}



/////////////////////////////////////////////////////////////////////////////
//
//  IRegDBGetValue - Function to get a value in a registry key
//
//  Params: HKEY - Registry key
//          LPTSTR - Subkey to query
//          LPTSTR - Value to query
//          LPDWORD - Returns the type
//          LPBYTE* - Pointer to a LPBYTE to hold data address
//          LPDWORD - Pointer to a DWORD that will hold the data size
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI IRegDBGetValue( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue,
                            LPDWORD lpdwType, LPBYTE *lplpData, LPDWORD lpdwDataSize )
{
    HKEY            hOpenKey = NULL;
    BOOL            bRet = FALSE;
    LPBYTE          lpBlob;

    LTrace( "BEGIN IRegDBGetValue( %s, %s, %s )", MapHiveToStr( hKey ), lpSubKey,
            lpValue == NULL ? "NULL" : lpValue );

    if ((RegOpenKeyEx( hKey, lpSubKey, 0, KEY_QUERY_VALUE, &hOpenKey ) == ERROR_SUCCESS) &&
         hOpenKey != NULL)
    {
        if ( RegQueryValueEx( hOpenKey, lpValue, NULL, NULL, NULL, lpdwDataSize ) ==
             ERROR_SUCCESS )
        {
            lpBlob = (LPBYTE)GlobalAllocPtr( GHND, *lpdwDataSize );

            if ( RegQueryValueEx( hOpenKey, lpValue, NULL, lpdwType, lpBlob,
                                  lpdwDataSize ) == ERROR_SUCCESS )
            {
                LTrace( "   Value retrieved successfully" );
                bRet = TRUE;
                *lplpData = lpBlob;
            }

        }

        RegCloseKey( hOpenKey );
    }
    else
        LTrace( "   RegOpenKeyEx failed" );

    LTrace( "END IRegDBGetValue( %d )\n", bRet );

    return( bRet );
}




/////////////////////////////////////////////////////////////////////////////
//
//  IRegDBGetKeyInfo - Function to retrieve the number of subkeys and values
//                     of a registry key
//
//  Params: HKEY - Registry key
//          LPTSTR - Subkey to query
//          LPDWORD - Pointer to DWORD that will hold the number of subkeys
//          LPDWORD - Pointer to DWORD that will hold the number of values
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI IRegDBGetKeyInfo( HKEY hKey, LPSTR lpszSubKey, LPDWORD lpdwKeys,
                              LPDWORD lpdwValues )
{
    HKEY            hNewKey = NULL;
    BOOL            bRet = FALSE;
    LONG            lStatus;
    DWORD           dwClass     = 0;
    DWORD           dwMaxSubkey = 0;
    DWORD           dwMaxClass  = 0;
    DWORD           dwMaxValueName = 0;
    DWORD           dwMaxValueData = 0;
    DWORD           dwSecurityDescriptor = 0;
    CWString        szClass(SYM_MAX_PATH);
    FILETIME        ftWriteTime;


    LTrace( "BEGIN IRegDBGetKeyInfo( %s, %s )", MapHiveToStr( hKey ), lpszSubKey );

    lStatus = RegOpenKeyEx( hKey, lpszSubKey, 0L, KEY_QUERY_VALUE, &hNewKey);

    if (lStatus == ERROR_SUCCESS && hNewKey != NULL)
    {
        dwClass = SYM_MAX_PATH;

        RegQueryInfoKey ( hNewKey,
                          szClass,
                          &dwClass,
                          NULL,
                          lpdwKeys,
                          &dwMaxSubkey,
                          &dwMaxClass,
                          lpdwValues,
                          &dwMaxValueName,
                          &dwMaxValueData,
                          &dwSecurityDescriptor,
                          &ftWriteTime);

        RegCloseKey(hNewKey);
        LTrace( "   Values retrieved, Subkeys=%lu, Values=%lu", *lpdwKeys, *lpdwValues );
        bRet = TRUE;
    }
    else
        LTrace( "   RegOpenKeyEx failed %l", lStatus );

    LTrace( "END IRegDBGetKeyInfo( %d )\n", bRet );

    return ( bRet );
}


