// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SymClnUp.cpp : Defines the entry point for SymClnUp.
//

#include "stdafx.h"
#include "SymClnUp.h"
#include "vpstrutils.h"


void FuncRemoveDirectoryEx(const TCHAR * FullDirName);


/////////////////////////////////////////////////////////////////////////////
//
// Function: WinMain
//
// Description: Entry point for SymClnUp. This program reads a list of files
//				and directories from the registry and deletes them then
//				deletes itself. It is normally used for uninstallion of a 
//				product to help remove directories that are left behind
//				after files marked for deletion in them have been removed on
//				reboot. Put in the "RunOnce" key for this.
//
// Returns: 0
/////////////////////////////////////////////////////////////////////////////
// 8/1/00 SKENNED :: Function Created
/////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// Is this the Original EXE or the clone EXE?
	// If the command-line 1  argument, this is the Original EXE
	// If the command-line >1 argument, this is the clone EXE
	if (__argc == 1)
	{

		// Original EXE: Spawn clone EXE to delete this EXE

		// Copy this EXEcutable image into the user's temp directory
		TCHAR szPathOrig[_MAX_PATH],szPathClone[_MAX_PATH];
		GetModuleFileName(NULL,szPathOrig,_MAX_PATH);
		GetTempPath(_MAX_PATH,szPathClone);
		GetTempFileName(szPathClone,__TEXT("SCU"),0,szPathClone); 
		CopyFile(szPathOrig,szPathClone,FALSE);

		// Open the clone EXE using FILE_FLAG_DELETE_ON_CLOSE
		HANDLE hfile = CreateFile(szPathClone,0,FILE_SHARE_READ,NULL,OPEN_EXISTING,
			FILE_FLAG_DELETE_ON_CLOSE,NULL);

		// Spawn the clone EXE passing it our EXE's process handle
		// and the full path name to the Original EXE file.
		TCHAR szCmdLine[512];
		HANDLE hProcessOrig = OpenProcess(PROCESS_TERMINATE,TRUE,GetCurrentProcessId());
		vpsnprintf(szCmdLine,sizeof (szCmdLine), __TEXT("\"%s\" %d \"%s\""),szPathClone,hProcessOrig,szPathOrig);

		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		GetStartupInfo(&si);
		
		BOOL bSuccess = CreateProcess(NULL,szCmdLine,NULL,NULL,TRUE,DETACHED_PROCESS,NULL,NULL,&si,&pi);
		Sleep(1000);
		if (hProcessOrig) 
			CloseHandle(hProcessOrig);

		if (hfile) 
			CloseHandle(hfile);

        if (bSuccess)
        {
            CloseHandle (pi.hProcess);
            CloseHandle (pi.hThread);
        }

		// This original process can now terminate.
	} 
	else 
	{
		// Clone EXE: When original EXE terminates, delete it
		HANDLE hProcessOrig = (HANDLE) _ttoi(__targv[1]);
		Sleep(1500);
		WaitForSingleObject(hProcessOrig,INFINITE);
		CloseHandle(hProcessOrig);
		DeleteFile(__targv[2]);
		
		// Insert actions here
		CleanUpFiles();
		CleanUpDirectories();
		CleanUpRegistry();

		// The system will delete the clone EXE automatically 
		// because it was opened with FILE_FLAG_DELETE_ON_CLOSE
	}
 
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function: CleanUpFiles
//
// Description: Read any file names from the SOFTWARE\Symantec\SymClnUp\Files
//				key and delete those files and remove that key from the registry.
//
// Returns: # of entries processed or -1 for error.
/////////////////////////////////////////////////////////////////////////////
// 8/1/00 SKENNED :: Function Created
/////////////////////////////////////////////////////////////////////////////
int CleanUpFiles()
{
	int		iRetVal = 0;
	LONG	lRegRetVal;
	HKEY	hSymClnUpFilesKey;

	// Open the file key
	lRegRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE,SYMCLNUP_FILES_KEY,0,KEY_READ,&hSymClnUpFilesKey);
	if(ERROR_SUCCESS == lRegRetVal)
	{
		DWORD	dwIndex = 0;
		DWORD	dwSize	= MAX_PATH;
		char	szFileName[MAX_PATH];
		
		// Enumerate files to remove
		lRegRetVal = RegEnumValue(hSymClnUpFilesKey,dwIndex,szFileName,&dwSize,0,NULL,NULL,NULL);

		do
		{
			DeleteFile(szFileName);
			++iRetVal;
			++dwIndex;
			dwSize = MAX_PATH;
			lRegRetVal = RegEnumValue(hSymClnUpFilesKey,dwIndex,szFileName,&dwSize,0,NULL,NULL,NULL);
		} while(ERROR_NO_MORE_ITEMS != lRegRetVal);


		// Close and delete key as we are done processing all files
		RegCloseKey(hSymClnUpFilesKey);
		lRegRetVal = RegDeleteKey(HKEY_LOCAL_MACHINE,SYMCLNUP_FILES_KEY);

	}
	else
		iRetVal = -1;

	return iRetVal;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function: CleanUpDirectories
