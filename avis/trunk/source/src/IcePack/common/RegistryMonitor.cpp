// RegistryMonitor.cpp: implementation of the RegistryMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <string>

#include <EventLog.h>
#include <Logger.h>
#include <bldinfo.h>
#include <SystemException.h>

#include "RegistryMonitor.h"
#include "GlobalData.h"
//#include "IcePackMonitor.h"
//#include "InformationServer.h"
#include "ScannerSubmittor.h"
#include "Attention.h"
#include "RegistryDefaults.h"

using namespace std;

//
//  RegistryMonitor
//
//  Overview:
//    This thread checks for changes to the IcePack configuration values stored in the
//    registry.
//
//  Algorithm:
//    (Simplified version, real code is slightly different)
//    while service not shutting down & sleep configurationChangeInterval*60000 milliseconds
//      if the configuration change counter has changed since the last time we looked
//        check every value in the registry to see if it changed.
//
//  Globals static methods/members Used
//    This thread touches almost all of the values.
//

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const char*  internetSettingsKey   = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
static const char*  httpMaxConnectionsKey = "MaxConnectionsPerServer";
static const char*  httpMax1_0ConnectionsKey= "MaxConnectionsPer1_0Server";

/*
 * Use these enum-ed values to index into the array which
 * follows.
 */
enum RKey
{
  rWebGatewayName,
  rWebGatewayPort,
  rWebGatewayPortSSL,
  rWebFirewallName,
  rWebFirewallPort,
  rWebFirewallUser,
  rWebFirewallPassword,
  rWebSecureSampleSubmission,
  rWebSecureStatusQuery,
  rWebSecureSignatureDownload,
  rWebSecureIgnoreHostname,
  rWebStatusInterval,
  rWebBlessedInterval,
  rWebNeededInterval,
  rWebTimeoutInterval,
  rWebRetryInterval,
  rWebRetryLimit,

  rCustomerName,
  rCustomerContactName,
  rCustomerContactTelephone,
  rCustomerContactEmail,
  rCustomerID,

  rSampleQuarantineInterval,
  rSampleSubmissionPriority,
  rSampleMaximumPending,
  rSampleStripContent,
  rSampleCompressContent,
  rSampleScrambleContent,

  rDefinitionBlessedBroadcast,
  rDefinitionBlessedTargets,
  rDefinitionDeliveryInterval,
  rDefinitionDeliveryPriority,
  rDefinitionDeliveryTimeout,
#if 0 /* inw 2000-02-08 -- ejp says this is deprecated. */
  rDefinitionCheckInterval,
#endif /* 0 */
  rDefinitionLibraryDirectory,
  rDefinitionPrune,
  rDefinitionUnblessedBroadcast,
  rDefinitionUnblessedNarrowcast,
  rDefinitionUnblessedPointcast,
  rDefinitionUnblessedTargets,
  rDefinitionUnpackTimeout,
  rDefinitionHeuristicLevel,

  rConfigurationChangeCounter,
  rConfigurationChangeInterval,

  rTraceEnable,
  rTraceFileName,
  rTraceSeverity,
  rTraceFilter,

  rTemporaryDirectory,

  rBillification,
  /*
   * Be aware that if you add more values to the end of this list,
   * you will need to do at least two things -- first, change
   * the declaration size of regTable; second, change the
   * loop in RegistryMonitor::InitializeRegistry() so that it
   * ends at the correct place.
   *
   * No, I wouldn't have written this code this way either.
   * Still, there it is.
   * [inw 2000-04-21]
   */
};

/*
 * For the default/minimum/maximum values, see RegistryDefaults.h.
 */
