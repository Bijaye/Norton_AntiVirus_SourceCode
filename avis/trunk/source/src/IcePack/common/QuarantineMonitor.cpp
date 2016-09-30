// QuarantineMonitor.cpp: implementation of the QuarantineMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <algorithm>
#include <assert.h>
#include <iostream>

#include <CMclAutoLock.h>
#include <AttributeKeys.h>
#include <XAnalysisState.h>
#include <SystemException.h>

#include "EventLog.h"

#include "QuarantineMonitor.h"
#include "IcePackException.h"

#include "GlobalData.h"
#include "SampleStatus.h"

#include <Logger.h>
//#include "IcePackMonitor.h"

#include <ScannerSubmittor.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//
//  QuarantineMonitor::QuarantineMonitor()
//
//  Overview:
//    This constructor takes a first look at the samples stored in
//    the quarantine server and builds the master list for the other
//    thread object constructors.  It also signals some event objects
//    if there is work for other threads to do.
//
//  Algorithm:
//    (Simplified version, real code is slightly different)
//    For each sample in the quaranine server not in a final state
//      If the sample has never been touched by IcePack or Scan and Deliver
//        Prep the sample for use inside of IcePack (add attributes, scan, etc).
//      If the sample is in the released state
//        signal that a new sample is ready for submission to the gateway
//      else if the sample is in the needed state
//        if the latest signature available to IcePack is newer than the needed signature
//          change the sample's state to available
//        else
//          request that the needed signature be downloaded from the gateway
//      If the sample is in the available state
//        Signal the job packager that a definition set needs to be delivered
//      If the sample is still not in a final state
//        Add to the master sample list
//    Sort the master sample list
//    DeActivate each sample object (release handle to quarantine server)
//        


QuarantineMonitor::QuarantineMonitor() : CMclThreadHandler(),
          monitorQuarantine(TRUE, TRUE),
          quarantineMonitorNotActive(TRUE, TRUE),
          stopMonitoringQuarantine(false)
{
//  IcePackMonitor::IcePackState("Initializing: Doing first pass through the quarantine list");
  QSThreadLife		qsThreadLife;		// initializes com for this thread

  bool  blessed;
  uint  neededSig     = 0;
  bool  signalSubmitSample  = false;
  bool  signalJobPackager = false;
  bool  newRecord     = false;
  VQSEnum qsEnum;
  Sample  sample;
  list<Sample>  emptyList;
  list<Sample>::iterator  i;
  VQuarantineServer	quarantineServer;


  try
  {
    for (quarantineServer.GetVQSEnum(qsEnum);
      !GlobalData::stop && qsEnum.Next(sample, emptyList, newRecord, i); )
    {
      try
      {
        /*
         * It turns out that under certain circumstances, valid samples
         * which appear to the customer to be 'released' could be
         * ignored by IcePack.  Specifically, these cases were:
         *
         *  * non-zero priorty, status == held
         *     This happens when IcePack initialises a sample,
         *     and holds it.  IcePack is then stopped, the sample
         *     is 'queued' from the GUI, and IcePack restarted.
         *     In this case, IcePack ignored the sample forever.
         *     Action: release the sample.
         *  * zero priority, status == released
         *     This is the opposite of the above case.
         *     Action: hold the sample.
         *
         * inw 2000-03-01.
         */
        if (0 == sample.Priority())
        {
          if (SampleStatus::released == sample.Status())
          {
            sample.Status(SampleStatus::held);
            sample.Commit();
          }
        }
        else
        {
          if (SampleStatus::held == sample.Status())
          {
            sample.Status(SampleStatus::released);
            sample.Commit();
          }
        }

        if (SampleStatus::quarantined == sample.Status())
        {
          PrepNewSample(sample);
        }

        if (SampleStatus::released == sample.Status())
          signalSubmitSample  = true;
        else if (SampleStatus::needed == sample.Status())
        {
          if (sample.NeededSeqNum() <= GlobalData::GetNewestSigAvailable(blessed))
            sample.Status(SampleStatus::available);
          else if (sample.NeededSeqNum() > neededSig)
            neededSig = sample.NeededSeqNum();
        }

        if (SampleStatus::available == sample.Status())
        {
          signalJobPackager = true;
        }

        if (!sample.FinalStatus())
          GlobalData::samplesList.push_back(sample);
      }
      catch (...)
      {
        GlobalData::HandleException("QuarantineMonitor constructor, sample processing",
                      Logger::LogIcePackQSMonitor);
      }
    }
  }
  catch (...)
  {
    GlobalData::HandleException("QuarantineMonitor constructor, enumeration",
                  Logger::LogIcePackQSMonitor);
  }

  if (GlobalData::stop)
    return;

  GlobalData::samplesList.sort();

  for (i = GlobalData::samplesList.begin();
     i != GlobalData::samplesList.end(); i++)
  {
    i->DeActivate();
  }

  if (neededSig > 0)
    GlobalData::SetSigToDownload(neededSig, false);

  if (signalSubmitSample)
    GlobalData::submitSample.Set();   // signal submit sample

  if (signalJobPackager && GlobalData::AutoDefinitionDelivery())
  {
    Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor,
              "Signal signatures to download to clients");

    GlobalData::deliverSignature.Set(); // signal new jobs!
  }
}

