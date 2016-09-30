#include "StdAfx.h"
#include "StateAP.h"
#include "symscriptsafe_h.h"
#include "navapscr_i.c"
#include "NAVAPSCR.H"
#include "NAVOptions.h"
#include "AutoProtectWrapper.h"
#include "ScriptableAP.h"           // For launching Scriptable AP on another thread

extern CNAVOptSettingsEx g_NavOpts;

CStateAP::CStateAP(CWMIIntegration* pWMI, CNSCIntegration* pNSC):
    CState(pWMI,pNSC)
{
    if ( makeEvent (SYM_REFRESH_AP_STATUS_EVENT, true, m_eventAP))
        m_vecWaitEvents.push_back (m_eventAP);

    m_lEventID = AV::Event_ID_StatusAP;           // What CC event ID is this for?

    m_dwOldStatus = AVStatus::statusError;
    m_dwOldStatusSpyware = AVStatus::statusError;
}

CStateAP::~CStateAP(void)
{
}

// AP doesn't use persisted cached data
//
bool CStateAP::Save ()
{
    CCTRACEI ( "CStateAP::Save() - start" ); 
    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

    DWORD dwStatus = AVStatus::statusError;
    DWORD dwstatusSpyware = AVStatus::statusError;
    DWORD dwSpywareCat = 0;
    DWORD dwAdwareCat = 0;
    DWORD dwNoSpywareInstalled = 0;

    m_edStatus.SetData ( AVStatus::propAPStatus, (long) dwStatus );
    m_edStatus.SetData ( AVStatus::propAPSpywareStatus, (long) dwstatusSpyware );

    // We need to check 'licensing' here to see if it's *supposed* to be off, instead of an error.
    //
    DWORD dwVal = 1;
    
    CNAVOptSettingsEx NavOpts;

    // Try to load the file.
    //
	if ( !NavOpts.Init() )
    {
        CCTRACEE ( "NavOpts.Init falied" );
        return true;
    }

    // Read the license settings.
    //
	NavOpts.GetValue(LICENSE_FeatureEnabled, dwVal, 1 );
    NavOpts.GetValue(THREAT_NoThreatCat, dwNoSpywareInstalled, 0 );
    
    if ( dwNoSpywareInstalled )
    {
        dwstatusSpyware = AVStatus::statusNotAvailable;
    }
    
	if(!dwVal)
	{
		dwStatus = AVStatus::statusDisabled;
	}
    else
    {
        // Check the Scriptable AP dll for a valid symantec signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_ScriptableAutoProtect) )
        {
            CCTRACEE( "CStatusAutoProtect::MakeStatus - Invalid signature" );

            // Security failure
            //
            dwStatus = AVStatus::statusError;
            dwstatusSpyware = AVStatus::statusDisabled;
        }
        else
        {
            // Fixed in build 83. Perforce changelist 889775.  Reverting NAV AP Scriptable back to single threaded model.
            // It hangs under MTA when marshalling out to the AP Service in 9x in the CoCreate. 
            // I will coodinate with NIS for changes in their code base.
            // Launching scriptable AP in a new STA thread. See defect 1-2VMSMB.
            //
            CScriptableAP::STATE Result;
            HRESULT hr = E_FAIL;

            CScriptableAP AP ( CScriptableAP::GetState, &Result, &hr );
            AP.Create ( NULL, 0, 0 );
            AP.WaitForExit ( INFINITE );

            if ( FAILED (hr))
            {
                if ( hr == E_ACCESSDENIED )
                    CCTRACEE("get_Enabled security failed");
                else
                    CCTRACEE("get_Enabled failed");
            }

            CCTRACEI("CStatusAutoProtect::getStatus - get_Enabled done %d", Result);
            dwstatusSpyware = AVStatus::statusDisabled; // If AP is off, Spyware is Off, not Error.

            if ( CScriptableAP::NotLoaded == Result )
                dwStatus = AVStatus::statusNotRunning;
        	
            if ( !dwNoSpywareInstalled )
            {
                // Check the Spyware setting in SAVRT
                //
	            SAVRT_OPTS_STATUS rtStatus;
                DWORD dwData = 0;

	            // Load the realtime options.
                CCTRACEI( _T("CStatusAutoProtect::getStatus - loading savrt options")); 
                
                CAutoProtectOptions SAVRTOptions;        
                rtStatus = SAVRTOptions.Load();
	            if ( rtStatus != SAVRT_OPTS_OK )
	            {
		            // Couldn't load options file.  OK with us.  We'll just use the default values.
		            CCTRACEE( _T("Could not load options file. Using default values. Error Code: %d"), rtStatus ); 
	            }

                // Spyware
	            //
	            if ( SAVRT_OPTS_OK != SAVRTOptions.GetDwordValue( AP_szNAVAPCFGdwRespondToThreats, &dwData, 1 ))
                    CCTRACEE (_T("Unable to read the THREAT_RealTime value"));

                dwstatusSpyware = ( dwData & SAVRT_THREAT_MODE_NONVIRAL )?AVStatus::statusEnabled : AVStatus::statusDisabled;
            }

            dwStatus = ( CScriptableAP::Enabled == Result ) ? AVStatus::statusEnabled : AVStatus::statusDisabled;
        }
    }

    bool bReturn = false;

    // Save data
    //
    if ( m_dwOldStatus != dwStatus ||
         m_dwOldStatusSpyware != dwstatusSpyware )
        bReturn = true;

    m_dwOldStatus = dwStatus;
    m_dwOldStatusSpyware = dwstatusSpyware;

    m_edStatus.SetData ( AVStatus::propAPStatus, (long) dwStatus );
    m_edStatus.SetData ( AVStatus::propAPSpywareStatus, (long) dwstatusSpyware );

    CCTRACEI ("AP - data saved update:%d status:%d", bReturn, dwStatus);
    return bReturn;
}
