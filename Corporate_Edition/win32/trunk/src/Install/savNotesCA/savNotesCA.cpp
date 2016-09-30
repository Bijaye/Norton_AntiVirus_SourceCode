// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "savNotesCA.h"
#include "vpstrutils.h"
#include "SymSaferRegistry.h"

UINT _stdcall WriteNotesIniValues( MSIHANDLE hInstall )
{
	TCHAR szNotesPath[MAX_PATH] = {0};
	TCHAR szSAVPath[MAX_PATH] = {0};

	try
	{
		if( ERROR_SUCCESS != GetMSIProperty( hInstall, szSAVPath, _T("CustomActionData") ) )
			throw( _T("Unable to get CustomActionData") );

		if( FALSE == GetNotesDir( szNotesPath, MAX_PATH ) )
			throw( _T("Unable to get the Notes folder") );
		
		if( FALSE == CopyDLLToNotesDir( szSAVPath, szNotesPath ) )
			throw( _T("Unable to copy the nLNVP.dll to the Notes folder") );
		
		if( FALSE == UpdateNotesINI( hInstall, szNotesPath ) )
			throw( _T("Unable to update the notes.ini file") );
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	return ERROR_SUCCESS;
}

BOOL GetNotesDir( TCHAR *szNotesPath, DWORD dwSize )
{
	BOOL bRet = FALSE;
	HKEY hKey = NULL;
	DWORD dwType = 0;
	DWORD dwBuffSize = dwSize;

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, NOTES_APPPATH, 0, KEY_READ, &hKey ) )
	{
		if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey, "Path", NULL, &dwType, (LPBYTE)szNotesPath, &dwBuffSize ) )
		{
			bRet = TRUE;
		}
		
		if ((!bRet) || (lstrcmp( szNotesPath, "" )== 0))
		{
			dwBuffSize = dwSize; // reset it in case previous call changed it.
			if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey, _T(""), NULL, &dwType, (LPBYTE)szNotesPath, &dwBuffSize ) )
			{
				if ((szNotesPath) && (lstrcmp( szNotesPath, "" ) != 0))
				{
					TCHAR *pLastBackSlash =_tcsrchr(szNotesPath, '\\');
					if (pLastBackSlash)
					{
						TCHAR *pNotesExe = pLastBackSlash;
						pNotesExe += sizeof(TCHAR);
						if (lstrcmp(pNotesExe, _T("notes.exe")) == 0)
						{
							// strip the path
							szNotesPath[_tcslen(szNotesPath) - _tcslen(pLastBackSlash)] = '\0';
						}
					}
				}
				bRet = TRUE;
			}
		}

		RegCloseKey( hKey );
	}

	return bRet;
}

BOOL CopyDLLToNotesDir( TCHAR *szSAVPath, TCHAR *szNotesPath )
{
	BOOL bRet = FALSE;

	TCHAR szSource[MAX_PATH] = {0};
	TCHAR szDest[MAX_PATH] = {0};

	// Ensure that path+"\nlnvp.dll" is less than MAX_PATH characters
	if( ((_tcslen(szSAVPath)+12) <= MAX_PATH) && ((_tcslen(szNotesPath)+12) <= MAX_PATH) )
	{
		vpsnprintf( szSource, sizeof (szSource), "%s\\nLNVP.dll", szSAVPath );
		vpsnprintf( szDest,   sizeof (szDest),   "%s\\nLNVP.dll", szNotesPath );

		if( FALSE != CopyFile( szSource, szDest, FALSE ) )
			bRet = TRUE;
	}

	return bRet;
}

