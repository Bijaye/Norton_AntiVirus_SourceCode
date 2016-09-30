// GlobalData.cpp: implementation of the GlobalData class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <algorithm>

#include "GlobalData.h"
#include <AttributeKeys.h>
#include <WinRegistry.h>
#include <Logger.h>
#include <EventLog.h>
#include <TraceSinkFile.h>

/* ----- */

using namespace std;

/* ----- */

CMclCritSec  GlobalData::updatingQuarantine;
CMclCritSec  GlobalData::critSamplesList;
list<Sample> GlobalData::samplesList;
CMclCritSec  GlobalData::regValuesCritSec;
CMclEvent    GlobalData::go(TRUE, TRUE);                    // event to signal go again after pause
CMclEvent    GlobalData::halt(FALSE, FALSE);                // event to shutdown threads that wait on events
CMclEvent    GlobalData::downloadSig(FALSE, FALSE);         // signal need to download sig file
CMclEvent    GlobalData::submitSample(FALSE, FALSE);        // signal need to upload sample
CMclEvent    GlobalData::deliverSignature(FALSE, FALSE);    // signal need to send signature to workstation(s)
CMclEvent    GlobalData::newBlessed(FALSE, FALSE);          // signal that new blessed signatures are now available.
ulong        GlobalData::quarantineChangeCounter = 0;
bool         GlobalData::stop                    = false;  // true if service is stopped(or wants to stop)
bool         GlobalData::pause                   = false;  // true if service is paused(or wants to pause)

/*
 * Web Communications objects.
 */
string GlobalData::gatewayAddress("");
string GlobalData::firewallName("");
string GlobalData::firewallUser("");
string GlobalData::firewallPassword("");
string GlobalData::gatewayURL("");
string GlobalData::gatewaySSLURL("");
string GlobalData::firewallURL("");
uint   GlobalData::firewallPort            = 0;
uint   GlobalData::gatewayPort             = 0;
uint   GlobalData::gatewayPortSSL          = 0;
uint   GlobalData::quarantineInterval      = 900000;
uint   GlobalData::newBlessedInterval      = 1440*60*1000;
uint   GlobalData::neededInterval          = 300000;
uint   GlobalData::timeoutInterval         = 900000;
uint   GlobalData::retryInterval           = 900000;
uint   GlobalData::retryLimit              = 2;
bool   GlobalData::secureSampleSubmission  = false;
bool   GlobalData::secureStatusQuery       = false;
bool   GlobalData::secureSigDownload       = false;
bool   GlobalData::secureIgnoreHostname    = false;
bool   GlobalData::isFirewall              = false;

/*
 * Customer Information.
 */
string GlobalData::companyName("");
string GlobalData::companyAddress("");
string GlobalData::contactName("");
string GlobalData::contactTelephone("");
string GlobalData::contactEmail("");
string GlobalData::customerAccount("");

/*
 * Sample Policy.
 */
uint   GlobalData::initialSubmissionPriority = 0;
uint   GlobalData::maxPendingSamples         = 2;
bool   GlobalData::stripUserData             = false;
bool   GlobalData::compressContent           = true;
bool   GlobalData::scrambleContent           = true;
uint   GlobalData::statusCheckInterval       = 300000;  // milliseconds
uint   GlobalData::registryCheckInterval     = 300000;  // milliseconds
uint   GlobalData::topologyCheckInterval     = 300000;  // milliseconds
uint   GlobalData::defUnpackTimeout          = 300000;  // milliseconds
bool   GlobalData::defPrune                  = true;
bool   GlobalData::defUnblessedBroadcast     = false;
bool   GlobalData::defUnblessedNarrowcast    = true;
bool   GlobalData::defUnblessedPointcast     = true;
string GlobalData::defUnblessedTargets       = "";
bool   GlobalData::defBlessedBroadcast       = 0;
string GlobalData::defBlessedTargets         = "";
uint   GlobalData::defDeliveryInterval       = 900000;  // milliseconds
uint   GlobalData::defDeliveryPriority       = 500;
uint   GlobalData::defDeliveryTimeout        = 1800000; // milliseconds
string GlobalData::defLibraryDirectory       = "\\signatures";

