////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// helper.cpp: 
//
// --implementation for all non-exported helper functions			
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include "helper.h"
#include "InstOptsNames.h"
#include "OSInfo.h"

#include "Quar32.h"

#include "fileaction.h"

using namespace InstallToolBox;

/////////////////////////////////////////////////////////////////////////////////////////////
// RegGetProgramFilesDir(): retrieve windows program files directory 
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL RegGetProgramFilesDir(LPTSTR pszPath, UINT cbSize)
{
   HKEY hKey = NULL;
   LONG lResult = 0L;
   
   // get CurrentVersion location
   lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, KEY_READ | KEY_WRITE, &hKey);
   if (lResult == ERROR_SUCCESS)
   {
	    DWORD dwSize = cbSize;
		lResult = RegQueryValueEx(hKey, REGVAL_PROGRAMFILESDIR, NULL, NULL, LPBYTE(pszPath), &dwSize);
		RegCloseKey(hKey);
   }

   // manufacture if not good to here
   if (lResult != ERROR_SUCCESS)
   {
      // manufacture program files location
      TCHAR szDirName[MAX_PATH] = {0};
      GetWindowsDirectory(szDirName, sizeof(szDirName)); //lint !e534
      _tcscpy(szDirName + 3, DEFAULT_PROGRAMFILES);

      // add key by open/create
      if (CreateDirectory(szDirName, NULL))
      {
         lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, NULL, NULL, KEY_READ | KEY_WRITE, NULL, &hKey, NULL);
         if (lResult == ERROR_SUCCESS)
         {     
            lResult = RegSetValueEx(hKey, REGVAL_PROGRAMFILESDIR, 0L, REG_SZ, LPBYTE(szDirName), (DWORD(lstrlen(szDirName)) + 1UL) * sizeof(TCHAR));
            RegCloseKey(hKey);
         };
      };
   };

   return (lResult == ERROR_SUCCESS);
};


/////////////////////////////////////////////////////////////////////////////////////////////
// RegGetCommonFilesDir(): retrieve windows common files directory 
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL RegGetCommonFilesDir(LPTSTR pszPath, UINT cbSize)
{
   HKEY hKey = NULL;
   LONG lResult = 0L;

   // get existing Common Files location
   lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, KEY_READ | KEY_WRITE, &hKey);
   if (lResult == ERROR_SUCCESS)
   {
	  DWORD dwSize = cbSize;
      lResult = RegQueryValueEx(hKey, REGVAL_COMMONFILESDIR, NULL, NULL, LPBYTE(pszPath), &dwSize);
      RegCloseKey(hKey);
   }

   // manufactur if not good to here
   if (lResult != ERROR_SUCCESS)
   {
      TCHAR szDirName[ MAX_PATH ] = {0};
      if (RegGetProgramFilesDir(szDirName, sizeof(szDirName)))
      {
         // manufacture program files location
         _tcscat(szDirName, _T("\\") DEFAULT_COMMONFILES);
         if (CreateDirectory(szDirName, NULL))
         {
            lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, NULL, NULL, KEY_READ | KEY_WRITE, NULL, &hKey, NULL);
            if (lResult == ERROR_SUCCESS)
            {    
               lResult = RegSetValueEx(hKey, REGVAL_COMMONFILESDIR, 0L, REG_SZ, LPBYTE(szDirName), (lstrlen(szDirName) + 1) * sizeof(TCHAR));
               RegCloseKey(hKey);
            };
         };
      };
   };

   return (lResult == ERROR_SUCCESS);
};


/////////////////////////////////////////////////////////////////////////////////////////////
// DirectoryExists():  Function to determine if a given directoryexists
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL DirectoryExists(LPTSTR szDir)
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    WIN32_FIND_DATA     fd;

    hFind = FindFirstFile(szDir, &fd);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            bRet = TRUE;

        FindClose(hFind);
    }

    return(bRet);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// WndEnumProc():  It is used to know when InstallShield install are done. (from NSW team)
