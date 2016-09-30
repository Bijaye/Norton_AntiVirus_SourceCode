/////////////////////////////////////////////////////////////////////////////
// callback.cpp
//
// This file contains code that implements the DrvComm callback required for
// communication with the NAVAP driver.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "navapsvc.h"

// All these are for logging SAVRT driver errors
//
#include "AVccModuleId.h"
#include "AllNavEvents.h"
#include "..\navapsvcres\resource.h"
#include "NAVVersion.h"
#include "AvEvents.h"
#include "NAVEventFactoryLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::driverCallback()

void CServiceModule::driverCallback( DWORD dwServiceCode, PVOID pvInBuffer,
									  DWORD dwInBufferSize, PVOID pvOutBuffer,
								      DWORD dwOutBufferSize, PDWORD pdwBytesReturned )
{
	UNREFERENCED_PARAMETER( pvOutBuffer );
	UNREFERENCED_PARAMETER( dwOutBufferSize );
	UNREFERENCED_PARAMETER( pdwBytesReturned );

	// Call our handler.
	_Module.serviceCallback( dwServiceCode, pvInBuffer, dwInBufferSize );
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::serviceCallback()

void CServiceModule::serviceCallback( DWORD dwServiceCode, PVOID pvInBuffer, DWORD dwInBufferSize )
{
	// Dispatch the event to the proper handler.
	switch( dwServiceCode )
	{
		case SAVRT_EVENTREPORT:
			handleProgressEvent( static_cast<PCSAVRT_EVENTREPORTPACKET>( pvInBuffer ), dwInBufferSize );
			break;

        // Scan results of container scan
        //
        case SAVRT_CONTAINERREPORT:
            handleContainerReportEvent( static_cast<PSAVRT_CONTAINERREPORTPACKET>( pvInBuffer ), dwInBufferSize );
            break;

        // Container scan status. Start, stop, blocked app, etc.
        //
        case SAVRT_SEMISYNCSTATUSREPORT:
            handleSemiSyncNotificationEvent( static_cast<PSAVRT_SEMISYNCSCANNOTIFICATIONPACKET>( pvInBuffer ), dwInBufferSize );
            break;

		case SAVRT_SCANMOUNTEDDEVICE:
			handleScanMountEvent( static_cast<PSAVRT_SCANMOUNTEDDEVICEPACKET>( pvInBuffer ), dwInBufferSize );
			break;

        case SAVRT_ERRORPACKETREPORT:
            {
            PSAVRT_ERRORPACKET pPacket = static_cast<PSAVRT_ERRORPACKET>(pvInBuffer);
            if ( !pPacket )
                break;

            CCTRACEE ("SAVRT - %d", pPacket->dwErrorStatus);
            ATL::CString strMessage;

            switch ( pPacket->dwErrorStatus )
            {
            case SAVRT_ERROR_NAVEX_INTERFACE:
                strMessage.LoadString ( IDS_ERROR_SAVRT_NAVEX_INTERFACE);
                break;
            case SAVRT_ERROR_SUCCESS: // We better not get any of these from the driver!
                return;
                break;
            default:
                strMessage.LoadString (IDS_ERROR_SAVRT_UNKNOWN);
                break;
            }

	        // Create an event.
	        CEventData eventData;

	        // Populate and submit the event.
            eventData.SetData ( AV::Event_Base_propType, AV::Event_ID_Error );

            // Fill out the data elements
            //
            eventData.SetData ( AV::Event_Error_propErrorID, pPacket->dwErrorStatus );
            eventData.SetData ( AV::Event_Error_propModuleID, SAVRT_MODULE_ID_DRIVER );
            eventData.SetData ( AV::Event_Error_propResult, pPacket->dwErrorStatus );
            eventData.SetData ( AV::Event_Error_propMessage, (LPCSTR) strMessage );

            CNAVVersion Version;
            eventData.SetData ( AV::Event_Error_propProductVersion, Version.GetPublicRevision());

            AV::IAvEventFactoryPtr pLogger;

            if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &pLogger)) ||
                pLogger.m_p == NULL )
            {
                CCTRACEE ( _T("CServiceModule::serviceCallback - Could not create IAvFactory object. - %d"), ::GetLastError() );
                break;
            }
            else
            {
	            SYMRESULT result = pLogger->BroadcastAvEvent(eventData);
                if ( SYM_FAILED (result))
                    CCTRACEE ("CServiceModule::serviceCallback - failed to log error %d", result);
            }

            // Send to the agent
            if ( SAVRT_ERROR_NAVEX_INTERFACE == pPacket->dwErrorStatus )
            {
	            VARIANT v;
	            VariantInit( &v );

		        // Construct Variant containing infection data.
		        eventData.SerializeToVariant( v );
                m_handlers.BroadcastData ( v );
            }
            } // end SAVRT_ERRORPACKETREPORT
            break;

        default:
			// Unknown event.
			_ASSERT( FALSE );
			break;
	}
}