// asubsamp.cpp - Implementation of the AvisSubmitSample class

#pragma warning (disable : 4786)
#include <assert.h>
#define ASSERT assert

#include "asubsamp.h"
#include <fstream>
#include <stdio.h>
#include "gosession.h"

#include "strutil.h"
#include "stringmap.h"
#include "utilexception.h"

#include "avisfile/md5/md5calculator.h"

#include "aviscommon/log/lock.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "aviscommon/datetime.h"

//#include "avispolicy/avispolicy.h"
#include "avisdb/dbsession.h"
#include "avisdb/sqlstmt.h"
#include "avisdb/analysisrequest.h"
#include "avisdb/analysisresults.h"
#include "avisdb/attributes.h"
#include "avisdb/signature.h"

#include "avisfilter/filterexception.h" 


const static long CONTENT_BLOCK_SIZE = 65536;


AvisSubmitSample::AvisSubmitSample(GoServletHost& host_,const char* gwapihandle_)
  :GoServletClient(host_,gwapihandle_),
   _content_hash(),
   _incoming_attributes(),
   _x_error(),
   _x_analysis_gateway(),
   _x_analysis_cookie(),
   _x_analysis_state(),
   _x_analysis_finished(),
   _x_signatures_sequence(),
   _x_sample_checksum(),
   _filter_mutex("SubmissionFilterMutex"),  
   _tracer(0)
{ 
  _tracer = new TraceClient(_trace_server,"AvisSubmitSample");
  _tracer->debug("Created AvisSubmitSample");

  // Add required Headers
  // Standard Headers
  addRequiredHeader("X-Content-Checksum");
  addRequiredHeader("Content-Length");
  addRequiredHeader("Content-Type");
  //    // Customer Attributes
  // Date Attributes
  addRequiredHeader("X-Date-Quarantined");
  // Removed Tue Jan 11 13:22:41 2000
  //  addRequiredHeader("X-Date-Created");
  //  addRequiredHeader("X-Date-Modified");
  // Sample attributes
  addRequiredHeader("X-Sample-Type");
  addRequiredHeader("X-Sample-Size");
  addRequiredHeader("X-Sample-Priority");
  //  addRequiredHeader("X-Sample-Extension");
  addRequiredHeader("X-Sample-Checksum");

  // Checksum headers
  addRequiredHeader("X-Checksum-Method");


  // Initialize the local copy of request headers
  // (this is done because we muck with 'em)
  _incoming_attributes = _gwapi.getRequestHeaders();
}

AvisSubmitSample::~AvisSubmitSample()
{
  _tracer->debug("Destroying AvisSubmitSample");
  if (_tracer)
    {
      delete _tracer;
      _tracer = 0;
    }
} 

