// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SAV Migration Code

#include "stdafx.h"
#include <string>
#include <atlbase.h>

// Imported functions
extern CString GetProductCode( MSIHANDLE );

// Internal functions
HKEY OpenSavTreeHKLM( DWORD );
HKEY OpenSavTreeHKU( DWORD );
HKEY OpenMigrateTreeHKLM( DWORD );
HKEY OpenMigrateTreeHKU( DWORD );
BOOL CopyRegKeys( HKEY, HKEY );
void CopyKeys( HKEY , HKEY, TCHAR * );
void CopyValues( HKEY, HKEY );

// Exported functions
UINT __stdcall SevInstUninstallWorkAround( MSIHANDLE );
UINT __stdcall SaveSAVSettings( MSIHANDLE );
UINT __stdcall SaveUserSettings( MSIHANDLE );
UINT __stdcall RestoreSAVSettings( MSIHANDLE );
UINT __stdcall RestoreUserSettings( MSIHANDLE );
UINT __stdcall SaveLogFiles( MSIHANDLE );
UINT __stdcall RestoreLogFiles( MSIHANDLE );
UINT __stdcall SaveQuarantineItems( MSIHANDLE );
UINT __stdcall RestoreQuarantineItems( MSIHANDLE );
