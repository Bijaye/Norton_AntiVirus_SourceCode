// ServiceConfig.cpp: implementation of the CServiceConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "const.h"
#include "ServiceConfig.h"

#include "NAVSettingsHelperEx.h"

#include "actions.h"
#include "optnames.h"
#include "navinfo.h"
#include "navversion.h"
#include "AutoProtectWrapper.h" // For CAutoProtectOptions
#include "cctrace.h"
#include "ccScanInterface.h"



//////////////////////////////////////////////////////////////////////
// CServiceConfig::CServiceConfig()

CServiceConfig::CServiceConfig() :
	m_bStartupScan( false ),
	m_bShutdownScan( false ),
	m_bScanBootRecords( true ),      
	m_bScanMasterBootRecords( true ),
	m_bAttemptRepair( false ),
	m_bLog( true ),
	m_dwBloodhoundLevel(2),
    m_bLoadAtStartup( true ),
    m_bDetectSpyware( false ),
    m_bFeatureEnabled(false)
{
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::~CServiceConfig()

CServiceConfig::~CServiceConfig()
{
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::Load()

void CServiceConfig::Load()
{
	// Claim exclusive access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( true );

	DWORD dwData;
	SAVRT_OPTS_STATUS rtStatus;  

	// Load the realtime options.
    CAutoProtectOptions APOptions;
    CCTRACEI( _T("CServiceConfig::Load() - loading savrt options")); 
    rtStatus = APOptions.Load();
	if ( rtStatus != SAVRT_OPTS_OK )
	{
		// Couldn't load options file.  OK with us.  We'll just use the default values.
		CCTRACEE( _T("Could not load options file. Using default values. Error Code: %d"), rtStatus ); 
		return;
	}

	// Get the realtime options

	//
	// Repair flag
	//

    // Set appropriate default before reading in the first main file action to take
    dwData = MAINACTION_REPAIR; // Repair
	APOptions.GetDwordValue( AP_FileAction1, &dwData, MAINACTION_REPAIR );

	switch( dwData )
	{
	case MAINACTION_REPAIR:
	case MAINACTION_DELETE:
		m_bAttemptRepair = true;
		break;
	default:
		m_bAttemptRepair = false;
	}

    //
    // Savrt defs path
    //
    CHAR szDefsPath[MAX_PATH] = {0};
    if (SAVRT_OPTS_OK != APOptions.GetStringValue(AP_VirusDefinitionsPath, szDefsPath, MAX_PATH, ""))
        CCTRACEE ("No virus defs path!!");

    m_sDefsPath = szDefsPath;
    CCTRACEI(_T("The definitions path read from savrt.dat is: %hs"), szDefsPath);

    //
	// Load AP at startup flag.
	//
	if ( SAVRT_OPTS_OK != APOptions.GetDwordValue( STARTUP_LoadVxD, &dwData, 1 ))
        CCTRACEE ("Failed to read the STARTUP_LoadVxD option");
	m_bLoadAtStartup = ( dwData != 0 );

	//
	// Scan Boot records flag.
	//
	APOptions.GetDwordValue( TSR_ScanBootRecs, &dwData, 1 );
	m_bScanBootRecords = ( dwData != 0 );

	//
	// Scan Master Boot records flag.
	//
	APOptions.GetDwordValue( TSR_ScanMasterBoot, &dwData, 1 );
	m_bScanMasterBootRecords = ( dwData != 0 );

	//
	// Shutdown scan of floppy drives flag.
	//
	APOptions.GetDwordValue( TSR_ChkFlopOnBoot, &dwData, 1 );
	m_bShutdownScan = ( dwData != 0 );

	//
	// Bloodhound level
	//
	APOptions.GetDwordValue( TSR_EnableHeuristicScan, &dwData, 1 );
	if( dwData )
		APOptions.GetDwordValue( TSR_HeuristicLevel, &m_dwBloodhoundLevel, 2 );
	else
		m_dwBloodhoundLevel = 0;

	// Get the options from the NAV options file

	// Load the NAV options file
    CCTRACEI( _T("CServiceConfig::Load() - loading nav options")); 
	CNAVOptSettingsEx NavOpts;
	HRESULT status = S_OK;
	if( !NavOpts.Init() )
	{
		CCTRACEE(_T("CServiceConfig::Load() - Could not load the options. Using default values.")); 
		return;
	}

	//
	// Startup scan of boot records flag.
	//
	NavOpts.GetValue(SCANNER_BootRecs, dwData, 1 );
	m_bStartupScan = ( dwData != 0 );

	//
	// Activity log stuff.
	//
	NavOpts.GetValue(ACTIVITY_LogKnown, dwData, 0 ); 
	m_bLog = (dwData != 0);

	//
	// Container scanning - blocked app alerts. Default to ON
	//
	NavOpts.GetValue(APCOMPRESSED_BlockedAppUI, dwData, 1 ); 
	m_bShowBlockedApp = (dwData != 0);

    // Licensing value - default to OFF of course.
    //
    NavOpts.GetValue(LICENSE_FeatureEnabled, dwData, 0);
    m_bFeatureEnabled = (dwData != 0);

    CCTRACEI( _T("CServiceConfig::Load() - getting other settings")); 

	// Get the computer name
	DWORD dwSize =  MAX_COMPUTERNAME_LENGTH + 1;
    std::vector<TCHAR> szBuffer( dwSize );

    if (!::GetComputerName( &szBuffer[0], &dwSize ))
    {
	    if ( GetLastError() == ERROR_BUFFER_OVERFLOW )
        {
            CCTRACEW( _T("CServiceConfig::Load() - overflow in GetComputerName %d"), dwSize); 
            szBuffer.resize( ++dwSize );
            if ( !::GetComputerName( &szBuffer[0], &dwSize ))
                CCTRACEE( _T("CServiceConfig::Load() - failed GetComputerName %d"), dwSize); 
        }
        else
            CCTRACEE( _T("CServiceConfig::Load() - failed GetComputerName %d"), ::GetLastError()); 
    }

	m_sComputerName = &szBuffer[0];

    // Spyware
	//
	if ( SAVRT_OPTS_OK != APOptions.GetDwordValue( AP_szNAVAPCFGdwRespondToThreats, &dwData, 1 ))
        CCTRACEE (_T("Unable to read the THREAT_RealTime value"));

    m_bDetectSpyware = ( dwData & SAVRT_THREAT_MODE_NONVIRAL )?true:false;

	// get the product version
	CNAVVersion NAVVersion;
	m_sProductVersion = NAVVersion.GetPublicRevision();
    CCTRACEI( _T("CServiceConfig::Load() - done"));

    // Test defs
    //
    char szTestDefsPath[MAX_PATH*2] = {0};
	if ( SUCCEEDED (NavOpts.GetValue(AUTOPROTECT_TestDefs, szTestDefsPath, sizeof(szTestDefsPath), "\0" )))
        m_strTestDefsPath = szTestDefsPath;
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetStartupScan()

bool CServiceConfig::GetStartupScan()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_bStartupScan;
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetShutdownScan()

bool CServiceConfig::GetShutdownScan()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_bShutdownScan;
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetScanBootRecords()

bool CServiceConfig::GetScanBootRecords()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_bScanBootRecords;
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetStartupScan()

bool CServiceConfig::GetScanMasterBootRecords()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_bScanMasterBootRecords;
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetStartupScan()

bool CServiceConfig::GetAttemptRepair()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_bAttemptRepair;
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetBloodhoundLevel()

DWORD CServiceConfig::GetBloodhoundLevel()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_dwBloodhoundLevel;
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetComputerName()

LPCTSTR CServiceConfig::GetComputerName()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );
																 
	return m_sComputerName.c_str();
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetShouldLog()

bool CServiceConfig::GetShouldLog()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_bLog;
}

bool CServiceConfig::GetLoadAtStartup()
{
    // Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_bLoadAtStartup;
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetShowBlockedAppAlerts()

bool CServiceConfig::GetShowBlockedAppAlerts()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

	return m_bShowBlockedApp;
}

LPCTSTR CServiceConfig::GetDefsRevision ()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

    return m_sDefsRevision.c_str();
}

