// goservletclient.cpp - Implementation for the GoServletClient class

#pragma warning(disable: 4786)
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "goservlethost.h"
#include "goservletclient.h"
#include "utilexception.h"
#include "redirectorstate.h"
#include "dbconpool.h"

#include "avisdb/avisdbexception.h"

GoServletClient::GoServletClient(GoServletHost& host_,const char* gwapihandle_)
  :SessionClient(host_),
   _gwapi((unsigned char*)gwapihandle_,_trace_server),
   _sig_cache(*(host_._sig_cache)),
   _redirector(*(host_._redirector_state)),
   _connection_pool(*(host_._connection_pool))
{
  _tracer = new TraceClient(_trace_server,"GoServletClient");
  _tracer->debug("Created GoServletClient");

  // Log the incoming header block. (It was pulled in from the _gwapi
  // constructor)

  if ((_environment.find("Trace_Include_EnvHeaders") != _environment.end()) &&
      (_environment["Trace_Include_EnvHeaders"] == "on"))
    {
      _tracer->debug(StringMapUtil::toPrefixedDebugString("ENVHEADERS",
                                                          _environment));
    }
  if ( (_environment.find("Trace_Include_GoHeaders") != _environment.end()) &&
       (_environment["Trace_Include_GoHeaders"] == "on"))
    {
      _tracer->debug(StringMapUtil::toPrefixedDebugString("GOHEADERS",
                                                          _gwapi.getRawEnvironment()));
    }
}

GoServletClient::~GoServletClient()
{
  _tracer->debug("Destroying GoServletClient");
  delete _tracer;
  _tracer = 0;
}

void GoServletClient::addRequiredHeader(const std::string& header_)
{
  _requiredHeaders.push_back(header_);
}

int GoServletClient::run()
{
  StackTrace _stack(*_tracer,"run");
  int _retval = 0;
  try
    {
      verifyHeaders();
      _gwapi.setSystemHeader("HTTP_Pragma","no-cache");
      _retval = fulfillRequest();
    }
  catch (InfrastructureException& e_)
    {
      std::string _msg("internal");
      _msg += e_.what();
      _gwapi.setResponseHeader("X-Error",e_.response());
      _retval = 204;
    }
  catch (ServletRecoverable& e_)
    {
      std::string _msg("Transaction Error: ");
      _msg += e_.what();
      _stack.error(_msg);
      _gwapi.setResponseHeader("X-Error",e_.response());
      _retval = 204;
    }
  catch (AVISDBException& e_)
    {
      std::string _msg("Database Error: ");
      _msg += e_.TypeAsString();
      _msg += "\n";
      _msg += e_.DetailedInfo();
      _stack.error(_msg);
      _gwapi.setResponseHeader("X-Error","internal");
      _retval = 204;
    }
  catch(...)
    {
      _stack.error("Untyped exception");
      _gwapi.setResponseHeader("X-Error","internal");
      _retval = 204;
    }
  return _retval;
}

bool GoServletClient::verifyHeaders() const
{
  StackTrace _stack(*_tracer,"verifyHeaders");
  bool _allthere = true;
  // Easier to deal with reference semantics
  const MStringString& _incoming = _gwapi.getRequestHeaders();

  // Send the headers to the log
  _stack.debug("Incoming Header Block...");
  _stack.debug(StringMapUtil::toPrefixedDebugString("INHEADER",_incoming)); 
  
  // Iterator for _required headers;
  std::list<std::string>::const_iterator _it = _requiredHeaders.begin();
  while (_allthere && (_it != _requiredHeaders.end()))
    {
      if (_incoming.find(*_it) == _incoming.end())
        {
          _allthere = false;
          std::string _msg("Missing header : ");
          _msg += ((*_it));
          _stack.error(_msg);
          throw InvalidHttpHeader((*_it).c_str());
        }
      _it++;
    }
  return _allthere;
}
