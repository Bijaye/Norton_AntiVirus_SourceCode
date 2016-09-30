//
// GoSession module
//
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4251 )
#include "gosession.h"
#include "gwapiprx.h"
#include "strutil.h"
#include "stringmap.h"
#include "utilexception.h"

#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"

GoSession::GoSession(const unsigned char* handle_,MTrace& trace_server_)
  :_gwapi(0),
   _tracer(0),
   _contentEof(false)
{
  _gwapi  = new GwapiProxy  ( handle_,trace_server_);
  _tracer = new TraceClient ( trace_server_,"GoSession");
  _tracer->switchOffLineType(MTrace::lt_enter);
  _tracer->switchOffLineType(MTrace::lt_exit);
  init();
}
GoSession::~GoSession()
{
  if (_tracer)
    delete _tracer;
  if (_gwapi)
    delete _gwapi;
}
// Value-add interface

long GoSession::init(void)
{
  StackTrace _stack(*_tracer,"init");
  // Doesn't do anything yet.  I'm going to drop the 'get environment' 
  // call in here at least.
  long retval = 0;
  retval = getEnvironment();
  return retval;
}

//-------------------------------------------------------------------------
// Public interface
//-------------------------------------------------------------------------

//long GoSession::isContentAvailable()
//{
  // I'm not quite sure how to deal with this.
  // I have the "disparate buffer size reconciliation" problem
  // I could possibly defer the handling down the component line.
  // 
//}

long GoSession::sendContent(const char* buffer_,long buffersize_)
{
  StackTrace _stack(*_tracer,"sendContent");
  setSystemHeader("Content-Length",StringUtil::ltos(buffersize_).c_str());
  
  long _retval = _gwapi->write((unsigned char*)buffer_,(unsigned long*)&buffersize_);

  std::string _message = "Content sent: ";
  _message += StringUtil::ltos(buffersize_);
  _message += " bytes";
  _stack.debug(_message.c_str());
  return _retval;

}
long GoSession::getContentChunk(unsigned char* buffer_,long bufferSize_)
{
  //  StackTrace _stack(*_tracer,"getContentChunk");
  // Returns the number of bytes filled in the buffer.
  long _retval = bufferSize_;
  _contentEof = (_gwapi->read(buffer_,(unsigned long*)&_retval) == GwapiProxy::_HTTPD_EOF);
  return _retval;
}