static RegEntries regTable[rBillification + 1] =
{
  RegEntries("webGatewayName",                ICEP_DEF_WEBGATEWAYNAME),
  RegEntries("webGatewayPort",                ICEP_DEF_WEBGATEWAYPORT,
                                              ICEP_MIN_WEBGATEWAYPORT,
                                              ICEP_MAX_WEBGATEWAYPORT),
  RegEntries("webGatewayPortSSL",             ICEP_DEF_WEBGATEWAYPORTSSL,
                                              ICEP_MIN_WEBGATEWAYPORTSSL,
                                              ICEP_MAX_WEBGATEWAYPORTSSL),
  RegEntries("webFirewallName",               ICEP_DEF_WEBFIREWALLNAME),
  RegEntries("webFirewallPort",               ICEP_DEF_WEBFIREWALLPORT,
                                              ICEP_MIN_WEBFIREWALLPORT,
                                              ICEP_MAX_WEBFIREWALLPORT),
  RegEntries("webFirewallUser",               ICEP_DEF_WEBFIREWALLUSER),
  RegEntries("webFirewallPassword",           ICEP_DEF_WEBFIREWALLPASSWORD),
  RegEntries("webSecureSampleSubmission",     ICEP_DEF_WEBSECURESAMPLESUBMISSION,
                                              ICEP_MIN_WEBSECURESAMPLESUBMISSION,
                                              ICEP_MAX_WEBSECURESAMPLESUBMISSION),
  RegEntries("webSecureStatusQuery",          ICEP_DEF_WEBSECURESTATUSQUERY,
                                              ICEP_MIN_WEBSECURESTATUSQUERY,
                                              ICEP_MAX_WEBSECURESTATUSQUERY),
  RegEntries("webSecureSignatureDownload",    ICEP_DEF_WEBSECURESIGNATUREDOWNLOAD,
                                              ICEP_MIN_WEBSECURESIGNATUREDOWNLOAD,
                                              ICEP_MAX_WEBSECURESIGNATUREDOWNLOAD),
  RegEntries("webSecureIgnoreHostname",       ICEP_DEF_WEBSECUREIGNOREHOSTNAME,
                                              ICEP_MIN_WEBSECUREIGNOREHOSTNAME,
                                              ICEP_MAX_WEBSECUREIGNOREHOSTNAME),
  RegEntries("webStatusInterval",             ICEP_DEF_WEBSTATUSINTERVAL,
                                              ICEP_MIN_WEBSTATUSINTERVAL,
                                              ICEP_MAX_WEBSTATUSINTERVAL),
  RegEntries("webBlessedInterval",            ICEP_DEF_WEBBLESSEDINTERVAL,
                                              ICEP_MIN_WEBBLESSEDINTERVAL,
                                              ICEP_MAX_WEBBLESSEDINTERVAL),
  RegEntries("webNeededInterval",             ICEP_DEF_WEBNEEDEDINTERVAL,
                                              ICEP_MIN_WEBNEEDEDINTERVAL,
                                              ICEP_MAX_WEBNEEDEDINTERVAL),
  RegEntries("webTimeoutInterval",            ICEP_DEF_WEBTIMEOUTINTERVAL,
                                              ICEP_MIN_WEBTIMEOUTINTERVAL,
                                              ICEP_MAX_WEBTIMEOUTINTERVAL),
  RegEntries("webRetryInterval",              ICEP_DEF_WEBRETRYINTERVAL,
                                              ICEP_MIN_WEBRETRYINTERVAL,
                                              ICEP_MAX_WEBRETRYINTERVAL),
  RegEntries("webRetryLimit",                 ICEP_DEF_WEBRETRYLIMIT,
                                              ICEP_MIN_WEBRETRYLIMIT,
                                              ICEP_MAX_WEBRETRYLIMIT),

  RegEntries("customerName",                  ICEP_DEF_CUSTOMERNAME),
  RegEntries("customerContactName",           ICEP_DEF_CUSTOMERCONTACTNAME),
  RegEntries("customerContactTelephone",      ICEP_DEF_CUSTOMERCONTACTTELEPHONE),
  RegEntries("customerContactEmail",          ICEP_DEF_CUSTOMERCONTACTEMAIL),
  RegEntries("customerIdentifier",            ICEP_DEF_CUSTOMERIDENTIFIER),

  RegEntries("sampleQuarantineInterval",      ICEP_DEF_SAMPLEQUARANTINEINTERVAL,
                                              ICEP_MIN_SAMPLEQUARANTINEINTERVAL,
                                              ICEP_MAX_SAMPLEQUARANTINEINTERVAL),
  RegEntries("sampleSubmissionPriority",      ICEP_DEF_SAMPLESUBMISSIONPRIORITY,
                                              ICEP_MIN_SAMPLESUBMISSIONPRIORITY,
                                              ICEP_MAX_SAMPLESUBMISSIONPRIORITY),
  RegEntries("sampleMaximumPending",          ICEP_DEF_SAMPLEMAXIMUMPENDING,
                                              ICEP_MIN_SAMPLEMAXIMUMPENDING,
                                              ICEP_MAX_SAMPLEMAXIMUMPENDING),
  RegEntries("sampleStripContent",            ICEP_DEF_SAMPLESTRIPCONTENT,
                                              ICEP_MIN_SAMPLESTRIPCONTENT,
                                              ICEP_MAX_SAMPLESTRIPCONTENT),
  RegEntries("sampleCompressContent",         ICEP_DEF_SAMPLECOMPRESSCONTENT,
                                              ICEP_MIN_SAMPLECOMPRESSCONTENT,
                                              ICEP_MAX_SAMPLECOMPRESSCONTENT),
  RegEntries("sampleScrambleContent",         ICEP_DEF_SAMPLESCRAMBLECONTENT,
                                              ICEP_MIN_SAMPLESCRAMBLECONTENT,
                                              ICEP_MAX_SAMPLESCRAMBLECONTENT),

  RegEntries("definitionBlessedBroadcast",    ICEP_DEF_DEFINITIONBLESSEDBROADCAST,
                                              ICEP_MIN_DEFINITIONBLESSEDBROADCAST,
                                              ICEP_MAX_DEFINITIONBLESSEDBROADCAST),
  RegEntries("definitionBlessedTargets",      ICEP_DEF_DEFINITIONBLESSEDTARGETS),
  RegEntries("definitionDeliveryInterval",    ICEP_DEF_DEFINITIONDELIVERYINTERVAL,
                                              ICEP_MIN_DEFINITIONDELIVERYINTERVAL,
                                              ICEP_MAX_DEFINITIONDELIVERYINTERVAL),
  RegEntries("definitionDeliveryPriority",    ICEP_DEF_DEFINITIONDELIVERYPRIORITY,
                                              ICEP_MIN_DEFINITIONDELIVERYPRIORITY,
                                              ICEP_MAX_DEFINITIONDELIVERYPRIORITY),
  RegEntries("definitionDeliveryTimeout",     ICEP_DEF_DEFINITIONDELIVERYTIMEOUT,
                                              ICEP_MIN_DEFINITIONDELIVERYTIMEOUT,
                                              ICEP_MAX_DEFINITIONDELIVERYTIMEOUT),
#if 0 /* inw 2000-02-08 -- ejp says this is deprecated. */
  RegEntries("definitionCheckInterval",       ICEP_DEF_DEFINITIONCHECKINTERVAL,
                                              ICEP_MIN_DEFINITIONCHECKINTERVAL,
                                              ICEP_MAX_DEFINITIONCHECKINTERVAL),
#endif /* 0 */
  RegEntries("definitionLibraryDirectory",    dtDir,
                                              ICEP_DEF_DEFINITIONLIBRARYDIRECTORY),
  RegEntries("definitionPrune",               ICEP_DEF_DEFINITIONPRUNE,
                                              ICEP_MIN_DEFINITIONPRUNE,
                                              ICEP_MAX_DEFINITIONPRUNE),
  RegEntries("definitionUnblessedBroadcast",  ICEP_DEF_DEFINITIONUNBLESSEDBROADCAST,
                                              ICEP_MIN_DEFINITIONUNBLESSEDBROADCAST,
                                              ICEP_MAX_DEFINITIONUNBLESSEDBROADCAST),
  RegEntries("definitionUnblessedNarrowcast", ICEP_DEF_DEFINITIONUNBLESSEDNARROWCAST,
                                              ICEP_MIN_DEFINITIONUNBLESSEDNARROWCAST,
                                              ICEP_MAX_DEFINITIONUNBLESSEDNARROWCAST),
  RegEntries("definitionUnblessedPointcast",  ICEP_DEF_DEFINITIONUNBLESSEDPOINTCAST,
                                              ICEP_MIN_DEFINITIONUNBLESSEDPOINTCAST,
                                              ICEP_MAX_DEFINITIONUNBLESSEDPOINTCAST),
  RegEntries("definitionUnblessedTargets",    ICEP_DEF_DEFINITIONUNBLESSEDTARGETS),
  RegEntries("definitionUnpackTimeout",       ICEP_DEF_DEFINITIONUNPACKTIMEOUT,
                                              ICEP_MIN_DEFINITIONUNPACKTIMEOUT,
                                              ICEP_MAX_DEFINITIONUNPACKTIMEOUT),
  RegEntries("definitionHeuristicLevel",      ICEP_DEF_DEFINITIONHEURISTICLEVEL,
                                              ICEP_MIN_DEFINITIONHEURISTICLEVEL,
                                              ICEP_MAX_DEFINITIONHEURISTICLEVEL),

  RegEntries("configurationChangeCounter",    ICEP_DEF_CONFIGURATIONCHANGECOUNTER,
                                              ICEP_MIN_CONFIGURATIONCHANGECOUNTER,
                                              ICEP_MAX_CONFIGURATIONCHANGECOUNTER),
  RegEntries("configurationChangeInterval",   ICEP_DEF_CONFIGURATIONCHANGEINTERVAL,
                                              ICEP_MIN_CONFIGURATIONCHANGEINTERVAL,
                                              ICEP_MAX_CONFIGURATIONCHANGEINTERVAL),

  RegEntries("traceEnable",                   ICEP_DEF_TRACEENABLE,
                                              ICEP_MIN_TRACEENABLE,
                                              ICEP_MAX_TRACEENABLE),
  RegEntries("traceFilename",                 ICEP_DEF_TRACEFILENAME),
  RegEntries("traceSeverity",                 ICEP_DEF_TRACESEVERITY),
  RegEntries("traceFilter",                   ICEP_DEF_TRACEFILTER,
                                              ICEP_MIN_TRACEFILTER,
                                              ICEP_MAX_TRACEFILTER),

  RegEntries("temporaryDirectory",            ICEP_DEF_TEMPORARYDIRECTORY),

  /*
   * The 'configurationIngoreRanges' registry value can be set to zero or one,
   * and is omitted from RegistryDefaults.h.  This is intentional.
   * It is an undocumented parameter, to be used for testing,
   * and should never be given to customers.
   *
   * Its [ae]ffect is to suspend observation of the minima and maxima
   * for numeric registry values given above.  So, if you want to
   * set the maximum submission counter ('sampleMaxPending') to
   * 15 million, you can do so provided you create the 'configurationIgnoreRanges'
   * value and set it to 1.
   *
   * This value does not exist by default.  It's not created by the
   * installation program, and it's not copied to the 'current' key.
   */
  RegEntries("configurationIgnoreRanges",     0, 0, 1, 1),
};

