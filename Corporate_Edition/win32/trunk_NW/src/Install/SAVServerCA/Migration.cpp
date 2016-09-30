// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Migration.cpp : Migration routines for SAV Server

#include "stdafx.h"
#include <tchar.h>
#include <string>
#include "Migration.h"
#include "vpcommon.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

using namespace std;

//---------------------------------------------------------------------------------
//	Routine:	CheckForPrimaryServer
//	Purpose:	Check to see if this is a migration over a primary server and if so
//				set the PRIMARYSERVER property
//	Parameters:	MSIHANDLE hInstall, Handle to the current install
//  Returns:	ERROR_SUCCESS if successfull
//	Notes:		This custom action should be run in immediate mode before
//				RemoveExistingProducts (Preferably keyed off AppSearch)
//---------------------------------------------------------------------------------
// 10/19/04 Sean Kennedy - Function created
// --------------------------------------------------------------------------------
UINT __stdcall CheckForPrimaryServer( MSIHANDLE hInstall )
{
	UINT ret = ERROR_SUCCESS;
	tstring baseName = _T("Software\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\DomainData");

	try
	{
		HKEY key = NULL;
	
		// Check for DomainData key which indicates a Primary Server
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, baseName.c_str(), 0, KEY_READ, &key ) )
			throw( (tstring)"Unable to open DomainData key" );
	
		MsiSetProperty( hInstall, "PRIMARYSERVER", "1");

		RegCloseKey( key );
	}
	catch( tstring err )
	{
		MsiLogMessage( hInstall, (TCHAR *)err.c_str() );
	}

	return ret;
}

//---------------------------------------------------------------------------------
//	Routine:	SetAllowUDPProtocol
//	Purpose:	Check to see if this is a migration of a server with clients and 
//				if so set the ALLOWUDPPROTOCOL property to 1
//	Parameters:	MSIHANDLE hInstall, Handle to the current install
//  Returns:	ERROR_SUCCESS always
//	Notes:		This custom action should be run in immediate mode
//---------------------------------------------------------------------------------
// 10/19/04 Sean Kennedy - Function created
// --------------------------------------------------------------------------------
UINT __stdcall SetAllowUDPProtocol( MSIHANDLE hInstall )
{
	HKEY key = NULL;
	tstring basePath( "Software\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\" );
	tstring allow = "0";

	// Check if we have any secondary servers and if so set the AllowUDPProtocol to 1
	tstring childPath( basePath );
	childPath.append( "Children" );
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, childPath.c_str(), 0, KEY_READ, &key ) )
	{
		MsiLogMessage( hInstall, _T("Unable to access Children key") );
	}
	else
	{
		DWORD index = 0;
		DWORD valSize = MAX_PATH;
		TCHAR childName[MAX_PATH] = {0};
		while( ERROR_SUCCESS == RegEnumValue( key, index, childName, &valSize, NULL, NULL, NULL, NULL ) )
		{
			++index;
			valSize = MAX_PATH;
			// Make sure we are not looking at the default value here
			if( strlen( childName ) > 0 )
			{
				MsiLogMessage( hInstall, childName );
				allow = "1";
				break;
			}
		}
		RegCloseKey( key );
	}

	// Check if we have any clients and if so set the AllowUDPProtocol to 1
	if( "0" == allow )
	{
		childPath = basePath;
		childPath.append( "Clients" );
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, childPath.c_str(), 0, KEY_READ, &key ) )
		{
			MsiLogMessage( hInstall, _T("Unable to access Clients key") );
		}
		else
		{
			FILETIME fTime;
			DWORD index = 0;
			DWORD valSize = MAX_PATH;
			TCHAR keyName[MAX_PATH] = {0};
			while( ERROR_SUCCESS == RegEnumKeyEx( key, index, keyName, &valSize, NULL, NULL, NULL, &fTime ) )
			{
				MsiLogMessage( hInstall, keyName );
				++index;
				valSize = MAX_PATH;
				allow = "1";
				break;
			}
			RegCloseKey( key );
		}
	}

	MsiSetProperty( hInstall, "ALLOWUDPPROTOCOL.2D6B2C77_9DB3_4019_A3E4_3F2892186836", allow.c_str() );
	tstring msg = _T("ALLOWUDPPROTOCOL.2D6B2C77_9DB3_4019_A3E4_3F2892186836 = ") + allow;
	MsiLogMessage( hInstall, (TCHAR *)msg.c_str() );

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SetIsServerSecure()
//
// Description: Look at the addresscache reg, load the flag2 key
//	then bitwise-and "&" it against the PF2_SECURE_COMM_ENABLED flag to 
//	determine if the target server supports secure SSL communications and
//	return BOOL TRUE or FALSE.
//
//////////////////////////////////////////////////////////////////////////
// 9/9/04 - Daniel Kowalyshyn function created.
// 12/15/04 - Sean Kennedy - Adapted for generic use in SAV Server MM
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SetIsServerSecure( MSIHANDLE hInstall )
{

	TCHAR szTemp [MAX_LOG_PATH];
	TCHAR szLog [MAX_LOG_PATH];
	HKEY  hKey = NULL;
	DWORD dwProductVersionFlag=0;
	BOOL bScsSecureCommsEnabled=FALSE;
	DWORD size = MAX_COMPUTERNAME_LENGTH + 1;		// size of computer or user name 
	TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];

	GetComputerName( computerName, &size );
	sssnprintf (szTemp, 
				sizeof(szTemp), 
				"%s\\%s\\%s", 
				_T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion"), 
				_T("AddressCache"), 
				computerName );

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szTemp), 0, KEY_QUERY_VALUE, &hKey))
	{
		DWORD   dwSize = sizeof( dwProductVersionFlag );
		if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey,
										_T("Flags2"), 
										NULL, 
										NULL, 
										(LPBYTE)&dwProductVersionFlag, 
										&dwSize ))
		{
			sssnprintf (szLog, sizeof(szLog), "VersionFlag %d", dwProductVersionFlag);
			MsiLogMessage( hInstall, szLog );
		}
		else
		{
			MsiLogMessage( hInstall, "Unable to get flags2 value from registry" );
		}
		RegCloseKey(hKey);
	}

	if (dwProductVersionFlag & PF2_SECURE_COMM_ENABLED)
	{
		bScsSecureCommsEnabled=TRUE;
		sssnprintf (szLog, sizeof(szLog), "%s is Secure", computerName);
		MsiLogMessage( hInstall, szLog );
		MsiSetProperty( hInstall, "PRESECUREFOUND", NULL );
	}
	else
	{
		bScsSecureCommsEnabled=FALSE;
		sssnprintf (szLog, sizeof(szLog), "%s is NOT Secure", computerName );
		MsiLogMessage( hInstall, szLog );
		MsiSetProperty( hInstall, "PRESECUREFOUND", "1" );
	}

	return ERROR_SUCCESS;
}

