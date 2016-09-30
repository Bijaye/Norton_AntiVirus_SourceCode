// ScannerSubmittor.cpp: implementation of the ScannerSubmittor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <list>
#include <algorithm>

#include <AVIS.h>
#include <CMclAutoLock.h>
#include "NAVScan.h"
#include <AttributeKeys.h>
#include <XAnalysisState.h>
#include <XError.h>
#include <SystemException.h>
#include "EventLog.h"
#include "Logger.h"

#include "ScannerSubmittor.h"
#include "Sample.h"
#include "GlobalData.h"
#include "SampleStatus.h"
#include "AttributesParser.h"
#include "CommErrorHandler.h"
#include "BlessedMonitor.h"

#include <MyTraceClient.h>
#include "IcePackAgentComm.h"
#include "SampleStream.h"
#include "VDBUnpacker.h"

//#include "IcePackMonitor.h"
#include "Attention.h"

using namespace std;

uint  ScannerSubmittor::submittedSamples  = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScannerSubmittor::ScannerSubmittor(QuarantineMonitor& qMon)
          : CMclThreadHandler(), quarantineMonitor(qMon),
            traceSampleComm(false), traceSigComm(false)
{
}

ScannerSubmittor::~ScannerSubmittor()
{

}

//
//  ScannerSubmittor
//
//  Overview:
//    This thread performs two function; it submits samples to the gateway and it
//    downloads signature files.  The reason that both functions are performed by
//    the same thread is that they are mutually exclusive tasks.  We do not want
//    submit a new sample when downloading a signature file because the signature
//    file might repair the virus in the sample we want to submit.  Therefor we
//    want to hold off on sample submissions until after the signature has been
//    downloaded, unpacked, and all pending samples (samples in the "hold" or
//    "release" states) have been scanned with the new signatures.  Conversely we
//    do not want to be downloading a signature file while submitting a sample
//    because the result of the sample submission could be the need for an even
//    new signature file.
//
//    The submitter portion of this thread can change samples in the "released" state
//    into;
//      "submitted"   sample was submitted but the gateway did not return
//              a final state.
//      "needed"    we need a newer signature file than we have available
//              to repair this file.  A request to download this
//              signature has been made.
//      "available"   the signature file needed to repair this sample (or a
//              newer one) is already on this machine ready to be
//              installed onto the machine that sent the sample to
//              the quarantine server.  The JobPackager thread has
//              been signaled to transfer the signature files to the
//              infected machine
//      "attention"   An error requiring the attention of the system
//              administrator occurred while submitting the sample.
//              More information about the error can be found in the
//              X-Attention attribute.
//      "error"     Not sure how this is different from "attention".
//
//    The scanner portion of this thread downloads signature files from the
//    gateway and then scans all files in the "held" or "released" states
//    using the signature files just downloaded.  Samples that can be repaired
//    by the newly downloaded signatures are changed to the "available" state
//    and the JobPackager thread is notified that a signature needs to be
//    set to a client machine.
//
//  Algorithm:
//    (Simplified version, real code is slightly different)
//    While service is not being shutdown
//      wait for a signal to either submit a sample or download signature files
//      if signaled to submit a sample
//        make a list of samples in the "available" state from master sample
//          list in descending priority order counting the number of samples
//          in the "submitted" state
//        while number of samples in the "submitted" state is less than the
//            maximum number of samples that can be submitted
//          package the sample for submission (OLE strip, compress, and
//            scramble if requested to do so)
//          submit the sample to the gateway for analysis
//          Based on the attributes returned from the gateway set the
//            sample to "submitted", "needed", "available", "attention",
//            or "error".
//          if a new signature is needed signal that a new signature needs
//            to be downloaded.
//          if a signature needs to be installed on a client machine signal
//            the JobPackager.
//      else if signaled to download a new signature file
//        Get the signature to download from the Globals class
//        Try the following up to 200 times to get the signature file
//          download the signature file from the gateway
//        if unable to get the signature after 200 tries
//          stop IcePack
//        else
//          make a list of all samples in "needed", "held", and "released" states
//            descending priority order
//          for each sample
//            if needed
//              if signature is >= one waiting for
//                change state to "available"
//            else (held or released states)
//              Scan the sample with the sig just downloaded
//              change state to "available" if sample is repaired
//      update the master sample list of any changes made on this pass to any sample
//      if any sample is changed to "available" & configured for auto definition delivery
//        signal JobPackager to install new signatures on client machine
//
//
//  Globals static methods/members Used
//    stop          bool    if true exit this thread.  Set by SCM when service is
//                      told to stop or the system is shutting down.
//    critSamplesList     CMclCritSec used to serialize access to the samplesList
//    samplesList       std::list List of samples that IcePack might deal with (samples not
//                      already in a final state or submitted via Scan and Deliver)
//    submitSample      CMclEvent used to signal this thread to submit new samples to the
//                      gateway.
//    downloadSig       CMclEvent used to signal this thread to download a signature file
//    deliverSignature    CMclEvent used to signal the JobPackager to send a definition
//                      package to a client machine.
//    SecureSampleSubmission  bool    are samples submitted to a SSL port?
//    SecureSigDownload   bool    are signatures downloaded from a SSL port?
//    GatewayURL        std::string the URL of the gateway. This value comes from two
//                      registry values, webGatewayName and webGatewayPort
//    GatewaySSLURL     std::string the URL of the gateway (SSL port version).  This value
//                      comes from two registry values, webGatewayName and
//                      webGatewayPortSSL.
//    MaxPendingSamples   uint    the maximum number of samples that IcePack is configured
//                      to submit at the same time.  This value is set from the
//                      sampleMaximumPending registry value.
//    SigToDownload     uint    the sequence number of the signature to download.
//    NewestSigAvailable    uint    the sequence number of the newest signature available on
//                      this machine.
//    DefLibraryDirectory   std::string the root directory where signatures sub directories are
//                      created.  This value is set from the
//                      definitionLibraryDirectory registry value.
//    
//

