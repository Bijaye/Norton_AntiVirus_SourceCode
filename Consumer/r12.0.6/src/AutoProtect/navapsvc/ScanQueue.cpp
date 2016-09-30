// ScanQueue.cpp: implementation of the CScanQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanQueue.h"
#include "DefsManager.h"
#include "notificationqueue.h"
#include "AllNavEvents.h"
#include "AVccModuleid.h"
#include "actions.h"
#include <winioctl.h> // For Disk geometry/floppy detection stuff.
#include "DetectMedia.h"
#include "const.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// CScanQueue::CScanQueue()

CScanQueue::CScanQueue( CNotificationQueue& notifyQueue ) :
	m_NotificationQueue( notifyQueue )
{
	// Intentionally empty	
}

//////////////////////////////////////////////////////////////////////
// CScanQueue::~CScanQueue()

CScanQueue::~CScanQueue()
{
	// Intentionally empty	
}

//////////////////////////////////////////////////////////////////////
// CScanQueue::AddItem()

void CScanQueue::AddItem( const CScanQueueItem& item )
{
    ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );

	// Q: What kind of device is this?  Could be a drive letter
	// or drive ID.  The test for this is kind of lame.  The assumption
	// here is that all fixed disks will have the high bit set.
	if( item.GetDeviceID() >= 0x80 && _Module.Config().GetScanMasterBootRecords() )
	{
	    // Make sure there isn't already an identical item in the queue
	    if( m_dupSet.find( item.GetDeviceID() ) == m_dupSet.end() )
	    {
		    // Add it to the duplicate map.
		    m_dupSet.insert( item.GetDeviceID() );
    		
		    // Add it to the queue.	
		    CSyncQueue< CScanQueueItem >::AddItem( item );
	    }
    }
}

//////////////////////////////////////////////////////////////////////
// CScanQueue::AddItems()

void CScanQueue::AddItems( vector< CScanQueueItem > &vItems )
{
	ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );

	for( vector< CScanQueueItem>::iterator it = vItems.begin();
		 it != vItems.end(); it++ )
	{
		AddItem( *it );
	}
}

//////////////////////////////////////////////////////////////////////
// CScanQueue::Pause()

void CScanQueue::Pause()
{
	// To effect a pause, claim the pause critical 
	// section object.
	m_csPause.Lock();
}

//////////////////////////////////////////////////////////////////////
// CScanQueue::Resume()

void CScanQueue::Resume()
{
	// Resume the process.
	m_csPause.Unlock();
}

//////////////////////////////////////////////////////////////////////
// CScanQueue::getQueueFront()

bool CScanQueue::getQueueFront( CScanQueueItem& item )
{
	ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );

	// Fetch the item
	bool bRet = CSyncQueue< CScanQueueItem >::getQueueFront( item );

	// If we got an item, remove it from the dupmap.
	if( bRet )
		m_dupSet.erase( item.GetDeviceID() );

	return bRet;
}

//////////////////////////////////////////////////////////////////////
// CScanQueue::processQueue()

void CScanQueue::processQueue()
{
	try
	{
		// Obtain the pause lock.
		ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );

        // Process the whole queue
        //
        if ( GetSize() )
        {
		    // Load the scanner once and reuse it.
            // Make a static ref count on the object since this object is on the stack
		    CScanner *pScanner = new CSymStaticRefCount<CScanner>;

            if (!pScanner)
            {
                CCTRACEE ( "CScanQueue::processQueue - failed to new scanner" );
            }
            else
            {
                CCTRACEI ( "CScanQueue::processQueue - initializing");

		        SCANSTATUS status = pScanner->Initialize( DEFUTILS_APP_ID,
                                                        _Module.Config().GetBloodhoundLevel(),
                                                        _Module.Config().GetAttemptRepair(),
                                                        _Module.Config().GetTestDefsPath());

                if ( SCAN_OK != status )
                {
                    CCTRACEE ( "CScanQueue::processQueue - scanner failed to init 0x%x", status );
                }
                else
                {
		            // Process the queue.
		            CScanQueueItem item;
		            while( !IsTerminating() && getQueueFront( item ) )
		            {
				        // Only scan master boot records if we have to.
				        scanMasterBootRecord( pScanner, item );
			        }
                }
                CCTRACEI ("CScanQueue::processQueue - scan complete");

                delete pScanner;
                pScanner = NULL;
		    }
        }
	}
	catch(exception &e)
	{
		_Module.LogEvent( e.what() );
	}
}


