///////////////////////////////////////////////////////////////////////////
// Copyright (c)1993-1997 - Bruce Hellstrom, All rights reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header: Cwstring.cpp  Revision:1.4  Sun Feb 08 19:59:20 1998  BRUCEH $
//
//  CWSTRING.CPP - This file contains member functions for the CWString
//                 class
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////

#include <afxwin.h>


#include "cwstring.h"


#ifndef _WIN32
#error This implementation of CWStrings requires Win32
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


///////////////////////////////////////////////////////////////////////////
//  Global Variables
///////////////////////////////////////////////////////////////////////////

static  int             g_nDefAlloc = MIN_ALLOC_DEFAULT;
static  UINT            g_uResIdText = 0;
static  UINT            g_uResIdCap = 0;
static  HWND            g_hParent = NULL;
static  HINSTANCE       g_hInstParent = NULL;


///////////////////////////////////////////////////////////////////////////
//  Application Registration
///////////////////////////////////////////////////////////////////////////

BOOL WINAPI RegisterCWStrings(HWND hWnd, HINSTANCE hInst, int nDefAlloc,
                              UINT uResText, UINT uResCaption)
{
    g_hParent = IsWindow(hWnd) ? hWnd : NULL;
    g_hInstParent = hInst;
    g_uResIdText = uResText;
    g_uResIdCap = uResCaption;

    if (nDefAlloc && (nDefAlloc > 0))
        g_nDefAlloc = nDefAlloc;
    else
        g_nDefAlloc = MIN_ALLOC_DEFAULT;

    return(TRUE);
}


///////////////////////////////////////////////////////////////////////////
//
//  CWString class member definitions
//
//
//
//
///////////////////////////////////////////////////////////////////////////
//  07/22/94 - BruceH - Function created
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//  Construction
///////////////////////////////////////////////////////////////////////////

CWString::CWString()
{
    m_nSize = 0;
    m_lpData = NULL;
    m_pdwExtra = new DWORD;

    Init( g_nDefAlloc );
    *m_pdwExtra = 0;

    return;
}

CWString::CWString( const CWString& strsrc )
{
    int     nBufSize;

    m_nSize = 0;
    m_lpData = NULL;
    m_pdwExtra = new DWORD;

    nBufSize = strsrc.GetAllocSize();

    if ( Init( nBufSize ) && ( strsrc.GetLength() != 0 ) )
        lstrcpy( m_lpData, strsrc );

    if ( strsrc.m_pdwExtra != NULL )
        *m_pdwExtra = *strsrc.m_pdwExtra;

    return;
}

CWString::CWString( int buffsize )
{
    m_nSize = 0;
    m_lpData = NULL;
    m_pdwExtra = new DWORD;

    Init( buffsize );
    *m_pdwExtra = 0;

    return;
}


///////////////////////////////////////////////////////////////////////////
//  Destruction
///////////////////////////////////////////////////////////////////////////

CWString::~CWString()
{
    DestroyBuf();

    if ( m_pdwExtra != NULL )
        delete m_pdwExtra;

    return;
}


///////////////////////////////////////////////////////////////////////////
//  Public operations
///////////////////////////////////////////////////////////////////////////

int CWString::StrLen( void ) const
{
    int         nLength = 0;

    if ( CheckValid() )
        nLength = GetLength();

    return( nLength );
}


int CWString::GetLength( void ) const
{
    int         nLength = 0;

    if ( CheckValid() )
        nLength = lstrlen( m_lpData );

    return( nLength );
}


int CWString::GetAllocSize( void ) const
{
    return( m_nSize );
}


void CWString::Empty( void )
{
    ZeroBuf();
    return;
}


BOOL CWString::IsEmpty( void ) const
{
    BOOL        bRet = TRUE;

    if ( ( m_lpData != NULL ) && *m_lpData )
        bRet = FALSE;

    return( bRet );
}