int AvisSubmitSample::fulfillRequest()
{
  // I'll be factoring this out into individual methods any
  // minute now.
  StackTrace _stack(*_tracer,"fulfillRequest");
  int _retval = 204;

  try
    {
      liveProcessing();
    }
  catch (InvalidConfigVariable& e_)
    {
      _stack.error(e_.what());
      throw e_;
    }
  return _retval;
}
bool AvisSubmitSample::liveProcessing()
{
  StackTrace _stack(*_tracer,"liveProcessing");

  bool _retval = true;
  std::string _response("");

  // Moved this logging to the GoServletClient class  so it works
  // for all transaction types
  //  _stack.info(_headers);
  //   //----------------------------------------------------------------
  //   // Validate Policy
  //   //----------------------------------------------------------------
  //   if (!validatePolicy(_headers) )
  //     {
  //       action_declined();
  //       throw ServletRecoverable("policy check failed","declined");
  //     }
  
  //-----------------------------------------------------------------
  // Submission size limits are now imposed here instead of in the
  // policy() function. (EJP 4/14/00)
  //-----------------------------------------------------------------

  // The external environment or servlet profile may contain
  // configuration parameters that limit the maximum size of a
  // submission.  If so, convert those parameters to integers and
  // store them in local variables for use below.  If not, store
  // zero in local variables, which will be interpreted below as
  // "no limit".  (EJP 4/12/00)
  
  long _maximumSampleSize = atol(((_environment)["maximumSubmissionSampleSize"]).c_str());
  long _maximumContentSize = atol(((_environment)["maximumSubmissionContentSize"]).c_str());
  
  // The incoming headers from LotusGo must include the required
  // attributes X-Sample-Size and Content-Length. Convert the
  // values of those attributes into integers and store them in
  // local variables for use below.  (EJP 4/12/00)
  
  long _sample_size = atol((_gwapi.incomingHeader("X-Sample-Size")).c_str());
  long _content_size = atol((_gwapi.incomingHeader("Content-Length")).c_str());
  
  // If a maximum sample size is configured, compare the submitted
  // sample size to it.  If it is smaller, log this check and
  // continue.  If it is larger, log an error and return the
  // intentionally vague "X-Error: declined" error to the
  // customer. (EJP 4/12/00)
  
  if (_maximumSampleSize)
    {
      if (_sample_size <= _maximumSampleSize)
	{
	  std::string _msg("submitted sample size ");
	  _msg += StringUtil::ltos(_sample_size);
	  _msg += " is less than or equal to maximum sample size ";
	  _msg += StringUtil::ltos(_maximumSampleSize);
	  _stack.debug(_msg);
	}
      else
	{
	  std::string _msg("policyException submitted sample size ");
	  _msg += StringUtil::ltos(_sample_size);
	  _msg += " exceeds maximum sample size ";
	  _msg += StringUtil::ltos(_maximumSampleSize);
	  _stack.error(_msg);
	  _x_error = "declined";
	  _gwapi.setResponseHeader("X-Error",_x_error);
	  return _retval;
	}
    }
  
  // If a maximum content size is configured, compare the submitted
  // content size to it.  If it is smaller, log this check and
  // continue.  If it is larger, log an error and return the
  // intentionally vague "X-Error: declined" error to the
  // customer. (EJP 4/12/00)
  
  if (_maximumContentSize)
    {
      if (_content_size <= _maximumContentSize)
	{
	  std::string _msg("submitted content size ");
	  _msg += StringUtil::ltos(_content_size);
	  _msg += " is less or equal to than maximum content size ";
	  _msg += StringUtil::ltos(_maximumContentSize);
	  _stack.debug(_msg);
	}
      else
	{
	  std::string _msg("policyException submitted content size ");
	  _msg += StringUtil::ltos(_content_size);
	  _msg += " exceeds maximum content size ";
	  _msg += StringUtil::ltos(_maximumContentSize);
	  _stack.error(_msg);
	  _x_error = "declined";
	  _gwapi.setResponseHeader("X-Error",_x_error);
	  return _retval;
	}
    }
  
  //-----------------------------------------------------------------
  // If the submission is not declined due to policy restrictions,
  // the submission is processed here.
  //-----------------------------------------------------------------

  try
    {
      // try dumping the entire hash to the log file...
#if 0
      _stack.info(StringMapUtil::toPrefixedDebugString("ENVHEADER",_environment));
#endif
      // If it doesn't already exist...
      //----------------------------------------------------------------
      // Grab the value for the X-Analysis-Gateway response attribute
      //----------------------------------------------------------------
      if (_environment.find("XAnalysisGateway") == _environment.end())
        {
          // Stuff the derived value in there
          _environment["XAnalysisGateway"] = _incoming_attributes["HOST"];
        }
      //
      _x_analysis_gateway = _environment["XAnalysisGateway"];
      //
      // The X-Analysis-Gateway attribute is a bit different because we want
      // to stuff it in the response to this transaction AND the
      // "incoming header block" that is associated with this sample,
      // so that it can be preserved along with the rest of the sample
      // status information.  The "quick and dirty" way to do this is
      // to add it to the _incoming_attributes map.
      //
      //----------------------------------------------------------------
      _incoming_attributes["X-Analysis-Gateway"] = _x_analysis_gateway;
      //----------------------------------------------------------------

      // Convert the _incoming_headers map to a "HTTP Transaction"
      // formatted string.  This is used in two places:
      //  1) Prime the Filter constructor.
      //  2) Save off to the "<checksum>.attributes" file.
      //----------------------------------------------------------------
      // These are likely to have changed as a result of the validatePolicy
      // method
      //----------------------------------------------------------------
      std::string _headers(StringMapUtil::toHttpHeaders(_incoming_attributes));
      //----------------------------------------------------------------
       
      //----------------------------------------------------------------
      // Populate the object member variable _x_sample_checksum here,
      // because we can.
      //----------------------------------------------------------------
      _x_sample_checksum = _gwapi.incomingHeader("X-Sample-Checksum");
      //----------------------------------------------------------------
      
      //----------------------------------------------------------------
      // Create a new filter object with the incoming
      // http header block
      //----------------------------------------------------------------

      //
      // Serialize all access to the process of filter construction.
      //

      // Capture and release the mutex here.
      _stack.debug("Creating Filter Object...");
      _filter_mutex.capture();

      Filter _filter(_headers);
      //
      // Release the mutex lock we grabed to construct the filter
      // If the filter throws an exception, the NamedMutex class WILL
      // call the "CloseHandle" method which will also release the mutex.
      //
      // Ok, here's why this works:
      //
      // The _filter_mutex is created on class instantiation (i.e. in
      // the constructor) but is NOT locked.  Before the filter object 
      // is created, the mutex lock is grabbed (and will wait
      // indefinitely, yadda yadda.)
      // 

      _filter_mutex.release();

      _stack.debug("Filter object created");
      //----------------------------------------------------------------

      //----------------------------------------------------------------
      // Execute the filter and pull back a std::pair containing...
      //  - first  = Filter::FilterStatus code returned from filter
      //  - second = the cookie  (long)
      //----------------------------------------------------------------
      std::pair<Filter::FilterStatus,long> _status;
      _status = executeFilter(_filter);
      //----------------------------------------------------------------
      // Assign the cookie value (if any!  This value may be zero!)
      //----------------------------------------------------------------
      long _cookie = _status.second;
      _x_analysis_cookie = StringUtil::ltos(_cookie);
      //----------------------------------------------------------------
      
      //----------------------------------------------------------------
      // Create a text-based "Filter Response: " message for the logs
      // Note that this is an important checkpoint before the catch
      // below.
      //----------------------------------------------------------------
      std::string _r_msg("Filter Response = [");
      _r_msg += _filter.StatusAsString();
      _r_msg += "]";
      _stack.msg(_r_msg);
      //----------------------------------------------------------------


      //----------------------------------------------------------------
      // Grab the current DateTime.
      // This is used to indicate in the "dispatch_response" method
      // whether or not this sample is in "finished" state. (note that 
      // I am only setting a temporary DateTime object to "now" here.
      // In the switch block below, that value is assigned to
      // _x_analysis_finished in the event that the filter has
      // returned "final" status.  That then will trigger the
      // dispatch_response() string test.
      //-------------------------------------------------------------------
      
      DateTime _current_date_time(DateTime::Now());

      // The conversion to an HttpString requires this as a parameter.
      bool _dt_convert_success = false;
      
      //----------------------------------------------------------------
      // Process the filter responses.  Note that there aren't nearly
      // as many actions as there are response codes.  The other
      // purpose served by this case statement is to convert the
      // Filter::FilterStatus codes into AnalysisStateInfo codes.
      //----------------------------------------------------------------
      switch (_status.first)
        {
          //--------------------------------------------------
          // Done->Export states
          //--------------------------------------------------
        case Filter::Done_FoundByHeuristic:
        case Filter::Done_NoRepair:
        case Filter::Done_UnRepairable:
        case Filter::Done_BadRepair:
        case Filter::Done_BadScan:
        case Filter::Done_NAVoverrun:
        case Filter::Done_NoDetect:
        case Filter::Done_NeedsHarderLook:
        case Filter::ScanError:
          {
            //
            // In this case, we are saving the content to disk and
            // essentially flagging it for export to the analysis
            // center.  A new step is added in here to save the block
            // of headers that were passed to the filter to a separate
            // disk file along with the content file.
            //
            action_export(_filter,_headers,_cookie);
            _x_analysis_state = AnalysisStateInfo::Accepted();
          }
          break;
          //--------------------------------------------------

          //--------------------------------------------------
          // Done->Finished states
          //--------------------------------------------------
        case Filter::Done_Misfired:
          _x_analysis_state = AnalysisStateInfo::Misfired();
          _x_analysis_finished = _current_date_time.AsHttpString(_dt_convert_success);
          action_finished(_cookie,_x_analysis_state);
          break;
        case Filter::Done_UnSubmittable:
        case Filter::Done_Infected:
          _x_analysis_state = AnalysisStateInfo::Infected();
          _x_analysis_finished = _current_date_time.AsHttpString(_dt_convert_success);
          action_finished(_cookie,_x_analysis_state);
          break;
        case Filter::Done_NotInfectable:
          _x_analysis_state = AnalysisStateInfo::UnInfectible();
          _x_analysis_finished = _current_date_time.AsHttpString(_dt_convert_success);
          action_finished(_cookie,_x_analysis_state);
          break;
        case Filter::Done_UnInfected:
          _x_analysis_state = AnalysisStateInfo::UnInfected();
          _x_analysis_finished = _current_date_time.AsHttpString(_dt_convert_success);
          action_finished(_cookie,_x_analysis_state);
          break;
        case Filter::Done_Piggybacked:
          // Ironically, there is no action to be taken in this case.
          // action_piggyback(_cookie);
          break;
          //--------------------------------------------------

          //--------------------------------------------------
          // The following are all error states
          // Of one kind or another.  Note that error states are
          // "final"
          //--------------------------------------------------
        case Filter::CRCsampleFailure:
          _x_error = "internal";
          _x_analysis_state = AnalysisStateInfo::CRCSampleFailure();
          break;
        case Filter::CRCcontentFailure:
          _x_error = "internal";
          _x_analysis_state = AnalysisStateInfo::CRCContentFailure();
          break;
        case Filter::BadSampleCheckSum:
          _x_error = "sample";
          _x_analysis_state = AnalysisStateInfo::BadSampleCheckSum();
          break;
        case Filter::BadContentCheckSum:
          _x_error = "content";
          _x_analysis_state = AnalysisStateInfo::BadContentCheckSum();
          break;
        case Filter::MissingAttributes:
          _x_error = AnalysisStateInfo::MissingAttributes();
          _x_analysis_state = AnalysisStateInfo::MissingAttributes();
          break;
        case Filter::BadSubmittorID:
          _x_error = "submittor";
          break;
        case Filter::DataOverflow:
          _x_error = "overrun";
          break;
        case Filter::UnknownSampleType:
          _x_error = "type";
          break;
        case Filter::UnpackageFailure:
          _x_error = "unpackage";
          break;
        case Filter::UnableToCreateMappedMem:
        case Filter::UnableToScan:
        case Filter::DatabaseError:
        case Filter::NeedMoreData:
          // Strange error.  This shouldn't occur.  At this stage,
          // the Filter has already accepted the content.  So I'm
          // promoting it to an InfrastructureException.
        case Filter::UnknownError:
        default:
          // The "underrun" case is not properly trapped by the filter
          // code, therefore the _x_error member is set  manually
          // inside the executeFilter() method and the filter status
          // is returned as "Filter::UnknownError";  This test is an
          // extension of that band-aid.  However, the code for
          // overrun (Filter::DataOverflwo) exists, so the status is
          // manually set to that value in that case, and it is
          // handled above.
          if (_x_error != "underrun")
            {
              _x_error = "internal";
            }
          break;
          //--------------------------------------------------
        } // Switch
    } // Try
  catch (FilterException& e_)
    {
      _filter_mutex.release();
      std::string _msg("FilterException: ");
      _msg += e_.DetailedInfo();
      _stack.error(_msg);
      _x_error = "internal";
    }
  catch (AVISDBException& e_)
    {
      _filter_mutex.release();
      std::string _msg("AVISDBException: ");
      _msg += e_.DetailedInfo();
      _stack.critical(_msg);
      _x_error = "internal";
    }
  catch (AvisServletsException& e_)
    {
      _filter_mutex.release();
      std::string _msg("AvisServletsException: ");
      _msg += e_.what();
      _stack.error(_msg);
      _x_error = "internal";
    }
  catch (AVISException& e_)
    {
      _filter_mutex.release();
      std::string _msg("AVISException: ");
      _msg += e_.DetailedInfo();
      _stack.error(_msg);
      _x_error = "internal";
    }
  catch (exception& e_)
    {
      _filter_mutex.release();
      std::string _msg("Standard C++ library exception: ");
      _msg += e_.what();
      _stack.error(_msg);
      _x_error = "internal";
    }
  catch (...)
    {
      _filter_mutex.release();
      std::string _msg("Default exception thrown, aborting transaction");
      _x_error = "internal";
    }
  // the AvisSubmitSample::dispatch_response() method takes all the
  // accumulated status, fills in the blanks (like signatures-sequence
  // if necessary, etc) and sets the response headers
  dispatch_response();
  
  return _retval;
}

