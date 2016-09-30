//
//	IBM AntiVirus Immune System
//
//	File Name:	SignatureComm.cpp
//	Author:		Milosz Muszynski
//
//	This class provides communication functionality
//	dealing with signature sets. Downloading the signature set and inquiring
//	for its availablility is implemented.
//	SignatureComm objects maintain their own threads for communication
//  purposes. It is recommended to keep the object in existence
//  between method calls so that the same thread gets used multiple times
//
//	$Log: $
//



// ------ common headers ----------------------------------------------
#include "base.h"
#include <string>
#include <map>
#include "ZExcept.h"
#include "AutoMutex.h"


// ------ AVIS transaction headers ------------------------------------
//#include "Transactions.h"
#include "SignatureImporter.h"
#include "SignatureInquirer.h"
#include "ClientType.h"
#include "Config.h"


// ------ FatAlbertUnpack headers -------------------------------------
#if 0 /* inw 2000-02-16 removed FatAlbert support */
#include "FatAlbertUnpack.h"
#endif /* 0 */



// ------ namespace using declarations --------------------------------
using namespace IcePackAgentComm;

// ------ local data --------------------------------------------------
int			s_connection_timeout				= CFG_DEFAULT_connection_timeout			   ;	
int			s_connection_retries				= CFG_DEFAULT_connection_retries			   ;	
int			s_send_request_timeout				= CFG_DEFAULT_send_request_timeout			   ;
int			s_send_request_data_timeout			= CFG_DEFAULT_send_request_data_timeout		   ;
int			s_receive_response_timeout			= CFG_DEFAULT_receive_response_timeout		   ;
int			s_receive_response_data_timeout		= CFG_DEFAULT_receive_response_data_timeout	   ;
Transaction::FirewallType s_firewallType		= Transaction::FW_NONE;					
std::string	s_firewallURL						= CFG_DEFAULT_firewall_URL   				   ;	
std::string s_proxyUserName						= CFG_DEFAULT_proxy_user_name				   ;	
std::string s_proxyPassword						= CFG_DEFAULT_proxy_password				   ;	
bool		s_enableWinINetDebugging			= CFG_DEFAULT_enable_WinINet_debugging		   ;
int			s_unpackingTimeout					= CFG_DEFAULT_unpacking_timeout				   ;
std::string s_unpackingTempDir					= CFG_DEFAULT_unpacking_temp_dir			   ;	
																								
std::string		defaultLogFileName				= CFG_DEFAULT_log_file_name					   ;
std::string		defaultTraceFileName			= CFG_DEFAULT_trace_file_name				   ;
std::string		defaultAgentName				= CFG_DEFAULT_agent_name					   ;

MyTraceClient	defaultTraceClient;
RawEvent		s_terminateEvent( NULL );

bool		s_ignoreSSLNameMismatch				= false;

// ------ internal functions ------------------------------------------

void initCommunications()
{
	Transaction::FirewallType fwType = Transaction::FW_NONE;

	// initialize AVIS internet classes with default values
	Transaction::InitInternetCommunicationsData(
		s_firewallType,									// firewall type
		"",												// socks address
		s_firewallURL.c_str(),							// proxy address
		s_proxyUserName.c_str(),						// proxy username
		s_proxyPassword.c_str(),						// proxy password
		s_connection_timeout				* 1000	,	// connect timeout
		s_connection_retries						,	// connect retries
		s_send_request_timeout				* 1000	,	// send request timeout
		s_send_request_data_timeout			* 1000	,	// send request data timeout
		s_receive_response_timeout			* 1000	,	// receive response timeout		
		s_receive_response_data_timeout		* 1000	,	// receive response data timeout
		s_ignoreSSLNameMismatch						);	// value for INTERNET_FLAG_IGNORE_CERT_CN_INVALID

	// enable WinInet Logging if specified in profile
	if ( s_enableWinINetDebugging )
	{
		Transaction::EnableWinInetLogging();
	}
}

// ------ exported functions ------------------------------------------

