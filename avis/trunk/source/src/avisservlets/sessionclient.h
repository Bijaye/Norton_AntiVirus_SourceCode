// sessionclient.h - Prototype for the sessionclient class
#if !defined (__SESSIONCLIENT_H__)
#define __SESSIONCLIENT_H__


#include "stringmap.h"
class SessionHost;
class TraceClient;
class MTrace;


//
//: SessionClient represents a consumer of resources initialized in a
//: SessionHost class
//
// The SessionClient is the top level in a class hierarchy that
// matches 1:1 the SessionHost hierarchy.  These two classes loosely
// represent a "producer consumer" pair.  The SessionClient is
// constructed with a reference to a SessionHost object.  It then
// grabs references to the resources managed by the SessionHost, and
// provides them in protected space for use by the child classes.
//

class SessionClient
{
public:

  //
  //: explicit constructor
  //
  // Called with a reference to an exisitng SessionHost 0bject
  //

  explicit SessionClient(SessionHost& host_);

  //
  //: Virtual destructor
  //

  virtual ~SessionClient();
protected:
  

  //
  //: Environment string table reference
  //
  // This is initialized from the similarly named member in the
  // SessionHost reference passed into the constructor, and is used
  // only as an access.
  //

  MStringString& _environment;


  //
  //: Environment string table reference
  //
  // This is initialized from the similarly named member in the
  // SessionHost reference passed into the constructor, and is used
  // only as an access.
  //

  MTrace&        _trace_server;

private:

  //
  //: Standard Trace Object
  //
  // In a class hierarchy the _tracer is private so that child classes
  // can define their own tracer that will be hide that
  // of the parent class.
  //

  TraceClient* _tracer;


private:

  //
  //: Intentionally Unimplemented default constructor
  //

  SessionClient();

  //
  //: Intentionally unimplemented const copy constructor
  //

  SessionClient(const SessionClient&);

  //
  //: Intentionally unimplemented assignment operator
  //

  SessionClient& operator =(const SessionClient&);


};
#endif // __SESSIONCLIENT_H__
