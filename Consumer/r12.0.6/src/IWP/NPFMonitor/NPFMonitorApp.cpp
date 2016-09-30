#include "StdAfx.h"

#include "cctrace.h"
#include "ccVersionInfo.h"
#include "NPFMonitorApp.h"
#include "InstallWatcher.h"

using namespace ccLib;

///////////////////////////////////////////////////////////////////////////////
// CNPFMonitorApp::CNPFMonitorApp()
CNPFMonitorApp::CNPFMonitorApp()
{
}

///////////////////////////////////////////////////////////////////////////////
// CNPFMonitorApp::~CNPFMonitorApp()
CNPFMonitorApp::~CNPFMonitorApp()
{
}

///////////////////////////////////////////////////////////////////////////////
// CNPFMonitorApp::Start()
int CNPFMonitorApp::Start()
{
    DWORD dwCheckPoint = 1;
    DWORD dwWaitHint = 5000;
    DWORD dwErrorSleep = 5000;

    // Notify SCM that we expect to be started shortly
    SetStarting(dwWaitHint, dwCheckPoint ++);

    // Log starting
    CCTRACEI(_T("CNPFMonitorApp::Start() : m_EventLog.ApplicationStarting()\n"));
    LogStarting();

    // Create console windows
    if (CreateConsoleWindows() == FALSE)
    {
        CCTRACEE(_T("CNPFMonitorApp::Start() : CreateConsoleWindows() == FALSE\n"));
        m_EventLog.TraceError(_T("Failed to create the console window, terminating process!\n"));
        return -1;
    }

    // Starting ...
    m_DebugWnd.Print(_T("Starting application ...\n")); 
    
    // System info
    CVersionInfo versionInfo;
    if (versionInfo.Load(NULL) != FALSE)
    {
        m_DebugWnd.Print(_T("Application Version: %s\n"), 
                    versionInfo.GetProductVersion());
    }
    m_DebugWnd.Print(_T("Administrative Rights: %s\n"),
                 m_OSInfo.IsAdministrator() ? _T("Yes") : _T("No")); 
    m_DebugWnd.Print(_T("Terminal Services: %s\n"),
                 m_OSInfo.IsTerminalServer() ? _T("Yes") : _T("No")); 
    m_DebugWnd.Print(_T("Window Mode: %s\n"),
                 IsWindow() ? _T("Yes") : _T("No")); 
    m_DebugWnd.Print(_T("Pumping Messages: %s\n"),
                 m_bPumpMessages != FALSE ? _T("Yes") : _T("No")); 
	
	// Notify SCM that we expect to be started shortly
    SetStarting(dwWaitHint, dwCheckPoint ++);

    int nRet = 0;

    // create the install watcher thread
	CInstallWatcher InstallWatcher;
	if(InstallWatcher.Create(NULL, 0, 0) == FALSE)
	{
		CCTRACEI(_T("CNPFMonitorApp::Start() : Registry listening thread did not start."));
	}

	// Notify SCM that we have started
    SetStarted();

    // Log start
    CCTRACEI(_T("CNPFMonitorApp::Start() : m_EventLog.ApplicationStarted()\n"));
    LogStarted();

    // Ready
    m_DebugWnd.Print(_T("Application started.\n"));

    // Trim the workingset size
    if (m_OSInfo.IsWinNT() != FALSE)
    {
        SetProcessWorkingSetSize(GetCurrentProcess(), 
                                    SIZE_T(-1),
                                    SIZE_T(-1));
    }
	
	
	if(InstallWatcher.IsThread())
	{
		// Wait for terminate signal
		CCTRACEI(_T("CNPFMonitorApp::Start() : WaitForTerminate() Enter\n"));

		while(!WaitForTerminate(1*60*1000 /*1 minute*/))
		{
			// check if the thread is still running. if it's not then we have no reason to stick around. shutdown.
			if(!InstallWatcher.IsThread())
			{
				CCTRACEI(_T("CNPFMonitorApp::Start() : InstallWatcher.IsThread() == FALSE.  Sevice Shutting Down.\n"));
				break;
			}
		}

		CCTRACEI(_T("CNPFMonitorApp::Start() : WaitForTerminate() Exit\n"));
	}

	if(InstallWatcher.IsThread())
	{
		m_DebugWnd.Print(_T("Terminating threads...\n"));
		if(InstallWatcher.Terminate(1*60*1000 /*1 minute*/) == FALSE)
		{
			CCTRACEI(_T("CNPFMonitorApp::Start() : Registry listening thread did not terminate within timeout period"));
		}
	}

    // Log stopping
    CCTRACEI(_T("CNPFMonitorApp::Start() : m_EventLog.ApplicationTerminating()\n"));
    LogStopping();
	
	// Terminating
    m_DebugWnd.Print(_T("Application terminating...\n"));

	// Notify SCM that we are shutting down shortly
    dwCheckPoint = 1;

	// Notify SCM that we are shutting down shortly
	SetStopping(dwWaitHint, dwCheckPoint ++);

    // Notify SCM that we are shutting down shortly
    SetStopping(dwWaitHint, dwCheckPoint ++);

    // Notify SCM that we are shutting down shortly
    SetStopping(dwWaitHint, dwCheckPoint ++);

	// Log terminate
    CCTRACEI(_T("CNPFMonitorApp::Start() : m_EventLog.ApplicationTerminated()\n"));
    LogStopped();

    // Terminated
    m_DebugWnd.Print(_T("Application terminated.\n"));

    if( IsDebugging() )
    {
        m_DebugWnd.Print(_T("Sleeping %lu seconds...\n"), dwErrorSleep / 1000);
        Sleep(dwErrorSleep);
    }

    // Destroy console windows
    DestroyConsoleWindows();

    return nRet;
}

///////////////////////////////////////////////////////////////////////////////
// CNPFMonitorApp::CreateConsoleWindows()
BOOL CNPFMonitorApp::CreateConsoleWindows()
{
    // Only create console windows if we are debugging
    if (IsDebugging() == FALSE)
    {
        return TRUE;
    }

    // Clear the default window and associate the window with 
    // VK_F1 | LEFT_CTRL_PRESSED
    m_DebugWnd.Clear();
    m_DebugWnd.SetTitle(_T("NPMonitor Service"));
    m_Console.Add(VK_F1 | LEFT_CTRL_PRESSED, &m_DebugWnd);

    // Create the Debug window and associate the window with 
    // VK_F3 | LEFT_CTRL_PRESSED
    m_DebugWnd.Create(ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT, m_Console.GetScreenBufferInfo(), _T("ccSettings Debug Output"));
    m_Console.Add(VK_F3 | LEFT_CTRL_PRESSED, &m_DebugWnd);

    // Activate the default window
    m_DebugWnd.Activate(&m_Console);
    
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CNPFMonitorApp::DestroyConsoleWindows()
BOOL CNPFMonitorApp::DestroyConsoleWindows()
{
    // Only destroy console windows if we are debugging
    if (IsDebugging() == FALSE)
    {
        return TRUE;
    }

    m_DebugWnd.Destroy();

    return TRUE;
}