QuarantineMonitor::~QuarantineMonitor()
{

}

bool QuarantineMonitor::Pause(void)
{
  EntryExit entryExit(Logger::LogIcePackQSMonitor, "Pause");

  monitorQuarantine.Reset();
  stopMonitoringQuarantine = true;

  /*
   * The five minute wait is arbitrary.
   */
  DWORD rc = quarantineMonitorNotActive.Wait(5 * 60 * 1000);

  if (WAIT_OBJECT_0 != rc)
    throw IcePackException(IcePackException::QuarantineMonitorPause, "WAIT_OBJECT_0 != rc");

  return true;
}

bool QuarantineMonitor::Resume(void)
{
  EntryExit entryExit(Logger::LogIcePackQSMonitor, "Resume");

  stopMonitoringQuarantine = false;
  monitorQuarantine.Set();

  return true;
}


//
//  QuarantineMonitor::PrepNewSample(Sample& sampleToPrep)
//
//  Overview:
//    This method takes the sample passed to it and preps it for use with
//    IcePack.  Basically it performs any operation that needs to be performed
//    once to any sample that is used by IcePack.
//
//  Algorithm:
//    (Simplified version, real code is slightly different)
//    If the sample has not been initialized before
//      set the X-Customer-* attributes based on registry values stored in
//        the GlobalData class
//      mark the sample as initialized by IcePack
//    Check that the size stored in the attributes is the same as the size stored
//      in the quarantine server (if not mark the sample as bad).
//    If the sample is not in a valid IcePack processing state
//      move the sample to released or held based on the default sample priority
//        stored in the database.
//      Set X-Analysis-State to Captured
//      Set X-Sample-Submission-Route to 1 (IcePack)
//    If the sample is in the held or released state
//      Scan the sample for viruses (state may change in this call)

