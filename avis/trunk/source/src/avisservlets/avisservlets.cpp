// avisslet.cpp - the AvisServlet interface code.

#pragma warning ( disable : 4786 ) // This is the "name too big" silliness for template expansion

#include <string.h>
#include <iostream>
#include <fstream>
#include <assert.h>

#include "aviscommon/log/cfgprof.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"

// The individual servlet transaction handler
// class headers
#include "agetsamp.h"
#include "agetsig.h"
#include "apstsig.h"
#include "apststat.h"
#include "asubsamp.h"
#include "agetstat.h"
#include "avisredirector.h"
#include "goservlethost.h"
#include "sessionclient.h"
#include "processclient.h"
#include "utilexception.h"
#include "avisservlets.h"

#include <malloc.h>
// I really hate doing this
GoServletHost* _context = 0;
// Global Session context
ProcessClient* _process = 0;


void initializeAvisServlets()
{
  _context = new GoServletHost("avisservlets.prf");
  _process = new ProcessClient(*_context);
}

void cleanupAvisServlets()
{
  delete _process;
  _process = 0;
  delete _context;
  _context = 0;
}

extern "C"
{
  void HTTPD_LINKAGE servletInit(unsigned char* handle_,
                                 unsigned long* major_version_,
                                 unsigned long* minor_version_,
                                 long*          return_code_)
  {
    //
    initializeAvisServlets();
    _process->tracer().info("AvisGateway Servlets Initialized");
  }
  void HTTPD_LINKAGE servletTerm(unsigned char* handle_,
                                 long*          return_code_)
  {
    _process->tracer().info("AvisGateway Servlets Terminating");
    cleanupAvisServlets();
  }

  void HTTPD_LINKAGE getSampleStatus(unsigned char* handle_,long* return_code_)
  {
    _process->tracer().info("Executing getSampleStatus");
    AvisGetStatus _servlet(*_context,(const char*)handle_);
    (*return_code_) = _servlet.run();
  }
  void HTTPD_LINKAGE postSampleStatus(unsigned char* handle_,long* return_code_)
  {
    _process->tracer().info("Executing postSampleStatus");
    AvisPostStatus _servlet(*_context,(const char*)handle_);
    (*return_code_) = _servlet.run();
  }
  void HTTPD_LINKAGE getSignatureSet(unsigned char* handle_,long* return_code_)
  {
    _process->tracer().info("Executing getSignatureSet");
    AvisGetSignature _servlet(*_context,(const char*)handle_);
    (*return_code_) = _servlet.run();
  }
  void HTTPD_LINKAGE postSignatureSet(unsigned char* handle_,long* return_code_)
  {
    _process->tracer().info("Executing postSignatureSet");
    AvisPostSignature _servlet(*_context,(const char*)handle_);
    (*return_code_) = _servlet.run();
  }
  void HTTPD_LINKAGE getSuspectSample(unsigned char* handle_,long* return_code_)
  {
    _process->tracer().info("Executing getSuspectSample");

    AvisGetSample _servlet(*_context,(const char*)handle_);
    (*return_code_) = _servlet.run();
  }
  void HTTPD_LINKAGE postSuspectSample(unsigned char* handle_,long* return_code_)
  {
    _process->tracer().info("Executing postSuspectSample");
    AvisSubmitSample _servlet(*_context,(const char*)handle_);
    (*return_code_) = _servlet.run();
  }
  void HTTPD_LINKAGE redirectSignatureSet(unsigned char* handle_,long* return_code_)
  {
    _process->tracer().info("Executing redirectSignatureSet");
    AvisRedirector _servlet(*_context,(const char*)handle_,AvisRedirector::RT_SIGNATURE_DOWNLOAD);
    (*return_code_) = _servlet.run();
  }
  void HTTPD_LINKAGE redirectSuspectSample(unsigned char* handle_,long* return_code_)
  {
    _process->tracer().info("Executing redirectSuspectSample");
    AvisRedirector _servlet(*_context,(const char*)handle_,AvisRedirector::RT_SAMPLE_SUBMISSION);
    (*return_code_) = _servlet.run();
  }
}
