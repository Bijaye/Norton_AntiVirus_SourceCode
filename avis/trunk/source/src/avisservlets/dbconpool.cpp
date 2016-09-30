#include "dbconpool.h"
#include "dbconn.h"
#include "avisdb/dbsession.h"

#include "aviscommon/log/mtrace.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "aviscommon/log/critsec.h"
#include "aviscommon/log/lock.h"
#include "aviscommon/log/semaphore.h"

#include "strutil.h"

DBConnectionPool::DBConnectionPool(MTrace& trace_server_,
                                   long reclaimation_threshold_,
                                   long absolute_maximum_)
  :_allocated_connections(),
   _reclaimation_threshold(reclaimation_threshold_),
   _absolute_maximum(absolute_maximum_),
   _queue_lock(NULL),
   _pool_semaphore(NULL),
   _trace_server(trace_server_),
   _trace_client(NULL)
{
  _trace_client = new TraceClient(trace_server_,"DBConnectionPool");

  //
  // Construct the critical section (which is what the locks
  // are implemented as here
  //

  _queue_lock = new CriticalSection();
  
  // Construct the semaphore 
  _pool_semaphore = new Semaphore("DBConnectionPoolSem",
                                  _absolute_maximum,
                                  _absolute_maximum);

  _trace_client->info(pool_state_msg());
}

DBConnectionPool::~DBConnectionPool()
{
  // Close all pending connections in the queue
  // Delete the CriticalSection
  _trace_client->info("Deconstructing DBConnectionPool");
  // Delete all connections
  while (!_allocated_connections.empty())
    {
      std::map<DBConnection*,int>::iterator i = _allocated_connections.begin();
      // Grab the pointer
      DBConnection* _connection = (*i).first;
      // Nuke it
      delete _connection;
      
      _allocated_connections.erase(i);
    }
  
  delete _pool_semaphore;
  _pool_semaphore = NULL;

  delete _queue_lock;
  _queue_lock = NULL;  

  delete _trace_client;
  _trace_client = NULL;
}

DBConnectionHandle DBConnectionPool::get_database_connection()
{
  StackTrace _stack(*_trace_client,"get_database_connection");

  // Block on the semaphore (if necessary)
  _pool_semaphore->capture();
  _stack.info(pool_state_msg());

  // Create a DBConnectionHandle object with the _connection_id
  // then pull it out of the available queue.
  DBConnection* _connection = get_available_connection();
  DBConnectionHandle _handle((*this),_connection);
  return _handle;
}

std::string DBConnectionPool::pool_state_msg() const
{
  std::string _message("STATE|Alloc:");
  _message.append(StringUtil::ltos(_allocated_connections.size()));
  _message.append("|Avail:");
  _message.append(StringUtil::ltos(_available_queue.size()));
  return _message;
}
DBConnection* DBConnectionPool::get_available_connection()
{
  StackTrace _stack(*_trace_client,"get_available_connection[S]");
  _stack.info(pool_state_msg());
  Lock _held_queue_lock(*_queue_lock);
  DBConnection* _connection = NULL;

  if (_available_queue.size() == 0)
    {
      _stack.info("Queue empty:  Creating new connection");
      // Create the connection
      // Note: we don't push it on to the queue since we need it now.
      _connection = new DBConnection(_trace_server);
      // Initialize with reference count of zero
      _allocated_connections[_connection] = 0;
    }
  else
    {
      _stack.info("Pulled connection from cache");
      _connection = _available_queue.front();
      _available_queue.pop_front();
    }
  return _connection;
}
void DBConnectionPool::make_connection_available(DBConnection* connection_)
{
  StackTrace _stack(*_trace_client,"make_connection_available");
  _stack.info(pool_state_msg());
  Lock _held_queue_lock(*_queue_lock);

  // Push it on to the queue
  _available_queue.push_back(connection_);

  // Increment the semaphore
  _pool_semaphore->release();
}
void DBConnectionPool::increment_reference_count(DBConnection* connection_)
{
  _trace_client->info("increment_reference_count()");
  _allocated_connections[connection_]++;
}
void DBConnectionPool::decrement_reference_count(DBConnection* connection_)
{
  StackTrace _stack(*_trace_client,"decrement_reference_count");
  _stack.info(pool_state_msg());
  if (--(_allocated_connections[connection_]) == 0)
    {
      _stack.info("Moving connection to available queue");
      Lock _held_queue_lock(*_queue_lock);
      _available_queue.push_back(connection_);
      // Increment the semaphore
      _pool_semaphore->release();
    }
}
