// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// OemCA.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "OemCA.h"
#include "Resource.h"

// Added for OEM below
#define MAX_TRIAL_LEN_DAYS	90
#include "oem.h"
#include "oem_common.h"
#include "oem_constants.h"
#include "regstr.h"
#include "EncryptedFileGenerator.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

#define TRIALWARE_SWITCH	_T("Trialware")
#define BOOTCOUNT_SWITCH	_T("BootCount")
#define ICON_SWITCH			_T("Icon")
#define ENABLEAUTOPROTECT_SWITCH _T("EnableAutoProtect")
#define OEM_INSTALL_SWITCH	_T("OEM")
// MCLEE 4/7/2005, 2:18pm
#define FACTORY_MODE_SWITCH	_T("FACTORYMODE")
#define TRIALAPP_SWITCH _T("TrialApp")
#define TRIALLEN_SWITCH _T("TrialLen")
#define REG_KEY_SWITCH _T("RegKey")

#define CONFIGFILE_SWITCH _T("ConfigFile")
#define INSTALL_SOURCE_DIR _T("InstallSourceDir")

#define KEY_WOW64_64KEY					(0x0100)
#define DEFAULT_COMMONFILES				_T("Common Files")
#define DEFAULT_PROGRAMFILES			_T("Program Files")
#define DEFAULT_COMMONFILESDIR			DEFAULT_PROGRAMFILESDIR _T("\\") DEFAULT_COMMONFILES
#define DEFAULT_SYMANTEC_SHARED			_T("Symantec Shared")

#if 0
BOOL APIENTRY DllMain( HANDLE /*hModule*/, DWORD /*dwReason*/, LPVOID /*lpReserved*/ )
{
//	switch( dwReason )
//	{
//	case DLL_PROCESS_ATTACH:
//	case DLL_THREAD_ATTACH:
//	case DLL_THREAD_DETACH:
//	case DLL_PROCESS_DETACH:
//		break;
//	}
	return TRUE;
}
#endif

// ---------------------------------------------------------------------------
// Routine:	SetTempInstallSetting
//
// Purpose:	Set temporary OEM settings
//
// ---------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Added function needed to store temporary OEM settings

void SetTempInstallSetting (TCHAR *sName, DWORD dValue)
{	
  	HKEY		hKey = NULL;
  	DWORD		dwDisp = 0;

  	if( ERROR_SUCCESS == RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
  											SYMANTEC_OEM_KEY,
  											NULL,
  											NULL,
  											REG_OPTION_NON_VOLATILE,
  											KEY_WRITE,
  											NULL,
  											&hKey,
  											&dwDisp) )
  	{
  		RegSetValueEx(	hKey,
  						sName,
  						0,
  						REG_DWORD,
  						(BYTE*)&dValue,
  						sizeof(dValue));


  		RegCloseKey( hKey );
  	}
}

// ---------------------------------------------------------------------------
// Routine:	GetTempInstallSetting
//
// Purpose:	Get temporary OEM settings
//
// ---------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Added function needed to retrieve temporary OEM settings

DWORD GetTempInstallSetting (TCHAR *sName, DWORD dDefault, BOOL bDelete = TRUE)
{
	DWORD lResult = 0;
	HKEY hKey = NULL;
	DWORD regOptions = KEY_READ | KEY_SET_VALUE;
	DWORD dValue = dDefault;

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYMANTEC_OEM_KEY, NULL, regOptions, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
		DWORD dwSize = sizeof (DWORD);
        lResult = SymSaferRegQueryValueEx(hKey, sName, NULL, NULL, LPBYTE(&dValue), &dwSize);

		if (lResult == ERROR_SUCCESS)
		{
			if (bDelete)
				RegDeleteValue(hKey, sName);
		}
		else dValue = dDefault;

        RegCloseKey(hKey);
	}

	return dValue;

}

// ---------------------------------------------------------------------------
// Routine:	SetTempInstallSettingStr
//
// Purpose:	Set temporary OEM settings
//
// ---------------------------------------------------------------------------
// 04/22/05 Michael Lee --- Added function needed to store temporary OEM settings

 void SetTempInstallSettingStr (TCHAR *sName, LPTSTR sValue)
 {	
   	HKEY		hKey = NULL;
   	DWORD		dwDisp = 0;
 
   	if( ERROR_SUCCESS == RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
   											SYMANTEC_OEM_KEY,
   											NULL,
   											_T(""),
   											REG_OPTION_NON_VOLATILE,
   											KEY_WRITE,
   											NULL,
   											&hKey,
   											&dwDisp) )
   	{
   		RegSetValueEx(	hKey,
   						sName,
   						0,
   						REG_SZ,
   						(BYTE*)sValue,
   						(DWORD) ((_tcslen(sValue) + 1) * sizeof (TCHAR)));
 
 
   		RegCloseKey( hKey );
   	}
 }
 // ---------------------------------------------------------------------------
// Routine:	GetTempInstallSetting
//
// Purpose:	Get temporary OEM settings
//
// ---------------------------------------------------------------------------
// 04/22/05 Michael Lee --- Added function needed to retrieve temporary OEM settings

 void GetTempInstallSettingStr (TCHAR *sName, LPTSTR sValue, LPTSTR sDefault, BOOL bDelete = TRUE)
 {
 	DWORD lResult = 0;
 	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
 	DWORD regOptions = KEY_READ | KEY_SET_VALUE;
 	_tcscpy(sValue, sDefault);
 
 	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYMANTEC_OEM_KEY, NULL, regOptions, &hKey);
     if (lResult == ERROR_SUCCESS)
     {
 		DWORD dwSize = 1024;
         lResult = SymSaferRegQueryValueEx(hKey, sName, NULL, &dwType, LPBYTE(sValue), &dwSize);
 
 		if (lResult == ERROR_SUCCESS)
 		{
			if (_tcscmp(sValue, "") == 0)
				_tcscpy(sValue, sDefault);

 			if (bDelete)
 				RegDeleteValue(hKey, sName);
 		}
		else _tcscpy(sValue, sDefault);
 
         RegCloseKey(hKey);
 	}
 
 
 }

// ---------------------------------------------------------------------------
// Routine:	GetSymInstallDir
//
// Purpose:	Gets the Symantec Antivirus Install Directory
//
// Makes
// ---------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
BOOL GetSymInstallDir (TCHAR *szInstallDir, DWORD dwSize)
{
	DWORD lResult = 0;
	HKEY hKey = NULL;
	DWORD regOptions = KEY_READ;

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion\\"), NULL, regOptions, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
        lResult = SymSaferRegQueryValueEx(hKey, _T("Home Directory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegCloseKey(hKey);
	}
	
	return (lResult == ERROR_SUCCESS);

}

// ---------------------------------------------------------------------------
// Routine:	GetProgramFilesDir
//
// Purpose:	Gets the system Program Files Directory. Copied from old Kepler code
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
BOOL GetProgramFilesDir( TCHAR* pszPath, UINT cbSize, bool isWin64 )
//Sets pszPath to the ProgramFiles directory value from the registry, setting the default if needed
//Returns TRUE on success, else FALSE
//Ripped from Phoenix (NSW 2) - originally RegGetProgramFilesDir
{
    HKEY hKey = NULL;
    LONG lResult = 0L;
	REGSAM regOptions = NULL;
	
    // get CurrentVersion location
	if( isWin64 )
	{
		regOptions = KEY_READ | KEY_WOW64_64KEY;
	}
	else
	{
		regOptions = KEY_READ;
	}
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, regOptions, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
        DWORD dwSize = cbSize;
        lResult = SymSaferRegQueryValueEx(hKey, _T("ProgramFilesDir"), NULL, NULL, LPBYTE(pszPath), &dwSize);
        RegCloseKey(hKey);
    }
	
    // manufacture if not good to here
    if (lResult != ERROR_SUCCESS)
    {
        // manufacture program files location
        TCHAR szDirName[ MAX_PATH ] = {0};
        GetWindowsDirectory(szDirName, sizeof(szDirName)); //lint !e534
        _tcscpy(szDirName + 3, DEFAULT_PROGRAMFILES);
		
        // add key by open/create
        if (CreateDirectory(szDirName, NULL))
        {
            lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | regOptions, NULL, &hKey, NULL);
            if (lResult == ERROR_SUCCESS)
            {
                lResult = RegSetValueEx(hKey, _T("ProgramFilesDir"), 0L, REG_SZ, LPBYTE(szDirName), (DWORD(_tcslen(szDirName)) + 1UL) * sizeof(TCHAR));
                RegCloseKey(hKey);
            }
        }
    }
	
    return (lResult == ERROR_SUCCESS);
}
// ---------------------------------------------------------------------------
// Routine:	GetCommonFilesDir
//
// Purpose:	Gets the Symantec Common Files Directory. Copied from old Kepler code
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
BOOL GetCommonFilesDir( LPTSTR pszPath, UINT cbSize, bool isWin64 )
//Sets pszPath to the CommonFiles directory value from the registry, setting the default if needed
//Returns TRUE on success, else FALSE
//Ripped from Phoenix (NSW 2) - originally RegGetProgramFilesDir
{
	HKEY hKey = NULL;
	LONG lResult = 0L;
	REGSAM regOptions = NULL;
	
	// get existing Common Files location
	if( isWin64 )
	{
		regOptions = KEY_READ | KEY_WOW64_64KEY;
	}
	else
	{
		regOptions = KEY_READ;
	}
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, regOptions, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD dwSize = cbSize;
		lResult = SymSaferRegQueryValueEx(hKey, _T("CommonFilesDir"), NULL, NULL, LPBYTE(pszPath), &dwSize);
		RegCloseKey(hKey);
	}
	
	// manufactur if not good to here
	if (lResult != ERROR_SUCCESS)
	{
		TCHAR szDirName[ MAX_PATH ] = {0};
		if (GetProgramFilesDir(szDirName, sizeof(szDirName), isWin64))
		{
			// manufacture program files location
			_tcscat(szDirName, _T("\\") DEFAULT_COMMONFILES);
			if (CreateDirectory(szDirName, NULL))
			{
	            lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | regOptions, NULL, &hKey, NULL);
				if (lResult == ERROR_SUCCESS)
				{
					lResult = RegSetValueEx(hKey, _T("CommonFilesDir"), 0L, REG_SZ, LPBYTE(szDirName), DWORD((_tcslen(szDirName) + 1) * sizeof(TCHAR)));
					RegCloseKey(hKey);
				}
			}
		}
	}
	
	return (lResult == ERROR_SUCCESS);
}

// ---------------------------------------------------------------------------
// Routine:	GetSymantecSharedDir
//
// Purpose:	Gets the Symantec Shared Directory. Copied from old Kepler code
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
BOOL GetSymantecSharedDir( LPTSTR pszPath, UINT cbSize, bool isWin64 )
//Sets pszPath to the Symantec Common directory (Common Files\Symantec Shared)
//Returns TRUE on success, else FALSE
//Ripped from Phoenix (NSW 2) - originally RegGetSymantecSharedDir
{
	BOOL fResult = FALSE;
	if (GetCommonFilesDir(pszPath, cbSize, isWin64))
	{
		_tcscat(pszPath, _T("\\") DEFAULT_SYMANTEC_SHARED);
		fResult = TRUE;
	}
	return fResult;
}