#define LogBoth ((enum Logger::LogSource) (Logger::LogIcePackSubmission | Logger::LogIcePackSigDownload))
unsigned ScannerSubmittor::ThreadHandlerProc(void)
{
  EntryExit   entryExit(LogBoth, "ThreadHandlerProc");
//  EnterExitThread eetSubmittor(IcePackMonitor::sampleSubmittor);
//  EnterExitThread eetSigDwnldr(IcePackMonitor::signatureDownloader);

  SystemException::Init();

  QSThreadLife  qsThreadLife;   // initializes com for this thread
  DWORD     rc;
  list<Sample>  listCopy;
  list<Sample>::iterator  i,j;
  Sample      sample;
  bool      updateSamplesList;
  bool      signalNewJob;


//  IcePackMonitor::ThreadPauseing(IcePackMonitor::sampleSubmittor,
//                  "Waiting for a signal that more samples/slots are ready for submission");
//  IcePackMonitor::ThreadPauseing(IcePackMonitor::signatureDownloader,
//                  "Waiting for signal that a new definition set is needed");

  while (!GlobalData::stop &&
      ((rc = GlobalData::submitSample.WaitForTwo(GlobalData::downloadSig, FALSE, 5000)) ,
        !GlobalData::stop))
  {
    try
    {
      signalNewJob    = false;
      updateSamplesList = false;

      if (WAIT_OBJECT_0 == rc)      // submitSample
      {
//        EnterExitActive eea(IcePackMonitor::sampleSubmittor,
//                  "Waiting for a signal that more samples/slots are ready for submission");

        Logger::Log(Logger::LogInfo, Logger::LogIcePackSubmission, "Looking for samples to submit");

        submittedSamples = 0;
        listCopy.clear();
        {
          CMclAutoLock  lock(GlobalData::critSamplesList);
    
          for (i = GlobalData::samplesList.begin();
            i != GlobalData::samplesList.end();
            i++)
          {
            if (SampleStatus::released == i->Status())
            {
              listCopy.push_back(*i);
              listCopy.back().Mark(Sample::sUnchanged);
            }
            else if (SampleStatus::submitted == i->Status())
              ++submittedSamples;
          }
        }
        if (submittedSamples < GlobalData::MaxPendingSamples() && listCopy.size())
        {
          updateSamplesList = SubmitNewSample(listCopy, signalNewJob);
        }
      }
      else if (WAIT_OBJECT_0 + 1 == rc) // download signature file
      {
//        EnterExitActive eea(IcePackMonitor::signatureDownloader,
//                  "Waiting for signal that a new definition set is needed");

#ifndef TIMEING_TESTS
        if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackSigDownload))
          Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload, "Deciding whether or not to download a new signature sequence?");

        bool  blessed;
        uint  sequenceNumber(GlobalData::GetSigToDownload(blessed));

        if (sequenceNumber)
        {
          if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackSigDownload))
          {
            char msgStr[128];
            sprintf(msgStr, "Decided to download signature sequence %08d (%sblessed)", sequenceNumber, (blessed ? "" : "not "));
            Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload, msgStr);
          }

          bool dummy;

          if ((DownloadSig(sigComm, traceSigDwnldClient, traceSigComm, sequenceNumber, blessed)) &&
              (GlobalData::GetNewestSigAvailable(dummy) >= sequenceNumber) &&
              (! GlobalData::stop)) 
          {
            QMLock  lock(quarantineMonitor);

            listCopy.clear();
            {
              // make a copy of all samples in the "master list"
              // Since the copy should contain only samples that should 
              // rescanned, and the master happens to contain only
              // samples whose states qualifies them to be rescanned,
              // then we will just make a copy of the entire list.
              CMclAutoLock  lock(GlobalData::critSamplesList);
    
              for (i = GlobalData::samplesList.begin();
                i != GlobalData::samplesList.end();
                i++)
              {
                listCopy.push_back(*i);
                listCopy.back().Mark(Sample::sUnchanged);
              }
            }

            updateSamplesList = ScanPendingSamples(listCopy, signalNewJob);
          }
        }
        else /* (!sequenceNumber) */
        {
          if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackSigDownload))
            Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload, "Decided not to download a signature sequence");
        }
#endif
      }
      if (updateSamplesList && !GlobalData::stop)
      {
        {
          CompSampleByKeyOnly comp(0L);
          CMclAutoLock    lock(GlobalData::critSamplesList);

          for (i = listCopy.begin(); i != listCopy.end(); i++)
          {
            if (!i->IsNull())
            {
              if (Sample::sChanged == i->Mark())
              {
                comp.Key(i->SampleKey());
                j = find_if(GlobalData::samplesList.begin(),
                      GlobalData::samplesList.end(),
                      comp);
                if (j != GlobalData::samplesList.end())
                  *j = *i;
  
                if (SampleStatus::needed == i->Status())
                  GlobalData::SetSigToDownload(i->NeededSeqNum(), false);
              }
              else if (Sample::sDelete == i->Mark())
              {
                GlobalData::samplesList.remove(*i);
              }
            }
          }
        }
        if (signalNewJob && GlobalData::AutoDefinitionDelivery())
        {
          Logger::Log(Logger::LogInfo, LogBoth, "Signal new defs to be downloaded to client(s)");
          GlobalData::deliverSignature.Set(); // signal new job
        }
      }
    }
    catch (...)
    {
      GlobalData::HandleException("ScannerSubmittor::ThreadHandlerProc()",
                      LogBoth);
    }
  }

  return 0;
}


