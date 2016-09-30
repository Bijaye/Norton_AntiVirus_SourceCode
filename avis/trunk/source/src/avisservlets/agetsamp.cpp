// agetsamp.cpp - Implementation of the AvisGetSample class

#pragma  warning (disable : 4786)
#include "agetsamp.h"
#include "strutil.h"
#include "gosession.h"
#include "utilexception.h"

#include "avisdb/stdafx.h"
#include "avisdb/analysisrequest.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"

AvisGetSample::AvisGetSample(GoServletHost& host_,const char* gwapihandle_)
  :GoServletClient(host_,gwapihandle_),
   _tracer(0),
   _sample_attributes(),
   _contentBuffer(0),
   _bufferLength(0)
{
  _tracer = new TraceClient(_trace_server,"AvisGetSample");
  _tracer->debug("Created AvisGetSample");
  // Add required headers
  // There are none for getsample

}

AvisGetSample::~AvisGetSample()
{
  _tracer->debug("Destroying AvisGetSample");
  if (_tracer)
    delete _tracer;
  if (_contentBuffer)
    {
      delete[] _contentBuffer;
      _contentBuffer = 0;
    }
}

  // Interface implementation
 void AvisGetSample::setRequiredHeaders()
{
  StackTrace _stack(*_tracer,"setRequiredHeaders");
}
 int AvisGetSample::verifyHeaderContent()
{
  StackTrace _stack(*_tracer,"verifyHeaderContent");
  int _retval = 1;
  return _retval;
}
int AvisGetSample::fulfillRequest()
{
  StackTrace _stack(*_tracer,"fulfillRequest");
  int _retval = 204;
  
  // LiveProcessing returns true if there's "real content"
  if (liveProcessing())
      _retval = 200;
  else
      _retval = 204;
  return _retval;
}
 

bool  AvisGetSample::liveProcessing()
{ 
  StackTrace _stack(*_tracer,"liveProcessing");
  // returns true if there's content being shipped back
  bool _retval = true;
  
  try
    {
      AnalysisRequest _request;
      // The constructor:
      //      AnalysisStateInfo::AnalysisStateInfo(std::string& findName);
      // should be taking a const reference.  Lint flags this as an error.
      //        if (AnalysisRequest::HighestPriorityInState( AnalysisStateInfo(AnalysisStateInfo::ExportForAnalysis()),_request))
      //  .\avisservlets\agetsamp.cpp(74) : Error 1058: Initializing a non-const
      //      reference typed 'std::basic_string<char,std::char_traits<char>,class
      //      std::allocator<char>> &' with a non-lvalue
      //  .\avisservlets\agetsamp.cpp(74) : Error 1058: Initializing a non-const
      //      reference typed 'AnalysisStateInfo &' with a non-lvalue

      if (AnalysisRequest::HighestPriorityInState( AnalysisStateInfo(AnalysisStateInfo::Accepted()),_request))
        {
          // Get the local cookie from the AnalysisRequest
          uint _cookie = _request.LocalCookie();
          
          //--------------------------------------------------------------
          std::string _traceout("Retrieved sample Cookie: ");
          _traceout += StringUtil::ltos(_cookie);
          _tracer->msg(_traceout);
          //--------------------------------------------------------------
          
          //--------------------------------------------------------------
          // Fetch the attributes from the database by local cookie
          //          Attributes _dbAttributes(_cookie);
          // Fetch attributes from the attributes file.
          loadSampleAttributes(_request.GetCheckSum());
          //--------------------------------------------------------------
          
          //--------------------------------------------------------------
          // Try to load the file first.
          // If it doesn't work, there's no reason to build a full
          // battery of response headers
          //--------------------------------------------------------------
          
          //--------------------------------------------------------------
          // Load the content
          //--------------------------------------------------------------
          _tracer->msg("Loading & Sending file...");
          loadSampleContent(_request.GetCheckSum());
          
          //--------------------------------------------------------------
          // Extract the required headers from the loaded file.
          //--------------------------------------------------------------
          MStringString::const_iterator _header(_sample_attributes.begin());
          for(;_header != _sample_attributes.end();++_header)
            {
              const std::string& _key = (*_header).first;
              const std::string& _value = (*_header).second;
              if ((_key[0] == 'X') || (_key[0] == 'x'))
                {
                  _gwapi.setResponseHeader(_key,_value);
                }

            }
          
          // The cookie field isn't in the request object.
          _gwapi.setResponseHeader("X-Analysis-Cookie",StringUtil::ltos(_cookie));

          // Send the content off after it
          _gwapi.sendContent(_contentBuffer,_bufferLength);
          
        }
      else
        {
          // Build the "Error response" headers & export them
          _retval = false;
        }
    }
  catch(AVISDBException& e_)
    {
      _tracer->msg (e_.DetailedInfo().c_str());
      throw InfrastructureException(e_.DetailedInfo().c_str());
    }
  catch(BadFilename& e_)
    {
      _tracer->msg (e_.what());
      throw InfrastructureException("bad sample filename");
    }

  return _retval;
}

