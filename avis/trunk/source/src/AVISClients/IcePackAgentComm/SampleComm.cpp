//
//	IBM AntiVirus Immune System
//
//	File Name:	SampleComm.cpp
//	Author:		Milosz Muszynski
//
//	This class provides communication functionality
//	dealing with samples. Uploading the sample and inquiring
//	for its status is implemented.
//	SampleComm objects maintain their own threads for communication
//  purposes. It is recommended to keep the object in existence
//  between method calls so that the same thread gets used multiple times
//
//	$Log: $
//



// ------ common headers ----------------------------------------------
#include "base.h"
#include <string>
#include "ZExcept.h"
#include "AutoMutex.h"

// ------ AVIS transaction headers ------------------------------------
//#include "Transactions.h"
#include "StatusReceiver.h"
#include "SampleUploader.h"
#include "ClientType.h"


// ------ namespace using declarations --------------------------------
using namespace IcePackAgentComm;

// ------ extern declarations -----------------------------------------
extern MyTraceClient defaultTraceClient;
extern std::string	 defaultLogFileName;
extern std::string	 defaultTraceFileName;
extern std::string	 defaultAgentName;

extern RawEvent		s_terminateEvent;

extern void			initCommunications();


// ------ exported functions ------------------------------------------

//
// constructor
//

SampleComm::SampleComm()
:	_p_tc				( &defaultTraceClient ),
	_logFileName		( defaultLogFileName ),
	_traceFileName		( defaultTraceFileName ),
	_agentName			( defaultAgentName ),
	_errorCode			( NoError ),
	_systemExceptionCode( 0 ),
	_sr					( NULL ),
	_su					( NULL ),
	_m					( NULL )
{
	_m = static_cast<void*>( new Mutex );
}


//
// destructor
//

SampleComm::~SampleComm()
{
	terminateClient<StatusReceiver>( &_sr, *_p_tc );
	terminateClient<SampleUploader>( &_su, *_p_tc );

	_p_tc->UnregisterThread();
	delete static_cast<Mutex*>( _m );
}


//
// GetStatus
//

bool SampleComm::GetStatus	(	/*[in]	*/	const std::string&	gatewayURL	  	  	, 
                              /*[in]	*/	const std::string&	headers	  	  	, 
								/*[in]	*/	const std::string& 	cookie			   	,
                /*[in]	*/	const std::string& 	sampleChecksumSubmitted			   	,
								/*[out]	*/	std::string&	 	attributeStrings  	) 
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AutoMutex autoMutex( *static_cast<Mutex*>( _m ) );

	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	_p_tc->msg("IcePackAgentComm ------ begin GetSampleStatus ----------------------" );

	try
	{
		StatusReceiver * sr;

		if ( !initClient<StatusReceiver,SampleComm>( *this, &sr, &_sr, *_p_tc, _logFileName, _traceFileName, "StatusReceiverThread: ", gatewayURL, _errorCode, s_terminateEvent ) )
			return false;

		sr->Go( gatewayURL, headers, cookie, sampleChecksumSubmitted );

		finishClient<StatusReceiver>( sr, *_p_tc, gatewayURL, _errorCode, _systemExceptionCode );

		attributeStrings		= sr->ResponseHeaders();
	}
	catch( ZException& exc )
	{
		_errorCode				= SystemException;
		_systemExceptionCode	= exc.code();
		_p_tc->critical( "system exception caught - code:%8.8x", _systemExceptionCode );
	}

	_p_tc->msg("IcePackAgentComm ------ end GetSampleStatus ------------------------" );

	return _errorCode == NoError;
}


//
// Upload
//

bool SampleComm::Upload	(	/*[in]		*/	const std::string&	gatewayURL	  	  	, 
							/*[in/out]	*/	std::string&	 	attributeStrings  	,
							/*[in]		*/	Stream&				stream				)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AutoMutex autoMutex( *static_cast<Mutex*>( _m ) );

	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	_p_tc->msg("IcePackAgentComm ------ begin UploadSample -------------------------" );

	try
	{
		SampleUploader * su;

		if ( !initClient<SampleUploader,SampleComm>( *this, &su, &_su, *_p_tc, _logFileName, _traceFileName, "SampleUploaderThread: ", gatewayURL, _errorCode, s_terminateEvent ) )
			return false;

		su->Go( gatewayURL, attributeStrings, stream );

		finishClient<SampleUploader>( su, *_p_tc, gatewayURL, _errorCode, _systemExceptionCode );

		attributeStrings		= su->ResponseHeaders();
	}
	catch( ZException& exc )
	{
		_errorCode				= SystemException;
		_systemExceptionCode	= exc.code();
		_p_tc->critical( "system exception caught - code:%8.8x", _systemExceptionCode );
	}

	_p_tc->msg("IcePackAgentComm ------ end UploadSample ---------------------------" );

	return _errorCode == NoError;
}


//
// Error
//

ErrorCode SampleComm::Error() const
{
	return _errorCode;
}


//
// SystemExceptionCode
//

unsigned long SampleComm::SystemExceptionCode() const
{
	return _systemExceptionCode;
}


//
// SetTraceClient
//

bool SampleComm::SetTraceClient(	MyTraceClient&	   traceClient,
				  				    const std::string& logFileName,
				  					const std::string& traceFileName,
				  					const std::string& agentName		)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AutoMutex autoMutex( *static_cast<Mutex*>( _m ) );

	_p_tc->UnregisterThread();
	_p_tc = &traceClient;

 	// register this thread to be able to output trace/log messages.
	_p_tc->RegisterThread( logFileName.c_str(), traceFileName.c_str(), agentName.c_str() );

	// allow the AVISTransactions DLL to use the trace client declared in this exe.
	//CAVISTransactionsApp::SetTraceClient( _p_tc ); 

	_logFileName	=	logFileName		;
	_traceFileName	=	traceFileName	;
	_agentName		=	agentName		;

	return true;
}



