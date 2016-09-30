// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Registry.cpp: Installation Registry Custom Actions
//

#include "stdafx.h"
#include "savMainCA.h"
#include "SymSaferRegistry.h"

//////////////////////////////////////////////////////////
//
//	VP6UsageCountDec
//	VP6UsageCountInc
//
//	These two functions are used to increment and decrement
//	the VP6UsageCount registry key.  Each function serves
//	as the rollback proceedure for the other.
//
//	Execution Mode: Deferred in System Context or Rollback
//
//	10/24/2003 - Thomas Brock - Inital work.
//

#define INSTALLED_APPS_KEY "Software\\Symantec\\InstalledApps"
#define VP6_REGVALUENAME "VP6UsageCount"

UINT _stdcall VP6UsageCountDec( MSIHANDLE hInstall )
{
	DWORD dwData, dwSize, dwType;
	HKEY hKey;
	LONG lValue;

	lValue = RegOpenKeyEx( HKEY_LOCAL_MACHINE, INSTALLED_APPS_KEY, NULL, KEY_ALL_ACCESS, &hKey );
	if( ERROR_SUCCESS == lValue )
	{
		dwSize = sizeof( dwData );
		lValue = SymSaferRegQueryValueEx( hKey, VP6_REGVALUENAME, NULL, &dwType, (LPBYTE) &dwData, &dwSize );
		if( ( ERROR_SUCCESS == lValue ) && ( REG_DWORD == dwType ) )
		{
			if( dwData > 1 )
			{
				dwData--;
				lValue = RegSetValueEx( hKey, VP6_REGVALUENAME, NULL, REG_DWORD, (CONST BYTE *) &dwData, dwSize );
				if( ERROR_SUCCESS == lValue )
				{
					MSILogMessage( hInstall, "Successfully decremented VP6UsageCount registry value." );
				}
				else
				{
					MSILogMessage( hInstall, "Error decrementing VP6UsageCount registry value." );
				}
			}
			else
			{
				RegDeleteValue( hKey, VP6_REGVALUENAME );
			}
		}
	}

	// Always return error success, because we don't 
	// want to return an error during uninstall.
	return ERROR_SUCCESS;
}

UINT _stdcall VP6UsageCountInc( MSIHANDLE hInstall )
{
	DWORD dwData, dwSize;
	HKEY hKey;
	LONG lValue;
	UINT retval = ERROR_SUCCESS;

	lValue = RegCreateKeyEx( HKEY_LOCAL_MACHINE, INSTALLED_APPS_KEY, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL );
	if( ERROR_SUCCESS == lValue )
	{
		dwSize = sizeof( dwData );
		lValue = SymSaferRegQueryValueEx( hKey, VP6_REGVALUENAME, NULL, NULL, (LPBYTE) &dwData, &dwSize );
		if( ERROR_SUCCESS == lValue )
		{
			dwData++;
		}
		else
		{
			dwData = 1;
		}

		dwSize = sizeof( dwData );
		lValue = RegSetValueEx( hKey, VP6_REGVALUENAME, NULL, REG_DWORD, (CONST BYTE *) &dwData, dwSize );
		if( ERROR_SUCCESS == lValue )
		{
			MSILogMessage( hInstall, "Successfully wrote VP6UsageCount registry value." );
		}
		else
		{
			MSILogMessage( hInstall, "Error writing VP6UsageCount registry value." );
			retval = ERROR_INSTALL_FAILURE;
		}
	}
	else
	{
		MSILogMessage( hInstall, "Error opening InstalledApps registry key." );
		retval = ERROR_INSTALL_FAILURE;
	}

	return retval;
}

//////////////////////////////////////////////////////////
//
//	AMSUsageCountDec
//	AMSUsageCountInc
//
//	These two functions are used to increment and decrement
//	the AMSUsageCount registry key.  Each function serves
//	as the rollback proceedure for the other.
//
//	Execution Mode: Deferred in System Context or Rollback
//
//	10/24/2003 - Thomas Brock - Inital work.
//	12/15/2003 - Dan Kowalyshyn - Bootlegged directly from Tom's work. :)
//
#define INSTALLED_APPS_KEY "Software\\Symantec\\InstalledApps"
#define AMS_REGVALUENAME "AMSUsageCount"

UINT _stdcall AMSUsageCountDec( MSIHANDLE hInstall )
{
	DWORD dwData, dwSize, dwType;
	HKEY hKey;
	LONG lValue;

	lValue = RegOpenKeyEx( HKEY_LOCAL_MACHINE, INSTALLED_APPS_KEY, NULL, KEY_ALL_ACCESS, &hKey );
	if( ERROR_SUCCESS == lValue )
	{
		dwSize = sizeof( dwData );
		lValue = SymSaferRegQueryValueEx( hKey, AMS_REGVALUENAME, NULL, &dwType, (LPBYTE) &dwData, &dwSize );
		if( ( ERROR_SUCCESS == lValue ) && ( REG_DWORD == dwType ) )
		{
			if( dwData > 1 )
			{
				dwData--;
				lValue = RegSetValueEx( hKey, AMS_REGVALUENAME, NULL, REG_DWORD, (CONST BYTE *) &dwData, dwSize );
				if( ERROR_SUCCESS == lValue )
				{
					MSILogMessage( hInstall, "Successfully decremented AMSUsageCount registry value." );
				}
				else
				{
					MSILogMessage( hInstall, "Error decrementing AMSUsageCount registry value." );
				}
			}
			else
			{
				RegDeleteValue( hKey, AMS_REGVALUENAME );
			}
		}
	}

	// Always return error success, because we don't 
	// want to return an error during uninstall.
	return ERROR_SUCCESS;
}

UINT _stdcall AMSUsageCountInc( MSIHANDLE hInstall )
{
	DWORD dwData, dwSize;
	HKEY hKey;
	LONG lValue;
	UINT retval = ERROR_SUCCESS;

	lValue = RegCreateKeyEx( HKEY_LOCAL_MACHINE, INSTALLED_APPS_KEY, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL );
	if( ERROR_SUCCESS == lValue )
	{
		dwSize = sizeof( dwData );
		lValue = SymSaferRegQueryValueEx( hKey, AMS_REGVALUENAME, NULL, NULL, (LPBYTE) &dwData, &dwSize );
		if( ERROR_SUCCESS == lValue )
		{
			dwData++;
		}
		else
		{
			dwData = 1;
		}

		dwSize = sizeof( dwData );
		lValue = RegSetValueEx( hKey, AMS_REGVALUENAME, NULL, REG_DWORD, (CONST BYTE *) &dwData, dwSize );
		if( ERROR_SUCCESS == lValue )
		{
			MSILogMessage( hInstall, "Successfully wrote AMSUsageCount registry value." );
		}
		else
		{
			MSILogMessage( hInstall, "Error writing AMSUsageCount registry value." );
			retval = ERROR_INSTALL_FAILURE;
		}
	}
	else
	{
		MSILogMessage( hInstall, "Error opening InstalledApps registry key." );
		retval = ERROR_INSTALL_FAILURE;
	}

	return retval;
}
