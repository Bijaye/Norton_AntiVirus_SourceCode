// traceclient.cpp - Implementation for the... cumon, you can get it.
// Bingo!  the TraceClient class.  See you're getting so good at this. :)


#include "stdafx.h"
#pragma  warning (disable : 4251)
#include "traceclient.h"

// This is screwey because of the reference
TraceClient::TraceClient(MTrace&      server_,
                         const char*  module_,
                         bool         traceon_
                         )
  :_traceOn(traceon_),
   _module(module_),
   _traceServer(server_)
{
  _lt_flags.reserve(8); // Number of flags.
  _lt_flags[MTrace::lt_message]  = true;
  _lt_flags[MTrace::lt_enter]    = true;
  _lt_flags[MTrace::lt_exit]     = true;
  _lt_flags[MTrace::lt_debug]    = true;
  _lt_flags[MTrace::lt_info]     = true;
  _lt_flags[MTrace::lt_warning]  = true;
  _lt_flags[MTrace::lt_error]    = true;
  _lt_flags[MTrace::lt_critical] = true;
  // Surprise, no dynamics
}

void TraceClient::startTrace()
{
    _traceOn = true;
}

void TraceClient::stopTrace()
{
    _traceOn = false;
}

void TraceClient::switchOnLineType(MTrace::line_type lt_)
{
  _lt_flags[lt_] = true;
}
void TraceClient::switchOffLineType(MTrace::line_type lt_)
{
  _lt_flags[lt_] = false;
}
void TraceClient::sendMessage(const std::string& text_, MTrace::line_type ltype_)
{
  if (_lt_flags[ltype_])
    {
      if (_traceOn && (&_traceServer != 0))
        _traceServer.msg(_module.c_str(),text_.c_str(),ltype_);
    }
}
