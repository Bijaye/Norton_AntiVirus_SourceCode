// MemScan.cpp : Defines the entry point for the application.
//

/////////////////////////////////////////////////////////////////
//REVISION HISTORY
//
// 1.1 -- Fixed a Bug that prevented the MemScan Temp folder
//		  From ever begin removed because it was being set as Current
//		  Working Directory. Added code to pop up a message box notifying
//		  virus was found and force a reboot immediately.   CDAVIS
//


#include "stdafx.h"
#include "isutil.h"
#include "resource.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	
	// Check OS version. If NT we bail.
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	
	if (osvi.dwPlatformId  == VER_PLATFORM_WIN32_NT)
	{
		return 1;
	}

	TCHAR szTempFolder[MAX_PATH] = {0};
	TCHAR szVirscan1DAT[MAX_PATH] = {0};
	TCHAR szVirscanINF[MAX_PATH] = {0};
	TCHAR szInstScanDLL[MAX_PATH] = {0};
	TCHAR szOldCurrentDir[MAX_PATH] = {0};
	DWORD dwResult = 0;
	

	// Get the path to the temp folder.
	GetTempPath(MAX_PATH, szTempFolder);
	_tcscat(szTempFolder, MEMORY_SCAN);
	
	
	// Extract resource files to the temp folder.
	ExtractFiles(hInstance, szTempFolder);
	
	wsprintf(szVirscan1DAT, _T("%s\\Virscan1.dat"), szTempFolder);
	wsprintf(szVirscanINF,  _T("%s\\Virscan.inf"), szTempFolder);
	wsprintf(szInstScanDLL, _T("%s\\instscan.dll"), szTempFolder);

	// Set the current directory to the temp folder, so instscan.dll
	// know where to reference its dependency files.
	// But FIRST SAVE THE OLD CURRENT DIRECTORY
	GetCurrentDirectory(MAX_PATH, szOldCurrentDir);
	SetCurrentDirectory(szTempFolder);

	// Load the instscan.dll.
	HINSTANCE hDll = LoadLibrary(szInstScanDLL);

	if (hDll == NULL)
	{
		MessageBox(NULL, "Unable to load the InstScan.dll library.", NULL, MB_OK);
	}

	typedef int (*PFNSCANMEMORY) (char*, char*);

	// Call the ScanMemory exported function to verify if there are any viruses.
	PFNSCANMEMORY pScanMemory = (PFNSCANMEMORY) GetProcAddress(hDll, "ScanMemory");
	if(pScanMemory != NULL)
	{
		if(pScanMemory(szVirscan1DAT, szVirscanINF) ==  TRUE)
		{	
			dwResult = 1;			
		}			
		else
		{
			dwResult = 0;
		}
	}
	else
		MessageBox(NULL, "Unable to GetProcAddress of exported DLL.", NULL, MB_OK);


	// Unload the instscan.dll
	FreeLibrary(hDll);
	
	// Remove the files that we installed.
	if (ISDeleteFile(szTempFolder) == IS_FILE_IN_USE)
	{
		MessageBox(NULL, "Can't Delete: File in Use", NULL, MB_OK);
	} 

	// Remove the directory that we created.
	SetCurrentDirectory(szOldCurrentDir);
	ISRemoveDirectory(szTempFolder);
	

	//if we found a virus we must force a Reboot
	if (dwResult == 0)
	{
		TCHAR szCaption[MAX_PATH*2] = {0};
		TCHAR szTitle[MAX_PATH*2] = {0};
		
		LoadString(hInstance, IDS_NAV_MEMORY_INFECTED, szCaption, sizeof(szCaption));
		LoadString(hInstance, IDS_NAV_MEMORY_INFECTED_TITLE, szTitle, sizeof(szTitle));
		
		MessageBox(NULL, szCaption, szTitle, MB_ICONERROR);
		ExitWindowsEx (EWX_FORCE | EWX_SHUTDOWN, 0);
		ExitProcess(0);
	}			
	
	return dwResult;
}



