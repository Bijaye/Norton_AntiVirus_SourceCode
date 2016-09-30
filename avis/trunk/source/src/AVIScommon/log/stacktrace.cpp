// stacktrace.cpp - StackTrace implementation

#include "stdafx.h"
#pragma warning (disable : 4251)
#include "stacktrace.h"

// Here's the man behind the curtains.  
// As you can see, nothing too fancy.

StackTrace::StackTrace(TraceClient& tracer_,const char* method_)
  :_tracer(tracer_),
   _method(method_)
{
  _tracer.enter(method_);
}

StackTrace::~StackTrace()
{
  _tracer.exit(_method);
}


void StackTrace::msg      ( const std::string& text_   )
{ _tracer.msg      ( buildMessage( text_ )); };
void StackTrace::enter    ( const std::string& method_ )
{ _tracer.enter    ( method_             ) ; };
void StackTrace::exit     ( const std::string& method_ )
{ _tracer.exit     ( method_             ) ; };
void StackTrace::debug    ( const std::string& text_   )
{ _tracer.debug    ( buildMessage( text_ )); };
void StackTrace::info     ( const std::string& text_   )
{ _tracer.info     ( buildMessage( text_ )); };
void StackTrace::warning  ( const std::string& text_   )
{ _tracer.warning  ( buildMessage( text_ )); };
void StackTrace::error    ( const std::string& text_   )
{ _tracer.error    ( buildMessage( text_ )); };
void StackTrace::critical ( const std::string& text_   )
{ _tracer.critical ( buildMessage( text_ )); };

std::string StackTrace::buildMessage(const std::string& text_) const
{
  std::string _msg(_method);
  _msg += " ";
  _msg += text_;
  return _msg;
}

