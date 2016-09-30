#include "helper.h"
#include "StdAfx.h"
#include <shlobj.h>					// SHGetFolder
#include <shlwapi.h>				// SHDeleteKey



const TCHAR* szCOMServerKey[] = 
{
	_T( "InprocHandler" ),
	_T( "InprocHandler32" ),
	_T( "InprocServer" ),
	_T( "InprocServer32" ), 
	_T( "LocalServer" ),
	_T( "LocalServer32" ) 
};

const TCHAR* szHKCUInstallerKeys[] =
{
	_T( "Software\\Microsoft\\Installer\\Products" ),
	_T( "Software\\Microsoft\\Installer\\Features" ),
	_T( "Software\\Classes\\Installer\\Products" ),
	_T( "Software\\Classes\\Installer\\Features" )
};

const TCHAR* szHKLMInstallerKeys[] =
{
	_T( "Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\Products" ),
	_T( "Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\Features" ),
	_T( "Software\\Classes\\Installer\\Products" ),
	_T( "Software\\Classes\\Installer\\Features" )
};

const TCHAR szLocalPackages[]		= _T( "Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\LocalPackages" );
const TCHAR szUninstall[]			= _T( "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall" );
const TCHAR szWinSharedDLLs[]		= _T( "Software\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs" );
const TCHAR szWinComponents[]		= _T( "Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\Components" );
const TCHAR szWinServices[]		    = _T( "System\\CurrentControlSet\\Services" );
const TCHAR szWinServicesVxD[]		= _T( "System\\CurrentControlSet\\Services\\VxD" );
const TCHAR szNTCommonPrograms[]    = _T("Common Programs");
const TCHAR sz9xCommonPrograms[]    = _T("Programs");
const TCHAR szNTCommonDesktop[]     = _T("Common Desktop");
const TCHAR sz9xCommonDesktop[]     = _T("Desktop");
const TCHAR szShellFolders[]        = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");



/////////////////////////////////////////////////////////////////////////////
//
//  LaunchAndWait() - Function to launch a process and wait for it to finish
//
//  Params: LPSTR - App to execute
//          LPSTR - Command line params
//
//  Return value:   TRUE if app successfully launched
//
/////////////////////////////////////////////////////////////////////////////
//  02/16/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////
BOOL LaunchAndWait(LPSTR lpExe, LPSTR lpParams)
{
    BOOL bRet = FALSE;
    TCHAR szLaunch[MAX_PATH * 3] = {0};
    STARTUPINFO sInfo;
    PROCESS_INFORMATION piInfo;

    memset(&sInfo, 0, sizeof(STARTUPINFO));
    memset(&piInfo, 0, sizeof(PROCESS_INFORMATION));

    sInfo.cb = sizeof(STARTUPINFO);

    if (lpExe != NULL && *lpExe)
    {
        _tcscpy(szLaunch, lpExe);
        GetShortPathName(szLaunch, szLaunch, sizeof(szLaunch));

        if (lpParams != NULL && *lpParams)
        {
            _tcscat(szLaunch, " ");
            _tcscat(szLaunch, lpParams);
        }
    }
    else
        _tcscpy(szLaunch, lpParams);

    if (CreateProcess(NULL, szLaunch, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &sInfo, &piInfo))
    {
        WaitForSingleObject(piInfo.hProcess, INFINITE);
        bRet = TRUE;
        CloseHandle(piInfo.hThread);
        CloseHandle(piInfo.hProcess);
    }
    else
    {
		// Failed to run the process
    }

    return(bRet);
}



////////////////////////////////////////////////////////////////////////////
//
//	This function removes all lines in [filename] that contain the string
//  [string]
//
//  NOTE: This function is NOT case sensitive.
//
//  Input:
//      string		-- This is a pointer to the string to search for
//      filename    -- This is the complete path and filename of the file
//						to remove lines from.
//
//  Output:
//      TRUE if all lines containing [string] were removed, FALSE otherwise
//
////////////////////////////////////////////////////////////////////////////
BOOL RemoveLine(LPCTSTR filename, LPCTSTR string)
{
	BOOL	bFirstRun = TRUE;
	BOOL	bNotAtEnd = TRUE;
	BOOL	bReadSuccess = FALSE;
	DWORD	dwOldFileSize = 0;
	DWORD	dwReadSize = 0;
	DWORD	WrittenBytes = 0;
	HANDLE	hFile;
	LPBYTE	NewFile = NULL;
	LPBYTE	OldFile = NULL;
	LPSTR	pSubStr = NULL;
	
	// Create a list of the seperating characters
	char seps[] = "\n";
	
	hFile = CreateFile(	filename,	// pointer to name of the file
						GENERIC_READ | GENERIC_WRITE,	// access (read-write) mode
						FILE_SHARE_READ,	// share mode
						NULL,	// pointer to security descriptor
						OPEN_EXISTING,	// how to create
						0,	// file attributes
						NULL 	// handle to file with attributes to copy
						);
	
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;
	
	dwOldFileSize = GetFileSize(hFile, NULL);
	
	// Allocate the buffers for files.
	OldFile = new BYTE[dwOldFileSize+10];
	if(NULL==OldFile)
	{
		return FALSE;
	}
	
	// Create a little extra space since it will add a '\n'
	// If the EOF occurs at the end of a line.
	NewFile = new BYTE[dwOldFileSize+10];
	if(NULL==NewFile)
	{
		delete[] OldFile;
		return FALSE;
	}
	
	// Clear the buffers.
	memset(NewFile, 0, dwOldFileSize+10);
	memset(OldFile, 0, dwOldFileSize+10);
	
	// Read in the file
	bReadSuccess = ReadFile(	hFile,	// handle of file to read
								OldFile,	// address of buffer that receives data
								dwOldFileSize,	// number of bytes to read
								&dwReadSize,	// address of number of bytes read
								NULL 	// address of structure for data
								);
	
	// Make sure that all the data was read in.
	if(FALSE == bReadSuccess || dwOldFileSize!=dwReadSize)
	{
		// Delete the buffers and close the file
		delete[] NewFile;
		delete[] OldFile;
		CloseHandle(hFile);
		return FALSE;
	}
	
	// Copy and conver string over to uppercase.
	pSubStr = _tcsdup(string);
	if(NULL == pSubStr)
	{
		// Delete the buffers and close the file
		delete[] NewFile;
		delete[] OldFile;
		CloseHandle(hFile);
		return FALSE;
	}
	_tcsupr(pSubStr);
	
	// Loop through the file analysing it one line at a time.
	while( bNotAtEnd )
	{
		LPSTR pLine = NULL;
		
		if(bFirstRun)
		{
			pLine = _tcstok( (LPTSTR)OldFile, seps );
		}
		else
			pLine = _tcstok( NULL, seps );
		
		// Check to see if it was allocated ok.
		if( pLine == NULL )
		{
			bNotAtEnd = FALSE;
		}
		else
		{
			// Make a duplicate
			LPSTR pCompare = _tcsdup(pLine);
			
			if(NULL == pCompare)
			{				
				// Delete the buffers and close the file
				delete[] NewFile;
				delete[] OldFile;
				CloseHandle(hFile);
				return FALSE;
			}
		
			// Convert to upper.
			_tcsupr(pCompare);
			// Check to see if the substring exists.
			LPSTR pSearch = _tcsstr(pCompare,pSubStr);
			if( pSearch == NULL )
			{
				// Append the string to the new file.
				_tcscat( (LPTSTR)NewFile, pLine);
				_tcscat( (LPTSTR)NewFile, "\n");
			}
			
			bFirstRun = FALSE;
			
			free(pCompare);
		}
		
	}
	
	SetFilePointer(	hFile,		// handle of file
		0,			// number of bytes to move file pointer
		NULL,		// address of high-order word of distance to move
		FILE_BEGIN	// how to move
				    );
	
	BOOL bSuccess = WriteFile(	hFile,
								NewFile,
								_tcslen((LPTSTR)NewFile),
								&WrittenBytes,
								NULL
								);
	
	// Check to see if the whole file was written to disk.
	if(_tcslen((LPTSTR)NewFile)!=WrittenBytes)
		bSuccess = FALSE;
	if(FALSE == SetEndOfFile(hFile))
		bSuccess = FALSE;
	if(FALSE == FlushFileBuffers(hFile))
		bSuccess = FALSE;
	
	// Delete the buffers and close the file
	free(pSubStr);
	delete[] NewFile;
	delete[] OldFile;
	CloseHandle(hFile);
	
	return bSuccess;
}


