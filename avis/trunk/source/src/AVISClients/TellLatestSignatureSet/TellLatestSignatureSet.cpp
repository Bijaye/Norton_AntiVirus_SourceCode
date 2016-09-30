//
//	IBM AntiVirus Immune System
//
//	File Name:	TellLatestSignatureSet.cpp
//	Author:		Milosz Muszynski
//
//	This is a command line utility program with double purpose:
//	1 - it is a sample program to show how to use two of 
//  	the IcePackAgentComm module functions
//  2 - it is an utility that can be used for testing
//  
//  if you invoke the program without parameters you'll see
//  information about its arguments
//  
//
//	$Log: $
//

//// ------ mfc headers -------------------------------------------------
//// hopefully we'll be able to eliminate MFC from here
//// the only reason MFC headers are here is the MyTraceClient
//// class that uses an MFC collection class
//#include <afxwin.h>

// ------ standard headers --------------------------------------------
#include <fstream.h>
#include <string>

// ------ IcePackAgentComm headers ------------------------------------
#include "MyTraceClient.h"
#include "IcePackAgentComm.h"

// ------ namespaces --------------------------------------------------
using namespace IcePackAgentComm;
using namespace std;

//
// TellLatestSignatureSet
//

void TellLatestSignatureSet(	const string&	gatewayURL	= "http://avisgw1.watson.ibm.com:888", 
								bool			blessed		= false,
								bool			loop		= false	)
{
	SignatureComm			signatureComm;
	static MyTraceClient	traceClient;

	traceClient.EnableTracing();

	signatureComm.SetTraceClient( traceClient, "test.log", "test.trc", "Main Thread:" );

	string sequenceNumber;

	IcePackAgentComm::ErrorCode errorCode = IcePackAgentComm::NoError;
	unsigned long				systemExceptionCode = 0;

	do
	{
		cout << "wait..." << endl;

		bool b;

		if ( blessed )
		{
			b = signatureComm.GetLatestBlessed( gatewayURL, sequenceNumber );
		}
		else
		{
			b = signatureComm.GetLatest( gatewayURL, sequenceNumber );
		}

		if ( !b )
		{
			errorCode = signatureComm.Error();
			systemExceptionCode = signatureComm.SystemExceptionCode();
		}

		cout << "latest " << (blessed ? "blessed " : "") << "sequence number = " << sequenceNumber.c_str() << endl;

		if ( !b )
		{
			std::string msg;
			GetErrorMessage( errorCode, msg );
			cout << "result = " << msg.c_str() << " exception code = " << hex << systemExceptionCode << endl;
		}

		if ( loop )
			blessed = !blessed;
	}
	while( loop );
}
//
// main
//

int main( int argc, char ** argv )
{
	if ( argc > 3 )
	{
		TellLatestSignatureSet( argv[ 1 ], argv[ 2 ][0]=='y' || argv[ 2 ][0]=='Y', argv[ 3 ][0]=='y' || argv[ 3 ][0]=='Y' );
	}
	if ( argc > 2 )
	{
		TellLatestSignatureSet( argv[ 1 ], argv[ 2 ][0]=='y' || argv[ 2 ][0]=='Y' );
	}
	else if ( argc > 1 )
	{
		TellLatestSignatureSet( argv[ 1 ] );
	}
	else
	{
		cout << "args: URL [blessed=y|n]" << endl;// hidden 3rd parameter - loop 
		cout << "e.g.: http://avisgw1.watson.ibm.com:888 y" << endl;
	}

	return 0;
}

