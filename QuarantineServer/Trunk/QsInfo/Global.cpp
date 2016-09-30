// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1999, 2005 Symantec Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////
// Global.cpp   created 2/9/1999
//
// $Header$
// 
// Description: Contains implementation misc. global functions 
//              for qsinfo32.dll. Ported/borrowed from Atomic/Quar32.
// Contains:    
//
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "global.h"
#include <tchar.h>
#include "SymSaferRegistry.h"

extern PFNNetWkstaGetInfo   g_pfnNetWkstaGetInfo;


////////////////////////////////////////////////////////////////////////////
// Function name    : GetDomainName
//
// Description      : Determines the domain name of the machine
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 2/09/99 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    GetDomainName( LPSTR lpszDomainName, DWORD nBuffSize )
{
    BOOL    bRet = FALSE;
    DWORD   dwBufferSize = nBuffSize;

    //
    // For Win9x systems, we can peek at a registry key for this.
    //
    if( !IsWinNT() )
    {
        HKEY hKey;
        LONG lResult;
        DWORD dwType = REG_SZ;

        // Clear buffer.
        ZeroMemory( lpszDomainName, nBuffSize );

        // Open MSNetwork key.
        lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                        _T("System\\CurrentControlSet\\Services\\MSNP32\\NetworkProvider"),
                        0,
                        KEY_READ,
                        &hKey );

        if( ERROR_SUCCESS == lResult )
        {
            // Read data.
            lResult = SymSaferRegQueryValueEx( hKey,
                                        _T("AuthenticatingAgent"),
                                        NULL,
                                        &dwType,
                                        (LPBYTE) lpszDomainName,
                                        &dwBufferSize );

            if( ERROR_SUCCESS == lResult )
                bRet = TRUE;

            // Cleanup.
            RegCloseKey(hKey);

            // If the buffer comes back empty, then get the workgroup
            if (_tcslen(lpszDomainName) == 0)
            {
                lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                _T("System\\CurrentControlSet\\Services\\VxD\\VNETSUP"),
                                0,
                                KEY_READ,
                                &hKey );
                        
                if( ERROR_SUCCESS == lResult )
                {
                    // Re-init
                    dwBufferSize = nBuffSize;

                    // Read data.
                    lResult = SymSaferRegQueryValueEx( hKey,
                                                _T("Workgroup"),
                                                NULL,
                                                &dwType,
                                                (LPBYTE) lpszDomainName,
                                                &dwBufferSize );

                    if( ERROR_SUCCESS == lResult )
                        bRet = TRUE;

                    // Cleanup.
                    RegCloseKey(hKey);
                }
            }                                   
        }
    }
    else if( g_pfnNetWkstaGetInfo )
        {
        //
        // This is ugly for NT machines.  We need to dynamically link to netapi32 in order
        // to call the function we need.  This code copied from the UNIUTIL project.
        //
        PWKSTA_INFO_100 pWkstaInfo;
        NET_API_STATUS  naStatus;
        BYTE            Buffer[ 1024 ];
        LPBYTE          pBuf = &Buffer[ 0 ];
        LPBYTE*         ppBuf = &pBuf;

        if ( lpszDomainName != NULL )
            {
            naStatus = g_pfnNetWkstaGetInfo( NULL, 100, ppBuf );

            if ( naStatus == 0 )
                {
                pWkstaInfo = ( PWKSTA_INFO_100 )pBuf;
                WideCharToMultiByte( CP_ACP,
                                     0,
                                     (LPWSTR) pWkstaInfo->wki100_langroup,
                                     -1,
                                     lpszDomainName,
                                     nBuffSize,
                                     NULL,
                                     NULL );
                bRet = TRUE;
                }
            else
                {
                bRet = FALSE;
                }
            }
        }

    
    // All done.  Return results.
    return bRet;
}    
    

////////////////////////////////////////////////////////////////////////////
// Function name    : IsWinNT
//
// Description      : Determines if the OS is WindowsNT
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 2/09/99 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL IsWinNT( void )
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    return (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
}

