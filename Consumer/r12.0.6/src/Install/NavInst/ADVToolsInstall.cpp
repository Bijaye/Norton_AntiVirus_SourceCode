/////////////////////////////////////////////////////////////////////////////////////////////
// ADVToolsInstall.cpp
//
// --Contains exported functions used in MSI custom actions for install NU ADVTools w/ NAVPRO
//
/////////////////////////////////////////////////////////////////////////////////////////////


#include "helper.h"
#include "fileaction.h"

using namespace InstallToolBox;

/////////////////////////////////////////////////////////////////////////////////////////////
// ConfigAdvChkISS(): sets up the Iss for this install
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall ConfigAdvChkISS(MSIHANDLE hInstall)
{

#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug ConfigAdvChkISS()", "Debug Break", NULL);
#endif

	g_Log.Log("ConfigAdvChkISS() Starting.");

	TCHAR szSetupISSFile[MAX_PATH]={0};
	TCHAR szAdvChkISSFile[MAX_PATH]={0};
	TCHAR szInstallPath[MAX_PATH]={0};
	TCHAR szAdvChkInstallPath[MAX_PATH]={0};
	DWORD dwSize = MAX_PATH;
	HKEY hKey;

	// Get the temp directory to copy AdvChk setup.iss file to advchk.iss
	// so we can change the destination path.
	GetTempPath(dwSize, szAdvChkISSFile);
	_tcscat(szAdvChkISSFile, _T("advchk.iss"));

	// Copy over the advchk setup.iss file to the temp directory and rename to advchk.iss.
	// Retrieve the source directory.
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szNAVAppsKey, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		dwSize = MAX_PATH;
		if(RegQueryValueEx(hKey, _T("AdvChkISSPath"), NULL, NULL, (BYTE*)szSetupISSFile, &dwSize) != ERROR_SUCCESS)
		{					
			g_Log.Log ( "ConfigAdvChkISS: Unable to query AdvChkISS path");
		}

		RegCloseKey(hKey); hKey = NULL;
	}
	
	CopyFile(szSetupISSFile, szAdvChkISSFile, FALSE);	
	SetFileAttributes(szAdvChkISSFile, FILE_ATTRIBUTE_NORMAL);
	

	// Retrieve the AdvChk install path.
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSymAppsPath, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		dwSize = MAX_PATH;
		if(RegQueryValueEx(hKey, _T("NAV"), NULL, NULL, (BYTE*)szAdvChkInstallPath, &dwSize) != ERROR_SUCCESS)
		{					
			g_Log.Log ( "ConfigAdvChkISS: Unable to query Install path");
		}

		RegCloseKey(hKey);
	}

	_tcscat(szAdvChkInstallPath, _T("\\AdvTools"));
	

	// Modify the destination path of SdAskDestPath-0 to the AdvChk directory.
	WritePrivateProfileString(_T("SdAskDestPath-0"), _T("szDir"), szAdvChkInstallPath, szAdvChkISSFile);
	
	g_Log.Log("ConfigAdvChkISS() Finsihed.");
	
	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//  InstallAdvChk(): Install AdvTools by launching the Iss file
// 
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall InstallAdvChk(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug InstallAdvChk()", "Debug Break", NULL);
#endif

	g_Log.Log("InstallAdvChk() Starting.");

	TCHAR szAdvChkSourcePath[MAX_PATH]={0};	
	TCHAR szAdvChkISSPath[MAX_PATH]={0};
	TCHAR szCommand[MAX_PATH]={0};
	DWORD dwSize = MAX_PATH;
	HKEY hKey;

	// Retrieve the AdvChk install path.
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szNAVAppsKey, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hKey, _T("AdvChkPath"), NULL, NULL, (BYTE*)szAdvChkSourcePath, &dwSize) != ERROR_SUCCESS)
		{					
			g_Log.Log ( "InstallAdvChkHelp: Unable to query AdvChk install dir");
		}
		
		RegCloseKey(hKey);
	}

		
	// Get the location AdvChk ISS file..
	GetTempPath(dwSize, szAdvChkISSPath);
	_tcscat(szAdvChkISSPath, _T("advchk.iss"));

	//Set up the working records that we'll use for the progressmessage stuff
	PMSIHANDLE hRec = MsiCreateRecord(3);
	
	// Setup the stuff to be displayed.
	TCHAR szString[MAX_PATH] = {0};
	LoadString(ghInstance, IDS_INSTALLING_ADVTOOLS, szString, sizeof(szString));
	MsiRecordSetString(hRec, 1, _T(""));	
	MsiRecordSetString(hRec, 2, szString);
	MsiRecordSetString(hRec, 3, _T(""));
	MsiProcessMessage(hInstall, INSTALLMESSAGE_ACTIONSTART, hRec);

	// Set the install command for AdvChk.
	// "setup.exe -G -s -f1[AdvChkISSPath]"
	
	wsprintf(szCommand, _T("%s -G -s -f1\"%s\""), szAdvChkSourcePath, szAdvChkISSPath);	

	SWLaunchSetupAndWait(szCommand, "", "InstallShield_Win", "Advanced Tools Setup", ghInstance, hInstall, szString);
		
	g_Log.Log("InstallAdvChk() Finished.");

	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//  UninstallAdvChk(): Uninstall AdvTools silently
// 
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall UninstallAdvChk(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug UninstallAdvChk()", "Debug Break", NULL);
#endif

	g_Log.Log("UninstallAdvChk() Starting.");

	TCHAR szAdvChkCmdLine[3*MAX_PATH];	
	DWORD dwRegAdvChkValueSize = sizeof(szAdvChkCmdLine);
	HKEY hKey;
	LONG lResult;

    lResult = RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Advanced Tools"), &hKey);

    if(lResult == ERROR_SUCCESS)
	{
        lResult = RegQueryValueEx ( hKey, "QuietUninstallString", NULL,
                                    NULL, (LPBYTE) szAdvChkCmdLine, &dwRegAdvChkValueSize );
    
        
		if ( ERROR_SUCCESS == lResult )
		{  
			ExecuteProgram(_T(""), szAdvChkCmdLine, TRUE);
		}

        RegCloseKey(hKey);
	}
	
	g_Log.Log("UninstallAdvChk() Finished.");

	return ERROR_SUCCESS;
}