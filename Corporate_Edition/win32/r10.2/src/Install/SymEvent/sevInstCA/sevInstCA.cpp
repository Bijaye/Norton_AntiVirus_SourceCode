// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include <regstr.h>
#include "sevInstCA.h"
#include "sevInstallWrapper.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

#define KEY_WOW64_64KEY         (0x0100)

#define DEFAULT_COMMONFILES				_T("Common Files")
#define DEFAULT_PROGRAMFILES			_T("Program Files")
#define DEFAULT_COMMONFILESDIR			DEFAULT_PROGRAMFILESDIR _T("\\") DEFAULT_COMMONFILES
#define DEFAULT_SYMANTEC_SHARED			_T("Symantec Shared")

extern "C" __declspec(dllexport) UINT __stdcall BuildSymEventCAData( MSIHANDLE hInstall )
{
	PMSIHANDLE	    databaseHandle                      = NULL;
    CString         propertyString;
    CString         workString;
    TCHAR           dataBuffer[MAX_PATH]                = {""};
    DWORD           dataBufferSize                      = sizeof(dataBuffer)/sizeof(dataBuffer[0]);
    DWORD           returnVal                           = ERROR_FILE_NOT_FOUND;
	
    databaseHandle = MsiGetActiveDatabase(hInstall);
    if (databaseHandle != NULL)
    {
        CSevInstallWrapper sevInstall(databaseHandle);
        returnVal = sevInstall.openTable();
        if (returnVal == ERROR_SUCCESS)
        {
            // Pack all records from the sevinst table into a property that will be passed to our custom
            // action that does the work
            while (sevInstall.getNext() == ERROR_SUCCESS)
            {
                workString.Format("%s;%s;%s;%s;", sevInstall.getID(), sevInstall.getInstCmd(), sevInstall.getRmveCmd(), sevInstall.getDivision());
                propertyString += workString;
                MSILogMessage(hInstall, workString);
            }
            returnVal = ERROR_SUCCESS;
            sevInstall.closeTable();
			
            // Append a flag indicating if this is Win64.  The EXE name for this version of sevinst is slightly
            // different than for Win32.
			
			// Don't forget that MsiGetProperty returns ERROR_SUCCESS if you try and retrieve a property
			// that does not exist.  You just get back a null string.
            if( MsiGetProperty(hInstall, "VersionNT64", (LPTSTR) &dataBuffer, &dataBufferSize) == ERROR_SUCCESS )
			{
				if( dataBuffer[0] != NULL )
				{
					// VersionNT64 defined...
					propertyString += "1";
				}
				else
				{
					// VersionNT64 not defined...
					propertyString += "0";
				}
			}
			else
			{
				// The chance of this happening is remote.  Just consider it defensive programming...
				MSILogMessage(hInstall, "Error calling MsiGetProperty on VersionNT64 property.  Defaulting to 32 bit.");
				propertyString += "0";
			}
			
            // Create the MSI property
            if (MsiSetProperty(hInstall, SEVINSTPROP, (LPCTSTR) propertyString) != ERROR_SUCCESS)
                MSILogMessage(hInstall, "Error setting interface property!");
        }
        else
        {
            MSILogMessage(hInstall, "Error opening the sevinstall table.");
        }
        MsiCloseHandle(databaseHandle);
        databaseHandle = NULL;
    }
    else
    {
        MSILogMessage(hInstall, "Unable to obtain active database handle.");
    }
	
	return returnVal;
}

extern "C" __declspec(dllexport) UINT __stdcall RegisterWithSymEvent( MSIHANDLE hInstall )
{
	UINT			nRet = ERROR_SUCCESS;
	CString			strData;
	CString			strTemp;
	CStringArray	strArray;
    bool            isWin64                             = false;
	
	try
	{
		strData = getCustomActionData(hInstall);
		parseCustomActionData(strData, &strArray, hInstall);
		
		strTemp.Format("Registering %s (command line:%s)...", strArray[0], strArray[1]);
        if (strArray[strArray.GetUpperBound()] == "1")
            isWin64 = true;
        else
            isWin64 = false;
		if (!launchSevInst(strArray[0], strArray[1], strArray[3], hInstall, isWin64))
			MSILogMessage(hInstall, _T("Failed to register"));
		
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}
	// this code is no longer acceptable in the vc7.1 compiler. DKowalyshyn 4/8/04
	//An exception was caught by value but the copy constructor and/or the assignment operator were inaccessible.
	//This code was accepted by the previous version's compiler but now gives an error. For more information, see Summary of Compile-Time Breaking Changes.
//	catch( CMemoryException cErr )
//	{
//		MSILogMessage( hInstall, _T("Memory allocation problem") );
//	}
	
	return nRet;
}