//
// constructor
//
SignatureComm::SignatureComm()
:	_p_tc				( &defaultTraceClient ),
	_logFileName		( defaultLogFileName ),
	_traceFileName		( defaultTraceFileName ),
	_agentName			( defaultAgentName ),
	_errorCode			( NoError ),
	_systemExceptionCode( 0 ),
	_si					( NULL ),
	_sq					( NULL ),
	_m					( NULL )
{
	_m = static_cast<void*>( new Mutex );
}


//
// destructor
//

SignatureComm::~SignatureComm()
{
	terminateClient<SignatureImporter>( &_si, *_p_tc );
	terminateClient<SignatureInquirer>( &_sq, *_p_tc );

	_p_tc->UnregisterThread();
	delete static_cast<Mutex*>( _m );
}


//
// Download
//

bool SignatureComm::Download	(	/*[in]	*/	const std::string&	gatewayURL	  	  	, 
                                /*[in]	*/	const std::string&	headers	  	  	, 
									/*[in]	*/	const std::string& 	sequenceNumber	   	, 
									/*[in]	*/	const std::string& 	directory		  	, 
									/*[out]	*/	std::string&	 	attributeStrings  	, 
									/*[in]	*/	SignatureType		signatureType	  	,
									/*[in]	*/	bool 				withUnpacking	  	)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AutoMutex autoMutex( *static_cast<Mutex*>( _m ) );

	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	_p_tc->msg("IcePackAgentComm ------ begin DownloadSignatureFile ----------------" );

	try
	{
		SignatureImporter * si;

		if ( !initClient<SignatureImporter,SignatureComm>( *this, &si, &_si, *_p_tc, _logFileName, _traceFileName, "SignatureImporterThread: ", gatewayURL, _errorCode, s_terminateEvent ) )
			return false;

		si->Go( gatewayURL, headers, sequenceNumber, directory, signatureType, withUnpacking );

		finishClient<SignatureImporter>( si, *_p_tc, gatewayURL, _errorCode, _systemExceptionCode );

		attributeStrings		= si->ResponseHeaders();
	}
	catch( ZException& exc )
	{
		_errorCode				= SystemException;
		_systemExceptionCode	= exc.code();
		_p_tc->critical( "system exception caught - code:%8.8x", _systemExceptionCode );
	}

	_p_tc->msg("IcePackAgentComm ------ end DownloadSignatureFile ------------------" );

	return _errorCode == NoError;
}


//
// GetLatest
//

bool	SignatureComm::GetLatest		(	/*[in]	*/	const string&	gatewayURL	  			,
                                    /*[in]  */  const string& headers,
								  			/*[out]	*/	string& 		sequenceNumber		  	)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AutoMutex autoMutex( *static_cast<Mutex*>( _m ) );

	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	_p_tc->msg("IcePackAgentComm ------ begin GetLatestSignatureSequence -----------" );

	try
	{
		SignatureInquirer * sq;

		if ( !initClient<SignatureInquirer,SignatureComm>( *this, &sq, &_sq, *_p_tc, _logFileName, _traceFileName, "SignatureInquirerThread: ", gatewayURL, _errorCode, s_terminateEvent ) )
			return false;

		IcePackAgentComm::SignatureType signatureType = IcePackAgentComm::Latest;
		if ( sequenceNumber == SignatureInquirer::NameForLatestBlessed )
			signatureType = IcePackAgentComm::LatestBlessed;

		sq->Go( gatewayURL, headers, sequenceNumber, signatureType );

		finishClient<SignatureInquirer>( sq, *_p_tc, gatewayURL, _errorCode, _systemExceptionCode );

		if ( _errorCode == NoError )
		{
			sequenceNumber = sq->SequenceNumber();
		}
		else
		{
			sequenceNumber = "";
		}
	}
	catch( ZException& exc )
	{
		_errorCode				= SystemException;
		_systemExceptionCode	= exc.code();
		_p_tc->critical( "system exception caught - code:%8.8x", _systemExceptionCode );
	}

	_p_tc->msg("IcePackAgentComm ------ end GetLatestSignatureSequence--------------" );
	return _errorCode == NoError;
}