void CWString::FreeString( void )
{
    DestroyBuf();
    return;
}


BOOL CWString::NewSize( int buffsize )
{
    BOOL        bRet = TRUE;

    if ( !ReAllocBuf( buffsize ) )
        bRet = FALSE;

    return( TRUE );
}


LPTSTR CWString::GetPtr( void )
{
    return( m_lpData );
}


LPTSTR CWString::GetPtrToChar( int nIndex )
{
    LPTSTR      lpRet = m_lpData;

    CWASSERT( nIndex >= 0 && nIndex < GetAllocSize() );

    if ( m_lpData != NULL )
    {
        for( int nCtr = 0; nCtr < nIndex; nCtr++ )
            lpRet = CharNext( lpRet );
    }

    return( lpRet );
}



LPTSTR CWString::FindEndOfString()
{
    LPTSTR      lpRet = m_lpData;

    if ( CheckValid() )
    {
        while( *lpRet )
            lpRet = AnsiNext( lpRet );
    }

    return( lpRet );
}


LPTSTR CWString::FindLastChar()
{
    LPTSTR lpRet = FindEndOfString();

    if ( lpRet != NULL )
        lpRet = AnsiPrev( m_lpData, lpRet );

    return( lpRet );
}


CWString CWString::Left( int nChars )
{
    LPTSTR          lpPtr;
    CWString        strTemp( GetAllocSize() + 1 );

    strTemp = m_lpData;
    lpPtr = strTemp;

    for( int nCtr = 0; nCtr <= ( nChars - 1 ) && *lpPtr != '\0'; nCtr++ )
        lpPtr = CharNext( lpPtr );

    if ( *lpPtr != '\0' )
    {
        *lpPtr++ = 0;
        *lpPtr = 0;
    }

    return( strTemp );
}



CWString CWString::Mid( int nStart, int nChars )
{
    LPTSTR          lpPtr;
    CWString        strTemp( GetAllocSize() + 1 );

    if ( nStart < StrLen() )
    {
        lpPtr = m_lpData;

        for( int nCtr = 0; nCtr < nStart; nCtr++ )
            lpPtr = CharNext( lpPtr );

        strTemp = lpPtr;
        lpPtr = strTemp;

        for( nCtr = 0; nCtr < nChars && *lpPtr != '\0'; nCtr++ )
            lpPtr = CharNext( lpPtr );

        if ( *lpPtr != '\0' )
        {
            *lpPtr++ = 0;
            *lpPtr = 0;
        }
    }

    return( strTemp );
}



CWString CWString::Right( int nChars )
{
    LPTSTR          lpPtr;
    CWString        strTemp( GetAllocSize() + 1 );

    lpPtr = FindEndOfString();

    for( int nCtr = 0; nCtr < nChars; nCtr++ )
        lpPtr = CharPrev( m_lpData, lpPtr );

    strTemp = lpPtr;

    return( strTemp );
}


BOOL CWString::SetExtraData( DWORD dwData )
{
    BOOL        bRet = TRUE;

    if ( m_pdwExtra == NULL )
        m_pdwExtra = new DWORD;

    *m_pdwExtra = dwData;

    return( bRet );
}



DWORD CWString::GetExtraData( void )
{
    DWORD   dwRet;

    if ( m_pdwExtra == NULL )
    {
        m_pdwExtra = new DWORD;
        *m_pdwExtra = 0;
    }

    dwRet = *m_pdwExtra;

    return( dwRet );
}



int CWString::FindChar( UINT tcFind )
{
    int     nRet = -1;
    int     nIndex = 0;

    if ( ( m_lpData != NULL ) && *m_lpData )
    {
        LPTSTR lpPtr = m_lpData;

        while( *lpPtr != '\0' )
        {
            if ( CWSIsDBCSLeadByte( *lpPtr ) )
            {
                if ( *(UINT *)lpPtr == tcFind )
                {
                    nRet = nIndex;
                    break;
                }
            }
            else
            {
                if ( *lpPtr == (TCHAR)tcFind )
                {
                    nRet = nIndex;
                    break;
                }
            }

            lpPtr = CharNext( lpPtr );
            nIndex++;
        }
    }

    return( nRet );
}



