/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// AVScanner.cpp: implementation of the CAVScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qserver.h"
#include "AVScanner.h"
#include "qscommon.h"
#include "FileNameParse.h"
#include "resource.h"
#include "vpstrutils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HLEGENGINE  CAVScanner::m_hEngine = NULL;
//CDefUtils   CAVScanner::defUtils;
CString     CAVScanner::m_sVirusDefs;
BOOL		CAVScanner::m_bVirusDBInfoLoaded=FALSE;
LEGVIRUSDBINFOEX CAVScanner::m_NavVirusDBInfo={0};
HLEGVIRUSDEFTABLE CAVScanner::m_hNavVDefTable=NULL;
DWORD		CAVScanner::m_dwNoVDefInTable=0;
int			CAVScanner::m_iNoInstances=0;
TCHAR		CAVScanner::m_szPathToDefs [MAX_PATH]={0};
TCHAR		CAVScanner::m_szDef[MAX_PATH]={0};

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
	m_iNoInstances++;
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
	// added by terrym 6-27-01
	m_iNoInstances--;
	if (m_iNoInstances==0)
	{
		//release the virus def table if loaded
		if (m_hNavVDefTable!=NULL)
		{
			m_LegacyIntfPtr->pVTable->ReleaseVirusDefTable(m_hNavVDefTable);
			m_hNavVDefTable = NULL;
		}
    
		// 
		// Shutdown NAVAPI32
		// 
		if( m_hEngine != NULL )
		{
			m_LegacyIntfPtr->pVTable->EngineClose( m_hEngine );
			m_hEngine = NULL;
		}
		UnloadECOM();
		m_bVirusDBInfoLoaded = FALSE;
		memset (&m_NavVirusDBInfo,0,sizeof(m_NavVirusDBInfo));
	}
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
//    char szDefsPath[ MAX_PATH ];
	DWORD dwSize = MAX_PATH;

	ECOM_RESULT ecResult;

    // 
    // If the engine has already been initialized, we're all done.
    // 
    if( m_hEngine != NULL )
        return S_OK;

    // 
    // Initialize Hawkings stuff
    // 
//    if( FALSE == defUtils.InitWindowsApp( QSCONSOLE_HAWKINGS_ID ) )
//        return E_FAIL;

//    if( FALSE == defUtils.UseNewestDefs() )
//        return E_FAIL;

//    if( FALSE == defUtils.UseNewestDefs( szDefsPath, MAX_PATH ) )
//        return E_FAIL;

	CRegKey reg;
    if( ERROR_SUCCESS == reg.Create( HKEY_LOCAL_MACHINE, REGKEY_AVISSERVER, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS) )
	{
		LONG lRc1, lRc2;
		lRc1 = reg.QueryStringValue( REGVALUE_DEFLIBRARY_FOLDER, m_szPathToDefs , &dwSize);
		lRc2 = reg.QueryStringValue( REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM, m_szDef, &dwSize);
		if (lRc1 != ERROR_SUCCESS || lRc2 != ERROR_SUCCESS)
		{
			hr = E_PENDING;
			return hr;
		}
	}
	DWORD dwSeq = 0;
	dwSeq = atoi(T2A(m_szDef));
	if (dwSeq == 0)
		return E_PENDING;
	TCHAR		buffer[64];
	_stprintf(buffer, _T("\\%08d"), dwSeq);

	CString sDefsPath = m_szPathToDefs;
	sDefsPath += buffer;
