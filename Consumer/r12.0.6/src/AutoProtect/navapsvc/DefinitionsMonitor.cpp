// DefinitionsMonitor.cpp: implementation of the CDefinitionsMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DefsManager.h"
#include "DefinitionsMonitor.h"

//////////////////////////////////////////////////////////////////////
// CDefinitionsMonitor::CDefinitionsMonitor()

CDefinitionsMonitor::CDefinitionsMonitor() :
	m_bPause( false )
{
	// Intentionally empty.

    // Do we need COM???
}

//////////////////////////////////////////////////////////////////////
// CDefinitionsMonitor::~CDefinitionsMonitor()

CDefinitionsMonitor::~CDefinitionsMonitor()
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// CDefinitionsMonitor::Pause()

void CDefinitionsMonitor::Pause()
{
	// Set the pause flag.
	m_bPause = true;
}

//////////////////////////////////////////////////////////////////////
// CDefinitionsMonitor::Resume()

void CDefinitionsMonitor::Resume()
{
	// Clear the pause flag
	m_bPause = false;
}

//////////////////////////////////////////////////////////////////////
// CDefinitionsMonitor::notifyQueueThreadProc()

int CDefinitionsMonitor::Run()
{
    CDefsManager defsMgr;
    TCHAR szDefsDir [MAX_PATH] = {0};

    // Set up waiting objects.
	HANDLE aHandles[2];
	aHandles[0] = m_Terminating.GetHandle();
    if ( !defsMgr.GetRootDefsDirectory( szDefsDir, MAX_PATH ))
        return 0;

	aHandles[1] = FindFirstChangeNotification( szDefsDir,
										       TRUE, 
											   FILE_NOTIFY_CHANGE_LAST_WRITE );
	if( aHandles[1] == INVALID_HANDLE_VALUE )
	{
		_Module.LogEvent( "Error creating watch event for definitions" );
		return 0xFFFFFFFF;
	}

	// Main thread loop.
	DWORD dwTimeElapsed = 0;
	DWORD dwWaitTime = INFINITE;
	for(;;)
	{
		// Wait here for something to happen
		DWORD dwWait = WaitForMultipleObjects( 2, aHandles, FALSE, dwWaitTime );

		// Check for exit signal
		if( dwWait - WAIT_OBJECT_0 == 0 )
		{
			// Yes, bail out.
			break;
		} else 

		// Check to see if our FindFirstChangeNotification handle was signaled.
		if( dwWait - WAIT_OBJECT_0 == 1 )
		{
			// Switch to a timed wait.
			dwWaitTime = WAIT_INTERVAL;
			
			// Q: Have we suspended watching for new definitions at this moment?
			if( m_bPause == false )
			{
				// The watch handle signaled.  Reset the elapsed time since
				// last write counter and continue.
				dwTimeElapsed = 0;
			}

			// Reset event
			FindNextChangeNotification( aHandles[1] );
		} 
		// Check to see if we should attempt reloading definitions.
		else if( dwWait == WAIT_TIMEOUT )
		{
			// We don't want to do anything with the definitions yet.
			// Wait 30 seconds or so for things to settle down.
			if( dwTimeElapsed < UPDATE_INTERVAL )
			{
				// Not yet.
				dwTimeElapsed += WAIT_INTERVAL;
				FindNextChangeNotification( aHandles[1] );
				continue;
			}

            bool bDefsWereBad = _Module.m_bDefsBad;

			// Ok, it's been more than 30 seconds since someone wrote to the
			// definitions tree.  Try reloading.
            // Call into module to reload AP.
			_Module.ReloadAP();

            // If AP was set to load at startup and the defs have just become good enable it now
            if( bDefsWereBad && _Module.Config().GetLoadAtStartup() )
            {
                _Module.EnableAP(true);

                // Tell the world that we are enabled.
	            CGlobalEvent event;
	            if( event.Create( SYM_REFRESH_AP_STATUS_EVENT ) )
	            {
		            ::PulseEvent ( event );
	            }
            }

			// We don't care about any more writes at this point
			FindCloseChangeNotification( aHandles[1] );

			// Create new watch handle.
			aHandles[1] = FindFirstChangeNotification(  szDefsDir,
													 	TRUE, 
													 	FILE_NOTIFY_CHANGE_LAST_WRITE );
			if( aHandles[1] == INVALID_HANDLE_VALUE )
			{
				_Module.LogEvent( "Error creating watch event for definitions" );
				return 0xFFFFFFFF;
			}
			
			// Go back to an infinite wait.
			dwWaitTime = INFINITE;
		}
	}

	// Clean things up.
	FindCloseChangeNotification( aHandles[1] );

	return 0;
}