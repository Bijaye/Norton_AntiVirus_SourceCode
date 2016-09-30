#include "StdAfx.h"
#include ".\statespyware.h"

CStateSpyware::CStateSpyware(CWMIIntegration* pWMI, CNSCIntegration* pNSC):
    CState(pWMI,pNSC)
{
    // We listen for options changes
    //
    if ( makeEvent (SYM_OPTIONS_CHANGED_EVENT, true, m_eventOptions))
        m_vecWaitEvents.push_back (m_eventOptions);

    m_lEventID = AV::Event_ID_StatusSpyware;           // What CC event ID is this for?
    m_lOldSpywareCat = 1;
}

CStateSpyware::~CStateSpyware(void)
{
}

bool CStateSpyware::Save ()
{
    CCTRACEI( _T("CStateSpyware::save()"));

    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

	// Use persisted data if available
	//
    if ( m_bLoadedOld && !m_bInit )
    {
        // Initialize the settings.
        //
        if ( m_edStatus.GetData ( AVStatus::propSpywareCat, m_lOldSpywareCat ))
        {
            CCTRACEI ( "CStateSpyware::Save - using cached data %d", m_lOldSpywareCat );
            return true; // we used persisted data
        }
    }

    CNAVOptSettingsEx NavOpts;
    DWORD dwSpywareCat = 0;
    bool bReturn = false;
    DWORD dwNoSpywareInstalled = 1;

    // Try to load the file.
    //
	if ( !NavOpts.Init() )
    {
        CCTRACEE ( "NavOpts.Init falied" );
    }
    else
    {
        // Read the settings.
        //
        NavOpts.GetValue("THREAT:Threat6", dwSpywareCat, 0 );
        NavOpts.GetValue(THREAT_NoThreatCat, dwNoSpywareInstalled, 1 );
        dwNoSpywareInstalled = dwNoSpywareInstalled ? 0 : 1; // flip it
    }

    // Save data
    //
    if ( m_lOldSpywareCat != (long)dwSpywareCat )
        bReturn = true;

    m_lOldSpywareCat = dwSpywareCat;

    m_edStatus.SetData ( AVStatus::propSpywareCat, (long)dwSpywareCat );
    m_edStatus.SetData ( AVStatus::propSpywareInstalled, (long)dwNoSpywareInstalled );

    CCTRACEI ("Spyware - data saved update:%d cat:%d", bReturn, dwSpywareCat);
    return bReturn;
}

