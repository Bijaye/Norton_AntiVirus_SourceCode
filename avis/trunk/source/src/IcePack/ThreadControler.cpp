// ThreadControler.cpp: implementation of the ThreadControler class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>
#include <CMclWaitableCollection.h>
#include <SystemException.h>

#include "ThreadControler.h"
#include "EventLog.h"
#include "Sample.h"
#include "VQSEnum.h"
#include "VQuarantineServer.h"
#include "Logger.h"
#include "GlobalData.h"
#include "bldinfo.h"

/* ----- */

using namespace std;

/* ----- */

static bool Started()
{
  return true;
}

/* ----- */

/*
 * Note that the order in which the member constructors are
 * called is critical.  In addition, it is not defined
 * by the order here, it's defined by the order of the
 * member declarations in the class definition.
 *
 * [inw 2000-06-05]
 */
ThreadController::ThreadController() : CMclThreadHandler(),
                                       started(Started()),
                                       registryMonitor(),
                                       blessedMonitor(),
                                       quarantineMonitor(),
                                       statusMonitor(),
                                       jobPackager(),
                                       scannerSubmittor(quarantineMonitor)
{
  EntryExit entryExit(Logger::LogIcePackOther, "ThreadController::ThreadController");
}

/* ----- */

ThreadController::~ThreadController()
{
}

/* ----- */

// ThreadController
//
// Overview:
//   This thread creates all of the other threads and then waits for all of those
//   threads to end
//
// Algorithm:
//   (Simplified version, real code is slightly different)
//   Create all of the other threads (registryMonitor, blessedMonitor,
//     quarantineMonitor, statusMonitor, deliveryMonitor, jobPackager,
//     and scannerSubmittor.
//   Wait for all of the threads to exit
//
// Globals static methods/members Used
//   None
unsigned ThreadController::ThreadHandlerProc()
{
  EntryExit entryExit(Logger::LogIcePackOther, "ThreadController::ThreadHandlerProc");

  SystemException::Init();

  try
  {
    CMclThread  registry(&registryMonitor);
    CMclThread  blessed(&blessedMonitor);
    CMclThread  quarantine(&quarantineMonitor);
    CMclThread  status(&statusMonitor);
    CMclThread  job(&jobPackager);
    CMclThread  sNs(&scannerSubmittor);

    CMclWaitableCollection  threads;

    threads.AddObject(registry);
    threads.AddObject(blessed);
    threads.AddObject(quarantine);
    threads.AddObject(status);
    threads.AddObject(job);
    threads.AddObject(sNs);

    if (GlobalData::stop)
      return -1;

    Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, "Waiting for threads to die");

    while (WAIT_TIMEOUT == threads.Wait(TRUE, 86400000))
      ;
  }

  catch (...)
  {
    GlobalData::HandleException("ThreadController::ThreadHandlerProc()",
                  Logger::LogIcePackOther);
  }

  return 0;
}
/* ----- End Transmission ----- */