//////////////////////////////////////////////////////////////////////
// CScanQueue::scanMasterBootRecord()

void CScanQueue::scanMasterBootRecord( CScanner* pScanner, CScanQueueItem& item )
{
	CScanPtr<IScanResults> pResults;
	
	// Perform the scan.
	pScanner->ScanMasterBootRecord( item.GetDeviceID(), (IScanResults**) &pResults );

	// Q: Virus found?
	if( pResults && pResults->GetTotalInfectionCount() )
	{
		// Yes, add to notification queue.
		reportVirus( item, pResults );
	}
}

//////////////////////////////////////////////////////////////////////
// CScanQueue::reportVirus()

void CScanQueue::reportVirus( CScanQueueItem& item, IScanResults* pResults )
{
    CCTRACEI ( _T("CScanQueue::reportVirus() - begin"));

	USES_CONVERSION;

	for( int i = 0; i < pResults->GetTotalInfectionCount(); i++ )
	{
		// Get the infection object from the results.
		CScanPtr< IScanInfection > pInfection;
		if( SCAN_OK != pResults->GetInfection( i, (IScanInfection**) &pInfection ) )
			throw runtime_error( "Error parsing scan results." );

        CEventData eventData;
        eventData.SetData ( AV::Event_Base_propVersion, 1 );    // version 1 of this packet

        eventData.SetData ( AV::Event_Threat_propFeature, SAVRT_MODULE_ID_NAVAPSVC );
        eventData.SetData ( AV::Event_Base_propType, AV::Event_ID_Threat );
        eventData.SetData ( AV::Event_Threat_propDefsRevision, _Module.Config().GetDefsRevision() );
        eventData.SetData ( AV::Event_Threat_propProductVersion, _Module.Config().GetProductVersion () );
        eventData.SetData ( AV::Event_Threat_propEventSubType, AV::Event_Threat );

        CEventData eventSubFile;
        int iObjectType = 0;

		if( item.FromService() )
		{
            // Call decode user info with a NULL user packet and it will use the default
            // user name and active session ID
            CNotification::DecodeUserInfo( NULL, eventData, true );

			// Capture drive letter.
			char szDriveLetter[] = "x:";
			szDriveLetter[0] = (char) item.GetDeviceID();
            eventSubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szDriveLetter );
		}
		else
		{
            // Gather data from packet.
            SAVRT_SCANMOUNTEDDEVICEPACKET* pPacket = item.GetPacket();

            // Send the boot record virus alert to the active session.
            // The problem here is that boot sector scans occur on
            // mount operations, which always come from session id 0, but we want
            // to display the alert to the active session
            CNotification::DecodeUserInfo( &pPacket->rUser, eventData, true );

	        BYTE byDeviceID = (BYTE)pPacket->szDevice[0];

	        // Q: Is this a MBR or drive letter?
	        if( byDeviceID < 0x80 )
	        {
		        // Drive letter.
		        WCHAR szDrive[] = L"x:";
		        szDrive[0] = byDeviceID;
		        eventSubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szDrive );
                iObjectType = AV::Event_Threat_ObjectType_BootRecord;
	        }
	        else
	        {
		        // MBR drive ID.
		        WCHAR szBuffer[16];
		        swprintf( szBuffer, L"0x%X", byDeviceID );
		        eventSubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szBuffer );
                iObjectType = AV::Event_Threat_ObjectType_MasterBootRecord;
	        }

            eventData.SetData ( AV::Event_Threat_propObjectType, iObjectType );
		}

		// Save off virus attributes.
        CEventData eventThreat;
        eventThreat.SetData( AV::Event_ThreatEntry_propVirusName, pInfection->GetVirusName() );
        eventThreat.SetData( AV::Event_ThreatEntry_propVirusID, pInfection->GetVirusID() );

		// Need to capture action status (repaired, not-repaired, etc.)
		bool bAttemptedRepair = _Module.Config().GetAttemptRepair();
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

        // Save the threat data
        CEventData eventAllThreats;
        eventAllThreats.SetNode ( 0, eventThreat );
        eventSubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, eventAllThreats ); 

        // Pack it all up
        CEventData AllFiles;
        AllFiles.SetNode ( 0, eventSubFile );

        eventData.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );


		// Create a notification object
		CNotification* pNotification = new CNotification ( eventData );

		// Finally, add to notification queue.
		m_NotificationQueue.AddItem( pNotification );
	}
    CCTRACEI ( _T("CScanQueue::reportVirus() - end"));	
}

