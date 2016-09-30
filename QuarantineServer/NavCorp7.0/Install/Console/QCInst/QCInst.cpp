#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "resource.h"

///////////////////////////////////////////////////////////////////////////////
// 
// Locals
// 
///////////////////////////////////////////////////////////////////////////////

HINSTANCE g_hInstance = NULL;

void CleanupSharedDefsKey(void);
void CleanupVirusDefs(void);
void CleanupRegistry(void);
BOOL DeleteFolderRecursive(LPCTSTR szFolder);

///////////////////////////////////////////////////////////////////////////////
//
// Function name: DllMain
//
// Description  : 
//
// Return type  : BOOL APIENTRY 
//
// Argument     : HANDLE hModule
// Argument     : DWORD  ul_reason_for_call
// Argument     : LPVOID lpReserved
//
///////////////////////////////////////////////////////////////////////////////
// 8/16/99 - CMCGOWEN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
    switch( ul_reason_for_call )
        {
        case DLL_PROCESS_ATTACH:
            g_hInstance = (HINSTANCE) hModule;
            break;

        }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: UninstInitialize
//
// Description  : 
//
// Return type  : LONG 
//
// Argument     : HWND hWnd
// Argument     : HANDLE hInstance
// Argument     : LONG lReserved
//
///////////////////////////////////////////////////////////////////////////////
// 8/16/99 - CMCGOWEN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) UninstInitialize( HWND hWnd, HANDLE hInstance, LONG lReserved )
{
	int nRtnVal = 0;

	return nRtnVal;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: UninstUnInitialize
//
// Description  : 
//
// Return type  : LONG 
//
// Argument     : HWND hWnd
// Argument     : HANDLE hInstance
// Argument     : LONG lReserved
//
///////////////////////////////////////////////////////////////////////////////
// 8/16/99 - CMCGOWEN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) UninstUnInitialize( HWND hWnd, HANDLE hInstance, LONG lReserved )
{
	int nRtnVal = 0;

	CleanupVirusDefs();
	CleanupRegistry();

	return nRtnVal;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CleanupVirusDefs
//
// Description  : Cleans up NAV Virus Defs.  If we're the only ones using them,
//				  we will remove the files.
//
// Return type  : None
//
// Argument     : 
///////////////////////////////////////////////////////////////////////////////
// 8/16/99 - CMCGOWEN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CleanupVirusDefs(void)
{
	HKEY hRegKey = NULL;
	TCHAR szVirusDefsFolder[MAX_PATH] = {0};

	CleanupSharedDefsKey();

	// if we fail to open the SharedDefs key, then it is not there, and we need to remove
	// the virus definitions folder.
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\SharedDefs",
										 0L, KEY_ALL_ACCESS, &hRegKey))
	{
		hRegKey = NULL;
		
		if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\InstalledApps", 
										 0L, KEY_ALL_ACCESS, &hRegKey))
		{
			DWORD dwType = 0;
			DWORD dwSize = sizeof(szVirusDefsFolder);
			if(RegQueryValueEx(hRegKey, _T("AVENGEDEFS"), 0L, &dwType,
							   (BYTE*)szVirusDefsFolder, &dwSize) == ERROR_SUCCESS)
			{
				DeleteFolderRecursive(szVirusDefsFolder);
				RegDeleteValue(hRegKey, "AVENGEDEFS");
			}
		}
	}
	else  // the VirusDefs are still in use, so remove us from them.
	{
		HKEY hInstAppKey = NULL;
		
		if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\InstalledApps", 
										 0L, KEY_ALL_ACCESS, &hInstAppKey))
		{
			DWORD dwType = 0;
			DWORD dwSize = sizeof(szVirusDefsFolder);
			if(ERROR_SUCCESS == RegQueryValueEx(hInstAppKey, _T("AVENGEDEFS"), 0L, &dwType, 
							   (BYTE*)szVirusDefsFolder, &dwSize))
			{
				TCHAR szDATFileName[MAX_PATH] = {0};
				TCHAR szCurrentDefs[MAX_PATH] = {0};

				lstrcpy(szDATFileName, szVirusDefsFolder);
				lstrcat(szDATFileName, _T("\\DEFINFO.DAT"));

				GetPrivateProfileString(_T("DefDates"), _T("CurDefs"), _T("ERROR"),
										szCurrentDefs, sizeof(szCurrentDefs), szDATFileName);

				if(lstrcmp(_T("ERROR"), szCurrentDefs) != 0)
				{
					lstrcpy(szDATFileName, szVirusDefsFolder);
					lstrcat(szDATFileName, _T("\\USAGE.DAT"));

					TCHAR szQSCON_VirusDefs[MAX_PATH] = {0};
					
					if(0 != LoadString(g_hInstance, IDS_REGVAL_QCVIRDEFS, szQSCON_VirusDefs, 
									   sizeof(szQSCON_VirusDefs)))
					{
						WritePrivateProfileString(szCurrentDefs, szQSCON_VirusDefs, NULL,
												  szDATFileName);
					}
				}

			}

			RegCloseKey(hInstAppKey);
		}
		RegCloseKey(hRegKey);
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CleanupSharedDefsKey
//
// Description  : Removes the QConsole value from the SharedDefs registry key.
//				  If there are no other values in the key, SharedDefs will be
//				  deleted.
//
// Return type  : None
//
// Argument     : 
///////////////////////////////////////////////////////////////////////////////
// 8/16/99 - CMCGOWEN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CleanupSharedDefsKey()
{
	BOOL bRtnVal = FALSE;

	TCHAR szRegVirusDefsValue[MAX_PATH] = {0};

	if(0 != LoadString(g_hInstance, IDS_REGVAL_QCVIRDEFS, szRegVirusDefsValue, 
					   sizeof(szRegVirusDefsValue)))
	{
		HKEY hRegKey = NULL;

		if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\SharedDefs",
										 0L, KEY_ALL_ACCESS, &hRegKey))
		{
			if(ERROR_SUCCESS == RegDeleteValue(hRegKey, szRegVirusDefsValue))
			{
				// we have successfully deleted the value, now check for more values in the
				// key.  if there are no more values, delete the registry key.
				TCHAR szValueName[MAX_PATH] = {0};				
				DWORD dwIndex = 0;
				DWORD dwSize = sizeof(szValueName);

				if(ERROR_SUCCESS == RegEnumValue(hRegKey, dwIndex, szValueName, &dwSize,
													0, NULL, NULL, NULL))
					++dwIndex;
								
				if(dwIndex == 0) 
				{
					RegCloseKey(hRegKey);
					hRegKey = NULL;
					if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
										"Software\\Symantec", 0L, KEY_ALL_ACCESS, &hRegKey))
						// use SHDeleteKey so sub keys get deleted as well.
						SHDeleteKey(hRegKey, "SharedDefs");
				}
			}
			RegCloseKey(hRegKey);
		}
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CleanupRegistry
//
// Description  : Cleans up our registry entries.
//
// Return type  : None
//
// Argument     : None
///////////////////////////////////////////////////////////////////////////////
// 8/17/99 - CMCGOWEN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CleanupRegistry()
{
	HKEY hRegKey = NULL;
		
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Quarantine"), 
									 0L, KEY_ALL_ACCESS, &hRegKey))
	{
		DWORD dwNumSubKeys = 0;

		RegQueryInfoKey(hRegKey, NULL, NULL, NULL, &dwNumSubKeys, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL);

		RegCloseKey(hRegKey);
		hRegKey = NULL;

		if(dwNumSubKeys == 0) // there were no sub keys, so delete the key.
		{
			if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), 0L, 
											 KEY_ALL_ACCESS, &hRegKey))
			{
				RegDeleteKey(hRegKey, _T("Quarantine"));
				RegCloseKey(hRegKey);
			}
		}
	}
	
	return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: DeleteFolderRecursive