void QuarantineMonitor::PrepNewSample(Sample& sample)
{
  bool  wasSampleInitialisedWhenIStarted = true;
  bool  continueToWorkWithSample         = true;
  char  buffer[128];

  sprintf(buffer, "New sample(%lx) found", sample.SampleKey());
  Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor, buffer);

  try
  {
    string  initialized;
    sample.GetAttribute(AttributeKeys::IcePackInitialized(), initialized);

    if (initialized == "")  // not initialized yet
    {
      /*
       * Toggle the boolean for the scanner later on.
       */
      wasSampleInitialisedWhenIStarted = false;

      /*
       * Arrange for the attributes to be saved...
       */
      sample.preserveAttributes();

      sample.SetAttribute(AttributeKeys::CustomerContactName(),      GlobalData::ContactName());
      sample.SetAttribute(AttributeKeys::CustomerContactTelephone(), GlobalData::ContactTelephone());
      sample.SetAttribute(AttributeKeys::CustomerContactEmail(),     GlobalData::ContactEmail());
      sample.SetAttribute(AttributeKeys::CustomerName(),             GlobalData::CompanyName());
      sample.SetAttribute(AttributeKeys::CustomerID(),               GlobalData::CustomerAccount());

      /*
       * Is there an X-Submission-Count attribute?  If not, then create
       * one with the value '0'.  In addition, if there isn't one,
       * then we need to set the initial submission priority to the
       * correct value.  This is now done only if there is no
       * X-Submission-Count attribute, as on a resubmission, we need
       * to leave the submission priority the same as it was before.
       * We also set the sig distribution priority in here, and only
       * if there's no submission count attribute (same reasons).
       */
      string subCount;
      sample.GetAttribute(AttributeKeys::SubmissionCount(), subCount);
      if (0 == subCount.size())
      {
        std::string sampleReason;

        /*
         * Initialise the submission counter.
         */
        sample.SetAttribute(AttributeKeys::SubmissionCount(), "0");

        /*
         * Set the initial priority appropriately.
         */
        sample.GetAttribute(AttributeKeys::SampleReason(), sampleReason);
        if (sampleReason == "manual")
          sample.Priority(0);
        else
          sample.Priority(GlobalData::InitialSubmissionPriority());

        /*
         * Set the sig distribution priority appropriately.
         */
        sprintf(buffer, "%d", GlobalData::DefUnblessedPointcast());
        sample.SetAttribute(AttributeKeys::SignaturePriority(), buffer);
      }

      sample.SetAttribute(AttributeKeys::IcePackInitialized(), "true");

      sample.Commit();

      Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor,
              "New sample initialized for use by IcePack");
    }

    //
    //  Check ths size stored in the attributes with the size stored in
    //  the quarantine server.

    ulong qsSize = 0;
    string  attSize("0");

    if (sample.GetAttribute(AttributeKeys::SampleFileSize(), attSize))
    {
      ulong attSize_ = atol(attSize.c_str());
      
      if (0 == attSize_)
      {
        sample.SetAttribute(AttributeKeys::AnalysisState(), "empty");
        sample.Status(SampleStatus::unneeded);
        sample.Commit();

        /*
         * We should stop now to prevent falling into any of the other
         * cases. [inw 2000-06-17]
         */
        continueToWorkWithSample = false;
      }
      else
      {
        sample.Size(qsSize);

        if (attSize_ != qsSize)
        {
          char  errMsg[128];
          sprintf(errMsg, "Size (%d) stored in sample does not match size (%d) for content returned from Quarantine Service.",
                  attSize_, qsSize);

          sample.SetAttribute(AttributeKeys::Attention(), errMsg);
          sample.Status(SampleStatus::attention);
          sample.Commit();

          Logger::Log(Logger::LogWarning, Logger::LogIcePackQSMonitor, errMsg);

          /*
           * We should stop now to prevent falling into any of the other
           * cases. [inw 2000-06-17]
           */
          continueToWorkWithSample = false;
        }
      }
    }
    else
    {
      const char* errMsg = "Unable to retrieve required attribute, X-Sample-Size";
      sample.SetAttribute(AttributeKeys::Attention(), errMsg);
      sample.Status(SampleStatus::attention);
      sample.Commit();

      Logger::Log(Logger::LogError, Logger::LogIcePackQSMonitor, errMsg);

      /*
       * We should stop now to prevent falling into any of the other
       * cases. [inw 2000-0617]
       */
      continueToWorkWithSample = false;
    }

    if (continueToWorkWithSample)
    {
      //
      //  Set the sample state to released or held based on the sample's
      //  priority if the sample has not yet entered IcePack's processing.
      if (SampleStatus::unknown   == sample.Status()  ||
        SampleStatus::quarantined == sample.Status()  )
      {
        if (sample.Priority() > 0)
        {
          sample.Status(SampleStatus::released);

          Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor, "Sample set to \"released\" state");
        }
        else
        {
          sample.Status(SampleStatus::held);

          Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor, "Sample set to \"held\" state");
        }

        sample.SetAttribute(AttributeKeys::AnalysisState(), XAnalysisState::Captured());
        sample.SetAttribute(AttributeKeys::SampleSubmissionRoute(), "1");
        sample.Commit();
      }

      /*
       * Rescan block.
       * Heavily modified inw 2000-05-16 as a workaround for the now-famous
       * amnesia problem.  It appears that, in certain circumstances,
       * IcePack 'forgets' about some samples in the master list.  This is
       * bad, as it means that it never goes back to check them again,
       * even when the next enumeration of central quarantine finds them.
       * The solution isn't a solution, it's a workaround, and it is to
       * scan more files when they are discovered in quarantine.  To be
       * precise, to scan any file that has not already been scanned by the
       * currently-available highest numbered identity set.
       */
      {
        bool        blessed;
        uint        identitiesAvailable      = 0;
        std::string identitiesScannedWithStr = "";
        uint        identitiesScannedWith    = 0;

        /*
         * ScanMeNow is initialised to the opposite of was...IStarted because
         * we must scan if we haven't seen the sample before.  If we have seen
         * the sample before, we should do the identity sequence number check
         * (see later).
         */
        bool        scanMeNow                = (! wasSampleInitialisedWhenIStarted);

        /*
         * First, find out the most recent identity set available locally.
         * 'blessed' is filled in by the call, and is set to true if
         * the most recent identity set is blessed.
         */
        identitiesAvailable = GlobalData::GetNewestSigAvailable(blessed);

        /*
         * The old code would always rescan if the sample status was 'held'
         * or 'released'.  This made (apparently) no sense, so it's gone.
         * Also, we know the sample is not in final state by the time we get
         * here, so there is no need to test for that.
         */

        /*
         * This is done this way to avoid attempting to get an attribute which
         * may either not be there, or may have been (helpfully) set by
         * the client to some sequence number that we don't have.
         */
        if (scanMeNow)
        {
          sprintf(buffer, "Deciding to scan sample %lx [not seen before]", sample.SampleKey());
          Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor, buffer);
        }
        else
        {
          /*
           * Now get, from the sample, the sequence number of the set of identities
           * with which the sample was last scanned.
           */
          sample.GetAttribute(AttributeKeys::ScanSignaturesSequence(), identitiesScannedWithStr);
          identitiesScannedWith = atoi(identitiesScannedWithStr.c_str());

          /*
           * If the latest available number is greater than the number with which the sample
           * was last scanned, then scan it.
           */
          if (identitiesScannedWith < identitiesAvailable)
          {
            sprintf(buffer, "Deciding to scan sample %lx [sequence %08d < %08d]", sample.SampleKey(), identitiesScannedWith, identitiesAvailable);
            Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor, buffer);
            scanMeNow = true;
          }
          else
          {
            sprintf(buffer, "Not scanning sample %lx [sequence %08d !< %08d]", sample.SampleKey(), identitiesScannedWith, identitiesAvailable);
            Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor, buffer);
          }
        }

        /*
         * Now, if the earlier tests suggested we need to rescan, then do so.
         */
        if (scanMeNow)
        {
          NAVScan::ScanRC scanRC;
        
          /*
           * Request a scan of this sample.  Tell the Scan() method whether or not
           * this is the 'initial' scan of the sample (this means the 'initial scan
           * of the sample _by_IcePack_').
           */
          scanRC = sample.Scan(identitiesAvailable, (! wasSampleInitialisedWhenIStarted));
        }
      }
    }
  }

  catch (IcePackException& e)
  {
    string  errMsg("Exception of type IcePackException caught in QuarantineMonitor::PrepNewSample [");
    errMsg  += e.FullString();
    errMsg  += "]";

    Logger::Log(Logger::LogError, Logger::LogIcePackQSMonitor, errMsg.c_str());

    sample.SetAttribute(AttributeKeys::Attention(), errMsg);
    sample.Status(SampleStatus::attention);
    sample.Commit();
  }
  catch (...)
  {
    string  typeInfo(""), detailedInfo(""), errMsg("Exception of type ");
    if (GlobalData::ExceptionTyper(typeInfo, detailedInfo))
    {
      errMsg    += typeInfo;
      errMsg    += " caught in QuarantineMonitor::PrepNewSample [";
      errMsg    += detailedInfo;
      errMsg    += "]";
      Logger::Log(Logger::LogError, Logger::LogIcePackQSMonitor, errMsg.c_str());

      sample.SetAttribute(AttributeKeys::Attention(), errMsg.c_str());
    }
    else
    {
      Logger::Log(Logger::LogError, Logger::LogIcePackQSMonitor, "Exception of unknown type caught in PrepNewSample");

      sample.SetAttribute(AttributeKeys::Attention(), "Exception of unknown type caught in QuarantineMonitor::PrepNewSample");
    }

    sample.Status(SampleStatus::attention);
    sample.Commit();

  }
}