bool RegistryMonitor::InitDWORDValue(WinRegistry& uRoot, WinRegistry& cRoot, const int dontCopy, const TCHAR* key,
                   DWORD& value, DWORD defaultValue, DWORD min, DWORD max)
{
  uchar charValue[MaxStrLength];
  DWORD length(MaxStrLength);

  if (!uRoot.QueryValue(key, charValue, length) &&
    !cRoot.QueryValue(key, charValue, length))
  {
    value = defaultValue;
  }
  else
  {
    sscanf((const char *)charValue, "%i", &value);

    /*
     * If we are not in billification mode, then observe the
     * minima and maxima.  If we are billifying, then skip this.
     */
    if (! billification)
    {
      if (value > max)
        value = max;
      else if (value < min)
        value = min;
    }
  }

  /*
   * Assuming this is a value that we are copying into 'current',
   * then do so (this also resets the non-current value, as we
   * may have changed the value in the min/max check).
   */
  if (! dontCopy)
  {
    sprintf((char *) charValue, "%d", value);

    uRoot.SetValue(key, charValue);
    cRoot.SetValue(key, charValue);
  }

  return true;
}

bool RegistryMonitor::InitStrValue(WinRegistry& uRoot, WinRegistry& cRoot, const int dontCopy, const TCHAR* key,
                   TCHAR* value, const TCHAR* defaultValue)
{
  DWORD length  = MaxStrLength;
  DWORD length2 = MaxStrLength;
  if (!uRoot.QueryValue(key, (uchar*)value, length) && !cRoot.QueryValue(key, (uchar*)value, length2))
    strcpy(value, defaultValue);

  uRoot.SetValue(key, (uchar*)value);
  cRoot.SetValue(key, (uchar*)value);

  return true;
}


bool RegistryMonitor::InitDirValue(WinRegistry& uRoot, WinRegistry& cRoot, const int dontCopy, const TCHAR* key,
                   TCHAR* value, const TCHAR* defaultValue)
{
  DWORD length  = MaxStrLength;
  DWORD length2 = MaxStrLength;
  if (!uRoot.QueryValue(key, (uchar*)value, length) && !cRoot.QueryValue(key, (uchar*)value, length2))
    strcpy(value, defaultValue);

  bool  okay = 0 != (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(value));
  if (!okay)
    okay = 0 != CreateDirectory(value, NULL);
  if (!okay && !strcmp(value, defaultValue))
  {
    strcpy(value, defaultValue);
    okay = 0 != CreateDirectory(value, NULL);
  }

  if (!okay)
    Attention::Set(Attention::Disk);

  uRoot.SetValue(key, (uchar*)value);
  cRoot.SetValue(key, (uchar*)value);

  return okay;
}


static void SetTraceSeverity(char *traceSeverity)
{
  Logger::SeverityFilter(Logger::SeverityFromChar(traceSeverity));
}

bool RegistryMonitor::CreateKeys(WinRegistry &sRoot)
{
  WinRegistry uRoot(HKEY_LOCAL_MACHINE, GlobalData::regUpdateRoot);
  if (!uRoot.ValidKey())
  {
    if (!uRoot.CreateKey(sRoot.Key(), "Avis"))
      return false;
  }

  WinRegistry cRoot(HKEY_LOCAL_MACHINE, GlobalData::regCurrentRoot);
  if (!cRoot.ValidKey())
  {
    if (!cRoot.CreateKey(uRoot.Key(), "current"))
      return false;
  }

  return true;
}

bool RegistryMonitor::InitializeRegistry()
{
  WinRegistry sRoot(HKEY_LOCAL_MACHINE, GlobalData::regSymantecRoot);

  if (!sRoot.ValidKey() || GlobalData::stop)
    return false;

  if (! CreateKeys(sRoot))
    return false;

  WinRegistry uRoot(HKEY_LOCAL_MACHINE, GlobalData::regUpdateRoot);
  WinRegistry cRoot(HKEY_LOCAL_MACHINE, GlobalData::regCurrentRoot);

  char  sValue[MaxStrLength];
  DWORD iValue;

  for (int i = 0; i <= rBillification && !GlobalData::stop; i++)
  {
    if (dtString == regTable[i].type)
      InitValue(uRoot, cRoot, regTable[i], sValue);
    else if (dtInt == regTable[i].type)
      InitValue(uRoot, cRoot, regTable[i], iValue);
    else if (dtDir == regTable[i].type)
      InitDirValue(uRoot, cRoot, regTable[i], sValue);
  }

  uRoot.SetValue("attention", (const uchar *) "");

  //
  //  Up the maximum number of simultanous HTTP connections allowed from
  //  this machine from two to five

  bool    failed = true;
  WinRegistry internetRoot(HKEY_CURRENT_USER, internetSettingsKey);
  if (internetRoot.ValidKey())
  {
    DWORD five = 5;

    if (internetRoot.SetValue(httpMaxConnectionsKey, five))
    {
      five = 5;
      if (internetRoot.SetValue(httpMax1_0ConnectionsKey, five))
        failed = false;
    }
  }
  if (failed)
    Logger::Log(Logger::LogError, Logger::LogIcePackRegMonitor,
            "Unable to change max http connects to 5");

  return true;
}


bool RegistryMonitor::DeInitializeRegistry()
{
  {
    WinRegistry uRoot(HKEY_LOCAL_MACHINE, GlobalData::regUpdateRoot);

    if (ERROR_SUCCESS != RegDeleteKey(uRoot.Key(), "current"))
      return false;
  }

  WinRegistry sRoot(HKEY_LOCAL_MACHINE, GlobalData::regSymantecRoot);

  if (ERROR_SUCCESS == RegDeleteKey(sRoot.Key(), "Avis"))
    return true;
  else
    return false;
}