std::pair<Filter::FilterStatus,long> AvisSubmitSample::executeFilter(Filter& filter_)
{
  StackTrace _stack(*_tracer,"executeFilter");
  Filter& _filter = filter_;
  std::pair<Filter::FilterStatus,long> _retval;

  // Prime the return value with unoptomistic results
  _retval.first = Filter::UnknownError;
  _retval.second = -1;
  
  // Don't even bother to allocate the buffer
  // if we don't need to
  long  _contentLen       = 0;
  long  _bytesTransferred = 0;
  long  _bytesRead        = 0;
  // A simple flag to indicate whether we've had
  // a content overrun or underrun
  bool  _content_length_match = true;

  // Execute the filter
  
  _stack.debug("Calling Filter::Status");
  Filter::FilterStatus _status = _filter.Status();

  // If the filter requires any content at all
  // Pull it and process it incrementally
  if (_status == Filter::NeedMoreData)
    {
      _stack.msg("... Filter says 'need more data'");
      _contentLen = atol(_gwapi.incomingHeader("Content-Length").c_str());

      // A bunch of code so I can generate an MD5 myself

      _stack.debug("creating MD5 calculator for sample content");
      MD5Calculator _md5er;
      
      //-------------------------------------------------------------
      // Allocate a content-buffer
      //-------------------------------------------------------------
      _stack.debug("allocating buffer to recieve sample content");
      char* _buffer           = 0;
      long  _bufferLen        = CONTENT_BLOCK_SIZE;
      _buffer = new char[_bufferLen]; 
      try
        {
          std::string _message;
          //-------------------------------------------------------------
          // As long as there's content needed
          //-------------------------------------------------------------
          //            while ((_status  == Filter::NeedMoreData) && 
          //                   (_bytesTransferred < _contentLen))

          // Let's change that to _gwapi.eof()
          while (! _gwapi.eof())
            {
              //-------------------------------------------------------------
              // Get another block 
              //-------------------------------------------------------------
              _bytesRead = _gwapi.getContentChunk((unsigned char*)_buffer,_bufferLen);
              _bytesTransferred += _bytesRead;
              if (_bytesRead)
                {
		  // This loop is executed too frequently while
		  // receiving sample data to log each iteration. Only
		  // end-of-data and errors will be logged.  (EJP
		  // 6/25/00)

#define DATA_RECEIVE_DETAILS 0
#if DATA_RECEIVE_DETAILS
                  _message = "transferred: ";
                  _message += StringUtil::ltos(_bytesTransferred);
                  _message += " bytes of: ";
                  _message += StringUtil::ltos(_contentLen);
                  _message += " total content length";
                  _stack.debug(_message);
#endif
                  // Send the block off to the filter
#if DATA_RECEIVE_DETAILS
                  _stack.debug("sending block to filter");
#endif
                  _status = _filter.AppendData(_buffer,_bytesRead);
              
		  // Log the filter return code only if it is non-zero
		  // (EJP 6/25/00)

		  if (_status)
		    {
		      _message = "Filter::AppendData returned code: ";
		      _message += StringUtil::ltos(_status);
		      _stack.debug(_message);
		    }
              
                  // Now step the MD5 generator so I can compare as well
#if DATA_RECEIVE_DETAILS
                  _stack.debug("incrementing MD5 calculation");
#endif
                  _md5er.CRCCalculate((const uchar*)_buffer,_bytesRead);
                }
              else
                {
                  // Received null buffer
                  _stack.warning("Received zero-byte content chunk, continuing...");
                }
            }
          _message = "transferred: ";
          _message += StringUtil::ltos(_bytesTransferred);
          _message += " bytes of: ";
          _message += StringUtil::ltos(_contentLen);
          _message += " total content length";
          _stack.debug(_message);

          //
          // Test for underrun and overrun, since the filter doesn't seem to be
          // doing it very well (if at all)
          //
          if (_bytesTransferred < _contentLen)
            {
              _x_error = "underrun";
              _stack.error("Content underrun condition");
            }
          else if (_bytesTransferred > _contentLen)
            {
              _x_error = "overrun";
              _status = Filter::DataOverflow;
              _stack.error("Content overrun condition");
              
            }

        }
      catch (FilterException& e_)
        {
          std::string _message("FilterException: ");
          _message += e_.DetailedInfo();
          _stack.error(_message);
          if (_buffer)
            {
              delete[] _buffer;
              _buffer = 0;
            }
          throw e_;
        }
      catch (AVISDBException& e_)
        {
          std::string _message("AVISDBException: ");
          _message += e_.DetailedInfo();
          _stack.error(_message);
          if (_buffer)
            {
              delete[] _buffer;
              _buffer = 0;
            }
          throw e_;
        }
      catch (...)
        {
          _stack.error("Unknown Exception");
          if (_buffer)
            {
              delete[] _buffer;
              _buffer = 0;
            }
          throw InfrastructureException("Unknown Exception from AvisSubmitSample::executeFilter");
        }
      // This is the line that was missing, causing a slow memory leak
      // over many sample submissions.  A careless mistake on my part, 
      // at best.
      delete[] _buffer;
      
      //-------------------------------------------------------------
      // Seal off the MD5 calculation & dump it to the log
      //-------------------------------------------------------------
      _md5er.Done(_content_hash);
      std::string _msg("Content Hash: ");
      _msg += _content_hash;
      _stack.msg(_msg);

      //-------------------------------------------------------------
      
      // Signal that we're done shipping data
      // And get the final code
    }

  // Shield this operation from the cases we handle inside this
  // method.
  // (the Filter class doesn't correctly handle overrun & underrun
  // conditions)
  if ((_x_error != "overrun") &&
      (_x_error != "underrun"))
    {
      _status = _filter.Done();
    }
  
  // Set the compound return value
  _retval.first = _status;                // Status returned by filter
  _retval.second =_filter.LocalCookie();  // Assigned cookie

  //-- Debugging message
  std::string _traceout("Bytes Read: ");
  _traceout += StringUtil::ltos(_bytesTransferred);
  _stack.debug(_traceout);

  _traceout = "Local Cookie: ";
  _traceout += StringUtil::ltos(_retval.second);
  _stack.debug(_traceout);

  return _retval;
}

