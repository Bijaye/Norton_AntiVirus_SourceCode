//
//	IBM AntiVirus Immune System
//
//	File Name:	StatusReceiver.cpp
//	Author:		Milosz Muszynski
//
//	This class is based on the CommClient and Thread classes
//	and provides a thread for a specific communication function
//	In this case the function is inquiring the gateway for the
//	status of a sample
//
//	$Log: $
//


// ------ headers -----------------------------------------------------
#include "base.h"
#include <string>
#include "StatusReceiver.h"
#include "ZExcept.h"
#include "AttributeKeys.h"

// ------ local definitions -------------------------------------------
#define STATUS_RECEIVER_SHUTDOWN_TIMEOUT 5



// ------ StatusReceiver ----------------------------------------------

//
// constructor
//

StatusReceiver::StatusReceiver(
	const std::string&				logFileName,
	const std::string&				traceFileName,
	const std::string&				agentName,
	RawEvent&						eventStop,
	MyTraceClient&					traceClient )
  : CommClient(	
				logFileName,  
				traceFileName,
				agentName,    
				eventStop,
				traceClient,
        CFG_status_receiver_servlet_name,
        HttpConnection::HTTP_VERB_GET),
	_cookie()
{
	_tc.enter("Entering StatusReceiver constructor...");
	_tc.exit("Leaving StatusReceiver constructor.");
}


//
// destructor
//

StatusReceiver::~StatusReceiver()
{
	_tc.enter("Entering StatusReceiver destructor...");
	_tc.exit("Leaving StatusReceiver destructor.");
}


//
// Go
//

void StatusReceiver::Go(	const std::string&	gatewayURL,
                          const std::string&  headers,
                        const std::string&	cookie, const std::string& sampleChecksumSubmitted )
{
	_gatewayURL = gatewayURL;
	_cookie		= cookie;
  _sampleChecksumSubmitted = sampleChecksumSubmitted;
  _responseHeaders = headers;
	CommClient::Go();
}


//
// ClientMain
//

IcePackAgentComm::ErrorCode StatusReceiver::ClientMain()
{
	do
	{	
		if ( WaitForGo() )
			break;
    _errorCode = Launch();

		_tc.msg("GetSampleStatus operation ended with rc = %d", (unsigned int)_errorCode );
		_tc.info("end ---------------------------------------------------");

		Done();
	}
	while( true );

	return _errorCode;
}


//
// End
//

BOOL StatusReceiver::End(DWORD, BOOL)
{
	return Thread::End(STATUS_RECEIVER_SHUTDOWN_TIMEOUT * 1000);
}


//
// GetSampleStatus
//

IcePackAgentComm::ErrorCode StatusReceiver::_reallyLaunch(const std::string &gateway, const std::string &target, const HttpConnection::connectionVerb verb)
{
  std::string requestHeader;
  StatusReceiverTransaction txSR( _agentName, gateway, _cookie, _eventStop, _tc, _useSSL, target, verb );

	_tc.msg( "running GetSampleStatus operation..." );

	txSR.EmptyRequestHeaders();
	//_tc.debug( "emptied request headers" );

  if (0 != _cookie.size())
  {
    requestHeader = AttributeKeys::AnalysisCookie() + ": " + _cookie;
	  txSR.AddRequestHeader( requestHeader );
    _tc.debug( "added header: %s", requestHeader.c_str() );
  }

  /*
   * Get the sample checksum, and put the header into the status request.
   */
  if (0 != _sampleChecksumSubmitted.size())
  {
    requestHeader = AttributeKeys::SampleCheckSum() + ": " + _sampleChecksumSubmitted;
    txSR.AddRequestHeader( requestHeader);
    _tc.debug( "added header: %s", requestHeader.c_str() );
  }

  /*
   * inw 2000-02-08 added to paste in the headers sent down from above.
   */
  txSR.AddRequestHeader( _responseHeaders );

  txSR.AddRequestHeader( "Connection: close" );

	// don't need to send any content
	IcePackAgentComm::ErrorCode errorCode = (IcePackAgentComm::ErrorCode)txSR.Execute();
	if		( errorCode == AVISTX_SUCCESS )			errorCode = IcePackAgentComm::NoError; 
	else if	( errorCode == AVISTX_NETWORK_FAILURE ) errorCode = IcePackAgentComm::NetworkError;
	else if ( errorCode == AVISTX_NO_HTTP_CODE    ) errorCode = IcePackAgentComm::NoHttpCodeError;
	else if ( errorCode == AVISTX_CRITICAL_ERROR  ) errorCode = IcePackAgentComm::NetworkCriticalError;
	else if ( errorCode == AVISTX_GATEWAY_ERROR   ) errorCode = IcePackAgentComm::GatewayError;
	else if ( errorCode == AVISTX_TERMINATED_BY_CALLER	) errorCode = IcePackAgentComm::TerminatedByCaller;
  else if ( errorCode == AVISTX_YOUVE_BEEN_REDIRECTED ) errorCode = IcePackAgentComm::YouveBeenRedirected;
	
	_tc.msg("GetSampleStatus has completed.");

	_responseHeaders = txSR.ResponseHeaders();

	return errorCode;
}

