// agetstat.h - Prototype for the AvisGetStatus class

#if !defined (__AGETSTAT_H__)
#define __AGETSTAT_H__


#include "goservletclient.h"

//: handler for getSampleStatus transaction
// AvisGetStatus is the simplest of all transactions.  Based on an
// X-Sample-Checksum attribute passed by the client, a couple simple
// database lookups are performed to populate at most three response
// headers to the client. (Those being X-Analysis-State which
// describes the current state of the sample or the final outcome of
// analysis if appropriate, X-Date-Analyzed indiciating that the
// sample has completed processing and that this is the final status,
// and X-Signatures-Sequence indicating that the definition package
// that must be retrieved by the client to handle this sample
// effectively.)

class AvisGetStatus : virtual public GoServletClient
{
public:

  //
  //: Object constructor
  //

  AvisGetStatus(GoServletHost& host_,const char* gwapihandle_);
  
  //
  //: Object Destructor
  //
  
  virtual ~AvisGetStatus();

protected:

  //
  //: Registration of headers required for processing of this
  //: transaction.
  //  Note: This method is deprecated.  The client may supply one or
  //  both of the X-Sample-Checksum or X-Analysis-Cookie.
  //

  virtual void setRequiredHeaders();

  //
  //: Envelope around liveProcessing business logic
  //
  // fulfillRequest is a method called by the parent class,
  // GoServletClient to actually execute the transaction
  // handler. Originally the plan was to use it as a try{} catch{}
  // block around some other function (namely liveProcessing) to trap
  // any outstanding errors before the transaction completed, to allow 
  // it to finish gracefully.  This logic now happens in the
  // GoServletClass::run() method, which serves this purpose quite
  // nicely.  However, we are past the "make no non-critical changes"
  // line in the development cycle, so this is staying in.
  //

  virtual int fulfillRequest() throw();

private:

  //
  //: Trace object
  //

  TraceClient* _tracer;

  //
  //: Intentionally unimplemented default constructor
  //

  AvisGetStatus();

  //
  //: Intentionally unimplemented const-copy constructor
  //

  AvisGetStatus(const AvisGetStatus&);

  //
  //: Intentionally unimplemented assignment operator
  //

  AvisGetStatus& operator =(const AvisGetStatus&);
};

#endif // __ASUBSAMP_H__