bool AvisSubmitSample::save_content_file(Filter& filter_)
{
  StackTrace _stack(*_tracer,"saveContent");
  // So how do I build the filename?
  // Pull the path off the _profile hash
  // Add the Cookie? Yep.
  std::string _filepath(_environment["sampleDirectory"]);
  
  _filepath  += "\\";
  //  _filepath += StringUtil::ltos(_filter.LocalCookie());
  _filepath += (*_incoming_attributes.find("X-Sample-Checksum")).second;
  _filepath += ".sample";
  _stack.msg(_filepath);
  
  bool _success = filter_.WriteContentsToDisk(_filepath);

  if (_success)
    {
      _stack.msg("Wrote contents successfully");
    }
  else
    {
      _stack.error("Content write Failed!");
      _x_error = "internal";
    }
  return _success;
}

void AvisSubmitSample::action_export(Filter& filter_,const std::string& headers_,long cookie_)
{
  StackTrace _stack(*_tracer,"action_export");

  if (_x_error.size() != 0)
    {
      std::string _msg("X-Error seems to be set internally to[");
      _msg.append(_x_error);
      _msg.append("]");
      _stack.error(_msg);
    }
  if (cookie_ == 0)
    {
      _stack.error("Attempted to set cookie [0] to accepted state");
      _x_error = "internal";
    }
  else
    {
      //----------------------------------------------------------------
      // Create an AnalysisRequest record
      //----------------------------------------------------------------

      AnalysisRequest _request(cookie_);
      if (_request.IsNull())
        {
          std::string _msg("Unable to create an AnalysisRequest object for cookie [");
          _msg.append(StringUtil::ltos(cookie_));
          _msg.append("]");
          _x_error = "internal";
          _stack.error(_msg);

        }
      
      if (save_local_files(filter_,headers_))
        {
      
          // Set Imported Date to now
          _request.Imported(DateTime());
          
          // Set the state for export into the analysis center.
          _request.State(AnalysisStateInfo(AnalysisStateInfo::Accepted()));
        }
      else
        {
          //
          // The write of one or both local content files failed.
          // Setting _x_error triggers the clean up mechanism
          //
          _x_error = "internal";
        }
    }
}
void AvisSubmitSample::action_intermediate(long cookie_,const std::string& state_)
{
  StackTrace _stack(*_tracer,"action_intermediate");
  //-----------------------------------------------------
  std::string _message("cookie: ");
  _message += StringUtil::ltos(cookie_);
  _message += ",State: ";
  _message += state_;
  _stack.info(_message);
  //-----------------------------------------------------
  if (cookie_ == 0)
    {
      throw InfrastructureException("tried to set state on a null cookie");
    }
  else
    {

      std::string _state(state_); // Because the AnalysisStateInfo isn't const correct
      AnalysisRequest _request(cookie_);
      if (_request.IsNull())
        {
          throw InfrastructureException("Unable to create AnalysisRequest object");
        }
      try
        {

          _request.State(AnalysisStateInfo(_state));
        }
      catch (AVISDBException& e_)
        {
          _stack.error(e_.DetailedInfo());
          throw e_;
        }
      catch (AVISException& e_)
        {
          _stack.error(e_.DetailedInfo());
          throw e_;
        }
      catch (...)
        {
          _stack.error("untyped exception");
          throw;
        }
    }

}
void AvisSubmitSample::action_finished(long cookie_,const std::string& state_)
{
  StackTrace _stack(*_tracer,"action_finished");
  //-----------------------------------------------------
  std::string _message("cookie[");
  _message += StringUtil::ltos(cookie_);
  _message += "] State[";
  _message += state_;
  _message += "]";
  _stack.info(_message);
  //-----------------------------------------------------
  
  try
    {
      uint _seqnum = 0;
      std::string _state(state_);
      AnalysisStateInfo _asi(_state);
      if ( ((std::string)(_asi) == AnalysisStateInfo::Infected()) ||
           ((std::string)(_asi) == AnalysisStateInfo::Misfired())
           )
        {
          _stack.info("Returning signature sequence number");
          // Grab the X-Sample-Checksum from our internals
          // It's inexcusable that I have to put this in here just 
          // because the CheckSum class isn't const correct.
          std::string _checksum_string(_gwapi.incomingHeader("X-Sample-Checksum"));
          CheckSum _checksum_object(_checksum_string);
          AnalysisResults _ar(_checksum_object);
          _seqnum = _ar.GetSignatureSequenceNum();
        }
      else
        {
          _stack.info("Misfired state, no signatures needed");
          _seqnum = 0;
          // ????
        }
      // If the cookie == 0 don't do this update.
      if (cookie_ != 0)
        {
          AnalysisRequest _request(cookie_);
          if (_request.IsNull())
            throw InfrastructureException("Unable to create AnalysisRequest object");
          _request.FinalState(_asi,_seqnum);
        }
      _stack.debug("FinalState returned normally");
    }
  catch (AVISDBException& e_)
    {
      std::string _msg("AvisDBException: ");
      _msg += e_.DetailedInfo();
      _stack.error(_msg);
      //      _stack.error(e_.DetailedInfo());
      throw e_;
    }
  catch (AVISException& e_)
    {
      std::string _msg("AVISException: ");
      _msg += e_.DetailedInfo();
      _stack.error(_msg);
      throw e_;
    }
  catch (...)
    {
      _stack.error("untyped exception");
      throw;
    }
}

