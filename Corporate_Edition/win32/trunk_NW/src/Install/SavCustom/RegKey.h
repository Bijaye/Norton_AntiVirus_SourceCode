// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _REGKEY_EXT_H
#define _REGKEY_EXT_H

#include "TransmanClass.h"

#define HKCU				HKEY_CURRENT_USER
#define HKCR				HKEY_CLASSES_ROOT
#define HKUR				HKEY_USERS
#define HKLM				HKEY_LOCAL_MACHINE

HKEY GetRegKeyHandle(LPCSTR sHandle);
DWORD RegKeyCreateValue(HKEY hKey,LPCSTR sKey,LPCSTR sName,DWORD dwType,LPBYTE pValue,DWORD dwSize,LPCSTR pServer,LPTM pTrans=NULL);
DWORD RegKeyGetValue(HKEY hKey,LPCSTR sKey,LPCSTR sName,LPBYTE pValue,DWORD dwSize,LPCSTR pServer,LPTM pTrans=NULL,LPDWORD pSize=NULL);
DWORD RegKeySetValue(HKEY hKey,LPCSTR sKey,LPCSTR sName,DWORD dwType,LPBYTE pValue,DWORD dwSize,LPCSTR pServer,LPTM pTrans=NULL);
DWORD RegKeyCreate(HKEY hKey,LPCSTR sKey,LPCSTR pServer,LPTM pTrans=NULL);
BOOL RegKeyExists(HKEY hKey,LPCSTR sKey,LPCSTR pServer,LPTM pTrans=NULL);
DWORD RegKeyDelete(HKEY hKey,LPCSTR sKey,LPCSTR pServer,LPTM pTrans=NULL);
DWORD RegKeyDeleteValue(HKEY hKey,LPCSTR sKey,LPCSTR sName,LPCSTR pServer,LPTM pTrans=NULL);
BOOL RegKeyIsEmpty(HKEY hKey, LPCSTR sKeyName);
DWORD RegKeyDeleteAll(HKEY hKey, LPCSTR sKeyName);
DWORD RegKeyDeleteEmptyKey(HKEY hKey, LPCSTR sKeyName);

// Shutdown rtvsnc.exe on Windows 95
DWORD StopWin95Service();

#endif

