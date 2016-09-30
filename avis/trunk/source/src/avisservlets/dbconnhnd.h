#if !defined (__DBCONNHND_H__)
#define __DBCONNHND_H__ 
// Do this EXACTLY like the Signature caching (for now)

class DBSession;
class DBConnection;
class DBConnectionPool;

class TraceClient;

//
//: DBConnectionHandle encapsulates a client usable interface to a
// database connection. 
//
//

class DBConnectionHandle
{
  friend DBConnectionPool;
public:

  //
  //: Copy Constructor
  //

  DBConnectionHandle(const DBConnectionHandle&);

  //
  //: Assignment operator
  //

  DBConnectionHandle& operator =(const DBConnectionHandle&);

  //
  //: comparitor
  //

  bool operator ==(const DBConnectionHandle& r_) const
    {
      // YES this is a pointer compare
      return _connection == r_._connection;
    };

  //
  //: Virtual Destructor
  //

  virtual ~DBConnectionHandle();


  //
  //: Single service method
  //

  DBSession& get_session();


protected:

  //
  //: Increment reference count for this database connection
  // 

  void increment_reference_count();

  //
  //: Decrement reference count for this database connection
  //

  void decrement_reference_count();

private:

  //
  //: The "live" constructor executed by the DBConnectionPool object
  //

  DBConnectionHandle(DBConnectionPool& connection_pool_,
                     DBConnection* connection_);

  //
  //: Connection Pool reference
  //

  DBConnectionPool& _connection_pool;

  //
  //: Pointer to the connection body
  //

  DBConnection* _connection;

  //
  //: Intentionally unimplemented default constructor
  //

  DBConnectionHandle();

  //
  //: Standard Tracing Object
  //

  TraceClient* _tracer;
    
};

#endif // __DBCONNHND_H__ Sentinel
