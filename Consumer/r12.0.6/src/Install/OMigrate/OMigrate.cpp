// OMigrate.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "COptionsMigration.h"

const TCHAR szSymAppsPath[] = _T("SOFTWARE\\Symantec\\InstalledApps");

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	HKEY hKey = NULL;
	TCHAR szCurDir[MAX_PATH] = {0};
	TCHAR szNavDir[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szNavDir);

	//The options library DELAYLOAD on ccTrust will fail unless the working dir is the NAV dir
	//so save the current working dir and restore it later
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSymAppsPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKey, _T("NAV"), NULL, NULL, (BYTE*)szNavDir, &dwSize);
		GetCurrentDirectory(MAX_PATH, szCurDir);
		SetCurrentDirectory(szNavDir);
		RegCloseKey(hKey);
	}

	g_Log.Log("SetNewOptions() Starting.");

	COptionsMigration Migration;
	Migration.Initialize();
	Migration.Migrate();

	//restore the current working dir
	SetCurrentDirectory(szCurDir);

	g_Log.Log("SetNewOptions() Finished.");

	return ERROR_SUCCESS;

	return 0;
}



