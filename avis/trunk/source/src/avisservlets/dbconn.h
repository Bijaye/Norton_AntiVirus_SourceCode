#if !defined (__DBCONN_H__)
#define __DBCONN_H__
// These are formally declared in aviscommon/log/mtrace.h and
// aviscommon/log/traceclient.h
class TraceClient;
class MTrace;

class DBSession; // from avisdb/dbsession.h
class DBConnectionPool;
class DBConnectionHandle;

//
//: DBConnection encapulates the interface to an ODBC database
//: connection.
//
// Note that there is no public interface to this class.  This is
// because it is used internal to a DBConnectionPool object. The
// DBConnectionPool creates and manages a collection of DBConnections, 
// but grants clients a "DBConnectionHandle" object to use as an
// interface.
//
class DBConnection
{
  friend DBConnectionPool;
  friend DBConnectionHandle;
protected:

  //
  //: Constructor
  //

  DBConnection(MTrace& trace_server_);
  
  //
  //: Destructor
  //

  ~DBConnection();

  //
  //: Client-side interface method
  //

  DBSession& get_session();
  
private:
  
  //
  //: Open a database connection
  //

  void connect_to_database();

  //
  //: Close a database connection
  //

  void disconnect_from_database();

  //
  //: Internal DBSession object.
  //
  // This is the actual "class that does the work" from src/avisdb.
  // The semantics of the DBSession class are such that it required a
  // wrapper to maintain the loose coupling required by the connection 
  // pooling.
  //

  DBSession*   _session;

  //
  //: Standard Trace Object
  //

  TraceClient* _tracer;
  
};
#endif // __DBCONN_H__ Sentinel
