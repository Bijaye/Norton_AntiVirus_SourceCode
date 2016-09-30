////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// helper.h: function prototypes for all non-exported helper functions			
//
/////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
// REVISION HISTORY
//
//	Revision 1:		03/02/2001		COLLIN DAVIS
//		Initial Draft.  Added DelNMain() Function.
//
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HELPER_H
#define HELPER_H

#include "stdafx.h"
#include <vector>

//
// these are files that we have have problems with either getting left behind or being 
// marked for deletion after an uninstall.  we use this list on uninstall to make sure
// they all get removed and on install to make sure none are marked for deletion.
//
static TCHAR* g_szNavFiles[] =
{
    _T("runsched.exe"),
    _T("defloc.dat"),
	_T("msievent.log"),
	_T("advchk.iss"),
	_T("setup.log"),
	_T("MemScan.exe"),
	_T("*.gid"),
    _T("resqloc.dat"),
	_T("AVApp.log"),
	_T("AvError.log"),
	_T("AVVirus.log"),
	_T("SYMREDIR.DLL"), 
	_T("NAVSHEXT.DLL"), 
	_T("NAVAPW32.EXE"), 
	_T("APWUTIL.DLL"),
	_T("APWCMD9X.DLL"),
	_T("APWCMDNT.DLL"),
	_T("NAVPROXY.DLL"),
	_T("CCEVTMGR.EXE"),
	_T("NAVEVENT.DLL"),
	_T("CCEVT.DLL"),
	_T("SYMSTORE.DLL"),
	_T("NAVLCOM.DLL"),
    NULL
};

#define REGVAL_PROGRAMFILESDIR			  _T("ProgramFilesDir")
#define REGVAL_COMMONFILESDIR             _T("CommonFilesDir")
#define DEFAULT_PROGRAMFILES              _T("Program Files")
#define DEFAULT_COMMONFILES               _T("Common Files")
#define LUCONST_LUCOMSERVER_MUTEX_NAME		(_T("Symantec.LuComServer.Running"))

const TCHAR szSymAppsPath[] = _T("SOFTWARE\\Symantec\\InstalledApps"); 
const TCHAR szTempRegKeys[] = _T("SOFTWARE\\Symantec\\SymSetup\\TempRegKeys"); 
const TCHAR szNAVAppsKey[] = _T("Software\\Symantec\\Norton AntiVirus");
const TCHAR szNAVAppsKeyNT[] = _T("SOFTWARE\\Symantec\\Norton AntiVirus NT");
const TCHAR szNAVLinkKey[] = _T("Software\\Symantec\\Norton AntiVirus\\links");
const TCHAR szMSRun[] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
const TCHAR szSharedDLLs[] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs");
const TCHAR szTaskMonKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\TaskMon");
const TCHAR g_szGlobalMutexName[] = _T("Global\\NAVInstallMutex");
const TCHAR g_szLocalMutexName[] = _T("NAVInstallMutex");
const TCHAR szNAVApp[] = _T("NAVW32");
const TCHAR cstrNavLSServiceID[] = _T("61D460E0-07C0-11d3-A985-00A0244D507A");

//Helper functions:
BOOL RegGetProgramFilesDir(LPTSTR pszPath, UINT cbSize);
BOOL RegGetCommonFilesDir(LPTSTR pszPath, UINT cbSize);
BOOL DirectoryExists(LPTSTR szDir);
LONG SWLaunchSetupAndWait(LPCTSTR pszSetup, LPCTSTR pszIssFile, LPCTSTR pszClass, LPCTSTR pszTitle, HINSTANCE hInstance, MSIHANDLE hInstall, LPCTSTR pszDisplayText);
BOOL RemoveLine(LPCTSTR filename, LPCTSTR string);
BOOL DeleteFilesInDirectory(LPCTSTR lpDirectory);
int RemoveTargetFiles(TCHAR* pszTargetDir, TCHAR** filenameArray);
BOOL ValidFileTarget(DWORD fileAttributes);
//BOOL ShutdownWndByClass( LPTSTR lpWndClassName, UINT uMsg ); MOVED TO SYMSETUP
int StrReplace(LPTSTR* ppszString, LPCTSTR lpszOld, LPCTSTR lpszNew, BOOL bCaseSensitive);
BOOL SetServiceManual( LPCTSTR lpszServiceName);
BOOL QuoteServiceBinaryPath(LPCTSTR lpszService);

#endif

