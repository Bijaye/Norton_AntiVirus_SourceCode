#include "dbconn.h"

// Tracing and Logging headers
#include "aviscommon/log/mtrace.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
// DBSession and dependent headers (blecchh)
#include "avisdb/dbsession.h"

DBConnection::DBConnection(MTrace& trace_server_)
  :_session(NULL),
   _tracer(NULL)
{
  _tracer = new TraceClient(trace_server_,"DBConnection");
  _tracer->debug("Creating DBConnection object");
  connect_to_database();
}

DBConnection::~DBConnection()
{
  _tracer->debug("Destroying DBConnection object");
  disconnect_from_database();
  // Kill the Tracing hook
  delete _tracer;
  _tracer = NULL;
}

DBSession& DBConnection::get_session()
{
  _tracer->info("get_session()");
  return (*_session);
}

void DBConnection::connect_to_database()
{
  StackTrace _stack(*_tracer,"connect_to_database");
  // Responsible for creating the _session object
  // I KNOW this isn't gonna be this easy...
  _session = new DBSession();
}
void DBConnection::disconnect_from_database()
{
  StackTrace _stack(*_tracer,"disconnect_from_database");
  // Responsible for deleting the _session object
  delete _session;
  _session = NULL;
}
