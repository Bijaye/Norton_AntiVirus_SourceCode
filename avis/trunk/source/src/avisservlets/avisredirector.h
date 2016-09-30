// avisredirector.h - Prototype for the AvisRedirector class

#if !defined (__AVISREDIRECTOR_H__)
#define __AVISREDIRECTOR_H__


#include "goservletclient.h"

//: Handler for all redirection transactions passed to the servlets.
//  There are currently two transactions that are redirected within
//  the Immune System Gateway Array from the customer entrypoint
//  machine to other machines within the array.  These transactions
//  are configured from within the webserver to use the
//  redirectSignatureSet and redirectSuspectSample handlers (defined
//  in avisservlets.h) both of which use the AvisRedirector class as
//  their handler.  The AvisRedirector accesses a method within a
//  RedirectorState to get the next server to redirect the client
//  transaction to. (see RedirectorState for more information.)
//  Depending on the value of the "REDIRECTOR_TYPE" parameter to the
//  AvisRedirector constructor (which is determined from the method
//  creating the handler object) it will call either
//  "get_definition_server()" or "get_sample_server()" and send back a
//  simple HTTP redirect message with that target specified in the
//  "Location: " header.
//  If the incoming transaction is an SSL transaction, the response
//  will be the SSL equivalent.

class AvisRedirector : virtual public GoServletClient
{
public:

  //: Simple enum indicating transaction type to be redirected.
  enum REDIRECTOR_TYPE
  {
    RT_INVALID            = 0,
    RT_SIGNATURE_DOWNLOAD = 1,
    RT_SAMPLE_SUBMISSION  = 2
  };

  //
  //: Constructor...  Note that this differs from the other
  //: transaction constructors.
  //  See the class documentation for more information about the
  //  REDIRECTOR_TYPE parameter.
  //
  AvisRedirector(GoServletHost&     host_,
                 const char*        gwapihandle_,
                 REDIRECTOR_TYPE type_);

  //
  //: Destructor
  //

  virtual ~AvisRedirector();

protected:

  //
  //: set required headers for this transaction (there are none for
  //: AvisRedirector)
  // 

  virtual void setRequiredHeaders();

  //
  //: This method is deprecated
  //

  virtual int verifyHeaderContent();

  //
  //: Top level logic wrapper.
  //
  // Based upon the value of the REDIRECTOR_TYPE parameter passed into
  // the constructor, this method builds a fully-qualified response
  // URL by calling get_next_server to pull the IP name, then
  // appending the same port and relative URL information used to call
  // this transaction, so as to effectively reroute the same request
  // to another IP address.
  //
  
  virtual int fulfillRequest() throw();


private:
  
  //
  //: Retrieve the redirection target IP name from the RedirectorState
  //: object based on the value of the REDIRECTOR_TYPE parameter.
  //
  // NOTE: This method should be "const" 
  //

  std::string get_next_server(REDIRECTOR_TYPE);

  //
  //: Is this transaction a getSignatureSet or postSuspectSample
  //: redirection.
  //

  REDIRECTOR_TYPE _type;

  //
  //: Standard Tracing object
  //

  TraceClient* _tracer;

  //
  //: Intentionally unimplemented default constructor
  //
 
  AvisRedirector();

  //
  //: Intentionally unimplemented const copy constructor
  //

  AvisRedirector(const AvisRedirector&);

  //
  //: Intentionally unimplemented assignment operator
  //

  AvisRedirector& operator =(const AvisRedirector&);

};

#endif // __AVISREDIRECTOR_H__