// ---------------------------------------------------------------------------
// Routine:	OEMInitSettings
//
// Purpose:	Reads MSI properties early (in Immediate execution) and stores them for later
//          when the SymOEM.dll is available
//
// Makes
// ---------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
extern "C" __declspec(dllexport) UINT __stdcall OEMInitSettings( MSIHANDLE hInstall )
{
    UINT	nRet = ERROR_SUCCESS;
	try
	{
		TCHAR	lpszInstallDir[1024], lpszOEMConfigFile[1024], lpszLogMessage[1024], lpszSourceDir[1024], lpszPathSource[1024];
		TCHAR	lpszInstalled[1024];
		DWORD	dwSize = 1024;

		DWORD dwDebug = 0;
		HKEY hKey = NULL;
		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD);
			if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
				dwDebug = 0;
			RegCloseKey(hKey);
		}
		 		// Check if TrialApp
 		// ---------------------  Set TrialApp (For CatFish --- Trial product) 
 		dwSize = 1024;
 		TCHAR lpszTrialApp [1024];
 		BOOL dTrialApp = 0;
 		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("TRIALAPP"), lpszTrialApp, &dwSize ) )
 		{
 			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "TRIALAPP = %s", lpszTrialApp);
 			MSILogMessage( hInstall, lpszLogMessage, TRUE );
 			if (dwDebug)
 				MessageBox( NULL, lpszLogMessage, "SAVOEM",  MB_OK );
 
 			if (_tcscmp(lpszTrialApp, "") != 0)
 				dTrialApp = atoi(lpszTrialApp);
 		}
 		else
 		{
 			if (dwDebug)
 				MessageBox( NULL, "TRIALAPP switch NOT FOUND", "SAVOEM",  MB_OK );
 		}
 
 	
 
 		if (dTrialApp)
 		{
 			if ( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("Installed"), lpszInstalled, &dwSize ) )
 			{
 				if (dwDebug)
 					MessageBox( NULL, "Must be Installed. Install Property exists and is not accessible", "SAV OEM", MB_OK );
 
 				CString strMessage, strTitle;
 				strMessage.LoadString( IDS_ALREADYINSTALLED);
 				strTitle.LoadString( IDS_ALREADYINSTALLED_TITLE );
 				MessageBox( NULL, strMessage, strTitle, MB_OK );
 	
 				return (UINT) E_FAIL;
 			}
 			else
 			{
 				sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "Installed = %s", lpszInstalled);
 				if (dwDebug)
 					MessageBox( NULL, lpszLogMessage, "SAVOEM",  MB_OK );
 
 				if (_tcscmp(lpszInstalled, "") != 0)
 				{
 					if (atoi(lpszInstalled) > 0)
 					{
 						CString strMessage, strTitle;
 						strMessage.LoadString( IDS_ALREADYINSTALLED );
 						strTitle.LoadString( IDS_ALREADYINSTALLED_TITLE );
 						MessageBox( NULL, strMessage, strTitle, MB_OK );
 	
 						return (UINT) E_FAIL;
 					}
 				}
 			}
 		}
 
 		SetTempInstallSetting(TRIALAPP_SWITCH, dTrialApp); 

		dwSize = 1024;
		if ( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("CONFIGFILE"), lpszOEMConfigFile, &dwSize ) )
		{
			sssnprintf( lpszLogMessage, sizeof(lpszLogMessage), _T("OEMCONFIGFILE not retrievable") );
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			throw(1);
		}
		else
		{
			if (dwDebug)
				MessageBox( NULL, lpszOEMConfigFile, _T("OEMConfigFile"), MB_OK );

		}


		dwSize = 1024;
		if ( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("INSTALLDIR"), lpszInstallDir, &dwSize ) )
		{
			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("INSTALLDIR not retrievable"));
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			throw(1);
		}
		else
		{
			//	if (dwDebug)
 			//		MessageBox( NULL, lpszInstallDir, "INSTALLDIR", MB_OK );
 
 		}
 
 		SetTempInstallSettingStr(CONFIGFILE_SWITCH, lpszOEMConfigFile);
 
 		dwSize = 1024;
 		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("SOURCEDIR"), lpszSourceDir, &dwSize ) )
 		{
   			if (dwDebug)
 				MessageBox( NULL, lpszSourceDir, _T("SOURCEDIR"), MB_OK );
 
 			TCHAR *lpszPathPtr = _tcsrchr(lpszSourceDir, '\\');
 			if (lpszPathPtr)
 			{
 				// 
 				// If has trailing \\, remove it.
 				//
				if (_tcslen(lpszPathPtr) == 1)
 				{
 					_tcsncpy(lpszPathSource, lpszSourceDir, _tcslen(lpszSourceDir) - sizeof(TCHAR));
 					lpszPathSource[_tcslen(lpszSourceDir) - sizeof(TCHAR)] = '\0';
 				}
 				else _tcscpy(lpszPathSource, lpszSourceDir);
 			}
 			else _tcscpy(lpszPathSource, lpszSourceDir);
 
 			SetTempInstallSettingStr(INSTALL_SOURCE_DIR, lpszPathSource);
		}   


		// ---------------------  Grab Other Settings early. 
		// MSI has a notion below that Deferred execution will not support the GetProperty function for many properties.
		// Deferred execution is needed because you need the SymOEMDLL to set properties.

		dwSize = 1024;
		TCHAR lpszLicenseType[1024];
		DWORD dIsTrialware = 1;
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("ANNUAL"), lpszLicenseType, &dwSize ) )
		{
			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("ANNUAL = %s"), lpszLicenseType);
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			if (dwDebug)
				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );

			if ((_tcscmp(lpszLicenseType, _T("0")) != 0) && (_tcscmp(lpszLicenseType, _T("")) != 0))
				dIsTrialware = 0;
		}
		else
		{
			if (dwDebug)
				MessageBox( NULL, _T("ANNUAL NOT FOUND"), _T("SAVOEM"),  MB_OK );
		}

	
		SetTempInstallSetting(TRIALWARE_SWITCH, dIsTrialware);

	
		// ---------------------  Set BootCount
		dwSize = 1024;
		TCHAR lpszBootCount [1024];
		BOOL dNumBootCount = 0;
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("BOOTCOUNT"), lpszBootCount, &dwSize ) )
		{
			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("BOOTCOUNT = %s"), lpszBootCount);
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			if (dwDebug)
				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );

			if (_tcscmp(lpszBootCount, _T("")) != 0)
				dNumBootCount = _tstoi(lpszBootCount);

			// 1-2VQHKZ -- Invalid Bootcounts
			if (dNumBootCount < 0)
				dNumBootCount = 0;
			else if (dNumBootCount > 14)
				dNumBootCount = 14;
		}
		else
		{
			if (dwDebug)
				MessageBox( NULL, _T("BOOTCOUNT NOT FOUND"), _T("SAVOEM"),  MB_OK );
		}

		SetTempInstallSetting(BOOTCOUNT_SWITCH, dNumBootCount);

		// ---------------------  Set Icon
		dwSize = 1024;
		TCHAR lpszIcon [1024];
		BOOL dIcon = 1;
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("ICON"), lpszIcon, &dwSize ) )
		{
			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("ICON = %s"), lpszIcon);
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			if (dwDebug)
				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );

			if (_tcscmp(lpszIcon, _T("")) != 0)
				dIcon = _tstoi(lpszIcon);
		}
		else
		{
			if (dwDebug)
				MessageBox( NULL, _T("ICON switch NOT FOUND"), _T("SAVOEM"),  MB_OK );
		}

		SetTempInstallSetting(ICON_SWITCH, dIcon);

		// ---------------------  Set AP status
		dwSize = 1024;
		TCHAR lpszAutoProtect [1024];
		BOOL dAutoProtect = 1;
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("ENABLEAUTOPROTECT"), lpszAutoProtect, &dwSize ) )
		{
			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("ENABLEAUTOPROTECT = %s"), lpszAutoProtect);
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			if (dwDebug)
				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );

			if (_tcscmp(lpszAutoProtect, _T("")) != 0)
				dAutoProtect = _tstoi(lpszAutoProtect);
		}
		else
		{
			if (dwDebug)
				MessageBox( NULL, _T("ENABLEAUTOPROTECT switch NOT FOUND"), _T("SAVOEM"),  MB_OK );
		}

		SetTempInstallSetting(ENABLEAUTOPROTECT_SWITCH, dAutoProtect);

		// ---------------------  Set OEM Install switch
		dwSize = 1024;
		TCHAR lpszOEMInstall [1024];
		BOOL dOEMInstall = 0;
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("OEM"), lpszOEMInstall, &dwSize ) )
		{
			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("OEM = %s"), lpszOEMInstall);
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			if (dwDebug)
				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );

			if (_tcscmp(lpszOEMInstall, _T("")) != 0)
				dOEMInstall = _tstoi(lpszOEMInstall);
		}
		else
		{
			if (dwDebug)
				MessageBox( NULL, _T("OEM switch NOT FOUND"), _T("SAVOEM"),  MB_OK );
		}

		SetTempInstallSetting(OEM_INSTALL_SWITCH, dOEMInstall);

		// MCLEE 04/07/2005, 2:14pm
		// ---------------------  Set OEM Factory Mode Switch
		dwSize = 1024;
		TCHAR lpszFactoryMode [1024];
		BOOL dFactoryMode = 0;
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("FACTORYMODE"), lpszFactoryMode, &dwSize ) )
		{
			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("FACTORYMODE = %s"), lpszFactoryMode);
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			if (dwDebug)
				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );

			if (_tcscmp(lpszFactoryMode, _T("")) != 0)
				dFactoryMode = _tstoi(lpszFactoryMode);
		}
		else
		{
			if (dwDebug)
				MessageBox( NULL, _T("FACTORYMODE Switch NOT FOUND"), _T("SAVOEM"),  MB_OK );
		}

		SetTempInstallSetting(FACTORY_MODE_SWITCH, dFactoryMode);

		// ---------------------  Set OEM Registry Key Switch
		dwSize = 1024;
		TCHAR lpszRegKeyName [1024];
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("REGKEY"), lpszRegKeyName, &dwSize ) )
		{
			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("REG KEY = %s"), lpszRegKeyName);
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			if (dwDebug)
				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );
		}
		else
		{
			if (dwDebug)
				MessageBox( NULL, _T("REGKEY Switch NOT FOUND"), _T("SAVOEM"),  MB_OK );
		}

		SetTempInstallSettingStr(REG_KEY_SWITCH, lpszRegKeyName);

 		// ---------------------  Set TrialLen (For CatFish --- Trial product and others) 
 		dwSize = 1024;
 		TCHAR lpszTrialLen [1024];
 		BOOL dTrialLen = 0;
 		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("TRIALLEN"), lpszTrialLen, &dwSize ) )
 		{
 			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("TRIALLEN = %s"), lpszTrialLen);
 			MSILogMessage( hInstall, lpszLogMessage, TRUE );
 			if (dwDebug)
 				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );
 
 			if (_tcscmp(lpszTrialLen, _T("")) != 0)
 				dTrialLen = _tstoi(lpszTrialLen);
 		}
 		else
 		{
 			if (dwDebug)
 				MessageBox( NULL, _T("TRIALLEN switch NOT FOUND"), _T("SAVOEM"),  MB_OK );
 		}
 
 		SetTempInstallSetting(TRIALLEN_SWITCH, dTrialLen);

 
	}
	catch( UINT )
	{
	}
	return nRet;

}
// ---------------------------------------------------------------------------
// Routine:	OEMSetSettings
//
// Purpose:	Sets the OEM properties found during install for use by SAV OEM
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
extern "C" __declspec(dllexport) UINT __stdcall OEMSetSettings( MSIHANDLE hInstall )
{
    UINT	nRet = ERROR_SUCCESS;
	 
 	TCHAR lpszLogMessage[1024];
 
 	DWORD dwDebug = 0;
 	HKEY hKey = NULL;
 	DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
 	if (dwResult == ERROR_SUCCESS)
 	{
 		DWORD dwSize = sizeof(DWORD);
 		if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
			dwDebug = 0;
 		RegCloseKey(hKey);
 	}
 
 	TCHAR lpszSourceDir[1024];
 	TCHAR lpszPathSource[1024];
 	TCHAR lpszOEMConfigFile[1024];
	TCHAR lpszOEMVendorDATFile[1024];
 	TCHAR lpszInstallDir[1024];
 	TCHAR lpszOldFile[1024];
 	TCHAR sDefault[1024];
 	_tcscpy(lpszOEMConfigFile, _T(""));
	_tcscpy(lpszOEMVendorDATFile, _T("vendor.dat"));
 	_tcscpy(sDefault, _T("sav-conf.ini"));
	if (dwDebug)
		MessageBox( NULL, sDefault, _T("Default CONFIG FILE"), MB_OK );

 	GetTempInstallSettingStr (CONFIGFILE_SWITCH, lpszOEMConfigFile, sDefault, TRUE);
 	if (dwDebug)
 		MessageBox( NULL, lpszOEMConfigFile, _T("CONFIG FILE"), MB_OK );
 	
	if (dwDebug)
		MessageBox( NULL, sDefault, _T("Default CONFIG FILE"), MB_OK );

 	GetTempInstallSettingStr (INSTALL_SOURCE_DIR, lpszSourceDir, _T("c:\\program files\\symantec antivirus"), TRUE);
 	if (dwDebug)
 		MessageBox( NULL, lpszSourceDir, _T("SOURCEDIR"), MB_OK );
 
 	if (_tcscmp(lpszOEMConfigFile, _T("")) != 0)
 	{
 
 		TCHAR *lpszFileName = _tcsrchr(lpszOEMConfigFile, '\\');
 		if (!lpszFileName)
 		{
 			if (dwDebug)
 				MessageBox( NULL, lpszSourceDir, _T("SOURCEDIR"), MB_OK );
 
 			TCHAR *lpszPathPtr = _tcsrchr(lpszSourceDir, '\\');
 			if (lpszPathPtr)
 			{
 				// 
 				// If has trailing \\, remove it.
 				//
 				if (_tcslen(lpszPathPtr) == 1)
 				{
 					_tcsncpy(lpszPathSource, lpszSourceDir, _tcslen(lpszSourceDir) - sizeof(TCHAR));
 					lpszPathSource[_tcslen(lpszSourceDir) - sizeof(TCHAR)] = '\0';
 				}
 				else _tcscpy(lpszPathSource, lpszSourceDir);
 			}
 			else _tcscpy(lpszPathSource, lpszSourceDir);
 
 			
 			lpszFileName = lpszOEMConfigFile;
 			//_stprintf(lpszOldFile, "%s\\%s", lpszPathSource, lpszFileName);
 
 		}
 		else 
 		{
 			_tcsncpy(lpszPathSource, lpszOEMConfigFile, lpszFileName - lpszOEMConfigFile);
 			lpszPathSource[lpszFileName - lpszOEMConfigFile] = '\0';
 			lpszFileName++;
 		}
 
 		sssnprintf(lpszOldFile, sizeof(lpszOldFile), _T("%s\\%s"), lpszPathSource, lpszFileName);
 
 		if (dwDebug)
 			MessageBox( NULL, lpszOldFile, _T("OldFile"), MB_OK );
 
 		DWORD dwSize = 1024;
		// install sav-conf.ini to SYMANTEC_SHARED
 		if (!GetSymantecSharedDir(lpszInstallDir, dwSize, FALSE))
 		{
 			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("InstallDir not retrievable. Error = %d"), GetLastError());
 			MSILogMessage( hInstall, lpszLogMessage, TRUE );
 			if (dwDebug)
 				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"), MB_OK );
 
 		}
 		else
 		{
 			TCHAR lpszInstallDirReal[1024];
 			TCHAR *lpszPathPtr = _tcsrchr(lpszInstallDir, '\\');
 			if (lpszPathPtr)
 			{
 				// 
 				// If has trailing \\, remove it.
 				//
 				if (_tcslen(lpszPathPtr) == 1)
 				{
 					_tcsncpy(lpszInstallDirReal, lpszInstallDir, _tcslen(lpszInstallDir) - sizeof(TCHAR));
 					lpszInstallDirReal[_tcslen(lpszInstallDir) - sizeof(TCHAR)] = '\0';
 				}
 				else _tcscpy(lpszInstallDirReal, lpszInstallDir);
 			}
 			else _tcscpy(lpszInstallDirReal, lpszInstallDir);
			_tcscat(lpszInstallDirReal, _T("\\OEM"));
 
 			TCHAR lpszNewFile[1024];
 			sssnprintf(lpszNewFile, sizeof(lpszNewFile), _T("%s\\%s"), lpszInstallDirReal, lpszFileName);
 
 			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("OEMSetSettings: New File = %s"), lpszNewFile);
 			MSILogMessage( hInstall, lpszLogMessage, TRUE );
 
 			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("OEMSetSettings: Old File = %s"), lpszOEMConfigFile);
 			MSILogMessage( hInstall, lpszLogMessage, TRUE );
 		
 			// MCLEE: TO-DO -- Validate Input of File before copying over to make sure it is correct and won't break our application
 			// that will eventually read it. Failing on the installer is better since it allows correction before
 			// application is installed.
 
 			// MCLEE: TO-DO -- Encrypt File to ensure security after End-User looks at it. We might also want to delete it
 			// after application initially reads it.
 
 			if (dwDebug)
 			{
 				MessageBox( NULL, lpszOldFile, _T("OldFile"),  MB_OK );
 				MessageBox( NULL, lpszNewFile, _T("NewFile"),  MB_OK );
 			}
 
 			//
 			// Create the destination directory if it doesn't exist
 			//
 			if (!CreateDirectory(lpszInstallDirReal, NULL))
 			{
 				//_stprintf(lpszLogMessage, _T("CopyOEMConfigFile: CreateDirectory %s had ERROR = %d"), lpszInstallDirReal, GetLastError());
 				//MSILogMessage( hInstall, lpszLogMessage, TRUE );
 				//if (dwDebug)
 				//	MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );
 			}
 			else
 			{
 				if (dwDebug)
 					MessageBox( NULL, _T("Created Destination Directory"), _T("SAVOEM"),  MB_OK );
 			}
 
 
 			if (!CopyFile(lpszOldFile, lpszNewFile, FALSE))
 			{
 				sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("OEMSetSettings: CopyFile had ERROR = %d"), GetLastError());
 				MSILogMessage( hInstall, lpszLogMessage, TRUE );
 				if (dwDebug)
 					MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );
 			}
 			else 
 			{
 				if (dwDebug)
 					MessageBox( NULL, _T("Copy Good"), _T("SAVOEM"),  MB_OK );
 			}
 		} // InstallDir defined
 
 	} // if there is a config file

	// Handle vendor.dat file
	// Ensure vendor.dat file exists before proceeding
	sssnprintf(lpszOldFile, sizeof(lpszOldFile), _T("%s\\%s"), lpszPathSource, lpszOEMVendorDATFile);
   	if (dwDebug)
		MessageBox( NULL, lpszOldFile, _T("Debug"),  MB_OK );
	WIN32_FIND_DATA stFind;
	HANDLE hResult = FindFirstFile (lpszOldFile, &stFind);
	BOOL bExist = (hResult != INVALID_HANDLE_VALUE);
	FindClose(hResult);
	if (dwDebug)
	{
		if (bExist)
   			MessageBox( NULL, "Vendor.DATA exists!", _T("Debug"),  MB_OK );
   		else MessageBox( NULL, "Vendor.DATA Does NOT exist.", _T("Debug"),  MB_OK );
	}

    DWORD dIsTrialware = 1;
    GetTempInstallSetting(TRIALWARE_SWITCH, dIsTrialware, FALSE);
    //
    // Only apply vendor.dat file if we are doing Trialware. 
    // If this is an annual install, don't do anything with vendor.dat
    //
    if ((bExist) && (dIsTrialware))
	{
		DWORD dwSize = 1024;
		if (GetSymInstallDir(lpszInstallDir, dwSize))
		{
			// Get rid of trailing \\ so it won't mess up formatting below.
   			size_t i = _tcslen(lpszInstallDir) - sizeof(TCHAR);
   			TCHAR *pStr = lpszInstallDir + i;
   			if ((*pStr) == _T('\\'))
   				lpszInstallDir[i] = '\0';
	   

			// Decrypt the file
			TCHAR szTempDir[1024];
			TCHAR szTempFile[1024];
				// Get the temp path
			GetTempPath(MAX_PATH, szTempDir);
			memset(szTempFile, 0, sizeof(szTempFile));
			// Create a temp file for the quarantine server package
			GetTempFileName(szTempDir, "vz", 0, szTempFile);

			CEncryptedFileGenerator aGenerator;
			aGenerator.CreateDecryptedFile(lpszOldFile, szTempFile);
   			if (dwDebug)
				MessageBox( NULL, szTempFile, _T("Debug"),  MB_OK );

   			TCHAR lpszOEMSessionWholePath[1024];
   			sssnprintf(lpszOEMSessionWholePath, sizeof(lpszOEMSessionWholePath), _T("%s\\savsess.txt"), lpszInstallDir);

   			// Use short path name. MSI likes this better
   			if (dwDebug)
   				MessageBox( NULL, lpszOEMSessionWholePath, _T("SAVOEM Vendor - SPN"),  MB_OK );

			TCHAR lpszOEMSessionWholePath_SPN[1024];
   			if (GetShortPathName(lpszOEMSessionWholePath, lpszOEMSessionWholePath_SPN, 1024))
   			{
   				if (dwDebug)
   					MessageBox( NULL, lpszOEMSessionWholePath_SPN, _T("SAVOEM - SPN"),  MB_OK );
   				_tcscpy(lpszOEMSessionWholePath, lpszOEMSessionWholePath_SPN);
   			}

			DWORD	dwFileAttrs	= GetFileAttributes(lpszOEMSessionWholePath);
			if (dwFileAttrs != INVALID_FILE_ATTRIBUTES) {
				dwFileAttrs &= ~FILE_ATTRIBUTE_READONLY;
				dwFileAttrs &= ~FILE_ATTRIBUTE_HIDDEN;
				SetFileAttributes(lpszOEMSessionWholePath, dwFileAttrs);

				if (!CopyFile(szTempFile, lpszOEMSessionWholePath, FALSE))
 				{
					sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("OEMSetSettings: CopyFile V had ERROR = %d"), GetLastError());
 					MSILogMessage( hInstall, lpszLogMessage, TRUE );
 					if (dwDebug)
 						MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );
		 	
				}
				else
				{
					if (dwDebug)
 						MessageBox( NULL, _T("Copy V Good"), _T("SAVOEM"),  MB_OK );
				}
			}
			DeleteFile(szTempFile);  // Delete the temp file
		} // if SAV directory exists
	} // if vendor.dat exists

	return nRet;
}