void AvisGetSample::loadSampleContent(const std::string& checksum_)
{
  StackTrace _stack(*_tracer,"loadSampleContent");
  bool _retval = false;
  //---------------------------------------------------------
  // Build the filepath
  //---------------------------------------------------------
  std::string _filepath = _environment["sampleDirectory"];
  _filepath << "\\" << checksum_ << ".sample";

  //---------------------------------------------------------
  FILE* _ifile = fopen(_filepath.c_str(),"rb");
  if (_ifile)
    {
      //---------------------------------------------------------
      // get filesize to allocate buffer;
      //---------------------------------------------------------
      fseek(_ifile,0,SEEK_END);
      _bufferLength = ftell(_ifile);
      //---------------------------------------------------------
      // Rewind
      fseek(_ifile,0,SEEK_SET);
      //---------------------------------------------------------
      // Load
      _contentBuffer = new char[_bufferLength];
      fread(_contentBuffer,_bufferLength,1,_ifile);
      fclose(_ifile);
      //---------------------------------------------------------
      
      // The content type is already in the response header
      // from when the analysis record was created

      //--------------------------------------------------------------
      // Set the 'content-type'
      //--------------------------------------------------------------
      _gwapi.setContentType("application/x-avis-sample");
      //--------------------------------------------------------------

    }
  else
    {
      std::string _traceout("Bad Filename: ");
      _traceout += _filepath;
      _tracer->error(_traceout);
      throw BadFilename(_filepath.c_str());
    }
  // Send it across the wire from here
}

void AvisGetSample::loadSampleAttributes(const std::string& checksum_)
{
  StackTrace _stack(*_tracer,"loadSampleAttributes");
  //--------------------------------------------------------------
  // Build the filepath
  //--------------------------------------------------------------
  std::string _filepath = _environment["sampleDirectory"];
  _filepath << "\\" << checksum_ << ".attributes";
  FILE* _ifile = fopen(_filepath.c_str(),"rb");
  if (_ifile)
    {
      //---------------------------------------------------------
      // get filesize to allocate buffer;
      //---------------------------------------------------------
      fseek(_ifile,0,SEEK_END);
      _bufferLength = ftell(_ifile);
      //---------------------------------------------------------
      // Rewind
      fseek(_ifile,0,SEEK_SET);
      //---------------------------------------------------------

      //---------------------------------------------------------
      // Build the internal buffer
      //---------------------------------------------------------

      char* _file_buffer = new char[_bufferLength + 1];
      memset(_file_buffer,0,(_bufferLength + 1));
      //---------------------------------------------------------
      // Load
      //---------------------------------------------------------
      fread(_file_buffer,_bufferLength,1,_ifile);
      fclose(_ifile);
      //---------------------------------------------------------
      
      //---------------------------------------------------------
      // Parse the block into the _sample_attributes table
      //---------------------------------------------------------
      std::string _string_headers(_file_buffer);

        // delete the buffer
      delete[] _file_buffer;

      _sample_attributes = StringMapUtil::fromHttpHeaders(_string_headers);

      // The content type is already in the response header
      // from when the analysis record was created

      //--------------------------------------------------------------
      // Set the 'content-type'
      //--------------------------------------------------------------
      _gwapi.setContentType("application/x-avis-sample");
      //--------------------------------------------------------------
    }      

}
