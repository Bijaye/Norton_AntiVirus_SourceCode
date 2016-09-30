// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// MSIEntryPoints.h
//
#define NUM_DLLS 6
#define LOG_PREFIX "SAVINST: "
#define DOUBLE_MAX_PATH			(MAX_PATH * 2)
#define MAX_PATH_LENGTH			128

const char *g_pszFiles[NUM_DLLS] = {"cba.dll","msgsys.dll","nts.dll","pds.dll","Transman.dll","ScsComms.dll"};
extern LPTM g_pTrans;
extern "C" DllExport BOOL _stdcall MsiDomPass(LPSTR , LPSTR );

HMODULE LoadTransman( TCHAR *, BOOL bTMRegistered = FALSE );
void __fastcall AddSlash( char szPath );
void			MSILogMessage( MSIHANDLE , TCHAR [], BOOL bDoAction = FALSE );
BOOL IsTransmanRegistered();
BOOL UnloadTransman( HMODULE *, BOOL bDontUnreg = FALSE, const TCHAR* strParentServerName = NULL );
UINT __stdcall ValidateServer( MSIHANDLE hInstall );
UINT __stdcall MsiDoServerListDlg( MSIHANDLE hInstall );
UINT __stdcall ValidateServerGroup( MSIHANDLE hInstall );
UINT __stdcall MsiCleanTempFolder( MSIHANDLE hInstall );
UINT __stdcall MsiGetServerGroupNamefromServername( MSIHANDLE hInstall );
UINT __stdcall ValidateServerMigration( MSIHANDLE hInstall );
UINT __stdcall MsiGetNewServerPassword( MSIHANDLE hInstall );
UINT __stdcall MSIValidateAdminLogin( MSIHANDLE hInstall );