/*
 * Trace-related things.
 */
bool   GlobalData::traceEnable     = false;
string GlobalData::traceFileName   = "";

/*
 * Default temporary directory.
 */
string GlobalData::tempDirectory   = "c:\\temp";

uint GlobalData::newestSig            = 0;
bool GlobalData::newestSigBlessed     = false;
uint GlobalData::blessedToDownload    = 0;
uint GlobalData::specdefToDownload    = 0;

uint GlobalData::newestBlessedSig  = 0;  // most recent sig that was blessed

const char *GlobalData::latestInstalledSigKey            = "definitionActiveSequence";
const char *GlobalData::latestInstalledSigIsBlessedKey   = "definitionActiveBlessed";
const char *GlobalData::definitionDownloadNextKey        = "definitionDownloadNext";
const char *GlobalData::definitionBlessedSequenceKey     = "definitionBlessedSequence";
const char *GlobalData::definitionDownloadNextBlessedKey = "definitionDownloadNextBlessed";

const char *GlobalData::regSymantecRoot = "Software\\Symantec\\Quarantine\\Server";
const char *GlobalData::regUpdateRoot   = "Software\\Symantec\\Quarantine\\Server\\Avis";
const char *GlobalData::regCurrentRoot  = "Software\\Symantec\\Quarantine\\Server\\Avis\\current";

bool    GlobalData::initializing  = true;

set<uint> GlobalData::badSeqNums;

/* ----- */

void GlobalData::Stop()
{
  Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, "GlobalData::Stop() called");
  stop = true;
  halt.Set();
}

/* ----- */

/*
 * This method takes two parameters -- the first is a sequence number,
 * and the second indicates whether or not that sequence number is
 * to be regarded as 'blessed'.
 */

bool GlobalData::SetNewestSigAvailable(uint sig, bool blessed)
{
  EntryExit     entryExit(Logger::LogIcePackOther, "GlobalData::SetNewestSigAvailable");
  WinRegistry   cRoot(HKEY_LOCAL_MACHINE, regCurrentRoot);
  char          asChar[20];

  char buffer[250];
  sprintf(buffer, "SetNewestSigAvailable(%d,%d) called with newestSig=%d blessedToDownload=%d specdefToDownload=%d",sig,blessed,newestSig,blessedToDownload,specdefToDownload);
  Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, buffer);

  sprintf(asChar, "%d", sig);
  if (blessed)
  {
    cRoot.SetValue(definitionBlessedSequenceKey, (uchar*) asChar);
    if (sig >= blessedToDownload)
      {
	cRoot.SetValue(definitionDownloadNextBlessedKey, (const uchar *) "None");
	Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, "download definition registry key set");
      }
    
    newestBlessedSig = sig;

    // If this new sequence number is equal to or greater than the
    // previous new sequence number, save it in a registry variable
    // and indicate that these new definitions are blessed.  (EJP 7/26/00)

    if (sig >= newestSig)
    {
      newestSig         = sig;
      newestSigBlessed  = true;
      cRoot.SetValue(latestInstalledSigKey, (uchar *)asChar);
      cRoot.SetValue(latestInstalledSigIsBlessedKey, (uchar *)("1"));
      Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, "blessed definition registry keys set");
    }
  }
  else /* (! blessed) */
  {
    if (sig >= specdefToDownload)
      cRoot.SetValue(definitionDownloadNextKey, (const uchar *) "None");
    newestSig         = sig;
    newestSigBlessed  = false;
    cRoot.SetValue(latestInstalledSigKey, (uchar *)asChar);
    cRoot.SetValue(latestInstalledSigIsBlessedKey, (uchar *)("0"));
      Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, "unblessed definition registry keys set");
  }
  return true;
}

/* ----- */

