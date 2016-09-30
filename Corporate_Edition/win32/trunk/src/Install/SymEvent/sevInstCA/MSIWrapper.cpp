// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"

#include "MSIWrapper.h"

CMSIWrapper::CMSIWrapper()
{
	m_hDatabase = NULL;
	m_strFilename = "";
}

CMSIWrapper::CMSIWrapper( MSIHANDLE hDatabase )
{
	m_hDatabase = hDatabase;
	m_strFilename = "";
}

CMSIWrapper::CMSIWrapper( CString strName )
{
	m_strFilename = strName;
	openDatabase();
}

CMSIWrapper::~CMSIWrapper()
{
	closeDatabase();
}

void CMSIWrapper::setFilename( CString& strName )
{
	if( strName.GetLength() > 0 )
		m_strFilename = strName;
}

UINT CMSIWrapper::openDatabase()
{
	UINT nRetVal = ERROR_OPEN_FAILED;

	closeDatabase(); // in case

	if( m_strFilename.GetLength() > 0 )
		nRetVal = MsiOpenDatabase( (LPCSTR)m_strFilename, MSIDBOPEN_READONLY, &m_hDatabase );

	return nRetVal;
}

UINT CMSIWrapper::closeDatabase()
{
	UINT nRetVal = ERROR_SUCCESS;

	if( m_hDatabase )
		nRetVal = MsiCloseHandle( m_hDatabase );

	return nRetVal;
}

UINT CMSIWrapper::msiOpenTable( MSIHANDLE *hView, CString& strQuery )
 {
	UINT nRetVal = ERROR_SUCCESS;

	if( m_hDatabase )
	{
		nRetVal = MsiDatabaseOpenView( m_hDatabase, (LPCSTR)strQuery, hView );
		if( ERROR_SUCCESS == nRetVal )
			nRetVal = MsiViewExecute( *hView, NULL );
	}
	else
		nRetVal = ERROR_INVALID_HANDLE;

	return nRetVal;
}

UINT CMSIWrapper::msiCloseTable( MSIHANDLE hView )
{
	UINT nRetVal = ERROR_SUCCESS;
	
	if( hView )
		nRetVal = MsiCloseHandle( hView );

	return nRetVal;
}

int CMSIWrapper::countRecords( MSIHANDLE hView )
{
	int i = 0;

	if( hView )
	{
		PMSIHANDLE hRecord;
		while( ERROR_SUCCESS == MsiViewFetch( hView, &hRecord ) )
			i++;
	}
	else
		i = -1;

	return i;
}
