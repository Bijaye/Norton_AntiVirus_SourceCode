// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "Migration.h"
#include "savMainCA.h"
#include <CREGENT.H>
#include "navver.h"
#include "SymSaferRegistry.h"
#include "vpcommon.h"
#include "clientreg.h"

//////////////////////////////////////////////////////////////////////////
//
// Function: SevInstUninstallWorkAround()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:
// The function below is a workaround for the bug in sevinst.exe 11.0, already deployed in the field.
// Sevinst will ignore our reference counting and mark "SYMEVENT.SYS" for deletion anyways, and
// SAVRT will generate "Auto-Protect failed to unload" errors after subsequent reboots because
// the driver will not be found.
// Our work-around is to check for the key that will delete symevent and remove it if present.
// This code should be taken out whenever we sunset migration of SCS 1.0 (SAV 7.5.0 and SAV 7.51).
// This code should only be called on the condition that our product is not installed.
//////////////////////////////////////////////////////////////////////////
// 11/21/03 - JMEADE function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SevInstUninstallWorkAround( MSIHANDLE hInstall )
{

	UINT nRet = ERROR_SUCCESS;

	try
	{
		HKEY hSEKey = NULL;
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\SymEvent"), NULL, KEY_ALL_ACCESS, &hSEKey ) )
			throw( _T("Unable to open SymEvent services key") );

		// Don't throw an exception if the following fails because we still want to close the key.
		//
		if( ERROR_SUCCESS != RegDeleteValue( hSEKey, _T("DeleteFlag") ) )
			MSILogMessage( hInstall, _T("Unable to delete 'DeleteFlag' value for SymEvent services key") );

		if( hSEKey )
			RegCloseKey( hSEKey );
	}
	catch( TCHAR *szErr )
	{
		// Don't change the return value from ERROR_SUCCESS or we'll rollback and have nothing installed.
		// There is a Knowledge Base article describing how to fix the problem.
		//
		MSILogMessage( hInstall, szErr );
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: DisableUninstallPassword()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function will attempt to set the UseUninstallPassword value to 0
//  so during a migration so the user will not be prompted for a password.
//	This function can only succeed if the logged in user is an administrator
//
//////////////////////////////////////////////////////////////////////////
// 2/1/04  - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall DisableUninstallPassword( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	HKEY hKey = NULL;

	try
	{
		// Open the key
		if( ERROR_SUCCESS != RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
											_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AdministratorOnly\\Security"),
											0,
											KEY_WRITE,
											&hKey ) )
			throw( _T("Unable to open source tree for write access") );

		DWORD dwValue = 0;
		if( ERROR_SUCCESS != RegSetValueEx( hKey, _T("UseVPUninstallPassword"), 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD) ) )
			throw( _T("Unable to set UseVPUninstallPassword to 0") );

	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	if( hKey )
		RegCloseKey( hKey );

	return nRet;
}