//
//  QuarantineMonitor
//
//  Overview:
//    This thread monitors the Quarantine Server and updates the master samples list
//    to match the values stored in the Quarantine Server.
//
//  Algorithm:
//    (Simplified version, real code is slightly different)
//    Do
//      If quarantine monitor has not been paused by ScannerSubmittor
//        Make a copy of the master sample list to work with.
//        Call LookForChanges method to find changes and update the master sample list
//      Sleep for QuarantineInterval seconds
//    while (IcePack is not shutting down)
//
//

#ifdef TIMEING_TESTS
#define TT_VARS \
  int   oldSampleCount = 0;                   \
  int   newSampleCount = 0;                   \
  clock_t loopClockTicksStart, loopClockTicksEnd;         \
  clock_t scanPendingClockTicksStart, scanPendingClockTicksEnd; \
  char  messageBuffer[512];

#define TT_LOOP_TOP \
  loopClockTicksStart   = clock();

#define TT_LOOP_BOTTOM \
  loopClockTicksEnd = clock();            \
  newSampleCount = GlobalData::samplesList.size();  \
  sprintf(messageBuffer, "QuarantineMonitor with %d samples took %8.4f seconds\n",    \
      newSampleCount,               \
      ((float)(loopClockTicksEnd - loopClockTicksStart))/((float) CLOCKS_PER_SEC)); \
  cout << messageBuffer;                \
  cout.flush();                   \
                            \
  if (newSampleCount != oldSampleCount && 0 == GlobalData::MaxPendingSamples()) \
  {                         \
    oldSampleCount = newSampleCount;        \
                            \
    string  defVersion;               \
    bool  dmmy;                 \
                            \
    listCopy.clear();               \
    {                       \
      CMclAutoLock  lock(GlobalData::critSamplesList);  \
      i = GlobalData::samplesList.begin();    \
                            \
      for (; i != GlobalData::samplesList.end(); i++) \
      {                     \
        listCopy.push_back(*i);         \
        listCopy.back().Mark(Sample::sDelete);  \
      }                     \
    }                       \
    NAVScan::ExplicitDefVersion(defVersion, 99999); \
                            \
    scanPendingClockTicksStart  = clock();      \
    ScannerSubmittor::ScanPendingSamples(listCopy, dmmy); \
    scanPendingClockTicksEnd  = clock();      \
    sprintf(messageBuffer, "Scanning of %d samples (including def reload) took %8.4f seconds\n",    \
        listCopy.size(),            \
        ((float)(scanPendingClockTicksEnd - scanPendingClockTicksStart))/((float) CLOCKS_PER_SEC)); \
    cout << messageBuffer;              \
    cout.flush();                 \
  }

