// NPFMonitor.cpp : Implementation of WinMain

#include "stdafx.h"
#include "NPFMonitorApp.h"
#include "ccSymCrashHandler.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

// The main application
CNPFMonitorApp _App;

// cc crash handler: this will give us crash dumps for easier debugging
ccSym::CCrashHandler g_CrashHandler;

///////////////////////////////////////////////////////////////////////////////
// Main program entry point
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    // Enable the crash handler
    if (g_CrashHandler.LoadOptions() == FALSE)
    {
        CCTRACEE(_T("WinMain() : g_CrashHandler.LoadOptions() == FALSE"));
    }

    ATL::CString strServiceName;
    strServiceName.LoadString (IDS_SERVICENAME);

    ATL::CString strServiceDisplayName;
    strServiceDisplayName.LoadString (IDS_SERVICE_DISPLAY_NAME);

    ATL::CString strServiceDescriptionName;
    strServiceDescriptionName.LoadString (IDS_SERVICE_DESCRIPTION);

    return _App.Run( strServiceName,
                     strServiceDisplayName,
                     strServiceDescriptionName,
					 NULL,
                     SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
}