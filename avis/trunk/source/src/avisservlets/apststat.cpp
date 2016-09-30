// apststat.cpp - Implementation of the AvisPostStatus class

#pragma warning (disable : 4786)
#include "apststat.h"
#include "stringmap.h"
#include "strutil.h"
#include "utilexception.h"
#include "gosession.h"

// External Module Headers
#include "avisdb/stdafx.h"
#include "avisdb/attributes.h"
#include "avisdb/analysisstateinfo.h"
#include "avisdb/analysisrequest.h"
#include "avisdb/analysisresults.h"
#include "avisdb/checksum.h"

#include "aviscommon/attributekeys.h"
#include "aviscommon/log/mtrace.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"

AvisPostStatus::AvisPostStatus(GoServletHost& host_,const char* gwapihandle_)
  :GoServletClient(host_,gwapihandle_),
   _tracer(0)
{
  _tracer = new TraceClient(_trace_server,"AvisPostStatus");
  _tracer->debug("Created AvisPostStatus");
  // Add Required Headers
  addRequiredHeader("X-Sample-Checksum");
  addRequiredHeader("X-Analysis-State");
}
AvisPostStatus::~AvisPostStatus()
{
  _tracer->debug("Destroying AvisPostStatus");
  if (_tracer)
    delete _tracer;
}

int AvisPostStatus::fulfillRequest()
{
  StackTrace _stack(*_tracer,"fulfillRequest");
  std::string _message;
  int _retval = 204;

  // Grab any incoming headers that we could POSSIBLY care about
  // These two are required.
  std::string _x_sample_checksum(_gwapi.incomingHeader("X-Sample-Checksum"));
  std::string _x_analysis_state(_gwapi.incomingHeader("X-Analysis-State"));
  CheckSum    _sample_checksum(_x_sample_checksum);
  AnalysisStateInfo _analysis_state(_x_analysis_state);
  // These two are not required in all cases.
  std::string _x_date_analyzed;
  std::string _x_signatures_sequence;
  try
    {
      _x_date_analyzed = _gwapi.incomingHeader("X-Date-Analyzed");
      _x_signatures_sequence = _gwapi.incomingHeader("X-Signatures-Sequence");
    }
  catch(InvalidHttpHeader&)
    {
      _stack.info("Non-final status posted (missing X-Date-Analyzed or X-Signatures-Sequence");
    }

  if (_x_date_analyzed.size() != 0)
    {
      // First try doing this through an AnalysisRequest record.
      // If that fails, use AnalysisResults only
      AnalysisRequest _finished_request(_sample_checksum);
      if (_finished_request.IsNull() == false)
        {
          _finished_request.FinalState(_analysis_state,
                                       atoi(_x_signatures_sequence.c_str()));
          //
        }
      else
        {
          AnalysisResults::AddNew(_sample_checksum,
                                  atoi(_x_signatures_sequence.c_str()),
                                  _analysis_state);
        }
    }
  else
    {
      //
      // Not finished state.
      // Just update the AnalysisRequest record
      //
      AnalysisRequest _request(_sample_checksum);
      if (_request.IsNull() == false)
        {
          _request.State((AnalysisStateInfo(_x_analysis_state)));
        }
    }
  return _retval;
}

