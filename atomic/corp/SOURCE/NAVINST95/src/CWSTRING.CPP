///////////////////////////////////////////////////////////////////////////
// Copyright (c)1996,1997 - Bruce Hellstrom, All rights reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/Cwstring.cpv   1.1   16 Jul 1997 16:55:46   JALLEE  $
//
//  CWSTRING.CPP - This file contains member functions for the CWString
//                 class
//
///////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/Cwstring.cpv  $
// 
//    Rev 1.1   16 Jul 1997 16:55:46   JALLEE
// Improvements to CWString class updated from Borg.
//
//    Rev 1.0   14 May 1997 17:10:58   jtaylor
// Initial revision.
//
//    Rev 1.0   05 May 1997 19:24:46   sedward
// Initial revision.
//
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <string.h>
#include <process.h>

#include "cwstring.h"


#ifndef _WIN32
#error This implementation of CWStrings requires Win32
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

    Init(g_nDefAlloc);
    return;
}

CWString::CWString(const CWString& strsrc)
{
    int     nBufSize;

    m_nSize = 0;
    m_lpData = NULL;

    nBufSize = strsrc.GetAllocSize();

    if (Init(nBufSize) && (strsrc.GetLength() != 0))
        lstrcpy(m_lpData, strsrc);

    return;
}

CWString::CWString(int buffsize)
{
    m_nSize = 0;
    m_lpData = NULL;

    Init(buffsize);

    return;
}


///////////////////////////////////////////////////////////////////////////
//  Destruction
///////////////////////////////////////////////////////////////////////////

CWString::~CWString()
{
    DestroyBuf();
    return;
}


///////////////////////////////////////////////////////////////////////////
//  Public operations
///////////////////////////////////////////////////////////////////////////

int CWString::StrLen(void) const
{
    int         nLength = 0;

    if (CheckValid())
        nLength = GetLength();

    return(nLength);
}


int CWString::GetLength(void) const
{
    int         nLength = 0;

    if (CheckValid())
        nLength = lstrlen(m_lpData);

    return(nLength);
}


int CWString::GetAllocSize(void) const
{
    return(m_nSize);
}


void CWString::Empty(void)
{
    ZeroBuf();
    return;
}


BOOL CWString::IsEmpty(void) const
{
    BOOL        bRet = TRUE;

    if ((m_lpData != NULL) && *m_lpData)
        bRet = FALSE;

    return(bRet);
}


void CWString::FreeString(void)
{
    DestroyBuf();
    return;
}


BOOL CWString::NewSize(int buffsize)
{
    BOOL        bRet = TRUE;

    if (!ReAllocBuf(buffsize))
        bRet = FALSE;

    return(TRUE);
}


LPTSTR CWString::GetPtr(void)
{
    return(m_lpData);
}


TCHAR CWString::GetAt(int nIndex) const
{
    TCHAR      cRet = 0;

    if ((nIndex < m_nSize) && (m_lpData != NULL))
        cRet = m_lpData[nIndex];

    return(cRet);
}

void CWString::SetAt(int nIndex, TCHAR ch)
{
    if (nIndex < m_nSize)
        m_lpData[nIndex] = ch;

    return;
}


void CWString::MakeLowerAnsi(void)
{
    if (m_nSize && (m_lpData != NULL))
        AnsiLower(m_lpData);

    return;
}


void CWString::MakeUpperAnsi(void)
{
    if (m_nSize && (m_lpData != NULL))
        AnsiUpper(m_lpData);

    return;
}


void CWString::AnsiToOem(void)
{
    if (m_nSize && (m_lpData != NULL))
        ::AnsiToOem(m_lpData, m_lpData);

    return;
}


void CWString::OemToAnsi(void)
{
    if (m_nSize && (m_lpData != NULL))
        ::OemToAnsi(m_lpData, m_lpData);

    return;
}


BOOL CWString::StripFileName(void)
{
    BOOL        bRet = FALSE;

    if (CheckValid())
    {
        LPTSTR lpStr = FindLastChar();

        do
        {
            if (*lpStr == '\\')
                bRet = TRUE;

            *lpStr = 0;
        }
        while(((lpStr = AnsiPrev(m_lpData, lpStr)) != m_lpData) && !bRet);

        if (lpStr == m_lpData && *lpStr != '\\')
        {
            *lpStr = 0;
            bRet = TRUE;
        }
    }

    return(bRet);
}



BOOL CWString::StripExtension(void)
{
    BOOL        bRet = FALSE;

    if (CheckValid())
    {
        LPTSTR lpStr = FindLastChar();

        do
        {
            if (*lpStr == '.')
            {
                bRet = TRUE;
                *lpStr = 0;
            }
        }
        while(((lpStr = AnsiPrev(m_lpData, lpStr)) != m_lpData) && !bRet);
    }

    return(bRet);
}



