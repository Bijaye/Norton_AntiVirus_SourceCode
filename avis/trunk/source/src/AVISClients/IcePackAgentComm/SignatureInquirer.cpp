//
//	IBM AntiVirus Immune System
//
//	File Name:	SignatureInquirer.cpp
//	Author:		Milosz Muszynski
//
//	This class is based on the CommClient and Thread classes
//	and provides a thread for a specific communication function
//	In this case the function is inquiring the gateway for the
//	availability of the given signature set
//
//	$Log: $
//


// ------ headers -----------------------------------------------------
#include "base.h"
#include <string>
#include "SignatureInquirer.h"
#include "ZExcept.h"
#include "AttributeKeys.h"
#include "Config.h"

// ------ local definitions -------------------------------------------
#define SIGNATURE_IMPORTER_SHUTDOWN_TIMEOUT 5


// ------ static data -------------------------------------------------
std::string SignatureInquirer::NameForLatest		= CFG_name_for_latest_signature;
std::string SignatureInquirer::NameForLatestBlessed	= CFG_name_for_latest_blessed_signature;


// ------ SignatureInquirer -------------------------------------------

//
// constructor
//

SignatureInquirer::SignatureInquirer(
	const std::string&				logFileName,
	const std::string&				traceFileName,
	const std::string&				agentName,
	RawEvent&						eventStop,
	MyTraceClient&					traceClient )
  : CommClient(	logFileName,  
				traceFileName,
				agentName,    
				eventStop,
				traceClient,
        CFG_signature_inquirer_servlet_name,
        HttpConnection::HTTP_VERB_HEAD),
	_signatureType(),
	_sequenceNumber()
{
	_tc.enter("Entering SignatureInquirer constructor...");
	_tc.exit("Leaving SignatureInquirer constructor.");
}


//
// destructor
//

SignatureInquirer::~SignatureInquirer()
{
	_tc.enter("Entering SignatureInquirer destructor...");
	_tc.exit("Leaving SignatureInquirer destructor.");
}


//
// Go
//

void SignatureInquirer::Go	(	const std::string&				gatewayURL,
                const std::string&				headers,
								const std::string&				sequenceNumber,
								IcePackAgentComm::SignatureType	signatureType	)
{
	_gatewayURL			= gatewayURL;
	_signatureType		= signatureType;
	_sequenceNumber		= sequenceNumber;
  _responseHeaders  = headers;
	CommClient::Go();
}


//
// ClientMain
//

IcePackAgentComm::ErrorCode SignatureInquirer::ClientMain()
{
	do
	{	
		if ( WaitForGo() )
			break;
    _errorCode = Launch();

		_tc.msg("GetSignatureSet operation ended with rc = %d", (unsigned int)_errorCode );
		_tc.info("end ---------------------------------------------------");

		Done();
	}
	while( true );

	return _errorCode;
}


//
// End
// 

BOOL SignatureInquirer::End(DWORD, BOOL)
{
	return Thread::End(SIGNATURE_IMPORTER_SHUTDOWN_TIMEOUT * 1000);
}

IcePackAgentComm::ErrorCode SignatureInquirer::_reallyLaunch(const std::string &gateway, const std::string &target, const HttpConnection::connectionVerb verb)
{
    SignatureInquirerTransaction txIS( _agentName, gateway, _eventStop, _tc, _useSSL, target, verb );

	_tc.msg( "running GetSignatureSet operation..." );

	txIS.EmptyRequestHeaders();
	//_tc.debug( "emptied request headers" );

  /*
   * inw 2000-02-08 added to paste in the headers sent down from above.
   *
   * At the time of writing, these are customer-contact-related headers,
   * but this might change.
   */
  txIS.AddRequestHeader( _responseHeaders );

	// add required headers for "getSignatureSet" transaction
	txIS.AddRequestHeader( CFG_import_signature_required_headers );

	std::string requestHeader = AttributeKeys::SignatureSequence() + ": ";

	if ( _signatureType == IcePackAgentComm::Regular )
	{
		requestHeader += _sequenceNumber;
	}
	else if ( _signatureType == IcePackAgentComm::Latest )
	{
		requestHeader += NameForLatest;
	}
	else if ( _signatureType == IcePackAgentComm::LatestBlessed )
	{
		requestHeader += NameForLatestBlessed;
	}
	else
	{
		_tc.critical( "unknown signature type" );
		return IcePackAgentComm::UnknownSignatureTypeError;
	}

	txIS.AddRequestHeader( requestHeader );
	txIS.AddRequestHeader( "Connection: close" );

	_tc.debug( "added required constant headers" );

	// don't need to send any content
	IcePackAgentComm::ErrorCode errorCode = (IcePackAgentComm::ErrorCode)txIS.Execute();
	if		( errorCode == AVISTX_SUCCESS )			errorCode = IcePackAgentComm::NoError; 
	else if	( errorCode == AVISTX_NETWORK_FAILURE ) errorCode = IcePackAgentComm::NetworkError;
	else if ( errorCode == AVISTX_NO_HTTP_CODE    ) errorCode = IcePackAgentComm::NoHttpCodeError;
	else if ( errorCode == AVISTX_CRITICAL_ERROR  ) errorCode = IcePackAgentComm::NetworkCriticalError;
	else if ( errorCode == AVISTX_GATEWAY_ERROR   ) errorCode = IcePackAgentComm::GatewayError;
	else if ( errorCode == AVISTX_TERMINATED_BY_CALLER	) errorCode = IcePackAgentComm::TerminatedByCaller;
  else if ( errorCode == AVISTX_YOUVE_BEEN_REDIRECTED ) errorCode = IcePackAgentComm::YouveBeenRedirected;
	
	_responseHeaders = txIS.ResponseHeaders();

	_sequenceNumber = txIS.SequenceNumber();

	return errorCode;
}

