#pragma warning (disable : 4786)
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/mtrace.h"
#include "aviscommon/log/stacktrace.h"
#include "redirectorstate.h"
#include <stdlib.h>
#include <time.h>


RedirectorState::RedirectorState(MTrace& trace_server_,
                                 const MStringString& environment_)
  :_tracer(NULL)
{
  // Create the trace object
  _tracer = new TraceClient(trace_server_,
                            "RedirectorState");
  _tracer->debug("constructing");

  // Load the server lists
  load_server_lists(environment_);
}

RedirectorState::~RedirectorState()
{
  _tracer->debug("deconstructing");
  // Empty out the lists & null the iterators

  // delete the trace object
  delete _tracer;
  _tracer = NULL;
}


const std::string RedirectorState::get_definition_server()
{
  StackTrace _stack(*_tracer,"get_definition_server");

  std::vector<std::string>::size_type _index = 0;
  time_t _time = time(NULL);
  div_t _result = div(_time,_definition_server_list.size());
  _index = _result.rem;
  std::string _definition_server(_definition_server_list[_index]);

  std::string _message("returning definition server [");
  _message.append(_definition_server);
  _message.append("]");
  _stack.info(_message);

  return _definition_server;  
}

const std::string RedirectorState::get_sample_server()
{
  StackTrace _stack(*_tracer,"get_sample_server");
  std::vector<std::string>::size_type _index = 0;
  time_t _time = time(NULL);
  div_t _result = div(_time,_sample_server_list.size());
  _index = _result.rem;
  std::string _sample_server(_sample_server_list[_index]);

  std::string _message("returning sample server [");
  _message.append(_sample_server);
  _message.append("]");
  _stack.info(_message);
  return _sample_server;
}

void RedirectorState::load_server_lists(const MStringString& environment_)
{
  StackTrace _stack(*_tracer,"load_server_lists");
  // Iterate across all values, testing for redirector configs.
  MStringString::const_iterator _i = environment_.begin();

  for (_i = environment_.begin();_i != environment_.end();++_i)
    {
    
      const std::string& _key   = (*_i).first;
      const std::string& _value = (*_i).second;
      std::string _message;
      // Is this a Definition Server
      if (_key.find("redirectSamples") != std::string::npos)
        {
          _sample_server_list.push_back(_value);
          _message = "adding sample redirector [";
          _message.append(_value);
          _message.append("]");
          _stack.info(_message);
          
        }
      // Is it a Sample Server
      else if (_key.find("redirectSignatures") != std::string::npos)
        {
          _definition_server_list.push_back(_value);
          _message = "adding signature redirector [";
          _message.append(_value);
          _message.append("]");
          _stack.info(_message);

        }
      
    }
}