int CWString::FindCharRev( UINT tcFind )
{
    int     nRet = -1;
    int     nIndex = 0;

    if ( ( m_lpData != NULL ) && *m_lpData )
    {
        LPTSTR lpPtr = FindLastChar();
        nIndex = StrLen() - 1;

        do
        {
            if ( CWSIsDBCSLeadByte( *lpPtr ) )
            {
                if ( *(UINT *)lpPtr == tcFind )
                {
                    nRet = nIndex;
                    break;
                }
            }
            else
            {
                if ( *lpPtr == (TCHAR)tcFind )
                {
                    nRet = nIndex;
                    break;
                }
            }

            if ( lpPtr == m_lpData )
                break;

            lpPtr = CharPrev( m_lpData, lpPtr );
            nIndex--;
        }
        while( lpPtr >= m_lpData );
    }

    return( nRet );
}



int CWString::FindString( LPTSTR lpSubStr, BOOL bCaseSensitive )
{
    int         nRet = -1;
    int         nIndex = 0;
    int         nSubSize = lstrlen( lpSubStr );
    LPTSTR      lpStr = m_lpData;
    CWString    strTemp( GetAllocSize() + 1 );

    if ( m_lpData != NULL && *m_lpData &&
         lstrlen( lpSubStr ) <= StrLen() && lpSubStr != NULL && *lpSubStr )
    {
        do
        {
            strTemp = Mid( nIndex, nSubSize );

            if ( bCaseSensitive )
            {
                if ( !lstrcmp( strTemp, lpSubStr ) )
                {
                    nRet = nIndex;
                    break;
                }
            }
            else
            {
                if ( !lstrcmpi( strTemp, lpSubStr ) )
                {
                    nRet = nIndex;
                    break;
                }
            }

            nIndex++;
        }
        while( strTemp.StrLen() == lstrlen( lpSubStr ) );
    }

    return( nRet );
}



int CWString::FindString( int nStart, LPTSTR lpSubStr, BOOL bCaseSensitive )
{
    int         nRet = -1;
    int         nIndex = nStart;
    int         nSubSize = lstrlen( lpSubStr );
    LPTSTR      lpStr = m_lpData;
    CWString    strTemp( GetAllocSize() + 1 );

    if ( m_lpData != NULL && *m_lpData &&
         ( lstrlen( lpSubStr ) <= ( StrLen() - nStart ) ) &&
         lpSubStr != NULL && *lpSubStr )
    {
        do
        {
            strTemp = Mid( nIndex, nSubSize );

            if ( bCaseSensitive )
            {
                if ( !lstrcmp( strTemp, lpSubStr ) )
                {
                    nRet = nIndex;
                    break;
                }
            }
            else
            {
                if ( !lstrcmpi( strTemp, lpSubStr ) )
                {
                    nRet = nIndex;
                    break;
                }
            }

            nIndex++;
        }
        while( strTemp.StrLen() > lstrlen( lpSubStr ) );
    }

    return( nRet );
}




TCHAR CWString::GetAt( int nIndex ) const
{
    TCHAR       cRet = 0;

    if ( ( nIndex < m_nSize ) && ( m_lpData != NULL ) )
        cRet = m_lpData[ nIndex ];

    return( cRet );
}

void CWString::SetAt( int nIndex, TCHAR ch )
{
    if ( nIndex < m_nSize )
        m_lpData[ nIndex ] = ch;

    return;
}


void CWString::MakeLowerAnsi( void )
{
    if ( m_nSize && ( m_lpData != NULL ) )
        CharLower( m_lpData );

    return;
}


