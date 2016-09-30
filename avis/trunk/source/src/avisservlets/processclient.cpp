
// processclient.cpp 

#include "processclient.h"
#include "aviscommon/log/traceclient.h"

ProcessClient::ProcessClient(SessionHost& host_)
  :SessionClient(host_),
   _tracer(0)
{
  _tracer = new TraceClient(_trace_server,"ProcessClient");
  _tracer->debug("Creating ProcessClient object");
}

ProcessClient::~ProcessClient()
{
  _tracer->debug("Destroying ProcessClient Object");
  delete _tracer;
  _tracer = 0;
}
