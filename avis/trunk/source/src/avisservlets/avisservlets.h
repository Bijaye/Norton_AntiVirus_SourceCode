// hgwapi.h - HelloGwapi 

#if !defined (__HGWAPI_H__)
#define __HGWAPI_H__

#include "htapi.h"


//
//: transaction handler for getSampleStatus
//
// One of the five client side gateway transactions, getSampleStatus
// is used, well, to query the status of a sample previously submited
// to the gateway for analysis. 
//
// Request Headers:
//     X-Sample-Checksum
//     X-Analysis-Cookie
//
//     either "X-Sample-Checksum" (preferrably) OR
//     "X-Analysis-Cookie" is required to specify the sample being
//     queried.
//
//
// Possible Response Headers:
//     X-Analysis-State
//     X-Sample-Checksum
//     X-Date-Analyzed
//     X-Signatures-Sequence
//     X-Error
//
// Response:
//     If the sample exists in the gateway database, it's status is
//     returned in the "X-Analysis-State" header.
//     If there is a corresponding row in the AnalysisResults table,
//     then "X-Date-Analyzed" is set and the signature sequence number
//     required for handling of this sample is returned in
//     "X-Signatures-Sequence"
//
//     If the sample specified is not found in the gateway database,
//     "X-Error: crumbled" is returned to the client.
//

extern "C" void HTTPD_LINKAGE getSampleStatus       ( unsigned char* handle_, 
                                                      long*          return_code_);



//
//: transaction handler for postSampleStatus
//
//
// Request Headers:
//     X-Sample-Checksum
//     X-Analysis-State
//     [X-Date-Analyzed]
//     [X-Signatures-Sequence]
//    
//
// Possible Response Headers:
//     X-Error
//
// postSampleStatus is one of the three transactions executed only by
// the Analysis Center, used to update the Gateway database with the 
// current state of a sample in process, or it's final state, if
// sample processing is complete.
//

extern "C" void HTTPD_LINKAGE postSampleStatus      ( unsigned char* handle_, 
                                                      long*          return_code_);

//
//: transaction handler for getSignatureSet and headSignatureSet
//
//
// Request Headers:
//     X-Signatures-Sequence
//
// Possible Response Headers
//     X-Signatures-Sequence
//     X-Date-Published
//     X-Date-Produced
//     [X-Date-Blessed]
//     X-Error
//
// The getSignatureSet servlet operates in two distinct modes.  If the 
// client executes the servlet as an http "HEAD" request (this is
// aliased as "headSignatureSet in the web server configuration) then
// only the headers are returned to the client, along with any error
// information concerning superceded or unavailable signatures.  This
// is used by the clients to determine whether or not the gateway has
// a blessed definition package later than the one the client is
// using.
//
// If the transaction is executed as a normal http "GET" the header
// response is exactly the same, and the definition package content is 
// returned (if available and not superceded) as well.
//
// the X-Signatures-Sequence attribute indicates which definition
// package the client is "interested in."  It can either be a literal
// sequence number, or one of two special values "blessed" or
// "newest". 
//
// If X-Signatures-Sequence is "blessed" information is gathered about 
// the latest blessed signatures currently in use on the gateway.  If
//
// X-Signatures-Sequence is "newest" then information is gathered
// about the latest signatures (blessed or not) currently in use on
// the gateway.
// 

extern "C" void HTTPD_LINKAGE getSignatureSet       ( unsigned char* handle_, 
                                                      long*          return_code_);

//
//: transaction handler for postSignatureSet
//
// Request Headers:
//     X-Signatures-Sequence
//     X-Content-Checksum
//     X-Signatures-Checksum
//     X-Date-Published
//     X-Date-Produced
//     [X-Date-Blessed] 
//
//
// Possible Response Headers
//     X-Error
// 

extern "C" void HTTPD_LINKAGE postSignatureSet      ( unsigned char* handle_, 
                                                      long*          return_code_);

//
//: transaction handler for getSuspectSample
//
// Request Headers:
//  none
//
// Possible Response Headers
//   X-Analysis-Cookie
//   <sample attributes>
// 

extern "C" void HTTPD_LINKAGE getSuspectSample      ( unsigned char* handle_, 
                                                      long*          return_code_);

//
//: transaction handler for postSuspectSample
//
// Request Headers:
//
// Possible Response Headers
// 

extern "C" void HTTPD_LINKAGE postSuspectSample     ( unsigned char* handle_, 
                                                      long*          return_code_);

//
//: transaction handler for redirectSignatureSet
//
// Request Headers:
//
// Possible Response Headers:
//

extern "C" void HTTPD_LINKAGE redirectSignatureSet  ( unsigned char* handle_,
                                                      long*          return_code_);

//
//: transaction handler for redirectSuspectSample
//
// Request Headers:
//
// Possible Response Headers
// 

extern "C" void HTTPD_LINKAGE redirectSuspectSample ( unsigned char* handle_,
                                                      long*          return_code_);

//
//: avisservlets.dll initialization callback to the Lotus Go webserver
//

extern "C" void HTTPD_LINKAGE servletInit           ( unsigned char* handle_,
                                                      unsigned long* major_version_,
                                                      unsigned long* minor_version_,
                                                      long*          return_code_);
//
//: avisservlets.dll termination callback to the Lotus Go webserver
//

extern "C" void HTTPD_LINKAGE servletTerm           ( unsigned char* handle_,
                                                      long*          return_code_);


#endif
