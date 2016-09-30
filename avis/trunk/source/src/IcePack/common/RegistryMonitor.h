// RegistryMonitor.h: interface for the RegistryMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRYMONITOR_H__5B816B6D_CB2B_11D2_AD61_00A0C9C71BBC__INCLUDED_)
#define AFX_REGISTRYMONITOR_H__5B816B6D_CB2B_11D2_AD61_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>
#include <CMclThread.h>
#include <WinRegistry.h>

enum DType { dtString, dtInt, dtDir };

class RegEntries
{
public:
  RegEntries (const char* keyName_, int dDefault_, int dMin_, int dMax_, int dontCopy_) :
    keyName(keyName_), type(dtInt), dDefault(dDefault_),
    dMin(dMin_), dMax(dMax_), sDefault(NULL), dontCopy(dontCopy_)
  {};

  RegEntries (const char* keyName_, int dDefault_, int dMin_, int dMax_) :
    keyName(keyName_), type(dtInt), dDefault(dDefault_),
    dMin(dMin_), dMax(dMax_), sDefault(NULL), dontCopy(0)
  {};

  RegEntries(const char* keyName_, const char* sDefault_) :
    keyName(keyName_), type(dtString), dDefault(0), dMin(0), dMax(0),
    sDefault(sDefault_), dontCopy(0)
  {};

  RegEntries(const char* keyName_, DType type_, const char* dirDefault) :
    keyName(keyName_), type(type_), dDefault(0), dMin(0), dMax(0),
    sDefault(dirDefault), dontCopy(0)
  {};

  const char* keyName;
  const DType type;
  const int dDefault;
  const int dMin;
  const int dMax;
  const char* sDefault;
  const int dontCopy;
};

class RegistryMonitor   : public CMclThreadHandler
{
private:


public:
  RegistryMonitor();
  virtual ~RegistryMonitor();

  unsigned ThreadHandlerProc(void);

  static void InitLogging(void);
  static void DeInitLogging(void);

  static bool CreateKeys(WinRegistry &sRoot);
  static bool InitializeRegistry(void);
  static bool DeInitializeRegistry(void);

  static void LogAllConfigValues(void);

private:
  uint  interval;
  uint  changeCount;

  static bool InitDWORDValue(WinRegistry& uRoot, WinRegistry& cRoot, const int dontCopy, const TCHAR* key,
              DWORD& value, DWORD defaultValue, DWORD min, DWORD max);
  static bool InitStrValue(WinRegistry& uRoot, WinRegistry& cRoot, const int dontCopy, const TCHAR* key,
              TCHAR* value, const TCHAR* defaultValue);
  static bool InitDirValue(WinRegistry& uRoot, WinRegistry& cRoot, const int dontCopy, const TCHAR* key,
              TCHAR* value, const TCHAR* defaultValue);


  static bool InitValue(WinRegistry& uRoot, WinRegistry& cRoot, RegEntries& regEntry, DWORD& value)
  {
    return InitDWORDValue(uRoot, cRoot, regEntry.dontCopy, regEntry.keyName, value,
                  regEntry.dDefault, regEntry.dMin, regEntry.dMax);
  }
  static bool InitValue(WinRegistry& uRoot, WinRegistry& cRoot, RegEntries& regEntry, TCHAR* value)
  {
    return InitStrValue(uRoot, cRoot, regEntry.dontCopy, regEntry.keyName, value,
                  regEntry.sDefault);
  }
  static bool InitDirValue(WinRegistry& uRoot, WinRegistry& cRoot, RegEntries& regEntry, TCHAR* value)
  {
    return InitDirValue(uRoot, cRoot, regEntry.dontCopy, regEntry.keyName, value,
                  regEntry.sDefault);
  }

  bool  CheckForChanges(void);    // return true if a change occured

          // 
          // value returns the new value, if changed
          // returns true if changed.
  bool  CheckForStrChange(WinRegistry& update, WinRegistry& current, const TCHAR* key, TCHAR* value);
  bool  CheckForDirChange(WinRegistry& update, WinRegistry& current, const TCHAR* key, TCHAR* value);
  bool  CheckForDWORDChange(WinRegistry& update, WinRegistry& current, const TCHAR* key, DWORD& value,
                const DWORD minValue, const DWORD maxValue);