//	sDefsPath.Replace(_T("Program Files"),_T("Progra~1"));
//	strcpy (szDefsPath, "C:\\Progra~1\\symantec\\quarantine\\server\\signatures\\00013543.");

    // 
    // Save off defs path.
    // 
    m_sVirusDefs = sDefsPath ;

	ecResult = ECOMInitialize(sDefsPath, &m_CoreIntfPtr);
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
    m_hEngine = m_LegacyIntfPtr->pVTable->EngineInit(m_LegacyIntfPtr, T2A(sDefsPath) ,
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
HRESULT CAVScanner::GetVirusDefsDate( SYSTEMTIME* pst,DWORD *pdwRev )
{
    HRESULT hr = S_OK;
	CString sTemp;
	USES_CONVERSION;



	if (pst == NULL || pdwRev == NULL)
		return E_INVALIDARG;

    ZeroMemory( pst, sizeof( SYSTEMTIME ) );
	
	// 
    // Make sure we are initialized.
    // 
	if (m_bVirusDBInfoLoaded == FALSE)
	{
		if ( S_OK == GetVirusDBInfo())
		{
			hr = S_OK;
		}
							
	}


	if (hr == S_OK)
	{
		pst->wYear = m_NavVirusDBInfo.wYear;
		pst->wMonth = m_NavVirusDBInfo.wMonth;
		pst->wDay = m_NavVirusDBInfo.wDay;
		*pdwRev  = m_NavVirusDBInfo.dwVersion;


	}

    


    return hr;
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


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::GetVirusDBInfo
//
// Description  : This function will get the virus db info from navapi and store
//				 it in a static member variable
//
// Return type  : HRESULT 
//
// Argument     : void
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::GetVirusDBInfo( void)
{
    HRESULT hr = E_FAIL;
	// 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return hr;
	if (m_bVirusDBInfoLoaded == FALSE)
	{
		m_NavVirusDBInfo.dwSize = sizeof(m_NavVirusDBInfo);
		if ( LEG_OK == m_LegacyIntfPtr->pVTable->GetVirusDBInfoEx(
								m_hEngine,        // [in]  initialized NAV Engine handle.
								&m_NavVirusDBInfo // [out] database information structure.
								))
		{
			m_bVirusDBInfoLoaded = TRUE;
			hr = S_OK;
		}
							
	}
    return hr;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::GetVirusDBSequenceNumber
//
// Description  : This function will get the virus db info from navapi and store
//				 it in a static member variable
//
// Return type  : HRESULT 
//
// Argument     : void
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::GetVirusDBSequenceNumber( DWORD *pdwSequence)
{
    HRESULT hr = S_OK;
	if (pdwSequence == NULL)
		return E_FAIL;
	// 
    // Make sure we are initialized.
    // 
	if (m_bVirusDBInfoLoaded == FALSE)
	{
		if ( S_OK != GetVirusDBInfo())
		{
			hr = E_FAIL;
		}
							
	}

	if (hr == S_OK)
		*pdwSequence = m_NavVirusDBInfo.dwSequence;

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::GetVirusDBSequenceNumber
//
// Description  : This function will get the virus db info from navapi and store
//				 it in a static member variable
//
// Return type  : HRESULT 
//
// Argument     : void
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::GetVirusDefVersionNumber( VARIANT *pV)
{
    HRESULT hr = S_OK;
	CString sTemp;
	char *cp;
	USES_CONVERSION;
	char szMonth[25], szDay[25], szYear[25], szVersion[25];

	if (pV == NULL)
		return E_FAIL;
	
	// 
    // Make sure we are initialized.
    // 
	if (m_bVirusDBInfoLoaded == FALSE)
	{
		if ( S_OK != GetVirusDBInfo())
		{
			hr = E_FAIL;
		}
							
	}

	DWORD dwVersion = m_NavVirusDBInfo.dwVersion;

	if (hr == S_OK)
	{
		itoa(m_NavVirusDBInfo.wYear-1971,szYear,10);
		itoa(m_NavVirusDBInfo.wMonth,szMonth,10);
		itoa(m_NavVirusDBInfo.wDay,szDay,10);
		cp = szVersion;
		if ( dwVersion > 26)
		{
			*cp ++				= (char)(dwVersion / 26 + 0x60);
			dwVersion		   %= 26;
		}

		*cp ++					= (char)(dwVersion + 0x60);
		*cp						= 0;
	

		sTemp.Format(_T("%s%s%s%s"),  A2T(szYear), A2T(szMonth), A2T(szDay), A2T(szVersion));
		
		VariantInit(pV);

		pV->vt = VT_BSTR;
		pV->bstrVal = sTemp.AllocSysString();

	}

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::GetVirusDBVersionNumber
//
// Description  : This function will get the virus db info from navapi and store
//				 it in a static member variable
//
// Return type  : HRESULT 
//
// Argument     : void
//
///////////////////////////////////////////////////////////////////////////////
// 06-26-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::GetVirusDBVersionString( VARIANT *pV)
{
    HRESULT hr = S_OK;
	char szTemp[MAX_PATH];
USES_CONVERSION;

	if (pV == NULL)
		return E_FAIL;

	hr = m_LegacyIntfPtr->pVTable->GetNavapiVersionString
							(m_LegacyIntfPtr,
							szTemp,           // [out] buffer to copy string into
							sizeof(szTemp)   // [in]  size of lpszVersionBuffer
							);


	if (hr == LEG_OK)
	{
		VariantInit (pV);
		pV->vt = VT_BSTR;
		pV->bstrVal = SysAllocStringLen( A2OLE( szTemp ), strlen( szTemp ) + 1 );
		hr = S_OK;
	}
	else
		hr = E_FAIL;
    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::LoadVirusTable
//
// Description  : This function will load the virus def table as store the number
//				  of virus defs in the table.
//
// Return type  : HRESULT 
//
// Argument     : void
//
///////////////////////////////////////////////////////////////////////////////
// 06-27-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::LoadVirusTable( void)
{
    HRESULT hr = E_FAIL;
	
	// check to see if already loaded
	if (m_hNavVDefTable != NULL)
		return S_OK;
	
	// load the virus def table
	m_hNavVDefTable = m_LegacyIntfPtr->pVTable->LoadVirusDefTable(m_hEngine);

	// alright check to see if it worked
	if (m_hNavVDefTable != NULL)
	{
		// it did now get the number of definitions in the table
		if (LEG_OK==m_LegacyIntfPtr->pVTable->GetVirusDefTableCount
			(
				m_hNavVDefTable ,// [in]  Handle to an open Virus Definition List.
				&m_dwNoVDefInTable// [out] Number of virus definitions in this table.
			))
		{
			// it worked so set the correct return code
			hr = S_OK;
		}
		else
		{
			// it didn't work so clear and release everything
			// there is no sense keeping it loaded if there are no
			// virus defs in the table.
			m_LegacyIntfPtr->pVTable->ReleaseVirusDefTable(m_hNavVDefTable);
			m_hNavVDefTable = NULL;	
			m_dwNoVDefInTable = 0;
		}
	}
	
    return hr;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CVirusPage::BuildInfectsString
//
// Description      : Build a string like (".COM files, .EXE files, etc" )
//                    Stolen from the VirusList project in AvCore
//
// Return type      : void BOOL
//
// Argument         : VLVIRUSENTRY hEntry - entry we are concerned with
// Argument         : CString&     sBuffer - output buffer
//
////////////////////////////////////////////////////////////////////////////
// 3/2/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CAVScanner::BuildInfectsString(
    DWORD  dwVirusDefIndex,
    CString&      sBuffer )
{

    static      UINT            uInfectsFilesString[] =
                                       {
                                       IDS_FILE,
                                       IDS_1FILE,
                                       IDS_2FILE,
                                       IDS_3FILE
                                       };

    static      UINT            uBootRecordString[] =
                                       {
                                       IDS_FILE,
                                       IDS_1BOOT,
                                       IDS_2BOOT,
                                       IDS_3BOOT
                                       };

    auto        UINT            uConditions;
    auto        UINT            uStringID;
    auto        char			szWork  [ MAX_PATH ];
    auto        BOOL            bBootType;
    auto        DWORD           dwBufferSize = MAX_PATH;

    CString aStrings[3];
    uConditions = 0;
	TCHAR szTemp[MAX_PATH];
    bBootType = !m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_INFECT_FILES, szWork, &dwBufferSize );

    if ( bBootType == TRUE )
        {
                                        // Boot Record viruses
        dwBufferSize = MAX_PATH;
        if ( m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_INFECT_FLOPPY_BOOT, szWork, &dwBufferSize ) )
		{
			LoadString(_Module.GetResourceInstance(),IDS_FLOPPY, szTemp, sizeof (szTemp));
			aStrings[uConditions++]=szTemp;
		}

        if ( m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_INFECT_HD_BOOT, szWork, &dwBufferSize ) )
		{
			LoadString(_Module.GetResourceInstance(),IDS_HARDDISK , szTemp, sizeof (szTemp));
			aStrings[uConditions++]=szTemp;
		}

        if ( m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_INFECT_MBR, szWork, &dwBufferSize ) )
		{
			LoadString(_Module.GetResourceInstance(),IDS_MASTER , szTemp, sizeof (szTemp));
			aStrings[uConditions++]=szTemp;
		}

        uStringID = uBootRecordString[uConditions];
        }
    else
        {
                                        // Program File viruses
        if ( m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_INFECT_COM, szWork, &dwBufferSize ) )
		{
			LoadString(_Module.GetResourceInstance(),IDS_COM, szTemp, sizeof (szTemp));
			aStrings[uConditions++]=szTemp;
		}

        if ( m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_INFECT_EXE, szWork, &dwBufferSize ) )
		{
			LoadString(_Module.GetResourceInstance(),IDS_EXE, szTemp, sizeof (szTemp));
			aStrings[uConditions++]=szTemp;
		}

        if ( m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_INFECT_SYS, szWork, &dwBufferSize ) )
		{
			LoadString(_Module.GetResourceInstance(),IDS_SYS, szTemp, sizeof (szTemp));
			aStrings[uConditions++]=szTemp;
		}

        if ( m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_MACRO_VIRUS, szWork, &dwBufferSize ) )
		{
			LoadString(_Module.GetResourceInstance(),IDS_MACRO, szTemp, sizeof (szTemp));
			aStrings[uConditions++]=szTemp;
		}

        if ( m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVirusDefIndex, LEG_VI_BOOL_AGENT_VIRUS, szWork, &dwBufferSize ) )
		{
			LoadString(_Module.GetResourceInstance(),IDS_MACRO, szTemp, sizeof (szTemp));
			aStrings[uConditions++]=szTemp;
		}

        uStringID = uInfectsFilesString[uConditions];
        }
	if ( uStringID )
        {
		LoadString(_Module.GetResourceInstance(),uStringID, szTemp, sizeof (szTemp));
        sBuffer.Format( szTemp,
                        aStrings[0],
                        aStrings[1],
                        aStrings[2]);
        }
    else
        sBuffer.Empty();
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CAVScanner::LoadVirusTable
//
// Description  : This function will load the virus def table as store the number
//				  of virus defs in the table.
//
// Return type  : HRESULT 
//
// Argument     : void
//
///////////////////////////////////////////////////////////////////////////////
// 06-27-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CAVScanner::GetVirusInfobyTableIndex( DWORD dwVIndex, VARIANT *pVariant )
{

USES_CONVERSION;

    HRESULT hr = E_FAIL;
    SAFEARRAYBOUND bounds;
	CString csWork;
	VARIANT v;
	LONG lField;
	char szTemp[MAX_PATH];
	TCHAR szTempString[MAX_PATH];
    DWORD dwBufferSize = sizeof(szTemp);

    // 
    // Create safearray for all elements.
    //     
    bounds.lLbound = 0;
    bounds.cElements = NO_VIRUS_INFO_FIELDS;
    SAFEARRAY* psa = SafeArrayCreate( VT_VARIANT, 1, &bounds );

    if( psa == NULL )
        {
        pVariant->vt = VT_ERROR;
        pVariant->scode = GetScode( E_OUTOFMEMORY );
        return E_OUTOFMEMORY;
        }

	memset(szTemp,0,sizeof(szTemp));
	// Get the virus name and store it in the Variant array
	VariantInit (&v);
	m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVIndex, LEG_VI_VIRUS_NAME, szTemp, &dwBufferSize );
    v.vt = VT_BSTR;
	v.bstrVal = SysAllocStringLen( A2W( szTemp ), strlen( szTemp ) + 1 );
	lField = VDF_NAME;
    hr = SafeArrayPutElement( psa, &lField, &v );

	// Get the virus infection string and store it in the Variant array
	memset(szTemp,0,sizeof(szTemp));
	VariantClear (&v);
	BuildInfectsString(dwVIndex,csWork);
    v.vt = VT_BSTR;
    v.bstrVal = csWork.AllocSysString();
	lField = VDF_INFECTS;
    hr = SafeArrayPutElement( psa, &lField, &v );
	
	// Get the string for if the virus is found in the wild and store it in the Variant array
	VariantClear (&v);

	memset(szTemp,0,sizeof(szTemp));
    LoadString(_Module.GetResourceInstance(),m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVIndex, LEG_VI_BOOL_INFECT_WILD, szTemp, &dwBufferSize) ? 
                              IDS_COMMON : IDS_RARE,szTempString,sizeof(szTempString));
	csWork=szTempString;
	v.vt = VT_BSTR;
    v.bstrVal = csWork.AllocSysString();
	lField = VDF_COMMON;
    hr = SafeArrayPutElement( psa, &lField, &v );

	// Get the string of info for the virus and store it in the Variant array
	memset(szTemp,0,sizeof(szTemp));
	VariantClear (&v);
	m_LegacyIntfPtr->pVTable->GetVirusDefTableInfo( m_hNavVDefTable, dwVIndex, LEG_VI_VIRUS_INFO, szTemp, &dwBufferSize );
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocStringLen( A2W( szTemp ), strlen( szTemp ) + 1 );
	lField = VDF_INFO;
    hr = SafeArrayPutElement( psa, &lField, &v );
    
    // 
    // Save off our variant.
    // 
    pVariant->vt = VT_ARRAY | VT_VARIANT;
    pVariant->parray = psa;


	return hr;
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
