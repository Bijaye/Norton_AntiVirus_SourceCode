//
//	IBM AntiVirus Immune System
//
//	File Name:	SampleUploaderTransaction.cpp
//	Author:		Milosz Muszynski
//
//	This class is based on the CAVISTransaction class
//	and overrides some of its virtual functions in order
//	to perform transaction to upload a suspect sample
//	to the gateway
//
//	$Log: $
//


// ------ Microsoft headers -------------------------------------------
#include "base.h"

//------ pragmas ------------------------------------------------------
#pragma warning ( disable : 4786 ) // identifier truncated

// ------ standard C++ headers ----------------------------------------
#include <string>
#include <map>
#include <fstream.h>
#include <strstrea.h>

// ------ transaction headers -----------------------------------------
#include "SampleUploaderTransaction.h"
#include "Config.h"

// ------ utility headers ---------------------------------------------
#include "StrStrMap.h"
#include "StrUtil.h"
#include "Util.h"



// ------ local variables ---------------------------------------------
std::string SampleUploaderServletName = CFG_sample_uploader_servlet;



// ------ SampleUploaderTransaction --------------------------------

//
// Constructor
//
SampleUploaderTransaction::SampleUploaderTransaction(
	const std::string&				agentName,
	const std::string&				gatewayURL,
	IcePackAgentComm::Stream&		stream,
	RawEvent&						eventStop,
	MyTraceClient&					traceClient,
	bool							useSSL,
  const std::string&        target,
  const HttpConnection::connectionVerb    verb)
  : TransactionExecutor(
		agentName,
		gatewayURL,
		verb,
		traceClient,
		eventStop,
		target,
		useSSL),
	_stream( stream ),
	_errorCode( IcePackAgentComm::NoError )
{
	_tc.enter("Entering SampleUploaderTransaction constructor...");
	_tc.exit("Leaving SampleUploaderTransaction constructor.");
}


//
// Destructor
//
SampleUploaderTransaction::~SampleUploaderTransaction()
{
	_tc.enter("Entering SampleUploaderTransaction destructor...");
	_tc.exit("Leaving SampleUploaderTransaction destructor.");
}


//
// InitRequest
//

DWORD SampleUploaderTransaction::InitRequest()
{
	AddRequestHeader( std::string( "Content-Type: " ) + CFG_sample_uploader_content_type );

	unsigned long errorCode;
	if ( !_stream.init( errorCode ) )
	{
		_tc.critical( "Stream::init() failed, error code = %8.8x", errorCode );
		_errorCode = static_cast<IcePackAgentComm::ErrorCode>(errorCode);
		return errorCode;//AVISTX_CRITICAL_ERROR;
	}

	_errorCode = static_cast<IcePackAgentComm::ErrorCode>(errorCode);
	return AVISTX_SUCCESS;
}


//
// EndRequest
//

void SampleUploaderTransaction::EndRequest()
{
	if ( _errorCode == IcePackAgentComm::NoError )
	{
		_tc.debug( "starting to read the response headers..." );

		RetrieveResponseHeaders();

		_tc.debug( "response headers have been read" );
	}
}


//
// StartRequest
//

int SampleUploaderTransaction::StartRequest(DWORD, PVOID)
{
  int rc = SendRequest( _stream.size(), _stream.start() );
	if ( rc == AVISTX_SUCCESS )
	{
		_errorCode = IcePackAgentComm::NoError;
	}
	else if ( rc == AVISTX_FILE_READ_ERROR )
	{
		_errorCode = IcePackAgentComm::FileReadError;
	}
	else
	{
		_errorCode = IcePackAgentComm::NetworkError;
	}

	return rc;
}


//
// GetNextRequestContentBlock
//

int SampleUploaderTransaction::GetNextRequestContentBlock( PVOID pvRequestContentBlock )
{
	unsigned long errorCode;
	unsigned long size = ReadBufferSize;

	if ( !_stream.read( (unsigned char *)pvRequestContentBlock, size, errorCode ) )
	{
		_tc.critical( "Stream::read() failed, error code = %8.8x", errorCode );
		return -1;
	}

	return size;
}





