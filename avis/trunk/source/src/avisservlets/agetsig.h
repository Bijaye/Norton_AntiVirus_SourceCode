// agetsig.h - Prototype for the AvisGetSignature class

#if !defined (__AGETSIG_H__)
#define __AGETSIG_H__

#include "goservletclient.h"

class Signature;


//
//: Handler for both the getSignatureSet and headSignatureSet transactions.
//
//  (The headSignatureSet transaction is defined only in the web
//  server configuration file, there is not a separate callback method
//  exported from the dll to handle it.

class AvisGetSignature : virtual public GoServletClient
{
public:

  //
  //: Object Constructor
  //

  AvisGetSignature(GoServletHost& host_,const char* gwapihandle_);
  
  //
  //: Virtual Destructor
  //
  
  virtual ~AvisGetSignature();
protected:

  //
  //: set minimal list of headers required for completion of this
  //: transaction.
  //
  // This is called by GoServletClient.
  //

  virtual void setRequiredHeaders();
  
  //
  //: Verify the incoming transaction headers.  This is deprecated
  //
  
  virtual int  verifyHeaderContent();

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

  virtual int  fulfillRequest();

  //
  //: Business logic
  //

  int liveProcessing();
  
private:

  //
  //: Standard trace object
  //

  TraceClient* _tracer;

  //
  //: Intentionally unimplemented default constructor
  //

  AvisGetSignature();

  //
  //: Intentionally unimplemented const copy constructor
  //

  AvisGetSignature(const AvisGetSignature&);

  //
  //: Intentionally unimplemented assignment operator
  //

  AvisGetSignature& operator =(const AvisGetSignature&);

};
#endif // __AGETSIG_H__