//
// Description: Read any directories from the SOFTWARE\Symantec\SymClnUp\Directories
//				key and remove those directories and delete key from the registry.
//
// Returns: # of entries processed or -1 for error.
/////////////////////////////////////////////////////////////////////////////
// 8/1/00 SKENNED :: Function Created
/////////////////////////////////////////////////////////////////////////////
int CleanUpDirectories()
{
	int		iRetVal = 0;
	LONG	lRegRetVal;
	HKEY	hSymClnUpDirsKey;

	// Open the directory key
	lRegRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE,SYMCLNUP_DIRS_KEY,0,KEY_READ,&hSymClnUpDirsKey);
	if(ERROR_SUCCESS == lRegRetVal)
	{
		DWORD	dwIndex = 0;
		DWORD	dwSize	= MAX_PATH;
		char	szPathName[MAX_PATH];
		
		// Enumerate directories to remove
		lRegRetVal = RegEnumValue(hSymClnUpDirsKey,dwIndex,szPathName,&dwSize,0,NULL,NULL,NULL);
		if(ERROR_SUCCESS == lRegRetVal)
		{			
			do {
				FuncRemoveDirectoryEx(szPathName);
				++iRetVal;
				++dwIndex;
				dwSize = MAX_PATH;
				lRegRetVal = RegEnumValue(hSymClnUpDirsKey,dwIndex,szPathName,&dwSize,0,NULL,NULL,NULL);
			} while(ERROR_NO_MORE_ITEMS != lRegRetVal);
		}

		// Close and delete key as we are done processing all directories
		RegCloseKey(hSymClnUpDirsKey);
		lRegRetVal = RegDeleteKey(HKEY_LOCAL_MACHINE,SYMCLNUP_DIRS_KEY);
	}
	else
		iRetVal = -1;

	return iRetVal;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function: CleanUpRegistry
//
// Description: Removes the SOFTWARE\Symantec\SymClnUp key from the registry
//				and if there are no more keys, also removes SOFTWARE\Symantec
//
// Returns: 0 for success or -1 for error.
/////////////////////////////////////////////////////////////////////////////
// 8/1/00 SKENNED :: Function Created
/////////////////////////////////////////////////////////////////////////////
int CleanUpRegistry()
{
	int		iRetVal = 0;
	char	szPathName[MAX_PATH];
	DWORD	dwSize	= MAX_PATH;
	LONG	lRegRetVal;
	HKEY	hKey;

	// Remove our application's key
	RegDeleteKey(HKEY_LOCAL_MACHINE,SYMCLNUP_KEY);
	
	// Open the Symantec key
	lRegRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE,SYMANTEC_KEY,0,KEY_READ,&hKey);
	if(ERROR_SUCCESS == lRegRetVal)
	{
		// Check to see if there are any sub-keys, if not remove Symantec key
		lRegRetVal = RegEnumKeyEx(hKey,0,szPathName,&dwSize,0,NULL,NULL,NULL);
		RegCloseKey(hKey);
		if(ERROR_NO_MORE_ITEMS == lRegRetVal)
			RegDeleteKey(HKEY_LOCAL_MACHINE,SYMANTEC_KEY);
	}
	else
		iRetVal = -1;

	return iRetVal;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function: SplitPath() -helper function for FuncRemoveDirEx()
