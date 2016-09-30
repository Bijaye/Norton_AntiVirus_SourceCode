// Copyright 1996 Symantec Corporation
//****************************************************************************
//
// $Header:   S:/UNIUTIL/VCS/navlmutl.cpv   1.2   19 May 1997 17:58:24   DSACKIN  $
//
// Description:
//
//****************************************************************************
// $Log:   S:/UNIUTIL/VCS/navlmutl.cpv  $
// 
//    Rev 1.2   19 May 1997 17:58:24   DSACKIN
// Reverted revision 1.1 because we will NEVER have to build this project
// for NON-UNICODE.
// 
//    Rev 1.0   18 Apr 1997 14:09:52   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:32   RFULLER
// Initial revision
// 
//    Rev 1.1   06 Feb 1997 22:52:16   TSmith
// Corrected added 'WINAPI' to 'NavGetDomainName' functions.
// 
//    Rev 1.0   06 Feb 1997 21:24:34   TSmith
// Initial revision.
//****************************************************************************
#include <windows.h>
#include <mbstring.h>                   // Multibyte character strings
#include <lm.h>                         // LanManager Net APIs
#include <assert.h>
#include "uniutil.h"



/*****************************************************************************
FUNCTION:
    NavGetDomainNameW

DESCRIPTION:
    Retrieves domain name that specified system is a member of.
    
PARAMETERS:
    IN  LPWSTR lpszServerName - Name of system to retrieve domain name for. Pass
                                NULL for local system.
    IN  UINT   cbDomainName - Size in bytes of buffer pointed to by lpszDomainName.
    OUT LPWSTR lpszDomainName - Buffer to hold Unicode domain name string.

RETURN VALUE:
    TRUE - Function success, lpszDomainName contains Unicode domain name string.
    FALSE - Function failure, NetWkstaGetInfo failed, lpszDomainName unchanged.
******************************************************************************
02/06/97 TSMITH  Function created.
*****************************************************************************/
BOOL WINAPI NavGetDomainNameW(
    IN  LPWSTR lpszServerName,
    IN  UINT   cbDomainName,
    OUT LPWSTR lpszDomainName
    )
    {
    PWKSTA_INFO_100 pWkstaInfo;
    NET_API_STATUS  naStatus;
    BYTE            Buffer[ 1024 ];
    LPBYTE          pBuf = &Buffer[ 0 ];
    LPBYTE*         ppBuf = &pBuf;
    BOOL            bResult = FALSE;

    assert( lpszDomainName != NULL );

    if ( lpszDomainName != NULL )
        {
        naStatus = NetWkstaGetInfo( lpszServerName, 100, ppBuf );

        if ( naStatus == 0 )
            {
            pWkstaInfo = ( PWKSTA_INFO_100 )pBuf;
            wcscpy( lpszDomainName, pWkstaInfo->wki100_langroup );
            bResult = TRUE;
            }
        else
            {
            bResult = FALSE;
            }
        }
    
    return( bResult );
    }



/*****************************************************************************
FUNCTION:
    NavGetDomainNameA

DESCRIPTION:
    Retrieves domain name that specified system is a member of.
    
PARAMETERS:
    IN  LPSTR lpszServerName - Name of system to retrieve domain name for. Pass
                               NULL for local system.
    IN  UINT  cbDomainName - Size in bytes of buffer pointed to by lpszDomainName.
    OUT LPSTR lpszDomainName - Buffer to hold Ansi domain name string.

RETURN VALUE:
    TRUE - Function success, lpszDomainName contains Ansi domain name string.
    FALSE - Function failure, NetWkstaGetInfo failed, lpszDomainName unchanged.
******************************************************************************
02/06/97 TSMITH  Function created.
*****************************************************************************/
BOOL WINAPI NavGetDomainNameA(
    IN  LPSTR lpszServerName,
    IN  UINT  cbDomainName,
    OUT LPSTR lpszDomainName
    )
    {
    WCHAR szWideServerName[ MAX_COMPUTERNAME_LENGTH + 1 ];
    WCHAR szWideDomainName[ MAX_COMPUTERNAME_LENGTH + 1 ];
    DWORD dwNumChars;
    BOOL  bResult = FALSE;

    assert( lpszDomainName != NULL );
    memset( szWideServerName, 0, sizeof( szWideServerName ) );
    memset( szWideDomainName, 0, sizeof( szWideDomainName ) );

    if ( lpszDomainName != NULL )
        {
        if ( lpszServerName != NULL )
            {
            // 
            // When lpszServerName isn't NULL, convert it to a wide
            // string and call our wide function to get the domain
            // name for whatever system name was passed in.
            // 
            dwNumChars = _mbstrlen( lpszServerName );
            mbstowcs( szWideServerName, lpszServerName, dwNumChars );
            bResult = NavGetDomainNameW( szWideServerName,
                                        sizeof( szWideDomainName ),
                                        szWideDomainName );
            }
        else
            {
            // 
            // Otherwise, pass NULL to our wide function
            // to get the domain name for the local system.
            // 
            bResult = NavGetDomainNameW( NULL,
                                        sizeof( szWideDomainName ),
                                        szWideDomainName );
            }
    
        if ( bResult )
            {
            // 
            // Convert the result to MB/SB CS
            // and copy into callers buffer.
            // 
            memset( lpszDomainName, 0, cbDomainName );
            wcstombs( lpszDomainName, szWideDomainName, cbDomainName - 1 );
            }
        }
    
    return( bResult );
    }

