// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "nlnvp.h"
#include "install.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "vpstrutils.h"
#include "resource.h"

// Global data

BOOL g_bMailContextRegistered = FALSE;

/*===========================================================================*/
// Miscellaneous functions
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

void MakeTempFileName(
		LPTSTR	pszPath, 
		LPTSTR	pszPrefix,
		DWORD	index,
		LPTSTR	pszExt,
		LPTSTR	pszFilePath,
        DWORD   dwFilePathSize)
{
	sssnprintf(pszFilePath, dwFilePathSize, "%s\\%s%.8x%s", pszPath, pszPrefix, index, pszExt);
}


/*---------------------------------------------------------------------------*/

void TranslateToLMBCS(char *source, char *dest, WORD maxDest)
{
	OSTranslate(OS_TRANSLATE_NATIVE_TO_LMBCS,				// NOTESAPI
		source,
		(WORD)_tcslen(source),
		dest,
		maxDest);
}

#define CSIDL_COMMON_APPDATA            0x0023      // All Users\Application Data
#define ERROR_GENERAL					0x20000000
typedef HRESULT (WINAPI *PFNSHGetFoldPathA)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);

DWORD GetAppDataDirectory(LPTSTR lpDataDir)
{
    DWORD   dwError = ERROR_SUCCESS;

    PFNSHGetFoldPathA pfnSHGetFolderPathA = NULL;

    TCHAR          szSystemDir[MAX_PATH] = {0};
    TCHAR          szSHFolderDLL[MAX_PATH] ={0};
    HINSTANCE      hFolderDLL = NULL;

    // Get the system directory
    if (GetSystemDirectory(szSystemDir, sizeof(szSystemDir)))
    {
        // Append the DLL name
        vpsnprintf (szSHFolderDLL, sizeof (szSHFolderDLL), _T("%s\\shfolder.dll"), szSystemDir);

        // Load it.
        hFolderDLL = LoadLibrary( szSHFolderDLL );

        if ( hFolderDLL )
        {
            // Get the function
            pfnSHGetFolderPathA = (PFNSHGetFoldPathA)GetProcAddress( hFolderDLL, _T("SHGetFolderPathA") );

            if ( pfnSHGetFolderPathA )
            {
                // Per machine, all users, non-roaming, set up at install
                HRESULT hr = pfnSHGetFolderPathA( NULL, CSIDL_COMMON_APPDATA, NULL, 0, lpDataDir);

                if ( FAILED(hr) )
                    dwError = ERROR_GENERAL;
            }
			else
                dwError = ERROR_GENERAL;

            FreeLibrary( hFolderDLL );
        }
        else
            dwError = GetLastError();
    }
    else
        dwError = GetLastError();

    return dwError;
}

/*===========================================================================*/
// Install functions
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

// AppendNotesHookString appends value to a comma
// separated list specified by key in the notes.ini file.
//	

void AppendNotesHookString()
{
	// note that unicode routines are not used
	// since the Notes API does not support unicode

	char	valueBuffer[MAX_PATH];
	char	keyName[] = "NSF_HOOKS";
	char	hookName[] = "NLNVP";

	valueBuffer[0] = 0;

	NotesInitExtended(0, NULL);

	// Check to see if the key exists
	if(OSGetEnvironmentString(keyName, valueBuffer, MAX_PATH-1))
	{
		// If the value doesn't exist, add it
		if(!strstr(valueBuffer, hookName))
		{
			strcat(valueBuffer, ",");
			strcat(valueBuffer, hookName);

			OSSetEnvironmentVariable(keyName, valueBuffer);
		}
	}
	else	// Otherwise, add the new key and value
		OSSetEnvironmentVariable(keyName, hookName);
	
	NotesTerm();

	return;
}

/*---------------------------------------------------------------------------*/

DWORD IsNotesClientInstalledForAllUsers()
{
	TCHAR notesKey[MAX_PATH+1];
	TCHAR path[MAX_PATH+1];
	DWORD dwMultiUser = 0;
	HKEY hKey = NULL;

	DebugOut(SM_GENERAL, "NLNHOOK: Checking for multi-user...");

	// check for the multiuser setting

	RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,					// main key
		_T("SOFTWARE\\LOTUS\\NOTES"),		// address of name of subkey to open
		0,									// reserved
		KEY_READ,							// security access
		&hKey);								// returned handle
	
	if(hKey)
	{
		DWORD dwSize = sizeof(dwMultiUser);
		SymSaferRegQueryValueExA(hKey, "MultiUser", 0, NULL, (BYTE*) &dwMultiUser, &dwSize);
		RegCloseKey(hKey);
	}

	if (dwMultiUser == 0)
		return 0;

	DebugOut(SM_GENERAL, "NLNHOOK: Notes installed as multi-user");

	if (GetAppDataDirectory(path) != ERROR_SUCCESS )
		return 0;

	_tcscpy(notesKey, "\\lotus\\notes\\data");
	int numchars = _tcslen(notesKey);

	if (numchars > 0 && (_tcslen(path) + numchars < MAX_PATH))
		vpstrnappend(path, notesKey, MAX_PATH);
	else
		vpstrnappend(path, _T("\\lotus\\notes\\data"), MAX_PATH);

	DebugOut(SM_GENERAL, "NLNHOOK: Checking ini file at '%s'", path);

	return IsNotesClient5Installed(path);
}

/*---------------------------------------------------------------------------*/

//	NSE_Install performs the primary installation necessary for the 
//	notes storage extension to function.
//	This includes:
//		1.	Installation of the notes hook
//		2.	Creation of configuration keys necessary for baseline operation.
//		3.	Uninstallation of all of these features
//
//	Parameters:
//		flags	A DWORD indicating whether this is an install or uninstall.
//
//	Returns:
//		ERROR_SUCCESS for success

DWORD NSE_Install(DWORD install)
{
    TCHAR   notesPath[MAX_PATH];
	DWORD	installed;
	DWORD	version;
	
	g_debug = TRUE;	// assume we want debug output when installing

	if(install)
		{
		version = GetNotesInstallPath(notesPath, MAX_PATH);

		if (version == NOTES4_INSTALLED)
			// in this case the notes.ini file is in the system dir
			installed = IsNotesClient4Installed();
		else if (version == NOTES5_INSTALLED)
			// in this case the notes.ini file is in the Notes dir
			installed = IsNotesClient5Installed(notesPath);
		else
			return ERROR_PATH_NOT_FOUND;

		// also check to see if Notes was installed for all users...
		// in this case, the notes.ini file is in the all-users folder
		installed |= IsNotesClientInstalledForAllUsers();

		// don't install on the server
		if(installed & S_SERVER)
			return ERROR_FUNCTION_NOT_SUPPORTED;

		// if not installing on a client, return
		if(!(installed & S_CLIENT))
			return ERROR_FUNCTION_NOT_SUPPORTED;

		AppendNotesHookString();
		}

	return ERROR_SUCCESS;
}
/*--- end of source ---*/