// ---------------------------------------------------------------------------
// Routine:	OEMRemoveSettings
//
// Purpose:	Remove OEM settings during uninstall
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
extern "C" __declspec(dllexport) UINT __stdcall OEMRemoveSettings( MSIHANDLE hInstall )
{
    UINT	nRet = ERROR_SUCCESS;
	try
	{
		DWORD	dIsELSMode=FALSE;
		DWORD	dIsSubscriptionMode=FALSE;
		TCHAR	lpszInstallDir[1024];
		TCHAR	lpszLogMessage[1024];
		DWORD dwSize= 1024;

		DWORD dwDebug = 0;
		HKEY hKey = NULL;
		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD);
			if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
				dwDebug = 0;
			RegCloseKey(hKey);
		}

		_tcscpy(lpszInstallDir, _T(""));
		// MCLEE - 4/13/2005 --- Use the OEM dir off of shared
		if (!GetSymantecSharedDir(lpszInstallDir, dwSize, FALSE))
		{
			if (dwDebug)
				MessageBox( NULL, lpszInstallDir, _T("INSTALLDIR"), MB_OK );

			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("INSTALLDIR not retrievable"));
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
		}
		else
		{
			_tcscat(lpszInstallDir, _T("\\OEM\\"));
		}

		if (dwDebug)
 			MessageBox( NULL, "RemoveOEMSettings", "SAVOEM",  MB_OK );

		// Get rid of trailing \\ so it won't mess up formatting below.
		size_t i = _tcslen(lpszInstallDir) - sizeof(TCHAR);
		TCHAR *pStr = lpszInstallDir + i;
		if ((*pStr) == _T('\\'))
			lpszInstallDir[i] = '\0';


		TCHAR lpszOEMDLL[1024];
		sssnprintf(lpszOEMDLL, sizeof(lpszOEMDLL), _T("%s\\%s"), lpszInstallDir, OEMDLL);
		HMODULE hOEM = LoadLibrary(lpszOEMDLL);
		if (hOEM)
		{
			if (dwDebug)
				MessageBox( NULL, _T("Loaded SymOEM.dll"), _T("SAVOEM"),  MB_OK );

			
			// ---------------------  IsOEM_RealELSMode
			lpfnIsOEM_RealELSMode IsOEM_RealELSMode = (lpfnIsOEM_RealELSMode) GetProcAddress(hOEM, "IsOEM_RealELSMode");
			if (IsOEM_RealELSMode)
				dIsELSMode = IsOEM_RealELSMode();
			lpfnIsOEM_RealSubscriptionMode IsOEM_RealSubscriptionMode = (lpfnIsOEM_RealSubscriptionMode) GetProcAddress(hOEM, "IsOEM_RealSubscriptionMode");
			if (IsOEM_RealSubscriptionMode)
				dIsSubscriptionMode = IsOEM_RealSubscriptionMode();
		
 
 			// delete sav-conf.ini file
 			lpfnLoadConfigIniToRegistry LoadConfigIniToRegistry = (lpfnLoadConfigIniToRegistry) GetProcAddress(hOEM, _T("LoadConfigIniToRegistry"));
 			TCHAR szError[1024] = _T("");
 			LoadConfigIniToRegistry(TRUE, szError); // delete sav-conf.ini
 			if (_tcslen(szError) > 0 && dwDebug)
 				MessageBox( NULL, szError, "SAVOEM", MB_OK);
   
		}
		
		DWORD lResult = 0;
		hKey = NULL;
		DWORD regOptions = KEY_ALL_ACCESS;

		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYMANTEC_ABOVE_OEM_KEY, NULL, regOptions, &hKey);
		if (lResult == ERROR_SUCCESS)
		{
			
			lResult = RegDeleteKey(hKey, SYMANTEC_OEM_VALUE);				

			if (lResult != ERROR_SUCCESS)
			{
				if (dwDebug)
				{
					 sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("Error Deleting OEM Key = %d, GetLastError = %d"), lResult, GetLastError());
					 MessageBox( NULL, lpszLogMessage, "SAVOEM",  MB_OK );
				}
				HKEY hKeyOEM = NULL;
				lResult = RegOpenKeyEx(hKey, SYMANTEC_OEM_VALUE, NULL, regOptions, &hKeyOEM);
				if (lResult == ERROR_SUCCESS)
				{
					// Must have subkeys. Need to enumerate --- only one level deep though
					DWORD dwLoc = 0, dwSize = 1023;
					TCHAR szKeyName[1024];
					CStringList szKeyValueNames;
					while(ERROR_SUCCESS == RegEnumKeyEx(hKeyOEM, dwLoc, szKeyName, &dwSize, NULL, NULL, NULL, NULL))
					{
						if (dwDebug)
							MessageBox( NULL, szKeyName, _T("SAVOEM"),  MB_OK );

						szKeyValueNames.AddTail(szKeyName);
						++dwLoc;
						dwSize = 1023;
					}
					
					// Need extra loop since RegEnumKeyEx gets index wrong
					// if you delete while you loop
					CString szKeyValueName;
					POSITION pos = szKeyValueNames.GetHeadPosition();
					while( pos )
					{
						szKeyValueName = szKeyValueNames.GetNext( pos );
						RegDeleteKey(hKeyOEM, szKeyValueName);				
					}

					// One more time to get rid of anything left over
					lResult = RegDeleteKey(hKey, SYMANTEC_OEM_VALUE);
					if (lResult != ERROR_SUCCESS)
					{
						if (dwDebug)
						{
							sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("Second Time: Error Deleting OEM Key = %d, GetLastError = %d"), lResult, GetLastError());
							MessageBox( NULL, lpszLogMessage, "SAVOEM",  MB_OK );
						}
					}
					RegCloseKey(hKeyOEM);
				} // if open OEM Key a success
			}
			RegCloseKey(hKey);

			
				
		}



		if (hOEM)
		{
			if (dIsELSMode)
			{
				sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("RETAIN ELS Mode"));
				MSILogMessage( hInstall, lpszLogMessage, TRUE );
				if (dwDebug)
					MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );
				lpfnSetOEM_RealELSMode SetOEM_RealELSMode = (lpfnSetOEM_RealELSMode) GetProcAddress(hOEM, "SetOEM_RealELSMode");
				if (SetOEM_RealELSMode)
					SetOEM_RealELSMode(TRUE);
			}

			if (dIsSubscriptionMode)
			{
				sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("RETAIN SUBSCRIPTION Mode"));
				MSILogMessage( hInstall, lpszLogMessage, TRUE );
				if (dwDebug)
					MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );
				lpfnSetOEM_RealSubscriptionMode SetOEM_RealSubscriptionMode = (lpfnSetOEM_RealSubscriptionMode) GetProcAddress(hOEM, "SetOEM_RealSubscriptionMode");
				if (SetOEM_RealSubscriptionMode)
					SetOEM_RealSubscriptionMode(TRUE);
			}

			FreeLibrary(hOEM);
		}
		
		nRet = (UINT)lResult;
