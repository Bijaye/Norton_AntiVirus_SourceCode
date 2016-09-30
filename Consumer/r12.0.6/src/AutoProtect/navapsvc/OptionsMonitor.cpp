// OptionsMonitor.cpp: implementation of the COptionsMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OptionsMonitor.h"
#include "globalevent.h"
#include "globalevents.h"
#include "n32exclu.h"
#include "CAPThreatExclusions.h"

//////////////////////////////////////////////////////////////////////
// COptionsMonitorThread::COptionsMonitorThread()

COptionsMonitorThread::COptionsMonitorThread() 
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// COptionsMonitorThread::~COptionsMonitorThread()

COptionsMonitorThread::~COptionsMonitorThread()
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// COptionsMonitorThread::~COptionsMonitorThread()

int COptionsMonitorThread::Run()
{
    // Create options event
	CGlobalEvent eventOptions;
    CGlobalEvent eventThreatExclusions; // threat exclusions changed

	if( FALSE == eventOptions.Create( SYM_OPTIONS_CHANGED_EVENT ) ||
        FALSE == eventThreatExclusions.Create ( SYM_REFRESH_NAV_EXCLUSIONS_TCHAR ))
	{
		_ASSERT( FALSE );
		return -1;
	}    

	// Set up waiting objects.
    std::vector<HANDLE> vecHandles;
    vecHandles.push_back (m_Terminating.GetHandle());
    vecHandles.push_back (eventOptions);
    vecHandles.push_back (eventThreatExclusions);

    HANDLE hScanDelayTimer = CreateWaitableTimer ( NULL, FALSE, NULL );

    if ( hScanDelayTimer )
    {
        LARGE_INTEGER liTime;
        liTime.QuadPart = - (10*60*1000 * 10000); // 10 minutes
        if (!SetWaitableTimer ( hScanDelayTimer, &liTime, 0 /*one time*/, NULL, NULL, FALSE ))
        {
            CCTRACEE ("Failed to set Timer - %d", ::GetLastError());
            hScanDelayTimer = NULL;
        }
    }
    else
        CCTRACEE ("Failed to create timer %d", ::GetLastError ());
    vecHandles.push_back (hScanDelayTimer);

	for(;;)
	{
		// Wait here for something to happen
		DWORD dwWait = WaitForMultipleObjects( vecHandles.size(), &vecHandles[0], FALSE, INFINITE );
		
		// Check for error.
		if( dwWait == WAIT_FAILED )
		{
            assert(false);
			return 0xFFFFFFFF;
		} 
		else if( dwWait - WAIT_OBJECT_0 == 0 )
		{
			// exit signaled.
			break;
		}

        // Scan
        if ( 3 == dwWait - WAIT_OBJECT_0 )
        {
            CCTRACEI("COptionsMonitorThread::Doing startup scan");
		    // Perform startup scans.
            _Module.DoStartupScan();

            // Take our timer off the wait handle list
            CloseHandle (hScanDelayTimer);
            vecHandles.pop_back();
        }
        else
        {
            // If the bloodhound level changes force AVAPI to reload in order
            // to use the updated level
            bool bForceReloadAVAPI = false;
            DWORD dwCurBloodhound = _Module.Config().GetBloodhoundLevel();
            bool bDetectSpyware = _Module.Config().GetDetectSpyware();

		    // Reload the options.
		    _Module.ReloadOptions();

            // Check the new bloodhound level
            if( dwCurBloodhound != _Module.Config().GetBloodhoundLevel() )
                bForceReloadAVAPI = true;
            
            // Has the Spyware detection been turned off?
            if ( bDetectSpyware != _Module.Config().GetDetectSpyware())
                bForceReloadAVAPI = true;

            // If the option change happens set the dirty bit for exclusions.
            // We will reload them the next time an event happens (if one happens).
            if( _Module.Config().GetDetectSpyware())
            {
                _Module.SetExclusionsDirty();
            }
            else
            {
                _Module.StopUsingExclusions();
            }

            // For now, re-initialize the driver.  If this becomes too costly,
            // we should come up with another global event specific to options
            // that the AP driver cares about.
            CCTRACEI ( "NAVAPSVC::OptionsMonitor - reloading AP - start" );
            _Module.ReloadAP(bForceReloadAVAPI);
            CCTRACEI ( "NAVAPSVC::OptionsMonitor - reloading AP - stop" );

            // Tell the world that spyware status has changed
	        CGlobalEvent event;
	        if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
	        {
		        ::PulseEvent ( event );
                CCTRACEI ("NAVAPSVC::OptionsMonitor - changing Spyware state to %d", _Module.Config().GetDetectSpyware());
	        }
            else
            {
                CCTRACEE ("NAVAPSVC::OptionsMonitor - failed creating AP event");
            }
        }

        // Flush out all unused memory
        SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1 );
    }

	return 0;
}