void RegistryMonitor::InitLogging()
{
  WinRegistry uRoot(HKEY_LOCAL_MACHINE, GlobalData::regUpdateRoot);
  WinRegistry cRoot(HKEY_LOCAL_MACHINE, GlobalData::regCurrentRoot);

  InitValue(uRoot, cRoot, regTable[rTraceFilter], traceFilter);
  Logger::SourceFilter(traceFilter);

  InitValue(uRoot, cRoot, regTable[rTraceSeverity], traceSeverity);
  SetTraceSeverity(traceSeverity);

  InitValue(uRoot, cRoot, regTable[rTraceFileName], traceFilename);
  GlobalData::TraceFileName(traceFilename);

  InitValue(uRoot, cRoot, regTable[rTraceEnable], traceEnable);
  GlobalData::TraceEnable(0 != traceEnable);

//  InitValue(uRoot, cRoot, regTable[rMonitorPort], monitorPort);
//  InformationServer::MonitorPort(monitorPort);
}

void RegistryMonitor::DeInitLogging()
{
  GlobalData::TraceEnable(false);
}

RegistryMonitor::RegistryMonitor()
{
//  IcePackMonitor::IcePackState("Initializing: Loading values from the registry");

  EntryExit entryExit(Logger::LogIcePackRegMonitor, "Constructor");

  if (GlobalData::stop)
    return;

  /*
   * Ensure that both ...\Server\AVIS and ...\Server\AVIS\current exist.
   */
  {
    WinRegistry sRoot(HKEY_LOCAL_MACHINE, GlobalData::regSymantecRoot);
    CreateKeys(sRoot);
  }

  bool    rc = false;
  WinRegistry uRoot(HKEY_LOCAL_MACHINE, GlobalData::regUpdateRoot);
  WinRegistry cRoot(HKEY_LOCAL_MACHINE, GlobalData::regCurrentRoot);

  uint  seqNum  = 1;
  bool  okay  = true;

//  GlobalData::SetNewestSigAvailable(seqNum, false);

  InitValue(uRoot, cRoot, regTable[rConfigurationChangeCounter], configurationChangeCounter);

  InitValue(uRoot, cRoot, regTable[rBillification], billification);

  InitValue(uRoot, cRoot, regTable[rWebGatewayName], webGatewayName);
  if ('\0' == webGatewayName[0])
    throw IcePackException(IcePackException::RegistryMonitor, "Missing gateway address");
  GlobalData::GatewayAddress(webGatewayName);

  InitValue(uRoot, cRoot, regTable[rWebGatewayPort], webGatewayPort);
  if (0 == webGatewayPort)
    throw IcePackException(IcePackException::RegistryMonitor, "Missing gateway port");
  GlobalData::GatewayPort(webGatewayPort);

  InitValue(uRoot, cRoot, regTable[rWebGatewayPortSSL], webGatewayPortSSL);
  GlobalData::GatewayPortSSL(webGatewayPortSSL);

  InitValue(uRoot, cRoot, regTable[rWebFirewallName], webFirewallName);
  GlobalData::FirewallName(webFirewallName);

  InitValue(uRoot, cRoot, regTable[rWebFirewallPort], webFirewallPort);
  GlobalData::FirewallPort(webFirewallPort);

  InitValue(uRoot, cRoot, regTable[rWebFirewallUser], webFirewallUser);
  GlobalData::FirewallUser(webFirewallUser);

  InitValue(uRoot, cRoot, regTable[rWebFirewallPassword], webFirewallPassword);
  GlobalData::FirewallPassword(webFirewallPassword);

  InitValue(uRoot, cRoot, regTable[rWebStatusInterval], webStatusInterval);
  GlobalData::StatusCheckInterval(webStatusInterval*60*1000);

  InitValue(uRoot, cRoot, regTable[rWebSecureSampleSubmission], webSecureSampleSubmission);
  GlobalData::SecureSampleSubmission(1 == webSecureSampleSubmission);

  InitValue(uRoot, cRoot, regTable[rWebSecureStatusQuery], webSecureStatusQuery);
  GlobalData::SecureStatusQuery(1 == webSecureStatusQuery);

  InitValue(uRoot, cRoot, regTable[rWebSecureSignatureDownload], webSecureSignatureDownload);
  GlobalData::SecureSigDownload(1 == webSecureSignatureDownload);

  InitValue(uRoot, cRoot, regTable[rWebSecureIgnoreHostname], webSecureIgnoreHostname);
  GlobalData::SecureIgnoreHostname(1 == webSecureIgnoreHostname);

  InitValue(uRoot, cRoot, regTable[rWebBlessedInterval], webBlessedInterval);
  GlobalData::NewBlessedInterval(webBlessedInterval*60*1000);

  InitValue(uRoot, cRoot, regTable[rWebNeededInterval], webNeededInterval);
  GlobalData::NeededInterval(webNeededInterval*60*1000);

  InitValue(uRoot, cRoot, regTable[rWebTimeoutInterval], webTimeoutInterval);
  GlobalData::TimeoutInterval(webTimeoutInterval*60*1000);

  InitValue(uRoot, cRoot, regTable[rWebRetryInterval], webRetryInterval);
  GlobalData::RetryInterval(webRetryInterval*60*1000);

  InitValue(uRoot, cRoot, regTable[rWebRetryLimit], webRetryLimit);
  GlobalData::RetryLimit(webRetryLimit);

  InitValue(uRoot, cRoot, regTable[rCustomerName], customerCompanyName);
  GlobalData::CompanyName(customerCompanyName);

//  InitStrValue(uRoot, cRoot, customerCompanyAddressKey, customerCompanyAddress, _T(""));
//  GlobalData::CompanyAddress(customerCompanyAddress);

  InitValue(uRoot, cRoot, regTable[rCustomerContactName], customerContactName);
  GlobalData::ContactName(customerContactName);

  InitValue(uRoot, cRoot, regTable[rCustomerContactTelephone], customerContactTelephone);
  GlobalData::ContactTelephone(customerContactTelephone);

  InitValue(uRoot, cRoot, regTable[rCustomerContactEmail], customerContactEmail);
  GlobalData::ContactEmail(customerContactEmail);

  InitValue(uRoot, cRoot, regTable[rCustomerID], customerAccount);
  GlobalData::CustomerAccount(customerAccount);

  InitValue(uRoot, cRoot, regTable[rSampleQuarantineInterval], sampleQuarantineInterval);
  GlobalData::QuarantineInterval(sampleQuarantineInterval*60*1000);

  InitValue(uRoot, cRoot, regTable[rSampleSubmissionPriority], sampleSubmissionPriority);
  GlobalData::InitialSubmissionPriority(sampleSubmissionPriority);

  InitValue(uRoot, cRoot, regTable[rSampleMaximumPending], sampleMaximumPending);
  GlobalData::MaxPendingSamples(sampleMaximumPending);

  InitValue(uRoot, cRoot, regTable[rSampleStripContent], sampleStripContent);
  GlobalData::StripUserData(0 != sampleStripContent);

  InitValue(uRoot, cRoot, regTable[rSampleCompressContent], sampleCompressContent);
  GlobalData::CompressContent(0 != sampleCompressContent);

  InitValue(uRoot, cRoot, regTable[rSampleScrambleContent], sampleScrambleContent);
  GlobalData::ScrambleContent(0 != sampleScrambleContent);

#if 0 /* inw 2000-02-08 -- ejp says this is deprecated. */
  InitValue(uRoot, cRoot, regTable[rDefinitionCheckInterval], definitionCheckInterval);
  GlobalData::TopologyCheckInterval(definitionCheckInterval*60*1000);
#endif /* 0 */

  InitValue(uRoot, cRoot, regTable[rDefinitionBlessedBroadcast], definitionBlessedBroadcast);
  GlobalData::DefBlessedBroadcast(1 == definitionBlessedBroadcast);

  InitValue(uRoot, cRoot, regTable[rDefinitionBlessedTargets], definitionBlessedTargets);
  GlobalData::DefBlessedTargets(definitionBlessedTargets);

  InitValue(uRoot, cRoot, regTable[rDefinitionDeliveryInterval], definitionDeliveryInterval);
  GlobalData::DefDeliveryInterval(definitionDeliveryInterval*60*1000);

  InitValue(uRoot, cRoot, regTable[rDefinitionDeliveryPriority], definitionDeliveryPriority);
  GlobalData::DefDeliveryPriority(definitionDeliveryPriority);

  InitValue(uRoot, cRoot, regTable[rDefinitionDeliveryTimeout], definitionDeliveryTimeout);
  GlobalData::DefDeliveryTimeout(definitionDeliveryTimeout*60*1000);

  InitValue(uRoot, cRoot, regTable[rDefinitionPrune], definitionPrune);
  GlobalData::DefPrune(1 == definitionPrune);

  InitValue(uRoot, cRoot, regTable[rDefinitionUnblessedBroadcast], definitionUnblessedBroadcast);
  GlobalData::DefUnblessedBroadcast(1 == definitionUnblessedBroadcast);

  InitValue(uRoot, cRoot, regTable[rDefinitionUnblessedNarrowcast], definitionUnblessedNarrowcast);
  GlobalData::DefUnblessedNarrowcast(1 == definitionUnblessedNarrowcast);

  InitValue(uRoot, cRoot, regTable[rDefinitionUnblessedPointcast], definitionUnblessedPointcast);
  GlobalData::DefUnblessedPointcast(1 == definitionUnblessedPointcast);

  InitValue(uRoot, cRoot, regTable[rDefinitionUnblessedTargets], definitionUnblessedTargets);
  GlobalData::DefUnblessedTargets(definitionUnblessedTargets);

  InitValue(uRoot, cRoot, regTable[rDefinitionUnpackTimeout], definitionUnpackTimeout);
  GlobalData::DefUnpackTimeout(definitionUnpackTimeout*60*1000);

  ulong temp;
  InitValue(uRoot, cRoot, regTable[rDefinitionHeuristicLevel], temp);

  InitDirValue(uRoot, cRoot, regTable[rDefinitionLibraryDirectory], definitionLibraryDirectory);
  GlobalData::DefLibraryDirectory(definitionLibraryDirectory);

  InitValue(uRoot, cRoot, regTable[rConfigurationChangeInterval], configurationChangeInterval);

  InitDirValue(uRoot, cRoot, regTable[rTemporaryDirectory], temporaryDirectory);
  GlobalData::TempDirectory(temporaryDirectory);

  GlobalData::FinishedInitializing();
  GlobalData::SetCommParameters();


  //
  //  Log the current number of max number of http connections


  WinRegistry internetRoot(HKEY_CURRENT_USER, internetSettingsKey);
  if (internetRoot.ValidKey())
  {
    DWORD maxConns  = 5;
    DWORD max1_0Conns = 5;

    if (internetRoot.QueryValue(httpMaxConnectionsKey, &maxConns))
    {
      if (internetRoot.QueryValue(httpMax1_0ConnectionsKey, &max1_0Conns))
      {
        char  buffer[96];
        sprintf(buffer, "Max http connections = %d, max http 1.0 connections = %d",
                maxConns, max1_0Conns);
        Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
      }
      else
        Logger::Log(Logger::LogWarning, Logger::LogIcePackRegMonitor,
              "Unable to get max http 1.0 connections value from registry");
    }
    else
      Logger::Log(Logger::LogWarning, Logger::LogIcePackRegMonitor,
            "Unable to get max http connections value from registry");
  }
  else
    Logger::Log(Logger::LogError, Logger::LogIcePackRegMonitor,
            "Key to max number of http connections is invalid");
  LogAllConfigValues();

}