bool ScannerSubmittor::SubmitNewSample(std::list<Sample>& listCopy, bool& signalNewJob)
{
  EntryExit       entryExit(Logger::LogIcePackSubmission, "SubmitNewSample");

  bool          changed = false;
  list<Sample>::iterator  i;
  string          attrs;

  signalNewJob  = false;

  string          url;
  if (GlobalData::SecureSampleSubmission())
    url = GlobalData::GatewaySSLURL();
  else
    url = GlobalData::GatewayURL();

  //
  //  Go through the list and submit samples with a priority > 0
  //  until the maximum number of samples that can be submitted are
  //  submitted;
  for (i = listCopy.begin();
      i != listCopy.end() && !GlobalData::stop &&
        submittedSamples < GlobalData::MaxPendingSamples();
     i++)
  {
    try
    {
      if (!i->IsNull())
      {
        if (SampleStatus::released == i->Status())
        {
          changed = true;
          i->Mark(Sample::sChanged);

          if (UploadSample(*i, attrs, url))
          {
            /*
             * This particular section of ugly code was added in order
             * to support the submission counter.  This, in turn, is
             * helpful when it becomes possible to resubmit samples.
             *
             * Unfortunately, because of the way the code 'works' around
             * these parts, this is much harder than one might have hoped
             * (I'm sure I've used that phrase before, elsewhere in the code...).
             */
            {
              std::string   attrValString;
              unsigned long attrVal           = 0;
              char          attrValArray[10];
              bool          reWriteAttr       = true;

              /*
               * Attempt to get the attribute out.  If it's not there, or it's
               * zero length (shouldn't be possible), then skip over the next bit.
               */
              if ((i->GetAttribute(AttributeKeys::SubmissionCount(), attrValString)) && (attrValString.size() > 0))
              {
                /*
                 * Convert the string to a number.  If the number has reached
                 * some unimaginably gigantically hugely collosal size, then
                 * give up this game now.
                 */
                attrVal = atol(attrValString.c_str());
                if (attrVal >= 99999999)
                  reWriteAttr = false;
              }

              /*
               * However we got here (whether or not we read an attribute),
               * unless there was an out-of-range error, add one, and attempt
               * to stamp the attribute back.
               */
              if (reWriteAttr)
              {
                attrVal++;
                sprintf(attrValArray, "%ld", attrVal);
                if (i->SetAttribute(AttributeKeys::SubmissionCount(), attrValArray))
                {
                  Logger::Log(Logger::LogInfo,  Logger::LogIcePackSubmission, "Succesfully updated submission count in attribute");
                  i->Commit();
                }
                else
                  Logger::Log(Logger::LogError, Logger::LogIcePackSubmission, "Failed to update submission count in attribute");
              }
            }


            Logger::Log(Logger::LogInfo, Logger::LogIcePackSubmission,
                    "Sample successfully uploaded to gateway");

            i->Update(attrs);

            if (SampleStatus::available == i->Status())
              signalNewJob  = true;
            else if (SampleStatus::released == i->Status())
            {
              i->Status(SampleStatus::submitted);
              ++submittedSamples;
            }
            else if (SampleStatus::needed == i->Status())
            {
              GlobalData::SetSigToDownload(i->NeededSeqNum(), false);
            }
            else if (SampleStatus::available == i->Status() &&
              GlobalData::AutoDefinitionDelivery())
            {
              GlobalData::deliverSignature.Set();
            }
          }
        }
      }
    }

    catch (...)
    {
      GlobalData::HandleException("ScannerSubmittor::SubmitNewSample()",
                      Logger::LogIcePackSubmission);
    }
  }

  return changed;
}



bool ScannerSubmittor::UploadSample(Sample& sample, string& attrs, string& url)
{
  EntryExit entryExit(Logger::LogIcePackSubmission, "UploadSample");
  bool    rc  = false;

  try
  {

    // update quarantine service (add required attributes)
    CreateAttributes(sample);

    // submit sample to gateway
    sample.GetAllAttributes(attrs);
    SampleStream  stream(sample);
    sample.GetAllAttributes(attrs);     // info got added by stream class

    //
    //  The sample may have been stripped of OLE non-macro content so the
    //  size of the sample that is being sent to the gateway may be different
    //  from the size of the sample stored in the quarantine server.  If this
    //  is the case we have to modify the attributes string to hold the correct
    //  sample size or the filter will reject the sample.

    if (stream.Stripped())
    {
      int   cPos = attrs.find(AttributeKeys::SampleFileSize(), 0);
          cPos = attrs.find(":", cPos);
      int   nPos = attrs.find("\n", cPos);
      if (string::npos != cPos && string::npos != nPos)
      {
        char  newSizeAsChar[20];
        sprintf(newSizeAsChar, "%d", stream.SampleSize());
        string  newSize(newSizeAsChar);

        cPos  += 2;
        attrs.erase(cPos, nPos - cPos);
        attrs.insert(cPos, newSize);
      }

      cPos = attrs.find(AttributeKeys::SampleCheckSum(), 0);
      cPos = attrs.find(":", cPos);
      nPos = attrs.find("\n", cPos);
      if (string::npos != cPos && string::npos != nPos)
      {
        cPos  += 2;
        attrs.erase(cPos, nPos - cPos);
        attrs.insert(cPos, stream.SampleMD5());
      }
    }

    if (GlobalData::StripUserData())
    {
      int cPos  = attrs.find(AttributeKeys::SampleFile(), 0);
      int nPos  = attrs.find("\n", cPos);
      if (string::npos != cPos && string::npos != nPos)
        attrs.erase(cPos, 1+nPos - cPos);
    }


  /*
  HANDLE  fileHandle;
  char  buff[4096];
  DWORD bytesWritten;
  DWORD inSize;
  ulong errorCode;
  char  sampleName[80];
  static  uint  fileCount = 0;
  ++fileCount;
  sprintf(sampleName, "c:\\temp\\sample%02d.sample", fileCount);

  fileHandle = CreateFile(sampleName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
  WriteFile(fileHandle, attrs.c_str(), attrs.length(), &bytesWritten, NULL);
  buff[0] = '\n';
  WriteFile(fileHandle, buff, 1, &bytesWritten, NULL);

  while (stream.read((uchar *) buff, inSize = 4096, errorCode))
  WriteFile(fileHandle, buff, inSize, &bytesWritten, NULL);
  CloseHandle(fileHandle);
  stream.init(errorCode);
  */

    string      readyForSubmissionAttributes = attrs;
    bool      retry;
    ErrorInfo   errorInfo;
    int       i = 1;

    if (GlobalData::stop)
      return false;

    //
    //  try to submit the sample
    do
    {
      retry = false;
      char  buffer[64];
      sprintf(buffer, "About to try %d upload attempt for sample %lx",
                i, sample.SampleKey());
      Logger::Log(Logger::LogInfo, Logger::LogIcePackSubmission, buffer);


      if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSubmission) &&
        Logger::WillLog(Logger::LogDebug, Logger::LogComm))
      {
        if (!traceSampleComm)
        {
          string  traceFileName(GlobalData::TraceFileName());

          traceSampleClient.EnableTracing();
          sampleComm.SetTraceClient(traceSampleClient, traceFileName.c_str(),
                        traceFileName.c_str(), "IcePack");
          traceSampleComm = true;
        }
      }
      else if (traceSampleComm)
      {
        traceSampleClient.EnableTracing(false);
        traceSampleComm = false;
      }

      if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSubmission))
      {
        string  msg("About to call sample Upload(");
        msg += url;
        msg += " , attrs[out variable], stream object)";

        Logger::Log(Logger::LogDebug, Logger::LogIcePackSubmission, msg.c_str());
      }

      attrs = readyForSubmissionAttributes;
      if (sampleComm.Upload(url, attrs, stream))
      {
        Logger::Log(Logger::LogInfo, Logger::LogIcePackSubmission,
              "Sample successfully uploaded");
        rc  = true;
        // clear the "submit" bit in the attention registry value
        Attention::UnSet(Attention::Submit);
      }
      else
      {
        retry = HandleSubmissionError(sampleComm, attrs, sample, errorInfo);
        i++;
      }
    } while (!GlobalData::stop && retry);
  }

  catch (IcePackException& e)
  {
    string  errMsg("Exception of type IcePackException caught in UploadSample [");
    errMsg  += e.FullString();
    errMsg  += "]";

    Logger::Log(Logger::LogError, Logger::LogIcePackSubmission, errMsg.c_str());

    sample.SetAttribute(AttributeKeys::Attention(), errMsg);
    sample.Status(SampleStatus::attention);
    sample.Commit();

    rc  = false;
  }
  catch (...)
  {
    string  typeInfo(""), detailedInfo(""), errMsg("Exception of type ");
    if (GlobalData::ExceptionTyper(typeInfo, detailedInfo))
    {
      errMsg    += typeInfo;
      errMsg    += " caught in ScannerSubmittor::UploadSample [";
      errMsg    += detailedInfo;
      errMsg    += "]";

      Logger::Log(Logger::LogCriticalError, Logger::LogIcePackSubmission,
            errMsg.c_str());
      EventLog::ExceptionStop("ScannerSubmittor::UploadSample()", errMsg.c_str());

      sample.SetAttribute(AttributeKeys::Attention(), typeInfo.c_str());
    }
    else
    {
      Logger::Log(Logger::LogCriticalError, Logger::LogIcePackSubmission,
            "Exception of type unknown caught in UploadSample");
      EventLog::UnknownExceptionStop("ScannerSubmittor::UploadSample()");

      sample.SetAttribute(AttributeKeys::Attention(), "Exception of unknown type caught in UploadSample");
    }

    sample.Status(SampleStatus::attention);
    sample.Commit();

    rc  = false;
    GlobalData::Stop();
  }

  return rc;
}


