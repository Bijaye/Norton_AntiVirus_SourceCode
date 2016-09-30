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
#define NUM_VC_RUN_DLLS 8
#define DOUBLE_MAX_PATH			(MAX_PATH * 2)

static TCHAR * pszTargetFiles[NUM_VC_RUN_DLLS] =
		{"FL_msvcp71_dll_2_____X86.3643236F_FC70_11D3_A536_0090278A1BB8",
			"FL_msvcr71_dll_3_____X86.3643236F_FC70_11D3_A536_0090278A1BB8",
			"cba.dll.93C43188_D2F5_461E_B42B_C3A2A318345C",
			"msgsys.dll.93C43188_D2F5_461E_B42B_C3A2A318345C",
			"nts.dll.93C43188_D2F5_461E_B42B_C3A2A318345C",
			"pds.dll.93C43188_D2F5_461E_B42B_C3A2A318345C",
			"Transman.dll.93C43188_D2F5_461E_B42B_C3A2A318345C",
			"ScsComms.dll.65CE4DDA_36B1_4B17_8E59_E63AE84B286A"};
static TCHAR * pszDestFiles[NUM_VC_RUN_DLLS] =
		{"msvcp71.dll",
			"msvcr71.dll",
			"cba.dll",
			"msgsys.dll",
			"nts.dll",
			"pds.dll",
			"Transman.dll",
			"ScsComms.dll"};
static TCHAR pszLUSETUPFile[] = {"LUSETUP.EXE"};
#define NUM_ADMIN_DATA1_DLLS 6
static TCHAR * pszADMINInstallSRCFiles[NUM_ADMIN_DATA1_DLLS] =
		{"msvcp71.dll",
		 "msvcr71.dll",
		 "cba.dll",
		 "msgsys.dll",
		 "nts.dll",
		 "pds.dll"};
#define NUM_ADMIN_SSC_DLLS 2
static TCHAR * pszADMINInstallSSCFiles[NUM_ADMIN_SSC_DLLS] =
		{"Transman.dll",
		 "ScsComms.dll"};
static TCHAR * pszSSC_SRC_PATHS="Common\\Symantec Shared\\SSC\\";

UINT __stdcall MsiUnCabRuntimeDlls( MSIHANDLE );
UINT __stdcall MsiCleanCabRuntimeDlls( MSIHANDLE );

extern UINT MSILogMessage( MSIHANDLE, TCHAR []);
extern void __fastcall AddSlash( char* pszPath, size_t nNumBytes );

#endif                                  // ifndef _UNCAB_H_