//---------------------------------------------------------------------------------
//	Routine:	CopyServerCerts
//	Purpose:	In the case of a SAV Server to SCS Server migration we need to copy
//				the certificates into the new PKI folder
//	Parameters:	MSIHANDLE hInstall, Handle to the current install
//  Returns:	ERROR_SUCCESS
//---------------------------------------------------------------------------------
// 10/19/04 Sean Kennedy - Function created
// 06/22/05 Jim Shock - Defect 1-44OH1B - copying of certs is required in several
//	other sceenarios - upgrading a direct install (SAV or SCS) with a deploy (SAV OR 
//	SCS) will change the install directory. enabling this for any upgrade of a 
//	secure SAV server. Also - if the source and target are the same, exit out
// --------------------------------------------------------------------------------
UINT __stdcall CopyServerCerts( MSIHANDLE hInstall )
{
#ifdef DEBUG
	MessageBox( NULL, "Debug", "Attach to me", MB_OK );
#endif

	try
	{
		// Get the custom action data
		TCHAR temp[ MAX_PATH ] = {0};
		_tcscpy( temp, _T("CustomActionData") );
		if( ERROR_SUCCESS != GetMSIProperty( hInstall, temp ) )
			throw( tstring( "Unable to get CustomActionData" ) );

		// Get the old and new base directories
		tstring data( temp );
		unsigned int delimiter = data.find( ";", 0 );
		if( delimiter == tstring::npos )
			throw( tstring( "Unable to parse CustomActionData" ) );

		tstring oldpki = data.substr( 0, delimiter );
		tstring newpki = data.substr( delimiter + 1 ); // +1 to skip the delimiter

		// check for matching source / target dirs - if upgrading using the same method,
		// the dirs are the same - settings are still migrated - but no cert copy is needed
		if (oldpki == newpki)
		{
			MsiLogMessage( hInstall, _T("CopyServerCerts - skip matching paths") );
			return ERROR_SUCCESS;
		}

		tstring oldclinst( oldpki );
		tstring newclinst( newpki );
		oldpki.append( "pki" );
		newpki.append( "pki" );
		oldclinst.append( "clt-inst" );
		newclinst.append( "clt-inst" );

		// Copy the certs
		if( 0 == CopyDirRecursive( hInstall, const_cast<TCHAR *>(oldpki.c_str()), const_cast<TCHAR *>(newpki.c_str()) ) )
			MsiLogMessage( hInstall, _T("No pki files copied") );
		if( 0 == CopyDirRecursive( hInstall, const_cast<TCHAR *>(oldclinst.c_str()), const_cast<TCHAR *>(newclinst.c_str()) ) )
			MsiLogMessage( hInstall, _T("No clt-inst files copied") );

		// Delete the old certs
		if( FALSE == DeleteDir( hInstall, const_cast<TCHAR *>(oldpki.c_str()), "*.*" ) )
			MsiLogMessage( hInstall, _T("Error deleting pki folders") );
		if( FALSE == DeleteDir( hInstall, const_cast<TCHAR *>(oldclinst.c_str()), "*.*" ) )
			MsiLogMessage( hInstall, _T("Error deleting clt-inst folders") );
	}
	catch( tstring& err )
	{
		MsiLogMessage( hInstall, const_cast<TCHAR *>(err.c_str()) );
	}

	return ERROR_SUCCESS;
}
