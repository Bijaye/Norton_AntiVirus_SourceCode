// agetstat.cpp - Implementation of the AvisGetStatus class

#pragma warning (disable : 4786)

#include "stringmap.h"
#include "agetstat.h"
#include "strutil.h"
// Avisdb headers
#include "gosession.h"
//-----------------------
#include <assert.h>
#define ASSERT	assert
#include "avisdb/attributes.h"
#include <windows.h>
#include "avisdb/analysisrequest.h"
#include "avisdb/analysisresults.h"
//-----------------------

#include "utilexception.h"

#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"

AvisGetStatus::AvisGetStatus(GoServletHost& host_,const char* gwapihandle_)
  :GoServletClient(host_,gwapihandle_),
   _tracer(0)
{
  _tracer = new TraceClient(_trace_server,"AvisGetStatus");
  _tracer->debug("Created AvisGetStatus");
  // Add Required headers
  // addRequiredHeader("X-Analysis-Cookie"); 
  // addRequiredHeader("X-Sample-Checksum");
}
AvisGetStatus::~AvisGetStatus()
{
  _tracer->debug("Destroying AvisGetStatus");
  if (_tracer)
    delete _tracer;
}

void AvisGetStatus::setRequiredHeaders  () 
{
  StackTrace _stack(*_tracer,"setRequiredHeaders");
  //  addRequiredHeader("X-Analysis-Cookie");

}

int AvisGetStatus::fulfillRequest()
{
  
  StackTrace _stack(*_tracer,"fulfillRequest");
  int _retval = 204;

  //
  // Declare string spaces for all headers that may be returned by
  // this transaction.
  //

  std::string _x_analysis_state("");
  std::string _x_sample_checksum(_gwapi.incomingHeader("X-Sample-Checksum"));
  CheckSum    _sample_checksum(_x_sample_checksum);
  std::string _x_date_analyzed("");
  std::string _x_signatures_sequence("");
  std::string _x_error("");
  
  try 
    {
      
      //
      // Test to see if this sample is finished.
      //
      AnalysisResults _results(_sample_checksum);
      
      if (! _results.IsNull())
        {
          // Set the state and the sequence # 
          _x_analysis_state = (std::string)(_results.SampleState());
          _x_signatures_sequence = StringUtil::ltos(_results.GetSignatureSequenceNum());
          
          // The default constructor of DateTime sets the internal
          // timestamp to "now" but that's really stupid behavior, so
          // I'm doing the initialization explicitly.

          DateTime _finished(DateTime::Now());
          bool _conversion_succeeded;
          _x_date_analyzed = 
            _finished.AsHttpString(_conversion_succeeded);

        }
      else
        {
          //
          // This sample isn't finished, grab it's status information
          // from the AnalysisRequest record
          //
          AnalysisRequest _request(_sample_checksum);

          if (!_request.IsNull())
            {
              _x_analysis_state = (std::string)(_request.State());
            }
          else
            {
              //
              // Error condition, we couldn't find the checksum or cookie
              // This is the "crumbled" condition.
              //

              _x_error = "crumbled";
            }
        }
      
      // Set the response headers.

      
    }
  catch (AVISDBException& e_)
    {
      _stack.error(e_.DetailedInfo());
      _x_error = "internal";
    }
  catch (AVISException& e_)
    {
      _stack.error(e_.DetailedInfo());
      _x_error = "internal";
    }
  catch (...)
    {
      _stack.error("untyped exception caught, setting X-Error to [internal]");
      _x_error = "internal";
    }


  // Set the response headers.
  if (_x_error.size() != 0)
    {

      //
      // If there was an error in processing, populate the X-Error
      // header then finish.
      //

      _gwapi.setResponseHeader("X-Error",_x_error);
    }
  else
    {

      //
      // Is this "final" status?
      //

      if (_x_date_analyzed.size() != 0)
        {
          
          //
          // Return Final Status
          //

          _gwapi.setResponseHeader("X-Date-Analyzed",_x_date_analyzed);
              
          //
          // Return the signatures sequence number
          //

          if (_x_signatures_sequence.size() != 0)
            {
              _gwapi.setResponseHeader("X-Signatures-Sequence",_x_signatures_sequence);
            }
        }

      // If we're successful, always return X-Sample-Checksum and X-Analysis-State
      _gwapi.setResponseHeader("X-Sample-Checksum",_x_sample_checksum);
      _gwapi.setResponseHeader("X-Analysis-State",_x_analysis_state);
    }
  return _retval;
}
