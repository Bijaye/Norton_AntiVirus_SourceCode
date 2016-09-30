// sessionhost.h - Prototype for the sessionhost class

#if !defined (__SESSIONHOST_H__)
#define __SESSIONHOST_H__


#include "stringmap.h"


class TraceSink;
class MTrace;
class TraceClient;
class SessionClient;


//
//: Top level resource manager class.  
//
// SessionHost is the top class in a hierarchy of resource management
// classes.  For every class in this hierarchy, there is a dependent
// "client" class.  The idea is that there will be one SessionHost (or
// child) per process, and that classes that need to access external
// resources will be descended from the SessionHost (or child) class.
// The paired class is constructed with a reference to the Host
// class.  Internally, references to the managed resources are
// assigned from the Host class, (to which the client has "friend"
// access.)
//
// That allows a child class (such as the individual servlet
// transaction classes) to access the managed resources while
// maintaining the absolute minimum decoupling possible from the
// administratoin of those resources.
//
// One simple case is the configuration file.  The configuration
// values need to be accessed throughout the process, but the file
// should only be loaded once.  Therefore, the SessionHost constructor
// takes the name of the configuration file, loads and parses it.  The
// SessionClient class (of which the servlets are "grandchildren")
// Takes a reference to SessionHost in the constructor, and assigns an
// internal reference to the string table.  From there, the servlet
// class itself has a simple reference to a string table, rather than
// worrying about where the file came from and the parsing of the text
// file itself.
//

class SessionHost
{
  friend SessionClient;

public:

  //
  //: Constructor, taking the name of the configuration file to load
  //

  SessionHost(const char* configfile_);
  
  //
  //: virtual destructor
  //

  virtual ~SessionHost();

protected:


  //
  //: Execute "initialize" methods in a meaningful order
  //

  void startup();

  //
  //: Execute "shutdown*" methods in the reverse order that the
  //: corresponding "initialize" methods were called in the
  //: constructor.
  //

  void shutdown();


  //
  //: Generate a "ping" message to the initialized trace system
  //: with the contents of the environment hash.
  //

  virtual void initialTrace();


  //
  //: Load the local environment string map
  //
  void initializeEnvironment(const char*);

  //
  //: Delete the local string table object
  //

  void shutdownEnvironment();

  //
  //: Pointer to an owned string table containing the parsed
  //: configuration file entries.
  //

  MStringString* _environment;

  //
  //: flag indicating success of the initializeEnvironment method
  //

  bool _validEnvironment;

  //
  //: Initialize the tracing and logging system.
  //
  // - Create the trace sink object.
  //
  // - Create the MTrace server class for this process
  //
  // - set trace options specified in avisservlets.prf 
  //
  
  void initializeTraceServer();

  //
  //: Shut down the tracing system for this process
  //
  // Delete the trace server object
  // Delete the Trace Sink object
  //

  void shutdownTraceServer();

  //
  //: Parse through the configuration options, and build the
  //: "TraceMode" field, represented as an int of or'ed bitflags
  //

  unsigned int buildTraceMode() const;

  //
  //: Set the tracing options based on the parsed configuration file
  //

  void setTraceSwitches(MTrace* trace_server_) const;

  //
  //: set the "verbosity" of the trace log (unimplemented as of this
  //: writing)
  //

  void setTraceVerbosity(MTrace* trace_server_) const;

  //
  //: MTrace Server object for the entire process
  //

  MTrace* _trace_server;

  //
  //: Trace Sink file object for the entire process
  //

  TraceSink* _trace_sink;

  //
  //:  flag indicating success of trace system initialization 
  //

  bool _validTraceServer;


private:
  
  //
  //: Standard Trace object
  //
  // Note that this is not created in the initializeTraceServer method
  // because it is local to this class and not considered a
  // process-shared resource.  It's created and initialized in the
  // constructor, after the trace system has been kicked off.
  //

  TraceClient* _tracer;
};
#endif // __SESSIONHOST_H__ Sentinel
