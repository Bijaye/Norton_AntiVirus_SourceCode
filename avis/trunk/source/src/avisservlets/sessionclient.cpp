// sessionclient.cpp - Implementation of the sessionclient class


#include "sessionhost.h"
#include "sessionclient.h"
#include "aviscommon/log/traceclient.h"

SessionClient::SessionClient(SessionHost& host_)
  :_environment(*(host_._environment)),
   _trace_server(*(host_._trace_server)),
   _tracer(0)
{
  _tracer = new TraceClient(_trace_server,"SessionClient");
  _tracer->debug("Created TraceClient");
}

SessionClient::~SessionClient()
{
  _tracer->debug("Destroying TraceClient");
  delete _tracer;
  _tracer = 0;
}
