#include "StdAfx.h"
#include "StateDefs.h"
const TCHAR AVENGEDEFS_VAL[] = _T("AVENGEDEFS");
const TCHAR SYMANTEC_INSTALLED_APPS_KEY[] = _T("SOFTWARE\\Symantec\\InstalledApps");
const TCHAR SYMSETUP_MUTEX_NAME[] = _T("SYMSETUP_MUTEX");
#define STATE_DEFS_MAX_TIMEOUT 60*60*1000 // 1 hour
#include "atltime.h"

#include "ccEraserInterface.h"

// Our Hawking ID
//
const TCHAR g_szNAVUIDefUtilAppID [] = _T("StatusHP");

CStateDefs::CStateDefs(CWMIIntegration* pWMI, CNSCIntegration* pNSC):CState(pWMI,pNSC)
{
    // Get the defs directory so we know where to look for changes.
    //
    if ( makeEvent (SYM_REFRESH_VIRUS_DEF_STATUS_EVENT, false, m_eventDefs))
        m_vecWaitEvents.push_back (m_eventDefs);  

    ZeroMemory ( m_szDefsDir, MAX_PATH );

    DWORD dwType = 0;
    DWORD dwSize = MAX_PATH;
    HKEY  hKey = NULL;
    
    // open "InstalledApps" key
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYMANTEC_INSTALLED_APPS_KEY, 0, KEY_READ, &hKey))
    {
        // check for "AVENGEDEFS" value
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, AVENGEDEFS_VAL, 0, &dwType, (LPBYTE)m_szDefsDir, &dwSize))
        {
            if ((dwType == REG_SZ) && _tcslen(m_szDefsDir))
            {
                // convert pathname to short name, if possible
                TCHAR szShort[_MAX_PATH];
                if (GetShortPathName(m_szDefsDir, szShort, _MAX_PATH))
                    _tcsncpy(m_szDefsDir, szShort, _MAX_PATH-1);

                CCTRACEI ( "CStateDefs::CStateDefs() - defs Dir = %s", m_szDefsDir );
            }
        }

        RegCloseKey(hKey);
    }

    m_lOldStatus = AVStatus::statusNone;
    m_timeOldDefTime = 0;
    wcscpy ( m_szOldDefsDate, L"\0" );

    m_lEventID = AV::Event_ID_StatusDefs;           // What CC event ID is this for?
    m_lUpdateReason = AVStatus::Defs_Update_Type_None;
}

CStateDefs::~CStateDefs(void)
{
}

//////////////////////////////////////////////////////////////////////
// CStateDefs::Run()

int CStateDefs::Run()
{
    // Refresh the data the first time to make sure it's good.
    //
    if ( m_szDefsDir[0] == _T('\0'))
        return 0xFFFFFFFF;

    // Set up waiting objects.
	HANDLE aHandles[3];
	aHandles[0] = m_Terminating.GetHandle();  // Has a HANDLE operator.
	aHandles[1] = FindFirstChangeNotification( m_szDefsDir,
										       TRUE, 
											   FILE_NOTIFY_CHANGE_LAST_WRITE );
    aHandles[2] = m_vecWaitEvents.at(0);
	if( aHandles[1] == INVALID_HANDLE_VALUE )
	{
		CCTRACEE( _T("Error creating watch event for definitions") );
		return 0xFFFFFFFF;
	}

    ccLib::CMessageLock msgLock ( TRUE, TRUE );

	// Main thread loop.
	DWORD dwTimeElapsed = 0;
	DWORD dwWaitTime = STATE_DEFS_MAX_TIMEOUT;
	for(;;)
	{
		// Wait here for something to happen
        //
        DWORD dwWait = msgLock.Lock ( 3, aHandles, FALSE, dwWaitTime, TRUE );

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
            dwTimeElapsed = 0;
			
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
                dwWaitTime = WAIT_INTERVAL;
				FindNextChangeNotification( aHandles[1] );
				continue;
			}

            // Don't do this if symsetup is running since it could've been our uninstall that blasted the
            // defs
            ccLib::CMutex mutexSymSetup;
            if ( mutexSymSetup.Open(SYNCHRONIZE, FALSE, SYMSETUP_MUTEX_NAME, TRUE ) )
            {
                CCTRACEE( _T( "CStateDefs::Run() - SymSetup mutex exists. An install is running so aborting check for defs." ));
            }
            else
            {
		        CCTRACEI ("CStateDefs::Run() - It's time to check the defs. been 30 seconds since last write.");

			    // Ok, it's been more than 30 seconds since someone wrote to the
			    // definitions tree.  
                // New defs are here
                m_lUpdateReason = AVStatus::Defs_Update_Type_IU;
			    if (Save())
                    sendData();
            }

            // We don't care about any more writes at this point
		    FindCloseChangeNotification( aHandles[1] );

		    // Create new watch handle.
		    aHandles[1] = FindFirstChangeNotification(  m_szDefsDir,
													    TRUE, 
													    FILE_NOTIFY_CHANGE_LAST_WRITE );
		    if( aHandles[1] == INVALID_HANDLE_VALUE )
		    {
			    CCTRACEE( _T("Error creating watch event for definitions") );
			    return 0xFFFFFFFF;
		    }
        	
		    // Go back to a long wait.
		    dwWaitTime = STATE_DEFS_MAX_TIMEOUT;
            
		}else
        // Did we get an event directly from the LiveUpdate callback?
        if ( dwWait - WAIT_OBJECT_0 == 2 )
        {
            CCTRACEI ("CStateDefs::Run - got Defs changed event");

            // We don't care about any more writes at this point
            FindCloseChangeNotification( aHandles[1] );

            // Update
            m_lUpdateReason = AVStatus::Defs_Update_Type_LU;
            if (Save())
                sendData();

            // Reset
	        dwTimeElapsed = 0;
	        dwWaitTime = STATE_DEFS_MAX_TIMEOUT;
	        aHandles[1] = FindFirstChangeNotification( m_szDefsDir,
										               TRUE, 
											           FILE_NOTIFY_CHANGE_LAST_WRITE );
        }
	}

	// Clean things up.
	FindCloseChangeNotification( aHandles[1] );

    CCTRACEI ("CStateDefs::Run () - exited");

	return 0;
}