/////////////////////////////////////////////////////////////////////////////
//
// RegDeleteKeyEX();  
//
// On NT/Win2K, RegDeleteKey() does not delete keys with sub-keys. This function
// is a replacement to RegDeleteKey() which recursively deletes a key & its sub-keys.
//
/////////////////////////////////////////////////////////////////////////////
LONG RegDeleteKeyEx(HKEY hRootKey, LPCTSTR szSubKey)
{
	// open this key

	HKEY hKey;
	LONG lResult = RegOpenKeyEx(hRootKey, szSubKey, 0, KEY_ALL_ACCESS, &hKey);

	if (lResult != ERROR_SUCCESS)
		return lResult;

	// look for any subkeys to delete

	DWORD dwSize;
	TCHAR szChildKey[MAX_PATH * 2];
	FILETIME FileTime;

	while(1)
	{
		dwSize = sizeof(szChildKey) / sizeof(TCHAR);
		
		lResult = RegEnumKeyEx(hKey, 0, szChildKey, &dwSize, NULL,
			NULL, NULL, &FileTime);

		if (lResult != ERROR_SUCCESS)
			break;

		// if a subkey exists, delete it

		lResult = RegDeleteKeyEx(hKey, szChildKey);

		if (lResult != ERROR_SUCCESS)
			break;
	}

	// now close this key and delete it

	RegCloseKey(hKey);

	return RegDeleteKey(hRootKey, szSubKey);
}


/////////////////////////////////////////////////////////////////////////////
//
//  DeleteFilesInDirectory() - Function that will remove a directory or queue it
//                        for deletion after rebooting
//
//  Params: LPSTR - Directory to remove
//
//  Return value:   IS_DIR_REMOVED
//                  IS_DIR_NOT_FOUND
//                  IS_DIR_NOT_EMPTY
//
/////////////////////////////////////////////////////////////////////////////
//  02/16/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL DeleteFilesInDirectory(LPTSTR lpDirectory)
{
    BOOL bRet = FALSE;
    HANDLE hFind;
	TCHAR szDirectory[MAX_PATH * 2] = {0};
    WIN32_FIND_DATA fd;

    SetFileAttributes(lpDirectory, FILE_ATTRIBUTE_NORMAL);

	_tcscpy(szDirectory, lpDirectory);
	_tcscat(szDirectory, "\\");
	_tcscat(szDirectory, "*.*");

    hFind = FindFirstFile(szDirectory, &fd);

    if (hFind != INVALID_HANDLE_VALUE)
    {
		TCHAR szFile[MAX_PATH * 2] = {0};

        do
        {
			_tcscpy(szFile, lpDirectory);

			// Make sure it's not a "." or ".."
            if (fd.cFileName[0] != '.' && fd.cAlternateFileName[0] != '.')
            {
				_tcscat(szFile, "\\");
				if (fd.cFileName[0])
					_tcscat(szFile, fd.cFileName);
				else
					_tcscat(szFile, fd.cAlternateFileName);

                SetFileAttributes(szFile, FILE_ATTRIBUTE_NORMAL);

                if (DeleteFile(szFile) == FALSE)
				{
					// DeleteFile() failed, try to delete the file again using
					// FileInUseHandler().
					FileInUseHandler(szFile);
				}
            }
        }
        while (FindNextFile(hFind, &fd));

        FindClose(hFind);
    }

    bRet = TRUE;

	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
//
//  FileInUseHandler() - Function to handle a file-in-use properly under the
//                       appropriate OS for file/directory deletions
//
//  Params: LPSTR - File or directory to delete
//
//  Return value:   TRUE if successfully scheduled for deletion
//
/////////////////////////////////////////////////////////////////////////////
//  05/19/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////
BOOL FileInUseHandler(LPTSTR lpName)
{
    BOOL bRet = FALSE;
    FILE* fpFile;
    LPTSTR lpWin95File = "WININIT.INI";
    LPTSTR lpValue = "NSI";
    LPTSTR lpSection = "Rename";
    TCHAR szFile[MAX_PATH] = {0};
    TCHAR szLine[MAX_PATH * 2] = {0};
    TCHAR szShort[MAX_PATH] = {0};
    OSVERSIONINFO osInfo;

    GetShortPathName(lpName, szShort, sizeof(szShort));

    // determine which platform we're running on (Win95 or NT)
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (FALSE == GetVersionEx(&osInfo))
    {
        return(bRet);
    }

    // do the right thing...
    if (VER_PLATFORM_WIN32_NT == osInfo.dwPlatformId)
    {
        MoveFileEx(szShort, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
        bRet = TRUE;
    }
    else if (VER_PLATFORM_WIN32_WINDOWS == osInfo.dwPlatformId)
    {
        GetWindowsDirectory(szFile, sizeof(szFile));
		_tcscat(szFile, "\\");
		_tcscat(szFile, lpWin95File);

        WritePrivateProfileString(lpSection, lpValue, szShort, szFile);
        WritePrivateProfileString(NULL, NULL, NULL, szFile);

        fpFile = fopen(szFile, "rt");

        if (fpFile != NULL)
        {
            fpos_t fpos;

            do
            {
                fgetpos(fpFile, &fpos);
                fgets(szLine, SYM_MAX_PATH * 2, fpFile);

                if (!_strnicmp(szLine, lpValue, 3))
                {
                    fflush(fpFile);
                    fclose(fpFile);

                    fpFile = fopen(szFile, "r+t");

                    if (fpFile != NULL)
                    {
                        fsetpos(fpFile, &fpos);
                        fprintf(fpFile, "NUL=%s\n", (LPTSTR)szShort);
                        fflush(fpFile);
                        fclose(fpFile);
                        fpFile = NULL;
                    }
                    break;
                }
            }
            while(!feof(fpFile));

			if (fpFile != NULL)
				fclose(fpFile);

			bRet = TRUE;
		}
	}

    return(bRet);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function:  CheckPreviousVersion
//
//  Purpose:  This function attempts to determine if there is already NAV on 
//            the target system. If so, it checks to see if it is a previous
//			  version, a new version, or the same version. It then output the
//			  results to the corresponding MSI properties.
//
//
///////////////////////////////////////////////////////////////////////////////

BOOL CheckPreviousVersion(LPTSTR lpszProductName)
{   
	
	TCHAR szSubKey[MAX_PATH] = {0};
	TCHAR szVal[MAX_PATH] = {0};
	DWORD dwType = REG_SZ;
	DWORD dwLen = MAX_PATH;	
	BOOL bNAVIsInstalled = FALSE;
	HKEY hTmpKey = NULL;
	
	
	// Checks the InstalledPath to see if NAV is installed
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSymAppsPath, REG_OPTION_NON_VOLATILE, KEY_READ, &hTmpKey) == ERROR_SUCCESS)
	{		
		if(RegQueryValueEx(hTmpKey, _T("NAV"), NULL, &dwType, (BYTE*)szVal, &dwLen) == ERROR_SUCCESS)
		{	
			bNAVIsInstalled = TRUE;
		}
		else if(RegQueryValueEx(hTmpKey, _T("NAV95"), NULL, &dwType, (BYTE*)szVal, &dwLen) == ERROR_SUCCESS)
		{
			bNAVIsInstalled = TRUE;		
		}
		
		else if(RegQueryValueEx(hTmpKey, _T("NAVNT"), NULL, &dwType, (BYTE*)szVal, &dwLen) == ERROR_SUCCESS)
		{
			bNAVIsInstalled = TRUE;		
		}
		RegCloseKey(hTmpKey);
	}
	
	// If we can't locate the NAV installed path... we return E_FAIL.
	else
	{
		return E_FAIL;
	}
		
	// If NAV is installed, get version info.
	if(bNAVIsInstalled)
	{	
		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx (&osvi);

		// Check OS type to determine correct regkey to look for the version.
		if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		{
			_tcscpy(szSubKey, szNAVAppsKey);
			g_bNT = FALSE;
		}

		else if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
		{
			_tcscpy(szSubKey, szNAVAppsKeyNT);
			g_bNT = TRUE;
		}
		
		// Finding the Version of NAV on the system
		HKEY hKey = NULL;
		LRESULT lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, REG_OPTION_NON_VOLATILE, 
					KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKey);

		//if we couldnt find the key, look for the other key, because its possible they upgraded
		//from 9x to 2k or something and the original install was under the 9x key even tho the machine 
		//is now an NT platform.
		if (lResult != ERROR_SUCCESS)
		{			
			//if we are an NT machine.. look for the 9x key
			if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
			{
				//try again to open the 98 key
				lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szNAVAppsKey, REG_OPTION_NON_VOLATILE, 
					KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKey);
			}
		}

		TCHAR szBuffer[3] = {0};
		DWORD dwSize = sizeof(szBuffer);
		
		if (RegQueryValueEx(hKey, _T("CurrentVersion"), NULL, NULL, (BYTE*) szBuffer, &dwSize) == ERROR_SUCCESS)
		{
			// If NAV2000 or 2001 is installed on the system, we can offer to uninstall for users.			
			if (szBuffer[0] == 6)
				_tcscpy(lpszProductName, _T("Norton AntiVirus 2000"));
			else if (szBuffer[0] == 7)
				_tcscpy(lpszProductName, _T("Norton AntiVirus 2001"));		
			else if (szBuffer[0] == 8)
				_tcscpy(lpszProductName, _T("Norton AntiVirus 2002"));
		}
		
		RegCloseKey(hKey);
		return S_OK;
	} 
	
	// If we can't retrieve the version info... return E_FAIL
	else
		return E_FAIL;

}    // CheckPreviousVersion