//		return lResult;
	
	}
	catch( UINT )
	{
	}
	return ERROR_SUCCESS;

}
// ---------------------------------------------------------------------------
// Routine:	OEMSetOff
//
// Purpose:	Custom Action for base build to set the OEM status to off if a 
//	        previous OEM install existed
//
//----------------------------------------------------------------------------
// 09/23/04 Michael Lee --- Function created
extern "C" __declspec(dllexport) UINT __stdcall OEMSetOff( MSIHANDLE hInstall )
{
    UINT	nRet = ERROR_SUCCESS;
    TCHAR	lpszInstallDir[1024];
	TCHAR	lpszLogMessage[1024];

	try
	{
		DWORD dwDebug = 0;
		HKEY hKey = NULL;
		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD);
			if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
				dwDebug = 0;
			RegCloseKey(hKey);
		}

		if (dwDebug)
			MessageBox( NULL, _T("OEMSetOff"), _T("SAVOEM"), MB_OK );

		DWORD dwSize= 1024;
		_tcscpy(lpszInstallDir, _T(""));
		// MCLEE - 4/13/2005 --- Use the OEM dir off of shared
		if (!GetSymantecSharedDir(lpszInstallDir, dwSize, FALSE))
		{
			if (dwDebug)
				MessageBox( NULL, lpszInstallDir, _T("INSTALLDIR"), MB_OK );

			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("INSTALLDIR not retrievable"));
			MSILogMessage( hInstall, lpszLogMessage, TRUE );
			throw(1);
		}
		else
		{
			_tcscat(lpszInstallDir, _T("\\OEM\\"));
		}


		// Get rid of trailing \\ so it won't mess up formatting below.
		size_t i = _tcslen(lpszInstallDir) - sizeof(TCHAR);
		TCHAR *pStr = lpszInstallDir + i;
		if ((*pStr) == _T('\\'))
			lpszInstallDir[i] = '\0';


		TCHAR lpszOEMDLL[1024];
		sssnprintf(lpszOEMDLL, sizeof(lpszOEMDLL), _T("%s\\%s"), lpszInstallDir, OEMDLL);

		// Use short path name. MSI likes this better
		if (dwDebug)
			MessageBox( NULL, _T("Convert SPN"), _T("SAVOEM - SPN"),  MB_OK );

		TCHAR lpszOEMDLL_SPN[1024];
		if (GetShortPathName(lpszOEMDLL, lpszOEMDLL_SPN, 1024))
		{
			if (dwDebug)
				MessageBox( NULL, lpszOEMDLL_SPN, _T("SAVOEM - SPN"),  MB_OK );
			_tcscpy(lpszOEMDLL, lpszOEMDLL_SPN);
		}

		if (dwDebug)
			MessageBox( NULL, lpszOEMDLL, _T("SAVOEM"),  MB_OK );


		HMODULE hOEM = LoadLibrary(lpszOEMDLL);
		if (hOEM)
		{
			if (dwDebug)
				MessageBox( NULL, _T("Loaded SymOEM.dll"), _T("SAVOEM"),  MB_OK );

			
			// ---------------------  Set OEM Install
			lpfnSetOEM_Install SetOEM_Install = (lpfnSetOEM_Install) GetProcAddress(hOEM, "SetOEM_Install");

			if (dwDebug)
			{
				sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("SetOEM_Install = 0"));
				MessageBox( NULL, lpszLogMessage, _T("SAVOEM"),  MB_OK );
			}

			if (SetOEM_Install)
				SetOEM_Install(FALSE);


			// Free the Library.
			FreeLibrary(hOEM);
	
		}
		else 
		{
			if (dwDebug)
			{
				TCHAR sError [1024];
				sssnprintf(sError, sizeof(sError), _T("Error loading SymOEM.dll = %d"), GetLastError());
				MessageBox( NULL, sError, _T("SAVOEM"),  MB_OK );
			}
		}
	
	}
	catch( UINT )
	{
	}
	return nRet;
}





