#if !defined (__DBCONPOOL_H__)
#define __DBCONPOOL_H__

#include "dbconnhnd.h"
#include "dbconn.h"
#include "aviscommon/log/lckobj.h"
#include "aviscommon/log/semaphore.h"

#include <string>
#include <map>
#include <deque>

class MTrace;
class TraceClient;
class DBConnection;
class LockableObject;
class Semaphore;


//
//: Maintains a highly configurable pool of connections to an ODBC datasource
//
// The DBConnectionPool was created to manage a significant resource
// bottleneck in the Immune System Gateway Array.
//
// Most of the Gateway Servlets make significant use of the database
// as a persistence mechanism.  However, the actual amount of database 
// activity is very small (a few selects or updates per servlet
// transaction), therefore it is extremely useful to release a
// database connection back to an internal pool as soon as it is no
// longer required. 
// 

class DBConnectionPool
{
  friend DBConnectionHandle;
public:

  //
  //: Object Constructor
  //
  // The Connection Pool object is created with two parameters.
  //
  //  reclaimation_threshold_:  The maximum number of available
  //          connections to allow before they are closed and removed
  //          from the pool.  For instance: If there is a surge of
  //          activity, causing the connection pool to allocate and use
  //          500 connections "at once" but that activity drops off to a 
  //          "normal" level, we do not want to hold hundreds of
  //          connections open that may be required by another server in 
  //          the gateway array.  We may therefore specify a
  //          reclaimation threshold to tell the ConnectionPool to
  //          "float down" the number of pooled connections to some
  //          reasonable number as they become available.
  //
  //  absolute_maximum_: The maximum number of connections for a
  //          DBConnectionPool to allocate at one time.  Requests for
  //          more connections once the pool has reached this maximum
  //          will block (as the connection maximum is managed by a
  //          win32 semaphore initialized with this absolute_maximum_
  //          number)
  //

  DBConnectionPool(MTrace& trace_server_,
                   long reclaimation_threshold_,
                   long absolute_maximum_);

  //
  //: Virtual destructor
  //

  virtual ~DBConnectionPool();

  //
  //: Method through which a client may request a new connection
  //: handle.
  //

  DBConnectionHandle get_database_connection();

protected:

  //
  //: Increment the reference count on a particular database
  //: connection handle.
  //
  
  void increment_reference_count(DBConnection* connection_);

  //
  //: Decrement the reference count on a particular database
  //: connection handle.
  //

  void decrement_reference_count(DBConnection* connection_);


  //
  //: retrieve a reference to the current active MTrace object
  //
  // This is required for the DBConnectionHandle objects to provide 
  // their own logging 
  // (see DBConnectionHandle& DBConnectionHandle::operator =
  //                    (const DBConnectionHandle&);)
  //  for more information (in dbconnhnd.cpp)
  //

  MTrace& get_trace_server() { return _trace_server;};

private:

  //
  //: Build a string suitable for debugging with some simple
  //: statistics about the state of the DBConnectionPool
  //

  std::string pool_state_msg() const;


  //
  //: These two methods are the "critical" entrypoints for serializing 
  //: access to the _available_queue.
  //

  DBConnection* get_available_connection();

  //: These two methods are the "critical" entrypoints for serializing 
  //: access to the _available_queue.

  void make_connection_available(DBConnection* connection_);

  //
  //: Pool of available DBConnection object pointers
  //

  std::deque<DBConnection*> _available_queue;

  //
  //: Separate map of all allocated connection objects and their reference counts
  //
  //
  // Note: These reference counts do NOT reflect the number of threads 
  // currently using the connections.  It represents the number of
  // 'in-scope' DBConnectionHandle objects currently referencing these 
  // connections.  When that reference count is decremented to zero,
  // the connection object is added to the _available_queue.
  //

  std::map<DBConnection*,int> _allocated_connections;


  //
  //: Maximum number of unused available connections to maintain at
  //: any given time.
  //

  long _reclaimation_threshold;

  //
  //: Total number of connections to manage.
  //
  // This is regulated by the semaphore count.  Therefore any requests 
  // for new database connections once this threshold has been reached 
  // (assuming there are no connections in the available pool) will
  // BLOCk until a connection is released.
  //

  long _absolute_maximum;

  //
  //: Top-level lock object for the connection cache
  //

  LockableObject* _queue_lock;

  //
  //: Semaphore to manage the size of the connection pool
  //
  // Semaphore to block on an empty pool (I still need to serialize
  // access to the pool collections themselves though with the
  // "_queue_lock" defined above
  //

  Semaphore* _pool_semaphore;


  //
  //: Reference to processes MTrace object, used in creation of
  //: DBConnection objects
  //

  MTrace&      _trace_server;

  //
  //: Standard Trace Object
  //

  TraceClient* _trace_client;
};

#endif // __DBCONPOOL_H__ Sentinel
