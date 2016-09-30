//
//	IBM AntiVirus Immune System
//
//	File Name:	SignatureInquirerTransaction.cpp
//	Author:		Milosz Muszynski
//
//	This class is based on the CAVISTransaction class
//	and overrides some of its virtual functions in order
//	to perform transaction to inquire for a signature
//	set availability
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

// ------ transaction headers -----------------------------------------
#include "SignatureInquirerTransaction.h"
#include "AttributeKeys.h"
#include "Config.h"

// ------ utility headers ---------------------------------------------
#include "StrStrMap.h"
#include "StrUtil.h"
#include "Util.h"





// ------ SignatureInquirerTransaction --------------------------------

//
// Constructor
//
SignatureInquirerTransaction::SignatureInquirerTransaction(
	const std::string&				agentName,
	const std::string&				gatewayURL,
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
		useSSL)
{
	_tc.enter("Entering SignatureInquirerTransaction constructor...");
	_tc.exit("Leaving SignatureInquirerTransaction constructor.");
}


//
// Destructor
//
SignatureInquirerTransaction::~SignatureInquirerTransaction()
{
	_tc.enter("Entering SignatureInquirerTransaction destructor...");
	_tc.exit("Leaving SignatureInquirerTransaction destructor.");
}


bool SignatureInquirerTransaction::SignatureWasAvailableAtGateway()
{
	// for this transaction, the gateway sends 'ok' when there is a signature available
	return GetResponseStatusCode() == HTTP_STATUS_OK;
}


//
// StartRequest
//

int SignatureInquirerTransaction::StartRequest(	DWORD	cbSendContent,		// how much data to send
												PVOID	pvSendContent	)	// data to send
{
	_rc = SendRequest(cbSendContent, pvSendContent);
	return _rc;
}

//
// EndRequest
//

void SignatureInquirerTransaction::EndRequest()
{
	if ( _rc == AVISTX_SUCCESS )
	{
		_tc.debug( "starting to read the response headers..." );

		RetrieveResponseHeaders();

		StrStrMap headers( _responseHeaders );

		_sequenceNumber = headers.getString( AttributeKeys::SignatureSequence() );

		_tc.debug( "response headers have been read" );
	}
}