// ---------------------------------------------------------------------------
// Routine:	InstallLiveSubscribe
//
// Purpose:	Installs the LiveSubscribe Component
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
extern "C" __declspec(dllexport) UINT __stdcall InstallLiveSubscribe( MSIHANDLE /*hInstall*/ )
{
	UINT	nRet = ERROR_SUCCESS;
	try
	{
		TCHAR szDir[1024];
		DWORD dwDebug = 0;
		HKEY hKey = NULL;
		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD);
			if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
				dwDebug = 0;
			RegCloseKey(hKey);
		}

		if (dwDebug)
			MessageBox( NULL, _T("InstallLiveSubscribe"), _T("Debug"), MB_OK );

		 
 		// Check if the registry that shows we are an active LiveSubscribe user is there. If so, let's not reinstall over it.
 		// it could have been liveupdated. 
 		BOOL bAlreadySAVInstalledLS = FALSE;
 
 		hKey = NULL;
 		DWORD regOptions = KEY_READ;
 
		if (ERROR_SUCCESS  == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\Shared Technology\\LiveReg\\Apps", NULL, regOptions, &hKey))
 		{
 			DWORD dwSize = 1024;
 			TCHAR szSAVOEMStr[1024];
 			if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey, "SAVOEM", NULL, NULL, (unsigned char *) szSAVOEMStr, &dwSize))
 				bAlreadySAVInstalledLS = TRUE;
 
 			RegCloseKey(hKey);
 		}
		// Check one more place
		if (!bAlreadySAVInstalledLS)
		{
			if (dwDebug)
			{
				MessageBox( NULL, "Not SAVOEM Installed 1st Time", _T("Debug"), MB_OK );
			}

			if (ERROR_SUCCESS  == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LiveReg", NULL, regOptions, &hKey))
 			{
				if (dwDebug)
				{
					MessageBox( NULL, "Version Read Hive good", _T("Debug"), MB_OK );
				}

				DWORD dwSize = 1024;
 				TCHAR szVersionStr[1024];
 				if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey, "DisplayVersion", NULL, NULL, (unsigned char *) szVersionStr, &dwSize))
				{
					if (dwDebug)
					{
						MessageBox( NULL, szVersionStr, _T("Debug"), MB_OK );
					}
				
					CString sVersionStr = szVersionStr;
					CString sMajorVersion, sMinorVersion, sMinorMinorVersion;
					int dIndex = -1;
					if ((dIndex = sVersionStr.Find(_T("."))) != -1)
					{
						sMajorVersion = sVersionStr.Left(dIndex);
						sVersionStr = sVersionStr.Mid(dIndex + 1);
					}

					if ((dIndex = sVersionStr.Find(_T("."))) != -1)
					{
						sMinorVersion = sVersionStr.Left(dIndex);
						sVersionStr = sVersionStr.Mid(dIndex + 1);
					}

					if ((dIndex = sVersionStr.Find(_T("."))) != -1)
					{
						sMinorMinorVersion = sVersionStr.Left(dIndex);
					}

					if (dwDebug)
						MessageBox( NULL, sMajorVersion, _T("Debug"), MB_OK );

					if (dwDebug)
						MessageBox( NULL, sMinorVersion, _T("Debug"), MB_OK );

					if (dwDebug)
						MessageBox( NULL, sMinorMinorVersion, _T("Debug"), MB_OK );

					BOOL bDupVersion = FALSE;
					int dMajorVersion = _tstoi(sMajorVersion);
					int dMinorVersion = _tstoi(sMinorVersion);
					int dMinorMinorVersion = _tstoi(sMinorMinorVersion);

					if (dMajorVersion > 2)
					{
						bDupVersion = TRUE;   // higher version is fine
					}
					else if (dMajorVersion == 2)
					{
						if (dMinorVersion > 4)
						{
							bDupVersion = TRUE;
						}
						else if (dMinorVersion == 4)
						{
							if (dMinorMinorVersion >= 2)
								bDupVersion = TRUE;
						}
					}

					RegCloseKey(hKey);
					
					if (bDupVersion)
					{
						if (dwDebug)
						{
							MessageBox( NULL, _T("Second 2.4 or higher version found SAVOEM in InstalledApps"), _T("Debug"), MB_OK );
						}

 						bAlreadySAVInstalledLS = TRUE;
 						RegCloseKey(hKey);

						HKEY		hKey = NULL;
  						DWORD		dwDisp = 0;

  						if( ERROR_SUCCESS == RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
  																_T("Software\\Symantec\\Shared Technology\\LiveReg\\Apps"),
  																NULL,
  																_T(""),
  																REG_OPTION_NON_VOLATILE,
  																KEY_WRITE,
  																NULL,
  																&hKey,
  																&dwDisp) )
  						{
							TCHAR s[1024];
							_tcscpy(s, _T("Symantec Antivirus OEM"));
  							RegSetValueEx(	hKey,
  											_T("SAVOEM"),
  											0,
  											REG_SZ,
  											(BYTE*)s,
  											(_tcslen(s) + 1) * sizeof(TCHAR));


  							RegCloseKey( hKey );
  						} // RegCreateKeyEx --- most recent
					} // if bDupVersion
 				} // If Query DisplayVersion found
				else 
				{
					RegCloseKey(hKey);
				}
			} // If Open LiveReg Key successful
		} // If bAlreadySAVInstalledLS
 
 		if ((GetSymantecSharedDir(szDir, 1024, FALSE)) && (!bAlreadySAVInstalledLS))
		{
			if (dwDebug)
			{
				MessageBox( NULL, _T("GotSymantecSharedDir"), _T("Debug"), MB_OK );
				MessageBox( NULL, szDir, _T("Debug"), MB_OK );
			}

			TCHAR szCmd[1024];
			PROCESS_INFORMATION pInfo;
			STARTUPINFO         sInfo;

			
			sInfo.cb              = sizeof(STARTUPINFO);
			sInfo.lpReserved      = NULL;
			sInfo.lpReserved2     = NULL;
			sInfo.cbReserved2     = 0;
			sInfo.lpDesktop       = NULL;
			sInfo.lpTitle         = NULL;
			sInfo.dwFlags         = STARTF_FORCEOFFFEEDBACK | STARTF_USESHOWWINDOW; // Don't show a wait cursor when spawning 2nd instance.
			sInfo.dwX             = 0;
			sInfo.dwY             = 0;
			sInfo.dwFillAttribute = 0;
			sInfo.wShowWindow     = SW_SHOW;

			TCHAR szLiveSubscribeSetup[1024];
			sssnprintf(szLiveSubscribeSetup, sizeof(szLiveSubscribeSetup), _T("%s\\%s"), szDir, _T("LRSetup.exe"));
			sssnprintf(szCmd, sizeof(szCmd), _T("\"%s\" /SILENT"), szLiveSubscribeSetup);
			if (dwDebug)
				MessageBox( NULL, szCmd, _T("Debug"), MB_OK);
			if (CreateProcess(NULL, szCmd, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo)) 
			{
				if (dwDebug)
					MessageBox( NULL, _T("Create Process succeeded"),  _T("Debug"), MB_OK );

				while (WaitForSingleObject(pInfo.hProcess, 1000 * 60) == WAIT_TIMEOUT) 
				{

				}

				if (pInfo.hProcess)
					CloseHandle(pInfo.hProcess);

				if (pInfo.hThread)
					CloseHandle(pInfo.hThread);

				HKEY		hKey = NULL;
  				DWORD		dwDisp = 0;

  				if( ERROR_SUCCESS == RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
  														_T("Software\\Symantec\\Shared Technology\\LiveReg\\Apps"),
  														NULL,
  														_T(""),
  														REG_OPTION_NON_VOLATILE,
  														KEY_WRITE,
  														NULL,
  														&hKey,
  														&dwDisp) )
  				{
					TCHAR s[1024];
					_tcscpy(s, _T("Symantec Antivirus OEM"));
  					RegSetValueEx(	hKey,
  									_T("SAVOEM"),
  									0,
  									REG_SZ,
  									(BYTE*)s,
  									(_tcslen(s) + 1) * sizeof(TCHAR));


  					RegCloseKey( hKey );
  				}

			}
			else
			{
				DWORD dwError = GetLastError();
				TCHAR szMsg[64] = _T("");
				_stprintf(szMsg, _T("CreateProcess Failed with %d"), dwError);
				if (dwDebug)
					MessageBox( NULL, szMsg, _T("Debug"), MB_OK);
			}
		
		}
	}
	catch( UINT )
	{
	}
	return nRet;
}

