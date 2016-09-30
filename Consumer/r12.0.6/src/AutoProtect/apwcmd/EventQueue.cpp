// EventQueue.cpp: implementation of the CEventQueue class.
//
// This class accepts the SAFEARRAY objects from the CEventHandler class.
// It makes a COPY of these events when they are added. In a separate
// thread it pushes these Q'ed events out to the CDispatchQueue for
// conversion into CEventData objects.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "windows.h"
#include "process.h"
#include "ole2.h"
#include "EventQueue.h"
#include "ccCatch.h"
#include "NAVError_h.h"

//////////////////////////////////////////////////////////////////////
// CEventQueue::CEventQueue()

CEventQueue::CEventQueue() :
    m_pVirusAlertQueue(NULL)
#ifdef AP_COMPRESSED_SUPPORT
        ,m_pBlockedAppQueue(NULL)
#endif //AP_COMPRESSED_SUPPORT
{
}

//////////////////////////////////////////////////////////////////////
// CEventQueue::~CEventQueue()

CEventQueue::~CEventQueue()
{
}

//////////////////////////////////////////////////////////////////////
// CEventQueue::processQueue()

void CEventQueue::processQueue()
{
    // Pass the CEventData into the correct queue
    // for processing.
    //
	CEventData* pEventData = NULL;
    
	while( getQueueFront( pEventData ) )
	{
        try
        {
            long lType = 0;
            if( false == pEventData->GetData( AV::Event_Base_propType, lType ) )
                throw std::runtime_error( "Missing data from event object" );

            switch ( lType )
            {
                case AV::Event_ID_ManualScan_Spyware_Detection:
                    ProcessManualScanEvent(pEventData);
                    break;
                case AV::Event_ID_Threat:
                    // Send to the Virus Alert Q.
                    //
                    CCTRACEI (_T("CEventQueue::processQueue - Got Virus Alert"));
                    ProcessVirusEvent(pEventData);
                    break;

#ifdef AP_COMPRESSED_SUPPORT
                case AV::Event_ID_Container_ScanStatus:
                    {
	                // Create blocked app queue object.
                    //
                    try
                    {
                        if ( !m_pBlockedAppQueue.get ())
                        {
                            m_pBlockedAppQueue = std::auto_ptr<CBlockedAppQueue> (new CBlockedAppQueue());

                            // Start it.
                            if ( !m_pBlockedAppQueue->Create(NULL,0,0))
                            {
                                CCTRACEE (_T("CEventQueue::processQueue - Failed to create the blocked app thread bailing out."));
                                return;
                            }
                        }
                    }
                    catch(...) // what does this throw? &$
                    {
                        CCTRACEE ("CEventQueue::processQueue - Failed to create blocked app queue!");
                        return;
                    }

                    long lContainerStatus = 0;
                    if( false == pEventData->GetData( AV::Event_Container_Status, lContainerStatus ) )
			            throw std::runtime_error( "Missing data from container event object" );
                    
                    switch (lContainerStatus)
                    {
                        case AV::Event_Container_Status_BLOCKED:
                        {
                            SAVRT_ROUS64 hTempCookie;
                            long lSize = sizeof (SAVRT_ROUS64);
                            if ( pEventData->GetData ( AV::Event_Container_Cookie, (BYTE*) &hTempCookie, lSize ))
                                CCTRACEI (_T("CEventQueue::processQueue - Got BLOCKED event %d"), hTempCookie);

                            m_pBlockedAppQueue->AddItem(pEventData);
                        }
                        break;

                        case AV::Event_Container_Status_COMPLETED:        // Container scan finished OK
                        case AV::Event_Container_Status_CANCELLED:        // The user cancelled a scan
                        case AV::Event_Container_Status_ABORTED:          // The scan took too long and timeout out
                        {
                            SAVRT_ROUS64 hTempCookie;
                            long lSize = sizeof (SAVRT_ROUS64);
                            if ( pEventData->GetData ( AV::Event_Container_Cookie, (BYTE*) &hTempCookie, lSize ))
                                CCTRACEI (_T("CEventQueue::processQueue - Got STOP event %d"), hTempCookie);

                            m_pBlockedAppQueue->AddItem(pEventData);
                        }
                        break;

                        case AV::Event_Container_Status_STARTED:
                            //CCTRACEI (_T("CEventQueue::processQueue - Got START event"));
                            break;

                        default:
                            // Starts and unknowns
                            CCTRACEI (_T("CEventQueue::processQueue - Got UNKNOWN event"));
                            break;
                    }
                    }
                    break;
#endif //AP_COMPRESSED_SUPPORT
                
                // AP Service sent us an error
                case AV::Event_ID_Error:
                    ProcessErrorEvent (pEventData);
                    break;

                default:
                    throw std::runtime_error( "Unknown event type" );
                    break;
            }
      
        }
        catch(...)
        {
            CCTRACEE (_T("CEventQueue::processQueue"));

            if ( pEventData )
            {
                delete pEventData;
                pEventData = NULL;
            }
        }
	}   
}

