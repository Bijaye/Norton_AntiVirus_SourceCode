#include "StdAfx.h"
#include ".\scanmonitor.h"

CScanMonitor::CScanMonitor(CScanMonitorSink* pSink)
{
    m_pSink = pSink;
    m_Options.m_bWaitForInitInstance = TRUE;
}

CScanMonitor::~CScanMonitor(void)
{
}

BOOL CScanMonitor::InitInstance(void)
{
    if ( !m_pSink )
        return FALSE;

    m_eventNewWork.Create ( NULL, FALSE, FALSE, NULL, FALSE );
    return TRUE;
}

int CScanMonitor::Run (void)
{
    ccLib::CMessageLock msgLock ( TRUE, FALSE );

    while ( true )
    {
        std::vector<HANDLE> vecWaitEvents;
        vecWaitEvents.push_back (m_Terminating.GetHandle());
        vecWaitEvents.push_back (m_eventNewWork.GetHandle());

        for ( iterScans iter = m_setScans.begin();
              iter != m_setScans.end();
              iter++)
        {
            vecWaitEvents.push_back (*iter);
        }

		// Wait here for something to happen
        DWORD dwWaitResult = msgLock.Lock ( vecWaitEvents.size(), &vecWaitEvents[0], FALSE, INFINITE, FALSE );

        if ( WAIT_FAILED == dwWaitResult )
        {
            CCTRACEE ("Wait failed in CVirusAlertQueue");
    		break;
        }

		// exit signaled.
		if (WAIT_OBJECT_0 == dwWaitResult)
			return 0;

		if (dwWaitResult > WAIT_OBJECT_0+1)
        {
            // Scan finished
            if (m_pSink)
            {
                iterScans iter = m_setScans.find (vecWaitEvents.at(dwWaitResult));
                if ( iter != m_setScans.end())
                {
                    CCTRACEI ("CScanMonitor::Run - scan finished %d", (HANDLE)(*iter));
                    ccLib::CSingleLock lock (&m_critEvents, INFINITE, FALSE);
                    m_pSink->ScanFinished ((HANDLE)(*iter));
                    m_setScans.erase (iter);
                }                
            }
        }

    }
    return 1;
}

void CScanMonitor::AddScan (HANDLE hHandle)
{
    CCTRACEI ("CScanMonitor::AddScan - new scan %d", hHandle);    
    ccLib::CSingleLock lock (&m_critEvents, INFINITE, FALSE);
    m_setScans.insert(hHandle);
    m_eventNewWork.SetEvent();
}

bool CScanMonitor::IsEmpty (void)
{
    ccLib::CSingleLock lock (&m_critEvents, INFINITE, FALSE);
    return (m_setScans.empty());
}
