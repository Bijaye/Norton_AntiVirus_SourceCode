////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// FindCode.cpp: implementation of the CFindCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptRecon.h"
#include "FindCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFindCode::CFindCode()
{

}

CFindCode::~CFindCode()
{

}

DWORD CFindCode::FindLine(LPCTSTR lpcszFileName,
                          LPCTSTR lpcszResourceName,
                          DWORD dwLineNumber,
                          LPTSTR lpszString,
                          DWORD dwBufferSize)
{
    DWORD dwReturn = FALSE;
    // Look up what .DLL the Script file is in via ScriptRecon.txt
    // 
    HRSRC hSource;
    HMODULE hModule;
    HGLOBAL hCode;
    DWORD dwSize = 0;
    DWORD dwStringSize = 0;
    char* pszFile = NULL;
    char* pszToken = NULL;
    LPVOID pszBuffer = NULL;

    hModule = ::LoadLibraryEx ( lpcszFileName, NULL, LOAD_LIBRARY_AS_DATAFILE );

    if ( !hModule )
    {
        ::MessageBox ( ::GetDesktopWindow(), lpcszFileName, "Error finding file", MB_OK);
        goto cleanup;
    }

    hSource = FindResource (hModule, lpcszResourceName, RT_HTML ); //RT_RCDATA

    if ( !hSource )
    {
        ::MessageBox ( ::GetDesktopWindow(), lpcszResourceName, "Error finding resource", MB_OK);
        goto cleanup;
    }


    hCode = LoadResource ( hModule, hSource );

    if ( !hCode )
    {
        AfxMessageBox ( "Error loading resource");
        goto cleanup;
    }

    dwSize = SizeofResource (hModule, hSource );

    if ( dwSize == 0)
    {
        AfxMessageBox ( "Resource is empty");
        goto cleanup;
    }

    pszFile = new char [dwSize+1];
    pszBuffer = LockResource ( hCode );

    if ( !pszBuffer )
    {
        AfxMessageBox ( "Failed to read buffer" );
        goto cleanup;
    }

    strncpy ( pszFile, (char*) pszBuffer, dwSize );

    pszToken = strtok ( pszFile, "\n" );

    {
        // Count lines until we get to the number we want or run out of file.
        // 
        int iLineNumber = dwLineNumber;

        for ( int iLine = 1; iLine < iLineNumber && pszToken; iLine++ )
        {
            pszToken = strtok ( NULL, "\n" );
        }
    }

    // Reached the end without a match.
    //
    if ( !pszToken )
        goto cleanup;

    dwStringSize = strlen ( pszToken );

    if (  dwStringSize > dwBufferSize )
        goto cleanup;

    strncpy ( lpszString, pszToken, dwStringSize-1 );    // No /n at the end

    dwReturn = TRUE;

cleanup:
    // Clean up
    //
    if ( pszFile )
        delete pszFile;

    pszFile = NULL;

    if ( hModule )
        FreeLibrary ( hModule );

    return dwReturn;
}
