// goservletclient.h - Prototype of the GoServletClient class
#if !defined (__GOSERVLETCLIENT_H__)
#define __GOSERVLETCLIENT_H__

class GoServletHost;
class TraceClient;

#include "gosession.h"
#include "sessionclient.h"

#include <list>
#include <string>

class SignatureCache;
class RedirectorState;
class DBConnectionPool;

//
//: GoServletClient Baseclass for the AvisServlets transaction handlers
//
// The GoServletClient class provides the summation of the entire
// framework in a base class for individual "servlet" transaction
// handler classes.  It's primary purpose it to provide an interface
// point for all resources managed elsewhere (within the process or
// otherwise)
//
// See the SessionClient and SessionHost documentation for more
// information on the interaction between GoServletHost and
// GoServletClient.

//

class GoServletClient :  public SessionClient
{
public:

  //
  //: Object Constructor
  //

  GoServletClient(GoServletHost& host_,const char* gwapihandle_);

  //
  //: Virtual Destructor
  //

  virtual ~GoServletClient();

  //
  //: Provides the hook for pre-testing the transaction request
  //

  virtual int run();

protected:

  //
  //: Pure virtual business logic driver
  //

  virtual int fulfillRequest() = 0;

  //
  //: Add a header to the list of required headers
  //

  void addRequiredHeader(const std::string& header_);

  //
  //: API Proxy to (and from) webserver
  //

  GoSession _gwapi;

  //
  //: A reference to the SignatureCache
  //

  SignatureCache& _sig_cache;

  //
  //: Reference to the "RedirectorState" object
  //

  RedirectorState& _redirector;

  //
  //: Reference to the DBConnectionPool;
  //
  //

  DBConnectionPool& _connection_pool;

private:

  //
  //: Standard Tracing object
  //

  TraceClient* _tracer;

  //
  //: validate incoming transaction
  //
  // Checks to see that the incoming headers are all present.  If it
  // finds one that is missing, it is returned.  If everything is ok,
  // the returned string is empty.  This is executed from run, before
  // fulfill request.
  //

  bool verifyHeaders() const;

  //
  //: List of required headers
  //

  std::list<std::string> _requiredHeaders;


private:

  //
  //: Intentionally unimplemented default constructor
  //

  GoServletClient();

  //
  //: Intentionally unimplemented const copy constructor
  //

  GoServletClient(const GoServletClient&);

  //
  //: Intentionally unimplemented assignment operator
  //

  GoServletClient& operator =(const GoServletClient&) ;

};
#endif // __GOSERVLETCLIENT_H__