RegistryMonitor::~RegistryMonitor()
{

}
bool RegistryMonitor::CheckForChanges(void)   // return true if a change occured
{
  bool    rc = false;
  WinRegistry uRoot(HKEY_LOCAL_MACHINE, GlobalData::regUpdateRoot);
  WinRegistry cRoot(HKEY_LOCAL_MACHINE, GlobalData::regCurrentRoot);


  if (CheckForChange(uRoot, cRoot, regTable[rConfigurationChangeCounter], configurationChangeCounter)
    && !GlobalData::stop)
  {
    bool  sigSubmissions  = false;

    rc = true;

    if (CheckForChange(uRoot, cRoot, regTable[rWebGatewayName], webGatewayName))
      GlobalData::GatewayAddress(webGatewayName);

    if (CheckForChange(uRoot, cRoot, regTable[rWebGatewayPort], webGatewayPort))
      GlobalData::GatewayPort(webGatewayPort);

    if (CheckForChange(uRoot, cRoot, regTable[rWebGatewayPortSSL], webGatewayPortSSL))
      GlobalData::GatewayPortSSL(webGatewayPortSSL);

    if (CheckForChange(uRoot, cRoot, regTable[rWebFirewallName], webFirewallName))
      GlobalData::FirewallName(webFirewallName);

    if (CheckForChange(uRoot, cRoot, regTable[rWebFirewallPort], webFirewallPort))
      GlobalData::FirewallPort(webFirewallPort);

    if (CheckForChange(uRoot, cRoot, regTable[rWebFirewallUser], webFirewallUser))
      GlobalData::FirewallUser(webFirewallUser);

    if (CheckForChange(uRoot, cRoot, regTable[rWebFirewallPassword], webFirewallPassword))
      GlobalData::FirewallPassword(webFirewallPassword);

    if (CheckForChange(uRoot, cRoot, regTable[rWebSecureSampleSubmission], webSecureSampleSubmission))
      GlobalData::SecureSampleSubmission(1 == webSecureSampleSubmission);

    if (CheckForChange(uRoot, cRoot, regTable[rWebSecureStatusQuery], webSecureStatusQuery))
      GlobalData::SecureStatusQuery(1 == webSecureStatusQuery);

    if (CheckForChange(uRoot, cRoot, regTable[rWebSecureSignatureDownload], webSecureSignatureDownload))
      GlobalData::SecureSigDownload(1 == webSecureSignatureDownload);

    if (CheckForChange(uRoot, cRoot, regTable[rWebSecureIgnoreHostname], webSecureIgnoreHostname))
      GlobalData::SecureSigDownload(1 == webSecureIgnoreHostname);

    if (CheckForChange(uRoot, cRoot, regTable[rWebStatusInterval], webStatusInterval))
      GlobalData::StatusCheckInterval(webStatusInterval*60*1000);

    if (CheckForChange(uRoot, cRoot, regTable[rWebBlessedInterval], webBlessedInterval))
      GlobalData::NewBlessedInterval(webBlessedInterval*60*1000);

    if (CheckForChange(uRoot, cRoot, regTable[rWebNeededInterval], webNeededInterval))
      GlobalData::NeededInterval(webNeededInterval*60*1000);

    if (CheckForChange(uRoot, cRoot, regTable[rWebTimeoutInterval], webTimeoutInterval))
      GlobalData::TimeoutInterval(webTimeoutInterval*60*1000);

    if (CheckForChange(uRoot, cRoot, regTable[rWebRetryInterval], webRetryInterval))
      GlobalData::RetryInterval(webRetryInterval*60*1000);

    if (CheckForChange(uRoot, cRoot, regTable[rWebRetryLimit], webRetryLimit))
      GlobalData::RetryLimit(webRetryLimit);

    if (CheckForChange(uRoot, cRoot, regTable[rCustomerName], customerCompanyName))
      GlobalData::CompanyName(customerCompanyName);

//    if (CheckForStrChange(uRoot, cRoot, customerCompanyAddressKey, customerCompanyAddress))
//      GlobalData::CompanyAddress(customerCompanyAddress);

    if (CheckForChange(uRoot, cRoot, regTable[rCustomerContactName], customerContactName))
      GlobalData::ContactName(customerContactName);

    if (CheckForChange(uRoot, cRoot, regTable[rCustomerContactTelephone], customerContactTelephone))
      GlobalData::ContactTelephone(customerContactTelephone);

    if (CheckForChange(uRoot, cRoot, regTable[rCustomerContactEmail], customerContactEmail))
      GlobalData::ContactEmail(customerContactEmail);

    if (CheckForChange(uRoot, cRoot, regTable[rCustomerID], customerAccount))
      GlobalData::CustomerAccount(customerAccount);


    if (CheckForChange(uRoot, cRoot, regTable[rSampleQuarantineInterval], sampleQuarantineInterval))
      GlobalData::QuarantineInterval(sampleQuarantineInterval*60*1000);

    if (CheckForChange(uRoot, cRoot, regTable[rSampleSubmissionPriority], sampleSubmissionPriority))
      GlobalData::InitialSubmissionPriority(sampleSubmissionPriority);

    DWORD oldMaxSamplePending = sampleMaximumPending;
    if (CheckForChange(uRoot, cRoot, regTable[rSampleMaximumPending], sampleMaximumPending))
    {
      sigSubmissions  = oldMaxSamplePending < sampleMaximumPending;
      GlobalData::MaxPendingSamples(sampleMaximumPending);
    }

    if (CheckForChange(uRoot, cRoot, regTable[rSampleStripContent], sampleStripContent))
      GlobalData::StripUserData(0 != sampleStripContent);

    if (CheckForChange(uRoot, cRoot, regTable[rSampleCompressContent], sampleCompressContent))
      GlobalData::CompressContent(0 != sampleCompressContent);

    if (CheckForChange(uRoot, cRoot, regTable[rSampleScrambleContent], sampleScrambleContent))
      GlobalData::ScrambleContent(0 != sampleScrambleContent);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionBlessedBroadcast], definitionBlessedBroadcast))
      GlobalData::DefBlessedBroadcast(1 == definitionBlessedBroadcast);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionBlessedTargets], definitionBlessedTargets))
      GlobalData::DefBlessedTargets(definitionBlessedTargets);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionDeliveryInterval], definitionDeliveryInterval))
      GlobalData::DefDeliveryInterval(definitionDeliveryInterval*60*1000);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionDeliveryPriority], definitionDeliveryPriority))
      GlobalData::DefDeliveryPriority(definitionDeliveryPriority);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionDeliveryTimeout], definitionDeliveryTimeout))
      GlobalData::DefDeliveryTimeout(definitionDeliveryTimeout*60*1000);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionPrune], definitionPrune))
      GlobalData::DefPrune(1 == definitionPrune);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionUnblessedBroadcast], definitionUnblessedBroadcast))
      GlobalData::DefUnblessedBroadcast(1 == definitionUnblessedBroadcast);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionUnblessedNarrowcast], definitionUnblessedNarrowcast))
      GlobalData::DefUnblessedNarrowcast(1 == definitionUnblessedNarrowcast);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionUnblessedPointcast], definitionUnblessedPointcast))
      GlobalData::DefUnblessedPointcast(1 == definitionUnblessedPointcast);

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionUnblessedTargets], definitionUnblessedTargets))
      GlobalData::DefUnblessedTargets(definitionUnblessedTargets);

