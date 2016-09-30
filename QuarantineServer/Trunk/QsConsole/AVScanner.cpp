/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// AVScanner.cpp: implementation of the CAVScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVScanner.h"
#include "itemdata.h"
#include "qscommon.h"
#include "FileNameParse.h"
#include "vpstrutils.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HLEGENGINE  CAVScanner::m_hEngine = NULL;
CDefUtils   CAVScanner::defUtils;
CString     CAVScanner::m_sVirusDefs;

EILegacy*	CAVScanner::m_LegacyIntfPtr	= NULL;
EITCList*	CAVScanner::m_ThreatListIntfPtr = NULL;
EICore*		CAVScanner::m_CoreIntfPtr		= NULL;
EIFoundation*	CAVScanner::m_FoundationPtr = NULL;
DWORD		CAVScanner::m_dwDataSets		= DATASET_VIRAL | DATASET_NONVIRAL;

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::CAVScanner
//
// Description  : Constructor. 
//
// Return type  : 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CAVScanner::CAVScanner() :
m_bMemoryScan( FALSE )
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::~CAVScanner
//
// Description  : Destructor
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CAVScanner::~CAVScanner()
{
    // 
    // Shutdown NAVAPI32
    // 
    if( m_hEngine != NULL )
        {
		m_LegacyIntfPtr->pVTable->EngineClose( m_hEngine );
        m_hEngine = NULL;
        }

	UnloadECOM();

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::Initialize
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : LPCTSTR szVirusDefsPath /* = NULL */
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::Initialize()
{
USES_CONVERSION;
    
    HRESULT hr = S_OK;
    char szDefsPath[ MAX_PATH ];
	ECOM_RESULT ecResult;

    // 
    // If the engine has already been initialized, we're all done.
    // 
    if( m_hEngine != NULL )
        return S_OK;

    // 
    // Initialize Hawkings stuff
    // 
    if( FALSE == defUtils.InitWindowsApp( QSCONSOLE_HAWKINGS_ID ) )
        return E_FAIL;

    if( FALSE == defUtils.UseNewestDefs() )
        return E_FAIL;

    if( FALSE == defUtils.UseNewestDefs( szDefsPath, MAX_PATH ) )
        return E_FAIL;

    // 
    // Save off defs path.
    // 
    m_sVirusDefs = A2T( szDefsPath );

	ecResult = ECOMInitialize(m_sVirusDefs, &m_CoreIntfPtr);
	if (!IS_ECOM_RESULT_FAILURE(ecResult))
	{
		//create a SCANREPAIR ECOM object
		ecResult = m_CoreIntfPtr->pVTable->CreateObject(m_CoreIntfPtr, ECOM_OID_SCANREPAIR, &m_FoundationPtr);
		if (!IS_ECOM_RESULT_FAILURE(ecResult))
		{

			//obtain a legacy interface
			ecResult = m_FoundationPtr->pVTable->QueryInterface(m_FoundationPtr, ECOM_IID_LEGACY, (void**) &m_LegacyIntfPtr);

			if (!IS_ECOM_RESULT_FAILURE(ecResult))
			{
				hr = S_OK;
			}
			else
			{
				hr = E_FAIL;
			}
		}
		else
		{
			m_FoundationPtr = NULL;
			hr = E_FAIL;
		}
	}
	else
	{
		m_CoreIntfPtr = NULL;
		hr = E_FAIL;
	}
	if (!SUCCEEDED(hr))
	{
		// loading failed; perform clean up in case of partial loading
		UnloadECOM();
	}

    // 
    // Initialize NAVAPI
    // 
    LEGSTATUS status;
    m_hEngine = m_LegacyIntfPtr->pVTable->EngineInit(m_LegacyIntfPtr, szDefsPath,
                               NULL,
                               (LPLEGFILEIO) (m_bMemoryScan ? (LPLEGFILEIO)m_MemCallbacks : NULL),
                               3,
							   m_dwDataSets,
                               0,
                               &status );
    // 
    // Translate status
    // 
    switch( status )
        {
        case LEG_OK:
            break;
        case LEG_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case LEG_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        default:
            hr = E_FAIL;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::GetVirusDefsDate
//
// Description  : This routine will populate a SYSTEMTIME structure with date
//                information about the definitions set currently in use.
//
// Return type  : HRESULT 
//
// Argument     : SYSTEMTIME* pst
//
///////////////////////////////////////////////////////////////////////////////
// 3/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::GetVirusDefsDate( SYSTEMTIME* pst )
{
    if( m_hEngine == NULL )
        return E_FAIL;

    // 
    // Clear input.
    // 
    ZeroMemory( pst, sizeof( SYSTEMTIME ) );
    
    DWORD dwRev;
    if( FALSE == defUtils.GetCurrentDefsDate( &pst->wYear,
                                              &pst->wMonth,
                                              &pst->wDay,
                                              &dwRev ) )
        {
        return E_FAIL;
        }

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::ScanFile
//
// Description  : Scan file for known viruses.  
//
// Return type  : HRESULT 
//
// Argument     : LPCTSTR pszFileName
// Argument     : LPVOID* pVirus
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::ScanFile( LPCTSTR pszFileName, LPVOID* pVirus )
{
USES_CONVERSION;    
    HRESULT hr = S_OK;
    LEGSTATUS status;
    HLEGVIRUS hVirus = NULL;
    // 
    // Clear output
    // 
    *pVirus = NULL;

    // 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return E_FAIL;

    // 
    // Get extension for this file.
    // 
    TCHAR szExt[ MAX_PATH ] = { 0 };
    CFileNameParse::GetFileExtension( pszFileName, szExt, MAX_PATH );

    // 
    // Convert extension to ANSI.
    // 
    LPSTR pszExtA = T2A( szExt );

    // 
    // Scan the file.
    // 
    {
    USES_CONVERSION;
    status = m_LegacyIntfPtr->pVTable->ScanFile( m_hEngine,
                          T2A(const_cast<LPTSTR>( (LPCTSTR) pszFileName )),
                          pszExtA,
                          FALSE,
                          (HLEGVIRUS FAR *) &hVirus );

    }

    // 
    // Translate status
    // 
    switch( status )
        {
        case LEG_OK:
            *pVirus = hVirus;
            break;
        case LEG_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case LEG_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case LEG_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        default:
            hr = E_FAIL;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::DeleteFile
//
// Description  : Performs a safe delete of an infected file.
//
// Return type  : HRESULT 
//
// Argument     : LPCTSTR pszFileName
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::DeleteFile( LPCTSTR pszFileName )
{
USES_CONVERSION;    
    HRESULT hr = S_OK;
    LEGSTATUS status;

    // 
    // Get extension for this file.
    // 
    TCHAR szExt[ MAX_PATH ] = { 0 };
    CFileNameParse::GetFileExtension( pszFileName, szExt, MAX_PATH );

    // 
    // Convert extension to ANSI.
    // 
    LPSTR pszExtA = T2A( szExt );

    // 
    // Nuke the file.
    // 
    {
    USES_CONVERSION;
    status =m_LegacyIntfPtr->pVTable->DeleteFile( m_hEngine,
                            T2A( const_cast<LPTSTR>( (LPCTSTR)pszFileName )),
                            pszExtA
                            );
    }


    // 
    // Translate status
    // 
    switch( status )
        {
        case LEG_OK:
            break;
        case LEG_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case LEG_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case LEG_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        case LEG_CANT_DELETE:
            hr = ::DeleteFile( pszFileName ) ? S_OK : E_FAIL;
            break;
        default:
            hr = E_FAIL;
        }
    
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::RepairFile
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : LPCTSTR pszFileName
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::RepairFile( LPCTSTR pszFileName )
{
USES_CONVERSION;    
    HRESULT hr = S_OK;
    LEGSTATUS status;

    // 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return E_FAIL;

    // 
    // Get extension for this file.
    // 
    TCHAR szExt[ MAX_PATH ] = { 0 };
    CFileNameParse::GetFileExtension( pszFileName, szExt, MAX_PATH );

    // 
    // Convert extension to ANSI.
    // 
    LPSTR pszExtA = T2A( szExt );

    // 
    // Repair the file.
    // 
    {
    USES_CONVERSION;
    status = m_LegacyIntfPtr->pVTable->RepairFile( m_hEngine,
                            T2A( const_cast<LPTSTR>( (LPCTSTR)pszFileName )),
                            pszExtA
                            );
    }


    // 
    // Translate status
    // 
    switch( status )
        {
        case LEG_OK:
            break;
        case LEG_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case LEG_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case LEG_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        case LEG_CANT_REPAIR:
            hr = S_FALSE;
            break;
        default:
            hr = E_FAIL;
        }
    
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::GetVirusName
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : LPVOID pVirus
// Argument     : LPTSTR szVirusName
// Argument     : int nBufferLen
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::GetVirusName( LPVOID pVirus, LPTSTR szVirusName, int nBufferLen )
{
USES_CONVERSION;
    HRESULT hr = S_OK;
    LEGSTATUS status;
    char szBuffer[ MAX_PATH ];
    DWORD dwBufferSize = MAX_PATH;

    // 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return E_FAIL;

    // 
    // Fetch info.
    // 
    status = m_LegacyIntfPtr->pVTable->GetVirusInfo( (HLEGVIRUS) pVirus,
                      LEG_VI_VIRUS_NAME,
                      szBuffer,
                      &dwBufferSize );
    // 
    // Translate status
    // 
    switch( status )
        {
        case LEG_OK:
            vpstrncpy( szVirusName, A2T( szBuffer ), nBufferLen );
            break;
        case LEG_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case LEG_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case LEG_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        case LEG_CANT_REPAIR:
            hr = S_FALSE;
            break;
        default:
            hr = E_FAIL;
        }
    
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::GetVirusID
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     :  LPVOID pVirus
// Argument     : DWORD& dwVirusID
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::GetVirusID( LPVOID pVirus, DWORD& dwVirusID )
{
USES_CONVERSION;
    HRESULT hr = S_OK;
    LEGSTATUS status;
    DWORD dwBufferSize = 10;
    char szBuffer[10];

    // 
    // Clear input
    // 
    dwVirusID = 0;

    // 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return E_FAIL;

    // 
    // Fetch info.
    // 
    status = m_LegacyIntfPtr->pVTable->GetVirusInfo( (HLEGVIRUS) pVirus,
                      LEG_VI_VIRUS_ID,
                      szBuffer,
                      &dwBufferSize );
    // 
    // Translate status
    // 
    switch( status )
        {
        case LEG_OK:
            dwVirusID = atol( szBuffer );
            break;
        case LEG_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case LEG_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case LEG_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        case LEG_CANT_REPAIR:
            hr = S_FALSE;
            break;
        default:
            hr = E_FAIL;
        }
    
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::FreeVirus
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : LPVOID pVirus
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::FreeVirus( LPVOID pVirus )
{
    HRESULT hr = S_OK;
    LEGSTATUS status;

    // 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return E_FAIL;

    status = m_LegacyIntfPtr->pVTable->FreeVirusHandle( (HLEGVIRUS) pVirus );

    // 
    // Translate status
    // 
    switch( status )
        {
        case LEG_OK:
            break;
        case LEG_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        default:
            hr = E_FAIL;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::LoadVirusDef
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     :  DWORD dwVirusID
// Argument     : LPVOID* pVirusDef
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::LoadVirusDef( DWORD dwVirusID, LPVOID* pVirusDef )
{
    *pVirusDef = 0;    

    // 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return E_FAIL;

    // 
    // Get def info.
    // 
    *pVirusDef = m_LegacyIntfPtr->pVTable->LoadVirusDef( m_hEngine, dwVirusID );

    return *pVirusDef == NULL ? E_FAIL : S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::GetVirusDefInfo
//
// Description  : 
//
// Return type  : DWORD 
//
// Argument     : LPVOID pVirusDef
// Argument     : NAVVIRINFO enumVirInfo
// Argument     : LPSTR lpszResultBuffer
// Argument     : LPDWORD lpdwBufferSize
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CAVScanner::GetVirusDefInfo( LPVOID pVirusDef, 
                         LEGVIRINFO enumVirInfo,
                         LPSTR       lpszResultBuffer,
                         LPDWORD      lpdwBufferSize )
{
    return m_LegacyIntfPtr->pVTable->GetVirusDefInfo( (HLEGVIRUSDEF) pVirusDef, 
                               enumVirInfo, 
                               lpszResultBuffer,
                               lpdwBufferSize );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::FreeVirusDef
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : LPVOID pVirusDef
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::FreeVirusDef( LPVOID pVirusDef )
{
    // 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return E_FAIL;
    
    m_LegacyIntfPtr->pVTable->ReleaseVirusDef( (HLEGVIRUSDEF) pVirusDef ); 
    return S_OK;
}
// Releases ECOM interfaces and unloads the ECOM server
HRESULT CAVScanner::UnloadECOM()
{
	HRESULT hr = E_FAIL;
	ECOM_RESULT ecResult;

	unsigned long refCount;
	unsigned long nResCount;

	if (m_LegacyIntfPtr != NULL)
	{
		refCount = m_LegacyIntfPtr->pVTable->Release(m_LegacyIntfPtr);
		m_LegacyIntfPtr = NULL;
	}
	if (m_FoundationPtr != NULL)
	{
		refCount = m_FoundationPtr->pVTable->Release(m_FoundationPtr);
		m_FoundationPtr = NULL;
	}
	if (m_CoreIntfPtr != NULL)
	{
		refCount = m_CoreIntfPtr->pVTable->Release(m_CoreIntfPtr);
		m_CoreIntfPtr = NULL;
		m_FoundationPtr = NULL;
	}

	ecResult = ECOMReleaseUnusedResources(&nResCount);
	if (!IS_ECOM_RESULT_FAILURE(ecResult))
	{
		hr = S_OK;
	}
	return hr;
}

