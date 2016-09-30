// Sample.cpp: implementation of the Sample class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Sample.h"

#include <algorithm>

#include <AVISfile.h>
#include <AttributeKeys.h>
#include <CMclAutoLock.h>
#include <MappedMemory.h>
#include <XScanResults.h>
#include <XAnalysisState.h>
#include <XError.h>
#include <md5\MD5Calculator.h>

#include "VQuarantineServer.h"
#include "GlobalData.h"
#include "ScannerSubmittor.h"
#include "SampleStatus.h"
#include "AttributesParser.h"

#include "Logger.h"
#include "Attention.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/* ----- */

Sample::~Sample()
{
  Close();
}

/* ----- */

Sample& Sample::operator=(const Sample& cpy)
{
  if (this == &cpy)
    return *this;

  SampleBase::operator=(cpy);

  action       = cpy.action;
  neededSeqNum = cpy.neededSeqNum;
  sigPriority  = cpy.sigPriority;

  return *this;
}

/* ----- */

void Sample::Priority(uint newPriority)
{
  EntryExit entryExit(Logger::LogIcePackSample, "Priority");

  NullCheck(IcePackException::SamplePriority);

  if (newPriority != priority)
  {
    char buffer[64];

    priority = newPriority;

    sprintf(buffer, "%d", newPriority);
    SetAttribute(AttributeKeys::SamplePriority().c_str(), buffer);

    Commit();

    if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackSample))
    {
      sprintf(buffer, "Sample(%x) priority changed to %d", DebugKey(), newPriority);
      Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, buffer);
    }
  }
}

/* ----- */

void Sample::SigPriority(uint newSigPriority)
{
  EntryExit entryExit(Logger::LogIcePackSample, "SigPriority");

  NullCheck(IcePackException::SampleSigPriority);

  if (newSigPriority != sigPriority)
  {
    char buffer[64];

    sigPriority = newSigPriority;

    sprintf(buffer, "%d", newSigPriority);
    SetAttribute(AttributeKeys::SignaturePriority().c_str(), buffer);

    Commit();

    if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackSample))
    {
      sprintf(buffer, "Sample(%x) signature priority changed to %d", DebugKey(), newSigPriority);
      Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, buffer);
    }
  }
}

/* ----- */

SampleStatus::Status  Sample::Status(SampleStatus::Status newStatus)
{
  EntryExit entryExit(Logger::LogIcePackSample, "Status");

  NullCheck(IcePackException::SampleStatus);

  if (newStatus == status)
    return status;

  /*
   * Moved this date construction out of the switch() in order
   * to allow for the new X-Date-Sample-Status attribute.
   * INW 2000-02-02
   */
  DateTime now;
  string   dateString;
  now.AsHttpString(dateString);

  char  asChar[20];
  SampleStatus::Status  oldStatus = status;
  status  = newStatus;

  sprintf(asChar, "%d", newStatus);
  SetAttribute(AttributeKeys::SampleStatus(), asChar);
  SetAttribute(AttributeKeys::SampleStatusReadable(), SampleStatus::ToConstChar(newStatus));

  switch (newStatus)
  {
    case SampleStatus::unneeded:
    case SampleStatus::installed:
    case SampleStatus::error:
    case SampleStatus::attention:
      SetAttribute(AttributeKeys::DateCompleted(), dateString);
      finalStatus = true;
      break;
  }

  /*
   * Change the attribute that records when the status last changed.
   * Use the date string we already made.
   */
  SetAttribute(AttributeKeys::DateSampleStatus(), dateString);

  Commit();

  if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackSample))
  {
    char msg[64];

    sprintf(msg, "Sample(%x) status changed to %s", DebugKey(), SampleStatus::ToConstChar(newStatus));
    Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, msg);
  }

  return oldStatus;
}

/* ----- */

void Sample::Clear()
{
  char      buff[32];

  sprintf(buff, "Clear(%x)", DebugKey());
  EntryExit   entryExit(Logger::LogIcePackSample, buff);

  priority      = 0;
  sigPriority   = 0;
  gatewayCookie = "";
  status        = SampleStatus::unknown;
  sigSeqNum     = 0;
  changes       = 0;
  finalStatus   = false;

  forwardTime.Null(true);
}

/* ----- */

