#include "StdAfx.h"
#include "StateALU.h"
#include "NetDetectController.h"    // For the AutoUpdate status
#include "NetDetectController_i.c"    // For the AutoUpdate status
#include "AutoUpdateDefines.h"      // For the ALU event name
#include "ccEveryoneDacl.h"

CStateALU::CStateALU(CWMIIntegration* pWMI, CNSCIntegration* pNSC):CState(pWMI,pNSC)
{
    if ( makeEvent (SYM_REFRESH_AUTOUPDATE_STATUS_EVENT, false, m_eventALU))
        m_vecWaitEvents.push_back (m_eventALU);

    m_lEventID = AV::Event_ID_StatusALU;
    m_lOldStatus = AVStatus::statusNone;
}

CStateALU::~CStateALU(void)
{
}


bool CStateALU::Save ()
{
    CCTRACEI( _T("CStateALU::save() - start"));
    
    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

    bool bReturn = false;

	// Use persisted data if available
	//
    if ( m_bLoadedOld && !m_bInit )
    {
        // Initialize the settings.
        //
        if ( m_edStatus.GetData ( AVStatus::propALUStatus, m_lOldStatus ))
        {
            CCTRACEI ( "CStateALU::Save - using cached data %d", m_lOldStatus );
            return true; // we used persisted data
        }
    }

    m_edStatus.SetData ( AVStatus::propALUStatus, m_lOldStatus );

    int iNewStatus = AVStatus::statusError;
	BOOL bEnabled = FALSE;


	// Check the ALU Net Detect dll for a valid symantec signature
	if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NDScheduler) )
	{
		CCTRACEE( _T( "CStateALU::save - Invalid digital signature on NDScheduler COM Server" ));
		return true;
	}

	ATL::CComPtr <INDScheduler> spAutoLiveUpdate;

	//
	// Get the interface for AutoLiveUpdate
	//
	HRESULT hr = S_OK;

	hr = spAutoLiveUpdate.CoCreateInstance ( CLSID_NDScheduler, 
		NULL,
		CLSCTX_INPROC_SERVER );
	if ( SUCCEEDED ( hr ))
	{
		//
		// Init the NetDetect scheduling object so we can talk to it
		// If this fails it means that there is no Task Scheduler or
		// the system won't support the feature.
		//
		hr = spAutoLiveUpdate -> Init ();

		if ( SUCCEEDED ( hr ))
		{
			//
			// Let's display the item even if it's not activated
			// At least then the user will know that the feature is available
			// to them to install. In order to do this they must open the
			// options and Enable it. If we don't display the item they
			// may not know to do this.
			//

			//
			// Is it enabled?
			//
			hr = spAutoLiveUpdate -> GetEnabled ( &bEnabled );

			if ( SUCCEEDED ( hr ))
			{
				//
				// Set the status.
				//
				if ( bEnabled )
					iNewStatus = AVStatus::statusEnabled;
				else 
					iNewStatus = AVStatus::statusDisabled;
			}
			else
			{
				// No NetDetect task?
				iNewStatus = AVStatus::statusDisabled;

				CCTRACEE(_T("CStateALU::save() : Failed to get ALU status 0x%08X"), hr);
			}
		}
		else
		{
			// If Init () fails it's usually because the system doesn't support
			// the ALU feature, i.e. it's missing Task Scheduler.
			//
			iNewStatus = AVStatus::statusNotAvailable;

			CCTRACEE(_T("CStateALU::save() : Failed to initialize scheduler object 0x%08X"), hr);
		}

	}
	else
	{
		CCTRACEE(_T("CStateALU::save() : Failed to create scheduler object 0x%08X"), hr);
	}

    if ( m_lOldStatus != iNewStatus )
        bReturn = true;

    m_edStatus.SetData ( AVStatus::propALUStatus, iNewStatus );
    m_lOldStatus = iNewStatus;

    CCTRACEI( _T("CStateALU::save() - exit"));
	return bReturn;
}

///////////////////////////////////
//
// HardRefresh for ALU was removed for performance. We used our persisted value now.
//
// Why are we hard-querying for ALU state? Because of a back door issue with ALU.
// See defect # 1-2USCHR. You can change ALU state in the Task Scheduler without generating
// a change event. LU team should really address this!!!
//
void CStateALU::HardRefresh ()
{
}