#if 0 /* inw 2000-02-08 -- ejp says this is deprecated. */
    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionCheckInterval], definitionCheckInterval))
      GlobalData::TopologyCheckInterval(definitionCheckInterval*60*1000);
#endif /* 0 */

    if (CheckForChange(uRoot, cRoot, regTable[rDefinitionUnpackTimeout], definitionUnpackTimeout))
      GlobalData::DefUnpackTimeout(definitionUnpackTimeout*60*1000);

    ulong temp;
    CheckForChange(uRoot, cRoot, regTable[rDefinitionHeuristicLevel], temp);

    if (CheckForDirChange(uRoot, cRoot, regTable[rDefinitionLibraryDirectory], definitionLibraryDirectory))
      GlobalData::DefLibraryDirectory(definitionLibraryDirectory);

    CheckForChange(uRoot, cRoot, regTable[rConfigurationChangeInterval], configurationChangeInterval);

    if (CheckForChange(uRoot, cRoot, regTable[rTraceFileName], traceFilename))
      GlobalData::TraceFileName(traceFilename);

    if (CheckForChange(uRoot, cRoot, regTable[rTraceFilter], traceFilter))
      Logger::SourceFilter(traceFilter);

    if (CheckForChange(uRoot, cRoot, regTable[rTraceSeverity], traceSeverity))
      SetTraceSeverity(traceSeverity);

    if (CheckForDirChange(uRoot, cRoot, regTable[rTemporaryDirectory], temporaryDirectory))
      GlobalData::TempDirectory(temporaryDirectory);

    if (CheckForChange(uRoot, cRoot, regTable[rTraceEnable], traceEnable))
      GlobalData::TraceEnable(0 != traceEnable);

//    if (CheckForChange(uRoot, cRoot, regTable[rMonitorPort], monitorPort))
//      ; //InformationServer::MonitorPort(monitorPort);

    if (sigSubmissions)
      GlobalData::submitSample.Set();
  }
  else
    Logger::Log(Logger::LogDebug, Logger::LogIcePackRegMonitor,
        "A change in the registry was not indicated by the change counter");


  return rc;
}
          // 
          // value returns the new value, if changed
          // returns true if changed.
