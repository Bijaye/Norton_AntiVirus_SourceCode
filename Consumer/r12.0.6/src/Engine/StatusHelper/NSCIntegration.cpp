#include "StdAfx.h"
#include ".\nscintegration.h"

// NSC headers
#include "avNSCPlg_GUIDs.h"     // Our GUIDs
#include "nscIConsoleServer.h"
#include "NSCLoader.h"

using namespace nsc;

CNSCIntegration::CNSCIntegration(void)
{
    m_setEventIDs.insert ( AV::Event_ID_StatusAP );
    m_setEventIDs.insert ( AV::Event_ID_StatusDefs );
    m_setEventIDs.insert ( AV::Event_ID_StatusIWP );
    m_setEventIDs.insert ( AV::Event_ID_StatusEmail );
    m_setEventIDs.insert ( AV::Event_ID_StatusFSS );
    m_setEventIDs.insert ( AV::Event_ID_StatusIMScan );
    m_setEventIDs.insert ( AV::Event_ID_StatusLicensing );
    m_setEventIDs.insert ( AV::Event_ID_StatusSpyware );
}

CNSCIntegration::~CNSCIntegration(void)
{
}

bool CNSCIntegration::Subscribe ( long lEventID )
{
    if ( m_setEventIDs.find ( lEventID ) != m_setEventIDs.end() )
         return true;

    return false;
}

// NSC cared about something so here's the status bag.
// lEventID = 0 when we do our initial update
//
void CNSCIntegration::OnStatusChange ( long lEventID /* 0 = INIT */,
                                       CEventData& edNewStatus /*contains the new data*/ )
{
    // We care about this event.
    if ( 0 != lEventID && !Subscribe (lEventID))
        return;

    // Data has changed, remove the Clean Bit(tm) mutex
    m_mutexNSCCleanBit.Destroy();

    // Load NSCServerAPI.dll
    //
    CCTRACEI ( "CNSCIntegration::OnStatusChange - start %d", lEventID );

    NSC_IConsoleServerLoader NSCLoader;
    nsc::IConsoleServerPtr pNSCServer;

    try
    {
        if ( SYM_SUCCEEDED ( NSCLoader.CreateObject ( &pNSCServer )) && pNSCServer)
        {
            CCTRACEI ( "CNSCIntegration::OnStatusChange - NSCAPI loaded, calling Update" );
            nsc::NSCRESULT result = nsc::NSC_FAIL;

            iterEventIDs iterStart = m_setEventIDs.find(lEventID);
            iterEventIDs iterEnd = m_setEventIDs.end();

            if ( 0 == lEventID )
            {
                iterStart = m_setEventIDs.begin();
            }

            for ( iterEventIDs iterEvent = iterStart; iterEvent != iterEnd; iterEvent++ )
            {
                long lNextID = *iterEvent;

                if ( AV::Event_ID_StatusEmail == lNextID )
                {
                    // Update both Email and OEH, since they are the same and both might have changed.
                    //
                    result = pNSCServer->UpdateWrapper ( CLSID_NAV_Feature_EmailScanning, 0 /*not currently defined*/);
                    if ( NSC_FAILED (result))
                        CCTRACEE ("CNSCIntegration UpdateWrapper AP failed %d %d", result, CLSID_NAV_Feature_EmailScanning);

                    result = pNSCServer->UpdateWrapper ( CLSID_NAV_Feature_WormBlocking, 0 /*not currently defined*/);
                    if ( NSC_FAILED (result))
                        CCTRACEE ("CNSCIntegration UpdateWrapper Spyware failed %d %d", result, CLSID_NAV_Feature_WormBlocking);
                }
                else
                {
                    // All 1-to-1 event/GUIDs
                    //
                    GUID guidTemp;

                    switch ( lNextID )
                    {
                        case AV::Event_ID_StatusAP:
                            guidTemp = CLSID_NAV_Feature_AutoProtect;
                            break;

                        case AV::Event_ID_StatusSpyware:
                            guidTemp = CLSID_NAV_Feature_SpywareProtection;
                            break;

                        case AV::Event_ID_StatusDefs:
                            guidTemp = CLSID_NAV_Feature_VirusDefs;
                            break;

                        case AV::Event_ID_StatusIWP:
                            guidTemp = CLSID_NAV_Feature_IFP;
                            break;

                        case AV::Event_ID_StatusFSS:
                            guidTemp = CLSID_NAV_Feature_FSS;
                            break;

                        case AV::Event_ID_StatusIMScan:
                            guidTemp = CLSID_NAV_Feature_IMScanning;
                            break;
/*
                        case AV::Event_ID_StatusLicensing:
                            guidTemp = CLSID_NAV_Feature_Subscription;
                            break;
*/
                    }

                    result = pNSCServer->UpdateWrapper ( guidTemp, 0 /*not currently defined*/);
                    if ( NSC_FAILED (result))
                        CCTRACEE ("CNSCIntegration UpdateWrapper failed %d %d", result, guidTemp);
                    else
                        CCTRACEI ( "CNSCIntegration::OnStatusChange - success" );

                    // If we only want one
                    if ( 0 != lEventID )
                        break;
                }
            } // end loop
        }
        else
        {
            CCTRACEE ( "CNSCIntegration::OnStatusChange - failed to load NSC API" );
        }
    }
    catch(...)
    {
        CCTRACEE ( "CNSCIntegration::OnStatusChange - caught exception" );
        assert(false);
    }
}