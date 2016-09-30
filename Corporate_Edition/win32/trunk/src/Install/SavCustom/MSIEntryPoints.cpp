// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// MSIEntryPoints.cpp
//
#include "stdafx.h"
#define INITIIDS

#include <windows.h>
#include <Winsock2.h>
#include <MSI.H>

#include "vpcommon.h"
#include "SavCustom.h"
#include "MSIEntryPoints.h"
#include "resource.h"
#include "SymSaferRegistry.h"
#include "vpstrutils.h"
#include "SymSaferStrings.h"
#include <list>
#include <string>
#include <utility>

// Added for OEM below
#include "oem.h"
#include "oem_common.h"
#include "oem_constants.h"
#include "regstr.h"
#define OEM_INSTALL_SWITCH	_T("OEM")
#define MAX_SERVER_NAME			48	  // limit imposed by LDVP technology
#define MIN_PASSWORD_LENGTH		6

#define KEY_WOW64_64KEY					            (0x0100)
#define REGVALUE_PENDINGFILERENAMES                 _T("PendingFileRenameOperations")
#define REGKEY_WINDOWSSESSIONMANAGER                _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager")
#define PROPERTY_MSIFILTERREBOOTMODE_ALREADYSET     _T("MsiFilterRebootMode_RebootAtEndModeBefore")
// Exclusions does not need to be a public property, but according to MSI docs it appears that it must be to be accessible in the execute sequence
#define PROPERTY_MSIFILTERREBOOTMODE_EXCLUSIONS     _T("MSIFILTERREBOOTMODE_FILENAMEEXCLUSIONS")
// List of properties which, if set, indicate that something else set the reboot mode
// Necessary because deferred CA's can't access the reboot-at-end mode
#define PROPERTY_MSIFILTERREBOOTMODE_ALREADYSETPROPS    _T("MSIFILTERREBOOTMODE_ALREADYSETPROPS")
#define DEFAULT_COMMONFILES				            _T("Common Files")
#define DEFAULT_PROGRAMFILES			            _T("Program Files")
#define DEFAULT_COMMONFILESDIR			            DEFAULT_PROGRAMFILESDIR _T("\\") DEFAULT_COMMONFILES
#define DEFAULT_SYMANTEC_SHARED			            _T("Symantec Shared")
 
typedef std::basic_string<TCHAR> tstring;
typedef std::pair<tstring, tstring> StringPair;
typedef std::list<StringPair> StringPairList;
typedef std::list<tstring> StringList;

// Returns S_OK on success, S_FALSE if no data, HFW32(ERROR_DATATYPE_MISMATCH) if PendingfileRenameOperations exists but is not a REG_MULTI_SZ
HRESULT GetPendingFileOperations( StringPairList* pendingOperations );
// Filters the passed list of pending file operations.  If removeDeletes is true, removes all deletes from the list.
// filenameUpdatesToRemove is an optional list of filenames being updated to remove from the list
// Except for argument validation, always succeeds
HRESULT FilterPendingFileOperationsList( StringPairList* pendingFileOperations, bool removeDeletes, const StringList* filenameUpdatesToRemove );
// Parses the strings out of a semicolon delimited list of strings
HRESULT ParseDelimitedStringList( LPCTSTR packedFilenameList, StringList* filenameList );
// Sets *nodeName equal to the file node name - the filename after the final path separator, if any.
HRESULT GetNodeName( const tstring& pathname, tstring* nodeName );