#else
#define TT_VARS
#define TT_LOOP_TOP
#define TT_LOOP_BOTTOM
#endif


unsigned QuarantineMonitor::ThreadHandlerProc()
{
  EntryExit   entryExit(Logger::LogIcePackQSMonitor, "ThreadHandlerProc");
  //  EnterExitThread enterExitThread(IcePackMonitor::quarantineMonitor);
  QSThreadLife		qsThreadLife;		// initializes com for this thread
  VQuarantineServer	quarantineServer;


  SystemException::Init();

  uint      sleptSoFar  = 0;
  DWORD     rc;
  bool      changed;

  if (GlobalData::stop)
    return -1;

  TT_VARS

  do
  {
    try
    {
      if (!stopMonitoringQuarantine &&
        WAIT_OBJECT_0 == (rc = monitorQuarantine.Wait(1000)))
      {
        TT_LOOP_TOP

        quarantineMonitorNotActive.Reset();

        changed = LookForChanges(quarantineServer);
      }
      quarantineMonitorNotActive.Set();

      TT_LOOP_BOTTOM
    }


    catch (...)
    {
      GlobalData::HandleException("QuarantineMonitor::ThreadHandlerProc()", Logger::LogIcePackQSMonitor);
    }

    GlobalData::SleepInSegments(GlobalData::QuarantineInterval, Logger::LogIcePackQSMonitor,
                  "Normal pause between check for changes in the quarantine server");

  } while (!GlobalData::stop);


  return 0;
}


