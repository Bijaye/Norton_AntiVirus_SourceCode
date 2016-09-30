////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <msi.h>
#include <Msiquery.h>

#include "MSIWrapper.h"

CMSIWrapper::CMSIWrapper()
{
	m_MSIHandle = NULL;
	m_FileView = NULL;
	m_RegistryView = NULL;
	m_ComponentView = NULL;
	m_DirectoryView = NULL;
	m_strFilename = _T("");
	m_hInstall = NULL;
}

CMSIWrapper::CMSIWrapper( CString& strName )
{
	m_strFilename = strName;
}

CMSIWrapper::~CMSIWrapper()
{
	CloseComponentTable();
	CloseRegistryTable();
	CloseFileTable();
	CloseDirectoryTable();
	CloseDatabase();
	ClosePackage();
}

void CMSIWrapper::SetFilename( CString& strName )
{
	if( strName.GetLength() > 0 )
		m_strFilename = strName;
}

CString CMSIWrapper::GetFilename()
{
	return m_strFilename;
}

UINT CMSIWrapper::OpenDatabase()
{
	UINT nRetVal = ERROR_OPEN_FAILED;

	if( m_strFilename.GetLength() > 0 )
		nRetVal = MsiOpenDatabase( m_strFilename, MSIDBOPEN_READONLY, &m_MSIHandle );

	return nRetVal;
}

UINT CMSIWrapper::CloseDatabase()
{
	UINT nRetVal = ERROR_SUCCESS;

	if( m_MSIHandle )
		nRetVal = MsiCloseHandle( m_MSIHandle );

	return nRetVal;
}


UINT CMSIWrapper::OpenTable( CString& strQuery, MSIHANDLE *hView )
{
	UINT nRetVal = ERROR_SUCCESS;

	if( m_MSIHandle )
	{
		nRetVal = MsiDatabaseOpenView( m_MSIHandle, strQuery, hView );
		if( ERROR_SUCCESS == nRetVal )
			nRetVal = MsiViewExecute( *hView, NULL );
	}
	else
		nRetVal = ERROR_INVALID_HANDLE;

	return nRetVal;
}

int CMSIWrapper::CountRecords( MSIHANDLE hView )
{
	int i = 0;

	if( hView )
	{
		MSIHANDLE hRecord;
		while( ERROR_SUCCESS == MsiViewFetch( hView, &hRecord ) )
			i++;
		
		MsiCloseHandle( hRecord );
	}
	else
		i = -1;

	return i;
}

UINT CMSIWrapper::CloseTable( MSIHANDLE hView )
{
	UINT nRetVal = ERROR_SUCCESS;
	
	if( hView )
		nRetVal = MsiCloseHandle( hView );

	return nRetVal;
}

UINT CMSIWrapper::OpenFileTable( CString strQuery )
{
	CString strSQL;

	// Determine if we have a predefined query or just open all records
	if( strQuery.GetLength() > 0 )
		strSQL = strQuery;
	else
		strSQL = "SELECT * FROM File";

	return OpenTable( strSQL, &m_FileView );
}

int CMSIWrapper::CountFiles()
{
	return CountRecords( m_FileView );
}

UINT CMSIWrapper::GetNextFile( CMSIWrapperFile *file )
{
	UINT nRetVal = ERROR_SUCCESS;
	DWORD dwSize;
	LPSTR pTemp;

	if( m_FileView )
	{
		// Fetch the record
		MSIHANDLE hRecord;
		nRetVal = MsiViewFetch( m_FileView, &hRecord );
		
		// Fill out the fields
		if( ERROR_SUCCESS == nRetVal )
		{
			dwSize = 255;
			pTemp = file->strFile.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 1, pTemp, &dwSize );
			file->strFile.ReleaseBuffer();

			dwSize = 255;
			pTemp = file->strComponent.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 2, pTemp, &dwSize );
			file->strComponent.ReleaseBuffer();

			dwSize = 255;
			pTemp = file->strFileName.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 3, pTemp, &dwSize );
			file->strFileName.ReleaseBuffer();

			file->nFileSize = MsiRecordGetInteger( hRecord, 4 );

			dwSize = 255;
			pTemp = file->strVersion.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 5, pTemp, &dwSize );
			file->strVersion.ReleaseBuffer();

			file->nLanguage = MsiRecordGetInteger( hRecord, 6 );

			file->nAttributes = MsiRecordGetInteger( hRecord, 7 );
			
			file->nSequence = MsiRecordGetInteger( hRecord, 8 );
		}

		// Close our handle
		if( hRecord )
			MsiCloseHandle( hRecord );
	}
	else
		nRetVal = ERROR_INVALID_HANDLE;
	
	return nRetVal;
}

