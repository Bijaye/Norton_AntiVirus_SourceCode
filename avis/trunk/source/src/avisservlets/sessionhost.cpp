// sessionhost.cpp - Implementation of the sessionhost class

#pragma warning (disable : 4786)
#include "sessionhost.h"
// String <-> map conversions (with config file loads, etc.)
#include "stringmap.h"

#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/mtrace.h"
#include "aviscommon/log/tracesinkfile.h"

#include <fstream>

SessionHost::SessionHost(const char* configfile_)
  :_environment(0),
   _validEnvironment(0),
   _trace_server(0),
   _validTraceServer(0),
   _tracer(0),
   _trace_sink(0)
{

  // Bootstrap the invariant
  initializeEnvironment(configfile_); 
  startup();
  _tracer = new TraceClient(*_trace_server,"SessionHost");
  // Pull in the rest of the service interfaces
  _tracer->debug("Created SessionHost");
}

SessionHost::~SessionHost()
{
  _tracer->debug("Destroying Down SessionHost");
  delete _tracer;
  shutdown();
  shutdownEnvironment();
}

void SessionHost::startup()
{
  // Initialize services
  initializeTraceServer();
}

void SessionHost::shutdown()
{
  // Shutdown Services
  shutdownTraceServer();
}

void SessionHost::initialTrace()
{
  std::string _traceout = "";

  // Dump the input environment (includes the config file)

  _traceout = "Initial Environment...\n";
  _traceout += StringMapUtil::toDebugString(*_environment);
  _tracer->debug(_traceout);
}

void SessionHost::initializeEnvironment(const char* configfile_)
{
  // Get the full environment
  // (_environ is defined in stdlib.h as char**)
  MStringString _procEnv = StringMapUtil::fromEnvParam(_environ);

  // Load the configuration file 
  MStringString _configEnv = StringMapUtil::fromConfigFile(configfile_);

  // Create the new object
  _environment = new MStringString;
  
  // Merge the hash tables into it
  (*_environment) = StringMapUtil::hashMerge(_procEnv,_configEnv);

  if (_environment && !_environment->empty())
    {
      _validEnvironment = true;
    }
}

void SessionHost::shutdownEnvironment()
{
  _validEnvironment = false;
  delete _environment;
  _environment = 0;
}


void SessionHost::initializeTraceServer()
{
  // Build the trace sink object
  // Get the value of the "Trace_Rollover_Hourly" var from the local
  // configuration.  As you can see below, this defaults to true.
  //
  bool _hourly_rollover = true;

  _hourly_rollover = !((*_environment)["Trace_Rollover_Hourly"] == "off");
  
  _trace_sink = new TraceSinkFile(_environment->operator[]("LogFile").c_str(),
                                  _hourly_rollover
                                  );

  _trace_server = new MTrace(*_trace_sink);
  _validTraceServer = (_trace_server != 0);


  _trace_server->setTraceMode(buildTraceMode());
  setTraceSwitches(_trace_server);
  setTraceVerbosity(_trace_server);
}

void SessionHost::shutdownTraceServer()
{

  _validTraceServer = 0;
  // Delete the server object
  delete _trace_server;
  _trace_server = 0;

  // Delete the sink
  delete _trace_sink;
  _trace_sink = 0;

}

unsigned int SessionHost::buildTraceMode() const
{
  unsigned int _tracemode = 0; //MODE_NORMAL;

  //  // Trace Mode Flags (default to MODE_NORMAL)
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Linenumbers") == "on")
    {
      _tracemode += TRACE_LNUMS;
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Datestamp") == "on")
    {
      _tracemode += TRACE_DSTAMP;
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Timestamp") == "on")
    {
      _tracemode += TRACE_TSTAMP;
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Severity") == "on")
    {
      _tracemode += TRACE_SEVERITY;
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Source") == "on")
    {
      _tracemode += TRACE_SOURCE;
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Indent") == "on")
    {
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_VStack") == "on")
    {
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Code") == "on")
    {
      _tracemode += TRACE_CODE;
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Message") == "on")
    {
      _tracemode += TRACE_MESSAGE;
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_ThreadId") == "on")
    {
      _tracemode += TRACE_THREADID;
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Mode_Finetime") == "on")
    {
      _tracemode += TRACE_FINETIME;
    }
  return _tracemode;
}


void SessionHost::setTraceSwitches(MTrace* trace_server_) const
{

  // Line Type Switches
  

  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Include_Msg") == "off")
    {
      trace_server_->switchOffLineType(MTrace::lt_message);
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Include_Stack") == "off")
    {
      trace_server_->switchOffLineType(MTrace::lt_enter);
      trace_server_->switchOffLineType(MTrace::lt_exit);
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Include_Debug") == "off")
    {
      trace_server_->switchOffLineType(MTrace::lt_debug);
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Include_Info") == "off")
    {
      trace_server_->switchOffLineType(MTrace::lt_info);
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Include_Warning") == "off")
    {
      trace_server_->switchOffLineType(MTrace::lt_warning);
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Include_Error") == "off")
    {
      trace_server_->switchOffLineType(MTrace::lt_error);
    }
  if ( StringMapUtil::valueIfExists(*_environment,"Trace_Include_Critical") == "off")
    {
      trace_server_->switchOffLineType(MTrace::lt_critical);
    }
}

void SessionHost::setTraceVerbosity(MTrace* trace_server_) const
{
  MTrace::trace_verbosity _verb = MTrace::tv_verbose;
  
  std::string _voption("");
  _voption = StringMapUtil::valueIfExists(*_environment,"Trace_Verbosity");
  if (_voption == "Verbose")
    _verb = MTrace::tv_verbose;
  if (_voption == "Terse")
    _verb = MTrace::tv_terse;
  if (_voption == "Exhaustive")
    _verb = MTrace::tv_exhaustive;
  
  trace_server_->setVerbosity(_verb);
}