/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		CleanupWindowsInstallerInfo()
//
//  Description:	Removes all Windows Installer information for all products
//						specified in the ini file.
//						Functions the same as running the msizap.exe T { product code }.
//
/////////////////////////////////////////////////////////////////////////////
VOID CleanupWindowsInstallerInfo( LPCTSTR lpszProductCode, BOOL bNT, BOOL bReboot )
{

	TCHAR szSubKey[ MAX_PATH ] = { 0 };
	TCHAR szProductGUID[ MAX_PATH ] = { 0 };

	if( ( ! lpszProductCode ) || ( lpszProductCode[ 0 ] == '\0' ) )
		return;

	FixGUID( lpszProductCode, szProductGUID, TRUE, FALSE, FALSE);

	// Remove the installer .msi file.

	DeleteMSIFile( lpszProductCode, bNT, bReboot );

	// Remove the product key under Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\LocalPackages

	_stprintf( szSubKey, _T( "%s\\%s" ), szLocalPackages, szProductGUID );

	if( DeleteRegKey( HKEY_LOCAL_MACHINE, szSubKey ) )
		UninstLog->Log("Delete HKLM\\ %s", szSubKey);
		

	// Remove the product key under Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall

	_stprintf( szSubKey, _T( "%s\\%s" ), szUninstall, lpszProductCode );

	if( DeleteRegKey( HKEY_LOCAL_MACHINE, szSubKey ) )
		UninstLog->Log("Delete HKLM\\%s", szSubKey);

	for( int i = 0; i < 4; i++ )
	{
		// Remove the Installer registry keys under HKEY_LOCAL_MACHINE

		_stprintf( szSubKey, _T( "%s\\%s" ), szHKLMInstallerKeys[ i ], szProductGUID );

		if( DeleteRegKey( HKEY_LOCAL_MACHINE, szSubKey ) )
			UninstLog->Log("Delete HKLM\\%s", szSubKey );

		// Remove the Installer registry keys under HKEY_CURRENT_USER

		_stprintf( szSubKey, _T( "%s\\%s" ), szHKCUInstallerKeys[ i ], szProductGUID );

		if( DeleteRegKey( HKEY_CURRENT_USER, szSubKey ) )
			UninstLog->Log("Delete HKLM\\%s", szSubKey );
	}

	// Remove upgrade codes.

	BOOL bKeyDeleted = FALSE;

	DeleteReferenceFromComponent( _T( "Software\\Classes\\Installer\\UpgradeCodes" ), lpszProductCode, bKeyDeleted );

	if( bKeyDeleted )
		UninstLog->Log("Remove %s from Software\\Classes\\Installer\\UpgradeCodes", szProductGUID);

	bKeyDeleted = FALSE;
	DeleteReferenceFromComponent( _T( "Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UpgradeCodes" ), lpszProductCode, bKeyDeleted );

	if( bKeyDeleted )
		UninstLog->Log("Remove %s from Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UpgradeCodes", szProductGUID);

	// Decrement the shared count of all shared file that the product depends on.

	ReduceSharedDllsCount( lpszProductCode );

	// Delete reference from all components that the product depends on.

	DeleteRefFromAllComponents( lpszProductCode );

		// Remove the installer folder. Examples of installer folder: 
		//  NT: c:\winnt\Installer\{3075C5C3-0807-4924-AF8F-FF27052C12AE}
		//  9x: c:\windows\application data\microsoft\installer\{3075C5C3-0807-4924-AF8F-FF27052C12AE}

	DeleteInstallerDir( lpszProductCode, bNT );
}



