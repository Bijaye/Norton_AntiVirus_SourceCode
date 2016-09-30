// Sample.h: interface for the Sample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLE_H__29AECADD_C66F_11D2_AD5A_00A0C9C71BBC__INCLUDED_)
#define AFX_SAMPLE_H__29AECADD_C66F_11D2_AD5A_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "SampleBase.h"
class MyTraceClient; //#include <MyTraceClient.h>
#include <IcePackAgentComm.h>

#include <DateTime.h>
#include "NAVScan.h"

class Sample : public SampleBase
{
public:
  Sample() : SampleBase(), action(sUnknown), neededSeqNum(0), sigPriority(0) { };
  virtual ~Sample();

  Sample(const Sample& cpy) : SampleBase(cpy), action(cpy.action),
                neededSeqNum(cpy.neededSeqNum),
                sigPriority(cpy.sigPriority)
                {};


  //
  //  Access methods
//  bool    IsNull(void)    { return id == InvalidID; }
//  uint    ID(void)      { NullCheck(IcePackException::SampleID);    return id; }
  uint    Priority(void)    { NullCheck(IcePackException::SamplePriority);  return priority; }
  uint    SigPriority(void) { NullCheck(IcePackException::SampleSigPriority);return sigPriority; }
  DateTime  ForwardTime(void) { NullCheck(IcePackException::SampleForwardTime); return forwardTime; }
  std::string GatewayCookie(void) { NullCheck(IcePackException::SampleGatewayCookie); return gatewayCookie; }
  SampleStatus::Status  Status(void)    { NullCheck(IcePackException::SampleStatus);  return status; }
  bool    FinalStatus(void) { NullCheck(IcePackException::SampleFinalStatus); return finalStatus; }
//  bool    Infected(void)    { NullCheck(IcePackException::SampleInfected);  return infected; }
  uint    SigSeqNum(void)   { NullCheck(IcePackException::SampleSigSeqNum); return sigSeqNum; }
  uint    NeededSeqNum(void)  { NullCheck(IcePackException::SampleNeededSeqNum); return neededSeqNum; }
  ulong   Changes(void)   { NullCheck(IcePackException::SampleChanges); return changes; }

  bool  GetAttribute(const std::string& key, std::string& value)
              { return SampleBase::GetAttribute(key.c_str(), value); }
  bool  SetAttribute(const std::string& key, const std::string& value)
              { return SetAttribute(key.c_str(), value.c_str()); }
  bool  SetAttribute(const std::string& key, const char* value)
              { return SetAttribute(key.c_str(), value); }
  bool  SetAttribute(const char* key, const char* value);

  NAVScan::ScanRC Scan(uint sigSeqNum, bool isThisTheInitialScan);

  //
  //  Modification methods
  void    Priority(uint newPriority);
  void    SigPriority(uint sigPriority);
  bool    SetID(ulong sID);

  SampleStatus::Status  Status(SampleStatus::Status s);



  //
  //  Operators
  int Compare(const Sample& rhs) const
        {
          NullCheck(IcePackException::SampleCompare);
          if (priority > rhs.priority) return -1;
          if (priority < rhs.priority) return  1;
          int rc = forwardTime.compare(rhs.forwardTime);
          if (0 != rc) return rc;
//          if (id > rhs.id) return -1;
//          if (id < rhs.id) return 1;
          if (DebugKey() > rhs.DebugKey()) return -1;
          if (DebugKey() < rhs.DebugKey()) return 1;
          return 0;
        }
/*
  bool operator<(Sample& rhs) { return -1 == Compare(rhs); }
  bool operator>(Sample& rhs) { return  1 == Compare(rhs); }
  bool operator==(Sample& rhs){ return  0 == Compare(rhs); }
  bool operator<=(Sample& rhs){ return  1 != Compare(rhs); }
  bool operator>=(Sample& rhs){ return -1 != Compare(rhs); }
*/
/*
  bool operator<(Sample rhs) { return -1 == Compare(rhs); }
  bool operator>(Sample rhs) { return  1 == Compare(rhs); }
  bool operator==(Sample rhs){ return  0 == Compare(rhs); }
  bool operator<=(Sample rhs){ return  1 != Compare(rhs); }
  bool operator>=(Sample rhs){ return -1 != Compare(rhs); }
*/
  bool operator<(const Sample& rhs)  const { return -1 == Compare(rhs); }
  bool operator>(const Sample& rhs)  const { return  1 == Compare(rhs); }
  bool operator==(const Sample& rhs) const { return  0 == Compare(rhs); }
  bool operator<=(const Sample& rhs) const { return  1 != Compare(rhs); }
  bool operator>=(const Sample& rhs) const { return -1 != Compare(rhs); }

  Sample& operator=(const Sample& cpy);

  //
  //  Takes a string of headers and updates the Sample object based
  //  on those headers.
  //  Returns true if the sample object changed, false if it did
  //  not change.
  bool  Update(std::string& attrs);


  //
  //  Mark
  enum Action { sDelete, sAdd, sChanged, sUnchanged, sUnknown };

  Action  Mark(void)      { return action; }
  void  Mark(Action act)  { action = act; }


  void  Reset(void);

protected:
  Action    action;
  uint    neededSeqNum;
  uint    sigPriority;

  void  NullCheck(IcePackException::TypeOfException type) const
  {
    if (IsNull())
      throw IcePackException(type, "Sample is null");
  }

  bool    ParseSpecialAttributes(const std::string& attrs);
  void    Clear();
};


#endif // !defined(AFX_SAMPLE_H__29AECADD_C66F_11D2_AD5A_00A0C9C71BBC__INCLUDED_)