void CWString::AppendFile(LPSTR lpFile)
{
    if (CheckValid())
    {
        LPTSTR lpStr = FindLastChar();

        if (*lpStr != '\\' && lpStr != m_lpData)
            lstrcat(m_lpData, "\\");

        lstrcat(m_lpData, lpFile);
    }

    return;

}



void _cdecl CWString::Format(LPTSTR lpFmt, ...)
{
    if (CheckValid())
    {
        LPTSTR lpArgs = (LPTSTR)&lpFmt + sizeof(lpFmt);
        wvsprintf(m_lpData, lpFmt, lpArgs );
    }

    return;
}


///////////////////////////////////////////////////////////////////////////
//  Operator overloading
///////////////////////////////////////////////////////////////////////////

TCHAR& CWString::operator[](int nIndex)
{
    if (nIndex < m_nSize)
        return(m_lpData[nIndex]);
    else
        return(m_lpData[m_nSize - 1]);
}


TCHAR& CWString::operator[](WORD wIndex)
{
    if (wIndex < (WORD)m_nSize)
        return(m_lpData[wIndex]);
    else
        return(m_lpData[m_nSize - 1]);
}


CWString::operator LPTSTR()
{
    return(m_lpData);
}

CWString::operator LPCTSTR() const
{
    return((LPCTSTR)m_lpData);
}


const CWString& CWString::operator=(const CWString& strsrc)
{
    BOOL        bAlloc = TRUE;

    if (strsrc.GetAllocSize() > m_nSize)
        bAlloc = ReAllocBuf(strsrc.GetAllocSize());

    if (bAlloc)
        lstrcpy(m_lpData, strsrc);

    return(*this);
}


const CWString& CWString::operator=(TCHAR ch)
{
    if (!m_nSize)
        AllocBuf(g_nDefAlloc);

    ZeroBuf();
    *m_lpData = ch;

    return(*this);
}


const CWString& CWString::operator=(LPTSTR lpData)
{
    int         nLength;

    if (lpData != NULL)
    {
        nLength = lstrlen(lpData);

        if (nLength >= m_nSize)
            ReAllocBuf(nLength + 1);

        lstrcpy(m_lpData, lpData);
    }
    else
        ZeroBuf();

    return(*this);
}


char& CWString::operator*()
{
    return(*m_lpData);
}


const CWString& CWString::operator+(const CWString& strAdd)
{
    int         nLen = strAdd.StrLen() + StrLen();

    if (nLen > m_nSize)
    {
        LPTSTR      lpTemp = new TCHAR[StrLen() + 1];
        lstrcpy(lpTemp, m_lpData);
        ReAllocBuf( nLen + 1 );
        lstrcpy(m_lpData, lpTemp);
        delete lpTemp;
    }

    lstrcat(m_lpData, strAdd);

    return(*this);
}


const CWString& CWString::operator+(LPCTSTR lpAdd)
{
    int         nLen = lstrlen(lpAdd) + StrLen();

    if (nLen > m_nSize)
    {
        LPTSTR      lpTemp = new TCHAR[StrLen() + 1];
        lstrcpy(lpTemp, m_lpData);
        ReAllocBuf( nLen + 1 );
        lstrcpy(m_lpData, lpTemp);
        delete lpTemp;
    }

    lstrcat(m_lpData, lpAdd);

    return(*this);
}


const CWString& CWString::operator+(TCHAR cAdd)
{
    int         nLen = StrLen() + 1;

    if (nLen > m_nSize)
    {
        LPTSTR      lpTemp = new TCHAR[StrLen() + 1];
        lstrcpy(lpTemp, m_lpData);
        ReAllocBuf( nLen + 1 );
        lstrcpy(m_lpData, lpTemp);
        delete lpTemp;
    }

    LPTSTR lpPtr = m_lpData;
    lpPtr += StrLen();
    *lpPtr++ = cAdd;
    *lpPtr = 0;

    return(*this);
}


const CWString& CWString::operator+=(const CWString& strAdd)
{
    int         nLen = strAdd.StrLen() + StrLen();

    if (nLen > m_nSize)
    {
        LPTSTR      lpTemp = new TCHAR[StrLen() + 1];
        lstrcpy(lpTemp, m_lpData);
        ReAllocBuf( nLen + 1 );
        lstrcpy(m_lpData, lpTemp);
        delete lpTemp;
    }

    lstrcat(m_lpData, strAdd);

    return(*this);
}


const CWString& CWString::operator+=(LPCTSTR lpAdd)
{
    int         nLen = lstrlen(lpAdd) + StrLen();

    if (nLen > m_nSize)
    {
        LPTSTR      lpTemp = new TCHAR[StrLen() + 1];
        lstrcpy(lpTemp, m_lpData);
        ReAllocBuf( nLen + 1 );
        lstrcpy(m_lpData, lpTemp);
        delete lpTemp;
    }

    lstrcat(m_lpData, lpAdd);

    return(*this);
}