//
/////////////////////////////////////////////////////////////////////////////////////////////
struct LaunchEnumData
{
   LPCTSTR pcszClassName;     // window class
   LPCTSTR pcszWindowName;    // window title
   DWORD dwProcessId;         // process id to ignore
   DWORD dwSetupId;           // remote process id
   HWND hwnd;                 // resulting window handle
};

// window enumeration to check for matching window with different process id
BOOL CALLBACK WndEnumProc(HWND hwnd, LPARAM lParam)
{
   BOOL fContinue = TRUE;

   // quick exit on non-valid pointer reference
   if (!lParam)
   { 
	   return FALSE;
	}

   LaunchEnumData* pData = reinterpret_cast < LaunchEnumData* > (lParam);

   // get incoming window info
   DWORD dwWndProcessId; 
   GetWindowThreadProcessId( hwnd, &dwWndProcessId ); //lint !e534

   TCHAR szClassName[64] = _T(""); 
   GetClassName(hwnd, szClassName, sizeof(szClassName)); //lint !e534

   TCHAR szWindowName[64] = _T("");
   GetWindowText(hwnd, szWindowName, sizeof(szWindowName)); //lint !e534
   
   // if all match correctly then save and leave
   if ((dwWndProcessId != pData->dwProcessId) &&
       (0 == lstrcmpi(szClassName, pData->pcszClassName)) &&
       (0 == lstrcmpi(szWindowName, pData->pcszWindowName)))
   {
      pData->hwnd = hwnd;
      pData->dwSetupId = dwWndProcessId; 
      fContinue = FALSE;
   };

   return fContinue;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// SWLaunchSetupAndWait():  Runs a command with params, then waits for window death
//
/////////////////////////////////////////////////////////////////////////////////////////////
LONG SWLaunchSetupAndWait(LPCTSTR pszSetup, LPCTSTR pszIssFile, LPCTSTR pszClass, LPCTSTR pszTitle, HINSTANCE hInstance, MSIHANDLE hInstall, LPCTSTR pszDisplayText)
{
	//	INPUT:	 
	//  LPCTSTR pszSetup,       // command-line executable
	//  LPCTSTR pszIssFile,     // command-line switches
	//  LPCTSTR pszClass,       // window class name
	//  LPCTSTR pszTitle        // window title
	
   LONG lResult = 0L;

   // build command line
   if (pszSetup)
   {
      TCHAR szCmdLine[MAX_PATH * 2];
      lstrcpy(szCmdLine, pszSetup);
      if (pszIssFile && *pszIssFile)
      {
         lstrcat(szCmdLine, _T(" -s -f1\""));
         lstrcat(szCmdLine, pszIssFile);
         lstrcat(szCmdLine, _T("\""));
      };

      // launch initial setup and wait for completion
      STARTUPINFO si;
      PROCESS_INFORMATION pi;
      ZeroMemory(&pi, sizeof(pi));
      ZeroMemory(&si, sizeof(si));
      si.cb = sizeof(si);
      if (CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
      {
         for (;;)
         {  // check for termination of the 16-bit proxy
            if (WaitForSingleObject(pi.hProcess, 100) != WAIT_ABANDONED)			
               break;

            // pump messages
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
               TranslateMessage(&msg);
               DispatchMessage(&msg);
            };
         }

         // check return code of process if it existed
         // release process handle et'al
         CloseHandle(pi.hThread);
         CloseHandle(pi.hProcess);
      }
      else
      {  // unable to create 16-bit thunk process, abort install
         lResult = LONG(GetLastError());
         return lResult;
      };
   };

   // build enumeration data
   LaunchEnumData data = { (pszClass && lstrlen(pszClass)) ? pszClass : _T("InstallShield_Win"),
                           (pszTitle && lstrlen(pszTitle)) ? pszTitle: _T("Setup"),
                           GetCurrentProcessId(), NULL }; //lint !e785

   // loop for 1 minute seconds max
   DWORD dwEndCount = GetTickCount() + 120000;
   #ifdef HARD_ABORT
   while (!gfAbortInstall && (GetTickCount() < dwEndCount))
   #else
   while (GetTickCount() < dwEndCount)
   #endif
   {  
      // find the first window that is not ourself and is a setup
      EnumWindows(WndEnumProc, LPARAM(&data));
      if (data.hwnd != NULL)
      {
         
		  // Set the text in the installer to now use File: [1] instead of Initializing...
		  PMSIHANDLE hRec = MsiCreateRecord(3);   
		  MsiRecordSetString(hRec, 1, pszDisplayText);		  
		  MsiRecordSetString(hRec, 2, pszDisplayText);		  		  
		  MsiRecordSetString(hRec, 3, _T(""));		  
		  MsiProcessMessage(hInstall, INSTALLMESSAGE_ACTIONSTART, hRec);
		 
		 //Set our hook so we can process their messages for our status dialog
         DWORD dwMyThreadID = GetWindowThreadProcessId(data.hwnd, NULL);
		 
		 // get window handle process
		 HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, data.dwSetupId);
         if (hProcess != NULL)
         {
            do
            {  // pump messages
               MSG msg;
               while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
               {
                  TranslateMessage(&msg);
                  DispatchMessage(&msg);
               };
               Sleep(10);

               // loop on this process while still active
               if (!GetExitCodeProcess(hProcess, LPDWORD(&lResult)))
               {   
				   break;   // error
			   }

               // leave now if process should be terminated
               #ifdef HARD_ABORT
               if (gfAbortInstall)
                  break;
               #endif
            }
            while (lResult == LONG(STILL_ACTIVE));

            // wait on the process object for three more seconds
            WaitForSingleObject(hProcess, 3000);  //lint !e534
            CloseHandle(hProcess);

            #ifdef _DEBUG
            MessageBeep(MB_ICONASTERISK);
            #endif
         }
         else
         {  // haneous error reporting
            TCHAR szErr[255] = _T("");
            TCHAR szFmt[40] = _T("");
            //&^LoadString(ghInstance, IDS_ERROR_NOPROCESS, szFmt, sizeof(szFmt) - sizeof(TCHAR));
            wsprintf(szErr, szFmt, GetLastError());            
            Sleep(3000);   // same 3-second wait at tail end
            lResult = -1;
         };

         // leave outer do-while
         break;
      }
      else
      {  // window not present yet, spin messages and sleep.
         MSG msg;
         while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         };
         Sleep(10);
      };
   };

   return lResult;
};

