// BlessedMonitor.cpp: implementation of the BlessedMonitor class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>
#include <list>

#include "BlessedMonitor.h"
#include <MyTraceClient.h>
#include <IcePackAgentComm.h>
#include <NAVScan.h>
#include <WinRegistry.h>
#include <SystemException.h>
#include "EventLog.h"

#include "GlobalData.h"
#include "Logger.h"
#include "CommErrorHandler.h"
#include "ScannerSubmittor.h"
#include "AttributeKeys.h"
#include "Attention.h"

/* ----- */

using namespace std;

/* ----- */

/*
 * This file extensively reformatted in an effort to make it
 * clearer.  Many of the comments are those of the original
 * author, much to my surprise.  I have not checked these
 * comments for correctness.
 *
 * In addition, I have added my own comments -- the original
 * author used the C++ double-slash comment style, but I
 * prefer the classic C slash-star-blah-blah-star-slash
 * style, so you can tell them apart.
 *
 * Also, I am the one that can spell.
 *
 * [inw 2000-06-04]
 */

/* ----- */

// BlessedMonitor constructor
//
// Overview:
//   The constructor ensures that a valid definition set is available and
//   configures IcePack to be using the most recent version available on the
//   Komodo server or downloads a new definition set if no valid definition set
//   is available on the Komodo server.
//
// Algorithm:
//   (Simplified version, real code is slightly different)
//   If the definitionActiveSequence and definitionActiveBlessed keys exist and
//     the definition set specified by them exist and are valid then set the
//     sequence number and blessed state as the current definition set for IcePack
//   else
//    scan the definitions directory for installed definition sets and create a
//       list of available defintion sets.
//     In decending order (highest to lowest) find the first valid definition set.
//     If a valid definition set is found then set configure IcePack to use that
//       definition set as IcePack's current definition set (unblessed).
//     else download the latest blessed definition set from the gateway and mark
//       it as IcePack's current definition set (blessed).
//
// Globals static methods/members Used
//   stop                           bool         if true exit this thread.  Set by SCM when service is
//                                               told to stop or the system is shutting down.
//   regCurrentRoot                 const char*  registry directory where IcePack values are stored.
//   latestInstalledSigKey          const char*  registry key for definitionActiveSequence value.
//   latestInstalledSigIsBlessedKey const char*  registry key for definitionActiveBlessed value.
//   DefLibraryDirectory            const char*  registry key where signatures directory is stored.
BlessedMonitor::BlessedMonitor()
{
  WinRegistry cRoot(HKEY_LOCAL_MACHINE, GlobalData::regCurrentRoot);
  uint        seqNum           = 0;
  ulong       reqSeqNumLen     = 20;
  ulong       reqSeqBlessedLen = 10;
  uint        blessedSeqNum    = 0;
  string      defVersion;
  uchar       regSeqNumAsChar[20];

  // See if the old DWORD version is present.  If it is delete the old entries and
  // let IcePack create new values in the string format.
  //
  // NOTE: This code will be dead wood after the first time it is run.  This dead
  //       wood is required because of Ed's insistance that private data be
  //       exposed in a format that Symantec can deal, which happens to only include
  //       strings.
  WinRegistry::ValueType  vType;
  if ((cRoot.GetValueType(GlobalData::latestInstalledSigKey, vType)) && (WinRegistry::vtString != vType))
  {
    cRoot.DeleteValue(GlobalData::latestInstalledSigKey);
    cRoot.DeleteValue(GlobalData::latestInstalledSigIsBlessedKey);
  }

  // Restore the latest blessed global data member variable if possible
  if ((cRoot.QueryValue(GlobalData::definitionBlessedSequenceKey, regSeqNumAsChar, reqSeqNumLen)) && (reqSeqNumLen > 1))
  {
    blessedSeqNum = atoi((char*) regSeqNumAsChar);
    if (NAVScan::ExplicitDefVersion(defVersion, blessedSeqNum))
      GlobalData::SetNewestSigAvailable(blessedSeqNum, true);
  }

  reqSeqNumLen = 20;

  // See if we can just pick off where IcePack left off the last time it was run.
  // (check registry value and verify that it is still valid information).

  // Okay, if that did not work we will have to check the disk for any definition
  // sets.  Search from highest sequence number to lowest sequence number until we find
  // a valid definition set.

  if (0 == seqNum)
  {
    WIN32_FIND_DATA info;
    HANDLE          findHandle;
    string          searchPattern(GlobalData::DefLibraryDirectory());
    list<int>       sigs;

    searchPattern += "\\0*.";

    findHandle = FindFirstFile(searchPattern.c_str(), & info);

    while (INVALID_HANDLE_VALUE != findHandle && !GlobalData::stop)
    {
      if ((FILE_ATTRIBUTE_DIRECTORY & info.dwFileAttributes) && (0 != (seqNum = atoi(info.cFileName))))
        sigs.push_back(seqNum);

      if (TRUE != FindNextFile(findHandle, &info))
      {
        FindClose(findHandle);
        findHandle = INVALID_HANDLE_VALUE;
      }
    }

    if (GlobalData::stop)
      return;

    seqNum = 0;
    sigs.sort();
    sigs.reverse();

    list<int>::iterator i;
    for (i = sigs.begin(); i != sigs.end() && !GlobalData::stop; i++)
    {
      if (NAVScan::ExplicitDefVersion(defVersion, *i))
      {
        seqNum = *i;
        break;
      }
    }
  }

  if (GlobalData::stop)
    return;

  //  If an existing definition set exists then use it,
  //  else get the latest blessed version from the gateway.
  //
  //  And be careful to preserve the "blessed/unblessed"
  //  characteristic of the existing definitions.  (EJP 7/26/00)

  if (seqNum > 0)
    {
      bool blessed = ((seqNum == blessedSeqNum) ? true : false);
      GlobalData::SetNewestSigAvailable(seqNum, blessed);
    }
  else
  {
#if 0
    // It is premature to call GlobalData::SetSigToDownload() with
    // sequence number "1" here.  It will be called below in
    // BlessedMonitor::ThreadHandlerProc() with the actual blessed
    // sequence number, when one is received from the gateway.
    // (EJP 5/15/00)
    GlobalData::SetSigToDownload(1, true);
#endif /* 0 */
    /*
     * Note that it's very important that we not try to download
     * the signatures here, which is what the original code did.
     * This is the bloody CONSTRUCTOR, you don't do lengthy
     * failure-prone operations in the constructor, now do you?
     *
     * The thread over in ScannerSubmittor will notice that it needs
     * to download anyway.
     *
     * I grant you, it would be nice to be able to download the
     * signatures here, but you can't.  Terribly sorry, and all that.
     *
     * This fix should deal with the 'failure to copy registry' bug
     * about which people have been complaining forever.
     */
#if 0
    MyTraceClient                   traceClient;
    IcePackAgentComm::SignatureComm sigComm;
    bool                            traceSigComm(false);

    ScannerSubmittor::DownloadSig(sigComm, traceClient, traceSigComm, 1, true);
#endif /* 0 */
  }
}

