//
// GwapiProxy module
//

#include "gwapiprx.h"
#include "htapi.h" // ------ web server header

#include <string>

#include "utilexception.h"

#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"


GwapiProxy::GwapiProxy(const unsigned char* handle,MTrace& trace_server_)
  :_handle(0),
   _tracer(0)
{
  _tracer = new TraceClient(trace_server_,"GwapiProxy");
  // Remove the stack tracing from the GwapiProxy class entirely
  _tracer->switchOffLineType(MTrace::lt_enter);
  _tracer->switchOffLineType(MTrace::lt_exit);
  //
  _handle = new unsigned char[ strlen((const char*)handle) + 1];
  strcpy((char*)_handle,(char*)handle);
  //  msg("constructor");
}
GwapiProxy::~GwapiProxy()
{
  //  msg("destructor");
  if (_handle)
    delete[] _handle;
  _handle = 0;
  delete _tracer;
  _tracer = 0;
}


bool GwapiProxy::validObject() const
{
  bool _vo = false;
  // almost no tests are required here
  // for the api to function.
  // The only thing I require is that the handle
  // be valid
  _vo = (_handle != 0);
  return (_vo);
}

// Value-add interface

// Stock API encapsulators
long GwapiProxy::authenticate()
{
  StackTrace _stack(*_tracer,"authenticate");
  long retval = 0;
  HTTPD_authenticate(_handle,&retval);
  return retval;
}

long GwapiProxy::extract(unsigned char *name,         /* i; name of the value to extract */
                         unsigned long *name_length,  /* i; length of the name */
                         unsigned char *value,        /* o; buffer in which to place the value */
                         unsigned long *value_length) /* i/o; size of buffer/length of value */
{
  StackTrace _stack(*_tracer,"extract");
  long retval = 0;
  HTTPD_extract(_handle,name,name_length,value,value_length,&retval);
  return retval;
}
/* set the value of a variable associated with this request     */
/* the available variables are the same as CGI                  */
/* - note that one can create variables with this function      */
/* - if created variables are prefixed by "HTTP_", they will    */
/*   be sent as headers in the response, without the "HTTP_"    */
/*   prefix                                                     */
/* valid in all steps, though not all variables are             */
long GwapiProxy::set(
                     unsigned char *name,         /* i; name of the value to set */
                     unsigned long *name_length,  /* i; length of the name */
                     unsigned char *value,        /* o; buffer which contains the value */
                     unsigned long *value_length) /* i; length of value */
{
  StackTrace _stack(*_tracer,"set");
  long retval = 0;
  HTTPD_set(_handle,name,name_length,value,value_length,&retval);
  return retval;
}


/* send a file to satisfy this request */
/* valid only in PreExit, Service, NameTrans, Error and Datafilter steps  */
long GwapiProxy::file(
                      unsigned char *name,         /* i; name of file to send */
                      unsigned long *name_length)  /* i; length of the name */
{
  StackTrace _stack(*_tracer,"file");
  long retval = 0;
  HTTPD_file(_handle,name,name_length,&retval);
  return retval;
}

/* execute a script to satisfy this request */
/* valid only in PreExit, Service, NameTrans and Error steps  */

long GwapiProxy::exec(
                      unsigned char *name,         /* i; name of script to execute */
                      unsigned long *name_length)  /* i; length of the name */
{
  StackTrace _stack(*_tracer,"file");
  long retval = 0;
  HTTPD_exec(_handle,name,name_length,&retval);
  return retval;
}

/* read the body of the client's request - use set/extract for headers */
/* Keep reading until HTTPD_EOF is returned; 4k is a good buffer size  */
/* valid only in the PreExit, Service, and Datafilter steps  */
long GwapiProxy::read(
                      unsigned char *value,        /* i; buffer in which to place the value */
                      unsigned long *value_length) /* i/o; size of buffer/length of header */
{
  StackTrace _stack(*_tracer,"read");
  long retval = 0;
  HTTPD_read(_handle,value,value_length,&retval);
  return retval;
}