bool Sample::ParseSpecialAttributes(const string& attrs)
{
  EntryExit entryExit(Logger::LogIcePackSample, "ParseSpecialAttributes");

  static const string priorityTag(AttributeKeys::SamplePriority());
  static const string statusTag(AttributeKeys::SampleStatus());
  static const string forwardedTag(AttributeKeys::DateForwarded());
  static const string gatewayCookieTag(AttributeKeys::AnalysisCookie());
  static const string sigSeqNumTag(AttributeKeys::ScanSignaturesSequence());
  static const string sigNeededTag(AttributeKeys::SignatureSequence());
  static const string sigPriorityTag(AttributeKeys::SignaturePriority());
  static const string changesTag(AttributeKeys::SampleChanges());
  static const string finalTag(AttributeKeys::DateCompleted());

  string              key;
  string              value;
  char                idAsChar[20];
  int                 pos             = 0;
  bool                foundPriority   = false;
  bool                foundForwarded  = false;

  sprintf(idAsChar, "%x", DebugKey());

  while (string::npos != (pos = AttributesParser::GetNextKeyValuePair(attrs, pos, key, value)))
  {
    AttributesParser::RightCase(key);

    if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSample))
    {
      string              msg;

      msg  = "Sample(";
      msg += idAsChar;
      msg += ") - " + key + ": " + value;

      Logger::Log(Logger::LogDebug, Logger::LogIcePackSample, msg.c_str());
    }

    if (statusTag == key)
      status = SampleStatus::FromInt(atoi(value.c_str()));
    else if (priorityTag == key)
    {
      priority = atoi(value.c_str());
      foundPriority = true;
    }
    else if (forwardedTag == key)
    {
      foundForwarded  = true;
      forwardTime   = DateTime(value);
    }
    else if (gatewayCookieTag == key)
      gatewayCookie = value;
    else if (sigSeqNumTag == key)
      sigSeqNum = (uint) atoi(value.c_str());
    else if (sigNeededTag == key)
      neededSeqNum= (uint) atoi(value.c_str());
    else if (sigPriorityTag == key)
      sigPriority= (uint) atoi(value.c_str());
    else if (changesTag == key)
      changes = (ulong) atol(value.c_str());
    else if (finalTag == key)
      finalStatus = true;
  }

  return foundPriority && foundForwarded && 0 < changes;
}

/* ----- */

void Sample::Reset()
{
  EntryExit entryExit(Logger::LogIcePackSample, "Reset");

  Clear();

  if (NULL != id)
  {
    if (GetAllAttributes(attributes))
    {
      ParseSpecialAttributes(attributes);
    }
    else
      throw IcePackException(IcePackException::SampleSetID, "GetAllAttributes failed");
  }
}

/* ----- */

bool Sample::Update(string& attrs)
{
  EntryExit entryExit(Logger::LogIcePackSample, "Update");

  static const string statusTag(AttributeKeys::SampleStatus());
  static const string sigSeqNumTag(AttributeKeys::ScanSignaturesSequence());
  static const string sigNeededTag(AttributeKeys::SignatureSequence());
  static const string sigMinimumTag(AttributeKeys::SignatureSequenceMinimum());
  static const string errorTag(AttributeKeys::Error());
  static const string gatewayCookieTag(AttributeKeys::AnalysisCookie());
  static const string dateAnalyzedTag(AttributeKeys::DateAnalyzed());
  static const string analysisStateTag(AttributeKeys::AnalysisState());
  static const string finalTag(AttributeKeys::DateCompleted());
  static const string analysisGatewayTag(AttributeKeys::AnalysisGateway());
  string              key;
  string              value;
  string              analysisState;
  bool                changed;
  bool                sigTagChanged;
  string              msg;
  char                idAsChar[20];
  int                 pos               = 0;
  bool                somethingChanged  = false;
  bool                analysisFinished  = false;
  bool                error             = false;

  sprintf(idAsChar, "%x", DebugKey());

  while (string::npos != (pos = AttributesParser::GetNextKeyValuePair(attrs, pos, key, value)))
  {
    changed       = false;
    sigTagChanged = false;

    AttributesParser::RightCase(key);

    if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSample))
    {
      msg = "Sample(";
      msg += idAsChar;
      msg += ") -- ";
      msg += key;
      msg += ": ";
      msg += value;
      Logger::Log(Logger::LogDebug, Logger::LogIcePackSample, msg.c_str());
    }

    if (statusTag == key)
    {
      SampleStatus::Status  newStatus = SampleStatus::FromInt(atoi(value.c_str()));

      if (newStatus != status)
      {
        Status(newStatus);
        somethingChanged = true;
      }
    }
    else if (dateAnalyzedTag == key)
    {
      analysisFinished  = true;
      changed = true;
    }
    else if (sigSeqNumTag == key)
    {
      uint newSeqNum = (uint) atoi(value.c_str());
      if (newSeqNum != sigSeqNum)
      {
        sigSeqNum = newSeqNum;
        changed = true;
      }
    }
    else if (sigNeededTag == key)
    {
      neededSeqNum = (uint) atoi(value.c_str());
      changed       = true;
      sigTagChanged = true;
    }
    else if (analysisStateTag == key)
    {
      string  tmp;
      GetAttribute(analysisStateTag, tmp);
      if (value != tmp)
        changed = true;

      analysisState = value;
    }
    else if (!strncmp(errorTag.c_str(), key.c_str(), errorTag.size()))
    {
      error = true;

      if (SampleStatus::error != status)
      {
        Status(SampleStatus::error);
        somethingChanged = true;
      }
    }
    else if (gatewayCookieTag == key)
    {
      if (gatewayCookie != value)
      {
        changed = true;
        gatewayCookie = value;
      }
    }
    else if (finalTag == key)
    {
      if (!finalStatus)
      {
        finalStatus = true;
        changed = true;
      }
    }
    else if (analysisGatewayTag == key)
    {
      string tmp;
      GetAttribute(analysisGatewayTag, tmp);
      if (value != tmp)
        changed = true;
    }

    /*
     * This next section of code has undesirable effects on stripped samples.
     * It causes the X-Sample-Checksum attribute in the sample to get overwritten
     * with the checksum coming back from the gateway in the response message.
     * This checksum is, alas, the checksum of the stripped sample, and so this
     * causes all manner of bad things to happen later on.
     */
