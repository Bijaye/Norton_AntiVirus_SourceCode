// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// local function prototypes
DWORD __stdcall SetDirectoryAccessAttributes(LPTSTR );
DWORD __stdcall SetDirectoryRestrictedAccessAttributes(LPTSTR );
DWORD VolumeIsNTFS(LPTSTR lpPath, LPBOOL lpbIsNTFS);

// MSI Exports
extern UINT __stdcall SetFolderPermissions( MSIHANDLE );

// External prototypes
extern CString GetMSIProperty( MSIHANDLE, CString );
extern UINT MSILogMessage( MSIHANDLE , TCHAR [] );
