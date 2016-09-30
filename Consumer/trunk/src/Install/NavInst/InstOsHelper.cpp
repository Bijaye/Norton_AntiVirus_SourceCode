////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0501
#include "stdafx.h"
#include "Windows.h"
#include "InstOsHelper.h"
#include "helper.h"

//////////////////////////////////////////////////////////////////////////////////
// InstOsHelper Methods for 32 & 64 bit regitry utility.
//////////////////////////////////////////////////////////////////////////////////

// Return:
//	If succeeded returns length of the SysWow64 directory string, copies the 
// string in lpBuffer.
//  If Failed, returns 0.

UINT _GetSystemWOW64Directory(LPTSTR lpBuffer, UINT uSize)
{	
	CCTRACEI(_T("_GetSystemWOW64Directory() Enter"));

	LPFN_GETSYSTEMWOW64DIRECTORY m_fnGetSystemWOW64Directory = NULL;
	UINT uiRet = 0;
	if( !lpBuffer ) { 
		CCTRACEE(_T("_GetSystemWOW64Directory()- lpBuffer == NULL"));
		return 0;
	}
	try
	{
		m_fnGetSystemWOW64Directory = (LPFN_GETSYSTEMWOW64DIRECTORY)GetProcAddress(GetModuleHandle(_T("kernel32")),GET_SYSTEM_WOW64_DIRECTORY_NAME_A_A);

		if(!m_fnGetSystemWOW64Directory){
			CCTRACEI(_T("_GetSystemWOW64Directory(): API %s not found in Kernel32.dll"),GET_SYSTEM_WOW64_DIRECTORY_NAME);
		}
		else {
			uiRet = m_fnGetSystemWOW64Directory(lpBuffer,uSize);			
			CCTRACEI(_T("_GetSystemWOW64Directory returned %ui = %s"), uiRet, (NULL != lpBuffer)? lpBuffer : _T(""));
		}
	}
 	catch(exception& ex) {
		CCTRACEE(CA2CT(ex.what()));
	}

	CCTRACEI(_T("_GetSystemWOW64Directory return = %ui"), uiRet);

	return uiRet;
}

// Returns TRUE if SysWow64 directory exists
BOOL	SysWow64DirExists()
{
	CCTRACEI(_T("SysWow64DirExists() Enter"));
	
	BOOL bSysWow64Dir = FALSE;
	UINT uiL = 0;
	TCHAR szPath[_MAX_PATH + 1];

	uiL = _GetSystemWOW64Directory(szPath,sizeof(szPath)/sizeof(szPath[0]));

	TCHAR szSys[] = {_T("SysWOW64")};

	if( uiL ) {
		TCHAR *pWow64 = _tcsstr(szPath,szSys);
		if( pWow64 ) {
			bSysWow64Dir = TRUE;
			CCTRACEI(_T("SysWOW64 found - running in 64-bit process on 64-bit OS"));
		}
		else {
			// This means SysWow64 directory doesn't exist
			CCTRACEI(_T("SysWOW64 directory not found"));
		}
	}
	else {
		//API doesn't exist
		CCTRACEI(_T("running in 32-bit process on 32-bit OS"));
	}
	CCTRACEI(_T("SysWow64DirExists() Exit"));
	return bSysWow64Dir;
}

// Return true if we are running in 64-bit OS on 64-bit Machine
BOOL	IsOS64Bit()
{
	CCTRACEI(_T("IsOS64Bit() Enter"));

	BOOL bRet = FALSE;
	OSVERSIONINFO osV = {0};
	osV.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	if( GetVersionEx( &osV ) ) {
		bool bIsWXPorLater = ( (osV.dwMajorVersion > 5) ||
							 ( (osV.dwMajorVersion == 5) && (osV.dwMinorVersion >= 1)) );

		if( bIsWXPorLater && SysWow64DirExists())	{		
			bRet = TRUE;
		}
	}
	CCTRACEI(_T("IsOS64Bit() Exit: %d"), bRet);
	return bRet;
}

// returns TRUE if it's a 32-bit process running under WOW subsystem
BOOL IsWow64()
{
	CCTRACEI(_T("IsWow64() Enter"));

	BOOL bIsWow64 = FALSE;
	HMODULE hk32 = NULL;
	hk32 = GetModuleHandle(_T("kernel32"));	
	LPFN_ISWOW64PROCESS fnIsWow64Process= NULL;
	LPFN_GETSYSTEMWOW64DIRECTORY	m_fnGetSystemWOW64Directory = NULL;

	try	{
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hk32,"IsWow64Process");
		if (!fnIsWow64Process) {
			CCTRACEI(_T("IsWow64(): IsWow64Process() API not found in Kernel32.dll"));
		}
		if (fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
			if(bIsWow64) {
				CCTRACEI(_T("IsWow64(): running in 32-bit process on 64-bit OS"));
			}
		}
	}
	catch(exception& ex) {
		CCTRACEE(CA2CT(ex.what()));
	}

	CCTRACEI(_T("IsWow64() Exit: %d"), bIsWow64);
	return bIsWow64;
}

void	SetRegKeyValueSZ(HKEY hParentKey, int iOption,ATL::CString szKey,ATL::CString szName,ATL::CString szValue)
{
	CCTRACEI(_T("SetRegKeyValueSZ() Key[%s]%s=%s"), szKey, szName, szValue);

	HKEY hKey;
	if (RegOpenKeyEx(hParentKey, szKey, NULL,KEY_READ | KEY_WRITE | iOption, &hKey) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, szName, NULL, REG_SZ,(BYTE*)szValue.GetBuffer(),szValue.GetLength()+1);
		CloseHandle(hKey);
	}
}

//Sets value of "Product Name" subkey under szNAVAppsKey to pszProductName
// Used by NavShExt.dll
void	SetRegProductName(ATL::CString szRegKeyName,ATL::CString szRegKeyValue)
{
	int iOption = 0;

	// Set the registry value in default hive ( either 32 or 64bit hive depending on who we are)
	SetRegKeyValueSZ(HKEY_LOCAL_MACHINE,0,szNAVAppsKey,szRegKeyName,szRegKeyValue); // Access a 32/64 reg key from 32/64 process

	// Now set the value in the other hive
	if(IsWow64()) {											// We are 32-bit process running under WOW on a 64-bit OS
		// Setting the value in 64bit hive from 32-bit process
		SetRegKeyValueSZ(HKEY_LOCAL_MACHINE,KEY_WOW64_64KEY,szNAVAppsKey,szRegKeyName,szRegKeyValue);	// Access 64bit hive from 32bit process
	}
	else if(IsOS64Bit()) {										//OR..a 64bit process running on 64bit OS
		// Setting the value in 32bit hive from 64bit process
		SetRegKeyValueSZ(HKEY_LOCAL_MACHINE,KEY_WOW64_32KEY,szNAVAppsKey,szRegKeyName,szRegKeyValue);	// Access a 32bit hive from 64 process
	}
}