#if 0
    else if (!strncmp("X-", key.c_str(), 2))    // we ignore http standard attributes
    {
      int begining  = attributes.find(key, 0);
      if (string::npos != begining)     // key found
      {
        begining = attributes.find(":", begining);
        begining = attributes.find_first_not_of(" \t", begining +1);
        int end = attributes.find("\n", begining);
        string  oldValue;

        if (end >= 1 && '\r' == attributes[end-1])
          oldValue = attributes.substr(begining, (end - begining) -1);
        else
          oldValue = attributes.substr(begining, end - begining);

        if (oldValue != value)
          changed = true;
      }
      else
        changed = true;
    }
#endif /* 0 */

    if (changed)
    {
      SetAttribute(key.c_str(), value.c_str());
      if (sigTagChanged)
        SetAttribute(sigMinimumTag.c_str(), value.c_str());
      somethingChanged = true;
    }
  }

  if (analysisFinished && !error)
  {
    /*
     * Do we have a 'needed' number?
     */
    if (0 != neededSeqNum)
    {
      bool  dummyBlessed;
      uint  latestAvailableNum;

      /*
       * Determine the latest locally-available sequence.
       */
      latestAvailableNum = GlobalData::GetNewestSigAvailable(dummyBlessed);

      if (neededSeqNum > latestAvailableNum)
      {
        /*
         * Oh dear, we need to go and get something from the gateway.
         * Mark the sample as needed -- code elsewhere takes care of
         * requesting the download thread to do its thing.
         */
        Status(SampleStatus::needed);
      }
      else /* (! (neededSeqNum > latestAvailableNum)) */
      {
        /*
         * We already have (locally) a suitable sequence (the same
         * or higher).  Go ahead and scan with it -- Scan() takes care
         * of setting the relevant attributes.
         */
        Scan(latestAvailableNum, false);
      }   
    }
    else if (XAnalysisState::Declined() == analysisState)
      Status(SampleStatus::error);
    else
      Status(SampleStatus::unneeded);
  }

  if (somethingChanged)
  {
    Commit();

    GetAllAttributes(attributes);

    CMclAutoLock      lock(GlobalData::critSamplesList);
    list<Sample>::iterator  j = find_if(GlobalData::samplesList.begin(),
                    GlobalData::samplesList.end(),
                    CompSampleByKeyOnly(SampleKey()));
    if (j != GlobalData::samplesList.end())
      *j = *this;
  }

  return somethingChanged;
}

/* ----- */

/*
 * This method heavily reformatted and commented inw 2000-05-24.
 *
 * Note that this function perpetuates the confusion in the original
 * code author's mind (this confusion is exhibited elsewhere in the code)
 * on the subject of 'int's and 'long's.  The poor chap is convinced
 * they are freely interchangable, and if I see him again I will beat
 * him firmly with my stout stick.  The one with
 * 'int != long != sizeof(unsigned char *)' written on the side.
 *
 * And now, we return to your regularly scheduled programming.
 * [inw 2000-05-24]
 */
/*
 * The first parameter is the sequence number with which to scan the
 * sample (this sequence has already been downloaded).  The second
 * is to let us know whether or not this is the first time this
 * particular sample has been scanned by IcePack.
 */
