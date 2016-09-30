// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation, All rights reserved.

#define _WIN32_WINNT 0x0501
#include "pscan.h"
#include "scanstatus.h"
#include "walker.h"
#include "uimanager.h"
#include "scanlistener.h"
#include "wtsloader.h"
#include "wtssessionpublisher.h"

CScanListener CScanListener::g_objScanListenerInstance;

CScanListener& CScanListener::GetInstance()
{
    CGuard objGuard(g_objScanListenerInstance.m_objLock);
    if( false == g_objScanListenerInstance.m_bInitialized )
    {
        CWtsSessionPublisher::Instance()->UpdateListener(g_objScanListenerInstance);
        g_objScanListenerInstance.m_bInitialized = true;
    }

    return g_objScanListenerInstance;
}

CScanListener::CScanListener(void):m_bInitialized(false)
{
}

CScanListener::~CScanListener(void)
{
}

void CScanListener::SessionChange(const CWtsSessionEvent& objEvent)
{
    switch (objEvent.GetEventType())
	{
		case WTS_SESSION_LOGON:
        	ProcessLogon();
			break;
		case WTS_SESSION_LOGOFF:
            break;
        case WTS_CONSOLE_DISCONNECT:
        case WTS_CONSOLE_CONNECT:
        case WTS_REMOTE_DISCONNECT:
        case WTS_REMOTE_CONNECT:
		default:
			// Ignore other types of events that we aren't interested in but log these.
			dprintfTag2(DEBUGSCAN, _T("Scan Listner: Ignoring session change event: %u for session ID: %u\n"), objEvent.GetEventType(), objEvent.GetSessionId());
			break;
	}
}

/** void ProcessLogon() **/
void CScanListener::ProcessLogon()
{
    CScanStatus* pobjScanStatus = NULL;
    if( ERROR_SUCCESS == GetActiveScan(pobjScanStatus) )
    {
        SAVASSERT(pobjScanStatus != NULL);
        if( pobjScanStatus != NULL  && 
            GetVal(pobjScanStatus->hkey,szReg_Value_DisplayStatusDlg,0) &&
            false == CUIManager::GetInstance().ResultsViewExists(pobjScanStatus->ScanID, LOGGER_Manual) )
        {
			// An 'After hours scan' occurs when a user defined scheduled scan begins while the user that created the scan
			// is not logged in.  The result is a user scan running in system context (no impersonation).  We currently
			// do not support showing UI in this scenario.  If it's determined that we are AHS, do not show the results view.
			// If this is an admin scan, allow the UI.
			bool bAfterHoursScan = pobjScanStatus->logger == LOGGER_Scheduled && !pobjScanStatus->GetIsCurrentUsersScan() && !pobjScanStatus->GetIsAdminDefinedScan();

			if( !bAfterHoursScan )
			{
				pobjScanStatus->SetSessionID(CWTSLoader::GetInstance().GetActiveConsoleSessionId());
				CUIManager::GetInstance().CreateResultsViewProxy(*pobjScanStatus);
			}
        }
    }

}