void CEventQueue::ProcessErrorEvent ( CEventData*& pEventData )
{
    // Just display the error in this thread. Let's not create a separate thread just for this.
    //
	// Get a NAVError object
	CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
	CComPtr <INAVCOMError> spNavError;

    long lErrorID = 0;
    long lModuleID = 0;
    std::string strText;

    pEventData->GetData ( AV::Event_Error_propErrorID, lErrorID );
    pEventData->GetData ( AV::Event_Error_propModuleID, lModuleID );
    pEventData->GetData ( AV::Event_Error_propMessage, strText );

	// Create, Populate, Log, and Display the error
	HRESULT hr = spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);
    if ( SUCCEEDED(hr))
    {
    	CComBSTR bstrText = strText.c_str();
		hr = spNavError->put_ModuleID(lModuleID);
		hr = spNavError->put_ErrorID(lErrorID);
		hr = spNavError->put_Message(bstrText);
		hr = spNavError->Show (TRUE, FALSE, 0); // Show it but don't log it. It's already logged.
    }
}

void CEventQueue::ProcessManualScanEvent (CEventData* pEventData)
{
    // Get the queue
    if ( !m_pVirusAlertQueue.get ())
    {
        ccLib::CExceptionInfo exceptionInfo(_T("CEventQueue::ProcessManualScanEvent()"));
        try
        {
            m_pVirusAlertQueue = std::auto_ptr<CVirusAlertQueue> (new CVirusAlertQueue());

            // Start it.
            if ( !m_pVirusAlertQueue->Create(NULL, 0, 0 ))
            {
                CCTRACEE (_T("CEventQueue::ProcessManualScanEvent() - Failed to create the queue bailing out."));
                return;
            }
        }
        CCCATCHMEM(exceptionInfo);

        if ( exceptionInfo.IsException() )
        {
            return;
        }
    }

    //
    // Get the data from the event to send to the queue
    //

    long lTemp = 0;
    long lScanCookie = 0;

    // Get the scan cookie
    if( !pEventData->GetData( AV::Event_ManualScan_Spyware_propCookie, lScanCookie ) )
    {
        CCTRACEE (_T("CEventQueue::ProcessManualScanEvent() - Missing cookie from manual scan event data."));
        return;
    }

    // Find out if this is a complete event or add event
    if( pEventData->GetData( AV::Event_ManualScan_Spyware_propEnd, lTemp ) && lTemp == 1 )
    {
        // This is a completion event
        m_pVirusAlertQueue->manualScanCompleteEvent(lScanCookie);
        return;
    }

    // This is an add event so we need to send it to the queue for tracking
    unsigned long ulVirusID = 0;
    std::wstring strFileName;

    // Get the VID
    if( !pEventData->GetData( AV::Event_ManualScan_Spyware_propVirusID, lTemp ) )
    {
        CCTRACEE (_T("CEventQueue::ProcessManualScanEvent() - Missing VID from spyware remove item data, using 0."));
    }
    else
    {
        ulVirusID = static_cast<unsigned long> (lTemp);
    }

    // Get the filename
    if( !pEventData->GetData( AV::Event_ManualScan_Spyware_propFileName, strFileName ) )
    {
        CCTRACEE (_T("CEventQueue::ProcessManualScanEvent() - Missing file name from spyware remove item data."));
        return;
    }

    // Send the data
    m_pVirusAlertQueue->manualScanFileEvent(lScanCookie, ulVirusID, strFileName.c_str());
}