void AvisSubmitSample::action_piggyback(long cookie_)
{
  StackTrace _stack(*_tracer,"action_piggyback");
  std::string _message("Cookie[");
  _message += StringUtil::ltos(cookie_);
  _message += "]";
  _stack.info(_message);
  AnalysisRequest _request(cookie_);
  if (_request.IsNull())
    throw InfrastructureException("Unable to create AnalysisRequest object");
  // Set Imported Date to now
  try
    {
      _request.Imported(DateTime());
    }
  catch (AVISDBException& e_)
    {
      _stack.error(e_.DetailedInfo());
      throw e_;
    }
  catch (AVISException& e_)
    {
      _stack.error(e_.DetailedInfo());
      throw e_;
    }
  catch (...)
    {
      _stack.error("untyped exception");
      throw;
    }
}

bool AvisSubmitSample::save_local_files(Filter& filter_,const std::string& headers_) 
{
  StackTrace _stack(*_tracer,"save_local_files");

  return (save_content_file(filter_) &&
          save_attributes_file(headers_));
}

void AvisSubmitSample::action_declined()
{
  StackTrace _stack(*_tracer,"action_declined");
  DateTime _dtime(DateTime::Now());
  std::string _dt;
  _dtime.AsHttpString(_dt);
  _gwapi.setResponseHeader("X-Error","declined");
  _gwapi.setResponseHeader("X-Date-Analyzed",_dt);
}