bool RegistryMonitor::CheckForStrChange(WinRegistry& update, WinRegistry& current,
                    const TCHAR* key, TCHAR* value)
{
  bool  rc = false;
  TCHAR cBuffer[MaxStrLength];
  TCHAR uBuffer[MaxStrLength];
  ulong cLength = MaxStrLength;
  ulong uLength = MaxStrLength;

  if (update.QueryValue(key, (uchar*) uBuffer, uLength) &&
    current.QueryValue(key, (uchar*)cBuffer, cLength) )
  {
    if (uLength != cLength ||
      strncmp(uBuffer, cBuffer, uLength))
    {
      strncpy(value, uBuffer, MaxStrLength);
      rc    = true;

      current.SetValue(key, (uchar *) uBuffer);

      if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackRegMonitor))
      {
        string  msg("Registry value change found, key = ");
        msg += key;
        msg += " old value = ";
        msg += cBuffer;
        msg += ", new value = ";
        msg += uBuffer;

        Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, msg.c_str());
      }
    }
  }

  return rc;
}

bool RegistryMonitor::CheckForDirChange(WinRegistry& update, WinRegistry& current,
                    const TCHAR* key, TCHAR* value)
{
  bool  okay = false;
  TCHAR cBuffer[MaxStrLength];
  TCHAR uBuffer[MaxStrLength];
  ulong cLength = MaxStrLength;
  ulong uLength = MaxStrLength;

  if (update.QueryValue(key, (uchar*) uBuffer, uLength) &&
    current.QueryValue(key, (uchar*)cBuffer, cLength) )
  {
    if (uLength != cLength ||
      strncmp(uBuffer, cBuffer, uLength))
    {
      okay = 0 != (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(uBuffer));

      if (!okay)
      {
        okay = 0 != CreateDirectory(uBuffer, NULL);
      }

      if (okay)
      {
        strncpy(value, uBuffer, MaxStrLength);

        current.SetValue(key, (uchar *) uBuffer);

        Attention::UnSet(Attention::Disk);

        if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackRegMonitor))
        {
          string  msg("Registry value change found, key = ");
          msg += key;
          msg += " old value = ";
          msg += cBuffer;
          msg += ", new value = ";
          msg += uBuffer;

          Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, msg.c_str());
        }
      }
      else
      {
        update.SetValue(key, (uchar *) cBuffer);

        string  msg("Registry value change found, key = ");
        msg += key;
        msg += ", unfortunatly the directory (";
        msg += uBuffer;
        msg += ") is invalid and could not be created.  ";
        msg += "Reseting to orignal value, ";
        msg += cBuffer;

        Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, msg.c_str());
      }
    }
  }

  return okay;
}


bool RegistryMonitor::CheckForDWORDChange(WinRegistry& update, WinRegistry& current, const TCHAR* key,
                      DWORD& value, const DWORD minValue, const DWORD maxValue)
{
  bool  rc = false;
  uchar updateValue[MaxStrLength], currentValue[MaxStrLength];
  DWORD updateLength(MaxStrLength), currentLength(MaxStrLength);

  if (update.QueryValue(key, updateValue, updateLength) &&
    current.QueryValue(key,currentValue, currentLength    ) )
  {
    if (stricmp((const char *) updateValue, (const char *) currentValue))
    {
      value = atoi((const char *) updateValue);
      rc    = true;

      if (value > maxValue)
      {
        if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackRegMonitor))
        {
          char  asChar[20];
          sprintf(asChar, "%d", maxValue);
          string  msg("Registry value change found, key = ");
          msg += key;
          msg += " old value = ";
          msg += (char *)currentValue;
          msg += ", new value = ";
          msg += (char *) updateValue;
          msg += ".  New value > maxValue(";
          msg += asChar;
          msg += ").  Lowering value to ";
          msg += asChar;
          
          Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, msg.c_str());
        }

        value = maxValue;
        sprintf((char *) updateValue, "%d", value);
        update.SetValue(key, updateValue);

      }
      else if (value < minValue)
      {
        if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackRegMonitor))
        {
          char  asChar[20];
          sprintf(asChar, "%d", minValue);
          string  msg("Registry value change found, key = ");
          msg += key;
          msg += " old value = ";
          msg += (char *) currentValue;
          msg += ", new value = ";
          msg += (char *) updateValue;
          msg += ".  New value < minValue(";
          msg += asChar;
          msg += ").  Increasing value to ";
          msg += asChar;
          
          Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, msg.c_str());
        }

        value = minValue;
        sprintf((char *) updateValue, "%d", value);
        update.SetValue(key, updateValue);

      }
      else if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackRegMonitor))
      {
        string  msg("Registry value change found, key = ");
        msg += key;
        msg += " old value = ";
        msg += (char *) currentValue;
        msg += ", new value = ";
        msg += (char *) updateValue;

        Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, msg.c_str());
      }

      current.SetValue(key, updateValue);
    }
  }

  return rc;
}


unsigned RegistryMonitor::ThreadHandlerProc(void)
{
  EntryExit   entryExit(Logger::LogIcePackRegMonitor,"ThreadHandlerProc");
//  EnterExitThread eet(IcePackMonitor::registryMonitor);

  SystemException::Init();

  while (!GlobalData::stop)
  {
    //
    //  This is not quite right.  This thread can be stalled longer than the
    //  5 seconds that is requested on each itteration.  This should really
    //  get the absolute time and do comparisions, but I don't think that if
    //  it's off by a second or two it will really matter much so I'd rather
    //  not pay the overhead of a system call every 5 seconds or so.
    //
    //  The reason this is not a single sleep call for DefCheckInterval
    //  milliseconds is because we need to be more responsive to a service
    //  shutdown request.  (This way we respond in at most about 5 seconds,
    //  with one call we could wait up to an hour to shut the service down).

//    IcePackMonitor::ThreadPauseing(IcePackMonitor::registryMonitor,
//                    "Normal pause between checks on the registry");
    GlobalData::SleepInSegments(configurationChangeInterval*60000);
//    IcePackMonitor::ThreadActive(IcePackMonitor::registryMonitor);

    try
    {
      if (!GlobalData::stop)
        CheckForChanges();
    }
    catch (...)
    {
      GlobalData::HandleException("RegistryMonitor::ThreadHandlerProc()",
                    Logger::LogIcePackRegMonitor);
    }
  }

  return 0;
}