void CEventQueue::ProcessVirusEvent( CEventData*& pEventData )
{
	// Create virus alert event queue object.
    //
    try
    {
        if ( !m_pVirusAlertQueue.get ())
        {
            m_pVirusAlertQueue = std::auto_ptr<CVirusAlertQueue> (new CVirusAlertQueue());

            // Start it.
            if ( !m_pVirusAlertQueue->Create(NULL, 0, 0 ))
            {
                CCTRACEE (_T("CEventQueue::ProcessVirusEvent - Failed to create the virus alert thread bailing out."));
                return;
            }
        }
    }
    catch(...) // what does this throw? &$
    {
        CCTRACEE ("CEventQueue::ProcessVirusEvent - Failed to create virus alert queue!");
        return;
    }

    try
	{
        CEventData eventSubFiles;
        CVirusAlertPopup* pUI = new CVirusAlertPopup();

        if (pEventData->GetNode ( AV::Event_Threat_propSubFileData, eventSubFiles ))
        {
            long lFileIndex = 0;
            long lSubType = 0;

		    // Handle container scans here too.
            //
            if ( pEventData->GetData ( AV::Event_Threat_propEventSubType, lSubType )
                && AV::Event_Threat_Container == lSubType)
            {
                // Container, don't do side effects scan
                // Check sub files for Spyware
                
                // Get the container name, if it exists
                CEventData eventParent;
                eventSubFiles.GetNode ( 0, eventParent );
                eventParent.GetData ( AV::Event_ThreatSubFile_propFileName, pUI->sContainerName );

                // Skip the parent
                lFileIndex = 1;
            }

            for (; lFileIndex < eventSubFiles.GetCount (); lFileIndex ++)
            {
                // One file
                CEventData eventFile;
                if ( eventSubFiles.GetNode ( lFileIndex, eventFile ))
                {
                    CEventData eventThreats;
                    eventFile.GetNode ( AV::Event_ThreatSubFile_propThreatData, eventThreats );
                    
                    // iterate threats
                    for ( long lThreatIndex = 0; lThreatIndex < eventThreats.GetCount (); lThreatIndex ++ )
                    {
                        CEventData eventThreat;
                        if ( eventThreats.GetNode ( lThreatIndex, eventThreat ))
                        {
                            long lThreatCount = 0;
                            if (eventThreat.GetData ( AV::Event_ThreatEntry_propThreatCatCount, lThreatCount ))
                            {
#ifdef AP_SPYWARE_SUPPORT
                                // Spyware detection!
                                // Add data to Spyware queue, if it isn't duped!
                                unsigned long ulVirusID = 0;
                                long lTempVirusID = 0;
                                std::wstring strTempFileName;
                                std::wstring strTempThreatName;
                                std::string strTempThreatCats;

                                eventThreat.GetData ( AV::Event_ThreatEntry_propVirusID, lTempVirusID );
                                ulVirusID = static_cast<unsigned long> (lTempVirusID);
                                
                                // If this is from a container use the container name so we scan it.
                                //
                                bool bCompressed = false;
                                if ( pUI->sContainerName.empty() )
                                    eventFile.GetData   ( AV::Event_ThreatSubFile_propFileName, strTempFileName );
                                else
                                {
                                    CCTRACEI ("CEventQueue::ProcessVirusEvent - This is a compressed spyware alert.");
                                    strTempFileName = pUI->sContainerName;
                                    bCompressed = true;
                                }

                                eventThreat.GetData ( AV::Event_ThreatEntry_propVirusName, strTempThreatName );
                                eventThreat.GetData ( AV::Event_ThreatEntry_propThreatCategories, strTempThreatCats );

                                m_pVirusAlertQueue->AddSpywareAlert (ulVirusID,
                                                                     strTempFileName,
                                                                     strTempThreatName,
                                                                     strTempThreatCats,
                                                                     bCompressed);

                                // next threat
#endif // AP_SPYWARE_SUPPORT
                            }
                            else
                            {
                                CVirusAlertPopup::DATA Data;
                                
                                // add data to viral queue
                                long lTempVirusID = 0;
                                eventThreat.GetData ( AV::Event_ThreatEntry_propVirusID, lTempVirusID );
                                Data.ulVirusID = static_cast<unsigned long> (lTempVirusID);

                                eventFile.GetData ( AV::Event_ThreatSubFile_propFileName, Data.sObjectName );

		                        // Get the virus name
                                eventThreat.GetData ( AV::Event_ThreatEntry_propVirusName, Data.sVirusName );
                                pEventData->GetData(AV::Event_Threat_propObjectType, Data.lType );

                                // Get actions.
                                long lSize = 0;
                                eventThreat.GetData ( AV::Event_ThreatEntry_propActionCount, lSize );
                                Data.vecActions.resize (lSize);
                                eventThreat.GetData ( AV::Event_ThreatEntry_propActionData, &Data.vecActions[0], lSize );

                                pUI->vecData.push_back(Data);
                            }
                        }
                    }
                }
            }

            if ( pUI->vecData.size ())
                m_pVirusAlertQueue->AddVirusAlert (pUI);
            else
            {
                CCTRACEI ("CEventQueue::ProcessVirusEvent - threat event, excluded, or ignored, not alerting now.");
                delete pUI;
                pUI = NULL;
            }

        } // end event
        else
        {
            CCTRACEE ("No sub files!");
            delete pUI;
            pUI = NULL;
        }
    }
    catch (...)
    {
        CCTRACEE (_T("CVirusAlertQueue::displayAlert"));
    }
}
