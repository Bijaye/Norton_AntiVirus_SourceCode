// IcePackServiceInterface.cpp: implementation of the IcePackServiceInterface class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>

#include "IcePackServiceInterface.h"
#include "ThreadControler.h"

#include "GlobalData.h"
#include "Logger.h"
#include "EventLog.h"

/* ----- */

using namespace std;

/* ----- */

void IcePackServiceInterface::Run()
{
  EntryExit entryExit(Logger::LogIcePackOther, "IcePackServiceInterface::Run");

  try
  {
    SetStatus(SERVICE_RUNNING, 0, 0, SERVICE_ACCEPT_STOP);

    ThreadController  threadController;

    threadController.ThreadHandlerProc();
  }

  catch (...)
  {
    GlobalData::HandleException("IcePackServiceInterface::Run()", Logger::LogIcePackOther);
  }

  SetStatus(SERVICE_STOP_PENDING, 2, 5000);
}

/* ----- */

void IcePackServiceInterface::OnStop()
{
  EntryExit entryExit(Logger::LogIcePackOther, "IcePackServiceInterface::OnStop");

  SetStatus(SERVICE_STOP_PENDING, 1, 6000);

  GlobalData::Stop();
}

/* ----- */

void IcePackServiceInterface::Init()
{
}

/* ----- */

void IcePackServiceInterface::DeInit()
{
}
/* ----- End Transmission ----- */