bool ScannerSubmittor::ScanPendingSamples(std::list<Sample>& listCopy, bool& signalNewJob)
{
  EntryExit       entryExit(Logger::LogIcePackSigDownload, "ScanPendingSamples");

  bool          changed = false;
  list<Sample>::iterator  i;
  bool          blessed;
  uint          newestSig = GlobalData::GetNewestSigAvailable(blessed);

  signalNewJob  = false;

  for (i = listCopy.begin(); i != listCopy.end() && !GlobalData::stop; i++)
  {
    try
    {
      if (!i->IsNull())
      {
        // rescan all samples
        i->Mark(Sample::sChanged);
        NAVScan::ScanRC scanRC = NAVScan::UnknownError;
          
        try
        {
          /*
           * In this call to Scan(), we can know for sure that
           * it's not the initial scan.
           */
          scanRC = i->Scan(newestSig, false);
        }
        catch (IcePackException& ice)
        {
          if (!(SampleStatus::attention == i->Status() ||
              SampleStatus::error     == i->Status() ))
          {
            string  msg("Scanning of sample failed [ ");
            msg += ice.FullString();
            msg += " ]";
            i->SetAttribute(AttributeKeys::Attention(), msg.c_str());
            i->Status(SampleStatus::attention);
          }
        }
        catch (...)
        {
          GlobalData::HandleException("ScannerSubmittor::ScanPendingSamples, scan sample",
                        Logger::LogIcePackSigDownload);


          if (!(SampleStatus::attention == i->Status() ||
              SampleStatus::error     == i->Status() ))
          {
            string  msg("Scanning of sample failed  [ exception thrown by scan interface ]");
            i->SetAttribute(AttributeKeys::Attention(), msg.c_str());
            i->Status(SampleStatus::attention);
          }
        }

        //
        //  If the sample is in the needed state (Immune System told it
        //  to get a particular definition set) and the current definition
        //  set is the equal to or greater than the required definition set
        //  and the scan returned that no repair instructions exist, then
        //  change to available.  In this case no repair is possible.

        if (SampleStatus::needed == i->Status())
        {
          if (newestSig >= i->NeededSeqNum())
          {
            if (NAVScan::NotInfected      == scanRC ||
              NAVScan::NotInfectable      == scanRC ||
              NAVScan::UnSubmittable      == scanRC )
              i->Status(SampleStatus::available);
          }
          else if (NAVScan::BadScan       == scanRC ||
               NAVScan::NavCrashed      == scanRC ||
               NAVScan::NavVcdInitError   == scanRC ||
               NAVScan::NavDefinitionFileError== scanRC ||
               NAVScan::NavLoadDefFileError == scanRC ||
               NAVScan::ScannerServiceError == scanRC ||
               NAVScan::WaitForScanTimedOut == scanRC ||
               NAVScan::CommWithScannerOffline== scanRC ||
               NAVScan::BadResponceFromScanner== scanRC)
          {
            string  msg("Scanning of sample failed [ ");
            msg += NAVScan::RCtoChar(scanRC);
            msg += " ]";
            i->SetAttribute(AttributeKeys::Attention(), msg.c_str());
            i->Status(SampleStatus::attention);
          }
        }

        changed = true;
        if (SampleStatus::available == i->Status())
          signalNewJob  = true;
        else if (i->FinalStatus())
          i->Mark(Sample::sDelete);
        i->Commit();
        GlobalData::UpdateMasterSampleList(*i);
      }
    }

    catch (IcePackException& e)
    {
      string  errMsg("Exception of type IcePackException caught in ScanPendingSamples [");
      errMsg  += e.FullString();
      errMsg  += "]";

      Logger::Log(Logger::LogError, Logger::LogIcePackSigDownload, errMsg.c_str());

      i->SetAttribute(AttributeKeys::Attention(), errMsg);
      i->Status(SampleStatus::attention);
      i->Commit();
    }
    catch (...)
    {
      string  typeInfo(""), detailedInfo(""), errMsg("Exception of type ");
      if (GlobalData::ExceptionTyper(typeInfo, detailedInfo))
      {
        errMsg    += typeInfo;
        errMsg    += " caught in ScannerSubmittor::ScanPendingSamples [";
        errMsg    += detailedInfo;
        errMsg    += "]";
        Logger::Log(Logger::LogCriticalError, Logger::LogIcePackSigDownload,
              errMsg.c_str());
        EventLog::ExceptionStop("ScannerSubmittor::ScanPendingSamples()", errMsg.c_str());

        i->SetAttribute(AttributeKeys::Attention(), errMsg.c_str());
      }
      else
      {
        Logger::Log(Logger::LogCriticalError, Logger::LogIcePackSigDownload,
              "Exception of type unknown caught in ScanPendingSamples");
        EventLog::UnknownExceptionStop("ScannerSubmittor::ScanPendingSamples()");

        i->SetAttribute(AttributeKeys::Attention(),
                  "Exception of unknown type caught in ScanPendingSamples");
      }

      i->Status(SampleStatus::attention);
      i->Commit();

      GlobalData::Stop();
    }
  }

  return changed;
}

