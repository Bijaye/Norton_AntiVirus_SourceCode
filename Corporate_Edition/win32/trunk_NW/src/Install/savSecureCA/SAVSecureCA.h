// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SAVSecureCA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SAVSecureCA_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
/*#ifdef SAVSecureCA_EXPORTS
#define SAVSecureCA_API __declspec(dllexport)
#else
#define SAVSecureCA_API __declspec(dllimport)
#endif*/

///////////////////////////////////////////////////////////////////////////////
//
//  Preprocessor
//
//  NOTE: the dllexport attribute eliminates the need for a .DEF file
//
///////////////////////////////////////////////////////////////////////////////
#include <msi.h>
#include <msiquery.h>
#include <string>

#ifdef UNICODE
#define tstring std::w_string
#else
#define tstring std::string
#endif

typedef struct 
{
	TCHAR   PkiBaseDir[MAX_PATH+1];
	TCHAR   ServerGroupName[MAX_PATH+1];
    TCHAR   PrimaryServerName[MAX_PATH+1];
    TCHAR   UserName[MAX_PATH+1];
    TCHAR   Password[MAX_PATH+1];
	TCHAR   CommonFilesPath[MAX_PATH+1];
} CAData;

static DWORD	GetServerGroupGuid		( TCHAR *, int );
UINT _stdcall	MSICopyRootCert			( MSIHANDLE hInstall );
UINT _stdcall	MSIServerPKIStandAlone	( MSIHANDLE hInstall );
UINT _stdcall	DeleteServerPKI			( MSIHANDLE );

UINT			MsiLogMessage			( MSIHANDLE, LPCTSTR );
BOOL			Findit					( MSIHANDLE, LPCTSTR, LPTSTR );
int				CopyDirRecursive		( MSIHANDLE, LPTSTR, LPTSTR );
void __fastcall AddSlash				( char* pszPath );
BOOL			Find_FQDN				( MSIHANDLE hInstall, TCHAR*, TCHAR* );
UINT            CreateServerPKI         ( MSIHANDLE hInstall,
                                          bool login,
                                          bool use_root_auth,
                                          HMODULE scscomms,
	                                      TCHAR* strPkiBasePath,
                                          TCHAR* strPrimaryServerName,
                                          TCHAR* strUserName,
                                          TCHAR* strPassword );
BOOL			CreatePKITargetPaths	( MSIHANDLE hInstall, LPTSTR, LPTSTR, LPTSTR, LPTSTR,
										  LPTSTR, LPTSTR, LPTSTR, LPTSTR, LPTSTR, LPTSTR, LPTSTR);
									   
bool			GetCustomActionData		( MSIHANDLE , CAData * );

bool			MSIDeleteDir			( MSIHANDLE , const char* , const char* );