bool CStateDefs::Save ()
{
    CCTRACEI( _T("CStateDefs::save() - start"));
    // Only one save activity at a time since they all share the same data object
    ccLib::CSingleLock lockStatus (&m_critStatus, INFINITE, FALSE);

	// Use persisted data if available
	//
    if ( m_bLoadedOld && !m_bInit )
    {
        // Initialize the settings.
        //
        if ( m_edStatus.GetData ( AVStatus::propDefsStatus, m_lOldStatus ))
        {
            m_edStatus.GetData ( AVStatus::propDefsTime, (long) m_timeOldDefTime );
            formatDate ( AVStatus::propDefsDate, m_timeOldDefTime );
			// Don't need m_lUpdateReason
            CCTRACEI ( "CStateDefs::Save - using cached data %d, %d", m_lOldStatus, m_timeOldDefTime );
            return true; // we used persisted data
        }
        else
        {
            // Set defaults in case we error
            m_edStatus.SetData ( AVStatus::propDefsStatus, m_lOldStatus );
            m_edStatus.SetData ( AVStatus::propDefsTime, (DWORD) m_timeOldDefTime );
            formatDate ( AVStatus::propDefsDate, m_timeOldDefTime );
            m_edStatus.SetData ( AVStatus::propDefsUpdateType, m_lUpdateReason );
        }
    }

    time_t timeDefTime = 0;     // for sending to Status (could be updated to something cleaner)
    long lStatus = AVStatus::statusNotInstalled;
    WCHAR szDefsDate [64] = {0};
    bool bResult = false;
	bool bUpToDateDefs = false;

    // Get the date of the defs we are using.
	WORD wYear = 0, wMonth = 0, wDay = 0;
	DWORD dwRev = 0;
    bool bNewer = true;
    bool bTrusted = true;

    // Try to use the newest defs.  If this fails, it's probably because
	// there is a new set of defs on the machine that has not been integrated
	// yet, but the current process doesn't have rights to the hawkings tree.
	// In this case, just use the current defs.
    char szDefsDir[MAX_PATH] = {0};

     // Scope DefUtils
    {
        DefUtilsLoader defUtilsLoader;
        IDefUtilsPtr pIDefUtils;

        SYMRESULT result = defUtilsLoader.CreateObject(pIDefUtils.m_p);
        if( SYM_FAILED(result) )
        {
            CCTRACEE ("CStateDefs::save() - failed to create DefUtils object 0x%x. Exiting...", result);
            return false;
        }

        CCTRACEI ("CStateDefs::Run() - IDefUtils created");

        if (!pIDefUtils->InitWindowsApp( g_szNAVUIDefUtilAppID ))
        {
            CCTRACEE ("CStateDefs::save() - failed InitWindowsApp, exiting...");
            return false;
        }


        // Make sure we are using the latest def set
	    if (!pIDefUtils->UseNewestDefs( &bNewer ))
	    {
            CCTRACEE ("CStateDefs::save () - UseNewestDefs failed");
	    }
        CCTRACEI ("CStateDefs::save () - NewestDefs: %d", bNewer);

        // Get the defs directory we are using
        pIDefUtils->GetCurrentDefs( szDefsDir, MAX_PATH );
        CCTRACEI ("CStateDefs::save () - Current defs directory: %s", szDefsDir);

        if (!pIDefUtils->GetCurrentDefsDate( &wYear, &wMonth, &wDay, &dwRev ) ||
            0 == wYear )
        {
            CCTRACEE ("CStateDefs::save - failure getting current def date setting state to disabled");

            // Place the defs in an error state
            lStatus = AVStatus::statusDisabled;
        }
        // Are the current definitions authentic?
        else if( pIDefUtils->IsAuthenticDefs(szDefsDir, &bTrusted) )
        {
            if( !bTrusted )
            {
                CCTRACEE ("CStateDefs::save () - Current defs are not authentic. Setting state disabled for defs");
                lStatus = AVStatus::statusDisabled;
            }
            else
            {
                CCTRACEI ("CStateDefs::save () - Current defs are authentic: %s", szDefsDir);

                //
                // So far so good, now confirm valid ccEraser
                //
                SYMRESULT symRes;
                
                CString cszEraserFileName = szDefsDir;
                DWORD dwSizeNeeded = cszEraserFileName.GetLength() + _tcslen(cc::sz_ccEraser_dll) + 1;
                PathAppend(cszEraserFileName.GetBuffer(dwSizeNeeded), cc::sz_ccEraser_dll);
                cszEraserFileName.ReleaseBuffer();

                cc::CSymInterfaceTrustedLoader ccEraserLoader;
                symRes = ccEraserLoader.Initialize(cszEraserFileName);
                if( SYM_SUCCEEDED(symRes) )
                {
                    ccEraser::IEraserPtr spEraser;
                    symRes = ccEraserLoader.CreateObject(ccEraser::IID_Eraser, ccEraser::IID_Eraser, (void**)&spEraser);                  
                    if( SYM_SUCCEEDED(symRes) )
                    {
                        ccEraser::eResult resEraser = ccEraser::Success;

                        // If the eraser version is 103.5.x.x make sure it is at least 103.5.3.x
                        DWORD dwVerMost = 0;
                        DWORD dwVerLeast = 0;
                        spEraser->GetEngineVersion(dwVerMost, dwVerLeast);
                        CCTRACEI(_T("CStateDefs::save() - ccEraser::IEraser is v.%d.%d.%d.%d"), HIWORD(dwVerMost), LOWORD(dwVerMost), HIWORD(dwVerLeast), LOWORD(dwVerLeast));                        
                        if( (HIWORD(dwVerMost)) == 103 && (LOWORD(dwVerMost)) == 5 && (HIWORD(dwVerLeast)) < 3 )
                        {
                            // The eraser version is pre-103.5.3.x we will display the virus defs not authentic
                            // error and refuse to scan with this version of eraser
                            CCTRACEW(_T("CStateDefs::save() - This version of ERASER does not meet the minimum requirements."));
                            
                            lStatus = AVStatus::statusDisabled;
                        }
                    }
                    else
                    {
                        CCTRACEW(_T("CStateDefs::save() - CreateObject(IID_IEraser) failed. sr=0x%08X"), symRes);
                    }
                }
                else
                {
                    CCTRACEW(_T("CStateDefs::save() - Unable to load ccEraser from %. sr=0x%08X"), szDefsDir, symRes);
                }
            }
        }
        else
            CCTRACEE ("CStateDefs::save () - Authentication check in defutils failed for directory %s", szDefsDir);
    } // end scope DefUtils

	CCTRACEI ("CStateDefs::save () - DefsDate: %d/%d/%d.%d", wYear, wMonth, wDay, dwRev);

    // Local string for the dates, so clients do have to convert if they are lazy.
    wcscpy ( szDefsDate, L"" );

    // For non-disabled defs set the defTime and date to the correct values
    if( lStatus != AVStatus::statusDisabled )
    {
        try
        {
            ATL::CTime defTime( wYear, wMonth, wDay, 0, 0, 0 );

            // Convert def date
            //
            timeDefTime = defTime.GetTime ();

            lStatus = AVStatus::statusEnabled;
            bResult = true;
        }
        catch(...)
        {
            CCTRACEE ("CStateDefs::save - exception converting dates!");
            return false;
        }
    }
    // Check to see if ALU is running.
    if ( AVStatus::Defs_Update_Type_IU == m_lUpdateReason )
    {
        ccLib::CMutex ALUMutex;
        if ( ALUMutex.Open ( SYNCHRONIZE, FALSE, "Symantec.LuComServer.Running", FALSE ))
        {
            m_lUpdateReason = AVStatus::Defs_Update_Type_ALU;
            CCTRACEI(_T("CStateDefs::Save - ALU is currently running."));
        }
    }

    // Save data
    //
    if ( lStatus != m_lOldStatus ||
         timeDefTime != m_timeOldDefTime )
        bResult = true;

    m_lOldStatus = lStatus;
    m_timeOldDefTime = timeDefTime;
    wcscpy ( m_szOldDefsDate, szDefsDate );

    m_edStatus.SetData ( AVStatus::propDefsStatus, m_lOldStatus );
    m_edStatus.SetData ( AVStatus::propDefsTime, (long) m_timeOldDefTime );
    m_edStatus.SetData ( AVStatus::propDefsUpdateType, m_lUpdateReason );
    formatDate ( AVStatus::propDefsDate, m_timeOldDefTime );

    // Save data
    //
    CCTRACEI ( "CStateDefs::save() - exit" );

    return bResult;
}

