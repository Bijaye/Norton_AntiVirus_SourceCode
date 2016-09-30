// savMainCA.cpp : Defines the initialization routines for the DLL.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003-2004, 2005 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include "savMainCA.h"
#include "PassDlg.h"
#include "Password.h"
#include "Resource.h"
#include "ClientReg.h"
#include "LicenseHelper.h"
#include "savrtmoduleinterface.h"
#include "veapi.h"
#include "VEFuncs.h"
#include "terminalsession.h"
#include "WindowsSecurityCenter.h"
#include "BldNum.h"
#include <psapi.h>
#include <CREGENT.H>
#include "vdefhlpr.h"
#include "uncab.h"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef UNICODE
#error We really don't want to compile this in Unicode.
#endif

// From PSCAN.H.  Currently, including it introduces many other dependancies.
// Control codes for ProductControl\szReg_Val_WindowsSecurityCenter_Control
#define     WINDOWSSECURITYCENTERCONTROL_NOACTION               0
#define     WINDOWSSECURITYCENTERCONTROL_DISABLEONCEONLY        1
#define     WINDOWSSECURITYCENTERCONTROL_DISABLEALWAYS          2
#define     WINDOWSSECURITYCENTERCONTROL_RESTORE                3

// Control codes for ProductControl\szReg_Val_WSCAVAlert
#define     WSCAVALERT_ENABLE   0
#define     WSCAVALERT_DISABLE  1
#define     WSCAVALERT_NOACTION 2


// Defines for MsxmlFixup
#define REG_EXCEPTIONCOMPONENTS		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup\\ExceptionComponents")
#define REG_FRIENDLYNAME			_T("FriendlyName")
#define REG_MSXML					_T("MSXML 3.0 Exception Package")
#define REG_EXCEPTIONCATALOGNAME	_T("ExceptionCatalogName")
#define REG_EXCEPTIONINFNAME		_T("ExceptionInfName")
#define SZ_MSXML3DLL				_T("msxml3.dll")
#define SZ_MSXML3RDLL				_T("msxml3r.dll")
#define SZ_MSXMLXCAT				_T("msxmlx.cat")
#define SZ_MSXMLXINF				_T("msxmlx.inf")
#define SZ_MSXMLX					_T("msxmlx")
 


/////////////////////////////////////////////////////////////////////////////
// CSavMainCAApp

BEGIN_MESSAGE_MAP(CSavMainCAApp, CWinApp)
	//{{AFX_MSG_MAP(CSavMainCAApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSavMainCAApp construction

CSavMainCAApp::CSavMainCAApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSavMainCAApp object

CSavMainCAApp theApp;

