#include "StdAfx.h"
#include "TraceHR.h"
#include "StateIWP.h"
#include "IWPSettingsLoader.h"
#include "IWPSettingsInterface.h"
#include "NAVOptions.h"

CStateIWP::CStateIWP(CWMIIntegration* pWMI, CNSCIntegration* pNSC):CState(pWMI,pNSC)
{
    if ( makeEvent (IWP::SYM_REFRESH_IWP_EVENT, true, m_eventIWP))
        m_vecWaitEvents.push_back (m_eventIWP);        

    m_lEventID = AV::Event_ID_StatusIWP;           // What CC event ID is this for?
    m_lOldStatus = AVStatus::statusError;
}

CStateIWP::~CStateIWP(void)
{
}

//////////////////////////////////////////////////////////////////////
// CStateIWP::Save()

bool CStateIWP::Save()
{
	CCTRACEI( _T("CStateIWP::save() - start"));
    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

	// Don't use persisted data for IWP since it loads with StatusHP
    // in ccApp and there might be timing issues.

    DWORD dwResult = IWP::IIWPSettings::IWPStateError;
    DWORD dwStatus = AVStatus::statusError;
    IWP::IIWPSettingsPtr pIWPSettings;

    if(SYM_FAILED(IWP_IWPSettings::CreateObject(GETMODULEMGR(), &pIWPSettings)))
    {
        // No IWP installed
        //
        CCTRACEW ( "CStatusIWP - no IWP installed" );
        pIWPSettings = NULL;
        dwStatus = AVStatus::statusNotAvailable; // IWP not in this layout
    }
    else
    {
        if (pIWPSettings->GetValue ( IWP::IIWPSettings::PROPERTY::IWPState, dwResult ))
        {
            // Map the IWP result to a NAV Status result.
            switch ( dwResult )
            {
            case IWP::IIWPSettings::IWPStateError:
                dwStatus = AVStatus::statusError;
                break;
            case IWP::IIWPSettings::IWPStateNotAvailable:
                dwStatus = AVStatus::statusNotAvailable;  // not an error
                break;
            case IWP::IIWPSettings::IWPStateYielding:
                dwStatus = AVStatus::statusNotRunning;    // not an error
                break;
            case IWP::IIWPSettings::IWPStateEnabled:
                dwStatus = AVStatus::statusEnabled;
                break;
            case IWP::IIWPSettings::IWPStateDisabled:
                dwStatus = AVStatus::statusDisabled;
                break;
            }
        }

        // Release the interface before the loader, just in case.
        //
        if ( pIWPSettings )
            pIWPSettings.Release();
    }

    bool bReturn = false;

    // Save data
    //
    if ( m_lOldStatus != dwStatus )
        bReturn = true;

    m_lOldStatus = dwStatus;
    m_edStatus.SetData ( AVStatus::propIWPStatus, (long) dwStatus );

    CCTRACEI( _T("CStateIWP::save() - exit"));
	return bReturn;
}