// ---------------------------------------------------------------------------
// Routine:	UnInstallLiveSubscribe
//
// Purpose:	Installs the LiveSubscribe Component
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
extern "C" __declspec(dllexport) UINT __stdcall UnInstallLiveSubscribe( MSIHANDLE /*hInstall*/ )
{
    UINT	nRet = ERROR_SUCCESS;
	DWORD	dwDebug = 0;

	try
	{
		DWORD regOptions = KEY_READ | KEY_WRITE;
		HKEY hKey = NULL;
		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD);
			DWORD dwType = REG_DWORD;
			if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, &dwType, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
				dwDebug = 0;
			RegCloseKey(hKey);
		}

		
		if (dwDebug)
		{
			MessageBox( NULL, _T("UnInstallLiveSubscribe"), _T("Debug"), MB_OK );
		}

		if (ERROR_SUCCESS  == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\Shared Technology\\LiveReg\\Apps", NULL, regOptions, &hKey))
 		{
			if (dwDebug)
			{
				MessageBox( NULL, _T("Exists: InstalledApps"), _T("Debug"), MB_OK );
			}
				
 			DWORD dwSize = 1024 * sizeof(TCHAR);
 			TCHAR szSAVOEMStr[1024];
			DWORD dwRet=0;
			DWORD dwType = REG_SZ;
 			if (ERROR_SUCCESS == (dwRet = SymSaferRegQueryValueEx(hKey, _T("SAVOEM"), NULL, &dwType, LPBYTE(szSAVOEMStr), &dwSize)))
			{
				if (dwDebug)
				{
					MessageBox( NULL, _T("Delete SAVOEM value"), _T("Debug"), MB_OK );
				}
				
				if (ERROR_SUCCESS != (dwRet = RegDeleteValue(hKey, _T("SAVOEM"))))
				{
					if (dwDebug)
					{
						TCHAR sError [1024];
						sssnprintf(sError, sizeof(sError), _T("Error Deleting SAVOEM = %d"), dwRet);
						MessageBox( NULL, sError, _T("SAVOEM"),  MB_OK );
					}
				}
			}
			else
			{
				if (dwDebug)
				{
					TCHAR sError [1024];
					sssnprintf(sError, sizeof(sError), _T("Error Querying SAVOEM = %d"), dwRet);
					MessageBox( NULL, sError, _T("SAVOEM"),  MB_OK );
				}
			}

 			RegCloseKey(hKey);
 		}
		else
		{
			if (dwDebug)
			{
				TCHAR sError [1024];
				sssnprintf(sError, sizeof(sError), _T("Error Opening Apps Key = %d"), GetLastError());
				MessageBox( NULL, sError, _T("SAVOEM"),  MB_OK );
			}
		}
#if 0
		TCHAR szDir[1024];
		DWORD dwDebug = 0;
		HKEY hKey = NULL;
		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD);
			if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, &dwType, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
				dwDebug = 0;
			RegCloseKey(hKey);
		}

		if (dwDebug)
			MessageBox( NULL, _T("UnInstallLiveSubscribe"), _T("Debug"), MB_OK );

		if (GetSymantecSharedDir(szDir, 1024, FALSE))
		{
			if (dwDebug)
			{
				MessageBox( NULL, _T("GotSymantecSharedDir"), _T("Debug"), MB_OK );
				MessageBox( NULL, szDir,  _T("Debug"), MB_OK );
			}

			TCHAR szCmd[1024];
			PROCESS_INFORMATION pInfo;
			STARTUPINFO         sInfo;

			
			sInfo.cb              = sizeof(STARTUPINFO);
			sInfo.lpReserved      = NULL;
			sInfo.lpReserved2     = NULL;
			sInfo.cbReserved2     = 0;
			sInfo.lpDesktop       = NULL;
			sInfo.lpTitle         = NULL;
			sInfo.dwFlags         = STARTF_FORCEOFFFEEDBACK; // Don't show a wait cursor when spawning 2nd instance.
			sInfo.dwX             = 0;
			sInfo.dwY             = 0;
			sInfo.dwFillAttribute = 0;
			sInfo.wShowWindow     = SW_SHOW;

			TCHAR szLiveSubscribeSetup[1024];
			sssnprintf(szLiveSubscribeSetup, sizeof(szLiveSubscribeSetup), _T("%s\\LiveReg\\%s"), szDir, _T("VCSetup.exe"));
			sssnprintf(szCmd, sizeof(szCmd), _T("\"%s\" /SILENT /REMOVE"), szLiveSubscribeSetup);
			if (CreateProcess(NULL, szCmd, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo)) 
			{
				if (dwDebug)
					MessageBox( NULL, _T("Create Process succeeded"), _T("Debug"), MB_OK );

				while (WaitForSingleObject(pInfo.hProcess, 1000 * 60) == WAIT_TIMEOUT) 
				{

				}

				if (pInfo.hProcess)
					CloseHandle(pInfo.hProcess);

				if (pInfo.hThread)
					CloseHandle(pInfo.hThread);
			}
		}
#endif
	}
	catch( UINT )
	{
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////////
//
// Function: MSILogMessage()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//		TCHAR [] - Text to log
//		BOOL - TRUE if only OutputDebugString is needed
// Returns:
//		NONE
//
// Description:  
//	This function logs messages to the MSI log file
//
//////////////////////////////////////////////////////////////////////////
// 8/07/01 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void MSILogMessage( MSIHANDLE hInstall, TCHAR szString[], BOOL bDoAction )
{
	TCHAR szPrefix[]=_T("OEMCA: ");
	TCHAR szLogLine[MAX_PATH];

	_tcsncpy( szLogLine, szPrefix, MAX_PATH - 1 );
	_tcsncat( szLogLine, szString, MAX_PATH - ( sizeof szPrefix / sizeof szPrefix[0] )- 1 );

	if( FALSE == bDoAction )
	{
		PMSIHANDLE hRec = MsiCreateRecord( 1 );

		if( hRec )
		{
			MsiRecordClearData( hRec );
			MsiRecordSetString( hRec, 0, szLogLine );
			MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
		}
	}

	OutputDebugString( szLogLine );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: DeleteDir()
//
// Parameters:
//		CString - Folder to delete
//		CString - optional file pattern of files inside
//
// Returns:
//		BOOL - TRUE if folder removed
//
// Description:  
//	Removes a folder including the files inside of it
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
// 1/30/04 - DKOWALYSHYN function de-MFC'ed and modified for my needs
//////////////////////////////////////////////////////////////////////////
BOOL DeleteDir( MSIHANDLE hInstall, TCHAR* strFolder, TCHAR* strPattern )
{
	BOOL bRet = TRUE;
	WIN32_FIND_DATA hFindData;
	TCHAR strAllFiles[ MAX_PATH] = {0};
	TCHAR strSFile[ MAX_PATH] = {0};
	DWORD		dwFileAttrs					= INVALID_FILE_ATTRIBUTES;
	dwFileAttrs &= ~FILE_ATTRIBUTE_READONLY;
	dwFileAttrs &= ~FILE_ATTRIBUTE_HIDDEN;
					
	_tcscpy (strAllFiles,strFolder);
	_tcscat (strAllFiles,"\\");
	_tcscat (strAllFiles,strPattern);
	MSILogMessage( hInstall, "DeleteDir folder" );
	MSILogMessage( hInstall, strFolder);
	MSILogMessage( hInstall, "DeleteDir pattern" );
	MSILogMessage( hInstall, strPattern);

	HANDLE hFind = FindFirstFile( strAllFiles, &hFindData );
	while( (INVALID_HANDLE_VALUE != hFind) && (bRet) )
	{
		_tcscpy (strSFile,strFolder);
		_tcscat (strSFile,"\\");
		_tcscat (strSFile,hFindData.cFileName);
		if( !(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			MSILogMessage( hInstall, "DeleteDir file" );
			MSILogMessage( hInstall, strSFile);

			SetFileAttributes(strSFile, dwFileAttrs);
					
			DeleteFile( strSFile );
		}
		else
		{

			if( lstrcmp( hFindData.cFileName, "." ) && lstrcmp( hFindData.cFileName, ".." ) )
				DeleteDir(  hInstall, strSFile, strPattern );
		}

		bRet = FindNextFile( hFind, &hFindData );
	}
	
    DWORD dwResult = GetLastError();
    if (dwResult == ERROR_NO_MORE_FILES)
    {
        OutputDebugString("No more files to delete.");
    }
    else
    {
			TCHAR szBuf[MAX_PATH] = {0}; 
		    _tprintf(szBuf, "Delete dir GetLastError returned %u\n", dwResult); 
			MSILogMessage( hInstall, szBuf );
    }
	if( hFind )
		FindClose( hFind );
	bRet = RemoveDirectory( strFolder );

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
//
// Function: OEMTempSavDir
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS reguardless of anything that happens
//		in the function.  This function should only be called when
//		uninstalling, and we would not want to rollback the uninstall
//		if a quarantine file could not be deleted.
//
// Description:
//	    Saves Home Directory during uninstall before registry is blown away
//
//////////////////////////////////////////////////////////////////////////
// 06/09/05 - Michael C. Lee - function created.
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall OEMTempSaveDir( MSIHANDLE hInstall )
{
	DWORD dwResult = 0;
	HKEY hKey = NULL;
	DWORD dwRegOptions = KEY_READ;
	TCHAR szInstallDir[1024];
	DWORD dwSize = 1024;
	TCHAR lpszLogMessage[1024];

	DWORD dwDebug = 0;
	dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
			dwDebug = 0;
		RegCloseKey(hKey);
	}

	if (dwDebug)
		MessageBox( NULL, "In OEMTempSaveDir", _T("Debug"), MB_OK );

	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion"), NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
		dwSize = 1024;
        dwResult = SymSaferRegQueryValueEx(hKey, _T("Home Directory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegCloseKey(hKey);
		if (dwResult != ERROR_SUCCESS)
		{
			dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion\\DomainData"), NULL, dwRegOptions, &hKey);
			if (dwResult == ERROR_SUCCESS)
			{
				dwSize = 1024;
				dwResult = SymSaferRegQueryValueEx(hKey, _T("Home Directory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
				RegCloseKey(hKey);
			}
		}
		if (dwResult == ERROR_SUCCESS)
		{
			if (dwDebug)
				MessageBox( NULL, "Found SAV Home Directory", _T("Debug"), MB_OK );

			if (dwDebug)
				MessageBox( NULL, szInstallDir, _T("Debug"), MB_OK );

			HKEY hKey;
			TCHAR szClassName[] = {0};

			if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  					HKEY_LOCAL_MACHINE,                   // handle to open key
	  					_T("Software\\Symantec"), 	      // subkey name
	  					0,									// reserved
	  					szClassName,									// class string
	  					REG_OPTION_NON_VOLATILE,              // special options
	  					KEY_ALL_ACCESS,                       // desired security access
	  					NULL,									// inheritance
	  					&hKey,								// key handle 
	  					NULL									// disposition value buffer
	  					))
			{
				RegSetValueEx (hKey,
						_T("OEMSAVHomeDirectory"),
						0,
						REG_SZ,
						(LPBYTE)szInstallDir,			// pointer to the value data
						(DWORD) ((_tcslen(szInstallDir) + 1) * sizeof(TCHAR)));		// length of value data 
				
				RegCloseKey( hKey );
	
			}
		}
				
	}

	//
	// OEM dir
	// 
	dwSize= 1024;
	// MCLEE - 4/13/2005 --- Use the OEM dir off of shared
	if (!GetSymantecSharedDir(szInstallDir, dwSize, FALSE))
	{
		if (dwDebug)
			MessageBox( NULL, szInstallDir, _T("OEMSHAREDDIR"), MB_OK );

		sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), _T("INSTALLDIR not retrievable"));
		MSILogMessage( hInstall, lpszLogMessage, TRUE );
		//throw(1);
	}
	else
	{
		_tcscat(szInstallDir, _T("\\OEM\\"));
		HKEY hKey;
		TCHAR szClassName[] = {0};

		if (dwDebug)
				MessageBox( NULL, "Found OEM Directory", _T("Debug"), MB_OK );

		if (dwDebug)
				MessageBox( NULL, szInstallDir, _T("Debug"), MB_OK );

		if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  				HKEY_LOCAL_MACHINE,                   // handle to open key
	  				_T("Software\\Symantec"), 	      // subkey name
	  				0,									// reserved
	  				szClassName,									// class string
	  				REG_OPTION_NON_VOLATILE,              // special options
	  				KEY_ALL_ACCESS,                       // desired security access
	  				NULL,									// inheritance
	  				&hKey,								// key handle 
	  				NULL									// disposition value buffer
	  				))
			{
				RegSetValueEx (hKey,
						_T("OEMCommonHomeDirectory"),
						0,
						REG_SZ,
						(LPBYTE)szInstallDir,			// pointer to the value data
						(DWORD) ((_tcslen(szInstallDir) + 1) * sizeof(TCHAR)));		// length of value data 
				
				RegCloseKey( hKey );
	
			}
	}
	// 
	// SCF dir
	// 
	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"), NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
		dwSize = 1024;
        dwResult = SymSaferRegQueryValueEx(hKey, _T("Internet Security"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegCloseKey(hKey);
		
		if (dwResult == ERROR_SUCCESS)
		{
		if (dwDebug)
				MessageBox( NULL, "SCF Install Dir", _T("Debug"), MB_OK );

		if (dwDebug)
				MessageBox( NULL, szInstallDir, _T("Debug"), MB_OK );

			HKEY hKey;
			TCHAR szClassName[] = {0};

			if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  					HKEY_LOCAL_MACHINE,                   // handle to open key
	  					_T("Software\\Symantec"), 	      // subkey name
	  					0,									// reserved
	  					szClassName,									// class string
	  					REG_OPTION_NON_VOLATILE,              // special options
	  					KEY_ALL_ACCESS,                       // desired security access
	  					NULL,									// inheritance
	  					&hKey,								// key handle 
	  					NULL									// disposition value buffer
	  					))
			{
				RegSetValueEx (hKey,
						_T("OEMSCFHomeDirectory"),
						0,
						REG_SZ,
						(LPBYTE)szInstallDir,			// pointer to the value data
						DWORD((_tcslen(szInstallDir) + 1)) * sizeof(TCHAR));		// length of value data 
				
				RegCloseKey( hKey );
	
			}
		}
				
	}
	return ERROR_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
