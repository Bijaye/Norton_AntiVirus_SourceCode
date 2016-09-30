/////////////////////////////////////////////////////////////////////////////////////////////
// helper.h: function prototypes for all non-exported helper functions			
//
/////////////////////////////////////////////////////////////////////////////////////////////


#ifndef HELPER_H
#define HELPER_H

#include "stdafx.h"
#include "platform.h"

#define NAVDX_EXE						  _T("navdx.exe")
#define NAV2000		6
#define NAV2001		7

const TCHAR szSymAppsPath[] = _T("SOFTWARE\\Symantec\\InstalledApps");
const TCHAR szNAVAppsKey[] = _T("Software\\Symantec\\Norton AntiVirus");
const TCHAR szNAVAppsKeyNT[] = _T("SOFTWARE\\Symantec\\Norton AntiVirus NT");

BOOL LaunchAndWait(LPSTR lpExe, LPSTR lpParams);
BOOL RemoveLine(LPCTSTR filename, LPCTSTR string);
LONG RegDeleteKeyEx(HKEY hRootKey, LPCTSTR szSubKey);
BOOL DeleteFilesInDirectory(LPTSTR lpDirectory);
BOOL FileInUseHandler(LPTSTR lpName);
BOOL CheckPreviousVersion(LPTSTR lpszProductName);


#endif

