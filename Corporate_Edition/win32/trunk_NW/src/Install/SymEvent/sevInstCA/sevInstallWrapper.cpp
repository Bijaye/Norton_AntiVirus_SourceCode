// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "sevInstallWrapper.h"

CSevInstallWrapper::CSevInstallWrapper()
{
	m_hTable = NULL;
}

CSevInstallWrapper::CSevInstallWrapper( MSIHANDLE hDatabase )
{
	m_hTable = NULL;
	CMSIWrapper::setDatabaseHandle( hDatabase );
}

CSevInstallWrapper::CSevInstallWrapper( CString strName )
{
	CMSIWrapper::setFilename( strName );
}

CSevInstallWrapper::~CSevInstallWrapper()
{
	CMSIWrapper::msiCloseTable( m_hTable );
}

UINT CSevInstallWrapper::openTable( CString strQuery )
{
	CString strSQL;

	// Determine if we have a predefined query or just open all records
	if( strQuery.GetLength() > 0 )
		strSQL = strQuery;
	else
		strSQL = "SELECT * FROM sevInstall";

	return CMSIWrapper::msiOpenTable( &m_hTable, strSQL );
}

UINT CSevInstallWrapper::closeTable()
{
	return CMSIWrapper::msiCloseTable( m_hTable );
}

int CSevInstallWrapper::count()
{
	return CMSIWrapper::countRecords( m_hTable );
}

UINT CSevInstallWrapper::getNext()
{
	DWORD dwSize;
	UINT nRet = ERROR_SUCCESS;
	char szTemp[MAX_PATH];

	if( m_hTable )
	{
		// Fetch the record
		PMSIHANDLE hRecord;
		nRet = MsiViewFetch( m_hTable, &hRecord );
		
		// Fill out the fields
		if( ERROR_SUCCESS == nRet )
		{
			ZeroMemory( szTemp, sizeof( szTemp ) );
			dwSize = sizeof(szTemp);
			MsiRecordGetString( hRecord, 1, szTemp, &dwSize );
			m_strID = CString( szTemp );

			ZeroMemory( szTemp, sizeof( szTemp ) );
			dwSize = sizeof(szTemp);
			MsiRecordGetString( hRecord, 2, szTemp, &dwSize );
			m_strInstCmd = CString( szTemp );

			ZeroMemory( szTemp, sizeof( szTemp ) );
			dwSize = sizeof(szTemp);
			MsiRecordGetString( hRecord, 3, szTemp, &dwSize );
			m_strRmveCmd = CString( szTemp );

			ZeroMemory( szTemp, sizeof( szTemp ) );
			dwSize = sizeof(szTemp);
			MsiRecordGetString( hRecord, 4, szTemp, &dwSize );
			m_strDivision = CString( szTemp );
		}
	}
	else
		nRet = ERROR_INVALID_HANDLE;

	return nRet;
}