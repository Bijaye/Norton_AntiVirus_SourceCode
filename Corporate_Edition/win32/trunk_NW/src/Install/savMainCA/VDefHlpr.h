// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VDefHlpr.cpp : Defines the entry point for the DLL application.
//
#define NUM_ADMIN_INSTALL_MANUAL_COPY_FILES 5
#define MAX_RND_PATH_CHRS 8
#define NUM_VIRUSDEF_FILES 2
#define VIRUS_DEF_TEMP	"VIRDEF"

static char * pszAdminSrcFiles[NUM_ADMIN_INSTALL_MANUAL_COPY_FILES] =
		{"vdefhub.zip",
		 "instopts.dat",
		 "CPOLICY.XML",
		 "LUSETUP.EXE"};
static char * pszVirusDefFiles[NUM_VIRUSDEF_FILES] = 
		{"vdefhub.zip",
		 "LUSETUP.EXE"};

UINT __stdcall MsiInstallDefsRB( MSIHANDLE hInstall );
UINT __stdcall MsiUnInstallDefs( MSIHANDLE hInstall );
UINT __stdcall MsiUnInstallDefsRB( MSIHANDLE hInstall );
UINT __stdcall MsiMigrateHold( MSIHANDLE hInstall );
UINT __stdcall MsiMigrateHoldRB( MSIHANDLE hInstall );
UINT __stdcall MsiMigrateRelease( MSIHANDLE hInstall );
UINT __stdcall MsiInstallDefs2( MSIHANDLE hInstall );
UINT __stdcall MSIDeleteVDEFHUB( MSIHANDLE hInstall );
UINT __stdcall MsiCopyVirusDefsAdmin( MSIHANDLE hInstall );
UINT __stdcall MsiCopyVirusDefsUNC( MSIHANDLE hInstall );
UINT __stdcall MsiSetRanVirDefsPath( MSIHANDLE hInstall );

BOOL DeleteDir( MSIHANDLE hInstall, TCHAR* strFolder, TCHAR* strPattern );
BOOL UnPakZip (MSIHANDLE hInstall, LPCTSTR szFile, LPCTSTR szDestDirectory );
BOOL Findit (MSIHANDLE hInstall, LPCTSTR szFile, TCHAR* szPath, size_t nNumPathBytes );
extern void __fastcall AddSlash( char* pszPath, size_t nNumBytes );