//
// GetLatestBlessed
//

bool	SignatureComm::GetLatestBlessed(	/*[in]	*/	const string&	gatewayURL	  			,
                                        /*[in]  */  const string& headers,
						 					/*[out]	*/	string& 		sequenceNumber		  	)
{
	sequenceNumber = SignatureInquirer::NameForLatestBlessed;
	bool b = GetLatest( gatewayURL, headers, sequenceNumber );
	if ( !b )
		sequenceNumber = "";
	return b;
}


//
// Error
//

ErrorCode SignatureComm::Error() const
{
	return _errorCode;
}


//
// SystemExceptionCode
//

unsigned long SignatureComm::SystemExceptionCode() const
{
	return _systemExceptionCode;
}


//
// SetTraceClient
//

bool SignatureComm::SetTraceClient(	MyTraceClient&	   traceClient,
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

#if 0 /* inw 2000-02-16 removed FatAlbert support */
//
// Install
//
bool SignatureComm::Install(	/*[in]	*/	const string&	packagePath		,
						   		/*[in]	*/	const string&	commandLine		,
						   		/*[in]	*/	int				timeout		  	)
{
	_p_tc->msg( "running %s with %s (timeout=%d)", 
			 packagePath.c_str(), commandLine.c_str(), timeout );

	FatAlbertPackage package;
	package.Run( packagePath, commandLine, timeout, NULL, _p_tc );

	std::string fatError;
	switch ( package.Error() )
	{
		case FatAlbertPackage::NoError:					_errorCode = IcePackAgentComm::NoError					; fatError = "NoError";				break;
		case FatAlbertPackage::UnknownError:			_errorCode = IcePackAgentComm::UnknownError				; fatError = "UnknownError";		break;
		case FatAlbertPackage::PackageNotFoundError:	_errorCode = IcePackAgentComm::PackageNotFoundError		; fatError = "PackageNotFound";		break;
		case FatAlbertPackage::TempDirError:			_errorCode = IcePackAgentComm::TempDirError				; fatError = "TempDirError";		break;
		case FatAlbertPackage::TargetDirError:			_errorCode = IcePackAgentComm::TargetDirError			; fatError = "TargetDirError";		break;
		case FatAlbertPackage::TargetDOSDirError:		_errorCode = IcePackAgentComm::TargetDOSDirError		; fatError = "TargetDOSDirError";	break;
		case FatAlbertPackage::FileCopyError:			_errorCode = IcePackAgentComm::FileCopyError			; fatError = "FileCopyError";		break;
		case FatAlbertPackage::NoFilesFoundError:		_errorCode = IcePackAgentComm::NoFilesFoundError		; fatError = "NoFilesFoundError";	break;
		case FatAlbertPackage::DiskFullError:			_errorCode = IcePackAgentComm::DiskFullError			; fatError = "DiskFull";			break;
		case FatAlbertPackage::VersionError:			_errorCode = IcePackAgentComm::VersionError				; fatError = "VersionError";		break;
		case FatAlbertPackage::SystemException:			_errorCode = IcePackAgentComm::FatAlbertSystemException ; fatError = "SystemException";		break;
		case FatAlbertPackage::ProcessCreationError:	_errorCode = IcePackAgentComm::ProcessCreationError 	; fatError = "ProcessCreationError";break;
		case FatAlbertPackage::ProcessTimedOutError:	_errorCode = IcePackAgentComm::ProcessTimedOutError 	; fatError = "ProcessTimedOutError";break;
		case FatAlbertPackage::ProcessTerminationError:	_errorCode = IcePackAgentComm::ProcessTerminationError 	; fatError = "ProcessTerminationError";	break;
		case FatAlbertPackage::ProcessSyncError:		_errorCode = IcePackAgentComm::ProcessSyncError	 		; fatError = "ProcessSyncError";	break;
		default:										_errorCode = IcePackAgentComm::UnknownError				; fatError = "UnknownError";		break;
	};

	_p_tc->msg( "%s completed, status: \"%s\"", packagePath.c_str(), fatError.c_str() );

	return _errorCode == IcePackAgentComm::NoError;
}
#endif /* 0 */

bool ICEPACKAGENTCOMM_API IcePackAgentComm::SetCommParams	( 	
								const string&	firewallURL	  					 , 
								const string&	proxyUserName					 ,
								const string&	proxyPassword					 ,
								int				connection_timeout				 ,
								int				connection_retries				 ,
								int				send_request_timeout			 ,
								int				send_request_data_timeout		 ,
								int				receive_response_timeout		 ,
								int				receive_response_data_timeout	 ,
								HANDLE			terminateEvent					 ,
								bool			ignore_SSL_name_mismatch		 )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// possibly add some mechanism that would enforce that this function is
	// being executed when none of the communication functions is active
	s_connection_timeout				= connection_timeout			   ;	
	s_connection_retries				= connection_retries			   ;	
	s_send_request_timeout				= send_request_timeout			   ;
	s_send_request_data_timeout			= send_request_data_timeout		   ;
	s_receive_response_timeout			= receive_response_timeout		   ;
	s_receive_response_data_timeout		= receive_response_data_timeout	   ;
	s_firewallURL						= firewallURL					   ;
	s_proxyUserName						= proxyUserName					   ;
	s_proxyPassword						= proxyPassword					   ;
	// add a firewall parameter and external enumeration 
	// in case a firewall type is needed
	// enum could be as follows
	// (in namespace IcePackAgentComm):
	//
	// enum FirewallType 
	// { 
	//		fw_none,
	//		fw_socks,
	//		fw_proxy,
	//		fw_defined_in_registry
	// }

	s_firewallType = firewallURL.empty() ? Transaction::FW_NONE : Transaction::FW_PROXY;

	s_terminateEvent.init( terminateEvent );

	s_ignoreSSLNameMismatch = ignore_SSL_name_mismatch;

	return true;
}


