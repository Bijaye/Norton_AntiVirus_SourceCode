//
//	IBM AntiVirus Immune System
//
//	File Name:	SampleUploader.cpp
//	Author:		Milosz Muszynski
//
//	This class is based on the CommClient and Thread classes
//	and provides a thread for a specific communication function
//	In this case the function is uploading the sample
//	to the gateway
//
//	$Log: $
//

// ------ includes ----------------------------------------------------
#include "base.h"
#include <string>
#include "SampleUploader.h"
#include "ZExcept.h"

// ------ local definitions -------------------------------------------
#define SAMPLE_UPLOADER_SHUTDOWN_TIMEOUT 5



// ------ SampleUploader ----------------------------------------------

//
// constructor
//

SampleUploader::SampleUploader(
	const std::string&				logFileName,
	const std::string&				traceFileName,
	const std::string&				agentName,
	RawEvent&						eventStop,
	MyTraceClient&					traceClient)
  : CommClient(	
				logFileName,  
				traceFileName,
				agentName,    
				eventStop,
				traceClient,
        CFG_sample_uploader_servlet,
        HttpConnection::HTTP_VERB_POST),
	_pstream( NULL )
{
	_tc.enter("Entering SampleUploader constructor...");
	_tc.exit("Leaving SampleUploader constructor.");
}


//
// destructor
//

SampleUploader::~SampleUploader()
{
	_tc.enter("Entering SampleUploader destructor...");
	_tc.exit("Leaving SampleUploader destructor.");
}


//
// Go
//

void SampleUploader::Go(	const std::string&				gatewayURL,
							const std::string&				headers,
							IcePackAgentComm::Stream&		stream )
{
	_gatewayURL			=	gatewayURL;
	_responseHeaders	=	headers;
	_pstream			=	&stream;
	CommClient::Go();
}


//
// ClientMain
//

IcePackAgentComm::ErrorCode SampleUploader::ClientMain()
{
	do
	{	
		if ( WaitForGo() )
			break;
    _errorCode = Launch();

		_tc.msg("PostSuspectSample operation ended with rc = %d", (unsigned int)_errorCode );
		_tc.info("end ---------------------------------------------------");

		Done();
	}
	while( true );

	return _errorCode;
}


//
// End
//

BOOL SampleUploader::End(DWORD, BOOL)
{
	return Thread::End(SAMPLE_UPLOADER_SHUTDOWN_TIMEOUT * 1000);
}


//
// PostSuspectSample
//

IcePackAgentComm::ErrorCode SampleUploader::_reallyLaunch(const std::string &gateway, const std::string &target, const HttpConnection::connectionVerb verb)
{
	SampleUploaderTransaction txSU( _agentName, gateway, *_pstream, _eventStop, _tc, _useSSL, target, verb );

	_tc.msg( "running PostSuspectSample operation..." );

	txSU.EmptyRequestHeaders();
	//_tc.debug( "emptied request headers" );

	txSU.AddRequestHeader( _responseHeaders );
  txSU.AddRequestHeader( "Connection: close" );

	_tc.debug( "added required constant headers" );

	// don't need to send any content
	IcePackAgentComm::ErrorCode errorCode = (IcePackAgentComm::ErrorCode)txSU.Execute();
	if		( errorCode == AVISTX_SUCCESS				) errorCode = IcePackAgentComm::NoError; 
	else if	( errorCode == AVISTX_NETWORK_FAILURE		) errorCode = IcePackAgentComm::NetworkError;
	else if ( errorCode == AVISTX_NO_HTTP_CODE			) errorCode = IcePackAgentComm::NoHttpCodeError;
	else if ( errorCode == AVISTX_CRITICAL_ERROR		) errorCode = IcePackAgentComm::NetworkCriticalError;
	else if ( errorCode == AVISTX_GATEWAY_ERROR			) errorCode = IcePackAgentComm::GatewayError;
	else if ( errorCode == AVISTX_TERMINATED_BY_CALLER	) errorCode = IcePackAgentComm::TerminatedByCaller;
  else if ( errorCode == AVISTX_YOUVE_BEEN_REDIRECTED ) errorCode = IcePackAgentComm::YouveBeenRedirected;

	_tc.msg("PostSuspectSample has completed.");

	_responseHeaders = txSU.ResponseHeaders();

	return errorCode;
}

