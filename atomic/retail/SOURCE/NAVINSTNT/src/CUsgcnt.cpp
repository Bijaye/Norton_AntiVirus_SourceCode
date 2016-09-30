///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/CUsgcnt.cpv   1.0   18 Jul 1997 15:04:26   JALLEE  $
//
//  CUSGCNT.CPP   - This file contains class definitions for the CUsageCount
//                  Class Objects.
//
///////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/CUsgcnt.cpv  $
// 
//    Rev 1.0   18 Jul 1997 15:04:26   JALLEE
// Initial revision.
//
//    Rev 1.7   13 Jul 1997 23:44:08   BRUCEH
// 1. Fixed bug in CRegistryEntry when setting value of a key that
//    doesn't exist
// 2. Fixed bug in CUsageCount
// 3. Added support for unregistering some of our own file instead of
//    allowing IS to do it
//
//
//
//  Revision 1.1  by: BRUCEH  Rev date: Mon Jul 07 05:06:14 1997
//    Allow override of DWORD usage counts
//
//  Revision 1.0  by: BRUCEH  Rev date: Sun Jun 29 01:03:18 1997
//    Initial revision.
//
//  $Endlog$
///////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include <stdlib.h>
#include "CUsgCnt.H"


#ifndef _WIN32
#error This implementation of CUsgCnt requires Win32
#endif



///////////////////////////////////////////////////////////////////////////
// Constructors/Destructors
///////////////////////////////////////////////////////////////////////////

CUsageCount::CUsageCount( HKEY hHive, LPTSTR lpSubKey, LPTSTR lpValue )
{
    pRegent = new CRegistryEntry( hHive, lpSubKey, lpValue );
    m_dwCntType = CUC_INVALID_COUNT;

    if ( lpValue != NULL )
    {
        m_lpValue = new TCHAR[ lstrlen( lpValue ) + 1 ];
        lstrcpy( m_lpValue, lpValue );
    }

    return;
}



CUsageCount::~CUsageCount()
{
    if ( pRegent != NULL )
        delete pRegent;

    if ( m_lpValue != NULL )
        delete m_lpValue;

}



///////////////////////////////////////////////////////////////////////////
// Member functions
///////////////////////////////////////////////////////////////////////////

void CUsageCount::SetValueName( LPTSTR lpValue )
{
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

    if ( pRegent != NULL )
        pRegent->SetValueName( lpValue );

    return;
}


BOOL CUsageCount::CountExists( void )
{
    BOOL        bRet = FALSE;

    if ( m_lpValue != NULL && pRegent != NULL )
    {
        if ( GetCountType() != CUC_INVALID_COUNT )
            bRet = TRUE;
    }

    return( bRet );
}



DWORD CUsageCount::GetCountType( void )
{
    DWORD       dwRet = CUC_INVALID_COUNT;
    DWORD       dwSize = 0;
    DWORD       dwType;

    if ( m_lpValue != NULL && pRegent != NULL )
    {
        if ( pRegent->GetValueInfo( &dwType, &dwSize ) == ERROR_SUCCESS )
        {
            if ( dwType == REG_SZ || dwType == REG_DWORD )
            {
                dwRet = dwType;
                m_dwCntType = dwRet;
            }
            else
            {
                dwRet = CUC_INVALID_COUNT;
                m_dwCntType = CUC_INVALID_COUNT;
            }
        }
    }


    return( dwRet );
}



DWORD CUsageCount::GetCount( void )
{
    TCHAR       szStrCnt[ REG_MAX_STR ];
    DWORD       dwRet = CUC_INVALID_COUNT;
    DWORD       dwType;
    DWORD       dwSize = REG_MAX_STR - 1;

    if ( m_lpValue != NULL && pRegent != NULL )
    {
        if ( GetCountType() != CUC_INVALID_COUNT )
        {
            if ( m_dwCntType == REG_SZ || m_dwCntType == REG_DWORD )
                dwType = m_dwCntType;

            if ( m_dwCntType == REG_SZ )
            {
                if ( pRegent->GetValue( &dwType, (LPBYTE)szStrCnt, &dwSize ) == ERROR_SUCCESS )
                    dwRet = StrCountToDword( szStrCnt );
            }
            else
            {
                if ( m_dwCntType == REG_DWORD )
                {
                    dwSize = sizeof( DWORD );

                    if ( pRegent->GetValue( &dwType, (LPBYTE)&dwRet, &dwSize ) !=
                         ERROR_SUCCESS )
                        dwRet = CUC_INVALID_COUNT;
                }
            }
        }
    }

    return( dwRet );
}



