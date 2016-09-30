// FixMSI.cpp
//
// Fixes-up an MSI file for patch pre-processing.
//
// History:
//
//	1.0 Brian Aljian	1/20/2005		Initial version
//

#include "stdafx.h"
#include <afx.h>
#include <windows.h>
#include <msi.h>
#include <msiquery.h>

// SQL query format templates
CString g_strUpdateProperty ("INSERT INTO `Property`(`Property`, `Value`) VALUES('%s', '%s')");

BOOL UpdateAnMSIProperty( MSIHANDLE hDatabase, LPCSTR szPropertyStr, LPCSTR szPropertyValue);


int main(int argc, char* argv[])
{
	MSIHANDLE hDatabase;
	DWORD dwError;
	BOOL bFeature = FALSE;
	BOOL bFile = FALSE;
	BOOL bFileUnversion = FALSE;

	if (argc < 4)
	{
		printf("Syntax: %s <file.MSI> <PropertyName> <PropertyValue>\n", argv[0]);
		return 1;
	}

	// Open the database
	dwError = MsiOpenDatabase(argv[1], MSIDBOPEN_TRANSACT, &hDatabase );
	if ( dwError != ERROR_SUCCESS )
	{
		printf("FIXMSI: MsiOpenDatabase failed on: %s.\n", argv[1]);
		return 1;
	}

	printf("FIXMSI: Opened database %s\n", argv[1]);

	
	printf("FIXMSI: Update Property => %s=%s\n", argv[2], argv[3]);

	UpdateAnMSIProperty(hDatabase, argv[2], argv[3]);

	// Commit the changes to the database
	dwError = MsiDatabaseCommit( hDatabase );
	if ( dwError != ERROR_SUCCESS )
	{
		printf("FIXMSI: MsiDatabaseCommit failed\n");
	}

	MsiCloseHandle( hDatabase );

	printf("FIXMSI: Closed database %s\n", argv[1]);

	return 0;
}



/////////////////////////////////////////////////////////////////////
//
// Function: UpdateAnMSIProperty
//
// Purpose: Updates for Property Table
//
// Inputs:	hDatabase - handle to the MSI database
//			strFile - The file to delete
//
// Returns: TRUE if the file was deleted successfully
//			FALSE otherwise
//
/////////////////////////////////////////////////////////////////////

BOOL UpdateAnMSIProperty( MSIHANDLE hDatabase, LPCSTR szPropertyStr, LPCSTR szPropertyValue)
{
	MSIHANDLE hView;
	CString strQuery;
	BOOL bRet = FALSE;
	DWORD dwRet = 0;

	strQuery.Format( g_strUpdateProperty, szPropertyStr, szPropertyValue);

	if ( ERROR_SUCCESS == (dwRet = MsiDatabaseOpenView( hDatabase, strQuery, &hView )) )
	{
		if ( ERROR_SUCCESS == (dwRet = MsiViewExecute( hView, NULL )) )
		{
			// Success
			printf("FIXMSI: Update Property Name %s\n", szPropertyStr);

			bRet = TRUE;

		}
		else
		{
			if (dwRet != ERROR_FUNCTION_FAILED) /* It already exists */
			{
				printf("FIXMSI: MsiViewExecute failed with query: %s\n", strQuery);
				return bRet;
			}
			else
			{
				// Success
				printf("FIXMSI: Property already exists %s\n", szPropertyStr);
				bRet = TRUE;
			}
		}
	}
	else
	{
		printf("FIXMSI: MsiDatabaseOpenView for PropertyName failed with query %s\n", strQuery);
	}

	MsiCloseHandle( hView );

	return bRet;
}
