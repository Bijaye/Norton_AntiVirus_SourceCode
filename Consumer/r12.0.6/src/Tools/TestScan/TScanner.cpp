// Scanner.cpp: implementation of the CScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccCommonClientInfo.h"

#define INITIIDS
#include "TScanner.h"
#undef INITIIDS

// Object map for syminterface

SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()

   
/////////////////////////////////////////////////////////////////////////////
// CTScanner::CTScanner()

CTScanner::CTScanner( ) :
	m_hNavScan( NULL ),
	m_pScanner( NULL ),
    m_bDecomposers( false ),
    m_bThreatCat( false ),
    m_bScanMem( false ),
    m_bBootRec( false )
{
	// Intentionally empty.
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::~CScanner()

CTScanner::~CTScanner()
{
	// Cleanup
	if( m_pScanner )
	{
		m_pScanner->Release();
	}

	// Unload CCSCAN.DLL
	if( m_hNavScan )
	{
		FreeLibrary( m_hNavScan );
		m_hNavScan = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::Initialize()

HRESULT CTScanner::Initialize(char* szAppID)
{

	char szTempDir[ _MAX_PATH ];
	GetTempPathA( _MAX_PATH, szTempDir );
	szTempDir[ strlen( szTempDir ) -1 ] = 0;
	
	try
	{
        // Build path to ccscan DLL.
        cc::CCommonClientInfo CCInfo;
        DWORD dwPathSize = MAX_PATH;
        TCHAR szScannerPath [MAX_PATH] = {0};
        CCInfo.GetCCDir ( szScannerPath, dwPathSize );
        _tcscat ( szScannerPath, _T("\\ccscan.dll"));

        // If we file api's are OEM we need to convert the path to OEM
        if( !AreFileApisANSI() )
            CharToOemBuff(szScannerPath, szScannerPath, MAX_PATH);

        // Load scanning library.
		m_hNavScan = LoadLibraryEx( szScannerPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
		if( m_hNavScan == NULL )
			throw runtime_error("Error loading ccSCAN.DLL.");
		
		// Get pointer to entry point.
		pfnSCANGETSCANNER pfnScanGetScanner = (pfnSCANGETSCANNER) GetProcAddress( m_hNavScan, "ScanGetScanner" );
		if( pfnScanGetScanner == NULL )
			throw runtime_error("Could not find entry point in ccSCAN.DLL.");
		
		// Get scanner interface.
		if( SCAN_OK != pfnScanGetScanner( IID_Scanner, (void**) &m_pScanner ) )
			throw runtime_error("Could not create scanner object.");
		
		// Finally, initialize the scanner.
		if( SCAN_OK != m_pScanner->Initialize( szAppID,
			"", 
			szTempDir, 
			3,
			0 ) )
		{
			m_pScanner->Release();
			m_pScanner = NULL;
			throw runtime_error("Failed to initialize scanner object.");	
		}
	}
	catch(...)
	{
		// Free resource.
		if(m_hNavScan)
		{
			FreeLibrary( m_hNavScan );
			m_hNavScan = NULL;
		}
		
		return E_FAIL;
	}

	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnBusy()

SCANSTATUS CTScanner::OnBusy()
{
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnError()

SCANSTATUS CTScanner::OnError( IScanError* pError )
{
	UNREFERENCED_PARAMETER(pError);
	return SCAN_ABORT;	
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnNewItem()

SCANSTATUS CTScanner::OnNewItem( const char * pszItem )
{
	UNREFERENCED_PARAMETER(pszItem);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnNewFile()

SCANSTATUS CTScanner::OnNewFile( const char * pszLongName, const char * pszShortName )
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnNewDirectory()

SCANSTATUS CTScanner::OnNewDirectory( const char * pszLongName, const char * pszShortName )
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnInfectionFound()

SCANSTATUS CTScanner::OnInfectionFound( IScanInfection* pInfection )
{
	UNREFERENCED_PARAMETER(pInfection);

	// We do not want to repair
	return SCAN_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnRepairFailed()

SCANSTATUS CTScanner::OnRepairFailed( IScanInfection* pInfection )
{
	UNREFERENCED_PARAMETER(pInfection);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnRemoveMimeComponent()

SCANSTATUS CTScanner::OnRemoveMimeComponent( IScanInfection* pInfection, char* pszMessage, 
										  int iMessageBufferSize )
{
	UNREFERENCED_PARAMETER(pInfection);
	UNREFERENCED_PARAMETER(pszMessage);
	UNREFERENCED_PARAMETER(iMessageBufferSize);
	return SCAN_DELETE;	
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::LockVolume()

SCANSTATUS CTScanner::LockVolume( const char* szVolume, bool bLock )
{
	UNREFERENCED_PARAMETER(bLock);
	UNREFERENCED_PARAMETER(szVolume);
	return SCAN_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::OnCleanFile()

SCANSTATUS CTScanner::OnCleanFile( const char * pszFileName, const char * pszTempFileName )
{
    UNREFERENCED_PARAMETER(pszFileName);
    UNREFERENCED_PARAMETER(pszTempFileName);
    return SCAN_OK;
}
/////////////////////////////////////////////////////////////////////////////
// CTScanner::LockVolume()

int CTScanner::GetProperty( const char* szProperty, int iDefault )
{
	if( strcmp( szProperty, SCAN_FILES ) == 0 )
		return 1; // Scan files.

	if( strcmp( szProperty, SCAN_BOOT_RECORDS ) == 0 )
		return ( m_bBootRec == true ? 1 : 0 ); 

	if( strcmp( szProperty, SCAN_MASTER_BOOT_RECORDS ) == 0 )
		return ( m_bBootRec == true ? 1 : 0 );

	if( strcmp( szProperty, SCAN_COMPRESSED_FILES ) == 0 )
        return ( m_bDecomposers == true ? 1 : 0 );

    if (strcmp(szProperty, SCAN_NON_VIRAL_THREATS) == 0)
        return ( m_bThreatCat == true ? 1 : 0 );

    if (strcmp(szProperty, SCAN_MEMORY) == 0)
        return ( m_bScanMem == true ? 1 : 0 );

	return iDefault;
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::GetProperity()

const char * CTScanner::GetProperty( const char* szProperty, const char * szDefault )
{
	return szDefault;
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::GetScanItemCount()

int CTScanner::GetScanItemCount()
{
	return m_ItemsToScan.size();
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::GetScanItemPath( int iIndex )

const char * CTScanner::GetScanItemPath( int iIndex )
{
	return m_ItemsToScan.at(iIndex).c_str();
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::RepairInfection(IScanInfection* pInfection)

SCANSTATUS CTScanner::RepairInfection(IScanInfection* pInfection)
{
	return m_pScanner->RepairInfection(pInfection);
}

void CTScanner::ProccessCmdLine()
{
    for (int iIndex = 1; iIndex < __argc; iIndex++)
	{
		// If the argument starts with a forward-slash it's a switch.

		if (__argv[iIndex][0] == _T('/'))
		{
			ProcessSwitch(__argv[iIndex]);
		}
        else
        {
            // Add it to the list of items to scan
            m_ItemsToScan.push_back(__argv[iIndex]);
        }
    }
}

void CTScanner::ProcessSwitch(const char* pszSwitch)
{
    TCHAR szSwitch[MAX_PATH] = {0};
    _tcscpy(szSwitch, pszSwitch);
    _tcslwr(szSwitch);
    // Check for the use decomposers switch
	if (_tcsicmp(szSwitch, "/usedec") == 0)
	{
		m_bDecomposers = true;
	}

    // Check for the use threat cat switch
	else if (_tcsicmp(szSwitch, "/usethreatcat") == 0)
	{
		m_bThreatCat = true;
	}

    else if (_tcsicmp(szSwitch, "/mem") == 0)
    {
        m_bScanMem = true;
    }

    else if (_tcsicmp(szSwitch, "/br") == 0)
    {
        m_bBootRec = true;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CTScanner::Scan()

SCANSTATUS CTScanner::Scan()
{
    //IScanInfection* pInfection = NULL;
	IScanResults* pIScanResults = NULL;
	if(SCAN_OK != m_pScanner->Scan(this, this, this, &pIScanResults))
	{
		pIScanResults->Release();
		return E_FAIL;
	}

	/* Check for a Null results object
	if(pIScanResults == NULL)
	{
		pIScanResults->Release();
		return E_FAIL;
	}

	// Now see if we found an infection
	if(0 == pIScanResults->GetTotalInfectionCount())
	{
		pIScanResults->Release();
		return SCAN_OK;
	}

	// So we found an infection, just get this infection and return
	else
	{
		HRESULT retVal = pIScanResults->GetInfection(0, &pInfection);
		pIScanResults->Release();
		return retVal;
	}*/
}