//
// Description  : Deletes the entire folder branch specified in szFolder.
//
// Return type  : BOOL
//
// Argument     : szFolder - Path to folder to be deleted.
///////////////////////////////////////////////////////////////////////////////
// 8/16/99 - CMCGOWEN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL DeleteFolderRecursive(LPCTSTR szFolder)
{
	TCHAR szFileName[MAX_PATH] = {0};
	WIN32_FIND_DATA FindFileData;
	HANDLE hFile = NULL;
	BOOL bRtnVal = TRUE;

	lstrcpy(szFileName, szFolder);
	lstrcat(szFileName, _T("\\*.*"));

	if( INVALID_HANDLE_VALUE != (hFile = FindFirstFile(szFileName, &FindFileData)))
	{
		// iterate through all of the files in the folder and remove them.
		do
		{
			TCHAR szFilePath[MAX_PATH] = {0};
	
			lstrcpy(szFilePath, szFolder);
			lstrcat(szFilePath, _T("\\"));
			lstrcat(szFilePath, FindFileData.cFileName);

			if((FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes))
			{
				// The found file is a folder, so call this function again
				// as long as the folder name is not "." or ".."
				if( (0 != lstrcmp(FindFileData.cFileName, _T("."))) &&
					 (0 != lstrcmp(FindFileData.cFileName, _T(".."))) )
					 DeleteFolderRecursive(szFilePath);
			}
			else
			{
				// It's a file, so delete it.
				DeleteFile(szFilePath);
			}
		}while(FindNextFile(hFile, &FindFileData));
		
		FindClose(hFile);
	}
	else
		bRtnVal = FALSE;

	RemoveDirectory(szFolder);

	return bRtnVal;
}