/*
 * This method returns to the caller a sequence number (as the return
 * code) and a parameterised 'true' or 'false' indicating whether or
 * not the sequence number refers to a blessed definition.
 *
 * If there are no signatures awaiting download, it returns 0.  The
 * value of the parameter should be ignored, but it is set to 'false'
 * nonetheless.
 */
uint GlobalData::GetSigToDownload(bool &blessed)
{
  uint retMe;

  /*
   * Give priority to blessed defsets.
   */
  if ((blessedToDownload) && (blessedToDownload > newestBlessedSig))
  {
    blessed = true;
    retMe   = blessedToDownload;
  }
  else if ((specdefToDownload) && (specdefToDownload > newestSig))
  {
    blessed = false;
    retMe   = specdefToDownload;
  }
  else /* Neither of the above. */
  {
    blessed = false;
    retMe   = 0;
  }

  /*
   * JD.
   */
  return retMe;
}

/* ----- */

/*
 * This method takes two parameters -- the first is a sequence number,
 * and the second indicates whether or not that sequence number is
 * to be regarded as 'blessed'.
 *
 * The method returns 'true', if the new sequence number (1st parameter)
 * in the blessed or unblessed category (2nd parameter) was greater
 * than the previously stored one,  or 'false' if it didn't.  That is
 * to say, 'true' if the status of the GlobalData has been in any
 * way affected by the gyrations herein.
 */
bool GlobalData::SetSigToDownload(uint sig, bool blessed)
{
  bool retVal        = false;
  uint reallyDoItNum = 0;
  bool reallyDoItBle;

  EntryExit entryExit(Logger::LogIcePackOther, "GlobalData::SetSigToDownload");

  if (blessed)
  {
    if (sig > blessedToDownload)
    {
      if (! GlobalData::IsOkaySeqNum(sig))
        return false;

      reallyDoItNum = sig;
      reallyDoItBle = true;
    }
  }
  else /* (! blessed) */
  {
    if (sig > specdefToDownload)
    {
      if (!GlobalData::IsOkaySeqNum(sig))
        while (!GlobalData::IsOkaySeqNum(++sig))
          ;

      reallyDoItNum = sig;
      reallyDoItBle = false;
    }
  }

  if (reallyDoItNum)
  {
    char        regStr[10];
    WinRegistry cRoot(HKEY_LOCAL_MACHINE, regCurrentRoot);

    sprintf(regStr, "%d", reallyDoItNum);

    if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackOther))
    {
      char msgStr[128];

      sprintf(msgStr, "Setting new %s to download to %d", (reallyDoItBle ? "blessed def" : "specdef"), reallyDoItNum);
      Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, msgStr);
    }

    if (reallyDoItBle)
    {
      blessedToDownload    = reallyDoItNum;
      cRoot.SetValue(definitionDownloadNextBlessedKey, (const uchar *)regStr);
    }
    else /* (! reallyDoItBle) */
    {
      specdefToDownload    = reallyDoItNum;
      cRoot.SetValue(definitionDownloadNextKey, (const uchar *)regStr);
    }

    downloadSig.Set();

    retVal = true;
  }

  return retVal;
}

/* ----- */

void GlobalData::RebuildGatewayURL()
{
  string msg("Gateway URL set to ");

  if (gatewayAddress == "")
    gatewayURL  = "";
  else
  {
    gatewayURL  = "http://";
    gatewayURL += gatewayAddress;

    if (0 != gatewayPort)
    {
      char buffer[32];

      sprintf(buffer, ":%04d", gatewayPort);
      gatewayURL += buffer;
    }
  }

  msg += gatewayURL;
  Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, msg.c_str());
}

/* ----- */

void GlobalData::RebuildSSLGatewayURL()
{
  string msg("Gateway SSL URL set to ");

  if (gatewayAddress == "")
    gatewaySSLURL  = "";
  else
  {
    gatewaySSLURL  = "https://";
    gatewaySSLURL += gatewayAddress;
    if (0 != gatewayPortSSL)
    {
      char buffer[32];

      sprintf(buffer, ":%04d", gatewayPortSSL);
      gatewaySSLURL += buffer;
    }
  }

  msg += gatewaySSLURL;
  Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, msg.c_str());
}

