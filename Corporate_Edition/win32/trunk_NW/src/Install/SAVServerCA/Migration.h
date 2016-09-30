// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Migration.h - Header file for server migration functions
#include <string>
#include <msi.h>

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

#define MAX_LOG_PATH			1024 // Ensure that we never buffer over run when dealing with strings as large as MAX_PATH

// External functions
extern UINT MsiLogMessage( MSIHANDLE, TCHAR* );
extern DWORD GetMSIProperty( MSIHANDLE, TCHAR [] );
extern int CopyDirRecursive( MSIHANDLE, TCHAR*, TCHAR* );
extern BOOL DeleteDir( MSIHANDLE, TCHAR*, TCHAR* );

// Local helper functions

// MSI Custom action calls
UINT __stdcall CheckForPrimaryServer( MSIHANDLE );
UINT __stdcall SetAllowUDPProtocol( MSIHANDLE );
UINT __stdcall SetIsServerSecure( MSIHANDLE );
UINT __stdcall CopyServerCerts( MSIHANDLE );