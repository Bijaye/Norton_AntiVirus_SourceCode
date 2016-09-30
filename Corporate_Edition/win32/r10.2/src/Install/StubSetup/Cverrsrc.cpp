///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/CVERRSRC.CPv   1.0   26 Oct 1998 12:39:20   mdunn  $
//
//  CVERRSRC.CPP  - This file contains class definitions for the CVersionResource
//                  Class Object.
//
//
//
///////////////////////////////////////////////////////////////////////////


//#include <afxwin.h>
#include "stdafx.h"

#if 0
#include <windows.H>
#endif
#include <tchar.h>
#include <winver.h>
#include "CVerrsrc.h"
#include "SymSaferStrings.h"

///////////////////////////////////////////////////////////////////////////
// Constructors/Destructors
///////////////////////////////////////////////////////////////////////////

CVersionResource::CVersionResource( LPCTSTR lpFileName )
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
        delete m_lpFileName;

    if ( m_lpBuff != NULL )
        delete m_lpBuff;

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

                if ( dwSize )           // Create the buffer to hold the ver resource
                {
                    dwBufSize = dwSize + 10;
                    m_lpBuff = new BYTE[ dwBufSize ];

                                        // Retrieve the version resource to our buffer
                                        // NOTE: Can't check for error return because
                                        //       sometimes the stupid function returns
                                        //       false even when it was successful

                    GetFileVersionInfo( m_lpFileName, dwHandle,
                                        dwBufSize, (LPVOID)m_lpBuff );

                                        // Try to obtain the fixed file info

                    if ( VerQueryValue( (LPVOID)m_lpBuff, "\\",
                                        (LPVOID *)&m_pvsff, &m_uFixedSize ) )
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


        sssnprintf( szValue, sizeof(szValue), lpName, LOWORD( m_dwCurLang ),
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
//  $Log:   S:/NAVINST95/VCS/CVERRSRC.CPv  $
// 
//    Rev 1.0   26 Oct 1998 12:39:20   mdunn
// Initial revision.
// 
//    Rev 1.0   18 Jul 1998 20:13:46   mdunn
// Initial revision.
//
//    Rev 1.0   14 Jul 1997 03:00:48   BRUCEH
// Initial checkin to Roswell
//
//    Rev 1.1   29 Jun 1997 15:37:32   BRUCEH
// 1. Fix in CVersionResource::FileExists
//
//
///////////////////////////////////////////////////////////////////////////