/////////////////////////////////////////////////////////////////////////////////////////////
// RemoveLne() -- This function removes all lines in [filename] that contain the string
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
/////////////////////////////////////////////////////////////////////////////////////////////
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
	LPTSTR	pSubStr = NULL;
	
	// Create a list of the seperating characters
	TCHAR seps[] = _T("\n");
	
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
        CloseHandle(hFile);
		return FALSE;
	}
	
	// Create a little extra space since it will add a '\n'
	// If the EOF occurs at the end of a line.
	NewFile = new BYTE[dwOldFileSize+10];
	if(NULL==NewFile)
	{
		delete[] OldFile;
        CloseHandle(hFile);
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
		LPTSTR pLine = NULL;
		
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
			LPTSTR pCompare = _tcsdup(pLine);
			
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
			LPTSTR pSearch = _tcsstr(pCompare,pSubStr);
			if( pSearch == NULL )
			{
				// Append the string to the new file.
				_tcscat( (LPTSTR)NewFile, pLine);
				_tcscat( (LPTSTR)NewFile, _T("\n"));
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


/////////////////////////////////////////////////////////////////////////////////////////////
// DeleteFilesInDirectory(): Function that will remove a directory or queue it for deletion after rebooting
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL DeleteFilesInDirectory(LPCTSTR lpDirectory)
{
    HANDLE hFind;
	TCHAR szDirectory[MAX_PATH * 2] = {0};
    WIN32_FIND_DATA fd;

    SetFileAttributes(lpDirectory, FILE_ATTRIBUTE_NORMAL);

	_tcscpy(szDirectory, lpDirectory);
	_tcscat(szDirectory, _T("\\"));
	_tcscat(szDirectory, _T("*.*"));

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
				_tcscat(szFile, _T("\\"));
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

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// RemoveTargetFiles() -- This function removes files from a target directory.  The input array
//  of filenames can contain wildcard characters.
//
//  Input:
//      pszTargetDir    -- directory where the target files live
//      filenameArray   -- an array of null-terminated strings (the last
//                         entry must contain a null string)
//
//  Output:
//      the number of files deleted (or marked for deletion)
//
/////////////////////////////////////////////////////////////////////////////////////////////
int RemoveTargetFiles(TCHAR* pszTargetDir, TCHAR** filenameArray)
{
    
    HANDLE fileHandle;
    int numDeletedFiles = 0;
    TCHAR szBuf[MAX_PATH];
    WIN32_FIND_DATA fileData;

    
    // loop and process each string in the array
    while (NULL != *filenameArray)
    {
	    // assemble the file specification
        _tcscpy(szBuf, pszTargetDir);
        _tcscat(szBuf, _T("\\"));
        _tcscat(szBuf, *filenameArray);

        // see if we get a hit
        fileHandle = FindFirstFile(szBuf, &fileData);
        if (INVALID_HANDLE_VALUE != fileHandle)
        {
            do  
			{
                // if we have a valid file, delete it
                if (TRUE == ValidFileTarget(fileData.dwFileAttributes))
                {
                    // construct a string specific to the current filename
                    _tcscpy(szBuf, pszTargetDir);
                    _tcscat(szBuf, _T("\\"));
                    _tcscat(szBuf, fileData.cFileName);

                
					if (DeleteFile(szBuf) == FALSE)
					{
						// DeleteFile() failed, try to delete the file again using
						// FileInUseHandler().
						FileInUseHandler(szBuf);
					}

					++numDeletedFiles;
				}

             // try to get another match from this string
             }  while (TRUE == FindNextFile(fileHandle, &fileData));
		}

        ++filenameArray;
	}

     return (numDeletedFiles);

}  // end of "RemoveTargetFiles"



/////////////////////////////////////////////////////////////////////////////////////////////
// ValidFileTarget():
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL ValidFileTarget(DWORD fileAttributes)
{
    BOOL bValidFile;

    if  ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (fileAttributes & FILE_ATTRIBUTE_READONLY))
	{
		bValidFile = FALSE;
	}
    else
	{
		bValidFile = TRUE;
	}

    return (bValidFile);

}   

/////////////////////////////////////////////////////////////////////////////////////////////
// strstri():
//
/////////////////////////////////////////////////////////////////////////////////////////////
LPTSTR strstri(LPTSTR string, LPCTSTR strToFind)
{
	while(*string != _T('\0'))
	{
		if(_tcsncicmp(string, strToFind, lstrlen(strToFind)) == 0)
			return string;

		++string;
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// StrReplace():
//
/////////////////////////////////////////////////////////////////////////////////////////////
int StrReplace(LPTSTR* ppszString, LPCTSTR lpszOld, LPCTSTR lpszNew, BOOL bCaseSensitive)
{
	int nSourceLen = lstrlen(lpszOld);
	if(nSourceLen == 0)
		return 0;

	int nReplacementLen = lstrlen(lpszNew);

	LPTSTR pszStr = *ppszString;
	int nStrLength = lstrlen(pszStr) + 1;

	// loop once to figure out the size of the result string
	int nCount = 0;
	LPTSTR lpszStart = pszStr;
	LPTSTR lpszEnd = pszStr + nStrLength;
	LPTSTR lpszTarget = NULL;

	while(lpszStart < lpszEnd)
	{
		lpszTarget = bCaseSensitive ? _tcsstr(lpszStart, lpszOld) : strstri(lpszStart, lpszOld);
		while(lpszTarget != NULL)
		{
			++nCount;
			lpszStart = lpszTarget + nSourceLen;
			lpszTarget = bCaseSensitive ? _tcsstr(lpszStart, lpszOld) : strstri(lpszStart, lpszOld);
		}

		lpszStart += lstrlen(lpszStart) + 1;
	}

	// if any changes were made, make them
	if(nCount > 0)
	{
		// if the buffer is too small, just
		//   allocate a new buffer (slow but sure)
		LPTSTR pszOut = NULL;
		int nOldLength = nStrLength;
		int nNewLength =  nOldLength + (nReplacementLen - nSourceLen) * nCount + 1;
		if(nStrLength < nNewLength)
		{
			LPTSTR pszNewStr = new TCHAR[nNewLength];
			memcpy(pszNewStr, pszStr, nStrLength * sizeof(TCHAR));
			delete[] pszStr;
			pszStr = pszNewStr;
		}

		// else, we just do it in-place
		lpszStart = pszStr;
		lpszEnd = pszStr + nStrLength;

		// loop again to actually do the work
		while(lpszStart < lpszEnd)
		{
			lpszTarget = bCaseSensitive ? _tcsstr(lpszStart, lpszOld) : strstri(lpszStart, lpszOld);
			while(lpszTarget != NULL)
			{
				int nBalance = nOldLength - (lpszTarget - pszStr + nSourceLen);
				memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen, nBalance * sizeof(TCHAR));
				memcpy(lpszTarget, lpszNew, nReplacementLen * sizeof(TCHAR));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
				lpszTarget = bCaseSensitive ? _tcsstr(lpszStart, lpszOld) : strstri(lpszStart, lpszOld);
			}
			lpszStart += lstrlen(lpszStart) + 1;
		}
	}

	*ppszString = pszStr;

	return nCount;
}

// Function to put quotes around the full path to the service binary.
BOOL QuoteServiceBinaryPath(LPCTSTR lpszService)
{
	SC_LOCK sclLock; 
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL;
	DWORD dwBufSize=0, dwBytesNeeded=0;
	TCHAR szNewBinaryPath[MAX_PATH] = {0};

	schSCManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_LOCK);

	if (schSCManager == NULL)
		return FALSE;
	
    // Need to acquire database lock before reconfiguring. 
	
    sclLock = LockServiceDatabase(schSCManager); 
	
    if (sclLock == NULL) 
    { 
    	// Close the handle to the service. 
		CloseServiceHandle(schSCManager); 
		return FALSE;
    } 
	
    // The database is locked, so it is safe to make changes. 
	
    // Open a handle to the service. 
	
    schService = OpenService( 
        schSCManager,           // SCManager database 
        lpszService,           // name of service 
        SERVICE_CHANGE_CONFIG | SERVICE_QUERY_CONFIG); // need CHANGE access 
    if (schService == NULL) 
	{
		// Release the database lock. 
		
		UnlockServiceDatabase(sclLock); 
		
		// Close the handle to the service. 
		
		CloseServiceHandle(schSCManager); 
		
		return FALSE;
	}

    // Query the service to get the binary path.  First call is with 0 BufSize.
	// This call is to get the size needed for proper allocation.
	if (!QueryServiceConfig(schService, lpServiceConfig, dwBufSize, &dwBytesNeeded))
	{
		switch(GetLastError())
		{
			// If access denied or invalid handle, we fail.
		case ERROR_ACCESS_DENIED:
		case ERROR_INVALID_HANDLE:
			// Release the database lock. 
			
			UnlockServiceDatabase(sclLock); 
			
			// Close the handle to the service. 
			
			CloseServiceHandle(schSCManager); 
			
			return FALSE;
			
			// We expect insufficient buffer.  We'll now allocate proper space and call again.
		case ERROR_INSUFFICIENT_BUFFER:
			lpServiceConfig = (LPQUERY_SERVICE_CONFIG) malloc(dwBytesNeeded);
			if (lpServiceConfig != NULL)
				dwBufSize = dwBytesNeeded;
			else
				dwBufSize = 0;
			
			// Make the call again.
			if (!QueryServiceConfig(schService, lpServiceConfig, dwBufSize, &dwBytesNeeded))
			{
				switch(GetLastError())
				{
					// Any errors now and we fail.
				case ERROR_ACCESS_DENIED:
				case ERROR_INVALID_HANDLE:
				case ERROR_INSUFFICIENT_BUFFER:
				default:
					free(lpServiceConfig);
					// Release the database lock. 
					
					UnlockServiceDatabase(sclLock); 
					
					// Close the handle to the service. 
					
					CloseServiceHandle(schSCManager); 
					
					return FALSE;
					
				}
			}
			break;
		}
	}
	
	// Add quotes if the path does not already start with one.
	if (lpServiceConfig->lpBinaryPathName[0] != '\"')
		wsprintf(szNewBinaryPath, _T("\"%s\""), lpServiceConfig->lpBinaryPathName);
	else
	{
		// If the path already starts with a quote, we don't need to do anything.
		
		free(lpServiceConfig);
		// Release the database lock. 
		
		UnlockServiceDatabase(sclLock); 
		
		// Close the handle to the service. 
		
		CloseServiceHandle(schSCManager); 
		
		return TRUE;
	}

	
    // Make the changes. 
	
    if (! ChangeServiceConfig( 
        schService,        // handle of service 
        SERVICE_NO_CHANGE, // service type: no change 
        SERVICE_NO_CHANGE, // change service start type 
        SERVICE_NO_CHANGE, // error control: no change 
        szNewBinaryPath,         // binary path: no change 
        NULL,              // load order group: no change 
        NULL,              // tag ID: no change 
        NULL,              // dependencies: no change 
        NULL,              // account name: no change 
        NULL,              // password: no change 
        NULL) )            // display name: no change
    {
		free(lpServiceConfig);
		// Release the database lock. 
		
		UnlockServiceDatabase(sclLock); 
		
		// Close the handle to the service. 
		
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);
        return FALSE;
    }
	
	free(lpServiceConfig);
    // Release the database lock. 
	
    UnlockServiceDatabase(sclLock); 
	
    // Close the handle to the service. 
	
    CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
	
	return TRUE;

}

// Changes the startup of a service to Manual.
BOOL SetServiceManual( LPCTSTR lpszServiceName)
{
	SC_LOCK sclLock; 
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	
	schSCManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_LOCK);

	if (schSCManager == NULL)
		return FALSE;
	
    // Need to acquire database lock before reconfiguring. 
	
    sclLock = LockServiceDatabase(schSCManager); 
	
    if (sclLock == NULL) 
    { 
    	// Close the handle to the service. 
		CloseServiceHandle(schSCManager); 
		return FALSE;
    } 
	
    // The database is locked, so it is safe to make changes. 
	
    // Open a handle to the service. 
	
    schService = OpenService( 
        schSCManager,           // SCManager database 
        lpszServiceName,           // name of service 
        SERVICE_CHANGE_CONFIG); // need CHANGE access 
    if (schService == NULL) 
	{
		// Release the database lock. 
		
		UnlockServiceDatabase(sclLock); 
		
		// Close the handle to the service. 
		
		CloseServiceHandle(schSCManager); 
		
		return FALSE;
	}
	
    
	
    // Make the changes. 
	
    if (! ChangeServiceConfig( 
        schService,        // handle of service 
        SERVICE_NO_CHANGE, // service type: no change 
        SERVICE_DEMAND_START,       // change service start type 
        SERVICE_NO_CHANGE, // error control: no change 
        NULL,              // binary path: no change 
        NULL,              // load order group: no change 
        NULL,              // tag ID: no change 
        NULL,              // dependencies: no change 
        NULL,              // account name: no change 
        NULL,              // password: no change 
        NULL) )            // display name: no change
    {
		// Release the database lock. 
		
		UnlockServiceDatabase(sclLock); 
		
		// Close the handle to the service. 
		
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);
        return FALSE;
    }
	
    // Release the database lock. 
	
    UnlockServiceDatabase(sclLock); 
	
    // Close the handle to the service. 
	
    CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
	
	return TRUE;
}