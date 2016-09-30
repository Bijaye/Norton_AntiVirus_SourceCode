//
// gosession.h - prototype for the GoSession class
//

#if !defined (__GOSESS_H__)
#define __GOSESS_H__

#include <map>
#include <string>
#include <fstream>

#include "aviscommon/log/mtrace.h"
#include "types.h" // MStringString

class GwapiProxy; // Why create an unnecessary header dependency
class MBuffer;
class TraceClient;

//
//: Provide a High-Level interface to the Lotus Go programming api
//
// GoSession is a rather heavyweight class that serves two major
// purposes:
//
// 1) Provide an access point for the gateway servlet transaction
// handler classes (GoServletClient children) to access the callbacks
// provided through the Lotus Go Webserver Api (GWAPI).
//
// 2) Provide the initialization and termination methods common to
// every transaction.  This includes (but is not limited to ;)
// importing and translating the http transaction headers on object
// construction.
//

class GoSession
{
    
public:

  //
  //: Object Constructor
  //
  // Must be created with the handle passed in to the extern "C"
  // method called to handle the transaction
  //
  // The constructor initilaizes the trace interface and calls the
  // "init()" method.
  //

  GoSession(const unsigned char* handle_,MTrace& trace_server_);

  //
  //: Virtual destructor
  //

  virtual ~GoSession(); 

  //
  //: Retrieve the list of headers and environment variables from
  //: Lotus Go.
  //
  // This is a fairly heavyweight method that is responsible for a
  // great deal of string processing.
  //

  long init();

  //
  //: "test the invariant" returns true if the object is "good"
  //

  virtual bool validObject() const;


  //
  //: Get a reference to the list of incoming transaction headers
  //
  
  const MStringString& getRequestHeaders() const { return _requestHeaders;};

  //
  //: Retrieve the full block of process environment headers,
  //: including the unprocessed incoming headers for the current
  //: transaction
  //

  const MStringString& getRawEnvironment() const { return _environment;};
  
  //
  //: Get one block of content an unsigned char* of specified length
  //
  
  long getContentChunk(unsigned char* buffer_,long bufferSize_);

  //
  //: Is there any more content available?
  //
  // This just returns the value of a boolean set when
  // "getContentChunk" is called.  This value is initialized in the
  // constructor to false.  Then, even if there is no content, the
  // worst that can happen is one call to getContentChunk that returns 
  // zero bytes.
  //
  
  bool eof() const { return _contentEof;};

  //
  //: set a block of headers.
  //
  // This method takes an MStringString table.  It iterates across
  // them and calls setResponseHeader for each of the name:value
  // pairs.
  //

  void setResponseHeaders(const MStringString&);

  //
  //: set a single http "X-" response header
  //
  // setResponseHeader test for name parameter to be sure it begins
  // with "x-" or "X-" before doing anything.  If it does not, the
  // method exits. Otherwise it logs the name value pair as an
  // "outheader", then translates the name using the goMunge method.
  //
  // Afterwards it calls set method in the _gwapi to actually set the
  // response header.
  //

  long setResponseHeader(const std::string& name_,const std::string& value_);
  
  //
  //: Set a single Debug header (will overwrite if exists)
  //
  // this takes a name value pair.  It appends "X-Gateway-Debug-" to
  // the front of the name, then sends the name-value pair to
  // setResponseHeader.  
  //
  // This isn't used anywhere within the servlets.  It was created to
  // aid debugging of servlets code from the client-side.
  //

  long setDebugResponseHeader(const std::string& name_,const std::string& value_);
  
  //
  //: Set a single system header (will overwrite if exists)
  //
  // Like setResponseHeader, this method takes a name value pair and
  // calls _gwapi->set() to send it back across the http transaction.
  // However, unlike setResponseHeader it does not translate the name
  // before doing so.
  //
  // This is used to set the value of standard http headers
  // ( "Location" "Content-Length" etc.)
  //

  long setSystemHeader(const std::string& name_,const std::string& value_);

  //
  //: a simple header set "Content-Type: <contentType_>"
  //
  // This is a simple wrapper around the setSystemHeader method.
  //

