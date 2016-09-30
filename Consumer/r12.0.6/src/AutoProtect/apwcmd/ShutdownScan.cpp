#include "StdAfx.h"
#include "shutdownscan.h"

#import "navapsvc.tlb" 					// For COM interfaces to service.
#include "navapcommands.h"				// Commands for service.
#include "apwntres.h"

#include "DetectMedia.h"
#include "AvEvents.h"
#include "AllNAVEvents.h"
#include "AvccModuleId.h"
#include "NAVEventFactoryLoader.h"

#include "EventQueue.h"

#include "const.h"
#include "Wtsapi32.h"                   // For NT terminal session
#include "NAVInfo.h"
#include "ccModule.h"			// for loading dlls


CShutdownScan::CShutdownScan(CApwNtCmd* pCmd) : m_pScanner(NULL)
{
    CCTRACEI ( _T("APAgent:CShutdownScan()"));
    m_pCmd = pCmd;
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
    m_Options.m_bPumpMessages = TRUE;
}

CShutdownScan::~CShutdownScan(void)
{
}

int CShutdownScan::Run ()
{
    CCTRACEI ( _T("APAgent:CShutdownScan::Run - starting"));

    // Only perform scan if AP is enabled.
    //
	if( !m_pCmd || m_pCmd->GetStatusOfDriver() != AP_STATE_ENABLED )
    {
        CCTRACEI ( _T("APAgent:CShutdownScan::Run - exiting1"));
		return 0;
    }

    // Have to load the configuration options so we know whether to attempt a
    // repair if the shutdown detects a boot infection...may want to consider
    // waiting till we actually get the infection before taking the overhead of
    // this load since from what I can tell it's only used in the case of an infection
    // detection.
    m_config.Load();

    CCTRACEI ( _T("APAgent:CShutdownScan::Run - calling service"));

	try
	{
		int             nRet;
		
        m_pScanner = new CSymStaticRefCount<CScanner>;

        if (!m_pScanner)
        {
            CCTRACEE ( "APAgent:CShutdownScan::Run - failed to new scanner" );
            return -1;
        }

        // Load the scanner.
        CCTRACEI ( _T("APAgent:CShutdownScan::Run - loading scanner"));
        SCANSTATUS status = m_pScanner->Initialize( DEFUTILS_APP_ID, m_config.GetBloodhoundLevel (), m_config.GetAttemptRepair() );
        
        if ( SCAN_OK != status )
        {
            CCTRACEE ( "APAgent:CShutdownScan::Run - scanner failed to init 0x%x", status );

            delete m_pScanner;
            m_pScanner= NULL;
            return -1;
        }

        CCTRACEI ( _T("APAgent:CShutdownScan::Run - scanning..."));
        
        // Keep scanning until there is no floppy, no infection, 
        // or the thread needs to exit.
        //
        while( !IsTerminating() && ScanFirstFloppy() )
		{
		    // Load strings.
		    TCHAR           szMessage[256];
		    TCHAR           szTitle[128];
		    nRet = LoadString( _Module.GetResourceInstance(), IDS_NAVAP_FLOPPYAINFECTED,
				    szMessage, sizeof (szMessage) );

		    if ( nRet > 0 )
		    {
		    nRet = LoadString(
				    _Module.GetResourceInstance(),
				    IDS_NAVAP_INFECTION_TITLE,
				    szTitle,
				    sizeof (szTitle) );
		    }

            // Here we need to put up a systemmodal
            // message to MAKE them remove the floppy.
            MessageBox( ::GetDesktopWindow(),
					   szMessage,
					   szTitle,
					   MB_OK | MB_ICONHAND | MB_SYSTEMMODAL);
		}
	}
	catch( _com_error e )
	{
        CCTRACEE ( _T("APAgent:CShutdownScan::Run - %s"), e.ErrorMessage() );
	}


    CCTRACEI ( _T("APAgent:CShutdownScan::Run - exiting2"));

    if ( m_pScanner )
    {
        delete m_pScanner;
        m_pScanner= NULL;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////
// CShutdownScan::ScanFirstFloppy()

bool CShutdownScan::ScanFirstFloppy()
{
	CCTRACEI( _T("APAgent - CShutdownScan::ScanFirstFloppy - Entered") );

	// Assume no virus found.
	bool bRet = false;

	// Look for the first floppy device.  
	char firstFloppy = findFirstFloppy();
	if( firstFloppy == 0 )
	{
		CCTRACEI( _T("APAgent - Found no floppy to scan") );
		return false;								  
	}

    CCTRACEI( _T("APAgent - CShutdownScan::ScanFirstFloppy - floppy found") );

	// Make sure there is media present.
	if( CDetectMedia::IsMediaPresent( firstFloppy, false ) == false )
	{
        std::string s;
		s = _T("No disk in drive: "); 
		s+= firstFloppy;
		CCTRACEI( s.c_str() );
		return false;
	}

    CCTRACEI( _T("APAgent - CShutdownScan::ScanFirstFloppy - attempting scan %c:"), firstFloppy );

	try
	{
		// Allocate scan item.
		char driveLetter;
        driveLetter = firstFloppy;

		// Perform the scan.
		bool bRepaired = false;
		bRet = scanBootRecord( driveLetter, bRepaired );
		
		// Q: Was a virus found?
		if( bRet )
		{
			// Yes, return status based on repair state.
			if( !bRepaired )
            {
                CCTRACEI( "APAgent - CShutdownScan::ScanFirstFloppy - still infected" );
				return true;  // Indicates that the boot record is still infected.
            }
			else
            {
                CCTRACEI( "APAgent - CShutdownScan::ScanFirstFloppy - infection repaired" );
				return false; // Indicates that the boot record virus was repaired.
            }
		}
	}
	catch(exception &e)
	{
		CCTRACEE( e.what() );
	}
  
	return bRet;
}

//////////////////////////////////////////////////////////////////////
// CShutdownScan::scanBootRecord()

bool CShutdownScan::scanBootRecord( char chDrive, bool &bRepaired )
{
	CScanPtr<IScanResults> pResults;
	
	// Perform the scan.
    m_pScanner->ScanBootRecord( chDrive, (IScanResults**) &pResults );

	// Q: Virus found?
	if( pResults && pResults->GetTotalInfectionCount() )
	{
		CCTRACEI( _T("APAgent - Boot virus found") );
		
		// Yes, add to notification queue.
		reportVirus( chDrive, pResults );

		// Q: did we repair?
		bRepaired = pResults->GetRemainingInfectionCount() == 0;

		if( bRepaired )
			CCTRACEI( _T("APAgent - Boot virus repaired") );
		else
			CCTRACEI( _T("APAgent - Boot virus NOT repaired") );

		// true == at least one infection found.
		return true;
	}

	CCTRACEI( _T("APAgent - No boot virus found") );

	// no virus detected.
	bRepaired = false;
	return false;
}

//////////////////////////////////////////////////////////////////////
// CShutdownScan::reportVirus()

void CShutdownScan::reportVirus( char chDrive, IScanResults* pResults )
{
	if (!pResults)
        return;
    
    for( int i = 0; i < pResults->GetTotalInfectionCount(); i++ )
	{
		// Get the infection object from the results.
		CScanPtr< IScanInfection > pInfection;
		if( SCAN_OK != pResults->GetInfection( i, (IScanInfection**) &pInfection ) )
            throw std::runtime_error( "Error parsing scan results." );

        CVirusAlertPopup alertUI;
        CVirusAlertPopup::DATA Data;

        Data.lType = AV::Event_Threat_ObjectType_BootRecord;
        Data.ulVirusID = pInfection->GetVirusID();
        Data.sVirusName = CA2W(pInfection->GetVirusName());
        

        CEventData EventData;
        EventData.SetData ( AV::Event_Base_propVersion, 1 );    // version 1 of this packet

        EventData.SetData ( AV::Event_Threat_propFeature, SAVRT_MODULE_ID_NAVAPSVC );
        EventData.SetData ( AV::Event_Base_propType, AV::Event_ID_Threat );
        EventData.SetData ( AV::Event_Threat_propEventSubType, AV::Event_Threat );
		EventData.SetData ( AV::Event_Base_propSessionID, getActiveSessionID() );

        // Product version
        EventData.SetData ( AV::Event_Threat_propProductVersion, m_config.GetProductVersion() );

        WORD wYear, wMonth, wDay = 0;
        unsigned long ulRev = 0;

        if ( m_pScanner->GetDefsDate( wYear, wMonth, wDay, ulRev ))
        {
            // Remember this value so we can put it in the activity log
            //
            // YYYYMMDDRRRR = 12 chars
            //
            WCHAR szDefsRevision [13] = {0};    
            wsprintfW ( szDefsRevision, L"%04u%02u%02u%04u", wYear, wMonth, wDay, ulRev );
            EventData.SetData ( AV::Event_Threat_propDefsRevision, szDefsRevision );
        }

        CEventData eventSubFile;
        EventData.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_BootRecord );

		// Capture drive letter "x:"
        {
        WCHAR szDriveLetter [] = L"x:";
        mbtowc ( &szDriveLetter[0], &chDrive, 1 );
		eventSubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szDriveLetter );
        Data.sObjectName = szDriveLetter;
        }

		// Save off virus attributes.
        CEventData eventThreat;
        eventThreat.SetData( AV::Event_ThreatEntry_propVirusName, pInfection->GetVirusName() );
        eventThreat.SetData( AV::Event_ThreatEntry_propVirusID, pInfection->GetVirusID() );

		// Need to capture action status (repaired, not-repaired, etc.)
		bool bAttemptedRepair = m_config.GetAttemptRepair();
        BYTE byAction = {0};
		switch( pInfection->GetStatus() )
		{
		case IScanInfection::INFECTED:
            byAction = bAttemptedRepair ? AV::Event_Action_RepairFailed : AV::Event_Action_Access_Denied;
			break;

		case IScanInfection::REPAIRED:
            byAction = AV::Event_Action_Repaired;
			break;

		default:
            byAction = AV::Event_Action_Unknown;
		}
        
        // Set action
        eventThreat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        eventThreat.SetData ( AV::Event_ThreatEntry_propActionData, (BYTE*) &byAction, 1 );

        Data.vecActions.push_back(byAction);

        // Save the threat data
        CEventData eventAllThreats;
        eventAllThreats.SetNode ( 0, eventThreat );
        eventSubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, eventAllThreats ); 

        // Pack it all up
        CEventData AllFiles;
        AllFiles.SetNode ( 0, eventSubFile );
        EventData.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        // Log the activity.
        //
        AV::IAvEventFactoryPtr pLogger;

        if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &pLogger)) ||
            pLogger.m_p == NULL )
        {
            CCTRACEE ( _T("CShutdownScan - Could not create IAvFactory object. - %d"), ::GetLastError() );
            pLogger = NULL;
        }
        else
        {
            pLogger->BroadcastAvEvent (EventData);
        }

        // Display the alert in this thread so it blocks the shutdown until the
        // user has read it and selected OK.
        alertUI.vecData.push_back(Data);
        alertUI.DoModal();
	}
    CCTRACEI ( _T("CScanQueue::reportVirus() - end"));	
}


