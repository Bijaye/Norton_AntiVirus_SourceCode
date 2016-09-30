// IcePack.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <string>

#include <CServiceInstall.h>
#include "IcePackServiceInterface.h"
#include "ThreadControler.h"
#include "RegistryMonitor.h"
#include "Logger.h"
#include "EventLog.h"
#include "GlobalData.h"
#include "Attention.h"

/* ----- */

IMPLEMENT_SERVICE(IcePackServiceInterface, IcePack)

/* ----- */

using namespace std;

/* ----- */

static char *shortName   = "IcePack";
static char *displayName = "Symantec Quarantine Agent";

/* ----- */

int ServiceMain(void)
{
  EventLog::Init(displayName);

  Logger::ApplicationName("IcePack");
  LogLife logLife;

  try
  {
    EntryExit entryExit(Logger::LogIcePackOther, "ServiceMain");

    /*
     * First things first, unset the 'shutdown' attention doohickey.
     */
    Attention::UnSet(Attention::Shutdown);

    TCHAR* pszCmdLine = GetCommandLine();
    CharLowerBuff(pszCmdLine, lstrlen(pszCmdLine));
    if (_tcsstr(pszCmdLine, _T("-install")))
    {
      EventLog::Register(shortName, "IcePackEventMsgs.dll", 13);

      CServiceInstall si(shortName, displayName);
      si.Install( SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START,
            "qserver\0ScanExplicit\0\0");

      RegistryMonitor::InitializeRegistry();
    }
    else if (_tcsstr(pszCmdLine, _T("-remove")))
    {
      CServiceInstall si(shortName, displayName);
      si.Remove();

      RegistryMonitor::DeInitializeRegistry();
    }
    else if (_tcsstr(pszCmdLine, _T("-run")))
    {
      ThreadController  threadController;

      threadController.ThreadHandlerProc();
    }
    else  // must be running as a service
    {
      EventLog::StartMsg();

      IcePackServiceInterface service(shortName, displayName, SERVICE_WIN32_OWN_PROCESS);

      BEGIN_SERVICE_MAP
        SERVICE_MAP_ENTRY(IcePackServiceInterface, IcePack)
      END_SERVICE_MAP

      EventLog::StopMsg();
    }
  }

  catch (...)
  {
    GlobalData::HandleException("ServiceMain", Logger::LogIcePackOther);
  }

  try
  {
    Attention::Set(Attention::Shutdown);
  }
  catch (...)
  {
    GlobalData::HandleException("ServiceMainAttentionSetter", Logger::LogIcePackOther);
  }

  return 0;
}

/* ----- */

int main(int argc, char* argv[])
{
  return ServiceMain();
}
/* ----- End Transmission ----- */
