// goservlethost.cpp - implementation of the goservlethost class

#include "goservlethost.h"

// Stuff for database test
#include <avisdb/stdafx.h>
#include <assert.h>
#define ASSERT assert
#include "avisdb/globals.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "aviscommon/log/logger.h"

// These are the external resources themselves
#include "sigcache.h"
#include "redirectorstate.h"
#include "dbconpool.h"

GoServletHost::GoServletHost(const char* configfile_)
  :SessionHost(configfile_),
   // SignatureCache manager
   _sig_cache(NULL),
   // Avis DB Service
   _validAvisDb(false),
   // RedirectorState
   _redirector_state(NULL),
   // Database Connection Pool
   _connection_pool(NULL)
{
  _tracer = new TraceClient(*_trace_server,"GoServletHost");
  _tracer->msg("**** GoServletHost Initializing ***");
  startup();
}

GoServletHost::~GoServletHost()
{
  _tracer->msg("**** GoServletHost Shutting down ***");
  shutdown();
  delete _tracer;
}

void GoServletHost::startup()
{
  initializeAvisDb();
  initialize_avis_logging();
  initialize_signature_cache();
  initialize_redirector_state();
  initialize_connection_pool();
  _tracer->msg("GoServletHost started...");
}
void GoServletHost::shutdown()
{
  _tracer->msg("GoServletHost shutting down");
  shutdown_connection_pool();
  shutdown_redirector_state();
  shutdown_signature_cache();
  shutdownAvisDb();
}

void GoServletHost::initialize_avis_logging()
{ 
  Logger::SendTo(_trace_server);
  Logger::SeverityFilter(Logger::LogEntry);
  Logger::SourceFilter(Logger::LogEverything);
}

void GoServletHost::initializeAvisDb()
{
  // Perform the test from "ResourceTests"
  try
    {
      ushort _version = 0;
      Globals::DatabaseVersion(_version);
      _tracer->msg("Database Ping Succeeded");
    }
  catch (AVISDBException& dbe_)
    {
      std::string _traceout("Database Ping Failed!  Aborting...\n");
      _traceout += dbe_.DetailedInfo();
      _tracer->critical(_traceout);
      throw dbe_;
    }
}

void GoServletHost::shutdownAvisDb()
{
  // NOP!
}

void GoServletHost::initialize_signature_cache()
{
  // Create a new SignatureCache object and feed it the
  // SignaturesDirectory parameter from the configuration file that
  // has already been processed in the parent class
  std::string _signature_path((*_environment)["signatureDirectory"]);
  _sig_cache = new SignatureCache((*_trace_server),
                                  _signature_path);
  
}
void GoServletHost::shutdown_signature_cache()
{
  delete _sig_cache;
  _sig_cache = NULL; 
}


void GoServletHost::initialize_redirector_state()
{
  // Construct a new RedirectorState object with the current
  // configuration environment table as a constructor argument
  _redirector_state = new RedirectorState((*_trace_server),*_environment);
}
void GoServletHost::shutdown_redirector_state()
{
  delete _redirector_state;
  _redirector_state = NULL;
}

void GoServletHost::initialize_connection_pool()
{
  StackTrace _stack(*_tracer,"initialize_connection_pool");
  
  // Grab the "appropriate" configuration/environment variables
  // for the connection pooling
  //
  std::string _reclaim_threshold((*_environment)["DBCP_Reclaimation_Threshold"]);
  std::string _max_connections((*_environment)["DBCP_Maximum_Connections"]);
#if 0
  std::string _message("ENVHEADER DBCP_Reclaimation_Threshold: ");
  _message.append(_reclaim_threshold);
  _stack.info(_message);
  _message = "ENVHEADER DBCP_Maximum_Connections: ";
  _message.append(_max_connections);
  _stack.info(_message);
#endif

                  
  long _dbcp_reclaimation_threshold = atoi(((*_environment)["DBCP_Reclaimation_Threshold"]).c_str());
  if (_dbcp_reclaimation_threshold == 0)
    {
      _dbcp_reclaimation_threshold = 10;
    }
  long _dbcp_max_connect            = atoi(((*_environment)["DBCP_Maximum_Connections"]).c_str());
  if (_dbcp_max_connect == 0)
    {
      _dbcp_max_connect = 100;
    }
  _connection_pool = new DBConnectionPool(*_trace_server,
                                          _dbcp_reclaimation_threshold,
                                          _dbcp_max_connect);
}
void GoServletHost::shutdown_connection_pool()
{
  StackTrace _stack(*_tracer,"shutdown_connection_pool");
  delete _connection_pool;
  _connection_pool = NULL;
}
