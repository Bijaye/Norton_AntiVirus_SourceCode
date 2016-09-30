// CreateOEMDesktopFolder.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
// Private, global headers.
#include <shellapi.h>
//#include "navstart.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <share.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"



void CreateOEMDesktopFolder(LPCTSTR lpszDesktopDir)
{
	DWORD dwResult = 0;
	HKEY hKey = NULL;
	DWORD dwRegOptions = KEY_READ;
	TCHAR szInstallDir[1024];
	TCHAR szSCFDir[1024];
	DWORD dwSize = 1024;
	CreateDirectory(lpszDesktopDir, NULL);

	CoInitialize(NULL);
	
	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion", NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = RegQueryValueEx(hKey, "Home Directory", NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegCloseKey(hKey);
		if (dwResult != ERROR_SUCCESS)
		{
			dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion\\DomainData", NULL, dwRegOptions, &hKey);
			if (dwResult == ERROR_SUCCESS)
			{
				dwSize = 1024;
				dwResult = RegQueryValueEx(hKey, "Home Directory", NULL, NULL, LPBYTE(szInstallDir), &dwSize);
				RegCloseKey(hKey);
			}
		}
		if (dwResult == ERROR_SUCCESS)
		{
			TCHAR szFile[1024];
			TCHAR szLink[1024];
			_stprintf(szFile, "%svpc32.exe", szInstallDir);
			_stprintf(szLink, "%s\\Symantec AntiVirus.lnk", lpszDesktopDir);
			CUtils::CreateShortcut("Symantec Antivirus", szFile, szLink, szFile, 0, NULL);
		}
				
	}
	
	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\InstalledApps", NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
		dwSize = 1024;
        dwResult = RegQueryValueEx(hKey, "Common Client", NULL, NULL, LPBYTE(szInstallDir), &dwSize);
		dwSize = 1024;
		dwResult = RegQueryValueEx(hKey, _T("Internet Security"), NULL, NULL, LPBYTE(szSCFDir), &dwSize);
        RegCloseKey(hKey);
		
		if (dwResult == ERROR_SUCCESS)
		{
			TCHAR szFile[1024];
			TCHAR szLink[1024];
			TCHAR szIconFile[1024];
			TCHAR szArg[1024];
			_stprintf(szFile, "%sNMain.exe", szInstallDir);
			_stprintf(szLink, "%s\\Symantec Client Firewall.lnk", lpszDesktopDir);
			_stprintf(szIconFile, "%s\\FIO.exe", szSCFDir);
			_stprintf(szArg, "/nosysworks /dat:%s\\NISPLUG.NSI", szSCFDir);
			CUtils::CreateShortcut("Symantec Client Firewall", szFile, szLink, szIconFile, 0, szArg);
		}
	
	}
	
	CoUninitialize();
}
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	TCHAR lpszDesktopDir[1024];
	TCHAR cRootDir = 'c';

	LPSHELLFOLDER psfDesk;
	HRESULT hr = SHGetDesktopFolder(&psfDesk);
    if (SUCCEEDED(hr)) 
	{
		//
		// The User's Desktop Folder is the most accurate to use.
		// However, since an OEM vendor is installing in the factory, we need to 
		// put it in an "All Users" desktop
		//
		TCHAR szDesktopFolder[1024];
		LPITEMIDLIST pidl;
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_DESKTOPDIRECTORY , &pidl)))
		{
			if (SUCCEEDED(SHGetPathFromIDList(pidl, szDesktopFolder)))
			{
				_stprintf (lpszDesktopDir, "%s\\Symantec Client Security", szDesktopFolder);
				CreateOEMDesktopFolder(lpszDesktopDir);
				SHChangeNotify( SHCNE_UPDATEDIR, SHCNF_IDLIST, pidl, NULL);
				SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, 0, SMTO_ABORTIFHUNG, 100000, NULL);


				LPMALLOC pMalloc;
				if (SUCCEEDED(SHGetMalloc(&pMalloc))) 
				{
					pMalloc->Free(pidl);
					pMalloc->Release();
				}

			}
		}
		psfDesk->Release();
	}


	return 0;
}

