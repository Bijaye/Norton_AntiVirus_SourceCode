// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// MSIEntryPoints.h
//
#define LOG_PREFIX "SAVINST: "
#define DOUBLE_MAX_PATH			(MAX_PATH * 2)
#define MAX_PATH_LENGTH			128


void __fastcall AddSlash( char szPath );
void			MSILogMessage( MSIHANDLE, LPCTSTR, BOOL bDoAction = FALSE );
UINT __stdcall MsiCleanTempFolder( MSIHANDLE hInstall );
UINT _stdcall MsiSetAtom( MSIHANDLE installHandle );
UINT _stdcall MsiGetAtom( MSIHANDLE installHandle );
