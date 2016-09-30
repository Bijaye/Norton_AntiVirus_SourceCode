// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
//	ScanShared.cpp
//
//	This module is used to share scan functions between vptray.exe and DoScan.exe
//
#include <windows.h>
#include <stdio.h>
#include "ldvpcom.c"
#include "clientreg.h"
#include <tchar.h>
#include <assert.h>
#include "vpstrutils.h"
#include "ScanShared.h"

#ifndef array_sizeof
#define array_sizeof(x) (sizeof(x)/sizeof((x)[0]))
#endif

// checks to see if Startup Scans are enabled
BOOL StartupScansEnabled()
{
	BOOL bEnabled = TRUE;
	HKEY hKey = NULL;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg_Key_Main "\\" szReg_Key_AdminOnly "\\" szReg_Key_General, NULL, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) 
	{
		DWORD dwEnabled = 1;
		if (ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey, szReg_Val_StartupScansEnabled, &dwEnabled))
			bEnabled = dwEnabled ? TRUE : FALSE;
		RegCloseKey(hKey);
	}

	return bEnabled;
}

// checks to see if Startup Scans are locked
BOOL StartupScansLocked()
{
	BOOL bLocked = FALSE;
	HKEY hKey = NULL;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg_Key_Main "\\" szReg_Key_AdminOnly "\\" szReg_Key_General, NULL, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) 
	{
		DWORD dwLocked = 0;
		if (ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey, szReg_Val_StartupScansLocked, &dwLocked))
			bLocked = dwLocked ? TRUE : FALSE;
		RegCloseKey(hKey);
	}

	return bLocked;
}

// Creates and opens a ScanConfig in one call
HRESULT CreateScanConfig(const CLSID &clsidScanner, DWORD RootID,char *SubKey, IScanConfig*& pConfig)
{
	HRESULT hr;
	hr = CoCreateLDVPObject(clsidScanner,IID_IScanConfig,(void**)&pConfig);
	if (hr==S_OK)
	{
		// open the new config object
		hr = pConfig->Open(NULL,RootID,SubKey);
		if (hr != S_OK)
		{
			pConfig->Release();
			pConfig=NULL;
		}
	}
	return hr;
}

void Relinquish()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE))
	{
		GetMessage(&msg, 0, 0, 0);
		if (msg.message == WM_QUIT)
            break;
		DispatchMessage(&msg);
	}
}
