#include "StdAfx.h"
#include "statefss.h"
#include "navtrust.h"
#include "navtasks.h"
#include "navtasks_i.c"
#include "symscriptsafe_h.h"
#include "GlobalEvents.h"
#include "ATLComTime.h"

CStateFSS::CStateFSS(CWMIIntegration* pWMI, CNSCIntegration* pNSC):CState(pWMI,pNSC)
{
    if ( makeEvent (SYM_REFRESH_FULLSYSTEMSCAN_EVENT, true, m_eventFSS))
        m_vecWaitEvents.push_back (m_eventFSS);        

    m_lEventID = AV::Event_ID_StatusFSS;           // What CC event ID is this for?
    m_dwTimeout = 60*60*1000; // 1 hour timeout
    //m_dwTimeout = 60*1000; // 1 min timeout for testing
}

CStateFSS::~CStateFSS(void)
{
}

bool CStateFSS::Save ()
{
    CCTRACEI( _T("CStateFSS::save()"));
    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

	// Use persisted data if available
    DATE dLastRunTime = 0;
    DWORD dwStatus = AVStatus::statusError;

	CTime convert_ctime = 0;
	//
    if ( m_bLoadedOld && !m_bInit )
    {
        long lSizeDate = sizeof(DATE);

        // Initialize the settings.
        //
        if ( m_edStatus.GetData ( AVStatus::propFSSDate, (BYTE*)&dLastRunTime, lSizeDate ))
        {
			SYSTEMTIME sysTm;
		    ATL::COleDateTime timeFSS (dLastRunTime);
			timeFSS.GetAsSystemTime(sysTm);
			CTime converted_time(sysTm);
			formatDate(AVStatus::propFSSDateS, converted_time);

            CCTRACEI ( "CStateFSS::Save - using cached data %d", dwStatus );
            return true; // we used persisted data
        }
    }

	long lScanAge = 0;

    // Save data
    //
    m_edStatus.SetData ( AVStatus::propFSSStatus, dwStatus );
    m_edStatus.SetData ( AVStatus::propFSSDate, (BYTE*)&dLastRunTime, sizeof(DATE) );
    formatDate (AVStatus::propFSSDateS, dLastRunTime);

    // Check the ScanTasks dll for a valid symantec signature
	if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVScanTasks) )
    {
        CCTRACEE( _T( "CStateFSS::save - Invalid digital signature on ScanTasks COM Server" ));
        return true;
    }
    
    CComPtr <INAVScanTasks> spTasks = NULL;
    CComPtr <ISymScriptSafe> spSymScriptSafe = NULL;

    if ( FAILED ( spTasks.CoCreateInstance ( CLSID_NAVScanTasks ) ))
    {
        CCTRACEE( _T( "Failed CoCreateInstance ( CLSID_NAVScanTasks )" ));
        return true;
    }

    if( FAILED( spTasks.QueryInterface(&spSymScriptSafe)))
    {
        CCTRACEE( _T( "Failed QueryInterface(&spSymScriptSafe) in CStatusFullSystemScan" ));
        return true;
    }

    // Set appropriate access so that calls to IsItSafe() 
	// return successfully.

	long dwAccess = ISYMSCRIPTSAFE_UNLOCK_KEYA;
	long dwKey = ISYMSCRIPTSAFE_UNLOCK_KEYB;
	spSymScriptSafe->SetAccess(dwAccess, dwKey);

    CComPtr <INAVScanTask> spTask;

    if ( SUCCEEDED ( spTasks->get_MyComputer ( &spTask )))
    {
        HRESULT hr = spTask->get_LastRunTime (&dLastRunTime);

        if ( S_FALSE == hr )
        {
            // Scan never run.
            //
            dwStatus = AVStatus::statusDisabled;

			CCTRACEW("Scan never run: %d", dwStatus);
        }
        else 
        {
            if ( SUCCEEDED (hr))
            {                
                dwStatus = AVStatus::statusEnabled;  // OK
				SYSTEMTIME sysTm;
		        ATL::COleDateTime timeFSS (dLastRunTime);
				timeFSS.GetAsSystemTime(sysTm);
				CTime temp_time(sysTm);
				convert_ctime = temp_time;
				CCTRACEI("Status: %d, Last Scan: %s", AVStatus::statusEnabled, (LPCTSTR)timeFSS.Format("%#c"));
            }
            else
                CCTRACEE( _T( "Failed get_MyComputer in CStatusFullSystemScan" ));
        }
    }

    m_edStatus.SetData ( AVStatus::propFSSStatus, dwStatus );
    m_edStatus.SetData ( AVStatus::propFSSDate, (BYTE*)&dLastRunTime, sizeof(DATE) );
    formatDate (AVStatus::propFSSDateS, convert_ctime);
    
    CCTRACEI ( "CStateFSS::Save() - exit" );
    return true;
}
