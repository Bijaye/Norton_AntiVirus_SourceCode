// $Header: $
//
// AUTHOR(S)	 : Shaun Cooley
// COMPILED WITH : Microsoft Visual C++ (v6.0)
// MODEL         : Flat (32bit)
// ENVIRONMENT   : WIN32, C runtime, C++ runtime, MFC(4.2), COM, OLE, STL, ATL
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1999 - 2003, 2005 Symantec Corporation. All rights reserved.
//
//
//============================================================================
//                               HEADER FILES                                 
//============================================================================
// The order of the following #include sections minimizes any symbol clashing.
// It's been determined mainly by trial and error over time.
#include "stdafx.h"
#include <new>

// This module's header.
#include "Utils.h"

// Private, global headers.
#include <shellapi.h>
//#include "navstart.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <share.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define dprintf printf
#define _ASSERTE ASSERT

// implementation-only includes
// <nothing>

#undef DBG_SYM
#undef DBG_STR
#ifdef _DEBUG
    #define DBG_SYM(sym) sym
    #define DBG_STR(sym) sym
#else
    #define DBG_SYM(sym)
    #define DBG_STR(sym) ""
#endif

#ifdef _DEBUG
    #include <crtdbg.h>
    #ifndef DEBUG_NEW
    	inline void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
    	{
    		return ::operator new(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
    	}
    	inline void __cdecl operator delete(void* pMem, LPCSTR, int)
    	{
    		::operator delete(pMem);
    	}
    	#define DEBUG_NEW new(THIS_FILE, __LINE__)
    #endif
    #define new DEBUG_NEW
    #ifndef THIS_FILE_DEF
    static char THIS_FILE[] = __FILE__;
    #endif
#endif

#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#undef FILE_LINE
#define FILE_LINE __FILE__ "(" PPSTRIZE(__LINE__) ") : "
#undef FUNCNAME
#define FUNCNAME FILE_LINE

#ifndef MAX_LOADSTRING
 #define MAX_LOADSTRING MAX_PATH * 2
#endif

// Font name that will revert to DEFAULT_GUI_FONT
const TCHAR _szDefaultFont[] = _T("MS Sans Serif");

// Windows installed app key
const TCHAR _szInstalledAppKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths");

// Symantec const's
const TCHAR _szSymInstalledAppKey[] = _T("Software\\Symantec\\InstalledApps");
const TCHAR _szSymRegKey[] = _T("Software\\Symantec\\");

// Speed start const's
const TCHAR _szSpeedStartRunRegLoc[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\SpdStart");
const TCHAR _szSpeedStartVXDRegLoc[] = _T("System\\CurrentControlSet\\Services\\VxD\\SpdStart\\StaticVxD");
const TCHAR _szSpeedStartRegLoc[] = _T("Software\\Symantec\\Symantec Utilities\\Symantec Speed Start\\1.0\\Enable");
const TCHAR _szSpeedStartProdName[] = _T("Symantec Utilities");
const TCHAR _szSpeedStartExe[] = _T("NSS\\SPDSTART.EXE");
const TCHAR _szSpeedStartVxD[] = _T("NSS\\SPDSTART.VXD");

// CrashGuard const's
const TCHAR _szNCGName[] = _T("SymantecCrashGuard");
const TCHAR _szCrashGuardRegLoc[] = _T("Software\\Symantec\\Symantec CrashGuard\\Statcap\\CaptureName");
const LPTSTR _szCGRunLoc = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\Symantec CrashGuard Monitor");
const LPTSTR _szCGExe = _T("CGMENU.EXE");
const LPTSTR _szCGClassName = _T("SymantecCrashGuard_MonitorWnd");
const LPTSTR _szCrashGuardSettings[] =
{
	_T("Software\\Symantec\\Symantec CrashGuard\\16-bit EH"),
	_T("Software\\Symantec\\Symantec CrashGuard\\32-bit JIT"),
	_T("Software\\Symantec\\Symantec CrashGuard\\EnableBlueScreenProtection")
};
const DWORD _dwCrashGuardOn = 1;
const DWORD _dwCrashGuardOff = 0;

// NAV AP const's
const TCHAR _szNavVxDRegLoc[] = _T("System\\CurrentControlSet\\Services\\VxD\\NAVAP\\StaticVxD");
const TCHAR _szNavVxDName[] = _T("\\NAVAP.VXD");
const TCHAR _szNavAPRegLoc[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\Symantec Auto-Protect");
const TCHAR _szNavAPName[] = _T("\\NAVAPW32.EXE /LOADQUIET");

// Symantec Protected Recycle Bin const's
const LPTSTR _szNUName = _T("Symantec Utilities");
const LPTSTR _szNPRBExe = _T("nprotect.exe");
const LPTSTR _szNPRBSettings = _T("Software\\Symantec\\Symantec Utilities\\Symantec Protection");
const LPTSTR _szNPRBSwitches = _T("Switches");
const LPTSTR _szNPRBLoc[2] = 
{
	_T("Software\\Microsoft\\Windows\\CurrentVersion\\RunServices\\NPROTECT"),
	_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\NPROTECT")
};

// Code Pages
const int _nCODEPAGE_KOREAN_WANSUNG			= 949;
const int _nCODEPAGE_KOREAN_JOHAB			= 1361;
const int _nCODEPAGE_CHINESE_TRADITIONAL	= 950;
const int _nCODEPAGE_CHINESE_SIMPLIFIED		= 936;

//////////////////////////////////////////////////////////////////////
// Registry Utilities
//////////////////////////////////////////////////////////////////////

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetRegistryKey:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/30/99 - 3:29:56 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetRegistryKey
// Description	    : 
// Return type		: BOOL 
// Arguments
//     HKEY hKey    : 
//     LPCTSTR pszSubKey: 
//     LPCTSTR pszOnValue: 
//     DWORD dwLen  : 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::GetRegistryKey(HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszOnValue, DWORD dwLen)
{
	LPTSTR pszTmp = _tcsdup(pszSubKey);
	LPTSTR pszName = _tcsrchr(pszTmp, '\\');
	HKEY hTmpKey = NULL;
	DWORD dwType = 0;
	LPTSTR pszVal = NULL;

	if(pszName == NULL)
	{
		free(pszTmp);
		return FALSE;
	}

	pszName = _tcsninc(pszName, 1);
	pszTmp[((int)pszName - (int)pszTmp) - 1] = '\0';
	dprintf("-----------------------------------------------------------------------------");
	dprintf("GetRegistryKey: Looking up \"%s\" in \"%s\"", pszName, pszTmp);

	LONG lRes = RegOpenKeyEx(hKey, pszTmp, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey);
	if(!hTmpKey)
	{
		dprintf("GetRegistryKey: Failed to open \"%s\", error code %08x, returning FALSE", pszTmp, lRes);
		dprintf("-----------------------------------------------------------------------------");
		free(pszTmp);
		return FALSE;
	}

	DWORD dwValLen = dwLen * 2;
	try
	{
		pszVal = new TCHAR[dwValLen];
	}
	catch (std::bad_alloc &)
	{
		return FALSE;
	}
	int nRet = RegQueryValueEx(hTmpKey, pszName, NULL, &dwType, (BYTE*)pszVal, &dwValLen);
	RegCloseKey(hTmpKey);
	if(nRet == ERROR_SUCCESS)
	{
		dprintf("GetRegistryKey: Comparing \"%s\" to \"%s\"", pszVal, pszOnValue);
		if(_tcsicmp(pszVal, pszOnValue) == 0)
		{
			dprintf("GetRegistryKey: matched, returning TRUE");
			dprintf("-----------------------------------------------------------------------------");
			delete[] pszVal;
			free(pszTmp);
			return TRUE;
		}
	}

	dprintf("GetRegistryKey: not matched, returning FALSE");
	dprintf("-----------------------------------------------------------------------------");
	delete[] pszVal;
	free(pszTmp);
	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetRegistryKey:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 7:05:53 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetRegistryKey
// Description	    : 
// Return type		: BOOL 
// Arguments
//     HKEY hKey    : 
//     LPCTSTR pszSubKey: 
//     DWORD dwOnValue: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::GetRegistryKey(HKEY hKey, LPCTSTR pszSubKey, DWORD dwOnValue)
{
	LPTSTR pszTmp = _tcsdup(pszSubKey);
	LPTSTR pszName = _tcsrchr(pszTmp, '\\');
	HKEY hTmpKey = NULL;
	DWORD dwType = REG_DWORD;
	LPTSTR pszVal = NULL;

	if(pszName == NULL)
	{
		free(pszTmp);
		return FALSE;
	}

	pszName = _tcsninc(pszName, 1);
	pszTmp[((int)pszName - (int)pszTmp) - 1] = '\0';
	dprintf("-----------------------------------------------------------------------------");
	dprintf("GetRegistryKey: Looking up \"%s\" in \"%s\"", pszName, pszTmp);

	LONG lRes = RegOpenKeyEx(hKey, pszTmp, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey);
	if(!hTmpKey)
	{
		dprintf("GetRegistryKey: Failed to open \"%s\", error code %08x, returning FALSE", pszTmp, lRes);
		dprintf("-----------------------------------------------------------------------------");
		free(pszTmp);
		return FALSE;
	}

	DWORD dwLen = sizeof(DWORD);
	DWORD dwVal = 0;
	int nRet = RegQueryValueEx(hTmpKey, pszName, NULL, &dwType, (BYTE*)&dwVal, &dwLen);
	RegCloseKey(hTmpKey);
	if(nRet == ERROR_SUCCESS)
	{
		dprintf("GetRegistryKey: Comparing \"%d\" to \"%d\"", dwVal, dwOnValue);
		if(dwVal == dwOnValue)
		{
			dprintf("GetRegistryKey: matched, returning TRUE");
			dprintf("-----------------------------------------------------------------------------");
			delete[] pszVal;
			free(pszTmp);
			return TRUE;
		}
	}

	dprintf("GetRegistryKey: not matched, returning FALSE");
	dprintf("-----------------------------------------------------------------------------");
	delete[] pszVal;
	free(pszTmp);
	return FALSE;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::SetRegistryKey:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/30/99 - 3:29:48 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::SetRegistryKey
// Description	    : 
// Return type		: BOOL 
// Arguments
//     HKEY hKey    : 
//     LPCTSTR pszSubKey: 
//     LPCTSTR pszValue: 
//     DWORD dwLen  : 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::SetRegistryKey(HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValue, DWORD dwLen)
{
	HKEY hTmpKey;
	DWORD dwType = REG_SZ;
	if(pszValue == NULL)
	{
		LPTSTR pszNCSubKey = _tcsdup(pszSubKey);
		LPTSTR pszName = _tcsrchr(pszNCSubKey, '\\');
		if(pszName == NULL)
			RegDeleteValue(hKey, pszNCSubKey);
		else
		{
			pszName = _tcsninc(pszName, 1);
			pszNCSubKey[((int)pszName - (int)pszNCSubKey) - 1] = '\0';
			dprintf("-----------------------------------------------------------------------------");
			dprintf("SetRegistryKey: Setting \"%s\" in \"%s\"", pszName, pszNCSubKey);

			RegOpenKeyEx(hKey, pszNCSubKey, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, &hTmpKey);
			if(!hTmpKey)
			{
				dprintf("Failed to open \"%s\", returning FALSE", pszNCSubKey);
				dprintf("-----------------------------------------------------------------------------");
				free(pszNCSubKey);
				return FALSE;
			}

			RegDeleteValue(hTmpKey, pszName);
			RegFlushKey(hTmpKey);
			RegCloseKey(hTmpKey);
		}

		dprintf("Succeeded in deleting \"%s\\%s\"", pszNCSubKey, pszName);
		dprintf("-----------------------------------------------------------------------------");
		free(pszNCSubKey);
		return TRUE;
	}
	else
	{
		LPTSTR pszNCSubKey = _tcsdup(pszSubKey);
		LPTSTR pszName = _tcsrchr(pszNCSubKey, '\\');
		DWORD dwVal;
		if(pszName == NULL)
			RegSetValue(hKey, pszNCSubKey, dwType, pszValue, dwLen);
		else
		{
			pszName = _tcsninc(pszName, 1);
			pszNCSubKey[((int)pszName - (int)pszNCSubKey) - 1] = '\0';
			RegCreateKeyEx(hKey, pszNCSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hTmpKey, &dwVal);
			if(!hTmpKey)
			{
				dprintf("Failed to create \"%s\", returning FALSE", pszNCSubKey);
				dprintf("-----------------------------------------------------------------------------");
				free(pszNCSubKey);
				return FALSE;
			}

			RegSetValueEx(hTmpKey, pszName, NULL, dwType, (BYTE *)pszValue, dwLen);
			RegCloseKey(hTmpKey);
		}

		dprintf("Succeeded in setting \"%s\\%s\" to \"%s\"", pszNCSubKey, pszName, pszValue);
		dprintf("-----------------------------------------------------------------------------");
		free(pszNCSubKey);
		return TRUE;
	}

	dprintf("SetRegistryKey: failed, returning FALSE");
	dprintf("-----------------------------------------------------------------------------");
	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::SetRegistryKey:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 7:06:39 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::SetRegistryKey
// Description	    : 
// Return type		: BOOL 
// Arguments
//     HKEY hKey    : 
//     LPCTSTR pszSubKey: 
//     DWORD dwValue: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::SetRegistryKey(HKEY hKey, LPCTSTR pszSubKey, DWORD dwValue)
{
	HKEY hTmpKey;
	DWORD dwType = REG_DWORD;

	LPTSTR pszNCSubKey = _tcsdup(pszSubKey);
	LPTSTR pszName = _tcsrchr(pszNCSubKey, '\\');
	DWORD dwVal;
	if(pszName == NULL)
		RegSetValueEx(hKey, pszSubKey, NULL, dwType, (BYTE*)&dwValue, sizeof(DWORD));
	else
	{
		pszName = _tcsninc(pszName, 1);
		pszNCSubKey[((int)pszName - (int)pszNCSubKey) - 1] = '\0';
		RegCreateKeyEx(hKey, pszNCSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hTmpKey, &dwVal);
		if(!hTmpKey)
		{
			dprintf("Failed to create \"%s\", returning FALSE", pszNCSubKey);
			dprintf("-----------------------------------------------------------------------------");
			free(pszNCSubKey);
			return FALSE;
		}

		RegSetValueEx(hTmpKey, pszName, NULL, dwType, (BYTE*)&dwValue, sizeof(dwValue));
		RegCloseKey(hTmpKey);
		dprintf("Succeeded in setting \"%s\\%s\" to \"%d\"", pszNCSubKey, pszName, dwValue);
		dprintf("-----------------------------------------------------------------------------");
		free(pszNCSubKey);
			return TRUE;
	}


	dprintf("SetRegistryKey: failed, returning FALSE");
	dprintf("-----------------------------------------------------------------------------");
	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetRegistryKeyValue:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/28/99 - 11:55:38 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetRegistryKeyValue
// Description	    : 
// Return type		: LONG 
// Arguments
//     HKEY key     : 
//     LPCTSTR lpszSubkey: 
//     DWORD* pdwValue: 
// 
/////////////////////////////////////////////////////////////////////////

LONG CUtils::GetRegistryKeyValue(HKEY key, LPCTSTR lpszSubkey, DWORD* pdwValue)
{
	LPTSTR pszTmp = _tcsdup(lpszSubkey);
	LPTSTR pszName = _tcsrchr(pszTmp, '\\');
	HKEY hTmpKey = NULL;
	DWORD dwType = REG_DWORD;
	DWORD dwLen = sizeof(DWORD);

	if(pszName)
	{
		pszName = _tcsninc(pszName, 1);
		pszTmp[((int)pszName - (int)pszTmp) - 1] = '\0';
		LONG ret = RegOpenKeyEx(key, pszTmp, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey);
		if(ret == ERROR_SUCCESS && hTmpKey)
		{
			ret = RegQueryValueEx(hTmpKey, pszName, NULL, &dwType, (BYTE*)pdwValue, &dwLen);
			RegCloseKey(hTmpKey);
		}
		free(pszTmp);
		return ret;
	}

	free(pszTmp);
	return NULL;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetRegistryKeyValue:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/21/99 - 1:54:39 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetRegistryKeyValue
// Description	    : 
// Return type		: LONG 
// Arguments
//     HKEY key     : 
//     LPCTSTR lpszSubkey: 
//     LPTSTR lpszValue: 
//     UINT nMaxTextLen:
// 
/////////////////////////////////////////////////////////////////////////

LONG CUtils::GetRegistryKeyValue(HKEY key, LPCTSTR lpszSubkey, LPTSTR lpszValue, UINT nMaxTextLen)
{
	if(!lpszValue || !lpszSubkey)
		return -1;

	HKEY hKey = NULL;
    LONG ret = 2;

    if((ret = RegOpenKeyEx(key, lpszSubkey, 0, KEY_READ, &hKey)) == ERROR_SUCCESS)
	{
		long size = _tcslen(lpszValue);
		RegQueryValue(hKey, NULL, lpszValue, &size);
		RegCloseKey(hKey);
		return ret;
	}
	else
	{
		// Try to use the last part of the key as a string value instead
		// of a key name
		LPTSTR pszTmp = _tcsdup(lpszSubkey);
		LPTSTR pszName = _tcsrchr(pszTmp, '\\');
		HKEY hTmpKey = NULL;
		DWORD dwType;
		DWORD dwLen = nMaxTextLen;

		if(pszName)
		{
			pszName = _tcsninc(pszName, 1);
			pszTmp[((int)pszName - (int)pszTmp) - 1] = '\0';
			ret = RegOpenKeyEx(key, pszTmp, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey);
			if(ret == ERROR_SUCCESS && hTmpKey)
			{
				if((ret = RegQueryValueEx(hTmpKey, pszName, NULL, &dwType, (BYTE*)lpszValue, &dwLen)) == ERROR_SUCCESS)
				{
					RegCloseKey(hTmpKey);
					free(pszTmp);
					return ret;
				}
			}
			free(pszTmp);
			return ret;
		}

		free(pszTmp);
		return ret;
	}

    return ret;
}
//////////////////////////////////////////////////////////////////////
// Shell Utilities
//////////////////////////////////////////////////////////////////////

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateShortcut:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/30/99 - 3:32:22 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateShortcut
// Description	    : 
// Return type		: BOOL 
// Arguments
//     LPCTSTR lpszDesc: Shortcuts text description
//     LPCTSTR lpszObjPath: Path\File to source file (source)
//     LPCTSTR lpszLinkPath: Path\File where link will go (dest, must end with ".lnk")
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::CreateShortcut(LPCTSTR lpszDesc, LPCTSTR lpszObjPath, LPCTSTR lpszLinkPath, LPCTSTR lpszIconFile, DWORD dwIconIndex, LPCTSTR lpszArguments)
{
	HRESULT hr = E_FAIL;
	BOOL bRet = FALSE;
	IShellLink* psl;
	
	if(SUCCEEDED(hr=CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl)))
	{
		IPersistFile* ppf;
		
		psl->SetPath(lpszObjPath);
		psl->SetDescription(lpszDesc);
		if (lpszIconFile)
			psl->SetIconLocation(lpszIconFile, dwIconIndex);

		if (lpszArguments)
			psl->SetArguments(lpszArguments);

		if(SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf)))
		{
			WCHAR wsz[MAX_PATH];
			MultiByteToWideChar(CP_ACP, 0, lpszLinkPath, -1, wsz, MAX_PATH);
			
			if(SUCCEEDED(ppf->Save(wsz, FALSE)))
				bRet = TRUE;
			
			ppf->Release();
		}
		psl->Release();
	}

	return bRet;
}



#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateShortcut:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/30/99 - 4:01:54 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateShortcut
// Description	    : 
// Return type		: BOOL 
// Arguments
//     LPCTSTR lpszDesc:
//     LPCTSTR lpszObjPath: 
//     int nCSIDLFolder: CSIDL folder ID that the .lnk file will go in
//     LPCTSTR lpszFile: the .lnk file
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::CreateShortcut(LPCTSTR lpszDesc, LPCTSTR lpszObjPath, int nCSIDLFolder, LPCTSTR lpszFile)
{
	TCHAR szLinkPath[MAX_PATH * 2];
	TCHAR szPathFile[MAX_PATH * 2];
	BOOL bRet = FALSE;
	if(GetSystemPath(nCSIDLFolder, szLinkPath))
	{
		wsprintf(szPathFile, "%s\\%s", szLinkPath, lpszFile);
		bRet = CreateShortcut(lpszDesc, lpszObjPath, szPathFile);
	}

	return bRet;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateShortcut:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 3:50:23 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateShortcut
// Description	    : 
// Return type		: BOOL 
// Arguments
//     LPCTSTR lpszDesc: 
//     LPCTSTR lpszObjPath: 
//     int nCSIDLFolder: 
//     UINT nFileNameID: 
//     HINSTANCE hInst: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::CreateShortcut(LPCTSTR lpszDesc, LPCTSTR lpszObjPath, int nCSIDLFolder, UINT nFileNameID, HINSTANCE hInst)
{
	LPTSTR pszFileName = NULL;
	try
	{
		pszFileName = new TCHAR[MAX_LOADSTRING + _tcslen(_T(".lnk"))];
	}
	catch (std::bad_alloc &)
	{
		;
		return FALSE;
	}
	LoadString(hInst, nFileNameID, pszFileName, MAX_LOADSTRING);
	_tcscat(pszFileName, _T(".lnk"));
	BOOL bRet = CreateShortcut(lpszDesc, lpszObjPath, nCSIDLFolder, pszFileName);
	delete[] pszFileName;
	return bRet;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetSystemPath:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 6/15/99 - 11:05:23 AM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetSystemPath
// Description	    : 
// Return type		: BOOL 
// Arguments
//     int nCSIDLFolder: CSIDL folder ID, such as: CSIDL_STARTUP
//     LPTSTR pszPath: Buffer to be filled with the path
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::GetSystemPath(int nCSIDLFolder, LPTSTR pszPath)
{
	LPITEMIDLIST pidl;
	BOOL bRet = FALSE;
	if(SUCCEEDED(SHGetSpecialFolderLocation(NULL, nCSIDLFolder, &pidl)))
	{
		bRet = SHGetPathFromIDList(pidl, pszPath);

		// Free the pidl
		LPMALLOC pMalloc;
		SHGetMalloc(&pMalloc);
		pMalloc->Free(pidl);
		pMalloc->Release();
	}
	return bRet;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::DeleteFileInSysPath:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/30/99 - 5:05:04 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::DeleteFileInSysPath
// Description	    : 
// Return type		: BOOL 
// Arguments
//     int nCSIDLFolder: 
//     LPTSTR pszFileName: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::DeleteFileInSysPath(int nCSIDLFolder, LPTSTR pszFileName)
{
	BOOL bRet = FALSE;
	TCHAR szPath[MAX_PATH * 2];
	if(GetSystemPath(nCSIDLFolder, szPath))
	{
		TCHAR szFile[MAX_PATH * 2];
		wsprintf(szFile, "%s\\%s", szPath, pszFileName);
		bRet = (_tremove(szFile) != -1);
	}

	return bRet;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::DeleteLnkInSysPath:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 3:56:48 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::DeleteLnkInSysPath
// Description	    : 
// Return type		: BOOL 
// Arguments
//     int nCSIDLFolder: 
//     UINT nLinkNameID: 
//     HINSTANCE hInst: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::DeleteLnkInSysPath(int nCSIDLFolder, UINT nLinkNameID, HINSTANCE hInst)
{
	LPTSTR pszLinkName = NULL;
	try
	{
		pszLinkName = new TCHAR[MAX_LOADSTRING + _tcslen(_T(".lnk"))];
	}
	catch(std::bad_alloc &)
	{
		return FALSE;
	}
	LoadString(hInst, nLinkNameID, pszLinkName, MAX_LOADSTRING);
	_tcscat(pszLinkName, _T(".lnk"));
	BOOL bRet = DeleteFileInSysPath(nCSIDLFolder, pszLinkName);
	delete[] pszLinkName;
	return bRet;	
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::FileExistsInSysPath:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 3:25:48 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::FileExistsInSysPath
// Description	    : 
// Return type		: BOOL 
// Arguments
//     int nCSIDLFolder: 
//     LPTSTR pszFileName: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::FileExistsInSysPath(int nCSIDLFolder, LPTSTR pszFileName)
{
	BOOL bRet = FALSE;
	TCHAR szPath[MAX_PATH * 2];
	if(GetSystemPath(nCSIDLFolder, szPath))
	{
		TCHAR szFile[MAX_PATH * 2];
		wsprintf(szFile, "%s\\%s", szPath, pszFileName);
		bRet = (_taccess(szFile, 00) == 0);
	}

	return bRet;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::LnkExistsInSysPath:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 3:52:49 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::LnkExistsInSysPath
// Description	    : 
// Return type		: BOOL 
// Arguments
//     int nCSIDLFolder: 
//     UINT nLinkNameID: 
//     HINSTANCE hInst: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::LnkExistsInSysPath(int nCSIDLFolder, UINT nLinkNameID, HINSTANCE hInst)
{
	LPTSTR pszLinkName = NULL;
	try
	{
		pszLinkName = new TCHAR[MAX_LOADSTRING + _tcslen(_T(".lnk"))];
	}
	catch(std::bad_alloc &)
	{
		;
		return FALSE;
	}
	LoadString(hInst, nLinkNameID, pszLinkName, MAX_LOADSTRING);
	_tcscat(pszLinkName, _T(".lnk"));
	BOOL bRet = FileExistsInSysPath(nCSIDLFolder, pszLinkName);
	delete[] pszLinkName;
	return bRet;	
}


/////////////////////////////////////////////////////////////////////////////
// OS Utils
/////////////////////////////////////////////////////////////////////////////


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsOSGoldOrOSR2:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 4:11:37 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsOSGoldOrOSR2
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsOSGoldOrOSR2()
{
	OSVERSIONINFO osInfo;
	memset(&osInfo, 0, sizeof(OSVERSIONINFO));
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&osInfo) && 
		(osInfo.dwPlatformId & VER_PLATFORM_WIN32_WINDOWS)
		 && (osInfo.dwMajorVersion == 4)
		 && (osInfo.dwMinorVersion == 0))
	{
		return TRUE;
	}

	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsSystemHCMode:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/7/99 - 6:08:48 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsSystemHCMode
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsSystemHCMode()
{
	OSVERSIONINFO osInfo;
	memset(&osInfo, 0, sizeof(OSVERSIONINFO));
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&osInfo) && 
		((osInfo.dwPlatformId & VER_PLATFORM_WIN32_WINDOWS)
		|| (osInfo.dwPlatformId & VER_PLATFORM_WIN32_NT && osInfo.dwMajorVersion >= 5)))
	{
		HIGHCONTRAST hcInfo;
		memset(&hcInfo, 0, sizeof(HIGHCONTRAST));
		hcInfo.cbSize = sizeof(HIGHCONTRAST);

		SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hcInfo, 0);

		if(hcInfo.dwFlags & HCF_HIGHCONTRASTON)
			return TRUE;
	}	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Misc Utils
/////////////////////////////////////////////////////////////////////////////


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateFontEz:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 6/3/99 - 6:29:48 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateFontEz
// Description	    : Reads a resource string that is formated like this:
//                    ";Arial;18;0"   or    "/Arial/18/0"
//                    The first character is the delimiter, followed by
//                    the font name, then the size, then the attributes
//                    to set attributes, OR together the values in the 
//                    utils.h header
// Return type		: HFONT 
// Arguments
//     HDC hdc      : 
//     UINT nFaceNameID: 
//     HINSTANCE hInst:
//     DWORD dwAddAttrib:
// 
/////////////////////////////////////////////////////////////////////////

HFONT CUtils::CreateFontEz(HDC hdc, UINT nFaceNameID, DWORD dwAddAttrib, HINSTANCE hInst)
{
	TCHAR szData[MAX_LOADSTRING] = {0};
	LPTSTR pszData = szData;
	TCHAR szDelim[2] = {0, 0};
	TCHAR szFont[33] = {0};
	UINT uSize = 0;
	UINT uAttributes = 0;

	if(hInst == INVALID_HANDLE_VALUE)
		hInst = GetModuleHandle(NULL);
	
	// Load the data string
	if(!LoadString(hInst, nFaceNameID, szData, MAX_LOADSTRING))
		return NULL;

	// Get the delimiter
	_tcsncpy(szDelim, pszData, 1);
	pszData = _tcsninc(pszData, 1);

	// Get the font name	
	LPTSTR pszTemp = _tcstok(pszData, szDelim);
	if(pszTemp == NULL)
		return NULL;
	else
		_tcsncpy(szFont, pszTemp, 32);

	// Get the point size
	pszTemp = _tcstok(NULL, szDelim);
	if(pszTemp == NULL)
		return NULL;
	else
		uSize = _ttoi(pszTemp);

	// Get the font attributes
	pszTemp = _tcstok(NULL, szDelim);
	if(pszTemp == NULL)
		return NULL;
	else
		uAttributes = _ttoi(pszTemp);

	return CreateFontEz(hdc, szFont, uSize, uAttributes | dwAddAttrib);
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateFontEz:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/5/99 - 12:55:36 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateFontEz
// Description	    : 
// Return type		: HFONT 
// Arguments
//     HDC hdc      : 
//     UINT nFaceNameID: 
//     HINSTANCE hInst: 
//     UINT uPtSize : 
//     DWORD dwAttributes: 
// 
/////////////////////////////////////////////////////////////////////////

HFONT CUtils::CreateFontEz(HDC hdc, UINT nFaceNameID, HINSTANCE hInst, UINT uPtSize, DWORD dwAttributes)
{
	TCHAR szFont[32];
	LoadString(hInst, nFaceNameID, szFont, 32);
	return CreateFontEz(hdc, szFont, uPtSize, dwAttributes);
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::CreateFontEz:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/16/99 - 1:34:20 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::CreateFontEz
// Description	    : 
// Return type		: HFONT 
// Arguments
//     HDC hdc      : 
//     LPTSTR szFaceName: 
//     UINT uPtSize : 
//     DWORD dwAttributes: 
// 
/////////////////////////////////////////////////////////////////////////

HFONT CUtils::CreateFontEz(HDC hdc, LPTSTR szFaceName, UINT uPtSize, DWORD dwAttributes)
{
	if(_tcsicmp(szFaceName, _szDefaultFont) == 0)
		return (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	
	int nLogPxY = GetDeviceCaps(hdc, LOGPIXELSY);
	if(nLogPxY != 96)
	{
		dprintf("CreateFontEz: LOGPIXELSY is not 96, it is %d, using 96 anyway...", nLogPxY);
		nLogPxY = 96;
	}

	LOGFONT lf;
	lf.lfHeight         = -MulDiv(uPtSize, nLogPxY, 72);
	lf.lfWidth          = 0;
	lf.lfEscapement     = lf.lfOrientation = 0;
	lf.lfWeight         = dwAttributes & CFEZ_ATTR_BOLD      ?  700 : 0;
	lf.lfItalic         = dwAttributes & CFEZ_ATTR_ITALIC    ? TRUE : FALSE;
	lf.lfUnderline      = dwAttributes & CFEZ_ATTR_UNDERLINE ? TRUE : FALSE;
	lf.lfStrikeOut      = dwAttributes & CFEZ_ATTR_STRIKEOUT ? TRUE : FALSE;
	lf.lfCharSet        = GetDBCSCharSet();
	lf.lfOutPrecision   = OUT_TT_ONLY_PRECIS;		// Try to get a truetype font
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lf.lfQuality        = ANTIALIASED_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
	_tcscpy(lf.lfFaceName, szFaceName);

	HFONT hFontRet = CreateFontIndirect(&lf);
	if(hFontRet == NULL)
	{
		// CreateFont failed ... let the system pick a similar font
		_tcscpy(lf.lfFaceName, _T(""));
		hFontRet = CreateFontIndirect(&lf);
	}
	
	return hFontRet;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetDBCSCharSet:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/15/99 - 4:19:33 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetDBCSCharSet
// Description	    : 
// Return type		: BYTE 
// 
/////////////////////////////////////////////////////////////////////////

BYTE CUtils::GetDBCSCharSet()
{
	if(!GetSystemMetrics(SM_DBCSENABLED))
		return ANSI_CHARSET;

	switch(GetACP())
	{
		case _nCODEPAGE_CHINESE_TRADITIONAL:
			return CHINESEBIG5_CHARSET;

		case _nCODEPAGE_CHINESE_SIMPLIFIED:
			return GB2312_CHARSET;

		case _nCODEPAGE_KOREAN_WANSUNG:                  
		case _nCODEPAGE_KOREAN_JOHAB:
			return HANGEUL_CHARSET;

		default:                    // Assume CODEPAGE_JAPAN
			return SHIFTJIS_CHARSET;
	}
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetID:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/15/99 - 4:19:34 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetID
// Description	    : 
// Return type		: UINT 
// Arguments
//     UINT nID     : 
// 
/////////////////////////////////////////////////////////////////////////

UINT CUtils::GetID(UINT nID)
{
	if(!GetSystemMetrics(SM_DBCSENABLED))
		return (nID);
    
	switch(GetACP())
	{
		case _nCODEPAGE_CHINESE_TRADITIONAL:
		case _nCODEPAGE_CHINESE_SIMPLIFIED:
			return nID+1;

		case _nCODEPAGE_KOREAN_WANSUNG:                  
		case _nCODEPAGE_KOREAN_JOHAB:
			return nID+2;

		default:	// Assume CODEPAGE_JAPAN
			return nID+3;
	}
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsWebBrowserInstalled:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 6/15/99 - 10:47:16 AM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsWebBrowserInstalled
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsWebBrowserInstalled()
{
	TCHAR key[MAX_PATH * 3];

	// get the .htm regkey and lookup the program
	LRESULT lRes = GetRegistryKeyValue(HKEY_CLASSES_ROOT, _T(".htm"), key, MAX_PATH + 1024);
	if(lRes != ERROR_SUCCESS)
		lRes = GetRegistryKeyValue(HKEY_CLASSES_ROOT, _T(".html"), key, MAX_PATH + 1024);

	if(lRes == ERROR_SUCCESS)
	{
		return TRUE;
	}

	return FALSE;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GotoURL:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/21/99 - 1:49:43 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GotoURL
// Description	    : 
// Return type		: HINSTANCE 
// Arguments
//     UINT nUrlID  : 
//     int nShowcmd : 
//     HINSTANCE hInst: 
// 
/////////////////////////////////////////////////////////////////////////

HINSTANCE CUtils::GotoURL(UINT nUrlID, int nShowcmd, HINSTANCE hInst)
{
	TCHAR* pszURL = NULL;
	try
	{
		pszURL = new TCHAR[1024];
	}
	catch(std::bad_alloc &)
	{
		;
		return NULL;
	}
	LoadString(hInst, nUrlID, pszURL, 1023);
	hInst = GotoURL(pszURL, nShowcmd);
	delete[] pszURL;
	return hInst;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GotoURL:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/21/99 - 1:49:50 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GotoURL
// Description	    : 
// Return type		: HINSTANCE 
// Arguments
//     LPCTSTR pszUrl: 
//     int nShowcmd : 
// 
/////////////////////////////////////////////////////////////////////////

HINSTANCE CUtils::GotoURL(LPCTSTR pszUrl, int nShowcmd)
{
	TCHAR key[MAX_PATH + 1024];

	// First try ShellExecute()
	HINSTANCE hInstRet = ShellExecute(NULL, _T("open"), pszUrl, NULL,NULL, nShowcmd);

	// If it failed, get the .htm regkey and lookup the program
	if((UINT)hInstRet <= HINSTANCE_ERROR)
	{
		LRESULT lRes = GetRegistryKeyValue(HKEY_CLASSES_ROOT, _T(".htm"), key, MAX_PATH + 1024);
		if(lRes != ERROR_SUCCESS)
			lRes = GetRegistryKeyValue(HKEY_CLASSES_ROOT, _T(".html"), key, MAX_PATH + 1024);

		if(lRes == ERROR_SUCCESS)
		{
			_tcscat(key, _T("\\shell\\open\\command"));

			if(GetRegistryKeyValue(HKEY_CLASSES_ROOT, key, key, MAX_PATH + 1024) == ERROR_SUCCESS)
			{
				TCHAR *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if(pos == NULL)							// No quotes found
				{
					pos = _tcsstr(key, _T("%1"));		// Check for %1, without quotes 
					if (pos == NULL)					// No parameter at all...
						pos = key + _tcslen(key) -1;
					else
						*pos = '\0';					// Remove the parameter
				}
				else
					*pos = '\0';						// Remove the parameter

				_tcscat(pos, _T(" "));
				_tcscat(pos, pszUrl);
				hInstRet = (HINSTANCE)WinExec(key, nShowcmd);
			}
		}
	}

	return hInstRet;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::ResMessageBox:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/6/99 - 2:35:12 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::ResMessageBox
// Description	    : 
// Return type		: int 
// Arguments
//     HWND hWnd    : 
//     UINT nMessageID: 
//     UINT nCaptionID: 
//     UINT nType   : 
//     HINSTANCE hInst: 
// 
/////////////////////////////////////////////////////////////////////////

int CUtils::ResMessageBox(HWND hWnd, UINT nMessageID, UINT nCaptionID, UINT nType, HINSTANCE hInst)
{
	TCHAR szText[MAX_LOADSTRING];
	TCHAR szCaption[MAX_LOADSTRING];
	LoadString(hInst, nMessageID, szText, MAX_LOADSTRING);
	LoadString(hInst, nCaptionID, szCaption, MAX_LOADSTRING);
	return MessageBox(hWnd, szText, szCaption, nType);
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::ResSMessageBox:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/6/99 - 2:37:23 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::ResSMessageBox
// Description	    : 
// Return type		: int 
// Arguments
//     HWND hWnd    : 
//     UINT nFormatStringID: 
//     UINT nCaptionID: 
//     UINT nType   : 
//     HINSTANCE hInst: 
//     ...          : 
// 
/////////////////////////////////////////////////////////////////////////

int CUtils::ResSMessageBox(HWND hWnd, UINT nFormatStringID, UINT nCaptionID, UINT nType, HINSTANCE hInst, ...)
{
	TCHAR szLoadStr[MAX_LOADSTRING];
	TCHAR szText[MAX_LOADSTRING * 3];
	TCHAR szCaption[MAX_LOADSTRING];
	LoadString(hInst, nFormatStringID, szLoadStr, MAX_LOADSTRING);
	LoadString(hInst, nCaptionID, szCaption, MAX_LOADSTRING);

	va_list args;
	va_start(args, hInst);
	_vstprintf(szText, szLoadStr, args);
	va_end(args);

	return MessageBox(hWnd, szText, szCaption, nType);
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetAppPath:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/27/99 - 11:05:36 AM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetAppPath
// Description	    : 
// Return type		: LPTSTR 
// Arguments
//     LPCTSTR lpszAppExeName: 
//     LPTSTR pszPath:
//
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::GetAppPath(LPCTSTR lpszAppExeName, LPTSTR pszPath)
{
	if(lpszAppExeName == NULL)
	{
		dprintf("GetAppPath: failed (bad parameter), returning NULL");
		return FALSE;
	}
	
	dprintf("-----------------------------------------------------------------------------");
	dprintf("GetAppPath: Looking up \"%s\\%s\"", _szInstalledAppKey, lpszAppExeName);
	HKEY hTmpKey = NULL;
	DWORD dwType = REG_SZ, dwLen = MAX_PATH;

	LPTSTR pszKey = NULL;
	try
	{
		pszKey = new TCHAR[_tcslen(_szInstalledAppKey) + _tcslen(lpszAppExeName) + 2];
	}
	catch (std::bad_alloc &)
	{
		;
		return FALSE;
	}

	wsprintf(pszKey, "%s\\%s", _szInstalledAppKey, lpszAppExeName);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszKey, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey);
	delete[] pszKey;
	if(hTmpKey)
	{
		TCHAR szVal[MAX_PATH * 2];
		int nRet = RegQueryValueEx(hTmpKey, _T("Path"), NULL, &dwType, (BYTE*)szVal, &dwLen);
		RegCloseKey(hTmpKey);
		if(nRet == ERROR_SUCCESS)
		{
			dprintf("GetAppPath: succeeded, returning \"%s\"", szVal);
			dprintf("-----------------------------------------------------------------------------");
			_tcscpy(pszPath, szVal);
			return TRUE;
		}
	}

	dprintf("GetAppPath: failed, returning NULL");
	dprintf("-----------------------------------------------------------------------------");
	_tcscpy(pszPath, _T(""));
	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsAppInstalled:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/27/99 - 11:06:53 AM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsAppInstalled
// Description	    : 
// Return type		: BOOL 
// Arguments
//     LPCTSTR lpszFileName: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsAppInstalled(LPCTSTR lpszFileName)
{
	if(lpszFileName == NULL)
	{
		dprintf("IsAppInstalled: failed (bad parameter), returning NULL");
		return FALSE;
	}
	
	dprintf("-----------------------------------------------------------------------------");
	dprintf("IsAppInstalled: Looking up \"%s\\%s\"", _szInstalledAppKey, lpszFileName);
	HKEY hTmpKey = NULL;
	DWORD dwType = REG_SZ, dwLen = MAX_PATH;

	LPTSTR pszKey = NULL;
	try
	{
		pszKey = new TCHAR[_tcslen(_szInstalledAppKey) + _tcslen(lpszFileName) + 2];
	}
	catch (std::bad_alloc &)
	{
		;
		return FALSE;
	}
	wsprintf(pszKey, "%s\\%s", _szInstalledAppKey, lpszFileName);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszKey, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey);
	delete[] pszKey;
	if(hTmpKey)
	{
		TCHAR szVal[MAX_PATH * 2];
		int nRet = RegQueryValueEx(hTmpKey, NULL, NULL, &dwType, (BYTE*)szVal, &dwLen);
		RegCloseKey(hTmpKey);
		if(nRet == ERROR_SUCCESS && (_taccess(szVal, 00) == 0))
		{
			dprintf("IsAppInstalled: succeeded, app=%s", szVal);
			dprintf("-----------------------------------------------------------------------------");
			return TRUE;
		}
	}

	dprintf("IsAppInstalled: failed, returning FALSE");
	dprintf("-----------------------------------------------------------------------------");
	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::LaunchApp:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/27/99 - 11:07:06 AM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::LaunchApp
// Description	    : 
// Return type		: HINSTANCE 
// Arguments
//     LPCTSTR lpszFileName: 
// 
/////////////////////////////////////////////////////////////////////////

HINSTANCE CUtils::LaunchApp(LPCTSTR lpszFileName)
{
	if(lpszFileName == NULL)
	{
		dprintf("LaunchApp: failed (bad parameter), returning NULL");
		return NULL;
	}
	
	dprintf("-----------------------------------------------------------------------------");
	dprintf("LaunchApp: Looking up \"%s\\%s\"", _szInstalledAppKey, lpszFileName);
	HKEY hTmpKey = NULL;
	DWORD dwType = REG_SZ, dwLen = MAX_PATH;

	LPTSTR pszKey = NULL;
	try
	{
		pszKey = new TCHAR[_tcslen(_szInstalledAppKey) + _tcslen(lpszFileName) + 2];
	}
	catch (std::bad_alloc &)
	{
		;
		return NULL;
	}
	wsprintf(pszKey, "%s\\%s", _szInstalledAppKey, lpszFileName);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszKey, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey);
	delete[] pszKey;
	if(hTmpKey)
	{
		TCHAR szVal[MAX_PATH * 2];
		int nRet = RegQueryValueEx(hTmpKey, NULL, NULL, &dwType, (BYTE*)szVal, &dwLen);
		RegCloseKey(hTmpKey);
		if(nRet == ERROR_SUCCESS && (_taccess(szVal, 00) == 0))
		{
			dprintf("LaunchApp: launching, app=%s", szVal);
			dprintf("-----------------------------------------------------------------------------");
			return (HINSTANCE)WinExec(szVal, SW_NORMAL);
		}
	}

	dprintf("LaunchApp: failed, returning NULL");
	dprintf("-----------------------------------------------------------------------------");
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Color functions
/////////////////////////////////////////////////////////////////////////////

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::RGBtoPALRGB:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/17/99 - 6:03:10 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::RGBtoPALRGB
// Description	    : 
// Return type		: inline COLORREF 
// Arguments
//     COLORREF cr  : 
// 
/////////////////////////////////////////////////////////////////////////

inline COLORREF CUtils::RGBtoPALRGB(COLORREF cr)
{
	return cr | 0x02000000;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::HLStoRGB:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/17/99 - 6:20:13 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::HLStoRGB
// Description	    : 
// Return type		: inline COLORREF 
// Arguments
//     DOUBLE dHue  : 
//     DOUBLE dLuminance: 
//     DOUBLE dSaturation: 
// 
/////////////////////////////////////////////////////////////////////////

inline COLORREF CUtils::HLStoRGB(DOUBLE dHue, DOUBLE dLuminance, DOUBLE dSaturation)
{
	DOUBLE dRed   = dLuminance + 1.402 * (dSaturation - 128);
	DOUBLE dGreen = dLuminance - 0.34414 * (dHue - 128) - 0.71414 * (dSaturation - 128);
	DOUBLE dBlue  = dLuminance + 1.772 * (dHue - 128);
	return RGB(dRed, dGreen, dBlue);
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::RGBtoHLS:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 7/17/99 - 6:03:04 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::RGBtoHLS
// Description	    : 
// Return type		: inline VOID 
// Arguments
//     COLORREF clrRGB: 
//     DOUBLE *pdHue: 
//     DOUBLE *pdLuminance: 
//     DOUBLE *pdSaturation: 
// 
/////////////////////////////////////////////////////////////////////////

inline VOID CUtils::RGBtoHLS(COLORREF clrRGB, DOUBLE *pdHue, DOUBLE *pdLuminance, DOUBLE *pdSaturation)
{
	WORD wRed = GetRValue(clrRGB);
	WORD wGreen = GetGValue(clrRGB);
	WORD wBlue = GetBValue(clrRGB);

	*pdLuminance = 0.299 * wRed + 0.587 * wGreen + 0.114 * wBlue;
	*pdHue =  -0.1687 * wRed - 0.3313 * wGreen + 0.5 * wBlue + 128;
	*pdSaturation = 0.5 * wRed - 0.4187 * wGreen - 0.0813 * wBlue + 128;

}

/////////////////////////////////////////////////////////////////////////////
// Symantec specific functions
/////////////////////////////////////////////////////////////////////////////

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::GetSymAppPath:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/30/99 - 10:19:47 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::GetSymAppPath
// Description	    : 
// Return type		: BOOL 
// Arguments
//     LPCTSTR lpszAppName: 
//     LPTSTR pszPath:
//
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::GetSymAppPath(LPCTSTR lpszAppName, LPTSTR pszPath)
{
	if(lpszAppName == NULL)
	{
		dprintf("GetSymAppPath: failed (bad parameter), returning NULL");
		return FALSE;
	}
	
	dprintf("-----------------------------------------------------------------------------");
	dprintf("GetSymAppPath: Looking up \"%s\\%s\"", _szSymInstalledAppKey, lpszAppName);
	HKEY hTmpKey = NULL;
	DWORD dwType = REG_SZ, dwLen = MAX_PATH;

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, _szSymInstalledAppKey, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey);
	if(hTmpKey)
	{
		TCHAR szVal[MAX_PATH * 2];
		int nRet = RegQueryValueEx(hTmpKey, lpszAppName, NULL, &dwType, (BYTE*)szVal, &dwLen);
		RegCloseKey(hTmpKey);
		if(nRet == ERROR_SUCCESS)
		{
			dprintf("GetSymAppPath: succeeded, returning \"%s\"", szVal);
			dprintf("-----------------------------------------------------------------------------");
			_tcscpy(pszPath, szVal);
			return TRUE;
		}
	}

	dprintf("GetSymAppPath: failed, returning NULL");
	dprintf("-----------------------------------------------------------------------------");
	_tcscpy(pszPath, _T(""));
	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsSymAppInstalled:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 4:31:34 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsSymAppInstalled
// Description	    : 
// Return type		: BOOL 
// Arguments
//     LPCTSTR lpszAppName: 
//     LPCTSTR lpszFileName: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsSymAppInstalled(LPCTSTR lpszAppName, LPCTSTR lpszFileName)
{
	// Debug code added by RT.  Will always return true if this file exists.
	if(_taccess("c:\\fakeinst.rt",00) == 0)
		return TRUE;

	TCHAR szAppPath[MAX_PATH * 2];
	if(!GetSymAppPath(lpszAppName, szAppPath))
		return FALSE;

	TCHAR szFile[MAX_PATH];
	wsprintf(szFile, "%s\\%s", szAppPath, lpszFileName);
	BOOL bRet = (_taccess(szFile, 00) == 0);
	return bRet;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : GetSymAppVersion:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/28/99 - 12:24:36 PM (Shaun Cooley) /////////////////////
// 
// Function name	: GetSymAppVersion
// Description	    : 
// Return type		: DWORD 
// Arguments
//     LPTSTR lpszAppRegName: 
// 
/////////////////////////////////////////////////////////////////////////

DWORD CUtils::GetSymAppVersion(LPTSTR lpszAppRegName)
{
	TCHAR szRegKey[1024];
	_tcscpy(szRegKey, _szSymRegKey);
	_tcscat(szRegKey, lpszAppRegName);

	return FindVersion(HKEY_LOCAL_MACHINE, szRegKey);
}

DWORD CUtils::FindVersion(HKEY hStartAt, LPTSTR pszStartAt)
{
	HKEY hKey;
	BOOL bWalkChildren = TRUE;
	FILETIME ft;
	TCHAR szKeyName[1024];
	DWORD dwSize = 1023;
	DWORD dwLoc = 0;
	WORD wVerMajor = 0, wVerMinor = 0;

	RegOpenKeyEx(hStartAt, pszStartAt, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKey);
	if(!hKey)
		return 0;

	while(ERROR_SUCCESS == RegEnumKeyEx(hKey, dwLoc, szKeyName, &dwSize, NULL, NULL, NULL, &ft))
	{
		WORD wVerMajorTmp = 0, wVerMinorTmp = 0;
		// do something with the info
		++dwLoc;
		if(iswdigit(szKeyName[0]))
		{
			LPTSTR pszPeriod = _tcsstr(szKeyName, _T("."));
			if(pszPeriod)
			{
				// Looks like a version #, don't walk child items
				bWalkChildren = FALSE;
				_tcscpy(pszPeriod, _T(""));
				wVerMajorTmp = (WORD)_ttol(szKeyName);
				int nLen = _tcslen(szKeyName);
				LPTSTR pszMinor = szKeyName + nLen + sizeof(TCHAR);
				wVerMinorTmp = (WORD)_ttol(pszMinor);

				if(wVerMajorTmp > wVerMajor)
				{
					wVerMajor = wVerMajorTmp;
					wVerMinor = wVerMinorTmp;
				}
				else if(wVerMinorTmp > wVerMinor)
					wVerMinor = wVerMinorTmp;
			}
		}

		if(bWalkChildren)
		{
			try
			{
				// Walk it's children
				LPTSTR pszLoc = new TCHAR[_tcslen(pszStartAt) + _tcslen(szKeyName) + (20 * sizeof(TCHAR))];
				_tcscpy(pszLoc, pszStartAt);
				_tcscat(pszLoc, _T("\\"));
				_tcscat(pszLoc, szKeyName);
				DWORD dwRet = FindVersion(hStartAt, pszLoc);
				if(dwRet > 0)
				{
					wVerMajorTmp = HIWORD(dwRet);
					wVerMinorTmp = LOWORD(dwRet);
					if(wVerMajorTmp > wVerMajor)
					{
						wVerMajor = wVerMajorTmp;
						wVerMinor = wVerMinorTmp;
					}
					else if(wVerMinorTmp > wVerMinor)
						wVerMinor = wVerMinorTmp;
				}

				delete[] pszLoc;
			}
			catch (std::bad_alloc &)
			{
				return 0;
			}
		}
		dwSize = 1023;
		memset(szKeyName, 0, dwSize + 1);
	}

	return MAKELONG(wVerMinor, wVerMajor);
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::LaunchSymApp:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 4/23/99 - 10:23:13 AM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::LaunchSymApp
// Description	    : 
// Return type		: HINSTANCE 
// Arguments
//     LPCTSTR lpszAppName: 
//     LPCTSTR lpszFileName: 
// 
/////////////////////////////////////////////////////////////////////////

HINSTANCE CUtils::LaunchSymApp(LPCTSTR lpszAppName, LPCTSTR lpszFileName)
{
	HINSTANCE hRet = NULL;
	TCHAR szAppPath[MAX_PATH * 2];
	if(!GetSymAppPath(lpszAppName, szAppPath))
		return FALSE;

	TCHAR szFile[MAX_PATH];
	wsprintf(szFile, "%s\\%s", szAppPath, lpszFileName);
	BOOL bExists = (_taccess(szFile, 00) == 0);
	if(bExists)
	{
        SetCurrentDirectory(szAppPath);
        //hRet = (HINSTANCE)WinExec(szFile, SW_NORMAL);
        hRet = ShellExecute(NULL, NULL, szFile, NULL, szAppPath, SW_SHOWNORMAL);
	}
	
	return hRet;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : IsSpeedStartEnabled:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 4:43:03 PM (Shaun Cooley) /////////////////////
// 
// Function name	: IsSpeedStartEnabled
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsSpeedStartEnabled()
{
	dprintf("-----------------------------------------------------------------------------");
	dprintf("IsSpeedStartEnabled: checking...");

	if(!IsSymAppInstalled(_szSpeedStartProdName, _szSpeedStartExe) || !IsOSGoldOrOSR2())
	{
		dprintf("IsSpeedStartEnabled: System is not < OSR2 or SpeedStart is not installed, returning FALSE");
		dprintf("-----------------------------------------------------------------------------");
		return FALSE;
	}

	TCHAR szVxDPath[MAX_PATH * 2];
	if(GetSymAppPath(_szSpeedStartProdName, szVxDPath))
	{
		wsprintf(szVxDPath, "%s\\%s", szVxDPath, _szSpeedStartVxD);
		TCHAR szVxD[MAX_PATH + sizeof(TCHAR)];
		GetShortPathName(szVxDPath, szVxD, MAX_PATH);
		if(GetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartVXDRegLoc, szVxD, _tcslen(szVxD))
			&& GetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartRegLoc, 1))
		{
			dprintf("IsSpeedStartEnabled: SpeedStart is enabled, returning TRUE");
			dprintf("-----------------------------------------------------------------------------");
			return TRUE;
		}
	}

	dprintf("IsSpeedStartEnabled: SpeedStart is NOT enabled, returning FALSE");
	dprintf("-----------------------------------------------------------------------------");
	return FALSE;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : IsSpeedStartVisible:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 4:43:05 PM (Shaun Cooley) /////////////////////
// 
// Function name	: IsSpeedStartVisible
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsSpeedStartVisible()
{
	if(!IsSymAppInstalled(_szSpeedStartProdName, _szSpeedStartExe) || !IsOSGoldOrOSR2())
	{
		dprintf("IsSpeedStartVisible: SpeedStart is not installed or OS is > OSR2, returning FALSE");
		return FALSE;
	}

	if(FindWindow("Bullwinkle", NULL) != NULL)
	{
		dprintf("IsSpeedStartVisible: SpeedStart is visible, returning TRUE");
		return TRUE;
	}

	TCHAR szExePath[MAX_PATH * 2];
	if(GetSymAppPath(_szSpeedStartProdName, szExePath))
	{
		wsprintf(szExePath, "%s\\%s /Autostart", szExePath, _szSpeedStartExe);
		if(GetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartRunRegLoc, szExePath, _tcslen(szExePath)))
		{
			dprintf("IsSpeedStartVisible: SpeedStart will be visible at next boot, returning TRUE");
			return TRUE;
		}
	}

	dprintf("IsSpeedStartVisible: SpeedStart is not visible, returning FALSE");
	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : EnableSpeedStart:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 3/31/99 - 4:43:07 PM (Shaun Cooley) /////////////////////
// 
// Function name	: EnableSpeedStart
// Description	    : 
// Return type		: BOOL 
// Arguments
//     BOOL bEnable : 
//     BOOL bVisible: 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::EnableSpeedStart(BOOL bEnable, BOOL bVisible)
{
	if(!IsSymAppInstalled(_szSpeedStartProdName, _szSpeedStartExe) || !IsOSGoldOrOSR2())
	{
		dprintf(_T("EnableSpeedStart: OS is > OSR2 or speedstart is not installed, returning false"));
		return FALSE;
	}

	if(bEnable == FALSE)
		bVisible = FALSE;

	dprintf(_T("EnableSpeedStart: %sabling speedstart and making it%s visible"), bEnable ? _T("En") : _T("Dis"), bVisible ? _T("") : _T(" not"));

	// Go to work...
	if(bVisible == FALSE)
	{
		dprintf(_T("EnableSpeedStart: closing Bullwinkle..."));

		HWND hWnd = FindWindow("Bullwinkle", NULL);

		if(hWnd != NULL)
		{
			dprintf(_T("EnableSpeedStart: found Bullwinkle window, sending close message..."));
			SendMessage(hWnd, WM_COMMAND, 40004, 0);
		}
		else
			dprintf(_T("EnableSpeedStart: could not find Bullwinkle window..."));

		dprintf(_T("EnalbeSpeedStart: Removing Bullwinkle run reg key"));
		SetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartRunRegLoc, NULL, 0);
	}
	else if(bVisible == TRUE && IsSpeedStartVisible() == FALSE)
	{
		dprintf(_T("EnableSpeedStart: starting bullwinkle..."));
		TCHAR szPath[MAX_PATH * 2];
		if(!GetSymAppPath(_szSpeedStartProdName, szPath))
		{
			dprintf(_T("EnableSpeedStart: could not find the bullwinkle exe, returning false"));
			return FALSE;
		}

		wsprintf(szPath, "%s\\%s", szPath, _szSpeedStartExe);
		WinExec(szPath, SW_SHOWNORMAL);
		dprintf(_T("EnableSpeedStart: Started bullwinkle..."));

		// Add exename "/autostart" to run reg key
		dprintf(_T("EnalbeSpeedStart: Adding Bullwinkle run reg key"));
		TCHAR szExePath[MAX_PATH * 2];
		if(!GetSymAppPath(_szSpeedStartProdName, szExePath))
		{
			dprintf(_T("EnableSpeedStart: SpeedStart exe not found, returning FALSE"));
			return FALSE;
		}
		wsprintf(szExePath, "%s\\%s /Autostart", szExePath, _szSpeedStartExe);
		if(bVisible)
			SetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartRunRegLoc, szExePath, _tcslen(szExePath));
	}

	if(bEnable == TRUE)
	{
		dprintf(_T("EnableSpeedStart: Adding reg keys to start SpeedStart at next boot"));
		TCHAR szVxDPath[MAX_PATH * 2];
		if(!GetSymAppPath(_szSpeedStartProdName, szVxDPath))
		{
			dprintf(_T("EnableSpeedStart: SpeedStart VxD not found, returning FALSE"));
			return FALSE;
		}

		wsprintf(szVxDPath, "%s\\%s", szVxDPath, _szSpeedStartVxD);
		TCHAR szVxD[MAX_PATH + sizeof(TCHAR)];
		GetShortPathName(szVxDPath, szVxD, MAX_PATH);
		SetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartVXDRegLoc, szVxD, _tcslen(szVxD));
		SetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartRegLoc, 1);
	}
	else
	{
		dprintf(_T("EnableSpeedStart: Removing reg keys so SpeedStart doesn't start at next boot"));
		SetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartVXDRegLoc, NULL, 0);
		SetRegistryKey(HKEY_LOCAL_MACHINE, _szSpeedStartRegLoc, 0);
	}

	return TRUE;
}

/*
#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsNAVAPEnabled:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/17/99 - 10:18:11 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsNAVAPEnabled
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsNAVAPEnabled()
{
	TCHAR szLongNavPath[MAX_PATH * 2];
	if(GetSymAppPath(_T("NAV"), szLongNavPath))
	{
		TCHAR szNavPath[MAX_LOADSTRING];
		GetShortPathName(szLongNavPath, szNavPath, MAX_LOADSTRING);

		///////////////////////////////////////
		// Check the registry key's for NAV ap

		// Loading of VxD
		TCHAR szVxD[MAX_LOADSTRING * 2];
		_tcscpy(szVxD, szNavPath);
		_tcscat(szVxD, _szNavVxDName);
		if(GetRegistryKey(HKEY_LOCAL_MACHINE, _szNavVxDRegLoc, szVxD, _tcslen(szVxD)))
			return TRUE;
	}

	return FALSE;
}

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::EnableNAVAP:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/17/99 - 4:49:17 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::EnableNAVAP
// Description	    : 
// Return type		: BOOL 
// Arguments
//     BOOL bEnable : 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::EnableNAVAP(BOOL bEnable)
{
	TCHAR szLongNavPath[MAX_PATH * 2];
	if(GetSymAppPath(_T("NAV"), szLongNavPath))
	{
		TCHAR szNavPath[MAX_LOADSTRING];
		GetShortPathName(szLongNavPath, szNavPath, MAX_LOADSTRING);

		TCHAR szDatFile[MAX_LOADSTRING];
		_tcscpy(szDatFile, szNavPath);
		_tcscat(szDatFile, _T("\\NAVSTART.DAT"));
		int DatFile = _tsopen(szDatFile, _O_RDWR, _SH_DENYWR);
		if(DatFile)
		{
			// Add/Delete the registry key's for NAV ap
			if(bEnable)
			{
				// Loading of VxD
				TCHAR szVxD[MAX_LOADSTRING * 2];
				_tcscpy(szVxD, szNavPath);
				_tcscat(szVxD, _szNavVxDName);
				SetRegistryKey(HKEY_LOCAL_MACHINE, _szNavVxDRegLoc, szVxD, _tcslen(szVxD));

				// Loading of AP
				TCHAR szAP[MAX_LOADSTRING * 2];
				_tcscpy(szAP, szNavPath);
				_tcscat(szAP, _szNavAPName);
				SetRegistryKey(HKEY_LOCAL_MACHINE, _szNavAPRegLoc, szAP, _tcslen(szAP));
			}
			else
			{
				// no more loading of VxD
				SetRegistryKey(HKEY_LOCAL_MACHINE, _szNavVxDRegLoc, NULL, 0);
				LPTSTR pszVxDKey = _tcsdup(_szNavVxDRegLoc);
				LPTSTR pszNullMe = _tcsrchr(pszVxDKey, '\\');
				pszNullMe[0] = NULL;
				RegDeleteKey(HKEY_LOCAL_MACHINE, pszVxDKey);
				free(pszVxDKey);

				// no more loading of AP
				SetRegistryKey(HKEY_LOCAL_MACHINE, _szNavAPRegLoc, NULL, 0);
			
			}

			// seek past the header and up to STARTOPT.bLoadVxD
			_lseek(DatFile, sizeof(NAVSTARTHEADER) + (3 * sizeof(BYTE)), SEEK_SET);
			
			// Write out the current VxD startup value
			_write(DatFile, &bEnable, sizeof(BYTE));

			// Close the file
			_close(DatFile);

			return TRUE;
		}
	}
	return FALSE;
}
*/

#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsCrashGuardEnabled:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/25/99 - 9:57:09 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsCrashGuardEnabled
// Description	    : 
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsCrashGuardEnabled()
{
	if(IsSymAppInstalled(_szNCGName, _szCGExe))
	{
		TCHAR szLoc[MAX_PATH * 2];
		TCHAR szBegLoc[MAX_PATH * 2];
		GetSymAppPath(_szNCGName, szBegLoc);

		_tcscpy(szLoc, _T("\""));
		_tcscat(szLoc, szBegLoc);
		_tcscat(szLoc, _T("\\"));
		_tcscat(szLoc, _szCGExe);
		_tcscat(szLoc, _T("\""));
		if(GetRegistryKey(HKEY_LOCAL_MACHINE, _szCGRunLoc, szLoc, _tcslen(szLoc)))
			return TRUE;

	}
	return FALSE;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::EnabledCrashGuard:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 5/25/99 - 9:57:06 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::EnabledCrashGuard
// Description	    : 
// Return type		: BOOL 
// Arguments
//     BOOL bEnable : 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::EnabledCrashGuard(BOOL bEnable)
{
	if(IsSymAppInstalled(_szNCGName, _szCGExe))
	{
		int nCount = sizeof(_szCrashGuardSettings) / sizeof(_szCrashGuardSettings[0]);
		if(bEnable)
		{
			for(int i = 0; i < nCount; ++i)
				SetRegistryKey(HKEY_LOCAL_MACHINE, _szCrashGuardSettings[i], _dwCrashGuardOn);

			TCHAR szOnVal[MAX_PATH * 2];
			TCHAR szBegLoc[MAX_PATH * 2];
			GetSymAppPath(_szNCGName, szBegLoc);

			_tcscpy(szOnVal, _T("\""));
			_tcscat(szOnVal, szBegLoc);
			_tcscat(szOnVal, _T("\\"));
			_tcscat(szOnVal, _szCGExe);
			_tcscat(szOnVal, _T("\""));
			if(SetRegistryKey(HKEY_LOCAL_MACHINE, _szCGRunLoc, szOnVal, _tcslen(szOnVal)))
				return TRUE;
		}
		else
		{
			// Kill CrashGuard
			HWND hWnd = FindWindow(_szCGClassName, NULL);
			if(hWnd)
				SendMessage(hWnd, WM_CLOSE, 0, 0);

			for(int i = 0; i < nCount; ++i)
				SetRegistryKey(HKEY_LOCAL_MACHINE, _szCrashGuardSettings[i], _dwCrashGuardOff);

			if(SetRegistryKey(HKEY_LOCAL_MACHINE, _szCGRunLoc, NULL, 0))
				return TRUE;
		}

	}
	return FALSE;
}



#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::IsNPRBEnabled:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 6/14/99 - 1:57:52 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::IsNPRBEnabled
// Description	    : Check if Symantec Protected Recycle Bin is enabled
// Return type		: BOOL 
// 
/////////////////////////////////////////////////////////////////////////

BOOL CUtils::IsNPRBEnabled()
{
	BOOL bRet = TRUE;
	TCHAR szNUPath[MAX_PATH * 2];
	GetSymAppPath(_szNUName, szNUPath);

	if(IsSymAppInstalled(_szNUName, _szNPRBExe))
	{
		TCHAR szNPRBExeLoc[MAX_PATH * 2];
		int nCount = sizeof(_szNPRBLoc) / sizeof(_szNPRBLoc[0]);
		wsprintf(szNPRBExeLoc, "%s\\%s", szNUPath, _szNPRBExe);

		// Check the run keys
		for(int i = 0; i < nCount; ++i)
		{
			if(!GetRegistryKey(HKEY_LOCAL_MACHINE, _szNPRBLoc[i], szNPRBExeLoc, _tcslen(szNPRBExeLoc)))
			{
				bRet = FALSE;
				break;
			}	
		}

		// Check the Switches\enabled key by enumerating sub keys of _szNPRBSettings
		HKEY hkNPRB = NULL, hkEnum = NULL;
		TCHAR szKeyName[MAX_PATH + 1];
		TCHAR szSwitchKey[MAX_PATH * 2];
		DWORD dwLen = MAX_PATH;
		LONG lRes = 0, lSubRes = 0;
		FILETIME ftLastWrite;
		int nLoc = 0;

		lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _szNPRBSettings, NULL, KEY_READ, &hkNPRB);
		if(lRes == ERROR_SUCCESS && hkNPRB)
		{
			lRes = RegEnumKeyEx(hkNPRB, nLoc, szKeyName, &dwLen, NULL, NULL, NULL, &ftLastWrite);
			while(lRes == ERROR_SUCCESS && bRet)
			{
				wsprintf(szSwitchKey, _T("%s\\%s"), szKeyName, _szNPRBSwitches);
				lSubRes = RegOpenKeyEx(hkNPRB, szSwitchKey, NULL, KEY_READ, &hkEnum);
				if(lSubRes == ERROR_SUCCESS && hkEnum)
				{
					DWORD dwType = REG_DWORD;
					DWORD dwValue = 0;
					DWORD dwSize = sizeof(DWORD);
					TCHAR szName[MAX_PATH];
					_tcscpy(szName, _T("Enabled"));
					if(ERROR_SUCCESS == RegQueryValueEx(hkEnum, szName, NULL, &dwType, (BYTE*)&dwValue, &dwSize))
					{
						if(dwValue == 0)
						{
							bRet = FALSE;
							break;
						}
					}
				}

				// Get the next subkey
				dwLen = MAX_PATH;
				lRes = RegEnumKeyEx(hkNPRB, ++nLoc, szKeyName, &dwLen, NULL, NULL, NULL, &ftLastWrite);
			}
		}
	}
	else
		bRet = FALSE;

	return bRet;
}


#undef FUNCNAME
#undef DBGOUT_FUNCNAME
#define FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : CUtils::EnableNPRB:  "
#define DBGOUT_FUNCNAME DBG_STR(TEXT(FUNCNAME))
//////////////// 6/14/99 - 2:26:10 PM (Shaun Cooley) /////////////////////
// 
// Function name	: CUtils::EnableNPRB
// Description	    : 
// Return type		: void 
// Arguments
//     BOOL bEnable : 
// 
/////////////////////////////////////////////////////////////////////////

void CUtils::EnableNPRB(BOOL bEnable)
{
	TCHAR szNUPath[MAX_PATH * 2];
	GetSymAppPath(_szNUName, szNUPath);

	if(IsSymAppInstalled(_szNUName, _szNPRBExe))
	{
		TCHAR szNPRBExeLoc[MAX_PATH * 2];
		int nCount = sizeof(_szNPRBLoc) / sizeof(_szNPRBLoc[0]);
		wsprintf(szNPRBExeLoc, "%s\\%s", szNUPath, _szNPRBExe);

		// Set the run keys
		for(int i = 0; i < nCount; ++i)
		{
			if(bEnable)
				SetRegistryKey(HKEY_LOCAL_MACHINE, _szNPRBLoc[i], szNPRBExeLoc, _tcslen(szNPRBExeLoc));
			else
				SetRegistryKey(HKEY_LOCAL_MACHINE, _szNPRBLoc[i], NULL, 0);
		}

		// Check the Switches\enabled key by enumerating sub keys of _szNPRBSettings
		HKEY hkNPRB = NULL, hkEnum = NULL;
		TCHAR szKeyName[MAX_PATH + 1];
		TCHAR szSwitchKey[MAX_PATH * 2];
		DWORD dwLen = MAX_PATH;
		LONG lRes = 0, lSubRes = 0;
		FILETIME ftLastWrite;
		int nLoc = 0;

		lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _szNPRBSettings, NULL, KEY_READ | KEY_WRITE, &hkNPRB);
		if(lRes == ERROR_SUCCESS && hkNPRB)
		{
			lRes = RegEnumKeyEx(hkNPRB, nLoc, szKeyName, &dwLen, NULL, NULL, NULL, &ftLastWrite);
			while(lRes == ERROR_SUCCESS)
			{
				wsprintf(szSwitchKey, _T("%s\\%s"), szKeyName, _szNPRBSwitches);
				lSubRes = RegOpenKeyEx(hkNPRB, szSwitchKey, NULL, KEY_READ | KEY_WRITE, &hkEnum);
				if(lSubRes == ERROR_SUCCESS && hkEnum)
				{
					DWORD dwValue = (bEnable ? 1 : 0);
					TCHAR szName[MAX_PATH];
					_tcscpy(szName, _T("Enabled"));
					RegSetValueEx(hkEnum, szName, NULL, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
				}

				// Get the next subkey
				dwLen = MAX_PATH;
				lRes = RegEnumKeyEx(hkNPRB, ++nLoc, szKeyName, &dwLen, NULL, NULL, NULL, &ftLastWrite);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// $Log:     $
//--------- 3/30/99 - 3:33:36 PM (Shaun Cooley) -----------
// Created
//--------- 3/31/99 - 5:01:13 PM (Shaun Cooley) -----------
// Added SpeedStart Functions
// 
/////////////////////////////////////////////////////////////////////////////