void CWString::MakeUpperAnsi( void )
{
    if ( m_nSize && ( m_lpData != NULL ) )
        CharUpper( m_lpData );

    return;
}


void CWString::AnsiToOem( void )
{
    if ( m_nSize && ( m_lpData != NULL ) )
    {
#ifdef _UNICODE

        LPTSTR      lpTemp = new TCHAR[ GetAllocSize() ];

        ::CharToOem( m_lpData, lpTemp );
        lstrcpy( m_lpData, lpTemp );
        delete lpTemp;

#else

        ::CharToOem( m_lpData, m_lpData );

#endif

    }

    return;
}


void CWString::OemToAnsi(void)
{
    if ( m_nSize && ( m_lpData != NULL ) )
    {
#ifdef _UNICODE

        LPTSTR      lpTemp = new TCHAR[ GetAllocSize() ];

        ::OemToChar( m_lpData, lpTemp );
        lstrcpy( m_lpData, lpTemp );
        delete lpTemp;

#else

        ::OemToChar( m_lpData, m_lpData );

#endif

    }

    return;
}


void CWString::StripEndWhiteSpace( void )
{
    if ( CheckValid() )
    {
        LPTSTR lpStr = FindLastChar();

        do
        {
            if ( *lpStr == '\n' || *lpStr == '\r' ||
                 *lpStr == '\t' || *lpStr == ' ' )
                *lpStr = 0;
            else
                break;
        }
        while( ( lpStr = CharPrev( m_lpData, lpStr ) ) != m_lpData );
    }

    return;
}



void CWString::StripBeginWhiteSpace( void )
{
    if ( CheckValid() )
    {
        CWString        szTemp( StrLen() + 1 );

        szTemp = m_lpData;

        LPTSTR lpStr = szTemp;

        while( ( *lpStr ) && ( *lpStr == '\n' || *lpStr == '\r' ||
               *lpStr == '\t' || *lpStr == ' ' ) )
            lpStr = CharNext( lpStr );

        lstrcpy( m_lpData, lpStr );
    }
    return;
}




BOOL CWString::StripFileName( void )
{
    BOOL        bRet = FALSE;

    if ( CheckValid() )
    {
        LPTSTR lpStr = FindLastChar();

        do
        {
            if ( *lpStr == '\\' )
                bRet = TRUE;

            if ( CWSIsDBCSLeadByte( *lpStr ) )
            {
                *lpStr = 0;
                *( lpStr + 1 ) = 0;
            }
            else
            {
                *lpStr = 0;
            }
        }
        while( ( ( lpStr = CharPrev( m_lpData, lpStr ) ) != m_lpData ) && !bRet );

        if ( lpStr == m_lpData || *lpStr == '\\' )
        {
            *lpStr = 0;
            bRet = TRUE;
        }
    }

    return( bRet );
}



BOOL CWString::StripExtension( void )
{
    BOOL        bRet = FALSE;

    if ( CheckValid() )
    {
        LPTSTR lpStr = FindLastChar();

        do
        {
            if ( *lpStr == '.' )
            {
                bRet = TRUE;
                *lpStr = 0;
            }
        }
        while( ( ( lpStr = AnsiPrev( m_lpData, lpStr ) ) != m_lpData ) && !bRet );
    }

    return( bRet );
}



void CWString::AppendFile(LPSTR lpFile)
{
    if ( CheckValid() )
    {
        LPTSTR lpStr = FindLastChar();

        if ( *lpStr != '\\' && lpStr != m_lpData )
            lstrcat( m_lpData, "\\" );

        lstrcat( m_lpData, lpFile );
    }

    return;
}



void _cdecl CWString::Format( LPTSTR lpFmt, ... )
{
    if ( CheckValid() )
    {
    	va_list lpArgs;
    	va_start( lpArgs, lpFmt );
    	wvsprintf( m_lpData, lpFmt, lpArgs );
    }

    return;
}


