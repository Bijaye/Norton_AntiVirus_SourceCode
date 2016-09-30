/////////////////////////////////////////////////////////////////////
// File: Utils.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGRAT/VCS/utils.cpv   1.2   19 Nov 1998 13:14:28   MBROWN  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/utils.cpv  $
// 
//    Rev 1.2   19 Nov 1998 13:14:28   MBROWN
// Changed method for getting shell folders, deleting NPS links, added entry point for unloading NAV
// 
//    Rev 1.1   25 Sep 1998 15:57:58   MBROWN
// Updated call to reflect new fn name in cwstring.h
// 
//    Rev 1.0   26 May 1998 14:31:16   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////




#include <windows.h>
#include <windowsx.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <tchar.h>
#include "resource.h"
#include "utils.h"
#include "cwstring.h"
#include "CRegEnt.h"




// ==== _mbsbytes =========================================================
//
//
// ========================================================================
// ========================================================================

INT _mbsbytes(IN LPCSTR  str)
{
    LPCSTR  pStr;

    // Find the nul terminator and return the number of bytes
    // occupied by the characters in the string, but don't
    // include the nul.

    pStr = (const char*)_mbschr((const unsigned char*)str, 0);
    return (pStr - str);

}  // end if "_mbsbytes"



/////////////////////////////////////////////////////////////////////////////
//
//  FileExists - Function to determine if a given file exists
//
//  Params: LPTSTR - Path and filename to look for
//
//  Return value:   TRUE if file is found
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI FileExists( LPTSTR lpFile )
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    WIN32_FIND_DATA     fd;

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

    return( bRet );

}  // end of "FileExists"



// ==== LTrace ============================================================
//
//  LTrace() - Function to trace output to a log fle put in the windows dir
//
//  Params: LPCSTR - Format string
//          Variable Args - To be substituted into format string
//
//  Return value:   VOID
//
// ========================================================================
//  02/14/97 - BruceH, function created
// ========================================================================

VOID CDECL LTrace(LPCSTR  lpszFmt, ...)
{
    FILE           *fp = NULL;
    DWORD           dwNumBytes = 0L;
    CWString        szBuffer( MAX_PATH*2 );
    CWString        szLogFile( MAX_PATH*2 );
    va_list         lpArgs;

    va_start( lpArgs, lpszFmt );

    wvsprintf( (LPSTR)szBuffer, lpszFmt, lpArgs );
    lstrcat( szBuffer, "\n" );


#ifdef _LOGFILE_TRACE_
    GetWindowsDirectory( szLogFile, (MAX_PATH - 1)*2 );
    szLogFile.AppendFileName( "SYMUPGD.LOG" );

    fp = fopen( szLogFile, "at" );

    if ( fp != NULL )
    {
        fputs( szBuffer, fp );
        fflush( fp );
        fclose( fp );
    }
#else
    OutputDebugString( szBuffer );
#endif

    return;

}     // end of "LTrace"



// ==== UnicodeToAnsi =====================================================
//
//  UnicodeToAnsi converts the Unicode string pszW to an ANSI string
//  and returns the ANSI string through ppszA. Space for the
//  converted string is allocated by UnicodeToAnsi.
//
// ========================================================================
// ========================================================================

BOOL    UnicodeToAnsi(LPCOLESTR  pszWide, LPSTR  pszAnsi,UINT  cbSize)
{
    auto    DWORD   dwError;
    auto    ULONG   cCharacters;

    // If input is null then just return the same.
    if (NULL == pszWide)
        {
        *pszAnsi = NULL;
        return TRUE;
        }

    cCharacters = wcslen(pszWide)+1;
    // Determine number of bytes to be allocated for ANSI string. An
    // ANSI string can have at most 2 bytes per character (for Double
    // Byte Character Strings.)

	// If buffer is smaller than # of characters, return FALSE
	// This function assumes conversion to one-byte per character ANSI
	if (cbSize < cCharacters)
        {
        #ifdef  SHOW_MESSAGE_BOXES
        MessageBox (NULL,"Buffer Passed to UnicodeToAnsi function can not hold Unicode String.  Function Failed!"
            , "Migration.DLL : Warning", MB_OK | MB_ICONEXCLAMATION);
        #endif  // SHOW_MESSAGE_BOXES
		return FALSE;
        }

    // Convert to ANSI.
    if (0 == WideCharToMultiByte(CP_ACP, 0, pszWide, cCharacters, pszAnsi, cbSize
                                                                , NULL, NULL))
        {
        dwError = GetLastError();
        *pszAnsi = NULL;
        return FALSE;
        }

    return TRUE;

}  // end of "UnicodeToAnsi"