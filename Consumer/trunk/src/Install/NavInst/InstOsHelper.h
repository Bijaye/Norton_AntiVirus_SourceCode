////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef INSTOSHELPER_H
#define INSTOSHELPER_H

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
typedef UINT (WINAPI *LPFN_GETSYSTEMWOW64DIRECTORY)(LPTSTR lpBuffer,UINT uSize);

#ifdef	UNICODE			
#define	GET_SYSTEM_WOW64_DIRECTORY_NAME	GET_SYSTEM_WOW64_DIRECTORY_NAME_A_W
#else
#define	GET_SYSTEM_WOW64_DIRECTORY_NAME GET_SYSTEM_WOW64_DIRECTORY_NAME_A_A
#endif

UINT _GetSystemWOW64Directory(LPTSTR lpBuffer,UINT uSize);
BOOL	SysWow64DirExists();
BOOL	IsOS64Bit();
BOOL	IsWow64();
void	SetRegKeyValueSZ(HKEY hParentKey, int iOption,ATL::CString szKey,ATL::CString szName,ATL::CString szValue);
void	SetRegProductName(ATL::CString szRegKeyName,ATL::CString szRegKeyValue);

#endif