//
// Function: OEMCleanupDir
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS reguardless of anything that happens
//		in the function.  This function should only be called when
//		uninstalling, and we would not want to rollback the uninstall
//		if a quarantine file could not be deleted.
//
// Description:
//		Deletes orphaned directories under the install folder
//////////////////////////////////////////////////////////////////////////
// 06/08/05 - Michael C. Lee - function created.
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall OEMCleanupDir( MSIHANDLE hInstall )
{
	DWORD dwResult = 0;
	HKEY hKey = NULL;
	DWORD dwRegOptions = KEY_READ | KEY_SET_VALUE;
	TCHAR szInstallDir[1024];
	TCHAR szInstallDirWithoutSlash[1024];
	DWORD dwSize = 1024;

	DWORD dwDebug = 0;
	dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
			dwDebug = 0;
		RegCloseKey(hKey);
	}
	if (dwDebug)
		MessageBox( NULL, "In OEMCleanupDir", _T("Debug"), MB_OK );

	// Delete SAV Home directory (if it exists)
	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
		dwSize = 1024;
        dwResult = SymSaferRegQueryValueEx(hKey, _T("OEMSAVHomeDirectory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegDeleteValue( hKey, _T("OEMSAVHomeDirectory") );
		RegCloseKey(hKey);
	
		if (dwResult == ERROR_SUCCESS)
		{
			if (dwDebug)
				MessageBox( NULL, "Found OEMSAVHomeDirectory", _T("Debug"), MB_OK );

			if (dwDebug)
				MessageBox( NULL, szInstallDir, _T("Debug"), MB_OK );

			ssStrnCpy(szInstallDirWithoutSlash, szInstallDir, sizeof(szInstallDirWithoutSlash)); 
            if (szInstallDirWithoutSlash[_tcslen(szInstallDirWithoutSlash) - 1] == _T('\\'))
            {
			    szInstallDirWithoutSlash[_tcslen(szInstallDirWithoutSlash) - 1] = '\0'; // get rid of \\ 
            }
			DeleteDir( hInstall, szInstallDirWithoutSlash, _T("*.*") );
			HKEY hKeySet=NULL;
			TCHAR szClassName[] = {0};
			if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  					HKEY_LOCAL_MACHINE,                   // handle to open key
	  					_T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"), 	      // subkey name
	  					0,									// reserved
	  					szClassName,									// class string
	  					REG_OPTION_NON_VOLATILE,              // special options
	  					KEY_ALL_ACCESS,                       // desired security access
	  					NULL,									// inheritance
	  					&hKeySet,								// key handle 
	  					NULL									// disposition value buffer
	  					))
			{
				TCHAR szRemoveNextBoot[1024];
				sssnprintf(szRemoveNextBoot, sizeof(szRemoveNextBoot), "cmd.exe /c rmdir \"%s\"", szInstallDir);
				RegSetValueEx (hKeySet,
						_T("OEMSAVHomeDirectory"),
						0,
						REG_SZ,
						(LPBYTE)szRemoveNextBoot,			// pointer to the value data
						DWORD((_tcslen(szRemoveNextBoot) + 1)) * sizeof(TCHAR));		// length of value data 
				
				RegCloseKey( hKeySet );
	
			}
		}
	}

	// Delete Common Dir
	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
		dwSize = 1024;
        dwResult = SymSaferRegQueryValueEx(hKey, _T("OEMCommonHomeDirectory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegDeleteValue( hKey, _T("OEMCommonHomeDirectory") );
		RegCloseKey(hKey);
	
		if (dwResult == ERROR_SUCCESS)
		{
			if (dwDebug)
				MessageBox( NULL, "Found OEMCommonHomeDirectory", _T("Debug"), MB_OK );

			if (dwDebug)
				MessageBox( NULL, szInstallDir, _T("Debug"), MB_OK );

			ssStrnCpy(szInstallDirWithoutSlash, szInstallDir, sizeof(szInstallDirWithoutSlash)); 
            if (szInstallDirWithoutSlash[_tcslen(szInstallDirWithoutSlash) - 1] == _T('\\'))
            {
			    szInstallDirWithoutSlash[_tcslen(szInstallDirWithoutSlash) - 1] = '\0'; // get rid of \\ 
            }
			DeleteDir( hInstall, szInstallDirWithoutSlash, _T("*.*") );
			HKEY hKeySet=NULL;
			TCHAR szClassName[] = {0};
			if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  					HKEY_LOCAL_MACHINE,                   // handle to open key
	  					_T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"), 	      // subkey name
	  					0,									// reserved
	  					szClassName,									// class string
	  					REG_OPTION_NON_VOLATILE,              // special options
	  					KEY_ALL_ACCESS,                       // desired security access
	  					NULL,									// inheritance
	  					&hKeySet,								// key handle 
	  					NULL									// disposition value buffer
	  					))
			{
				TCHAR szRemoveNextBoot[1024];
				sssnprintf(szRemoveNextBoot, sizeof(szRemoveNextBoot), "cmd.exe /c rmdir \"%s\"", szInstallDir);
				RegSetValueEx (hKeySet,
						_T("OEMCommonHomeDirectory"),
						0,
						REG_SZ,
						(LPBYTE)szRemoveNextBoot,			// pointer to the value data
						DWORD((_tcslen(szRemoveNextBoot) + 1)) * sizeof(TCHAR));		// length of value data 
				
				RegCloseKey( hKeySet );
	
			}
		}
	}
	

	// Delete SCF Directory if it exists
	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
		dwSize = 1024;
        dwResult = SymSaferRegQueryValueEx(hKey, _T("OEMSCFHomeDirectory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegDeleteValue( hKey, _T("OEMSCFHomeDirectory") );
		RegCloseKey(hKey);
	
		if (dwResult == ERROR_SUCCESS)
		{
			if (dwDebug)
				MessageBox( NULL, "Found OEMSCFHomeDirectory", _T("Debug"), MB_OK );

			if (dwDebug)
				MessageBox( NULL, szInstallDir, _T("Debug"), MB_OK );

			ssStrnCpy(szInstallDirWithoutSlash, szInstallDir, sizeof(szInstallDirWithoutSlash)); 
            if (szInstallDirWithoutSlash[_tcslen(szInstallDirWithoutSlash) - 1] == _T('\\'))
            {
			    szInstallDirWithoutSlash[_tcslen(szInstallDirWithoutSlash) - 1] = '\0'; // get rid of \\ 
            }
			DeleteDir( hInstall, szInstallDirWithoutSlash, _T("*.*") );
			HKEY hKeySet=NULL;
			TCHAR szClassName[] = {0};
			if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  					HKEY_LOCAL_MACHINE,                   // handle to open key
	  					_T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"), 	      // subkey name
	  					0,									// reserved
	  					szClassName,									// class string
	  					REG_OPTION_NON_VOLATILE,              // special options
	  					KEY_ALL_ACCESS,                       // desired security access
	  					NULL,									// inheritance
	  					&hKeySet,								// key handle 
	  					NULL									// disposition value buffer
	  					))
			{
				TCHAR szRemoveNextBoot[1024];
				sssnprintf(szRemoveNextBoot, sizeof(szRemoveNextBoot), "cmd.exe /c rmdir \"%s\"", szInstallDir);
				RegSetValueEx (hKeySet,
						_T("OEMSCFHomeDirectory"),
						0,
						REG_SZ,
						(LPBYTE)szRemoveNextBoot,			// pointer to the value data
						DWORD((_tcslen(szRemoveNextBoot) + 1)) * sizeof(TCHAR));		// length of value data 
				
				RegCloseKey( hKeySet );
	
			}
		}
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: CopyValues()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//
// Returns:
//		N/A at the moment, need to return true/false
//
// Description:  
//	This function enumerates all the values of hSource and copies them to 
//  hDest.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void CopyValues( HKEY hSKey, HKEY hDKey )
{
	TCHAR szName[MAX_PATH] = {0};
	BYTE *pData = NULL;

	DWORD dwIndex = 0;
	DWORD dwSize = sizeof( szName );
	DWORD dwType = 0;
	DWORD dwDatasize = 0;

	LONG lRet = RegEnumValue( hSKey, dwIndex, szName, &dwSize, NULL, &dwType, NULL, &dwDatasize );
	while( ERROR_SUCCESS == lRet )
	{
		pData = new BYTE[ dwDatasize ];
		if( NULL != pData )
		{
			if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hSKey, szName, NULL, NULL, (LPBYTE)pData, &dwDatasize ) )
				RegSetValueEx( hDKey, szName, NULL, dwType, (CONST BYTE *)pData, dwDatasize );

			delete [] pData;
			pData = NULL;
		}

		++dwIndex;
		dwDatasize = 0;
		dwSize = sizeof( szName );
		lRet = RegEnumValue( hSKey, dwIndex, szName, &dwSize, NULL, &dwType, NULL, &dwDatasize );
	}
}
//////////////////////////////////////////////////////////////////////////
//
// Function: CopyKeys()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//		TCHAR * - Name of key to open/create
//
// Returns:
//		N/A at the moment, need to return true/false
//
// Description:  
//	This recursive function copies the hSource keys to the hDest including all
//  associated values.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void CopyKeys( HKEY hSKey, HKEY hDKey, TCHAR *szKeyname )
{
	HKEY hSubkey = NULL;
	HKEY hWSubkey = NULL;
	TCHAR szSubkeyname[ MAX_PATH ] = {0};
	DWORD dwSize = sizeof( szSubkeyname );
	DWORD dwIndex = 0;

	if( ERROR_SUCCESS != RegOpenKeyEx( hSKey, szKeyname, NULL, KEY_READ, &hSubkey ) )
	{
		// Source key does not exist, do not continue...
		return;
	}

	RegCreateKeyEx( hDKey, szKeyname, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hWSubkey, NULL );

	LONG lRet = RegEnumKeyEx( hSubkey, dwIndex, szSubkeyname, &dwSize, NULL, NULL, NULL, NULL );
	while( ERROR_SUCCESS == lRet )
	{
		++dwIndex;
		dwSize = sizeof( szSubkeyname );
		CopyKeys( hSubkey, hWSubkey, szSubkeyname );
		lRet = RegEnumKeyEx( hSubkey, dwIndex, szSubkeyname, &dwSize, NULL, NULL, NULL, NULL );
	}

	CopyValues( hSubkey, hWSubkey );

	if( hSubkey )
		RegCloseKey( hSubkey );

	if( hWSubkey )
		RegCloseKey( hWSubkey );
}
//////////////////////////////////////////////////////////////////////////
//
// Function: OEMSaveCurrentSettings
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS reguardless of anything that happens
//		in the function.  This function should only be called when
//		uninstalling, and we would not want to rollback the uninstall
//		if a quarantine file could not be deleted.
//
// Description:
//		Saves OEM settings during an overinstall or patch
//////////////////////////////////////////////////////////////////////////
// 09/12/05 - Michael C. Lee - function created.
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall OEMSaveCurrentSettings( MSIHANDLE hInstall )
{
	HKEY hKey = NULL;
	DWORD dwDebug = 0;
	DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
			dwDebug = 0;
		RegCloseKey(hKey);
	}

	HKEY hKeySrc=NULL, hKeyDest=NULL;
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec"), NULL, KEY_READ, &hKeySrc ) )
	{	
		if (dwDebug)
			MessageBox( NULL, "Opened Symantec OEM Key.", _T("Debug"), MB_OK );

		if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE\\Symantec"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyDest, NULL ) )
		{
			if (dwDebug)
				MessageBox( NULL, "Successfully created new Symantec Key", _T("Debug"), MB_OK );

			CopyKeys(hKeySrc, hKeyDest, _T("OEM"));
			RegCloseKey(hKeyDest);

			if (dwDebug)
				MessageBox( NULL, "OEM Key copied and saved", _T("Debug"), MB_OK );

		}
		RegCloseKey(hKeySrc);
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: OEMRestoreCurrentSettings
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS reguardless of anything that happens
//		in the function.  This function should only be called when
//		uninstalling, and we would not want to rollback the uninstall
//		if a quarantine file could not be deleted.
//
// Description:
//		Restores OEM settings during an overinstall or patch
//////////////////////////////////////////////////////////////////////////
// 09/12/05 - Michael C. Lee - function created.
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall OEMRestoreCurrentSettings( MSIHANDLE hInstall )
{
	HKEY hKey = NULL;
	DWORD dwDebug = 0;
	DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
			dwDebug = 0;
		RegCloseKey(hKey);
	}

	HKEY hKeySrc=NULL, hKeyDest=NULL;
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE\\Symantec"), NULL, KEY_READ, &hKeySrc ) )
	{	
		if (dwDebug)
			MessageBox( NULL, "Opened Saved Symantec OEM Key.", _T("Debug"), MB_OK );

		if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyDest, NULL ) )
		{
			if (dwDebug)
				MessageBox( NULL, "Successfully created new Symantec Key", _T("Debug"), MB_OK );

			CopyKeys(hKeySrc, hKeyDest, _T("OEM"));
			RegCloseKey(hKeyDest);

			if (dwDebug)
				MessageBox( NULL, "OEM Key Restored", _T("Debug"), MB_OK );

		}
		RegCloseKey(hKeySrc);
	}

	return ERROR_SUCCESS;
}