bool AvisSubmitSample::save_attributes_file(const std::string& header_block_) const
{
  StackTrace _stack(*_tracer,"save_attributes_file");
  // So how do I build the filename?
  // Pull the path off the _profile hash
  // Add the Cookie? Yep.
  std::string _filepath(_environment["sampleDirectory"]);
  
  _filepath  += "\\";
  //  _filepath += StringUtil::ltos(_filter.LocalCookie());
  _filepath += (*(_incoming_attributes.find("X-Sample-Checksum"))).second;
  _filepath += ".attributes";
  _stack.msg(_filepath);
  std::ofstream _out(_filepath.c_str());
  _out << header_block_;
  _out.close();
  return _out.good();
}

void AvisSubmitSample::dispatch_response()
{
  StackTrace _stack(*_tracer,"dispatch_response");

  // This approach to determining how this transaction "went" could
  // mark me as a bit of a luddite, but it's the simplest block of
  // code with the smallest number of interdependencies I can think of 
  // at the moment.  It's a load of string compares that are used to
  // decide what information we still need to collect (and goes to
  // collect it.) Once all that has been done, we set all the response 
  // headers as appropriate and exit.

  //-------------------------------------------------------------------
  //
  // #1 - Did the transaction "fail"
  //
  //-------------------------------------------------------------------
  // The existence of a value other than "" in the _x_error
  // object-scope member indicates that this transaction failed for
  // "some reason."  This reason has been logged ALREADY to the extent 
  // that this class is able to determine the cause.  The only
  // remaining action is to perform any cleanup that can be
  // done. (This cleanup is offloaded to another method and is
  // GUARANTEED (by nothing more than convention) to not set any
  // response headers. 
  //-------------------------------------------------------------------
  if (_x_error.size() != 0)
    {

      //
      // Call the cleanup method (which currently does nothing)
      //

      transaction_error_cleanup();

      //
      // Drop out.
      //

    }
  else
    {

      //-------------------------------------------------------------------
      //
      // #2 - Is the sample in "final" state?
      //
      //-------------------------------------------------------------------
      //
      // This is indicated by the presence (or absence) of a value in
      // the _x_analysis_finished member variable.
      //
      //-------------------------------------------------------------------

      if (_x_analysis_finished.size() != 0)
        {
          //
          // Set the X-Date-Analyzed header 
          //

          //-------------------------------------------------------------------
          //
          // Now that we know we have a Finished sample, we need to
          // create an AnalysisResults "object" to get further
          // information (i.e. Are there "signatures" that need to be
          // distributed to the client to handle this sample?  Or is
          // it in "final error" state. 
          //
          //-------------------------------------------------------------------
          //
          //
          CheckSum _checksum_object(_x_sample_checksum);
          AnalysisResults _analysis_results(_checksum_object);

          //
          // There are still opportunities for strange errors
          // here. For instance if the sql select above (in
          // AnalysisResults) fails for some reason and the IsNull
          // test returns true...
          //
          if (_analysis_results.IsNull() == false)
            {
              // Ok, the object is good.  Let's pull the remaining
              // data we need.  Ironically, state has already been
              // set, so all that remains is the sequence number (if
              // it exists)

              // 
              // Get the sequence number
              //
              uint _sequence_number =
                _analysis_results.GetSignatureSequenceNum();
              
              // 
              // Assuming it's not zero (indicating that there was
              // none)
              //

              if (_sequence_number != 0)
                {

                  //
                  // Convert the sequence number to ascii using sprintf
                  //

                  char _sequence_convert[64];
                  memset(_sequence_convert,0,64);
                  sprintf(_sequence_convert,"%08d",_sequence_number);

                  //
                  // Now set that into the _x_signatures_sequence
                  // member variable
                  //

                  _x_signatures_sequence = _sequence_convert;

                }
            }
          else
            {
              _stack.error("AnalysisResults Creation failed badly.");
              _x_error = "internal";
            }
        }
      else
        {
          //-------------------------------------------------------------------
          //
          // #3 - Given that the sample is in interim status, go grab
          // the AnalysisRequest record to retrieve the most updated status.
          //
          //-------------------------------------------------------------------
          // This could be reduced further in the following way:
          // - If the submitted sample is a duplicate of an in-process
          //   sample, go and retrieve the state information from the
          //   database.  Otherwise, we already have it.
          // However:
          //   This is a well documented (if I do say so myself)
          //   "opportunity for enhancement" to use Morar-ese. So I am 
          //   opting for the more simple code of just grabbing that
          //   AnalysisRequest object in ALL interim state cases.
          //-------------------------------------------------------------------
          //
          // I don't actually care about the cookie...
          //
          CheckSum _checksum_object(_x_sample_checksum);
          AnalysisRequest _analysis_request(_checksum_object);
          //
          //-------------------------------------------------------------------
          //
          // Test this object for null
          //
          if (_analysis_request.IsNull() == true)
            {
              _stack.error("Creation of the AnalysisRequest object failed badly");
              _x_error = "internal";
            }
          else
            {
              //
              // The object is OK.  Go ahead and pull the required
              // information. (only the state)
              //
              AnalysisStateInfo _request_state(_analysis_request.State());
              
              // Assign the state value into the _x_analysis_state
              // through it's operator std::string() method.  (I'm
              // doing this explicitly since I hate that operator so
              // much)

              _x_analysis_state = (std::string)_request_state;

            }
        }
      
      //-------------------------------------------------------------------
      //-------------------------------------------------------------------
      //-------------------------------------------------------------------
      
    }
  //-------------------------------------------------------------------
  // Now all the information we need has been collected in one space.
  // Set all the response headers we can/should
  // This is a decision structure similar to the one above, but
  // different in the respect that we can't encounter "database
  // exceptions" or the like in here. So the structure is "pure"
  //-------------------------------------------------------------------
  if (_x_error.size() != 0)
    {
      _gwapi.setResponseHeader("X-Error",_x_error);
    }
  else
    {
      _gwapi.setResponseHeader( "X-Analysis-Gateway" , _x_analysis_gateway );
      _gwapi.setResponseHeader( "X-Sample-Checksum"  , _x_sample_checksum  );
      _gwapi.setResponseHeader( "X-Analysis-State"   , _x_analysis_state   );
      
      if (_x_analysis_finished.size() != 0)
        {

          _gwapi.setResponseHeader("X-Date-Analyzed",_x_analysis_finished);
          
          if (_x_signatures_sequence.size() != 0)
            {
              _gwapi.setResponseHeader("X-Signatures-Sequence",_x_signatures_sequence);
            }
        }
      else
        {
          _gwapi.setResponseHeader("X-Analysis-Cookie",_x_analysis_cookie);
        }
    }

  //-------------------------------------------------------------------
  // Here endeth the transaction...
  //-------------------------------------------------------------------

}
void AvisSubmitSample::transaction_error_cleanup()
{
  Lock _held_submission_mutex(_filter_mutex);
  StackTrace _stack(*_tracer,"transaction_error_cleanup[MUTEXED]");

  // In the event that there was an error in the sample submission
  // transaction that resulted in "transaction failure" of one type or 
  // another, the database is left in an indeterminate state.
  //
  // It is the responsibility of the servlet to detect this and
  // perform any database cleanup required in order to remove all
  // instances of the sample that was being submitted.
  // 
  // This includes:

  // If we have an "X-Analysis-Cookie" delete that row from the
  // AnalysisRequest table

  // Delete all corresponding rows from the Attributes table.
  // (note: this will be depricated soon.)
  
  // I don't know yet if I should also be deleting any AnalysisResults 
  // row. I have to suppose not.

  // delete the sample files (both content and attributes) if they
  // exist.

  // There is a significant exposure for serialization trouble (seeing 
  // as how the database is code is "less than optimally thread safe."
  //  Therefore, my approach is to take the same mutex object that was 
  //  used to serialize access to the Filter creation (and, therefore, 
  //  the database routines that most likely caused the error we are
  //  trying to recover from) and lock it a second time in order to
  //  prevent any clashes with the "partially cleaned up" sample (in
  //  the unlikely real-world event of a duplicate of a
  //  partially-failed sample submission)
  //

  // Is there a "valid" cookie number here?

  //------------------------------------------------------------
  // Test to see if we even have anything to clean up. (it's possible
  // that we never even got that far.)
  //------------------------------------------------------------
  if (_x_analysis_cookie.size() != 0)

    {

      //============================================================
      //
      // Create the required database access class objects to run 
      // the two sql statements against the database.
      //
      //============================================================

      //------------------------------------------------------------
      // Create a DBSession (i.e. HDBC ODBC connectionhandle)
      // This connects in the constructor.
      //------------------------------------------------------------

      DBSession _db_connection; 
      
      //------------------------------------------------------------
      
      SQLStmt _delete_sql(_db_connection,
                          "Cleanup of possibly bad AnalysisRequest",
                          AVISDBException::etSQLStmt);
      
      //============================================================
      //
      // Build the SQL string for deleting the row from the
      // AnalysisRequest table, and "ExecDirect" it.
      //
      //============================================================

      std::string _sql_string("delete from analysisrequest where localcookie = ");
      _sql_string.append(_x_analysis_cookie);

      //------------------------------------------------------------
      // I've no choice here but to call SQLStmt.ExecDirect with the
      // "throw exceptions" flag set to false. (this is because the
      // SQLExecDirect ODBC call will return "SQL_NO_DATA" in the
      // event that a delete is executed and there were no rows to
      // delete.  The behavior of the SQLStmt class is such that it
      // does not differentiate past "SQL_SUCCESS",
      // "SQL_SUCCESS_WITH_INFO", and "SQL_ERROR".  The return of
      // SQL_NO_DATA will cause a condition that the SQLStmt class
      // thinks is some form of critical error and throw an exception
      // if it's allowed to. 
      //
      // Nice design sparky. :/
      //
      //------------------------------------------------------------

      _stack.info(_sql_string);
      _delete_sql.ExecDirect(_sql_string,true,false);

      //============================================================
      //
      // Delete any rows from the attributes table associated with
      // this cookie number.
      //
      //============================================================
      
      _sql_string = "delete from attributes where localcookie = ";
      _sql_string.append(_x_analysis_cookie);

      //------------------------------------------------------------
      // Now execute that SQL statement and delete the block of rows
      // from the Attributes table.
      //------------------------------------------------------------

      _stack.info(_sql_string);
      _delete_sql.ExecDirect(_sql_string,true,false);

      //============================================================

      //============================================================
      // Delete the sample content and sample attributes files if they 
      // exist.
      //============================================================

      //------------------------------------------------------------
      //
      // Build the name of the sample content file.
      //
      //------------------------------------------------------------

      std::string _file_base(_environment["sampleDirectory"]);
      _file_base.append("\\");
      _file_base.append((*(_incoming_attributes.find("X-Sample-Checksum"))).second);

      std::string _full_file(_file_base + ".sample");
      
      //------------------------------------------------------------
      //
      // Delete the .sample file
      //
      //------------------------------------------------------------

      _stack.info("Attempting to delete the sample content file");

      if (unlink(_full_file.c_str()) == 0)
        {
          _stack.info("deleted sample content file");
        }
      else
        {
          _stack.info("Unable to delete content file (no biggie)");
        }

      //------------------------------------------------------------
      //
      // Now for the .attributes file
      //
      //------------------------------------------------------------
      _stack.info("Attempting to delete the sample attributes file");
      
      _full_file = _file_base + ".attributes";
      
      if (unlink(_full_file.c_str()) == 0)
        {
          _stack.info("deleted attributes file");
        }
      else
        {
          _stack.info("Unable to delete attributes file (no biggie)");
        }
  
      //============================================================

    }

}
