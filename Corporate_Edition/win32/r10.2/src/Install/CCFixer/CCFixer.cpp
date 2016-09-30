// CCFixer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

int _tmain( int argc, TCHAR* argv[] )
{
	int iRetval = 0;

	if( argc != 2 )
	{
		wcout << _T("Usage: CCFixer Filename.msi") << endl;
		return 1;
	}

	MSIHANDLE hDatabase, hView;
	CString strSQL;
	
	if( MsiOpenDatabase( argv[1], MSIDBOPEN_TRANSACT, &hDatabase ) == ERROR_SUCCESS )
	{
		strSQL = _T("UPDATE CustomAction SET Source = 'Callreginst.58B3CBD8_773E_456F_B761_5F9C67C2E7B1' WHERE Source = 'Callreginst.82434E30_423F_11D3_AF4B_00600811C705'");

		if( MsiDatabaseOpenView( hDatabase, strSQL, &hView ) == ERROR_SUCCESS )
		{
			if( MsiViewExecute( hView, NULL ) == ERROR_SUCCESS )
			{
				wcout << _T("Successfully updated the CustomAction table...") << endl;
			}
			else
			{
				wcout << _T("Error: Failed to update the CustomAction table...") << endl;
				iRetval = 1;
			}
			MsiCloseHandle( hView );
		}

/*		strSQL.Format( _T("UPDATE Property SET Value = '%s' WHERE Property = 'ProductVersion'"), _T(NAVFILEPRODUCTVERSION)  );

		if( MsiDatabaseOpenView( hDatabase, strSQL, &hView ) == ERROR_SUCCESS )
		{
			if( MsiViewExecute( hView, NULL ) == ERROR_SUCCESS )
			{
				wcout << _T("Successfully updated the Property table...") << endl;
			}
			else
			{
				wcout << _T("Error: Failed to update the Property table...") << endl;
				iRetval = 1;
			}
			MsiCloseHandle( hView );
		}
*/
		if( iRetval == 0 )
		{
			MsiDatabaseCommit( hDatabase );
		}

		MsiCloseHandle( hDatabase );
	}
	else
	{
		wcout << _T("Error: Failed to open the MSI file...") << endl;
		iRetval = 1;
	}

	wcout << _T("Done!") << endl;
	return iRetval;
}