BOOL FixGUID(LPCTSTR pszGUIDIn, LPTSTR pszGUIDOut, BOOL bFlip, BOOL bDashes, BOOL bBraces)
{
	TCHAR szGUIDCopy[256] = {0};
	TCHAR szSectA[9] = {0};
	TCHAR szSectB[5] = {0};
	TCHAR szSectC[5] = {0};
	TCHAR szSectD[5] = {0};
	TCHAR szSectE[13] = {0};

	// Make a copy of the input
	_tcscpy(szGUIDCopy, pszGUIDIn);

	// Remove all dashes
	int nCopyLen = _tcslen(szGUIDCopy);
	LPTSTR pszDash = _tcschr(szGUIDCopy, _T('-'));
	while(pszDash)
	{
		memmove(pszDash, pszDash + sizeof(TCHAR), nCopyLen * sizeof(TCHAR));
		--nCopyLen;

		// Find the next
		pszDash = _tcschr(szGUIDCopy, _T('-'));
	}

	// Remove the {'s and }'s
	if(*szGUIDCopy == _T('{'))
	{
		memmove(szGUIDCopy, szGUIDCopy + sizeof(TCHAR), nCopyLen * sizeof(TCHAR));
		--nCopyLen;
	}

	if(szGUIDCopy[nCopyLen - 1] == _T('}'))
	{
		szGUIDCopy[nCopyLen - 1] = _T('\0');
		--nCopyLen;
	}

	// Verify the length
	if(nCopyLen != 32)
		return FALSE;

	// Break up the input
	_tcsncpy(szSectA, szGUIDCopy, 8);
	_tcsncpy(szSectB, szGUIDCopy + 8, 4);
	_tcsncpy(szSectC, szGUIDCopy + 12, 4);
	_tcsncpy(szSectD, szGUIDCopy + 16, 4);
	_tcsncpy(szSectE, szGUIDCopy + 20, 12);

	if(bFlip)
	{
		ReverseString(szSectA);
		ReverseString(szSectB);
		ReverseString(szSectC);

		// modified by lhoang
		FlipString(szSectD);
		FlipString(szSectE);
	}

	if(bDashes)
		wsprintf(pszGUIDOut, _T("%s%s-%s-%s-%s-%s%s"), bBraces ? _T("{") : _T(""), szSectA, szSectB, szSectC, szSectD, szSectE, bBraces ? _T("}") : _T(""));
	else
		wsprintf(pszGUIDOut, _T("%s%s%s%s%s%s%s"), bBraces ? _T("{") : _T(""), szSectA, szSectB, szSectC, szSectD, szSectE, bBraces ? _T("}") : _T(""));

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		DeleteMSIFile
//
//  Description:	Delete windows installer msi file.
//						Returns TRUE if the file is deleted; FALSE otherwise.
//
/////////////////////////////////////////////////////////////////////////////
VOID DeleteMSIFile( LPCTSTR lpszProductCode, BOOL bNT, BOOL& bReboot )
{
	DWORD dwSize = MAX_PATH;
	TCHAR szSubKey[ MAX_PATH ] = { 0 };
	TCHAR szSubKeyName[ MAX_PATH ] = { 0 };
	TCHAR szFileName[ MAX_PATH ] = { 0 };
	TCHAR szProductGUID[ MAX_PATH ] = { 0 };

	if( bNT )
	{
		// Get the full file name from
		//  HKLM\Software\Microsoft\Windows\CurrentVersion\Installer\LocalPackages\3C5C570370804294FAF8FF7250C221EA\S-1-5-18

		FixGUID( lpszProductCode, szProductGUID, TRUE, FALSE, FALSE);
		_stprintf( szSubKey, _T( "Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\LocalPackages\\%s" ), szProductGUID );
		_tcscpy( szSubKeyName, _T( "S-1-5-18" ) );
	}
	else
	{
		// Get the full file name from
		//  Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{3075C5C3-0807-4924-AF8F-FF27052C12AE}\\LocalPackage

		_stprintf( szSubKey, _T( "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s" ), lpszProductCode );
		_tcscpy( szSubKeyName, _T( "LocalPackage" ) );
	}

	if( GetKeyValue( HKEY_LOCAL_MACHINE, szSubKey, szSubKeyName, ( LPBYTE ) szFileName, dwSize ) )
	{
		if( RemoveFile( szFileName, bReboot ) )
			UninstLog->Log("Delete %s", szFileName);
	}
}



/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		DeleteRefereceFromComponent
//
//  Description:	Remove the specified reference from the given component.
//						For example, the function enumerates the key lpszComponent 
//						"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\Components\\AAAAAAASKDFASLFK5",
//						If it finds szGUID "3C5C570370804294FAF8FF7250C221EA", it deletes the value.
//						If the lpszComponent key is now empty, it deletes the key and sets bKeyDeleted to TRUE.
//
//						Return TRUE if the value is deleted; FALSE otherwise.
//
/////////////////////////////////////////////////////////////////////////////
BOOL DeleteReferenceFromComponent( LPCTSTR lpszComponent, LPCTSTR lpszProductCode, BOOL& bKeyDeleted )
{
	HKEY hKey = NULL;
	LONG lResult = ERROR_FILE_NOT_FOUND;
	TCHAR szName[ MAX_PATH ] = { 0 };
	DWORD dwSize = MAX_PATH;
	TCHAR szProductGUID[ MAX_PATH ] = { 0 };

	if( ( ! lpszProductCode ) || ( lpszProductCode[ 0 ] == '\0' ) )
		return( FALSE );

	// Get the GUID from the product code.

	FixGUID( lpszProductCode, szProductGUID, TRUE, FALSE, FALSE );

	bKeyDeleted = FALSE;

	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpszComponent, 0, KEY_ALL_ACCESS, &hKey );

	if( lResult == ERROR_SUCCESS )
	{
		// Delete the client key.

		lResult = RegDeleteValue( hKey, szProductGUID );

		if( lResult == ERROR_SUCCESS )
		{
			// If the key is empty, delete it.

			if( ERROR_NO_MORE_ITEMS == RegEnumValue( hKey, 0, szName, &dwSize, NULL, NULL, NULL, NULL ) )
			{
				if( ERROR_SUCCESS == SHDeleteKey( HKEY_LOCAL_MACHINE, lpszComponent ) )
					bKeyDeleted = TRUE;
			}
		}

		RegCloseKey( hKey );
	}

	return( lResult == ERROR_SUCCESS );
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		ReduceSharedDllCount
//
//  Description:	The function finds all shared Dlls that the product 
//						It reduces the corresponding shared counts under
//						software\microsoft\windows\currentversion\shareddlls.
//
/////////////////////////////////////////////////////////////////////////////
VOID ReduceSharedDllsCount( LPCTSTR lpszProductCode )
{
	HKEY hKey = NULL;
	HKEY hSubKey = NULL;
	TCHAR szBuff[ MAX_PATH ] = { 0 };
	TCHAR szComponent[ MAX_PATH ] = { 0 };
	TCHAR szSubKey[ MAX_PATH ] = { 0 };
	DWORD dwSize = MAX_PATH;
	DWORD dwSubKeyIdx = 0;
	TCHAR szProductGUID[ MAX_PATH ] = { 0 };

	if( ( ! lpszProductCode ) || ( lpszProductCode[ 0 ] == '\0' ) )
		return;

	FixGUID( lpszProductCode, szProductGUID, TRUE, FALSE, FALSE);

	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, szWinComponents, 0, KEY_ALL_ACCESS, &hKey ) )
		return;

	// We want to interate through all subkeys under
	//  Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\Components.
	//  For each subkey, 
	//  1. Look for shared dll that the product depends on. For example.
	//		 3C5C570370804294FAF8FF7250C221EA			c?\windows\system\msvcrt.dll
	//
	//  2. Look for NAV shared file. For example, 
	//     3C5C570370804294FAF8FF7250C221EA			c?\Program Files\Common Files\Symantec Shared\Sevinst.exe.

	while( ERROR_NO_MORE_ITEMS != RegEnumKeyEx( hKey, dwSubKeyIdx, szSubKey, &dwSize, NULL, NULL, NULL, NULL ) )
	{
		// Set up the component string.

		_stprintf( szComponent, _T( "%s\\%s" ), szWinComponents, szSubKey );

		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szComponent, 0, KEY_ALL_ACCESS, &hSubKey ) )
		{
			// Must reset the size before calling RegQueryValueEx

			dwSize = MAX_PATH;

			//	Check to see if the subkey contains our lpszProdGUID

			if( ERROR_SUCCESS == RegQueryValueEx( hSubKey, szProductGUID, NULL, NULL, ( LPBYTE ) szBuff, &dwSize ) )
			{
				// If szSharedFile contains '?'( i.e. c?\Windows\System\msvcrt.dll ), 
				//  replace '?' with ':'.

				UINT i1 = 0, i2 = 0;
				TCHAR szSharedFile[ MAX_PATH ] = { 0 };

				for( i1 = 0; i1 < dwSize; i1++ )
				{
					szSharedFile[ i2++ ] = ( szBuff[ i1 ] == '?' ) ? ':' : szBuff[ i1 ];
				}
				szSharedFile[ i2 ] = '\0';

				// Reduce the share count.

				DecrementSharedCount( szSharedFile );
			}

			RegCloseKey( hSubKey );
		}

		// Must reset the size before calling RegEnumKeyEx

		dwSize = MAX_PATH;

		dwSubKeyIdx++;
	}

	RegCloseKey( hKey );
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		DeleteRefFromAllComponents
//
//  Description:	Delete references from all components that lpszProdGUI depends on.
//						"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\Components".
//
/////////////////////////////////////////////////////////////////////////////
VOID DeleteRefFromAllComponents( LPCTSTR lpszProductCode )
{
	HKEY hKey = NULL;
	TCHAR szSubKeyName[ MAX_PATH ] = { 0 };
	TCHAR szComponent[ MAX_PATH ] = { 0 };
	DWORD dwSize = MAX_PATH;
	DWORD dwSubKeyIdx = 0;
	BOOL bKeyDeleted = FALSE;
	UINT uClientCount = 0;

	if( ( ! lpszProductCode ) || ( lpszProductCode[ 0 ] == '\0' ) )
		return;

	// Open the key Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\Components.

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szWinComponents, 0, KEY_ALL_ACCESS, &hKey ) )
	{
		// Iterate the subkeys.

		while( ERROR_SUCCESS == RegEnumKeyEx( hKey, dwSubKeyIdx, szSubKeyName, &dwSize, NULL, NULL, NULL, NULL ) )
		{
			// Set up the component string.

			_stprintf( szComponent, _T( "%s\\%s" ), szWinComponents, szSubKeyName );

			// Delete reference to the component.

			bKeyDeleted = FALSE;
			if( DeleteReferenceFromComponent( szComponent, lpszProductCode, bKeyDeleted ) )
			{
				UninstLog->Log("Delete reference from component %s", szSubKeyName);
				uClientCount++;

				// If the key with index = x is deleted, the system will
				//  assign index x to the next key. 
				//	 We increment the index in two case:
				//  1. If the current subkey contain the NavUpgradeCode and it is not deleted.
				//	 2. If the current subkey does not contains the NavUpgradeCode. 

				if( ! bKeyDeleted )
					dwSubKeyIdx++;
			}
			else
			{
				dwSubKeyIdx++;
			}

			// Must reset the size before get the next subkey.

			dwSize = sizeof( szSubKeyName ) / sizeof( TCHAR );
		}

		RegCloseKey( hKey );
	}

	UninstLog->Log("Total references deleted = %d", uClientCount);
}



