//
//	IBM AntiVirus Immune System
//
//	File Name:	SignatureImporter.cpp
//	Author:		Milosz Muszynski
//
//	This class is based on the CommClient and Thread classes
//	and provides a thread for a specific communication function
//	In this case the function is importing the signature set 
//	from the gateway
//
//	$Log: $
//


// ------ headers -----------------------------------------------------
#include "base.h"
#include <string>
#include "SignatureImporter.h"
#include "ZExcept.h"
#include "AttributeKeys.h"
#include "Config.h"

// ------ local definitions -------------------------------------------
#define SIGNATURE_IMPORTER_SHUTDOWN_TIMEOUT 5


// ------ static data -------------------------------------------------
std::string SignatureImporter::NameForLatest		= CFG_name_for_latest_signature;
std::string SignatureImporter::NameForLatestBlessed	= CFG_name_for_latest_blessed_signature;


// ------ SignatureImporter -------------------------------------------

//
// constructor
//

SignatureImporter::SignatureImporter(
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
        CFG_signature_importer_servlet_name,
        HttpConnection::HTTP_VERB_GET),
	_signatureType(),
	_sequenceNumber(),
	_targetDirectory(),
	_withUnpacking( false )
{
	_tc.enter("Entering SignatureImporter constructor...");
	_tc.exit("Leaving SignatureImporter constructor.");
}


//
// destructor
//

SignatureImporter::~SignatureImporter()
{
	_tc.enter("Entering SignatureImporter destructor...");
	_tc.exit("Leaving SignatureImporter destructor.");
}


//
// Go
//

void SignatureImporter::Go	(	const std::string&				gatewayURL,
                              const std::string&        headers,
								const std::string&				sequenceNumber,
								const std::string&				targetDirectory,
								IcePackAgentComm::SignatureType	signatureType,
								bool							withUnpacking	)
{
	_gatewayURL			= gatewayURL;
	_signatureType		= signatureType;
	_sequenceNumber		= sequenceNumber;
	_targetDirectory	= targetDirectory;
	_withUnpacking		= withUnpacking;
  _responseHeaders  = headers;
	CommClient::Go();
}


//
// ClientMain
//

IcePackAgentComm::ErrorCode SignatureImporter::ClientMain()
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

BOOL SignatureImporter::End(DWORD, BOOL)
{
	return Thread::End(SIGNATURE_IMPORTER_SHUTDOWN_TIMEOUT * 1000);
}

IcePackAgentComm::ErrorCode SignatureImporter::_reallyLaunch(const std::string &gateway, const std::string &target, const HttpConnection::connectionVerb verb)
{
  SignatureImporterTransaction txIS( _agentName, gateway, _targetDirectory, _withUnpacking, _eventStop, _tc, _useSSL, target, verb);

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
		_tc.critical( "signature type unknown" );
		return IcePackAgentComm::UnknownSignatureTypeError;
	}

	txIS.AddRequestHeader( requestHeader );
	txIS.AddRequestHeader( "Connection: close" );
	
	_tc.debug( "added required constant headers" );

	// don't need to send any content
	IcePackAgentComm::ErrorCode errorCode = (IcePackAgentComm::ErrorCode)txIS.Execute();
	if		( errorCode == AVISTX_SUCCESS )			errorCode = IcePackAgentComm::NoContentError; 
	// this return code is sometimes OK but in this case it is an error because we need a content
	// I can't provide my own virtual function EndRequest for two reasons:
	// 1) this function is void
	// 2) CAVISTransaction member that contains HTTP response code is private
	else if	( errorCode == AVISTX_NETWORK_FAILURE ) errorCode = IcePackAgentComm::NetworkError;
	else if ( errorCode == AVISTX_NO_HTTP_CODE    ) errorCode = IcePackAgentComm::NoHttpCodeError;
	else if ( errorCode == AVISTX_CRITICAL_ERROR  ) errorCode = IcePackAgentComm::NetworkCriticalError;
	else if ( errorCode == AVISTX_GATEWAY_ERROR   ) errorCode = IcePackAgentComm::GatewayError;
	else if ( errorCode == AVISTX_TERMINATED_BY_CALLER	) errorCode = IcePackAgentComm::TerminatedByCaller;
  else if ( errorCode == AVISTX_YOUVE_BEEN_REDIRECTED ) errorCode = IcePackAgentComm::YouveBeenRedirected;
	
	// if the transaction ran successfully, but there was no content, 
	// then no signatures were imported
	if ((errorCode == IcePackAgentComm::NoError) && (!txIS.SignatureWasAvailableAtGateway()))
		errorCode = IcePackAgentComm::NoSignaturesAvailable;

	_tc.msg("GetSignatureSet has completed.");

	_responseHeaders = txIS.ResponseHeaders();

	return errorCode;
}

