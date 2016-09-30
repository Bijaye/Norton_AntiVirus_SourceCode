// MTrace.cpp - Implementation details for the MTrace class

#include "stdafx.h"

#ifdef _WIN32
#pragma warning (disable : 4251)
#pragma warning (disable : 4786)

#include <winbase.h>

#endif // _WIN32

//#include <exception>
#include <string.h>
#include <stdlib.h>
#include <time.h>
//  #include <fstream>

// Library Headers
// Project Headers

#include "mtrace.h"
#include "tracesink.h"
#include "strutil.h" // for the itoa stuff

// Publics
MTrace::MTrace(TraceSink& sink_,
               unsigned int      tmode_,
               trace_verbosity tgran_,
               const char*       fielddelimiter_)
  :_tmode(tmode_),
   _sink(sink_),
   _delimiter(fielddelimiter_),
   _verbosity(tgran_),
   _traceon(false),
   _linenum(0)
{
  _lt_flags.reserve(8); // Number of flags
  _lt_flags[lt_message]  = true;
  _lt_flags[lt_enter]    = true;
  _lt_flags[lt_exit]     = true;
  _lt_flags[lt_debug]    = true;
  _lt_flags[lt_info]     = true;
  _lt_flags[lt_warning]  = true;
  _lt_flags[lt_error]    = true;
  _lt_flags[lt_critical] = true;

  // Set the default sinks:
  _lt_sinks.reserve(8);
  _lt_sinks[ lt_message  ] = &sink_;
  _lt_sinks[ lt_enter    ] = &sink_;
  _lt_sinks[ lt_exit     ] = &sink_;
  _lt_sinks[ lt_debug    ] = &sink_;
  _lt_sinks[ lt_info     ] = &sink_;
  _lt_sinks[ lt_warning  ] = &sink_;
  _lt_sinks[ lt_error    ] = &sink_;
  _lt_sinks[ lt_critical ] = &sink_;

  startTrace();
}

// Destructor
MTrace::~MTrace()
{
  stopTrace();
}
void MTrace::switchOnLineType(line_type lt_)
{
  _lt_flags[lt_] = true;
}
void MTrace::switchOffLineType(line_type lt_)
{
  _lt_flags[lt_] = false;
}

void MTrace::setLineTypeSink(line_type lt_,TraceSink* new_sink_)
{
  _lt_sinks[ lt_ ] = new_sink_;
}

// Other Public Methods
void MTrace::addTraceLine(const char *sSource_,const char *sText_,line_type eLineType_,long code_)
{
   if (_traceon && _lt_flags[eLineType_])
    {
      _linenum++;
      sendTraceLine(eLineType_,buildTraceLine(sSource_,sText_,eLineType_,code_).c_str());
    }
}

void MTrace::startTrace()
{
  _traceon = true;
}
void MTrace::stopTrace()
{
  _traceon = false;
}

void MTrace::set_module_mode(const char* source_,
                             unsigned int tmode_)
{
  _module_modes[source_] = tmode_;
}
void MTrace::setTraceMode(unsigned int eTraceMode_)
{
  _tmode = eTraceMode_;
}
void MTrace::msg(const char* source_,const char* sText_,line_type eLineType_,long code_)
{
  addTraceLine(source_,sText_,eLineType_,code_);
}

void MTrace::enter(const char* source_,const char* sMethod_,long code_)
{
  addTraceLine(source_,sMethod_,lt_enter,code_);
}
void MTrace::exit(const char* source_,const char* sMethod_,long code_)
{
  addTraceLine(source_,sMethod_,lt_exit,code_);
}

std::string MTrace::buildTraceLine(const char* source_,
                                   const char* text_,
                                   line_type   ltype_,
                                   long        code_)
{
  // These correspond to the "line_type" severity codes 0-7
  const char* _severity[] = { "MSG",  // lt_message
                              "ENT",  // lt_enter
                              "EXT",  // lt_exit
                              "DBG",  // lt_debug
                              "INF",  // lt_info
                              "WRN",  // lt_warning
                              "ERR",  // lt_error
                              "CRT"}; // lt_critical

  std::string _traceLine("");
  unsigned int _local_mode = _tmode;
  if (_module_modes.find(source_) != _module_modes.end())
    _local_mode = _module_modes[source_];

  //  Line Number
  if ( _local_mode & TRACE_LNUMS)
    {
      _traceLine += appendLineNumber();
      _traceLine += _delimiter;
    }
  // Thread ID
  if (_local_mode & TRACE_THREADID)
    {
      _traceLine += appendThreadId();
      _traceLine += _delimiter;
    }
  
  // Date Stamp
  if (_local_mode & TRACE_DSTAMP)
    {
      _traceLine += appendDateStamp();
      _traceLine += _delimiter;
    }
  // Time Stamp
  if (_local_mode & TRACE_TSTAMP)
    {
      // appendTimeStamp takes a boolean to indicate whether or not
      // to add "millisecond" level timing to the timestamp.
      _traceLine += appendTimeStamp( (_local_mode & TRACE_FINETIME) );
      _traceLine += _delimiter;
    }

  // Severity token
  if ( _local_mode & TRACE_SEVERITY)
    {
      _traceLine += _severity[ltype_];
      _traceLine += _delimiter;
    }

  // Source String
  if (_local_mode & TRACE_SOURCE)
     {
      _traceLine += source_;
      _traceLine += _delimiter;
    }

  // Return code
  if (_local_mode & TRACE_CODE)
    {
      _traceLine += StringUtil::itos(code_); 
      _traceLine += _delimiter;
    }
  
  // Message Append flag
  if (_local_mode & TRACE_MESSAGE) 
    {
      _traceLine += text_;
    }
  // the line is build completed
  return _traceLine;
}

std::string MTrace::appendLineNumber()
{
  std::string _return("");
  char _sLineNumber[16];
  ltoa(_linenum,_sLineNumber,10);
  return (_return = _sLineNumber);
}

std::string MTrace::appendTimeStamp(bool fine_time_)
{
  std::string _return("");
  char _sTime[9];
  _strtime(_sTime);
  _return = _sTime;
  if (fine_time_)
    {
      // winbase.h
      SYSTEMTIME _sys_time;
      GetLocalTime(&_sys_time);
      _return.append(".");
      std::string _fine_time(StringUtil::ltos(_sys_time.wMilliseconds));
      StringUtil::padLeft(_fine_time,'0',3);
      _return.append(_fine_time);
    }
  return _return;
}
std::string MTrace::appendDateStamp()
{
  // Return "MM-DD"
  std::string _return("");
  
  struct tm *newtime = 0;
  long long_time     = 0;
  time( &long_time );                // Get time as long integer.
  newtime = localtime( &long_time ); // Convert to local time.
  
  // Add the month (it comes back zero relative)
  
  
  if ((newtime->tm_mon + 1) < 10)
    _return += "0";
  _return += StringUtil::itos(newtime->tm_mon + 1);
  
  _return += "-";

  // Add the day

  if (newtime->tm_mday < 10)
    _return += "0";
  _return += StringUtil::itos(newtime->tm_mday);
  return _return;
}


std::string MTrace::appendThreadId()
{
  std::string _return("");
  DWORD threadid = 0;
  threadid = GetCurrentThreadId();
  _return = StringUtil::ltos((unsigned long)threadid);
  return _return;
}
void MTrace::sendTraceLine(line_type lt_,const char* linetext_)
{
  _lt_sinks[lt_]->sendTraceLine(linetext_);
  //    _sink.sendTraceLine(linetext_);
}

void MTrace::setVerbosity(trace_verbosity verb_)
{
  _verbosity = verb_;
}