///////////////////////////////////////////////////////////////////////////
//  Operator overloading
///////////////////////////////////////////////////////////////////////////

TCHAR& CWString::operator[]( int nIndex )
{
    if ( nIndex < m_nSize )
        return( m_lpData[ nIndex ] );
    else
        return( m_lpData[ m_nSize - 1 ] );
}


TCHAR& CWString::operator[]( WORD wIndex )
{
    if ( wIndex < (WORD)m_nSize )
        return( m_lpData[ wIndex ] );
    else
        return( m_lpData[ m_nSize - 1 ] );
}


CWString::operator LPTSTR()
{
    return( m_lpData );
}

CWString::operator LPCTSTR() const
{
    return( (LPCTSTR)m_lpData );
}


const CWString& CWString::operator=( const CWString& strsrc )
{
    BOOL        bAlloc = TRUE;

    if ( strsrc.GetAllocSize() > m_nSize )
        bAlloc = ReAllocBuf( strsrc.GetAllocSize() );

    if ( bAlloc && strsrc.m_lpData != NULL )
        lstrcpy( m_lpData, strsrc.m_lpData );
    else
        Empty();

    if ( strsrc.m_pdwExtra != NULL &&
         !IsBadReadPtr( (LPVOID)strsrc.m_pdwExtra, 4 ) )
        SetExtraData( *strsrc.m_pdwExtra );
    else
        *m_pdwExtra = 0;

    return( *this );
}


const CWString& CWString::operator=( TCHAR ch )
{
    if ( !m_nSize )
        AllocBuf( g_nDefAlloc );

    ZeroBuf();
    *m_lpData = ch;

    return( *this );
}


const CWString& CWString::operator=( LPTSTR lpData )
{
    int         nLength;

    if ( lpData != NULL )
    {
        nLength = lstrlen( lpData );

        if ( nLength >= m_nSize )
            ReAllocBuf( nLength + 1 );

        lstrcpy( m_lpData, lpData );
    }
    else
        ZeroBuf();

    return( *this );
}


const CWString& CWString::operator=( LPCTSTR lpData )
{
    int         nLength;

    if ( lpData != NULL )
    {
        nLength = lstrlen( lpData );

        if ( nLength >= m_nSize )
            ReAllocBuf( nLength + 1 );

        lstrcpy( m_lpData, lpData );
    }
    else
        ZeroBuf();

    return( *this );
}



char& CWString::operator*()
{
    return( *m_lpData );
}


const CWString& CWString::operator+( const CWString& strAdd )
{
    int         nLen = strAdd.StrLen() + StrLen();

    if ( nLen > m_nSize )
    {
        LPTSTR      lpTemp = new TCHAR[ StrLen() + 1 ];
        lstrcpy( lpTemp, m_lpData );
        ReAllocBuf( nLen + 1 );
        lstrcpy( m_lpData, lpTemp );
        delete lpTemp;
    }

    lstrcat( m_lpData, strAdd );

    return( *this );
}


const CWString& CWString::operator+( LPCTSTR lpAdd )
{
    int         nLen = lstrlen( lpAdd ) + StrLen();

    if ( nLen > m_nSize )
    {
        LPTSTR      lpTemp = new TCHAR[ StrLen() + 1 ];
        lstrcpy( lpTemp, m_lpData );
        ReAllocBuf( nLen + 1 );
        lstrcpy( m_lpData, lpTemp );
        delete lpTemp;
    }

    lstrcat( m_lpData, lpAdd );

    return( *this );
}


const CWString& CWString::operator+( TCHAR cAdd )
{
    int         nLen = StrLen() + 1;

    if ( nLen > m_nSize )
    {
        LPTSTR      lpTemp = new TCHAR[ StrLen() + 1 ];
        lstrcpy( lpTemp, m_lpData );
        ReAllocBuf( nLen + 1 );
        lstrcpy( m_lpData, lpTemp );
        delete lpTemp;
    }

    LPTSTR lpPtr = m_lpData;
    lpPtr += StrLen();
    *lpPtr++ = cAdd;
    *lpPtr = 0;

    return(*this);
}