const CWString& CWString::operator+=(TCHAR cAdd)
{
    int         nLen = StrLen() + 1;

    if (nLen > m_nSize)
    {
        LPTSTR      lpTemp = new TCHAR[StrLen() + 1];
        lstrcpy(lpTemp, m_lpData);
        ReAllocBuf( nLen + 1 );
        lstrcpy(m_lpData, lpTemp);
        delete lpTemp;
    }

    LPTSTR lpPtr = m_lpData;
    lpPtr += StrLen();
    *lpPtr++ = cAdd;
    *lpPtr = 0;

    return(*this);
}



LPTSTR WINAPI operator+(CWString& cwStr, int nIndex)
{
    LPTSTR       lpRet;

    if (nIndex < cwStr.GetAllocSize())
        lpRet = cwStr.GetPtr() + nIndex;
    else
        lpRet = cwStr.GetPtr() + (cwStr.GetAllocSize() - 1);

    return(lpRet);
}



LPTSTR WINAPI operator+(int nIndex, CWString& cwStr)
{
    LPTSTR       lpRet;

    if (nIndex < cwStr.GetAllocSize())
        lpRet = cwStr.GetPtr() + nIndex;
    else
        lpRet = cwStr.GetPtr() + (cwStr.GetAllocSize() - 1);

    return(lpRet);
}


///////////////////////////////////////////////////////////////////////////
//  Protected operations
///////////////////////////////////////////////////////////////////////////

BOOL CWString::Init(int buffsize)
{
    BOOL        bRet = FALSE;

    if (m_nSize && (m_lpData != NULL))
        DestroyBuf();

    if (AllocBuf(buffsize))
        bRet = TRUE;

    return(bRet);
}


BOOL CWString::AllocBuf(int buffsize)
{
    BOOL        bRet = TRUE;

    if (!buffsize)
    {
        m_nSize = 0;
        m_lpData = NULL;
    }
    else
    {
        m_lpData = new TCHAR[buffsize];
        m_nSize = buffsize;
        ZeroBuf();
    }

    return(bRet);
}


void CWString::DestroyBuf(void)
{
    if (m_lpData != NULL)
        delete m_lpData;

    m_nSize = 0;
    m_lpData = NULL;

    return;
}


BOOL CWString::ReAllocBuf(int nNewSize)
{
    BOOL        bRet = FALSE;

    DestroyBuf();
    bRet = AllocBuf(nNewSize);

    return(bRet);
}


int CWString::CWStringAssert()
{
    TCHAR      szMessage[MAX_CWSERRMSG_SIZE];
    TCHAR      szCaption[MAX_CWSERRCAP_SIZE];

    *szMessage = *szCaption = 0;

    if (g_uResIdText && g_hInstParent)
        LoadString(g_hInstParent, g_uResIdText, szMessage, sizeof(szMessage));

    if (g_uResIdCap && g_hInstParent)
        LoadString(g_hInstParent, g_uResIdCap, szCaption, sizeof(szCaption));

    if (*szMessage)
        MessageBox(g_hParent == NULL ? GetActiveWindow() : g_hParent,
                   szMessage, szCaption, MB_OK | MB_SYSTEMMODAL);

    CWASSERT(0);

    return(TRUE);
}


void CWString::ZeroBuf(void)
{
    if (CheckValid())
        memset(m_lpData, 0, m_nSize * sizeof(TCHAR));

    return;
}


LPTSTR CWString::FindEndOfString()
{
    LPTSTR      lpRet = m_lpData;

    if (CheckValid())
    {
        while(*lpRet)
            lpRet = AnsiNext(lpRet);
    }

    return(lpRet);
}


LPTSTR CWString::FindLastChar()
{
    LPTSTR lpRet = FindEndOfString();

    if (lpRet != NULL)
        lpRet = AnsiPrev(m_lpData, lpRet);

    return(lpRet);
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

void WINAPI CW_AssertFailedLine(LPCSTR lpszFileName, int nLine)
{
    int         nMsg;
    CWString    szErr(CW_MAX_PATH);

    wsprintf((LPSTR)szErr, (LPSTR)STR_ASSERT_FAILED, lpszFileName, nLine);

	nMsg = ::MessageBox(GetActiveWindow(), szErr, "CWClass",
			            MB_SYSTEMMODAL | MB_ICONHAND | MB_ABORTRETRYIGNORE);

    if (nMsg == IDIGNORE)
        return;
    else
    {
        if (nMsg == IDRETRY)
        {
#ifdef _DEBUG
            _asm { int 3 };
#endif
            return;
        }
    }

    abort();

    return;

}