extern "C" __declspec(dllexport) UINT __stdcall UnregisterFromSymEvent( MSIHANDLE hInstall)
{
	UINT			nRet = ERROR_SUCCESS;
	CString			strData;
	CString			strTemp;
	CStringArray	strArray;
    bool            isWin64                             = false;
	
	try
	{
		strData = getCustomActionData( hInstall );
		parseCustomActionData(strData, &strArray);
		
		// Check to see if the /u parameter is present and if not add it
		strTemp = strArray[2];
		if( ( -1 == strTemp.Find("/u") ) && ( -1 == strTemp.Find("/U") ) )
		{
			strTemp += " /u";
			strArray[2] = strTemp;
		}
        if (strArray[strArray.GetUpperBound()] == "1")
            isWin64 = true;
        else
            isWin64 = false;
		
		strTemp.Format( "Unregistering %s (command line:%s)...", strArray[0], strArray[2] );
		if( !launchSevInst( strArray[0], strArray[2], strArray[3], hInstall, isWin64 ) )
			MSILogMessage( hInstall, _T("Failed to un-register") );
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}
	// this code is no longer acceptable in the vc7.1 compiler. DKowalyshyn 4/8/04
	//An exception was caught by value but the copy constructor and/or the assignment operator were inaccessible.
	//This code was accepted by the previous version's compiler but now gives an error. For more information, see Summary of Compile-Time Breaking Changes.
//	catch( CMemoryException cErr )
//	{
//		MSILogMessage( hInstall, _T("Memory allocation problem") );
//	}
	
	return nRet;
}

UINT MSILogMessage( MSIHANDLE hInstall, CString strMessage)
{
	UINT nRet = ERROR_SUCCESS;
	CString szLogLine( LOG_PREFIX );
	
	szLogLine += strMessage;
	
	PMSIHANDLE hRec = MsiCreateRecord( 1 );
	
	if( hRec )
	{
		MsiRecordClearData( hRec );
		MsiRecordSetString( hRec, 0, (LPCSTR)szLogLine );
		MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
	}
	
	// Also output this message to the debug window
	OutputDebugString( strMessage );
	
	return nRet;
}

CString getCustomActionData( MSIHANDLE hInstall )
{
	CString strData;
	char	*pCAData = NULL;
	DWORD	dwLen = 0;
	
	try
	{
		// Get the size of the data
		if( ERROR_MORE_DATA != MsiGetProperty( hInstall, _T("CustomActionData"), "", &dwLen ) )
			throw( _T("Error getting length of CustomActionData") );
		
		// Allocate our memory and grab the data, add one for the trailing null
		pCAData = new char[ ++dwLen ];
		if( !pCAData )
			throw( _T("Unable to allocate memory for CustomActionData buffer") );
		
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("CustomActionData"), pCAData, &dwLen ) )
			throw( _T("Unable to obtain the CustomActionData") );
		
		strData.Format( "%s", pCAData );
		delete [] pCAData;
	}
	catch( TCHAR *szErr )
	{
		// Clean up
		if( pCAData )
			delete [] pCAData;
		
		// Throw the error back up
		throw( szErr );
	}	
	
	return strData;
}