void ScannerSubmittor::CreateAttributes(Sample& sample)
{
  EntryExit entryExit(Logger::LogIcePackSubmission, "CreateAttributes");

  std::string tmp;
  DateTime  now;
  now.AsHttpString(tmp);
  sample.SetAttribute(AttributeKeys::DateSubmitted(), tmp);
  sample.SetAttribute(AttributeKeys::AnalysisState(), XAnalysisState::Submitting());
  sample.SetAttribute(AttributeKeys::SampleSubmissionRoute(), "1");

  sample.Commit();
}

uint ScannerSubmittor::UpgradeSequenceNumber(IcePackAgentComm::SignatureComm& sigComm, uint minSequenceNumber,
                       bool& blessed)
{
using namespace IcePackAgentComm;
using namespace std;

  EntryExit   entryExit(Logger::LogIcePackSigDownload, "UpgradeSequenceNumber");
  string      asStr("");
  string      url;
  uint      newSig  = 0;
  bool      done = false;

  if (GlobalData::SecureSigDownload())
    url = GlobalData::GatewaySSLURL();
  else
    url = GlobalData::GatewayURL();

  //
  //  Check if the latest blessed definition set will do
  if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSigDownload))
  {
    string  msg("About to look for blessed sequence number, GetLatestBlessed(");
    msg += url;
    msg += " , asStr[out variable])";
    Logger::Log(Logger::LogDebug, Logger::LogIcePackSigDownload, msg.c_str());
  }

  do
  {
    done = BlessedMonitor::GetLatestBlessedSeqNum(sigComm, newSig,
                            Logger::LogIcePackSigDownload);
  } while (!done &&
        GlobalData::SleepInSegments(GlobalData::RetryInterval,
                      Logger::LogIcePackSigDownload,
                      "Pausing between attempts to get the sequence number of the latest blessed definition set"));

  if ((newSig < minSequenceNumber || !GlobalData::IsOkaySeqNum(newSig)) && !GlobalData::stop)
  {
    Logger::Log(Logger::LogDebug, Logger::LogIcePackSigDownload,
          "Latest blessed is not new enough, check for latest available");

    do
    {
      done = BlessedMonitor::GetLatestSeqNum(sigComm, newSig,
                          Logger::LogIcePackSigDownload);
    } while ((!done || newSig < minSequenceNumber || !GlobalData::IsOkaySeqNum(newSig)) &&
          GlobalData::SleepInSegments(GlobalData::RetryInterval,
                        Logger::LogIcePackSigDownload,
                        "Pausing between attempts to get the sequence number of the latest definition set"));
    blessed = false;
  }
  else
  {
    blessed = true;
    Logger::Log(Logger::LogDebug, Logger::LogIcePackSigDownload,
          "Latest blessed sequence number >= minRequired sequence number");
  }

  if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSigDownload))
  {
    char  buffer[128];
    sprintf(buffer, "UpgradeSequenceNumber choose sequence number = %d", newSig);
    Logger::Log(Logger::LogDebug, Logger::LogIcePackSigDownload, buffer);
  }

  return newSig;
}



