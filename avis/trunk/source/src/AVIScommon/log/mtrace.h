// mtrace.h - Prototype for the MTrace class

#if !defined (__MTRACE_H__)
#define __MTRACE_H__

// Standard Headers
#include <string>
#include <vector>
#include <map>
#include "aviscommon.h"

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4251 )
// These are the field inclusion flags. 
// They can be or'ed together  

const unsigned int TRACE_NULL     = 0x0000; // Nothing
const unsigned int TRACE_LNUMS    = 0x0001; // Include line numbers
const unsigned int TRACE_DSTAMP   = 0x0002; // include date stamp
const unsigned int TRACE_TSTAMP   = 0x0004; // include time stamp
const unsigned int TRACE_SEVERITY = 0x0008; // include severity token
const unsigned int TRACE_SOURCE   = 0x0010; // include source name
const unsigned int TRACE_CODE     = 0x0080; // include user code
const unsigned int TRACE_MESSAGE  = 0x0100; // include user message
const unsigned int TRACE_THREADID = 0x0200; // include thread id (yuk)
const unsigned int TRACE_FINETIME = 0x0400; // include millisecond timing
// Here are some predefined configurations
const unsigned int MODE_NORMAL    = 0x031e;
const unsigned int MODE_MAX       = 0x07ff; // For the information junkie in us all

// Forward Declarations
class NamedMutex;
class TraceClient;
class TraceSink;

class AVISCOMMON_API MTrace 
{
public:
  
  friend TraceClient;
  
  // These are indexed into a const char* array off in the cpp file
  enum line_type 
  {
    lt_message  = 0, // MSG
    lt_enter    = 1, // ENT
    lt_exit     = 2, // EXT
    lt_debug    = 3, // DEB
    lt_info     = 4, // INF
    lt_warning  = 5, // WAR
    lt_error    = 6, // ERR
    lt_critical = 7  // CRI
  };

  //: trace_granularity is not yet functional.
  // But it's in the interface so use at will.
  enum trace_verbosity
  {
    tv_terse,
    tv_verbose,
    tv_exhaustive
  };
  
  // Public Constructors & Destructor
  
  MTrace(TraceSink& sink_,                          // Trace Message Destination
         unsigned int tmode_         = MODE_NORMAL, // flags (see consts above)
         trace_verbosity tgran_      = tv_verbose,  // "verbosity" (not implemented)
         const char* fielddelimiter_ = " ");        // trace record field delim

  virtual ~MTrace();

  // Here's a couple functions people are all hot & bothered for
  // Note that these switches pertain to all trace messages
  // there are similar routines in the TraceClient class to switch
  // message types from an individual module.  Rather cool if you
  // ask me. (Not that you did.)

  void switchOnLineType(line_type lt_);
  void switchOffLineType(line_type lt_); 
  void setLineTypeSink(line_type lt_,TraceSink* new_sink_);


  // Set Module mode (overriding defaults)
  void set_module_mode(const char* source_,unsigned int tmode_);

  // Specify the mode
  void setTraceMode(unsigned int eTraceMode_);
  // Verbosity
  void setVerbosity(trace_verbosity verb_);


  //: add a trace/log message.  
  void msg   ( const char* source_,
               const char* sText_,
               line_type eLineType_ = lt_info,
               long        code_ = 0);

  // enter & exit are used for tracing entry into and exit from functions.

  void enter ( const char* source_,
               const char* sMethod_,
               long        code_ = 0);

  void exit  ( const char* source_,
               const char* method_,
               long        code_ = 0);

protected:

  // Configure the trace record, field delimiter
  // If you want to post-process log files, it's 
  // useful to set the delimiter to "|" or something similar.
  void setFieldDelimiter(const char* newDelimiter_);

  // Other Public Methods

  // Start trace just sets the current trace mode MODE_NORMAL
  // (this could probably go in a macro as a wrapper to 
  //  setTraceMode)

  void startTrace();
  // "pipe to /dev/null" 
  // (again, this just wraps setTraceMode one level)
  void stopTrace();

private:

   void addTraceLine(const char *sSource_, 
                     const char *sText_,
                     line_type eLineType_ = lt_info , 
                     long code_ = 0);
  
  // Build a complete trace log line item.  
  std::string buildTraceLine(const char* source_, 
                             const char* text_,
                             line_type   ltype_, 
                             long        code_);

  // Write the completed line to disk.  
  void sendTraceLine(line_type lt_,const char* linetext_);
  
  // Build atomic string units:
  std::string appendLineNumber();
  std::string appendTimeStamp(bool fine_time_);
  std::string appendDateStamp();
  std::string appendIndentation();
  std::string appendThreadId();
  
  // Internal data
  bool              _traceon;     // Simple "is this running" flag
  unsigned int      _tmode;       // Trace_Mode (flags for 'field inclusion')
  long              _linenum;     // trace line number
  trace_verbosity   _verbosity;   // Verbosity (not yet implemented)
  std::string       _delimiter;   // customizable field delimiter (defaults to " ")

  // Module mode overrides.  for changing the trace mode of individual
  // modules.  Note that this is a shallow container.  
  std::map<std::string,unsigned int> _module_modes;

  // There's probably a delightfully elegant way to do
  // this, I'm sure someone will point it out.
  std::vector<bool> _lt_flags;
  std::vector<TraceSink* > _lt_sinks;
  
  TraceSink& _sink;



};

#endif // __MTRACE_H__ Sentinel