/* ----- */

void GlobalData::RebuildFirewallURL()
{
  string  msg("Firewall URL set to ");

  /*
   * Used to read:
   *  if (firewallName == "")
   * Change is BoundsChecker-induced.
   * inw 2000-06-17
   */
  if (! firewallName.size())
  {
    firewallURL = "";
    isFirewall  = false;
  }
  else
  {
    firewallURL  = "http://";
    firewallURL += firewallName;

    if (0 != firewallPort)
    {
      char buffer[32];

      sprintf(buffer, ":%04d", firewallPort);
      firewallURL += buffer;
      isFirewall = true;
    }
  }

  msg += firewallURL;
  Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, msg.c_str());
}

/* ----- */

void GlobalData::SetCommParameters()
{
  using namespace IcePackAgentComm;

  if (!initializing)
  {
    char    asChar[20];
    uint    timeout = (GlobalData::TimeoutInterval() / 1000);
    string  msg("Communications parameters set to:  firewallURL = ");

    SetUnpackParams((GlobalData::DefUnpackTimeout() / 1000), GlobalData::TempDirectory());

    SetCommParams(GlobalData::FirewallURL(),
                  GlobalData::FirewallUser(),
                  GlobalData::FirewallPassword(),
                  timeout,
                  GlobalData::RetryLimit(),
                  timeout,
                  timeout,
                  timeout,
                  timeout,
                  halt,
                  secureIgnoreHostname);

    msg += GlobalData::FirewallURL();
    msg += " , firewallUser = ";
    msg += GlobalData::FirewallUser();
    msg += " , firewallPassword = xxxxx (nice try), retryInterval = ";
    sprintf(asChar, "%d seconds", GlobalData::RetryInterval()/1000);
    msg += asChar;
    msg += " , All Timeout intervals set to ";
    sprintf(asChar, "%d seconds", timeout);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, msg.c_str());

    msg = "Unpack parameters set to: unpack timeout = ";
    sprintf(asChar, "%d seconds", GlobalData::DefUnpackTimeout()/1000);
    msg += asChar;
    msg += " , temporary directory = ";
    msg += GlobalData::TempDirectory();

    Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, msg.c_str());
  }
}

/* ----- */

void GlobalData::ContactEmail(TCHAR* cEmail)
{
  list<Sample>::iterator i;
  EntryExit              entryExit(Logger::LogIcePackOther, "GlobalData::ContactEmail");
  CMclAutoLock           lock(regValuesCritSec);

  /*
   * Used to read:
   *  if (contactEmail == "" && '\0' != cEmail[0])
   * Change is BoundsChecker-induced.
   * inw 2000-06-17
   */
  if ((! contactEmail.size()) && ('\0' != cEmail[0]))
  { 
    contactEmail  = cEmail;

    /*
     * Now check to see if any samples are in attention state, and have no
     * email address.  If one is found, stamp the new email address in,
     * and send it back to quarantine state.
     */
    string      tmp;
    CMclAutoLock lock(GlobalData::critSamplesList);
  
    for (i = GlobalData::samplesList.begin(); i != GlobalData::samplesList.end(); i++)
    {
      if ((!i->IsNull()) && (SampleStatus::attention == i->Status()))
      {
        i->GetAttribute(AttributeKeys::CustomerContactEmail(), tmp);
        if (! tmp.size())
        {
          i->SetAttribute(AttributeKeys::CustomerContactEmail(), cEmail);
          i->SetAttribute(AttributeKeys::Attention(), "");
          i->Status(SampleStatus::quarantined);
          i->Commit();

          Logger::Log(Logger::LogInfo, Logger::LogIcePackOther,
                "Changing sample state from attention to quarantined");
        }
      }
    }
  }
  else
    contactEmail  = cEmail;
}