const CWString& CWString::operator+=( const CWString& strAdd )
{
    int         nLen = strAdd.StrLen() + StrLen();

    if ( nLen > m_nSize )
    {
        LPTSTR      lpTemp = new TCHAR[ StrLen() + 1 ];
        lstrcpy( lpTemp, m_lpData );
        ReAllocBuf( nLen + 1 );
        lstrcpy( m_lpData, lpTemp );
        delete lpTemp;
    }

    lstrcat( m_lpData, strAdd );

    return( *this );
}


const CWString& CWString::operator+=( LPCTSTR lpAdd )
{
    int         nLen = lstrlen( lpAdd ) + StrLen();

    if ( nLen > m_nSize )
    {
        LPTSTR      lpTemp = new TCHAR[ StrLen() + 1 ];
        lstrcpy( lpTemp, m_lpData );
        ReAllocBuf( nLen + 1 );
        lstrcpy( m_lpData, lpTemp );
        delete lpTemp;
    }

    lstrcat( m_lpData, lpAdd );

    return( *this );
}


const CWString& CWString::operator+=( TCHAR cAdd )
{
    int         nLen = StrLen() + 1;

    if ( nLen > m_nSize )
    {
        LPTSTR      lpTemp = new TCHAR[ StrLen() + 1 ];
        lstrcpy( lpTemp, m_lpData );
        ReAllocBuf( nLen + 1 );
        lstrcpy( m_lpData, lpTemp );
        delete lpTemp;
    }

    LPTSTR lpPtr = m_lpData;
    lpPtr += StrLen();
    *lpPtr++ = cAdd;
    *lpPtr = 0;

    return( *this );
}



LPTSTR WINAPI operator+( CWString& cwStr, int nIndex )
{
    LPTSTR       lpRet;

    if ( nIndex < cwStr.GetAllocSize() )
        lpRet = cwStr.GetPtr() + nIndex;
    else
        lpRet = cwStr.GetPtr() + ( cwStr.GetAllocSize() - 1 );

    return( lpRet );
}



LPTSTR WINAPI operator+( int nIndex, CWString& cwStr )
{
    LPTSTR       lpRet;

    if ( nIndex < cwStr.GetAllocSize() )
        lpRet = cwStr.GetPtr() + nIndex;
    else
        lpRet = cwStr.GetPtr() + ( cwStr.GetAllocSize() - 1 );

    return( lpRet );
}


///////////////////////////////////////////////////////////////////////////
//  Protected operations
///////////////////////////////////////////////////////////////////////////

BOOL CWString::Init( int buffsize  )
{
    BOOL        bRet = FALSE;

    if ( m_nSize && ( m_lpData != NULL ) )
        DestroyBuf();

    if ( AllocBuf( buffsize ) )
        bRet = TRUE;

    return( bRet );
}


BOOL CWString::AllocBuf( int buffsize )
{
    BOOL        bRet = TRUE;

    if ( !buffsize )
    {
        m_nSize = 0;
        m_lpData = NULL;
    }
    else
    {
        m_lpData = new TCHAR[ buffsize ];
        m_nSize = buffsize;
        ZeroBuf();
    }

    return( bRet );
}


void CWString::DestroyBuf( void )
{
    if ( m_lpData != NULL )
        delete m_lpData;

    m_nSize = 0;
    m_lpData = NULL;

    if ( m_pdwExtra != NULL )
    {
        delete m_pdwExtra;
        m_pdwExtra = NULL;
    }

    return;
}


BOOL CWString::ReAllocBuf( int nNewSize )
{
    BOOL        bRet = FALSE;

    DestroyBuf();
    bRet = AllocBuf( nNewSize );

    return( bRet );
}