void RegistryMonitor::LogAllConfigValues()
{
  if (Logger::WillLog(Logger::LogInfo, Logger::LogIcePackRegMonitor))
  {
    char  buffer[512];

    sprintf(buffer, "IcePack version info, AVIS version = %s, build = %s, built on %s at %s",
                AVIS_VERSION, AVIS_BUILD, __DATE__, __TIME__);
    Logger::Log(Logger::LogInfo, Logger::LogEverything, buffer);

    sprintf(buffer, "webGatewayName = \"%s\"", webGatewayName);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webGatewayPort[SSL port] = %d[%d]", webGatewayPort, webGatewayPortSSL);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webFirewallName = \"%s\"", webFirewallName);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webFirewallPort = %d", webFirewallPort);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webFirewallUser = \"%s\"", webFirewallUser);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webSecureSampleSubmission = %s", webSecureSampleSubmission ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webSecureStatusQuery = %s", webSecureStatusQuery ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webSecureIgnoreHostname = %s", webSecureIgnoreHostname ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webSecureSignatureDownload = %s", webSecureSignatureDownload ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webStatusInterval = %d minute%c", webStatusInterval, webStatusInterval > 1 ? 's' : ' ');
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webBlessedInterval = %d minute%c", webBlessedInterval, webBlessedInterval > 1 ? 's' : ' ');
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webNeededInterval = %d minute%c", webNeededInterval, webNeededInterval > 1 ? 's' : ' ');
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webTimeoutInterval = %d minute%c", webTimeoutInterval, webTimeoutInterval > 1 ? 's' : ' ');
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webRetryInterval = %d minute%c", webRetryInterval, webRetryInterval > 1 ? 's' : ' ');
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "webRetryLimit = %d", webRetryLimit);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);

    sprintf(buffer, "customerName = \"%s\"", customerCompanyName);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "customerContactName = \"%s\"", customerContactName);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "customerContactTelephone = \"%s\"", customerContactTelephone);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "customerContactEmail = \"%s\"", customerContactEmail);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "customerIdentifier = \"%s\"", customerAccount);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);

    sprintf(buffer, "sampleQuarantineInterval = %d minute%c", sampleQuarantineInterval, sampleQuarantineInterval > 1 ? 's' : ' ');
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "sampleSubmissionPriority = %d", sampleSubmissionPriority);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "sampleMaximumPending = %d", sampleMaximumPending);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "sampleStripContent = %s", sampleStripContent  ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "sampleCompressContent = %s", sampleCompressContent ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "sampleScrambleContent = %s", sampleScrambleContent ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);

    
    sprintf(buffer, "definitionBlessedBroadcast = %s", definitionBlessedBroadcast ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionBlessedTargets = %s", definitionBlessedTargets);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionDeliveryInterval = %d", definitionDeliveryInterval);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionDeliveryTimeout = %d", definitionDeliveryTimeout);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionPrune = %s", definitionPrune ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionUnblessedBroadcast = %s", definitionUnblessedBroadcast ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionUnblessedNarrowcast = %s", definitionUnblessedNarrowcast ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionUnblessedPointcast = %s", definitionUnblessedPointcast ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionUnblessedTargets = %s", definitionUnblessedTargets);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);

    sprintf(buffer, "defintionDeliveryPriority = %d", definitionDeliveryPriority);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
#if 0 /* inw 2000-02-08 -- ejp says this is deprecated. */
    sprintf(buffer, "definitionCheckInterval = %d", definitionCheckInterval);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
#endif /* 0 */
    sprintf(buffer, "definitionUnpackTimeout = %d", definitionUnpackTimeout);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "definitionLibraryDirectory = \"%s\"", definitionLibraryDirectory);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);

    sprintf(buffer, "configurationChangeCounter = %d", configurationChangeCounter);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "configurationChangeInterval = %d minute%c", configurationChangeInterval, configurationChangeInterval > 1 ? 's' : ' ');
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);

    sprintf(buffer, "traceEnable = %s", traceEnable ? "true" : "false");
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "traceFilename = \"%s\"", traceFilename);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "traceFilter = 0x%08x", traceFilter);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
    sprintf(buffer, "traceSeverity = \"%s\"", traceSeverity);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);

//    sprintf(buffer, "monitorPort = %d", monitorPort);
//    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);

    sprintf(buffer, "temporaryDirectory = \"%s\"", temporaryDirectory);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackRegMonitor, buffer);
  }
}

  //
  //  Local copy of registry values

DWORD RegistryMonitor::billification = 0;
TCHAR RegistryMonitor::webGatewayName[MaxStrLength];
DWORD RegistryMonitor::webGatewayPort     = 0;
DWORD RegistryMonitor::webGatewayPortSSL    = 0;
TCHAR RegistryMonitor::webFirewallName[MaxStrLength];
DWORD RegistryMonitor::webFirewallPort    = 0;
TCHAR RegistryMonitor::webFirewallUser[MaxStrLength];
TCHAR RegistryMonitor::webFirewallPassword[MaxStrLength];
DWORD RegistryMonitor::webSecureSampleSubmission  = false;
DWORD RegistryMonitor::webSecureStatusQuery   = false;
DWORD RegistryMonitor::webSecureSignatureDownload = false;
DWORD RegistryMonitor::webSecureIgnoreHostname  = false;
DWORD RegistryMonitor::webStatusInterval    = 1;
DWORD RegistryMonitor::webBlessedInterval   = 1;
DWORD RegistryMonitor::webNeededInterval    = 1;
DWORD RegistryMonitor::webTimeoutInterval   = 1;
DWORD RegistryMonitor::webRetryInterval   = 1;
DWORD RegistryMonitor::webRetryLimit      = 1;
  
TCHAR RegistryMonitor::customerCompanyName[MaxStrLength];
TCHAR RegistryMonitor::customerCompanyAddress[MaxStrLength];
TCHAR RegistryMonitor::customerContactName[MaxStrLength];
TCHAR RegistryMonitor::customerContactTelephone[MaxStrLength];
TCHAR RegistryMonitor::customerContactEmail[MaxStrLength];
TCHAR RegistryMonitor::customerAccount[MaxStrLength];

DWORD RegistryMonitor::sampleQuarantineInterval= 1;
DWORD RegistryMonitor::sampleSubmissionPriority= 1;
DWORD RegistryMonitor::sampleMaximumPending = 2;
DWORD RegistryMonitor::sampleStripContent   = 1;
DWORD RegistryMonitor::sampleCompressContent  = 1;
DWORD RegistryMonitor::sampleScrambleContent  = 1;

DWORD RegistryMonitor::definitionBlessedBroadcast = false;
TCHAR RegistryMonitor::definitionBlessedTargets[MaxStrLength];
DWORD RegistryMonitor::definitionDeliveryInterval = 15;
DWORD RegistryMonitor::definitionDeliveryPriority = 500;
DWORD RegistryMonitor::definitionDeliveryTimeout  = 30;
#if 0 /* inw 2000-02-08 -- ejp says this is deprecated. */
DWORD RegistryMonitor::definitionCheckInterval  = 1;
#endif /* 0 */
DWORD RegistryMonitor::definitionPrune      = 1;
DWORD RegistryMonitor::definitionUnblessedBroadcast = 0;
DWORD RegistryMonitor::definitionUnblessedNarrowcast  = 1;
DWORD RegistryMonitor::definitionUnblessedPointcast = 1;
TCHAR RegistryMonitor::definitionUnblessedTargets[MaxStrLength];
DWORD RegistryMonitor::definitionUnpackTimeout  = 1;
TCHAR RegistryMonitor::definitionLibraryDirectory[MaxStrLength];

DWORD RegistryMonitor::configurationChangeCounter = 0;
DWORD RegistryMonitor::configurationChangeInterval= 1;

DWORD RegistryMonitor::traceEnable        = 0;
TCHAR RegistryMonitor::traceFilename[MaxStrLength];
DWORD RegistryMonitor::traceFilter        = 0;
TCHAR RegistryMonitor::traceSeverity[MaxStrLength];

//DWORD RegistryMonitor::monitorPort        = 2063;

TCHAR RegistryMonitor::temporaryDirectory[MaxStrLength];
