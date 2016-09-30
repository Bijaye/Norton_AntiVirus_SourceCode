// AVScanner.cpp: implementation of the CAVScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVScanner.h"
#include "itemdata.h"
#include "qscommon.h"
#include "FileNameParse.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HNAVENGINE  CAVScanner::m_hEngine = NULL;
CDefUtils   CAVScanner::defUtils;
CString     CAVScanner::m_sVirusDefs;

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
        NAVEngineClose( m_hEngine );
        m_hEngine = NULL;
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
    char szDefsPath[ MAX_PATH ];

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

    // 
    // Initialize NAVAPI
    // 
    NAVSTATUS status;
    m_hEngine = NAVEngineInit( szDefsPath,
                               NULL,
                               (LPNAVFILEIO) (m_bMemoryScan ? (LPNAVFILEIO)m_MemCallbacks : NULL),
                               3,
                               0,
                               &status );
    // 
    // Translate status
    // 
    switch( status )
        {
        case NAV_OK:
            break;
        case NAV_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case NAV_INVALID_ARG:
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
    NAVSTATUS status;
    HNAVVIRUS hVirus = NULL;
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
    status = NAVScanFile( m_hEngine,
                          T2A( pszFileName ),
                          pszExtA,
                          FALSE,
                          (HNAVVIRUS FAR *) &hVirus );

    }

    // 
    // Translate status
    // 
    switch( status )
        {
        case NAV_OK:
            *pVirus = hVirus;
            break;
        case NAV_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case NAV_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case NAV_NO_ACCESS:
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
    NAVSTATUS status;

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
    status = NAVDeleteFile( m_hEngine,
                            T2A( pszFileName ),
                            pszExtA
                            );
    }


    // 
    // Translate status
    // 
    switch( status )
        {
        case NAV_OK:
            break;
        case NAV_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case NAV_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case NAV_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        case NAV_CANT_DELETE:
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
    NAVSTATUS status;

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
    status = NAVRepairFile( m_hEngine,
                            T2A( pszFileName ),
                            pszExtA
                            );
    }


    // 
    // Translate status
    // 
    switch( status )
        {
        case NAV_OK:
            break;
        case NAV_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case NAV_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case NAV_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        case NAV_CANT_REPAIR:
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
    NAVSTATUS status;
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
    status = NAVGetVirusInfo( (HNAVVIRUS) pVirus,
                      NAV_VI_VIRUS_NAME,
                      szBuffer,
                      &dwBufferSize );
    // 
    // Translate status
    // 
    switch( status )
        {
        case NAV_OK:
            _tcsncpy( szVirusName, A2T( szBuffer ), nBufferLen );
            break;
        case NAV_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case NAV_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case NAV_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        case NAV_CANT_REPAIR:
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
    NAVSTATUS status;
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
    status = NAVGetVirusInfo( (HNAVVIRUS) pVirus,
                      NAV_VI_VIRUS_ID,
                      szBuffer,
                      &dwBufferSize );
    // 
    // Translate status
    // 
    switch( status )
        {
        case NAV_OK:
            dwVirusID = atol( szBuffer );
            break;
        case NAV_MEMORY_ERROR:
            hr = E_OUTOFMEMORY;
            break;
        case NAV_INVALID_ARG:
            hr = E_INVALIDARG;
            break;
        case NAV_NO_ACCESS:
            hr = E_ACCESSDENIED;
            break;
        case NAV_CANT_REPAIR:
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
    NAVSTATUS status;

    // 
    // Make sure we are initialized.
    // 
    if( m_hEngine == NULL )
        return E_FAIL;

    status = NAVFreeVirusHandle( (HNAVVIRUS) pVirus );

    // 
    // Translate status
    // 
    switch( status )
        {
        case NAV_OK:
            break;
        case NAV_INVALID_ARG:
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
    *pVirusDef = NAVLoadVirusDef( m_hEngine, dwVirusID );

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
                         NAVVIRINFO enumVirInfo,
                         LPSTR       lpszResultBuffer,
                         LPDWORD      lpdwBufferSize )
{
    return NAVGetVirusDefInfo( (HNAVVIRUSDEF) pVirusDef, 
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
    
    NAVReleaseVirusDef( (HNAVVIRUSDEF) pVirusDef ); 
    return S_OK;
}

