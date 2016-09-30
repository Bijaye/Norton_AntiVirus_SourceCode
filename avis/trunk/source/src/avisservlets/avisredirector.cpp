// avisredirector.cpp - Implementation of the AvisRedirector class

#pragma warning (disable : 4786)

#include "stringmap.h"
#include "avisredirector.h"
#include "strutil.h"
// Avisdb headers
#include "gosession.h"
//-----------------------
#include "utilexception.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "redirectorstate.h"

AvisRedirector::AvisRedirector(GoServletHost& host_,
                               const char* gwapihandle_,
                               REDIRECTOR_TYPE type_
                               )
  :GoServletClient(host_,gwapihandle_),
   _type(type_),
   _tracer(0)
   
{
  _tracer = new TraceClient(_trace_server,"AvisRedirector");
  _tracer->debug("Created AvisRedirector");
}
AvisRedirector::~AvisRedirector()
{
  _tracer->debug("Destroying AvisRedirector");
  if (_tracer)
    delete _tracer;
}

void AvisRedirector::setRequiredHeaders  () 
{
  StackTrace _stack(*_tracer,"setRequiredHeaders");

}
int  AvisRedirector::verifyHeaderContent () 
{
  StackTrace _stack(*_tracer,"verifyHeaderContent");
  int _retval = 1;
  return _retval;
} 

int AvisRedirector::fulfillRequest()
{
  
  StackTrace _stack(*_tracer,"fulfillRequest");

  // Note the return code for the redirector!!!
  // Temporary redirection
  int _retval = 302;
  std::string _responseHeaders("");
  
  // Get the server name from the redirector state object held up
  // above and compose the full URL response string
  // Build the protocol...
  std::string _host("http");
  // ssl mode urls start with "https" instead of "http"
  // The port number will be reflected appropriately
  long _port_number = atol(_gwapi.incomingHeader("SERVER-PORT").c_str());
  bool _is_ssl_transaction = false;
  
  //
  // An exception will be thrown if this header is tested and it
  // doesn't exist at all. Apparently, the HTTPS* headers are only
  // present if there is a valid SSL configuration for the running
  // instance of Lotus Go (but regardless of whether or not SSL is
  // actually being used for the current transaction)
  // That's why this test is wrapped in a try catch block.
  //

  try
    {
      _is_ssl_transaction = (_gwapi.incomingHeader("HTTPS") == "ON");
    }
  catch(InvalidHttpHeader& e_)
    {
      //
      // Log the absense of the header, just in case.
      //
      _stack.warning("Missing the HTTPS Header, assuming non-ssl...");
      
      //
      // set the flag to false for clarity (even though it's done by
      // default above.)
      //
      _is_ssl_transaction = false;
    }

  if (_is_ssl_transaction)
    {
      _stack.debug("Redirecting SSL transaction");
      _host.append("s://");
      _port_number++;
    }
  else
    {
      _stack.debug("Redirecting NON-SSL transaction");
      _host.append("://");
    }
  
  _host.append(get_next_server(_type));
  // Add the port number
  _host.append(":");
  
  _host.append(StringUtil::ltos(_port_number));
  _host.append(_gwapi.incomingHeader("SCRIPT-NAME"));
  //
  // Plug it into the outbound interface
  //  _gwapi.setSystemHeader("Location",_host);
  _gwapi.setSystemHeader("HTTP_Location",_host);
  
  
  return _retval;
}

std::string AvisRedirector::get_next_server(REDIRECTOR_TYPE type_)
{
  std::string _server;
  switch (type_)
    {
    case RT_SIGNATURE_DOWNLOAD:
      _server = _redirector.get_definition_server();
      break;
    case RT_SAMPLE_SUBMISSION:
      _server = _redirector.get_sample_server();
      break;      
    default:
      throw InfrastructureException("Bad Redirector Type (probably invalid httpd.cnf)");
    };
  return _server;
}