BOOL UpdateNotesINI( MSIHANDLE hInstall, TCHAR *szNotesPath )
{
	BOOL bRet = TRUE;
	HMODULE hDLL = NULL;
	TCHAR szFilename[MAX_PATH] = {0};
	TCHAR szCurDir[ MAX_PATH ] = {0};

	// Have to set the current working directory, so all
	// of the DLL's can find each other.
	GetCurrentDirectory( MAX_PATH, szCurDir );
	SetCurrentDirectory( szNotesPath );

	try
	{
		// Ensure that path+"\nlnvp.dll" is less than MAX_PATH characters
		if( ((_tcslen(szNotesPath)+12) > MAX_PATH) )
		{
			MSILogMessage( hInstall, "Path too long" );
			throw( FALSE );
		}
		vpsnprintf( szFilename, sizeof (szFilename), "%s\\nLNVP.dll", szNotesPath );

		hDLL = LoadLibrary( szFilename );
		if( hDLL == NULL )
		{
			MSILogMessage( hInstall, _T("Unable to load nLNVP.dll") );
			throw( FALSE );
		}

		LPFNNSEINSTALL lpfnNSEInstall = (LPFNNSEINSTALL)GetProcAddress( hDLL, "NSE_Install" );
		if( lpfnNSEInstall == NULL )
		{
			MSILogMessage( hInstall, _T("Unable to GetProcAddress on NSE_Install") );
			throw( FALSE );
		}

		DWORD dwRet = lpfnNSEInstall( TRUE );
		if( lpfnNSEInstall == NULL )
		{
			TCHAR szErr[MAX_PATH] = {0};
			vpsnprintf( szErr, sizeof (szErr), "NSE_Install failed: dwRet = %d", dwRet );
			MSILogMessage( hInstall, szErr );
			throw( FALSE );
		}

	}
	catch( BOOL bErr )
	{
		bRet = bErr;
	}

	SetCurrentDirectory( szCurDir );

	return bRet;
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
//////////////////////////////////////////////////////////////////////////
void MSILogMessage( MSIHANDLE hInstall, TCHAR szString[] )
{
	UINT	uiRetVal = ERROR_SUCCESS;

	MSIHANDLE hRec = MsiCreateRecord( 1 );
	if( hRec )
	{
	    TCHAR	szLogLine[MAX_PATH];

		vpstrncpy    (szLogLine, "SAVNOTES: ", sizeof (szLogLine));
		vpstrnappend (szLogLine, szString,     sizeof (szLogLine));

		MsiRecordClearData( hRec );
		MsiRecordSetString( hRec, 0, szLogLine );
		MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
		MsiCloseHandle( hRec );
	}
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
// 8/25/03 - DKOWALYSHYN changed.
// 2/20/04 - SKENNED Ported to savNotesCA
//////////////////////////////////////////////////////////////////////////
DWORD GetMSIProperty( MSIHANDLE hInstall, TCHAR szValue[], const TCHAR szProperty[] )
{
	TCHAR szErrorString[MAX_PATH]={0};

	char *pCAData = NULL;
	DWORD dwLen = 0;	// This needs to be 0 so that we get ERROR_MORE_DATA error
	UINT nRet = ERROR_SUCCESS;
	UINT nErr = ERROR_SUCCESS;

	try
	{
		nErr = MsiGetProperty( hInstall, szProperty, "", &dwLen );
		if( ERROR_MORE_DATA != nErr )
		{
			vpsnprintf(szErrorString, sizeof (szErrorString), "Error getting length of %s (nErr = %d)", szProperty, nErr);
			throw( szErrorString );
		}

		// Allocate our memory and grab the data, add one for the trailing null
		pCAData = new char[ ++dwLen * sizeof( TCHAR ) ];
		if( !pCAData )
		{
			vpsnprintf(szErrorString, sizeof (szErrorString), "Unable to allocate memory for data buffer %s", szErrorString);
			throw( szErrorString );
		}

		nErr = MsiGetProperty( hInstall, szProperty, pCAData, &dwLen );
		if( ERROR_SUCCESS != nErr )
		{
			vpsnprintf( szErrorString, sizeof (szErrorString), "Unable to allocate memory for data buffer %s (nErr = %d)", szProperty, nErr );
			throw( szErrorString );
		}

		// Log the value to the MSI log file
		vpsnprintf( szErrorString, sizeof (szErrorString), "%s=%s", szProperty, pCAData );
		MSILogMessage( hInstall, szErrorString );
		_tcscpy( szValue, pCAData );
	}
	catch( TCHAR *szErr )
	{
		nRet = ERROR_FUNCTION_FAILED;
		MSILogMessage( hInstall, szErr);
	}


	if( pCAData )
		delete [] pCAData;

	return nRet;
}