//
//  QuarantineMonitor::LookForChanges(...)
//
//  Overview:
//    This method compares the samples stored in the master sample list with the
//    samples stored in the quarantine server and looks for differences between the
//    two.  (Actually some of the comparision work is done inside of the VQSEnum class)
//
//  Arguments:
//    quarantineServer  represents the DCOM rep of the quarantine server
//    listCopy      a copy of the master list
//    signalNewJobs   tells the caller to signal the JobPacker to deliver def files
//    submitNewSample   tells ScannerSubmittor to load new samples to the gateway
//
//  Algorithm:
//    (Simplified version, real code is slightly different)
//    For every sample not in a final state that's been changed
//        (VQSEnum looks at a sample's change number and compares
//        it to the change number of the same sample in the master list
//        if they are the same then VQSEnum ignores the sample)
//        (If the sample is in a final state in the quarantine server
//        and still exists in the master list then it will be returned
//        by VQSEnum)
//      If the sample is in the final state
//        remove it from the master sample list
//      else if the sample is a new sample
//        prep the sample (see PrepNewSample method)
//        if the sample is not in a final state after prepping
//          Add to master sample list
//          signal new sample to send to gateway if in release state
//          signal new definition to download if in available state
//          resort the master sample list
//      else
//        if the sample priority changed
//          if the sample priority changed from zero to non-zero
//            put the sample into the released state
//            signal scanner submittor to submit the new sample
//          else if the sample priority changed from non-zero to zero
//            put the sample into the held state
//          resort the master sample list
//        if the signature priority goes from zero to non-zero
//          signal JobPacker to deliver a definition set
//        Update the master sample list
//    Check the sample list and delete any sample that were marked for deletion by
//      VQSEnum.