//////////////////////////////////////////////////////////////////////////
//
// Function: ProcessGRC()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ol
//
// Description:  
//	Checks for a GRC and reads the parent key if one exists.
//////////////////////////////////////////////////////////////////////////
// 8/23/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall ProcessGRC( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;

	try
	{
		// Get the source directory
		CString strSourcedir = GetMSIProperty( hInstall, "SourceDir" );
		if( 0 == strSourcedir.GetLength() )
			throw( _T("Unable to resolve SourceDir") );

		// Build grc.dat full path name
		CString strFilename = strSourcedir + "grc.dat";

		// Check the parent server key
		CString strData;
		char *pszData = strData.GetBuffer( MAX_PATH );
		GetPrivateProfileString( _T("KEYS"), _T("Parent"), _T("NONE"), pszData, MAX_PATH, strFilename );
		MSILogMessage( hInstall, pszData );
		strData.ReleaseBuffer();

		// If we do have a parent server, set the property
		if( 0 != strData.CompareNoCase( "none" ) )
		{
			CString strTemp;

			// Remove the first character from the returned data...
			strData = strData.Right( strData.GetLength() - 1 );

			strTemp.Format( "Setting SERVERNAME=%s from GRC.DAT", strData );

			MSILogMessage( hInstall, strTemp );
			MsiSetProperty( hInstall, _T("SERVERNAME"), strData );

			// Set this property so the installer can know that a GRC.DAT file got processed.
			MsiSetProperty( hInstall, _T("ProcessedGRC"), _T("1") );
		}
	}
	catch( TCHAR *szError )
	{
		MSILogMessage( hInstall, szError );
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: TickleRTVScan()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:  
//	This function will set ProccessGRCNow to 1, causing RTVScan to process
//  all of the ProductControl keys.  The purpose of this is to cause RTVScan
//  to check if ProccessLICNow is set to 1 and check for a license file
//////////////////////////////////////////////////////////////////////////
// 2/24/04 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall TickleRTVScan( MSIHANDLE hInstall )
{
	HKEY hKey = NULL;

	try
	{
		if( ERROR_SUCCESS != RegCreateKeyEx( HKEY_LOCAL_MACHINE, REG_PRODUCTCONTROL, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL ) )
			throw( _T("Unable to open ProductControl key") );
	
		DWORD dwData = 1;
		DWORD dwSize = sizeof( dwData );

		if( ERROR_SUCCESS != RegSetValueEx( hKey, _T("ProcessGRCNow"), NULL, REG_DWORD, (CONST BYTE *) &dwData, dwSize ) )
			throw( _T("Failed to set ProccessGRCNow to 1") );
	}
	catch( TCHAR *szError )
	{
		MSILogMessage( hInstall, szError );
	}

	if( hKey )
		RegCloseKey( hKey );

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SetOSInfo()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ol
//
// Description:  
//	Sets some properties based on the OS.
//////////////////////////////////////////////////////////////////////////
// 9/03/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SetOSInfo( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	CString strTemp;

	try
	{
		OSVERSIONINFO osInfo;
		
		ZeroMemory( &osInfo, sizeof( OSVERSIONINFO ) );
		osInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

		if( 0 == GetVersionEx( &osInfo ) )
			throw( _T("Unable to get OS Version information") );

		strTemp.Format( "%d", osInfo.dwMajorVersion );
		MsiSetProperty( hInstall, _T("MAJORVERSION"), strTemp );

		strTemp.Format( "%d", osInfo.dwMinorVersion );
		MsiSetProperty( hInstall, _T("MINORVERSION"), strTemp );

		if( VER_PLATFORM_WIN32_WINDOWS == osInfo.dwPlatformId )
			strTemp = _T("1");
		else
			strTemp = _T("2");
		MsiSetProperty( hInstall, _T("OSVER_PLATFORMID"), strTemp );
	}
	catch( TCHAR *szError )
	{
		MSILogMessage( hInstall, szError );
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MSILogMessage()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//		TCHAR [] - Text to log
//
// Returns:
//		NONE
//
// Description:  
//	This function logs messages to the MSI log file
//
//////////////////////////////////////////////////////////////////////////
// 8/07/01 - SKENNED function created.
// 11/14/03 - Thomas Brock: Added CString variation.
// 2/24/05 - Added std::string overload
//////////////////////////////////////////////////////////////////////////
UINT MSILogMessage( MSIHANDLE hInstall, TCHAR* szString )
{
	UINT uiRetVal = ERROR_SUCCESS;
	CString szLogLine( LOG_PREFIX );

	szLogLine += szString;

	PMSIHANDLE hRec = MsiCreateRecord( 1 );

	if( hRec )
	{
		MsiRecordClearData( hRec );
		MsiRecordSetString( hRec, 0, (LPCSTR)szLogLine );
		MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
	}

	OutputDebugString( szLogLine );

	return uiRetVal;
}

UINT MSILogMessage( MSIHANDLE hInstall, CString& szString )
{
	UINT uiRetVal = MSILogMessage( hInstall, szString.LockBuffer() );
	szString.UnlockBuffer();
	return uiRetVal;
}

UINT MSILogMessage( MSIHANDLE hInstall, tstring& string )
{
	return MSILogMessage( hInstall, (TCHAR*)string.c_str() );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: DirExist()
//
// Parameters:
//		LPCSTR - directory to check for
//
// Returns:
//		BOOL - TRUE if files directory exists
//
// Description:  
//	Checks for the existance of strDir folder
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
BOOL DirExist( LPCSTR strName )
{
	BOOL bRet = FALSE;

	HANDLE hDir = CreateFile(	strName,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_FLAG_BACKUP_SEMANTICS,
								NULL ); // this allows us to open a dir

	if( INVALID_HANDLE_VALUE != hDir )
	{
		CloseHandle( hDir );
		bRet = TRUE;
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: CopyDir()
//
// Parameters:
//		CString - Source folder
//		CString - Destination folder
//
// Returns:
//		int - number of files copied, -1 if error	
//
// Description:  
//	Copies all files from source to destination
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
int CopyDir( MSIHANDLE hInstall, CString strSource, CString strDest )
{
	int nRet = 0;
	BOOL bRet = TRUE;
	WIN32_FIND_DATA hFindData;
	CString temp;
	CString strAllFiles = strSource + _T("\\*.*");
	CString strDFile, strSFile;

	ZeroMemory( &hFindData, sizeof( hFindData ) );
	HANDLE hFind = FindFirstFile( strAllFiles, &hFindData );
	while( (INVALID_HANDLE_VALUE != hFind) && (bRet) )
	{
		// Do not copy folders, only files
		if( !(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			strDFile = strDest + _T("\\") + hFindData.cFileName;
			strSFile = strSource + _T("\\") + hFindData.cFileName;
			temp.Format( "Copying %s to %s", strSFile, strDFile );
			MSILogMessage( hInstall, temp );
			if( CopyFile( strSFile, strDFile, FALSE ) )
				++nRet;
			else
			{
				CString temp;
				temp.Format( "Error copying file: %0xd", GetLastError() );
				MSILogMessage( hInstall, temp.GetBuffer() );
				temp.ReleaseBuffer();
			}
		}
		bRet = FindNextFile( hFind, &hFindData );
	}
	
	if( 0 == nRet )
		MSILogMessage( hInstall, _T("No files copied") );

	if( hFind )
		FindClose( hFind );

	return nRet;
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
// 6/16/04 - DKOWALYSHYN ported over to clean up virusdef directory
// 2/23/05 - SKENNED Ported back into savMain and added some logic to unify
//			         functions, no need for two copies of this
//////////////////////////////////////////////////////////////////////////
BOOL DeleteDir( MSIHANDLE hInstall, TCHAR* strFolder, TCHAR* strPattern )
{
	BOOL bRet = TRUE;
	WIN32_FIND_DATA hFindData;
	TCHAR strAllFiles[MAX_PATH] = {0};
	TCHAR strSFile[MAX_PATH] = {0};

	vpstrncpy        (strAllFiles,strFolder,  sizeof (strAllFiles));
	vpstrnappendfile (strAllFiles,strPattern, sizeof (strAllFiles));

	tstring temp( "Folder: " );
	temp.append( strFolder );
	MSILogMessage( hInstall, temp );
	temp = "Pattern: ";
	temp.append( strPattern );
	MSILogMessage( hInstall, temp );

	ZeroMemory( &hFindData, sizeof(hFindData) );
	HANDLE hFind = FindFirstFile( strAllFiles, &hFindData );
	while( (INVALID_HANDLE_VALUE != hFind) && (bRet) )
	{
		vpstrncpy        (strSFile, strFolder,           sizeof (strSFile));
		vpstrnappendfile (strSFile, hFindData.cFileName, sizeof (strSFile));

		if( !(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			// Display deletion on the UI
            TCHAR buffer[ MAX_PATH ];
			LoadString( ::AfxGetInstanceHandle(), IDS_DELETING, buffer, MAX_PATH );
			tstring info( buffer );
			info.append( tstring(" ") + tstring(hFindData.cFileName) );
			PMSIHANDLE record = MsiCreateRecord( 3 );
			if( record )
			{
				MsiRecordClearData( record );
				MsiRecordSetString( record, 2, info.c_str() );
				MsiProcessMessage( hInstall, INSTALLMESSAGE_ACTIONSTART, record );
			}

			DeleteFile( strSFile );
			// Log the deleting
			temp = "Deleteing File: ";
			temp.append( strSFile );
			MSILogMessage( hInstall, temp );
		}
		else
		{
			if( (0 != lstrcmp( hFindData.cFileName, "." )) && (0 != lstrcmp( hFindData.cFileName, ".." )) )
			{
				DeleteDir(  hInstall, strSFile, strPattern );
			}
		}

		bRet = FindNextFile( hFind, &hFindData );
	}
	
    DWORD dwResult = GetLastError();
    if(dwResult != ERROR_NO_MORE_FILES)
    {
		TCHAR szBuf[80] = {0}; 
	    vpsnprintf(szBuf, sizeof (szBuf), "GetLastError returned %u", dwResult); 
		MSILogMessage( hInstall, szBuf );
    }

	if( hFind )
		FindClose( hFind );

	bRet = RemoveDirectory( strFolder );
	temp = "Removing folder ";
	temp.append( strFolder );
	MSILogMessage( hInstall, temp );
 
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: GetTempDir()
//
// Parameters:
//
// Returns:
//		CString - path to the temp folder
//
// Description:  
//	
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
CString GetTempDir()
{
	CString strRet = "";

	GetTempPath( MAX_PATH, strRet.GetBuffer( MAX_PATH ) );
	strRet.ReleaseBuffer();

	if( '\\' != strRet.Right(1) )
		strRet += "\\";

	return strRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: GetMSIProperty()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//		CString - Name of property to read
//
// Returns:
//		CString - Product code of the MSI
//
// Description:  
//	This function reads the CustomActionData property and returns it
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
CString GetMSIProperty( MSIHANDLE hInstall, CString strProperty )
{
	CString strRet = "", strTemp;
	char *pCAData = NULL;
	DWORD dwLen = 0;	// This needs to be 0 so that we get ERROR_MORE_DATA error
	UINT nRet = 0;

	strProperty.LockBuffer();

	try
	{
		nRet = MsiGetProperty( hInstall, strProperty, "", &dwLen );
		if( ERROR_MORE_DATA != nRet )
		{
			strTemp.Format( "Error getting length of %s (nRet = %d)", strProperty, nRet );
			throw( strTemp );
		}

		// Allocate our memory and grab the data, add one for the trailing null
		pCAData = new char[ ++dwLen ];
		if( !pCAData )
			throw( CString("Unable to allocate memory for data buffer") );
		
		nRet = MsiGetProperty( hInstall, strProperty, pCAData, &dwLen );
		if( ERROR_SUCCESS != nRet )
		{
			strTemp.Format( "Unable to obtain the %s (nRet = %d)", strProperty, nRet );
			throw( strTemp );
		}

		// Save the value
		strRet.Format( "%s", pCAData );
		
		// Log the value to the MSI log file
		strTemp.Format( "%s=%s", strProperty, strRet );
		MSILogMessage( hInstall, strTemp );
	}
	catch( CString strErr )
	{
		MSILogMessage( hInstall, strErr );
		strRet = "";
	}

	strProperty.UnlockBuffer();

	if( pCAData )
		delete [] pCAData;

	return strRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: CheckUninstallPassword()
//
// Parameters:
//		MSIHANDLE - handle to current install
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//	
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall CheckUninstallPassword( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	HKEY hKey = NULL;
	TCHAR strUILevel[4] = {0};

	try
	{
		// Check whether or not we will be using a Password to uninstall
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                           szReg_Key_Main "\\" szReg_Key_AdminOnly "\\" szReg_Key_Security,
                                           0, KEY_READ, &hKey ) )
		{
			DWORD dwCheckPass = 0;
			DWORD dwSize = sizeof( DWORD );

			if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey, szReg_Val_UseVPUninstallPassword, NULL, NULL, (LPBYTE)&dwCheckPass, &dwSize ) )
			{
				if( dwCheckPass > 0 )
				{
					// Get the UILevel...
					dwSize = 4;
					// If MsiGetProperty fails here, we don't care.  We will simply prompt in that case...
					MsiGetProperty( hInstall, _T("UILevel"), strUILevel, &dwSize );
					if( _tcscmp( strUILevel, _T("2") ) )
					{
						// Get handle to MSI window, if it is not open use desktop window as parent instead
						HWND hWnd = GetForegroundWindow();
						if( !hWnd )
							hWnd = GetDesktopWindow();

						// Prompt for the password
						CPassDlg passDlg( CWnd::FromHandle( hWnd ) );
						if(passDlg.DoModal() != IDOK)
							nRet = ERROR_INSTALL_USEREXIT;
						else
							nRet = ERROR_SUCCESS;
					}
					else
					{
						// Fail the install in silent mode.
						MSILogMessage( hInstall, _T("UseVPUninstallPassword is set, but uninstall is silent.  Failing uninstall.") );
						nRet = ERROR_INSTALL_USEREXIT;
					}
				}
				else
				{
					MSILogMessage( hInstall, _T("UseVPUninstallPassword = 0") );
				}
			}
			else
			{
				MSILogMessage( hInstall, _T("Unable to query UseVPUninstallPassword value") );
			}
			RegCloseKey( hKey );
		}
		else
		{
			MSILogMessage( hInstall, _T("Unable to open REGKEY_ADMIN_SECURITY key") );
		}
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	return nRet;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function: AddSlash
//
//  Simply adds a slash to the end of a path, if needed.
//
//  08/16/2003 Thomas Brock - Function created.
//  09/18/2003 Thomas Brock - Added CString version of AddSlash.
//
///////////////////////////////////////////////////////////////////////////////

void __fastcall AddSlash( char* pszPath, size_t nNumBytes )
{
	if( pszPath[ strlen( pszPath ) - 1 ] != '\\' )
		vpstrnappend (pszPath, "\\", nNumBytes);
}

void __fastcall AddSlash( CString& strPath )
{
	if( strPath.GetAt( strPath.GetLength() - 1 ) != '\\' )
	{
		strPath += _T("\\");
	}
}

//////////////////////////////////////////////////////////////////////////
//
// Function: QuarantineCleanup
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
//		Prompts user to delete quarantine files only if quarantine
//		files exist.  Should not be called during silent uninstalls.
//
//////////////////////////////////////////////////////////////////////////
// 9/18/03 - Thomas Brock - function created.
// 2/25/05 - SKENNED - Added logic to detect and remove folders underneath
//                     the Quarantine folder.  Expanded threats now have
//                     an additional folder they create and using a pattern
//					   will hide those
//////////////////////////////////////////////////////////////////////////
UINT __stdcall QuarantineCleanup( MSIHANDLE hInstall )
{
	HANDLE hNext;
	WIN32_FIND_DATA hFindData;
	CString strFile, strFiles, strLog;
	CString strPath = GetMSIProperty( hInstall, _T("CustomActionData") );
	BOOL bRet = TRUE;
	TCHAR strAllFiles[MAX_PATH] = {0};
	TCHAR strSFile[MAX_PATH] = {0};

	// Set up a path to the quarantine files...
	AddSlash( strPath );
	strPath += _T("Quarantine\\");

	// Set up a full path and wildcard for FindFirstFile...
	strFiles = strPath;
	strFiles += _T("*.*");

	// Look for any files in the quarantine folder...
	hNext = FindFirstFile( strFiles, &hFindData );
	while( (INVALID_HANDLE_VALUE != hNext) && (bRet) )
	{
		vpstrncpy        (strSFile, strPath,           sizeof (strSFile));
		vpstrnappendfile (strSFile, hFindData.cFileName, sizeof (strSFile));

		if( (0 != lstrcmp( hFindData.cFileName, "." )) && (0 != lstrcmp( hFindData.cFileName, ".." )) )
		{
			CString strTitle;
			CString strMessage;

			strTitle.LoadString( IDS_Q_UNINSTALL_TITLE );
			strMessage.LoadString( IDS_Q_UNINSTALL_MESSAGE );

			if( IDYES == MessageBox( GetForegroundWindow(), strMessage, strTitle, MB_YESNO | MB_ICONQUESTION ) )
			{
				DeleteDir( hInstall, (TCHAR*)strPath.LockBuffer(), _T("*.*") );
				strPath.ReleaseBuffer();
			}
			break; // get out now!
		}

		bRet = FindNextFile( hNext, &hFindData );
	}

	if( hNext)
		FindClose( hNext );

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SetOneTimeUpdateCookie()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//	    Drops an encrypted cookie in the SAV settings to allow the product
//      to update itself once after install.
//      THIS FUNCTION REQUIRES ELEVATED PRIVS.
//////////////////////////////////////////////////////////////////////////
// 10/20/03 - RSUTTON function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SetOneTimeUpdateCookie( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_BAD_FORMAT;

	try
	{
        if( License_SetOneTimeUpdateMode( TRUE ) )
        {
            nRet = ERROR_SUCCESS;
        }
	}
	catch( ... )
	{
	}

    if( nRet != ERROR_SUCCESS )
    {
		MSILogMessage( hInstall, "Failed setting One-Time Update Cookie" );
    }

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SetOneTimeUpdateCookie_RB()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//	    Cleans up the encrypted license cookie data.
//      THIS FUNCTION REQUIRES ELEVATED PRIVS.
//////////////////////////////////////////////////////////////////////////
// 10/20/03 - RSUTTON function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SetOneTimeUpdateCookie_RB( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;

	try
	{
        License_OneTimeUpdateCleanup();
	}
	catch( ... )
	{
	    nRet = ERROR_BAD_FORMAT;

		MSILogMessage( hInstall, "Failed deleting One-Time Update Cookie" );
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SetLastStartTime()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//	    This function will set the local current machine time regsirty
//		for use by rtvscan and LU.
//////////////////////////////////////////////////////////////////////////
// 10/22/03 - DKOWALYSHYN function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SetLastStartTime( MSIHANDLE hInstall )
{
	HKEY hKey = NULL;
	UINT nRet = ERROR_SUCCESS;
	time_t systemtime;
	time( &systemtime );
	TCHAR szErrorString[512];

	try
	{
		// Check whether or not we will be using a Password to uninstall
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE,
										szReg_Key_Main "\\" szReg_Key_PatternManager "\\"szReg_Key_ScheduleKey,
										0, KEY_ALL_ACCESS, &hKey ) )
		{
			DWORD dwCheckPass = 0;
			DWORD dwSize = sizeof( DWORD );

			if( ERROR_SUCCESS ==  RegSetValueEx (hKey,
				szReg_Val_Schedule_LastStart,
				0,
				REG_DWORD,
				(LPBYTE)&systemtime,			// pointer to the value data
				sizeof( &dwSize )))		// length of value data 
					{
						nRet = ERROR_SUCCESS;
						vpsnprintf( szErrorString, sizeof (szErrorString), "SetLastStartTime %d", systemtime );
						MSILogMessage( hInstall, szErrorString );
					}
			else
			{
				nRet = ERROR_BAD_FORMAT;
				MSILogMessage( hInstall, "Failed SetLastStartTime" );
			}
		}
   	}
	catch( ... )
	{
	    nRet = ERROR_BAD_FORMAT;
		MSILogMessage( hInstall, "Catch exception in SetLastStartTime" );
	}

	return ERROR_SUCCESS; // always return ERROR_SUCCESS for msi
}


//////////////////////////////////////////////////////////////////////////
//
// Function: CreateGlobalAddAtom()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//		TRUE if the installer may continue, or FALSE if the ATOM couldn't
//  be created or if it already existed, in which case the installer should
//  exit. This keeps vpc32.exe from running if MSVCRT.dll needs to be
//	upgraded on NT 4.
//
//////////////////////////////////////////////////////////////////////////
// 11/07/03 - DKOWALYSHYN function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall CreateGlobalAddAtom( MSIHANDLE hInstall )
{
	ATOM aGlbAtom;
	UINT nRet = ERROR_SUCCESS;
	TCHAR szErrorString[512];

	aGlbAtom = GlobalAddAtom ( ATOM_REBOOTREQUIRED );
    if ( 0 != aGlbAtom  &&
         ERROR_ALREADY_EXISTS != GetLastError() )
        {
			vpsnprintf(szErrorString, sizeof (szErrorString), "CreateGlobalAddAtom- %s created %d", ATOM_REBOOTREQUIRED, aGlbAtom); 
			MSILogMessage( hInstall, szErrorString );
        }
    else
        {
			DWORD dw = GetLastError(); 
			vpsnprintf(szErrorString, sizeof (szErrorString), "CreateGlobalAddAtom- %s failed: GetLastError returned %u", ATOM_REBOOTREQUIRED, dw); 
			MSILogMessage( hInstall, szErrorString );
			nRet = ERROR_BAD_FORMAT;
        }
	return ERROR_SUCCESS; // always return ERROR_SUCCESS for msi
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MigStopRTVScan()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:  
//		During migration we know of problems getting Defwatch, RTVScan,
//		SAVRT, and SAVRTPEL to shutdown safely and in the required order
//		and this function addresses this problem programmatically.	
//		This custom should only be called prior to RemoveExistingProducts
//		when we detect a migration from Almagest V10 or earlier.
//		In the future the new features of the StopRTVScan function should
//		handle a proper shutdown.
//		* Note - This function will be called in Immediate mode and as such
//		will run in the users context. So if the user doesn't have the 
//		rights to shut down a service, then this will fail.
//
//////////////////////////////////////////////////////////////////////////
// 12/07/05 - DKOWALYSHYN function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall MigStopRTVScan( MSIHANDLE hInstall )
{
    DWORD returnValDW = ERROR_SUCCESS;
    
	MSILogMessage( hInstall, "MigStopRTVScan calling: StopRTVScan." );
	returnValDW = StopRTVScan( hInstall );
    if (returnValDW != ERROR_SUCCESS)
        MSILogMessage( hInstall, "MigStopRTVScan:  Unable to shutdown RTVScan, aborting migration.");
    
    return returnValDW; 
}

//////////////////////////////////////////////////////////////////////////
//
// Function: StopRTVScan()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//		During migration we know of problems getting Defwatch, RTVScan,
//		SAVRT, and SAVRTPEL to shutdown safely and in the required order
//		and this function addresses this problem programmatically.	
//
//////////////////////////////////////////////////////////////////////////
// 11/12/03 - DKOWALYSHYN function created.
//  8/29/05 - DKOWALYSHYN function updated to shutdown SAVRT & SAVRTPEL
//////////////////////////////////////////////////////////////////////////
UINT __stdcall StopRTVScan( MSIHANDLE hInstall )
{
    HWND    hSavAp                  = NULL;
    UINT    returnValDW             = ERROR_SUCCESS;
    DWORD   returnValDWb            = ERROR_OUT_OF_PAPER;
    
	// Kill VPTray app
    hSavAp = FindWindow(VPTRAY_CLASS, NULL);
    if (hSavAp)
	{
		// found the windows, send a message to close it
		PostMessage(hSavAp, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
		MSILogMessage( hInstall, "Close VPTray PostMessage" );
	}

	// Stop DefWatch
	returnValDWb = StopService ( hInstall, DEFWATCHSERVICENAME, MAX_LOOPS );
    returnValDW = (returnValDW == ERROR_SUCCESS ? returnValDWb : returnValDW);
	// Stop RTVSCAN - RTVSCAN_MAX_LOOPS wait up to 5 minutes
    returnValDWb = StopService ( hInstall, RTVSERVICENAME, RTVSCAN_MAX_LOOPS );
    returnValDW = (returnValDW == ERROR_SUCCESS ? returnValDWb : returnValDW);
	// Stop SAVRT
    returnValDWb = StopService ( hInstall, SAVRT_SERVICE_NAME, MAX_LOOPS );
    returnValDW = (returnValDW == ERROR_SUCCESS ? returnValDWb : returnValDW);
    // Stop SAVRTPEL.
    StopService ( hInstall, SAVRTPEL_SERVICE_NAME, MAX_LOOPS );
	// Wait on NAVENG
    returnValDWb = WaitonService ( hInstall, NAVENG_SERVICE_NAME, MAX_LOOPS);
    returnValDW = (returnValDW == ERROR_SUCCESS ? returnValDWb : returnValDW);
	// Wait on SAVRTPEL
    returnValDWb = WaitonService ( hInstall, NAVEX15_SERVICE_NAME, MAX_LOOPS );
    returnValDW = (returnValDW == ERROR_SUCCESS ? returnValDWb : returnValDW);

	return returnValDW;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: WaitonService()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//		Wait for any Service to shutdown via SCM
//
//////////////////////////////////////////////////////////////////////////
// 11/07/05 - DKOWALYSHYN function created.
//////////////////////////////////////////////////////////////////////////
UINT WaitonService ( MSIHANDLE hInstall, TCHAR szServiceName[], INT nLoopCount )
{ 
    DWORD dwState =					0xFFFFFFFF;
    DWORD dwErr =					0xFFFFFFFF;
	UINT nRet =						ERROR_OUT_OF_PAPER;
	UINT count =					0;
	TCHAR szErrorString[512] =		{0};
	BOOL bwEnumOnce =				FALSE;
	BOOL bCSRet =					0;
	SERVICE_STATUS					ss;
	memset(&ss, 0, sizeof(ss));

	// open the service control manager
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if ( 0 != hSCM )
	{ 
		// open the service
		SC_HANDLE hService = OpenService(hSCM,
										 szServiceName,
										 SERVICE_INTERROGATE | SERVICE_QUERY_STATUS );
		if ( 0 != hService )
		{  
            nRet = ERROR_SERVICE_REQUEST_TIMEOUT;
			vpsnprintf( szErrorString, sizeof ( szErrorString ),
				"WaitonService:OpenService handle %d %s", hService, szServiceName ); 
			MSILogMessage( hInstall, szErrorString );
			// Exit when service state set to service_stopped or nLoopCount seconds & cycles have elapsed 			
			while ((( dwState != SERVICE_STOPPED ) || ( dwState == SERVICE_STOP_PENDING )) && ( count < nLoopCount ))
			{
				// Loop and wait for service to close
				if (hService) 
				{
					// Get the current status
					bCSRet = QueryServiceStatus( hService, &ss );
					if (!bCSRet)
					{
						DWORD dwGetLastErr = GetLastError();
                        nRet = dwGetLastErr;
                        dwState = ss.dwCurrentState;
                        if (nRet != ERROR_SERVICE_NOT_ACTIVE)
                        {
    						vpsnprintf(szErrorString, sizeof (szErrorString),
    							"ERROR! WaitonService: QueryServiceStatus GetLastError:%d dwState:%d.", dwGetLastErr, dwState); 
                        }
                        else
                        {
                            nRet = ERROR_SUCCESS;
                            dwState = SERVICE_STOPPED;
                            vpsnprintf(szErrorString, sizeof (szErrorString), "WaitonService: Service %s not yet started.", szServiceName);
                        }
                        MSILogMessage( hInstall, szErrorString );
					}
					else
					{
						dwErr = ss.dwWin32ExitCode;
						dwState = ss.dwCurrentState;

						// If the service is running, send a SERVICE_CONTROL_INTERROGATE control request
						// to tell the service to update the SCM with it's latest status
						if ( dwState == SERVICE_RUNNING ) 
						{
							if ( ControlService(hService,
												SERVICE_CONTROL_INTERROGATE,
												&ss) )
							{
								dwErr = ss.dwWin32ExitCode;
								dwState = ss.dwCurrentState;
							}
						}
					}
				}
				vpsnprintf( szErrorString, sizeof ( szErrorString ),
					"WaitonService wait count & dwcode %d %d", count,dwState ); 
				MSILogMessage( hInstall, szErrorString );
				count++;
				Sleep( 1000 );
			}
            if (dwState == SERVICE_STOPPED)
                nRet = ERROR_SUCCESS;
			// close the service handle
			CloseServiceHandle( hService );
		}
		else
		{
			DWORD dw = GetLastError();
            nRet = dw;
            if (nRet == ERROR_SERVICE_DOES_NOT_EXIST)
            {
                nRet = ERROR_SUCCESS;
                vpsnprintf( szErrorString, sizeof (szErrorString), "WaitonService: service %s does not exist, returning success.", szServiceName);
            }
            else
            {
                vpsnprintf( szErrorString, sizeof (szErrorString), "ERROR! WaitonService:SRV:%s  OpenService handle / getlasterror %d %u", szServiceName, hService, dw ); 
            }
			MSILogMessage( hInstall, szErrorString );
		}
		// close the service control manager handle
		CloseServiceHandle(hSCM);
	}
	else
	{
		DWORD dw = GetLastError();
        nRet = dw;
		vpsnprintf( szErrorString, sizeof ( szErrorString ),
			"ERROR! WaitonService:SRV:%s OpenSCManager handle / getlasterror %d %u", szServiceName, hSCM, dw ); 
		MSILogMessage( hInstall, szErrorString );
	}
	return nRet;
}


//////////////////////////////////////////////////////////////////////////
//
// Function: StopService()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//		Stop the any Service via SCM, fails from MSI for some reason
//
//////////////////////////////////////////////////////////////////////////
// 11/12/03 - DKOWALYSHYN function created.
//////////////////////////////////////////////////////////////////////////
UINT StopService (MSIHANDLE hInstall, TCHAR szServiceName[], INT nLoopCount )
{ 
    DWORD dwState =					0xFFFFFFFF;
    DWORD dwErr =					0xFFFFFFFF;
	DWORD dw =						0;
	DWORD dwAccess =				0; 
	UINT nRet =						ERROR_OUT_OF_PAPER;
	UINT count =					0;
	TCHAR szErrorString[512] =		{0};
	SERVICE_STATUS					ss;
	memset(&ss, 0, sizeof(ss));

	// open the service control manager
    SC_HANDLE hSCM = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( 0 != hSCM )
	{  
		// open the service
		SC_HANDLE hService = OpenService(hSCM,
										 szServiceName,
										 SERVICE_STOP | SERVICE_INTERROGATE | SERVICE_QUERY_STATUS );
		if ( 0 != hService )
		{  
			vpsnprintf( szErrorString, sizeof ( szErrorString ),
				"StopService:OpenService handle %d %s", hService, szServiceName ); 
			MSILogMessage( hInstall, szErrorString );
			// Sending SERVICE_CONTROL_STOP instruction to SCM
			BOOL bCSRet = ControlService(hService,
										SERVICE_CONTROL_STOP,
										&ss);
			if( 0 == bCSRet ) // ControlService SERVICE_CONTROL_STOP failed
			{
				// Without a valid controlservice we should just exit. E.g. ERROR_ACCESS_DENIED, ERROR_SERVICE_CANNOT_ACCEPT_CTRL, ERROR_SHUTDOWN_IN_PROGRESS
				dw = GetLastError(); 
                nRet = dw;
                if (nRet != ERROR_SERVICE_NOT_ACTIVE)
                {
    				vpsnprintf(szErrorString, sizeof ( szErrorString ),
    					"ERROR! StopService:ControlService handle:%d getlasterror: %u", bCSRet, dw); 
    				MSILogMessage( hInstall, szErrorString );
                }
                else
                {
                    vpsnprintf(szErrorString, sizeof(szErrorString), "StopService:  Service %s already stopped.", szServiceName);
                    MSILogMessage( hInstall, szErrorString );
                    nRet = ERROR_SUCCESS;
                }
			}
			else
			{
                nRet = ERROR_SERVICE_REQUEST_TIMEOUT;
				dwState = ss.dwCurrentState;
				vpsnprintf(szErrorString, sizeof ( szErrorString ),
					"StopService:ControlService handle:%d dwState:%d", bCSRet, dwState); 
				MSILogMessage( hInstall, szErrorString );
				// Exit when service state set to service_stopped or nLoopCount seconds & cycles have elapsed 			
				while ((( dwState != SERVICE_STOPPED ) || ( dwState == SERVICE_STOP_PENDING )) && ( count < nLoopCount ))
				{
					// Loop and wait for service to close
					if (hService) 
					{
						// Get the current status
						bCSRet = QueryServiceStatus( hService, &ss );
						if(!bCSRet)
						{
							DWORD dwGetLastErr = GetLastError();
                            nRet = dwGetLastErr;
							dwState = ss.dwCurrentState;
                            if (nRet != ERROR_SERVICE_NOT_ACTIVE)
                            {
    							vpsnprintf(szErrorString, sizeof(szErrorString),
    								"ERROR! QueryServiceStatus GetLastError:%d dwState:%d.", dwGetLastErr, dwState); 
                            }
                            else
                            {
                                nRet = ERROR_SUCCESS;
                                dwState = SERVICE_STOPPED;
                                vpsnprintf(szErrorString, sizeof(szErrorString), "StopService:  Service %s not yet started.", szServiceName);
                            }
                            MSILogMessage( hInstall, szErrorString );
						}
						else
						{
							dwErr = ss.dwWin32ExitCode;
							dwState = ss.dwCurrentState;

							// If the service is running, send a SERVICE_CONTROL_INTERROGATE control request
							// to tell the service to update the SCM with it's latest status
							if ( dwState == SERVICE_RUNNING ) 
							{
								if ( ControlService(hService,
													SERVICE_CONTROL_INTERROGATE,
													&ss) )
								{
									dwErr = ss.dwWin32ExitCode;
									dwState = ss.dwCurrentState;
								}
							}
						}
					}
					vpsnprintf( szErrorString, sizeof ( szErrorString ),
						"SERVICE_STOPPED wait count & dwcode %d %d", count,dwState ); 
					MSILogMessage( hInstall, szErrorString );
					count++;
					Sleep( 1000 );
				}
                if (dwState == SERVICE_STOPPED)
                    nRet = ERROR_SUCCESS;
			}
			// close the service handle
			CloseServiceHandle( hService );
		}
		else
		{
			DWORD dw = GetLastError(); 
            nRet = dw;
            if (nRet == ERROR_SERVICE_DOES_NOT_EXIST)
            {
                nRet = ERROR_SUCCESS;
                vpsnprintf( szErrorString, sizeof (szErrorString), "StopService: service %s does not exist, returning success.", szServiceName);
            }
            else
            {
                vpsnprintf( szErrorString, sizeof (szErrorString), "ERROR! StopService:SRV:%s  OpenService handle / getlasterror %d %u", szServiceName, hService, dw ); 
            }
			MSILogMessage( hInstall, szErrorString );
		}
		// close the service control manager handle
		CloseServiceHandle(hSCM);
	}
	else
	{
		DWORD dw = GetLastError(); 
        nRet = dw;
		vpsnprintf( szErrorString, sizeof ( szErrorString ),
			"ERROR! StopService:SRV:%s OpenSCManager handle / getlasterror %d %u", szServiceName, hSCM, dw ); 
		MSILogMessage( hInstall, szErrorString );
	}
	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MigrationDeletionFixup
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:  
//		There is a problem when migrating from SAV/SCS prior to Kepler
//		where the previous installer marks ScanDlgs.dll for deletion due
//		to it being in use during the uninstall of the previous product.
//		However, when it comes time for the new installer to overwrite the
//		file, it is no longer in use.  We overwrite it, but then it gets
//		deleted during the reboot.
//
//		Right now, this only appears to be happening on Windows 9x boxes.
//		If it ever shows up on NT boxes, extra logic will be needed to
//		handle OSes that don't use WININIT.INI.
//
//		Seems like this is showing up on NT boxes now.  Specifically,
//		when SAVRoam is running, several files get marked for deletion.
//		For some reason, our installer is able to overwrite some of them,
//		causing them to get deleted at reboot.  Extending this function
//		to all SSC shared files and Intel PDS dlls.
//
//////////////////////////////////////////////////////////////////////////
// 12/04/03 - Thomas Brock function created.
// 01/18/04 - Thomas Brock exteded to include NT and many more files.
// 11/19/04 - Thomas Brock removed Windows 9x support.
//////////////////////////////////////////////////////////////////////////

BOOL IsSharedFile( char * pstr )
{
	char * pszFiles[] = {"LOTNTSUI.OCX",
						"EXCHNGUI.OCX",
						"IMAILUI.OCX",
						"LDDATETM.OCX",
						"LDVPCTLS.OCX",
						"LDVPDLGS.OCX",
						"LDVPTASK.OCX",
						"LDVPUI.OCX",
						"LDVPVIEW.OCX",
						"TRANSMAN.DLL",
						"SCANDLGS.DLL",
						"VPSHELL2.DLL",
						"WEBSHELL.DLL",
						"CBA.DLL",
						"LOC32VC0.DLL",
						"MSGSYS.DLL",
						"NTS.DLL",
						"PDS.DLL",
						"PDS.EXE",
						"RTVSCAN.EXE"};

	char strTemp[MAX_PATH];

	vpstrncpy( strTemp, pstr, sizeof (strTemp) );
	strupr( strTemp );

	for( int i = 0; i < ( sizeof pszFiles / sizeof pszFiles[0] ); i++ )
	{
		if( strstr( pstr, pszFiles[i] ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

UINT _stdcall MigrationDeletionFixup( MSIHANDLE hInstall )
{
	HKEY hKey;
	LONG lRetval;
	bool bOutput;
	DWORD dwType, dwSize, dwPosIn, dwPosOut, dwPosStr;
	char * pBuffIn = NULL;
	char * pBuffOut = NULL;
	char strTemp1[MAX_PATH];
	char strTemp2[MAX_PATH];

	// MessageBox( NULL, _T("Stop!"), _T("Debug Me!"), MB_OK | MB_ICONSTOP );

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager"), NULL, KEY_ALL_ACCESS, &hKey ) )
	{
		dwSize = 0;
		lRetval = SymSaferRegQueryValueEx( hKey, _T("PendingFileRenameOperations"), NULL, &dwType, NULL, &dwSize );
		if( ( !lRetval ) && dwSize )
		{
			pBuffIn = new char[ dwSize ];
			pBuffOut = new char[ dwSize ];

			if( pBuffIn && pBuffOut )
			{
				lRetval = SymSaferRegQueryValueEx( hKey, _T("PendingFileRenameOperations"), NULL, &dwType, (LPBYTE) pBuffIn, &dwSize );
				if( ( ERROR_SUCCESS == lRetval ) && ( dwType = REG_MULTI_SZ ) )
				{
					dwPosIn = 0;
					dwPosOut = 0;
					while( pBuffIn[ dwPosIn ] )
					{
						bOutput = true;

						// Get the first string of the pair...
						dwPosStr = 0;
						do
						{
							strTemp1[ dwPosStr++ ] = pBuffIn[ dwPosIn++ ];
						}
						while( strTemp1[ dwPosStr - 1 ] );

						// Get the second string of the pair...
						dwPosStr = 0;
						do
						{
							strTemp2[ dwPosStr++ ] = pBuffIn[ dwPosIn++ ];
						}
						while( strTemp2[ dwPosStr - 1 ] );

						// If the second string is empty, this is a delete...
						if( ! strTemp2[0] )
						{
							// This is a delete...
							if( IsSharedFile( strTemp1 ) )
							{
								// It is a shared file...
								MSILogMessage( hInstall, _T("Preventing deletion of file:") );
								MSILogMessage( hInstall, strTemp1 );
								bOutput = false;
							}
						}

						if( bOutput )
						{
							// Write the first string of the pair...
							dwPosStr = 0;
							do
							{
								pBuffOut[ dwPosOut++ ] = strTemp1[ dwPosStr++ ];
							}
							while( pBuffOut[ dwPosOut - 1 ] );

							// Write the second string of the pair...
							dwPosStr = 0;
							do
							{
								pBuffOut[ dwPosOut++ ] = strTemp2[ dwPosStr++ ];
							}
							while( pBuffOut[ dwPosOut - 1 ] );
						}
					}

					// Did we write anything to the output buffer?
					if( dwPosOut )
					{
						// Add final null...
						pBuffOut[ dwPosOut++ ] = NULL;
						RegSetValueEx( hKey, _T("PendingFileRenameOperations"), NULL, REG_MULTI_SZ, (CONST BYTE *) pBuffOut, dwPosOut );
					}
					else
					{
						RegDeleteValue( hKey, _T("PendingFileRenameOperations") );
					}
				}
			}

			if( pBuffIn ) delete[] pBuffIn;
			if( pBuffOut ) delete[] pBuffOut;
		}
	}
	
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: FixupImagePaths
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:  
//		See defect 1-22R9CF
//		Image paths for services that use long file names need
//		to be quoted.  There is no way I currently know of to have
//		MSI either quote the image path or use short path names
//		when using the ServiceInstall table.
//
//		This function, along with FixupImagePathWorker, will
//		apply quotes around the string contained in the ImagePath
//		section of the registry.  If the string is already quoted,
//		this function should not change it.
//
//////////////////////////////////////////////////////////////////////////
// 12/11/03 - Thomas Brock functions created.
// 11/19/04 - Thomas Brock removed OS checks.
//////////////////////////////////////////////////////////////////////////

void __fastcall FixupImagePathWorker( LPCTSTR strSvc )
{
	HKEY hKey;
	DWORD dwType;
	DWORD dwSize;
	CString strTemp;
	TCHAR strPath[MAX_PATH];

	strTemp = _T("SYSTEM\\CurrentControlSet\\Services\\");
	strTemp += strSvc;

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, strTemp, NULL, KEY_ALL_ACCESS, &hKey ) )
	{
		dwSize = sizeof( TCHAR ) * MAX_PATH;
		if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey, _T("ImagePath"), NULL, &dwType, (LPBYTE) strPath, &dwSize ) )
		{
			// reusing strTemp here...
			strTemp.Empty();
			if( _T('\"') != strPath[0] )
			{
				strTemp = _T("\"");
			}
			strTemp += strPath;
			if( _T('\"') != strPath[_tcslen(strPath)-1] )
			{
				strTemp += _T("\"");
			}
			if( _tcscmp( strPath, strTemp ) )
			{
				vpstrncpy( strPath, strTemp, sizeof (strPath) );
				RegSetValueEx( hKey, _T("ImagePath"), NULL, dwType, (CONST BYTE *) strPath, sizeof( TCHAR ) * ( _tcslen( strPath ) + 1 ) );
			}
			RegCloseKey( hKey );
		}
	}
}

UINT _stdcall FixupImagePaths( MSIHANDLE /*hInstall*/ )
{
/*
	// brian schlatter - we might want to conditional this since MSI 3.01 will 
	// add quotes for us. For now, just comment it out.
	FixupImagePathWorker( _T("ccEvtMgr") );
	FixupImagePathWorker( _T("ccProxy") );
	FixupImagePathWorker( _T("ccPwdSvc") );
	FixupImagePathWorker( _T("ccSetMgr") );
	FixupImagePathWorker( _T("DefWatch") );
	FixupImagePathWorker( _T("SAVRoam") );
	FixupImagePathWorker( _T("SNDSrvc") );
	FixupImagePathWorker( _T("Symantec AntiVirus") );
	FixupImagePathWorker( _T("SymSecurePort") );
*/
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: RemoveVDBDirs()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//		Remove directories created when *.VDB files are expanded.
//      Needs CustomActionData set to [I2_LDVP.VDB] or [I2_LDVP.VDB_9X]
//
//////////////////////////////////////////////////////////////////////////
// 1/13/04 - JLENZ function created.
//////////////////////////////////////////////////////////////////////////

UINT _stdcall RemoveVDBDirs( MSIHANDLE hInstall )
{
	MSILogMessage( hInstall, "enter RemoveVDBDirs" );

	UINT nRet = ERROR_INSTALL_FAILURE;

	try
	{
		CString strData = GetMSIProperty( hInstall, "CustomActionData" );
		if( strData.GetLength() == 0 )
		{
			throw( _T("No CustomActionData present for RemoveVDBDirs.") );
		}

        DeleteDir( hInstall, (TCHAR*)strData.LockBuffer(), "*.*" );
		strData.ReleaseBuffer();

		nRet = ERROR_SUCCESS;
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	MSILogMessage( hInstall, "exit RemoveVDBDirs" );

	return ERROR_SUCCESS; // always return ERROR_SUCCESS
}

//////////////////////////////////////////////////////////////////////////
//
// Function: RemoveVirusProtect6Hive()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS if all ok
//
// Description:  
//		Remove the VP6 hive if we have detected that the usage count is zero
//
//////////////////////////////////////////////////////////////////////////
// 2/19/04 - DKOWALYSHYN function created.
//////////////////////////////////////////////////////////////////////////

UINT _stdcall RemoveVirusProtect6Hive( MSIHANDLE hInstall )
{
	MSILogMessage( hInstall, "RemoveVirusProtect6Hive" );

	// Remove the install backup keys
	CRegistryEntry reg( HKEY_LOCAL_MACHINE, "Software\\INTEL\\LANDesk\\VirusProtect6", NULL );
	reg.DeleteKey( TRUE );

	MSILogMessage( hInstall, "exit RemoveVirusProtect6Hive" );

	return ERROR_SUCCESS; // always return ERROR_SUCCESS
}


//////////////////////////////////////////////////////////////////////////
//
// Function: W9XSharedFileFixerUpper
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS always.
//
// Description:
//	See defect 1-28QW8B.
//	Apparently, all of our shared file are ending
//	up with SharedDll counts one higher than they should be.  This
//	only occurs during migration.  My theory is that FileCosting
//	is run while the previous product is installed,
//	and is causing our install to think that the files exist, even
//	after they are removed by RemoveExistingProducts.  This custom
//	action will examine the SharedDll values and reduce them by one
//	if they have a value higher than one.
//
//	Must be run in defered mode.
//
//	Needs to have the CommonFilesFolder value sent as CustomActionData.
//
//////////////////////////////////////////////////////////////////////////
// 01/13/04 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////

#define NUM_FILES 13
const TCHAR *g_pszSharedFiles[NUM_FILES] = {_T("LotNtsUI.ocx"),
											_T("ExchngUI.ocx"),
											_T("IMailUI.ocx"),
											_T("LDDateTM.ocx"),
											_T("LDVPCtls.ocx"),
											_T("LDVPDlgs.ocx"),
											_T("LDVPTask.ocx"),
											_T("LDVPUI.ocx"),
											_T("LDVPView.ocx"),
											_T("Transman.dll"),
											_T("scandlgs.dll"),
											_T("vpshell2.dll"),
											_T("webshell.dll")};

UINT __stdcall SharedFileFixerUpper( MSIHANDLE hInstall )
{
	HKEY hKey;
	DWORD dwValue, dwType, dwSize;

	CString strFileName;
	CString strCommonDir = GetMSIProperty( hInstall, "CustomActionData" );

	AddSlash( strCommonDir );
	strCommonDir += _T("Symantec Shared\\SSC\\");

	if( 0 == strCommonDir.GetLength() )
	{
		MSILogMessage( hInstall, _T("Error getting CustomActionData for SharedFileFixerUpper.") );
	}
	else
	{
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"), NULL, KEY_ALL_ACCESS, &hKey ) )
		{
			for( int i = 0; i < NUM_FILES; i++ )
			{
				// Generate file name..
				strFileName = strCommonDir;
				strFileName += g_pszSharedFiles[i];

				dwSize = 4;
				if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey, strFileName, NULL, &dwType, (LPBYTE) &dwValue, &dwSize ) )
				{
					if( ( REG_DWORD == dwType ) && ( dwValue > 1 ) )
					{
						dwValue--;
						RegSetValueEx( hKey, strFileName, NULL, dwType, (CONST BYTE *) &dwValue, dwSize );
						MSILogMessage( hInstall, _T("Reducing SharedDLLs count for:") );
						MSILogMessage( hInstall, strFileName );
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: DisableCancelButton()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:  
//	Disables the cancel button during a uninstall 
//////////////////////////////////////////////////////////////////////////
// 2/27/04 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall DisableCancelButton( MSIHANDLE hInstall )
{
	PMSIHANDLE hRec = MsiCreateRecord(2);
	
	if( NULL != hRec )
	{
		MsiRecordSetInteger(hRec, 1, 2);
		MsiRecordSetInteger(hRec, 2, 0);

		if( IDOK == MsiProcessMessage( hInstall, INSTALLMESSAGE_COMMONDATA, hRec ) )
			MSILogMessage( hInstall, _T("Disabling cancel button to prevent user initiated rollbacks") );
		else
			MSILogMessage( hInstall, _T("Failed to disable cancel button") );
	}
	else
		MSILogMessage( hInstall, _T("Unable to create records") );

	return ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////
//
// Function: MsiWindowsSecurityCenterAddAV()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:
//	Undoes the actions of MsiWindowsSecurityCenterRemoveAV, putting us back
//  into the WMI repository for Windows Security Center.  This is currently
//  unused, since RTVScan does this for us on startup, but in case it's ever
//  needed, here it is.
//////////////////////////////////////////////////////////////////////////
// 5/28/04 - DLUXFORD function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiWindowsSecurityCenterAddAV( MSIHANDLE hInstall )
{
    WindowsSecurityCenter::WMIfailPoint     failurePoint                    = WindowsSecurityCenter::WMIfailPoint_CreateLocator;
    std::string                             failurePointDescription;
    TCHAR                                   versionString[MAX_PATH+1]       = {""};
    TCHAR                                   errorString[2*MAX_PATH+1]       = {""};
    HRESULT                                 returnValHR                     = E_FAIL;
    HRESULT                                 comInitializationStatus         = E_FAIL;

    comInitializationStatus = CoInitialize(NULL);

    if (SUCCEEDED(comInitializationStatus))
    {
        vpsnprintf(versionString, sizeof (versionString), "%d.%d.%d.%d", MAINPRODUCTVERSION, SUBPRODUCTVERSION, INLINEPRODUCTVERSION, BUILDNUMBER);
        returnValHR = WindowsSecurityCenter::UpdateAntivirusStatus(WindowsSecurityCenter::WMI_ID_SCS_Antivirus, IDS_WSC_COMPANYNAME, IDS_WSC_PRODUCTDISPLAYNAME, versionString, true, true, &failurePoint);
        if (FAILED(returnValHR))
        {
            failurePointDescription = WindowsSecurityCenter::WMIfailPointToString(failurePoint);
            vpsnprintf(errorString, sizeof (errorString), "MsiWindowsSecurityCenterAddAV:  Warning, error 0x%08x returned when registering with Windows Security Center WMI repository.  Failure point:  %s\n", returnValHR, failurePointDescription.c_str());
            MSILogMessage(hInstall, errorString);
        }
    }
    else
    {
        vpsnprintf(errorString, sizeof (errorString), "MsiWindowsSecurityCenterAddAV:  Error 0x%08x returned initializing COM.\n", returnValHR);
    }
    returnValHR = WindowsSecurityCenter::InstallDisableLegacy(WindowsSecurityCenter::LEGACYDETECT_SAV, szReg_Key_Main "\\" szReg_Key_WindowsSecurityCenter_LegacyDetectBackup, szReg_Val_WindowsSecurityCenter_LegacyDetectDisabled);
    if (FAILED(returnValHR))
    {
        vpsnprintf(errorString, sizeof (errorString), "MsiWindowsSecurityCenterAddAV:  Warning, error 0x%08x returned when disabling Windows Security Center legacy detection.", returnValHR);
        MSILogMessage(hInstall, errorString);
    }

    // Shutdown and return
    if (SUCCEEDED(comInitializationStatus))
        CoUninitialize();
    // A failure here is normal on systems pre XPSP2, and can even fail correctly on XPSP2 systems if the Windows Security Center MOF is deregistered.
    // Also, RTVScan will reattempt this.
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiWindowsSecurityCenterRemoveAV()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:
//  Remove our AV registration from the WMI Windows Security Center
//  and re-enables WSC's legacy detection of us.
//////////////////////////////////////////////////////////////////////////
// 5/28/04 - DLUXFORD function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiWindowsSecurityCenterRemoveAV( MSIHANDLE hInstall )
{
    WindowsSecurityCenter::WMIfailPoint     failurePoint                    = WindowsSecurityCenter::WMIfailPoint_CreateLocator;
    std::string                             failurePointDescription;
    TCHAR                                   errorString[2*MAX_PATH+1]       = {""};
    HRESULT                                 comInitializationStatus         = E_FAIL;
    HRESULT                                 returnValHR                     = E_FAIL;

    comInitializationStatus = CoInitialize(NULL);

    if (SUCCEEDED(comInitializationStatus))
    {
        returnValHR = WindowsSecurityCenter::RemoveAntivirusStatus(WindowsSecurityCenter::WMI_ID_SCS_Antivirus, &failurePoint);
        if (FAILED(returnValHR))
        {
            failurePointDescription = WindowsSecurityCenter::WMIfailPointToString(failurePoint);
            vpsnprintf(errorString, sizeof (errorString), "MsiWindowsSecurityCenterRemoveAV:  Error 0x%08x returned when unregistering from Windows Security Center WMI repository.  Failure point:  %s\n", returnValHR, failurePointDescription.c_str());
            MSILogMessage(hInstall, errorString);
        }
    }
    else
    {
        vpsnprintf(errorString, sizeof (errorString), "MsiWindowsSecurityCenterRemoveAV:  Error 0x%08x returned initializing COM.\n", returnValHR);
    }
    returnValHR = WindowsSecurityCenter::InstallRestoreLegacy(WindowsSecurityCenter::LEGACYDETECT_SAV, szReg_Key_Main "\\" szReg_Key_WindowsSecurityCenter_LegacyDetectBackup, szReg_Val_WindowsSecurityCenter_LegacyDetectDisabled);
    if (FAILED(returnValHR))
    {
        vpsnprintf(errorString, sizeof (errorString), "MsiWindowsSecurityCenterRemoveAV:  Error 0x%08x returned when re-enabling Windows Security Center legacy detection.", returnValHR);
        MSILogMessage(hInstall, errorString);
    }
	returnValHR = WindowsSecurityCenter::RestoreAlert(WindowsSecurityCenter::Alert_Antivirus, szReg_Key_Main "\\" szReg_Val_WindowsSecurityCenter_AVAlertBackup);
    if (FAILED(returnValHR))
    {
        vpsnprintf(errorString, sizeof (errorString), "MsiWindowsSecurityCenterRemoveAV:  Error 0x%08x returned when restoring alerting.", returnValHR);
        MSILogMessage(hInstall, errorString);
    }

    // Shutdown and return
    if (SUCCEEDED(comInitializationStatus))
        CoUninitialize();

    // A failure here is normal on systems pre XPSP2, and can even fail correctly on XPSP2 systems if the Windows Security Center MOF is deregistered.
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiValidateWSCproperties()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:
//  Validates the WSC configuration properties and corrects
//  them if they are invalid/out of range.
//////////////////////////////////////////////////////////////////////////
// 10/19/04 - DLUXFORD function rewritten.
// 9/1/04 - JFREEMAN function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall MsiValidateWSCproperties( MSIHANDLE hInstall )
{
	DWORD				defsUpToDate				= 0;
	DWORD				avAlertControl				= 0;
	TCHAR				stringBuffer[MAX_PATH+1]	= {""};
	DWORD				stringBufferSize			= 0;
	DWORD				returnValDW					= ERROR_FILE_NOT_FOUND;

	// Validate the Defs Up-to-date property
	stringBufferSize = sizeof(stringBuffer)/sizeof(stringBuffer[0]);
	MsiGetProperty(hInstall, PROPERTY_WSC_DEFSUPTODATE, stringBuffer, &stringBufferSize);
	defsUpToDate = atoi(stringBuffer);
	if ((defsUpToDate < 1) || (defsUpToDate > 90))
	{
		MSILogMessage(hInstall, _T("ValidateWSCproperties() : An invalid setting for the WSCAVUPTODATE property was attempted."));
		returnValDW = MsiSetProperty(hInstall, PROPERTY_WSC_DEFSUPTODATE, "30");
		if (returnValDW == ERROR_SUCCESS)
			MSILogMessage(hInstall, _T("ValidateWSCproperties() : Defaulting to 30 days."));
		else
			MSILogMessage(hInstall, _T("ValidateWSCproperties() : An invalid setting for the WSCAVUPTODATE property was attempted.  Unable to reassign the property to the default of 30 days."));
	}

	// Validate the AV Alert Control property
	stringBufferSize = sizeof(stringBuffer)/sizeof(stringBuffer[0]);
	MsiGetProperty(hInstall, PROPERTY_WSC_AVALERTCONTROL, stringBuffer, &stringBufferSize);
	avAlertControl = atoi(stringBuffer);
	if ((avAlertControl != WSCAVALERT_ENABLE) && (avAlertControl != WSCAVALERT_DISABLE) && (avAlertControl != WSCAVALERT_NOACTION))
	{
		MSILogMessage(hInstall, _T("ValidateWSCAVAlertProperty() : An invalid setting for the WSCAVALERT property was attempted."));
		returnValDW = MsiSetProperty(hInstall, PROPERTY_WSC_AVALERTCONTROL, "1");
		if (returnValDW == ERROR_SUCCESS)
			MSILogMessage(hInstall, _T("ValidateWSCproperties() : Defaulting to the disabled state."));
		else
			MSILogMessage(hInstall, _T("ValidateWSCproperties() : An invalid setting for the WSCAVALERT property was attempted.  Unable to reassign the property to the default disabled state."));
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiPersistSAVSettings()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:
//  Ensures WSC control properties are maintained across an
//  update to a later build.
//////////////////////////////////////////////////////////////////////////
// 9/1/04 - JFREEMAN function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall MsiPersistSAVSettings( MSIHANDLE hInstall )
// Persist specific settings during a migration that may have been replaced by RestoreSAVSettings.
// This CA should be safe for non-administrative installs as I am making changes to the HKCU registry
// that was backed up by SaveSAVSettings during a migration.  When RestoreSAVSettings runs later
// in Deferred In System Context mode, these settings will propagate back where we need them under HKLM.
// Note that this CA is conditioned to a build over build settings migration.
// A roll back may be difficult because we don't know which settings the user passed in to MSI and 
// which were the default settings.  These keys exist in the install, so the worst case scenario
// is that the user must run the install again to get the settings they want.  They keys are created in
// the registry table for HKLM and removed recursively in RestoreSAVSettings for HKCU, so they will 
// not be left behind.
{
	const TCHAR			szProp_WSC_Control[]						= "WSCCONTROL";
	const TCHAR			szProp_WSC_AVAlert[]						= "WSCAVALERT";
	const TCHAR			szProp_WSC_AVUpToDate[]						= "WSCAVUPTODATE";
	// Not truly worthy of ClientReg.h
	const TCHAR			szReg_MigrateHKLM_Intel_AdminOnly[]			= _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE\\AdministratorOnly\\General");
	HKEY				hSymantecHKCUKey							= 0;
	
	// Persist specific new settings under HKEY_LOCAL_MACHINE\\Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion\\AdministratorOnly\\General
	try
	{
		// From OpenMigrateTreeHKLM() ..
		if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER,
			szReg_MigrateHKLM_Intel_AdminOnly, 0, NULL, REG_OPTION_NON_VOLATILE, 
			KEY_READ | KEY_WRITE, NULL,  &hSymantecHKCUKey, NULL))
		{
			throw( _T("Unable to open the Symantec key.") );
		}

		// If this CA is triggered (default for a build over build migration) default values will be written out
		// along with any new values if present.
		
		{ // "WindowsSecurityCenterControl" and the MSI property WSCCONTROL
			DWORD				cchValueBuf									= 0;
			TCHAR*				szValueBuf									= NULL;
			DWORD				dwPropertyValue								= 0;
			
			if( ERROR_MORE_DATA != MsiGetProperty(hInstall, szProp_WSC_Control, _T(""), &cchValueBuf) )
			{
				throw( _T("Unable to read the WSCCONTROL property.") );
			}
			
			++cchValueBuf; // on output does not include terminating null, so add 1
			szValueBuf = new TCHAR[cchValueBuf];
			if (!szValueBuf)
			{
				throw( _T("Out of memory.") );
			}
			
			if( ERROR_SUCCESS != MsiGetProperty(hInstall, szProp_WSC_Control, szValueBuf, &cchValueBuf) )
			{
				throw( _T("Unable to read the WSCCONTROL property.") );
			}
			
			dwPropertyValue = _ttol(szValueBuf);
			
			if( ERROR_SUCCESS != RegSetValueEx( hSymantecHKCUKey, szReg_Val_WindowsSecurityCenter_Control, 0,
				(DWORD)REG_DWORD, (LPBYTE)&dwPropertyValue, sizeof(DWORD)) )
			{
				throw( _T("Unable to store the new WSCCONTROL value.") );
			}
			
			delete [] szValueBuf;
		} // End "WindowsSecurityCenterControl" and the MSI property WSCCONTROL
		
		{ // "WSCAVAlert" and the MSI property WSCAVALERT
			DWORD				cchValueBuf									= 0;
			TCHAR*				szValueBuf									= NULL;
			DWORD				dwPropertyValue								= 0;
			
			if( ERROR_MORE_DATA != MsiGetProperty(hInstall, szProp_WSC_AVAlert, _T(""), &cchValueBuf) )
			{
				throw( _T("Unable to read the WSCAVALERT property.") );
			}
			
			++cchValueBuf; // on output does not include terminating null, so add 1
			szValueBuf = new TCHAR[cchValueBuf];
			if (!szValueBuf)
			{
				throw( _T("Out of memory.") );
			}
			
			if( ERROR_SUCCESS != MsiGetProperty(hInstall, szProp_WSC_AVAlert, szValueBuf, &cchValueBuf) )
			{
				throw( _T("Unable to read the WSCAVALERT property.") );
			}
			
			dwPropertyValue = _ttol(szValueBuf);
			
			if( ERROR_SUCCESS != RegSetValueEx( hSymantecHKCUKey, szReg_Val_WSCAVAlert, 0,
				(DWORD)REG_DWORD, (LPBYTE)&dwPropertyValue, sizeof(DWORD)) )
			{
				throw( _T("Unable to store the new WSCAVALERT value.") );
			}
			
			delete [] szValueBuf;
		} // End "WSCAVAlert" and the MSI property WSCAVALERT
		
		{ // "WSCDefsUpToDate" and the MSI property WSCAVUPTODATE
			DWORD				cchValueBuf									= 0;
			TCHAR*				szValueBuf									= NULL;
			DWORD				dwPropertyValue								= 0;
			
			if( ERROR_MORE_DATA != MsiGetProperty(hInstall, szProp_WSC_AVUpToDate, _T(""), &cchValueBuf) )
			{
				throw( _T("Unable to read the WSCDEFSUPTODATE property.") );
			}
			
			++cchValueBuf; // on output does not include terminating null, so add 1
			szValueBuf = new TCHAR[cchValueBuf];
			if (!szValueBuf)
			{
				throw( _T("Out of memory.") );
			}
			
			if( ERROR_SUCCESS != MsiGetProperty(hInstall, szProp_WSC_AVUpToDate, szValueBuf, &cchValueBuf) )
			{
				throw( _T("Unable to read the WSCDEFSUPTODATE property.") );
			}
			
			dwPropertyValue = _ttol(szValueBuf);
			
			if( ERROR_SUCCESS != RegSetValueEx( hSymantecHKCUKey, szReg_Val_WindowsSecurityCenter_DefsUpToDateCriteria, 0,
				(DWORD)REG_DWORD, (LPBYTE)&dwPropertyValue, sizeof(DWORD)) )
			{
				throw( _T("Unable to store the new WSCDEFSUPTODATE value.") );
			}
			
			delete [] szValueBuf;
		} // End "WSCDefsUpToDate" and the MSI property WSCAVUPTODATE
		
	}
	catch( TCHAR* szError )
	{
		MSILogMessage(hInstall, _T("PersistSAVSettings() : "));
		MSILogMessage(hInstall, szError);
	}

	if( hSymantecHKCUKey )
	{
		RegCloseKey(hSymantecHKCUKey);
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: StartVPTray()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:
//    Launches VPTray.exe.
//////////////////////////////////////////////////////////////////////////
// 2/24/05 - John Lenz function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall StartVPTray( MSIHANDLE hInstall )
{
    // Using ShellExecute here in order to get the App Path registry settings
    // to be effective. Without them it can't find the symantec shared dlls 
    // (ccL35.dll is the error I've seen).

    LPCTSTR szCmdLine = _T("-l -s -i");

    if ((int)ShellExecute(NULL,_T("open"),_T("VPTRAY.EXE"),szCmdLine,NULL, SW_SHOWDEFAULT) > 32)
    {
    	MSILogMessage(hInstall, _T("StartVPTray() : VPTray started."));
    }
    else
    {
    	MSILogMessage(hInstall, _T("StartVPTray() : VPTray FAILED to start."));
    }
 
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SetServiceRecovery()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS
//
// Description:
//    Sets the service recovery options for RTVScan.
//    Should be run after InstallServices.
//
//////////////////////////////////////////////////////////////////////////
// 03/07/05 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall SetServiceRecovery( MSIHANDLE hInstall )
{
	// Set service to restart 30 seconds after the first and second failures...
	SC_ACTION sca[]=
	{
		{ SC_ACTION_RESTART, 10 * 1000 }, // First failure...
		{ SC_ACTION_RESTART, 10 * 1000 }, // Second failure...
		{ SC_ACTION_NONE, 0 }  // Subsequent failures...
	};

	SERVICE_FAILURE_ACTIONS sfa;

	sfa.dwResetPeriod = 24 * 60 * 60; // Number of seconds in a day...
	sfa.lpRebootMsg = NULL; // No reboot message, since we aren't rebooting...
	sfa.lpCommand = NULL; // No command to run, since we aren't running a command...
	sfa.cActions = 3; // Number of elements in the sca array, see above...
	sfa.lpsaActions = sca;

	MSILogMessage(hInstall, _T("SetServiceRecovery starting..."));
	
	SC_HANDLE scManager, scService;

	scManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( scManager )
	{
		scService = OpenService( scManager, _T("Symantec AntiVirus"), SERVICE_ALL_ACCESS );
		if( scService )
		{
			if( ChangeServiceConfig2( scService, SERVICE_CONFIG_FAILURE_ACTIONS, (LPVOID) &sfa ) )
			{
				MSILogMessage( hInstall, _T("RTVScan recovery options set successfully.") );
			}
			else
			{
				MSILogMessage( hInstall, _T("Could not set RTVScan recovery options.") );
			}
		}
		CloseServiceHandle( scManager );
	}

	// Do not fail install if service recovery options can't be set.
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: DeleteClientPKI()
//
// Parameters:
//		MSIHANDLE hInstall
//
// Returns:
//		Error_Success	
//
// Description:  
//	Delete the PKI folder structure, since MSI did not create these files
//		it does not know to delete them
//
//////////////////////////////////////////////////////////////////////////
//	02/14/05 - Sean Kennedy
//////////////////////////////////////////////////////////////////////////
UINT _stdcall DeleteClientPKI ( MSIHANDLE hInstall )
{
	DWORD size = 0;
	TCHAR *basepath = NULL;

	try
	{
		// Get our INSTALLDIR
		if( ERROR_MORE_DATA != MsiGetProperty( hInstall, _T("CustomActionData"), "", &size ) )
			throw( _T("MsiGetProperty returned unexepected value when determining CustomActionData size") );

	    basepath = new TCHAR [ size + 1 ];
		if( NULL == basepath )
			throw( _T("Unable to allocate memory to hold CustomActionData") );

		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("CustomActionData"), basepath, &size ) )
			throw( _T("Unable to obtain CustomActionData") );

		// Delete the INSTALLDIR\pki folder structure
		tstring pkipath( basepath );
		pkipath.append( "pki" );
		DeleteDir( hInstall, const_cast<TCHAR *>(pkipath.c_str()), _T("*.*") );
	}
	catch( TCHAR *err )
	{
		MSILogMessage( hInstall, err );
	}

	delete [] basepath;

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: CreateProcessAndWait()
//
// Parameters:
//		LPCTST	Command Line argument
//		LPINT	process return code
//
// Returns:
//		Create process return code	
//
// Description:  
//		Wrapper for CreateProcess api call
//
//////////////////////////////////////////////////////////////////////////
//	06/07/2005 - DKowalyshyn, lifted code from LU
//////////////////////////////////////////////////////////////////////////
BOOL CreateProcessAndWait(LPCTSTR szCmd, LPINT pnExitCode)
{
	PROCESS_INFORMATION	ProcessInfo;
	STARTUPINFO			StartUpInfo;

	ZeroMemory(&StartUpInfo,sizeof(STARTUPINFO));
	StartUpInfo.cb = sizeof(STARTUPINFO); 	

    int nSafeCmdLength = _tcslen(szCmd) + 3;
	TCHAR *szSafeCmd= new TCHAR[nSafeCmdLength];

	// double quote command to prevent possible execution of unexpected program
	if (! _tcschr( szCmd, _T('\"')))
		sssnprintf( szSafeCmd, nSafeCmdLength, "\"%s\"", szCmd );
	else
		_tcscpy( szSafeCmd, szCmd );

	BOOL bSuccess = CreateProcess(NULL, (LPSTR)szSafeCmd, NULL, NULL, FALSE, HIGH_PRIORITY_CLASS, NULL, NULL, &StartUpInfo, &ProcessInfo);

	delete [] szSafeCmd;

	if (bSuccess)
	{
		while (WaitForSingleObject(ProcessInfo.hProcess, 50) == WAIT_TIMEOUT)
		{
			MSG		msg;
		    while (PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
			{
				TranslateMessage(&msg);
			    DispatchMessage(&msg);
			}
		}
		DWORD dwExitCode;
		if (GetExitCodeProcess(ProcessInfo.hProcess, &dwExitCode))
			*pnExitCode = (int)dwExitCode;
		else
			bSuccess = FALSE;

		CloseHandle(ProcessInfo.hProcess);
        CloseHandle(ProcessInfo.hThread);
	}

	return bSuccess;
}
 
 // Check for File or Directory Existence
BOOL FileExists(LPCSTR sFile)
{
#ifdef _WIN32
	return (GetFileAttributes(sFile) != (DWORD) -1);
#else
	CFileStatus status;
	return CFile::GetStatus(sFile,status);
#endif
}
 

//////////////////////////////////////////////////////////////////////////
//
// Function: MsxmlFixup()
//
// Parameters:
//		MSIHANDLE hInstall
//
// Returns:
//		Error_Success	
//
// Description:  
//	Restores msxml3.dll, msxml3r.dll,  msxmlx.cat, and msxmlx.inf
//  to the cache directory before patching.
//////////////////////////////////////////////////////////////////////////
//	05/26/05 - Brian Aljian
//	07/11/05 - Jimmy Hoa - forward port from Almagest, defect 1-44P85Q fix
//////////////////////////////////////////////////////////////////////////
UINT _stdcall MsxmlFixup ( MSIHANDLE hInstall )
{
	CString strSystemFolder, strCDirSystemSFPCacheDir;
	CString strSrcFile, strDestFile, strTemp;

	HKEY hKey = NULL;
	HKEY hSubKey = NULL;
	DWORD dwIndex = 0;
	DWORD dwSize = MAX_PATH;
	TCHAR szName[MAX_PATH] = {0};
	TCHAR szFriendlyName[MAX_PATH] = {0};
	TCHAR szExceptionCatalogName[MAX_PATH] = {0};
	TCHAR szExceptionInfName[MAX_PATH] = {0};
	CString strKey;
	FILETIME fTime;

	try
	{
		// Get the system directory
		if( !GetSystemDirectory(strSystemFolder.GetBuffer(MAX_PATH+1), MAX_PATH+1) )
			throw( _T("Could not get system directory") );

		strSystemFolder.ReleaseBuffer();

		// Get the MSXML cache directory
		strCDirSystemSFPCacheDir = GetMSIProperty( hInstall, "CustomActionData" );
		if( strCDirSystemSFPCacheDir.GetLength() == 0 )
			throw( _T("CDirSystemSFPCacheDir is empty or not set") );

		strSystemFolder.TrimRight( _T("\\") );
		strCDirSystemSFPCacheDir.TrimRight( _T("\\") );

		// Exit if the cache directory *is* the system directory.  In this
		// case the files do not need to be restored.
		if( 0 == strSystemFolder.CompareNoCase( strCDirSystemSFPCacheDir ) )
			throw( _T("SystemFolder and CDirSystemSFPCacheDir are the same, no fix-up necessary") );

		//////////////////////////////////////////////////////////////////////////
		//	Find and copy msxml3.dll.  msxml3.dll is in the system directory.
		//////////////////////////////////////////////////////////////////////////

		bool bSaveDir = false;
 		CString strCDirSystemSFPCacheDir_Save = strCDirSystemSFPCacheDir + _T("-Save");
 		if (FileExists(strCDirSystemSFPCacheDir_Save))
 		{
 			bSaveDir = true;
 			strSystemFolder = strCDirSystemSFPCacheDir_Save;
 		}
  
		strSrcFile = strSystemFolder;
		strSrcFile += _T("\\");
		strSrcFile += SZ_MSXML3DLL;

		strDestFile = strCDirSystemSFPCacheDir;
		strDestFile += _T("\\");
		strDestFile += SZ_MSXML3DLL;

		strTemp.Format( "Copying \"%s\" to \"%s\"...", strSrcFile, strDestFile );
		MSILogMessage( hInstall, strTemp );

		//
 		// TRUE on the bFailIfExists parameter so don't overwrite newer version that a previous
 		// build may have put in.
 		//
 		if (!FileExists(strDestFile))
 		{
 			if( !CopyFile( strSrcFile, strDestFile, TRUE ) )
 				throw( _T("Error copying file") );
 		}
   
		//////////////////////////////////////////////////////////////////////////
		//	Find and copy msxml3r.dll.  msxml3r.dll is in the system directory.
		//////////////////////////////////////////////////////////////////////////

		strSrcFile = strSystemFolder;
		strSrcFile += _T("\\");
		strSrcFile += SZ_MSXML3RDLL;

		strDestFile = strCDirSystemSFPCacheDir;
		strDestFile += _T("\\");
		strDestFile += SZ_MSXML3RDLL;

		strTemp.Format( "Copying \"%s\" to \"%s\"...", strSrcFile, strDestFile );
		MSILogMessage( hInstall, strTemp );

		//
 		// TRUE on the bFailIfExists parameter so don't overwrite newer version that a previous
 		// build may have put in.
 		//
 		if (!FileExists(strDestFile))
 		{
 			if( !CopyFile( strSrcFile, strDestFile, TRUE ) )
 				throw( _T("Error copying file") );
 		}


		//////////////////////////////////////////////////////////////////////////
		//	Find msxmlx.cat and msxmlx.inf.  The location of these files
		//  is in the registry.
		//////////////////////////////////////////////////////////////////////////

		if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, REG_EXCEPTIONCOMPONENTS, 0, KEY_READ, &hKey ) )
		{
			dwSize = MAX_PATH;
			while ( ERROR_NO_MORE_ITEMS != RegEnumKeyEx( hKey, dwIndex, szName, &dwSize, NULL, NULL, NULL, &fTime) )
			{
				strKey = REG_EXCEPTIONCOMPONENTS;
				strKey += _T("\\");
				strKey += szName;

				if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSubKey ) )
				{
					dwSize = MAX_PATH;
					if ( ERROR_SUCCESS == RegQueryValueEx( hSubKey, REG_FRIENDLYNAME, NULL, NULL, (LPBYTE)&szFriendlyName, &dwSize ) )
					{
						if ( 0 == strcmp( szFriendlyName, REG_MSXML ) )
						{
							strTemp.Format( "Found friendly name \"%s\"", szFriendlyName );
							MSILogMessage( hInstall, strTemp );

							dwSize = MAX_PATH;
							if ( ERROR_SUCCESS != RegQueryValueEx( hSubKey, REG_EXCEPTIONCATALOGNAME, NULL, NULL, (LPBYTE)&szExceptionCatalogName, &dwSize ) )
								throw( _T("Error getting ExceptionCatalogName") );

							dwSize = MAX_PATH;
							if ( ERROR_SUCCESS != RegQueryValueEx( hSubKey, REG_EXCEPTIONINFNAME, NULL, NULL, (LPBYTE)&szExceptionInfName, &dwSize ) )
								throw( _T("Error getting ExceptionInfName") );
						}
						else
						{
							strTemp.Format( "Skipping non-matching friendly name \"%s\"", szFriendlyName );
							MSILogMessage( hInstall, strTemp );
						}
					}

					RegCloseKey( hSubKey );
				}
				else
				{
					strTemp.Format( "Could not open registry key: %s", szName );
					throw( strTemp );
				}

				++dwIndex;
				dwSize = MAX_PATH;
			}

			RegCloseKey( hKey );
		}
		else
			throw( _T("Could not open ExceptionComponents registry key.") );

		// Verify that the file names (without extension) are "msxmlx"
		// (so an attacker can't maliciously change the registry to make
		// us copy a random file in system context).

		_splitpath( szExceptionCatalogName, NULL, NULL, szName, NULL );
		if ( 0 != stricmp( szName, SZ_MSXMLX ) )
			throw( _T("ExceptionCatalogName was not valid.") );

		_splitpath( szExceptionInfName, NULL, NULL, szName, NULL );
		if ( 0 != stricmp( szName, SZ_MSXMLX ) )
			throw( _T("ExceptionInfName was not valid.") );

		//////////////////////////////////////////////////////////////////////////
		//	Copy msxmlx.cat
		//////////////////////////////////////////////////////////////////////////

	 		if (bSaveDir)
 		{
 			strSrcFile = strSystemFolder;
 			strSrcFile += _T("\\");
 			strSrcFile += SZ_MSXMLXCAT;
 		}
 		else strSrcFile = szExceptionCatalogName;
 
 		strDestFile = strCDirSystemSFPCacheDir;
 		strDestFile += _T("\\");
 		strDestFile += SZ_MSXMLXCAT;
 
 		strTemp.Format( "Copying \"%s\" to \"%s\"...", strSrcFile, strDestFile );
 		MSILogMessage( hInstall, strTemp );
 
 		//
 		// TRUE on the bFailIfExists parameter so don't overwrite newer version that a previous
 		// build may have put in.
 		//
 		if (!FileExists(strDestFile))
 		{
 			if( !CopyFile( strSrcFile, strDestFile, TRUE ) )
 				throw( _T("Error copying file") );
 		}
 
 		//////////////////////////////////////////////////////////////////////////
 		//	Copy msxmlx.inf
 		//////////////////////////////////////////////////////////////////////////
 
 		if (bSaveDir)
 		{
 			strSrcFile = strSystemFolder;
 			strSrcFile += _T("\\");
 			strSrcFile += SZ_MSXMLXINF;
 		}
 		else strSrcFile = szExceptionInfName;
 
 		strDestFile = strCDirSystemSFPCacheDir;
 		strDestFile += _T("\\");
 		strDestFile += SZ_MSXMLXINF;
 
 		strTemp.Format( "Copying \"%s\" to \"%s\"...", strSrcFile, strDestFile );
 		MSILogMessage( hInstall, strTemp );
 
 		//
 		// TRUE on the bFailIfExists parameter so don't overwrite newer version that a previous
 		// build may have put in.
 		//
 		if (!FileExists(strDestFile))
 		{
 			if( !CopyFile( strSrcFile, strDestFile, TRUE ) )
 				throw( _T("Error copying file") );
 		}
 
 		// Delete Save dir
 		if (bSaveDir)
 		{
 			strDestFile = strCDirSystemSFPCacheDir_Save;
 			strDestFile += _T("\\");
 			strDestFile += SZ_MSXML3DLL;
 			DeleteFile(strDestFile);
 
 			strDestFile = strCDirSystemSFPCacheDir_Save;
 			strDestFile += _T("\\");
 			strDestFile += SZ_MSXML3RDLL;
 			DeleteFile(strDestFile);
 
 			strDestFile = strCDirSystemSFPCacheDir_Save;
 			strDestFile += _T("\\");
 			strDestFile += SZ_MSXMLXCAT;
 			DeleteFile(strDestFile);
 
 			strDestFile = strCDirSystemSFPCacheDir_Save;
 			strDestFile += _T("\\");
 			strDestFile += SZ_MSXMLXINF;
 			DeleteFile(strDestFile);
 
 			RemoveDirectory(strCDirSystemSFPCacheDir_Save);
 		}
 	}
 
 	catch( TCHAR *err )
 	{
 		MSILogMessage( hInstall, err );
 	}
 
 	return ERROR_SUCCESS;
 }

 //////////////////////////////////////////////////////////////////////////
 //
 // Function: MsxmlFixupSaveBefore()
 //
 // Parameters:
 //		MSIHANDLE hInstall
 //
 // Returns:
 //		Error_Success	
 //
 // Description:  
 //	Saves current msxml3.dll, msxml3r.dll,  msxmlx.cat, and msxmlx.inf
 //  in the cache directory before patching. Executed during a Patch only!
 //////////////////////////////////////////////////////////////////////////
 //	08/26/05 - Michael Lee
 //////////////////////////////////////////////////////////////////////////
 UINT _stdcall MsxmlFixupSaveBefore ( MSIHANDLE hInstall )
 {
 	CString strSystemFolder, strCDirSystemSFPCacheDir;
 	CString strSrcFile, strDestFile, strTemp;
 
 	HKEY hKey = NULL;
 	HKEY hSubKey = NULL;
 	DWORD dwIndex = 0;
 	DWORD dwSize = MAX_PATH;
 	TCHAR szName[MAX_PATH] = {0};
 	TCHAR szFriendlyName[MAX_PATH] = {0};
 	TCHAR szExceptionCatalogName[MAX_PATH] = {0};
 	TCHAR szExceptionInfName[MAX_PATH] = {0};
 	CString strKey;
 	FILETIME fTime;
 
 	try
 	{
 		// Get the MSXML cache directory
 		strCDirSystemSFPCacheDir = GetMSIProperty( hInstall, "CustomActionData" );
 		if( strCDirSystemSFPCacheDir.GetLength() == 0 )
 		{
 			strCDirSystemSFPCacheDir = GetMSIProperty( hInstall, "CDirSystemSFPCacheDir.7E4F6CB4_E769_4917_AA7E_0E3CA074ABB3" );
 			if( strCDirSystemSFPCacheDir.GetLength() == 0 )
 				throw( _T("CDirSystemSFPCacheDir is empty or not set") );
 		}
 
 		strCDirSystemSFPCacheDir.TrimRight( _T("\\") );
 
 		//////////////////////////////////////////////////////////////////////////
 		//	Find and copy msxml3.dll.  msxml3.dll is in the system directory.
 		//////////////////////////////////////////////////////////////////////////
 
 		strSrcFile = strCDirSystemSFPCacheDir;
 		strSrcFile += _T("\\");
 		strSrcFile += SZ_MSXML3DLL;
 
 		strDestFile = strCDirSystemSFPCacheDir;
 		strDestFile += _T("-Save");
 		CreateDirectory (strDestFile, NULL);
 		strDestFile += _T("\\");
 		strDestFile += SZ_MSXML3DLL;
 
 		strTemp.Format( "Copying \"%s\" to \"%s\"...", strSrcFile, strDestFile );
 		MSILogMessage( hInstall, strTemp );
 
 		//
 		// Always overwrite
 		//
 		if( !CopyFile( strSrcFile, strDestFile, FALSE ) )
 				throw( _T("Error copying file") );
 
 		//////////////////////////////////////////////////////////////////////////
 		//	Find and copy msxml3r.dll.  msxml3r.dll is in the system directory.
 		//////////////////////////////////////////////////////////////////////////
 
 		strSrcFile = strCDirSystemSFPCacheDir;
 		strSrcFile += _T("\\");
 		strSrcFile += SZ_MSXML3RDLL;
 
 		strDestFile = strCDirSystemSFPCacheDir;
 		strDestFile += _T("-Save");
 		strDestFile += _T("\\");
 		strDestFile += SZ_MSXML3RDLL;
 
 		strTemp.Format( "Copying \"%s\" to \"%s\"...", strSrcFile, strDestFile );
 		MSILogMessage( hInstall, strTemp );
 
 		//
 		// Always overwrite
 		//
 		if( !CopyFile( strSrcFile, strDestFile, FALSE ) )
 			throw( _T("Error copying file") );
 
 		
 		//////////////////////////////////////////////////////////////////////////
 		//	Copy msxmlx.cat
 		//////////////////////////////////////////////////////////////////////////
 		strSrcFile = strCDirSystemSFPCacheDir;
 		strSrcFile += _T("\\");
 		strSrcFile += SZ_MSXMLXCAT;
   
   		strDestFile = strCDirSystemSFPCacheDir;
 		strDestFile += _T("-Save");
   		strDestFile += _T("\\");
   		strDestFile += SZ_MSXMLXCAT;
   
   		strTemp.Format( "Copying \"%s\" to \"%s\"...", strSrcFile, strDestFile );
   		MSILogMessage( hInstall, strTemp );
   
 		//
 		// Always overwrite
 		//
   		if( !CopyFile( strSrcFile, strDestFile, FALSE ) )
   			throw( _T("Error copying file") );
   
   		//////////////////////////////////////////////////////////////////////////
   		//	Copy msxmlx.inf
   		//////////////////////////////////////////////////////////////////////////
 		strSrcFile = strCDirSystemSFPCacheDir;
 		strSrcFile += _T("\\");
 		strSrcFile += SZ_MSXMLXINF;
   
   		strDestFile = strCDirSystemSFPCacheDir;
 		strDestFile += _T("-Save");
   		strDestFile += _T("\\");
   		strDestFile += SZ_MSXMLXINF;
   
   		strTemp.Format( "Copying \"%s\" to \"%s\"...", strSrcFile, strDestFile );
   		MSILogMessage( hInstall, strTemp );
   
 		//
 		// Always overwrite
 		//
   		if( !CopyFile( strSrcFile, strDestFile, FALSE ) )
   			throw( _T("Error copying file") );
   	}
   
   	catch( TCHAR *err )
   	{
   		MSILogMessage( hInstall, err );
   	}
   
   	return ERROR_SUCCESS;
}
