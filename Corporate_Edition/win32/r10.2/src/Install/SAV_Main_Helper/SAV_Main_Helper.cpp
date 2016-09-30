// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SAV_Main_Helper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HANDLE /*hModule*/, DWORD /*dwReason*/, LPVOID /*lpReserved*/ )
{
    return TRUE;
}

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
//  Function: MsiLogMessage
//
//  Yet another copy of the code to write messages to the MSI log file.
//
//  08/16/2003 Thomas Brock
//
///////////////////////////////////////////////////////////////////////////////

UINT __fastcall MsiLogMessage( MSIHANDLE hInstall, char szString[] )
{
	UINT uiRetVal = ERROR_INSTALL_FAILURE;

	MSIHANDLE hRec = MsiCreateRecord( 1 );
	if( hRec )
	{
		char szLogLine[MAX_PATH];

		strncpy( szLogLine, "MSIInstall - SAVCustom.dll: ", MAX_PATH );
		strncat( szLogLine, szString, MAX_PATH - strlen( szLogLine ) );
		strncat( szLogLine, "\r\n", MAX_PATH - strlen( szLogLine ) );

		MsiRecordClearData( hRec );
		MsiRecordSetString( hRec, 0, szLogLine );
		MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
		MsiCloseHandle( hRec );

		OutputDebugString( szString );

		uiRetVal = ERROR_SUCCESS;
	}

	return uiRetVal;
}
///////////////////////////////////////////////////////////////////////////////
//
//  Function: MsiWriteDec3CfgFile
//
//  Right now, this is simply going to write the SAV_Main merge module's
//  INSTALLDIR value, and then write the rest of the file from a hard
//  coded string in this file.  If we ever get to the point of "featurizing"
//  the decomposers in the install, we will need much more complex logic to
//  write a file that matches what we will actually install.
//
//  08/16/2003 Thomas Brock
//
///////////////////////////////////////////////////////////////////////////////

char g_szFile[] = "\r\n" \
"1000000\r\n" \
"16384\r\n" \
"500000\r\n" \
"14\r\n" \
"Dec2ID.dll\r\n" \
"10\r\n" \
"Dec2ZIP.dll\r\n" \
"24\r\n" \
"Dec2SS.dll\r\n" \
"18\r\n" \
"Dec2GZIP.dll\r\n" \
"7\r\n" \
"Dec2CAB.dll\r\n" \
"4\r\n" \
"Dec2LHA.dll\r\n" \
"12\r\n" \
"Dec2EXE.dll\r\n" \
"5\r\n" \
"Dec2ARJ.dll\r\n" \
"3\r\n" \
"Dec2TNEF.dll\r\n" \
"22\r\n" \
"Dec2LZ.dll\r\n" \
"14\r\n" \
"Dec2AMG.dll\r\n" \
"1\r\n" \
"Dec2TAR.dll\r\n" \
"21\r\n" \
"Dec2RTF.dll\r\n" \
"20\r\n" \
"Dec2Text.dll\r\n" \
"33\r\n" \
"\r\n" \
"[Options]\r\n" \
"EnableMIMEEngine=1\r\n" \
"EnableUUEEngine=1\r\n" \
"EnableBinHexEngine=1\r\n" \
"EnableMBOXEngine=1\r\n" \
"MIMEFuzzyMainHeader=0\r\n" \
"MaxTextScanBytes=8192\r\n" \
"EnhancedTextID=0\r\n" ;

UINT __stdcall MsiWriteDec3CfgFile( MSIHANDLE hInstall )
{
	DWORD dwBytes;
	DWORD dwLen = MAX_PATH;
	char szInstallDir[MAX_PATH];
	char szDec3FileName[MAX_PATH];
	UINT uiRetVal = ERROR_INSTALL_FAILURE;
	HANDLE hFile;

	if( ERROR_SUCCESS == MsiGetProperty( hInstall, "CustomActionData", szInstallDir, &dwLen ) )
	{
		AddSlash( szInstallDir );
		strncpy( szDec3FileName, szInstallDir, MAX_PATH );
		strncat( szDec3FileName, "Dec3.cfg", MAX_PATH - strlen( szDec3FileName ) );
		hFile = CreateFile( szDec3FileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			if( WriteFile( hFile, szInstallDir, strlen( szInstallDir ), &dwBytes, NULL ) )
			{
				if( WriteFile( hFile, g_szFile, strlen( g_szFile ), &dwBytes, NULL ) )
				{
					 CloseHandle( hFile );
					 uiRetVal = ERROR_SUCCESS;
				}
				else
				{
					MsiLogMessage( hInstall, "Could not write to file:" );
					MsiLogMessage( hInstall, szDec3FileName );
				}
			}
			else
			{
				MsiLogMessage( hInstall, "Could not write to file:" );
				MsiLogMessage( hInstall, szDec3FileName );
			}
		}
		else
		{
			MsiLogMessage( hInstall, "Could not create file:" );
			MsiLogMessage( hInstall, szDec3FileName );
		}
	}
	else
	{
		MsiLogMessage( hInstall, "Could not obtain CustomActionData for MsiWriteDec3CfgFile" );
	}

	return uiRetVal;
}
