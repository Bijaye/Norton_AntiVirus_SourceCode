// GlobalData.h: interface for the GlobalData class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALDATA_H__E4E7A0A1_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
#define AFX_GLOBALDATA_H__E4E7A0A1_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
# pragma once
#endif /* _MSC_VER > 1000 */

#include <string>
#include <list>
#include <set>

#include <AVIS.h>
#include <CMclEvent.h>
#include <CMclCritSec.h>
#include <CMclAutoLock.h>
#include <Logger.h>
#include "Sample.h"

class GlobalData  
{
  public :
    static bool               stop;             // true if service is stopped(or wants to stop)
    static bool               pause;            // true if service is paused(or wants to pause)
    static CMclEvent          go;               // event to signal go again
    static CMclEvent          halt;             // signal to stop threads (and comm) that wait for events
    static CMclEvent          downloadSig;      // signal need to download sig file
    static CMclEvent          submitSample;     // signal need to upload sample
    static CMclEvent          deliverSignature; // signal need to send signature to workstations
    static CMclEvent          newBlessed;       // signal indicates that new blessed signatures
                                                // are now available.
    static CMclCritSec        updatingQuarantine;
    static ulong              quarantineChangeCounter;
    static CMclCritSec        critSamplesList;
    static std::list<Sample>  samplesList;

    static uint               GetSigToDownload(bool& blessed);
    static bool               SetSigToDownload(uint sig, bool blessed);
    static bool               SetNewestSigAvailable(uint sig, bool blessed);
    static uint               GetNewestSigAvailable(bool& blessed)
    {
      blessed = newestSigBlessed; return newestSig;
    }

    static uint               GetNewestBlessedSigAvailable(void)
    {
      return newestBlessedSig;
    }

  private :
    static void RebuildGatewayURL();
    static void RebuildSSLGatewayURL();
    static void RebuildFirewallURL();




// 8/28/00 jhill Re-enabled for IcePack Build 217a 8/30/00.
  /*
   * The Captain Midnight Secret Decoder Ring is not necessary any more,
   * as the password in the registry is no longer scrambled.  This, in turn,
   * is because the field is no longer accessible via the GUI, and the values
   * can only be modified by directly changing the registry.
   */
  //#if 0
  /*
   * This function descrambles the string pointed to by 'in'.
   * It descrambles it in-place, but returns it anyway, for
   * the convenience of the caller.
   */
  static char    *deployCaptainMidnightSecretDecoderRing(char *in)
  {
    char *walker;

    /*
     * Sanity check.
     */
    if (NULL == in)
      return NULL;

    /*
     * Walk the string, descrambling in place.
     */
    for (walker = in; *walker; walker++)
      *walker ^= 0xFF;

    /*
     * JD.
     */
    return in;
  }
  //#endif /* 0 */




public:
  static std::string GetContactHeaders(void);
  static void        UpdateMasterSampleList(Sample& sample);
  static void        RecoverDiskSpace(void)
  {
  }

