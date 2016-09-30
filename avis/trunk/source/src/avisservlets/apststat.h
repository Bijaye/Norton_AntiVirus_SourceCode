// apststat - Prototype of the AvisPostStatus class
#if !defined (__APSTSTAT_H__)
#define __APSTSTAT_H__

#include "goservletclient.h"


//
//: handler for the postSampleStatus transaction
//
// getSuspectSample is one of the three transactions that are only
// ever executed from within the Analysis Center (and the web server
// is configured to prevent them from being accessed from anywhere
// else.) 
//
// AvisPostStatus is the transaction used by the Analysis Center to
// update the status of a sample in a Gateway database.
//
// Given two required attributes (X-Sample-Checksum and
// X-Analysis-State) the transaction handler looks in the database for
// a sample matching that checksum.  If it is present, the status is
// updated.
//
// Additionally, if the headers "X-Date-Analyzed" and
// "X-Signatures-Sequence" are included, the state is considered to
// be "final" and an entry is created in the AnalysisResults table in
// the database to reflect this.  Further GetSampleStatus transactions
// using this checksum will yield both of these additional fields.
//
// Note: If "final" status is posted for a sample that does NOT exist
// in the database, a row IS created in the AnalysisResults table.
// This allows the AnalysisCenter to populate the database with a list
// of checksums with a known result.  
//

class AvisPostStatus : virtual public GoServletClient
{
public:
  
  //
  //: Constructor
  //

  AvisPostStatus(GoServletHost& host_,const char* gwapihandle_);

  //
  //: Virtual Destructor
  //

  virtual ~AvisPostStatus();

protected:

  //
  //: Primary logic driver method, called by the base class
  //

  virtual int fulfillRequest();

private:

  //
  //: Standard Trace object
  //

  TraceClient* _tracer;

  //
  //: Intentionally unimplemented default constructor
  //

  AvisPostStatus();

  //
  //: Intentionally unimplemented const copy constructor
  //

  AvisPostStatus(const AvisPostStatus&);

  //
  //: Intentionally unimplemented assignment operator
  //

  AvisPostStatus& operator =(const AvisPostStatus&);
};
#endif // __APSTSTAT_H__