bool ScannerSubmittor::DownloadSig(IcePackAgentComm::SignatureComm& sigComm,
              MyTraceClient& traceClient, bool& traceSigComm,
              uint sequenceNumber, bool blessed)
{
using namespace IcePackAgentComm;
using namespace std;

  EntryExit   entryExit(Logger::LogIcePackSigDownload, "DownloadSig");
  char        buffer[32];
  string      attrs;
  string      seqNum;
  bool        downloaded  = false;
  ErrorInfo   errorInfo;
  string      headers   = "";
  bool        dummy;

  if ((blessed) && (sequenceNumber <= GlobalData::GetNewestBlessedSigAvailable()))
    return false;
  if ((! blessed) && (sequenceNumber <= GlobalData::GetNewestSigAvailable(dummy)))
    return false;

  if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSigDownload) &&
    Logger::WillLog(Logger::LogDebug, Logger::LogComm))
  {
    if (!traceSigComm)
    {
      string  traceFileName(GlobalData::TraceFileName());

      traceClient.EnableTracing();
      sigComm.SetTraceClient(traceClient, traceFileName.c_str(),
                        traceFileName.c_str(), "IcePack");
      traceSigComm = true;
    }
  }
  else if (traceSigComm)
  {
    traceClient.EnableTracing(false);
    traceSigComm  = false;
  }


  string  url;
  if (GlobalData::SecureSigDownload())
  {
    /*
     * In the event that we're going to be talking to the gateway
     * in a secure fashion, stick the contact headers in.  If we're
     * going to the server unprotected, then don't.
     */
    headers = GlobalData::GetContactHeaders();
    url     = GlobalData::GatewaySSLURL();
  }
  else
    url = GlobalData::GatewayURL();

  
  bool  startedAsBlessedRequest = blessed;

  while (!downloaded && !GlobalData::stop)
  {
    try
    {
      if (!startedAsBlessedRequest)
      {
        sequenceNumber = UpgradeSequenceNumber(sigComm, sequenceNumber, blessed);
        if (GlobalData::stop)
          return false;
      }
      else if (!GlobalData::IsOkaySeqNum(sequenceNumber))
      {
        char  buffer[16];
        sprintf(buffer, "%08d", sequenceNumber);
        string  msg("Downloading of blessed definition set ");
        msg += buffer;
        msg += " aborted because that definition set has been marked invalid by IcePack";
        Logger::Log(Logger::LogError, Logger::LogIcePackSigDownload, msg.c_str());

        return false;
      }

      sprintf(buffer, "%u", sequenceNumber);
      seqNum  = buffer;

      if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSigDownload))
      {
        string  msg("About to call sig Download(");
        msg += url;
        msg += " , ";
        msg += seqNum;
        msg += " , ";
        msg += GlobalData::DefLibraryDirectory();
        msg += " , attrs[out variable], ";
        if (blessed)
          msg += "LatestBlessed, true)";
        else
          msg += "Regular, true)";
        Logger::Log(Logger::LogDebug, Logger::LogIcePackSigDownload, msg.c_str());
      }

      downloaded = sigComm.Download(url, headers, seqNum, GlobalData::DefLibraryDirectory(),
                      attrs, (blessed ? LatestBlessed : Regular), true);

      if (GlobalData::stop)
        return false;

      if (downloaded)
      {
        Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload,
                  "Successfully downloaded definition files");

        // Clear any of the "disk" & "Download" & "unavailable" flags
        // that may currently be set in the "attention" registry
        // variable. (EJP 5/14/00)

        Attention::UnSet(Attention::Disk);
        Attention::UnSet(Attention::Download);
        Attention::UnSet(Attention::Unavailable);

        string  dateBlessed("");
        if (AttributesParser::ParseOneAttribute(attrs, AttributeKeys::DateBlessed(), dateBlessed))
          blessed = true;
        else
          blessed = false;

        if (AttributesParser::ParseOneAttribute(attrs, AttributeKeys::SignatureSequence(), seqNum))
        {
          sequenceNumber  = atoi(seqNum.c_str());
          GlobalData::SetNewestSigAvailable(sequenceNumber, blessed);

          string  fullPathAndName(GlobalData::DefLibraryDirectory());

          // prune old definitions if the definitions we just downloaded are
          // blessed definitions, and the "prune" option is set in the
          // registry
          if ((GlobalData::DefPrune()) && blessed)
          {
            Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload,
                      "Pruning of old definitions is enabled, and just downloaded a blessed definition.");
            Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload,
                      "Looking for old definitions to prune...");
            // iterate through all packages in the definition library directory
            WIN32_FIND_DATA fd;

            // construct a wildcard string that will allow FindFirstFile()
            // to search for all files in the definition directory
            string strPackageWildcard = fullPathAndName + "\\*.*";
            
            HANDLE hFind = ::FindFirstFile(strPackageWildcard.c_str(), &fd);

            // only iterate if a file was found
            if (hFind != INVALID_HANDLE_VALUE)
              do
              {
                // the base file name of each package contains the sequence number for the definition

                // only care about package files, not directories
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                  char szBase[_MAX_FNAME];

                  // get the base filename, which is a 0-padded sequence number
                  ::_splitpath(fd.cFileName, NULL, NULL, szBase, NULL);

                  int iSeqNum = ::atoi(szBase);

                  char szMsg[1024 * 10];
                  sprintf(szMsg, "Found a package file with sequence number %d", iSeqNum);
                  Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload, szMsg);

                  // if the sequence number of the package on disk is older than the current (blessed) definition
                  // then prune it
                  if ((iSeqNum > 0) && (iSeqNum < sequenceNumber))
                  {
                    sprintf(szMsg, "This package file is older than the definitions just downloaded (seq # = %d), so it will be pruned.", sequenceNumber);
                    Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload, szMsg);

                    VDBPackage package;

                    // Ignore the return code for Prune();
                    // Don't care if there was an error during pruning;
                    // Can't do anything about it...
                    string strPathToPackage = fullPathAndName + "\\" + fd.cFileName;
                    string strPathToExtractedDirectory = fullPathAndName + "\\" + szBase;

                    sprintf(szMsg, "Attempting to prune package \"%s\" and its associated directory \"%s\"...",
                        strPathToPackage.c_str(), strPathToExtractedDirectory.c_str());
                    Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload, szMsg);
                    package.Prune(strPathToPackage, strPathToExtractedDirectory, NULL, static_cast<void*>(&traceClient));

                    string result;

                    switch (package.Error())
                    {
                      case VDBPackage::NoError:       result = "NoError";         break;  
                      case VDBPackage::PackageNotFoundError:  result = "PackageNotFoundError";  break;  
                      case VDBPackage::TargetDirError:    result = "TargetDirError";      break;  
                      case VDBPackage::NoFilesFoundError:   result = "NoFilesFoundError";     break;  
                      case VDBPackage::DiskFullError:     result = "DiskFullError";     break;  
                      case VDBPackage::VersionError:      result = "VersionError";      break;  
                      case VDBPackage::SystemException:     result = "SystemException";     break;  
                      case VDBPackage::ZipArchiveError:     result = "ZipArchiveError";     break;  
                      case VDBPackage::MemoryError:       result = "MemoryError";       break;  
                      case VDBPackage::ParameterError:    result = "ParameterError";      break;  
                      case VDBPackage::ZipNoFilesFoundError:  result = "ZipNoFilesFoundError";  break;  
                      case VDBPackage::UnexpectedEOFError:  result = "UnexpectedEOFError";    break;  
                      case VDBPackage::ZipUnknownError:   result = "ZipUnknownError";     break;  
                      case VDBPackage::FileRemovingError:   result = "FileRemovingError";   break;  
                      case VDBPackage::DirPruningError:   result = "DirPruningError";     break;  
                      default:                result = "UnknownError";      break;  
                    }

                    sprintf(szMsg, "Result of Pruning -- %s", result.c_str());
                    Logger::Log(Logger::LogInfo, Logger::LogIcePackSigDownload, szMsg);
                  }
                }

              // iterate to the next file in the directory
              } while (::FindNextFile(hFind, &fd));

            ::FindClose(hFind);
          }

          if (GlobalData::DefBlessedBroadcast() && blessed)
          {
            GlobalData::newBlessed.Set();

            Logger::Log(Logger::LogError, Logger::LogIcePackSigDownload,
                  "Signaling that a new blessed definition set is here");
          }
        }
        else
        {
          GlobalData::Stop();
          Logger::Log(Logger::LogCriticalError, Logger::LogIcePackSigDownload,
                "Successful signature download did not return an X-Signature-Sequence attribute");
          EventLog::Error(FACILITY_SIGDOWNLOAD, IPREG_SIG_DOWNLOAD_NO_XSIGSEQ);
        }
      }
      else
      {
        Logger::Log(Logger::LogError, Logger::LogIcePackSigDownload,
              "Downloading of signatures failed");
        HandleDownloadError(sigComm, attrs, sequenceNumber, blessed, errorInfo);

        //
        //  Reset just in case we switched from a blessed download to
        //  a more recent non-blessed download
        sequenceNumber = GlobalData::GetSigToDownload(blessed);
      }
    }

    catch (IcePackException& e)
    {
      string  errMsg("Exception of type IcePack caught in DownloadSig [");
      errMsg  += e.FullString();
      errMsg  += "]";

      Logger::Log(Logger::LogError, Logger::LogIcePackSigDownload, errMsg.c_str());
      EventLog::ExceptionStop("ScannerSubmittor::DownloadSig()", e.FullString().c_str());

      GlobalData::SleepInSegments(GlobalData::NeededInterval,
                    Logger::LogIcePackSigDownload,
                    "Pausing because the last attempt to download and install a new definition set caused an exception");
    }

    catch (...)
    {
      GlobalData::HandleException("ScannerSubmittor::DownloadSig()",
                    Logger::LogIcePackSigDownload);
    }
  }

  return downloaded;
}