  long  setContentType(const std::string& contentType_);

  //
  //: get a simple incoming header attribute
  //: value.
  //

  const std::string incomingHeader(const std::string& key_) const;

  //
  //: Send the content using GwapiProxy::write();
  //
  // This method is called when live content is included in a
  // response. the buffer containing the content and a long value
  // indicating the size of the buffer are passed in, the
  // "Content-Length" response header is set, and the content is
  // submitted to the HTTP stream.
  //
  // NOTE: If you call sendContent, and make subsequent calls to
  // set*Header methods, the attributes will be appended to the
  // content.  This is almost certainly undesirable.
  //

  long sendContent(const char* buffer_,long bufferSize_);

  //
  //: Set any required headers, and ship the content across the pipe
  //
  // Much to my surprise, this method is unimplemented.
  //

  long closeSession(long httpReturnCode_);

protected:

  //
  //: Lotus Go API wrapper class object
  //
  // Provides nicer semantics to the callback interface given by the
  // Go webserver
  //
  
  GwapiProxy* _gwapi;

  //
  //: Maintains a boolean indicating the end of an incoming content
  //: stream.
  //
  // This is set to false in the constructor and is subsequently set
  // as a side effect of calls to getContentChunk.
  //

  bool _contentEof;


  //
  //: Dump the complete environment table to the content stream
  //
  // This is a debug method that should really be dropped.  It formats 
  // the full block of incoming headers as a string, and sends them
  // back across the HTTP content stream.
  //

  void debug_dumpEnvToContent();

  //
  //: Translate an outgoing "X-" header
  //
  // the Go server has some bizarre conventions for renaming
  // environment variables with a prepended HTTP_ sometimes.
  // Not always, but sometimes. (when the headers are not standard
  // CGI headers.  I assume this is done to avoid process environment
  // clashing. Whatever the reason, we have to deal with it. So here
  // they are, back from their amazingly successful european tour: 
  // let's have a big round of applause for The goMunge and goDeMunge
  // methods!

  std::string& goMunge(std::string& name_) const;
  
  //
  //: Translate an incoming http header
  //
  // the Go server has some bizarre conventions for renaming
  // environment variables with a prepended HTTP_ sometimes.
  // Not always, but sometimes. (when the headers are not standard
  // CGI headers.  I assume this is done to avoid process environment
  // clashing. Whatever the reason, we have to deal with it. So here
  // they are, back from their amazingly successful european tour: 
  // let's have a big round of applause for The goMunge and goDeMunge
  // methods!
  //
  
  std::string& goDeMunge(std::string& name_) const;

  //
  //: Retrieve the entire environment from Go (extract
  //: "ALL_VARIABLES")
  //

  long getEnvironment();

  //
  //: Translate the environment string into the _environment map
  //
  // called by getEnvironment()
  //
  
  void translateEnvironment(const std::string& buffer_);

  //
  //: call 'deMunge' on a value and add it to a local string table
  //
  // If the header is an http transaction header, the name value pair
  // is added to the _requestHeaders table, otherwise it's added to
  // the _environment table.
  //
  // This is called by translateEnvironment.
  //

  void setRequestHeader(const std::string& name_,const std::string& value_);


  //
  //: Full incoming environment (including untranslated headers, and
  //: everything)
  //

  MStringString _environment;

  //
  //: unravelled headers specific to GO and to this request
  //

  MStringString _requestHeaders;


  //
  //: ship a serialized version of the _responseHeaders down the pipe
  //: through the go interface
  //

  void dispatchResponseHeaders();

  //
  //: Note: this contains "Munged" headers for the Lotus Go Httpd
  //

  MStringString _responseHeaders;

private:

  //
  //: Standard Trace Object
  //

  TraceClient* _tracer;

  //
  //: Intentionally unimplemented default constructor
  //

  GoSession();

  //
  //: Intentionally unimplemented const copy constructor
  //

  GoSession(const GoSession&);

  //
  //: intentionally unimplemented assignment operator
  //

  GoSession& operator =(const GoSession&);

};

#endif // __GOSESS_H__
