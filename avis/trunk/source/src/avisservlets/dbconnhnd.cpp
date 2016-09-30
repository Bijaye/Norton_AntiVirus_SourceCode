#include "dbconnhnd.h"
#include "dbconpool.h"

#include "aviscommon/log/traceclient.h"


DBConnectionHandle::DBConnectionHandle(const DBConnectionHandle& r_)
  :_connection_pool(r_._connection_pool),
   _connection(r_._connection),
   _tracer(NULL)
{
  _tracer = new TraceClient(_connection_pool.get_trace_server(),
                            "DBConnectionHandle");
  increment_reference_count();
}
DBConnectionHandle& DBConnectionHandle::operator =(const DBConnectionHandle& r_)
{
  if (&r_ != this)
    {
      // Note: The _connection_pool reference can't be assigned.  This
      // is ok because there's only ever one.  If there is an object
      // to assign to at all, it MUST have been initialized fully,
      // meaning that the _connection_pool reference is already
      // GUARANTEED to be valid by C++ semantics.  (Unless some
      // opportunistic fool trys to muck with this it can't be broken)
      _connection = r_._connection;
      increment_reference_count();
    }
  return (*this);
}

DBConnectionHandle::~DBConnectionHandle()
{
  decrement_reference_count();
  delete _tracer;
}

DBConnectionHandle::DBConnectionHandle(DBConnectionPool& connection_pool_,
                                       DBConnection*     connection_)
  :_connection_pool(connection_pool_),
   _connection(connection_),
   _tracer(NULL)
{
  _tracer = new TraceClient(_connection_pool.get_trace_server(),
                            "DBConnectionHandle");
  increment_reference_count();
}

DBSession& DBConnectionHandle::get_session()
{
  return _connection->get_session();
}
void DBConnectionHandle::increment_reference_count()
{
  _connection_pool.increment_reference_count(_connection);
}

void DBConnectionHandle::decrement_reference_count()
{
  _connection_pool.decrement_reference_count(_connection);
}
