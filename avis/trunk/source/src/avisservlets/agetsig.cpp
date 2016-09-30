// agetsig.cpp - Implementation of the AvisGetSignature class

#include "agetsig.h"
#include "sigdlselect.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "strutil.h"
#include "utilexception.h"
#include "gosession.h"
#include "sigfilehnd.h"
#include "sigcache.h"
// The following two headers (sys/types.h & sys/stat.h) are required for
// the _stat function
#include <sys/types.h>
#include <sys/stat.h>

// AvisDb

AvisGetSignature::AvisGetSignature(GoServletHost& host_,const char* gwapihandle_)
  : GoServletClient(host_,gwapihandle_)	,
    _tracer(0)
{
  _tracer = new TraceClient(_trace_server,"AvisGetSignature");
  _tracer->debug("Created AvisGetSignature");
  // Add Required Headers
}
AvisGetSignature::~AvisGetSignature()
{
  _tracer->debug("Destroying AvisGetSignature");
  if (_tracer)
    delete _tracer;
}

// Interface implementation
void AvisGetSignature::setRequiredHeaders()
{
  StackTrace _stack(*_tracer,"setRequiredHeaders");
}
int AvisGetSignature::verifyHeaderContent()
{
  StackTrace _stack(*_tracer,"verifyHeaderContent");
  int _retval = 1;
  return _retval;
}
int AvisGetSignature::fulfillRequest()
{
  StackTrace _stack(*_tracer,"fulfillRequest");
  int _retval = 204; // 
  _retval = liveProcessing();
  return _retval;
}

int AvisGetSignature::liveProcessing()
{
  StackTrace _stack(*_tracer,"liveProcessing");
  int _retval = 204;
  //----------------------------------------------------------------------
  // This the new object that performs all the database selects with
  // cached (pooled) database connection handles.'
  // By the time this object is fully instantiated, the handle has
  // been released back to the pool. (i.e. all SQL is performed inside 
  // the stack from the constructor itself.) All "possible" database
  // data that may be required for a signature download response (HEAD
  // or full GET) is pulled into the SignatureDownloadSelect members
  // and exposed through rather unexciting accessor methods
  
  //----------------------------------------------------------------------
  // The SignatureDownloadSelect object is constructed with a
  // reference to the database connection pool, and the string passed
  // in by the client in the X-Signature-Sequence header
  //----------------------------------------------------------------------
  std::string  _sequence(_gwapi.incomingHeader("X-Signatures-Sequence"));
  SignatureDownloadSelect _siginfo(_trace_server,
                                   _environment["signatureDirectory"],
                                   _connection_pool,
                                   _sequence);
  //----------------------------------------------------------------------

  //----------------------------------------------------------------------
  if (_siginfo.availability() == SignatureDownloadSelect::SA_AVAILABLE)
    {  
      // Set the response headers
      // Are these "blessed" signatures?
      if (_siginfo.blessed_date().size())
        {
          _gwapi.setResponseHeader("X-Date-Blessed",_siginfo.blessed_date());
        }
      // set the published date
      _gwapi.setResponseHeader("X-Date-Published",_siginfo.published_date());
      // set the produced date
      _gwapi.setResponseHeader("X-Date-Produced",_siginfo.produced_date());
      
      // set the "proper" sequence number
      _gwapi.setResponseHeader("X-Signatures-Sequence",_siginfo.sequence_number());
      

      if (_gwapi.incomingHeader("REQUEST_METHOD") == "HEAD")
        {
          // Return headers only
          _tracer->info("Returning Headers Only");
          _retval = 204;
        }
      else
        {
          // Load the signature package content from the sig file cache
          SignatureFileHandle _sigfile(_sig_cache.get_signature_file(_siginfo.sequence_number()));

          // Set the appropriate response headers
          _gwapi.setResponseHeader("X-Content-Checksum",_sigfile.get_checksum());
          _gwapi.setResponseHeader("X-Checksum-Method","md5");
          _gwapi.setContentType(_sigfile.get_content_type());
      
          // Dispatch the file
          _gwapi.sendContent(_sigfile.get_content(),
                             _sigfile.get_content_length());
          _retval = 200;
        }
    }

  //----------------------------------------------------------------------
  // handling of the unfulfillable requests...
  //----------------------------------------------------------------------
  else if (_siginfo.availability() == SignatureDownloadSelect::SA_SUPERCEDED)
    {
      _gwapi.setResponseHeader("X-Error","superceded");
      _retval = 204;
    }
  //----------------------------------------------------------------------
  else if (_siginfo.availability() == SignatureDownloadSelect::SA_UNAVAILABLE)
    {
      _gwapi.setResponseHeader("X-Error","unavailable");
      _retval = 204;
    }
  //----------------------------------------------------------------------
  else
    {
      _gwapi.setResponseHeader("X-Error","internal");
      _retval = 204;
    }
  //----------------------------------------------------------------------
  return _retval;
  //----------------------------------------------------------------------
}