// JHoa - Forward Port - Defect 1-47X9JD Fix
void SaveServerTuningKeys(MSIHANDLE hInstall, HKEY hKeySrc, HKEY hKeyDest, TCHAR *szMsgAction)
{
	TCHAR szMsg[1024];
	DWORD dwType = REG_DWORD;
	DWORD dwData = 0;
	DWORD dwDataSize = sizeof(DWORD);
	if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKeySrc, szReg_Val_AllowUdpProtocol, NULL, NULL, (LPBYTE)&dwData, &dwDataSize ) )
	{
		if (RegSetValueEx( hKeyDest, szReg_Val_AllowUdpProtocol, NULL, dwType, (CONST BYTE *)&dwData, dwDataSize ) == ERROR_SUCCESS)
		{
			sssnprintf(szMsg, sizeof(szMsg), _T("AllowUdpProtocol value %s successfully"), szMsgAction);
			MSILogMessage( hInstall, szMsg);
		}
	}

	dwData = 0;
	dwDataSize = sizeof(DWORD);
	if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKeySrc, szReg_Val_VerifyClientBeforeRollout, NULL, NULL, (LPBYTE)&dwData, &dwDataSize ) )
	{
		if (RegSetValueEx( hKeyDest, szReg_Val_VerifyClientBeforeRollout, NULL, dwType, (CONST BYTE *)&dwData, dwDataSize ) == ERROR_SUCCESS)
		{
			sssnprintf(szMsg, sizeof(szMsg), _T("VerifyClientBeforeRollout value %s successfully"), szMsgAction);
			MSILogMessage( hInstall, szMsg);
		}
	}

	dwData = 0;
	dwDataSize = sizeof(DWORD);
	if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKeySrc, szReg_Val_IgnoreLateClients, NULL, NULL, (LPBYTE)&dwData, &dwDataSize ) )
	{
		if (RegSetValueEx( hKeyDest, szReg_Val_IgnoreLateClients, NULL, dwType, (CONST BYTE *)&dwData, dwDataSize ) == ERROR_SUCCESS)
		{
			sssnprintf(szMsg, sizeof(szMsg), _T("IgnoreLateClients value %s successfully"), szMsgAction);
			MSILogMessage( hInstall, szMsg);
		}
	}

	dwData = 0;
	dwDataSize = sizeof(DWORD);
	if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKeySrc, szReg_Val_HonorClientGoodFlag, NULL, NULL, (LPBYTE)&dwData, &dwDataSize ) )
	{
		if (RegSetValueEx( hKeyDest, szReg_Val_HonorClientGoodFlag, NULL, dwType, (CONST BYTE *)&dwData, dwDataSize ) == ERROR_SUCCESS)
		{
			sssnprintf(szMsg, sizeof(szMsg), _T("HonorClientGoodFlag value %s successfully"), szMsgAction);
			MSILogMessage( hInstall, szMsg);
		}
	}

	dwData = 0;
	dwDataSize = sizeof(DWORD);
	if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKeySrc, szReg_Val_ClientUpdateThreadPool, NULL, NULL, (LPBYTE)&dwData, &dwDataSize ) )
	{
		if (RegSetValueEx( hKeyDest, szReg_Val_ClientUpdateThreadPool, NULL, dwType, (CONST BYTE *)&dwData, dwDataSize ) == ERROR_SUCCESS)
		{
			sssnprintf(szMsg, sizeof(szMsg), _T("ClientUpdateThreadPool value %s successfully"), szMsgAction);
			MSILogMessage( hInstall, szMsg);
		}
	}

	dwData = 0;
	dwDataSize = sizeof(DWORD);
	if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKeySrc, szReg_Val_ClientPushUpdateRate, NULL, NULL, (LPBYTE)&dwData, &dwDataSize ) )
	{
		if (RegSetValueEx( hKeyDest, szReg_Val_ClientPushUpdateRate, NULL, dwType, (CONST BYTE *)&dwData, dwDataSize ) == ERROR_SUCCESS)
		{
			sssnprintf(szMsg, sizeof(szMsg), _T("ClientPushUpdateRate value %s successfully"), szMsgAction);
			MSILogMessage( hInstall, szMsg);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SaveSAVSettings()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function backs up the current settings before migration takes place.
//  Needs to come before RemoveExistingProducts standard action.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03  - SKENNED function created.
// 11/11/03 - JMEADE  function modified to fix 1-1YTCI4.
// 07/05/05 - JHOA function modified to forward port 1-47X9AN and 1-47X9JD
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SaveSAVSettings( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	HKEY hSource = NULL;
	HKEY hDest   = NULL;
	char ParentComputerName[NAME_SIZE] = {0};
	DWORD dwParentComputerName = NAME_SIZE;

	try
	{
		hSource = OpenSavTreeHKLM( KEY_READ );
		if( !(hSource) )
			throw( _T("Unable to open source tree for reading current HKLM settings") );

		hDest = OpenMigrateTreeHKLM( KEY_WRITE );
		if( !(hDest) )
			throw( _T("Unable to create destination tree for saving current HKLM settings") );

		CopyRegKeys( hSource, hDest );

		// JHoa - Forward Port - 1-47X9AN: need to migrate "Parent" registry value
		CRegistryEntry regSource( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("Parent") );
		CRegistryEntry regDest( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("Parent") );
		if ( ERROR_SUCCESS == regSource.GetStringValue(ParentComputerName, &dwParentComputerName) )
		{
			if (ERROR_SUCCESS == regDest.SetValue(REG_SZ, (LPBYTE)ParentComputerName, dwParentComputerName) )
				MSILogMessage( hInstall, _T("SAV Parent value saved successfully") );
		}

		// JHoa - Forward Port - 1-47X9JD: Server Tuning keys need to preserved and restored
		HKEY hKeySrc=NULL, hKeyDest=NULL;
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), NULL, KEY_READ, &hKeySrc ) )
		{	
			if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyDest, NULL ) )
			{
				SaveServerTuningKeys(hInstall, hKeySrc, hKeyDest, _T("saved"));
				RegCloseKey(hKeyDest);
			}

			RegCloseKey(hKeySrc);
		}

		// DKowalyshyn - Forward Port - 1-4YH8Q7: need to migrate "LogFileRollOverDays" & "LogFrequency" registry value
		DWORD dwLogReg, dwType, dwSz = sizeof(DWORD);
		CRegistryEntry regSource2( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("LogFileRollOverDays") );
		CRegistryEntry regDest2( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("LogFileRollOverDays") );
		if ( ERROR_SUCCESS == regSource2.GetValue(&dwType, (LPBYTE)&dwLogReg, &dwSz) )
		{
			if (ERROR_SUCCESS == regDest2.SetValue(REG_DWORD, (LPBYTE)&dwLogReg, dwSz) )
				MSILogMessage( hInstall, _T("LogFileRollOverDays value saved successfully") );
		}
		CRegistryEntry regSource3( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("LogFrequency") );
		CRegistryEntry regDest3( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("LogFrequency") );
		if ( ERROR_SUCCESS == regSource3.GetValue(&dwType, (LPBYTE)&dwLogReg, &dwSz) )
		{
			if (ERROR_SUCCESS == regDest3.SetValue(REG_DWORD, (LPBYTE)&dwLogReg, dwSz) )
				MSILogMessage( hInstall, _T("LogFileRollOverDays value saved successfully") );
		}

		// Migrate "Connected" registry value
		CRegistryEntry regSource4( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("Connected") );
		CRegistryEntry regDest4( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("Connected") );
		if ( ERROR_SUCCESS == regSource4.GetValue(&dwType, (LPBYTE)&dwLogReg, &dwSz) )
		{
			if (ERROR_SUCCESS == regDest4.SetValue(REG_DWORD, (LPBYTE)&dwLogReg, dwSz) )
			{
				MSILogMessage( hInstall, _T("Connected value saved successfully") );
			}
			else
			{
				MSILogMessage( hInstall, _T("Connected value not saved successfully") );
			}
		}
		else
		{
				MSILogMessage( hInstall, _T("Connected value not found") );
		}

		// Migrate "ClientType" registry value
		CRegistryEntry regSource5( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("ClientType") );
		CRegistryEntry regDest5( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("ClientType") );
		if ( ERROR_SUCCESS == regSource5.GetValue(&dwType, (LPBYTE)&dwLogReg, &dwSz) )
		{
			if (ERROR_SUCCESS == regDest5.SetValue(REG_DWORD, (LPBYTE)&dwLogReg, dwSz) )
			{
				MSILogMessage( hInstall, _T("ClientType value saved successfully") );
			}
			else
			{
				MSILogMessage( hInstall, _T("ClientType value not saved successfully") );
			}
		}
		else
		{
				MSILogMessage( hInstall, _T("ClientType value not found") );
		}

		MSILogMessage( hInstall, _T("SAV HKLM settings saved successfully") );
		// Save the custom scans and scheduled scans for all users.
		//
		SaveUserSettings( hInstall );
	}
	catch( TCHAR *szErr )
	{
		// Kill migration branch here somewhere
		MSILogMessage( hInstall, szErr );
	}

	if( hSource )
		RegCloseKey( hSource );

	if( hDest )
		RegCloseKey( hDest );

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SaveUserSettings()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function backs up all SAV per-user settings before migration takes place.
//  Needs to come before RemoveExistingProducts standard action.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/03 - JMEADE function created to fix 1-1YTCI4.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SaveUserSettings( MSIHANDLE hInstall )
{

	UINT nRet = ERROR_SUCCESS;
	HKEY hSource = NULL;
	HKEY hDest   = NULL;
	TCHAR szKeyname[ MAX_PATH ] = {0};
	DWORD dwSize = sizeof( szKeyname );
	DWORD dwIndex = 0;

	try
	{
		hSource = OpenSavTreeHKU( KEY_READ );
		if( !(hSource) )
			throw( _T("Unable to open source tree for reading current HKU settings") );

		hDest = OpenMigrateTreeHKU( KEY_WRITE );
		if( !(hDest) )
			throw( _T("Unable to create destination tree for saving current HKU settings") );

		// For everything directly under HKEY_USERS ...
		//
		LONG lRet = RegEnumKeyEx( hSource, dwIndex, szKeyname, &dwSize, NULL, NULL, NULL, NULL );
		while( ERROR_SUCCESS == lRet )
		{
			++dwIndex;

			MSILogMessage( hInstall, _T("Checking key under HKU:") );
			MSILogMessage( hInstall, _T( szKeyname ) );

			// If the source key has the expected "CurrentVersion" subkey ...
			//
			HKEY hRSubkey = NULL;
			CString strFromKey;
			strFromKey.Format( "%s\\Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion", szKeyname );
			{
				char *pszData = strFromKey.GetBuffer( MAX_PATH );
				MSILogMessage( hInstall, "Here is strFromKey:" );
				MSILogMessage( hInstall, pszData );
				strFromKey.ReleaseBuffer();
			}

			if( ERROR_SUCCESS == RegOpenKeyEx( hSource, strFromKey, NULL, KEY_READ, &hRSubkey ) )
			{

				MSILogMessage( hInstall, _T("Found CurrentVersion key under HKU") );
				MSILogMessage( hInstall, _T( szKeyname ) );

				// Create the destination key.
				//
				HKEY hWSubkey = NULL;
				CString strToKey;
				strToKey.Format( "%s\\Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion", szKeyname );
				{
					char *pszData = strToKey.GetBuffer( MAX_PATH );
					MSILogMessage( hInstall, "Here is strToKey (to be created under destination):" );
					MSILogMessage( hInstall, pszData );
					strToKey.ReleaseBuffer();
				}
				if( ERROR_SUCCESS == RegCreateKeyEx( hDest, strToKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hWSubkey, NULL ) )
				{
					MSILogMessage( hInstall, _T("Created destination key under HKU") );

					// Copy all keys and values recursively.
					//
					CopyKeys( hRSubkey, hWSubkey, _T("Custom Tasks") );
					CopyKeys( hRSubkey, hWSubkey, _T("Filters") );

					CopyValues( hRSubkey, hWSubkey );

					RegCloseKey( hWSubkey );
				}
				else
				{
					MSILogMessage( hInstall, _T( "Failed to create destination key under HKU" ) );
				}
				
				RegCloseKey( hRSubkey );
			}
			else
			{
				MSILogMessage( hInstall, _T("Failed to find CurrentVersion key under HKU:") );
				MSILogMessage( hInstall, _T( szKeyname ) );
			}

			// Get the next key.
			//
			dwSize = sizeof( szKeyname );
			lRet = RegEnumKeyEx( hSource, dwIndex, szKeyname, &dwSize, NULL, NULL, NULL, NULL );

		}

		MSILogMessage( hInstall, _T("SAV HKU settings saved successfully") );

	}
	catch( TCHAR *szError )
	{
		OutputDebugString( szError );
	}

	if( hSource )
		RegCloseKey( hSource );

	if( hDest )
		RegCloseKey( hDest );

	return nRet;
}



