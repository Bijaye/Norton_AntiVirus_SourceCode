// SampleBase.h: interface for the SampleBase class.
//
//////////////////////////////////////////////////////////////////////

//
//  This class contains the parts of the sample class that are
//  independent of the source of the sample data (quarantine server
//  in the real version and the Access database in the test version)
//

#if !defined(AFX_SAMPLEBASE_H__6F37300F_024E_11D3_ADD5_00A0C9C71BBC__INCLUDED_)
#define AFX_SAMPLEBASE_H__6F37300F_024E_11D3_ADD5_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include <DateTime.h>
#include "NAVScan.h"
#include <qserver.h>

#include "IcePackException.h"
#include "SampleStatus.h"

bool isThisAttributePseudoDeleted(VARIANT &value);

class SampleBase  
{
public:
  SampleBase();
  SampleBase(const SampleBase& cpy);
  virtual ~SampleBase();

  SampleBase& operator=(const SampleBase& cpy);

  bool    Open(void);
  bool    Size(ulong& size);
  bool    Read(ulong inSize, ulong& outSize, char* buffer);
  bool    Close(void);

  bool    GetAttribute(const char* key, std::string& value);
  bool    GetAttribute(const std::string& key, std::string& value)
              { return GetAttribute(key.c_str(), value); }
  bool    GetAllAttributes(std::string& value);

  bool    preserveAttributes();

  virtual bool SetAttribute(const char* key, const char* value);
  bool    SetAttribute(const std::string& key, const std::string& value)
              { return SetAttribute(key.c_str(), value.c_str()); }
  bool    SetAttribute(const std::string& key, const char* value)
              { return SetAttribute(key.c_str(), value); }

  bool    Commit(void);

  bool    ID(IQuarantineServerItem* item);
  IQuarantineServerItem*  ID(void);

  ulong   SampleKey() { return itemKey.ulVal; }
  bool    Key(ulong key);

  virtual void  Reset() = 0;

  bool  IsNull(void) const  { return 0 == itemKey.lVal; }

  static std::string  GenErrorMsg(HRESULT hr, const char *where, ulong id);

  void    DeActivate(void);

protected:
  bool          active;
  IQuarantineServerItem *id;
  _variant_t        itemKey;
  uint          priority;
  DateTime        forwardTime;
  std::string       gatewayCookie;
  SampleStatus::Status  status;
  bool          finalStatus;
  uint          sigSeqNum;
  std::string       attributes;   // all attributes (minus http com attrs)
  ulong         changes;

  HANDLE          fileHandle;

  enum { InvalidID = NULL };

  /*
   * Hard as it may be to imagine, we can actually get
   * down here when there's no data in this Sample
   * object.  I know, I wouldn't have done it this
   * way either, but there it is.  BoundsChecker
   * complains on the old code in that case -- the old
   * code was:
   *  ulong DebugKey(void) const  { return itemKey.ulVal; };
   * the _variant_t (I hate these stupid OLE types)
   * that is 'itemKey' will be empty, and the old code
   * didn't check.
   * inw 2000-06-17
   */
  ulong DebugKey(void) const  { return ((VT_EMPTY == itemKey.vt) ? 0 : itemKey.ulVal); };

private:
  ICopyItemData*  copyItemData;
//  CMclCritSec   inUse;

  IQuarantineServer*  GetIQS() const;
  ICopyItemData*    GetICopyItemData();

  void ConvertToBSTR(VARIANT& value);
};

class CompSampleByKeyOnly
{
public:
  CompSampleByKeyOnly(ulong key_) : key(key_) {};

  void Key(ulong key_) { key = key_; }

  bool operator()(SampleBase& sample);

private:
  ulong key;
};

#endif // !defined(AFX_SAMPLEBASE_H__6F37300F_024E_11D3_ADD5_00A0C9C71BBC__INCLUDED_)