UINT CMSIWrapper::CloseFileTable()
{
	return CloseTable( m_FileView );
}

UINT CMSIWrapper::OpenComponentTable( CString strQuery )
{
	CString strSQL;

	// Determine if we have a query to execute or if we just need to open all records
	if( strQuery.GetLength() > 0 )
		strSQL = strQuery;
	else
		strSQL = "SELECT * FROM Component";

	return OpenTable( strSQL, &m_ComponentView );
}

int CMSIWrapper::CountComponents()
{
	return CountRecords( m_ComponentView );
}

UINT CMSIWrapper::GetNextComponent( CMSIWrapperComponent *component )
{
	UINT nRetVal = ERROR_SUCCESS;
	DWORD dwSize;
	LPSTR pTemp;

	if( m_ComponentView )
	{
		// Fetch the record
		MSIHANDLE hRecord;
		nRetVal = MsiViewFetch( m_ComponentView, &hRecord );
		
		// Fill out the fields
		if( ERROR_SUCCESS == nRetVal )
		{
			dwSize = 255;
			pTemp = component->strComponent.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 1, pTemp, &dwSize );
			component->strComponent.ReleaseBuffer();

			dwSize = 255;
			pTemp = component->strComponentID.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 2, pTemp, &dwSize );
			component->strComponentID.ReleaseBuffer();

			dwSize = 255;
			pTemp = component->strDirectory.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 3, pTemp, &dwSize );
			component->strDirectory.ReleaseBuffer();

			component->nAttributes = MsiRecordGetInteger( hRecord, 4 );

			dwSize = 255;
			pTemp = component->strCondition.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 5, pTemp, &dwSize );
			component->strCondition.ReleaseBuffer();

			dwSize = 255;
			pTemp = component->strKeyPath.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 6, pTemp, &dwSize );
			component->strKeyPath.ReleaseBuffer();
}

		// Close our handle
		if( hRecord )
			MsiCloseHandle( hRecord );
	}
	else
		nRetVal = ERROR_INVALID_HANDLE;
	
	return nRetVal;
}

UINT CMSIWrapper::CloseComponentTable()
{
	return CloseTable( m_ComponentView );
}
UINT CMSIWrapper::OpenDirectoryTable( CString strQuery )
{
	CString strSQL;

	// Determine if we have a query to execute or if we just need to open all records
	if( strQuery.GetLength() > 0 )
		strSQL = strQuery;
	else
		strSQL = "SELECT * FROM Directory";

	return OpenTable( strSQL, &m_DirectoryView );
}

int CMSIWrapper::CountDirectorys()
{
	return CountRecords( m_DirectoryView );
}

UINT CMSIWrapper::GetNextDirectory( CMSIWrapperDirectory *directory )
{
	UINT nRetVal = ERROR_SUCCESS;
	DWORD dwSize;
	LPSTR pTemp;

	if( m_DirectoryView )
	{
		// Fetch the record
		MSIHANDLE hRecord;
		nRetVal = MsiViewFetch( m_DirectoryView, &hRecord );
		
		// Fill out the fields
		if( ERROR_SUCCESS == nRetVal )
		{
			dwSize = 255;
			pTemp = directory->strDirectory.GetBuffer(dwSize);
			MsiRecordGetString(hRecord, 1, pTemp, &dwSize );
			directory->strDirectory.ReleaseBuffer();
			
			dwSize = 255;
			pTemp = directory->strParent.GetBuffer(dwSize);
			MsiRecordGetString(hRecord, 2, pTemp, &dwSize );
			directory->strParent.ReleaseBuffer();

			dwSize = 255;
			pTemp = directory->strDefault.GetBuffer(dwSize);
			MsiRecordGetString(hRecord, 3, pTemp, &dwSize );
			directory->strDefault.ReleaseBuffer();
	
		}	

		// Close our handle
		if( hRecord )
			MsiCloseHandle( hRecord );
	}
	else
		nRetVal = ERROR_INVALID_HANDLE;
	
	return nRetVal;
}

UINT CMSIWrapper::CloseDirectoryTable()
{
	return CloseTable( m_DirectoryView );
}

