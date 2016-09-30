// agetsamp.h - Prototype for the AvisGetSample class
#if !defined (__AGETSAMP_H__)
#define __AGETSAMP_H__

#include "goservletclient.h"

//
//: handler for the getSuspectSample transaction
//
// getSuspectSample is one of the three transactions that are only
// ever executed from within the Analysis Center (and the web server
// is configured to prevent them from being accessed from anywhere
// else.) 
//
// AvisGetSample is the transaction used by the Analysis Center to
// retrieve suspect samples from the immune systme gateway(s).  This
// transaction merely queries the database for the "first" sample
// pending import into the analysis center, and sends the content and
// attributes as a response.  In the event that there are no samples
// pending import, the gateway returns nothing.
//
// Note that this transaction does not affect the state of a sample
// being imported into the analysis center, but relies instead on the
// postSampleStatus mechanism to "remove a sample from the import
// queue".


class AvisGetSample : virtual public GoServletClient
{
public:

  //
  //: Constructor
  //

  AvisGetSample(GoServletHost& host_,const char* gwapihandle_);

  //
  //: Virtual Destructor
  //

  virtual ~AvisGetSample();

protected:
  
  //
  //: Load Sample Content
  // Given a sample checksum, retrieve the content file into this
  // object's content buffer for dispatching.  That content file is
  // named "<checksum>.sample" and exists in the directory referenced
  // by the "sampleDirectory" parameter in the avisservlets.prf
  // configuration file.
  //
  
  void loadSampleContent    ( const std::string& checksum_);

  //
  //: Load the block of sample attributes.
  //
  // Given a smaple checksum, retrieve the attributes table associated
  // with this sample into the _sample_attributes container.  These
  // attributes will be sent along with the content to the analysis
  // center in the HTTP response message.
  //
  // The attributes file is named "<checksum>.attributes" and also
  // exists in the directory referenced by the "sampleDirectory"
  // parameter in the avisservlets.prf configuration file.
  //

  void loadSampleAttributes ( const std::string& checksum_);

  //
  //: Assign required header list for this transaction (there are none)
  //

  virtual void setRequiredHeaders();

  //
  //: Verify content of headers (deprecated)
  //
  virtual int verifyHeaderContent();

  //
  // Top level logic wrapper.  This drives the transaction execution
  // and is called by the parent class.
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


  virtual int fulfillRequest();


private:


  bool liveProcessing() throw();


  //
  //: standard Trace object
  //

  TraceClient* _tracer;

  //
  //: Attributes from file
  //
  // This is the block of attributes loaded from the
  // loadSampleAttributes method
  //

  MStringString _sample_attributes;

  //
  //: full sample content buffer
  //
  // Buffer holding the contents of the <checksum>.sample file loaded
  // in the loadSampleContent method.
  //

  char* _contentBuffer;

  //
  //: Length of the content buffer
  //

  long  _bufferLength;
  
private:

  //
  //: Intentionally unimplemented default constructor
  //
  
  AvisGetSample();

  //
  //: Intentionally unimplemented const copy constructor
  //

  AvisGetSample(const AvisGetSample&);
  
  //
  //: Intentionally unimplemented assignment operator
  //

  AvisGetSample& operator =(const AvisGetSample&);

};
#endif // __AGETSAMP_H__ Sentinel;
