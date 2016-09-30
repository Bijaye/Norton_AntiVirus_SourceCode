// asubsamp.h - Prototype for the AvisSubmitSample class

#if !defined (__ASUBSAMP_H__)
#define __ASUBSAMP_H__ 

#include "aviscommon/log/namedmutex.h"
#include "avisfilter/stdafx.h"
#include "avisfilter/filter.h" // AvisFilter

#include "goservletclient.h"

class Attributes;
class AnalysisRequest;


//
//: Handler for the postSuspectSample transaction
//
// AvisSubmitSample is by far the most complex of the Gateway
// transactions.  As such, I'm going to bail out on detailed
// documentation for the moment.
//

class AvisSubmitSample : virtual public GoServletClient
{
public:

  //
  //: Object Constructor
  //

  AvisSubmitSample(GoServletHost& host_,const char* gwapihandle_);

  //
  //: Virtual Destructor
  //

  virtual ~AvisSubmitSample();

protected:

  //
  //: Primary logic driver method, called by the base class
  //

  virtual int fulfillRequest();

private:

  //
  //: Internal logic driver, called by fulfillRequest to shield
  //: exceptions.
  //

  bool liveProcessing();
  
  //
  //: Execute the "Filter" the sample being submitted.
  //  This returns an STL pair of "FilterStatus and long" to retrieve
  //  both the state of the sample after handling, and the localCookie
  //  number if any.
  //

  std::pair<Filter::FilterStatus,long> executeFilter(Filter&);


  //
  //: Export sample to analysis center for further processing
  //

  void action_export(Filter&,const std::string&,long);

  //
  //: Set the state to an intermediate state,
  //: (cookie,AnalysisStateInfo string)
  //

  void action_intermediate(long,const std::string&);

  //
  //: Set a finished state
  //

  void action_finished(long,const std::string&);

  //
  //: Piggypacked sample handler
  //

  void action_piggyback(long);

  //
  //: Decline submission of the sample
  //
  // If a sample is declined an X-Date-Finished header
  // is still generated for the transaction response, however this
  // sample is never entered into the database.
  //

  void action_declined();

  //
  //: attempt to save the content and attributes files, returning a success
  //: flag
  //
  // This method calls save_attributes_file and save_content_file to
  // do it's dirty work for it.
  //

  bool save_local_files(Filter& filter_,const std::string& header_block_);

  //
  //: Save attribute block off to a file named "<sample-checksum>.attributes"
  //

  bool save_attributes_file(const std::string& header_block_) const;

  //
  //: Simple wrapper to save the content to disk (as received)
  //
  // It is worth noting that the AvisSubmitSample object does NOT
  // maintain an internal copy of the content, but the Filter object
  // does.  Therefore saving the content (as transmitted over HTTP) to
  // disk is a matter of calling a method within the filter object,
  // NOT saving a local buffer.
  //

  bool save_content_file(Filter&);

  // 
  //: Gather up all state information used to generate a client
  // response, and dispatch that response.
  //
  // Gather any remaining information about the "current working
  // sample" and set all response headers. Note that ALL calls to
  // setResponseHeader have been moved into retrieve_status to cut
  // down heavily on the code pasta factor.
  //
  // Get the sample status headers from the Attributes of the
  // sample in the database.  Include those headers in the
  // HTTP response (and return a copy of the string as well.)
  //

  void dispatch_response();


  //
  //: Expunge the sample from the disk and database in the event of a
  //: transaction failure
  //
  // Should the transmission or scanning of this sample fail, or
  // another unpredicted error occur for any reason, this method
  // cleans all traces of the submitted sample from the file system
  // and database.
  //

  void transaction_error_cleanup();
  
  //
  //: Actual MD5 hash of the content
  //
  // Calculated as the content comes across the HTTP stream, verified
  // against the required X-Content-Checksum header.
  //

  std::string _content_hash;

  //
  //: Attributes submitted as HTTP headers by the client
  //

  MStringString _incoming_attributes;


  //
  //: Any X-Error response we may need to generate
  //

  std::string _x_error;

  //
  //: Response telling the client where to look for status updates for
  //: this sample.
  //

  std::string _x_analysis_gateway;

  //
  //: Database "Cookie" or row ID for this sample in the
  //: AnalysisRequest table.
  //
  // Note that this field is no longer used for keying into the
  // database.  This function is not fulfilled by the
  // X-Sample-Checksum field. Cookie values are still generated.
  //
  
  std::string _x_analysis_cookie;

  //
  //: State of the sample after submission (or the status as pulled
  //: from the database in the case of a duplicate submission)
  //

  std::string _x_analysis_state;

  //
  //: If the gateway was able to "complete processing" of this sample,
  //: an X-Analysis-Finished timestamp is created or fetched from the
  //: database.
  //
  // Note that the presence of an X-Analysis-Finished timestamp is the
  // primary indication of the completion of sample processing.
  //

  std::string _x_analysis_finished;
  
  //
  //: Signatures required for download in the event of "Gateway
  //: Completion"
  //

  std::string _x_signatures_sequence;

  //
  //: Checksum of the sample itself.  Used in preference to the
  //: X-Analysis-Cookie for status queries
  //

  std::string _x_sample_checksum;

  //
  //: Process Lock object used to serialize both the Filter object
  //: constructor and the cleanup of failed sample submission
  //: transactions
  //
  
  NamedMutex _filter_mutex;

  //
  //: Standard Trace object
  //

  TraceClient* _tracer;
};


#endif // __ASUBSAMP_H__
