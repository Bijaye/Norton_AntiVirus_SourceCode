// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"

#define NOTES_APPPATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\notes.exe"

typedef UINT (__cdecl* LPFNNSEINSTALL)( MSIHANDLE );

void MSILogMessage( MSIHANDLE , TCHAR [] );
DWORD GetMSIProperty( MSIHANDLE , TCHAR [], const TCHAR [] );

BOOL GetNotesDir( TCHAR *, DWORD );
BOOL CopyDLLToNotesDir( TCHAR *, TCHAR * );
BOOL UpdateNotesINI( MSIHANDLE, TCHAR * );

UINT _stdcall WriteNotesIniValues( MSIHANDLE );