/* ----- */

BlessedMonitor::~BlessedMonitor()
{
}

/* ----- */

// BlessedMonitor::ThreadHandlerProc()
//
// Overview:
//   This thread checks for blessed signatures from the gateway that are newer
//   then the latest signature already on this machine.
//
// Algorithm:
//   (Simplified version, real code is slightly different)
//   Do
//     get the sequence number of the latest blessed signature file from the gateway
//     if newer than the latest signatures on this machine
//       signal ScannerSubmittor thread to download the signatures onto this machine
//   while service not shutting down & sleep DefCheckInterval milliseconds
//
// Globals static methods/members Used
//   stop              bool         if true exit this thread.  Set by SCM when service is
//                                  told to stop or the system is shutting down.
//   GatewayURL        std::string  the URL of the gateway. This value comes from two
//                                  registry values, webGatewayName and webGatewayPort.
//   DefCheckInterval  uint         How long often to check for the latest blessed
//                                  signature sequence number.  This value is set from
//                                  the definitionCheckInterval registry value times 60000.
unsigned BlessedMonitor::ThreadHandlerProc()
{
  EntryExit           entryExit(Logger::LogIcePackBlessedMonitor, "ThreadHandlerProc()");
  SystemException::Init();

  MyTraceClient                   traceClient;
  IcePackAgentComm::SignatureComm sigComm;
  string                          sequenceNumber;
  uint                            seqNum;
  bool                            tracingEnabled = false;

  if (GlobalData::stop)
    return -1;

  do
  {
    Logger::Log(Logger::LogDebug,
                Logger::LogIcePackBlessedMonitor,
                "Ask gateway for sequence number of latest blessed signature");
    try
    {
      if ((Logger::WillLog(Logger::LogDebug, Logger::LogIcePackBlessedMonitor)) &&
          (Logger::WillLog(Logger::LogDebug, Logger::LogComm)))
      {
        if (!tracingEnabled)
        {
          string  traceFileName(GlobalData::TraceFileName());

          traceClient.EnableTracing();
          sigComm.SetTraceClient(traceClient, traceFileName.c_str(), traceFileName.c_str(), "IcePack");
          tracingEnabled = true;
        }
      }
      else if (tracingEnabled)
      {
        traceClient.EnableTracing(false);
        tracingEnabled = false;
      }

      bool succeeded = GetLatestBlessedSeqNum(sigComm, seqNum, Logger::LogIcePackBlessedMonitor);

      if ((succeeded) && (!GlobalData::stop))
      {
        char buffer[64];

        sprintf(buffer, "Latest blessed signature(%d) reported by gateway", seqNum);
        Logger::Log(Logger::LogInfo, Logger::LogIcePackBlessedMonitor, buffer);
        GlobalData::SetSigToDownload(seqNum, true);
      }
    }

    catch (...)
    {
      GlobalData::HandleException("BlessedMonitor::ThreadHandlerProc()", Logger::LogIcePackBlessedMonitor);
    }

    // This method will wake up when the interval has passed or IcePack is shutting
    // down.  It also handles the issue of configuration changes while asleep.
    GlobalData::SleepInSegments(GlobalData::NewBlessedInterval,
                                Logger::LogIcePackBlessedMonitor,
                                "Normal pause between queries to the gateway");
  }
  while (!GlobalData::stop);

  return 0;
}

