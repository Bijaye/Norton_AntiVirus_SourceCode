// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SavRoam.cpp : Defines the entry point for the console application.
//

#define _WINSOCKAPI_
// CreateWaitableTimer function is now loaded dynamically (incompatible with Win95)
//     so this may not be necessary -- leaving it in just to be safe.  ESCHOOP 6/2/01
#define _WIN32_WINNT 0x0400     // this picks up the CreateWaitableTimer API's

#include "stdafx.h"
#include "resource.h"

#include "tchar.h"
#include "windows.h"
#include "stdio.h"
#include "direct.h"
#include "ResourceLoader.h" //Include before SymSaferStrings to avoid weirdness
extern CResourceLoader g_ResLoader;

#include "SymSaferStrings.h"

#ifdef __cplusplus
extern "C" {
#endif
     
#include "dprintf.h"
#include "dprintf_2.h"

extern DWORD debug;

#ifdef __cplusplus
}
#endif
    

#define SAVROAM_SHIM 1		// define the things that rtvscan doesn't
#include "savroam.h"

#include "roam.h"


// the following functions are shims for roam.cpp not required when linking with rtvscan

BOOL KeepSCMHappyOnStop( int nWaitHint )
{
    (void) nWaitHint;

    return TRUE;
}

//Added an extra parameter to decide if we should or not display the windows error
//this is so that when we get an error from our functions we do not want use
//getlasterror to display microsoft error messages
// void ReportLastError( char *szMsg, FILE *fFile, BOOL bGetLastError = TRUE);

void OutputLine( char *szLine, FILE *fFile, BOOL bDetail )
{
    (void) fFile;
    (void) bDetail;

    dprintfTag1( DEBUGROAM, "%s\n", szLine );
}

void OutputDebugLine( char *szLine, FILE *fFile, BOOL bDetail )
{
    (void) fFile;
    (void) bDetail;

    dprintfTag1( DEBUGROAM, "%s\n", szLine );
}

void ReportLastError( char *szMsg, FILE *fFile, BOOL bGetLastError )
{
    LPVOID lpMsgBuf = NULL;
    char szLine[1024];
	char szError[1024];

    
	if(bGetLastError)
	{
		FormatMessage(
                 FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL,
                 GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 (LPTSTR) &lpMsgBuf,
                 0,
                 NULL );
		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR, szError, MAX_BUFFER);
		sssnprintf( szLine, sizeof(szLine), _T("%s - %s (%x) %s\n"), szMsg, szError, GetLastError( ), (LPCTSTR)lpMsgBuf );
	}
	else
		sssnprintf( szLine, sizeof(szLine), _T("%s \n"), szMsg);
    OutputLine( szLine, fFile, FALSE );

	if ( lpMsgBuf )
		LocalFree( lpMsgBuf );
}