//////////////////////////////////////////////////////////////////////////
//
// Function: RestoreSAVSettings()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function restores the settings from the migration.
//  Needs to be run with elevated privileges.  This should be called
//	before the StartServices standard action
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03  - SKENNED function created.
// 11/11/03 - JMEADE  function modified to fix 1-1YTCI4.
// 1/7/04 - SKENNED function updated to fix 1-21UIKB (Remove backup keys)
// 07/05/05 - JHOA function modified to forward port 1-47X9AN and 1-47X9JD
//////////////////////////////////////////////////////////////////////////
UINT __stdcall RestoreSAVSettings( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	HKEY hSource = NULL;
	HKEY hDest   = NULL;
	char		ParentComputerName[NAME_SIZE] = {0};
	DWORD		dwParentComputerName = NAME_SIZE;

	try
	{
		hSource = OpenMigrateTreeHKLM( KEY_READ );
		if( !(hSource) )
			throw( _T("Unable to open source tree for reading saved HKLM settings") );

		hDest = OpenSavTreeHKLM( KEY_WRITE );
		if( !(hDest) )
			throw( _T("Unable to open destination tree for writing saved HKLM settings") );

		CopyRegKeys( hSource, hDest );
		MSILogMessage( hInstall, _T("SAV HKLM settings restored successfully") );

		// JHoa - Forward Port - 1-47X9AN: need to restore "Parent" registry value
		CRegistryEntry regDest( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("Parent") );
		CRegistryEntry regSource( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("Parent") );
		if ( ERROR_SUCCESS == regSource.GetStringValue(ParentComputerName, &dwParentComputerName) )
		{
			if ( ERROR_SUCCESS == regDest.SetValue(REG_SZ, (LPBYTE)ParentComputerName, dwParentComputerName) )
				MSILogMessage( hInstall, _T("SAV Parent value restored successfully") );
		}
		
		// JHoa - Forward Port - 1-47X9JD: Server Tuning keys need to preserved and restored
		HKEY hKeySrc=NULL, hKeyDest=NULL;
		if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), NULL, KEY_READ, &hKeySrc ) )
		{	
			if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), NULL, KEY_WRITE, &hKeyDest ) )
			{
				SaveServerTuningKeys(hInstall, hKeySrc, hKeyDest, _T("restored"));
				RegCloseKey(hKeyDest);
			}

			RegCloseKey(hKeySrc);
		}

		RestoreUserSettings( hInstall );

		// DKowalyshyn - Forward Port - 1-4YH8Q7: need to migrate "LogFileRollOverDays" & "LogFrequency" registry value
		DWORD dwLogReg, dwType, dwSz = sizeof(DWORD);
		CRegistryEntry regDest2( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("LogFileRollOverDays") );
		CRegistryEntry regSource2( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("LogFileRollOverDays") );
		if ( ERROR_SUCCESS == regSource2.GetValue(&dwType, (LPBYTE)&dwLogReg, &dwSz) )
		{
			if (ERROR_SUCCESS == regDest2.SetValue(REG_DWORD, (LPBYTE)&dwLogReg, dwSz) )
				MSILogMessage( hInstall, _T("LogFileRollOverDays value saved successfully") );
		}
		CRegistryEntry regDest3( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("LogFrequency") );
		CRegistryEntry regSource3( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("LogFrequency") );
		if ( ERROR_SUCCESS == regSource3.GetValue(&dwType, (LPBYTE)&dwLogReg, &dwSz) )
		{
			if (ERROR_SUCCESS == regDest3.SetValue(REG_DWORD, (LPBYTE)&dwLogReg, dwSz) )
				MSILogMessage( hInstall, _T("LogFileRollOverDays value saved successfully") );
		}
		// Migrate "Connected" registry value
		CRegistryEntry regDest4( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("Connected") );
		CRegistryEntry regSource4( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("Connected") );
		if ( ERROR_SUCCESS == regSource4.GetValue(&dwType, (LPBYTE)&dwLogReg, &dwSz) )
		{
			if (ERROR_SUCCESS == regDest4.SetValue(REG_DWORD, (LPBYTE)&dwLogReg, dwSz) )
				MSILogMessage( hInstall, _T("Connected value restored successfully") );
		}

		// Migrate "ClientType" registry value
		CRegistryEntry regDest5( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion"), _T("ClientType") );
		CRegistryEntry regSource5( HKEY_CURRENT_USER, _T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"), _T("ClientType") );
		if ( ERROR_SUCCESS == regSource5.GetValue(&dwType, (LPBYTE)&dwLogReg, &dwSz) )
		{
			if (ERROR_SUCCESS == regDest5.SetValue(REG_DWORD, (LPBYTE)&dwLogReg, dwSz) )
				MSILogMessage( hInstall, _T("ClientType value restored successfully") );
		}
		// Remove the install backup keys
		CRegistryEntry reg( HKEY_CURRENT_USER, "Software\\Symantec\\Install", NULL );
		reg.DeleteKey( TRUE );

		// Remove the Symantec key if it is empty
		CRegistryEntry symReg( HKEY_CURRENT_USER, "Software\\Symantec", NULL );
		symReg.DeleteKey( FALSE );

		// Need to reset values for
		// HKEY_LOCAL_MACHINE\SOFTWARE\Intel\LANDesk\VirusProtect6\CurrentVersion\LocalScans\CScan Scan Options\
		//  FirstAction
		//  FirstMacroAction
		//  SecondAction
		//  SecondMacroAction
		// All of these keys should be set to 1, reguardless of previous values.  See 1-3L78Z3
		// 1-4PYCHP - It was decided to convert these all to 4 instead of 1. DJK. Approved by IBarile & KSack

		DWORD dwFour = 4;	// AC_NOTHING
		CRegistryEntry SymRegFirstAction( HKEY_LOCAL_MACHINE, "SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\LocalScans\\CScan Scan Options", "FirstAction" );
		SymRegFirstAction.SetValue( REG_DWORD, (LPBYTE) &dwFour, sizeof(DWORD) );

		CRegistryEntry SymRegFirstMacroAction( HKEY_LOCAL_MACHINE, "SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\LocalScans\\CScan Scan Options", "FirstMacroAction" );
		SymRegFirstMacroAction.SetValue( REG_DWORD, (LPBYTE) &dwFour, sizeof(DWORD) );

		CRegistryEntry SymRegSecondAction( HKEY_LOCAL_MACHINE, "SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\LocalScans\\CScan Scan Options", "SecondAction" );
		SymRegSecondAction.SetValue( REG_DWORD, (LPBYTE) &dwFour, sizeof(DWORD) );

		CRegistryEntry SymRegSecondMacroAction( HKEY_LOCAL_MACHINE, "SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\LocalScans\\CScan Scan Options", "SecondMacroAction" );
		SymRegSecondMacroAction.SetValue( REG_DWORD, (LPBYTE) &dwFour, sizeof(DWORD) );
	}
	catch( TCHAR *szErr )
	{
		// do we want to kill migration branch here?
		MSILogMessage( hInstall, szErr );
	}

	if( hSource )
		RegCloseKey( hSource );

	if( hDest )
		RegCloseKey( hDest );

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: RestoreUserSettings()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function restores the settings from the migration.
//  Needs to be run with elevated privlidges.  This should be called
//	before the StartServices standard action
//
//////////////////////////////////////////////////////////////////////////
// 11/11/03 - JMEADE function created to fix 1-1YTCI4.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall RestoreUserSettings( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	HKEY hSource = NULL;
	HKEY hDest   = NULL;
	TCHAR szKeyname[ MAX_PATH ] = {0};
	DWORD dwSize = sizeof( szKeyname );
	DWORD dwIndex = 0;

	try
	{
		hSource = OpenMigrateTreeHKU( KEY_READ );
		if( !(hSource) )
			throw( _T("Unable to open source tree for restoring HKU settings") );

		// Applications cannot write directly to HKEY_USERS.
		//
		hDest = OpenSavTreeHKU( KEY_READ );
		if( !(hDest) )
			throw( _T("Unable to create destination tree for restoring HKU settings") );

		// For everything directly under HKEY_USERS ...
		//
		LONG lRet = RegEnumKeyEx( hSource, dwIndex, szKeyname, &dwSize, NULL, NULL, NULL, NULL );
		while( ERROR_SUCCESS == lRet )
		{
			++dwIndex;

			// If the source key has the expected "CurrentVersion" subkey ...
			//
			HKEY hRSubkey = NULL;
			CString strFromKey;
			strFromKey.Format( "%s\\Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion", szKeyname );
			{
				char *pszData = strFromKey.GetBuffer( MAX_PATH );
				MSILogMessage( hInstall, "Here is strFromKey:" );
				MSILogMessage( hInstall, pszData );
				strFromKey.ReleaseBuffer();
			}
			if( ERROR_SUCCESS == RegOpenKeyEx( hSource, strFromKey, NULL, KEY_READ, &hRSubkey ) )
			{

				MSILogMessage( hInstall, _T("Found CurrentVersion key under HKU") );
				MSILogMessage( hInstall, _T( szKeyname ) );

				// Create the destination key.
				//
				HKEY hWSubkey = NULL;
				CString strToKey;
				strToKey.Format( "%s", szKeyname );
				{
					char *pszData = strToKey.GetBuffer( MAX_PATH );
					MSILogMessage( hInstall, "Here is strToKey:" );
					MSILogMessage( hInstall, pszData );
					strToKey.ReleaseBuffer();
				}
				// We cannot create subkeys directly from HKEY_USERS!
				//
				HKEY hTempSubkey = NULL;
				if( ERROR_SUCCESS == RegOpenKeyEx( hDest, strToKey, NULL, KEY_READ, &hTempSubkey ) )
				{
					if( ERROR_SUCCESS == RegCreateKeyEx( hTempSubkey, _T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hWSubkey, NULL ) )
					{
						MSILogMessage( hInstall, _T("Created destination key under HKU") );

						// Copy all keys and values recursively.
						//
						CopyKeys( hRSubkey, hWSubkey, _T("Custom Tasks") );
						CopyKeys( hRSubkey, hWSubkey, _T("Filters") );

						CopyValues( hRSubkey, hWSubkey );

						RegCloseKey( hWSubkey );
					}
					else
					{
						MSILogMessage( hInstall, _T( "Failed to restore destination key under HKU" ) );
					}

					RegCloseKey( hTempSubkey );
				}
				
				RegCloseKey( hRSubkey );
			}
			else
			{
				MSILogMessage( hInstall, _T("Key does not contain CurrentVersion") );
				MSILogMessage( hInstall, _T( szKeyname ) );
			}

			// Get the next key.
			//
			dwSize = sizeof( szKeyname );
			lRet = RegEnumKeyEx( hSource, dwIndex, szKeyname, &dwSize, NULL, NULL, NULL, NULL );

		}

		MSILogMessage( hInstall, _T("SAV HKU settings restored successfully") );

	}
	catch( TCHAR *szError )
	{
		OutputDebugString( szError );
	}

	if( hSource )
		RegCloseKey( hSource );

	if( hDest )
		RegCloseKey( hDest );

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SaveLogFiles()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function backs up the current log files
//  This function needs its CustomActionData set to [CommonAppDataDir]
//  Needs to come before RemoveExistingProducts standard action.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SaveLogFiles( MSIHANDLE hInstall )
{
	CString strSource, strDest, temp;

	try
	{
		strSource.Format( "%sLogs", GetMSIProperty( hInstall, "MigrationDir.93C43188_D2F5_461E_B42B_C3A2A318345C" ) );
		temp.Format( "Source: %s", strSource );
		MSILogMessage( hInstall, temp.GetBuffer() );
		temp.ReleaseBuffer();

		strDest.Format( "%sLogs", GetTempDir() );
		temp.Format( "Destination: %s", strDest );
		MSILogMessage( hInstall, temp.GetBuffer() );
		temp.ReleaseBuffer();

		CreateDirectory( strDest, NULL );

		if( CopyDir( hInstall, strSource, strDest ) > 0 )
			MSILogMessage( hInstall, _T("Log files saved successfully") );
	}
	catch( TCHAR *szErr )
	{
		temp.Format( "Error in SaveLogFiles: %s", szErr );
		MSILogMessage( hInstall, temp.GetBuffer() );
		temp.ReleaseBuffer();
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: RestoreLogFiles()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//	
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function restores backed up log files
//	Needs CustomActionData set to [CommonAppDataFolder]
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall RestoreLogFiles( MSIHANDLE hInstall )
{
	CString strSource, strDest;

	try
	{
		strSource.Format( "%sLogs", GetTempDir() );
		strDest.Format( "%sLogs", GetMSIProperty( hInstall, "CustomActionData" ) );

		if( CopyDir( hInstall, strSource, strDest ) > 0 )
		{
			MSILogMessage( hInstall, _T("Log files restored successfully") );
			DeleteDir( hInstall, (TCHAR*)strSource.GetBuffer(), "*.LOG" );
			strSource.ReleaseBuffer();
		}
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SaveQuarantineItems()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function backs up the current quarantine items
//  Needs to come before RemoveExistingProducts standard action.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SaveQuarantineItems( MSIHANDLE hInstall )
{
	CString strSource, strDest, temp;

	try
	{
		strSource.Format( "%sQuarantine", GetMSIProperty( hInstall, "MigrationDir.93C43188_D2F5_461E_B42B_C3A2A318345C" ) );
		temp.Format( "Source: %s", strSource );
		MSILogMessage( hInstall, temp.GetBuffer() );
		temp.ReleaseBuffer();

		strDest.Format( "%sQuarantine", GetTempDir() );
		CreateDirectory( strDest, NULL );
		temp.Format( "Destination: %s", strDest );
		MSILogMessage( hInstall, temp.GetBuffer() );
		temp.ReleaseBuffer();

		if( CopyDir( hInstall, strSource, strDest ) > 0 )
		{
			MSILogMessage( hInstall, _T("Quarantine items saved successfully") );
			// We've successfully backed up the files so lets delete the originals
			// This will prevent the popup asking if we wish to delete them during
			// the migration.
			if( DeleteDir( hInstall, (TCHAR*)strSource.GetBuffer(), "*.*" ) > 0 )
			{
				MSILogMessage( hInstall, _T("Quarantine items deleted successfully") );
			}
			strSource.ReleaseBuffer();
		}
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: RestoreQuarantineItems()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function restores backed up quarantine items
//	Needs CustomActionData set to [CommonAppDataFolder]
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall RestoreQuarantineItems( MSIHANDLE hInstall )
{
	CString strSource, strDest;

	try
	{
		strSource.Format( "%sQuarantine", GetTempDir() );
		strDest.Format( "%sQuarantine", GetMSIProperty( hInstall, "CustomActionData" ) );

		if( CopyDir( hInstall, strSource, strDest ) > 0 )
		{
			MSILogMessage( hInstall, _T("Quarantine items restored successfully") );
			DeleteDir( hInstall, (TCHAR*)strSource.GetBuffer(), "*.*" );
			strSource.ReleaseBuffer();
		}
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: OpenSavTreeHKLM()
//
// Parameters:
//		DWORD - Mode to open reg key in, ie KEY_READ or KEY_WRITE
//
// Returns:
//		HKEY - Handle to the registry key if successful, NULL otherwise
//
// Description:  
//	This opens the HKEY_LOCAL_MACHINE\\Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion
//  registry key 
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
HKEY OpenSavTreeHKLM( DWORD dwMode )
{
	HKEY hKey = NULL;

	RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
					_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"),
					0,
					dwMode,
					&hKey );

	return hKey;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: OpenHKUTree()
//
// Parameters:
//		DWORD - Mode to open reg key in, ie KEY_READ or KEY_WRITE
//
// Returns:
//		HKEY - Handle to the registry key if successful, NULL otherwise
//
// Description:  
//	This opens the HKEY_USERS registry key
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
HKEY OpenSavTreeHKU( DWORD dwMode )
{
	HKEY hKey = NULL;

	RegOpenKeyEx(	HKEY_USERS,
					NULL,
					0,
					dwMode,
					&hKey );

	return hKey;
}


//////////////////////////////////////////////////////////////////////////
//
// Function: OpenMigrateTreeHKML()
//
// Parameters:
//		DWORD - Mode to open reg key in, ie KEY_READ or KEY_WRITE
//
// Returns:
//		HKEY - Handle to the registry key if successful, NULL otherwise
//
// Description:  
//	This opens the Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE
//  registry key.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/03 - JMEADE function created to fix 1-1YTCI4.
//////////////////////////////////////////////////////////////////////////
HKEY OpenMigrateTreeHKLM( DWORD dwMode )
{
	HKEY hKey = NULL;

	RegCreateKeyEx(	HKEY_CURRENT_USER,
					_T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE"),
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					dwMode,
					NULL,
					&hKey,
					NULL );

	return hKey;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: OpenMigrateTreeHKU()
//
// Parameters:
//		DWORD - Mode to open reg key in, ie KEY_READ or KEY_WRITE
//
// Returns:
//		HKEY - Handle to the registry key if successful, NULL otherwise
//
// Description:  
//	This opens the Software\\Symantec\\Install\\MigrateSettings\\HKEY_USERS
//  registry key.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/03 - JMEADE function created to fix 1-1YTCI4.
//////////////////////////////////////////////////////////////////////////
HKEY OpenMigrateTreeHKU( DWORD dwMode )
{
	HKEY hKey = NULL;

	RegCreateKeyEx(	HKEY_CURRENT_USER,
					_T("Software\\Symantec\\Install\\MigrateSettings\\HKEY_USERS"),
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					dwMode,
					NULL,
					&hKey,
					NULL );

	return hKey;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: CopyRegKeys()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//
// Returns:
//		BOOL - TRUE if all keys copied without error
//
// Description:  
//	This function copies the settings we need to preserve during a migration
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
BOOL CopyRegKeys( HKEY hSource, HKEY hDest )
{
	BOOL bRet = TRUE;

	// Common keys
	CopyKeys( hSource, hDest, _T("AdministratorOnly") );
	CopyKeys( hSource, hDest, _T("Common") );
	CopyKeys( hSource, hDest, _T("Custom Tasks") );
	CopyKeys( hSource, hDest, _T("LocalScans") );
	CopyKeys( hSource, hDest, _T("PatternManager") );
	CopyKeys( hSource, hDest, _T("ProductControl") );
	CopyKeys( hSource, hDest, _T("Quarantine") );

	CopyKeys( hSource, hDest, _T("Storages\\Filesystem") );

	CopyKeys( hSource, hDest, _T("Storages\\LotusNotes\\RealTimeScan") );
	CopyKeys( hSource, hDest, _T("Storages\\MicrosoftExchangeClient\\RealTimeScan") );
	CopyKeys( hSource, hDest, _T("Storages\\InternetMail\\RealTimeScan") );
	CopyKeys( hSource, hDest, _T("Storages\\SymProtect\\RealTimeScan") );

	CopyKeys( hSource, hDest, szReg_Key_Exclusions );
	CopyKeys( hSource, hDest, szReg_Key_Reporting );
	
	// Server keys
	CopyKeys( hSource, hDest, _T("Groups") );
	CopyKeys( hSource, hDest, _T("Clients") );
	CopyKeys( hSource, hDest, _T("Children") );
	CopyKeys( hSource, hDest, _T("Accounts") );
	CopyKeys( hSource, hDest, _T("ScsComms") );
	CopyKeys( hSource, hDest, _T("DomainData") );
	CopyKeys( hSource, hDest, _T("ClientConfig") );
	CopyKeys( hSource, hDest, _T("ClientTrack") );
	// JHoa - 08/04/05 - Forward port Defect 1-4H37NQ fix.
	// Copy server's LiveUpdateSource settings. 
	CopyKeys( hSource, hDest, _T("LiveUpdateSource") );

	return bRet;
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
// Function: DeleteKeys()
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
void DeleteKeys( HKEY hSKey, HKEY hDKey, TCHAR *szKeyname )
{
	HKEY hSubkey = NULL;
	HKEY hWSubkey = NULL;
	TCHAR szSubkeyname[ MAX_PATH ] = {0};
	DWORD dwSize = sizeof( szSubkeyname );
	DWORD dwIndex = 0;

	RegCreateKeyEx( hDKey, szKeyname, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hWSubkey, NULL );

	if( ERROR_SUCCESS == RegOpenKeyEx( hSKey, szKeyname, NULL, KEY_READ, &hSubkey ) )
	{
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
	}

	if( hWSubkey )
		RegCloseKey( hWSubkey );
}
