///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header: CVerrsrc.cpp  Revision:1.4  Mon Feb 01 19:02:40 1999  BRUCEH $
//
//  CVERRSRC.CPP  - This file contains class definitions for the CVersionResource
//                  Class Object.
//
//
//
///////////////////////////////////////////////////////////////////////////


#include <afxwin.h>

#include "CVerrsrc.h"

#ifndef _WIN32
#error This implementation of CVersionResource requires Win32
#endif


///////////////////////////////////////////////////////////////////////////
// Constructors/Destructors
///////////////////////////////////////////////////////////////////////////

CVersionResource::CVersionResource( LPTSTR lpFileName )
{
    m_bInitialized = FALSE;
    m_dwCurLang = 0;
    m_lpFileName = NULL;
    m_lpBuff = NULL;
    m_pvsff = NULL;
    m_uLangSize = 0;
    m_uFixedSize = 0;

    if ( lpFileName != NULL )
    {
        m_lpFileName = new TCHAR[ lstrlen( lpFileName ) + 1 ];
        lstrcpy( m_lpFileName, lpFileName );
    }

    return;
}



CVersionResource::~CVersionResource()
{
    if ( m_lpFileName != NULL )
        delete [] m_lpFileName;

    if ( m_lpBuff != NULL )
        delete [] m_lpBuff;

    return;
}



///////////////////////////////////////////////////////////////////////////
// Public Member Functions
///////////////////////////////////////////////////////////////////////////

DWORD CVersionResource::InitializeData( void )
{
    DWORD               dwRet = CVER_NODATA_IN_FILE;
    DWORD               dwHandle = 0;
    DWORD               dwSize = 0;
    DWORD               dwBufSize;

                                        // If they've already called this function
                                        // just figure out what the return value
                                        // should be

    if ( m_bInitialized )
    {
        if ( m_lpBuff != NULL )
        {
            if ( m_pvsff != NULL )
                dwRet = 0;
            else
                dwRet = CVER_NO_FIXED_INFO;
        }
    }
    else
    {
        if ( m_lpFileName != NULL && !m_bInitialized )
        {
            if ( !FileExists( m_lpFileName ) )
                dwRet = ERROR_FILE_NOT_FOUND;
            else
            {
                                        // Get the size of the version info block

                dwSize = GetFileVersionInfoSize( m_lpFileName, &dwHandle );

                if ( dwSize >= sizeof( VS_FIXEDFILEINFO ) )          
                {
                                         // Create the buffer to hold the ver resource
                    dwBufSize = dwSize + 10;
                    m_lpBuff = new BYTE[ dwBufSize ];

                                        // Retrieve the version resource to our buffer
                                        // NOTE: GetFileVersionInfo returns FALSE on VXDs
                                        //       when it was really successful, so
                                        //       we are forced to ignore the error.
                                        //       We're getting the error here just to
                                        //       see it in the debugger.

                    if ( !GetFileVersionInfo( m_lpFileName, dwHandle,
                                              dwBufSize, (LPVOID)m_lpBuff ) )
                    {
                        DWORD dwError = GetLastError();
                    }

                                        // Try to obtain the fixed file info

                    if ( VerQueryValue( (LPVOID)m_lpBuff, "\\", (LPVOID *)&m_pvsff,
                                        &m_uFixedSize ) )
                    {
                                        // No fixed info, but a valid resource

                        if ( m_uFixedSize == 0 )
                        {
                            dwRet = CVER_NO_FIXED_INFO;
                            m_pvsff = NULL;
                        }
                        else        // We got everything OK
                            dwRet = 0;
                    }
                    else
                    {               // Some kind of error
                        m_pvsff = NULL;
                        m_uFixedSize = 0;
                    }

                    m_bInitialized = TRUE;

                }
            }
        }
    }

    return( dwRet );
}



DWORD CVersionResource::GetLanguageList( LPDWORD *lplpdwLangList, LPDWORD lpdwElements )
{
    DWORD           dwRet = CVER_NODATA_IN_FILE;

                                        // Initialize the caller's data
    *lplpdwLangList = NULL;
    *lpdwElements = 0;

    if ( !m_bInitialized )
        dwRet = CVER_NOT_INITIALIZED;
    else
    {
        if ( m_lpBuff != NULL )
        {
                                        // Query for the array of charset/language ids
                                        // available in this resource

            if ( VerQueryValue( (LPVOID)m_lpBuff, "\\VarFileInfo\\Translation",
                                (LPVOID *)&m_lpLangs, &m_uLangSize ) )
            {
                dwRet = 0;
                m_dwCurLang = *m_lpLangs;
                *lplpdwLangList = m_lpLangs;
                *lpdwElements = m_uLangSize / sizeof(DWORD);
            }
        }
    }

    return( dwRet );
}



