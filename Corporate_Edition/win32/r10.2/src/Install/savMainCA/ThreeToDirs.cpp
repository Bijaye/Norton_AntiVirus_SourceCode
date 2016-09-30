// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#include "Stdafx.h"
#include "savMainCA.h"
#include "ThreeToDirs.h"

//////////////////////////////////////////////////////////////////////////
//
// Function: ProcessToDirs()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function checks for the existence of three directories off of the
//  root install dir (ToSAV, ToAPP and ToLU ) and copies their contents
//  appropriatly: The ToSAV folder to the INSTALLDIR, ToLU to the LiveUpdate
//  folder and the ToAPP to the Documents and Settings folder.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall ProcessToDirs( MSIHANDLE hInstall )
{
	DWORD	dwLen = 0;
	CString strData;

	try
	{
		// Get the size of the data
		if( ERROR_MORE_DATA != MsiGetProperty( hInstall, _T("CustomActionData"), "", &dwLen ) )
			throw( _T("Error getting length of CustomActionData") );

		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("CustomActionData"), strData.GetBuffer( dwLen ), &dwLen ) )
			throw( _T("Unable to obtain the CustomActionData") );
		strData.ReleaseBuffer();
		
		// Get the parameters, order here must match the data in CustomActionData
		CString strSource = GetParam( &strData );
		CString strInstallDir = GetParam( &strData );
		CString strProgramFiles = GetParam( &strData );
		CString strCommonApp = GetParam( &strData );

		// Process the folders
		ToSAV( hInstall, strSource, strInstallDir );
		ToApp( hInstall, strSource, strCommonApp );
		ToLU( hInstall, strSource, strProgramFiles );
		MSILogMessage( hInstall, "ToSAV, ToApp and ToLU proccessed" );

	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: getParam()
//
// Parameters:
//		CString *- String containing list of parameters
//
// Returns:
//		CString - left most value
//
// Description:  
//	Given a single string comprised of N parameters delimited with the ';' 
//  character, this function returns the left most parameter as a return 
//  value and modifies the data string removing that parameter and the ';'
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
CString GetParam( CString *strData )
{
	CString strParam = "";
	CString strTemp;

	if( strData->GetLength() > 0 )
	{
		// Find delimeter, if not found that means we are at last entry in list
		int nSemi = strData->FindOneOf( ";" );
		if( -1 != nSemi )
		{
			strParam = strData->Left( nSemi );
			
			// Remove parameter from the list, taking delimiter as well (the extra - 1 )
			strTemp = strData->Right( (strData->GetLength() - nSemi) - 1 );
			*strData = strTemp;
		}
		else
		{
			strParam = *strData;
			*strData = "";
		}
	}

	return strParam;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: ToSAV()
//
// Parameters:
//		CString - Source directory to check for folder
//		CString - INSTALLDIR
//
// Returns:
//		BOOL - TRUE if files were copied, FALSE otherwise
//
// Throws:
//		TCHAR * - string of error
//
// Description:  
//	Checks for the existance of strSource\ToSAV folder and if present
//  copies the contents to strDest. 
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
BOOL ToSAV( MSIHANDLE hInstall, CString strSource, CString strDest )
{
	BOOL bRet = FALSE;
	CString strSrcDir = strSource + _T("ToSAV");

	if( DirExist( strSrcDir ) )
		bRet = CopyDir( hInstall, strSrcDir, strDest );
	else
		MSILogMessage( hInstall, _T("No ToSAV folder") );

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: ToApp()
//
// Parameters:
//		CString - Source directory to check for folder
//		CString - CommonAppData
//
// Returns:
//		BOOL - TRUE if files were copied, FALSE otherwise
//
// Throws:
//		TCHAR * - string of error
// Description:  
//	Checks for the existance of strSource\ToApp folder and if present
//  copies the contents to strDest. 
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
BOOL ToApp( MSIHANDLE hInstall, CString strSource, CString strDest )
{
	BOOL bRet = FALSE;
	CString strSrcDir = strSource + _T("ToApp");
	CString strDestination = strDest + _T("Symantec\\Symantec AntiVirus Corporate Edition\\7.5");

	if( DirExist( strSrcDir ) )
		bRet = CopyDir( hInstall, strSrcDir, strDestination );
	else
		MSILogMessage( hInstall, _T("No ToApp folder") );

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: ToLU()
//
// Parameters:
//		CString - Source directory to check for folder
//		CString - ProgramFiles
//
// Returns:
//		BOOL - TRUE if files were copied, FALSE otherwise
//
// Throws:
//		TCHAR * - string of error
// Description:  
//	Checks for the existance of strSource\ToLU folder and if present
//  copies the contents to strDest. 
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
BOOL ToLU( MSIHANDLE hInstall, CString strSource, CString strDest )
{
	BOOL bRet = FALSE;
	CString strSrcDir = strSource + _T("ToLU");
	CString strDestination = strDest + _T("Symantec\\LiveUpdate");

	if( DirExist( strSrcDir ) )
		bRet = CopyDir( hInstall, strSrcDir, strDestination );
	else
		MSILogMessage( hInstall, _T("No ToLU folder") );

	return bRet;
}
