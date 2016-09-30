// ServiceConfig.cpp: implementation of the CServiceConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "const.h"
#include "ServiceConfig.h"

#include "StahlSoft.h"
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

#include "NAVSettingsHelperEx.h"

#include "apoptnames.h"
#include "actions.h"
#include "optnames.h"
#include "navinfo.h"
#include "navversion.h"
#include "savrt32.h"
#include "cctrace.h"
#include "ccScanInterface.h"

//////////////////////////////////////////////////////////////////////
// CServiceConfig::CServiceConfig()

CServiceConfig::CServiceConfig() :
	m_bAttemptRepair( false ),
	m_dwBloodhoundLevel(2)
{
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::~CServiceConfig()

CServiceConfig::~CServiceConfig()
{
	// Intentionally empty.
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
    CAutoProtectOptions APOptions;

	// Load the realtime options.
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
	// Bloodhound level
	//
	APOptions.GetDwordValue( TSR_EnableHeuristicScan, &dwData, 1 );
	if( dwData )
		APOptions.GetDwordValue( TSR_HeuristicLevel, &m_dwBloodhoundLevel, 2 );
	else
		m_dwBloodhoundLevel = 0;

	// get the product version
	CNAVVersion NAVVersion;
	m_sProductVersion = NAVVersion.GetPublicRevision();
    CCTRACEI( _T("CServiceConfig::Load() - done"));
}

//////////////////////////////////////////////////////////////////////
// CServiceConfig::GetAttemptRepair()

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

LPCTSTR CServiceConfig::GetProductVersion()
{
	// Claim shared access to data.
	CReadWriteLock lock( m_Sync );
	lock.Lock( false );

    return m_sProductVersion.c_str();
}