DWORD CVersionResource::SetCurrentLanguage( DWORD dwLang )
{
    DWORD           dwRet = CVER_LANG_NOT_AVAIL;


    if ( m_dwCurLang != dwLang )
    {
                                        // If not initialized, fail
        if ( !m_bInitialized )
            dwRet = CVER_NOT_INITIALIZED;
        else
        {
            if ( m_lpLangs != NULL )
            {
                DWORD           dwElements = m_uLangSize / sizeof(DWORD);
                LPDWORD         lpdwCur = m_lpLangs;

                                        // Check to see if the lang/charset they've
                                        // requested is in our list

                for ( DWORD dwCtr = 0; dwCtr < dwElements; dwCtr++ )
                {
                    if ( *lpdwCur == dwLang )
                    {
                        m_dwCurLang = dwLang;
                        dwRet = 0;
                        break;
                    }

                    lpdwCur++;
                }
            }
        }
    }

    return( dwRet );
}



DWORD CVersionResource::GetBinaryFileVer( LPDWORD lpdwFileVerMS,
                                          LPDWORD lpdwFileVerLS ) const
{
    DWORD           dwRet = CVER_NOT_INITIALIZED;

    if ( m_bInitialized )
    {
        if ( m_pvsff != NULL )
        {
            if ( lpdwFileVerMS != NULL )
                *lpdwFileVerMS = m_pvsff->dwFileVersionMS;

            if ( lpdwFileVerLS != NULL )
                *lpdwFileVerLS = m_pvsff->dwFileVersionLS;

            dwRet = 0;
        }
        else
            dwRet = CVER_NO_FIXED_INFO;
    }

    return( dwRet );
}



DWORD CVersionResource::GetBinaryProductVer( LPDWORD lpdwProdVerMS,
                                             LPDWORD lpdwProdVerLS ) const
{
    DWORD           dwRet = CVER_NOT_INITIALIZED;

    if ( m_bInitialized )
    {
        if ( m_pvsff != NULL )
        {
            if ( lpdwProdVerMS != NULL )
                *lpdwProdVerMS = m_pvsff->dwProductVersionMS;

            if ( lpdwProdVerLS != NULL )
                *lpdwProdVerLS = m_pvsff->dwProductVersionLS;

            dwRet = 0;
        }
        else
            dwRet = CVER_NO_FIXED_INFO;
    }

    return( dwRet );
}




DWORD CVersionResource::CompareFileVer( DWORD dwCompareMS, DWORD dwCompareLS )
{
    DWORD           dwRet = CVER_NOT_INITIALIZED;


    if ( m_bInitialized )
    {
        dwRet = CVER_COMPARE_NORESRC;

        if ( m_pvsff != NULL )
        {
            dwRet = VersionCompare( m_pvsff->dwFileVersionMS,
                                    m_pvsff->dwFileVersionLS,
                                    dwCompareMS, dwCompareLS );
        }
    }


    return( dwRet );
}




DWORD CVersionResource::CompareFileVer( const CVersionResource *cver )
{
    DWORD           dwRet = CVER_NOT_INITIALIZED;


    if ( m_bInitialized && cver->IsInitialized() )
    {
        dwRet = CVER_COMPARE_NORESRC;

        if ( m_pvsff != NULL )
        {
            DWORD           dwCompareMS, dwCompareLS;

            if ( !cver->GetBinaryFileVer( &dwCompareMS, &dwCompareLS ) )
            {
                dwRet = VersionCompare( m_pvsff->dwFileVersionMS,
                                        m_pvsff->dwFileVersionLS,
                                        dwCompareMS, dwCompareLS );
            }
        }
    }


    return( dwRet );
}



LPCTSTR CVersionResource::GetStringInfo( DWORD dwItem )
{
    TCHAR           szInfoName[ CVER_STR_SIZE ];
    TCHAR           szValue[ CVER_STR_SIZE ];
    LPTSTR          lpName = _T( "\\StringFileInfo\\%04X%04X\\%s" );
    LPTSTR          lpRet = NULL;

    if ( m_bInitialized && m_lpBuff != NULL && m_dwCurLang )
    {
        switch( dwItem )
        {
            case CVER_SINFO_COMPNAME:
                lstrcpy( szInfoName, _T( "CompanyName" ) );
                break;

            case CVER_SINFO_FILEDESCRIP:
                lstrcpy( szInfoName, _T( "FileDescription" ) );
                break;

            case CVER_SINFO_FILEVER:
                lstrcpy( szInfoName, _T( "FileVersion" ) );
                break;

            case CVER_SINFO_INTERNALNAME:
                lstrcpy( szInfoName, _T( "InternalName" ) );
                break;

            case CVER_SINFO_COPYRIGHT:
                lstrcpy( szInfoName, _T( "LegalCopyright" ) );
                break;

            case CVER_SINFO_ORIGFNAME:
                lstrcpy( szInfoName, _T( "OriginalFilename" ) );
                break;

            case CVER_SINFO_PRODNAME:
                lstrcpy( szInfoName, _T( "ProductName" ) );
                break;

            case CVER_SINFO_PRODVER:
                lstrcpy( szInfoName, _T( "ProductVersion" ) );
                break;

            default:
                *szInfoName = 0;
                break;
        }

        UINT            uSize;
        LPTSTR          lpString;


        wsprintf( szValue, lpName, LOWORD( m_dwCurLang ),
                  HIWORD( m_dwCurLang ), szInfoName );

        if ( VerQueryValue( m_lpBuff, szValue, (LPVOID*)&lpString, &uSize ) )
        {
            if ( uSize )
            {
                lpRet = lpString;
            }
        }
    }


    return( lpRet );
}