bool CServiceConfig::SetDefsRevision (LPCTSTR lpszRevision)
{
	if ( !lpszRevision )
        return false;

    // Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );
    
    m_sDefsRevision = lpszRevision;

    return true;
}

LPCTSTR CServiceConfig::GetProductVersion()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

    return m_sProductVersion.c_str();
}

bool CServiceConfig::UpdateSavrtDefsPath ( LPCSTR lpszDefPath )
{
    // If it's the same path as before, there's nothing to do
    std::basic_string<char> sNewDefsPath = lpszDefPath;
    if( 0 == m_sDefsPath.compare( sNewDefsPath ) )
    {
        CCTRACEI(_T("CServiceConfig::UpdateSavrtDefsPath - The definitions path has not changed."));
        return true;
    }

    CCTRACEI(_T("CServiceConfig::UpdateSavrtDefsPath - The definitions path has changed to: %hs"),lpszDefPath);

    SAVRT_OPTS_STATUS rtStatus;  

	// Load the realtime options.  
    CAutoProtectOptions APOptions;
    rtStatus = APOptions.Load();
	if ( rtStatus != SAVRT_OPTS_OK )
	{
		// Couldn't load options file.
		CCTRACEE( _T("Could not load options file. Error Code: %d"), rtStatus ); 
		return false;
	}

    rtStatus = APOptions.SetStringValue(AP_VirusDefinitionsPath, lpszDefPath);
    if( SAVRT_OPTS_OK != rtStatus )
    {
        CCTRACEE( _T("Failed to set the SAVRT virus definitions path. Error Code: %d"), rtStatus );
        return false;
    }

    rtStatus = APOptions.Save();
    if( SAVRT_OPTS_OK != rtStatus )
    {
        CCTRACEE( _T("Failed to save the SAVRT virus definitions path. Error Code: %d"), rtStatus );
        return false;
    }

    // Save updated path
    m_sDefsPath = lpszDefPath;

    return true;
}

LPCSTR CServiceConfig::GetCurrentDefsPath()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );
    return m_sDefsPath.c_str();
}


bool CServiceConfig::GetDetectSpyware()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );
    return m_bDetectSpyware;
}

LPCSTR CServiceConfig::GetTestDefsPath()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );
    return m_strTestDefsPath.c_str();
}

bool CServiceConfig::GetFeatureEnabled()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );
    return m_bFeatureEnabled;
}