int CWString::CWStringAssert()
{
    TCHAR      szMessage[ MAX_CWSERRMSG_SIZE ];
    TCHAR      szCaption[ MAX_CWSERRCAP_SIZE ];

    *szMessage = *szCaption = 0;

    if ( g_uResIdText && g_hInstParent )
        LoadString( g_hInstParent, g_uResIdText, szMessage, sizeof( szMessage ) );

    if ( g_uResIdCap && g_hInstParent )
        LoadString( g_hInstParent, g_uResIdCap, szCaption, sizeof( szCaption ) );

    if ( *szMessage )
        MessageBox( g_hParent == NULL ? GetActiveWindow() : g_hParent,
                    szMessage, szCaption, MB_OK | MB_SYSTEMMODAL );

    CWASSERT( 0 );

    return( TRUE );
}


void CWString::ZeroBuf( void )
{
    if ( CheckValid() )
        memset( m_lpData, 0, m_nSize * sizeof( TCHAR ) );

    return;
}


LPTSTR CWString::DBCSCopyChar( LPTSTR lpSrc, LPTSTR lpDest )
{
    LPTSTR      lpRet = lpDest;
    LPTSTR      lpSrcPtr = lpSrc;
    LPTSTR      lpDestPtr = lpDest;

    if ( lpSrc != NULL && lpDest != NULL )
    {
        if ( CWSIsDBCSLeadByte( *lpSrcPtr ) )
            *lpDestPtr++ = *lpSrcPtr++;

        *lpDestPtr = *lpSrcPtr;
    }

    return( lpRet );
}


BOOL CWString::CWSIsDBCSLeadByte( BYTE ch )
{
    BOOL            bRet = FALSE;

#ifndef _UNICODE

    bRet = IsDBCSLeadByte( ch );

#endif;

    return( bRet );
}



///////////////////////////////////////////////////////////////////////////
//
//  CW_Class Debug Support   - These functions handle some of the debugging
//                             support of the CW_Class objects.
//
//
//
///////////////////////////////////////////////////////////////////////////
//  03/24/95 - BruceH - Function created
///////////////////////////////////////////////////////////////////////////

void WINAPI CW_AssertFailedLine( LPCTSTR lpszFileName, int nLine )
{
    int         nMsg;
    CWString    szErr( CW_MAX_PATH );

    wsprintf( (LPSTR)szErr, (LPSTR)STR_ASSERT_FAILED, lpszFileName, nLine );

	nMsg = ::MessageBox( GetActiveWindow(), szErr, "CWClass",
			             MB_SYSTEMMODAL | MB_ICONHAND | MB_ABORTRETRYIGNORE );

    if ( nMsg == IDIGNORE )
        return;
    else
    {
        if ( nMsg == IDRETRY )
        {
#ifdef _DEBUG
            DebugBreak();
#endif
            return;
        }
    }

    abort();

    return;

}


///////////////////////////////////////////////////////////////////////////
//  $Log: G:\QVCS\LOGS\BHUtils32\Cwstring.dqq $
//  
//    
//  
//  Revision 1.4  by: BRUCEH  Rev date: Sun Feb 08 19:59:22 1998
//    Fixed minor bugs
//  
//  Revision 1.3  by: BRUCEH  Rev date: Tue Feb 03 23:01:36 1998
//    
//  
//  Revision 1.2  by: BRUCEH  Rev date: Mon Jan 26 07:18:36 1998
//    1. Added new functionality to CWString
//  
//  Revision 1.1  by: BRUCEH  Rev date: Tue Aug 05 04:26:56 1997
//    Added List Object and TextFile stuff
//  
//  Revision 1.0  by: BRUCEH  Rev date: Sun Jun 29 01:03:20 1997
//    Initial revision.
//  
//  $Endlog$
//
//
//
///////////////////////////////////////////////////////////////////////////