/* ----- */

static TraceSinkFile *traceSink = NULL;
static MTrace        *mTrace    = NULL;

/* ----- */

void GlobalData::TraceEnable(bool te)
{
  if (te != traceEnable)
  {
    CMclAutoLock  lock(regValuesCritSec);
    traceEnable = te;
    if (traceEnable)
    {
      if (traceFileName.size() > 0)
      {
        if (NULL != traceSink)
        {
          Logger::SendTo(NULL);
          delete mTrace;
          delete traceSink;
        }
        traceSink = new TraceSinkFile(traceFileName.c_str());
        mTrace    = new MTrace(*traceSink, MODE_MAX, MTrace::tv_exhaustive, ",");
        Logger::SendTo(mTrace);
      }
    }
    else if (NULL != traceSink)
    {
      Logger::SendTo(NULL);

      delete mTrace;
      delete traceSink;
      mTrace    = NULL;
      traceSink = NULL;
    }
  }
}

/* ----- */

void GlobalData::TraceFileName(TCHAR* tfn)
{
  if (stricmp(tfn, traceFileName.c_str()))
  {
    CMclAutoLock  lock(regValuesCritSec);
    traceFileName = tfn;
  }
}

/* ----- */

bool GlobalData::SleepInSegments(uint (*IntervalFunc)(),
                                 const Logger::LogSource  source,
                                 const char              *whyTheNap)
{
  uint sleep            = 0;
  uint totalSlept       = 0;
  char msg[300];
  
  sprintf(msg, "About to SleepInSegments() for %d.%03ds", (IntervalFunc() / 1000), (IntervalFunc() % 1000));
  Logger::Log(Logger::LogDebug, source, msg);

  while (!GlobalData::stop && totalSlept < IntervalFunc())
  {
    Sleep(maxSleepInterval);
    totalSlept  += maxSleepInterval;
  }

  sprintf(msg, "Slept for %d.%03ds", (totalSlept / 1000), (totalSlept % 1000));

  if (GlobalData::stop)
    strcat(msg, " [global stop message received]");

  Logger::Log(Logger::LogDebug, source, msg);

  return ((totalSlept >= IntervalFunc()) && (!GlobalData::stop));
}

/* ----- */

/*
 * GlobalData::GetContactHeaders
 *
 * Returns a string containing the customer contact attributes
 * formatted ready for use in a HTTP request.
 *
 * inw 2000-02-08 put this here, for want of a better place.
 */
std::string GlobalData::GetContactHeaders()
{
  string tmpString;
  string headers;

  /*
   * Make sure this is empty to start with.
   */
  headers = "";

  tmpString = GlobalData::ContactEmail();
  if (0 != tmpString.length())
  {
    headers += AttributeKeys::CustomerContactEmail();
    headers += ": ";
    headers += tmpString;
    headers += "\n";
  }

  tmpString = GlobalData::ContactName();
  if (0 != tmpString.length())
  {
    headers += AttributeKeys::CustomerContactName();
    headers += ": ";
    headers += tmpString;
    headers += "\n";
  }

  tmpString = GlobalData::CompanyName();
  if (0 != tmpString.length())
  {
    headers += AttributeKeys::CustomerName();
    headers += ": ";
    headers += tmpString;
    headers += "\n";
  }

  tmpString = GlobalData::ContactTelephone();
  if (0 != tmpString.length())
  {
    headers += AttributeKeys::CustomerContactTelephone();
    headers += ": ";
    headers += tmpString;
    headers += "\n";
  }

  tmpString = GlobalData::CustomerAccount();
  if (0 != tmpString.length())
  {
    headers += AttributeKeys::CustomerID();
    headers += ": ";
    headers += tmpString;
    headers += "\n";
  }

  return headers;
}

/* ----- */