BOOL GetProgramFilesDir( LPTSTR pszPath, UINT cbSize, bool isWin64 )
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
        lResult = SymSaferRegQueryValueEx(hKey, "ProgramFilesDir", NULL, NULL, LPBYTE(pszPath), &dwSize);
        RegCloseKey(hKey);
    }
	
    // manufacture if not good to here
    if (lResult != ERROR_SUCCESS)
    {
        // manufacture program files location
        TCHAR szDirName[ MAX_PATH ] = {0};
        GetWindowsDirectory(szDirName, sizeof(szDirName)); //lint !e534
        lstrcpy(szDirName + 3, DEFAULT_PROGRAMFILES);
		
        // add key by open/create
        if (CreateDirectory(szDirName, NULL))
        {
            lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | regOptions, NULL, &hKey, NULL);
            if (lResult == ERROR_SUCCESS)
            {
                lResult = RegSetValueEx(hKey, "ProgramFilesDir", 0L, REG_SZ, LPBYTE(szDirName), (DWORD(lstrlen(szDirName)) + 1UL) * sizeof(TCHAR));
                RegCloseKey(hKey);
            }
        }
    }
	
    return (lResult == ERROR_SUCCESS);
}

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
		lResult = SymSaferRegQueryValueEx(hKey, "CommonFilesDir", NULL, NULL, LPBYTE(pszPath), &dwSize);
		RegCloseKey(hKey);
	}
	
	// manufactur if not good to here
	if (lResult != ERROR_SUCCESS)
	{
		TCHAR szDirName[ MAX_PATH ] = {0};
		if (GetProgramFilesDir(szDirName, sizeof(szDirName), isWin64))
		{
			// manufacture program files location
			lstrcat(szDirName, _T("\\") DEFAULT_COMMONFILES);
			if (CreateDirectory(szDirName, NULL))
			{
	            lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | regOptions, NULL, &hKey, NULL);
				if (lResult == ERROR_SUCCESS)
				{
					lResult = RegSetValueEx(hKey, "CommonFilesDir", 0L, REG_SZ, LPBYTE(szDirName), (lstrlen(szDirName) + 1) * sizeof(TCHAR));
					RegCloseKey(hKey);
				}
			}
		}
	}
	
	return (lResult == ERROR_SUCCESS);
}

BOOL GetSymantecSharedDir( LPTSTR pszPath, UINT cbSize, bool isWin64 )
//Sets pszPath to the Symantec Common directory (Common Files\Symantec Shared)
//Returns TRUE on success, else FALSE
//Ripped from Phoenix (NSW 2) - originally RegGetSymantecSharedDir
{
	BOOL fResult = FALSE;
	if (GetCommonFilesDir(pszPath, cbSize, isWin64))
	{
		TCHAR szTemp[MAX_PATH] = {""};
		
		lstrcat(pszPath, _T("\\") DEFAULT_SYMANTEC_SHARED);
		fResult = TRUE;
	}
	return fResult;
}

int parseCustomActionData( CString packedString, CStringArray* dataFields, MSIHANDLE /*hInstall*/ )
{
	StringList      words;
    int             currFieldNo                         = 0;
	
	Breakapart((LPCTSTR) packedString, &words);
	for (StringList::iterator currWord = words.begin(); currWord != words.end(); currWord++)
	{
		dataFields->Add(currWord->c_str());
	}
	if (dataFields->GetSize() < NUM_FIELDS)
	{
		for (currFieldNo = 0; currFieldNo < (NUM_FIELDS-dataFields->GetSize()); currFieldNo++)
			dataFields->Add("");
	}
	return dataFields->GetSize();
}

