
// traceclient.h - Prototype for the.. you guessed it... TraceClient class
#if !defined (__TRACECLIENT_H__)
#define __TRACECLIENT_H__

#include "aviscommon.h"
#include "mtrace.h"

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4251 )
//class MTrace; // The "real" trace implementation.

class AVISCOMMON_API TraceClient
{
public:
  
  TraceClient(MTrace& server_,const char* module_,bool traceon_ = true);
  virtual ~TraceClient() {};

  void startTrace();
  void stopTrace();

  // These are the module local versions of the
  // line type flag switches
  // (I may want info messages in the business
  // classes, but I sure don't in the architecture
  // classes)
  void switchOnLineType(MTrace::line_type lt_);
  void switchOffLineType(MTrace::line_type lt_);

  // is to have a TraceInterface class with a bunch of pure
  // virts & make this an envelope/letter implementation.
  // But, pending alot of free time and magical ambition...
  // (The latter of which I seem to have received all of a sudden)

  // Rather than add "enum" parameters to one send-message
  // method, I figure I'll just do this.

  void msg      ( const std::string& text_   ) { sendMessage ( text_   , MTrace::lt_message  ); }; 
  void enter    ( const std::string& method_ ) { sendMessage ( method_ , MTrace::lt_enter    ); };
  void exit     ( const std::string& method_ ) { sendMessage ( method_ , MTrace::lt_exit     ); };
  void debug    ( const std::string& text_   ) { sendMessage ( text_   , MTrace::lt_debug    ); };
  void info     ( const std::string& text_   ) { sendMessage ( text_   , MTrace::lt_info     ); };
  void warning  ( const std::string& text_   ) { sendMessage ( text_   , MTrace::lt_warning  ); };
  void error    ( const std::string& text_   ) { sendMessage ( text_   , MTrace::lt_error    ); };
  void critical ( const std::string& text_   ) { sendMessage ( text_   , MTrace::lt_critical ); };
  //-----------------------------------------------------------------------

private:
  std::vector<bool> _lt_flags;

  // This is the actual proxy method.
  // It checks to see that the line_type is enabled for this module
  // then sends (or not) the message to the _traceServer object
  void sendMessage( const std::string& text_, MTrace::line_type ltype_);


  bool _traceOn;
  std::string _module;            // goes in each line
  MTrace& _traceServer;           // where is this all really going.

};

#endif // __TRACECLIENT_H__ Sentinel;