UINT CMSIWrapper::OpenRegistryTable( CString strQuery )
{
	CString strSQL;

	// Determine if we have a query to execute or if we just need to open all records
	if( strQuery.GetLength() > 0 )
		strSQL = strQuery;
	else
		strSQL = "SELECT * FROM Registry";

	return OpenTable( strSQL, &m_RegistryView );
}

int CMSIWrapper::CountRegistry()
{
	return CountRecords( m_RegistryView );
}

UINT CMSIWrapper::GetNextRegistry( CMSIWrapperRegistry *registry )
{
	UINT nRetVal = ERROR_SUCCESS;
	DWORD dwSize;
	LPSTR pTemp;

	if( m_RegistryView )
	{
		// Fetch the record
		MSIHANDLE hRecord;
		nRetVal = MsiViewFetch( m_RegistryView, &hRecord );
		
		// Fill out the fields
		if( ERROR_SUCCESS == nRetVal )
		{
			dwSize = 255;
			pTemp = registry->strRegistry.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 1, pTemp, &dwSize );
			registry->strRegistry.ReleaseBuffer();

			registry->nRoot = MsiRecordGetInteger( hRecord, 2 );

			dwSize = 255;
			pTemp = registry->strKey.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 3, pTemp, &dwSize );
			registry->strKey.ReleaseBuffer();

			dwSize = 255;
			pTemp = registry->strName.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 4, pTemp, &dwSize );
			registry->strName.ReleaseBuffer();

			dwSize = 255;
			pTemp = registry->strValue.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 5, pTemp, &dwSize );
			registry->strValue.ReleaseBuffer();

			dwSize = 255;
			pTemp = registry->strComponent.GetBuffer( dwSize );
			MsiRecordGetString( hRecord, 6, pTemp, &dwSize );
			registry->strComponent.ReleaseBuffer();
		}

		// Close our handle
		if( hRecord )
			MsiCloseHandle( hRecord );
	}
	else
		nRetVal = ERROR_INVALID_HANDLE;
	
	return nRetVal;
}