/* write the body of the response - use set/extract for headers */
/* valid only in PreExit, Service, NameTrans, Error and Datafilter steps  */

long GwapiProxy::write(
                       unsigned char *value,        /* i; data to send */
                       unsigned long *value_length) /* i; length of the data */
{
  StackTrace _stack(*_tracer,"write");
  long retval = 0;
  HTTPD_write(_handle,value,value_length,&retval);
  return retval;
}

/* write a string to the server's error log */
/* valid in all steps */

long GwapiProxy::log_error(
                           unsigned char *value,        /* i; data to write */
                           unsigned long *value_length) /* i; length of the data */
  
{
  // This is pretty funny.
  StackTrace _stack(*_tracer,"log_error");

  long retval = 0;
  HTTPD_log_error(_handle,value,value_length,&retval);
  return retval;
}
/* write a string to the server's trace log */
/* valid in all steps */

long GwapiProxy::log_trace(
                           unsigned char *value,        /* i; data to write */
                           unsigned long *value_length) /* i; length of the data */
{
  StackTrace _stack(*_tracer,"log_trace");
  long retval = 0;
  HTTPD_log_trace(_handle,value,value_length,&retval);
  return retval;
}

/* restart the server after all active requests have been processed */
/* valid in all steps except ServerInit and ServerTerm */
long GwapiProxy::restart()
{
  StackTrace _stack(*_tracer,"restart");
  long retval = 0;
  HTTPD_restart(&retval);
  return retval;
}

/* make a proxy request */
/* valid in PreExit and Service steps  */
/* Note: This is a completion function; i.e, the response is complete after this */
long GwapiProxy::proxy(
                       unsigned char *url_name,     /* i; url to which the proxy request is made */
                       unsigned long *name_length,  /* i; length of the url */
                       unsigned char *request_body, /* i; body of the request */
                       unsigned long *body_length)  /* i; length of the body */
{
  StackTrace _stack(*_tracer,"proxy");
  long retval = 0;
  HTTPD_proxy(_handle,url_name,name_length,request_body,body_length,&retval);
  return retval;
}

/* get the attributes of a file */
/* valid in all steps */

long GwapiProxy::attributes(
                            unsigned char *name,         /* i; name of the file */
                            unsigned long *name_length,  /* i; length of the name */
                            unsigned char *value,        /* o; buffer which contains the attributes */
                            unsigned long *value_length) /* i/o; size of buffer/length of attributes */
{
  StackTrace _stack(*_tracer,"attributes");
  long retval = 0;
  HTTPD_attributes(_handle,name,name_length,value,value_length,&retval);
  return retval;
}

/* reverse translate a file system object to a URL */
/* valid in all steps */

long GwapiProxy::reverse_translate(
                                   unsigned char *name,         /* i; name of the file system object */
                                   unsigned long *name_length,  /* i; length of the name */
                                   unsigned char *value,        /* o; buffer which contains the URL */
                                   unsigned long *value_length) /* i/o; size of buffer/length of URL */
{
  StackTrace _stack(*_tracer,"reverse_translate");
  long retval = 0;
  HTTPD_reverse_translate(_handle,name,name_length,value,value_length,&retval);
  return retval;
}
/* translate a URL */
/* valid in all steps */
long GwapiProxy::translate(
                           unsigned char *name,         /* i; name of the URL */
                           unsigned long *name_length,  /* i; length of the name */
                           unsigned char *url_value,    /* o; buffer which contains the translated URL */
                           unsigned long *url_value_length, /* i/o; size of buffer/length of translated URL */
                           unsigned char *path_trans,   /* o; buffer which contains PATH_TRANSLATED */
                           unsigned long *path_trans_length, /* i/o; size of buffer/length of PATH_TRANSLATED */
                           unsigned char *query_string, /* o; buffer which contains QUERY_STRING */
                           unsigned long *query_string_length) /* i/o; size of buffer/length of QUERY_STRING */
{
  StackTrace _stack(*_tracer,"translate");
  long retval = 0;
  HTTPD_translate(_handle,name,name_length,url_value,url_value_length,
                  path_trans,path_trans_length,query_string,query_string_length,
                  &retval);
  return retval;
}