void GoSession::setResponseHeaders(const MStringString& newHeaders_)
{
  StackTrace _stack(*_tracer,"setResponseHeaders");
  // And you thought this would be a straight assign.
  // We have to munge the header block.  
  // How much fun is this?
  for (MStringString::const_iterator it = newHeaders_.begin();
       it != newHeaders_.end();
       it++)
    {
      const std::string& _key = (*it).first;
      const std::string& _value = (*it).second;
      // Don't dump headers that don't have values.
      if ( _value.length())
        {
          // This is the method that actually does the work
          // of munging the header and passing it into the callback
          setResponseHeader(_key,_value);
        }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Protected methods
//-------------------------------------------------------------------------
long GoSession::getEnvironment()
{
  StackTrace _stack(*_tracer,"getEnvironment");
  //-----------------------------------------------------------------------
  // fill the _environment map
  //-----------------------------------------------------------------------
  long retval = 0;
  unsigned char* _allvars = (unsigned char*)"ALL_VARIABLES";
  
  //-----------------------------------------------------------------------
  // A very simple 128 character buffer
  //-----------------------------------------------------------------------
  unsigned char* _buffer = (unsigned char*) new char[128];
  unsigned long  _bufferSize = ( 128 * sizeof(char));
  //-----------------------------------------------------------------------
  _buffer[0] = '\0';

  //-----------------------------------------------------------------------
  // The first call to extract, I call with a 0 length buffer.
  // extract will then return HTTPD_BUFFER_TOO_SMALL in the error code
  // and the minimum required buffer size in the &value_length parameter.
  // Then I allocate the appropriate buffer size, and recall extract.
  //-----------------------------------------------------------------------
  retval = _gwapi->extract(_allvars,_buffer,&_bufferSize);
  //-----------------------------------------------------------------------

  //-----------------------------------------------------------------------
  // This had better be true!  (Otherwise, there's a teeny weensy env)
  //-----------------------------------------------------------------------
  if (retval == GwapiProxy::_HTTPD_BUFFER_TOO_SMALL)
    {
      //-----------------------------------------------------------------------
      // "\nSecond call to extract, buffer size = " << _bufferSize;
      //-----------------------------------------------------------------------
      // Allocate the appropriate size buffer;
      
      delete[] _buffer;
      _buffer = 0;
      _buffer = (unsigned char*)new char[(_bufferSize + 1)];

      // This is the line of code that was missing.
      memset(_buffer,'\0',(sizeof(char) * (_bufferSize + 1)));

      //_buffer = new char[_bufferCount + 1];
      
      retval = _gwapi->extract(_allvars,_buffer,&_bufferSize);

    }
  //-----------------------------------------------------------------------

  //-----------------------------------------------------------------------
  // If the extract was successful, fill the hash table
  // Otherwise, we'll have the calling method deal with whatever the error was
  //-----------------------------------------------------------------------
  if (retval == GwapiProxy::_HTTPD_SUCCESS)
    {
      std::string _strbuffer((const char*)_buffer);
      translateEnvironment(_strbuffer);
    }

  //-----------------------------------------------------------------------
  if (_buffer)
    delete[] _buffer;

  return retval;
}

void GoSession::translateEnvironment(const std::string& buffer_)
{
  StackTrace _stack(*_tracer,"translateEnvironment");

  _environment = StringMapUtil::fromGoEnvironment(buffer_);
  MStringString::const_iterator _it = _environment.end();
    
  for (_it = _environment.begin();_it != _environment.end(); _it++)
    {
      // Add it to the request header space
      setRequestHeader((*_it).first.c_str(),(*_it).second.c_str());
#if 0
      std::string _debug("GOHEADER ");
      _debug 
        << (*_it).first
        << ": "
        << (*_it).second;
      _stack.debug(_debug.c_str());
#endif
    }
}

const std::string GoSession::incomingHeader(const std::string& key_) const
{
  if (_requestHeaders.find(key_) == _requestHeaders.end())
    {
      throw InvalidHttpHeader(key_.c_str());
    }
  return (*(_requestHeaders.find(key_))).second;
}

void GoSession::debug_dumpEnvToContent()
{
#if defined (_DEBUG)
  std::string _buffer("");
  for (MStringString::const_iterator it = _environment.begin();
       it != _environment.end();
       it++)
    {
      _buffer += "\n[ ";
      _buffer += (*it).first;
      _buffer += " ] = ";
      _buffer += (*it).second;
    }
  _gwapi->write((unsigned char*)_buffer.c_str());
  _tracer->msg(_buffer.c_str());
#endif // _DEBUG
}

long GoSession::setContentType(const std::string& contentType_)
{
  StackTrace _stack(*_tracer,"setContentType");
  long _retval = setSystemHeader("Content-Type",contentType_);
  return _retval;
}
void GoSession::setRequestHeader(const std::string& name_,const std::string& value_)
{
  //  StackTrace _stack(*_tracer,"setRequestHeader");
  std::string _name(name_);
  if ( (_name.find("QUERY") == 0) ||  // Accept QUERY_STRING
       (_name.find("HTTP_") == 0) ||   // All HTTP headers
       (_name.find("HTTPS") == 0) ||   // All HTTPS headers
       (_name.find("SERVER_") == 0) ||  // All SERVER_ headers
       (_name.find("SCRIPT_") == 0) ||  // SCRIPT_ headers
       (_name.find("REQUEST_") == 0) || // Request Method
       (_name.find("CONTENT_TYPE") == 0) || // content type header (explicit)
       (_name.find("REMOTE_ADDR") == 0) || // Remote IP (included for debugging)
       (_name.find("CONTENT_LENGTH") == 0) // Content length header
       ) 
    {
      goDeMunge(_name);
      // Don't forget this is a simple internal stl map.  It's never
      // ever fed back into the go server.  So I don't need to do anything
      // fancy at all
      _requestHeaders[_name] = value_;
      // Tough huh.
    }
  else
    {
      _environment[name_] = value_;
    }
}
long GoSession::setResponseHeader(const std::string& name_,const std::string& value_)
{
  long _retval = -1;
  StackTrace _stack(*_tracer,"setResponseHeader");

  // Make sure it's not a system header
  // There are cases where a header block is iterated
  // across and "set".  Unfortunately, that includes
  // standard headers that shouldn't be set this
  // way.  So I use this conditional to bulletproof
  // that aspect of failure.
  if ( (name_.find("X") == 0) || (name_.find("x") == 0))
    {
      std::string _name(name_);

      std::string _message("OUTHEADER ");
      _message += name_;
      _message += ": ";
      _message += value_;
      _stack.info(_message);

      goMunge(_name);
      _retval = _gwapi->set((unsigned char*)(_name.c_str()),(unsigned char*)(value_.c_str()));
    }
  return _retval;
}
long GoSession::setDebugResponseHeader(const std::string& name_,const std::string& value_)
{
  long _retval = -1;
  std::string _name("X-Gateway-Debug-");
  _name += name_;
  setResponseHeader(_name,value_);
  return _retval;
}
long GoSession::setSystemHeader(const std::string& name_,const std::string& value_)
{
  StackTrace _stack(*_tracer,"setSystemHeader");
  std::string _message("OUTHEADER ");
  _message.append(name_);
  _message.append(": ");
  _message.append(value_);
  _stack.info(_message);
  long _retval = _gwapi->set((unsigned char*)(name_.c_str()),(unsigned char*)(value_.c_str()));
  return _retval;
}

std::string& GoSession::goMunge(std::string& name_) const
{
  //  StackTrace _stack(*_tracer,"goMunge");
  const std::string _prefix("HTTP_");
  if (strstr(name_.c_str(),"HTTP_") == 0)
    {
      StringUtil::repchr(name_,'-','_');
      name_ = _prefix + name_;
      //      name_ = "HTTP_" + name_;
    }
  return name_;
}

std::string& GoSession::goDeMunge(std::string& name_) const
{
  //  StackTrace _stack(*_tracer,"goDeMunge");
  if  (strstr(name_.c_str(),"HTTP_") != 0)
    {
      name_ = name_.c_str() + strlen("HTTP_");
    }
  if (name_ != "REQUEST_METHOD")
    {
      StringUtil::repchr(name_,'_','-');
    }

  return name_;
}

void GoSession::dispatchResponseHeaders()
{
  StackTrace _stack(*_tracer,"dispatchResponseHeaders");
  for ( MStringString::const_iterator _it = _responseHeaders.begin();
        _it != _responseHeaders.end();
        _it++)
    {
      setResponseHeader((*_it).first.c_str(),(*_it).second.c_str());
    }
}

bool GoSession::validObject() const
{
  bool _vo = false;
  // Perform invariant tests
  if (_gwapi->validObject())
    _vo = true;
  return (_vo);
}