NAVScan::ScanRC Sample::Scan(uint scanSeqNum, bool isThisTheInitialScan)
{
  EntryExit         entryExit(Logger::LogIcePackSample, "Scan");

  enum {
         invalidWhatToDoValue,                              /* Initialisation value. */
         goDownPub,                                         /* Do diddly-squat. */
         finalStateNow,                                     /* All done, see [finalState] for state ID. */
         clearToSubmit,                                     /* We need to submit this one. */
         itsAllGonePearShaped                               /* Set attention flag on sample. */
       }            whatToDoWithTheDamnSample;
  enum {
         automatic,                                         /* Sample was captured automatically. */
         manual                                             /* Sample was captured by a silly user. */
       }            sampleReason;
  bool                  gatewayStitchedUsUp;
  string                analysisState;
  SampleStatus::Status  sampleStatus;
  NAVScan::ScanRC       scanRC;
  string                mapName;
  string                virusName;
  string                navVersion;
  uint                  virusID;
  DateTime              now;
  char                  newestSigAsChar[20];
  string                httpNow;
  string                sampleFileName;
  char                  virusIDCharStr[32];
  string                mappedPrefix("\\\\.\\mapped\\");
  string                extension;
  int                   dot;
  unsigned long         sampleSize;
  unsigned long         outSize;
  unsigned char        *ptr;
  char                  baseFileName[64];
  string                sampleFileID;
  unsigned long         sampleFileIDNum;
  string                sigSeqMinimum;
  unsigned long         sigSeqMinimumNum;
  string                sampleReasonStr;
  string                sigSeqClient;
  unsigned long         sigSeqClientNum;
  string                clientVirusName;
  bool                  didClientFindVirus;
  string                sampleCRC;
  string                virginCRC;
  MD5Calculator         md5Virgin;
  string                navErrStr;
  int                   codePathTaken = 0;
  char                 *scanResult    = NULL;
  bool                  scanOk        = true;
  bool                  boCommError   = false;
  bool                  useVirName    = true;

  /*
   * We need this at several points below.
   */
  sprintf(newestSigAsChar, "%08u", scanSeqNum);

  /*
   * Initialise these to something inocuous.
   */
  whatToDoWithTheDamnSample = invalidWhatToDoValue;
  sampleStatus              = SampleStatus::unknown;
  analysisState             = XAnalysisState::UnknownSampleState();
  gatewayStitchedUsUp       = false;

  /*
   * First, attempt to get these attributes from the sample.  If anything
   * goes wrong (this does not include the fact that the attribute does not
   * exist), give up.
   */
  if ((!GetAttribute(AttributeKeys::SampleFile(),                   sampleFileName))  ||
      (!GetAttribute(AttributeKeys::SampleFileID(),                 sampleFileID))    ||
      (!GetAttribute(AttributeKeys::SampleReason(),                 sampleReasonStr)) ||
      (!GetAttribute(AttributeKeys::ClientScanSignaturesSequence(), sigSeqClient))    ||
      (!GetAttribute(AttributeKeys::ClientScanVirusName(),          clientVirusName)) ||
      (!GetAttribute(AttributeKeys::SignatureSequenceMinimum(),     sigSeqMinimum)))
    throw IcePackException(IcePackException::SampleScan,
                      "GetAttribute(...SampleFile()) failed");

  /*
   * Determine the sample's reason for being here.
   */
  sampleReason = ((sampleReasonStr == "manual") ? manual : automatic);

  /*
   * Determine if, when the original client scanned the file
   * (at the workstation), it found a virus.
   */
  didClientFindVirus = ((clientVirusName.size()) ? true : false);

  /*
   * We need to get the extension from the original sample name,
   * because the scanners need this.  If there is no dot in the
   * name, then give up.
   */
  dot = sampleFileName.find_last_of(".");
  if (std::string::npos != dot)
    extension= sampleFileName.substr(dot, 4);
  else
    extension = "";

  /*
   * Get numeric versions of all these things.  They will be zero if the
   * string is empty, this is intentional.
   */
  sampleFileIDNum  = atoi(sampleFileID.c_str());
  sigSeqMinimumNum = atoi(sigSeqMinimum.c_str());
  sigSeqClientNum  = atoi(sigSeqClient.c_str());

  /*
   * Construct the name for the memory-mapped file.  Note that this includes
   * the sample's ID (this is to make log file autopsies easier).
   */
  mapName = mappedPrefix;
  sprintf(baseFileName, "sample_%x%s", sampleFileIDNum, extension.c_str());
  mapName += baseFileName;

  /*
   * Now get started on building the memory mapped file.
   */
  if (! Open())
    throw IcePackException(IcePackException::SampleScan, "Open failed");

  /*
   * Get sample size (fans of Klapperisms will note that this style is
   * what the cognoscente call 'Klassic Klapper').
   */
  Size(sampleSize);

  /*
   * Create the map.  Note that whilst the _subtraction_ of sizeof(ulong)
   * from the base pointer is wrong and looks exception-prone, it's actually
   * 'safe' (ahem) because the underlying classes allocated sizeof(ulong)
   * bytes _back_ from the start of the returned base pointer anyway.
   *
   * I am too scared to fix this, as if it goes wrong, the effects would
   * be disastrous, and probably unpredictable.
   */
  MappedMemory  memCopy(baseFileName, sampleSize + sizeof(ulong));
  ptr = memCopy.Begining();
  ptr -= sizeof(ulong);
  *((ulong *) ptr) = sampleSize;

  /*
   * Attempt to swallow the whole file.
   */
  if (! Read(sampleSize, outSize, (char *)memCopy.Begining()))
    throw IcePackException(IcePackException::ScannerSubmittor_ScanPendingSamples, "Read failed");

  /*
   * Check the size.
   */
  if (outSize != sampleSize)
    throw IcePackException(IcePackException::SampleScan, "outSize != sampleSize");
  
  /*
   * Attempt to close the file.
   */
  if (! Close())
    throw IcePackException(IcePackException::SampleScan, "Close failed");

  /*
   * Calculate the CRC for the memory copy of the file.
   */
  md5Virgin.CRCCalculate(memCopy.Begining(), outSize);
  md5Virgin.Done(virginCRC);

  /*
   * Compare the newly-calculated checksum against the copy stored
   * in the sample.
   */
  GetAttribute(AttributeKeys::SampleCheckSum(), sampleCRC);
  if (0 != stricmp(virginCRC.c_str(), sampleCRC.c_str()))
  {
    string errMsg;
    errMsg = "Checksum (" + sampleCRC + ") stored in sample does not match checksum (" + virginCRC + ") calculated for content returned from Quarantine Service";

    SetAttribute(AttributeKeys::Attention(), errMsg);
    Status(SampleStatus::attention);
    throw IcePackException(IcePackException::SampleScan, errMsg);
  }

  /*
   * Call the actual scanner.
   */
  scanRC = NAVScan::ScanExplicit(mapName, 0, false, virusName, scanSeqNum, navVersion, virusID);
  now.Now();

  /*
   * Get an HTTP-compliant version of the date... again, in many
   * execution paths, we won't use this, but it's conceptually
   * clearer to create it now rather than create it N different
   * times later on in various paths.
   */
  now.AsHttpString(httpNow);

  /*
   * ENTIRELY NEW POST-SCAN logic added inw 2000-06-08.
   */

  /*
   * This first switch{} exists solely to turn the return
   * code from the scanner into a string for the
   * X-Scan-Result attribute.  Most of the work is done
   * in the second switch{}.
   */
  switch (scanRC)
  {
    case NAVScan::RepairTriedAndError :
      scanResult = (char *)XScanResults::BadRepair();
      break;
    case NAVScan::RepairTriedAndFailed :
      scanResult = (char *)XScanResults::UnRepairable();
      break;
    case NAVScan::NotInfectable :
      useVirName = false;
      scanResult = (char *)XScanResults::UnInfectable();
      break;
		case NAVScan::NotInfected :
      useVirName = false;
      scanResult = (char *)XScanResults::NoDetect();
      break;
    case NAVScan::Repaired :
      scanResult = (char *)XScanResults::Repaired();
      break;
    case NAVScan::UnSubmittable :
      scanResult = (char *)XScanResults::UnSubmittable();
      break;
    case NAVScan::Heuristic :
      scanResult = (char *)XScanResults::Heuristic();
      break;
    case NAVScan::NoRepairInstructions :
      scanResult = (char *)XScanResults::NoRepair();
      break;
    case NAVScan::ScannerServiceError :
    case NAVScan::WaitForScanTimedOut :
      boCommError = true;
      /* FALL THROUGH */
    case NAVScan::BadScan :
    case NAVScan::NavCrashed :
    case NAVScan::NavVcdInitError :
    case NAVScan::NavDefinitionFileError :
    case NAVScan::NavLoadDefFileError :
    case NAVScan::SigDefMissing :
    case NAVScan::CommWithScannerOffline :
    case NAVScan::BadResponceFromScanner :
    case NAVScan::UnknownError :
    default :
      useVirName = false;
      scanOk     = false;
      scanResult = (char *)XScanResults::BadScan();
      break;
  }

  /*
   * Use the result of the above switch{} to set the
   * value of the X-Scan-Result attribute.
   */
  SetAttribute(AttributeKeys::ScanResult(), scanResult);

  /*
   * If we came through above and useVirName is still true,
   * then write the virus name and ID into the sample.
   */
  if (useVirName)
  {
    sprintf(virusIDCharStr, "%u", virusID);
    SetAttribute(AttributeKeys::ScanVirusID(),   virusIDCharStr);
    SetAttribute(AttributeKeys::ScanVirusName(), virusName);
  }

  /*
   * This is the second switch{}, and this determines what
   * action to take based on a variety of information, namely:
   *   + What did the scanner return? [scanRC]
   *   + Is this the first time IcePack has scanned the
   *     sample? [isThisTheInitialScan]
   *   + Was the sample automatically or manually
   *     captured? [sampleReason == automatic or manual]
   *   + Has the gateway told us which defs we need, and if
   *     so, what's the sequence number? [sigSeqMinimumNum]
   *   + What sequence number did the client scan with?
   *     [sigSeqClientNum]
   *   + When the client scanned, did it find a virus?
   *     [didClientFindVirus]
   *
   * It looks at these pieces of information, and uses
   * the logic described in John's Magic Table Of Sample States
   * to set some variables, as follows:
   *   + [whatToDoWithTheDamnSample]
   *   + [sampleStatus]
   *   + [analysisState]
   *   + [gatewayStitchedUsUp]
   *
   * The marginal-note-comments in this switch{} relate to
   * the line numbers in John's Magic Table of Sample States.
   * The eager reader is referred there for more information.
   */
  switch (scanRC)
  {
    case NAVScan::BadScan :
    case NAVScan::RepairTriedAndError :
    case NAVScan::RepairTriedAndFailed :
         if (isThisTheInitialScan)
         {
/* 01 */   whatToDoWithTheDamnSample = clearToSubmit;
/* 01 */   codePathTaken             = 1;
         }
         else /* (! isThisTheInitialScan) */
         {
           if (sigSeqMinimumNum)
           {
             if (scanSeqNum >= sigSeqMinimumNum)
             {
/* 02 */       whatToDoWithTheDamnSample = itsAllGonePearShaped;
/* 02 */       gatewayStitchedUsUp       = true;
/* 02 */       codePathTaken             = 2;
             }
             else /* (scanSeqNum < sigSeqMinimumNum) */
             {
/* 03 */       whatToDoWithTheDamnSample = goDownPub;
/* 03 */       codePathTaken             = 3;
             }
           }
           else /* (! SigSeqMinimumNum) */
           {
/* 04 */     whatToDoWithTheDamnSample = goDownPub;
/* 04 */     codePathTaken             = 4;
           }
         }
         break;
    case NAVScan::NotInfectable :
		case NAVScan::NotInfected :
         if (isThisTheInitialScan)
         {
           if (automatic == sampleReason)
           {
             if (scanSeqNum >= sigSeqClientNum)
             {
/* 05 */       whatToDoWithTheDamnSample = finalStateNow;
/* 05 */       sampleStatus              = SampleStatus::available;
/* 05 */       analysisState             = XAnalysisState::Misfired();
/* 05 */       codePathTaken             = 5;
             }
             else
             {
/* 06 */       whatToDoWithTheDamnSample = clearToSubmit;
/* 06 */       codePathTaken             = 6;
             }
           }
           else /* (manual == sampleReason) */
           {
/* 07 */     whatToDoWithTheDamnSample = clearToSubmit;
/* 07 */     codePathTaken             = 7;
           }
         }
         else /* (! isThisTheInitialScan) */
         {
           if (automatic == sampleReason)
           {
/* 08 */     whatToDoWithTheDamnSample = finalStateNow;
/* 08 */     sampleStatus              = SampleStatus::available;
/* 08 */     analysisState             = XAnalysisState::Misfired();
/* 08 */     codePathTaken             = 8;
           }
           else /* (manual == sampleReason) */
           {
             if (sigSeqMinimumNum)
             {
               if (scanSeqNum >= sigSeqMinimumNum)
               {
                 if (didClientFindVirus)
                 {
/* 09 */           whatToDoWithTheDamnSample = finalStateNow;
/* 09 */           sampleStatus              = SampleStatus::available;
/* 09 */           analysisState             = XAnalysisState::Misfired();
/* 09 */           codePathTaken             = 9;
                 }
                 else /* (! didClientFindVirus) */
                 {
/* 10 */           whatToDoWithTheDamnSample = finalStateNow;
/* 10 */           sampleStatus              = SampleStatus::unneeded;
/* 10 */           analysisState             = XAnalysisState::UnInfected();
/* 10 */           codePathTaken             = 10;
                 }
               }
               else /* (scanSeqNum < sigSeqMinimumNum) */
               {
/* 11 */         whatToDoWithTheDamnSample = goDownPub;
/* 11 */         codePathTaken             = 11;
               }
             }
             else /* (! sigSeqMinumNum) */
             {
/* 12 */       whatToDoWithTheDamnSample = goDownPub;
/* 12 */       codePathTaken             = 12;
             }
           }
         }
         break;
    case NAVScan::Repaired :
/* 13 */ whatToDoWithTheDamnSample = finalStateNow;
/* 13 */ sampleStatus              = SampleStatus::available;
/* 13 */ analysisState             = XAnalysisState::Infected();
/* 13 */ codePathTaken             = 13;
         break;
    case NAVScan::UnSubmittable :
         if (isThisTheInitialScan)
         {
           if (automatic == sampleReason)
           {
/* 14 */     whatToDoWithTheDamnSample = finalStateNow;
/* 14 */     sampleStatus              = SampleStatus::available;
/* 14 */     analysisState             = XAnalysisState::UnSubmitable();
/* 14 */     codePathTaken             = 14;
           }
           else /* (manual == sampleReason) */
           {
/* 15 */     whatToDoWithTheDamnSample = clearToSubmit;
/* 15 */     codePathTaken             = 15;
           }
         }
         else /* (! isThisTheInitialScan) */
         {
           if (automatic == sampleReason)
           {
/* 16 */     whatToDoWithTheDamnSample = finalStateNow;
/* 16 */     sampleStatus              = SampleStatus::available;
/* 16 */     analysisState             = XAnalysisState::UnSubmitable();
/* 16 */     codePathTaken             = 16;
           }
           else /* (manual == sampleReason) */
           {
             if (sigSeqMinimumNum)
             {
               if (scanSeqNum >= sigSeqMinimumNum)
               {
/* 17 */         whatToDoWithTheDamnSample = finalStateNow;
/* 17 */         sampleStatus              = SampleStatus::available;
/* 17 */         analysisState             = XAnalysisState::UnSubmitable();
/* 17 */         codePathTaken             = 17;
               }
               else /* (scanSeqNum < sigSeqMinimumNum) */
               {
/* 18 */         whatToDoWithTheDamnSample = goDownPub;
/* 18 */         codePathTaken             = 18;
               }
             }
             else /* (! sigSeqMinumNum) */
             {
/* 19 */       whatToDoWithTheDamnSample = goDownPub;
/* 19 */       codePathTaken             = 19;
             }
           }
         }
         break;
    case NAVScan::Heuristic :
    case NAVScan::NoRepairInstructions :
         if (isThisTheInitialScan)
         {
/* 20 */   whatToDoWithTheDamnSample = clearToSubmit;
/* 20 */   codePathTaken             = 20;
         }
         else /* (! isThisTheInitialScan) */
         {
           if (sigSeqMinimumNum)
           {
             if (scanSeqNum >= sigSeqMinimumNum)
             {
/* 21 */       whatToDoWithTheDamnSample = finalStateNow;
/* 21 */       sampleStatus              = SampleStatus::available;
/* 21 */       analysisState             = ((NAVScan::Heuristic == scanRC) ? XAnalysisState::Infected() : XAnalysisState::NoRepair());
/* 21 */       codePathTaken             = 21;
             }
             else /* (scanSeqNum < sigSeqMinimumNum) */
             {
/* 22 */       whatToDoWithTheDamnSample = goDownPub;
/* 22 */       codePathTaken             = 22;
             }
           }
           else /* (! sigSeqMinumNum) */
           {
/* 23 */     whatToDoWithTheDamnSample = goDownPub;
/* 23 */     codePathTaken             = 23;
           }
         }
         break;
    case NAVScan::NavCrashed :
    case NAVScan::NavVcdInitError :
    case NAVScan::NavDefinitionFileError :
    case NAVScan::NavLoadDefFileError :
    case NAVScan::SigDefMissing :
    case NAVScan::ScannerServiceError :
    case NAVScan::WaitForScanTimedOut :
    case NAVScan::CommWithScannerOffline :
    case NAVScan::BadResponceFromScanner :
    case NAVScan::UnknownError :
    default :
/* 24 */ whatToDoWithTheDamnSample = itsAllGonePearShaped;
/* 24 */ codePathTaken             = 24;
         break;
  }

  /*
   * Drop a log entry for ease of autopsy.
   */
  if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSample))
  {
    char msgStr[128];
    sprintf(msgStr, "Sample(%lx) -- Took path %02d through post-scan monster switch.", DebugKey(), codePathTaken);
    Logger::Log(Logger::LogDebug, Logger::LogIcePackSample, msgStr);
  }

  /*
   * Now, we have all the state we require in a few locals:
   *
   *  + [whatToDoWithTheDamnSample]
   *      This contains the 'status' of the sample at a
   *      very coarse level.  It can have the values:
   *        + invalidWhatToDoValue
   *        + goDownPub
   *        + finalStateNow
   *        + clearToSubmit
   *        + itsAllGonePearShaped
   *  + [analysisState]
   *      If [whatToDoWithTheDamnSample] is set to 'finalStateNow',
   *      then this variable defines the final state into which
   *      the sample should be put.  It can have the values:
   *        + XAnalysisState::UnknownSampleState()
   *        + XAnalysisState::UnInfected()
   *        + XAnalysisState::Infected()
   *        + XAnalysisState::Unsubmitable()
   *        + XAnalysisState::NoRepair()
   *        + XAnalysisState::Misfired()
   *  + [sampleStatus]
   *      If [whatToDoWithTheDamnSample] is set to 'finalStateNow',
   *      then this variable tells us whether or not we should
   *      distribute the current definition set (the one with which
   *      we just scanned the sample).  It can have the values:
   *        + SampleStatus::available
   *        + SampleStatus::unneeded
   *        + SampleStatus::unknown
   *      It's initialised to 'SampleStatus::unknown'.  If it's
   *      set to 'SampleStatus::available', then we're going to distribute.
   *      If it's set to 'SampleStatus::unneeded', then we're not.
   *  + [gatewayStitchedUsUp]
   *      If [whatToDoWithTheDamnSample] is set to 'itsAllGonePearShaped',
   *      and [gatewayStitchedUsUp] is set to 'true', then we got the defs
   *      which the gateway said we needed, and they didn't work.  Sulk
   *      massively.
   */

  /*
   * Before we get too bogged down, set the X-Analysis-State attribute.
   * Note that we only do this if it's valid.
   */
  if (analysisState != XAnalysisState::UnknownSampleState())
    SetAttribute(AttributeKeys::AnalysisState(), analysisState);

  /*
   * Get a stringified version of the NAV return code.
   */
  navErrStr = NAVScan::RCtoChar(scanRC);

  /*
   * Yes, folks, it's ANOTHER switch.
   */
  switch (whatToDoWithTheDamnSample)
  {
    case finalStateNow :        /* We're finished with this sample. */
      /*
       * Set the X-Date-Analyzed attribute, because we're done.
       */
      SetAttribute(AttributeKeys::DateAnalyzed(), httpNow);

      /*
       * Set the status, which is available in [sampleStatus].
       */
      Status(sampleStatus);

      /*
       * If we're going to distribute, then store the sequence number
       * of the definitions we scanned the sample with in its header.
       * This determines the minimum definition package to distribute
       * to the client that submitted the sample.  (EJP 6/16/00)
       */
      if (SampleStatus::available == sampleStatus)
        SetAttribute(AttributeKeys::SignatureSequence(), newestSigAsChar);

      /*
       * This incantation appears to ensure the signature gets downloaded.
       */
      sigSeqNum = scanSeqNum;

      break;
    case clearToSubmit :        /* Sample can be sent to the gateway. */
      if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSample))
      {
        char msgStr[128];
        sprintf(msgStr, "Sample %lx -- Cleared for submission", DebugKey());
        Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, msgStr);
      }
      break;
    case itsAllGonePearShaped : /* There was an error. */
      if (gatewayStitchedUsUp)  /* We should have been okay. */
      {
        string msg;

        msg = "Definitions returned were inadequate [ " + navErrStr + " ]";

        SetAttribute(AttributeKeys::Attention(), msg.c_str());
        Status(SampleStatus::attention);

        if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSample))
        {
          char msgStr[128];
          sprintf(msgStr, "Sample %lx -- Should have been handled by sequence %08d but was not", DebugKey(), scanSeqNum);
          Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, msgStr);
        }
      }
      else /* (! gatewayStitchedUsUp) */
      {
        string msg;

        msg = "NAV encountered an error scanning the sample [ " + navErrStr + " ]";

        SetAttribute(AttributeKeys::Attention(), msg.c_str());
        Status(SampleStatus::attention);

        if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSample))
        {
          char msgStr[128];
          sprintf(msgStr, "Sample %lx -- NAV engine utterly failed in the scan [%s]", DebugKey(), navErrStr);
          Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, msgStr);
        }
      }
      break;
    case goDownPub :            /* Leave sample alone for now. */
      if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSample))
      {
        char msgStr[128];
        sprintf(msgStr, "Sample %lx -- Not doing anything at the moment", DebugKey(), sampleFileIDNum);
        Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, msgStr);
      }
      break;
    case invalidWhatToDoValue : /* Something really weird went down. */
    default :
      if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackSample))
      {
        char msgStr[128];
        sprintf(msgStr, "Sample %lx -- Generated an error in the monster switch", DebugKey(), sampleFileIDNum);
        Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, msgStr);
      }
      break;
  }

  /*
   * I don't know why this is called boCommError if it indicates
   * an error with the scanner, but here it is nonetheless.
   */
  if (boCommError)
    Attention::Set(Attention::ScanExplicit);
  else
    Attention::UnSet(Attention::ScanExplicit);

  /*
   * Did the scan go off without an error?
   */
  if (scanOk)
  {
    /*
     * It did.  Stamp into the sample information about the def set with
     * which we just scanned.
     */
    SetAttribute(AttributeKeys::ScanSignaturesSequence(), newestSigAsChar);
    SetAttribute(AttributeKeys::ScanSignaturesVersion(),  navVersion.c_str());

    /*
     * Make appropriate noises in the general direction of the log file.
     */
    if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackSample))
    {
      char  msgStr[128];

      sprintf(msgStr, "Sample %lx -- Scan succeeded with return code of %s",
                      DebugKey(),
                      NAVScan::RCtoChar(scanRC));
      Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, msgStr);
    }
  }
  else /* (! scanOk) */
  {
    /*
     * No real work to do here -- just the logging.
     */
    if (Logger::WillLog(Logger::LogWarning, Logger::LogIcePackSample))
    {
      char  msgStr[128];

      sprintf(msgStr, "Sample %lx -- Scan failed with return code of %s",
                      DebugKey(),
                      NAVScan::RCtoChar(scanRC));
      Logger::Log(Logger::LogWarning, Logger::LogIcePackSample, msgStr);
    }
  }

  /*
   * Finally, stamp the changes back into the sample.
   */
  Commit();

  /*
   * JD.
   */
  return scanRC;
}