  static void        GatewayAddress(TCHAR* gAddress)
  {
    /*
     * Size check is BoundsChecker-induced.
     * inw 2000-06-17
     */
    if ((! gatewayAddress.size()) || (gatewayAddress != gAddress))
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        gatewayAddress  = gAddress;
      }
      RebuildGatewayURL();
      RebuildSSLGatewayURL();
      SetCommParameters();
    }
  }

  static void        GatewayPort(uint gPort)
  {
    if (gPort != gatewayPort)
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        gatewayPort = gPort;
      }
      RebuildGatewayURL();
    }
  }

  static void        GatewayPortSSL(uint gPort)
  {
    if (gPort != gatewayPortSSL)
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        gatewayPortSSL  = gPort;
      }
      RebuildSSLGatewayURL();
    }
  }

  static void        FirewallName(TCHAR* fName)
  {
    /*
     * Size check is BoundsChecker-induced.
     * inw 2000-06-17
     */
    if ((! firewallName.size()) || (firewallName != fName))
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        firewallName  = fName;
      }
      RebuildFirewallURL();
      SetCommParameters();
    }
  }
  
  static void         FirewallPort(uint fPort)
  {
    if (fPort != firewallPort)
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        firewallPort  = fPort;
      }
      RebuildFirewallURL();
      SetCommParameters();
    }
  }

  static void         FirewallUser(TCHAR* user)
  {
    /*
     * Size check is BoundsChecker-induced.
     * inw 2000-06-17
     */
    if ((! firewallUser.size()) || (firewallUser != user))
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        firewallUser  = user;
      }
      SetCommParameters();
    }
  }

  static void        FirewallPassword(TCHAR* password)
  {
    /*
     * Size check is BoundsChecker-induced.
     * inw 2000-06-17
     */
    if ((! firewallPassword.size()) || (firewallPassword != password))
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        /*
         * The Captain Midnight Secret Decoder Ring is not necessary any more,
         * as the password in the registry is no longer scrambled.  This, in turn,
         * is because the field is no longer accessible via the GUI, and the values
         * can only be modified by directly changing the registry.
         */

         // 8/28/00 jhill Re-enabled for IcePack Build 217a 8/30/00.
         firewallPassword = deployCaptainMidnightSecretDecoderRing(password); 
         //firewallPassword  = password;
      }
      SetCommParameters();
    }
  }

  static void        QuarantineInterval(uint qt)
  {
    CMclAutoLock lock(regValuesCritSec);
    quarantineInterval  = qt;
  }

  static void        NewBlessedInterval(uint nbi)
  {
    CMclAutoLock lock(regValuesCritSec);
    newBlessedInterval  = nbi;
  }

  static void        NeededInterval(uint ni)
  {
    CMclAutoLock lock(regValuesCritSec);
    neededInterval  = ni;
  }

  static void        TimeoutInterval(uint toi)
  {
    if (toi != timeoutInterval)
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        timeoutInterval = toi;
      }
      SetCommParameters();
    }
  }

  static void        RetryInterval(uint rti)
  {
    if (rti != retryInterval)
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        retryInterval = rti;
      }
      SetCommParameters();
    }
  }

  static void        RetryLimit(uint rtl)
  {
    if (rtl != retryLimit)
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        retryLimit = rtl;
      }
    }
  }

  static void        CompanyName(TCHAR* cName)
  {
    CMclAutoLock lock(regValuesCritSec);
    companyName = cName;
  }

  static void        CompanyAddress(TCHAR* cAddress)
  {
    CMclAutoLock lock(regValuesCritSec);
    companyAddress  = cAddress;
  }

  static void    ContactName(TCHAR* cName)
  {
    CMclAutoLock lock(regValuesCritSec);
    contactName   = cName;
  }

  static void    ContactTelephone(TCHAR* cTele)
  {
    CMclAutoLock lock(regValuesCritSec);
    contactTelephone = cTele;
  }

  /*
   * For reasons best known to the original implementor, this
   * member is defined in GlobalData.cpp.
   */
  static void    ContactEmail(TCHAR* cEmail);

  static void    CustomerAccount(TCHAR* cAccount)
  {
    CMclAutoLock lock(regValuesCritSec);
    customerAccount = cAccount;
  }

  static void    InitialSubmissionPriority(uint isp)
  {
    CMclAutoLock lock(regValuesCritSec);
    initialSubmissionPriority = isp;
  }

  static void    MaxPendingSamples(uint mps)
  {
    bool      submitNewSamples = false;
    {
      CMclAutoLock lock(regValuesCritSec);

      if (maxPendingSamples < mps)
        submitNewSamples = true;
      maxPendingSamples = mps;
    }
    GlobalData::submitSample.Set();
  }

  static void    StripUserData(bool sud)
  {
    CMclAutoLock lock(regValuesCritSec);
    stripUserData = sud;
  }

  static void    CompressContent(bool cc)
  {
    CMclAutoLock lock(regValuesCritSec);
    compressContent = cc;
  }

  static void    ScrambleContent(bool sc)
  {
    CMclAutoLock lock(regValuesCritSec);
    scrambleContent = sc;
  }

  static void    StatusCheckInterval(uint sci)
  {
    CMclAutoLock lock(regValuesCritSec);
    statusCheckInterval = sci;
  }

  static void    RegistryCheckInterval(uint rci)
  {
    CMclAutoLock lock(regValuesCritSec);
    registryCheckInterval = rci;
  }

  static void    TopologyCheckInterval(uint tci)
  {
    CMclAutoLock lock(regValuesCritSec);
    topologyCheckInterval = tci;
  }

  static void    DefUnpackTimeout(uint dut)
  {
    if (dut != defUnpackTimeout)
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        defUnpackTimeout  = dut;
      }
      SetCommParameters();
    }
  }

  static void    DefBlessedBroadcast(bool dbb)
  {
    CMclAutoLock lock(regValuesCritSec);
    defBlessedBroadcast = dbb;
  }

  static void    DefBlessedTargets(TCHAR* dbt)
  {
    CMclAutoLock lock(regValuesCritSec);
    defBlessedTargets = dbt;
  }

  static void    DefDeliveryInterval(uint dbdi)
  {
    CMclAutoLock lock(regValuesCritSec);
    defDeliveryInterval = dbdi;
  }

  static void    DefDeliveryPriority(uint dbdp)
  {
    CMclAutoLock lock(regValuesCritSec);
    defDeliveryPriority = dbdp;
  }

  static void    DefDeliveryTimeout(uint dbdt)
  {
    CMclAutoLock lock(regValuesCritSec);
    defDeliveryTimeout  = dbdt;
  }

  static void    DefPrune(bool dp)
  {
    CMclAutoLock lock(regValuesCritSec);
    defPrune  = dp;
  }

  static void    DefUnblessedBroadcast(bool dub)
  {
    CMclAutoLock lock(regValuesCritSec);
    defUnblessedBroadcast = dub;
  }

  static void    DefUnblessedNarrowcast(bool dun)
  {
    CMclAutoLock lock(regValuesCritSec);
    defUnblessedNarrowcast  = dun;
  }

  static void    DefUnblessedPointcast(bool dup)
  {
    CMclAutoLock lock(regValuesCritSec);
    defUnblessedPointcast = dup;
  }

  static void    DefUnblessedTargets(TCHAR* dut)
  {
    CMclAutoLock lock(regValuesCritSec);
    defUnblessedTargets = dut;
  }

  static void    DefLibraryDirectory(TCHAR* dld)
  {
    /*
     * Size check is BoundsChecker-induced.
     * inw 2000-06-17
     */
    if (((! defLibraryDirectory.size()) || (defLibraryDirectory != dld)) &&
        ((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(dld)) ||
         (CreateDirectory(dld, NULL))))
    {
      CMclAutoLock lock(regValuesCritSec);
      defLibraryDirectory = dld;
    }
  }

  /*
   *  These are also defined in GlobalData.cpp.
   */
  static void    TraceEnable(bool te);
  static void    TraceFileName(TCHAR* tfn);

  static void    TempDirectory(TCHAR* td)
  {
    /*
     * Size check is BoundsChecker-induced.
     * inw 2000-06-17
     */
    if (((! tempDirectory.size()) || (tempDirectory != td)) &&
        ((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(td)) ||
        (CreateDirectory(td, NULL))))
    {
      {
        CMclAutoLock lock(regValuesCritSec);
        tempDirectory = td;
      }
      SetCommParameters();
    }
  }

  static void    SecureSampleSubmission(bool sss)
  {
    CMclAutoLock lock(regValuesCritSec);
    secureSampleSubmission  = sss;
  }

  static void    SecureStatusQuery(bool ssq)
  {
    CMclAutoLock lock(regValuesCritSec);
    secureStatusQuery = ssq;
  }

  static void    SecureSigDownload(bool ssd)
  {
    {
      CMclAutoLock lock(regValuesCritSec);
      secureSigDownload = ssd;
    }
    SetCommParameters();
  }

  static void    SecureIgnoreHostname(bool sih)
  {
    CMclAutoLock lock(regValuesCritSec);
    secureIgnoreHostname= sih;
  }

  static std::string GatewayAddress(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return gatewayAddress;
  }

  static uint    GatewayPort(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return gatewayPort;
  }

  static uint    GatewayPortSSL(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return gatewayPortSSL;
  }

  static std::string FirewallName(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return firewallName;
  }

  static uint    FirewallPort(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return firewallPort;
  }

  static std::string FirewallUser(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return firewallUser;
  }
  static std::string FirewallPassword(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return firewallPassword;
  }

  static std::string GatewayURL(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return gatewayURL;
  }

  static std::string GatewaySSLURL(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return gatewaySSLURL;
  }
  static std::string FirewallURL(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return firewallURL;
  }

  static bool    IsFirewall(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return isFirewall;
  }

  static uint    QuarantineInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return quarantineInterval;
  }

  static uint    NewBlessedInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return newBlessedInterval;
  }

  static uint    NeededInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return neededInterval;
  }

  static uint    TimeoutInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return timeoutInterval;
  }

  static uint    RetryInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return retryInterval;
  }

  static uint    RetryLimit(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return retryLimit;
  }

  static std::string CompanyName(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return companyName;
  }

  static std::string CompanyAddress(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return companyAddress;
  }

  static std::string ContactName(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return contactName;
  }

  static std::string ContactTelephone(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return contactTelephone;
  }

  static std::string ContactEmail(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return contactEmail;
  }

  static std::string CustomerAccount(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return customerAccount;
  }

  static uint    InitialSubmissionPriority(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return initialSubmissionPriority;
  }

  static uint    MaxPendingSamples(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return maxPendingSamples;
  }

  static bool    StripUserData(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return stripUserData;
  }

  static bool    CompressContent(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return compressContent;
  }

  static bool    ScrambleContent(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return scrambleContent;
  }

  static uint    StatusCheckInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return statusCheckInterval;
  }

  static uint    RegistryCheckInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return registryCheckInterval;
  }

  static uint    TopologyCheckInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return topologyCheckInterval;
  }

  static uint    DefUnpackTimeout(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defUnpackTimeout;
  }

  static bool    AutoDefinitionDelivery(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return (defDeliveryPriority > 0);
  }

  static bool    DefBlessedBroadcast(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defBlessedBroadcast;
  }

  static std::string DefBlessedTargets(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defBlessedTargets;
  }

  static uint    DefDeliveryInterval(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defDeliveryInterval;
  }

  static uint    DefDeliveryPriority(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defDeliveryPriority;
  }

  static uint    DefDeliveryTimeout(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defDeliveryTimeout;
  }

  static bool    DefPrune(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defPrune;
  }

  static bool    DefUnblessedBroadcast(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defUnblessedBroadcast;
  }

  static bool    DefUnblessedNarrowcast(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defUnblessedNarrowcast;
  }

  static bool    DefUnblessedPointcast(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defUnblessedPointcast;
  }

  static std::string DefUnblessedTargets(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defUnblessedTargets;
  }

  static std::string DefLibraryDirectory(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return defLibraryDirectory;
  }
  
  static bool    TraceEnable(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return traceEnable;
  }

  static std::string TraceFileName(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return traceFileName;
  }

  static std::string TempDirectory(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return tempDirectory;
  }

  static bool    SecureSampleSubmission(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return secureSampleSubmission;
  }

  static  bool    SecureStatusQuery(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return secureStatusQuery;
  }

  static  bool    SecureSigDownload(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return secureSigDownload;
  }

  static  bool    SecureIgnoreHostname(void)
  {
    CMclAutoLock lock(regValuesCritSec);
    return secureIgnoreHostname;
  }

  /*
   * Also implemented elsewhere.
   */
  static void    SetCommParameters();

  static void    FinishedInitializing()
  {
    initializing = false;
    SetCommParameters();
  }

  static void    Stop();

              //
              //  sometimes, based on repeated errors, IcePack
              //  will decide not to attempt to download a
              //  particular sequence number.  These two methods
              //  facilitate that behavior.
  static void    AddToBadSeqNums(uint seqNum)
  {
    char  buffer[96];
    sprintf(buffer,
            "Adding %08d to the set of definitions sets to never download",
            seqNum);
    Logger::Log(Logger::LogError, Logger::LogIcePackSigDownload, buffer);
    {
      CMclAutoLock lock(regValuesCritSec);
      badSeqNums.insert(seqNum);
    }
  }

  static bool    IsOkaySeqNum(uint seqNum)
  {
    bool  okaySeqNum  = false;
    {
      CMclAutoLock lock(regValuesCritSec);
      okaySeqNum = badSeqNums.end() == badSeqNums.find(seqNum);
    }
    if (!okaySeqNum)
    {
      char  buffer[96];

      sprintf(buffer,
              "IsOkaySeqNum(%08d) returning false, this definition set should NOT be downloaded",
              seqNum);
      Logger::Log(Logger::LogError, Logger::LogIcePackSigDownload, buffer);
    }
    return okaySeqNum;
  }