BOOL launchSevInst( CString strID, CString strCmdLine, CString strOption, MSIHANDLE hInstall, bool isWin64 )
{
	BOOL            bRet                                = FALSE;
    LPCTSTR         executableName                      = NULL;
	TCHAR	        symantecSharedDir[MAX_PATH]			= {""};
	CString         strCommandLine;
	LPTSTR			commandLineLPTSTR					= NULL;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	// Set up the process info
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	
	GetSymantecSharedDir(symantecSharedDir, sizeof(symantecSharedDir), isWin64);
	
    if (isWin64)
		// Only call IsAMD64 if we know we are on a 64 bit machine.
		if( IsAMD64( hInstall ) )
			executableName = SEVINSTALLERAMD64;
		else
			executableName = SEVINSTALLER64;
    else
        executableName = SEVINSTALLER;
	// Check for consumer option and build command line
	if( strOption.GetLength() > 0 )
		strCommandLine.Format( "\"%s\\%s\" %s /CONSUMER %s", symantecSharedDir, executableName, strCmdLine, strID );
	else
        strCommandLine.Format( "\"%s\\%s\" %s %s", symantecSharedDir, executableName, strCmdLine, strID );
	// Check to see if we are in a migrate mode
	if( mutexActive() )
		strCommandLine += " /NODEL";
	
	// Launch sevinst.exe
	OutputDebugString( strCommandLine );
	commandLineLPTSTR = strCommandLine.LockBuffer();
	if( CreateProcess(	NULL,
		commandLineLPTSTR,
		NULL,
		NULL,
		TRUE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&si,
		&pi ) )
	{
		WaitForSingleObject( pi.hProcess, INFINITE );
		CloseHandle( pi.hThread );
		CloseHandle( pi.hProcess );
		bRet = TRUE;
	}
	else
	{
		DWORD exitCode = 0;
		DWORD lastError = 0;
		TCHAR errorCode[MAX_PATH] = {""};
		
		lastError = GetLastError();
		GetExitCodeProcess(pi.hProcess, &exitCode);
		sssnprintf(errorCode, sizeof(errorCode), "Process create failed, exit code %d, last error %d, command line %s.", exitCode, lastError, commandLineLPTSTR);
		MSILogMessage(hInstall, errorCode);
	}
	
	return bRet;
}

BOOL mutexActive()
{
	BOOL bRet = FALSE;
	
	HANDLE hMigrating = OpenMutex( NULL, TRUE, SEVINSTMUTEX );
	if( !hMigrating )
		hMigrating = OpenMutex( NULL, TRUE, GSEVINSTMUTEX ); // Try the global one if local failed
	else
		bRet = TRUE;
	
	if( hMigrating )
	{
		ReleaseMutex( hMigrating );
		CloseHandle( hMigrating );
	}
	
	return bRet;
}

HRESULT Breakapart( string inputString, StringList* words, char separator )
// Breaks inputString into an array of words delimited by separator
{
	string::size_type	currPos				= string::npos;
	string::size_type	separatorPos		= string::npos;
	string::size_type	inputStringLen		= string::npos;
	
	// Validate parameters
	if (words == NULL)
		return E_POINTER;
	
	// Base case
	if (inputString == "")
	{
		words->clear();
		return S_OK;
	}
	
	currPos = 0;
	inputStringLen = inputString.length();
	while (currPos < inputStringLen)
	{
		separatorPos = inputString.find_first_of(separator, currPos);
		if (separatorPos != string::npos)
		{
			words->push_back(inputString.substr(currPos, separatorPos - currPos));
			currPos = separatorPos + 1;
		}
		else
		{
			words->push_back(inputString.substr(currPos, inputStringLen - currPos));
			currPos = inputStringLen+1;
		}
	}
	
	return S_OK;
}

// Determins if we are on an AMD64 machine in WOW64 mode.
// Stolen from Common\src\PScan\driver.cpp
BOOL IsAMD64( MSIHANDLE hInstall )
{
	MSILogMessage( hInstall, _T("Checking for AMD64 or compatible processor.") );

	BOOL bRetval = FALSE;
    SYSTEM_INFO tSysInfo;
    memset(&tSysInfo, 0, sizeof(tSysInfo));
    HMODULE hModKernel = NULL;

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\KERNEL32.DLL"));
	hModKernel = LoadLibrary(szPath);

    if( NULL != hModKernel )
    {
        pfGetNativeSystemInfo GetNativeSystemInfo_pfn = NULL;
        GetNativeSystemInfo_pfn = (pfGetNativeSystemInfo)GetProcAddress(hModKernel,"GetNativeSystemInfo");
        if( NULL != GetNativeSystemInfo_pfn )
        {
            GetNativeSystemInfo_pfn(&tSysInfo);
			if( PROCESSOR_ARCHITECTURE_AMD64 == tSysInfo.wProcessorArchitecture )
			{
				MSILogMessage( hInstall, _T("Found AMD64 or compatible processor.") );
				bRetval = TRUE;
			}
        }
        FreeLibrary(hModKernel);
    }
    return bRetval;
}