UINT CMSIWrapper::CloseRegistryTable()
{
	return CloseTable( m_RegistryView );
}
UINT CMSIWrapper::OpenAppIDTable( CString strQuery )
   {
   	CString strSQL;
   
   	// Determine if we have a query to execute or if we just need to open all records
   	if( strQuery.GetLength() > 0 )
   		strSQL = strQuery;
   	else
   		strSQL = "SELECT * FROM AppId";
   
   	return OpenTable( strSQL, &m_AppIDView );
   }
   
   int CMSIWrapper::CountAppID()
   {
   	return CountRecords( m_AppIDView );
   }
   
   UINT CMSIWrapper::GetNextAppID( CMSIWrapperAppID *appid )
   {
   	UINT nRetVal = ERROR_SUCCESS;
   	DWORD dwSize;
   	LPSTR pTemp;
   
   	if( m_AppIDView )
   	{
   		// Fetch the record
   		MSIHANDLE hRecord;
   		nRetVal = MsiViewFetch( m_AppIDView, &hRecord );
   		
   		// Fill out the fields
   		if( ERROR_SUCCESS == nRetVal )
   		{
   			dwSize = 255;
   			pTemp = appid->strAppID.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 1, pTemp, &dwSize );
   			appid->strAppID.ReleaseBuffer();
   
   			dwSize = 255;
   			pTemp = appid->strRemoteSvrName.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 2, pTemp, &dwSize );
   			appid->strRemoteSvrName.ReleaseBuffer();
   
   			dwSize = 255;
   			pTemp = appid->strLocalService.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 3, pTemp, &dwSize );
   			appid->strLocalService.ReleaseBuffer();
   
   			dwSize = 255;
   			pTemp = appid->strServiceParameters.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 5, pTemp, &dwSize );
   			appid->strServiceParameters.ReleaseBuffer();
   
   			dwSize = 255;
   			pTemp = appid->strDllSurrogate.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 6, pTemp, &dwSize );
   			appid->strDllSurrogate.ReleaseBuffer();
   
   			appid->nActivateAtStorage = MsiRecordGetInteger (hRecord,7);
   			appid->nRunAsInteractiveUser = MsiRecordGetInteger (hRecord,8);
   			
   }
   
   		// Close our handle
   		if( hRecord )
   			MsiCloseHandle( hRecord );
   	}
   	else
   		nRetVal = ERROR_INVALID_HANDLE;
   	
   	return nRetVal;
   }
   UINT CMSIWrapper::CloseAppIDTable()
   {
   	return CloseTable( m_AppIDView );
   }
   //ProgID
   UINT CMSIWrapper::OpenProgIDTable( CString strQuery )
   {
   	CString strSQL;
   
   	// Determine if we have a query to execute or if we just need to open all records
   	if( strQuery.GetLength() > 0 )
   		strSQL = strQuery;
   	else
   		strSQL = "SELECT * FROM ProgId";
   
   	return OpenTable( strSQL, &m_ProgIDView );
   }
   
   int CMSIWrapper::CountProgID()
   {
   	return CountRecords( m_ProgIDView );
   }
   
   UINT CMSIWrapper::GetNextProgID( CMSIWrapperProgID *progid )
   {
   	UINT nRetVal = ERROR_SUCCESS;
   	DWORD dwSize;
   	LPSTR pTemp;
   
   	if( m_ProgIDView )
   	{
   		// Fetch the record
   		MSIHANDLE hRecord;
   		nRetVal = MsiViewFetch( m_ProgIDView, &hRecord );
   		
   		// Fill out the fields
   		if( ERROR_SUCCESS == nRetVal )
   		{
   			dwSize = 255;
   			pTemp = progid->strProgID.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 1, pTemp, &dwSize );
   			progid->strProgID.ReleaseBuffer();

   			dwSize = 255;
   			pTemp = progid->strProgID_Parent.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 2, pTemp, &dwSize );
   			progid->strProgID_Parent.ReleaseBuffer();
   
   			dwSize = 255;
   			pTemp = progid->strClass.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 3, pTemp, &dwSize );
   			progid->strClass.ReleaseBuffer();
   
   			dwSize = 255;
   			pTemp = progid->strDescription.GetBuffer( dwSize );
   			MsiRecordGetString( hRecord, 4, pTemp, &dwSize );
			progid->strDescription.ReleaseBuffer();
 
 			dwSize = 255;
 			pTemp = progid->strIcon.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 5, pTemp, &dwSize );
 			progid->strIcon.ReleaseBuffer();
 
 			progid->nIconIndex = MsiRecordGetInteger (hRecord,6);
 			
 }
 
 		// Close our handle
 		if( hRecord )
 			MsiCloseHandle( hRecord );
 	}
 	else
 		nRetVal = ERROR_INVALID_HANDLE;
 	
 	return nRetVal;
 }
 UINT CMSIWrapper::CloseProgIDTable()
 {
 	return CloseTable( m_ProgIDView );
 }
 //MIME
 UINT CMSIWrapper::OpenMIMETable( CString strQuery )
 {
 	CString strSQL;
 
 	// Determine if we have a query to execute or if we just need to open all records
 	if( strQuery.GetLength() > 0 )
 		strSQL = strQuery;
 	else
 		strSQL = "SELECT * FROM MIME";
 
 	return OpenTable( strSQL, &m_MIMEView );
 }
 
 int CMSIWrapper::CountMIME()
 {
 	return CountRecords( m_MIMEView );
 }
 
 UINT CMSIWrapper::GetNextMIME( CMSIWrapperMIME *mime )
 {
 	UINT nRetVal = ERROR_SUCCESS;
 	DWORD dwSize;
 	LPSTR pTemp;
 
 	if( m_MIMEView )
 	{
 		// Fetch the record
 		MSIHANDLE hRecord;
 		nRetVal = MsiViewFetch( m_MIMEView, &hRecord );
 		
 		// Fill out the fields
 		if( ERROR_SUCCESS == nRetVal )
 		{
 			dwSize = 255;
 			pTemp = mime->strContentType.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 1, pTemp, &dwSize );
 			mime->strContentType.ReleaseBuffer();
 
 			dwSize = 255;
 			pTemp = mime->strExtension.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 2, pTemp, &dwSize );
 			mime->strExtension.ReleaseBuffer(); 
 			dwSize = 255;
 			pTemp = mime->strCLSID.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 3, pTemp, &dwSize );
 			mime->strCLSID.ReleaseBuffer();
 			
		}
 
 		// Close our handle
 		if( hRecord )
 			MsiCloseHandle( hRecord );
 	}
 	else
 		nRetVal = ERROR_INVALID_HANDLE;
 	
 	return nRetVal;
 }
 UINT CMSIWrapper::CloseMIMETable()
 {
 	return CloseTable( m_MIMEView );
 }
 //Typelib
 UINT CMSIWrapper::OpenTypeLibTable( CString strQuery )
 {
 	CString strSQL;
 
 	// Determine if we have a query to execute or if we just need to open all records
 	if( strQuery.GetLength() > 0 )
 		strSQL = strQuery;
 	else
 		strSQL = "SELECT * FROM TypeLib";
 
 	return OpenTable( strSQL, &m_TypeLibView );
 }
 
 int CMSIWrapper::CountTypeLib()
 {
 	return CountRecords( m_TypeLibView );
 }
 
 UINT CMSIWrapper::GetNextTypeLib( CMSIWrapperTypeLib *typelib )
 {
 	UINT nRetVal = ERROR_SUCCESS;
 	DWORD dwSize;
 	LPSTR pTemp;
 
 	if( m_TypeLibView )
 	{
 		// Fetch the record
 		MSIHANDLE hRecord;
 		nRetVal = MsiViewFetch( m_TypeLibView, &hRecord );
 		
 		// Fill out the fields
 		if( ERROR_SUCCESS == nRetVal )
 		{
 			dwSize = 255;
 			pTemp = typelib->strLibID.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 1, pTemp, &dwSize );
 			typelib->strLibID.ReleaseBuffer();
 
 			typelib->nLanguage = MsiRecordGetInteger (hRecord,2);
 		
 			dwSize = 255;
 			pTemp = typelib->strComponent.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 3, pTemp, &dwSize );
 			typelib->strComponent.ReleaseBuffer();
 
 			typelib->nVersion = MsiRecordGetInteger (hRecord,4);
 
 			dwSize = 255;
 			pTemp = typelib->strDescription.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 5, pTemp, &dwSize );
 			typelib->strDescription.ReleaseBuffer();
 
 			dwSize = 255;
 			pTemp = typelib->strDirectory.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 6, pTemp, &dwSize );
 			typelib->strDirectory.ReleaseBuffer();
 
 			dwSize = 255;
 			pTemp = typelib->strFeature.GetBuffer( dwSize );
 			MsiRecordGetString( hRecord, 7, pTemp, &dwSize );
 			typelib->strFeature.ReleaseBuffer();
 
 			typelib->nCost = MsiRecordGetInteger (hRecord,8);
 	}
 
 		// Close our handle
 		if( hRecord )
 			MsiCloseHandle( hRecord );
 	}
 	else
 		nRetVal = ERROR_INVALID_HANDLE;
 	
 	return nRetVal;
 }
 UINT CMSIWrapper::CloseTypeLibTable()
 {
 	return CloseTable( m_TypeLibView );
 }
 UINT CMSIWrapper::OpenPackage(LPCSTR szPackagePath)
 {
	UINT nRetVal = ERROR_SUCCESS;

	nRetVal = MsiOpenPackage(szPackagePath, &m_hInstall);
	if (nRetVal !=ERROR_SUCCESS)
	{
		return nRetVal;
	}

	if(MsiDoAction(m_hInstall, _T("CostInitialize"))==ERROR_SUCCESS  
        && MsiDoAction(m_hInstall, _T("FileCost"))==ERROR_SUCCESS  
        && MsiDoAction(m_hInstall, _T("CostFinalize"))==ERROR_SUCCESS)   
	{
		return ERROR_SUCCESS;		
	}else
	{
		return ERROR_FUNCTION_FAILED;
	}

	
 }
 UINT CMSIWrapper::ClosePackage()
 {
	UINT nRetVal = ERROR_SUCCESS;
	if(m_hInstall)
		nRetVal = MsiCloseHandle( m_hInstall);
	return nRetVal;
 }
 UINT CMSIWrapper::GetTargetPath(LPCSTR szName, CString *path )
 {
	//MSIHANDLE hInstall;
	TCHAR *szBuf;
	DWORD cch  = 0 ;
	UINT ret = ERROR_SUCCESS;

	TCHAR* szValueBuf = NULL;
	DWORD cchValueBuf = 0;
	UINT uiStat =  MsiGetProperty(m_hInstall, szName, _T(""), &cchValueBuf);
	if (ERROR_MORE_DATA == uiStat)
	{
		++cchValueBuf; // on output does not include terminating null, so add 1
		szValueBuf = new TCHAR[cchValueBuf];
		if (szValueBuf)
		{
			uiStat = MsiGetProperty(m_hInstall, szName, szValueBuf, &cchValueBuf);
		}
	}
	if (ERROR_SUCCESS != uiStat)
	{
		if (szValueBuf != NULL) 
		delete [] szValueBuf;
		return uiStat;
	}
		
	path->SetString(szValueBuf);
	delete [] szValueBuf;

		
	return ERROR_SUCCESS;

 }