private:
  static CMclCritSec regValuesCritSec;
  //
  //    Web Communications objects
  static std::string gatewayAddress;
  static uint        gatewayPort;
  static uint        gatewayPortSSL;
  static std::string firewallName;
  static uint        firewallPort;
  static std::string firewallUser;
  static std::string firewallPassword;
  static uint        timeoutInterval;
  static uint        retryInterval;
  static uint        retryLimit;

  static std::string gatewayURL;
  static std::string gatewaySSLURL;
  static std::string firewallURL;
  static bool        isFirewall;

  static bool        secureSampleSubmission;
  static bool        secureStatusQuery;
  static bool        secureSigDownload;
  static bool        secureIgnoreHostname;

  //
  //    Customer Information
  static std::string companyName;
  static std::string companyAddress;
  static std::string contactName;
  static std::string contactTelephone;
  static std::string contactEmail;
  static std::string customerAccount;

  //
  //    Sample Policy
  static uint        initialSubmissionPriority;
  static uint        maxPendingSamples;
  static bool        stripUserData;
  static bool        compressContent;
  static bool        scrambleContent;
  static uint        quarantineInterval;    // milliseconds
  static uint        newBlessedInterval;    // milliseconds
  static uint        neededInterval;        // milliseconds
  static uint        statusCheckInterval;   // milliseconds
  static uint        registryCheckInterval; // milliseconds
  static uint        topologyCheckInterval; // milliseconds
  static uint        defUnpackTimeout;      // milliseconds

  static bool        defBlessedBroadcast;
  static std::string defBlessedTargets;
  static uint        defDeliveryInterval;  // milliseconds
  static uint        defDeliveryPriority;
  static uint        defDeliveryTimeout;   // milliseconds
  static bool        defPrune;
  static bool        defUnblessedBroadcast;
  static bool        defUnblessedNarrowcast;
  static bool        defUnblessedPointcast;
  static std::string defUnblessedTargets;
  static std::string defLibraryDirectory;

  static bool        traceEnable;
  static std::string traceFileName;      // includes full path

  static std::string tempDirectory;

  static bool         initializing;

  static std::set<uint>  badSeqNums;

