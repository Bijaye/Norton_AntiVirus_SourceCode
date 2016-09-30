#include "StdAfx.h"
#include ".\stateemail.h"


CStateEmail::CStateEmail(CWMIIntegration* pWMI, CNSCIntegration* pNSC):CState(pWMI,pNSC)
{
    if ( makeEvent (SYM_REFRESH_NAVPROXY_STATUS_EVENT, true, m_eventEmail))
        m_vecWaitEvents.push_back (m_eventEmail);        

    if ( makeEvent (SYM_REFRESH_NAV_LICENSE_STATUS_EVENT, true, m_eventLicensing))
        m_vecWaitEvents.push_back (m_eventLicensing);

    m_lEventID = AV::Event_ID_StatusEmail;           // What CC event ID is this for?
    m_lOldStatus = AVStatus::statusNone;
    m_lOldSMTP = 0;
    m_lOldPOP = 0;
    m_lOldOEH = 0;
}

CStateEmail::~CStateEmail(void)
{
}

bool CStateEmail::Save ()
{
    CCTRACEI("CStateEmail::MakeStatus()");

    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

	// Don't use persisted data for Email since it loads with StatusHP
    // in ccApp and there might be timing issues.

    // Default to off
    //
    DWORD dwStatus = AVStatus::statusError;
    DWORD dwSMTP = 0;
    DWORD dwPOP = 0;
    DWORD dwOEH = 0;
    bool bReturn = false;

    // Save data
    //
    { // scope the lock
        m_edStatus.SetData ( AVStatus::propEmailStatus, (long) dwStatus );
        m_edStatus.SetData ( AVStatus::propEmailSMTP, (long) dwSMTP );
        m_edStatus.SetData ( AVStatus::propEmailPOP, (long) dwPOP );
        m_edStatus.SetData ( AVStatus::propEmailOEH, (long) dwOEH );
    }

    // Is Email enabled?
    //
    try
    {
        CNAVOptSettingsEx NavOpts;

        // Try to load the file.
        //
	    if ( !NavOpts.Init() )
        {
            CCTRACEE("Failed to init options" );
            throw FALSE;
        }

        DWORD dwRetries = 0;

        // Read the Email settings.
        //
		NavOpts.GetValue(NAVEMAIL_FeatureEnabledScanIn, dwPOP, 0 );
		NavOpts.GetValue(NAVEMAIL_FeatureEnabledScanOut, dwSMTP, 0 );
        NavOpts.GetValue(NAVEMAIL_FeatureEnabledOEH, dwOEH, 0 );

		if ( dwPOP || dwSMTP )
		{
			// Check to see if it's ACTUALLY running 
			//
			dwStatus = AVStatus::statusNotRunning;

			for (int i = 0; i < 100 ; i++)		// Wait up to 10 Sec.
			{
				CCTRACEI("OpenMutex");

				ccLib::CMutex mutex;
				if (!mutex.Open( SYNCHRONIZE, FALSE, SYM_NAVPROXY_MUTEX, TRUE))
				{
					CCTRACEW("Failed to open (Global) SYM_NAVPROXY_MUTEX: %d", GetLastError());

					if (!mutex.Open( SYNCHRONIZE, FALSE, SYM_NAVPROXY_MUTEX, FALSE))
					{
						CCTRACEW("Failed to open (Local) SYM_NAVPROXY_MUTEX: %d", GetLastError());

						CCTRACEI("Waiting for mutex");
		  				HANDLE hSleep = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		  				if (hSleep != NULL)
						{
                            ::WaitForSingleObject ( hSleep, 100 );
							CloseHandle(hSleep);
						}
						continue;
					}
				}

				CCTRACEI("Email started");
				dwStatus = AVStatus::statusEnabled;
				break;
			}
		}
		else
		{
			dwStatus = AVStatus::statusDisabled;
		}

		CCTRACEI("Email status: %d", dwStatus);
    }
    catch (...)
    {
        dwStatus = AVStatus::statusNotInstalled;
        CCTRACEE("Unknown exception");
        return true;
    }

    // Save data, fire event if there was a change
    //
    if ( m_lOldStatus != dwStatus ||
         m_lOldSMTP != dwSMTP ||
         m_lOldPOP != dwPOP ||
         m_lOldOEH != dwOEH )
        bReturn = true;

    m_lOldStatus = dwStatus;
    m_lOldSMTP = dwSMTP;
    m_lOldPOP = dwPOP;
    m_lOldOEH = dwOEH;

    m_edStatus.SetData ( AVStatus::propEmailStatus, m_lOldStatus );
    m_edStatus.SetData ( AVStatus::propEmailSMTP, m_lOldSMTP );
    m_edStatus.SetData ( AVStatus::propEmailPOP, m_lOldPOP );
    m_edStatus.SetData ( AVStatus::propEmailOEH, m_lOldOEH );

	CCTRACEI("Data saved");
    return bReturn;
}