bool QuarantineMonitor::LookForChanges(VQuarantineServer& quarantineServer)
{
  EntryExit     entryExit(Logger::LogIcePackQSMonitor, "LookForChanges");

  list<Sample>    listCopy;
  bool        signalNewJobs = false;
  bool        submitNewSample = false;

  bool        changed     = false;
  bool        resortMasterList= false;
  bool        newRecord;
  Sample        sample;
  list<Sample>::iterator  i;
  bool        blessed;
  uint        sigSeqNum = GlobalData::GetNewestSigAvailable(blessed);
  uint        neededSig = 0;
  CompSampleByKeyOnly comp(0L);
  VQSEnum       qsEnum;
  string        qsChangeCount, glChangeCount;

  signalNewJobs = false;

  try
  {
    char buffer[128];

    sprintf(buffer, "About to make copy of samplesList [%d samples in samplesList]", GlobalData::samplesList.size());
    Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, buffer);

    {
      CMclAutoLock  lock(GlobalData::critSamplesList);

      Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, "Got lock on samplesList");

      i = GlobalData::samplesList.begin();

      for (; i != GlobalData::samplesList.end(); i++)
      {
        listCopy.push_back(*i);
        listCopy.back().Mark(Sample::sDelete);
      }
    }

    sprintf(buffer, "Finished making copy of samplesList [%d samples in copy]", listCopy.size());  
    Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, buffer);

    /*
     * This is purely here for diagnostic purposes -- there's no way this
     * case should ever arise, but then this is programming.
     * inw/ejp 2000-05-12.
     */
    if (listCopy.size() != GlobalData::samplesList.size())
    {
      sprintf(buffer, "Oh dear -- listCopy.size() [%d] != samplesList.size() [%d]", listCopy.size(), GlobalData::samplesList.size());
      Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, buffer);
    }

    Logger::Log(Logger::LogDebug, Logger::LogIcePackQSMonitor, "About to LookForChanges");
    for (quarantineServer.GetVQSEnum(qsEnum);
      !stopMonitoringQuarantine && !GlobalData::stop &&
         qsEnum.Next(sample, listCopy, newRecord, i); )
    {
      //
      // I do not know why this block is here.  If this is a new
      // record then it would have this value would already have been
      // stored.  The only explaination I can come up with is that
      // this code pre-dates the constructor scanning the quarantine
      // server.
      // I would drop this code after the pass off to Symantec - atk.

      if (newRecord)
      {
        if (SampleStatus::needed == sample.Status())
        {
          if (sample.NeededSeqNum() > neededSig)
            neededSig = sample.NeededSeqNum();
        }
      }

      //
      // if the sample is in final status and is not a new record then
      // delete the sample from the master list
      if (sample.FinalStatus())
      {
        if (!newRecord)
        {
          changed = true;
          i->Mark(Sample::sDelete);
          if (i->IsNull() || SampleStatus::submitted == i->Status())
            submitNewSample = true;

          GlobalData::UpdateMasterSampleList(*i);

          Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor,
                "Removing finished sample from master list");
        }
      } // if we have a new record then prep it, add it to the master sample list
      else if (newRecord)
      {
        PrepNewSample(sample);

        if (!sample.FinalStatus())    // scan may have changed this to final status
        {
          if (SampleStatus::available == sample.Status() &&
              GlobalData::AutoDefinitionDelivery())
            signalNewJobs = true;

          // these next two lines are unnecessary, atk
//          listCopy.push_back(sample);
//          listCopy.back().Mark(Sample::sAdd);

          sample.Mark(Sample::sAdd);
          GlobalData::UpdateMasterSampleList(sample);
          if (SampleStatus::released == sample.Status())
            submitNewSample = true;
          resortMasterList  = true;
          changed = true;
        }
      }
      else
      {
        // check to see if the priority has changed, change state to released
        // and tell scanner submittor to upload a new sample if the priority
        // went from zero to non-zero, change to held if the priority changed
        // from non-zero to zero.
        // In all cases of changed priority resort the master list
        if (sample.Priority() != i->Priority())
        {
          Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor, "Sample priority changed");

          if (sample.Priority() > 0 && sample.Status() == SampleStatus::held)
          {
            sample.Status(SampleStatus::released);
            submitNewSample = true;
          }
          else if (sample.Priority() == 0 && sample.Status() == SampleStatus::released)
            sample.Status(SampleStatus::held);

          resortMasterList  = true;
        }

        //
        // if the signature priority goes from zero to non-zero,
        // signal the job packager to deliver the definition set
        if (0 == i->SigPriority() && 0 != sample.SigPriority() )
          signalNewJobs = true;

        changed = true;
        *i    = sample;
        i->Mark(Sample::sChanged);
        GlobalData::UpdateMasterSampleList(*i);
        submitNewSample = true;

        Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor, "Sample changed");

      }

      sample.DeActivate();
      if (!newRecord)
        i->DeActivate();
    }

    if (GlobalData::stop)
      return false;

    //
    //  Do we need to delete any items
    for (i = listCopy.begin(); i != listCopy.end(); i++)
    {
      if (Sample::sDelete == i->Mark())
      {
        changed     = true;
        submitNewSample = true;
        GlobalData::UpdateMasterSampleList(*i);
      }
    }

    if (resortMasterList && !GlobalData::stop)
    {
      CMclAutoLock  lock(GlobalData::critSamplesList);

      GlobalData::samplesList.sort();
    }

    listCopy.clear();

    if (signalNewJobs && GlobalData::AutoDefinitionDelivery())
    {
      Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor,
              "Signal signatures to download to clients");

      GlobalData::deliverSignature.Set(); // signal new jobs!
    }

    if (submitNewSample)
    {
      Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor,
              "Signal samples ready for submission");
      GlobalData::submitSample.Set();
    }
  }


  catch (...)
  {
    GlobalData::HandleException("QuarantineMonitor::LookForChanges()", Logger::LogIcePackQSMonitor);
  }

  try { listCopy.clear(); } catch (...) { };

  if (!GlobalData::stop)
  {
    if (changed)
      Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor,
        "At least one difference was found between the master list and the Quarantine server.");
    else
      Logger::Log(Logger::LogInfo, Logger::LogIcePackQSMonitor,
        "No differences were found between the master list and the Quarantine server.");


    if (neededSig > 0)
      GlobalData::SetSigToDownload(neededSig, false);
  }

  return changed;
}