private:          // this is a singleton class
  GlobalData();
  virtual ~GlobalData();

  static uint    newestSig;            // most recent sig successfully downloaded
  static bool    newestSigBlessed;     // is the most recent version blessed?
  static uint    newestBlessedSig;     // most recent sig that was blessed

  static uint    specdefToDownload;    // specdef sequence waiting to be downloaded
  static uint    blessedToDownload;    // blessed sequence waiting to be downloaded.

public:
  static const char *latestInstalledSigKey;
  static const char *latestInstalledSigIsBlessedKey;
  static const char *definitionDownloadNextKey;
  static const char *definitionBlessedSequenceKey;
  static const char *definitionDownloadNextBlessedKey;
  static const char *regSymantecRoot;
  static const char *regCurrentRoot;
  static const char *regUpdateRoot;

  enum { maxSleepInterval = 5000 };   // 5 seconds

  static bool SleepInSegments(uint sleepTimeInMilliSeconds)
  {
    uint  sleep       = 0;
    uint  totalSlept  = 0;
    char msg[300];

    sprintf(msg, "About to SleepInSegments() for %d.%03ds", (sleepTimeInMilliSeconds / 1000), (sleepTimeInMilliSeconds % 1000));
    Logger::Log(Logger::LogDebug, Logger::LogIcePackOther, msg);

    while (!GlobalData::stop && totalSlept < sleepTimeInMilliSeconds)
    {
      Sleep(maxSleepInterval);
      totalSlept  += maxSleepInterval;
    }

    sprintf(msg, "Slept for %d.%03ds", (totalSlept / 1000), (totalSlept % 1000));

    if (GlobalData::stop)
      strcat(msg, " [global stop message received]");

    Logger::Log(Logger::LogDebug, Logger::LogIcePackOther, msg);

    return ((totalSlept >= sleepTimeInMilliSeconds) && (!GlobalData::stop));
  }

  static bool SleepInSegments(uint (*IntervalFunc)(),
                              const Logger::LogSource source,
                              const char* whyTheNap);
  static void HandleException(const char* where, const Logger::LogSource source);
  static bool ExceptionTyper(std::string& typeInfo, std::string& details);
};

#endif /* !defined(AFX_GLOBALDATA_H__E4E7A0A1_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_) */
