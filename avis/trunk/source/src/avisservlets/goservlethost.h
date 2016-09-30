// goservlethost.h - Prototype for the GoServletHost class

#if !defined (__GOSERVLETHOST_H__)
#define __GOSERVLETHOST_H__

#include "sessionhost.h"

class TraceClient;
class GoServletClient;
class SignatureCache;
class RedirectorState;
class DBConnectionPool;

//
//: Initializes and manages external resources for the gateway
//: servlets.
//
// GoServletHost manages the initialization and shutdown of the
// following resources for the gateway servlets: 
// \br Tracing and logging.
// \br Database connectivity
// \br Database connection pooling
// \br Definition package content caching.

class GoServletHost : virtual public SessionHost
{
  friend GoServletClient;
public:

  //
  //: Object constructor.
  //
  // The only parameter to the constructor is the name of the
  // configuration file for the gateway servlets (currently
  // "avisservlets.prf") which it passes to the SessionHost
  // constructor for loading.
  //

  GoServletHost(const char* configfile_);

  //
  //: Virtual destructor
  //

  virtual ~GoServletHost();

protected:

  //
  //: perform all required initializations
  //

  void startup();

  //
  //: Shutdown all external resources
  //

  void shutdown();

  //
  //: basic validator. (this is actually unimplemented)
  //

  bool validObject() const;

  //-------------------------------------------------
  // Initialize Services
  //-------------------------------------------------

  //
  //: Initialize logging
  //
  // initialize_avis_logging hooks the SessionHost's 
  // MTrace object into the back end of AvisCommon's
  // internal logger by calling a few static methods.
  //

  void initialize_avis_logging();

  //
  //: Initialize the database interface
  //
  // initializeAvisDb only performs a transient 
  // connection test, then sets the flag.  But it 
  // still qualifies as external service 
  // initialization.

  void initializeAvisDb();

  //
  //: Cleanup database interface
  //
  // This is actually a NOP method.  There is no manual cleanup
  // required of the database connections.  
  //

  void shutdownAvisDb();

  //
  //: Is the database configuration valid and functional.
  //
  bool _validAvisDb;


  //
  //: Initialize the signature content caching system
  //
  // The SignatureCache object maintains an internal
  // cache of all Signature content files currently
  // being downloaded by clients to prevent redundant
  // buffers and file access
  //

  void initialize_signature_cache();

  //
  //: Cleanup signature caching system
  //
  // this method deletes the _sig_cache object dynamically created by
  // the corresponding initialization method
  //

  void shutdown_signature_cache();

  //
  //: Pointer to the SignatureCache object.
  //
  // This is created in initialize_signature_cache and deleted in
  // shutdown_signature_cache.
  //

  SignatureCache* _sig_cache;
  


  //
  //: Initialize the internal redirector lists
  //
  // The RedirectorState object maintains two lists
  // of server URLs for the gateway array.  One for 
  // Sample Submission, and one for Signature Download
  // The corresponding redirector servlets query this
  // object for the next round-robin server to direct
  // the client to for their live transaction.
  //

  void initialize_redirector_state();

  //
  //: Delete the redirector state object
  //

  void shutdown_redirector_state();

  //
  //: Pointer to the redirector state object
  //

  RedirectorState* _redirector_state;


  //
  //: Initialize the database connection pool
  //
  //
  // The DBConnectionPool is exactly that. a 'lazily'
  // initialized pool of database connection handles
  // Connections from the pool are only (currently) 
  // used by the AvisGetSignature transaction.
  // Note: This must (obviously) be initialized after
  // and shutdown before the database.
  //
  // It is important to note that at this stage of development only
  // the Signature Download servlets use the database connection
  // pool.  The other servlets use the dubious thread-local-storage
  // database connections from the AvisDatabase dll.  The plan has
  // always been to convert all the servlets to this method of
  // connection management.  
  //

  void initialize_connection_pool();

  //
  //: Delete the connection pool object
  //

  void shutdown_connection_pool();

  //
  //: Pointer to the database connection pool
  //

  DBConnectionPool* _connection_pool;

private:

  //
  //: Standard trace object
  //

  TraceClient* _tracer;

};
#endif // __GOSERVLETHOST_H__ Sentinel