//
// Description: Helper function for FuncRemoveDirEx()
//				Gets the children path within parent dirs, 
//				Also checks dirs with '.', if so, pass it back
//
// Returns: 0 for success or -1 for error.-- return NOT used..
/////////////////////////////////////////////////////////////////////////////
// July/14/2002 CWINTERS :: Function Created
/////////////////////////////////////////////////////////////////////////////
BOOL SplitPath(const TCHAR *WideFileName, TCHAR *szReturnPath, TCHAR *szReturnFile, TCHAR *szReturnExtension)
{
	TCHAR  szPath[_MAX_PATH], *pPosition;
    TCHAR* pszFileName;

    ZeroMemory(szPath, _MAX_PATH);

	if (!GetFullPathName(WideFileName,
                             _MAX_PATH,
        //                   sizeof(WideFileName)/sizeof(TCHAR),
                             szPath,
                             &pszFileName) )
		return FALSE;

    //position would be '\\end of string path" here...
	pPosition = strrchr(szPath, '\\');
	if( pPosition != NULL )
	{
      	  *pPosition = '\0';
	
		  if( szReturnPath != NULL )
		  {
            //Since we reverse serach for a character and chopped the rest off fill in path minus the path passed in
            strcpy(szReturnPath, szPath);
           }
	     pPosition++;
	}
	else
		pPosition = (char *) WideFileName;

	strcpy(szPath, pPosition);
    
    //Check to see if the directory has a '.' as a character!
	pPosition = strrchr(szPath, '.');

	if( szReturnExtension )
		if( pPosition )
			strcpy(szReturnExtension, pPosition+1);
		else	strcpy(szReturnExtension, "");

	if( pPosition )
		*pPosition = '\0';
	if( szReturnFile )
	{
		strcpy(szReturnFile, szPath);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//
// Function: FuncRemoveDirectoryEx
//
// Description: Removes directories.
//				Same as RemoveDir() WIN32 API, however, FuncRemoveDirEx will
//				traverse and remove directories from the parent.
//				For example
//				FuncRemoveDirEx("C:\\Test1\\test2\\test3\\test4");
//				This function would remove ..\\..\\..\\test4 first and 
//				work its way down to the parent directory "c:\test1".
//				The parent directory will then be deleted.
//
//				Notes:
//				If 'c:\test1' contained other directories, then FuncRemoveDirEx()
//				WILL NOT eventually remove the parent directory. 
//				It will remove only those directories specified and its children.
//				For example: If c:\test1 has two other directories within that 
//				directory. Then the called directory and its children under it 
//				will be removed, except for the parent, because it has another
//				directory under it.
//				This is provided that other important data *might* be inside 
//				the directory(ies).
//				FuncRemoveDirEx(), when called, the parent and the children 
//				directories *must* be empty. It will not traverse an delete files.
//				FuncRemoveDirEx() will not remove the root directory of a drive.
//				
//	Parameters: 
//
// Returns: none
/////////////////////////////////////////////////////////////////////////////
// July.14.2002 CWINTERS :: Function Created
/////////////////////////////////////////////////////////////////////////////
void FuncRemoveDirectoryEx(const TCHAR * FullDirName)
{
   	TCHAR	Extension[_MAX_PATH],	Filename[2*MAX_PATH];
	TCHAR	szTemp[2* _MAX_PATH];

    //Zero out the vars.
	ZeroMemory(szTemp,		2* _MAX_PATH);
	ZeroMemory(Filename,	2* _MAX_PATH);
	ZeroMemory(Extension,	_MAX_PATH);

    //split the path up and minus the ending path tail.
    //Checks dirs if has dot in them.
	SplitPath(FullDirName, szTemp, Filename, Extension);

	// nothing happened
    //If the final string matches, get out of recursion!
	if(  strcmp(FullDirName, szTemp) == 0 )
       return;
    
    else
       {
          if( strlen(FullDirName) <= 3)
           return;

		  RemoveDirectory(FullDirName);
         //Recurse....
         FuncRemoveDirectoryEx(szTemp);
        }
        //After the strings match, then return out from each recursion.... doing.. nothing...
}



