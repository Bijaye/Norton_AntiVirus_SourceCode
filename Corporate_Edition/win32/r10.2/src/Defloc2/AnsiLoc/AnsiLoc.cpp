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

    strcat(szNAVInstallLocation, "\\Defloc.dat");

    // write defloc.dat out in ANSI - overwrite if necessary.  This is hard coded for Danish.

    WritePrivateProfileString("DefBaseLocation",
                              "Location",
                              "C:\\PROGRA~1\\F’LLES~1\\SYMANT~1\\VIRUSD~1",
                              szNAVInstallLocation);

    RegCloseKey(hKey);
    return 0;
}
