// stacktrace.h - Prototype for the stack trace class
#if !defined (__STACKTRACE_H__)
#define __STACKTRACE_H__


#include "aviscommon.h"
#include "traceclient.h"


//: Trace client variant (not quite derivative yet)
//: used to trace context within a method call.
// All of these messages add the method name to the 
// beginning of the trace message.
// If you're not doing method-level tracking, feel
// free to dispense with this entirely and just use
// the TraceClient (which this just value-adds on
// top of anyway)
class AVISCOMMON_API StackTrace
{
public:
  StackTrace(TraceClient& tracer_,const char* method_);
  virtual ~StackTrace();

  // Rather than add "enum" parameters to one send-message
  // method, I figure I'll just do this.
  // The cost is really low, and it's a buttload more intuitive

  void msg      ( const std::string& text_   );
  void enter    ( const std::string& method_ );
  void exit     ( const std::string& method_ );
  void debug    ( const std::string& text_   );
  void info     ( const std::string& text_   );
  void warning  ( const std::string& text_   );
  void error    ( const std::string& text_   );
  void critical ( const std::string& text_   );
  //-----------------------------------------------------------------------
protected:
  std::string buildMessage(const std::string& text_) const;
private:
  
  TraceClient& _tracer;
  const char* _method;
  
};


#endif // __STACKTRACE_H__ Sentinel