  bool  CheckForChange(WinRegistry& update, WinRegistry& current, RegEntries& regEntry, TCHAR* value)
  {
    return CheckForStrChange(update, current, regEntry.keyName, value);
  }
  bool  CheckForDirChange(WinRegistry& update, WinRegistry& current, RegEntries& regEntry, TCHAR* value)
  {
    return CheckForDirChange(update, current, regEntry.keyName, value);
  }
  bool  CheckForChange(WinRegistry& update, WinRegistry& current, RegEntries& regEntry, DWORD& value)
  {
    return CheckForDWORDChange(update, current, regEntry.keyName, value, regEntry.dMin, regEntry.dMax);
  }


  enum  { MaxStrLength  = 256 };

  //
  //  Local copy of registry values

  static  DWORD billification;

  static  TCHAR webGatewayName[MaxStrLength];
  static  DWORD webGatewayPort;
  static  DWORD webGatewayPortSSL;
  static  TCHAR webFirewallName[MaxStrLength];
  static  DWORD webFirewallPort;
  static  TCHAR webFirewallUser[MaxStrLength];
  static  TCHAR webFirewallPassword[MaxStrLength];
  static  DWORD webSecureSampleSubmission;
  static  DWORD webSecureStatusQuery;
  static  DWORD webSecureSignatureDownload;
  static  DWORD webSecureIgnoreHostname;
  static  DWORD webStatusInterval;
  static  DWORD webBlessedInterval;
  static  DWORD webNeededInterval;
  static  DWORD webTimeoutInterval;
  static  DWORD webRetryInterval;
  static  DWORD webRetryLimit;
  
  static  TCHAR customerCompanyName[MaxStrLength];
  static  TCHAR customerCompanyAddress[MaxStrLength];
  static  TCHAR customerContactName[MaxStrLength];
  static  TCHAR customerContactTelephone[MaxStrLength];
  static  TCHAR customerContactEmail[MaxStrLength];
  static  TCHAR customerAccount[MaxStrLength];

  static  DWORD sampleQuarantineInterval;
  static  DWORD sampleSubmissionPriority;
  static  DWORD sampleMaximumPending;
  static  DWORD sampleStripContent;
  static  DWORD sampleCompressContent;
  static  DWORD sampleScrambleContent;

  static  DWORD definitionBlessedBroadcast;
  static  TCHAR definitionBlessedTargets[MaxStrLength];
  static  DWORD definitionDeliveryInterval;
  static  DWORD definitionDeliveryPriority;
  static  DWORD definitionDeliveryTimeout;
#if 0 /* inw 2000-02-08 -- ejp says this is deprecated. */
  static  DWORD definitionCheckInterval;
#endif /* 0 */
  static  TCHAR definitionLibraryDirectory[MaxStrLength];
  static  DWORD definitionPrune;
  static  DWORD definitionUnblessedBroadcast;
  static  DWORD definitionUnblessedNarrowcast;
  static  DWORD definitionUnblessedPointcast;
  static  TCHAR definitionUnblessedTargets[MaxStrLength];
  static  DWORD definitionUnpackTimeout;

  static  DWORD configurationChangeCounter;
  static  DWORD configurationChangeInterval;

  static  DWORD traceEnable;
  static  TCHAR traceFilename[MaxStrLength];
  static  TCHAR traceSeverity[MaxStrLength];
  static  DWORD traceFilter;

  static  TCHAR temporaryDirectory[MaxStrLength];

};


class LogLife
{
public:
  LogLife(void) { RegistryMonitor::InitLogging(); };
  ~LogLife()    { RegistryMonitor::DeInitLogging(); }
};
#endif // !defined(AFX_REGISTRYMONITOR_H__5B816B6D_CB2B_11D2_AD61_00A0C9C71BBC__INCLUDED_)
