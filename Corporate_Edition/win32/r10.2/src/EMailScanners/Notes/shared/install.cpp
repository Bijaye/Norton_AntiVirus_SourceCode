// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "install.h"
#include "SymSaferRegistry.h"

/*===========================================================================*/
// Install functions
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

// IsNotesClientXInstalled may be used by the install program
// to verify whether or not the notes client version X or server
// is installed on a particular computer.
//
//	Returns:
//		S_CLIENT if the notes client is installed.
//		S_SERVER if the notes server is installed.
//		These values may both be bitwise OR'ed
//		together if the computer is a server.
//		Note that 0 is returned if there was an error
//		or if Notes is not installed.

DWORD IsNotesClient4Installed()
{
	DWORD	result = 0;
	TCHAR	dest[128];
    int     nKitType;

	if(GetPrivateProfileString(
			NOTESSECTION,			// points to section name
			CLIENTSETUP,			// points to key name
			_T(""),					// points to default string
			dest,					// points to destination buffer
			128,					// size of destination buffer
			_T("notes.ini")))		// points to initialization filename
    {
		result |= S_CLIENT;
    }

	if(GetPrivateProfileString(
			NOTESSECTION,			// points to section name
			SERVERSETUP,			// points to key name
			_T(""),					// points to default string
			dest,					// points to destination buffer
			128,					// size of destination buffer
			_T("notes.ini")))		// points to initialization filename
    {
		result |= S_SERVER;
    }

    // After Notes is installed but before the first time it is run, the keys checked
    // above will not be present. Before the one-time initialization is done, notes.ini
    // will typically have a [Notes] section with three entries:
    //  KitType= 1,2        # 1 = workstation only, 2 = server, or server+workstation install
    //  Directory= <path>   # notes data directory
    //  InstallType= 0-9    # Client or server type (new in v5.x?)
    //
    // Try reading client/server from the KitType key.

    nKitType = GetPrivateProfileInt( NOTESSECTION, KITTYPE, 0, _T("notes.ini") );

    if ( KITTYPE_WORKSTATION == nKitType )
    {
        result |= S_CLIENT;
    }
    else if ( KITTYPE_SERVER == nKitType )
    {
        result |= S_SERVER;
    }

	return result;
}

DWORD IsNotesClient5Installed(LPTSTR lpsNotesPath)
{
	DWORD	result = 0;
	TCHAR	iniDir[MAX_PATH];
	TCHAR	dest[128];
    int     nKitType;

	_tcscpy(iniDir, lpsNotesPath);
	_tcscat(iniDir, _T("\\notes.ini"));
	
	if(GetPrivateProfileString(
			NOTESSECTION,			// points to section name
			CLIENTSETUP,			// points to key name
			_T(""),					// points to default string
			dest,					// points to destination buffer
			128,					// size of destination buffer
			iniDir))				// points to initialization filename
    {	
		result |= S_CLIENT;
    }

	if(GetPrivateProfileString(
			NOTESSECTION,			// points to section name
			SERVERSETUP,			// points to key name
			_T(""),					// points to default string
			dest,					// points to destination buffer
			128,					// size of destination buffer
			iniDir))				// points to initialization filename
    {
		result |= S_SERVER;
    }

    // After Notes is installed but before the first time it is run, the keys checked
    // above will not be present. Before the one-time initialization is done, notes.ini
    // will typically have a [Notes] section with three entries:
    //  KitType= 1,2        # 1 = workstation only, 2 = server, or server+workstation install
    //  Directory= <path>   # notes data directory
    //  InstallType= 0-9    # Client or server type (new in v5.x?)
    //
    // Try reading client/server from the KitType key.

    nKitType = GetPrivateProfileInt( NOTESSECTION, KITTYPE, 0, iniDir );

    if ( KITTYPE_WORKSTATION == nKitType )
    {
        result |= S_CLIENT;
    }
    else if ( KITTYPE_SERVER == nKitType )
    {
        result |= S_SERVER;
    }

	return result;
}