LONG CUsageCount::SetCount( DWORD dwCount, BOOL bStrCnt )
{
    LONG        lRet = ERROR_SUCCESS;
    TCHAR       szStrCnt[ REG_MAX_STR ];
    DWORD       dwType;
    DWORD       dwSize;


    if ( m_lpValue != NULL && pRegent != NULL )
    {
        if ( GetCountType() == CUC_INVALID_COUNT )
        {
            if ( bStrCnt )
                m_dwCntType = REG_SZ;
            else
                m_dwCntType = REG_DWORD;
        }
        else
        {
            if ( bStrCnt )
                m_dwCntType = REG_SZ;
        }

        if ( m_dwCntType == REG_SZ || m_dwCntType == REG_DWORD )
            dwType = m_dwCntType;

        if ( m_dwCntType == REG_SZ )
        {
            DwordCountToStr( dwCount, szStrCnt );
            lRet = pRegent->SetValue( dwType, (LPBYTE)szStrCnt, lstrlen( szStrCnt ) + 1 );
        }
        else
        {
            if ( m_dwCntType == REG_DWORD )
            {
                dwSize = sizeof( DWORD );
                lRet = pRegent->SetValue( dwType, (LPBYTE)&dwCount, dwSize );
            }
        }
    }

    return( lRet );
}


DWORD CUsageCount::IncrementCount( void )
{
    DWORD       dwRet = CUC_INVALID_COUNT;
    DWORD       dwCount;

    dwCount = GetCount();

    if ( dwCount == CUC_INVALID_COUNT )
    {
        m_dwCntType = REG_DWORD;
        dwCount = 1;
    }
    else
    {
        dwCount++;
    }

    if ( SetCount( dwCount ) == ERROR_SUCCESS )
        dwRet = dwCount;

    return( dwRet );
}



DWORD CUsageCount::DecrementCount( BOOL bDelIfZero )
{
    DWORD       dwRet = CUC_INVALID_COUNT;
    DWORD       dwCount;

    dwCount = GetCount();

    if ( dwCount != CUC_INVALID_COUNT )
    {
        if ( dwCount > 0 )
            dwCount--;

        if ( dwCount == 0 )
        {
            if ( bDelIfZero )
            {
                pRegent->DeleteValue();
                dwRet = 0;
            }
        }

        if ( dwCount > 0 || !bDelIfZero )
        {
            if ( SetCount( dwCount ) == ERROR_SUCCESS )
                dwRet = dwCount;
        }
    }

    return( dwRet );
}



LONG CUsageCount::DeleteCount( void )
{
    LONG            lRet = ERROR_INVALID_DATA;

    if ( m_lpValue != NULL && pRegent != NULL )
    {
        lRet = pRegent->DeleteValue();
    }

    return( lRet );
}



///////////////////////////////////////////////////////////////////////////
// Protected Member functions
///////////////////////////////////////////////////////////////////////////

DWORD CUsageCount::StrCountToDword( LPTSTR lpCount )
{
    DWORD       dwRet = CUC_INVALID_COUNT;

    if ( lpCount != NULL )
    {
        dwRet = strtoul( lpCount, NULL, 10 );
    }

    return( dwRet );
}


BOOL CUsageCount::DwordCountToStr( DWORD dwCount, LPTSTR lpCount )
{
    BOOL        bRet = FALSE;

    if ( lpCount != NULL )
    {
        _ultoa( dwCount, lpCount, 10 );
        bRet = TRUE;
    }

    return( bRet );
}