//////////////////////////////////////////////////////////////////////
// class CScanQueueItem implementation.
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::CScanQueueItem()

CScanQueueItem::CScanQueueItem() :
	m_pPacket( NULL ),
	m_byDevice( 0 ),
	m_tc( ::GetTickCount() )
{
	// Intentionally empty
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::CScanQueueItem()

CScanQueueItem::CScanQueueItem( BYTE byDevice ) :
	m_pPacket( NULL ),
	m_byDevice( byDevice ),
	m_tc( ::GetTickCount() )
{
	// Intentionally empty
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::CScanQueueItem()

CScanQueueItem::CScanQueueItem( const CScanQueueItem& other ) :
	m_pPacket( NULL ),
	m_byDevice( 0 ),
	m_tc( ::GetTickCount() )
{
	if( other.m_pPacket )
	{
		m_pPacket = new SAVRT_SCANMOUNTEDDEVICEPACKET( *other.m_pPacket );
		m_byDevice = 0;
	}
	else
	{
		m_byDevice = other.m_byDevice;
	}
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::CScanQueueItem()

CScanQueueItem::CScanQueueItem( const SAVRT_SCANMOUNTEDDEVICEPACKET* pPacket ) :
	m_tc( ::GetTickCount() )
{
	// Allocate a packet.
	m_pPacket = new SAVRT_SCANMOUNTEDDEVICEPACKET( *pPacket );
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::~CScanQueueItem()

CScanQueueItem::~CScanQueueItem()
{
	if( m_pPacket )
	{
		delete m_pPacket;
		m_pPacket = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::operator=()

CScanQueueItem& CScanQueueItem::operator=( const CScanQueueItem& other )
{
	if( &other != this )
	{
		if( other.m_pPacket )
		{
			// Allocate packet if needed.
			if( m_pPacket == NULL )
				m_pPacket = new SAVRT_SCANMOUNTEDDEVICEPACKET;
			
			// Copy the data
			*m_pPacket = *other.m_pPacket;
		}
		else
		{
			// Remove existing allocation.
			if( m_pPacket )
			{
				delete m_pPacket;
				m_pPacket = NULL;
			}
			
			// Save off device ID.
			m_byDevice = other.m_byDevice;
		}
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::FromService()

bool CScanQueueItem::FromService()
{
	return m_pPacket == NULL;
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::GetDeviceID()

BYTE CScanQueueItem::GetDeviceID() const
{
	return m_pPacket ? (BYTE) m_pPacket->szDevice[0] : m_byDevice;
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::SetDeviceID()

void CScanQueueItem::SetDeviceID( BYTE byDevice )
{
	m_byDevice = byDevice;
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::GetPacket()

SAVRT_SCANMOUNTEDDEVICEPACKET * CScanQueueItem::GetPacket()
{
	return m_pPacket;
}

//////////////////////////////////////////////////////////////////////
// CScanQueueItem::operator==()

bool CScanQueueItem::operator==(const CScanQueueItem& other ) const
{
	// Q: Do they have the same device id?
	return GetDeviceID() == other.GetDeviceID();
}