/* ----- */

// BlessedMonitor::GetSeqNum
//
// Overview:
//   This method is used to get the signature sequence number of the latest blessed or
//   unblessed definition set from the gateway.
//   This method will try until it either succeeds or IcePack is shutdown.
//
// Algorithm:
//   (Simplified version, real code is slightly different)
//   Do
//     Get the latest blessed or unblessed signature sequence number from the gateway.
//   while did not succeed and IcePack not shutting down
//
// Globals static methods/members Used
//   stop              bool         if true exit this thread.  Set by SCM when service is
//                                  told to stop or the system is shutting down.
//   GatewayURL        std::string  the URL of the gateway. This value comes from two
//                                  registry values, webGatewayName and webGatewayPort
//   GatewaySSLURL     std::string  the URL of the SSL gateway. This value comes from two
//                                  registry values, webGatewaySSLName and webGatewaySSLPort
//   RetryInterval     uint         How long to wait between unsuccessful attempts
bool BlessedMonitor::GetSeqNum(IcePackAgentComm::SignatureComm& sigComm,
                               uint&                            seqNum,
                               const Logger::LogSource          source,
                               bool                             getBlessed)
{
  bool                        retryWithDelay;
  bool                        retryWithoutDelay;
  ErrorInfo                   errorInfo;
  string                      seqNumStr;
  bool                        succeeded = false;
  string                      headers   = "";
  IcePackAgentComm::ErrorCode commError = IcePackAgentComm::NoError;

  do
  {
    retryWithDelay    = false;
    retryWithoutDelay = false;

    string  url;
    // There is no reason to use SSL for "HEAD headSignatureSet"
    // transactions, even if SSL is enabled for definition downloads,
    // since they do not carry any private customer information or
    // proprietary Symantec information.  By avoiding SSL here, we
    // reduce the processor load on the customer entry point machine
    // with no loss of security.  (EJP 6/16/00)
#if 0
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
#endif /* 0 */
      url = GlobalData::GatewayURL();

    if (getBlessed)
      succeeded = sigComm.GetLatestBlessed(url, headers, seqNumStr);
    else
      succeeded = sigComm.GetLatest(url, headers, seqNumStr);

    if (succeeded)
      seqNum = atoi(seqNumStr.c_str());
    else
    {
      commError = sigComm.Error();

      if (IcePackAgentComm::TerminatedByCaller == commError)
        /*
         * Excellent Klapperism here.  Did he mean to fill this
         * in?  Did he mean it to do nothing?  Who knows?!
         */
        ;
      else if (IcePackAgentComm::GatewayError == commError)
      {
        // If the gateway returned an error, log it and try again
        // after waiting for the configured retry interval.  (EJP
        // 5/15/00)
        errorInfo.GatewayError(ErrorInfo::gwUnknown);

        Logger::Log(Logger::LogWarning,
                    source,
                    (getBlessed ? "Gateway error trying to get latest blessed sequence number" :
                                  "Gateway error trying to get latest unblessed sequence number"));

        retryWithDelay = TRUE;
      }
      else
        CommErrorHandler::HandleCommError(commError,
                                          sigComm.SystemExceptionCode(),
                                          errorInfo,
                                          retryWithDelay,
                                          retryWithoutDelay,
                                          source, 
                                          (getBlessed ? "error getting latest blessed sequence number" :
                                                        "error getting latest unblessed sequence number"));

      if (commError)
        Attention::Set(Attention::Download);
      else
        Attention::UnSet(Attention::Download);

      if (retryWithDelay)
        GlobalData::SleepInSegments(GlobalData::RetryInterval,
                                    source,
                                    (getBlessed ?
                                       "Attempt to get the sequence number of latest blessed definition set failed, pausing before a retry" :
                                       "Attempt to get the sequence number of latest unblessed definition set failed, pausing before a retry"));
    }
  }
  while ((!succeeded) && (!GlobalData::stop) && (retryWithDelay || retryWithoutDelay));

  if (!succeeded)
    seqNum = 0;
  else
  {
    if (commError)
      Attention::Set(Attention::Download);
    else
      Attention::UnSet(Attention::Download);
  }

  return succeeded;
}
/* ----- End Transmission ----- */