/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		DeleteInstallerDir
//
//  Description:	Delete the Installer directory of the given product.
//						return TRUE if the directory is deleted; FALSE otherwise.
//
/////////////////////////////////////////////////////////////////////////////
BOOL DeleteInstallerDir( LPCTSTR lpszProdCode, BOOL bNT )
{
	BOOL bSuccess = FALSE;
	TCHAR szDir[ MAX_PATH ] = { 0 };
	TCHAR szInstallerDir[ MAX_PATH ] = { 0 };

	if( bNT )
	{
		// if NT, the directory is c:\winnt\Installer\{3075C5C3-0807-4924-AF8F-FF27052C12AE}

		if( GetWindowsDirectory( szDir, MAX_PATH ) )
		{
			_stprintf( szInstallerDir, _T( "%s\\Installer\\%s"), szDir, lpszProdCode );
			bSuccess = TRUE;
		}
	}
	else
	{
		// if 9x, the directory is c:\windows\application data\microsoft\installer\{3075C5C3-0807-4924-AF8F-FF27052C12AE}

		if( RNAVSHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szDir )  == S_OK )
		{
			_stprintf( szInstallerDir, _T( "%s\\Microsoft\\Installer\\%s"), szDir, lpszProdCode );
			bSuccess = TRUE;
		}
        else
        {
            // Could not get the folder path using shfolder.dll, now try to get it from the registry

            DWORD dwSize = sizeof( TCHAR ) * MAX_PATH;

            if( GetKeyValue( HKEY_CURRENT_USER, 
                             szShellFolders, 
                             _T("AppData"),
                             reinterpret_cast<LPBYTE>(szDir),
                             dwSize ) )
            {
                _stprintf( szInstallerDir, _T( "%s\\Microsoft\\Installer\\%s"), szDir, lpszProdCode );
                bSuccess = TRUE;
            }
            
        }

	}

	if( bSuccess )
	{
		if( DirectoryExists( szInstallerDir ) )
		{
			BOOL bReboot = FALSE;
			bSuccess = DeleteDirectory( szInstallerDir, bReboot );
			if( bSuccess )
				UninstLog->Log("Delete %s", szInstallerDir);
		}
	}

	return( bSuccess );
}