bool	ICEPACKAGENTCOMM_API IcePackAgentComm::SetUnpackParams( 	
		int				timeout			  ,
		const string&	temporaryDirectory	)
{
	// to do if accepted!!!!!!!!!!!!
	s_unpackingTimeout	= timeout;
	s_unpackingTempDir	= temporaryDirectory;
	return true;
}




void ICEPACKAGENTCOMM_API IcePackAgentComm::GetErrorMessage( ErrorCode errorCode, string& msg )
{
	static std::string messages[ UnknownError + 1 ] =
	{	"NoError", 
		"PackageNotFoundError", 
		"TempDirError", 
		"TargetDirError", 
		"TargetDOSDirError", 
		"FileCopyError",
		"NoFilesFoundError", 
		"DiskFullError", 
		"VersionError", 
		"SystemException",
		"LibraryException",
		"ProcessCreationError", 
		"ProcessTimedOutError", 
		"ProcessTerminationError", 
		"ProcessSyncError",
		"NoSignaturesAvailable", 
		"FileWriteError", 
		"FileReadError", 
		"FileOpenError", 
		"UnknownSignatureTypeError",
		"MissingSignatureSequenceError", 
		"NetworkError", 
		"NoHttpCodeError",
		"NetworkCriticalError", 
		"GatewayError", 
		"NoContentError",
		"FatAlbertSystemException",
		"ThreadCouldNotStartError",
		"OutOfMemoryError",
		"TerminatedByCaller",
		"VDBSystemException", 
		"VDBZipArchiveError", 
		"VDBMemoryError", 
		"VDBParameterError",
		"VDBUnexpectedEOFError", 
		"VDBUnknownError",
    "YouveBeenRedirected",
    "InvalidRedirectError",
		"UnknownError"
	};

	if ( errorCode > UnknownError )
		errorCode = UnknownError;

	// NOTE:	UnknownError has to be the last error code 
	//			in the IcePackAgentComm::ErrorCode enumeration

	msg = messages[ errorCode ];
}