BOOL CVersionResource::operator==( const CVersionResource *cver )
{
    BOOL            bRet = FALSE;

    if ( m_bInitialized && cver->IsInitialized() )
    {
        if ( m_pvsff != NULL )
        {
            DWORD           dwRet;
            DWORD           dwCompareMS, dwCompareLS;

            if ( !cver->GetBinaryFileVer( &dwCompareMS, &dwCompareLS ) )
            {
                dwRet = VersionCompare( m_pvsff->dwFileVersionMS,
                                        m_pvsff->dwFileVersionLS,
                                        dwCompareMS, dwCompareLS );

                if ( dwRet == CVER_COMPARE_EQUAL )
                    bRet = TRUE;
            }
        }
    }

    return( bRet );
}



///////////////////////////////////////////////////////////////////////////
// Protected Member Functions
///////////////////////////////////////////////////////////////////////////

DWORD CVersionResource::VersionCompare( DWORD dwCurrentMS, DWORD dwCurrentLS,
                                        DWORD dwNewMS, DWORD dwNewLS )
{
    DWORD       dwMSmaj = HIWORD( dwCurrentMS );
    DWORD       dwMSmin = LOWORD( dwCurrentMS );
    DWORD       dwLSmaj = HIWORD( dwCurrentLS );
    DWORD       dwLSmin = LOWORD( dwCurrentLS );
    DWORD       dwCMSmaj = HIWORD( dwNewMS );
    DWORD       dwCMSmin = LOWORD( dwNewMS );
    DWORD       dwCLSmaj = HIWORD( dwNewLS );
    DWORD       dwCLSmin = LOWORD( dwNewLS );
    DWORD       dwRet = CVER_COMPARE_EQUAL;

    dwRet = dwMSmaj == dwCMSmaj ? CVER_COMPARE_EQUAL :
            dwMSmaj > dwCMSmaj  ? CVER_COMPARE_NEWER : CVER_COMPARE_OLDER;

    if ( dwRet == CVER_COMPARE_EQUAL )
    {
        dwRet = dwMSmin == dwCMSmin ? CVER_COMPARE_EQUAL :
                dwMSmin > dwCMSmin  ? CVER_COMPARE_NEWER : CVER_COMPARE_OLDER;
    }

    if ( dwRet == CVER_COMPARE_EQUAL )
    {
        dwRet = dwLSmaj == dwCLSmaj ? CVER_COMPARE_EQUAL :
                dwLSmaj > dwCLSmaj  ? CVER_COMPARE_NEWER : CVER_COMPARE_OLDER;
    }

    if ( dwRet == CVER_COMPARE_EQUAL )
    {
        dwRet = dwLSmin == dwCLSmin ? CVER_COMPARE_EQUAL :
                dwLSmin > dwCLSmin  ? CVER_COMPARE_NEWER : CVER_COMPARE_OLDER;
    }


    return( dwRet );
}



BOOL CVersionResource::FileExists( LPTSTR lpFile )
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    WIN32_FIND_DATA     fd;

    if ( lpFile != NULL && *lpFile )
    {
        hFind = FindFirstFile( lpFile, &fd );

        if ( hFind != INVALID_HANDLE_VALUE )
        {
            do
            {
                if ( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                     *fd.cFileName != '.' && *fd.cAlternateFileName != '.' )
                {
                    bRet = TRUE;
                    break;
                }
            }
            while( FindNextFile( hFind, &fd ) );

            FindClose( hFind );
        }
    }


    return( bRet );
}



///////////////////////////////////////////////////////////////////////////
//  $Log: G:\QVCS\Arch\LOGS\BHUtils32\CVerrsrc.dqq $
//  
//    
//  
//  Revision 1.4  by: BRUCEH  Rev date: Mon Feb 01 19:02:40 1999
//    Remove error checking on GetFileVersionInfo because of a bug in Windows
//    that makes it fail on VXDs when it really worked.
//  
//  Revision 1.3  by: BRUCEH  Rev date: Tue Dec 29 02:13:34 1998
//    Convert to VC6 and fix bugs found by VC6 compiler
//  
//  Revision 1.2  by: BRUCEH  Rev date: Sun Feb 08 21:07:58 1998
//    Workaround for bug in GetFileVersionInfo
//  
//  Revision 1.1  by: BRUCEH  Rev date: Mon Aug 25 03:48:22 1997
//    Added CCommandLine Object
//  
//  Revision 1.0  by: BRUCEH  Rev date: Sun Jun 29 01:03:18 1997
//    Initial revision.
//  
//  $Endlog$
//
//
///////////////////////////////////////////////////////////////////////////