/////////////////////////////////////////////////////////////////
// 18 Sep 2001 WJORDAN - added to support NAV 5.0 removal
//
// Function:		RNAVSHGetFolderPath
//
// Description:	Dynamically link to shfolder.dll and get the
//                  folder path specified. Returns the return
//                  value of SHGetFolderPath if the
//                  function was retrieved, E_FAIL if not.
//
/////////////////////////////////////////////////////////////////
HRESULT RNAVSHGetFolderPath(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath)
{
    HRESULT hRet = E_FAIL;

    HMODULE hMod = LoadLibrary( _T("shfolder.dll") );
    if( hMod )
    {
        typedef HRESULT (WINAPI *pfnSHGetFolderPath )( HWND, int, HANDLE, DWORD, LPTSTR );
        pfnSHGetFolderPath pGetFolderPath = NULL;

        #ifdef _UNICODE
            pGetFolderPath = (pfnSHGetFolderPath) GetProcAddress( hMod, _T("SHGetFolderPathW") );
        #else
            pGetFolderPath = (pfnSHGetFolderPath) GetProcAddress( hMod, _T("SHGetFolderPathA") );
        #endif

        if(pGetFolderPath)
        {
            hRet = pGetFolderPath( hwndOwner, nFolder, hToken, dwFlags, pszPath );
        }

        FreeLibrary( hMod );
    }

    return hRet;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Function:		DeleteDirectory
//
//  Description:	Delete the specified directory together with all of its files 
//						and subdirectories or queue it for deletion after rebooting.
//
//  Output:			Returns TRUE if successful; FALSE otherwise.
//						If a reboot is needed, bReboot is set to TRUE.
//
//  Note:			lpDirectory must be checked for existing before calling this function.
//						This function is a modification of the function 
//						DeleteFilesInDirectory() in help.cpp of the project navinst.
//
/////////////////////////////////////////////////////////////////////////////
BOOL DeleteDirectory( LPTSTR lpDirectory, BOOL& bReboot )
{
	HANDLE hFind;
	TCHAR szDirectory[MAX_PATH * 2] = {0};
	WIN32_FIND_DATA fd;
	
	if( ( ! lpDirectory ) || ( lpDirectory[ 0 ] == '\0' ) )
		return( TRUE );

	SetFileAttributes(lpDirectory, FILE_ATTRIBUTE_NORMAL);

	_tcscpy(szDirectory, lpDirectory);
	_tcscat(szDirectory, "\\");
	_tcscat(szDirectory, "*.*");

	hFind = FindFirstFile(szDirectory, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		TCHAR szFile[MAX_PATH * 2] = {0};

		do
		{
			_tcscpy(szFile, lpDirectory);

			// Make sure it's not a "." or ".."
			if (fd.cFileName[0] != '.' && fd.cAlternateFileName[0] != '.')
			{
				_tcscat(szFile, "\\");

				if (fd.cFileName[0])
					_tcscat(szFile, fd.cFileName);
				else
					_tcscat(szFile, fd.cAlternateFileName);

				SetFileAttributes(szFile, FILE_ATTRIBUTE_NORMAL);

				if (DeleteFile(szFile) == FALSE)
				{
					// Check to see if it is a subdirectory.

	 				if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
					{
						TCHAR szSubDir[ MAX_PATH ];
						_stprintf( szSubDir, _T( "%s\\%s" ), lpDirectory, fd.cFileName );

						DeleteDirectory( szSubDir, bReboot );
					}
					else
					{
						// DeleteFile() failed, try to delete the file again using
						// FileInUseHandler().
						FileInUseHandler(szFile);
						bReboot = TRUE;
					}
				}
			}
		}
		while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
	else
	{
		return( FALSE );
	}

	// Remove the empty directory. If not successful, remove them after
	//  the computer restarts.

	if( ! RemoveDirectory( lpDirectory ) )
	{
		FileInUseHandler( lpDirectory );
		bReboot = TRUE;
	}

	return( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
//
//  DirectoryExists - Function to determine if a given directoryexists
//
//  Params: LPTSTR - Path to look for
//
//  Return value:   TRUE if dir is found
//
//  Note:	Source code for this function is from Navinstl\helper.cpp
//
/////////////////////////////////////////////////////////////////////////////
BOOL DirectoryExists( LPTSTR szDir )
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    WIN32_FIND_DATA     fd;

    hFind = FindFirstFile(szDir, &fd);

    if ( hFind != INVALID_HANDLE_VALUE )
    {
        if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            bRet = TRUE;

        FindClose(hFind);
    }

    return( bRet );
}


/////////////////////////////////////////////////////////////////
// 
// Function:		RemoveFile
//
// Description:	Returns TRUE if the file is deleted; FALSE otherwise.
//						If a reboot is required, bReboot is set TRUE.
//
//	Note:				lpszFileName must be a full file name.
//
/////////////////////////////////////////////////////////////////
BOOL RemoveFile( LPTSTR lpszFileName, BOOL& bReboot )
{
	// Check if the file exists.

	if( FileExists( lpszFileName ) )
	{
		// Delete the file.

		if( ! DeleteFile( lpszFileName ) )
		{
			FileInUseHandler( lpszFileName );
			bReboot = TRUE;
		}

		return( TRUE );	
	}

	return( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		GetKeyValue
//
//  Description:	Return TRUE if retrieve the value successfully; FALSE otherwise.
//						If successful, lpByteValue is set to the retrieved value.
//
/////////////////////////////////////////////////////////////////////////////
BOOL GetKeyValue( HKEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszName, LPBYTE lpByteValue, DWORD& dwBufSize )
{
	HKEY hSubKey = NULL;
	LRESULT lResult = ERROR_SUCCESS;
	DWORD dwType = 0;

	lResult = RegOpenKeyEx( hKey, lpszSubKey, REG_OPTION_NON_VOLATILE, 
				KEY_READ, &hSubKey);

	if ( lResult == ERROR_SUCCESS)
		lResult = RegQueryValueEx( hSubKey, lpszName, NULL, NULL, ( LPBYTE ) lpByteValue, &dwBufSize );

	RegCloseKey( hSubKey );

	return( lResult == ERROR_SUCCESS );
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		ReverseString
//
/////////////////////////////////////////////////////////////////////////////
void ReverseString(LPTSTR pszStr)
{
	int nLen = _tcslen(pszStr) - 1;
	
	TCHAR cTemp = _T('\0');
	for(int nPos = 0; nPos < ((nLen + 1) / 2); ++nPos)
	{
		cTemp = pszStr[nPos];
		pszStr[nPos] = pszStr[nLen - nPos];
		pszStr[nLen - nPos] = cTemp;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		Swap
//
/////////////////////////////////////////////////////////////////////////////
void Swap( TCHAR& c1, TCHAR& c2 )
{
	TCHAR cTemp = _T( '\0' );

	cTemp = c1;
	c1 = c2;
	c2 = cTemp;
}

/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		FlipString
//
/////////////////////////////////////////////////////////////////////////////
void FlipString( LPTSTR lpszStr )
{
	int nLen = 0;
	TCHAR cTemp = _T( '\0' );
	
	nLen = _tcslen( lpszStr );

	if( nLen % 2 )
		return;
	
	for( int i = 0; i < nLen - 1; i += 2 )
		Swap( lpszStr[ i ], lpszStr[ i + 1 ] );
}



/////////////////////////////////////////////////////////////////////////////
//
//  Function:		FileExists
//
//  Description:	return TRUE if the file exists; FALSE otherwise.
//
/////////////////////////////////////////////////////////////////////////////
BOOL FileExists( LPCTSTR lpszFileName )
{
	HANDLE hFile = NULL;

	// If the file name is NULL or empty, exit the function.

	if( ( ! lpszFileName ) || ( *lpszFileName == '\0' ) )
		return( FALSE );

	hFile = CreateFile( lpszFileName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	CloseHandle( hFile );

	return( hFile != INVALID_HANDLE_VALUE );
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		DeleteRegKey
//
//  Description:	Delete the specified registry key. If the key has "read-only"
//						attribute, the function sets "write" permission on the key, 
//						then delete the key. 
//						Return TRUE if the key is deleted; FALSE otherwise.
//						hKey ( i.e HKEY_LOCAL_MACHINE )
//
/////////////////////////////////////////////////////////////////////////////
BOOL DeleteRegKey( HKEY hKey, LPCTSTR lpszSubKey )
{
	// Delete the key and all subkeys.

	if( ERROR_ACCESS_DENIED == SHDeleteKey( hKey, lpszSubKey ) )
	{
		// We get access denied error because we don't have the 
		//  write permission on the key ( NT only ). 
		//  Enable the write security for the key.

		if( FixKeySecurity( hKey, lpszSubKey, TRUE, NULL ) )
		{
			// Now we have the permission, try to delete again.

			return( ERROR_SUCCESS == SHDeleteKey( hKey, lpszSubKey ) );
		}
		else
		{
			return( FALSE );
		}
	}

	return( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		FixKeySecurity
//
//  Description:	Set the security property for the specified key.
//
/////////////////////////////////////////////////////////////////////////////
BOOL FixKeySecurity(HKEY hKey, LPCTSTR pszSubKey, BOOL bSubKeys, PSECURITY_ATTRIBUTES pKMSA)
{
	if(!SetPrivilege(SE_TAKE_OWNERSHIP_NAME) || !SetPrivilege(SE_SECURITY_NAME))
		return FALSE;

	BOOL bRet = FALSE;
	BOOL bFreeKMSA = FALSE;
	if(pKMSA == NULL)
	{
		CreateAllAccessSecurityAttributes(&pKMSA);
		bFreeKMSA = TRUE;
	}

	if(pKMSA)
	{
		HKEY hFixKey = NULL;
		DWORD dwRet = RegOpenKeyEx(hKey, pszSubKey, 0, ACCESS_SYSTEM_SECURITY | WRITE_OWNER | WRITE_DAC, &hFixKey);
		if(ERROR_SUCCESS == dwRet)
		{
			// Give all access
			if(ERROR_SUCCESS == RegSetKeySecurity(hFixKey, SACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION, pKMSA->lpSecurityDescriptor))
				bRet = TRUE;
			
			RegCloseKey(hFixKey);
		}
	}

	if(bRet && bSubKeys)
	{
		// Enum subkeys... call this function for each
		HKEY hFixedKey = NULL;
		if(ERROR_SUCCESS == RegOpenKeyEx(hKey, pszSubKey, 0, KEY_ENUMERATE_SUB_KEYS, &hFixedKey))
		{
			TCHAR szSubKey[256] = {0};
			DWORD cbSubKey = 256;
			DWORD dwIdx = 0;

			while(ERROR_SUCCESS == RegEnumKey(hFixedKey, dwIdx++, szSubKey, cbSubKey))
				bRet = bRet && FixKeySecurity(hFixedKey, szSubKey, TRUE, pKMSA);

			RegCloseKey(hFixedKey);
		}
	}
	
	if(pKMSA && bFreeKMSA)
		FreeSecurityAttributes(pKMSA);

	return bRet;
}


//--------------------------------------------------------------------------
// 
// SetPrivilege
//
// Set a process privilege
//
//--------------------------------------------------------------------------

BOOL SetPrivilege(char *attribute)
{
	HANDLE					hToken;
	TOKEN_PRIVILEGES    	tokenPriv;
	LUID				 	luid;
	BOOL					result = FALSE;

	if (LookupPrivilegeValue(NULL, attribute, &luid)) {
		tokenPriv.PrivilegeCount           = 1;
		tokenPriv.Privileges[0].Luid       = luid;
		tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
			AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(tokenPriv), NULL, NULL);
			if (ERROR_SUCCESS == GetLastError()) {
				result = TRUE;
			}
		}
	}

	return result;
}


//---------------------------------------------------------------------------
// CreateKeyModifyAdminSecurityAttributes
//
// Allocate and initialize a security attributes structure that can be used
// to create registry entries that have the following characteristics;
//
//	- are owned by the administrator group
//  - the administrator group has all access rights
//  - the creator has all access rights
//  - the operating system all access rights
//  - the world has read access rights
//
// Returns ERROR_SUCCESS (0) if the functions is successful, otherwise a
// WIN32 error code.
//
// If the platform (i.e. WIN95) doesn't support security then th function
// will succeed but the pointer returned will be NULL.
//
// Also, this functions allocates resources that must be freed by calling
// FreeSecurityAttributes.
// 
//---------------------------------------------------------------------------
//
DWORD CALLBACK CreateAllAccessSecurityAttributes(PSECURITY_ATTRIBUTES *ppSecurityAttributes)
{
	return(customAdminSecurityAttributes(ppSecurityAttributes, KEY_ALL_ACCESS));
}


//---------------------------------------------------------------------------
// FreeSecurityAttributes
//
// Free the SIDs and memory allocated by the CreateSecurityAttributes
// functions.
// 
//---------------------------------------------------------------------------
//
DWORD CALLBACK FreeSecurityAttributes(PSECURITY_ATTRIBUTES pSA)
{
	if (pSA) {
		free(pSA);
	}
	return(0);
}


//---------------------------------------------------------------------------
// customAdminSecurityAttributes
//
// Allocate and initialize a security attributes structure that can be used
// to create registry entries that have the following characteristics;
//
//	- are owned by the administrator group
//  - the administrator group has all access rights
//  - the creator has all access rights
//  - the operating system all access rights
//  - the world has read access rights plus the caller input
//
// Returns ERROR_SUCCESS (0) if the functions is successful, otherwise a
// WIN32 error code.
//
// If the platform (i.e. WIN95) doesn't support security then th function
// will succeed but the pointer returned will be NULL.
//
// Also, this functions allocates resources that must be freed by calling
// FreeSecurityAttributes.
// 
//---------------------------------------------------------------------------
//
static DWORD customAdminSecurityAttributes(PSECURITY_ATTRIBUTES *ppSecurityAttributes, DWORD globalAccess)
{
	PDWORD pSubAuthority;
	PVOID pMem;
	DWORD dError;
	DWORD dMemSize;
	DWORD dSIDSize;
	PSECURITY_ATTRIBUTES pSA;
	PSECURITY_DESCRIPTOR pSD;
	PSID_IDENTIFIER_AUTHORITY pSIDAdminA;
	PSID_IDENTIFIER_AUTHORITY pSIDCreatorA;
	PSID_IDENTIFIER_AUTHORITY pSIDWorldA;
	PSID_IDENTIFIER_AUTHORITY pSIDSystemA;
	PACL  pACL;

	PSID  pSIDAdmin = NULL;
	PSID  pSIDCreator = NULL;
	PSID  pSIDWorld = NULL;
	PSID  pSIDSystem = NULL;

	SID_IDENTIFIER_AUTHORITY securityNTAuthority = SECURITY_NT_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY securityCreatorAuthority = SECURITY_CREATOR_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY securityWorldAuthority = SECURITY_WORLD_SID_AUTHORITY;

	*ppSecurityAttributes = NULL;
	//
	// Determine if this WIN32 platform support security functions by
	// calling a simple security function to see if it fails. If it is not
	// supported then return success.
	//
	if (IsSecuritySupported() == FALSE) {
		return(0);
	}
	//
	// compute the size of a SID (assume a maximum of 2 subathorities)
	//
	dSIDSize = GetSidLengthRequired(2);
	//
	// allocate a block of memory for all the security structures
	//
	dMemSize  = sizeof(SECURITY_ATTRIBUTES)
			 + sizeof(SECURITY_DESCRIPTOR)
			 + 4 * sizeof(SID_IDENTIFIER_AUTHORITY)
			 + 4 * dSIDSize
			 + ACL_BUFFER_SIZE;
	
	if ((pMem = malloc(dMemSize)) == NULL) {
		return(ERROR_NOT_ENOUGH_MEMORY);
	}
	//
	// initialize the structure pointers
	//
	pSA = (PSECURITY_ATTRIBUTES)pMem;					//this must be 1st
	pSD = (PSECURITY_DESCRIPTOR)(pSA + 1);
	pSIDAdminA   = (PSID_IDENTIFIER_AUTHORITY)((PBYTE)pSD + sizeof(SECURITY_DESCRIPTOR));
	pSIDCreatorA = (PSID_IDENTIFIER_AUTHORITY)(pSIDAdminA + 1);
	pSIDWorldA   = (PSID_IDENTIFIER_AUTHORITY)(pSIDCreatorA + 1);
	pSIDSystemA  = (PSID_IDENTIFIER_AUTHORITY)(pSIDWorldA + 1);
	pSIDAdmin    = (PSID)(pSIDSystemA + 1);
	pSIDCreator  = (PSID)((PBYTE)pSIDAdmin   + dSIDSize);
	pSIDWorld    = (PSID)((PBYTE)pSIDCreator + dSIDSize);
	pSIDSystem   = (PSID)((PBYTE)pSIDWorld   + dSIDSize);
	pACL         = (PACL)((PBYTE)pSIDSystem  + dSIDSize);
	//
	// initialize the security identifer authority structures
	//
	*pSIDAdminA = securityNTAuthority;
	*pSIDCreatorA = securityCreatorAuthority;
	*pSIDWorldA = securityWorldAuthority;
	*pSIDSystemA = securityNTAuthority;
	//
	// initialize the security attributes struture
	//
	pSA->nLength = sizeof(SECURITY_ATTRIBUTES);
	pSA->bInheritHandle = FALSE;
	pSA->lpSecurityDescriptor = pSD;
	//
	// initialize the security descriptor
	//
	InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION1);
	//
	// initialize the security identifier for the administrator group
	//
	InitializeSid(pSIDAdmin, pSIDAdminA, 2);
	pSubAuthority = GetSidSubAuthority(pSIDAdmin, 0);
	*pSubAuthority = SECURITY_BUILTIN_DOMAIN_RID;
	pSubAuthority = GetSidSubAuthority(pSIDAdmin, 1);
	*pSubAuthority = DOMAIN_ALIAS_RID_ADMINS;
	//
	// initialize the security identifier for the creator
	//
	InitializeSid(pSIDCreator, pSIDCreatorA, 1);
	pSubAuthority = GetSidSubAuthority(pSIDCreator, 0);
	*pSubAuthority = SECURITY_CREATOR_OWNER_RID;
	//
	// initialize the security identifier for the world
	//
	InitializeSid(pSIDWorld, pSIDWorldA, 1);
	pSubAuthority = GetSidSubAuthority(pSIDWorld, 0);
	*pSubAuthority = SECURITY_WORLD_RID;
	//
	// initialize the security identifier for the system
	//
	InitializeSid(pSIDSystem, pSIDSystemA, 1);
	pSubAuthority = GetSidSubAuthority(pSIDSystem, 0);
	*pSubAuthority = SECURITY_LOCAL_SYSTEM_RID;
	//
	// set the administrator group as the owner in the security descriptor
	//
	SetSecurityDescriptorOwner(pSD, pSIDAdmin, FALSE);
	//
	// Initialize an ACL (access control list)
	// this function could fail if the ACL_BUFFER_SIZE we allocated is not
	// large enough
	//
	if (!InitializeAcl(pACL, ACL_BUFFER_SIZE, ACL_REVISION2)) {
		goto error;
	}
	//
	// Add the ACE's (access control entries) to the ACL
	//
	AddAccessAllowedAce(pACL, ACL_REVISION2, (KEY_ALL_ACCESS), pSIDAdmin);
	AddAccessAllowedAce(pACL, ACL_REVISION2, (KEY_ALL_ACCESS), pSIDCreator);
	AddAccessAllowedAce(pACL, ACL_REVISION2, KEY_READ | globalAccess, pSIDWorld);
	AddAccessAllowedAce(pACL, ACL_REVISION2, (KEY_ALL_ACCESS), pSIDSystem);
	//
	// set the ACL in the security descriptor
	//	
	SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE);

	*ppSecurityAttributes = pSA;

	return(0);

error:
	//
	// error - free resources
	//
	dError = GetLastError();

	if (pMem) {
		free(pMem);
	}
	return(dError);
}


//---------------------------------------------------------------------------
// IsSecuritySupported
//
// Returns TRUE if security is available, else returns FALSE
//---------------------------------------------------------------------------
BOOL CALLBACK IsSecuritySupported(void)
{
	SetLastError(0);

	if (GetSidLengthRequired(1) == 0) {
		if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
			return(FALSE);
		}
	}
	return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		DecrementSharedCount
//
//  Description:	Decrement shared count of the given file.
//						The count is found under 
//						Software\\Microsoft\Windows\CurrentVersion\SharedDLLS.
//						If the count is 0 after reducing, the function delete the keyname.
//
/////////////////////////////////////////////////////////////////////////////
VOID DecrementSharedCount( LPCTSTR lpszFile )
{
	DWORD dwCount = 0;
	DWORD dwSize = 4;

	// If the file name is NULL or empty, exit the function.

	if( ( ! lpszFile ) || ( lpszFile[ 0 ] == '\0' ) )
		return;

	// Check if the key name exists.

	if( GetKeyValue( HKEY_LOCAL_MACHINE, szWinSharedDLLs, lpszFile, ( LPBYTE ) &dwCount, dwSize ) )
	{
		if( dwCount > 0 )
		{
			// Record the shared count to the log file.
				
			UninstLog->Log("Reduce shared DLL count %s, before = %d", lpszFile, dwCount);

			// Decrement the count.

			if( --dwCount == 0 )
			{
				if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, szWinSharedDLLs, lpszFile ) )
					UninstLog->Log(", after = deleted" );
			}
			else
			{
				SetKeyValue( HKEY_LOCAL_MACHINE, szWinSharedDLLs, lpszFile, ( LPBYTE ) &dwCount, REG_DWORD, 4 );

				// Record the shared count to the log file.

				dwSize = 4;
				GetKeyValue( HKEY_LOCAL_MACHINE, szWinSharedDLLs, lpszFile, ( LPBYTE ) &dwCount, dwSize );
				UninstLog->Log(", after = %d", dwCount);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////
//
// Function:		DeleteRegKeyName
//
//	Description:	Remove a specified key name.
//						Return TRUE if successful; FALSE otherwise.
//
/////////////////////////////////////////////////////////////////
BOOL DeleteRegKeyName( HKEY hRootKey, LPCTSTR lpszSubKey, LPCTSTR lpszName )
{
	HKEY hRegKey;
	LONG lResult = ERROR_SUCCESS;

	lResult = RegOpenKeyEx( hRootKey, lpszSubKey, 0, KEY_ALL_ACCESS, &hRegKey );

	if( lResult == ERROR_SUCCESS )
	{
		lResult = RegDeleteValue( hRegKey, lpszName );

		RegCloseKey( hRegKey );
	}

	return( lResult == ERROR_SUCCESS );
}


/////////////////////////////////////////////////////////////////////////////
//
//	 Function:		SetKeyValue
//
//  Description:	Return TRUE if set the value successfully; FALSE otherwise.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SetKeyValue( HKEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszName, LPBYTE lpszValue, DWORD dwType, DWORD dwBufSize )
{
	HKEY hSubKey = NULL;
	LRESULT lResult = ERROR_SUCCESS;

	lResult = RegOpenKeyEx( hKey, lpszSubKey, 0, KEY_SET_VALUE, &hSubKey );

	if ( lResult == ERROR_SUCCESS)
		lResult = RegSetValueEx( hSubKey, lpszName, 0, dwType, ( LPBYTE ) lpszValue, dwBufSize );

	RegCloseKey( hSubKey );

	return( lResult == ERROR_SUCCESS );
}


/////////////////////////////////////////////////////////////////
//
// Function:		RemoveNavReg
//
//	Description:	Remove Nav registry keys.
//
/////////////////////////////////////////////////////////////////
VOID RemoveNavRegKeys() 
{
	TCHAR szBuff[ MAX_PATH ] = { 0 };
	TCHAR szInfo[ MAX_PATH ] = { 0 };
	DWORD dwSize = sizeof( szBuff ) / sizeof( TCHAR );

    // Remove the LiveReg & LiveAdvisor Apps registry key(s) so we don't prevent it from being uninstalled
    if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Shared Technology\\LiveReg\\Apps"), _T( "NAV" ) ) )
		UninstLog->Log("Delete HKLM\\Software\\Symantec\\Shared Technology\\LiveReg\\Apps\\NAV\n");
    if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Shared Technology\\LiveReg\\Apps"), _T( "Norton AntiVirus" ) ) )
		UninstLog->Log("Delete HKLM\\Software\\Symantec\\Shared Technology\\LiveReg\\Apps\\Norton AntiVirus\n");
    if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Shared Technology\\LiveAdvisor\\Apps"), _T( "NAV" ) ) )
		UninstLog->Log("Delete HKLM\\Software\\Symantec\\Shared Technology\\LiveAdvisor\\Apps");

    // Remove the Norton Program Scheduler entry that is installed with NAV 5.0
    if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, szKeyRun, _T( "Norton Program Scheduler Event Checker" ) ) )
		UninstLog->Log("Delete HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\Norton Program Scheduler Event Checker\n");
		
	// Remove installed path values.
	//  Remove all NAVxx keys in case where a 9x machine that has NAV
	//  installed was upgraded to NT.

	if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, szSymAppsPath, _T( "NAV" ) ) )
		
		UninstLog->Log("Delete HKLM\\%s\\NAV\n", szSymAppsPath);

	if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, szSymAppsPath, _T( "NAVNT") ) )
		UninstLog->Log("Delete HKLM\\%s\\NAVNT\n", szSymAppsPath);

	if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, szSymAppsPath, _T( "NAV95") ) )
		UninstLog->Log("Delete HKLM\\%s\\NAV95\n", szSymAppsPath);

	// Remove the "Norton AntiVirus" folder.

	if( DeleteRegKey( HKEY_LOCAL_MACHINE, szNAVAppsKey ) )
		UninstLog->Log("Delete HKLM\\%s", szNAVAppsKey);

	if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, szKeyRun, _T( "NAV Agent" ) ) )
			UninstLog->Log("Delete HKLM\\%s\\NAV Agent\n", szKeyRun);

	if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, szKeyRun, _T( "NAV Cfgwiz" ) ) )
			UninstLog->Log("Delete HKLM\\%s\\NAV Cfgwiz\n", szKeyRun);

	if( DeleteRegKeyName( HKEY_LOCAL_MACHINE, szKeyRun, _T( "BootWarn" ) ) )
			UninstLog->Log("Delete HKLM\\%s\\BootWarn\n", szKeyRun);
}