void GlobalData::UpdateMasterSampleList(Sample& sample)
{
  list<Sample>::iterator  i;
  CMclAutoLock      lock(critSamplesList);

  if (!sample.IsNull())
  {
    if (Sample::sChanged == sample.Mark())
    {
      i = find_if(samplesList.begin(), samplesList.end(),
            CompSampleByKeyOnly(sample.SampleKey()));
      if (i != samplesList.end())
        *i = sample;
    }
    else if (Sample::sAdd  == sample.Mark())
    {
      samplesList.push_back(sample);
    }
    else if (Sample::sDelete == sample.Mark())
    {
      samplesList.remove(sample);
    }
  }
}

/* ---- */

void GlobalData::HandleException(const char* where, const Logger::LogSource source)
{
  bool  finishUp  = true;
  string  errMsg("Exception of type ");

  try
  {
    throw;
  }
  catch (IcePackException& e)
  {
    errMsg  += "IcePackException caught in ";
    errMsg  += where;
    errMsg  += " [";
    errMsg  += e.FullString();
    errMsg  +="]";
    Logger::Log(Logger::LogError, source, errMsg.c_str());

    finishUp  = false;
  }
  catch (AVISException& e)
  {
    errMsg  += "AVISException caught in ";
    errMsg  += where;
    errMsg  += " [";
    errMsg  += e.FullString();
    errMsg  +="]";
  }
  catch (exception& e)
  {
    errMsg  += "std:: exception caught in  ";
    errMsg  += where;
    errMsg  += " [";
    errMsg  += e.what();
    errMsg  += "]";
  }
  catch (const _com_error& e)
  {
    errMsg  += "_com_error exception caught in ";
    errMsg  += where;
    errMsg  += " [";
    errMsg  += e.ErrorMessage();
    errMsg  += "]";
  }
  catch (const string& e)
  {
    errMsg  += "string exception caught in ";
    errMsg  += where;
    errMsg  += " [";
    errMsg  += e;
    errMsg  += "]";
  }
  catch (const int e)
  {
    char  buffer[24];
    sprintf(buffer, "%d", e);
    errMsg  += "string exception caught in ";
    errMsg  += where;
    errMsg  += " [";
    errMsg  += buffer;
    errMsg  += "]";
  }
  catch (...)
  {
    errMsg  += "unknown caught in ";
    errMsg  += where;
    Logger::Log(Logger::LogCriticalError, source, errMsg.c_str());
    EventLog::UnknownExceptionStop(where);
    GlobalData::Stop();
    finishUp = false;
  }

  if (finishUp)
  {
    Logger::Log(Logger::LogCriticalError, source, errMsg.c_str());
    EventLog::ExceptionStop(where, errMsg.c_str());
    GlobalData::Stop();
  }
}

/* ----- */

bool GlobalData::ExceptionTyper(string& type, string& details)
{
  bool  rc = true;

  try
  {
    throw;
  }
  catch (IcePackException& e)
  {
    type  = "IcePackException";
    details = e.FullString();
  }
  catch (AVISException& e)
  {
    type  = "AVISException";
    details = e.FullString();
  }
  catch (exception& e)
  {
    type  = "std::exception";
    details = e.what();
  }
  catch (_com_error& e)
  {
    type  = "_com_error";
    details = e.ErrorMessage();
  }
  catch (string& e)
  {
    type  = "std::string";
    details = e;
  }
  catch (char* e)
  {
    type  = "char *";
    details = e;
  }
  catch (int e)
  {
    char  buffer[24];
    sprintf(buffer, "%d", e);
    type  = "int";
    details = buffer;
  }
  catch (float e)
  {
    char  buffer[24];
    sprintf(buffer, "%f", e);
    type  = "float";
    details = buffer;
  }
  catch (double e)
  {
    char  buffer[24];
    sprintf(buffer, "%fl", e);
    type  = "double";
    details = buffer;
  }
  catch (...)
  {
    type  = "Unknown";
    details = "";
    rc = false;
  }

  return rc;
}
/* ----- End Transmission ----- */