//----------------------------------------------------------------------------
// 03/02/06 Michael Lee --- Function created
extern "C" __declspec(dllexport) UINT __stdcall InstallSymOEMSvc( MSIHANDLE /*hInstall*/ )
{
	UINT	nRet = ERROR_SUCCESS;
	try
	{
		TCHAR szDir[1024];
		DWORD dwDebug = 0;
		HKEY hKey = NULL;
		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD);
			if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
				dwDebug = 0;
			RegCloseKey(hKey);
		}

		if (dwDebug)
			MessageBox( NULL, _T("InstallSymOEMSvc"), _T("Debug"), MB_OK );

		 
 		if (GetSymantecSharedDir(szDir, 1024, FALSE))
		{
			if (dwDebug)
			{
				MessageBox( NULL, _T("GotSymantecSharedDir"), _T("Debug"), MB_OK );
				MessageBox( NULL, szDir, _T("Debug"), MB_OK );
			}

			TCHAR szCmd[1024];
			PROCESS_INFORMATION pInfo;
			STARTUPINFO         sInfo;

			//
			// First stop the service if it's there
			//
			CString sCmd;
			sCmd.Format("/C net stop \"Symantec OEM Service\"");
			ShellExecute( NULL,"open","cmd",sCmd, ".",SW_HIDE);
			TCHAR szSymOEMSetupSetup[1024];

			//
			// Now Install the Service
			//
			sInfo.cb              = sizeof(STARTUPINFO);
			sInfo.lpReserved      = NULL;
			sInfo.lpReserved2     = NULL;
			sInfo.cbReserved2     = 0;
			sInfo.lpDesktop       = NULL;
			sInfo.lpTitle         = NULL;
			sInfo.dwFlags         = STARTF_FORCEOFFFEEDBACK | STARTF_USESHOWWINDOW; // Don't show a wait cursor when spawning 2nd instance.
			sInfo.dwX             = 0;
			sInfo.dwY             = 0;
			sInfo.dwFillAttribute = 0;
			sInfo.wShowWindow     = SW_SHOW;

			sssnprintf(szSymOEMSetupSetup, sizeof(szSymOEMSetupSetup), _T("%s\\OEM\\SymOEMSvc.exe"), szDir);
			sssnprintf(szCmd, sizeof(szCmd), _T("\"%s\" -Service"), szSymOEMSetupSetup);
			if (dwDebug)
				MessageBox( NULL, szCmd, _T("Debug"), MB_OK);
			if (CreateProcess(NULL, szCmd, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo)) 
			{
				if (dwDebug)
					MessageBox( NULL, _T("Create Process succeeded"),  _T("Debug"), MB_OK );

				while (WaitForSingleObject(pInfo.hProcess, 1000 * 60) == WAIT_TIMEOUT) 
				{

				}

				if (pInfo.hProcess)
					CloseHandle(pInfo.hProcess);

				if (pInfo.hThread)
					CloseHandle(pInfo.hThread);

			}
			else
			{
				DWORD dwError = GetLastError();
				TCHAR szMsg[64] = _T("");
				_stprintf(szMsg, _T("CreateProcess Failed with %d"), dwError);
				if (dwDebug)
					MessageBox( NULL, szMsg, _T("Debug"), MB_OK);
			}
			//
			// Now Start the Service
			//
			sCmd.Format("/C net start \"Symantec OEM Service\"");
			ShellExecute( NULL,"open","cmd",sCmd, ".",SW_HIDE);
		
		}
	}
	catch( UINT )
	{
	}
	return nRet;
}

// ---------------------------------------------------------------------------
// Routine:	UnInstallSymOEMSvc
//
// Purpose:	UnInstalls the Symantec OEM/COM Service
//
//----------------------------------------------------------------------------
// 03/02/06 Michael Lee --- Function created
extern "C" __declspec(dllexport) UINT __stdcall UnInstallSymOEMSvc( MSIHANDLE /*hInstall*/ )
{
	UINT	nRet = ERROR_SUCCESS;
	try
	{
		TCHAR szDir[1024];
		DWORD dwDebug = 0;
		HKEY hKey = NULL;
		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\OEM"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD);
			if (SymSaferRegQueryValueEx(hKey, _T("Debug"), NULL, NULL, LPBYTE(&dwDebug), &dwSize) != ERROR_SUCCESS)
				dwDebug = 0;
			RegCloseKey(hKey);
		}

		if (dwDebug)
			MessageBox( NULL, _T("UnInstallSymOEMSvc"), _T("Debug"), MB_OK );

		 
 		if (GetSymantecSharedDir(szDir, 1024, FALSE))
		{
			if (dwDebug)
			{
				MessageBox( NULL, szDir, _T("Debug"), MB_OK );
			}

			TCHAR szCmd[1024];
			PROCESS_INFORMATION pInfo;
			STARTUPINFO         sInfo;

			//
			// First stop the service if it's there
			//
			CString sCmd;
			sCmd.Format("/C net stop \"Symantec OEM Service\"");
			ShellExecute( NULL,"open","cmd",sCmd, ".",SW_HIDE);
			TCHAR szSymOEMSetupSetup[1024];
	
			//
			// Now do the uninstall
			//
			sInfo.cb              = sizeof(STARTUPINFO);
			sInfo.lpReserved      = NULL;
			sInfo.lpReserved2     = NULL;
			sInfo.cbReserved2     = 0;
			sInfo.lpDesktop       = NULL;
			sInfo.lpTitle         = NULL;
			sInfo.dwFlags         = STARTF_FORCEOFFFEEDBACK | STARTF_USESHOWWINDOW; // Don't show a wait cursor when spawning 2nd instance.
			sInfo.dwX             = 0;
			sInfo.dwY             = 0;
			sInfo.dwFillAttribute = 0;
			sInfo.wShowWindow     = SW_SHOW;

			sssnprintf(szSymOEMSetupSetup, sizeof(szSymOEMSetupSetup), _T("%s\\OEM\\SymOEMSvc.exe"), szDir);
			sssnprintf(szCmd, sizeof(szCmd), _T("\"%s\"  -UnRegServer"), szSymOEMSetupSetup);
			if (dwDebug)
				MessageBox( NULL, szCmd, _T("Debug"), MB_OK);
			if (CreateProcess(NULL, szCmd, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo)) 
			{
				if (dwDebug)
					MessageBox( NULL, _T("Create Process succeeded"),  _T("Debug"), MB_OK );

				while (WaitForSingleObject(pInfo.hProcess, 1000 * 60) == WAIT_TIMEOUT) 
				{

				}

				if (pInfo.hProcess)
					CloseHandle(pInfo.hProcess);

				if (pInfo.hThread)
					CloseHandle(pInfo.hThread);

			}
			else
			{
				DWORD dwError = GetLastError();
				TCHAR szMsg[64] = _T("");
				_stprintf(szMsg, _T("CreateProcess Failed with %d"), dwError);
				if (dwDebug)
					MessageBox( NULL, szMsg, _T("Debug"), MB_OK);
			}
		
		}
	}
	catch( UINT )
	{
	}
	return nRet;
}
