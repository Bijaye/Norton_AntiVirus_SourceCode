// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include <windows.h>
#include "SymSaferRegistry.h"

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
{
    // first get NAV's location

    HKEY  hKey;
    char  szNAVInstallLocation[_MAX_PATH];
    DWORD dwType;
    DWORD dwSize = sizeof(szNAVInstallLocation);

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     "SOFTWARE\\Symantec\\InstalledApps",
                     0,
                     KEY_READ,
                     &hKey) != ERROR_SUCCESS)
	{
		return 1;
	}

    if (SymSaferRegQueryValueEx(hKey,
                        "NAV95",
                        NULL,
                        &dwType,
                        (LPBYTE)szNAVInstallLocation,
                        &dwSize) != ERROR_SUCCESS)
	{
	    RegCloseKey(hKey);
        return 1;
	}

    // now see if Defloc.dat exists and exit if so

	OFSTRUCT FileInfo;

    strcat(szNAVInstallLocation, "\\Defloc.dat");

	if (OpenFile(szNAVInstallLocation, &FileInfo, OF_EXIST) != HFILE_ERROR)
	{
        RegCloseKey(hKey);
	    return 0;
	}

    // okay, the file does not exist see we need to create it

    char szDefsLocation[_MAX_PATH];
    char szOEMDir[_MAX_PATH];

    dwSize = sizeof(szDefsLocation);

    if (SymSaferRegQueryValueEx(hKey,
                        "AVENGEDEFS",
                        NULL,
                        &dwType,
                        (LPBYTE)szDefsLocation,
                        &dwSize) != ERROR_SUCCESS)
	{
	    RegCloseKey(hKey);
        return 1;
	}

    CharToOem(szDefsLocation, szOEMDir);  

    WritePrivateProfileString("DefBaseLocation", 
                              "Location",
                              szOEMDir, 
                              szNAVInstallLocation);
    
    RegCloseKey(hKey);
    return 0;
}