/* ----- */

/*
 * The history of this method, as with others, is opaque.  The alert reader
 * will note that very similar functionality is to be found in three methods
 * both in this module and elsewhere:
 *   ScannerSubmittor::HandleDownloadError()
 *   ScannerSubmittor::HandleSubmissionError()
 *   StatusMonitor::HandleGatewayError()
 * Exactly why this is, I have no idea.  However, in the future it will be
 * nice to merge the logic into one central place.
 */
bool ScannerSubmittor::HandleDownloadError(IcePackAgentComm::SignatureComm &sig,
                                           std::string                     &attrs,
                                           uint                             seqNum,
                                           bool                            &blessed,
                                           ErrorInfo                       &errorInfo)
{
  EntryExit                   entryExit(Logger::LogIcePackSigDownload, "HandleDownloadError");

  bool                        retryWithDelay    = false;
  bool                        retryWithoutDelay = false;
  bool                        commError         = false;
  IcePackAgentComm::ErrorCode errorCode         = sig.Error();

  /*
   * In the case of a disk error, set 'Attention' with the
   * appropriate flag.
   */
  switch (errorCode)
  {
    case IcePackAgentComm::TempDirError :
    case IcePackAgentComm::TargetDirError :
    case IcePackAgentComm::TargetDOSDirError :
    case IcePackAgentComm::FileCopyError :
    case IcePackAgentComm::DiskFullError :
    case IcePackAgentComm::FileWriteError :
    case IcePackAgentComm::FileReadError :
    case IcePackAgentComm::FileOpenError :
      Logger::Log(Logger::LogWarning, Logger::LogIcePackSigDownload, "Disk I/O error occurred while downloading/unpacking a definition.");
      Attention::Set(Attention::Disk);
      break;
    default :
      break;
  }

  if (IcePackAgentComm::TerminatedByCaller == errorCode)
    return false;
  else if (IcePackAgentComm::GatewayError == errorCode)
  {
    string  xError("");
    string  why;
    string  msg;

    /*
     * Is there an X-Error: header?
     */
    if ((AttributesParser::ParseMultipuleAttributes(attrs, AttributeKeys::Error(), xError)) &&
        (xError != ""))
    {
      if (XError::Abandoned() == xError)
      {
        GlobalData::AddToBadSeqNums(seqNum);
        why = "GatewayError, sig abandoned";
      }
      else if (XError::Internal() == xError)
      {
        why = "GatewayError, gateway internal error";
      }
      else if (XError::Superceded() == xError)
      {
        blessed = true; /* ??? */
        why = "GatewayError, sig superceded";
      }
      else if (XError::Unavailable() == xError)
      {
        why = "GatewayError, sig unavailable";
        Attention::Set(Attention::Unavailable); // set "unavailable" registry variable (EJP 5/14/00)
      }
      else if ((XError::Malformed() == xError) ||
               (XError::Missing()   == xError))
      {
        why = "GatewayError, malformed message or missing critical attribute";
      }
      else
      {
        /*
         * WTF error.
         */
        why = "Unknown or unexpected GatewayError [" + xError + "]";
      }
    }
    else
    {
      /*
       * There is no X-Error: header.
       */
      why = "GatewayError w/o an X-Error attribute!";
    }

    msg = "Error downloading definitions file, error = " + why;
    Logger::Log(Logger::LogWarning, Logger::LogIcePackSigDownload, msg.c_str());

    retryWithDelay = true;
  }
  else
  {
    /*
     * Not a gateway error.
     */

    if ((IcePackAgentComm::NetworkError         == errorCode) ||
        (IcePackAgentComm::NetworkCriticalError == errorCode))
      commError = true;

    /*
     * Note carefully that retryWithDelay and retryWithoutDelay
     * may be set by HandleCommError().  In fact, Stop() could
     * be triggered too...
     */
    CommErrorHandler::HandleCommError(errorCode,
                                      sig.SystemExceptionCode(),
                                      errorInfo,
                                      retryWithDelay,
                                      retryWithoutDelay,
                                      Logger::LogIcePackSigDownload,
                                      "error downloading definitions file, error = ");

    if ((!GlobalData::stop) && (!retryWithDelay) && (!retryWithoutDelay))
    {
      switch (errorCode)
      {
        case IcePackAgentComm::PackageNotFoundError :
        case IcePackAgentComm::NoFilesFoundError :
        case IcePackAgentComm::VersionError :
        case IcePackAgentComm::ProcessTimedOutError :
        case IcePackAgentComm::FatAlbertSystemException :
        case IcePackAgentComm::SystemException :
        case IcePackAgentComm::VDBSystemException :
        case IcePackAgentComm::VDBZipArchiveError :
        case IcePackAgentComm::VDBMemoryError :
        case IcePackAgentComm::VDBParameterError :
        case IcePackAgentComm::VDBUnexpectedEOFError :
        case IcePackAgentComm::VDBUnknownError :
          GlobalData::AddToBadSeqNums(seqNum);
          break;
        default :
          break;
      }
    }
  }

  if (commError)
    Attention::Set(Attention::Download);
  else
    Attention::UnSet(Attention::Download);

  if ((!GlobalData::stop) && retryWithDelay)
    GlobalData::SleepInSegments(GlobalData::RetryInterval,
                                Logger::LogIcePackSigDownload,
                                "Pausing before retrying a definition set download");

  return ((!GlobalData::stop) && (retryWithDelay || retryWithoutDelay));
}