/* ----- */

bool Sample::SetAttribute(const char* key, const char* value)
{
  EntryExit entryExit(Logger::LogIcePackSample, "SetAttribute");
  
  char  newVal[10];
  bool  rc  = false;

  if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackSample))
  {
    char msg[512];

    sprintf(msg, "Setting sample(%x) attribute %s to %s", DebugKey(), key, value);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, msg);
  }

  /*
   * We need a special hack here (I KNOW, IT MAKES ME ILL TOO).
   * In some instances, we have observed signature sequence numbers
   * coming back from the gateway (in status responses) that are not
   * the correct length (8 characters).  We're not sure what this will
   * do to the rest of the system, but just in case I'm padding them out
   * here.
   */
  newVal[0] = 0;
  if (AttributeKeys::SignatureSequence() == key)
  {
    if (8 != strlen(value))
    {
      /*
       * Ensure there are lots of NULLs in the new value.
       */
      memset(newVal+1, '\0', sizeof(newVal) - 1);

      /*
       * Pad the first bit of the new value with zeros.
       */
      memset(newVal, '0', (8 - strlen(value)));

      /*
       * Append the value to the zero-padding.
       */
      strcat(newVal, value);
    }
  }

  if (SampleBase::SetAttribute(key, (newVal[0] ? newVal : value)))
  {
    if (AttributeKeys::SamplePriority() == key)
      priority  = atoi(value);
    else if (AttributeKeys::ScanSignaturesSequence() == key)
      sigSeqNum = (uint) atoi(value);
    else if (AttributeKeys::SignatureSequence() == key)
      neededSeqNum= (uint) atoi(value);
    else if (AttributeKeys::SignaturePriority() == key)
      sigPriority = (uint) atoi(value);

    rc = true;
  }
  return rc;
}
