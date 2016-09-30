// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _UNCAB_H_
#define _UNCAB_H_


/*
 * Function prototypes 
 */
BOOL UnCabRuntimeDlls(MSIHANDLE hInstall, TCHAR *szCabinetFileName, TCHAR *szCabinetPathName);
char   *return_fdi_error_string(int err);
/*
 * Destination directory for extracted files
 */
static TCHAR	dest_dir	[MAX_PATH];
// Number of entries in pszTargetFiles and pszDestFiles
#define NUM_VC_RUN_DLLS 2
#define DOUBLE_MAX_PATH			(MAX_PATH * 2)

static TCHAR * pszTargetFiles[NUM_VC_RUN_DLLS] =
		{"FL_msvcp71_dll_2_____X86.3643236F_FC70_11D3_A536_0090278A1BB8",
			"FL_msvcr71_dll_3_____X86.3643236F_FC70_11D3_A536_0090278A1BB8",
			};
static TCHAR * pszDestFiles[NUM_VC_RUN_DLLS] =
		{"msvcp71.dll",
			"msvcr71.dll",
		};
static TCHAR pszLUSETUPFile[] = {"LUSETUP.EXE"};
#define NUM_ADMIN_DATA1_DLLS 2
static TCHAR * pszADMINInstallSRCFiles[NUM_ADMIN_DATA1_DLLS] =
		{"msvcp71.dll",
		 "msvcr71.dll",
		};

static TCHAR * pszSSC_SRC_PATHS="Common\\Symantec Shared\\SSC\\";

UINT __stdcall MsiUnCabRuntimeDlls( MSIHANDLE );
UINT __stdcall MsiCleanCabRuntimeDlls( MSIHANDLE );

extern UINT MSILogMessage( MSIHANDLE, TCHAR []);
extern void __fastcall AddSlash( char* pszPath, size_t nNumBytes );

#endif                                  // ifndef _UNCAB_H_