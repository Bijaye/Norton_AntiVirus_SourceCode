//
//	IBM AntiVirus Immune System
//
//	File Name:	StatusReceiverTransaction.cpp
//	Author:		Milosz Muszynski
//
//	This class is based on the CAVISTransaction class
//	and overrides some of its virtual functions in order
//	to perform transaction to inquire for a sample
//	status
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
#include "StatusReceiverTransaction.h"
#include "Config.h"

// ------ utility headers ---------------------------------------------
#include "StrStrMap.h"
#include "StrUtil.h"
#include "Util.h"





// ------ StatusReceiverTransaction --------------------------------

//
// Constructor
//
StatusReceiverTransaction::StatusReceiverTransaction(
	const std::string&				agentName,
	const std::string&				gatewayURL,
	const std::string&				cookie,
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
	_tc.enter("Entering StatusReceiverTransaction constructor...");
	_tc.exit("Leaving StatusReceiverTransaction constructor.");
}


//
// Destructor
//
StatusReceiverTransaction::~StatusReceiverTransaction()
{
	_tc.enter("Entering StatusReceiverTransaction destructor...");
	_tc.exit("Leaving StatusReceiverTransaction destructor.");
}


//
// StartRequest
//

int StatusReceiverTransaction::StartRequest(	DWORD	cbSendContent,		// how much data to send
												PVOID	pvSendContent	)	// data to send
{
	_rc = SendRequest(cbSendContent, pvSendContent);
	return _rc;
}

//
// EndRequest
//

void StatusReceiverTransaction::EndRequest()
{
	if ( _rc == AVISTX_SUCCESS )
	{
		_tc.debug( "starting to read the response headers..." );

		RetrieveResponseHeaders();

		_tc.debug( "response headers have been read" );
	}
}










