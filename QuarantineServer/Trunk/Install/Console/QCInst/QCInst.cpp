#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "resource.h"
#include "qcinst.h"
#include <string>
#include "SymSaferRegistry.h"

///////////////////////////////////////////////////////////////////////////////
// 
// Locals
// 
///////////////////////////////////////////////////////////////////////////////

HINSTANCE g_hInstance = NULL;
HWND g_hWnd = NULL;

void CleanupSharedDefsKey(void);
void CleanupVirusDefs(void);
void CleanupRegistry(void);
BOOL DeleteFolderRecursive(LPCTSTR szFolder);

#define CLIENT_TYPE_SERVER		4	// from vpcommon.h
#define DLL_REGISTER			"DllRegisterServer"

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
    g_hWnd = hWnd;

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
// Function name: CleanupScandlvr
//
// Description  : If removing QConsole, and SAV Server is still installed,
//                re-register the SAV version of ScanDlvr.dll (1-259X2V)
//
// Return type  : LONG 
//
// Argument     : 
//
///////////////////////////////////////////////////////////////////////////////
// 7/19/04 - Jim Shock: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) CleanupScandlvr( )
{
	int nRtnVal = 0;
	HKEY hRegKey = NULL;

	// moved this from UninstUninitialize - needs to be called later - new function - JS
	// Bugfix - 1-259X2V - both sav and qconsole register (and unregister) ScanDlvr.dll
	//    The first one to uninstall unregisters it, rendering it inoperative 
	//	  and you get an error uninstalling SAV: ScanDlvr failed to un-register
	//	  If SAV is still installed here, go back and re-register ScanDlvr
	//    This corresponds to SAV - NavInstNT.dll - UninstUninitialize that does the same thing
	// Check for SAV Server

 	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion",
									 0L, KEY_ALL_ACCESS, &hRegKey))
	{
		DWORD dwValue = 0;
		DWORD dwType = 0;
		DWORD dwSize = sizeof(dwValue);

		// First query the ProductVersion - many of the CurrentVersion keys are left behind if SSC 
		// is still installed
		if(SymSaferRegQueryValueEx(hRegKey, _T("ProductVersion"), 0L, &dwType,
						   (BYTE*)&dwValue, &dwSize) == ERROR_SUCCESS)
		{
			// Next query the ClientType - only servers also use ScanDlvr
			dwValue = 0;
			if((SymSaferRegQueryValueEx(hRegKey, _T("ClientType"), 0L, &dwType,
							   (BYTE*)&dwValue, &dwSize) == ERROR_SUCCESS) && 
							   (CLIENT_TYPE_SERVER == dwValue))
			{
			    char	szDataBuf[MAX_PATH];

				// SAV Server is still installed so re-register 
				// scandlvr.dll to point to the SAV directory

				// get the sav directory
				dwSize = sizeof(szDataBuf);
				if(SymSaferRegQueryValueEx(hRegKey, _T("Home Directory"), 0L, &dwType,
								   (BYTE*)szDataBuf, &dwSize) == ERROR_SUCCESS)
				{
					char	szCurrDir[MAX_PATH];
					HINSTANCE hLibrary;
					FARPROC lpDllEntryPoint;

					// Save the current directory
					GetCurrentDirectory(MAX_PATH, szCurrDir);

					// Set the directory to the SAV directory
					SetCurrentDirectory(szDataBuf);
	
					// Register SCANDLVR.DLL
					lstrcat(szDataBuf, _T("\\scandlvr.dll"));
					hLibrary = LoadLibrary(szDataBuf);
					if (hLibrary) 
					{
						lpDllEntryPoint = GetProcAddress(hLibrary, DLL_REGISTER);
						if (lpDllEntryPoint)
							nRtnVal = (DWORD)(*lpDllEntryPoint)();
					}

					// Restore the current directory
					SetCurrentDirectory ( szCurrDir );
				}
			}
		}
	}
	// 1-259X2V - end of changes

	// return SUCCESS in all cases
	return ERROR_SUCCESS;
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
			if(SymSaferRegQueryValueEx(hRegKey, _T("AVENGEDEFS"), 0L, &dwType,
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
			if(ERROR_SUCCESS == SymSaferRegQueryValueEx(hInstAppKey, _T("AVENGEDEFS"), 0L, &dwType, 
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

#ifndef VER_SUITE_TERMINAL
#define VER_SUITE_TERMINAL                  0x00000010
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsNT4TerminalServer
//
// Description  :
//
// Return type  : LONG
//
// Argument     : 
//
///////////////////////////////////////////////////////////////////////////////
// 5/25/02 - ASiddiqui: Function created
///////////////////////////////////////////////////////////////////////////////

extern "C" LONG __declspec( dllexport ) IsNT4TerminalServer()
{
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;

    // Assume not terminal server install
    LONG lRetVal = 0;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure,
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    // Get the version information about the system
    bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi);
    if( !bOsVersionInfoEx )
    {
        // Failed to get the version info. Exit the function
        return lRetVal;
    }

    // Test to see if this is Terminal Server edition
//    if( (osvi.wReserved[0] & VER_SUITE_TERMINAL) )  field not avalible in .net replaced with
	if ((osvi.wSuiteMask & VER_SUITE_TERMINAL))		  // tmarles 4-7-04
    {
        // Yes it is terminal server
        lRetVal = 1;
    }

    return lRetVal;
}

/////////////////////////////////////////////////////////
// Routine:	IsFolderWriteable()
//
// Purpose:	Determine if the user has write access to the
//          install folder.
// Params:	LPCTSTR szInputDir - directory to check
//
// Return:	1 - folder is writable
//			0 - folder is not writable
//
//-------------------------------------------------------
//Talmage Williams created, added here by Jenny Huang
// 5-29-2002 modified by Rich Sutton
//-------------------------------------------------------
/////////////////////////////////////////////////////////
extern "C" __declspec( dllexport ) UINT IsFolderWriteable( LPCTSTR szInputDirUnNormalized )
{
	std::string szInputDir = szInputDirUnNormalized;

	// Test input param - must be at least a root directory.
	if( szInputDir.length() < 2 || szInputDir[1] != _T(':') )
		return( 0 );

	// Make sure that there is no terminating slash,
	// because the rest of the function makes this assumption.
	if( szInputDir[ szInputDir.length() - 1 ] == _T('\\') )
		szInputDir.erase( szInputDir.length() - 1 );

	// Create a bogus file to test writability.
	std::string szFileName = szInputDir;
	szFileName.append("\\");
	szFileName.append( "DeleteThisFile.txt" );

	//Check to see if directory exists and is writeable
	HANDLE hFile = CreateFile( szFileName.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL );
	if( INVALID_HANDLE_VALUE != hFile )
	{
		CloseHandle( hFile );
		DeleteFile( szFileName.data() );
		return 1;
	}

	//Reset the directory name
	szFileName = szInputDir;

	//Check to see if Directory exists
	BOOL bReturn = CreateDirectory( szFileName.c_str(), NULL );
	if(bReturn)
	{
		return 1;
	}

	DWORD dwLastError = GetLastError();
 
	//If subDirectory does not exist, then shorten path name until existing subDirectory or root is encountered
	while( (!bReturn) && (ERROR_ACCESS_DENIED != dwLastError) && (szFileName.length() > 3) )
	{
		size_t nSize = szFileName.find_last_of('\\');
		if( szFileName.length() == nSize)
		{
			szFileName = szFileName.substr(0, nSize -1);
			nSize = szFileName.find_last_of('\\');
		}
		szFileName = szFileName.substr(0, nSize);
		szFileName.append("\\");

		//Check to see if Directory exists
		bReturn =  CreateDirectory( szFileName.data(), NULL );
		dwLastError = GetLastError();

		szFileName = szFileName.substr( 0, szFileName.length() -1 );
	}

	//WritePermissions exist
	if(bReturn)
	{
		RemoveDirectory( szFileName.data() );
		return 1;
	}

	//If you get here, Folder is ReadOnly

	return 0;
}