///////////////////////////////////////////////////////////////////////////////
//
//  Function: AddSlash
//
//  Simply adds a slash to the end of a path, if needed.
//
//  08/16/2003 Thomas Brock
//
///////////////////////////////////////////////////////////////////////////////
void __fastcall AddSlash( char* pszPath )
{
	if( pszPath[ strlen( pszPath ) - 1 ] != '\\' )
	{
		strcat( pszPath, "\\" );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function: SetMSIEditControlText
//
//  Send WM_SETTEXT message to the specified edit control on the current
//	MSI UI pane. This will cause MSI to show its updated property value.
//
//  02/24/2005 Sean Kennedy
//
///////////////////////////////////////////////////////////////////////////////
void SetMSIEditControlText( DWORD control )
{
	// Find the MSI UI Window
	HWND msiWindow = FindWindow( "MsiDialogCloseClass", NULL );
	if( NULL != msiWindow )
	{
		DWORD current = 0;
		HWND msiChild = NULL;
		HWND msiLast = NULL;
		do
		{
			// Find any edit controls on the window
			msiChild = FindWindowEx( msiWindow, msiLast, "RichEdit20W", NULL );
			if( msiChild )
			{
				++current;
				if( current == control )
				{
					OutputDebugString( "Sending WM_SETTEXT message" );
					SendMessage( msiChild, WM_SETTEXT, 0L, 0L );
					break;
				}
			}
			msiLast = msiChild;
		} while( NULL != msiChild );
	}
	else
		OutputDebugString( "Unable to find MSI window" );
}


// ---------------------------------------------------------------------------
// Routine:	CheckInstallPath
//
// Purpose:	Make sure install path contains valid information
//
// ---------------------------------------------------------------------------
// 04/11/02 Joe Antonelli Created function
// 10/03/03 Sean Kennedy - Updated function and imported to SCF project
// 12/18/03 Sean Kennedy - Imported to SCS project
// 02/22/04 Dan Kowalyshyn - Changes to block large install paths
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) UINT __stdcall CheckInstallPath( MSIHANDLE hInstall )
{
    UINT	nRet = ERROR_SUCCESS;
    TCHAR	lpszPath[DOUBLE_MAX_PATH];
	TCHAR	szTemp[DOUBLE_MAX_PATH];
    DWORD	dwSize = DOUBLE_MAX_PATH;
    bool	bValidPath = true;
    int     iLen = 0;

	try
	{
		if ( ERROR_SUCCESS != MsiGetProperty( hInstall, "INSTALLDIR", lpszPath, &dwSize ) )
			throw( (UINT)IDS_ERR_NOPATH );
		vpsnprintf(szTemp, sizeof (szTemp), "INSTALLDIR = %s.",lpszPath);
		MSILogMessage( hInstall, szTemp, TRUE );

		// Do not allow NULL path
		if ( !lpszPath )
			throw( (UINT)IDS_ERR_BLANKPATH );

		// path must be greater than 3 characters (path too short)
		iLen = lstrlen(lpszPath);
		if ( iLen <= 3 )
			throw( (UINT)IDS_ERR_PATHTOOSHORT );

		// path must not be greater then MAX_PATH_LENGTH, need room for pki directory & certs 
		if ( iLen >= MAX_PATH_LENGTH )
		{
			vpsnprintf(szTemp, sizeof (szTemp), "INSTALLDIR length = %d.",iLen);
			MSILogMessage( hInstall, szTemp, TRUE );
			throw( (UINT)IDS_ERR_PATHTOOLONG );
		}

		TCHAR szPath[MAX_PATH];
		lstrcpy(szPath, lpszPath);
		if ((szPath[ iLen-1 ] == _T('\\')) && !IsDBCSLeadByte(BYTE(szPath[ iLen-2 ])))
			szPath[ iLen - 1 ] = 0;

		// Ensure no portion of path includes leading spaces.
		// (i.e.  "c:\ program files\  ssc\blah".
		LPCTSTR pszChar = _tcschr(szPath, _T('\\'));

		while (pszChar && *pszChar && (_T(' ') != *CharNext(pszChar)))
		{
			pszChar = CharNext(pszChar);
			pszChar = _tcschr(pszChar, _T('\\'));
		}

		if (pszChar && *pszChar && (_T(' ') == *CharPrev(szPath, pszChar)))
			throw( (UINT)IDS_ERR_NOLEADSPACE );

		// Ensure no portion of the path includes a ';' character
		// i.e. "c:\progra\a;b\SSC"
		pszChar = _tcschr(szPath, _T('\\'));

		while (pszChar && *pszChar &&  (_T(';') != *CharNext(pszChar)))
		{
			pszChar = CharNext(pszChar);
		}

		if (pszChar && *pszChar &&  (_T(';') == *CharNext(pszChar)))
			throw( (UINT)IDS_ERR_NOSEMICOLON );

		// Ensure no portion of the path includes a '&' character
		// i.e. "c:\Joe\SSC&\"
		pszChar = _tcschr(szPath, _T('\\'));
		while (pszChar && *pszChar &&  (_T('&') != *CharNext(pszChar)))
		{
			pszChar = CharNext(pszChar);
		}

		if (pszChar && *pszChar &&  (_T('&') == *CharNext(pszChar)))
			throw( (UINT)IDS_ERR_NOAMPERSAND );
	}
	catch( UINT nErr )
	{
		// Set valid path to false for any exceptions we trap
		bValidPath = false;

		// Get our error strings
		CString strMessage;
		CString strTitle;
		strTitle.LoadString( IDS_PATHTITLE );
		strMessage.LoadString( nErr );

		// Check the UI Level
		TCHAR szUILevel[ MAX_PATH ];
		DWORD dwSize = sizeof( szUILevel );
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, "UILevel", szUILevel, &dwSize ) )
		{
			int uiLevel = atoi( szUILevel );

			if( uiLevel == 2 )
			{
				// In silent mode there is nothing we can do to fix the path so we must abort
				nRet = ERROR_INSTALL_USEREXIT;
			}
			else
			{
				// If in interactive mode display a message
				HWND hWnd = GetForegroundWindow();
				if( !hWnd )
					hWnd = GetDesktopWindow();
				// format the strMessage to include the MAX_PATH_LENGTH in the messagebox
				if ( nErr == IDS_ERR_PATHTOOLONG )
				{
					strMessage.Format(IDS_ERR_PATHTOOLONG, iLen, MAX_PATH_LENGTH);
				}
				MessageBox( hWnd, strMessage, strTitle, MB_OK );
			}
		}

		// Always log the message
		MSILogMessage( hInstall, strMessage.LockBuffer(), TRUE );
		strMessage.UnlockBuffer();
	}

    if( true == bValidPath )
    {
        MsiSetProperty( hInstall, "ValidInstallDir", "1" );
    }
    else
    {
        MsiSetProperty( hInstall, "ValidInstallDir", "0" );
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
void MSILogMessage( MSIHANDLE hInstall, LPCTSTR szString, BOOL bDoAction )
{
	CString szLogLine( LOG_PREFIX );

	szLogLine += szString;

	if( FALSE == bDoAction )
	{
		PMSIHANDLE hRec = MsiCreateRecord( 1 );

		if( hRec )
		{
			MsiRecordClearData( hRec );
			MsiRecordSetString( hRec, 0, (LPCSTR)szLogLine );
			MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
		}
	}

	OutputDebugString( szLogLine );
}

/////////////////////////////////////////////////////////////////////////
//
// Function: IsLicensedOverUnlicensed
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
// Returns:
//		ERROR_SUCCESS
//
// Description:  
//	This function checks for installing a licensed version over an unlicensed version
//    - set the LICENSEDOVERUNLICENSED property
//  This is only called if installing a licensed version
//
//////////////////////////////////////////////////////////////////////////
// 6/14/04 - Jim Shock - function created.
//////////////////////////////////////////////////////////////////////////
// void IsLicensedOverUnlicensed( MSIHANDLE hInstall )
extern "C" __declspec(dllexport) UINT __stdcall IsLicensedOverUnlicensed( MSIHANDLE hInstall )
{

	MSILogMessage( hInstall, _T("Checking for Licensed over Unlicensed migration...") );

	// determine if the existing product is licensed or unlicensed
	DWORD dwLicensed = 0;
	HKEY hKey = NULL;
	DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\Licensing", NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		SymSaferRegQueryValueEx(hKey, "Enabled", NULL, NULL, LPBYTE(&dwLicensed), &dwSize);
		RegCloseKey(hKey);
	}
	if (dwLicensed == 0)
	{
		MSILogMessage( hInstall, _T("Doing Licensed over Unlicensed migration.") );
		MsiSetProperty( hInstall, _T("LICENSEDOVERUNLICENSED"), _T("1") );
	}
	else
	{
		MSILogMessage( hInstall, _T("Not doing Licensed over Unlicensed migration.") );
	}

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////
//
// Function: IsUnlicensedOverLicensed
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
// Returns:
//		ERROR_SUCCESS
//
// Description:  
//	This function checks for installing an unlicensed version over a licensed version
//    - set the UNLICENSEDOVERLICENSED property
//  This is only called if installing an unlicensed version
//
//////////////////////////////////////////////////////////////////////////
// 6/14/04 - Jim Shock - function created.
//////////////////////////////////////////////////////////////////////////
// void IsUnlicensedOverLicensed( MSIHANDLE hInstall )
extern "C" __declspec(dllexport) UINT __stdcall IsUnlicensedOverLicensed( MSIHANDLE hInstall )
{

	MSILogMessage( hInstall, _T("Checking for Unlicensed over Licensed migration...") );

	// determine if the existing product is licensed or unlicensed
	DWORD dwLicensed = 0;
	HKEY hKey = NULL;
	DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\Licensing", NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		SymSaferRegQueryValueEx(hKey, "Enabled", NULL, NULL, LPBYTE(&dwLicensed), &dwSize);
		RegCloseKey(hKey);
	}
	if (dwLicensed == 1)
	{
		MSILogMessage( hInstall, _T("Doing Unlicensed over Licensed migration.") );
		MsiSetProperty( hInstall, _T("UNLICENSEDOVERLICENSED"), _T("1") );
	}
	else
	{
		MSILogMessage( hInstall, _T("Not doing Unlicensed over Licensed migration.") );
	}

	return ERROR_SUCCESS;

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
 		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\OEM", NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
 		if (dwResult == ERROR_SUCCESS)
 		{
 			DWORD dwSize = sizeof(DWORD);
 			SymSaferRegQueryValueEx(hKey, "Debug", NULL, NULL, LPBYTE(&dwDebug), &dwSize);
 			RegCloseKey(hKey);
 		}
 
 		if (dwDebug)
 			MessageBox( NULL, "OEMSetOff", "SAVOEM", MB_OK );
 
 		bool	bValidPath = true;
 		int     iLen = 0;
 		int		nError = ERROR_SUCCESS;	
 
 		DWORD dwSize= 1024;
 		_tcscpy(lpszInstallDir, "");
 		// MCLEE - 4/13/2005 --- Use the OEM dir off of shared
	  	if (!GetSymantecSharedDir(lpszInstallDir, dwSize, FALSE))
 		{
 			if (dwDebug)
 				MessageBox( NULL, lpszInstallDir, "INSTALLDIR", MB_OK );
 
 			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "INSTALLDIR not retrievable");
 			MSILogMessage( hInstall, lpszLogMessage, TRUE );
 			throw( (UINT)IDS_ERR_NOPATH );
 		}
		else
		{
			_tcscat(lpszInstallDir, _T("\\OEM\\"));
 		}

 
 		// Get rid of trailing \\ so it won't mess up formatting below.
 		int i = _tcslen(lpszInstallDir) - sizeof(TCHAR);
 		TCHAR *pStr = lpszInstallDir + i;
 		if ((*pStr) == _T('\\'))
 			lpszInstallDir[i] = '\0';
 
 
 		TCHAR lpszOEMDLL[1024];
 		sssnprintf(lpszOEMDLL, sizeof(lpszOEMDLL), "%s\\%s", lpszInstallDir, OEMDLL);
 
 		// Use short path name. MSI likes this better
 		if (dwDebug)
 			MessageBox( NULL, "Convert SPN", "SAVOEM - SPN",  MB_OK );
 
 		TCHAR lpszOEMDLL_SPN[1024];
 		if (GetShortPathName(lpszOEMDLL, lpszOEMDLL_SPN, 1024))
 		{
 			if (dwDebug)
 				MessageBox( NULL, lpszOEMDLL_SPN, "SAVOEM - SPN",  MB_OK );
 			_tcscpy(lpszOEMDLL, lpszOEMDLL_SPN);
 		}
 
 		if (dwDebug)
 			MessageBox( NULL, lpszOEMDLL, "SAVOEM",  MB_OK );
 
 
 		HMODULE hOEM = LoadLibrary(lpszOEMDLL);
 		if (hOEM)
 		{
 			if (dwDebug)
 				MessageBox( NULL, "Loaded SymOEM.dll", "SAVOEM",  MB_OK );
 
 			
 			// ---------------------  Set OEM Install
 			lpfnSetOEM_Install SetOEM_Install = (lpfnSetOEM_Install) GetProcAddress(hOEM, _T("SetOEM_Install"));
 
 			if (dwDebug)
 			{
 				sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "SetOEM_Install = 0");
 				MessageBox( NULL, lpszLogMessage, "SAVOEM",  MB_OK );
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
 				sssnprintf(sError, sizeof(sError), "Error loading SymOEM.dll = %d", GetLastError());
 				MessageBox( NULL, sError, "SAVOEM",  MB_OK );
 			}
 		}
 	
 	}
 	catch( UINT )
 	{
 	}
 	return nRet;
 }
 

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiCleanTempFolder()
//
// Description: This function cleans up the temp folder on an installer
// cancel event.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 12/10/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiCleanTempFolder( MSIHANDLE hInstall )
{
	TCHAR			strTargetFile[MAX_PATH]				= {0};
	TCHAR			szFile[MAX_PATH]					= {0};
	TCHAR			szTempFolder[MAX_PATH]				= {0};
	TCHAR			szBuf[MAX_PATH]						= {0};
	DWORD			dwLen;

	// get the Install temp folder
	dwLen = sizeof( szTempFolder );
	if ( 0 < GetTempPath ((sizeof(szTempFolder)), szTempFolder))
	{
		sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder - GetTempPath succeeded %s", szTempFolder); 
		MSILogMessage( hInstall, szBuf );
		for ( int i = 0; i < NUM_TEMP_FOLDER_FILES; i++ )
		{
			lstrcpy( szFile, szTempFolder );
			AddSlash( szFile );
			lstrcat( szFile, pszDestFiles[i] );

			// Set file attribs so we can Deletefile or MoveFileEx them
			if ( !SetFileAttributes ( szFile, FILE_ATTRIBUTE_NORMAL ))
			{
				sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder - SetFileAttributes failed %s", szFile); 
				MSILogMessage( hInstall, szBuf );
			}
			else
			{
				sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder - SetFileAttributes succeeded %s", szFile); 
				MSILogMessage( hInstall, szBuf );
			}

			if( !DeleteFile( szFile ) )
			{
				sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder - DeleteFile failed %s", szFile); 
				MSILogMessage( hInstall, szBuf );
				MoveFileEx( szFile, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
			}
			else
			{
				sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder - DeleteFile succeeded %s", szFile); 
				MSILogMessage( hInstall, szBuf );
			}
		}
	}
	else
	{
		sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder - GetTempPath Failed %s", szTempFolder); 
		MSILogMessage( hInstall, szBuf );
	}
	return ERROR_SUCCESS;
}


#include "WinSvc.h"
// ---------------------------------------------------------------------------
 // Routine:	StartCCEvtMgr
 //
 // Purpose:	Custom Action to start CCEvtMgr after installation
 //             since it isn't started in SAVWin64
 //
 //----------------------------------------------------------------------------
 // 03/02/05 Michael Lee --- Function created
 extern "C" __declspec(dllexport) UINT __stdcall StartCCEvtMgr( MSIHANDLE hInstall )
 {
 	try
 	{
		SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
 		if (hSCM)
		{
			SC_HANDLE       hService;
			// ccEvtMgr service name won't change so use it as a constant string (rather than put into a resource file).
			hService = OpenService( hSCM, _T("ccEvtMgr"), SERVICE_START);
			if( hService != NULL )
			{
				StartService( hService, 0, NULL );
				CloseServiceHandle( hService );
			}
			CloseServiceHandle( hSCM );
		}
		
 	}
 	catch( UINT )
 	{
 	}
 	return ERROR_SUCCESS;  // Always return ERROR_SUCCESS since this is NOT a catastrophic error;
 }
 
//////////////////////////////////////////////////////////////////////////
//
// Function: MsiSetAtom()
//
// Parameters:
//      MSIHANDLE - handle to current install
//
// Returns:
//      UINT - ERROR_SUCCESS if all ok
//
// Description:  
//  
//
//////////////////////////////////////////////////////////////////////////
// 1/25/2006 - DLUXFORD function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall MsiSetAtom( MSIHANDLE installHandle )
// Sets the atom named by CustomActionAction to 1
{
    CString     customActionData;
    TCHAR       errorMessage[MAX_PATH]      = {_T("")};
    ATOM        returnVal                   = (ATOM) 0;
    
    customActionData = GetMSIProperty(installHandle, _T("MsiSetAtom_Atom"));
    returnVal = GlobalAddAtom((LPCTSTR) customActionData);
    if (returnVal != 0)
    {
		sssnprintf(errorMessage, sizeof (errorMessage), "MsiSetAtom:  Successfully set atom %s.", (LPCTSTR) customActionData);
        MSILogMessage(installHandle, errorMessage);
    }
	else
	{
		sssnprintf(errorMessage, sizeof (errorMessage), "MsiSetAtom:  Error %d adding atom %s", GetLastError(), (LPCTSTR) customActionData); 
        MSILogMessage(installHandle, errorMessage);
	}

    return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiGetAtom()
//
// Parameters:
//      MSIHANDLE - handle to current install
//
// Returns:
//      UINT - ERROR_SUCCESS if all ok
//
// Description:  
//  
//
//////////////////////////////////////////////////////////////////////////
// 1/25/2006 - DLUXFORD function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall MsiGetAtom( MSIHANDLE installHandle )
// Looks for an atom of the specified name.  If found, sets a property with the same name to 1.
{
    CString     customActionData;
    ATOM        atomID                      = (ATOM) 0;
    TCHAR       errorMessage[MAX_PATH]      = {_T("")};
	DWORD		returnValDW					= ERROR_OUT_OF_PAPER;
    
    customActionData = GetMSIProperty(installHandle, _T("MsiGetAtom_Atom"));
    atomID = GlobalFindAtom((LPCTSTR) customActionData);
    if (atomID != 0)
    {
        MsiSetProperty(installHandle, (LPCTSTR) customActionData, "1");
        sssnprintf(errorMessage, sizeof (errorMessage), "MsiGetAtom:  Atom %s found, setting property %s=1", (LPCTSTR) customActionData, (LPCTSTR) customActionData);
        MSILogMessage(installHandle, errorMessage);
		SetLastError(ERROR_SUCCESS);
		GlobalDeleteAtom(atomID);
		atomID = (ATOM) 0;
		returnValDW = GetLastError();
		if (returnValDW != ERROR_SUCCESS)
		{
			sssnprintf(errorMessage, sizeof (errorMessage), "MsiGetAtom:  Error %d deleting atom %s.", returnValDW, (LPCTSTR) customActionData);
			MSILogMessage(installHandle, errorMessage);
		}
    }
    else
    {
        sssnprintf(errorMessage, sizeof (errorMessage), "MsiGetAtom:  Atom %s not found", (LPCTSTR) customActionData);
        MSILogMessage(installHandle, errorMessage);
    }

    return ERROR_SUCCESS;
}

UINT _stdcall MsiFilterRebootMode_SaveStatus( MSIHANDLE installHandle )
// Saves the current status of the RebootAtEnd mode to a property so MsiFilterRebootMode can determine if anything
// else has set the RebootAtEnd mode.  Should be called just before InstallFinalize
{
	if (MsiGetMode(installHandle, MSIRUNMODE_REBOOTATEND))
	{
		MSILogMessage(installHandle, "MsiFilterRebootMode_SaveStatus:  Detected RebootAtEnd mode already set, recording status");
        MsiSetProperty(installHandle, PROPERTY_MSIFILTERREBOOTMODE_ALREADYSET, _T("1"));
	}

	return ERROR_SUCCESS;
}

UINT _stdcall MsiFilterRebootMode( MSIHANDLE installHandle )
// Filters the list of in-use files being updated and, if no unfiltered files remain, clears the MSI RebootAtEnd mode.
{
    StringList                  alreadySetPropertyFlags;
    StringList::iterator        currAlreadySetPropertyFlag;
    CString                     propertyValue;
	StringList					excludeFilenames;
	StringPairList		        pendingFileOperations;
	StringPairList::iterator    currPendingFileOperations;
	DWORD						currPendingFileoperationNo	= 0;
	CString						customActionData;
	TCHAR						logMessage[2*MAX_PATH+1]	= {_T("")};
	DWORD						returnValDW					= ERROR_OUT_OF_PAPER;

	// Was the reboot at end mode set before InstallFinalize?
    customActionData = GetMSIProperty(installHandle, PROPERTY_MSIFILTERREBOOTMODE_ALREADYSET);
	if (_ttoi((LPCTSTR) customActionData) != 0)
	{
		// Yes, something else is requiring a reboot.  Don't change the RebootAtEnd mode
		MSILogMessage(installHandle, "MsiFilterRebootMode:  Detected MsiFilterRebootMode_RebootAtEndModeBefore set.  Something else is requiring a reboot - cannot safely override RebootAtEnd mode - exiting.");
		return ERROR_SUCCESS;
	}
    
    // Are any of the properties set indicating that the reboot at end mode is already set before InstallFinalize?
    // This is a workaround for an MSI limitation where deferred CA's can't determine the RB mode.  Whatever does so can
    // also set a property flag indicating this, allowing us to sense what the above logic should sense autoamtically...but can't.
    customActionData = GetMSIProperty(installHandle, PROPERTY_MSIFILTERREBOOTMODE_ALREADYSETPROPS);
    ParseDelimitedStringList((LPCTSTR) customActionData, &alreadySetPropertyFlags);
    sssnprintf(logMessage, sizeof(logMessage), "MsiFilterRebootMode:  Already set property flag list (%d):  %s", alreadySetPropertyFlags.size(), (LPCTSTR) customActionData);
    MSILogMessage(installHandle, logMessage);
    for (currAlreadySetPropertyFlag = alreadySetPropertyFlags.begin(); currAlreadySetPropertyFlag != alreadySetPropertyFlags.end(); currAlreadySetPropertyFlag++)
    {
        propertyValue = GetMSIProperty(installHandle, currAlreadySetPropertyFlag->c_str());
        if (propertyValue != "")
        {
			sssnprintf(logMessage, sizeof(logMessage), "MsiFilterRebootMode:  Detected %s set (%s).  Something else is requiring a reboot - cannot safely override RebootAtEnd mode - exiting.", currAlreadySetPropertyFlag->c_str(), (LPCTSTR) propertyValue);
            MSILogMessage(installHandle, logMessage);
            return ERROR_SUCCESS;
        }
    }

	// Is there anything to do?
	if (!MsiGetMode(installHandle, MSIRUNMODE_REBOOTATEND))
	{
		// Nope, we're done
		MSILogMessage(installHandle, "MsiFilterRebootMode:  RebootAtEnd mode not set, nothing to do.");
		return ERROR_SUCCESS;
	}

	// Get the list of filename exclusions
    customActionData = GetMSIProperty(installHandle, PROPERTY_MSIFILTERREBOOTMODE_EXCLUSIONS);
	ParseDelimitedStringList((LPCTSTR) customActionData, &excludeFilenames);
	sssnprintf(logMessage, sizeof(logMessage), "MsiFilterRebootMode:  Filename exclusions list (%d):  %s", excludeFilenames.size(), (LPCTSTR) customActionData);
	MSILogMessage(installHandle, logMessage);

	// Report the list of pending file operations
	GetPendingFileOperations(&pendingFileOperations);
	sssnprintf(logMessage, sizeof(logMessage), "MsiFilterRebootMode:  List of %d in-use file updates pending\n", pendingFileOperations.size());
	MSILogMessage(installHandle, logMessage);
	for (currPendingFileOperations = pendingFileOperations.begin(); currPendingFileOperations != pendingFileOperations.end(); currPendingFileOperations++)
	{
		sssnprintf(logMessage, sizeof(logMessage), "MsiFilterRebootMode:  %d: %s=%s", currPendingFileoperationNo, currPendingFileOperations->first.c_str(), currPendingFileOperations->second.c_str());
		MSILogMessage(installHandle, logMessage);
		currPendingFileoperationNo += 1;
	}

	// Filter out deletes and certain file updates
	FilterPendingFileOperationsList(&pendingFileOperations, true, &excludeFilenames);

	// Filtered pending file ops
	sssnprintf(logMessage, sizeof(logMessage), "MsiFilterRebootMode:  Filtered list of in-use file updates pending is %d item long", pendingFileOperations.size());
	MSILogMessage(installHandle, logMessage);
	currPendingFileoperationNo = 0;
	for (currPendingFileOperations = pendingFileOperations.begin(); currPendingFileOperations != pendingFileOperations.end(); currPendingFileOperations++)
	{
		sssnprintf(logMessage, sizeof(logMessage), "MsiFilterRebootMode:  %d: %s=%s", currPendingFileoperationNo, currPendingFileOperations->first.c_str(), currPendingFileOperations->second.c_str());
		MSILogMessage(installHandle, logMessage);
		currPendingFileoperationNo += 1;
	}

	// If list size == 0, set the reboot mode, else allow it
	if (pendingFileOperations.size() == 0)
	{
		returnValDW = MsiSetMode(installHandle, MSIRUNMODE_REBOOTATEND, FALSE);
		if (returnValDW == ERROR_SUCCESS)
		{
			MSILogMessage(installHandle, "MsiFilterRebootMode:  No unfiltered files updated, cleared RebootAtEnd.");
            // The MSI ReplacedInUseFiles property is not being cleared to avoid any possible side effects in MSI
		}
		else
		{
			sssnprintf(logMessage, sizeof (logMessage), "MsiFilterRebootMode:  Error %d clearing RebootAtEnd mode.", returnValDW);
			MSILogMessage(installHandle, logMessage);
		}
	}
	else
	{
		sssnprintf(logMessage, sizeof (logMessage), "MsiFilterRebootMode:  %d files remain after filter - cannot override RebootAtEnd mode.", pendingFileOperations.size());
		MSILogMessage(installHandle, logMessage);
	}

	return ERROR_SUCCESS;
}

HRESULT GetPendingFileOperations( StringPairList* pendingOperations )
// Returns S_OK on success, S_FALSE if no data, HFW32(ERROR_DATATYPE_MISMATCH) if PendingfileRenameOperations exists but is not a REG_MULTI_SZ
{
	HKEY	keyHandle					= NULL;
	DWORD	valueType					= REG_DWORD;
	DWORD	pendingOperationLinesSize	= 0;
	LPTSTR	pendingOperationLines		= NULL;
	LPCTSTR	currPendingOperationLine	= NULL;
	LPCTSTR nextPendingOperationLine	= NULL;
	DWORD	returnValDW					= ERROR_OUT_OF_PAPER;
	HRESULT	returnValHR					= E_FAIL;

	// Validate parameters
	if (pendingOperations == NULL)
		return E_POINTER;

    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY_WINDOWSSESSIONMANAGER, NULL, KEY_READ, &keyHandle);
	if (returnValDW == ERROR_SUCCESS)
	{
		// Read in the pending file operations value and ensure it is a REG_MULTI_SZ
        returnValDW = SymSaferRegQueryValueExA(keyHandle, REGVALUE_PENDINGFILERENAMES, NULL, &valueType, NULL, &pendingOperationLinesSize);
		if (returnValDW == ERROR_SUCCESS)
		{
			pendingOperationLines = new(std::nothrow) TCHAR[pendingOperationLinesSize+1];
			if (pendingOperationLines != NULL)
			{
                returnValDW = SymSaferRegQueryValueExA(keyHandle, REGVALUE_PENDINGFILERENAMES, NULL, &valueType, (BYTE*) pendingOperationLines, &pendingOperationLinesSize);
				if (returnValDW == ERROR_SUCCESS)
				{
					if (valueType != REG_MULTI_SZ)
						returnValDW = ERROR_DATATYPE_MISMATCH;
				}
			}
			else
			{
				returnValDW = ERROR_NOT_ENOUGH_MEMORY;
			}
		}
		RegCloseKey(keyHandle);
		keyHandle = NULL;

		// Parse the lines out of the value
		if (returnValDW == ERROR_SUCCESS)
		{
            returnValHR = S_FALSE;
			currPendingOperationLine = pendingOperationLines;
			while (currPendingOperationLine < (pendingOperationLines + pendingOperationLinesSize))
			{
				nextPendingOperationLine = currPendingOperationLine + _tcslen(currPendingOperationLine) + 1;
				if (nextPendingOperationLine < (pendingOperationLines + pendingOperationLinesSize))
				{
                    try
                    {
    					pendingOperations->push_back(StringPair(tstring(currPendingOperationLine), tstring(nextPendingOperationLine)));
                        returnValHR = S_OK;
                    }
                    catch (std::bad_alloc&)
                    {
                        returnValHR = E_OUTOFMEMORY;
                        break;
                    }
					// Advance to next pair of lines
					currPendingOperationLine = nextPendingOperationLine + _tcslen(nextPendingOperationLine) + 1;
				}
				else
				{
					currPendingOperationLine = nextPendingOperationLine;
				}
				nextPendingOperationLine = NULL;
			}
		}
		else if (returnValDW == ERROR_FILE_NOT_FOUND)
		{
			pendingOperations->clear();
		}
		else
		{
			returnValHR = HRESULT_FROM_WIN32(returnValDW);
		}

		if (pendingOperationLines != NULL)
		{
			delete [] pendingOperationLines;
			pendingOperationLines = NULL;
		}
	}

	// Return result
	return returnValHR;
}

HRESULT FilterPendingFileOperationsList( StringPairList* pendingFileOperations, bool removeDeletes, const StringList* filenameUpdatesToRemove )
// Filters the passed list of pending file operations.  If removeDeletes is true, removes all deletes from the list.
// filenameUpdatesToRemove is an optional list of filenames being updated to remove from the list
// Except for argument validation, always succeeds
{
	StringPairList		        pendingFileOperationsSource;
	StringPairList		        pendingFileOperationsFiltered;
	StringPairList::iterator    currPendingFileOperations;
	tstring						nodeName;
	StringList::const_iterator	currFilenameUpdateToRemove;
	bool						foundFilenameInList				= false;
	HRESULT				        returnValHR						= E_FAIL;

	// Validate parameters
	if (pendingFileOperations == NULL)
		return E_POINTER;

    // Initialize
    returnValHR = S_OK;
    pendingFileOperationsSource = *pendingFileOperations;

	// Filter out deletes (copy over only non-deletes)
    if (removeDeletes)
    {
        pendingFileOperationsFiltered.clear();
        for (currPendingFileOperations = pendingFileOperationsSource.begin(); currPendingFileOperations != pendingFileOperationsSource.end(); currPendingFileOperations++)
        {
            if (_tcsicmp(currPendingFileOperations->second.c_str(), "") != 0)
            {
                try
                {
                    pendingFileOperationsFiltered.push_back(*currPendingFileOperations);
                }
                catch (std::bad_alloc&)
                {
                    returnValHR = E_OUTOFMEMORY;
                    break;
                }
            }
        }
        pendingFileOperationsSource = pendingFileOperationsFiltered;
    }
    
	// Filter out requested file updates
    if (SUCCEEDED(returnValHR) && (filenameUpdatesToRemove != NULL))
	{
		pendingFileOperationsFiltered.clear();
		for (currPendingFileOperations = pendingFileOperationsSource.begin(); currPendingFileOperations != pendingFileOperationsSource.end(); currPendingFileOperations++)
		{
			nodeName = "";
			GetNodeName(currPendingFileOperations->second, &nodeName);
			foundFilenameInList = false;
			for(currFilenameUpdateToRemove = filenameUpdatesToRemove->begin(); currFilenameUpdateToRemove != filenameUpdatesToRemove->end(); currFilenameUpdateToRemove++)
			{
				if (_tcsicmp(nodeName.c_str(), currFilenameUpdateToRemove->c_str()) == 0)
					foundFilenameInList = true;
			}
			if (!foundFilenameInList)
            {
                try
                {
    				pendingFileOperationsFiltered.push_back(*currPendingFileOperations);
                }
                catch (std::bad_alloc&)
                {
                    returnValHR = E_OUTOFMEMORY;
                    break;
                }
            }
		}
	}

	// Return result
	*pendingFileOperations = pendingFileOperationsFiltered;
	return returnValHR;
}

HRESULT ParseDelimitedStringList( LPCTSTR packedFilenameList, StringList* filenameList )
// Parses the strings out of a semicolon delimited list of strings
{
	LPCTSTR		currFilenameStart	= NULL;
	LPCTSTR		nextSeparator		= NULL;
	tstring		newFilename;
	HRESULT		returnValHR			= S_FALSE;

	// Validate parameters
	if (filenameList == NULL)
		return E_POINTER;

	currFilenameStart = packedFilenameList;
	filenameList->clear();
	while (currFilenameStart < (packedFilenameList + _tcslen(packedFilenameList)))
	{
		nextSeparator = (LPCTSTR) _tcschr(currFilenameStart, TEXT(';'));
		if (nextSeparator != NULL)
		{
			newFilename = currFilenameStart;
			newFilename.erase(nextSeparator - currFilenameStart, tstring::npos);
		}
		else
		{
			newFilename = currFilenameStart;
		}
        try
        {
    		filenameList->push_back(newFilename);
        }
		catch (std::bad_alloc&)
        {
            returnValHR = E_OUTOFMEMORY;
            break;
        }
		currFilenameStart += newFilename.size() + 1;
		returnValHR = S_OK;
	}
	return returnValHR;
}

HRESULT GetNodeName( const tstring& pathname, tstring* nodeName )
//Returns the name of the specified file or registry entry given a full pathname to that entity
{
	LPCTSTR lastBackslash   = NULL;
    HRESULT returnValHR     = S_OK;

	if (nodeName == NULL)
		return E_POINTER;

    try
    {
        lastBackslash = (LPCTSTR) _tcsrchr(pathname.c_str(), TEXT('\\'));
        if (lastBackslash != NULL)
            *nodeName = CharNext(lastBackslash);
        else
            *nodeName = pathname;
        returnValHR = S_OK;
            
    }
    catch (std::bad_alloc&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
	return returnValHR;
}