/*---------------------------------------------------------------------------*/

// GetNotesInstallPath returns the Notes path as specified in the registry.
//
//	Returns:
//		NOTES_NOT_INSTALLED if the path is not found.
//		NOTES4_INSTALLED if the path is for Notes v4.
//		NOTES5_INSTALLED if the path is for Notes v5.
//

DWORD GetNotesInstallPath(LPTSTR installPath, DWORD maxLen)
{
	HKEY hKey = NULL;

	// try all versions of Notes

	RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,					// main key
		_T("SOFTWARE\\LOTUS\\NOTES"),		// address of name of subkey to open
		0,									// reserved
		KEY_READ,							// security access
		&hKey);								// returned handle
	
	if(hKey)
		{
		if(SymSaferRegQueryValueEx(hKey, _T("Path"), 0,	NULL, (BYTE *)installPath,	&maxLen) == ERROR_SUCCESS)
			{
			// Strip off any trailing '\' from the path
			if(installPath[_tcslen(installPath) - 1] == _T('\\'))
				installPath[_tcslen(installPath) - 1] = 0;
			RegCloseKey(hKey);
			// this is ok, since the rest of the hook for Notes 5+ is the same...
			return NOTES5_INSTALLED;
			}
		// if we can't get the path, then let's try v6...
		RegCloseKey(hKey);
		}

	hKey = NULL;

	// try Notes v6

	RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,					// main key
		_T("SOFTWARE\\LOTUS\\NOTES\\6.0"),	// address of name of subkey to open
		0,									// reserved
		KEY_READ,							// security access
		&hKey);								// returned handle
	
	if(hKey)
		{
		if(SymSaferRegQueryValueEx(hKey, _T("Path"), 0,	NULL, (BYTE *)installPath,	&maxLen) == ERROR_SUCCESS)
			{
			// Strip off any trailing '\' from the path
			if(installPath[_tcslen(installPath) - 1] == _T('\\'))
				installPath[_tcslen(installPath) - 1] = 0;
			RegCloseKey(hKey);
			// this is ok, since the rest of the hook for Notes 5 and 6 is the same...
			return NOTES5_INSTALLED;
			}
		// if we can't get the path, then let's try v5...
		RegCloseKey(hKey);
		}

	hKey = NULL;

	// try Notes v5

	RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,					// main key
		_T("SOFTWARE\\LOTUS\\NOTES\\5.0"),	// address of name of subkey to open
		0,									// reserved
		KEY_READ,							// security access
		&hKey);								// returned handle
	
	if(hKey)
		{
		if(SymSaferRegQueryValueEx(hKey, _T("Path"), 0,	NULL, (BYTE *)installPath,	&maxLen) == ERROR_SUCCESS)
			{
			// Strip off any trailing '\' from the path
			if(installPath[_tcslen(installPath) - 1] == _T('\\'))
				installPath[_tcslen(installPath) - 1] = 0;
			RegCloseKey(hKey);
			return NOTES5_INSTALLED;
			}
		// if we can't get the path, then let's try v4...
		RegCloseKey(hKey);
		}

	// try Notes v4

	hKey = NULL;

	RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,					// main key
		_T("SOFTWARE\\LOTUS\\NOTES\\4.0"),	// address of name of subkey to open
		0,									// reserved
		KEY_READ,							// security access
		&hKey);								// returned handle
	
	if(hKey)
		{
		if(SymSaferRegQueryValueEx(hKey, _T("Path"), 0,	NULL, (BYTE *)installPath,	&maxLen) == ERROR_SUCCESS)
			{
			// Strip off any trailing '\' from the path
			if(installPath[_tcslen(installPath) - 1] == _T('\\'))
				installPath[_tcslen(installPath) - 1] = 0;
			RegCloseKey(hKey);
			return NOTES4_INSTALLED;
			}
		RegCloseKey(hKey);
		}


	return NOTES_NOT_INSTALLED;
}

/*--- end of source ---*/