long GwapiProxy::supply_label(
                              
    unsigned char *value,        /* i; PICS label being supplied */
    unsigned long *value_size)   /* i; length of the PICS label */

{
  StackTrace _stack(*_tracer,"supply_label");
  long retval = 0;
  HTTPD_supply_label(_handle,value,value_size,&retval);
  return retval;
}

// value-add overloads
long GwapiProxy::set( unsigned char* name_,unsigned char* value_)
{
  StackTrace _stack(*_tracer,"set");
  long retval = 0;
  unsigned long _namelen  = strlen((const char*)name_);
  unsigned long _valuelen = strlen((const char*)value_);
  retval = set(name_,&_namelen,value_,&_valuelen);
  return retval;
}

long GwapiProxy::extract(unsigned char* name_,unsigned char* value_,unsigned long* value_length_)
{
  StackTrace _stack(*_tracer,"extract");
  long retval = 0;
  unsigned long _namelen = strlen((const char*)name_);
  retval =  (extract(name_,&_namelen,value_,value_length_));
  return retval;
}

long GwapiProxy::file(unsigned char* name_)
{
  StackTrace _stack(*_tracer,"file");
  long retval = 0;
  unsigned long _namelen = strlen((const char*)name_);
  retval =  file(name_,&_namelen);
  return retval;
}
long GwapiProxy::write(unsigned char* value_)
{
  StackTrace _stack(*_tracer,"write");
  long retval = 0;
  unsigned long _valuelen = strlen((const char*)value_);
  retval =  write(value_,&_valuelen);
  return retval;
}

long GwapiProxy::exec(unsigned char* name_)
{
  StackTrace _stack(*_tracer,"exec");
  unsigned long _namelen = strlen((const char*)name_);
  long retval = exec(name_,&_namelen);
  return retval;
}

long GwapiProxy::log_error(unsigned char* value_)
{
  StackTrace _stack(*_tracer,"log_error");
  unsigned long _valuelen = strlen((const char*)value_);
  long retval =  log_error(value_,&_valuelen);
  return retval;
}
long GwapiProxy::log_trace(unsigned char* value_)
{
  StackTrace _stack(*_tracer,"log_trace");
  unsigned long _valuelen = strlen((const char*)value_);
  long retval =  log_trace(value_,&_valuelen);
  return retval;
}
long GwapiProxy::proxy(unsigned char* url_name_,unsigned char* request_body_)
{
  StackTrace _stack(*_tracer,"proxy");
  unsigned long _url_name_length = strlen((const char*)url_name_);
  unsigned long _request_body_length = strlen((const char*)request_body_);
  long retval =  proxy(url_name_,&_url_name_length,request_body_,&_request_body_length);
  return retval;
}
long GwapiProxy::attributes(unsigned char* name_,unsigned char* value_,unsigned long* value_length_)
{
  StackTrace _stack(*_tracer,"attributes");
  unsigned long _namelen = strlen((const char*)name_);
  long retval =  attributes(name_,&_namelen,value_,value_length_);
  return retval;
}

long GwapiProxy::reverse_translate(unsigned char *name_,        
                                   unsigned char *value_,       
                                   unsigned long *value_length_)
{
  StackTrace _stack(*_tracer,"reverse_translate");
  unsigned long _namelen = strlen((const char*)name_);
  long retval =  reverse_translate(name_,&_namelen,value_,value_length_);
  return retval;
}

long GwapiProxy::supply_label(unsigned char *value_)
{
  StackTrace _stack(*_tracer,"supply_label");
  unsigned long _valuelen = strlen((const char*)value_);
  long retval  =  supply_label(value_,&_valuelen);
  return retval;
}