//////////////////////////////////////////////////////////////////////
// CShutdownScan::findFirstFloppy()

TCHAR CShutdownScan::findFirstFloppy()
{						   
	TCHAR  szDrive[] = _T("\\\\.\\x:");
    DWORD  fdwLogicalDrives, dwDriveBit;
    TCHAR  chDrive, chRet = 0;
    std::vector<DISK_GEOMETRY> vGeometry(20);
	HANDLE hDrive;
	DWORD dwBytesReturned;
	DWORD dwValidCount;
    
    // Get a bitmap of all logical drives.
	fdwLogicalDrives = GetLogicalDrives();
    
    for ( chDrive = _T('A'), dwDriveBit = 1;
		  chDrive <= _T('Z') && chRet == 0; chDrive++, dwDriveBit <<= 1 )
    {
        // If the bit is set, query the drive.
        if ( 0 != ( fdwLogicalDrives & dwDriveBit ) )
        {
			// Open the drive.
			szDrive[4] = chDrive;
			hDrive = CreateFile( szDrive, 0, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL ); 
			if( hDrive == INVALID_HANDLE_VALUE )
				continue;

			// Get drive geometry from drive.
			if( DeviceIoControl( hDrive, IOCTL_DISK_GET_MEDIA_TYPES, NULL,
						 0, &vGeometry[0], sizeof(DISK_GEOMETRY) * 20, &dwBytesReturned, NULL ) )
			{
				// Compute number of valid DISK_GEOMETRY structures returned.
				dwValidCount = dwBytesReturned / sizeof(DISK_GEOMETRY);

				// Look for floppy geometry type.
				for( DWORD i = 0; i < dwValidCount; ++i )
				{
					switch ( vGeometry[i].MediaType ) 
					{
					case F5_1Pt2_512:	// 5.25, 1.2MB,  512 bytes/sector
					case F3_1Pt44_512:	// 3.5,  1.44MB, 512 bytes/sector
					case F3_2Pt88_512:	// 3.5,  2.88MB, 512 bytes/sector
					case F3_20Pt8_512:	// 3.5,  20.8MB, 512 bytes/sector
					case F3_720_512:	// 3.5,  720KB,  512 bytes/sector
					case F5_360_512:	// 5.25, 360KB,  512 bytes/sector
					case F5_320_512:	// 5.25, 320KB,  512 bytes/sector
					case F5_320_1024:	// 5.25, 320KB,  1024 bytes/sector
					case F5_180_512:	// 5.25, 180KB,  512 bytes/sector
					case F5_160_512:	// 5.25, 160KB,  512 bytes/sector
						chRet = chDrive;
						i = dwValidCount;
						break;
					}
				}
			}

			// Clean up.
			CloseHandle( hDrive );
        }  
    }
		  
	return chRet;
}