/* ----- */

/*
 * The history of this method, as with others, is opaque.  The alert reader
 * will note that very similar functionality is to be found in three methods
 * both in this module and elsewhere:
 *   ScannerSubmittor::HandleDownloadError()
 *   ScannerSubmittor::HandleSubmissionError()
 *   StatusMonitor::HandleGatewayError()
 * Exactly why this is, I have no idea.  However, in the future it will be
 * nice to merge the logic into one central place.
 */
bool ScannerSubmittor::HandleSubmissionError(IcePackAgentComm::SampleComm &sampComm,
                                             string                       &attrs,
                                             Sample                       &sample,
                                             ErrorInfo                    &errorInfo)
{
  EntryExit                   entryExit(Logger::LogIcePackSubmission, "HandleSubmissionError");

  bool                        retryWithDelay    = false;
  bool                        retryWithoutDelay = false;
  bool                        commError         = false;
  IcePackAgentComm::ErrorCode errorCode         = sampComm.Error();

  if (IcePackAgentComm::TerminatedByCaller == errorCode)
    return false;
  else if (IcePackAgentComm::GatewayError  == errorCode)
  {
    string  xError("");
    string  why;
    string  msg;

    /*
     * Is there an X-Error: header?
     */
    if ((AttributesParser::ParseMultipuleAttributes(attrs, AttributeKeys::Error(), xError)) &&
        (xError != ""))
    {
      if (XError::Content() == xError)
      {
        why = "checksum of the content did not match X-Content-Checksum";
        sample.Status(SampleStatus::error);
      }
      else if (XError::Declined() == xError)
      {
        why = "sample declined";
        sample.Status(SampleStatus::error);
      }
      else if (XError::Internal() == xError)
      {
        why = "internal gateway error";
        sample.Status(SampleStatus::error);
      }
      else if (XError::Lost() == xError)
      {
        why = "sample lost by the gateway";
        sample.Status(SampleStatus::error);
      }
      else if (XError::Sample() == xError)
      {
        why = "checksum of the sample did not match X-Sample-Checksum";
        sample.Status(SampleStatus::error);
      }
      else if (XError::Type() == xError)
      {
        why = "sample type not accepted by the Immune System";
        sample.Status(SampleStatus::error);
      }
      else if (XError::Overrun() == xError) /* This error can never occur.  inw 2000-03-06 */
      {
        why = "length of the content is larger than specified in Content-Length attribute";
        sample.Status(SampleStatus::error);
      }
      else if (XError::Underrun() == xError) /* This error can never occur.  inw 2000-03-06 */
      {
        why = "length of the content is smaller than specified in Content-Length attribute";
        sample.Status(SampleStatus::error);
      }
      else if ((XError::Malformed() == xError) ||
               (XError::Missing()   == xError))
      {
        why = "GatewayError, malformed message or missing critical attribute";
        sample.Status(SampleStatus::error);
      }
      else
      {
        /*
         * WTF error.
         */
        why = "Unknown or unexpected GatewayError [" + xError + "]";
        sample.Status(SampleStatus::error);
     }

     /*
      * Place the appropriate error into the sample.
      */
     sample.SetAttribute(AttributeKeys::Error(), xError);
    }
    else
    {
      /*
       * There is no X-Error: header.
       */
      why = "GatewayError w/o an X-Error attribute!";
      sample.SetAttribute(AttributeKeys::Attention(),
                          "Failed to upload sample to Immune System without an error attribute");
      sample.Status(SampleStatus::attention);
    }

    msg = "Error submitting a sample, error = " + why;
    Logger::Log(Logger::LogWarning, Logger::LogIcePackSubmission, msg.c_str());
  }
  else
  {
    /*
     * Not a gateway error.
     */

    if ((IcePackAgentComm::NetworkError         == errorCode) ||
        (IcePackAgentComm::NetworkCriticalError == errorCode))
      commError = true;

    /*
     * Note carefully that retryWithDelay and retryWithoutDelay
     * may be set by HandleCommError().  In fact, Stop() could
     * be triggered too...
     */
    CommErrorHandler::HandleCommError(errorCode,
                                      sampComm.SystemExceptionCode(),
                                      errorInfo,
                                      retryWithDelay,
                                      retryWithoutDelay,
                                      Logger::LogIcePackSubmission,
                                      "error submitting a sample, error = ");

    if ((!GlobalData::stop) && (!retryWithDelay) && (!retryWithoutDelay))
    {
      switch (errorCode)
      {
        case IcePackAgentComm::ProcessTimedOutError :
        case IcePackAgentComm::ProcessTerminationError :
        case IcePackAgentComm::ProcessSyncError :
        case IcePackAgentComm::NetworkCriticalError :
          sample.SetAttribute(AttributeKeys::Attention(),
                              "Failed to resolve communication error");
          sample.Status(SampleStatus::attention);
          break;
        case IcePackAgentComm::UnknownError :
          sample.SetAttribute(AttributeKeys::Attention(),
                              "Failed to resolve unknown communication error");
          sample.Status(SampleStatus::attention);
          break;
        case IcePackAgentComm::InvalidRedirectError :
          sample.SetAttribute(AttributeKeys::Attention(),
                              "Redirection error at gateway");
          sample.Status(SampleStatus::attention);
          break;
        default :
          break;
      }
    }
  }

  if (commError)
    Attention::Set(Attention::Submit);
  else
    Attention::UnSet(Attention::Submit);

  /*
   * Stamp the changes back into the sample.
   */
  sample.Commit();

  if ((!GlobalData::stop) && retryWithDelay)
    GlobalData::SleepInSegments(GlobalData::RetryInterval,
                                Logger::LogIcePackSubmission,
                                "Pausing before retrying a sample submission upload");

  return ((!GlobalData::stop) && (retryWithDelay || retryWithoutDelay));
}