//////////////////////////////////////////////////////////////////////
// CShutdownScan::getActiveSessionID()

DWORD CShutdownScan::getActiveSessionID()
{
	// Need to dynamic link to wtsapi32.dll for WTSEnumerateSessions() 
	// function.
    ccLib::CModule moduleWTS;
    if ( !moduleWTS.Load ( "wtsapi32.dll" ))
        return 0;

	// get entry points.
	typedef BOOL (WINAPI *pfnWTSEnumerateSessions )( HANDLE, DWORD, DWORD, PWTS_SESSION_INFO*, DWORD* );
	typedef void (WINAPI *pfnWTSFreeMemory)(PVOID);
	pfnWTSEnumerateSessions pEnum = (pfnWTSEnumerateSessions) GetProcAddress( moduleWTS, "WTSEnumerateSessionsW" );
	pfnWTSFreeMemory pFree = (pfnWTSFreeMemory) GetProcAddress( moduleWTS, "WTSFreeMemory" );
	if( pEnum == NULL || pFree == NULL )
		return 0;

	// Call the function.
	DWORD dwCount = 0;
	PWTS_SESSION_INFO pSessionInfo = NULL;
	if( FALSE == pEnum( WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount ) )
		return 0;

	// Figure out which session is active.
	DWORD dwRet = 0;
	for( DWORD i = 0; i < dwCount; ++i)
	{
		if( pSessionInfo[i].State == WTSActive )
		{
			dwRet = pSessionInfo[i].SessionId;
			break;
		}
	}

	// Clean up.
	if( pSessionInfo )
		pFree( pSessionInfo );

    return dwRet;
}
